/************************************************************************
 * Id: buffer.c                                                         *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014 netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/
#include "cwmp/buffer.h"
#include "cwmp/util.h"
#include "cwmp/memory.h"
#include "cwmp/log.h"
#include "cwmp_private.h"

void  cwmp_buffer_clear(void * data)
{
    cwmp_buffer_t * b;
    //FUNCTION_TRACE();
    if (!data)
        return;
    b = (cwmp_buffer_t*)data;
    b->writed = 0;
    b->readed = 0;
    b->offset = b->string;
    b->size = DEFAULT_BUFFER_SIZE;
    b->next = 0;
    b->string[0] = 0;
}


int cwmp_buffer_create(cwmp_buffer_t **news, pool_t * pool)
{
    (*news) =  PMALLOC(sizeof(cwmp_buffer_t));

    if ((*news) == NULL)
    {
        return CWMP_ENOMEM;
    }
    (*news)->size = DEFAULT_BUFFER_SIZE;
    (*news)->writed = 0;
    (*news)->readed = 0;
    (*news)->offset = (*news)->string;
    (*news)->next = 0;
    (*news)->string[0] = 0;

    pool_cleanup_add(pool, cwmp_buffer_clear, *news);

    return CWMP_OK;
}

int cwmp_buffer_init(cwmp_buffer_t * b)
{
    b->writed = 0;
    b->readed = 0;
    b->size = DEFAULT_BUFFER_SIZE;
    b->offset = b->string;
    b->next = 0;
    b->string[0] = 0;
    return CWMP_OK;
}

void  cwmp_buffer_free(cwmp_buffer_t * b, pool_t * pool)
{
    if (b == NULL)
        return;

    PFREE(b);

}


char * cwmp_buffer_current(cwmp_buffer_t * b)
{
    return b->string + b->writed;
}

char * cwmp_buffer_string(cwmp_buffer_t * b)
{
    return b->string;
}

size_t cwmp_buffer_length(cwmp_buffer_t * b)
{
    return b == NULL ? 0: (b->writed - b->readed) ;
}

size_t cwmp_buffer_size(cwmp_buffer_t * b)
{
    return b == NULL ? 0: b->size;
}

int cwmp_buffer_remain(cwmp_buffer_t * b)
{
    return b == NULL ? 0: (b->size - b->writed);
}

size_t	cwmp_chunk_length(cwmp_chunk_t * c)
{
    return c == NULL? 0 : c->bytes;
}




void  cwmp_buffer_write_position(cwmp_buffer_t * b, size_t pos, const void * val, cwmp_uint32_t len)
{
    memcpy(b->string + pos, val, len);
    if (pos + len > b->writed)
    {
        /* b->writed = pos + len;  */
    }
}

void  cwmp_buffer_write_bytes(cwmp_buffer_t * b, const void * val, size_t len)
{
    if (len > (b->size - b->writed))
        return;
    memcpy(b->offset, val, len);
    b->offset += len;
    b->writed += len;
}

void  cwmp_buffer_write_uint64(cwmp_buffer_t * b, cwmp_uint64_t val)
{
    cwmp_buffer_write_bytes(b, &val, 8);
}

void  cwmp_buffer_write_uint32(cwmp_buffer_t * b, cwmp_uint32_t val)
{
    cwmp_buffer_write_bytes(b, &val, 4);
}

void  cwmp_buffer_write_uint16(cwmp_buffer_t * b, cwmp_uint16_t val)
{
    cwmp_buffer_write_bytes(b, &val, 2);
}

void cwmp_buffer_write_uint8(cwmp_buffer_t * b, cwmp_byte_t val)
{
    cwmp_buffer_write_bytes(b, &val, 1);
}

void  cwmp_buffer_write_format_string(cwmp_buffer_t * b, const char * fmt, ...)
{
    va_list ap;
    int len;
    va_start(ap, fmt);
#ifdef WIN32
    len = _vsnprintf(cwmp_buffer_current(b), b->size - b->writed, fmt, ap);
#else
    len =vsnprintf(cwmp_buffer_current(b), b->size - b->writed, fmt, ap);
#endif
    b->offset += len;
    b->writed += len;
    va_end(ap);
}



void  cwmp_buffer_write_string(cwmp_buffer_t * b, const char * str, size_t len)
{
    cwmp_buffer_write_bytes(b, str, len);
}

void  cwmp_buffer_clone(cwmp_buffer_t * b, const cwmp_buffer_t * cb)
{
    size_t len = cb->writed;
    cwmp_buffer_write_bytes(b, cb->string, len);
}

int cwmp_buffer_read_bytes(void * val,  cwmp_buffer_t * b, const cwmp_uint32_t len)
{
    if (len > cwmp_buffer_length(b))
    {
        return CWMP_ERROR;
    }
    else
    {
        memcpy(val, b->string + b->readed, len);
        b->readed += len;
        return CWMP_OK;
    }
}

int cwmp_buffer_read_uint8(cwmp_byte_t * val, cwmp_buffer_t * b)
{
    return cwmp_buffer_read_bytes((void *)val,b,1);
}


int cwmp_buffer_read_uint16(cwmp_uint16_t * val, cwmp_buffer_t * b)
{
    cwmp_uint16_t v;
    if (cwmp_buffer_read_bytes((void *)&v,b,2) != CWMP_OK)
    {
        return CWMP_ERROR;
    }
    else
    {
        (*val) = v;
        return CWMP_OK;
    }
}

int cwmp_buffer_read_uint32(cwmp_uint32_t * val, cwmp_buffer_t * b)
{
    cwmp_uint32_t v;
    if (cwmp_buffer_read_bytes((void *)&v,b,4) != CWMP_OK)
    {
        return CWMP_ERROR;
    }
    else
    {
        (*val) = v;
        return CWMP_OK;
    }
}

int cwmp_chunk_create(cwmp_chunk_t ** news, pool_t * pool)
{
    (*news) = PMALLOC(sizeof(cwmp_chunk_t));
    if (!(*news))
    {
        cwmp_log_error("create array buffer out of memory\n");
        return CWMP_ENOMEM;
    }
    (*news)->buffer = NULL;
    (*news)->current = NULL;
    (*news)->bytes = (*news)->count = (*news)->used = 0;

    /*
       if (cwmp_buffer_create(&buffer, pool) != CWMP_OK)
       {
       cwmp_log_error("create buffer out of memory\n");
       PFREE(*news);
       return CWMP_ENOMEM;
       }
       (*news)->buffer = (*news)->current = buffer;
       */

    pool_cleanup_add(pool, (pool_cleanup_handler)cwmp_chunk_clear, (*news));
    return CWMP_OK;
}

void  cwmp_chunk_print(cwmp_chunk_t * cb)
{
    cwmp_buffer_t * b = cb->buffer;
    FUNCTION_TRACE();
    while (b)
    {
        //fwrite(cwmp_buffer_string(b), 1, cwmp_buffer_length(b), out);
        cwmp_log_info(cwmp_buffer_string(b));
        b = b->next;
    }

}


int cwmp_chunk_write_string(cwmp_chunk_t * cb, const char * str, size_t length, pool_t * pool)
{
    cwmp_buffer_t *buffer;
    size_t len, leftbytes, writebytes, writed;
    //FUNCTION_TRACE();
    writed = 0;
    leftbytes = length;

    while (leftbytes>0)
    {
        len = cwmp_buffer_remain(cb->current);

        if (len <= 0)
        {
            if (cb->current && cb->current->next)
            {
                cb->current = cb->current->next;
                cwmp_buffer_clear(cb->current);
            }
            else
            {
                if (cwmp_buffer_create(&buffer, pool) != CWMP_OK)
                {
                    cwmp_log_error("create buffer out of memory\n");
                    return CWMP_ERROR;
                }
                if (cb->current != NULL)
                {
                    cb->current->next = buffer;
                }
                else
                {
                    cb->buffer = buffer;
                }
                cb->current = buffer;
            }


            len=cwmp_buffer_remain(cb->current);

        }


        writebytes = leftbytes > len ? len :leftbytes;

        cwmp_buffer_write_string(cb->current, str + writed, writebytes);
        writed += writebytes;

        cb->bytes += writebytes;
        leftbytes -= writebytes;
        /*if (leftbytes > 0)
          {
          if (cwmp_buffer_create(&buffer, pool) != CWMP_OK)
          {
          cwmp_log_error("create buffer out of memory\n");
          return CWMP_ERROR;
          }
          cb->current->next = buffer;
          cb->current = buffer;
          }
          else
          {
          break;
          }
          */
    }

    return writed;
}


int cwmp_chunk_copy(char * dest, const cwmp_chunk_t * cb, size_t max_length)
{
    size_t bufleft, destwrited, writebytes;
    size_t b,d;
    char * ptr = dest;
    cwmp_buffer_t * buffer = cb->buffer;
    bufleft = cb->bytes;
    destwrited = 0;
    while ((destwrited <= max_length) && (bufleft > 0) && (buffer!=NULL))
    {
        b = bufleft > cwmp_buffer_length(buffer) ? cwmp_buffer_length(buffer) : bufleft;
        d = max_length - destwrited;
        writebytes =  d > b	? b : d;
        strncpy(ptr, cwmp_buffer_string(buffer), writebytes);
        ptr += writebytes;
        destwrited += writebytes;
        bufleft -= writebytes;
        buffer = buffer->next;
    }

    dest[destwrited] = 0;
    return destwrited;
}

void cwmp_chunk_init(cwmp_chunk_t * cb)
{
    FUNCTION_TRACE();

    cb->buffer = NULL;
    cb->bytes = 0;
    cb->count = 0;
    cb->used = 0;
    cb->current = cb->buffer;
}

void cwmp_chunk_clear(cwmp_chunk_t * cb)
{
    //FUNCTION_TRACE();

    cb->buffer = NULL;
    cb->bytes = 0;
    cb->count = 0;
    cb->used = 0;
    cb->current = cb->buffer;
}


int cwmp_chunk_release(cwmp_chunk_t * cb, int remain, pool_t * pool)
{
    int i;
    cwmp_buffer_t * next;
    cwmp_buffer_t * buffer = cb->buffer;
    for (i=0; i<remain && buffer; i++)
    {
        buffer = buffer->next;
    }


    while (buffer)
    {
        next = buffer->next;
        cwmp_buffer_free(buffer, pool);
        buffer = next;
    }
    return 0;
}
