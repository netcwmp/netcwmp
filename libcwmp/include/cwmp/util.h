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

#ifndef __CWMPUTIL_H__
#define __CWMPUTIL_H__

#include <cwmp/types.h>
#include <cwmp/md5.h>



#define Public		/* Accessible outside this module     */
#define Private static		/* Accessible only within this module */

#ifdef WIN32
#define INLINE __inline
#else
#define INLINE inline
#endif

#define STRING_MAX_LENGTH		1024*64


#ifdef WIN32
#define TRsnprintf _snprintf
#else
#define TRsnprintf snprintf
#endif

void MD5(char *buf, ...);


static INLINE unsigned int TRstrcmp(const char * str, const char * dst)
{
    if (!str || !dst)
    {
        return -1;
    }
    return strcmp(str, dst);
}



static INLINE unsigned int TRstrncmp(const char * str, const char * dst, unsigned int length)
{
    if (!str || !dst || length <= 0)
    {
        return -1;
    }
    return strncmp(str, dst, length);
}


static INLINE int TRstrncasecmp(const char * s1, const char * s2, size_t n)
{
    if (!s1 || !s2)
    {
        return -1;
    }
#ifdef WIN32

    if (n == 0)
        return 0;

    while (n-- != 0 && tolower(*s1) == tolower(*s2))
    {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
#else
    return strncasecmp(s1, s2, n);
#endif
}


static INLINE  int TRstrcasecmp(const char * str, const char * dst)
{
    if (!str || !dst)
    {
        return -1;
    }
#ifdef WIN32
    return lstrcmpiA(str, dst);
#else
    return strcasecmp(str, dst);
#endif
}

/*

static int TRstrnncmp(const char *s1,const char *s2)
{
    while (*s1)
    {
        if ((*s1)!=(*s2))
        {
            return (int)((*s1)-(*s2));
        }
        else
        {
            s1++;
            s2++;
        }
    }

    if (*s1) return 1;
    if (*s2) return -1;

    return 0;
}

*/

static INLINE size_t TRstrlen(const char * str)
{
    if (!str)
    {
        return 0;
    }
    else
    {
        return strlen(str);
    }
    /*
            const char * p = str;
            unsigned int i = 0;
            while (p && ((*p) != '\0') && i < STRING_MAX_LENGTH)
            {
                ++i;
                p++;
            }
            return i;
    */
}



static INLINE char * TRstrdup(const char * str)
{
    if (!str)
        return NULL;
#ifdef WIN32
    return _strdup(str);
#else
    return strdup(str);
#endif
}


static INLINE char * TRitoa(int num)
{
#ifdef WIN32
    static char buffer[128] = {0};
    return itoa(num, buffer, 10);
#else
    static char buffer[128] = {0};
    snprintf(buffer, 128, "%d", num);
    return buffer;
#endif
}

static INLINE int TRatoi(const char * string)
{
    if (!string)
        return 0;
    return atoi(string);
}

static INLINE int TRstrncpy(char * dst, const char * src, size_t maxlen)
{
    if (!dst || !src)
        return CWMP_ERROR;

    strncpy(dst, src, maxlen);
    return CWMP_OK;
}



/*
    static INLINE int TRinetntop(int family, const struct in_addr *src,  char *dst, size_t dstLength)
    {
#ifdef WIN32
        char * d = inet_ntoa(*src);
        TRstrncpy(dst, d, dstLength);
#else
        inet_ntop(family, src, dst, dstLength);
#endif
        return CWMP_OK;
    }


    static INLINE cwmp_uint32_t TRntoh32(cwmp_uint32_t val)
    {
        return ntohl(val);
    }

    static INLINE cwmp_uint32_t TRhton32(cwmp_uint32_t val)
    {
        return htonl(val);
    }

    static INLINE cwmp_uint16_t TRntoh16(cwmp_uint16_t val)
    {
        return ntohs(val);
    }

    static INLINE cwmp_uint16_t TRhton16(cwmp_uint16_t val)
    {
        return htons(val);
    }

*/
static INLINE char * TRrighttrim(char * s)
{
    register   size_t   l;

    for (l=strlen(s); l>0 && isspace(s[l-1]); l--)
        s[l-1]='\0';
    return (s);
}

static INLINE char * TRlefttrim(char   *s)
{
    register   char   *p;

    for (p=s; isspace(*p); p++);
    if (p!=s)
        strcpy(s, p);
    return (s);
}

void convert_to_hex(const char *Bin, char *Hex);


static INLINE int is_digit(const char *str)
{
    int     i;
    int     n;
    
    if(!str)
    {
        return -1;
    }

    n = TRstrlen(str);
    for(i=0; i<n; i++)
    {
        if(!isdigit(str[i]))
        {
            return -1;
        }
    }

    return 0;
}





#endif

