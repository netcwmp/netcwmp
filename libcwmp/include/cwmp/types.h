/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                            *
 *                                                                      *
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#ifndef __CWMPTYPES_H__
#define __CWMPTYPES_H__


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <sys/timeb.h>
#include <malloc.h>
#include <ctype.h>



#ifdef WIN32

#include <winsock2.h>
#include <windows.h>
#include <process.h>

#else

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <unistd.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<netinet/in.h>
#include<net/if_arp.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef USE_CWMP_OPENSSL
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#endif


#endif

#include <cwmp/error.h>
#include <cwmp/pool.h>

#ifdef WIN32
#define TRC_STDCALL __stdcall
#else
#define TRC_STDCALL
#endif


#ifndef TRDECL
#ifdef WIN32
#ifndef TRC_DLL
#define TRDECL(type) extern type __stdcall
#define TRDECL_DATA  extern
#else /* TR_DLL */

#ifdef TRC_EXPORTS
#define TRDECL(type) __declspec(dllexport) type __stdcall
#define TRDECL_DATA  extern __declspec(dllexport)
#else
#define TRDECL(type) __declspec(dllimport) type __stdcall
#define TRDECL_DATA  extern __declspec(dllimport)
#endif

#endif /* TRC_DLL */

#else /* WIN32 */
#define TRDECL(type)  type
#define TRDECL_DATA
#endif /* WIN32 */

#endif /* TRDECL */





typedef enum  TRBOOLEAN
{ TRFalse = 0, TRTrue = !TRFalse } TRBOOLEAN;

#ifndef TRUE
#define TRUE TRTrue
#endif

#ifndef FALSE
#define FALSE TRFalse
#endif


typedef int							cwmp_pid_t;

typedef unsigned int 		cwmp_uint_t;
typedef int	cwmp_flag_t;

typedef	int							cwmp_fd_t;

///* Typedefs that OT needs. */

typedef  unsigned char        cwmp_byte_t;
typedef  short                		cwmp_int16_t;
typedef  unsigned short      cwmp_uint16_t;
typedef  int                  		cwmp_int32_t;
typedef  unsigned int         	cwmp_uint32_t;

typedef  long long            	cwmp_int64_t;
typedef  unsigned long long   cwmp_uint64_t;
typedef  size_t               		cwmp_size_t;
//typedef  ssize_t              		cwmp_ssize_t;
//typedef  off64_t              		cwmp_off_t;
typedef  int            			cwmp_socklen_t;
typedef  unsigned long        	cwmp_ino_t;

#define CWMP_SIZEOF_VOIDP 4

#if CWMP_SIZEOF_VOIDP == 8
typedef  cwmp_uint64_t          	cwmp_uintptr_t;
#else
typedef  cwmp_uint32_t          	cwmp_uintptr_t;
#endif


#define TR_INET4   AF_INET
#define TR_INET6   AF_INET6
#define TR_TCP	    SOCK_STREAM
#define TR_UDP	    SOCK_DGRAM



# define ISSPACE(x) isspace (x)
# define ISDIGIT(x) isdigit (x)
# define ISXDIGIT(x) isxdigit (x)
# define ISALPHA(x) isalpha (x)
# define ISALNUM(x) isalnum (x)
# define TOLOWER(x) tolower (x)
# define TOUPPER(x) toupper (x)

#define DIGIT_TO_NUM(h) ((h) < 'A' ? (h) - '0' : TOUPPER (h) - 'A' + 10)
#define X2DIGITS_TO_NUM(h1, h2) ((DIGIT_TO_NUM (h1) << 4) + DIGIT_TO_NUM (h2))


#define NUM_TO_DIGIT(x) ("0123456789ABCDEF"[x] + 0)
#define NUM_TO_digit(x) ("0123456789abcdef"[x] + 0)



/********************************************************************************
*
* time
*
 ********************************************************************************/

#define TR_INT64_C(val) (val##LL)
#define TR_UINT64_C(val) (val##ULL)


#define TR_TIME_C(val)		TR_INT64_C(val)
#define TR_USEC_PER_SEC	TR_TIME_C(1000000)

/** @return TRTime as a second */
#define TringleTimeSec(time) ((time) / TR_USEC_PER_SEC)

/** @return TRTime as a usec */
#define TringleTimeUsec(time) ((time) % TR_USEC_PER_SEC)

/** @return TRTime as a msec */
#define TringleTimeMsec(time) (((time) / 1000) % 1000)


/** @return TRTime as a msec milisecond */
#define TringleTimeAsMsec(time) ((time) / 1000)

//    int GetCurrentTime(char * buf);

#ifdef WIN32
#define close_fd    closesocket
#else
#define close_fd    close
#endif








typedef struct cwmp_st cwmp_t;
typedef struct device_id_st device_id_t;
typedef struct event_code_st event_code_t;
typedef struct event_list_st event_list_t;
typedef struct datatime_st datatime_t;
typedef struct parameter_value_node_st parameter_value_node_t;
typedef struct parameter_st parameter_t;
typedef struct parameter_list_st parameter_list_t;
typedef struct header_st header_t;

typedef struct parameter_node_st parameter_node_t;
typedef struct parameter_node_attr_st parameter_node_attr_t;
typedef struct download_arg_st download_arg_t;
typedef struct upload_arg_st upload_arg_t;
typedef struct fault_code_st fault_code_t;
typedef struct model_func_st model_func_t;




typedef int (*callback_func_t)(void *arg1, void *arg2);
typedef int (*callback_register_func_t)(cwmp_t * cwmp, callback_func_t, void * arg1, void *arg2);

typedef int   	(*parameter_get_handler_pt)(cwmp_t * cwmp, const char * param_name, char ** value, pool_t * pool);

typedef int     (*parameter_set_handler_pt)(cwmp_t * cwmp, const char * param_name, const char * value, int length, callback_register_func_t callback_reg);

typedef int     (*parameter_notify_handler_pt)(cwmp_t * cwmp, const char * param_name, const char * value, int length, callback_register_func_t callback_reg);

typedef int     (*parameter_add_handler_pt)(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg);

typedef int     (*parameter_del_handler_pt)(cwmp_t * cwmp, parameter_node_t * param_node, int instance_number,  callback_register_func_t callback_reg);

typedef int     (*parameter_refresh_handler_pt)(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg ); 

typedef enum cwmp_type_t cwmp_type_t;



#endif

