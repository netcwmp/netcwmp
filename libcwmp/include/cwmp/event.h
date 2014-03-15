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

#ifndef __CWMPEVENT_H__
#define __CWMPEVENT_H__

#include <cwmp/types.h>

typedef enum
{
    EVENT_BOOTSTRAP = 0,
    EVENT_BOOT,
    EVENT_PERIODIC,
    EVENT_SCHEDULED,
    EVENT_VALUECHANGE,
    EVENT_KICKED,
    EVENT_CONNECTIONREQUEST,
    EVENT_TRANSFERCOMPLETE,
    EVENT_DIAGNOSTICSCOMPLETE,
    EVENT_REQUESTDOWNLOAD,
    EVENT_AUTONOMOUSTRANSFERCOMPLETE,
    EVENT_MREBOOT,
    EVENT_MSCHEDULEINFORM,
    EVENT_MDOWNLOAD,
    EVENT_MUPLOAD,
    EVENT_MAXCOUNT
}EventType;




#define COMMAND_KEY_LEN         32

#define POLICY_NEVER_DISCARD    0
#define POLICY_DISCARD_BOOT     1
#define POLICY_NOT_RETRY        2


#define EVENT_REBOOT_NONE_FLAG   0
#define EVENT_REBOOT_BOOTSTRAP_FLAG  1
#define EVENT_REBOOT_ACS_FLAG     2
#define EVENT_REBOOT_TRANSFERCOMPLETE_FLAG 4

#define EVENT_REBOOT_UNKNOWN_FLAG 64



#define TASK_DOWNLOAD_TAG  1
#define TASK_UPLOAD_TAG  2
#define TASK_REBOOT_TAG  3
#define TASK_FACTORYRESET_TAG 4
#define TASK_CALLBACK_TAG 5 


struct event_code_st
{
    int   	event;
    char *	code;
    char   	command_key[COMMAND_KEY_LEN+1];
    int      	policy;     /* 0:始终不能丢弃，1:直到重启，丢弃  2:不需要retry，可以丢弃 */
    int      	have_key;   /* 0: 没有 1:有*/
    int      	ref;        /*事件发送次数*/
    int      	fault_code;
    time_t	start;
    time_t	end;	
};


struct event_list_st
{
    event_code_t ** events;
    int count;
    int size;
};




typedef struct event_global_st
{
    int      	event_flag;        /*0 其他原因重启 1 reboot命令重启2由于升级导致的重启*/
    char   	event_key[COMMAND_KEY_LEN+1]; /* command key */
    int      	fault_code;
    time_t	start;
    time_t	end;	
	
}event_global_t;


typedef struct transfer_st{
    int     type;   //0 无用 1 download 2 upload
    char    commad_key[COMMAND_KEY_LEN+1];
    int      fault_code;
    time_t  start_tm;
    time_t  end_tm;
}transfer_t;


typedef struct inform_event_st inform_event_t;
struct inform_event_st
{
    int event;
    int behavior;  /* 0:Single  1:Multiple  2:Not specified*/
    char * code;
    char * key;
    char * command;
};



int cwmp_event_init(cwmp_t *cwmp);
event_list_t * cwmp_event_list_create(pool_t * pool, int size);
event_code_t * cwmp_event_code_create(pool_t * pool );
int cwmp_event_list_init(pool_t * pool, event_list_t * el);
int cwmp_event_global_init(cwmp_t * cwmp);


int cwmp_event_set_value(cwmp_t *cwmp,  int event,   int value, const char * cmd_key, int fault_code, time_t start, time_t end);
int cwmp_event_clear_active(cwmp_t *cwmp);

#endif




