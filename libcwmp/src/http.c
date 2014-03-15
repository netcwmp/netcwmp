/************************************************************************
 * Id: http.c                                                           *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/
 
#include "cwmp/http.h"
#include "cwmp/log.h"
#include "cwmp_private.h"
#include <cwmp/md5.h>




struct http_sockaddr_t
{
    struct sockaddr_in sin4;

#if HAVE_IPV6
    /** IPv6 sockaddr structure */
    struct sockaddr_in6 sin6;
#endif
};






char * http_get_variable(http_parser_t * parser, const char *name)
{
    int i;
    for (i=0; i<parser->count; i++)
    {
        if (TRstrcasecmp(parser->header[i]->name, name) == 0)
        {
            return parser->header[i]->value;
        }
    }

    return NULL;

}

void http_set_variable(http_parser_t *parser, const char *name, const char *value, pool_t * pool)
{
    key_value_t *var;

    //FUNCTION_TRACE();

    if (name == NULL || value == NULL)
        return;


    var = (key_value_t *)pool_pcalloc(pool, sizeof(key_value_t));
    if (var == NULL)
    {
        return;
    }

    var->name = pool_pstrdup_lower(pool, name);
    var->value = pool_pstrdup(pool, value);
    if (parser->count >= MAX_HEADERS)
    {
        return;
    }
    parser->header[parser->count++] = var;
}


int http_connect(http_socket_t * sock, const char * url)
{
    return 0;
}

int http_dest_create(http_dest_t ** dest, const char * url, pool_t * pool)
{
    http_dest_t * d = (http_dest_t*)pool_pcalloc(pool, sizeof(http_dest_t));
//    cwmp_uint32_t length = TRstrlen(url);
    http_parse_url(d, url);
    d->url = pool_pstrdup(pool, url);
//	d->url = (char *)pool_pcalloc(pool, length+1);
//	strncpy(d->url, url, length);
    cwmp_log_debug("dest create url is %s", d->url);
    *dest = d;
    return CWMP_OK;
}

void http_sockaddr_set(http_sockaddr_t * addr, int family, int port, const char * host)
{
    addr->sin4.sin_family = family;

    if (family == AF_INET)
    {

    }

    if (port)
    {
        addr->sin4.sin_port = htons((unsigned short)port);
    }

    if (host)
    {
		//inet_aton(host, &addr->sin4.sin_addr);
        addr->sin4.sin_addr.s_addr = inet_addr(host);
    }
    else
    {
       // addr->sin4.sin_addr.s_addr = INADDR_ANY;
    }
}


int http_socket_calloc(http_socket_t **news, pool_t * pool)
{
    (*news) = (http_socket_t *)pool_pcalloc(pool, sizeof(http_socket_t));

    if ((*news) == NULL)
    {
        cwmp_log_error("socket create pool pcalloc null.\n");
        return CWMP_ERROR;
    }

    (*news)->addr = (http_sockaddr_t*)pool_pcalloc(pool, sizeof(http_sockaddr_t));
    if ((*news)->addr == NULL)
    {
        (*news) = NULL;
        cwmp_log_error("http_sockaddr_t  pool pcalloc  null.\n");
        return CWMP_ERROR;
    }
    (*news)->sockdes = -1;
    (*news)->timeout = -1;
    (*news)->pool = pool;


    pool_cleanup_add(pool, (pool_cleanup_handler)http_socket_close, (*news));
    return CWMP_OK;
}


int http_socket_create(http_socket_t **news, int family, int type, int protocol, pool_t * pool)
{
    int stat;
    stat = http_socket_calloc(news, pool);
    if (stat == CWMP_ERROR)
    {
        return CWMP_ERROR;
    }

   
    (*news)->sockdes = socket(family, type, protocol);

#if HAVE_IPV6
    if ((*news)->sockdes == -1)
    {
        family = AF_INET;
        (*news)->sockdes = socket(family, type, protocol);
    }
#endif

    if ((*news)->sockdes == -1)
    {
        cwmp_log_error("sockdes is -1.\n");
        return - errno;
    }

    (*news)->type = type;
    (*news)->protocol = protocol;
    http_sockaddr_set((*news)->addr,family, 0, NULL);
    (*news)->timeout = -1;

    return CWMP_OK;
}

int http_socket_server (http_socket_t **news, int port, int backlog, int timeout, pool_t * pool)
{
    int i;
    http_socket_t * sock;
    int rc;

    rc = http_socket_create(&sock, AF_INET, SOCK_STREAM, 0, pool);
    if (rc != CWMP_OK)
    {
        cwmp_log_error("http_socket_create faild. %s", strerror(errno));
        return CWMP_ERROR;
    }
    i = 1;
    if (setsockopt (sock->sockdes, SOL_SOCKET, SO_REUSEADDR, (void *)&i, sizeof i) == -1)
    {
        cwmp_log_error ("http_socket_server: setsockopt SO_REUSEADDR: %sock", strerror (errno));
    }

    http_sockaddr_set(sock->addr, AF_INET, port, NULL);

    if (bind (sock->sockdes, (struct sockaddr *)&sock->addr->sin4, sizeof (struct sockaddr)) == -1)
    {
        http_socket_close (sock);
        return CWMP_ERROR;
    }

    if (listen (sock->sockdes, (unsigned)backlog) == -1)
    {
        http_socket_close (sock);
        return CWMP_ERROR;
    }

    *news = sock;

    return CWMP_OK;


}


int http_socket_connect(http_socket_t * sock, int family, const char * host, int port)
{ 

    http_sockaddr_set(sock->addr, family, port, host);
    if (connect(sock->sockdes, (const struct sockaddr *)&sock->addr->sin4,
                sizeof(struct sockaddr_in)) == -1)
    {
        return CWMP_ERROR;
    }   
         
       

    return CWMP_OK; 
}

int http_socket_accept(http_socket_t *sock, http_socket_t ** news)
{
    struct sockaddr addr;
    size_t len;
    pool_t * pool;
    int rc, s;
    cwmp_log_debug("TRACE: socket_tcp_accept\n");

    len = sizeof addr;
    s = accept (sock->sockdes, &addr, &len);
    if (s == -1)
    {
        return CWMP_ERROR;
    }

    pool = pool_create(POOL_DEFAULT_SIZE);
    rc = http_socket_calloc(news, pool);
    if (rc != CWMP_OK)
    {
        return CWMP_ERROR;
    }
    (*news)->sockdes = s;
    memcpy(&(*news)->addr->sin4, &addr, sizeof(struct sockaddr_in));


    return CWMP_OK;

}



void http_socket_close(http_socket_t * sock)
{
    FUNCTION_TRACE();
    if (sock)
    {
        if (sock->sockdes != -1)
        {
#ifdef WIN32
            closesocket(sock->sockdes);
#else
            close(sock->sockdes);
#endif
            sock->sockdes = -1;
        }

    }

}

void http_socket_destroy(http_socket_t * sock)
{
    pool_t * pool;
    pool = sock->pool;

    pool_destroy(pool);

}

int http_socket_get_fd(http_socket_t * sock)
{
    if (sock)
        return sock->sockdes;
    else
        return -1;
}

pool_t * http_socket_get_pool(http_socket_t * sock)
{
    if(sock && sock->pool)
    {
        return sock->pool;
    }
    else
    {
        return NULL;
    }
}


int http_socket_read (http_socket_t * sock, char *buf, int bufsize)
{
    int res = 0;
    
    if(sock->use_ssl)
    {
        
#ifdef USE_CWMP_OPENSSL        
        do
        {
            res = SSL_read(sock->ssl, buf, bufsize);
        }
        while (res == -1 && errno == EINTR);
#endif   
        return res;
    }
    else
    {        
        do
        {
            res = recv (sock->sockdes, buf, bufsize, 0);
        }
        while (res == -1 && errno == EINTR);
        return res;
        
    }
}

int http_socket_write (http_socket_t * sock, const char *buf, int bufsize)
{
    int res = 0;
    if(sock->use_ssl)
    {
        cwmp_log_debug("http socket ssl write buffer: %s, length: %d", buf, bufsize);
#ifdef USE_CWMP_OPENSSL
        do
        {
            
            res = SSL_write (sock->ssl, buf, bufsize);
        }
        while (res == -1 && errno == EINTR);
#endif        
        return res;
    }
    else
    {
        cwmp_log_debug("http socket write buffer fd:%d, length:%d,  [\n%s\n]", sock->sockdes, bufsize, buf);
        do
        {
            
            res = send (sock->sockdes, buf, bufsize, 0);
        }
        while (res == -1 && errno == EINTR);
        return res;
        
    }    
}

void http_socket_set_sendtimeout(http_socket_t * sock, int timeout)
{
    struct timeval to;
    to.tv_sec = timeout;
    to.tv_usec = 0;
    sock->timeout = timeout;
    setsockopt(sock->sockdes, SOL_SOCKET, SO_SNDTIMEO,
               (char *) &to,
               sizeof(to));
}

void http_socket_set_recvtimeout(http_socket_t * sock, int timeout)
{
    struct timeval to;
    to.tv_sec = timeout;
    to.tv_usec = 0;
    sock->timeout = timeout;
    setsockopt(sock->sockdes, SOL_SOCKET, SO_RCVTIMEO,
               (char *) &to,
               sizeof(to));
}

int http_socket_set_writefunction(http_socket_t * sock, http_write_callback_pt callback, void * calldata)
{
    if(!sock)
    {
        return CWMP_ERROR;
    }
    sock->write_callback = callback;
    sock->write_calldata = calldata;
    return CWMP_OK;
}


int http_request_create(http_request_t ** request , pool_t * pool)
{
    http_request_t * req;
    req = (http_request_t*)pool_pcalloc(pool, sizeof(http_request_t));
    req->parser = (http_parser_t*)pool_pcalloc(pool, sizeof(http_parser_t));

    *request = req;

    return CWMP_OK;
}

int http_response_create(http_response_t ** response, pool_t * pool)
{
    http_response_t * res;
    res = (http_response_t*)pool_pcalloc(pool, sizeof(http_response_t));
    res->parser = (http_parser_t*)pool_pcalloc(pool, sizeof(http_parser_t));

    *response = res;

    return CWMP_OK;
}


int http_parse_cookie(const char * cookie, char * dest_cookie)
{
    char data[MIN_BUFFER_LEN+1] = {0};
    char * s ;
    char buffer[128];
    char * end;

    FUNCTION_TRACE();

    if (!cookie)
        return CWMP_ERROR;

    for (s =  (char*)cookie; isspace(*s); s++);


    TRstrncpy(dest_cookie, s, MIN_BUFFER_LEN);

    return CWMP_OK;

}


void http_parse_key_value(char ** from, char *to, int len, int shift)
{
    int n;
    char fmt[20];
    char *p = *from + shift;

    *from = p;

    if (*p == '"')//notice that '"' is not two " ,but ' and " and ',Jeff Sun - Jul.24.2005
    {
        TRsnprintf(fmt, sizeof(fmt), "%%%d[^\"]%%n", len - 1);
        p++;
    }
    else
    {
        TRsnprintf(fmt, sizeof(fmt), "%%%d[^ \t,]%%n", len - 1);
    }

    if (sscanf(p, fmt, to, &n))
    {
        p += n;
        *from = p;
    }
}




int http_parse_url(http_dest_t * dest, const char * url)
{
    char *d;
    const char *p, *q;
    const char * uri;
    int i;

    /* allocate struct url */
    //char urlbuf[1024] = {0};
    //strncpy(urlbuf, url, strlen(url));
    FUNCTION_TRACE();
    uri = url;
    /* scheme name */
    if ((p = strstr(url, ":/")))
    {
        TRsnprintf(dest->scheme, URL_SCHEME_LEN+1,
                   "%.*s", (int)(p - uri), uri);
        uri = ++p;
        /*
         * Only one slash: no host, leave slash as part of document
         * Two slashes: host follows, strip slashes
         */
        if (uri[1] == '/')
            uri = (p += 2);
    }
    else
    {
        p = uri;
    }
    if (!*uri || *uri == '/' || *uri == '.')
        goto nohost;

    p = strpbrk(uri, "/@");
    if (p && *p == '@')
    {
        /* username */
        for (q = uri, i = 0; (*q != ':') && (*q != '@'); q++)
            if (i < URL_USER_LEN)
            {
                dest->user[i++] = *q;
            }

        /* password */
        if (*q == ':')
            for (q++, i = 0; (*q != ':') && (*q != '@'); q++)
                if (i < URL_PWD_LEN)
                {
                    dest->password[i++] = *q;
                }

        p++;
    }
    else
    {
        p = uri;
    }

    /* hostname */
#ifdef INET6
    if (*p == '[' && (q = strchr(p + 1, ']')) != NULL &&
            (*++q == '\0' || *q == '/' || *q == ':'))
    {
        if ((i = q - p - 2) > MAX_HOST_NAME_LEN)
            i = MAX_HOST_NAME_LEN;
        strncpy(dest->host, ++p, i);

        p = q;
    }
    else
#endif
        memset(dest->host, 0, MAX_HOST_NAME_LEN+1);
    for (i = 0; *p && (*p != '/') && (*p != ':'); p++)
        if (i < MAX_HOST_NAME_LEN)
        {
            dest->host[i++] = *p;
        }


    /* port */
    if(strncmp(url, "https:", 6) == 0)
    {
        dest->port = 443;    
    }
    else
    {
        dest->port = 80;
    }
    if (*p == ':')
    {
        dest->port = 0;
        for (q = ++p; *q && (*q != '/'); q++)
            if (isdigit(*q))
                dest->port = dest->port * 10 + (*q - '0');
            else
            {
                /* invalid port */
                goto outoff;
            }
        p = q;
    }

nohost:
    /* document */
    if (!*p)
        p = "/";

    if (TRstrcasecmp(dest->scheme, "http") == 0 ||
            TRstrcasecmp(dest->scheme, "https") == 0)
    {
        const char hexnums[] = "0123456789abcdef";
        d = dest->uri;
        while (*p != '\0')
        {
            if (!isspace(*p))
            {
                *d++ = *p++;
            }
            else
            {
                *d++ = '%';
                *d++ = hexnums[((unsigned int)*p) >> 4];
                *d++ = hexnums[((unsigned int)*p) & 0xf];
                p++;
            }
        }
        *d = '\0';
    }
    else
    {
        //strncpy(d, p, MAX_URI_LEN);
    }

    cwmp_log_debug(
        "scheme:   [%s]\n"
        "user:     [%s]\n"
        "password: [%s]\n"
        "host:     [%s]\n"
        "port:     [%d]\n"
        "uri: [%s]\n",
        dest->scheme, dest->user, dest->password,
        dest->host, dest->port, dest->uri);


    return CWMP_OK;

outoff:
    cwmp_log_error("parse url error.\n");
    return CWMP_ERROR;
}



static int http_split_headers(char *data, unsigned long len, char **line)
{
    int lines = 0;
    unsigned long i;

    //FUNCTION_TRACE();

    line[lines] = data;
    for (i = 0; i < len && lines < MAX_HEADERS; i++)
    {
        if (data[i] == '\r')
            data[i] = '\0';
        if (data[i] == '\n')
        {
            lines++;
            data[i] = '\0';
            if (lines >= MAX_HEADERS)
                return MAX_HEADERS;
            if (i + 1 < len)
            {
                if (data[i + 1] == '\n' || data[i + 1] == '\r')
                    break;
                line[lines] = &data[i + 1];
            }
        }
    }

    i++;
    while (i < len && data[i] == '\n') i++;

    return lines;
}





static void http_parse_headers(http_parser_t * parser, char **line, int lines, pool_t * pool)
{
    int i,l;
    int whitespace, where, slen;
    char *name = NULL;
    char *value = NULL;

    //FUNCTION_TRACE();

    /* parse the name: value lines. */
    for (l = 1; l < lines; l++)
    {
        where = 0;
        whitespace = 0;
        name = line[l];
        value = NULL;
        slen = strlen(line[l]);
        for (i = 0; i < slen; i++)
        {
            if (line[l][i] == ':')
            {
                whitespace = 1;
                line[l][i] = '\0';
            }
            else
            {
                if (whitespace)
                {
                    whitespace = 0;
                    while (i < slen && line[l][i] == ' ')
                        i++;

                    if (i < slen)
                        value = &line[l][i];

                    break;
                }
            }
        }

        if (name != NULL && value != NULL)
        {            
            http_set_variable(parser, name, value, pool);
            name = NULL;
            value = NULL;
        }
    }
}

int http_read_line(http_socket_t * sock, char * buffer, int max)
{
    char c;

    int i=0;
    while (i < max)
    {

        if ( http_socket_read(sock, &c, 1) <= 0 )
        {
            cwmp_log_error("recv, CANNOT READ 1 char");
            return CWMP_ERROR;
        };

        buffer[i++]=c;

        if (c=='\r')  // GOT CR
        {
            if ( http_socket_read(sock, &c, 1) < 0 )
            {
                return CWMP_ERROR;
            };

            buffer[i++]=c;
            break ;
        }
    }
    if (i >= max)
        return CWMP_ERROR;

    buffer[i] = 0;
    return i;
}

int http_read_header(http_socket_t * sock, cwmp_chunk_t * header, pool_t * pool)
{
    char buffer[1024];
    int rc, bytes;

    FUNCTION_TRACE();
    bytes = 0;
    for (;;)
    {
        rc = http_read_line(sock, buffer, 1023);
        if (rc <= 0)
            return rc;
        buffer[rc] = 0;       
	//cwmp_log_debug("%s", buffer);
        cwmp_chunk_write_string(header, buffer, rc, pool);
        bytes += rc;
        if (buffer[0] == '\r' && buffer[1] == '\n')
        {
            break;
        }
    }

    return bytes;

}





int http_read_body(http_socket_t * sock, int max)//, cwmp_chunk_t * body, pool_t * pool)
{
    int bytes = 0;
    int len;
    char buffer[512];

    while (bytes < max)
    {

        if ( (len = http_socket_read(sock, buffer, 512)) < 0 )
        {
            cwmp_log_error("recv, CANNOT READ 512 chars");
            return CWMP_ERROR;
        }
        if (len <= 0)
        {
            //*body = 0;
            if (len == 0)
            {
                return bytes;
            }
            return -1;
        }

        //memcpy(b, buffer, len);

        if(sock->write_callback)
        {
            (*sock->write_callback)(buffer, 1, len, sock->write_calldata);
        }

        bytes += len;
    }



    return bytes;

}

int http_read_request(http_socket_t * sock, http_request_t * request, pool_t * pool)
{
    int rc;
    cwmp_chunk_t * header;
    char *line[MAX_HEADERS]; /* limited to 64 lines, should be more than enough */

    int lines, len;
    size_t	bytes;
    char *req_type = NULL;
    char *uri = NULL;
    char *version = NULL;
    int whitespace, wheres, slen;
    int i;
    http_parser_t * parser;
    char data[2048];


    FUNCTION_TRACE();
    bytes = 0;
    parser = request->parser;
    cwmp_chunk_create(&header, pool);

    rc = http_read_header(sock, header, pool);
    if (rc <= 0)
    {
        return rc;
    }



    len = cwmp_chunk_copy(data, header, 2047);
    cwmp_log_debug("http read request: %s\n", data);
    bytes += len;
    lines = http_split_headers(data, len, line);


    wheres = 0;
    whitespace = 0;
    slen = strlen(line[0]);
    req_type = line[0];
    for (i = 0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            whitespace = 1;
            line[0][i] = '\0';
        }
        else
        {
            /* we're just past the whitespace boundry */
            if (whitespace)
            {
                whitespace = 0;
                wheres++;
                switch (wheres)
                {
                case 1:
                    uri = &line[0][i];
                    break;
                case 2:
                    version = &line[0][i];
                    break;
                }
            }
        }
    }

    if (TRstrcasecmp("GET", req_type) == 0)
    {
        request->method = HTTP_GET;
    }
    else if (TRstrcasecmp("POST", req_type) == 0)
    {
        request->method = HTTP_POST;
    }
    else if (TRstrcasecmp("HEAD", req_type) == 0)
    {
        request->method = HTTP_HEAD;
    }
    else
    {
        request->method = HTTP_UNKNOWN;
    }


    http_parse_headers(parser, line, lines, pool);

    return bytes;

#if 0
    cwmp_chunk_t header;
    cwmp_chunk_t body;
    int rc;

    char *tmp;
    char *line[MAX_HEADERS]; /* limited to 32 lines, should be more than enough */
    int i;
    int lines;
    char *req_type = NULL;
    char *uri = NULL;
    char *version = NULL;
    int whitespace, where, slen;


    rc = http_read_header(sock, &header);
    if (rc <= 0)
    {
        return CWMP_ERROR;
    }

    lines = http_split_headers(data, len, line);

    where = 0;
    whitespace = 0;
    slen = strlen(line[0]);
    req_type = line[0];
    for (i = 0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            whitespace = 1;
            line[0][i] = '\0';
        }
        else
        {
            /* we're just past the whitespace boundry */
            if (whitespace)
            {
                whitespace = 0;
                where++;
                switch (where)
                {
                case 1:
                    uri = &line[0][i];
                    break;
                case 2:
                    version = &line[0][i];
                    break;
                }
            }
        }
    }

    http_parse_headers(request->parser,
#endif

}

int http_parse_request(http_request_t * request, char *data, unsigned long len)
{
    char *line[MAX_HEADERS]; /* limited to 32 lines, should be more than enough */
    int i;
    int lines;
    char *req_type = NULL;
    char *uri = NULL;
    char *version = NULL;
    int whitespace, where, slen;

    if (data == NULL)
        return 0;

    /* make a local copy of the data, including 0 terminator */
    //data = (char *)malloc(len+1);
    //if (data == NULL) return 0;
    //memcpy(data, http_data, len);
    //data[len] = 0;

    lines = http_split_headers(data, len, line);

    /* parse the first line special
    ** the format is:
    ** REQ_TYPE URI VERSION
    ** eg:
    ** GET /index.html HTTP/1.0
    */
    where = 0;
    whitespace = 0;
    slen = strlen(line[0]);
    req_type = line[0];
    for (i = 0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            whitespace = 1;
            line[0][i] = '\0';
        }
        else
        {
            /* we're just past the whitespace boundry */
            if (whitespace)
            {
                whitespace = 0;
                where++;
                switch (where)
                {
                case 1:
                    uri = &line[0][i];
                    break;
                case 2:
                    version = &line[0][i];
                    break;
                }
            }
        }
    }

#if 0
    if (strcasecmp("GET", req_type) == 0)
    {
        parser->req_type = httpp_req_get;
    }
    else if (strcasecmp("POST", req_type) == 0)
    {
        parser->req_type = httpp_req_post;
    }
    else if (strcasecmp("HEAD", req_type) == 0)
    {
        parser->req_type = httpp_req_head;
    }
    else if (strcasecmp("SOURCE", req_type) == 0)
    {
        parser->req_type = httpp_req_source;
    }
    else if (strcasecmp("PLAY", req_type) == 0)
    {
        parser->req_type = httpp_req_play;
    }
    else if (strcasecmp("STATS", req_type) == 0)
    {
        parser->req_type = httpp_req_stats;
    }
    else
    {
        parser->req_type = httpp_req_unknown;
    }

    if (uri != NULL && strlen(uri) > 0)
    {
        char *query;
        if ((query = strchr(uri, '?')) != NULL)
        {
            http_set_variable(parser, HTTPP_VAR_RAWURI, uri);
            *query = 0;
            query++;
            parse_query(parser, query);
        }

        parser->uri = strdup(uri);
    }
    else
    {
        free(data);
        return 0;
    }

    if ((version != NULL) && ((tmp = strchr(version, '/')) != NULL))
    {
        tmp[0] = '\0';
        if ((strlen(version) > 0) && (strlen(&tmp[1]) > 0))
        {
            http_set_variable(parser, HTTPP_VAR_PROTOCOL, version);
            http_set_variable(parser, HTTPP_VAR_VERSION, &tmp[1]);
        }
        else
        {
            free(data);
            return 0;
        }
    }
    else
    {
        free(data);
        return 0;
    }

    if (parser->req_type != httpp_req_none && parser->req_type != httpp_req_unknown)
    {
        switch (parser->req_type)
        {
        case httpp_req_get:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "GET");
            break;
        case httpp_req_post:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "POST");
            break;
        case httpp_req_head:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "HEAD");
            break;
        case httpp_req_source:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "SOURCE");
            break;
        case httpp_req_play:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "PLAY");
            break;
        case httpp_req_stats:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "STATS");
            break;
        default:
            break;
        }
    }
    else
    {
        free(data);
        return 0;
    }

    if (parser->uri != NULL)
    {
        http_set_variable(parser, HTTPP_VAR_URI, parser->uri);
    }
    else
    {
        free(data);
        return 0;
    }

    parse_headers(parser, line, lines);

    free(data);
#endif

    return 1;
}

int http_read_response(http_socket_t * sock, http_response_t * response, pool_t * pool)
{
    char *line[MAX_HEADERS];
    int lines, slen,i, whitespace=0, where=0,code;
    char *version=NULL, *resp_code=NULL, *message=NULL;

    cwmp_chunk_t *header;
    //cwmp_chunk_t body;
    int rc;
    size_t len;

    char * data;
    char * ctxlen;
    size_t cont_len;

    cwmp_chunk_create(&header, pool);
    rc = http_read_header(sock, header, pool);
    if (rc <= 0)
    {
        return CWMP_ERROR;
    }

    len = cwmp_chunk_length(header);

    data = pool_pcalloc(pool, len + 1);
    cwmp_chunk_copy(data,header,  len);

    data[len] = 0;

    cwmp_log_debug("http read header length: %d, [\n%s\n]", len, data);

    lines = http_split_headers(data, len, line);

    /* In this case, the first line contains:
     * VERSION RESPONSE_CODE MESSAGE, such as HTTP/1.0 200 OK
     */
    slen = strlen(line[0]);
    version = line[0];
    for (i=0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            line[0][i] = 0;
            whitespace = 1;
        }
        else if (whitespace)
        {
            whitespace = 0;
            where++;
            if (where == 1)
                resp_code = &line[0][i];
            else
            {
                message = &line[0][i];
                break;
            }
        }
    }


    if (version == NULL || resp_code == NULL || message == NULL)
    {
        return CWMP_ERROR;
    }



    http_set_variable(response->parser, HTTPP_VAR_ERROR_CODE, resp_code, pool);
    code = TRatoi(resp_code);
    response->status = code;
    if (code < 200 || code >= 300)
    {
        http_set_variable(response->parser, HTTPP_VAR_ERROR_MESSAGE, message, pool);
    }

    //http_set_variable(response->parser, HTTPP_VAR_URI, uri);
    http_set_variable(response->parser, HTTPP_VAR_REQ_TYPE, "NONE", pool);

    http_parse_headers(response->parser, line, lines, pool);

    ctxlen = http_get_variable(response->parser, "Content-Length");
    cont_len = 0;
    if (ctxlen)
    {
        cont_len = TRatoi(ctxlen);
    }
    rc = http_read_body(sock, cont_len);//, &body, pool);
    if (rc < 0 || code != 200)
    {
        cwmp_log_info("Http read response code is (%d)\n", code);        
    }
   
     return code;
    
}

//#define http_set_variable(header, name, value)  http_set_var( &header, name, value)

char * http_method(int method)
{
    switch (method)
    {
    case HTTP_POST:
        return "POST";
    case HTTP_PUT:
        return "PUT";
    default:

        return "GET";

    };

    return "GET";
}




/* calculate H(A1) as per spec */

void http_digest_calc_ha1(
        const char *pszAlg,
        const char *pszUserName,
        const char *pszRealm,
        const char *pszPassword,
        const char *pszNonce,
        const char *pszCNonce,
        char *SessionKey)
{
    MD5_CTX Md5Ctx;
    char HA1[HASHLEN];

    MD5Init(&Md5Ctx);
    MD5Update(&Md5Ctx, (unsigned char *)pszUserName, strlen(pszUserName));
    MD5Update(&Md5Ctx, (unsigned char *)":", 1);
    MD5Update(&Md5Ctx, (unsigned char *)pszRealm, strlen(pszRealm));
    MD5Update(&Md5Ctx, (unsigned char *)":", 1);
    MD5Update(&Md5Ctx, (unsigned char *)pszPassword, strlen(pszPassword));
    MD5Final((unsigned char *)HA1, &Md5Ctx);
    if (TRstrcasecmp(pszAlg, "md5-sess") == 0)
    {
        MD5Init(&Md5Ctx);
        MD5Update(&Md5Ctx, (unsigned char *)HA1, HASHLEN);
        MD5Update(&Md5Ctx, (unsigned char *)":", 1);
        MD5Update(&Md5Ctx, (unsigned char *)pszNonce, strlen(pszNonce));
        MD5Update(&Md5Ctx, (unsigned char *)":", 1);
        MD5Update(&Md5Ctx, (unsigned char *)pszCNonce, strlen(pszCNonce));
        MD5Final((unsigned char *)HA1, &Md5Ctx);
    };
    convert_to_hex(HA1, SessionKey);
};


int http_check_digest_auth(const char * auth_realm, const char * auth, char * cpeuser, char * cpepwd)
{
    char data[512] = {0};
    char * s ;
    char buffer[128];
	char		realm[256] = {0};
    char		user[256] = {0}; /*CDRouter will test largest size ConnectionRequest Username*/
    char		uri[256] = {0};//uri[32768]
    char		cnonce[33] = {0};
    char		nonce[33] = {0};

    char		qop[16] = {0};
    char		nc[16] = {0};

    char		response[128] = {0};
//    char		method[16] = {0};
//    char		resp[33] = {0};


    char ha1[HASHHEXLEN+1];
    char ha2[HASHHEXLEN+1];
    char validResponse[HASHHEXLEN+1];

    char * end;

    if (!auth)
        return -1;

    for (s =  (char*)auth; isspace(*s); s++);
    strncpy(data, s, 511);
    s = data;
    if (TRstrncasecmp(s, "digest", 6) != 0)
        return -1;
    for (s += 6;  isspace(*s); s++);

    end = s + strlen(s);
    memset(buffer, 128, 0);
    while (s<end)
    {
        if (!strncmp(s, "username=", 9))
            http_parse_key_value(&s, user, sizeof(user), 9);
        else if (! strncmp(s, "nonce=", 6))
            http_parse_key_value(&s, nonce, sizeof(nonce), 6);
        else if (! strncmp(s, "response=", 9))
            http_parse_key_value(&s, response, sizeof(response), 9);
        else if (! strncmp(s, "uri=", 4))
            http_parse_key_value(&s, uri, sizeof(uri), 4);
        else if (! strncmp(s, "qop=", 4))
            http_parse_key_value(&s, qop, sizeof(qop), 4);
        else if (! strncmp(s, "cnonce=", 7))
            http_parse_key_value(&s, cnonce, sizeof(cnonce), 7);
        else if (! strncmp(s, "nc=", 3))
            http_parse_key_value(&s, nc, sizeof(nc), 3);
		else if (! strncmp(s, "realm=", 6))
            http_parse_key_value(&s, realm, sizeof(nc), 6);
		
		
        s ++;
    }
    cwmp_log_info("user[%s], nonce[%s], response[%s], uri[%s], qop[%s], cnonce[%s], nc[%s]\n",
                  user, nonce, response, uri, qop, cnonce, nc);

    if (TRstrcmp(cpeuser, user) != 0)
        return -1;

    http_digest_calc_ha1("MD5", cpeuser, realm, cpepwd, nonce, cnonce, ha1);

    MD5(ha2, "GET", ":", uri, NULL);
    MD5(validResponse, ha1, ":", nonce, ":", nc, ":", cnonce, ":", qop, ":", ha2, NULL);

    
    if (TRstrcasecmp(validResponse, response) == 0)
	{
		cwmp_log_info("auth ok. [%s] [%s]\n", validResponse, response);
        return 0;
	}
    else
        return -1;
}

int http_calc_digest_response(const char * user, const char * pwd,
                const char * realm,
                const char * nonce,
                const char * uri,
                const char * cnonce,
                const char * nc,
                const char * qop,
                char * response)
{
    char ha1[HASHHEXLEN+1];
    char ha2[HASHHEXLEN+1];
    char valid_response[HASHHEXLEN+1];
    http_digest_calc_ha1("MD5", user, realm, pwd, nonce, cnonce, ha1);
    MD5(ha2, "POST", ":", uri, NULL);
    //MD5(valid_response, ha1, ":", nonce, ":", nc, ":", cnonce, ":", qop, ":", ha2, NULL);
    MD5(valid_response, ha1, ":", nonce, ":", ha2, NULL);


    TRstrncpy(response, valid_response, HASHHEXLEN);

    return CWMP_OK;
}

int http_parse_digest_auth(const char * auth, http_digest_auth_t * digest_auth)
{
    char data[512] = {0};
    char * s ;
    char buffer[128];
    char * end;

    char		user[256] = {0}; /*CDRouter will test largest size ConnectionRequest Username*/
    char		uri[256] = {0};//uri[32768]
    char		nonce[33] = {0};
    char		cnonce[33] = {0};
    char        realm[128] = {0};

    char		qop[16] = {0};
    char		nc[16] = {0};

    char		response[128] = {0};


    FUNCTION_TRACE();

    if (!auth)
        return CWMP_ERROR;

    for (s =  (char*)auth; isspace(*s); s++);
    strncpy(data, s, 511);
    s = data;
    if (TRstrncasecmp(s, "digest", 6) != 0)
        return -1;
    for (s += 6;  isspace(*s); s++);

    end = s + strlen(s);
    memset(buffer, 128, 0);
    while (s<end)
    {
        if (!strncmp(s, "realm=", 6))
            http_parse_key_value(&s, realm, sizeof(realm), 6);
        else if (! strncmp(s, "nonce=", 6))
            http_parse_key_value(&s, nonce, sizeof(nonce), 6);
        else if (! strncmp(s, "response=", 9))
            http_parse_key_value(&s, response, sizeof(response), 9);
        else if (! strncmp(s, "uri=", 4))
            http_parse_key_value(&s, uri, sizeof(uri), 4);
        else if (! strncmp(s, "qop=", 4))
            http_parse_key_value(&s, qop, sizeof(qop), 4);
        else if (! strncmp(s, "cnonce=", 7))
            http_parse_key_value(&s, cnonce, sizeof(cnonce), 7);
        else if (! strncmp(s, "nc=", 3))
            http_parse_key_value(&s, nc, sizeof(nc), 3);
        else if (! strncmp(s, "domain=", 7))
            http_parse_key_value(&s, uri, sizeof(uri), 7);
        s ++;
    }
    cwmp_log_info("user[%s], realm[%s], nonce[%s], response[%s], uri[%s], qop[%s], cnonce[%s], nc[%s]\n",
                  user, realm, nonce, response, uri, qop, cnonce, nc);

    TRstrncpy(digest_auth->realm, realm, MIN_DEFAULT_LEN);
    TRstrncpy(digest_auth->nonce, nonce, MIN_DEFAULT_LEN);
    TRstrncpy(digest_auth->uri, uri, MIN_DEFAULT_LEN*4);
    TRstrncpy(digest_auth->cnonce, cnonce, MIN_DEFAULT_LEN);
    TRstrncpy(digest_auth->qop, "auth", MIN_DEFAULT_LEN);
    TRstrncpy(digest_auth->nc, nc, MIN_DEFAULT_LEN);

    return CWMP_OK;

}






int http_write_request(http_socket_t * sock , http_request_t * request, cwmp_chunk_t * chunk, pool_t * pool)
{
    char buffer[HTTP_DEFAULT_LEN+1];
    char * data;

    size_t len1, len2, totallen;


    const char * header_fmt =
        "%s %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "User-Agent: %s\r\n"
        "Accept: */*\r\n"
        "Content-Type: text/xml; charset=utf-8\r\n"
        "Content-Length: %d\r\n"
        ;
    const char * auth_fmt = "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n";
    //qop=%s, nc=%s, cnonce=\"%s\"

    http_dest_t * dest = request->dest;




    len2 = cwmp_chunk_length(chunk);

    len1 = TRsnprintf(buffer, HTTP_DEFAULT_LEN, header_fmt,
                    http_method(request->method),
                    dest->uri,
                    dest->host,
                    dest->port,
                    "CPE Netcwmp Agent",
                    len2);
    if(len2 > 0)
    {
         if((dest->auth.active == CWMP_FALSE) && (dest->auth_type == HTTP_DIGEST_AUTH))
        {
            http_calc_digest_response(dest->user, dest->password,
                    dest->auth.realm, dest->auth.nonce, dest->auth.uri, dest->auth.cnonce, dest->auth.nc, dest->auth.qop, dest->auth.response);

            len1 += TRsnprintf(buffer + len1, HTTP_DEFAULT_LEN - len1, auth_fmt,
                            dest->user,
                            dest->auth.realm, dest->auth.nonce,
                            dest->auth.uri, dest->auth.response
                            //dest->auth.qop, dest->auth.nc, dest->auth.cnonce
                            );
        }
    }

    if(dest->cookie[0] != '\0')
    {

        len1 += TRsnprintf(buffer + len1, HTTP_DEFAULT_LEN - len1, "Cookie: %s\r\n",
                    dest->cookie);
    }

    len1 += TRsnprintf(buffer + len1, HTTP_DEFAULT_LEN - len1, "\r\n");



    len1 = TRstrlen(buffer);

    if(len2 > 0)
    {
        data = (char *)pool_palloc(pool, len1 + len2 + 1);
        TRstrncpy(data, buffer, len1);
        cwmp_chunk_copy(data+len1,chunk,  len2);
    }
    else
    {
        data = buffer;
    }

    return http_socket_write(sock, data, (int)len1 + len2);
}

int http_get(http_socket_t * sock, http_request_t * request, cwmp_chunk_t * data, pool_t * pool)
{
    request->method = HTTP_GET;


    return http_write_request(sock, request, data, pool);

}

int http_post(http_socket_t * sock, http_request_t * request, cwmp_chunk_t * data, pool_t * pool)
{
    request->method = HTTP_POST;


    return http_write_request(sock, request, data, pool);

}

size_t http_send_file_callback(char *data, size_t size, size_t nmemb, void * calldata)
{
	FILE * tf = (FILE*) calldata;

	return  fread(data, size, nmemb, tf);
}


size_t http_receive_file_callback(char *data, size_t size, size_t nmemb, void * calldata)
{
	FILE * tf = (FILE*) calldata;

	return  fwrite(data, size, nmemb, tf);
}




int http_send_file_request(http_socket_t * sock , http_request_t * request, const char  * fromfile, pool_t * pool)
{
    char buffer[HTTP_DEFAULT_LEN+1];
    char * data;

    size_t len1, len2, totallen;


    const char * header_fmt =
        "%s %s HTTP/1.1\r\n"
        "Authorization: Basic ZnRwdXNlcjpmdHB1c2Vy\r\n"
        "Host: %s:%d\r\n"
        //"User-Agent: %s\r\n"
        "Accept: */*\r\n"
       // "Content-Type: multipart/form-data\r\n"
        //"Connection: Keep-Alive\r\n"
        "Content-Length: %d\r\n"
        "Expect: 100-continue\r\n"
        ;
    const char * auth_fmt = "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n";
    //qop=%s, nc=%s, cnonce=\"%s\"

    http_dest_t * dest = request->dest;

    struct stat buf;
	if(stat(fromfile, &buf)<0)
	{
		len2 = 0;
	}
	else
	{
		 len2 = buf.st_size;
	}

    FILE *tf = fopen(fromfile, "rb");
    if(!tf)
    {
	return CWMP_ERROR;
    }

	


    len1 = TRsnprintf(buffer, HTTP_DEFAULT_LEN, header_fmt,
                    http_method(request->method),
                    dest->uri,
                    dest->host,
                    dest->port,
                    "CPE Netcwmp Agent",
                    len2);

    len1 += TRsnprintf(buffer + len1, HTTP_DEFAULT_LEN - len1, "\r\n");

    cwmp_log_debug("SEND: %d[\n%s\n]", len1, buffer);

    http_socket_write(sock, buffer, (int)len1);

   http_response_t * response;
   http_response_create(&response, pool);

    int rc = http_read_response(sock, response, pool);
    if(rc != HTTP_100)
    {
	if(tf != NULL)
	{
		fclose(tf);
	}
	return CWMP_ERROR;
    }


    totallen = len1;

    while(1)
    {
	len2 = fread(buffer, HTTP_DEFAULT_LEN, 1, tf);
	if(len2 <= 0)
	{
		break;
	}
	len2 = http_socket_write(sock, buffer, (int)len2);
	if(len2 <= 0)
	{
		break;
	}
	totallen += len2;
    }

    if(tf != NULL)
    {
	fclose(tf);
    }

    return totallen;
}


int http_send_file(const char * fromfile, const char *tourl )
{
	pool_t * pool;
	http_dest_t *  dest;
	http_socket_t * sock;
	http_request_t * request;

	http_response_t * response;

	
	pool = pool_create(POOL_DEFAULT_SIZE);
	http_dest_create(&dest, tourl, pool);
   
        int rc = http_socket_create(&sock, AF_INET, SOCK_STREAM, 0, pool);
        if (rc != CWMP_OK)
        {
            cwmp_log_error("http send file: create socket error.");
            goto out;
        }

        rc = http_socket_connect(sock, AF_INET, dest->host, dest->port);
        if(rc != CWMP_OK)
        {
            cwmp_log_error("connect to host faild. Host is %s:%d.", dest->host, dest->port);
            goto out;
        }

        http_socket_set_recvtimeout(sock, 30);

	http_request_create(&request, pool);
	request->dest = dest;
        request->method = HTTP_PUT;
		
	rc = http_send_file_request(sock, request, fromfile, pool);
        if(rc <= 0)
        {
            cwmp_log_error("http get host faild. Host is %s:%d.", dest->host, dest->port);
            goto out;
        }


        http_response_create(&response, pool);

	rc = http_read_response(sock, response, pool);


	
out:

	pool_destroy(pool);

	if(rc != HTTP_200)
		return CWMP_ERROR;
	else
		return CWMP_OK;


		

	
	


}

int http_receive_file(const char *fromurl, const char * tofile)
{
	pool_t * pool;
	http_dest_t *  dest;
	http_socket_t * sock;
	http_request_t * request;

	http_response_t * response;

	FILE * tf = NULL;
	
	pool = pool_create(POOL_DEFAULT_SIZE);
	http_dest_create(&dest, fromurl, pool);
   
        int rc = http_socket_create(&sock, AF_INET, SOCK_STREAM, 0, pool);
        if (rc != CWMP_OK)
        {
            cwmp_log_error("http receive file: create socket error.");
            goto out;
        }

        rc = http_socket_connect(sock, AF_INET, dest->host, dest->port);
        if(rc != CWMP_OK)
        {
            cwmp_log_error("connect to host faild. Host is %s:%d.", dest->host, dest->port);
            goto out;
        }

	tf = fopen(tofile, "wb+");
	if(!tf)
	{
		cwmp_log_error("create file faild. %s\n", tofile);
		goto out;
	}

	http_socket_set_writefunction(sock, http_receive_file_callback, tf);
        http_socket_set_recvtimeout(sock, 30);

	http_request_create(&request, pool);
	request->dest = dest;
	rc = http_get(sock, request, NULL, pool);
        if(rc <= 0)
        {
            cwmp_log_error("http get host faild. Host is %s:%d.", dest->host, dest->port);
            goto out;
        }


        http_response_create(&response, pool);

	rc = http_read_response(sock, response, pool);


	
out:
	if(tf)
	{
		fclose(tf);
	}
	pool_destroy(pool);

	return rc;
		

	
}



