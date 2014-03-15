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

#ifndef __CWMPLOG_H__
#define __CWMPLOG_H__


#define CWMP_LOG_STDERR            0
#define CWMP_LOG_EMERG             1
#define CWMP_LOG_ALERT             	2
#define CWMP_LOG_CRIT              	3
#define CWMP_LOG_ERROR               	4
#define CWMP_LOG_WARN              5
#define CWMP_LOG_NOTICE            6
#define CWMP_LOG_INFO              	7
#define CWMP_LOG_TRACE		        8
#define CWMP_LOG_DEBUG             9

#define CWMP_LOG_DEBUG_CORE        0x010
#define CWMP_LOG_DEBUG_ALLOC       0x020
#define CWMP_LOG_DEBUG_MUTEX       0x040
#define CWMP_LOG_DEBUG_EVENT       0x080
#define CWMP_LOG_DEBUG_HTTP        0x100
#define CWMP_LOG_DEBUG_MAIL        0x200
#define CWMP_LOG_DEBUG_SQL       0x400

#ifdef WIN32
#define FUNCTION_TRACE()	cwmp_log_debug("TRACE: %s()\n", __FUNCTION__)

#else

#define FUNCTION_TRACE()	cwmp_log_debug("TRACE: %s()\n", __func__)
#endif

typedef struct cwmp_log_t cwmp_log_t;

int cwmp_log_init(const char * filename, int level);
void cwmp_log_fini();



void cwmp_log_debug(const char * fmt, ...);
void cwmp_log_info(const char * fmt, ...);
void cwmp_log_error(const char * fmt, ...);
void cwmp_log_alert(const char * fmt, ...);
void cwmp_log_critical(const char * fmt, ...);
void cwmp_log_tracer(int level, cwmp_log_t *log, const char * fmt, ...);





#endif





