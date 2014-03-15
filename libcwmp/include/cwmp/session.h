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

#ifndef __CWMPSESSION_H__
#define __CWMPSESSION_H__

#include <cwmp/cwmp.h>
#include <cwmp/buffer.h>
#include <cwmp/http.h>




#define ACS_CA_PATH         "/etc/acs.crt"
#define CPE_CA_PATH         "/etc/cpe.crt"
#define CPE_COOKIE_FILE     "/tmp/cwmp.cookie"
#define UPLOAD_CONF_FILE     "/backup/config.cfg"
#define UPLOAD_LOG_FILE      "/var/log/cwmplog.tar.gz"

typedef struct cwmp_session_t cwmp_session_t;


struct cwmp_session_t
{
    cwmp_t * cwmp;
    int status;
    int reconnect;
    int newdata;
    int	timeout;
    int retry_count;

    int	last_method;
    int last_code;

    env_t	* env;
    pool_t * pool;
    pool_t * connpool;
    pool_t * envpool;
    cwmp_chunk_t * writers;
    cwmp_chunk_t * readers;



    http_socket_t * sock;
    http_dest_t   * dest;

    char 	id[128];
    char error_buffer[256];



    parameter_node_t * root;
};


#define ParameterFormatEnd  ".%s"
#define ParameterFormat1    "%s"
#define ParameterFormat2 ParameterFormat1 ParameterFormatEnd
#define ParameterFormat3 ParameterFormat2 ParameterFormatEnd
#define ParameterFormat4 ParameterFormat3 ParameterFormatEnd
#define ParameterFormat5 ParameterFormat4 ParameterFormatEnd
#define ParameterFormat6 ParameterFormat5 ParameterFormatEnd
#define ParameterFormat7 ParameterFormat6 ParameterFormatEnd
#define ParameterFormat8 ParameterFormat7 ParameterFormatEnd
#define ParameterFormat9 ParameterFormat8 ParameterFormatEnd
#define ParameterFormat10 ParameterFormat9 ParameterFormatEnd
#define ParameterFormat11 ParameterFormat10 ParameterFormatEnd
#define ParameterFormat12 ParameterFormat11 ParameterFormatEnd

#define CWMP_APPEND_PARAMETER_NAME  cwmp_data_append_parameter_name
#define CWMP_SPRINTF_PARAMETER_NAME cwmp_data_sprintf_parameter_name

char *	cwmp_data_append_parameter_name(pool_t * pool, int count,  ...);
int	    cwmp_data_sprintf_parameter_name(char * buffer, int count, ...);
char *  cwmp_data_get_parameter_value(cwmp_t * cwmp, parameter_node_t * root, const char * name, pool_t * pool);

int cwmp_data_set_parameter_value(cwmp_t * cwmp, parameter_node_t * root, const char * name, const char * value, int value_length, pool_t * pool);

void    cwmp_agent_session(cwmp_t * cwmp);
cwmp_session_t * cwmp_session_create(cwmp_t * cwmp);

int     cwmp_session_open(cwmp_session_t * session);
int     cwmp_session_close(cwmp_session_t * session);
int     cwmp_session_connect(cwmp_session_t * session, const char * url);
void    cwmp_session_free(cwmp_session_t * session);

int     cwmp_session_send_request(cwmp_session_t * session);
int     cwmp_session_recv_response(cwmp_session_t * session);

int     cwmp_session_create_connection(cwmp_session_t * session);
int     cwmp_session_set_auth(cwmp_session_t * session, const char * user, const char * pwd);
int     cwmp_session_set_headers(cwmp_session_t * session, int postempty);

device_id_t *       cwmp_session_create_inform_device(cwmp_session_t * session, pool_t * pool);
parameter_list_t *  cwmp_session_create_inform_parameters(cwmp_session_t * session, pool_t * pool);

datatime_t *        cwmp_session_create_inform_datetimes(cwmp_session_t * session, pool_t * pool);




xmldoc_t *  cwmp_session_create_getrpcmethods_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);

xmldoc_t *  cwmp_session_create_inform_message(cwmp_session_t * session, event_list_t * evtlist,  pool_t * pool);
xmldoc_t *  cwmp_session_create_getparameternames_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);
xmldoc_t *  cwmp_session_create_getparametervalues_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);
xmldoc_t *  cwmp_session_create_setparametervalues_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);
xmldoc_t *  cwmp_session_create_download_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);
xmldoc_t *  cwmp_session_create_upload_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);
xmldoc_t *  cwmp_session_create_reboot_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);
xmldoc_t *  cwmp_session_create_addobject_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);
xmldoc_t *  cwmp_session_create_deleteobject_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);

xmldoc_t *  cwmp_session_create_transfercomplete_message(cwmp_session_t * session, event_code_t * evcode,  pool_t * pool);
xmldoc_t *  cwmp_session_create_factoryreset_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool);


int cwmp_session_get_localip(char *hostip);

#endif
