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


#ifndef __CWMPQUEUE_H__
#define __CWMPQUEUE_H__
#include <cwmp/types.h>


#define QUEUE_PRIORITY_LOW	0
#define QUEUE_PRIORITY_COMMON	1
#define QUEUE_PRIORITY_HIGH	2

typedef struct qnode_t qnode_t;
typedef struct queue_t queue_t;

struct qnode_t {
	void *data;
	void *arg1;
	void *arg2;
	int	datatype;
	int 	priority;
	struct qnode_t *next;
} ;

struct queue_t {
	int size;
	struct qnode_t *first;
	struct qnode_t *last;
	
	pthread_mutex_t     mutex;
} ;

void queue_add(queue_t *q, void * data, int type, int priority, void * arg1, void *arg2);
void queue_push(queue_t *q, void* data, int type);
int queue_pop(queue_t *q, void ** data);
void queue_view(queue_t *q);
queue_t *queue_create(pool_t * pool);
int queue_is_empty(queue_t *q);
void queue_free(pool_t * pool, queue_t *q);




#endif //__CWMPQUEUE_H__