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
*   XmlNodeInit
*       Intializes a node.
*       External function.
*
*=================================================================*/
void
XmlNodeInit( XmlNode *   nodeptr )
{
    assert( nodeptr != NULL );
    memset( nodeptr, 0, sizeof( XmlNode ) );

}

/*================================================================
*   XmlCDATASectionInit
*       Initializes a CDATASection node.
*       External function.
*
*=================================================================*/
void
XmlCDATASectionInit( XmlCDATASection * nodeptr )
{
    memset( nodeptr, 0, sizeof( XmlCDATASection ) );
}

/*================================================================
*   XmlCDATASectionFree
*       XFREEs a CDATASection node.
*       External function.
*
*=================================================================*/
void
XmlCDATASectionFree(
    Pool * pool,
    XmlCDATASection * nodeptr )
{
    if ( nodeptr != NULL )
    {
        XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            ( XmlNode *   ) nodeptr );
    }
}

/*================================================================
*   XmlNodeFreeSingleNode
*       XFREEs a node content.
*       Internal to parser only.
*
*=================================================================*/
void
XmlNodeFreeSingleNode(
    Pool * pool,
    XmlNode *   nodeptr )
{
    XmlElement *element = NULL;

    if ( nodeptr != NULL )
    {

        if ( nodeptr->nodeName != NULL )
        {
            PFREE( nodeptr->nodeName );
        }

        if ( nodeptr->nodeValue != NULL )
        {
            PFREE( nodeptr->nodeValue );
        }

        if ( nodeptr->namespaceURI != NULL )
        {
            PFREE( nodeptr->namespaceURI );
        }

        if ( nodeptr->prefix != NULL )
        {
            PFREE( nodeptr->prefix );
        }

        if ( nodeptr->localName != NULL )
        {
            PFREE( nodeptr->localName );
        }

        if ( nodeptr->nodeType == XML_ELEMENT_NODE )
        {
            element = ( XmlElement * ) nodeptr;
            PFREE( element->tagName );
        }

        PFREE( nodeptr );

    }
}

/*================================================================
*   XmlNodeFree
*       Frees all nodes under nodeptr subtree.
*       External function.
*
*=================================================================*/
void
XmlNodeFree(


    Pool * pool,
    XmlNode *   nodeptr )
{
    if ( nodeptr != NULL )
    {
        XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            nodeptr->firstChild );
        XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            nodeptr->nextSibling );
        XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            nodeptr->firstAttr );

        XmlNodeFreeSingleNode(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            nodeptr );
    }
}

/*================================================================
*   XmlNodeGetNodeName
*       Returns the nodename(the qualified name)
*       External function.
*
*=================================================================*/
const char *
XmlNodeGetNodeName( XmlNode *   nodeptr )
{

    if ( nodeptr != NULL )
    {
        return ( nodeptr->nodeName );
    }

    return NULL;
}

/*================================================================
*   XmlNodeGetLocalName
*       Returns the node local name
*       External function.
*
*=================================================================*/
const char *
XmlNodeGetLocalName( XmlNode *   nodeptr )
{

    if ( nodeptr != NULL )
    {
        return ( nodeptr->localName );
    }

    return NULL;
}

/*================================================================
*   XmlNodeSetNamespaceURI
*       sets the namespace URI of the node.
*       Internal function.
*	Return:
*       XML_OK or failure
*
*=================================================================*/
int
XmlNodeSetNamespaceURI(
    Pool * pool,
    XmlNode *   nodeptr,
    IN char *namespaceURI )
{

    if ( nodeptr == NULL )
    {
        return XML_INVALID_PARAMETER;
    }

    if ( nodeptr->namespaceURI != NULL )
    {
        PFREE( nodeptr->namespaceURI );
        nodeptr->namespaceURI = NULL;
    }

    if ( namespaceURI != NULL )
    {
        nodeptr->namespaceURI = PSTRDUP( namespaceURI );
        if ( nodeptr->namespaceURI == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
    }

    return XML_OK;
}

/*================================================================
*   XmlNodeSetPrefix
*       set the prefix of the node.
*       Internal to parser only.
*	Returns:
*       XML_OK or failure.
*
*=================================================================*/
int
XmlNodeSetPrefix(
    Pool * pool,
    XmlNode *   nodeptr,
    IN char *prefix )
{

    if ( nodeptr == NULL )
    {
        return XML_INVALID_PARAMETER;
    }

    if ( nodeptr->prefix != NULL )
    {
        PFREE( nodeptr->prefix );
        nodeptr->prefix = NULL;
    }

    if ( prefix != NULL )
    {
        nodeptr->prefix = PSTRDUP( prefix );
        if ( nodeptr->prefix == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
    }

    return XML_OK;

}

/*================================================================
*   XmlNodeSetLocalName
*	    set the localName of the node.
*       Internal to parser only.
*	Returns:
*       XML_OK or failure.
*
*=================================================================*/
int
XmlNodeSetLocalName(
    Pool * pool,
    XmlNode *   nodeptr,
    IN char *localName )
{

    assert( nodeptr != NULL );

    if ( nodeptr->localName != NULL )
    {
        PFREE( nodeptr->localName );
        nodeptr->localName = NULL;
    }

    if ( localName != NULL )
    {
        nodeptr->localName = PSTRDUP( localName );
        if ( nodeptr->localName == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
    }

    return XML_OK;
}

/*================================================================
*   XmlNodeGetNodeNamespaceURI
*       Returns the node namespaceURI
*       External function.
*   Returns:
*       the namespaceURI of the node
*
*=================================================================*/
const char *
XmlNodeGetNamespaceURI( XmlNode *   nodeptr )
{
    char *  retNamespaceURI = NULL;

    if ( nodeptr != NULL )
    {
        retNamespaceURI = nodeptr->namespaceURI;
    }

    return retNamespaceURI;
}

/*================================================================
*   XmlNodeGetPrefix
*       Returns the node prefix
*       External function.
*   Returns:
*       the prefix of the node.
*
*=================================================================*/
char *
XmlNodeGetPrefix( XmlNode *   nodeptr )
{
    char *  prefix = NULL;

    if ( nodeptr != NULL )
    {
        prefix = nodeptr->prefix;
    }

    return prefix;

}

/*================================================================
*   XmlNodeGetNodeValue
*       Returns the nodeValue of this node
*       External function.
*   Return:
*       the nodeValue of the node.
*
*=================================================================*/
char *
XmlNodeGetNodeValue( XmlNode *   nodeptr )
{

    if ( nodeptr != NULL )
    {
        return ( nodeptr->nodeValue );
    }

    return NULL;
}

/*================================================================
*   XmlNodeSetNodeValue
*       Sets the nodeValue
*       Internal function.
*   Returns:
*       XML_OK or failure
*
*=================================================================*/
int
XmlNodeSetNodeValue(
    Pool * pool,
    XmlNode *   nodeptr,
    const char *newNodeValue )
{
    int rc = XML_OK;

    if ( nodeptr == NULL )
    {
        return XML_INVALID_PARAMETER;
    }

    if ( nodeptr->nodeValue != NULL )
    {
        PFREE( nodeptr->nodeValue );
        nodeptr->nodeValue = NULL;
    }

    if ( newNodeValue != NULL )
    {
        nodeptr->nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                 pool ,
#endif
                                 newNodeValue );
        if ( nodeptr->nodeValue == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
    }

    return rc;
}

/*================================================================
*   XmlNodeGetNodeType
*       Gets the NodeType of this node
*       External function.
*
*=================================================================*/
int
XmlNodeGetNodeType( XmlNode *   nodeptr )
{
    if ( nodeptr != NULL )
    {
        return ( nodeptr->nodeType );
    }
    else
    {
        return ( XML_INVALID_NODE );
    }
}

/*================================================================
*   XmlNodeGetParentNode
*       Get the parent node
*       External function.
*   Return:
*
*=================================================================*/
XmlNode *
XmlNodeGetParentNode( XmlNode *   nodeptr )
{

    if ( nodeptr != NULL )
    {
        return nodeptr->parentNode;
    }
    else
    {
        return NULL;
    }
}

/*================================================================
*   XmlNodeGetFirstChild
*       Returns the first child of nodeptr.
*       External function.
*
*=================================================================*/
XmlNode *
XmlNodeGetFirstChild( XmlNode *   nodeptr )
{
    if ( nodeptr != NULL )
    {
        return nodeptr->firstChild;
    }
    else
    {
        return NULL;
    }
}

/*================================================================
*   XmlNodeGetLastChild
*       Returns the last child of nodeptr.
*       External function.
*
*=================================================================*/
XmlNode *
XmlNodeGetLastChild( XmlNode *   nodeptr )
{
    XmlNode * prev;
    XmlNode * next;

    if ( nodeptr != NULL )
    {
        prev = nodeptr;
        next = nodeptr->firstChild;

        while ( next != NULL )
        {
            prev = next;
            next = next->nextSibling;
        }
        return prev;
    }
    else
    {
        return NULL;
    }

}

/*================================================================
*   XmlNodeGetPreviousSibling
*       returns the previous sibling node of nodeptr.
*       External function.
*
*=================================================================*/
XmlNode *
XmlNodeGetPreviousSibling( XmlNode *   nodeptr )
{
    if ( nodeptr != NULL )
    {
        return nodeptr->prevSibling;
    }
    else
    {
        return NULL;
    }
}

/*================================================================
*   XmlNodeGetNextSibling
*       Returns the next sibling node.
*       External function.
*
*=================================================================*/
XmlNode *
XmlNodeGetNextSibling( XmlNode *   nodeptr )
{

    if ( nodeptr != NULL )
    {
        return nodeptr->nextSibling;
    }
    else
    {
        return NULL;
    }

}

/*================================================================
*   XmlNodeGetOwnerDocument
*       Returns the owner document node.
*       External function.
*
*=================================================================*/
XmlDocument *
XmlNodeGetOwnerDocument( XmlNode *   nodeptr )
{
    if ( nodeptr != NULL )
    {
        return ( XmlDocument * ) nodeptr->ownerDocument;
    }
    else
    {
        return NULL;
    }
}

/*================================================================
*   XmlNodeIsAncestor
*       check if ancestorNode is ancestor of toFind
*       Internal to parser only.
*   Returns:
*       TRUE or FALSE
*
*=================================================================*/
BOOL
XmlNodeIsAncestor( XmlNode *   ancestorNode,
                   XmlNode *   toFind )
{

    BOOL found = FALSE;

    if ( ( ancestorNode != NULL ) && ( toFind != NULL ) )
    {
        if ( toFind->parentNode == ancestorNode )
        {
            return TRUE;
        }
        else
        {
            found =
                XmlNodeIsAncestor( ancestorNode->firstChild, toFind );
            if ( found == FALSE )
            {
                found =
                    XmlNodeIsAncestor( ancestorNode->nextSibling,
                                       toFind );
            }
        }
    }

    return found;
}

/*================================================================
*   XmlNodeIsParent
*       Check whether toFind is a children of nodeptr.
*       Internal to parser only.
*   Return:
*       TRUE or FALSE
*
*=================================================================*/
BOOL
XmlNodeIsParent( XmlNode *   nodeptr,
                 XmlNode *   toFind )
{
    BOOL found = FALSE;

    assert( nodeptr != NULL && toFind != NULL );
    if ( toFind->parentNode == nodeptr )
    {
        found = TRUE;
    }

    return found;
}

/*================================================================
*   XmlNodeAllowChildren
*       Check to see whether nodeptr allows children of type newChild.
*       Internal to parser only.
*   Returns:
*       TRUE, if nodeptr can have newChild as children
*       FALSE,if nodeptr cannot have newChild as children
*
*=================================================================*/
BOOL
XmlNodeAllowChildren( XmlNode *   nodeptr,
                      XmlNode *   newChild )
{

    assert( nodeptr != NULL && newChild != NULL );
    switch ( nodeptr->nodeType )
    {
    case XML_ATTRIBUTE_NODE:
    case XML_TEXT_NODE:
    case XML_CDATA_SECTION_NODE:
        return FALSE;
        break;

    case XML_ELEMENT_NODE:
        if ( ( newChild->nodeType == XML_ATTRIBUTE_NODE ) ||
                ( newChild->nodeType == XML_DOCUMENT_NODE ) )
        {
            return FALSE;
        }
        break;

    case XML_DOCUMENT_NODE:
        if ( newChild->nodeType != XML_ELEMENT_NODE )
        {
            return FALSE;
        }

    default:
        break;
    }

    return TRUE;
}

/*================================================================
*   XmlNodeCompare
*       Compare two nodes to see whether they are the same node.
*       Parent, sibling and children node are ignored.
*       Internal to parser only.
*   Returns:
*       TRUE, the two nodes are the same.
*       FALSE, the two nodes are not the same.
*
*=================================================================*/
BOOL
XmlNodeCompare( XmlNode *   srcNode,
                XmlNode *   destNode )
{
    assert( srcNode != NULL && destNode != NULL );
    if ( ( srcNode == destNode ) ||
            ( strcmp( srcNode->nodeName, destNode->nodeName ) == 0 &&
              strcmp( srcNode->nodeValue, destNode->nodeValue ) == 0 &&
              ( srcNode->nodeType == destNode->nodeType ) &&
              strcmp( srcNode->namespaceURI, destNode->namespaceURI ) == 0 &&
              strcmp( srcNode->prefix, destNode->prefix ) == 0 &&
              strcmp( srcNode->localName, destNode->localName ) == 0 ) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*================================================================
*   XmlNodeInsertBefore
*       Inserts the node newChild before the existing child node refChild.
*       If refChild is null, insert newChild at the end of the list of
*       children. If the newChild is already in the tree, it is first
*       removed.
*       External function.
*   Parameters:
*       newChild: the node to insert.
*   Returns:
*
*=================================================================*/
int
XmlNodeInsertBefore( XmlNode *   nodeptr,
                     XmlNode *   newChild,
                     XmlNode *   refChild )
{

    int ret = XML_OK;

    if ( ( nodeptr == NULL ) || ( newChild == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }
    // whether nodeptr allow children of the type of newChild
    if ( XmlNodeAllowChildren( nodeptr, newChild ) == FALSE )
    {
        return XML_HIERARCHY_REQUEST_ERR;
    }
    // or if newChild is one of nodeptr's ancestors
    if ( XmlNodeIsAncestor( newChild, nodeptr ) == TRUE )
    {
        return XML_HIERARCHY_REQUEST_ERR;
    }
    // if newChild was created from a different document
    if ( nodeptr->ownerDocument != newChild->ownerDocument )
    {
        return XML_WRONG_DOCUMENT_ERR;
    }
    // if refChild is not a child of nodeptr
    if ( XmlNodeIsParent( nodeptr, refChild ) == FALSE )
    {
        return XML_NOT_FOUND_ERR;
    }

    if ( refChild != NULL )
    {
        if ( XmlNodeIsParent( nodeptr, newChild ) == TRUE )
        {
            XmlNodeRemoveChild( nodeptr, newChild, NULL );
            newChild->nextSibling = NULL;
            newChild->prevSibling = NULL;
        }

        newChild->nextSibling = refChild;
        if ( refChild->prevSibling != NULL )
        {
            ( refChild->prevSibling )->nextSibling = newChild;
            newChild->prevSibling = refChild->prevSibling;
        }

        refChild->prevSibling = newChild;

        if ( newChild->prevSibling == NULL )
        {
            nodeptr->firstChild = newChild;
        }

        newChild->parentNode = nodeptr;

    }
    else
    {
        ret = XmlNodeAppendChild( nodeptr, newChild );
    }

    return ret;
}

/*================================================================
*   XmlNodeReplaceChild
*       Replaces the child node oldChild with newChild in the list of children,
*       and returns the oldChild node.
*       External function.
*   Parameters:
*       newChild:   the new node to put in the child list.
*       oldChild:   the node being replaced in the list.
*       returnNode: the node replaced.
*   Return Value:
*       XML_OK
*       XML_INVALID_PARAMETER:     if anyone of nodeptr, newChild or oldChild is NULL.
*       XML_HIERARCHY_REQUEST_ERR: if the newChild is ancestor of nodeptr or nodeptr
*                                   is of a type that does not allow children of the
*                                   type of the newChild node.
*       XML_WRONG_DOCUMENT_ERR:    if newChild was created from a different document than
*                                   the one that created this node.
*       XML_NOT_FOUND_ERR:         if oldChild is not a child of nodeptr.
*
*=================================================================*/
int
XmlNodeReplaceChild( XmlNode *   nodeptr,
                     XmlNode *   newChild,
                     XmlNode *   oldChild,
                     OUT XmlNode ** returnNode )
{
    int ret = XML_OK;

    if ( ( nodeptr == NULL ) || ( newChild == NULL )
            || ( oldChild == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }
    // if nodetype of nodeptr does not allow children of the type of newChild
    // needs to add later

    // or if newChild is one of nodeptr's ancestors
    if ( XmlNodeIsAncestor( newChild, nodeptr ) == TRUE )
    {
        return XML_HIERARCHY_REQUEST_ERR;
    }

    if ( XmlNodeAllowChildren( nodeptr, newChild ) == FALSE )
    {
        return XML_HIERARCHY_REQUEST_ERR;
    }
    // if newChild was created from a different document
    if ( nodeptr->ownerDocument != newChild->ownerDocument )
    {
        return XML_WRONG_DOCUMENT_ERR;
    }
    // if refChild is not a child of nodeptr
    if ( XmlNodeIsParent( nodeptr, oldChild ) != TRUE )
    {
        return XML_NOT_FOUND_ERR;
    }

    ret = XmlNodeInsertBefore( nodeptr, newChild, oldChild );
    if ( ret != XML_OK )
    {
        return ret;
    }

    ret = XmlNodeRemoveChild( nodeptr, oldChild, returnNode );
    return ret;
}

/*================================================================
*   XmlNodeRemoveChild
*       Removes the child node indicated by oldChild from the list of
*       children, and returns it.
*       External function.
*   Parameters:
*       oldChild: the node being removed.
*       returnNode: the node removed.
*   Return Value:
*       XML_OK
*       XML_NOT_FOUND_ERR: if oldChild is not a child of this node.
*       XML_INVALID_PARAMETER: if either oldChild or nodeptr is NULL
*
*=================================================================*/
int
XmlNodeRemoveChild( XmlNode *   nodeptr,
                    XmlNode *   oldChild,
                    OUT XmlNode ** returnNode )
{

    if ( ( nodeptr == NULL ) || ( oldChild == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }

    if ( XmlNodeIsParent( nodeptr, oldChild ) == FALSE )
    {
        return XML_NOT_FOUND_ERR;
    }

    if ( oldChild->prevSibling != NULL )
    {
        ( oldChild->prevSibling )->nextSibling = oldChild->nextSibling;
    }

    if ( nodeptr->firstChild == oldChild )
    {
        nodeptr->firstChild = oldChild->nextSibling;
    }

    if ( oldChild->nextSibling != NULL )
    {
        ( oldChild->nextSibling )->prevSibling = oldChild->prevSibling;
    }

    oldChild->nextSibling = NULL;
    oldChild->prevSibling = NULL;
    oldChild->parentNode = NULL;

    if ( returnNode != NULL )
    {
        *returnNode = oldChild;
    }
    return XML_OK;
}

/*========================================================================================
*   XmlNodeAppendChild
*       Adds the node newChild to the end of the list of children of this node.
*       If the newChild is already in the tree, it is first removed.
*       External function.
*   Parameter:
*       newChild: the node to add.
*   Return Value:
*       XML_OK
*       XML_INVALID_PARAMETER:     if either nodeptr or newChild is NULL
*       XML_WRONG_DOCUMENT_ERR:    if newChild was created from a different document than
*                                   the one that created nodeptr.
*       XML_HIERARCHY_REQUEST_ERR: if newChild is ancestor of nodeptr or if nodeptr is of
*                                   a type that does not allow children of the type of the
*                                   newChild node.
*
*========================================================================================*/
int
XmlNodeAppendChild( XmlNode *   nodeptr,
                    XmlNode *   newChild )
{

    XmlNode *  prev = NULL;
    XmlNode *  next = NULL;


    if ( ( nodeptr == NULL ) || ( newChild == NULL ) )
    {
        return XML_INVALID_PARAMETER;
    }
    // if newChild was created from a different document
    if ( ( newChild->ownerDocument != NULL ) &&
            ( nodeptr->ownerDocument != newChild->ownerDocument ) )
    {
        return XML_WRONG_DOCUMENT_ERR;
    }
    // if newChild is an ancestor of nodeptr
    if ( XmlNodeIsAncestor( newChild, nodeptr ) == TRUE )
    {
        return XML_HIERARCHY_REQUEST_ERR;
    }
    // if nodeptr does not allow to have newChild as children
    if ( XmlNodeAllowChildren( nodeptr, newChild ) == FALSE )
    {
        return XML_HIERARCHY_REQUEST_ERR;
    }

    if ( XmlNodeIsParent( nodeptr, newChild ) == TRUE )
    {
        XmlNodeRemoveChild( nodeptr, newChild, NULL );
    }
    // set the parent node pointer
    newChild->parentNode = nodeptr;
    newChild->ownerDocument = nodeptr->ownerDocument;

    //if the first child
    if ( nodeptr->firstChild == NULL )
    {
        nodeptr->firstChild = newChild;
    }
    else
    {
        prev = nodeptr->firstChild;
        next = prev->nextSibling;
        while ( next != NULL )
        {
            prev = next;
            next = prev->nextSibling;
        }
        prev->nextSibling = newChild;
        newChild->prevSibling = prev;
    }


    return XML_OK;
}

/*================================================================
*   XmlNodeCloneTextNode
*       Returns a clone of nodeptr
*       Internal to parser only.
*
*=================================================================*/
XmlNode *
#ifdef USE_CWMP_MEMORY_POOL
XmlNodeCloneTextNode(Pool * pool, XmlNode *   nodeptr )
#else
XmlNodeCloneTextNode( XmlNode *   nodeptr )
#endif

{
    XmlNode *  newNode = NULL;

    assert( nodeptr != NULL );

    newNode = ( XmlNode *   ) PMALLOC( sizeof( XmlNode ) );
    if ( newNode == NULL )
    {
        return NULL;
    }
    else
    {
        XmlNodeInit( newNode );

        XmlNodeSetNodeName(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            newNode, nodeptr->nodeName );
        XmlNodeSetNodeValue(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newNode, nodeptr->nodeValue );
        newNode->nodeType = XML_TEXT_NODE;
    }

    return newNode;
}

/*================================================================
*   XmlNodeCloneCDATASect
*       Return a clone of CDATASection node.
*       Internal to parser only.
*
*=================================================================*/
XmlCDATASection *
#ifdef USE_CWMP_MEMORY_POOL
XmlNodeCloneCDATASect(Pool * pool, XmlCDATASection * nodeptr )
#else
XmlNodeCloneCDATASect( XmlCDATASection * nodeptr )
#endif

{
    XmlCDATASection *newCDATA = NULL;
    XmlNode *  newNode;
    XmlNode *  srcNode;

    assert( nodeptr != NULL );
    newCDATA =
        ( XmlCDATASection * ) PMALLOC( sizeof( XmlCDATASection ) );
    if ( newCDATA != NULL )
    {
        newNode = ( XmlNode *   ) newCDATA;
        XmlNodeInit( newNode );

        srcNode = ( XmlNode *   ) nodeptr;
        XmlNodeSetNodeName(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newNode, srcNode->nodeName );
        XmlNodeSetNodeValue(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newNode, srcNode->nodeValue );
        newNode->nodeType = XML_CDATA_SECTION_NODE;
    }

    return newCDATA;
}

/*================================================================
*   XmlNodeCloneElement
*       returns a clone of element node
*       Internal to parser only.
*
*=================================================================*/
XmlElement *
#ifdef USE_CWMP_MEMORY_POOL
XmlNodeCloneElement(Pool * pool, XmlElement * nodeptr )
#else
XmlNodeCloneElement( XmlElement * nodeptr )
#endif

{
    XmlElement *newElement;
    XmlNode *  elementNode;
    XmlNode *  srcNode;
    int rc;

    assert( nodeptr != NULL );

    newElement = ( XmlElement * ) PMALLOC( sizeof( XmlElement ) );
    if ( newElement == NULL )
    {
        return NULL;
    }

    XmlElementInit( newElement );
    rc = XmlElementSetTagName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             newElement, nodeptr->tagName );
    if ( rc != XML_OK )
    {
        goto ErrorHandle;
    }

    elementNode = ( XmlNode *   ) newElement;
    srcNode = ( XmlNode *   ) nodeptr;
    rc = XmlNodeSetNodeName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             elementNode, srcNode->nodeName );
    if ( rc != XML_OK )
    {
        goto ErrorHandle;
    }

    rc = XmlNodeSetNodeValue(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             elementNode, srcNode->nodeValue );
    if ( rc != XML_OK )
    {
        goto ErrorHandle;
    }

    rc = XmlNodeSetNamespaceURI(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             elementNode, srcNode->namespaceURI );
    if ( rc != XML_OK )
    {
        goto ErrorHandle;
    }

    rc = XmlNodeSetPrefix(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             elementNode, srcNode->prefix );
    if ( rc != XML_OK )
    {
        goto ErrorHandle;
    }

    rc = XmlNodeSetLocalName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             elementNode, srcNode->localName );
    if ( rc != XML_OK )
    {
        goto ErrorHandle;
    }

    elementNode->nodeType = XML_ELEMENT_NODE;

    return newElement;

ErrorHandle:
    XmlElementFree(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        newElement );

    return NULL;

}

/*================================================================
*   XmlNodeCloneDoc
*       Returns a clone of document node
*       Internal to parser only.
*
*=================================================================*/
XmlDocument *
#ifdef USE_CWMP_MEMORY_POOL
XmlNodeCloneDoc(Pool * pool, XmlDocument * nodeptr)
#else
XmlNodeCloneDoc(XmlDocument * nodeptr)
#endif
{
    XmlDocument *newDoc;
    XmlNode *  docNode;
    int rc;

    assert( nodeptr != NULL );
    newDoc = ( XmlDocument * ) PMALLOC( sizeof( XmlDocument ) );
    if ( newDoc == NULL )
    {
        return NULL;
    }

    XmlDocumentInit( newDoc );
    docNode = ( XmlNode *   ) newDoc;

    rc = XmlNodeSetNodeName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif

             docNode, DOCUMENTNODENAME );
    if ( rc != XML_OK )
    {
        XmlDocumentFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newDoc );
        return NULL;
    }

    newDoc->node.nodeType = XML_DOCUMENT_NODE;

    return newDoc;

}

/*================================================================
*   XmlNodeCloneAttr
*       Returns a clone of attribute node
*       Internal to parser only
*
*=================================================================*/
XmlAttribute *
#ifdef USE_CWMP_MEMORY_POOL
XmlNodeCloneAttr(Pool * pool, XmlAttribute * nodeptr )
#else
XmlNodeCloneAttr( XmlAttribute * nodeptr )
#endif
{
    XmlAttribute *newAttr;
    XmlNode *  attrNode;
    XmlNode *  srcNode;
    int rc;

    assert( nodeptr != NULL );
    newAttr = ( XmlAttribute * ) PMALLOC( sizeof( XmlAttribute ) );
    if ( newAttr == NULL )
    {
        return NULL;
    }

    XmlAttrInit( newAttr );
    attrNode = ( XmlNode *   ) newAttr;
    srcNode = ( XmlNode *   ) nodeptr;

    rc = XmlNodeSetNodeName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             attrNode, srcNode->nodeName );
    if ( rc != XML_OK )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            newAttr );
        return NULL;
    }

    rc = XmlNodeSetNodeValue(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             attrNode, srcNode->nodeValue );
    if ( rc != XML_OK )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newAttr );
        return NULL;
    }
    //check to see whether we need to split prefix and localname for attribute
    rc = XmlNodeSetNamespaceURI(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             attrNode, srcNode->namespaceURI );
    if ( rc != XML_OK )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newAttr );
        return NULL;
    }

    rc = XmlNodeSetPrefix(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             attrNode, srcNode->prefix );
    if ( rc != XML_OK )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newAttr );
        return NULL;
    }

    rc = XmlNodeSetLocalName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             attrNode, srcNode->localName );
    if ( rc != XML_OK )
    {
        XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newAttr );
        return NULL;
    }

    attrNode->nodeType = XML_ATTRIBUTE_NODE;

    return newAttr;
}

/*================================================================
*   XmlNodeCloneAttrDirect
*       Return a clone of attribute node, with specified field set
*       to TRUE.
*
*=================================================================*/
XmlAttribute *
XmlNodeCloneAttrDirect(
    Pool * pool,

    XmlAttribute * nodeptr )
{

    XmlAttribute *newAttr;

    assert( nodeptr != NULL );

    newAttr = XmlNodeCloneAttr(
#ifdef USE_CWMP_MEMORY_POOL
                  pool ,
#endif

                  nodeptr );
    if ( newAttr != NULL )
    {
        newAttr->specified = TRUE;
    }

    return newAttr;
}

void
XmlNodeSetSiblingNodesParent( XmlNode *   nodeptr )
{
    XmlNode *  parentNode = nodeptr->parentNode;
    XmlNode *  nextptr = nodeptr->nextSibling;

    while ( nextptr != NULL )
    {
        nextptr->parentNode = parentNode;
        nextptr = nextptr->nextSibling;
    }
}

/*================================================================
*   XmlNodeCloneNodeTreeRecursive
*       recursive functions that clones node tree of nodeptr.
*       Internal to parser only.
*
*=================================================================*/
XmlNode *
XmlNodeCloneNodeTreeRecursive(
    Pool * pool,

    XmlNode *   nodeptr,
    IN BOOL deep )
{
    XmlNode *  newNode = NULL;
    XmlElement *newElement;
    XmlAttribute *newAttr = NULL;
    XmlCDATASection *newCDATA = NULL;
    XmlDocument *newDoc;
    XmlNode *  nextSib;

    if ( nodeptr != NULL )
    {
        switch ( nodeptr->nodeType )
        {
        case XML_ELEMENT_NODE:
            newElement =
                XmlNodeCloneElement(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    ( XmlElement * ) nodeptr );
            newElement->node.firstAttr =
                XmlNodeCloneNodeTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    nodeptr->firstAttr,
                    deep );
            if ( deep )
            {
                newElement->node.firstChild =
                    XmlNodeCloneNodeTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                        pool ,
#endif
                        nodeptr->
                        firstChild,
                        deep );
                if ( newElement->node.firstChild != NULL )
                {
                    ( newElement->node.firstChild )->parentNode =
                        ( XmlNode *   ) newElement;
                    XmlNodeSetSiblingNodesParent( newElement->node.
                                                  firstChild );
                }
                nextSib =
                    XmlNodeCloneNodeTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                        pool ,
#endif

                        nodeptr->
                        nextSibling,
                        deep );
                newElement->node.nextSibling = nextSib;
                if ( nextSib != NULL )
                {
                    nextSib->prevSibling = ( XmlNode *   ) newElement;
                }
            }

            newNode = ( XmlNode *   ) newElement;
            break;

        case XML_ATTRIBUTE_NODE:
            newAttr = XmlNodeCloneAttr(
#ifdef USE_CWMP_MEMORY_POOL
                          pool ,
#endif

                          ( XmlAttribute * ) nodeptr );
            nextSib =
                XmlNodeCloneNodeTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    nodeptr->nextSibling,
                    deep );
            newAttr->node.nextSibling = nextSib;

            if ( nextSib != NULL )
            {
                nextSib->prevSibling = ( XmlNode *   ) newAttr;
            }
            newNode = ( XmlNode *   ) newAttr;
            break;

        case XML_TEXT_NODE:
            newNode = XmlNodeCloneTextNode(
#ifdef USE_CWMP_MEMORY_POOL
                          pool ,
#endif

                          nodeptr );
            break;

        case XML_CDATA_SECTION_NODE:
            newCDATA =
                XmlNodeCloneCDATASect(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    ( XmlCDATASection * )
                    nodeptr );
            newNode = ( XmlNode *   ) newCDATA;
            break;

        case XML_DOCUMENT_NODE:
            newDoc = XmlNodeCloneDoc(
#ifdef USE_CWMP_MEMORY_POOL
                         pool ,
#endif

                         ( XmlDocument * ) nodeptr );
            newNode = ( XmlNode *   ) newDoc;
            if ( deep )
            {
                newNode->firstChild =
                    XmlNodeCloneNodeTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                        pool ,
#endif

                        nodeptr->
                        firstChild,
                        deep );
                if ( newNode->firstChild != NULL )
                {
                    newNode->firstChild->parentNode = newNode;
                }
            }

            break;

        case XML_INVALID_NODE:
        case XML_ENTITY_REFERENCE_NODE:
        case XML_ENTITY_NODE:
        case XML_PROCESSING_INSTRUCTION_NODE:
        case XML_COMMENT_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DOCUMENT_FRAGMENT_NODE:
        case XML_NOTATION_NODE:
            break;
        }
    }

    return newNode;
}

/*================================================================
*   XmlNodeCloneNodeTree
*       clones a node tree.
*       Internal to parser only.
*
*=================================================================*/
XmlNode *
XmlNodeCloneNodeTree(
    Pool * pool,
    XmlNode *   nodeptr,
    IN BOOL deep )
{
    XmlNode *  newNode = NULL;
    XmlElement *newElement;
    XmlNode *  childNode;

    assert( nodeptr != NULL );

    switch ( nodeptr->nodeType )
    {
    case XML_ELEMENT_NODE:
        newElement =
            XmlNodeCloneElement(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                ( XmlElement * ) nodeptr );
        newElement->node.firstAttr =
            XmlNodeCloneNodeTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                nodeptr->firstAttr,
                deep );
        if ( deep )
        {
            newElement->node.firstChild =
                XmlNodeCloneNodeTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    nodeptr->firstChild,
                    deep );
            childNode = newElement->node.firstChild;
            while ( childNode != NULL )
            {
                childNode->parentNode = ( XmlNode *   ) newElement;
                childNode = childNode->nextSibling;
            }
            newElement->node.nextSibling = NULL;
        }

        newNode = ( XmlNode *   ) newElement;
        break;

    case XML_ATTRIBUTE_NODE:
    case XML_TEXT_NODE:
    case XML_CDATA_SECTION_NODE:
    case XML_DOCUMENT_NODE:
        newNode = XmlNodeCloneNodeTreeRecursive(
#ifdef USE_CWMP_MEMORY_POOL
                      pool ,
#endif

                      nodeptr, deep );
        break;

    case XML_INVALID_NODE:
    case XML_ENTITY_REFERENCE_NODE:
    case XML_ENTITY_NODE:
    case XML_PROCESSING_INSTRUCTION_NODE:
    case XML_COMMENT_NODE:
    case XML_DOCUMENT_TYPE_NODE:
    case XML_DOCUMENT_FRAGMENT_NODE:
    case XML_NOTATION_NODE:
        break;
    }

    // by spec, the duplicate node has no parent
    newNode->parentNode = NULL;

    return newNode;
}

/*================================================================
*   XmlNodeCloneNode
*       Clones a node, if deep==TRUE, clones subtree under nodeptr.
*       External function.
*   Returns:
*       the cloned node or NULL if error occurs.
*
*=================================================================*/
XmlNode *
XmlNodeCloneNode(
    Pool * pool,

    XmlNode *   nodeptr,
    IN BOOL deep )
{

    XmlNode *  newNode;
    XmlAttribute *newAttrNode;

    if ( nodeptr == NULL )
    {
        return NULL;
    }

    switch ( nodeptr->nodeType )
    {
    case XML_ATTRIBUTE_NODE:
        newAttrNode =
            XmlNodeCloneAttrDirect(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                ( XmlAttribute * ) nodeptr );
        return ( XmlNode *   ) newAttrNode;
        break;

    default:
        newNode = XmlNodeCloneNodeTree(
#ifdef USE_CWMP_MEMORY_POOL
                      pool ,
#endif

                      nodeptr, deep );
        return newNode;
        break;
    }

}

/*================================================================
*   XmlNodeGetChildNodes
*       Returns a XmlNodeList of all the child nodes of nodeptr.
*       External function.
*
*=================================================================*/
XmlNodeList *
#ifdef USE_CWMP_MEMORY_POOL
XmlNodeGetChildNodes(Pool * pool, XmlNode *   nodeptr )
#else
XmlNodeGetChildNodes( XmlNode *   nodeptr )
#endif
{
    XmlNode *  tempNode;
    XmlNodeList *newNodeList;
    int rc;

    if ( nodeptr == NULL )
    {
        return NULL;
    }

    newNodeList = ( XmlNodeList * ) PMALLOC( sizeof( XmlNodeList ) );
    if ( newNodeList == NULL )
    {
        return NULL;
    }

    XmlNodeListInit( newNodeList );

    tempNode = nodeptr->firstChild;
    while ( tempNode != NULL )
    {
        rc = XmlNodeListAddToNodeList(
#ifdef USE_CWMP_MEMORY_POOL
                 pool ,
#endif

                 &newNodeList, tempNode );
        if ( rc != XML_OK )
        {
            XmlNodeListFree(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                newNodeList );
            return NULL;
        }

        tempNode = tempNode->nextSibling;
    }
    return newNodeList;
}

/*================================================================
*   XmlNodeGetAttributes
*       returns a namedNodeMap of attributes of nodeptr
*       External function.
*   Returns:
*
*=================================================================*/
XmlNamedNodeMap *
#ifdef USE_CWMP_MEMORY_POOL
XmlNodeGetAttributes(Pool * pool, XmlNode *   nodeptr )
#else
XmlNodeGetAttributes( XmlNode *   nodeptr )
#endif
{
    XmlNamedNodeMap *returnNamedNodeMap = NULL;
    XmlNode *  tempNode;
    int rc;

    if ( nodeptr == NULL )
    {
        return NULL;
    }

    if ( nodeptr->nodeType == XML_ELEMENT_NODE )
    {
        returnNamedNodeMap =
            ( XmlNamedNodeMap * ) PMALLOC( sizeof( XmlNamedNodeMap ) );
        if ( returnNamedNodeMap == NULL )
        {
            return NULL;
        }

        XmlNamedNodeMapInit( returnNamedNodeMap );

        tempNode = nodeptr->firstAttr;
        while ( tempNode != NULL )
        {
            rc = XmlNamedNodeMapAddToNamedNodeMap(
#ifdef USE_CWMP_MEMORY_POOL
                     pool ,
#endif
                     &returnNamedNodeMap,
                     tempNode );
            if ( rc != XML_OK )
            {
                XmlNamedNodeMapFree(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    returnNamedNodeMap );
                return NULL;
            }

            tempNode = tempNode->nextSibling;
        }
        return returnNamedNodeMap;
    }
    else                      // if not an ELEMENT_NODE
    {
        return NULL;
    }
}

/*================================================================
*   XmlNodeHasChildNodes
*       External function.
*
*=================================================================*/
BOOL
XmlNodeHasChildNodes( XmlNode *   nodeptr )
{
    if ( nodeptr == NULL )
    {
        return FALSE;
    }

    return ( nodeptr->firstChild != NULL );

}

/*================================================================
*   XmlNodeHasAttributes
*       External function.
*
*=================================================================*/
BOOL
XmlNodeHasAttributes( XmlNode *   nodeptr )
{
    if ( nodeptr != NULL )
    {
        if ( ( nodeptr->nodeType == XML_ELEMENT_NODE )
                && ( nodeptr->firstAttr != NULL ) )
        {
            return TRUE;
        }
    }
    return FALSE;

}

/*================================================================
*   XmlNodeGetElementsByTagNameRecursive
*       Recursively traverse the whole tree, search for element
*       with the given tagname.
*       Internal to parser.
*
*=================================================================*/
void
XmlNodeGetElementsByTagNameRecursive(
    Pool * pool,
    XmlNode *   n,
    IN char *tagname,
    OUT XmlNodeList ** list )
{
    const char *name;

    if ( n != NULL )
    {
        if ( XmlNodeGetNodeType( n ) == XML_ELEMENT_NODE )
        {
            name = XmlNodeGetNodeName( n )? XmlNodeGetNodeName( n ):"";
            if ( strcmp( tagname, name ) == 0
                    || strcmp( tagname, "*" ) == 0 )
            {
                XmlNodeListAddToNodeList(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    list, n );
            }
        }

        XmlNodeGetElementsByTagNameRecursive(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            XmlNodeGetFirstChild
            ( n ), tagname, list );
        XmlNodeGetElementsByTagNameRecursive(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            XmlNodeGetNextSibling
            ( n ), tagname, list );
    }

}

/*================================================================
*   XmlNodeGetElementsByTagName
*       Returns a nodeList of all descendant Elements with a given
*       tagName, in the order in which they are encountered in a
*       traversal of this element tree.
*       External function.
*
*=================================================================*/
void
XmlNodeGetElementsByTagName(
    Pool * pool,
    XmlNode *   n,
    IN char *tagname,
    OUT XmlNodeList ** list )
{
    const char *name;

    assert( n != NULL && tagname != NULL );

    if ( XmlNodeGetNodeType( n ) == XML_ELEMENT_NODE )
    {
        name = XmlNodeGetNodeName( n )?XmlNodeGetNodeName( n ):"";
        if ( strcmp( tagname, name ) == 0 || strcmp( tagname, "*" ) == 0 )
        {
            XmlNodeListAddToNodeList(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                list, n );
        }
    }

    XmlNodeGetElementsByTagNameRecursive(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        XmlNodeGetFirstChild( n ),
        tagname, list );

}

/*================================================================
*   XmlNodeGetElementsByTagNameNSRecursive
*	    Internal function to parser.
*
*
*=================================================================*/
void
XmlNodeGetElementsByTagNameNSRecursive(
    Pool * pool,
    XmlNode *   n,
    const char *namespaceURI,
    const char *localName,
    OUT XmlNodeList ** list )
{
    const char *  nsURI;
    const char *  name;

    if ( n != NULL )
    {
        if ( XmlNodeGetNodeType( n ) == XML_ELEMENT_NODE )
        {
            name = XmlNodeGetLocalName( n );
            nsURI = XmlNodeGetNamespaceURI( n );

            if ( ( name != NULL ) && ( nsURI != NULL ) &&
                    ( strcmp( namespaceURI, nsURI ) == 0
                      || strcmp( namespaceURI, "*" ) == 0 )
                    && ( strcmp( name, localName ) == 0
                         || strcmp( localName, "*" ) == 0 ) )
            {
                XmlNodeListAddToNodeList(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    list, n );
            }
        }

        XmlNodeGetElementsByTagNameNSRecursive(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif

            XmlNodeGetFirstChild
            ( n ), namespaceURI,
            localName, list );
        XmlNodeGetElementsByTagNameNSRecursive(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            XmlNodeGetNextSibling
            ( n ), namespaceURI,
            localName, list );
    }

}

/*================================================================
*   XmlNodeGetElementsByTagNameNS
*       Returns a nodeList of all the descendant Elements with a given
*       local name and namespace URI in the order in which they are
*       encountered in a preorder traversal of this Elememt tree.
*		External function.
*
*=================================================================*/
void
XmlNodeGetElementsByTagNameNS(
    Pool * pool,
    XmlNode *   n,
    const char *namespaceURI,
    const char *localName,
    OUT XmlNodeList ** list )
{
    const char *  nsURI;
    const char *  name;

    assert( n != NULL && namespaceURI != NULL && localName != NULL );

    if ( XmlNodeGetNodeType( n ) == XML_ELEMENT_NODE )
    {
        name = XmlNodeGetLocalName( n );
        nsURI = XmlNodeGetNamespaceURI( n );

        if ( ( name != NULL ) && ( nsURI != NULL ) &&
                ( strcmp( namespaceURI, nsURI ) == 0
                  || strcmp( namespaceURI, "*" ) == 0 )
                && ( strcmp( name, localName ) == 0
                     || strcmp( localName, "*" ) == 0 ) )
        {
            XmlNodeListAddToNodeList(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                list, n );
        }
    }

    XmlNodeGetElementsByTagNameNSRecursive(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        XmlNodeGetFirstChild( n ),
        namespaceURI, localName,
        list );

}

/*================================================================
*   XmlNodeSetNodeName
*       Internal to parser only.
*
*=================================================================*/
int
XmlNodeSetNodeName(
    Pool * pool,

    XmlNode *   node,
    const char * qualifiedName )
{
    int rc = XML_OK;

    assert( node != NULL );

    if ( node->nodeName != NULL )
    {
        PFREE( node->nodeName );
        node->nodeName = NULL;
    }

    if ( qualifiedName != NULL )
    {
        // set the name part
        node->nodeName = PSTRDUP( qualifiedName );
        if ( node->nodeName == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }

        rc = XmlParserSetNodePrefixAndLocalName(
#ifdef USE_CWMP_MEMORY_POOL
                 pool ,
#endif
                 node );
        if ( rc != XML_OK )
        {
            PFREE( node->nodeName );
        }
    }

    return rc;
}

/*================================================================
*   XmlNodeSetNodeProperties
*       Internal to parser only.
*
*=================================================================*/
int
XmlNodeSetNodeProperties(
    Pool * pool,
    XmlNode *   destNode,
    XmlNode *   src )
{

    int rc;

    assert( destNode != NULL || src != NULL );

    rc = XmlNodeSetNodeValue(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             destNode, src->nodeValue );
    if ( rc != XML_OK )
    {
        goto ErrorHandler;
    }

    rc = XmlNodeSetLocalName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             destNode, src->localName );
    if ( rc != XML_OK )
    {
        goto ErrorHandler;
    }

    rc = XmlNodeSetPrefix(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             destNode, src->prefix );
    if ( rc != XML_OK )
    {
        goto ErrorHandler;
    }
    // set nodetype
    destNode->nodeType = src->nodeType;

    return XML_OK;

ErrorHandler:
#ifndef USE_CWMP_MEMORY_POOL
    if ( destNode->nodeName != NULL )
    {
        PFREE( destNode->nodeName );
        destNode->nodeName = NULL;
    }
    if ( destNode->nodeValue != NULL )
    {
        PFREE( destNode->nodeValue );
        destNode->nodeValue = NULL;
    }
    if ( destNode->localName != NULL )
    {
        PFREE( destNode->localName );
        destNode->localName = NULL;
    }
#endif
    return XML_INSUFFICIENT_MEMORY;
}
