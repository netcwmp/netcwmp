
#if (defined _UNICODE || defined __UNICODE__ || defined UNICODE) && !defined MININI_ANSI
# if !defined UNICODE   /* for Windows */
#   define UNICODE
# endif
# if !defined _UNICODE  /* for C library */
#   define _UNICODE
# endif
#endif

#include "ini.h"
#if defined NDEBUG
#define assert(e)
#else
#include <assert.h>
#endif

#if !defined __T
#include <string.h>
#include <stdlib.h>
/* definition of tchar_t already in minIni.h */
#define __T(s)    s
#define _tcscat   strcat
#define _tcschr   strchr
#define _tcscmp   strcmp
#define _tcscpy   strcpy
#define _tcsicmp  stricmp
#define _tcslen   strlen
#define _tcsncpy  strncpy
#define _tcsnicmp strncmp
#define _tcsrchr  strrchr
#define _tcstol   strtol
#define _tfgets   fgets
#define _tfputs   fputs
#define _tfopen   fopen
#define _tremove  remove
#define _trename  rename

#endif

#if defined __linux || defined __linux__
#define __LINUX__
#endif
#if defined FREEBSD && !defined __FreeBSD__
#define __FreeBSD__
#endif
#if !defined strnicmp
#if defined __LINUX__ || defined __FreeBSD__ || defined __OpenBSD__
#define strnicmp  strncasecmp
#endif
#endif

#if !defined INI_LINETERM
#define INI_LINETERM    __T("\n")
#endif
#if !defined INI_FILETYPE
#define INI_FILETYPE    FILE*
#endif

#if !defined sizearray
#define sizearray(a)    (sizeof(a) / sizeof((a)[0]))
#endif


#define INI_BUFFERSIZE  128

static tchar_t *ini_skip_leading(const tchar_t *str)
{
    assert(str != NULL);
    while (*str != '\0' && *str <= ' ')
        str++;
    return (tchar_t *)str;
}

static tchar_t *ini_skip_trailing(const tchar_t *str, const tchar_t *base)
{
    assert(str != NULL);
    assert(base != NULL);
    while (str > base && *(str-1) <= ' ')
        str--;
    return (tchar_t *)str;
}

static tchar_t *ini_strip_trailing(tchar_t *str)
{
    tchar_t *ptr = ini_skip_trailing(_tcschr(str, '\0'), str);
    assert(ptr != NULL);
    *ptr='\0';
    return str;
}

static tchar_t *ini_save_strncpy(tchar_t *dest, const tchar_t *source, size_t maxlen)
{
    assert(maxlen>0);
    _tcsncpy(dest,source,maxlen);
    dest[maxlen-1]='\0';
    return dest;
}

static int ini_get_keystring(INI_FILETYPE *fp, const tchar_t *section, const tchar_t *key,
                        int section_index, int idxKey, tchar_t *buffer, int buffer_size)
{
    tchar_t *sp, *ep;
    int len, idx;
    tchar_t local_buffer[INI_BUFFERSIZE];

    assert(fp != NULL);
    /* Move through file 1 line at a time until a section is matched or EOF. If
     * parameter section is NULL, only look at keys above the first section. If
     * section_index is postive, copy the relevant section name.
     */
    len = (section != NULL) ? _tcslen(section) : 0;
    if (len > 0 || section_index >= 0)
    {
        idx = -1;
        do
        {
            if (!ini_read(local_buffer, INI_BUFFERSIZE, fp))
                return 0;
            sp = ini_skip_leading(local_buffer);
            ep = _tcschr(sp, ']');
        }
        while (*sp != '[' || ep == NULL || (((int)(ep-sp-1) != len || _tcsnicmp(sp+1,section,len) != 0) && ++idx != section_index));
        if (section_index >= 0)
        {
            if (idx == section_index)
            {
                assert(ep != NULL);
                assert(*ep == ']');
                *ep = '\0';
                ini_save_strncpy(buffer, sp + 1, buffer_size);
                return 1;
            } /* if */
            return 0; /* no more section found */
        } /* if */
    } /* if */

    /* Now that the section has been found, find the entry.
     * Stop searching upon leaving the section's area.
     */
    assert(key != NULL || idxKey >= 0);
    len = (key != NULL) ? (int)_tcslen(key) : 0;
    idx = -1;
    do
    {
        if (!ini_read(local_buffer,INI_BUFFERSIZE,fp) || *(sp = ini_skip_leading(local_buffer)) == '[')
            return 0;
        sp = ini_skip_leading(local_buffer);
        ep = _tcschr(sp, '='); /* Parse out the equal sign */
        if (ep == NULL)
            ep = _tcschr(sp, ':');
    }
    while (*sp == ';' || *sp == '#' || ep == NULL || (((int)(ini_skip_trailing(ep,sp)-sp) != len || _tcsnicmp(sp,key,len) != 0) && ++idx != idxKey));
    if (idxKey >= 0)
    {
        if (idx == idxKey)
        {
            assert(ep != NULL);
            assert(*ep == '=' || *ep == ':');
            *ep = '\0';
            ini_strip_trailing(sp);
            ini_save_strncpy(buffer, sp, buffer_size);
            return 1;
        } /* if */
        return 0;   /* no more key found (in this section) */
    } /* if */

    /* Copy up to buffer_size chars to buffer */
    assert(ep != NULL);
    assert(*ep == '=' || *ep == ':');
    sp = ini_skip_leading(ep + 1);
    ini_strip_trailing(sp);
    /* Remove double quotes surrounding a value */
    if (*sp == '"' && (ep = _tcschr(sp, '\0')) != NULL && *(ep - 1) == '"')
    {
        sp++;
        *--ep = '\0';
    } /* if */
    ini_save_strncpy(buffer, sp, buffer_size);
    return 1;
}

/** ini_gets()
 * \param section     the name of the section to search for
 * \param key         the name of the entry to find the value of
 * \param default_value    default string in the event of a failed read
 * \param buffer      a pointer to the buffer to copy into
 * \param buffer_size  the maximum number of characters to copy
 * \param filename    the name and full path of the .ini file to read from
 *
 * \return            the number of characters copied into the supplied buffer
 */
int ini_gets(const tchar_t *section, const tchar_t *key, const tchar_t *default_value,
             tchar_t *buffer, int buffer_size, const tchar_t *filename)
{
    INI_FILETYPE fp;
    int ok = 0;

    if (buffer == NULL || buffer_size <= 0 || key == NULL)
        return 0;
    if (ini_openread(filename, &fp))
    {
        ok = ini_get_keystring(&fp, section, key, -1, -1, buffer, buffer_size);
        ini_close(&fp);
    } /* if */
    if (!ok)
    {
        if(default_value == NULL)
        {
            buffer[0] = 0;
        }
        else
        {
            ini_save_strncpy(buffer, default_value, buffer_size);
        }
    }
    return _tcslen(buffer);
}

/** ini_getl()
 * \param section     the name of the section to search for
 * \param key         the name of the entry to find the value of
 * \param default_value    the default value in the event of a failed read
 * \param filename    the name of the .ini file to read from
 *
 * \return            the value located at key
 */
long ini_getl(const tchar_t *section, const tchar_t *key, long default_value, const tchar_t *filename)
{
    tchar_t buff[64];
    int len = ini_gets(section, key, __T(""), buff, sizearray(buff), filename);
    return (len == 0) ? default_value : _tcstol(buff,NULL,10);
}

/** ini_get_section()
 * \param idx         the zero-based sequence number of the section to return
 * \param buffer      a pointer to the buffer to copy into
 * \param buffer_size  the maximum number of characters to copy
 * \param filename    the name and full path of the .ini file to read from
 *
 * \return            the number of characters copied into the supplied buffer
 */
int  ini_get_section(int idx, tchar_t *buffer, int buffer_size, const tchar_t *filename)
{
    INI_FILETYPE fp;
    int ok = 0;

    if (buffer == NULL || buffer_size <= 0 || idx < 0)
        return 0;
    if (ini_openread(filename, &fp))
    {
        ok = ini_get_keystring(&fp, NULL, NULL, idx, -1, buffer, buffer_size);
        ini_close(&fp);
    } /* if */
    if (!ok)
        *buffer = '\0';
    return _tcslen(buffer);
}

/** ini_get_key()
 * \param section     the name of the section to browse through, or NULL to
 *                    browse through the keys outside any section
 * \param idx         the zero-based sequence number of the key to return
 * \param buffer      a pointer to the buffer to copy into
 * \param buffer_size  the maximum number of characters to copy
 * \param filename    the name and full path of the .ini file to read from
 *
 * \return            the number of characters copied into the supplied buffer
 */
int  ini_get_key(const tchar_t *section, int idx, tchar_t *buffer, int buffer_size, const tchar_t *filename)
{
    INI_FILETYPE fp;
    int ok = 0;

    if (buffer == NULL || buffer_size <= 0 || idx < 0)
        return 0;
    if (ini_openread(filename, &fp))
    {
        ok = ini_get_keystring(&fp, section, NULL, -1, idx, buffer, buffer_size);
        ini_close(&fp);
    } /* if */
    if (!ok)
        *buffer = '\0';
    return _tcslen(buffer);
}


#if ! defined INI_READONLY
static void ini_tempname(tchar_t *dest, const tchar_t *source, int maxlength)
{
    tchar_t *p;

    ini_save_strncpy(dest, source, maxlength);
    p = _tcsrchr(dest, '\0');
    assert(p != NULL);
    *(p - 1) = '~';
}

static void ini_write_section(tchar_t *local_buffer, const tchar_t *section, INI_FILETYPE *fp)
{
    tchar_t *p;

    if (section != NULL && _tcslen(section) > 0)
    {
        local_buffer[0] = '[';
        ini_save_strncpy(local_buffer + 1, section, INI_BUFFERSIZE - 4);  /* -1 for '[', -1 for ']', -2 for '\r\n' */
        p = _tcsrchr(local_buffer, '\0');
        assert(p != NULL);
        *p++ = ']';
        _tcscpy(p, INI_LINETERM); /* copy line terminator (typically "\n") */
        ini_write(local_buffer, fp);
    } /* if */
}

static void ini_write_key(tchar_t *local_buffer, const tchar_t *key, const tchar_t *value, INI_FILETYPE *fp)
{
    tchar_t *p;

    ini_save_strncpy(local_buffer, key, INI_BUFFERSIZE - 3);  /* -1 for '=', -2 for '\r\n' */
    p = _tcsrchr(local_buffer, '\0');
    assert(p != NULL);
    *p++ = '=';
    ini_save_strncpy(p, value, INI_BUFFERSIZE - (p - local_buffer) - 2); /* -2 for '\r\n' */
    p = _tcsrchr(local_buffer, '\0');
    assert(p != NULL);
    _tcscpy(p, INI_LINETERM); /* copy line terminator (typically "\n") */
    ini_write(local_buffer, fp);
}

/** ini_puts()
 * \param section     the name of the section to write the string in
 * \param key         the name of the entry to write, or NULL to erase all keys in the section
 * \param value       a pointer to the buffer the string, or NULL to erase the key
 * \param filename    the name and full path of the .ini file to write to
 *
 * \return            1 if successful, otherwise 0
 */
int ini_puts(const tchar_t *section, const tchar_t *key, const tchar_t *value, const tchar_t *filename)
{
    INI_FILETYPE rfp;
    INI_FILETYPE wfp;
    tchar_t *sp, *ep;
    tchar_t local_buffer[INI_BUFFERSIZE];
    int len, match, count;

    assert(filename!=NULL);
    if (!ini_openread(filename, &rfp))
    {
        /* If the .ini file doesn't exist, make a new file */
        if (key!=NULL && value!=NULL)
        {
            if (!ini_openwrite(filename, &wfp))
                return 0;
            ini_write_section(local_buffer, section, &wfp);
            ini_write_key(local_buffer, key, value, &wfp);
            ini_close(&wfp);
        } /* if */
        return 1;
    } /* if */

    /* If parameters key and value are valid (so this is not an "erase" request)
     * and the setting already exists and it already has the correct value, do
     * nothing. This early bail-out avoids rewriting the INI file for no reason.
     */
    if (key!=NULL && value!=NULL)
    {
        match = ini_get_keystring(&rfp, section, key, -1, -1, local_buffer, sizearray(local_buffer));
        if (match && _tcscmp(local_buffer,value)==0)
        {
            ini_close(&rfp);
            return 1;
        } /* if */
        /* key not found, or different value -> proceed (but rewind the input file first) */
        ini_rewind(&rfp);
    } /* if */

    /* Get a temporary file name to copy to. Use the existing name, but with
     * the last character set to a '~'.
     */
    ini_tempname(local_buffer, filename, INI_BUFFERSIZE);
    if (!ini_openwrite(local_buffer, &wfp))
    {
        ini_close(&rfp);
        return 0;
    } /* if */

    /* Move through the file one line at a time until a section is
     * matched or until EOF. Copy to temp file as it is read.
     */
    count = 0;
    len = (section != NULL) ? _tcslen(section) : 0;
    if (len > 0)
    {
        do
        {
            if (!ini_read(local_buffer, INI_BUFFERSIZE, &rfp))
            {
                /* Failed to find section, so add one to the end */
                if (key!=NULL && value!=NULL)
                {
                    ini_write(INI_LINETERM, &wfp);  /* force a new line (there may not have been one) behind the last line of the INI file */
                    ini_write_section(local_buffer, section, &wfp);
                    ini_write_key(local_buffer, key, value, &wfp);
                } /* if */
                /* Clean up and rename */
                ini_close(&rfp);
                ini_close(&wfp);
                ini_remove(filename);
                ini_tempname(local_buffer, filename, INI_BUFFERSIZE);
                ini_rename(local_buffer, filename);
                return 1;
            } /* if */
            /* Copy the line from source to dest, but not if this is the section that
             * we are looking for and this section must be removed
             */
            sp = ini_skip_leading(local_buffer);
            ep = _tcschr(sp, ']');
            match = (*sp == '[' && ep != NULL && (int)(ep-sp-1) == len && _tcsnicmp(sp + 1,section,len) == 0);
            if (!match || key!=NULL)
            {
                /* Remove blank lines, but insert a blank line (possibly one that was
                 * removed on the previous iteration) before a new section. This creates
                 * "neat" INI files.
                 */
                if (_tcslen(sp) > 0)
                {
                    if (*sp == '[' && count > 0)
                        ini_write(INI_LINETERM, &wfp);
                    ini_write(sp, &wfp);
                    count++;
                } /* if */
            } /* if */
        }
        while (!match);
    } /* if */

    /* Now that the section has been found, find the entry. Stop searching
     * upon leaving the section's area. Copy the file as it is read
     * and create an entry if one is not found.
     */
    len = (key!=NULL) ? _tcslen(key) : 0;
    for ( ;; )
    {
        if (!ini_read(local_buffer, INI_BUFFERSIZE, &rfp))
        {
            /* EOF without an entry so make one */
            if (key!=NULL && value!=NULL)
            {
                ini_write(INI_LINETERM, &wfp);  /* force a new line (there may not have been one) behind the last line of the INI file */
                ini_write_key(local_buffer, key, value, &wfp);
            } /* if */
            /* Clean up and rename */
            ini_close(&rfp);
            ini_close(&wfp);
            ini_remove(filename);
            ini_tempname(local_buffer, filename, INI_BUFFERSIZE);
            ini_rename(local_buffer, filename);
            return 1;
        } /* if */
        sp = ini_skip_leading(local_buffer);
        ep = _tcschr(sp, '='); /* Parse out the equal sign */
        if (ep == NULL)
            ep = _tcschr(sp, ':');
        match = (ep != NULL && (int)(ep-sp) == len && _tcsnicmp(sp,key,len) == 0);
        if ((key!=NULL && match) || *sp == '[')
            break;  /* found the key, or found a new section */
        /* in the section that we re-write, do not copy empty lines */
        if (key!=NULL && _tcslen(sp) > 0)
            ini_write(sp, &wfp);
    } /* for */
    if (*sp == '[')
    {
        /* found start of new section, the key was not in the specified
         * section, so we add it just before the new section
         */
        if (key!=NULL && value!=NULL)
        {
            /* We cannot use "ini_write_key()" here, because we need to preserve the
             * contents of local_buffer.
             */
            ini_write(key, &wfp);
            ini_write("=", &wfp);
            ini_write(value, &wfp);
            ini_write(INI_LINETERM INI_LINETERM, &wfp); /* put a blank line between the current and the next section */
        } /* if */
        /* write the new section header that we read previously */
        ini_write(sp, &wfp);
    }
    else
    {
        /* We found the key; ignore the line just read (with the key and
         * the current value) and write the key with the new value.
         */
        if (key!=NULL && value!=NULL)
            ini_write_key(local_buffer, key, value, &wfp);
    } /* if */
    /* Copy the rest of the INI file (removing empty lines, except before a section) */
    while (ini_read(local_buffer, INI_BUFFERSIZE, &rfp))
    {
        sp = ini_skip_leading(local_buffer);
        if (_tcslen(sp) > 0)
        {
            if (*sp == '[')
                ini_write(INI_LINETERM, &wfp);
            ini_write(sp, &wfp);
        } /* if */
    } /* while */
    /* Clean up and rename */
    ini_close(&rfp);
    ini_close(&wfp);
    ini_remove(filename);
    ini_tempname(local_buffer, filename, INI_BUFFERSIZE);
    ini_rename(local_buffer, filename);
    return 1;
}

/* Ansi C "itoa" based on Kernighan & Ritchie's "Ansi C" book. */
#define ABS(v)  ((v) < 0 ? -(v) : (v))

static void ini_string_reverse(tchar_t *str)
{
    tchar_t t;
    int i, j;

    for (i = 0, j = _tcslen(str) - 1; i < j; i++, j--)
    {
        t = str[i];
        str[i] = str[j];
        str[j] = t;
    } /* for */
}

static void ini_long2str(long value, tchar_t *str)
{
    int i = 0;
    long sign = value;
    int n;

    /* generate digits in reverse order */
    do
    {
        n = (int)(value % 10);              /* get next lowest digit */
        str[i++] = (tchar_t)(ABS(n) + '0');   /* handle case of negative digit */
    }
    while (value /= 10);                /* delete the lowest digit */
    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';

    ini_string_reverse(str);
}

/** ini_putl()
 * \param section     the name of the section to write the value in
 * \param key         the name of the entry to write, or NULL to erase all keys in the section
 * \param value       the value to write
 * \param filename    the name and full path of the .ini file to write to
 *
 * \return            1 if successful, otherwise 0
 */
int ini_putl(const tchar_t *section, const tchar_t *key, long value, const tchar_t *filename)
{
    tchar_t str[32];
    ini_long2str(value, str);
    return ini_puts(section, key, str, filename);
}
#endif /* !INI_READONLY */


#if defined PORTABLE_STRNICMP
int strnicmp(const tchar_t *s1, const tchar_t *s2, size_t n)
{
    register unsigned tchar_t c1, c2;

    while (n-- != 0 && (*s1 || *s2))
    {
        c1 = *(const unsigned tchar_t *)s1++;
        if ('a' <= c1 && c1 <= 'z')
            c1 += ('A' - 'a');
        c2 = *(const unsigned tchar_t *)s2++;
        if ('a' <= c2 && c2 <= 'z')
            c2 += ('A' - 'a');
        if (c1 != c2)
            return c1 - c2;
    } /* while */
    return 0;
}
#endif /* PORTABLE_STRNICMP */



