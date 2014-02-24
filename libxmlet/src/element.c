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
*   XmlElement_Init
*       Initializes an element node.
*       External function.
*
*=================================================================*/
void
XmlElementInit( XmlElement * element )
{
    if ( element != NULL )
    {
        memset( element, 0, sizeof( XmlElement ) );
        element->node.nodeType = XML_ELEMENT_NODE;
    }
}

/*================================================================
*   XmlElementGetTagName
*       Gets the element node's tagName
*       External function.
*
*=================================================================*/
const char *
XmlElementGetTagName( XmlElement * element )
{

    if ( element != NULL )
    {
        return element->tagName;
    }
    else
    {
        return NULL;
    }
}

/*================================================================
*   XmlElementSetTagName
*       Sets the given element's tagName.
*   Parameters:
*       tagName: new tagName for the element.
*
*=================================================================*/
int
XmlElementSetTagName(
    Pool * pool,
    XmlElement * element,
    const char *tagName )
{
    int rc = XML_OK;

    assert( ( element != NULL ) && ( tagName != NULL ) );
    if ( ( element == NULL ) || ( tagName == NULL ) )
    {
        return XML_FAILED;
    }

    if ( element->tagName != NULL )
    {
        PFREE( element->tagName );
    }

    element->tagName = PSTRDUP( tagName );
    if ( element->tagName == NULL )
    {
        rc = XML_INSUFFICIENT_MEMORY;
    }

    return rc;

}

/*=========================================================================
*   XmlElementGetAttribute
*       Retrievea an attribute value by name.
*       External function.
*   Parameters:
*       name: the name of the attribute to retrieve.
*   Return Values:
*       attribute value as a string, or the empty string if that attribute
*       does not have a specified value.
*
*========================================================================*/
char *
XmlElementGetAttribute( XmlElement * element,
                        const char *  name )
{
    XmlNode *  attrNode;

    if ( ( element == NULL ) || ( name == NULL ) )
    {
        return NULL;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->nodeName, name ) == 0 )  // found it
        {
            return attrNode->nodeValue;
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    return NULL;
}

/*======================================================================
*   XmlElementSetAttribute
*  	Adds a new attribute.  If an attribute with that name is already
*  	present in the element, its value is changed to be that of the value
*  	parameter. If not, a new attribute is inserted into the element.
*
*  	External function.
*   Parameters:
*   name: the name of the attribute to create or alter.
*  	value: value to set in string form
*   Return Values:
*  	XML_OK or failure code.
*=======================================================================*/
int
XmlElementSetAttribute(
    Pool * pool,
    XmlElement * element,
    const char *name,
    const char *value )
{
    XmlNode *  attrNode;
    XmlAttribute *newAttrNode;
    short errCode = XML_OK;

    if ( ( element == NULL ) || ( name == NULL ) || ( value == NULL ) )
    {
        errCode = XML_INVALID_PARAMETER;
        goto ErrorHandler;
    }

    if ( XmlParserIsValidXmlName( name ) == FALSE )
    {
        errCode = XML_INVALID_CHARACTER_ERR;
        goto ErrorHandler;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->nodeName, name ) == 0 )
        {
            break;              //found it
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    if ( attrNode == NULL )     // add a new attribute
    {
        errCode =
            XmlDocCreateAttributeEx(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                ( XmlDocument * ) element->node.
                ownerDocument, name,
                &newAttrNode );
        if ( errCode != XML_OK )
        {
            goto ErrorHandler;
        }

        attrNode = ( XmlNode *   ) newAttrNode;

        attrNode->nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                  pool ,
#endif
                                  value );
        if ( attrNode->nodeValue == NULL )
        {
            XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                newAttrNode );
            errCode = XML_INSUFFICIENT_MEMORY;
            goto ErrorHandler;
        }

        errCode =
            XmlElementSetAttributeNode( element, newAttrNode, NULL );
        if ( errCode != XML_OK )
        {
            XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                newAttrNode );
            goto ErrorHandler;
        }

    }
    else
    {
        if ( attrNode->nodeValue != NULL )  // attribute name has a value already
        {
            PFREE( attrNode->nodeValue );
        }

        attrNode->nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                  pool ,
#endif
                                  value );
        if ( attrNode->nodeValue == NULL )
        {
            errCode = XML_INSUFFICIENT_MEMORY;
        }
    }

ErrorHandler:
    return errCode;
}

/*================================================================
*   XmlElementRemoveAttribute
*       Removes an attribute value by name. The attribute node is
*       not removed.
*       External function.
*   Parameters:
*       name: the name of the attribute to remove.
*   Return Values:
*       XML_OK or error code.
*
*=================================================================*/
int
XmlElementRemoveAttribute(
    Pool * pool,
    XmlElement * element,
    const char *name )
{

    XmlNode *  attrNode;

    if ( ( element == NULL ) || ( name == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->nodeName, name ) == 0 )
        {
            break;              //found it
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    if ( attrNode != NULL )     // has the attribute
    {
        if ( attrNode->nodeValue != NULL )
        {
            PFREE( attrNode->nodeValue );
            attrNode->nodeValue = NULL;
        }
    }

    return XML_OK;
}

/*================================================================
*   XmlElementGetAttributeNode
*       Retrieve an attribute node by name.
*       External function.
*   Parameters:
*       name: the name(nodeName) of the attribute to retrieve.
*   Return Value:
*       The attr node with the specified name (nodeName) or NULL if
*       there is no such attribute.
*
*=================================================================*/
XmlAttribute *
XmlElementGetAttributeNode( XmlElement * element,
                            IN char *name )
{

    XmlNode *  attrNode;

    if ( ( element == NULL ) || ( name == NULL ) )
    {
        return NULL;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->nodeName, name ) == 0 )  // found it
        {
            break;
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    return ( XmlAttribute * ) attrNode;

}

/*==========================================================================
*   XmlElementSetAttributeNode
*       Adds a new attribute node.  If an attribute with that name(nodeName)
*       is already present in the element, it is replaced by the new one.
*       External function.
*   Parameters:
*       The attr node to add to the attribute list.
*   Return Value:
*       if newAttr replaces an existing attribute, the replaced
*       attr node is returned, otherwise NULL is returned.
*
*===========================================================================*/
int
XmlElementSetAttributeNode( XmlElement * element,
                            XmlAttribute * newAttr,
                            OUT XmlAttribute ** rtAttr )
{

    XmlNode *  attrNode;
    XmlNode *  node;
    XmlNode *  nextAttr = NULL;
    XmlNode *  prevAttr = NULL;
    XmlNode *  preSib;
    XmlNode *  nextSib;

    if ( ( element == NULL ) || ( newAttr == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    if ( newAttr->node.ownerDocument != element->node.ownerDocument )
    {
        return XML_WRONG_DOCUMENT_ERR;
    }

    if ( newAttr->ownerElement != NULL )
    {
        return XML_INUSE_ATTRIBUTE_ERR;
    }

    newAttr->ownerElement = element;
    node = ( XmlNode *   ) newAttr;

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->nodeName, node->nodeName ) == 0 )
        {
            break;              //found it
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    if ( attrNode != NULL )     // already present, will replace by newAttr
    {
        preSib = attrNode->prevSibling;
        nextSib = attrNode->nextSibling;

        if ( preSib != NULL )
        {
            preSib->nextSibling = node;
        }

        if ( nextSib != NULL )
        {
            nextSib->prevSibling = node;
        }

        if ( element->node.firstAttr == attrNode )
        {
            element->node.firstAttr = node;
        }

        if ( rtAttr != NULL )
        {
            *rtAttr = ( XmlAttribute * ) attrNode;
        }
    }
    else                      // add this attribute
    {
        if ( element->node.firstAttr != NULL )
        {
            prevAttr = element->node.firstAttr;
            nextAttr = prevAttr->nextSibling;
            while ( nextAttr != NULL )
            {
                prevAttr = nextAttr;
                nextAttr = prevAttr->nextSibling;
            }
            prevAttr->nextSibling = node;
            node->prevSibling = prevAttr;
        }
        else                  // this is the first attribute node
        {
            element->node.firstAttr = node;
            node->prevSibling = NULL;
            node->nextSibling = NULL;
        }

        if ( rtAttr != NULL )
        {
            *rtAttr = NULL;
        }
    }

    return XML_OK;
}

/*=======================================================================
*   XmlElementFindAttributeNode
*       Find a attribute node whose contents are the same as the oldAttr.
*       Internal only to parser.
*   Parameter:
*       oldAttr: the attribute node to match
*   Return:
*       if found it, the attribute node is returned,
*       otherwise, return NULL.
*
*========================================================================*/
XmlNode *
XmlElementFindAttributeNode( XmlElement * element,
                             XmlAttribute * oldAttr )
{
    XmlNode *  attrNode;
    XmlNode *  oldAttrNode = ( XmlNode *   ) oldAttr;

    assert( ( element != NULL ) && ( oldAttr != NULL ) );

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )  // parentNode, prevSib, nextSib and ownerDocument doesn't matter
    {
        if ( XmlNodeCompare( attrNode, oldAttrNode ) == TRUE )
        {
            break;              //found it
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    return attrNode;

}

/*==================================================================
*   XmlElementRemoveAttributeNode
*       Removes the specified attribute node.
*       External function.
*
*   Parameters:
*       oldAttr: the attr node to remove from the attribute list.
*
*   Return Value:
*       XML_OK or failure
*
*===================================================================*/
int
XmlElementRemoveAttributeNode( XmlElement * element,
                               XmlAttribute    * oldAttr,
                               OUT XmlAttribute   ** rtAttr )
{
    XmlNode * attrNode;
    XmlNode * preSib;
    XmlNode * nextSib;

    if ( ( element == NULL ) || ( oldAttr == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    attrNode = XmlElementFindAttributeNode( element, oldAttr );
    if ( attrNode != NULL )     // has the attribute
    {
        preSib = attrNode->prevSibling;
        nextSib = attrNode->nextSibling;

        if ( preSib != NULL )
        {
            preSib->nextSibling = nextSib;
        }

        if ( nextSib != NULL )
        {
            nextSib->prevSibling = preSib;
        }

        if ( element->node.firstAttr == attrNode )
        {
            element->node.firstAttr = nextSib;
        }

        attrNode->parentNode = 0;
        attrNode->prevSibling = 0 ;
        attrNode->nextSibling = 0;
        *rtAttr = ( XmlAttribute * ) attrNode;
        return XML_OK;

    }
    else
    {
        return XML_NOT_FOUND_ERR;
    }

}

/*================================================================
*   XmlElementGetElementsByTagName
*       Returns a nodeList of all descendant Elements with a given
*       tag name, in the order in which they are encountered in a preorder
*       traversal of this element tree.
*       External function.
*
*   Parameters:
*       tagName: the name of the tag to match on. The special value "*"
*       matches all tags.
*
*   Return Value:
*       a nodeList of matching element nodes.
*
*=================================================================*/
XmlNodeList *
XmlElementGetElementsByTagName(
    Pool * pool,
    XmlElement * element,
    IN char *tagName )
{
    XmlNodeList *returnNodeList = NULL;

    if ( ( element != NULL ) && ( tagName != NULL ) )
    {
        XmlNodeGetElementsByTagName(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            ( XmlNode *   ) element, tagName,
            &returnNodeList );
    }
    return returnNodeList;
}

/*================================================================
*   XmlElementGetAttributeNS
*       Retrieves an attribute value by local name and namespace URI.
*       External function.
*
*   Parameters:
*       namespaceURI: the namespace URI of the attribute to retrieve.
*       localName: the local name of the attribute to retrieve.
*
*   Return Value:
*       the attr value as a string, or NULL if that attribute does
*       not have the specified value.
*
*=================================================================*/
char *
XmlElementGetAttributeNS( XmlElement * element,
                          const char *  namespaceURI,
                          const char *  localName )
{
    XmlNode *  attrNode;

    if ( ( element == NULL ) || ( namespaceURI == NULL )
            || ( localName == NULL ) )
    {
        return NULL;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->localName, localName ) == 0 && strcmp( attrNode->namespaceURI, namespaceURI ) == 0 )     // found it
        {
            return attrNode->nodeValue;
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    return NULL;

}

/*================================================================
*   XmlElementSetAttributeNS
*       Adds a new attribute. If an attribute with the same local name
*       and namespace URI is already present on the element, its prefix
*       is changed to be the prefix part of the qualifiedName, and its
*       value is changed to be the value parameter.  This value is a
*       simple string.
*       External function.
*
*   Parameter:
*       namespaceURI: the namespace of the attribute to create or alter.
*       qualifiedName: the qualified name of the attribute to create or alter.
*       value: the value to set in string form.
*
*   Return Value:
*       XML_OK or failure
*
*=================================================================*/
int
XmlElementSetAttributeNS(
    Pool * pool,
    XmlElement * element,
    const char *  namespaceURI,
    const char *  qualifiedName,
    const char *  value )
{
    XmlNode *  attrNode = NULL;
    XmlNode newAttrNode;
    XmlAttribute *newAttr;
    int rc;

    if ( ( element == NULL ) || ( namespaceURI == NULL ) ||
            ( qualifiedName == NULL ) || ( value == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    if ( XmlParserIsValidXmlName( qualifiedName ) == FALSE )
    {
        return XML_INVALID_CHARACTER_ERR;
    }

    XmlNodeInit( &newAttrNode );

    newAttrNode.nodeName = PSTRDUP( qualifiedName );
    if ( newAttrNode.nodeName == NULL )
    {
        return XML_INSUFFICIENT_MEMORY;
    }

    rc = XmlParserSetNodePrefixAndLocalName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif

             &newAttrNode );
    if ( rc != XML_OK )
    {
        XmlParserFreeNodeContent(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            &newAttrNode );
        return rc;
    }
    // see DOM 2 spec page 59
    if ( ( newAttrNode.prefix != NULL && namespaceURI == NULL ) ||
            ( strcmp( newAttrNode.prefix, "xml" ) == 0 &&
              strcmp( namespaceURI,
                      "http://www.w3.org/XML/1998/namespace" ) != 0 )
            || ( strcmp( qualifiedName, "xmlns" ) == 0
                 && strcmp( namespaceURI,
                            "http://www.w3.org/2000/xmlns/" ) != 0 ) )
    {
        XmlParserFreeNodeContent(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            &newAttrNode );
        return XML_NAMESPACE_ERR;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->localName, newAttrNode.localName ) == 0 &&
                strcmp( attrNode->namespaceURI, namespaceURI ) == 0 )
        {
            break;              //found it
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    if ( attrNode != NULL )
    {
        if ( attrNode->prefix != NULL )
        {
            PFREE( attrNode->prefix );   // remove the old prefix
        }
        // replace it with the new prefix
        attrNode->prefix = PSTRDUP( newAttrNode.prefix );
        if ( attrNode->prefix == NULL )
        {
            XmlParserFreeNodeContent(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                &newAttrNode );
            return XML_INSUFFICIENT_MEMORY;
        }

        if ( attrNode->nodeValue != NULL )
        {
            PFREE( attrNode->nodeValue );
        }

        attrNode->nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                  pool ,
#endif
                                  value );
        if ( attrNode->nodeValue == NULL )
        {
            PFREE( attrNode->prefix );
            XmlParserFreeNodeContent(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                &newAttrNode );
            return XML_INSUFFICIENT_MEMORY;
        }

    }
    else
    {
        // add a new attribute
        rc = XmlDocCreateAttributeNSEx(
#ifdef USE_CWMP_MEMORY_POOL
                 pool ,
#endif

                 ( XmlDocument * )
                 element->node.ownerDocument,
                 namespaceURI, qualifiedName,
                 &newAttr );
        if ( rc != XML_OK )
        {
            return rc;
        }

        newAttr->node.nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                      pool ,
#endif
                                      value );
        if ( newAttr->node.nodeValue == NULL )
        {
            XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                newAttr );
            return XML_INSUFFICIENT_MEMORY;
        }

        if ( XmlElementSetAttributeNodeNS( element, newAttr, NULL ) !=
                XML_OK )
        {
            XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                newAttr );
            return XML_FAILED;
        }

    }

    XmlParserFreeNodeContent(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        &newAttrNode );
    return XML_OK;
}

/*================================================================
*   XmlElementRemoveAttributeNS
*       Removes an attribute by local name and namespace URI. The replacing
*       attribute has the same namespace URI and local name, as well as
*       the original prefix.
*       External function.
*
*   Parameters:
*       namespaceURI: the namespace URI of the attribute to remove.
*       localName: the local name of the atribute to remove.
*
*   Return Value:
*       XML_OK or failure.
*
*=================================================================*/
int
XmlElementRemoveAttributeNS(
    Pool * pool,
    XmlElement * element,
    const char *  namespaceURI,
    const char *  localName )
{
    XmlNode *  attrNode;

    if ( ( element == NULL ) || ( namespaceURI == NULL )
            || ( localName == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->localName, localName ) == 0 &&
                strcmp( attrNode->namespaceURI, namespaceURI ) == 0 )
        {
            break;              //found it
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    if ( attrNode != NULL )     // has the attribute
    {
        if ( attrNode->nodeValue != NULL )
        {
            PFREE( attrNode->nodeValue );
            attrNode->nodeValue = NULL;
        }
    }

    return XML_OK;

}

/*================================================================
*   XmlElementGetAttributeNodeNS
*       Retrieves an attr node by local name and namespace URI.
*       External function.
*
*   Parameter:
*       namespaceURI: the namespace of the attribute to retrieve.
*       localName: the local name of the attribute to retrieve.
*
*   Return Value:
*       The attr node with the specified attribute local name and
*       namespace URI or null if there is no such attribute.
*
*=================================================================*/
XmlAttribute *
XmlElementGetAttributeNodeNS( XmlElement * element,
                              const char *  namespaceURI,
                              const char *  localName )
{

    XmlNode *  attrNode;

    if ( ( element == NULL ) || ( namespaceURI == NULL )
            || ( localName == NULL ) )
    {
        return NULL;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->localName, localName ) == 0 && strcmp( attrNode->namespaceURI, namespaceURI ) == 0 )     // found it
        {
            break;
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    return ( XmlAttribute * ) attrNode;

}

/*================================================================
*   XmlElementSetAttributeNodeNS
*       Adds a new attribute. If an attribute with that local name and
*       that namespace URI is already present in the element, it is replaced
*       by the new one.
*       External function.
*
*   Parameter:
*       newAttr: the attr node to add to the attribute list.
*
*   Return Value:
*       If the newAttr attribute replaces an existing attribute with the
*       same local name and namespace, the replaced attr node is returned,
*       otherwise null is returned.
*
*=================================================================*/
int
XmlElementSetAttributeNodeNS( XmlElement * element,
                              XmlAttribute * newAttr,
                              OUT XmlAttribute ** rtAttr )
{
    XmlNode *  attrNode;
    XmlNode *  node;
    XmlNode *  prevAttr = NULL;
    XmlNode *  nextAttr = NULL;
    XmlNode *  preSib;
    XmlNode *  nextSib;

    if ( ( element == NULL ) || ( newAttr == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    if ( newAttr->node.ownerDocument != element->node.ownerDocument )
    {
        return XML_WRONG_DOCUMENT_ERR;
    }

    if ( ( newAttr->ownerElement != NULL )
            && ( newAttr->ownerElement != element ) )
    {
        return XML_INUSE_ATTRIBUTE_ERR;
    }

    newAttr->ownerElement = element;
    node = ( XmlNode *   ) newAttr;

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->localName, node->localName ) == 0 &&
                strcmp( attrNode->namespaceURI, node->namespaceURI ) == 0 )
        {
            break;              //found it
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    if ( attrNode != NULL )     // already present, will replace by newAttr
    {
        preSib = attrNode->prevSibling;
        nextSib = attrNode->nextSibling;

        if ( preSib != NULL )
        {
            preSib->nextSibling = node;
        }

        if ( nextSib != NULL )
        {
            nextSib->prevSibling = node;
        }

        if ( element->node.firstAttr == attrNode )
        {
            element->node.firstAttr = node;
        }

        *rtAttr = ( XmlAttribute * ) attrNode;

    }
    else                      // add this attribute
    {
        if ( element->node.firstAttr != NULL ) // element has attribute already
        {
            prevAttr = element->node.firstAttr;
            nextAttr = prevAttr->nextSibling;
            while ( nextAttr != NULL )
            {
                prevAttr = nextAttr;
                nextAttr = prevAttr->nextSibling;
            }
            prevAttr->nextSibling = node;
        }
        else                  // this is the first attribute node
        {
            element->node.firstAttr = node;
            node->prevSibling = NULL;
            node->nextSibling = NULL;
        }

        if ( rtAttr != NULL )
        {
            *rtAttr = NULL;
        }
    }

    return XML_OK;
}

/*================================================================
*   XmlElementGetElementsByTagNameNS
*       Returns a nodeList of all the descendant Elements with a given
*       local name and namespace in the order in which they are encountered
*       in a preorder traversal of the element tree.
*       External function.
*
*   Parameters:
*       namespaceURI: the namespace URI of the elements to match on. The
*               special value "*" matches all namespaces.
*       localName: the local name of the elements to match on. The special
*               value "*" matches all local names.
*
*   Return Value:
*       A new nodeList object containing all the matched Elements.
*
*=================================================================*/
XmlNodeList *
XmlElementGetElementsByTagNameNS(
    Pool * pool,
    XmlElement * element,
    const char *  namespaceURI,
    const char *  localName )
{
    XmlNode *  node = ( XmlNode *   ) element;
    XmlNodeList *nodeList = NULL;

    if ( ( element != NULL ) && ( namespaceURI != NULL )
            && ( localName != NULL ) )
    {
        XmlNodeGetElementsByTagNameNS(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            node, namespaceURI, localName,
            &nodeList );
    }

    return nodeList;
}

/*================================================================
*   XmlElementHasAttribute
*       Returns true when an attribute with a given name is specified on
*       this element, false otherwise.
*       External function.
*
*   Parameters:
*       name: the name of the attribute to look for.
*
*   Return Value:
*       ture if an attribute with the given name is specified on this
*       element, false otherwise.
*
*=================================================================*/
BOOL
XmlElementHasAttribute( XmlElement * element,
                        const char *  name )
{

    XmlNode *  attrNode;

    if ( ( element == NULL ) || ( name == NULL ) )
    {
        return FALSE;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->nodeName, name ) == 0 )
        {
            return TRUE;
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    return FALSE;
}

/*================================================================
*   XmlElementHasAttributeNS
*       Returns true when attribute with a given local name and namespace
*       URI is specified on this element, false otherwise.
*       External function.
*
*   Parameters:
*       namespaceURI: the namespace URI of the attribute to look for.
*       localName: the local name of the attribute to look for.
*
*   Return Value:
*       true if an attribute with the given local name and namespace URI
*       is specified, false otherwise.
*
*=================================================================*/
BOOL
XmlElementHasAttributeNS( XmlElement * element,
                          const char *  namespaceURI,
                          const char *  localName )
{

    XmlNode *  attrNode;

    if ( ( element == NULL ) || ( namespaceURI == NULL )
            || ( localName == NULL ) )
    {
        return FALSE;
    }

    attrNode = element->node.firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->localName, localName ) == 0 &&
                strcmp( attrNode->namespaceURI, namespaceURI ) == 0 )
        {
            return TRUE;
        }
        else
        {
            attrNode = attrNode->nextSibling;
        }
    }

    return FALSE;
}


/*================================================================
*   XmlElementFree
*       XFREEs a element node.
*       External function.
*
*=================================================================*/
void
XmlElementFree(
    Pool * pool,
    XmlElement * element )
{
    if ( element != NULL )
    {
        XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            ( XmlNode *   ) element );
    }
}
