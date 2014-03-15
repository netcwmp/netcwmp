/************************************************************************
 *                                                                      *
 * Netcwmp/Opencwmp Project                                             *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU General Public License          *
 * as published by the Free Software Foundation; either version 2       *
 * of the License, or (at your option) any later version.               *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the                *
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,          *
 * Boston, MA  02111-1307 USA                                           *
 *                                                                      *
 * Copyright 2013-2014  Mr.x(Mr.x) <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#include "cwmp_module.h"
#include "cwmp_thread.h"
#include "cwmp_httpd.h"


static int cwmp_max_threads;

static int 				cwmp_worker_threaded;


#ifdef WIN32
#else
static pthread_attr_t  	cwmp_worker_thread_attr;

int
cwmp_worker_thread_init(cwmp_t * cwmp, int num, size_t size)
{
    int  err;

    cwmp_max_threads = num;

    err = pthread_attr_init(&cwmp_worker_thread_attr);

    if (err != 0)
    {
        cwmp_log_error("pthread_attr_init() failed");
        return CWMP_ERROR;
    }

    err = pthread_attr_setstacksize(&cwmp_worker_thread_attr, size);

    if (err != 0)
    {
        cwmp_log_error("pthread_attr_setstacksize() failed");

        return CWMP_ERROR;
    }

    cwmp_worker_threaded = 1;

    return CWMP_OK;
}
#endif




static void * cwmp_worker_thread_agent(cwmp_t * cwmp)
{
    cwmp_agent_session(cwmp);
    return NULL;
}


static void * cwmp_worker_thread_udpd(cwmp_t * cwmp)
{

    return NULL;
}

#ifdef WIN32
static unsigned int __stdcall cwmp_worker_thread_httpd(cwmp_t * cwmp)
#else
static unsigned int cwmp_worker_thread_httpd(cwmp_t * cwmp)
#endif
{
    return httpd_build_server(cwmp);
}

#ifdef WIN32
static unsigned int __stdcall cwmp_worker_thread_tasks(cwmp_t * cwmp)
#else
static unsigned int cwmp_worker_thread_tasks(cwmp_t * cwmp)
#endif
{
    //tasks_build_server(cwmp);
    pthread_exit(0);
    return 0;
}




void cwmp_worker_thread_start(cwmp_t * cwmp)
{
#ifdef WIN32
    unsigned tid1, tid2;
    CreateThread(NULL, 0, cwmp_worker_thread_httpd, cwmp, 0, &tid1);
//    CreateThread(NULL, 0, cwmp_worker_thread_tasks, cwmp, 0, &tid2);
#else
    pthread_t th1, th2, th3;
    pthread_create(&th1, NULL, (void*)cwmp_worker_thread_httpd, cwmp);
//    pthread_create(&th2, NULL, (void*)cwmp_worker_thread_tasks, cwmp);

#endif
    cwmp_agent_session(cwmp);

}

