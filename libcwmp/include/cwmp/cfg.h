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


#ifndef __CWMPCFG_H__
#define __CWMPCFG_H__

#include <cwmp/pool.h>

#define INI_BUFFERSIZE  128

int 		cwmp_conf_get(const char * key, char *value);
char *	cwmp_conf_pool_get(pool_t * pool, const char * key);
int		cwmp_conf_get_int(const char * key);
int		cwmp_conf_set(const char * key, const char * value);
int		cwmp_conf_open(const char * file);


#endif

