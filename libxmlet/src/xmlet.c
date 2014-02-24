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

#include "xmlbuffer.h"
#include "xmlparser.h"

/*================================================================
*   XmlCopyWithEscape
*
*
*=================================================================*/
void
XmlCopyWithEscape(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlBuffer * buf,
    IN char *p )
{
    int i;
    int plen;

    if ( p == NULL )
        return;

    plen = strlen( p );

    for ( i = 0; i < plen; i++ )
    {
        switch ( p[i] )
        {
        case '<':
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "&lt;" );
            break;

        case '>':
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "&gt;" );
            break;

        case '&':
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "&amp;" );
            break;

        case '\'':
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "&apos;" );
            break;

        case '\"':
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "&quot;" );
            break;

        default:
            XmlBufferAppend(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                buf, &p[i] );
        }
    }
}

/*================================================================
*	XmlPrintDomTreeRecursive
*       It is a recursive function to print all the node in a tree.
*       Internal to parser only.
*
*=================================================================*/
void
XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlNode *   nodeptr,
    XmlBuffer * buf )
{
    char *nodeName = NULL;
    char *nodeValue = NULL;
    XmlNode *  child = NULL;
    XmlNode *  sibling = NULL;

    if ( nodeptr != NULL )
    {
        nodeName = ( char * )XmlNodeGetNodeName( nodeptr )? ( char * )XmlNodeGetNodeName( nodeptr ):"";
        nodeValue = XmlNodeGetNodeValue( nodeptr );

        switch ( XmlNodeGetNodeType( nodeptr ) )
        {

        case XML_TEXT_NODE:
            XmlCopyWithEscape(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                buf, nodeValue );
            break;

        case XML_CDATA_SECTION_NODE:
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeValue );
            break;

        case XML_PROCESSING_INSTRUCTION_NODE:
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "<?" );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeName );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, " " );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeValue );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "?>\n" );
            break;

        case XML_DOCUMENT_NODE:
            XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                XmlNodeGetFirstChild
                ( nodeptr ), buf );
            break;

        case XML_ATTRIBUTE_NODE:
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeName );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "=\"" );
            if ( nodeValue != NULL )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, nodeValue );
            }
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "\"" );
            if ( nodeptr->nextSibling != NULL )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, " " );
                XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    nodeptr->nextSibling, buf );
            }
            break;

        case XML_ELEMENT_NODE:
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "<" );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeName );

            if ( nodeptr->firstAttr != NULL )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    buf, " " );
                XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    nodeptr->firstAttr, buf );
            }

            child = XmlNodeGetFirstChild( nodeptr );
            if ( ( child != NULL )
                    && ( XmlNodeGetNodeType( child ) ==
                         XML_ELEMENT_NODE ) )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    buf, ">\n" );
                //  output the children
                XmlPrintDomTreeRecursive(
#ifdef   USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    XmlNodeGetFirstChild
                    ( nodeptr ), buf );


            }
            else
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    buf, ">" );


                if (nodeValue)
                {
                    XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                        pool ,
#endif
                        buf, nodeValue );
                }

            }


            // Done with children.  Output the end tag.
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "</" );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeName );

            sibling = XmlNodeGetNextSibling( nodeptr );
            if ( sibling != NULL
                    && XmlNodeGetNodeType( sibling ) == XML_TEXT_NODE )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, ">" );
            }
            else
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, ">\n" );
            }
            XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                XmlNodeGetNextSibling
                ( nodeptr ), buf );
            break;

        default:
            break;
        }
    }
}

/*================================================================
*   XmlPrintDomTree
*       Print a DOM tree.
*       Element, and Attribute nodes are handled differently.
*       We don't want to print the Element and Attribute nodes' sibling.
*       External function.
*
*=================================================================*/
void
XmlPrintDomTree(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlNode *   nodeptr,
    XmlBuffer * buf )
{
    char *nodeName = NULL;
    char *nodeValue = NULL;
    XmlNode *  child = NULL;

    if ( ( nodeptr == NULL ) || ( buf == NULL ) )
    {
        return;
    }

    nodeName = ( char * )XmlNodeGetNodeName( nodeptr )? ( char * )XmlNodeGetNodeName( nodeptr ):"";
    nodeValue = XmlNodeGetNodeValue( nodeptr );

    switch ( XmlNodeGetNodeType( nodeptr ) )
    {

    case XML_TEXT_NODE:
    case XML_CDATA_SECTION_NODE:
    case XML_PROCESSING_INSTRUCTION_NODE:
    case XML_DOCUMENT_NODE:
        XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            nodeptr, buf );
        break;

    case XML_ATTRIBUTE_NODE:
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, nodeName );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, "=\"" );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, nodeValue );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, "\"" );
        break;

    case XML_ELEMENT_NODE:
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, "<" );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            buf, nodeName );

        if ( nodeptr->firstAttr != NULL )
        {
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, " " );
            XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                nodeptr->firstAttr, buf );
        }


        child = XmlNodeGetFirstChild( nodeptr );
        if ( ( child != NULL )
                && ( XmlNodeGetNodeType( child ) == XML_ELEMENT_NODE ) )
        {
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, ">\n" );


            //  output the children
            XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                XmlNodeGetFirstChild( nodeptr ),
                buf );

        }
        else
        {
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, ">" );


            if (nodeValue)
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, nodeValue );
            }
        }



        // Done with children.  Output the end tag.
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, "</" );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, nodeName );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, ">\n" );
        break;

    default:
        break;
    }
}

/*================================================================
*	XmlPrintDomTreeIdent
*       It is a recursive function to print all the node in a tree.
*       Internal to parser only.
*
*=================================================================*/
void
XmlPrintDomTreeIdent(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    XmlNode *   nodeptr,
    XmlBuffer * buf )
{
    char *nodeName = NULL;
    char *nodeValue = NULL;
    XmlNode *  child = NULL;
    XmlNode *  sibling = NULL;
    int i;
    static int curDeepth = 0;

    if ( nodeptr != NULL )
    {
        nodeName = ( char * )XmlNodeGetNodeName( nodeptr )?( char * )XmlNodeGetNodeName( nodeptr ):"";
        nodeValue = XmlNodeGetNodeValue( nodeptr );

        switch ( XmlNodeGetNodeType( nodeptr ) )
        {

        case XML_TEXT_NODE:
            XmlCopyWithEscape(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeValue );
            break;

        case XML_CDATA_SECTION_NODE:
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeValue );
            break;

        case XML_PROCESSING_INSTRUCTION_NODE:
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "<?" );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeName );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, " " );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeValue );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "?>\n" );
            break;

        case XML_DOCUMENT_NODE:
            XmlPrintDomTreeIdent(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                XmlNodeGetFirstChild
                ( nodeptr ), buf );
            break;

        case XML_ATTRIBUTE_NODE:
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeName );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "=\"" );
            if ( nodeValue != NULL )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, nodeValue );
            }
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "\"" );
            if ( nodeptr->nextSibling != NULL )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, " " );
                XmlPrintDomTreeIdent(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    nodeptr->nextSibling, buf );
            }
            break;

        case XML_ELEMENT_NODE:

            for (i=0;i<curDeepth;i++)
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, "  " );

            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "<" );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeName );

            if ( nodeptr->firstAttr != NULL )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, " " );
                XmlPrintDomTreeIdent(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    nodeptr->firstAttr, buf );
            }

            child = XmlNodeGetFirstChild( nodeptr );
            if ( ( child != NULL )
                    && ( XmlNodeGetNodeType( child ) ==
                         XML_ELEMENT_NODE ) )
            {

                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, ">\n" );

                curDeepth++;

            }
            else
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, ">" );
            }

            //  output the children
            XmlPrintDomTreeIdent(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                XmlNodeGetFirstChild
                ( nodeptr ), buf );

            if (( child != NULL ) &&
                    XmlNodeGetNodeType( child ) == XML_ELEMENT_NODE)
            {
                curDeepth--;

                for (i=0;i<curDeepth;i++)
                    XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                        pool ,
#endif
                        buf, "  " );
            }

            // Done with children.  Output the end tag.
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, "</" );
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, nodeName );

            sibling = XmlNodeGetNextSibling( nodeptr );
            if ( sibling != NULL
                    && XmlNodeGetNodeType( sibling ) == XML_TEXT_NODE )
            {
                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, ">" );
            }
            else
            {

                XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    buf, ">\n" );

            }
            XmlPrintDomTreeIdent(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                XmlNodeGetNextSibling
                ( nodeptr ), buf );
            break;

        default:
            break;
        }
    }

}

/*================================================================
*   XmlDomTreetoString
*       Converts a DOM tree into a text string
*       Element, and Attribute nodes are handled differently.
*       We don't want to print the Element and Attribute nodes' sibling.
*       External function.
*
*=================================================================*/
void
XmlDomTreetoString(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlNode *   nodeptr,
    XmlBuffer * buf )
{
    char *nodeName = NULL;
    char *nodeValue = NULL;
    XmlNode *  child = NULL;

    if ( ( nodeptr == NULL ) || ( buf == NULL ) )
    {
        return;
    }

    nodeName = ( char * )XmlNodeGetNodeName( nodeptr )?( char * )XmlNodeGetNodeName( nodeptr ):"";
    nodeValue = XmlNodeGetNodeValue( nodeptr );

    switch ( XmlNodeGetNodeType( nodeptr ) )
    {

    case XML_TEXT_NODE:
    case XML_CDATA_SECTION_NODE:
    case XML_PROCESSING_INSTRUCTION_NODE:
    case XML_DOCUMENT_NODE:
        XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            nodeptr, buf );
        break;

    case XML_ATTRIBUTE_NODE:
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, nodeName );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, "=\"" );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, nodeValue );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, "\"" );
        break;

    case XML_ELEMENT_NODE:
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, "<" );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, nodeName );

        if ( nodeptr->firstAttr != NULL )
        {
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, " " );
            XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                nodeptr->firstAttr, buf );
        }

        child = XmlNodeGetFirstChild( nodeptr );
        if ( ( child != NULL )
                && ( XmlNodeGetNodeType( child ) == XML_ELEMENT_NODE ) )
        {
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, ">" );
        }
        else
        {
            XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                buf, ">" );
        }

        //  output the children
        XmlPrintDomTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            XmlNodeGetFirstChild( nodeptr ),
            buf );

        // Done with children.  Output the end tag.
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, "</" );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, nodeName );
        XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            buf, ">" );
        break;

    default:
        break;
    }
}

/*================================================================
*   XmlLoadDocumentEx
*       Parses the given file, and returns the DOM tree from it.
*       External function.
*
*=================================================================*/
int
XmlLoadDocumentEx(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    IN char *xmlFile,
    XmlDocument ** doc )
{

    if ( ( xmlFile == NULL ) || ( doc == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    return XmlParserLoadDocument(
#ifdef USE_CWMP_MEMORY_POOL
               pool ,
#endif
               doc, xmlFile, TRUE );
}

/*================================================================
*   XmlLoadDocument
*       Parses the given file, and returns the DOM tree from it.
*       External function.
*
*=================================================================*/
XmlDocument *
XmlLoadDocument(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    IN char *xmlFile )
{

    XmlDocument *doc = NULL;

    XmlLoadDocumentEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        xmlFile, &doc );
    return doc;
}

/*================================================================
*   XmlPrintNode
*       Print DOM tree under node. Puts lots of white spaces
*       External function.
*
*=================================================================*/
char *
XmlPrintNode(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlNode *   node )
{

    XmlBuffer memBuf;
    XmlBuffer *buf = &memBuf;

    if ( node == NULL )
    {
        return NULL;
    }

    XmlBufferInit( buf );
    XmlPrintDomTree(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif

        node, buf );
    return buf->buf;

}

/*================================================================
*   XmlPrintNode
*       converts DOM tree under node to text string
*       External function.
*
*=================================================================*/
char *
XmlNodeToString(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlNode *   node )
{

    XmlBuffer memBuf;
    XmlBuffer *buf = &memBuf;

    if ( node == NULL )
    {
        return NULL;
    }

    XmlBufferInit( buf );
    XmlDomTreetoString(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif

        node, buf );
    return buf->buf;

}

/*================================================================
*   XmlParseBufferEx
*       Parse xml file stored in buffer.
*       External function.
*
*=================================================================*/
int
XmlParseBufferEx(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    IN char *buffer,
    XmlDocument ** retDoc )
{

    if ( ( buffer == NULL ) || ( retDoc == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    if ( strlen( buffer ) == 0 )
    {
        return XML_INVALID_PARAMETER;
    }

    return XmlParserLoadDocument(
#ifdef USE_CWMP_MEMORY_POOL
               pool ,
#endif
               retDoc, buffer, FALSE );
}


/*================================================================
*   XmlParseBuffer
*       Parse xml file stored in buffer.
*       External function.
*
*=================================================================*/
XmlDocument *
XmlParseBuffer(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    IN char *buffer )
{
    XmlDocument *doc = NULL;

    XmlParseBufferEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        buffer, &doc );
    return doc;
}

/*================================================================
*   XmlCloneDOMString
*       Clones a DOM String.
*       External function.
*
*=================================================================*/
char *
XmlCloneDOMString(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    const char *  src )
{
    if ( src == NULL )
    {
        return NULL;
    }

    return ( PSTRDUP( src ) );
}

/*================================================================
*   XmlFreeDOMString
*       Frees a DOM String.
*       External function.
*
*=================================================================*/
void
XmlFreeDOMString(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    IN char *  buf )
{
    if ( buf != NULL )
    {
        PFREE( buf );
    }
}


