/************************************************************************
 * Id: memory.c                                                         *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *  
 ***********************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cwmp/memory.h"
#include "cwmp/log.h"

#ifdef USE_CWMP_MEMORY

#define TRLOCK()
#define TRUNLOCK()


enum {TR_ALIGN = 8};
enum {TR_MAX_BYTES = 128};
enum {TR_FREELISTS = 32}; // TR_MAX_BYTES/TR_ALIGN



#define TR_THROW_ERROR_ALLOC()	do{ cwmp_log_error("TRC throw Alloc error. out of memory."); } while(0)
#define TR_THROW_ERROR_MALLOC()	do{ cwmp_log_error("TRC throw Malloc error. out of memory."); } while(0)


typedef union TRObj TRObj;

union TRObj
{
    union TRObj* freeListLink;
    char sessionData[1];    /* The session sees this. */
};


void (* TriMallocAllocSafeHandler)() = 0;


static char*		TR_start_free;
static char*		TR_end_free;
static size_t		TR_heap_size;
static TRObj*		TR_free_list[TR_FREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };



void * TRSafeRefill(size_t n);


static INLINE void * TRMemStrdup(const char * src)
{
    size_t len;
    char * dest;
    if (!src)
        return 0;
    len = strlen(src);
    dest = XMALLOC(len);
    if (!dest)
        return 0;
    strncpy(dest,src, len);
    return dest;
}

static void* TRSafeMalloc(size_t n)
{
    void (* TriMallocHandler)();
    void * result;

    for (;;)
    {
        TriMallocHandler = TriMallocAllocSafeHandler;
        if (TriMallocHandler == 0)
        {
            TR_THROW_ERROR_ALLOC();
        }
        else
        {
            (*TriMallocHandler)();
        }
        result = TRmalloc(n);
        if (result)
        {
            return result;
        }
        else
        {
            TR_THROW_ERROR_MALLOC();
        }
    }

}


static void* TRSafeRealloc(void * ptr, size_t n)
{
    void (* TriMallocHandler)();
    void * result;

    for (;;)
    {
        TriMallocHandler = TriMallocAllocSafeHandler;
        if (TriMallocHandler == 0)
        {
            TR_THROW_ERROR_ALLOC();
        }
        else
        {
            (*TriMallocHandler)();
        }
        result = realloc(ptr, n);
        if (result)
            return result;
    }
}


static void* TRMemAllocate(size_t n)
{
    void * result = TRmalloc(n);
    if (0 == result)
        result = TRSafeMalloc(n);
    return result;
}


static void  TRMemDeallocate(void* ptr, size_t n )
{
    TRfree(ptr);
}


static void* TRMemReallocate(void * ptr, size_t old_size, size_t new_size)
{
    void * result = realloc(ptr, new_size);
    if (0 == result) result = TRSafeRealloc(ptr, new_size);
    return result;
}


static size_t	TRSafeRoundUp(size_t bytes)
{
    return (((bytes) + (size_t) TR_ALIGN-1) & ~((size_t) TR_ALIGN - 1));
}



static size_t	TRSafeFreeListIndex(size_t  bytes)
{
    return (((bytes) + (size_t)TR_ALIGN-1)/(size_t)TR_ALIGN - 1);
}



char * TRSafecwmp_chunk_tAlloc(size_t size, int * nobjs)
{
    char * result = 0;
    size_t total_bytes = size * (*nobjs);
    size_t bytes_left = TR_end_free - TR_start_free;

    if (bytes_left > total_bytes)
    {
        result = TR_start_free;
        TR_start_free += total_bytes;
        return result;
    }
    else if (bytes_left > size)
    {
        (*nobjs) = (int)(bytes_left / size);
        total_bytes = size * (*nobjs);
        result = TR_start_free;
        TR_start_free += total_bytes;
        return result;
    }
    else
    {
        size_t bytes_to_get = 2 * total_bytes + TRSafeRoundUp(TR_heap_size >> 4);
        if (bytes_left > 0)
        {
            TRObj ** my_free_list = TR_free_list + TRSafeFreeListIndex(bytes_left);
            ((TRObj*)TR_start_free)->freeListLink = *my_free_list;
            *my_free_list = (TRObj*)TR_start_free;
        }
        TR_start_free = (char *)TRmalloc(bytes_to_get);
        if (TR_start_free == 0)
        {
            size_t i;
            TRObj ** my_free_list;
            TRObj * p;

            for (i = size; i <= (size_t)TR_MAX_BYTES; i += (size_t)TR_ALIGN)
            {
                my_free_list = TR_free_list + TRSafeFreeListIndex(i);
                p = *my_free_list;
                if (p != 0)
                {
                    *my_free_list = p->freeListLink;
                    TR_start_free = (char *)p;
                    TR_end_free = TR_start_free + i;
                    return TRSafecwmp_chunk_tAlloc(size, nobjs);
                }
            }
            TR_end_free = 0;
            TR_start_free = (char *)TRMemAllocate(bytes_to_get);

        }
        TR_heap_size += bytes_to_get;
        TR_end_free = TR_start_free + bytes_to_get;
        return(TRSafecwmp_chunk_tAlloc(size, nobjs));
    }

}


void * TRSafeRefill(size_t n)
{
    int nobjs = 10;
    char* chunk = TRSafecwmp_chunk_tAlloc(n, &nobjs);
    TRObj** my_free_list;
    TRObj* result;
    TRObj* current_obj;
    TRObj* next_obj;
    int i;

    if (1 == nobjs) return(chunk);
    my_free_list = TR_free_list + TRSafeFreeListIndex(n);

    /* Build free list in chunk */
    result = (TRObj*)chunk;
    *my_free_list = next_obj = (TRObj*)(chunk + n);
    for (i = 1; ; i++)
    {
        current_obj = next_obj;
        next_obj = (TRObj*)((char*)next_obj + n);
        if (nobjs - 1 == i)
        {
            current_obj -> freeListLink = 0;
            break;
        }
        else
        {
            current_obj -> freeListLink = next_obj;
        }
    }

    return(result);
}



static void* TRAllocate(size_t n)
{
    void * result = 0;

    if (n > (size_t)TR_MAX_BYTES)
    {
        result = TRMemAllocate(n);
    }
    else
    {
        TRObj ** my_free_list = TR_free_list + TRSafeFreeListIndex(n);
        TRObj * res ;
#ifndef __TRI_NOTHREADS__
        TRLOCK();
#endif

        res = * my_free_list;
        if (res == 0)
            result = TRSafeRefill(TRSafeRoundUp(n));
        else
        {
            (*my_free_list) = res->freeListLink;
            result = res;
        }
#ifndef __TRI_NOTHREADS__
        TRUNLOCK();
#endif

    }

    return result;
}

/*
 *
 */

static void TRDeallocate(void* ptr, size_t n)
{
    if (n > (size_t)TR_MAX_BYTES)
    {
        TRMemDeallocate(ptr,n);
    }
    else
    {
        TRObj ** my_free_list = TR_free_list + TRSafeFreeListIndex(n);
        TRObj * q = (TRObj *)ptr;

#ifndef __TRI_NOTHREADS__
        TRLOCK();
#endif

        q->freeListLink = *my_free_list;
        (*my_free_list) = q;

#ifndef __TRI_NOTHREADS__
        TRUNLOCK();
#endif

    }
}

/*
 *
 */

static void* TRReallocate(void * ptr, size_t old_size, size_t new_size)
{
    void * result;
    size_t copy_size;
    if (old_size > (size_t) TR_MAX_BYTES && new_size > (size_t) TR_MAX_BYTES)
    {
        return realloc(ptr, new_size);
    }
    if (TRSafeRoundUp(old_size) == TRSafeRoundUp(new_size))
        return ptr;
    result = TRAllocate(new_size);
    copy_size = new_size > old_size ? old_size : new_size;
    memcpy(result,ptr, copy_size);
    TRDeallocate(ptr, old_size);
    return result;
}

/*
 *
 */



void *	MemoryMalloc(size_t size)
{
    return TRAllocate(size);
}
void *	MemoryCalloc(size_t count, size_t size)
{
    return calloc(count , size);
}
void *	MemoryRealloc(void * ptr,size_t olds, size_t news)
{
    return TRReallocate(ptr,olds,news);
}

void 	MemoryFree(void* ptr, size_t size)
{
    TRDeallocate(ptr,size);
}




#endif
