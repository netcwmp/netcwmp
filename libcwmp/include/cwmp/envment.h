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

#ifndef __CWMPENVMENT_H__
#define __CWMPENVMENT_H__

#include <cwmp/pool.h>

typedef struct env_t env_t;

struct env_t
{
    cwmp_t * cwmp;
    pool_t * pool;

};


#endif
