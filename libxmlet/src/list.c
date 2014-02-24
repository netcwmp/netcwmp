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

#include "xmlparser.h"

/*================================================================
*   XmlNodeListInit
*       initializes a nodelist
*       External function.
*
*=================================================================*/
void
XmlNodeListInit( XmlNodeList * nList )
{
    assert( nList != NULL );

    memset( nList, 0, sizeof( XmlNodeList ) );

}

/*================================================================
*   XmlNodeListItem
*       Returns the indexth item in the collection. If index is greater
*       than or equal to the number of nodes in the list, this returns
*       null.
*       External function.
*
*=================================================================*/
XmlNode *
XmlNodeListItem( XmlNodeList * nList,
                 unsigned long index )
{
    XmlNodeList *next;
    unsigned int i;

    // if the list ptr is NULL
    if ( nList == NULL )
    {
        return NULL;
    }
    // if index is more than list length
    if ( index > XmlNodeListLength( nList ) - 1 )
    {
        return NULL;
    }

    next = nList;
    for ( i = 0; i < index && next != NULL; ++i )
    {
        next = next->next;
    }

    return next->nodeItem;

}

/*================================================================
*   XmlNodeListAddToNodeList
*       Add a node to nodelist
*       Internal to parser only.
*
*=================================================================*/
int

#ifdef USE_CWMP_MEMORY_POOL
XmlNodeListAddToNodeList(Pool * pool, XmlNodeList ** nList,
#else
XmlNodeListAddToNodeList( XmlNodeList ** nList,
#endif

                         XmlNode *   add )
{
    XmlNodeList *traverse,
    *p = NULL;
    XmlNodeList *newListItem;

    assert( add != NULL );

    if ( add == NULL )
    {
        return XML_FAILED;
    }

    if ( *nList == NULL )       // nodelist is empty
    {
        *nList = ( XmlNodeList * ) PMALLOC( sizeof( XmlNodeList ) );
        if ( *nList == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }

        XmlNodeListInit( *nList );
    }

    if ( ( *nList )->nodeItem == NULL )
    {
        ( *nList )->nodeItem = add;
    }
    else
    {
        traverse = *nList;
        while ( traverse != NULL )
        {
            p = traverse;
            traverse = traverse->next;
        }

        newListItem =
            ( XmlNodeList * ) PMALLOC( sizeof( XmlNodeList ) );
        if ( newListItem == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
        p->next = newListItem;
        newListItem->nodeItem = add;
        newListItem->next = NULL;
    }

    return XML_OK;
}

/*================================================================
*   XmlNodeListLength
*       Returns the number of nodes in the list.  The range of valid
*       child node indices is 0 to length-1 inclusive.
*       External function.
*
*=================================================================*/
unsigned long
XmlNodeListLength( XmlNodeList * nList )
{
    XmlNodeList *list;
    unsigned long length = 0;

    list = nList;
    while ( list != NULL )
    {
        ++length;
        list = list->next;
    }

    return length;
}

/*================================================================
*   XmlNodeListFree
*       XFREEs a nodeList
*       External function
*
*=================================================================*/
void
XmlNodeListFree(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    XmlNodeList * nList )
{
    XmlNodeList *next;

    while ( nList != NULL )
    {
        next = nList->next;

        PFREE( nList );
        nList = next;
    }

}

