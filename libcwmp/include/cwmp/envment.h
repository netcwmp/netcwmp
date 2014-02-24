/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2008-2010  netcwmp.kongdai.com                         *
 *                                                                      *
 * Copyright 2008-2010           Mr.x() <azhenglive@gmail.com>          *
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
