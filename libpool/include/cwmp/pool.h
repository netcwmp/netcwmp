/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014 netcwmp group                         *
 *                                                                      *
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/


#ifndef __CWMPPOOL_H__
#define __CWMPPOOL_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MALLOC(size)					malloc(size)
#define FREE(x)							free(x)


#define XMALLOC(size)                   pool_pcalloc(env->pool, size)
#define XREALLOC(p,olds,news)           pool_prealloc(env->pool, p, olds, news)
#define XSTRDUP(p)                      pool_pstrdup(env->pool, p)
#define XFREE(p)                        pool_pfree(env->pool, p)

#define PMALLOC(size)                   pool_pcalloc(pool, size)
#define PREALLOC(p, olds, news)         pool_prealloc(pool, p, olds, news)
#define PSTRDUP(p)                      pool_pstrdup(pool, p)
#define PFREE(p)                        pool_pfree(pool, p)


#define POOL_MAX_ALLOC_FROM_POOL  (4096 - 1)
#define POOL_DEFAULT_SIZE    (4096)
#define POOL_MIN_SIZE        	(128)


typedef struct pool_t  pool_t;
typedef struct pool_t 	Pool;
typedef struct pool_chain_t      pool_chain_t;
typedef struct pool_cleanup_t  pool_cleanup_t;
typedef struct pool_data_t pool_data_t;
typedef struct pool_large_t  pool_large_t;
typedef struct pool_cleanup_file_t pool_cleanup_file_t;

typedef void (*pool_cleanup_handler)(void *data);






pool_t *pool_create(size_t size);
void pool_destroy(pool_t *pool);
void pool_clear(pool_t *pool);
void *pool_palloc(pool_t *pool, size_t size);
void *pool_pnalloc(pool_t *pool, size_t size);
void *pool_pcalloc(pool_t *pool, size_t size);
void *pool_pmemalign(pool_t *pool, size_t size, size_t alignment);
int   pool_pfree(pool_t *pool, void *p);
void *pool_prealloc(pool_t * pool, void * ptr, size_t oldsize, size_t newsize);
void *pool_pmemdup(pool_t * pool, const void * ptr, size_t size);
char *pool_pstrdup(pool_t * pool, const void * ptr);
char *pool_pstrdup_lower (pool_t *pool, const char *ptr);
void *pool_cleanup_add(pool_t * pool, pool_cleanup_handler handler, void * data);
void pool_cleanup_file(void *data);
void pool_delete_file(void *data);



#endif
