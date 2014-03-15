/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                         *
 *                                                                      *
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#ifndef __CWMPMEMORY_H__
#define __CWMPMEMORY_H__

#include <cwmp/types.h>
#include <cwmp/util.h>

#include <cwmp/envment.h>





#define TRBZERO(b, s)       memset(b, 0, (s))
#define TRMALLOC(s)         XMALLOC(s)
#define TRFREE(p,s)         XFREE(p,s)



#ifdef USE_CWMP_MEMORY


#define TMALLOC(size)		            MemoryMalloc((size))
#define TCALLOC(count, size)	        MemoryCalloc((count), (size))
#define TREALLOC(pointer, olds, news)	MemoryRealloc((pointer), (olds), (news))
#define TFREE(pointer, size)		    do{MemoryFree((pointer),(size));((pointer))=NULL;}while(0)

void * MemoryMalloc(size_t size);
void * MemoryCalloc(size_t count, size_t size);
void * MemoryRealloc(void * ptr, size_t olds, size_t news);
void   MemoryFree(void* ptr, size_t size);




#define TRmalloc    malloc
#define TRfree      free



#else

#define TMALLOC(size)		            malloc((size))
#define TRCALLOC(count, size)	        calloc((count), (size))
#define TREALLOC(pointer, olds, news)	realloc((pointer), (news))
#define TFREE(pointer, size)		    free(pointer)


#endif




#endif

