///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2003 Intel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// * Neither name of Intel Corporation nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "xmlbuffer.h"
#include "cwmp/xmlet.h"

#include "xmlparser.h"
/*================================================================
*   XmlBufferSetSize
*
*   Increases or decreases buffer cap so that at least
*   'new_length' bytes can be stored
*
*   On error, m's fields do not change.
*
*   returns:
*       UPNP_E_SUCCESS
*       UPNP_E_OUTOF_MEMORY
*
*=================================================================*/
static int
XmlBufferSetSize(
    Pool * pool,
    XmlBuffer * m,
    IN size_t new_length )
{
    size_t diff;
    size_t alloc_len;
    char *temp_buf;

    if ( new_length > m->length )  // increase length
    {
        // need more mem?
        if ( new_length < m->capacity )
        {
            return 0;           // have enough mem; done
        }

        if (m->length == 0)
        {
            alloc_len = new_length + 1;
            temp_buf = PMALLOC(alloc_len);
            if ( temp_buf == NULL )
                return XML_INSUFFICIENT_MEMORY;
            m->buf = temp_buf;
            m->capacity = alloc_len;
            return 0;
        }

        diff = new_length - m->length + 1;
        alloc_len = MAXVAL( m->size_inc, diff ) + m->capacity;
    }
    else                      // decrease length
    {
        assert( new_length <= m->length );
        return 0;
        /*        // if diff is 0..m->size_inc, don't PFREE
                if( ( m->capacity - new_length ) <= m->size_inc ) {
                    return 0;
                }

                alloc_len = new_length + m->size_inc;
        */
    }

    assert( alloc_len > new_length );

    temp_buf = PREALLOC( m->buf, m->length, alloc_len );
    if ( temp_buf == NULL )
    {
        // try smaller size
        alloc_len = new_length + 1;
        temp_buf = PREALLOC( m->buf, m->length,  alloc_len);

        if ( temp_buf == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
    }
    // save
    m->buf = temp_buf;
    m->capacity = alloc_len;
    return 0;
}

/*================================================================
*   memcwmp_buffer_init
*
*
*=================================================================*/
void
XmlBufferInit( XmlBuffer * m )
{
    assert( m != NULL );

    m->size_inc = MEMBUF_DEF_SIZE_INC;
    m->buf = NULL;
    m->length = 0;
    m->capacity = 0;
}

/*================================================================
*   memcwmp_buffer_destroy
*
*
*=================================================================*/
void
XmlBufferDestroy(
    Pool * pool,
    XmlBuffer * m )
{
    if ( m == NULL )
    {
        return;
    }

    if (m->buf)	//laoyou fixed
        PFREE( m->buf );
    XmlBufferInit( m );
}

/*================================================================
*   XmlBufferAssign
*
*
*=================================================================*/
int
XmlBufferAssign(
    Pool * pool,

    XmlBuffer * m,
    IN const void *buf,
    IN size_t buf_len )
{
    int return_code;

    assert( m != NULL );

    // set value to null
    if ( buf == NULL )
    {
        XmlBufferDestroy(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            m );
        return XML_OK;
    }
    // alloc mem
    return_code = XmlBufferSetSize(
#ifdef USE_CWMP_MEMORY_POOL
                      pool ,
#endif
                      m, buf_len );
    if ( return_code != 0 )
    {
        return return_code;
    }
    // copy
    memcpy( m->buf, buf, buf_len );
    m->buf[buf_len] = 0;        // null-terminate

    m->length = buf_len;

    return XML_OK;

}

/*================================================================
*   XmlBufferAssignString
*
*
*=================================================================*/
int
XmlBufferAssignString(
    Pool * pool,
    XmlBuffer * m,
    IN const char *c_str )
{
    return XmlBufferAssign(
#ifdef USE_CWMP_MEMORY_POOL
               pool ,
#endif
               m, c_str, strlen( c_str ) );
}

/*================================================================
*   XmlBufferAppend
*
*
*=================================================================*/
int
XmlBufferAppend(
    Pool * pool,

    XmlBuffer * m,
    IN const void *buf )
{
    assert( m != NULL );

    return XmlBufferInsert(
#ifdef USE_CWMP_MEMORY_POOL
               pool ,
#endif

               m, buf, 1, m->length );
}

/*================================================================
*   XmlBufferAppendString
*
*
*=================================================================*/
int
XmlBufferAppendString(
    Pool * pool,

    XmlBuffer * m,
    IN const char *c_str )
{
    return XmlBufferInsert(
#ifdef USE_CWMP_MEMORY_POOL
               pool ,
#endif

               m, c_str, strlen( c_str ), m->length );
}

/*================================================================
*   XmlBufferInsert
*
*
*=================================================================*/
int
XmlBufferInsert(
    Pool * pool,

    XmlBuffer * m,
    IN const void *buf,
    IN size_t buf_len,
    int index )
{
    int return_code;

    assert( m != NULL );

    if ( index < 0 || index > ( int )m->length )
        return XML_INDEX_SIZE_ERR;

    if ( buf == NULL || buf_len == 0 )
    {
        return 0;
    }
    // alloc mem
    return_code = XmlBufferSetSize(
#ifdef USE_CWMP_MEMORY_POOL
                      pool ,
#endif

                      m, m->length + buf_len );
    if ( return_code != 0 )
    {
        return return_code;
    }
    // insert data
    // move data to right of insertion point
    memmove( m->buf + index + buf_len, m->buf + index, m->length - index );
    memcpy( m->buf + index, buf, buf_len );
    m->length += buf_len;
    m->buf[m->length] = 0;      // null-terminate

    return 0;
}
