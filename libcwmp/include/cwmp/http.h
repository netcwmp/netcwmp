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

#ifndef __CWMPHTTP_H__
#define __CWMPHTTP_H__

#include <cwmp/types.h>
#include <cwmp/cwmp.h>
#include <cwmp/buffer.h>
#include <cwmp/pool.h>

#define CIPHER_LIST "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"


#define MIN_DEFAULT_LEN     32
#define URL_SCHEME_LEN      8
#define URL_USER_LEN        64
#define URL_PWD_LEN         64
#define MAX_HEADERS         64
#define MAX_HOST_NAME_LEN   128
#define MAX_URI_LEN         512
#define MIN_BUFFER_LEN      256
#define HTTP_DEFAULT_LEN         1024

#define HTTPP_VAR_PROTOCOL      "__protocol__"
#define HTTPP_VAR_VERSION       "__version__"
#define HTTPP_VAR_URI           "__uri__"
#define HTTPP_VAR_RAWURI        "__rawuri__"
#define HTTPP_VAR_REQ_TYPE      "__req_type__"
#define HTTPP_VAR_ERROR_MESSAGE "__errormessage__"
#define HTTPP_VAR_ERROR_CODE    "__errorcode__"
#define HTTPP_VAR_PASSWORD      "__password__"

#define HTTP_NONE_AUTH      0x00
#define HTTP_BASIC_AUTH     0x01
#define HTTP_DIGEST_AUTH    0x02



#define HTTP_100		100
#define HTTP_200		200
#define HTTP_400		400







typedef enum
{
    HTTP_GET,
    HTTP_PUT,
    HTTP_POST,
    HTTP_OPTIONS,
    HTTP_HEAD,
    HTTP_DELETE,
    HTTP_TRACE,
    HTTP_UNKNOWN
} http_method_t;


typedef struct key_value_t key_value_t;
typedef key_value_t http_header_t;
typedef key_value_t http_query_t;
typedef struct http_parser_t        http_parser_t;
typedef struct http_dest_t          http_dest_t;
typedef struct http_request_t       http_request_t;
typedef struct http_response_t      http_response_t;
typedef struct http_socket_t        http_socket_t;
typedef struct http_sockaddr_t      http_sockaddr_t;
typedef struct http_digest_auth_t   http_digest_auth_t;

typedef size_t (*http_write_callback_pt)(char *data, size_t size, size_t nmemb, void * calldata);



struct http_request_t
{
    int method;
    int major;
    int minor;
    http_parser_t * parser;

    http_query_t  *query;


    http_dest_t * dest;

    void * data;
    cwmp_uint32_t length;
    cwmp_chunk_t  * writers;
} ;



struct http_response_t
{
    int major;
    int minor;
    int status;

    http_parser_t * parser;
    cwmp_chunk_t  * readers;
    void * data;
} ;

void socket_set_recv_timeout(int fd, int timeout);

int http_parse_url(http_dest_t * dest, const char * url);

int http_dest_create(http_dest_t ** dest, const char * url, pool_t * pool);
int http_request_create(http_request_t ** request , pool_t * pool);
int http_response_create(http_response_t ** response, pool_t * pool);

int http_read_header(http_socket_t * sock, cwmp_chunk_t * header, pool_t * pool);
int http_read_body(http_socket_t * sock, int max);//, cwmp_chunk_t * body, pool_t * pool);
int http_read_request(http_socket_t * sock, http_request_t * request, pool_t * pool);
int http_read_response(http_socket_t * sock, http_response_t * response, pool_t * pool);

char * http_get_variable(http_parser_t * parser, const char *name);
void http_set_variable(http_parser_t *parser, const char *name, const char *value, pool_t * pool);

int http_socket_read (http_socket_t * sock, char *buf, int bufsize);
int http_socket_write (http_socket_t * sock, const char *buf, int bufsize);
int http_socket_create(http_socket_t **news, int family, int type, int protocol, pool_t * pool);
int http_socket_calloc(http_socket_t **news, pool_t * pool);
int http_socket_server (http_socket_t **news, int port, int backlog, int timeout, pool_t * pool);
int http_socket_connect(http_socket_t * sock, int family, const char * host, int port);
int http_socket_accept(http_socket_t *sock, http_socket_t ** news);
void http_socket_close(http_socket_t * sock);
void http_socket_destroy(http_socket_t * sock);
int http_socket_get_fd(http_socket_t * sock);
pool_t * http_socket_get_pool(http_socket_t * sock);
void http_socket_set_recvtimeout(http_socket_t * sock, int timeout);
void http_socket_set_sendtimeout(http_socket_t * sock, int timeout);

int http_check_digest_auth(const char * auth_realm, const char * auth, char * cpeuser, char * cpepwd);
int http_parse_digest_auth(const char * auth, http_digest_auth_t * digest_auth);
int http_parse_cookie(const char * cookie, char * dest_cookie);
int http_socket_set_writefunction(http_socket_t * sock, http_write_callback_pt callback, void * calldata);


int http_post(http_socket_t * sock, http_request_t * request, cwmp_chunk_t * data, pool_t * pool);


#ifdef USE_CWMP_OPENSSL

int openssl_check_cert(SSL *ssl, char *host);
SSL * openssl_connect(SSL_CTX * ctx, int fd);
SSL_CTX *openssl_initialize_ctx(char *keyfile,char *password);


#endif

#endif

