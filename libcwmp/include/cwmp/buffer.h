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

#ifndef __CWMPBUFFER_H__
#define __CWMPBUFFER_H__
#include <cwmp/types.h>
#include <cwmp/memory.h>

#include <cwmp/pool.h>

#define DEFAULT_BUFFER_SIZE  1024

typedef struct cwmp_buffer_t cwmp_buffer_t;
typedef struct cwmp_chunk_t cwmp_chunk_t;

struct cwmp_buffer_t
{
    size_t writed;
    size_t readed;
    size_t size;
    char * offset;
    cwmp_byte_t string[DEFAULT_BUFFER_SIZE + 1];
    cwmp_buffer_t * next;
};


struct cwmp_chunk_t
{
    cwmp_buffer_t * buffer;
    cwmp_buffer_t * current;
    size_t used;
    size_t count;
    size_t bytes;
} ;




char * cwmp_buffer_current(cwmp_buffer_t * b);
char * cwmp_buffer_string(cwmp_buffer_t * b);
size_t cwmp_buffer_length(cwmp_buffer_t * b);
size_t cwmp_buffer_size(cwmp_buffer_t * b);
int cwmp_buffer_remain(cwmp_buffer_t * b);
size_t	cwmp_chunk_length(cwmp_chunk_t * cb);


int cwmp_buffer_init(cwmp_buffer_t * b);
int cwmp_buffer_create(cwmp_buffer_t **news, pool_t * pool);
void  cwmp_buffer_free(cwmp_buffer_t * b, pool_t * pool);
void  cwmp_buffer_clear(void * data);

void  cwmp_buffer_write_position(cwmp_buffer_t * b, size_t pos, const void * val, cwmp_uint32_t len);
void  cwmp_buffer_write_bytes(cwmp_buffer_t * b, const void * val, size_t len);
void  cwmp_buffer_write_uint64(cwmp_buffer_t * b, cwmp_uint64_t val);
void  cwmp_buffer_write_uint32(cwmp_buffer_t * b, cwmp_uint32_t val);
void  cwmp_buffer_write_uint16(cwmp_buffer_t * b, cwmp_uint16_t val);
void  cwmp_buffer_write_uint8(cwmp_buffer_t * b, cwmp_byte_t val);
void  cwmp_buffer_write_string(cwmp_buffer_t *, const char * str, size_t length);
void  cwmp_buffer_clone(cwmp_buffer_t *, const cwmp_buffer_t *);
void  cwmp_buffer_write_format_string(cwmp_buffer_t * b, const char * fmt, ...);

int cwmp_buffer_read_bytes(void * val,  cwmp_buffer_t * b, const cwmp_uint32_t len);
int cwmp_buffer_read_uint8(cwmp_byte_t * val, cwmp_buffer_t * b);
int cwmp_buffer_read_uint16(cwmp_uint16_t * val, cwmp_buffer_t * b);
int cwmp_buffer_read_uint32(cwmp_uint32_t * val, cwmp_buffer_t * b);


int cwmp_chunk_create(cwmp_chunk_t ** news, pool_t * pool);
void cwmp_chunk_init(cwmp_chunk_t * cb);
int cwmp_chunk_write_string(cwmp_chunk_t * cb, const char * str, size_t length, pool_t * pool);
int cwmp_chunk_print_format_string(cwmp_chunk_t * cb, pool_t * pool, const char * fmt, ...);
int cwmp_chunk_copy(char * dest, const cwmp_chunk_t * cb,  size_t maxLength);

void  cwmp_chunk_print(cwmp_chunk_t * cb);
int cwmp_chunk_release(cwmp_chunk_t * cb, int count, pool_t * pool);
void cwmp_chunk_clear(cwmp_chunk_t * cb);
int cwmp_chunk_free(cwmp_chunk_t * cb);




#endif

