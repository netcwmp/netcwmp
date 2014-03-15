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

#include <cwmp/http.h>
#include <cwmp/event.h>
#include "cwmp_httpd.h"


#define MAX_CLIENT_NUMS 8


static char * AuthRealm = "cwmpd";
static char * AuthQop = "auth";
static char   AuthOpaque[33] = {0};
static int	  AuthNonce = 0;

const char * RESPONSE_200 = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 2\r\nContent-Type: text/xml; charset=\"utf-8\"\r\n\r\nOK";
const char * RESPONSE_400 = "HTTP/1.1 400 Bad request\r\nServer: CWMP-Agent\r\nConnection: close\r\nContent-Length: 5\r\n\r\nError";
const char * RESPONSE_401 = "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Digest qop=\"%s\" nonce=\"%s\" opaque=\"%s\" realm=\"%s\"\r\nServer: TR069Agent\r\nContent-Length: 0\r\n\r\n";


struct http_session_fd_t
{
    //int fd;
    time_t time;
    http_socket_t * sock;
};


struct http_session_fd_t sessionfd[MAX_CLIENT_NUMS];


void setnonblocking(int fd)
{
#ifdef WIN32
#else
    int opts;
    opts=fcntl(fd, F_GETFL);
    if (opts < 0)
    {
        cwmp_log_error("setnonblocking fcntl GETFL failed: fd(%d)\n", fd);
        return;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) < 0)
    {
        cwmp_log_error("setnonblocking fcntl SETFL failed: fd(%d)\n", fd);
        return;
    }
    return;
#endif
}



int httpd_response_unauthorization(http_socket_t * sock)
{

    char buffer[256];
    char nonce[33];
    FUNCTION_TRACE();
    AuthNonce ++;
    TRsnprintf(buffer, 256,  "%d", AuthNonce);
    MD5(nonce, buffer, NULL);

    nonce[32] = 0;

    TRsnprintf(buffer, 256, RESPONSE_401, AuthQop, nonce, AuthOpaque, AuthRealm);


    return	http_socket_write(sock, buffer, TRstrlen(buffer));
}

int httpd_response_ok(http_socket_t * sock)
{
    FUNCTION_TRACE();
    return	http_socket_write(sock, RESPONSE_200, TRstrlen(RESPONSE_200));
}

int httpd_response_unkonw_error(http_socket_t * sock)
{
    FUNCTION_TRACE();
    return	http_socket_write(sock, RESPONSE_400, TRstrlen(RESPONSE_400));
}

int httpd_build_server(cwmp_t * cwmp)
{
    http_socket_t * lsnsock;
    pool_t * pool;
    int rc;
    int lsnfd, maxfd, nready;
    int i;

    int fd, newfd;
    http_socket_t * s;
    http_request_t * request;

    char * auth;
    time_t now;
    fd_set readset, rdset;
    struct timeval timeout;
    int port;

    char  cpe_user[INI_BUFFERSIZE] = {0};
    char  cpe_pwd[INI_BUFFERSIZE] = {0};



    port = cwmp->httpd_port;

    pool = pool_create(POOL_DEFAULT_SIZE);
    rc = http_socket_server(&lsnsock, port, 5, -1, pool);
    if (rc != CWMP_OK)
    {
        cwmp_log_error("build httpd server faild. %s\n", strerror(errno));
        exit(-1);
    }


    lsnfd = http_socket_get_fd(lsnsock);

    for (i=0; i < MAX_CLIENT_NUMS; i++)
    {

        sessionfd[i].time = 0;
        sessionfd[i].sock = NULL;
    }

    FD_ZERO(&readset);
    FD_SET(lsnfd, &readset);

    maxfd = lsnfd;
    /*maxi = -1;*/
    while (1)
    {
        FD_ZERO(&rdset);
        rdset = readset;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        if ((nready = select(maxfd + 1, &rdset, NULL, NULL, &timeout)) <= 0)
        {
            sleep(1);
            cwmp_log_info("select new connection timeout. no new request.\n");
            now = time(NULL);
            for (i=0; i<MAX_CLIENT_NUMS; i++)
            {
                //cwmp_log_debug("socket time: %d, timeout %d, fd is %d\n", sessionfd[i].time, now -  sessionfd[i].time,
                //               sessionfd[i].sock == NULL? -1 : http_socket_get_fd(sessionfd[i].sock));
                fd = http_socket_get_fd(sessionfd[i].sock);
                if ((sessionfd[i].sock != NULL) && (now -  sessionfd[i].time > 15))
                {
                    cwmp_log_info("close a timeout socket. fd is %d.\n", fd);
                    FD_CLR(fd, &readset);
                    //http_socket_close(sessionfd[i].sock);
                    http_socket_destroy(sessionfd[i].sock);
                    sessionfd[i].time = 0;
                    sessionfd[i].sock = NULL;
                }
            }
            continue;
        }

        if (FD_ISSET(lsnfd, &rdset))
        {
            http_socket_t * newsock;
            //FIXME
            http_socket_accept(lsnsock, &newsock);
            newfd = http_socket_get_fd(newsock);

            for (i=0; i<MAX_CLIENT_NUMS; i++)
            {
                if (sessionfd[i].sock == NULL)
                {
                    sessionfd[i].sock = newsock;
                    sessionfd[i].time = time(NULL);
                    break;
                }
            }
            if (i == MAX_CLIENT_NUMS)
            {
                //http_socket_close(newsock);
                http_socket_destroy(newsock);

                cwmp_log_error("too many ACS request connection");
                continue;
            }
            FD_SET(newfd, &readset);
            if (newfd > maxfd)
            {
                maxfd = newfd;
            }

            newfd = -1;
            if (--nready <= 0)
            {
                continue;
            }

        }

        //readpool = pool_create(POOL_DEFAULT_SIZE);
        cwmp_log_debug("nready is %d.\n", nready);
        for (i=0; (i<MAX_CLIENT_NUMS) && (nready > 0) ; i++)
        {
            s = sessionfd[i].sock;
            fd = http_socket_get_fd(s);

            if ((fd != -1) && FD_ISSET(fd, &rdset))
            {
                nready--;
                sessionfd[i].time = time(NULL);
                http_request_create(&request, http_socket_get_pool(s));
                rc = http_read_request(s, request, http_socket_get_pool(s));
                if (rc <= 0)
                {
                    httpd_response_unkonw_error(s);
                    goto faild;
                }

                if (request->method != HTTP_GET)
                {
                    httpd_response_unkonw_error(s);
                    goto faild;
                }

                if (cwmp->cpe_auth)
                {
                    auth = http_get_variable(request->parser, "Authorization");

                    if (!auth)
                    {
                        httpd_response_unauthorization(s);
                        goto faild;
                    }


                    cwmp_conf_get("cwmp:cpe_username", cpe_user);
                    cwmp_conf_get("cwmp:cpe_password", cpe_pwd);
					
                    cwmp_log_debug("cpe username: %s, cpe password: %s\n", cpe_user, cpe_pwd);


                    if (http_check_digest_auth(AuthRealm, auth, cpe_user, cpe_pwd) != 0)
                    {
                        httpd_response_unauthorization(s);
						goto faild;
                    }
                }
                
                httpd_response_ok(s);

                //get a new request from acs
                cwmp->new_request = CWMP_YES;
				cwmp_log_debug("set cwmp new request to %d\n", cwmp->new_request);
				cwmp_event_set_value(cwmp, INFORM_CONNECTIONREQUEST, 1, NULL, 0, 0, 0);

                

faild:

                FD_CLR(fd, &readset);
                sessionfd[i].time = 0;
                sessionfd[i].sock = NULL;
                http_socket_destroy(s);


            }

        }



    }

}


