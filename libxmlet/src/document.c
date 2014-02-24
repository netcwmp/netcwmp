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

#include <stdio.h>
#include <stdlib.h>

#include "xmlparser.h"

/*================================================================
*   XmlDocumentInit
*       It initialize the document structure.
*       External function.
*
*=================================================================*/
void
XmlDocumentInit( XmlDocument * doc )
{
    memset( doc, 0, sizeof( XmlDocument ) );
}

/*================================================================
*   XmlDocumentFree
*       It XFREEs the whole document tree.
*       External function.
*
*=================================================================*/
void
XmlDocumentFree(
    Pool * pool,
    XmlDocument * doc )
{
    if ( doc != NULL )
    {
        XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            ( XmlNode *   ) doc );
    }

}

/*================================================================
*   XmlDocSetOwnerDocument
*
*       When this function is called first time, nodeptr is the root
*       of the subtree, so it is not necessay to do two steps
*       recursion.
*
*       Internal function called by XmlDocImportNode
*
*=================================================================*/
void
XmlDocSetOwnerDocument( XmlDocument * doc,
                        XmlNode *   nodeptr )
{
    if ( nodeptr != NULL )
    {
        nodeptr->ownerDocument = doc;
        XmlDocSetOwnerDocument( doc,
                                XmlNodeGetFirstChild( nodeptr ) );
        XmlDocSetOwnerDocument( doc,
                                XmlNodeGetNextSibling
                                ( nodeptr ) );
    }
}

/*================================================================
*   XmlDocImportNode
*       Imports a node from another document to this document. The
*       returned node has no parent; (parentNode is null). The source
*       node is not altered or removed from the original document;
*       this method creates a new copy of the source node.

*       For all nodes, importing a node creates a node object owned
*       by the importing document, with attribute values identical to
*       the source node's nodeName and nodeType, plus the attributes
*       related to namespaces (prefix, localName, and namespaceURI).
*       As in the cloneNode operation on a node, the source node is
*       not altered.
*
*       External function.
*
*=================================================================*/
int
XmlDocImportNode(
    Pool * pool,

    XmlDocument * doc,
    XmlNode *   importNode,
    IN BOOL deep,
    OUT XmlNode ** rtNode )
{
    unsigned short nodeType;
    XmlNode *  newNode;

    *rtNode = NULL;

    if ( ( doc == NULL ) || ( importNode == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    nodeType = XmlNodeGetNodeType( importNode );
    if ( nodeType == XML_DOCUMENT_NODE )
    {
        return XML_NOT_SUPPORTED_ERR;
    }

    newNode = XmlNodeCloneNode(
#ifdef USE_CWMP_MEMORY_POOL
                  pool ,
#endif

                  importNode, deep );
    if ( newNode == NULL )
    {
        return XML_FAILED;
    }

    XmlDocSetOwnerDocument( doc, newNode );
    *rtNode = newNode;

    return XML_OK;
}

/*================================================================
*   XmlDocCreateElementEx
*       Creates an element of the type specified.
*       External function.
*   Parameters:
*       doc:        pointer to document
*       tagName:    The name of the element, it is case-sensitive.
*   Return Value:
*       XML_OK
*       XML_INVALID_PARAMETER:     if either doc or tagName is NULL
*       XML_INSUFFICIENT_MEMORY:   if not enough memory to finish this operations.
*
*=================================================================*/
int
#ifdef USE_CWMP_MEMORY_POOL
XmlDocCreateElementEx(Pool * pool, XmlDocument * doc,
#else
XmlDocCreateElementEx( XmlDocument * doc,
#endif

                      IN char *  tagName,
                      OUT XmlElement ** rtElement )
{

    int errCode = XML_OK;
    XmlElement *newElement = NULL;

    if ( ( doc == NULL ) || ( tagName == NULL ) )
    {
        errCode = XML_INVALID_PARAMETER;
        goto ErrorHandler;
    }

    newElement = ( XmlElement * ) PMALLOC( sizeof( XmlElement ) );
    if ( newElement == NULL )
    {
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    XmlElementInit( newElement );
    newElement->tagName = PSTRDUP( tagName );
    if ( newElement->tagName == NULL )
    {
        XmlElementFree(
#ifdef USE_CWMP_MEMORY_POOL

            pool ,
#endif
            newElement );
        newElement = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }
    // set the node fields
    newElement->node.nodeType = XML_ELEMENT_NODE;
    newElement->node.nodeName = PSTRDUP( tagName );
    if ( newElement->node.nodeName == NULL )
    {
        XmlElementFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newElement );
        newElement = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    newElement->node.ownerDocument = doc;

ErrorHandler:
    *rtElement = newElement;
    return errCode;

}

/*================================================================
*   XmlDocCreateElement
*       Creates an element of the type specified.
*       External function.
*   Parameters:
*       doc:        pointer to document
*       tagName:    The name of the element, it is case-sensitive.
*   Return Value:
*       A new element object with the nodeName set to tagName, and
*       localName, prefix and namespaceURI set to null.
*
*=================================================================*/
XmlElement *

XmlDocCreateElement(
    Pool * pool,
    XmlDocument * doc,
    IN char *  tagName )
{
    XmlElement *newElement = NULL;

    XmlDocCreateElementEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        doc, tagName, &newElement );
    return newElement;

}

/*================================================================
*   XmlDocCreateDocumentEx
*       Creates an document object
*       Internal function.
*   Parameters:
*       rtDoc:  the document created or NULL on failure
*   Return Value:
*       XML_OK
*       XML_INSUFFICIENT_MEMORY:   if not enough memory to finish this operations.
*
*=================================================================*/
int
XmlDocCreateDocumentEx(
    Pool * pool,
    OUT XmlDocument ** rtDoc )
{
    XmlDocument *doc;
    int errCode = XML_OK;

    doc = NULL;
    doc = ( XmlDocument * ) PMALLOC( sizeof( XmlDocument ) );
    if ( doc == NULL )
    {
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    XmlDocumentInit( doc );

    doc->node.nodeName = PSTRDUP( DOCUMENTNODENAME );
    if ( doc->node.nodeName == NULL )
    {
        XmlDocumentFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            doc );
        doc = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    doc->node.nodeType = XML_DOCUMENT_NODE;
    doc->node.ownerDocument = doc;

ErrorHandler:
    *rtDoc = doc;
    return errCode;
}

/*================================================================
*   XmlDocCreateDocument
*       Creates an document object
*       Internal function.
*   Parameters:
*       none
*   Return Value:
*       A new document object with the nodeName set to "#document".
*
*=================================================================*/
XmlDocument *
XmlDocCreateDocument(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool
#endif
)
{
    XmlDocument *doc = NULL;

    XmlDocCreateDocumentEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        &doc );

    return doc;

}

/*================================================================
*   XmlDocCreateTextNodeEx
*       Creates an text node.
*       External function.
*   Parameters:
*       data: text data for the text node. It is stored in nodeValue field.
*   Return Value:
*       XML_OK
*       XML_INVALID_PARAMETER:     if either doc or data is NULL
*       XML_INSUFFICIENT_MEMORY:   if not enough memory to finish this operations.
*
*=================================================================*/
int
#ifdef USE_CWMP_MEMORY_POOL
XmlDocCreateTextNodeEx(Pool * pool,  XmlDocument * doc,
#else
XmlDocCreateTextNodeEx( XmlDocument * doc,
#endif
                       IN char *data,
                       OUT XmlNode ** textNode )
{
    XmlNode *  returnNode;
    int rc = XML_OK;

    returnNode = NULL;
    if ( ( doc == NULL ) || ( data == NULL ) )
    {
        rc = XML_INVALID_PARAMETER;
        goto ErrorHandler;
    }

    returnNode = ( XmlNode *   ) PMALLOC( sizeof( XmlNode ) );
    if ( returnNode == NULL )
    {
        rc = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }
    // initialize the node
    XmlNodeInit( returnNode );

    returnNode->nodeName = PSTRDUP( TEXTNODENAME );
    if ( returnNode->nodeName == NULL )
    {
        XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            returnNode );
        returnNode = NULL;
        rc = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }
    // add in node value
    if ( data != NULL )
    {
        returnNode->nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                    pool ,
#endif
                                    data );
        if ( returnNode->nodeValue == NULL )
        {
            XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                returnNode );
            returnNode = NULL;
            rc = XML_INSUFFICIENT_MEMORY;
            goto ErrorHandler;
        }
    }

    returnNode->nodeType = XML_TEXT_NODE;
    returnNode->ownerDocument = doc;

ErrorHandler:
    *textNode = returnNode;
    return rc;

}

/*================================================================
*   XmlDocCreateTextNode
*       Creates an text node.
*       External function.
*   Parameters:
*       data: text data for the text node. It is stored in nodeValue field.
*   Return Value:
*       The new text node.
*
*=================================================================*/
XmlNode *
XmlDocCreateTextNode(
    Pool * pool,

    XmlDocument * doc,
    IN char *data )
{
    XmlNode *  returnNode = NULL;

    XmlDocCreateTextNodeEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        doc, data, &returnNode );

    return returnNode;
}

/*================================================================
*   XmlDocCreateAttributeEx
*       Creates an attribute of the given name.
*       External function.
*   Parameters:
*       name: The name of the Attribute node.
*   Return Value:
*       XML_OK
*       XML_INSUFFICIENT_MEMORY:   if not enough memory to finish this operations.
*
================================================================*/
int
#ifdef USE_CWMP_MEMORY_POOL
XmlDocCreateAttributeEx(Pool * pool, XmlDocument * doc,
#else
XmlDocCreateAttributeEx( XmlDocument * doc,
#endif

                        const char *name,
                        XmlAttribute ** rtAttr )
{
    XmlAttribute *attrNode = NULL;
    int errCode = XML_OK;

    attrNode = ( XmlAttribute * ) PMALLOC( sizeof( XmlAttribute ) );
    if ( attrNode == NULL )
    {
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    if ( ( doc == NULL ) || ( name == NULL ) )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            attrNode );
        attrNode = NULL;
        errCode = XML_INVALID_PARAMETER;
        goto ErrorHandler;
    }

    XmlAttrInit( attrNode );

    attrNode->node.nodeType = XML_ATTRIBUTE_NODE;

    // set the node fields
    attrNode->node.nodeName = PSTRDUP( name );
    if ( attrNode->node.nodeName == NULL )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            attrNode );
        attrNode = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    attrNode->node.ownerDocument = doc;

ErrorHandler:
    *rtAttr = attrNode;
    return errCode;

}

/*================================================================
*   XmlDocCreateAttribute
*       Creates an attribute of the given name.
*       External function.
*   Parameters:
*       name: The name of the Attribute node.
*   Return Value:
*       A new attr object with the nodeName attribute set to the
*       given name, and the localName, prefix and namespaceURI set to NULL.
*       The value of the attribute is the empty string.
*
================================================================*/
XmlAttribute *
XmlDocCreateAttribute(
    Pool * pool,
    XmlDocument * doc,
    IN char *name )
{
    XmlAttribute *attrNode = NULL;

    XmlDocCreateAttributeEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif

        doc, name, &attrNode );
    return attrNode;

}

/*================================================================
*   XmlDocCreateAttributeNSEx
*       Creates an attrbute of the given name and namespace URI
*       External function.
*   Parameters:
*       namespaceURI: the namespace fo the attribute to create
*       qualifiedName: qualifiedName of the attribute to instantiate
*   Return Value:
*       XML_OK
*       XML_INVALID_PARAMETER:     if either doc,namespaceURI or qualifiedName is NULL
*       XML_INSUFFICIENT_MEMORY:   if not enough memory to finish this operations.
*
*=================================================================*/
int
XmlDocCreateAttributeNSEx(
    Pool * pool,

    XmlDocument * doc,
    const char *  namespaceURI,
    const char *  qualifiedName,
    OUT XmlAttribute ** rtAttr )
{
    XmlAttribute *attrNode = NULL;
    int errCode = XML_OK;

    if ( ( doc == NULL ) || ( namespaceURI == NULL )
            || ( qualifiedName == NULL ) )
    {
        errCode = XML_INVALID_PARAMETER;
        goto ErrorHandler;
    }

    errCode =
        XmlDocCreateAttributeEx(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            doc, qualifiedName, &attrNode );
    if ( errCode != XML_OK )
    {
        goto ErrorHandler;
    }
    // set the namespaceURI field
    attrNode->node.namespaceURI = PSTRDUP( namespaceURI );
    if ( attrNode->node.namespaceURI == NULL )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            attrNode );
        attrNode = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }
    // set the localName and prefix
    errCode =
        XmlNodeSetNodeName(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            ( XmlNode *   ) attrNode, qualifiedName );
    if ( errCode != XML_OK )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            attrNode );
        attrNode = NULL;
        goto ErrorHandler;
    }

ErrorHandler:
    *rtAttr = attrNode;
    return errCode;

}

/*================================================================
*   XmlDocCreateAttributeNS
*       Creates an attrbute of the given name and namespace URI
*       External function.
*   Parameters:
*       namespaceURI: the namespace fo the attribute to create
*       qualifiedName: qualifiedName of the attribute to instantiate
*   Return Value:
*       Creates an attribute node with the given namespaceURI and
*       qualifiedName. The prefix and localname are extracted from
*       the qualifiedName. The node value is empty.
*
*=================================================================*/
XmlAttribute *
XmlDocCreateAttributeNS(
    Pool * pool,

    XmlDocument * doc,
    IN char *  namespaceURI,
    IN char *  qualifiedName )
{
    XmlAttribute *attrNode = NULL;

    XmlDocCreateAttributeNSEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif

        doc, namespaceURI, qualifiedName,
        &attrNode );
    return attrNode;
}

/*================================================================
*   XmlDocCreateCDATASectionEx
*       Creates an CDATASection node whose value is the specified string
*       External function.
*   Parameters:
*       data: the data for the CDATASection contents.
*   Return Value:
*       XML_OK
*       XML_INVALID_PARAMETER:     if either doc or data is NULL
*       XML_INSUFFICIENT_MEMORY:   if not enough memory to finish this operations.
*
*=================================================================*/
int
#ifdef USE_CWMP_MEMORY_POOL
XmlDocCreateCDATASectionEx(Pool * pool, XmlDocument * doc,
#else
XmlDocCreateCDATASectionEx( XmlDocument * doc,
#endif
                           IN char *  data,
                           OUT XmlCDATASection ** rtCD )
{
    int errCode = XML_OK;
    XmlCDATASection *cDSectionNode = NULL;

    if ( ( doc == NULL ) || ( data == NULL ) )
    {
        errCode = XML_INVALID_PARAMETER;
        goto ErrorHandler;
    }

    cDSectionNode =
        ( XmlCDATASection * ) PMALLOC( sizeof( XmlCDATASection ) );
    if ( cDSectionNode == NULL )
    {
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    XmlCDATASectionInit( cDSectionNode );

    cDSectionNode->node.nodeType = XML_CDATA_SECTION_NODE;
    cDSectionNode->node.nodeName = PSTRDUP( CDATANODENAME );
    if ( cDSectionNode->node.nodeName == NULL )
    {
        XmlCDATASectionFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            cDSectionNode );
        cDSectionNode = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    cDSectionNode->node.nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                        pool ,
#endif
                                        data );
    if ( cDSectionNode->node.nodeValue == NULL )
    {
        XmlCDATASectionFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            cDSectionNode );
        cDSectionNode = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    cDSectionNode->node.ownerDocument = doc;

ErrorHandler:
    *rtCD = cDSectionNode;
    return errCode;

}

/*================================================================
*   XmlDocCreateCDATASection
*       Creates an CDATASection node whose value is the specified string
*       External function.
*   Parameters:
*       data: the data for the CDATASection contents.
*   Return Value:
*       The new CDATASection object.
*
*=================================================================*/
XmlCDATASection *
XmlDocCreateCDATASection(
    Pool * pool,

    XmlDocument * doc,
    IN char *  data )
{

    XmlCDATASection *cDSectionNode = NULL;

    XmlDocCreateCDATASectionEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif

        doc, data, &cDSectionNode );
    return cDSectionNode;
}

/*================================================================
*   XmlDocCreateElementNSEx
*       Creates an element of the given qualified name and namespace URI.
*       External function.
*   Parameters:
*       namespaceURI: the namespace URI of the element to create.
*       qualifiedName: the qualified name of the element to instantiate.
*   Return Value:
*   Return Value:
*       XML_OK
*       XML_INVALID_PARAMETER:     if either doc,namespaceURI or qualifiedName is NULL
*       XML_INSUFFICIENT_MEMORY:   if not enough memory to finish this operations.
*
*=================================================================*/
int
XmlDocCreateElementNSEx(
    Pool * pool,

    XmlDocument * doc,
    IN char *  namespaceURI,
    IN char *  qualifiedName,
    OUT XmlElement ** rtElement )
{

    XmlElement *newElement = NULL;
    int errCode = XML_OK;

    if ( ( doc == NULL ) || ( namespaceURI == NULL )
            || ( qualifiedName == NULL ) )
    {
        errCode = XML_INVALID_PARAMETER;
        goto ErrorHandler;
    }

    errCode =
        XmlDocCreateElementEx(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            doc, qualifiedName, &newElement );
    if ( errCode != XML_OK )
    {
        goto ErrorHandler;
    }
    // set the namespaceURI field
    newElement->node.namespaceURI = PSTRDUP( namespaceURI );
    if ( newElement->node.namespaceURI == NULL )
    {
        XmlElementFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newElement );
        newElement = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }
    // set the localName and prefix
    errCode =
        XmlNodeSetNodeName(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            ( XmlNode *   ) newElement, qualifiedName );
    if ( errCode != XML_OK )
    {
        XmlElementFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newElement );
        newElement = NULL;
        errCode = XML_INSUFFICIENT_MEMORY;
        goto ErrorHandler;
    }

    newElement->node.nodeValue = NULL;

ErrorHandler:
    *rtElement = newElement;
    return errCode;

}

/*================================================================
*   XmlDocCreateElementNS
*       Creates an element of the given qualified name and namespace URI.
*       External function.
*   Parameters:
*       namespaceURI: the namespace URI of the element to create.
*       qualifiedName: the qualified name of the element to instantiate.
*   Return Value:
*       The new element object with tagName qualifiedName, prefix and
*       localName extraced from qualfiedName, nodeName of qualfiedName,
*	    namespaceURI of namespaceURI.
*
*=================================================================*/
XmlElement *
XmlDocCreateElementNS(
    Pool * pool,

    XmlDocument * doc,
    IN char *  namespaceURI,
    IN char *  qualifiedName )
{
    XmlElement *newElement = NULL;

    XmlDocCreateElementNSEx(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif

        doc, namespaceURI, qualifiedName,
        &newElement );
    return newElement;
}

/*================================================================
*   XmlDocGetElementsByTagName
*       Returns a nodeList of all the Elements with a given tag name
*       in the order in which they are encountered in a preorder traversal
*       of the document tree.
*       External function.
*   Parameters:
*       tagName: the name of the tag to match on. The special value "*"
*                matches all tags.
*   Return Value:
*       A new nodeList object containing all the matched Elements.
*
*=================================================================*/
XmlNodeList *
XmlDocGetElementsByTagName(
    Pool * pool,
    XmlDocument * doc,
    IN char *tagName )
{
    XmlNodeList *returnNodeList = NULL;

    if ( ( doc == NULL ) || ( tagName == NULL ) )
    {
        return NULL;
    }

    XmlNodeGetElementsByTagName(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        ( XmlNode *   ) doc, tagName,
        &returnNodeList );
    return returnNodeList;
}

/*================================================================
*   XmlDocGetElementsByTagNameNS
*       Returns a nodeList of all the Elements with a given local name and
*       namespace URI in the order in which they are encountered in a
*       preorder traversal of the document tree.
*       External function.
*   Parameters:
*       namespaceURI: the namespace of the elements to match on. The special
*               value "*" matches all namespaces.
*       localName: the local name of the elements to match on. The special
*               value "*" matches all local names.
*   Return Value:
*       A new nodeList object containing all the matched Elements.
*
*=================================================================*/
XmlNodeList *
XmlDocGetElementsByTagNameNS(
    Pool * pool,
    XmlDocument * doc,
    IN char *  namespaceURI,
    IN char *  localName )
{
    XmlNodeList *returnNodeList = NULL;

    if ( ( doc == NULL ) || ( namespaceURI == NULL )
            || ( localName == NULL ) )
    {
        return NULL;
    }

    XmlNodeGetElementsByTagNameNS(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        ( XmlNode *   ) doc, namespaceURI,
        localName, &returnNodeList );
    return returnNodeList;
}

/*================================================================
*   XmlDocGetElementById
*       Returns the element whose ID is given by tagName. If no such
*       element exists, returns null.
*       External function.
*   Parameter:
*       tagName: the tag name for an element.
*   Return Values:
*       The matching element.
*
*=================================================================*/
XmlElement *
XmlDocGetElementById( XmlDocument * doc,
                      IN char *  tagName )
{
    XmlElement *rtElement = NULL;
    XmlNode *  nodeptr = ( XmlNode *   ) doc;
    const char *name;

    if ( ( nodeptr == NULL ) || ( tagName == NULL ) )
    {
        return rtElement;
    }

    if ( XmlNodeGetNodeType( nodeptr ) == XML_ELEMENT_NODE )
    {
        name = XmlNodeGetNodeName( nodeptr );
        if ( name == NULL )
        {
            return rtElement;
        }

        if ( strcmp( tagName, name ) == 0 )
        {
            rtElement = ( XmlElement * ) nodeptr;
            return rtElement;
        }
        else
        {
            rtElement = XmlDocGetElementById( ( XmlDocument * )
                                              XmlNodeGetFirstChild
                                              ( nodeptr ),
                                              tagName );
            if ( rtElement == NULL )
            {
                rtElement = XmlDocGetElementById( ( XmlDocument
                                                    * )
                                                  XmlNodeGetNextSibling
                                                  ( nodeptr ),
                                                  tagName );
            }
        }
    }
    else
    {
        rtElement = XmlDocGetElementById( ( XmlDocument * )
                                          XmlNodeGetFirstChild
                                          ( nodeptr ), tagName );
        if ( rtElement == NULL )
        {
            rtElement = XmlDocGetElementById( ( XmlDocument * )
                                              XmlNodeGetNextSibling
                                              ( nodeptr ),
                                              tagName );
        }
    }

    return rtElement;
}
