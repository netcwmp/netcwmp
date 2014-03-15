/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014 netcwmp group                         *
 *                                                                      *
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

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
// A PARTICULAR PURPOSE ARE DISCLAIMED. NO EVENT SHALL INTEL OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef __CWMPXMLET_H__
#define __CWMPXMLET_H__
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <errno.h>


#include <cwmp/pool.h>



#define TRXML_OK 0
#define TRXML_ERROR -1

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef XmlChar
typedef char XmlChar ;
#endif


#ifndef USE_CWMP_MEMORY_POOL
#define PMALLOC(size)               malloc((size))
#define PREALLOC(p,olds,news)       realloc(p, news)
#define PSTRDUP(size)               strdup(size)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif


/*================================================================
*
*   DOM node type
*
*
*=================================================================*/
typedef enum
{
    XML_INVALID_NODE                   = 0,
    XML_ELEMENT_NODE                   = 1,
    XML_ATTRIBUTE_NODE                 = 2,
    XML_TEXT_NODE                      = 3,
    XML_CDATA_SECTION_NODE             = 4,
    XML_ENTITY_REFERENCE_NODE          = 5,
    XML_ENTITY_NODE                    = 6,
    XML_PROCESSING_INSTRUCTION_NODE    = 7,
    XML_COMMENT_NODE                   = 8,
    XML_DOCUMENT_NODE                  = 9,
    XML_DOCUMENT_TYPE_NODE             = 10,
    XML_DOCUMENT_FRAGMENT_NODE         = 11,
    XML_NOTATION_NODE                  = 12,

}   XmlNodeType;

/*================================================================
*
*   error code
*
*
*=================================================================*/
typedef enum
{
    // see DOM spec
    XML_INDEX_SIZE_ERR                 = 1,
    XML_DOMSTRING_SIZE_ERR             = 2,
    XML_HIERARCHY_REQUEST_ERR          = 3,
    XML_WRONG_DOCUMENT_ERR             = 4,
    XML_INVALID_CHARACTER_ERR          = 5,
    XML_NO_DATA_ALLOWED_ERR            = 6,
    XML_NO_MODIFICATION_ALLOWED_ERR    = 7,
    XML_NOT_FOUND_ERR                  = 8,
    XML_NOT_SUPPORTED_ERR              = 9,
    XML_INUSE_ATTRIBUTE_ERR            = 10,
    XML_INVALID_STATE_ERR              = 11,
    XML_SYNTAX_ERR                     = 12,
    XML_INVALID_MODIFICATION_ERR       = 13,
    XML_NAMESPACE_ERR                  = 14,
    XML_INVALID_ACCESS_ERR             = 15,

    XML_OK                        = 0,
    XML_NO_SUCH_FILE                   = 101,
    XML_INSUFFICIENT_MEMORY            = 102,
    XML_FILE_DONE                      = 104,
    XML_INVALID_PARAMETER              = 105,
    XML_FAILED                         = 106,
    XML_INVALID_ITEM_NUMBER            = 107,

} XML_ERRORCODE;


#define DOCUMENTNODENAME    "#document"
#define TEXTNODENAME        "#text"
#define CDATANODENAME       "#cdata-section"

/*================================================================
*
*   DOM data structures
*
*
*=================================================================*/
typedef struct XmlDocument XmlDocument;
typedef struct XmlNode XmlNode;
typedef struct XmlCDATASection XmlCDATASection;
typedef struct XmlElement XmlElement;
typedef struct XmlAttribute XmlAttribute;
typedef struct XmlText XmlText;
typedef struct XmlNodeList XmlNodeList;
typedef struct XmlNamedNodeMap XmlNamedNodeMap;

typedef XmlDocument     xmldoc_t;
typedef XmlNode         xmlnode_t;
typedef XmlElement      xmlelemt_t;
typedef XmlAttribute    xmlattr_t;




struct XmlNode
{
    char *        nodeName;
    char *        nodeValue;
    XmlNodeType     nodeType;
    char *        namespaceURI;
    char *        prefix;
    char *        localName;
    BOOL            readOnly;

    XmlNode *         parentNode;
    XmlNode *         firstChild;
    XmlNode *         prevSibling;
    XmlNode *         nextSibling;
    XmlNode *         firstAttr;
    XmlDocument *     ownerDocument;

};






struct XmlCDATASection
{
    XmlNode    node;
};


struct XmlElement
{
    XmlNode   node;
    char *    tagName;
};

struct XmlDocument
{
    XmlNode    node;
};



struct XmlAttribute
{
    XmlNode   node;
    BOOL        specified;
    XmlElement *ownerElement;
};



struct XmlText
{
    XmlNode   node;
};



struct XmlNodeList
{
    XmlNode    *nodeItem;
    struct  XmlNodeList *next;
};




struct XmlNamedNodeMap
{
    XmlNode                *nodeItem;
    struct XmlNamedNodeMap *next;
};



#ifdef __cplusplus
extern "C"
{
#endif

    /*================================================================
    *
    *   NODE interfaces
    *
    *
    *=================================================================*/


    const char *
    XmlNodeGetNodeName(XmlNode *  nodeptr
                      );

    char *
    XmlNodeGetNodeValue(XmlNode *  nodeptr
                       );

    int
    XmlNodeSetNodeValue(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlNode *  nodeptr,
        const char *newNodeValue
    );

    int
    XmlNodeGetNodeType(XmlNode *  nodeptr
                      );

    XmlNode*
    XmlNodeGetParentNode(XmlNode *  nodeptr
                        );

    XmlNodeList*
    XmlNodeGetChildNodes(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlNode *  nodeptr
    );

    XmlNode*
    XmlNodeGetFirstChild(XmlNode *  nodeptr
                        );

    XmlNode*
    XmlNodeGetLastChild(XmlNode *  nodeptr
                       );

    XmlNode*
    XmlNodeGetPreviousSibling(XmlNode *  nodeptr
                             );

    XmlNode*
    XmlNodeGetNextSibling(XmlNode *  nodeptr
                         );

    XmlNamedNodeMap*
    XmlNodeGetAttributes(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlNode *  nodeptr
    );

    XmlDocument*
    XmlNodeGetOwnerDocument(XmlNode *  nodeptr
                           );

    const char *
    XmlNodeGetNamespaceURI(XmlNode *  nodeptr
                          );

    char *
    XmlNodeGetPrefix(XmlNode *  nodeptr
                    );

    const char *
    XmlNodeGetLocalName(XmlNode *  nodeptr
                       );

    int
    XmlNodeInsertBefore(XmlNode *  nodeptr,
                        XmlNode* newChild,
                        XmlNode* refChild
                       );

    int
    XmlNodeReplaceChild(XmlNode *  nodeptr,
                        XmlNode* newChild,
                        XmlNode* oldChild,
                        XmlNode** returnNode
                       );

    int
    XmlNodeRemoveChild(XmlNode *  nodeptr,
                       XmlNode* oldChild,
                       XmlNode **returnNode
                      );

    int
    XmlNodeAppendChild(XmlNode *  nodeptr,
                       XmlNode* newChild
                      );

    BOOL
    XmlNodeHasChildNodes(XmlNode *  nodeptr
                        );

    XmlNode*
    XmlNodeCloneNode(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlNode *  nodeptr,
        BOOL deep
    );

    BOOL
    XmlNodeHasAttributes(XmlNode *  node
                        );

    void
    XmlNodeFree(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlNode *  XmlNode
    );


    /*================================================================
    *
    *   Attribute interfaces
    *
    *
    *=================================================================*/



    void
    XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlAttribute *attrNode
    );



    /*================================================================
    *
    *   CDATASection interfaces
    *
    *
    *=================================================================*/



    void
    XmlCDATASectionInit(XmlCDATASection *nodeptr
                       );


    void
    XmlCDATASectionFree(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlCDATASection *nodeptr
    );


    /*================================================================
    *
    *   Document interfaces
    *
    *
    *=================================================================*/


    void
    XmlDocumentInit(XmlDocument *nodeptr
                   );

    int XmlDocCreateDocumentEx(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument** doc
    );


    XmlDocument* XmlDocCreateDocument(
#if USE_CWMP_MEMORY_POOL
        Pool * pool
#endif
    );

    int

#ifdef USE_CWMP_MEMORY_POOL
    XmlDocCreateElementEx(Pool * pool, XmlDocument * doc,
#else
    XmlDocCreateElementEx( XmlDocument * doc,
#endif

                          char *  tagName,
                          XmlElement **rtElement
                         );

    XmlElement*
    XmlDocCreateElement(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlDocument *doc,
        char *  tagName
    );


    int
    XmlDocCreateTextNodeEx(
#ifdef USE_CWMP_MEMORY_POOL
        Pool* pool ,
#endif
        XmlDocument *doc,
        char *  data,
        XmlNode** textNode
    );


    XmlNode*
    XmlDocCreateTextNode(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlDocument *doc,
        char *  data
    );

    int
    XmlDocCreateCDATASectionEx(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument *doc,
        char *  data,
        XmlCDATASection** cdNode
    );


    XmlCDATASection*
    XmlDocCreateCDATASection(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument *doc,
        char *  data
    );

    XmlAttribute*
    XmlDocCreateAttribute(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument *doc,
        char *name
    );


    int
    XmlDocCreateAttributeEx(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlDocument *doc,
        const char *name,
        XmlAttribute** attrNode
    );


    XmlNodeList*
    XmlDocGetElementsByTagName(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlDocument *doc,
        char *  tagName


    );

// introduced in DOM level 2

    int
    XmlDocCreateElementNSEx(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument *doc,
        char *  namespaceURI,
        char *  qualifiedName,
        XmlElement** rtElement
    );


    XmlElement*
    XmlDocCreateElementNS(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument *doc,
        char *  namespaceURI,
        char *  qualifiedName
    );

    int
    XmlDocCreateAttributeNSEx(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument *doc,
        const char *  namespaceURI,
        const char *  qualifiedName,
        XmlAttribute** attrNode
    );

    XmlAttribute*
    XmlDocCreateAttributeNS(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument *doc,
        char *  namespaceURI,
        char *  qualifiedName
    );

    XmlNodeList*
    XmlDocGetElementsByTagNameNS(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlDocument* doc,
        char *  namespaceURI,
        char *  localName
    );

    XmlElement*
    XmlDocGetElementById(XmlDocument* doc,
                         char *  tagName
                        );

    void
    XmlDocumentFree(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlDocument* doc
    );

    int
    XmlDocImportNode(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlDocument* doc,
        XmlNode* importNode,
        BOOL deep,
        XmlNode** rtNode
    );

    /*================================================================
    *
    *   Element interfaces
    *
    *
    *=================================================================*/


    void XmlElementInit(XmlElement *element
                       );


    const char *
    XmlElementGetTagName(XmlElement* element
                        );

    char *
    XmlElementGetAttribute(XmlElement* element,
                           const char *  name
                          );

    int
    XmlElementSetAttribute(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlElement* element,
        const char *  name,
        const char *  value
    );

    int
    XmlElementRemoveAttribute(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlElement* element,
        const char *  name
    );

    XmlAttribute*
    XmlElementGetAttributeNode(XmlElement* element,
                               char *  name
                              );

    int
    XmlElementSetAttributeNode(XmlElement* element,
                               XmlAttribute* newAttr,
                               XmlAttribute** rtAttr
                              );

    int
    XmlElementRemoveAttributeNode(XmlElement* element,
                                  XmlAttribute* oldAttr,
                                  XmlAttribute** rtAttr
                                 );

    XmlNodeList*
    XmlElementGetElementsByTagName(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlElement* element,
        char *  tagName
    );

// introduced in DOM 2

    char *
    XmlElementGetAttributeNS(XmlElement* element,
                             const char *  namespaceURI,
                             const char *  localname
                            );

    int
    XmlElementSetAttributeNS(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlElement* element,
        const char *  namespaceURI,
        const char *  qualifiedName,
        const char *  value
    );

    int
    XmlElementRemoveAttributeNS(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlElement* element,
        const char *  namespaceURI,
        const char *  localName
    );

    XmlAttribute*
    XmlElementGetAttributeNodeNS(XmlElement* element,
                                 const char *  namespaceURI,
                                 const char *  localName
                                );

    int
    XmlElementSetAttributeNodeNS(XmlElement* element,
                                 XmlAttribute*   newAttr,
                                 XmlAttribute**  rcAttr
                                );

    XmlNodeList*
    XmlElementGetElementsByTagNameNS(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlElement* element,
        const char *  namespaceURI,
        const char *  localName
    );

    BOOL
    XmlElementHasAttribute(XmlElement* element,
                           const char *  name
                          );

    BOOL
    XmlElementHasAttributeNS(XmlElement* element,
                             const char *  namespaceURI,
                             const char *  localName
                            );

    void
    XmlElementFree(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlElement* element
    );


    /*================================================================
    *
    *   NamedNodeMap interfaces
    *
    *
    *=================================================================*/


    unsigned long
    XmlNamedNodeMapGetLength(XmlNamedNodeMap *nnMap
                            );

    XmlNode*
    XmlNamedNodeMapGetNamedItem(XmlNamedNodeMap *nnMap,
                                char *  name
                               );

    XmlNode*
    XmlNamedNodeMapSetNamedItem(XmlNamedNodeMap *nnMap,
                                XmlNode *  arg
                               );

    XmlNode*
    XmlNamedNodeMapRemoveNamedItem(XmlNamedNodeMap *nnMap,
                                   char *  name
                                  );

    XmlNode*
    XmlNamedNodeMapItem(XmlNamedNodeMap *nnMap,
                        unsigned long index
                       );

// introduced in DOM level 2

    XmlNode*
    XmlNamedNodeMapGetNamedItemNS(XmlNamedNodeMap *nnMap,
                                  char *  *namespaceURI,
                                  char *  localName
                                 );

    XmlNode*
    XmlNamedNodeMapSetNamedItemNS(XmlNamedNodeMap *nnMap,
                                  XmlNode *  arg
                                 );

    XmlNode*
    XmlNamedNodeMapRemoveNamedItemNS(XmlNamedNodeMap *nnMap,
                                     char *  namespaceURI,
                                     char *  localName
                                    );

    void
    XmlNamedNodeMapFree(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlNamedNodeMap *nnMap
    );


    /*================================================================
    *
    *   NodeList interfaces
    *
    *
    *=================================================================*/


    XmlNode*
    XmlNodeListItem(XmlNodeList *nList,
                    unsigned long index
                   );

    unsigned long
    XmlNodeListLength(XmlNodeList *nList
                     );

    void
    XmlNodeListFree(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlNodeList *nList
    );




    /*================================================================
    *
    *   Xml interfaces
    *
    *
    *=================================================================*/

#ifdef USE_CWMP_MEMORY_POOL
#define     XmlPrintDocument(pool ,  doc)  XmlPrintNode(pool ,  (XmlNode *) &doc->node)
#else
#define     XmlPrintDocument(doc)  XmlPrintNode((XmlNode *)doc)
#endif

#ifdef USE_CWMP_MEMORY_POOL
#define XmlDocumentToString(pool ,  doc)        XmlNodeToString(pool ,  (XmlNode *)doc)
#else
#define XmlDocumentToString(doc)        XmlNodeToString((XmlNode *)doc)
#endif
    char *
    XmlPrintNode(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlNode *  doc
    );

    char *
    XmlNodeToString(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        XmlNode *  doc
    );

    XmlDocument*
    XmlParseBuffer(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        char *buffer
    );


    int
    XmlParseBufferEx(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        char *buffer,
        XmlDocument** doc
    );

    XmlDocument*
    XmlLoadDocument(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        char* xmlFile
    );

    int
    XmlLoadDocumentEx(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif

        char* xmlFile,
        XmlDocument** doc
    );

    char *
    XmlCloneDOMString(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        const char *  src
    );

    void
    XmlFreeDOMString(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        char *  buf
    );

    int
    XmlElementSetTagName(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlElement *element, const char *tagName);

    int
    XmlNodeSetNodeName(
#ifdef USE_CWMP_MEMORY_POOL
        Pool * pool,
#endif
        XmlNode * node,const char * qualifiedName);


#ifdef __cplusplus
}
#endif


#endif  // CWMP_XML_H
