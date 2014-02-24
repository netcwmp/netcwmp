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
// A PARTICULAR PURPOSE ARE DISCLAIMED. NO EVENT SHALL INTEL OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING ANY WAY OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#ifndef CWMP_XML_PARSER_H
#define CWMP_XML_PARSER_H

#include "cwmp/xmlet.h"
#include "xmlbuffer.h"

// XMLParser definitions
#define QUOT        "&quot;"
#define LT          "&lt;"
#define GT          "&gt;"
#define APOS        "&apos;"
#define AMP         "&amp;"
#define ESC_HEX     "&#x"
#define ESC_DEC     "&#"

typedef struct XmlNameSpaceURI
{
    char                        *nsURI;
    char                        *prefix;
    struct XmlNameSpaceURI   *nextNsURI;
} XmlNameSpaceURI;


typedef struct XmlElementStack
{
    char                    *element;
    char                    *prefix;
    char                    *namespaceUri;
    XmlNameSpaceURI            *pNsURI;
    struct XmlElementStack    *nextElement;
} XmlElementStack;


typedef enum
{
    E_ELEMENT,
    E_ATTRIBUTE,
    E_CONTENT,
} XmlParserState;

typedef struct XmlParser
{
    char            *dataBuffer;	//data buffer
    char            *curPtr;		//ptr to the token parsed
    char            *savePtr;		//Saves for backup
    XmlBuffer     lastElem;
    XmlBuffer     tokenBuf;

    XmlNode           *pNeedPrefixNode;
    XmlElementStack   *pCurElement;
    XmlNode           *currentNodePtr;
    XmlParserState        state;

    BOOL                bHasTopLevel;

} XMLParser;



int     XmlParserLoadDocument(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    XmlDocument **retDoc, char * xmlFile, BOOL file);
BOOL    XmlParserIsValidXmlName(const char *  name);
int     XmlParserSetNodePrefixAndLocalName(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlNode *   node);
void    XmlParserFreeNodeContent(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    XmlNode *   node);

void    XmlAttrFree(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlAttribute *attrNode);
void    XmlAttrInit(XmlAttribute *attrNode);

void    XmlNamedNodeMapInit(XmlNamedNodeMap *nnMap);
int     XmlNamedNodeMapAddToNamedNodeMap(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif

    XmlNamedNodeMap **nnMap, XmlNode *  add);

void    XmlNodeInit(XmlNode *   node);
BOOL    XmlNodeCompare(XmlNode *   left, XmlNode *   right);

void    XmlNodeGetElementsByTagName(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    XmlNode *  n, char *tagname, XmlNodeList **list);
void    XmlNodeGetElementsByTagNameNS(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    XmlNode *   node,const char *namespaceURI,
    const char *localName, XmlNodeList **list);

int     XmlNodeSetNodeProperties(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    XmlNode* node, XmlNode *  src);

void    XmlNodeListInit(XmlNodeList *nList);
int     XmlNodeListAddToNodeList(
#ifdef USE_CWMP_MEMORY_POOL
    Pool * pool,
#endif
    XmlNodeList **nList, XmlNode *  add);



static char * XmlStrduptrim(Pool * pool, const char * data)
{
    char buffer[1024] ;
    char * p;
    char * q;
    if (!data)
    {
        return NULL;
    }

    memset(buffer, 0, 1024);
    strncpy(buffer, data, 1024);
    p =  buffer;
    while ((*p <= ' ')  && (*p != '\n'))
    {
        p ++;
    }

    q = buffer + strlen(data);
    if ((*q == 0) && (q != p))
        q --;
    while ((*p <= ' ')  && (q != p) )
    {
        q --;
    }
    *(q+1) = 0;
    return PSTRDUP(p);

}

#endif  // _IXMLPARSER_H

