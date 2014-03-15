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


#ifndef __CWMPINI_H__
#define __CWMPINI_H__

#include <stdio.h>

#define ini_openread(filename,file)   ((*(file) = fopen((filename),"rt")) != NULL)
#define ini_openwrite(filename,file)  ((*(file) = fopen((filename),"wt")) != NULL)
#define ini_close(file)               fclose(*(file))
#define ini_read(buffer,size,file)    fgets((buffer),(size),*(file))
#define ini_write(buffer,file)        fputs((buffer),*(file))
#define ini_rename(source,dest)       rename((source),(dest))
#define ini_remove(filename)          remove(filename)
#define ini_rewind(file)              rewind(*(file))


#if (defined _UNICODE || defined __UNICODE__ || defined UNICODE) && !defined INI_ANSIONLY
#include <tchar.h>
#elif !defined __T
typedef char TCHAR;
#endif


typedef TCHAR tchar_t;




#if defined __cplusplus
extern "C"
{
#endif

    long ini_getl(const tchar_t *section, const tchar_t *key, long default_value, const tchar_t *filename);
    int ini_gets(const tchar_t *section, const tchar_t *key, const tchar_t *default_value,
             tchar_t *buffer, int buffer_size, const tchar_t *filename);
    int ini_putl(const tchar_t *section, const tchar_t *key, long value, const tchar_t *filename);
    int ini_puts(const tchar_t *section, const tchar_t *key, const tchar_t *value, const tchar_t *filename);
    int  ini_get_section(int idx, tchar_t * buffer, int buffer_size, const tchar_t *filename);
    int  ini_get_key(const tchar_t *section, int idx, tchar_t *buffer, int buffer_size, const tchar_t *filename);

#if defined __cplusplus
}
#endif


#endif /* __CWMPINI_H__ */


