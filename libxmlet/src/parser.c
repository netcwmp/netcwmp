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

#include <string.h>
#include "xmlparser.h"

static const char LESSTHAN = '<';
static const char GREATERTHAN = '>';
static const char SLASH = '/';
static const char EQUALS = '=';
static const char QUOTE = '\"';
static const char SINGLEQUOTE = '\'';

static const char *WHITESPACE = "\n\t\r ";
static const char *COMPLETETAG = "/>";
static const char *ENDTAG = "</";
static const char *XMLDECL = "<?xml ";
static const char *XMLDECL2 = "<?xml?";
static const char *BEGIN_COMMENT = "<!--";
static const char *END_COMMENT = "-->";
static const char *BEGIN_PI = "<?";
static const char *END_PI = "?>";
static const char *BEGIN_DOCTYPE = "<!DOCTYPE";
static const char *CDSTART = "<![CDATA[";
static const char *CDEND = "]]>";
static const char *DEC_NUMBERS = "0123456789";
static const char *HEX_NUMBERS = "0123456789ABCDEFabcdef";

typedef struct XmlCharInfo
{
    unsigned short l,
    h;
} XmlCharInfo;

typedef char utf8char[8];

/*==============================================================================*
*	Letter table contains all characters in XML 1.0 plus ":", "_" and
*   ideographic.
*
*   This table contains all the characters that an element name can start with.
*   See XML 1.0 (2nd Edition) for more details.
*
*===============================================================================*/
static XmlCharInfo Letter[] =
{
    {0x003A, 0x003A},           // character ":"
    {0x0041, 0x005A},
    {0x005F, 0x005F},           // character "_"
    {0x0061, 0x007A}, {0x00C0, 0x00D6}, {0x00D8, 0x00F6}, {0x00F8, 0x00FF},
    {0x0100, 0x0131}, {0x0134, 0x013E}, {0x0141, 0x0148}, {0x014A, 0x017E},
    {0x0180, 0x01C3}, {0x01CD, 0x01F0}, {0x01F4, 0x01F5}, {0x01FA, 0x0217},
    {0x0250, 0x02A8}, {0x02BB, 0x02C1}, {0x0386, 0x0386}, {0x0388, 0x038A},
    {0x038C, 0x038C}, {0x038E, 0x03A1}, {0x03A3, 0x03CE}, {0x03D0, 0x03D6},
    {0x03DA, 0x03DA},
    {0x03DC, 0x03DC}, {0x03DE, 0x03DE}, {0x03E0, 0x03E0}, {0x03E2, 0x03F3},
    {0x0401, 0x040C}, {0x040E, 0x044F}, {0x0451, 0x045C}, {0x045E, 0x0481},
    {0x0490, 0x04C4}, {0x04C7, 0x04C8}, {0x04CB, 0x04CC}, {0x04D0, 0x04EB},
    {0x04EE, 0x04F5}, {0x04F8, 0x04F9}, {0x0531, 0x0556}, {0x0559, 0x0559},
    {0x0561, 0x0586}, {0x05D0, 0x05EA}, {0x05F0, 0x05F2}, {0x0621, 0x063A},
    {0x0641, 0x064A}, {0x0671, 0x06B7}, {0x06BA, 0x06BE}, {0x06C0, 0x06CE},
    {0x06D0, 0x06D3}, {0x06D5, 0x06D5}, {0x06E5, 0x06E6}, {0x0905, 0x0939},
    {0x093D, 0x093D}, {0x0958, 0x0961}, {0x0985, 0x098C}, {0x098F, 0x0990},
    {0x0993, 0x09A8}, {0x09AA, 0x09B0}, {0x09B2, 0x09B2}, {0x09B6, 0x09B9},
    {0x09DC, 0x09DD}, {0x09DF, 0x09E1}, {0x09F0, 0x09F1}, {0x0A05, 0x0A0A},
    {0x0A0F, 0x0A10}, {0x0A13, 0x0A28}, {0x0A2A, 0x0A30}, {0x0A32, 0x0A33},
    {0x0A35, 0x0A36}, {0x0A38, 0x0A39}, {0x0A59, 0x0A5C}, {0x0A5E, 0x0A5E},
    {0x0A72, 0x0A74}, {0x0A85, 0x0A8B}, {0x0A8D, 0x0A8D}, {0x0A8F, 0x0A91},
    {0x0A93, 0x0AA8}, {0x0AAA, 0x0AB0}, {0x0AB2, 0x0AB3}, {0x0AB5, 0x0AB9},
    {0x0ABD, 0x0ABD}, {0x0AE0, 0x0AE0}, {0x0B05, 0x0B0C}, {0x0B0F, 0x0B10},
    {0x0B13, 0x0B28}, {0x0B2A, 0x0B30}, {0x0B32, 0x0B33}, {0x0B36, 0x0B39},
    {0x0B3D, 0x0B3D}, {0x0B5C, 0x0B5D}, {0x0B5F, 0x0B61}, {0x0B85, 0x0B8A},
    {0x0B8E, 0x0B90}, {0x0B92, 0x0B95}, {0x0B99, 0x0B9A}, {0x0B9C, 0x0B9C},
    {0x0B9E, 0x0B9F}, {0x0BA3, 0x0BA4}, {0x0BA8, 0x0BAA}, {0x0BAE, 0x0BB5},
    {0x0BB7, 0x0BB9}, {0x0C05, 0x0C0C}, {0x0C0E, 0x0C10}, {0x0C12, 0x0C28},
    {0x0C2A, 0x0C33}, {0x0C35, 0x0C39}, {0x0C60, 0x0C61}, {0x0C85, 0x0C8C},
    {0x0C8E, 0x0C90}, {0x0C92, 0x0CA8}, {0x0CAA, 0x0CB3}, {0x0CB5, 0x0CB9},
    {0x0CDE, 0x0CDE}, {0x0CE0, 0x0CE1}, {0x0D05, 0x0D0C}, {0x0D0E, 0x0D10},
    {0x0D12, 0x0D28}, {0x0D2A, 0x0D39}, {0x0D60, 0x0D61}, {0x0E01, 0x0E2E},
    {0x0E30, 0x0E30}, {0x0E32, 0x0E33}, {0x0E40, 0x0E45}, {0x0E81, 0x0E82},
    {0x0E84, 0x0E84}, {0x0E87, 0x0E88}, {0x0E8A, 0x0E8A}, {0x0E8D, 0x0E8D},
    {0x0E94, 0x0E97}, {0x0E99, 0x0E9F}, {0x0EA1, 0x0EA3}, {0x0EA5, 0x0EA5},
    {0x0EA7, 0x0EA7}, {0x0EAA, 0x0EAB}, {0x0EAD, 0x0EAE}, {0x0EB0, 0x0EB0},
    {0x0EB2, 0x0EB3}, {0x0EBD, 0x0EBD}, {0x0EC0, 0x0EC4}, {0x0F40, 0x0F47},
    {0x0F49, 0x0F69}, {0x10A0, 0x10C5}, {0x10D0, 0x10F6}, {0x1100, 0x1100},
    {0x1102, 0x1103}, {0x1105, 0x1107}, {0x1109, 0x1109}, {0x110B, 0x110C},
    {0x110E, 0x1112}, {0x113C, 0x113C}, {0x113E, 0x113E}, {0x1140, 0x1140},
    {0x114C, 0x114C}, {0x114E, 0x114E}, {0x1150, 0x1150}, {0x1154, 0x1155},
    {0x1159, 0x1159}, {0x115F, 0x1161}, {0x1163, 0x1163}, {0x1165, 0x1165},
    {0x1167, 0x1167}, {0x1169, 0x1169}, {0x116D, 0x116E}, {0x1172, 0x1173},
    {0x1175, 0x1175}, {0x119E, 0x119E}, {0x11A8, 0x11A8}, {0x11AB, 0x11AB},
    {0x11AE, 0x11AF}, {0x11B7, 0x11B8}, {0x11BA, 0x11BA}, {0x11BC, 0x11C2},
    {0x11EB, 0x11EB}, {0x11F0, 0x11F0}, {0x11F9, 0x11F9}, {0x1E00, 0x1E9B},
    {0x1EA0, 0x1EF9}, {0x1F00, 0x1F15}, {0x1F18, 0x1F1D}, {0x1F20, 0x1F45},
    {0x1F48, 0x1F4D}, {0x1F50, 0x1F57}, {0x1F59, 0x1F59}, {0x1F5B, 0x1F5B},
    {0x1F5D, 0x1F5D}, {0x1F5F, 0x1F7D}, {0x1F80, 0x1FB4}, {0x1FB6, 0x1FBC},
    {0x1FBE, 0x1FBE}, {0x1FC2, 0x1FC4}, {0x1FC6, 0x1FCC}, {0x1FD0, 0x1FD3},
    {0x1FD6, 0x1FDB}, {0x1FE0, 0x1FEC}, {0x1FF2, 0x1FF4}, {0x1FF6, 0x1FFC},
    {0x2126, 0x2126}, {0x212A, 0x212B}, {0x212E, 0x212E}, {0x2180, 0x2182},
    {0x3007, 0x3007}, {0x3021, 0x3029}, // these two are ideographic
    {0x3041, 0x3094}, {0x30A1, 0x30FA}, {0x3105, 0x312C},
    {0x4E00, 0x9FA5},           // ideographic
    {0xAC00, 0xD7A3}
};

#define LETTERTABLESIZE (sizeof(Letter)/sizeof(Letter[0]))

/*==============================================================================*
*   NameChar table contains
*   CombiningChar, Extender, Digit, '-', '.', less '_', ':'
*   NameChar ::= Digit | '-' | '.' | CombiningChar | Extender
*   See XML 1.0 2nd Edition
*
*===============================================================================*/
static XmlCharInfo NameChar[] =
{
    {0x002D, 0x002D},           // character "-"
    {0x002E, 0x002E},           // character "."
    {0x0030, 0x0039},           // digit
    {0x00B7, 0x00B7}, {0x02D0, 0x02D0}, {0x02D1, 0x02D1},   // extended
    {0x0300, 0x0345}, {0x0360, 0x0361},
    {0x0387, 0x0387},           // extended
    {0x0483, 0x0486}, {0x0591, 0x05A1}, {0x05A3, 0x05B9},
    {0x05BB, 0x05BD}, {0x05BF, 0x05BF}, {0x05C1, 0x05C2}, {0x05C4, 0x05C4},
    {0x0640, 0x0640},           // extended
    {0x064B, 0x0652},
    {0x0660, 0x0669},           // digit
    {0x0670, 0x0670},
    {0x06D6, 0x06DC}, {0x06DD, 0x06DF}, {0x06E0, 0x06E4}, {0x06E7, 0x06E8},
    {0x06EA, 0x06ED},
    {0x06F0, 0x06F9},           // digit
    {0x0901, 0x0903}, {0x093C, 0x093C},
    {0x093E, 0x094C}, {0x094D, 0x094D}, {0x0951, 0x0954}, {0x0962, 0x0963},
    {0x0966, 0x096F},           // digit
    {0x0981, 0x0983}, {0x09BC, 0x09BC}, {0x09BE, 0x09BE},
    {0x09BF, 0x09BF}, {0x09C0, 0x09C4}, {0x09C7, 0x09C8}, {0x09CB, 0x09CD},
    {0x09D7, 0x09D7}, {0x09E2, 0x09E3},
    {0x09E6, 0x09EF},           // digit
    {0x0A02, 0x0A02},
    {0x0A3C, 0x0A3C}, {0x0A3E, 0x0A3E}, {0x0A3F, 0x0A3F}, {0x0A40, 0x0A42},
    {0x0A47, 0x0A48}, {0x0A4B, 0x0A4D},
    {0x0A66, 0x0A6F},           // digit
    {0x0A70, 0x0A71},
    {0x0A81, 0x0A83}, {0x0ABC, 0x0ABC}, {0x0ABE, 0x0AC5}, {0x0AC7, 0x0AC9},
    {0x0ACB, 0x0ACD},
    {0x0AE6, 0x0AEF},           // digit
    {0x0B01, 0x0B03}, {0x0B3C, 0x0B3C},
    {0x0B3E, 0x0B43}, {0x0B47, 0x0B48}, {0x0B4B, 0x0B4D}, {0x0B56, 0x0B57},
    {0x0B66, 0x0B6F},           // digit
    {0x0B82, 0x0B83}, {0x0BBE, 0x0BC2}, {0x0BC6, 0x0BC8},
    {0x0BCA, 0x0BCD}, {0x0BD7, 0x0BD7},
    {0x0BE7, 0x0BEF},           // digit
    {0x0C01, 0x0C03},
    {0x0C3E, 0x0C44}, {0x0C46, 0x0C48}, {0x0C4A, 0x0C4D}, {0x0C55, 0x0C56},
    {0x0C66, 0x0C6F},           // digit
    {0x0C82, 0x0C83}, {0x0CBE, 0x0CC4}, {0x0CC6, 0x0CC8},
    {0x0CCA, 0x0CCD}, {0x0CD5, 0x0CD6},
    {0x0CE6, 0x0CEF},           // digit
    {0x0D02, 0x0D03},
    {0x0D3E, 0x0D43}, {0x0D46, 0x0D48}, {0x0D4A, 0x0D4D}, {0x0D57, 0x0D57},
    {0x0D66, 0x0D6F},           // digit
    {0x0E31, 0x0E31}, {0x0E34, 0x0E3A},
    {0x0E46, 0x0E46},           // extended
    {0x0E47, 0x0E4E},
    {0x0E50, 0x0E59},           // digit
    {0x0EB1, 0x0EB1}, {0x0EB4, 0x0EB9},
    {0x0EBB, 0x0EBC},
    {0x0EC6, 0x0EC6},           // extended
    {0x0EC8, 0x0ECD},
    {0x0ED0, 0x0ED9},           // digit
    {0x0F18, 0x0F19},
    {0x0F20, 0x0F29},           // digit
    {0x0F35, 0x0F35}, {0x0F37, 0x0F37},
    {0x0F39, 0x0F39}, {0x0F3E, 0x0F3E}, {0x0F3F, 0x0F3F}, {0x0F71, 0x0F84},
    {0x0F86, 0x0F8B}, {0x0F90, 0x0F95}, {0x0F97, 0x0F97}, {0x0F99, 0x0FAD},
    {0x0FB1, 0x0FB7}, {0x0FB9, 0x0FB9}, {0x20D0, 0x20DC}, {0x20E1, 0x20E1},
    {0x3005, 0x3005},           // extended
    {0x302A, 0x302F},
    {0x3031, 0x3035},           // extended
    {0x3099, 0x3099}, {0x309A, 0x309A}, // combining char
    {0x309D, 0x309E}, {0x30FC, 0x30FE}  // extended
};

#define NAMECHARTABLESIZE   (sizeof(NameChar)/sizeof(NameChar[0]))

static int XMLStrncasecmp(const char * s1, const char * s2, size_t n)
{
    if (!s1 || !s2)
    {
        return -1;
    }
#ifdef WIN32

    if (n == 0)
        return 0;

    while (n-- != 0 && tolower(*s1) == tolower(*s2))
    {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
#else
    return strncasecmp(s1, s2, n);
#endif
}



// functions used in this file
static void XMLParserFree(
    Pool * pool,
    XMLParser * myParser );
static int XMLParserSkipDocType( char **pstr );
static int XMLParserSkipProlog( XMLParser * xmlParser );
static int XMLParserSkipMisc( XMLParser * xmlParser );
static void XMLParserFreeElementStackItem(
    Pool * pool,
    XmlElementStack * pItem );
static void XMLParserFreeNsURI(
    Pool * pool,
    XmlNameSpaceURI * pNsURI );

static int XMLParserGetNextNode(
    Pool * pool,
    XMLParser * myParser,
    XmlNode *   newNode,
    BOOL * isEnd );
static int XMLParserGetNextToken(
    Pool * pool,
    XMLParser * myParser );
static int XMLParserXmlNamespace(
    Pool * pool,
    XMLParser * myParser,
    XmlNode *   newNode );
static BOOL XMLParserElementPrefixDefined( XMLParser * myParser,
        XmlNode *   newNode,
        char **nsURI );
static int XMLParserSetElementNamespace(
    Pool * pool,
    XmlElement * newElement,
    char *nsURI );
static int XMLParserParseDocument(
    Pool * pool,

    XmlDocument ** retDoc,
    XMLParser * domParser );
static BOOL XMLParserHasDefaultNamespace( XMLParser * xmlParser,
        XmlNode *   newNode,
        char **nsURI );
static int XMLParserGetChar( IN char *src,
                             INOUT int *cLen );

/*==============================================================================*
*   XMLParserIsCharInTable
*       will determine whether character c is in the table of tbl
*       (either Letter table or NameChar table)
*
*===============================================================================*/
static BOOL
XMLParserIsCharInTable( IN int c,
                        IN XmlCharInfo * tbl,
                        IN int sz )
{
    int t = 0,
            b = sz,
                m;

    while ( t <= b )
    {
        m = ( t + b ) / 2;
        if ( c < tbl[m].l )
        {
            b = m - 1;
        }
        else if ( c > tbl[m].h )
        {
            t = m + 1;
        }
        else
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*==============================================================================*
*	XMLParserIsXmlChar
*	    see XML 1.0 (2nd Edition) 2.2.
*       Internal to parser only
*
*===============================================================================*/
static BOOL
XMLParserIsXmlChar( IN int c )
{
    return ( c == 0x9 || c == 0xA || c == 0xD ||
             ( c >= 0x20 && c <= 0xD7FF ) ||
             ( c >= 0xE000 && c <= 0xFFFD ) ||
             ( c >= 0x10000 && c <= 0x10FFFF ) );
}

/*==============================================================================*
*   XMLParserIsNameChar
*       check whether c (int) is in LetterTable or NameCharTable
*       Internal to parser only.
*
*===============================================================================*/
static BOOL
XMLParserIsNameChar( IN int c,
                     IN BOOL bNameChar )
{
    if ( XMLParserIsCharInTable( c, Letter, LETTERTABLESIZE ) )
    {
        return TRUE;
    }

    if ( bNameChar
            && XMLParserIsCharInTable( c, NameChar, NAMECHARTABLESIZE ) )
    {
        return TRUE;
    }
    return FALSE;
}

/*==============================================================================*
*   XmlParserIsValidXmlName
*       Check to see whether name is a valid xml name.
*       External function.
*
*===============================================================================*/
BOOL
XmlParserIsValidXmlName( const char *  name )
{
    char *pstr = NULL;
    int i = 0,
            nameLen = 0;

    assert( name != NULL );

    nameLen = strlen( name );

    pstr = (char *)name;
    if ( XMLParserIsNameChar( *pstr, FALSE ) == TRUE )
    {
        for ( i = 1; i < nameLen; i++ )
        {
            if ( XMLParserIsNameChar( *( pstr + 1 ), TRUE ) == FALSE )    //illegal char
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*==============================================================================*
*   XMLParserIntToUTF8:
*       Encoding a character to its UTF-8 character string, and return its length
*       internal function.
*
*===============================================================================*/
static int
XMLParserIntToUTF8( IN int c,
                    IN utf8char s )
{
    if ( c < 0 )
    {
        return 0;
    }

    if ( c <= 127 )
    {
        s[0] = c;
        s[1] = 0;
        return 1;
    }
    else if ( c <= 0x07FF )   // 0x0080 < c <= 0x07FF
    {
        s[0] = 0xC0 | ( c >> 6 );
        s[1] = 0x80 | ( c & 0x3f );
        s[2] = 0;
        return 2;
    }
    else if ( c <= 0xFFFF )   // 0x0800 < c <= 0xFFFF
    {
        s[0] = 0xE0 | ( c >> 12 );
        s[1] = 0x80 | ( ( c >> 6 ) & 0x3f );
        s[2] = 0x80 | ( c & 0x3f );
        s[3] = 0;
        return 3;
    }
    else if ( c <= 0x1FFFFF )     // 0x10000 < c <= 0x1FFFFF
    {
        s[0] = 0xF0 | ( c >> 18 );
        s[1] = 0x80 | ( ( c >> 12 ) & 0x3f );
        s[2] = 0x80 | ( ( c >> 6 ) & 0x3f );
        s[3] = 0x80 | ( c & 0x3f );
        s[4] = 0;
        return 4;
    }
    else if ( c <= 0x3FFFFFF )    // 0x200000 < c <= 3FFFFFF
    {
        s[0] = 0xF8 | ( c >> 24 );
        s[1] = 0x80 | ( ( c >> 18 ) & 0x3f );
        s[2] = 0x80 | ( ( c >> 12 ) & 0x3f );
        s[3] = 0x80 | ( ( c >> 6 ) & 0x3f );
        s[4] = 0x80 | ( c & 0x3f );
        s[5] = 0;
        return 5;
    }
    else if ( c <= 0x7FFFFFFF )   // 0x4000000 < c <= 7FFFFFFF
    {
        s[0] = 0xFC | ( c >> 30 );
        s[1] = 0x80 | ( ( c >> 24 ) & 0x3f );
        s[2] = 0x80 | ( ( c >> 18 ) & 0x3f );
        s[3] = 0x80 | ( ( c >> 12 ) & 0x3f );
        s[4] = 0x80 | ( ( c >> 6 ) & 0x3f );
        s[5] = 0x80 | ( c & 0x3f );
        s[6] = 0;
        return 6;
    }
    else                      // illegal
    {
        return 0;
    }
}

/*==============================================================================*
*   Parser_UTF8ToInt
*       In UTF-8, characters are encoded using sequences of 1 to 6 octets.
*       This functions will return a UTF-8 character value and its octets number.
*       Internal to parser only.
*       Internal to parser only
*
*===============================================================================*/
static int
Parser_UTF8ToInt( IN char *ss,
                  OUT int *len )
{

    unsigned char *s = ( unsigned char * )ss;
    int c = *s;

    if ( c <= 127 )             // if c<=127, c is just the character.
    {
        *len = 1;
        return c;
    }
    else if ( ( c & 0xE0 ) == 0xC0 && ( s[1] & 0xc0 ) == 0x80 )   // a sequence of 110xxxxx and 10xxxxxx?
    {
        *len = 2;
        return ( ( ( c & 0x1f ) << 6 ) | ( s[1] & 0x3f ) );
    }
    else if ( ( c & 0xF0 ) == 0xE0 && ( s[1] & 0xc0 ) == 0x80 && ( s[2] & 0xc0 ) == 0x80 )    // a sequence of 1110xxxx,10xxxxxx and 10xxxxxx ?
    {
        *len = 3;
        return ( ( ( c & 0xf ) << 12 ) | ( ( s[1] & 0x3f ) << 6 ) |
                 ( s[2] & 0x3f ) );
    }
    else if ( ( c & 0xf8 ) == 0xf0 && ( s[1] & 0xc0 ) == 0x80 && ( s[2] & 0xc0 ) == 0x80 && ( s[3] & 0xc0 ) == 0x80 )     // a sequence of 11110xxx,10xxxxxx,10xxxxxx and 10xxxxxx ?
    {
        *len = 4;
        return ( ( ( c & 0x7 ) << 18 ) | ( ( s[1] & 0x3f ) << 12 ) |
                 ( ( s[2] & 0x3f ) << 6 ) | ( s[3] & 0x3f ) );
    }
    else if ( ( c & 0xfc ) == 0xf8 && ( s[1] & 0xc0 ) == 0x80 && ( s[2] & 0xc0 ) == 0x80 && ( s[3] & 0xc0 ) == 0x80 && ( s[4] & 0xc0 ) == 0x80 )  // a sequence of 111110xx,10xxxxxx,10xxxxxx,10xxxxxx,10xxxxxx ?
    {
        *len = 5;
        return ( ( ( c & 0x3 ) << 24 ) | ( ( s[1] & 0x3f ) << 18 ) |
                 ( ( s[2] & 0x3f ) << 12 ) | ( ( s[3] & 0x3f ) << 6 ) |
                 ( s[4] & 0x3f ) );
    }
    else if ( ( c & 0xfe ) == 0xfc && ( s[1] & 0xc0 ) == 0x80 && ( s[2] & 0xc0 ) == 0x80 && ( s[3] & 0xc0 ) == 0x80 && ( s[4] & 0xc0 ) == 0x80 && ( s[5] & 0xc0 ) == 0x80 )   // a sequence of 1111110x,10xxxxxx,10xxxxxx,10xxxxxx,10xxxxxx and 10xxxxxx ?
    {
        *len = 6;
        return ( ( ( c & 0x1 ) << 30 ) | ( ( s[1] & 0x3f ) << 24 ) |
                 ( ( s[2] & 0x3f ) << 18 ) | ( ( s[3] & 0x3f ) << 12 ) |
                 ( ( s[4] & 0x3f ) << 6 ) | ( s[5] & 0x3f ) );
    }
    else                      // none of above, error
    {
        *len = 0;
        return -1;
    }
}

/*==============================================================================*
*   ParserInit
*       Initializes a xml parser.
*       Internal to parser only
*
*===============================================================================*/
static XMLParser *


#ifdef USE_CWMP_MEMORY_POOL
ParserInit( Pool * pool )

#else
ParserInit(  )
#endif

{
    XMLParser *newParser = NULL;

    newParser = ( XMLParser * ) PMALLOC( sizeof( XMLParser ) );
    if ( newParser == NULL )
    {
        return NULL;
    }

    memset( newParser, 0, sizeof( XMLParser ) );

    XmlBufferInit( &( newParser->tokenBuf ) );
    XmlBufferInit( &( newParser->lastElem ) );

    return newParser;
}

/*================================================================
*   XMLParserIsValidEndElement
*       check if a new node->nodeName matches top of element stack.
*       Internal to parser only.
*
*=================================================================*/
static int
XMLParserIsValidEndElement( XMLParser * xmlParser,
                            XmlNode *   newNode )
{
    return ( strcmp( xmlParser->pCurElement->element, newNode->nodeName )
             == 0 );
}

/*===============================================================
*   Parser_pushElement
*       push a new element onto element stack
*       Internal to parser only.
*
*=================================================================*/
static int
#ifdef USE_CWMP_MEMORY_POOL
Parser_pushElement(Pool * pool, XMLParser * xmlParser,
#else
Parser_pushElement( XMLParser * xmlParser,
#endif


                   XmlNode *   newElement )
{

    XmlElementStack *pCurElement = NULL;
    XmlElementStack *pNewStackElement = NULL;

    assert( newElement );
    if ( newElement != NULL )
    {
        // push new element
        pNewStackElement =
            ( XmlElementStack * ) PMALLOC( sizeof( XmlElementStack ) );
        if ( pNewStackElement == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }

        memset( pNewStackElement, 0, sizeof( XmlElementStack ) );
        // the element member includes both prefix and name

        pNewStackElement->element = PSTRDUP( newElement->nodeName );
        if ( pNewStackElement->element == NULL )
        {
            PFREE( pNewStackElement );
            return XML_INSUFFICIENT_MEMORY;
        }

        if ( newElement->prefix != 0 )
        {
            pNewStackElement->prefix = PSTRDUP( newElement->prefix );
            if ( pNewStackElement->prefix == NULL )
            {
                XMLParserFreeElementStackItem(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    pNewStackElement );
                PFREE( pNewStackElement );
                return XML_INSUFFICIENT_MEMORY;
            }
        }

        if ( newElement->namespaceURI != 0 )
        {
            pNewStackElement->namespaceUri =
                PSTRDUP( newElement->namespaceURI );
            if ( pNewStackElement->namespaceUri == NULL )
            {
                XMLParserFreeElementStackItem(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    pNewStackElement );
                PFREE( pNewStackElement );
                return XML_INSUFFICIENT_MEMORY;
            }
        }

        pCurElement = xmlParser->pCurElement;

        // insert the new element into the top of the stack
        pNewStackElement->nextElement = pCurElement;
        xmlParser->pCurElement = pNewStackElement;

    }

    return XML_OK;
}

/*================================================================
*   ParserPopElement
*       Remove element from element stack.
*       Internal to parser only.
*
*=================================================================*/
static void
ParserPopElement(
    Pool * pool,
    XMLParser * xmlParser )
{
    XmlElementStack *pCur = NULL;
    XmlNameSpaceURI *pnsUri = NULL,
                              *pNextNS = NULL;

    pCur = xmlParser->pCurElement;
    if ( pCur != NULL )
    {
        xmlParser->pCurElement = pCur->nextElement;

        XMLParserFreeElementStackItem(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            pCur );

        pnsUri = pCur->pNsURI;
        while ( pnsUri != NULL )
        {
            pNextNS = pnsUri->nextNsURI;

            XMLParserFreeNsURI(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                pnsUri );
            PFREE( pnsUri );
            pnsUri = pNextNS;
        }

        PFREE( pCur );
    }

}

/*================================================================
*   ParserReadFileOrBuffer
*       read a xml file or buffer contents into xml parser.
*       Internal to parser only.
*
*=================================================================*/
static int

#ifdef USE_CWMP_MEMORY_POOL
ParserReadFileOrBuffer(Pool * pool, XMLParser * xmlParser,
#else
ParserReadFileOrBuffer( XMLParser * xmlParser,
#endif


                       IN char *xmlFileName,
                       IN BOOL file )
{
    int fileSize = 0;
    int bytesRead = 0;
    FILE *xmlFilePtr = NULL;

    if ( file )
    {
        xmlFilePtr = fopen( xmlFileName, "rb" );
        if ( xmlFilePtr == NULL )
        {
            return XML_NO_SUCH_FILE;
        }
        else
        {
            fseek( xmlFilePtr, 0, SEEK_END );
            fileSize = ftell( xmlFilePtr );
            if ( fileSize == 0 )
            {
                fclose( xmlFilePtr );
                return XML_SYNTAX_ERR;
            }

            xmlParser->dataBuffer = ( char * )PMALLOC( fileSize + 1 );
            if ( xmlParser->dataBuffer == NULL )
            {
                fclose( xmlFilePtr );
                return XML_INSUFFICIENT_MEMORY;
            }

            fseek( xmlFilePtr, 0, SEEK_SET );
            bytesRead =
                fread( xmlParser->dataBuffer, 1, fileSize, xmlFilePtr );
            xmlParser->dataBuffer[bytesRead] = '\0';    // append null
            fclose( xmlFilePtr );
        }
    }
    else
    {
        xmlParser->dataBuffer = PSTRDUP( xmlFileName );
        if ( xmlParser->dataBuffer == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
    }

    return XML_OK;
}

/*================================================================
*   XmlParserLoadDocument
*       parses a xml file and return the DOM tree.
*       Internal to parser only
*
*=================================================================*/
int
XmlParserLoadDocument(
    Pool * pool,
    OUT XmlDocument ** retDoc,
    IN char *xmlFileName,
    IN BOOL file )
{
    int rc = XML_OK;
    XMLParser *xmlParser = NULL;

    xmlParser = ParserInit(
#ifdef USE_CWMP_MEMORY_POOL
                    pool
#endif

                );
    if ( xmlParser == NULL )
    {
        return XML_INSUFFICIENT_MEMORY;
    }

    rc = ParserReadFileOrBuffer(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             xmlParser, xmlFileName, file );
    if ( rc != XML_OK )
    {
        XMLParserFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            xmlParser );
        return rc;
    }

    xmlParser->curPtr = xmlParser->dataBuffer;
    rc = XMLParserParseDocument(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif

             retDoc, xmlParser );
    return rc;

}

/*================================================================
*   isTopLevelElement
*       decides whether we have top level element already.
*       Internal to parser only.
*
*=================================================================*/
static int
isTopLevelElement( XMLParser * xmlParser )
{
    assert( xmlParser );
    return ( xmlParser->pCurElement == NULL );
}

/*================================================================
*   isDuplicateAttribute
*       Decide whether the new attribute is the same as an
*       existing one.
*       Internal to parser only.
*
*=================================================================*/
static int
isDuplicateAttribute( XMLParser * xmlParser,
                      XmlNode *   newAttrNode )
{
    XmlNode *  elementNode = NULL;
    XmlNode *  attrNode = NULL;

    elementNode = xmlParser->currentNodePtr;
    attrNode = elementNode->firstAttr;
    while ( attrNode != NULL )
    {
        if ( strcmp( attrNode->nodeName, newAttrNode->nodeName ) == 0 )
        {
            return TRUE;
        }

        attrNode = attrNode->nextSibling;
    }

    return FALSE;
}

/*================================================================
*   XMLParserProcessAttributeName
*       processes the attribute name.
*       Internal to parser only.
*
*=================================================================*/
static int
XMLParserProcessAttributeName(
    Pool * pool,
    XmlDocument * rootDoc,
    XMLParser * xmlParser,
    XmlNode *   newNode )
{
    XmlAttribute *attr = NULL;
    int rc = XML_OK;

    if ( isDuplicateAttribute( xmlParser, newNode ) == TRUE )
    {
        return XML_SYNTAX_ERR;
    }

    rc = XmlDocCreateAttributeEx(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             rootDoc, newNode->nodeName,
             &attr );
    if ( rc != XML_OK )
    {
        return rc;
    }

    rc = XmlNodeSetNodeProperties(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             ( XmlNode *   ) attr, newNode );
    if ( rc != XML_OK )
    {
        return rc;
    }

    rc = XmlElementSetAttributeNode(

             ( XmlElement * ) xmlParser->currentNodePtr, attr, NULL );
    return rc;
}

/*================================================================
*   ParserProcessElementName
*       Processes element name
*       Internal to parser only.
*
*=================================================================*/
static int
ParserProcessElementName(
    Pool * pool,

    XmlDocument * rootDoc,
    XMLParser * xmlParser,
    XmlNode *   newNode )
{
    XmlElement *newElement = NULL;
    char *nsURI = NULL;
    int rc = XML_OK;

    if ( xmlParser->bHasTopLevel == TRUE )
    {
        if ( isTopLevelElement( xmlParser ) == TRUE )
        {
            return XML_SYNTAX_ERR;
        }
    }
    else
    {
        xmlParser->bHasTopLevel = TRUE;
    }

    xmlParser->savePtr = xmlParser->curPtr;
    rc = XmlDocCreateElementEx(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             rootDoc, newNode->nodeName,
             &newElement );
    if ( rc != XML_OK )
    {
        return rc;
    }

    rc = XmlNodeSetNodeProperties(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             ( XmlNode *   ) newElement, newNode );
    if ( rc != XML_OK )
    {
        XmlElementFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newElement );
        return rc;
    }

    if ( newNode->prefix != NULL )  // element has namespace prefix
    {
        if ( XMLParserElementPrefixDefined( xmlParser, newNode, &nsURI ) !=
                TRUE )
        {
            // read next node to see whether it includes namespace definition
            xmlParser->pNeedPrefixNode = ( XmlNode *   ) newElement;
        }
        else                  // fill in the namespace
        {
            XMLParserSetElementNamespace(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                newElement, nsURI );
        }
    }
    else                      // does element has default namespace
    {
        // the node may have default namespace definition
        if ( XMLParserHasDefaultNamespace( xmlParser, newNode, &nsURI ) ==
                TRUE )
        {
            XMLParserSetElementNamespace(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                newElement, nsURI );
        }
        else if ( xmlParser->state == E_ATTRIBUTE )
        {
            // the default namespace maybe defined later
            xmlParser->pNeedPrefixNode = ( XmlNode *   ) newElement;
        }
    }

    rc = XmlNodeAppendChild( xmlParser->currentNodePtr,
                             ( XmlNode *   ) newElement );
    if ( rc != XML_OK )
    {
        XmlElementFree(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            newElement );
        return rc;
    }

    xmlParser->currentNodePtr = ( XmlNode *   ) newElement;

    // push element to stack
    rc = Parser_pushElement(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif

             xmlParser, ( XmlNode *   ) newElement );
    return rc;
}

/*================================================================
*   ParserETagVerification
*       Verifies endof element tag is the same as the openning
*       element tag.
*       Internal to parser only.
*
*=================================================================*/
static int
ParserETagVerification(
    Pool * pool,
    XMLParser * xmlParser,
    XmlNode *   newNode )
{

    assert( newNode->nodeName );
    assert( xmlParser->currentNodePtr );

    if ( newNode->nodeType == XML_ELEMENT_NODE )
    {
        if ( XMLParserIsValidEndElement( xmlParser, newNode ) == TRUE )
        {
            ParserPopElement(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser );
        }
        else                  // syntax error
        {
            return XML_SYNTAX_ERR;
        }
    }

    if ( strcmp( newNode->nodeName, xmlParser->currentNodePtr->nodeName ) ==
            0 )
    {
        xmlParser->currentNodePtr = xmlParser->currentNodePtr->parentNode;
    }
    else
    {
        return XML_SYNTAX_ERR;
    }

    return XML_OK;

}

/*================================================================
*   XmlParserFreeNodeContent
*       XFREEs a node contents
*       Internal to parser only.
*
*=================================================================*/
void
XmlParserFreeNodeContent(
    Pool * pool,
    XmlNode *   nodeptr )
{

    if ( nodeptr == NULL )
    {
        return;
    }

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

}

/*================================================================
*   XMLParserParseDocument
*       Parses the xml file and returns the DOM document tree.
*       External function.
*
*=================================================================*/
static int
XMLParserParseDocument(
    Pool * pool,
    OUT XmlDocument ** retDoc,
    XMLParser * xmlParser )
{

    XmlDocument *gRootDoc = NULL;
    XmlNode newNode;
    BOOL bETag = FALSE;
    XmlNode *  tempNode = NULL;
    int rc = XML_OK;
    XmlCDATASection *cdataSecNode = NULL;

    XmlNodeInit( &newNode );

    rc = XmlDocCreateDocumentEx(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif

             &gRootDoc );
    if ( rc != XML_OK )
    {
        goto ErrorHandler;
    }

    xmlParser->currentNodePtr = ( XmlNode *   ) gRootDoc;

    rc = XMLParserSkipProlog( xmlParser );
    if ( rc != XML_OK )
    {
        goto ErrorHandler;
    }

    while ( bETag == FALSE )
    {
        // clear the newNode contents
        XmlNodeInit( &newNode );

        if ( XMLParserGetNextNode(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    xmlParser, &newNode, &bETag ) ==
                XML_OK )
        {
            if ( bETag == FALSE )
            {
                switch ( newNode.nodeType )
                {
                case XML_ELEMENT_NODE:
                    rc = ParserProcessElementName(
#ifdef USE_CWMP_MEMORY_POOL
                             pool ,
#endif
                             gRootDoc,
                             xmlParser,
                             &newNode );
                    if ( rc != XML_OK )
                    {
                        goto ErrorHandler;
                    }
                    break;

                case XML_TEXT_NODE:
                    rc = XmlDocCreateTextNodeEx(
#ifdef USE_CWMP_MEMORY_POOL
                             pool ,
#endif

                             gRootDoc,
                             newNode.
                             nodeValue,
                             &tempNode );
                    if ( rc != XML_OK )
                    {
                        goto ErrorHandler;
                    }

                    rc = XmlNodeAppendChild( xmlParser->
                                             currentNodePtr,
                                             tempNode );
                    if ( rc != XML_OK )
                    {
                        goto ErrorHandler;
                    }

                    break;

                case XML_CDATA_SECTION_NODE:
                    rc = XmlDocCreateCDATASectionEx(
#ifdef USE_CWMP_MEMORY_POOL
                             pool ,
#endif

                             gRootDoc,
                             newNode.
                             nodeValue,
                             &cdataSecNode );
                    if ( rc != XML_OK )
                    {
                        goto ErrorHandler;
                    }

                    rc = XmlNodeAppendChild( xmlParser->
                                             currentNodePtr,
                                             &( cdataSecNode->node ) );
                    if ( rc != XML_OK )
                    {
                        goto ErrorHandler;
                    }
                    break;

                case XML_ATTRIBUTE_NODE:
                    rc = XMLParserProcessAttributeName(
#ifdef USE_CWMP_MEMORY_POOL
                             pool ,
#endif
                             gRootDoc,
                             xmlParser,
                             &newNode );
                    if ( rc != XML_OK )
                    {
                        goto ErrorHandler;
                    }
                    break;

                default:
                    break;
                }
            }
            else              // ETag==TRUE, endof element tag.
            {
                rc = ParserETagVerification(
#ifdef USE_CWMP_MEMORY_POOL
                         pool ,
#endif
                         xmlParser, &newNode );
                if ( rc != XML_OK )
                {
                    goto ErrorHandler;
                }
                xmlParser->state = E_CONTENT;
            }

            // reset bETag flag
            bETag = FALSE;

        }
        else if ( bETag == TRUE )     // file is done
        {
            break;
        }
        else
        {
            rc = XML_FAILED;
            goto ErrorHandler;
        }
        XmlParserFreeNodeContent(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            &newNode );

    }

    if ( xmlParser->pCurElement != NULL )
    {
        rc = XML_SYNTAX_ERR;
        goto ErrorHandler;
    }

    *retDoc = ( XmlDocument * ) gRootDoc;
    XMLParserFree(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        xmlParser );
    return rc;

ErrorHandler:
    XmlParserFreeNodeContent(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        &newNode );
    XmlDocumentFree(

#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        gRootDoc );
    XMLParserFree(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        xmlParser );
    return rc;

}

/*==============================================================================*
*   XMLParserSetLastElem
*       set the last element to be the given string.
*       Internal to parser only.
*
*===============================================================================*/
static int
XMLParserSetLastElem(
    Pool * pool,

    XMLParser * xmlParser,
    IN const char *s )
{
    int rc;

    if ( ( xmlParser == NULL ) || ( s == NULL ) )
    {
        return XML_FAILED;
    }

    rc = XmlBufferAssignString(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif

             &( xmlParser->lastElem ), s );
    return rc;
}

/*==============================================================================*
*
*   ParserClearTokenBuf
*       clear token buffer.
*       Internal to parser only.
*
*===============================================================================*/
static void
ParserClearTokenBuf(
    Pool * pool,
    XMLParser * xmlParser )
{
    XmlBufferDestroy(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        &( xmlParser->tokenBuf ) );
}

/*==============================================================================*
*
*   ParserAppendTokBufStr
*       Appends string s to token buffer
*       Internal to parser only.
*
*===============================================================================*/
static int
ParserAppendTokBufStr(
    Pool * pool,

    XMLParser * xmlParser,
    IN const char *s )
{
    int rc = XML_OK;

    if ( s != NULL )
    {
        rc = XmlBufferAppendString(
#ifdef USE_CWMP_MEMORY_POOL
                 pool ,
#endif

                 &( xmlParser->tokenBuf ), s );
    }

    return rc;
}

/*==============================================================================*
*
*   ParserAppendTokBufChar
*       Appends c to token buffer.
*       Internal to parser only.
*
*===============================================================================*/
static int
ParserAppendTokBufChar(
    Pool * pool,
    XMLParser * xmlParser,
    IN char c )
{
    int rc;

    rc = XmlBufferAppend(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif

             &( xmlParser->tokenBuf ), &c );
    return rc;
}

/*==============================================================================*
*
*   XMLParserSkipWhiteSpaces
*       skip white spaces
*       Internal to parser only
*
*===============================================================================*/
static void
XMLParserSkipWhiteSpaces( XMLParser * xmlParser )
{
    while ( ( *( xmlParser->curPtr ) != 0 ) &&
            ( strchr( WHITESPACE, *( xmlParser->curPtr ) ) != NULL ) )
    {
        xmlParser->curPtr++;
    }

}

/*==============================================================================*
*   XMLParserGetChar
*       returns next char value and its length
*       Internal to parser only
*
*===============================================================================*/
static int
XMLParserGetChar( IN char *src,
                  INOUT int *cLen )
{
    char *pnum;
    int sum;
    char c;
    int i;

    if ( src == NULL || cLen == NULL )
    {
        return -1;
    }

    *cLen = 0;

    if ( *src != '&' )
    {
        if ( *src > 0 && XMLParserIsXmlChar( *src ) )
        {
            *cLen = 1;
            return *src;
        }

        i = Parser_UTF8ToInt( src, cLen );
        if ( !XMLParserIsXmlChar( i ) )
        {
            return -1;
        }
        return i;
    }
    else if ( XMLStrncasecmp( src, QUOT, strlen( QUOT ) ) == 0 )
    {
        *cLen = strlen( QUOT );
        return '"';
    }
    else if ( XMLStrncasecmp( src, LT, strlen( LT ) ) == 0 )
    {
        *cLen = strlen( LT );
        return '<';
    }
    else if ( XMLStrncasecmp( src, GT, strlen( GT ) ) == 0 )
    {
        *cLen = strlen( GT );
        return '>';
    }
    else if ( XMLStrncasecmp( src, APOS, strlen( APOS ) ) == 0 )
    {
        *cLen = strlen( APOS );
        return '\'';
    }
    else if ( XMLStrncasecmp( src, AMP, strlen( AMP ) ) == 0 )
    {
        *cLen = strlen( AMP );
        return '&';
    }
    else if ( XMLStrncasecmp( src, ESC_HEX, strlen( ESC_HEX ) ) == 0 )   // Read in escape characters of type &#xnn where nn is a hexadecimal value
    {
        pnum = src + strlen( ESC_HEX );
        sum = 0;
        while ( strchr( HEX_NUMBERS, *pnum ) != 0 )
        {
            c = *pnum;
            if ( c <= '9' )
            {
                sum = sum * 16 + ( c - '0' );
            }
            else if ( c <= 'F' )
            {
                sum = sum * 16 + ( c - 'A' + 10 );
            }
            else
            {
                sum = sum * 16 + ( c - 'a' + 10 );
            }

            pnum++;
        }

        if ( ( pnum == src ) || *pnum != ';' || !XMLParserIsXmlChar( sum ) )
        {
            return -1;
        }

        *cLen = pnum - src + 1;
        return sum;

    }
    else if ( XMLStrncasecmp( src, ESC_DEC, strlen( ESC_DEC ) ) == 0 )
    {
        // Read in escape characters of type &#nn where nn is a decimal value
        pnum = src + strlen( ESC_DEC );
        sum = 0;
        while ( strchr( DEC_NUMBERS, *pnum ) != 0 )
        {
            sum = sum * 10 + ( *pnum - '0' );
            pnum++;
        }

        if ( ( pnum == src ) || *pnum != ';' || !XMLParserIsXmlChar( sum ) )
        {
            return -1;
        }

        *cLen = pnum - src + 1;
        return sum;
    }

    return -1;
}

/*==============================================================================*
*   XMLParserCopyToken
*       copy string in src into xml parser token buffer
*		Internal to parser only.
*
*===============================================================================*/
static int
XMLParserCopyToken(
    Pool * pool,

    XMLParser * xmlParser,
    IN char *src,
    IN int len )
{
    int i,
    c,
    cl;
    char *psrc,
    *pend;
    utf8char uch;

    if ( !src || len <= 0 )
    {
        return XML_FAILED;
    }

    psrc = src;
    pend = src + len;

    while ( psrc < pend )
    {
        if ( ( c = XMLParserGetChar( psrc, &cl ) ) <= 0 )
        {
            return XML_FAILED;
        }

        if ( cl == 1 )
        {
            ParserAppendTokBufChar(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                xmlParser, ( char )c );
            psrc++;
        }
        else
        {

            i = XMLParserIntToUTF8( c, uch );
            if ( i == 0 )
            {
                return XML_FAILED;
            }

            ParserAppendTokBufStr(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser, uch );
            psrc += cl;
        }
    }

    if ( psrc > pend )
    {
        return XML_FAILED;
    }
    else
    {
        return XML_OK;    // success
    }

}

/*==============================================================================*
*
*   XMLParserSkipString
*       Skips all characters in the string until it finds the skip key.
*       Then it skips the skip key and returns.
*       Internal to parser only
*
*===============================================================================*/
static int
XMLParserSkipString( INOUT char **pstrSrc,
                     IN const char *strSkipKey )
{
    if ( !( *pstrSrc ) || !strSkipKey )
    {
        return XML_FAILED;
    }

    while ( ( **pstrSrc )
            && strncmp( *pstrSrc, strSkipKey,
                        strlen( strSkipKey ) ) != 0 )
    {
        ( *pstrSrc )++;
    }

    if ( **pstrSrc == '\0' )
    {
        return XML_SYNTAX_ERR;
    }
    *pstrSrc = *pstrSrc + strlen( strSkipKey );

    return XML_OK;        //success
}

/*==============================================================================*
*
* Function:
* Returns:
*
*
*===============================================================================*/
static int
XMLParserSkipPI( INOUT char **pSrc )
{
    char *pEnd = NULL;

    assert( *pSrc );
    if ( *pSrc == NULL )
    {
        return XML_FAILED;
    }

    if ( ( XMLStrncasecmp( *pSrc, ( char * )XMLDECL, strlen( XMLDECL ) ) == 0 ) || ( XMLStrncasecmp( *pSrc, ( char * )XMLDECL2, strlen( XMLDECL2 ) ) == 0 ) )     // not allowed
    {
        return XML_SYNTAX_ERR;
    }

    if ( XMLStrncasecmp( *pSrc, ( char * )BEGIN_PI, strlen( BEGIN_PI ) ) == 0 )
    {
        pEnd = strstr( *pSrc, END_PI );
        if ( ( pEnd != NULL ) && ( pEnd != *pSrc ) )
        {
            *pSrc = pEnd + strlen( BEGIN_PI );
        }
        else
        {
            return XML_SYNTAX_ERR;
        }
    }

    return XML_OK;
}

/*==============================================================================*
*   XMLParserSkipXMLDecl:
*       skips XML declarations.
*       Internal only to parser.
*
*===============================================================================*/
static int
XMLParserSkipXMLDecl( INOUT XMLParser * xmlParser )
{
    int rc = XML_FAILED;

    assert( xmlParser );
    if ( xmlParser == NULL )
    {
        return rc;
    }

    rc = XMLParserSkipString( &( xmlParser->curPtr ), END_PI );
    XMLParserSkipWhiteSpaces( xmlParser );
    return rc;

}

/*==============================================================================*
*   XMLParserSkipProlog
*       skip prolog
*       Internal to parser only.
*
*===============================================================================*/
static int
XMLParserSkipProlog( INOUT XMLParser * xmlParser )
{
    int rc = XML_OK;

    assert( xmlParser != NULL );
    if ( xmlParser == NULL )
    {
        return XML_FAILED;
    }

    XMLParserSkipWhiteSpaces( xmlParser );

    if ( strncmp( xmlParser->curPtr, ( char * )XMLDECL, strlen( XMLDECL ) ) == 0 )  // <?xml
    {
        rc = XMLParserSkipXMLDecl( xmlParser );
        if ( rc != XML_OK )
        {
            return rc;
        }
    }

    rc = XMLParserSkipMisc( xmlParser );
    if ( ( rc == XML_OK ) && strncmp( xmlParser->curPtr, ( char * )BEGIN_DOCTYPE, strlen( BEGIN_DOCTYPE ) ) == 0 )    // <! DOCTYPE
    {
        xmlParser->curPtr++;
        rc = XMLParserSkipDocType( &( xmlParser->curPtr ) );
    }

    if ( rc == XML_OK )
    {
        rc = XMLParserSkipMisc( xmlParser );
    }

    return rc;
}

/*==============================================================================*
*
* Function:
* Returns:
*       Skips all characters in the string until it finds the skip key.
*       Then it skips the skip key and returns.
*
*===============================================================================*/
static int
XMLParserSkipComment( INOUT char **pstrSrc )
{
    char *pStrFound = NULL;

    assert( ( *pstrSrc ) != NULL );
    if ( *pstrSrc == NULL )
    {
        return XML_FAILED;
    }

    pStrFound = strstr( *pstrSrc, END_COMMENT );
    if ( ( pStrFound != NULL ) && ( pStrFound != *pstrSrc ) &&
            ( *( pStrFound - 1 ) != '-' ) )
    {
        *pstrSrc = pStrFound + strlen( END_COMMENT );
    }
    else
    {
        return XML_SYNTAX_ERR;
    }

    return XML_OK;
}

/*==============================================================================*
*   XMLParserSkipDocType
*       skips document type declaration
*
*===============================================================================*/
static int
XMLParserSkipDocType( INOUT char **pstr )
{
    char *pCur = *pstr;
    char *pNext = NULL;         // default there is no nested <
    int num = 1;

    assert( ( *pstr ) != NULL );
    if ( *pstr == NULL )
    {
        return XML_FAILED;
    }

    while ( ( pCur != NULL ) && ( num != 0 ) && ( *pCur != 0 ) )
    {
        if ( *pCur == '<' )
        {
            num++;
        }
        else if ( *pCur == '>' )
        {
            num--;
        }
        else if ( *pCur == '"' )
        {
            pNext = strchr( pCur + 1, '"' );
            if ( pNext == NULL )
            {
                return XML_SYNTAX_ERR;
            }

            pCur = pNext;
        }

        pCur++;
    }

    if ( num == 0 )
    {
        *pstr = pCur;
        return XML_OK;
    }
    else
    {
        return XML_SYNTAX_ERR;
    }
}

/*==============================================================================*
*
*   XMLParserSkipMisc:
*       skip comment, PI and white space
*
*
*===============================================================================*/
static int
XMLParserSkipMisc( XMLParser * xmlParser )
{
    int rc = XML_OK;
    int done = FALSE;

    while ( ( done == FALSE ) && ( rc == XML_OK ) )
    {
        if ( XMLStrncasecmp( xmlParser->curPtr, ( char * )BEGIN_COMMENT, strlen( BEGIN_COMMENT ) ) == 0 )  // <!--
        {
            rc = XMLParserSkipComment( &( xmlParser->curPtr ) );

        }
        else if ( ( XMLStrncasecmp( xmlParser->curPtr, ( char * )XMLDECL, strlen( XMLDECL ) ) == 0 ) || ( XMLStrncasecmp( xmlParser->curPtr, ( char * )XMLDECL2, strlen( XMLDECL2 ) ) == 0 ) )  // <?xml or <?xml?
        {
            rc = XML_SYNTAX_ERR;
        }
        else if ( XMLStrncasecmp( xmlParser->curPtr, ( char * )BEGIN_PI, strlen( BEGIN_PI ) ) == 0 )     // <?
        {
            rc = XMLParserSkipString( &( xmlParser->curPtr ), END_PI );
        }
        else
        {
            done = TRUE;
        }

        XMLParserSkipWhiteSpaces( xmlParser );
    }

    return rc;
}

/*==============================================================================*
*
*   XMLParserGetNextToken
*       return the length of next token in tokenBuff
*
*
*===============================================================================*/
static int
XMLParserGetNextToken(
    Pool * pool,
    XMLParser * xmlParser )
{
    int tokenLength = 0;
    int temp,
    tlen;
    int rc;

    ParserClearTokenBuf(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        xmlParser );

    if ( *( xmlParser->curPtr ) == '\0' )
    {
        return 0;
    }
    // skip XML instructions
    rc = XMLParserSkipMisc( xmlParser );
    if ( rc != XML_OK )
    {
        return 0;
    }
    // Attribute value logic must come first, since all text untokenized until end-quote
    if ( *( xmlParser->curPtr ) == QUOTE )
    {
        tokenLength = 1;
    }
    else if ( *( xmlParser->curPtr ) == SINGLEQUOTE )
    {
        tokenLength = 1;
    }
    else if ( *( xmlParser->curPtr ) == LESSTHAN )    // Check for start tags
    {
        temp = Parser_UTF8ToInt( xmlParser->curPtr + 1, &tlen );
        if ( temp == '/' )
        {
            tokenLength = 2;    // token is '</' end tag
        }
        else if ( XMLParserIsNameChar( temp, FALSE ) == TRUE )
        {
            tokenLength = 1;    // '<' found, so return '<' token
        }
        else
        {
            return 0;           //error
        }
    }
    else if ( *( xmlParser->curPtr ) == EQUALS )  // Check for '=' token, return it as a token
    {
        tokenLength = 1;
    }
    else if ( *( xmlParser->curPtr ) == SLASH )
    {
        if ( *( xmlParser->curPtr + 1 ) == GREATERTHAN )    // token '/>' found
        {
            tokenLength = 2;
            xmlParser->savePtr = xmlParser->curPtr; // fix
        }
    }
    else if ( *( xmlParser->curPtr ) == GREATERTHAN )     // > found, so return it as a token
    {
        tokenLength = 1;
    }
    else if ( XMLParserIsNameChar( Parser_UTF8ToInt( xmlParser->curPtr, &tlen ), FALSE ) )  // Check for name tokens, name found, so find out how long it is
    {
        int iIndex = tlen;

        while ( XMLParserIsNameChar
                ( Parser_UTF8ToInt( xmlParser->curPtr + iIndex, &tlen ),
                  TRUE ) )
        {
            iIndex += tlen;
        }
        tokenLength = iIndex;
    }
    else
    {
        return 0;
    }

    // Copy the token to the return string
    if ( XMLParserCopyToken(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser, xmlParser->curPtr, tokenLength ) !=
            XML_OK )
    {
        return 0;
    }

    xmlParser->curPtr += tokenLength;
    return tokenLength;
}

/*==============================================================================*
*
*   XMLParserGetNameSpace
*       return the namespce as defined as prefix.
*       Internal to parser only
*
*===============================================================================*/
static char *
XMLParserGetNameSpace( XMLParser * xmlParser,
                       IN char *prefix )
{
    XmlElementStack *pCur;
    XmlNameSpaceURI *pNsUri;

    pCur = xmlParser->pCurElement;
    if ( strcmp( pCur->prefix, prefix ) != 0 )
    {
        pNsUri = pCur->pNsURI;
        while ( pNsUri != NULL )
        {
            if ( strcmp( pNsUri->prefix, prefix ) == 0 )
            {
                return pNsUri->nsURI;
            }
            pNsUri = pNsUri->nextNsURI;
        }
    }
    else
    {
        return pCur->namespaceUri;
    }

    return NULL;

}

/*==============================================================================*
*
*   ParserAddNamespace
*       Add a namespace definition
*       Internal to parser only
*
*===============================================================================*/
static int
ParserAddNamespace(
    Pool * pool,
    XMLParser * xmlParser )
{
    XmlNode *  pNode;
    XmlElementStack *pCur;
    char *namespaceUri;

    pNode = xmlParser->pNeedPrefixNode;
    pCur = xmlParser->pCurElement;

    if ( pNode->prefix == NULL )    // element does not have prefix
    {
        if ( strcmp( pNode->nodeName, pCur->element ) != 0 )
        {
            return XML_FAILED;
        }
        if ( pCur->namespaceUri != NULL )
        {
            // it would be wrong that pNode->namespace != NULL.
            assert( pNode->namespaceURI == NULL );

            pNode->namespaceURI = PSTRDUP( pCur->namespaceUri );
            if ( pNode->namespaceURI == NULL )
            {
                return XML_INSUFFICIENT_MEMORY;
            }
        }

        xmlParser->pNeedPrefixNode = NULL;

    }
    else
    {
        if ( ( strcmp( pNode->nodeName, pCur->element ) != 0 ) &&
                ( strcmp( pNode->prefix, pCur->prefix ) != 0 ) )
        {
            return XML_FAILED;
        }

        namespaceUri = XMLParserGetNameSpace( xmlParser, pCur->prefix );
        if ( namespaceUri != NULL )
        {
            pNode->namespaceURI = PSTRDUP( namespaceUri );
            if ( pNode->namespaceURI == NULL )
            {
                return XML_INSUFFICIENT_MEMORY;
            }

            xmlParser->pNeedPrefixNode = NULL;
        }
    }
    return XML_OK;
}

/*==============================================================================*
*
*   XmlParserSetNodePrefixAndLocalName
*       set the node prefix and localName as defined by the nodeName
*       in the form of ns:name
*       Internal to parser only.
*
*===============================================================================*/
int
#ifdef USE_CWMP_MEMORY_POOL
XmlParserSetNodePrefixAndLocalName(Pool * pool, XmlNode *   node )
#else
XmlParserSetNodePrefixAndLocalName( XmlNode *   node )
#endif

{

    char *pStrPrefix = NULL;
    char *pLocalName;
    int nPrefix;

    assert( node != NULL );
    if ( node == NULL )
    {
        return XML_FAILED;
    }

    pStrPrefix = strchr( node->nodeName, ':' );
    if ( pStrPrefix == NULL )
    {
        node->prefix = NULL;
        node->localName = PSTRDUP( node->nodeName );
        if ( node->localName == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }

    }
    else                      // fill in the local name and prefix
    {

        pLocalName = ( char * )pStrPrefix + 1;
        nPrefix = pStrPrefix - node->nodeName;
        node->prefix = PMALLOC( nPrefix + 1 );
        if ( node->prefix == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }

        memset( node->prefix, 0, nPrefix + 1 );
        strncpy( node->prefix, node->nodeName, nPrefix );

        node->localName = PSTRDUP( pLocalName );
        if ( node->localName == NULL )
        {
            PFREE( node->prefix );
            node->prefix = NULL;    //no need to PFREE really, main loop will XFREEs it
            //when return code is not success
            return XML_INSUFFICIENT_MEMORY;
        }
    }

    return XML_OK;
}

/*==============================================================================*
*
*   XMLParserXmlNamespace
*       add namespace definition.
*       internal to parser only.
*
*===============================================================================*/
static int
#ifdef USE_CWMP_MEMORY_POOL
XMLParserXmlNamespace(Pool * pool, XMLParser * xmlParser,
#else
XMLParserXmlNamespace( XMLParser * xmlParser,
#endif

                      XmlNode *   newNode )
{

    XmlElementStack *pCur = xmlParser->pCurElement;
    XmlNameSpaceURI *pNewNs = NULL,
                              *pNs = NULL,
                                     *pPrevNs = NULL;
    int rc;

    // if the newNode contains a namespace definition
    assert( newNode->nodeName != NULL );

    if ( strcmp( newNode->nodeName, "xmlns" ) == 0 ) // default namespace def.
    {
        if ( pCur->namespaceUri != NULL )
        {
            PFREE( pCur->namespaceUri );
        }

#if 1 /* Jerry Lu Modified@Sep,14,2005 for fix PSTRDUP(NULL) cause Segment Fault */
        if (newNode->nodeValue==NULL)
            pCur->namespaceUri = PSTRDUP("");
        else
#endif
            pCur->namespaceUri = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                     pool ,
#endif
                                     newNode->nodeValue );

        if ( pCur->namespaceUri == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }

    }
    else if ( strncmp( newNode->nodeName, "xmlns:", strlen( "xmlns:" ) ) == 0 )   // namespace definition
    {
        rc = XmlParserSetNodePrefixAndLocalName(
#ifdef USE_CWMP_MEMORY_POOL
                 pool ,
#endif
                 newNode );
        if ( rc != XML_OK )
        {
            return rc;
        }

        assert( newNode->localName != NULL );

        if ( pCur == NULL )
        {
            return XML_FAILED;
        }

        if ( ( pCur->prefix != NULL )
                && ( strcmp( pCur->prefix, newNode->localName ) == 0 ) )
        {
            pCur->namespaceUri = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                     pool ,
#endif
                                     newNode->nodeValue );
            if ( pCur->namespaceUri == NULL )
            {
                return XML_INSUFFICIENT_MEMORY;
            }
        }
        else
        {
            pPrevNs = pCur->pNsURI;
            pNs = pPrevNs;
            while ( pNs != NULL )
            {
                if ( ( pNs->prefix != NULL ) &&
                        ( strcmp( pNs->prefix, newNode->localName ) == 0 ) )
                {
                    break;      // replace namespace definition
                }
                else
                {
                    pPrevNs = pNs;
                    pNs = pNs->nextNsURI;
                }
            }

            if ( pNs == NULL )  // a new definition
            {
                pNewNs =
                    ( XmlNameSpaceURI * )
                    PMALLOC( sizeof( XmlNameSpaceURI ) );
                if ( pNewNs == NULL )
                {
                    return XML_INSUFFICIENT_MEMORY;
                }
                memset( pNewNs, 0, sizeof( XmlNameSpaceURI ) );

                pNewNs->prefix = PSTRDUP( newNode->localName );
                if ( pNewNs->prefix == NULL )
                {
                    PFREE( pNewNs );
                    return XML_INSUFFICIENT_MEMORY;
                }

                pNewNs->nsURI = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                    pool ,
#endif
                                    newNode->nodeValue );
                if ( pNewNs->nsURI == NULL )
                {
                    XMLParserFreeNsURI(
#ifdef USE_CWMP_MEMORY_POOL
                        pool ,
#endif
                        pNewNs );
                    PFREE( pNewNs );
                    return XML_INSUFFICIENT_MEMORY;
                }

                if ( pCur->pNsURI == NULL )
                {
                    pCur->pNsURI = pNewNs;
                }
                else
                {
                    pPrevNs->nextNsURI = pNewNs;
                }
            }
            else              // udpate the namespace
            {
                if ( pNs->nsURI != NULL )
                {
                    PFREE( pNs->nsURI );
                }

                pNs->nsURI = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                 pool ,
#endif
                                 newNode->nodeValue );
                if ( pNs->nsURI == NULL )
                {
                    return XML_INSUFFICIENT_MEMORY;
                }
            }
        }
    }

    if ( xmlParser->pNeedPrefixNode != NULL )
    {
        rc = ParserAddNamespace(
#ifdef USE_CWMP_MEMORY_POOL
                 pool ,
#endif
                 xmlParser );
        return rc;
    }
    else
    {
        return XML_OK;
    }

}

/*==============================================================================*
*
*   ParserProcessSTag:
*       Processes the STag as defined by XML spec.
*       Internal to parser only.
*
*===============================================================================*/
static int
ParserProcessSTag(
    Pool * pool,
    XMLParser * xmlParser,
    XmlNode *   node )
{
    char *pCurToken = NULL;
    int rc;

    if ( XMLParserGetNextToken(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser ) == 0 )
    {
        return XML_SYNTAX_ERR;
    }

    pCurToken = ( xmlParser->tokenBuf ).buf;
    if ( pCurToken != NULL )
    {
        node->nodeName = PSTRDUP( pCurToken );
        if ( node->nodeName == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
    }
    else
    {
        return XML_SYNTAX_ERR;
    }

    rc = XMLParserSetLastElem(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif

             xmlParser, node->nodeName );
    if ( rc != XML_OK )   // no need to PFREE node->nodeName, main loop will PFREE it
    {
        return XML_FAILED;
    }

    rc = XmlParserSetNodePrefixAndLocalName(
#ifdef USE_CWMP_MEMORY_POOL
             pool ,
#endif
             node );
    if ( rc != XML_OK )   // no need to PFREE node->nodeName, main loop will PFREE it
    {
        return XML_FAILED;
    }

    node->nodeValue = NULL;
    node->nodeType = XML_ELEMENT_NODE;

    xmlParser->savePtr = xmlParser->curPtr;
    if ( XMLParserGetNextToken(

#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser ) == 0 )    // no need to PFREE node->nodeName, main loop will PFREE it
    {
        return XML_SYNTAX_ERR;
    }

    pCurToken = ( xmlParser->tokenBuf ).buf;
    // check to see what is the next token
    if ( strcmp( pCurToken, "/>" ) == 0 )   // empty element
    {
        xmlParser->state = E_ELEMENT;
        xmlParser->curPtr = xmlParser->savePtr; // backup to />
    }
    else if ( strcmp( pCurToken, ">" ) == 0 ) // expecting text node
    {
        xmlParser->state = E_CONTENT;
    }
    else
    {
        xmlParser->state = E_ATTRIBUTE;
        xmlParser->curPtr = xmlParser->savePtr;
    }

    return XML_OK;
}

/*==============================================================================*
*
*   XMLParserHasDefaultNamespace
*       decide whether the current element has default namespace
*       Internal to parser only.
*
*===============================================================================*/
static BOOL
XMLParserHasDefaultNamespace( XMLParser * xmlParser,
                              XmlNode *   newNode,
                              IN char **nsURI )
{
    XmlElementStack *pCur = xmlParser->pCurElement;

    while ( pCur != NULL )
    {
        if ( ( pCur->prefix == NULL ) && ( pCur->namespaceUri != NULL ) )
        {
            *nsURI = pCur->namespaceUri;
            return TRUE;
        }
        else
        {
            pCur = pCur->nextElement;
        }
    }

    return FALSE;

}

/*==============================================================================*
*
*   XMLParserElementPrefixDefined
*       decides whether element's prefix is already defined.
*       Internal to parser only.
*
*===============================================================================*/
static BOOL
XMLParserElementPrefixDefined( XMLParser * xmlParser,
                               XmlNode *   newNode,
                               IN char **nsURI )
{

    XmlElementStack *pCur = xmlParser->pCurElement;
    XmlNameSpaceURI *pNsUri;

    while ( pCur != NULL )
    {
        if ( ( pCur->prefix != NULL )
                && ( strcmp( pCur->prefix, newNode->prefix ) == 0 ) )
        {
            *nsURI = pCur->namespaceUri;
            return TRUE;
        }
        else
        {
            pNsUri = pCur->pNsURI;

            while ( pNsUri != NULL )
            {
                if ( strcmp( pNsUri->prefix, newNode->prefix ) == 0 )
                {
                    *nsURI = pNsUri->nsURI;
                    return TRUE;
                }
                else
                {
                    pNsUri = pNsUri->nextNsURI;
                }
            }
        }

        pCur = pCur->nextElement;

    }

    return FALSE;

}

/*==============================================================================*
*
*   ParserProcessCDSect
*       Processes CDSection as defined by XML spec.
*       Internal to parser only.
*
*===============================================================================*/
static int
#ifdef USE_CWMP_MEMORY_POOL
ParserProcessCDSect(Pool * pool, IN char **pSrc,
#else
ParserProcessCDSect( IN char **pSrc,
#endif

                    XmlNode *   node )
{

    char *pEnd;
    int tokenLength = 0;
    char *pCDataStart;

    if ( *pSrc == NULL )
    {
        return XML_FAILED;
    }

    pCDataStart = *pSrc + strlen( CDSTART );
    pEnd = pCDataStart;
    while ( ( XMLParserIsXmlChar( *pEnd ) == TRUE ) && ( *pEnd != '0' ) )
    {
        if ( strncmp( pEnd, CDEND, strlen( CDEND ) ) == 0 )
        {
            break;
        }
        else
        {
            pEnd++;
        }
    }

    if ( ( pEnd - pCDataStart > 0 ) && ( *pEnd != '0' ) )
    {
        tokenLength = pEnd - pCDataStart;
        node->nodeValue = ( char * )PMALLOC( tokenLength + 1 );
        if ( node->nodeValue == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
        strncpy( node->nodeValue, pCDataStart, tokenLength );
        node->nodeValue[tokenLength] = '\0';

        node->nodeName = PSTRDUP( CDATANODENAME );
        if ( node->nodeName == NULL )
        {
            // no need to PFREE node->nodeValue at all, bacause node contents
            // will be XFREEd by the main loop.
            return XML_INSUFFICIENT_MEMORY;
        }

        node->nodeType = XML_CDATA_SECTION_NODE;
        *pSrc = pEnd + strlen( CDEND );
        return XML_OK;
    }
    else
    {
        return XML_SYNTAX_ERR;
    }

}

/*==============================================================================*
*
*   XMLParserSetElementNamespace
*       set element's namespace
*       Internal to parser only.
*
*===============================================================================*/
static int
XMLParserSetElementNamespace(
    Pool * pool,
    XmlElement * newElement,
    IN char *nsURI )
{
    if ( newElement != NULL )
    {
        if ( newElement->node.namespaceURI != NULL )
        {
            return XML_SYNTAX_ERR;
        }
        else
        {
            if (nsURI == NULL)
            {
                return XML_NAMESPACE_ERR;
            }
            ( newElement->node ).namespaceURI = PSTRDUP( nsURI );
            if ( ( newElement->node ).namespaceURI == NULL )
            {
                return XML_INSUFFICIENT_MEMORY;
            }
        }
    }

    return XML_OK;
}

/*==============================================================================*
*
*   ParserProcessContent
*       processes the CONTENT as defined in XML spec.
*       Internal to parser only
*
*===============================================================================*/
static int
ParserProcessContent(
    Pool * pool,
    XMLParser * xmlParser,
    XmlNode *   node )
{
    char *pEndContent;
    BOOL bReadContent;
    int tokenLength;
    char *notAllowed = "]]>";
    char *pCurToken = NULL;

    // save pointer for backup
    xmlParser->savePtr = xmlParser->curPtr;
    XMLParserSkipWhiteSpaces( xmlParser );

    if ( *( xmlParser->curPtr ) == '\0' )   // end of file is reached
    {
        return XML_OK;
    }

    pEndContent = xmlParser->curPtr;
    if ( *pEndContent == LESSTHAN )
    {
        if ( strncmp( pEndContent, ( char * )CDSTART, strlen( CDSTART ) ) ==
                0 )
        {
            if ( ParserProcessCDSect(
#ifdef USE_CWMP_MEMORY_POOL
                        pool ,
#endif
                        &pEndContent, node ) !=
                    XML_OK )
            {
                return XML_SYNTAX_ERR;
            }
            else
            {
                xmlParser->curPtr = pEndContent;
            }
        }
        else
            if ( strncmp
                    ( pEndContent, ( char * )BEGIN_COMMENT,
                      strlen( BEGIN_COMMENT ) ) == 0 )
            {
                if ( XMLParserSkipComment( &pEndContent ) != XML_OK )
                {
                    return XML_SYNTAX_ERR;
                }
                else
                {
                    xmlParser->curPtr = pEndContent;
                }
            }
            else
                if ( strncmp
                        ( pEndContent, ( char * )BEGIN_PI,
                          strlen( BEGIN_PI ) ) == 0 )
                {
                    if ( XMLParserSkipPI( &pEndContent ) != XML_OK )
                    {
                        return XML_SYNTAX_ERR;
                    }
                    else
                    {
                        xmlParser->curPtr = pEndContent;
                    }
                }
                else                  // empty content
                {
                    xmlParser->state = E_ELEMENT;
                }
    }
    else
    {
        // backup
        xmlParser->curPtr = xmlParser->savePtr;
        pEndContent = xmlParser->curPtr;

        while ( ( *pEndContent != LESSTHAN ) &&
                ( strncmp
                  ( pEndContent, ( const char * )notAllowed,
                    strlen( notAllowed ) ) != 0 ) && *pEndContent )
        {
            pEndContent++;
        }

        if ( *pEndContent == '\0' )
        {
            bReadContent = FALSE;
        }

        if ( strncmp
                ( pEndContent, ( const char * )notAllowed,
                  strlen( notAllowed ) ) == 0 )
        {
            return XML_SYNTAX_ERR;
        }

        tokenLength = pEndContent - xmlParser->curPtr;
        ParserClearTokenBuf(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            xmlParser );

        if ( XMLParserCopyToken(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif
                    xmlParser, xmlParser->curPtr, tokenLength )
                != XML_OK )
        {
            return XML_SYNTAX_ERR;
        }

        pCurToken = ( xmlParser->tokenBuf ).buf;
        if ( pCurToken != NULL )
        {
            node->nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                                  pool ,
#endif
                                  pCurToken );
            if ( node->nodeValue == NULL )
            {
                return XML_INSUFFICIENT_MEMORY;
            }
        }
        else
        {
            return XML_SYNTAX_ERR;
        }

        node->nodeName = PSTRDUP( TEXTNODENAME );
        if ( node->nodeName == NULL )
        {
            return XML_SYNTAX_ERR;
        }
        node->nodeType = XML_TEXT_NODE;

        // adjust curPtr
        xmlParser->curPtr += tokenLength;

    }

    return XML_OK;
}

/*==============================================================================*
*
*   ParserProcessETag
*       process ETag as defined by XML spec.
*       Internal to parser only.
*
*===============================================================================*/
static int
ParserProcessETag(
    Pool * pool,
    XMLParser * xmlParser,
    XmlNode *   node,
    OUT BOOL * bETag )
{
    char *pCurToken = NULL;

    assert( xmlParser != NULL );
    if ( XMLParserGetNextToken(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser ) == 0 )
    {
        return XML_SYNTAX_ERR;
    }

    pCurToken = ( xmlParser->tokenBuf ).buf;
    if ( pCurToken == NULL )
    {
        return XML_SYNTAX_ERR;
    }
    node->nodeName = PSTRDUP( pCurToken );
    if ( node->nodeName == NULL )
    {
        return XML_INSUFFICIENT_MEMORY;
    }

    node->nodeValue = NULL;
    node->nodeType = XML_ELEMENT_NODE;

    XMLParserSkipWhiteSpaces( xmlParser );

    // read the >
    if ( XMLParserGetNextToken(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser ) == 0 )
    {
        return XML_SYNTAX_ERR;
    }

    pCurToken = ( xmlParser->tokenBuf ).buf;
    if ( pCurToken == NULL )    // no need to PFREE node->nodeName, it is XFREEd by main loop
    {
        return XML_SYNTAX_ERR;
    }

    if ( strcmp( pCurToken, ">" ) != 0 )
    {
        return XML_SYNTAX_ERR;
    }

    *bETag = TRUE;
    return XML_OK;
}

/*==============================================================================*
*
*   XMLParserFreeElementStackItem
*       XFREEs one ElementStack item.
*       Internal to parser only.
*
*===============================================================================*/
static void
XMLParserFreeElementStackItem(
    Pool * pool,
    XmlElementStack * pItem )
{
    assert( pItem != NULL );
    if ( pItem->element != NULL )
    {
        PFREE( pItem->element );
        pItem->element = NULL;
    }
    if ( pItem->namespaceUri != NULL )
    {
        PFREE( pItem->namespaceUri );
        pItem->namespaceUri = NULL;
    }
    if ( pItem->prefix != NULL )
    {
        PFREE( pItem->prefix );
        pItem->prefix = NULL;
    }

}

/*==============================================================================*
*
*   XMLParserFreeNsURI
*       XFREEs namespaceURI item.
*       Internal to parser only.
*
*===============================================================================*/
static void
XMLParserFreeNsURI(
    Pool * pool,
    XmlNameSpaceURI * pNsURI )
{
    assert( pNsURI != NULL );
    if ( pNsURI->nsURI != NULL )
    {
        PFREE( pNsURI->nsURI );
    }
    if ( pNsURI->prefix != NULL )
    {
        PFREE( pNsURI->prefix );
    }
}

/*==============================================================================*
*
*   XMLParserFree
*       XFREEs all temporary memory allocated by xmlparser.
*       Internal to parser only
*
*
*===============================================================================*/
static void
XMLParserFree(
    Pool * pool,
    XMLParser * xmlParser )
{
    XmlElementStack *pElement,
    *pNextElement;
    XmlNameSpaceURI *pNsURI,
    *pNextNsURI;

    if ( xmlParser == NULL )
    {
        return;
    }

    if ( xmlParser->dataBuffer != NULL )
    {
        PFREE( xmlParser->dataBuffer );
    }

    XmlBufferDestroy(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        &( xmlParser->tokenBuf ) );
    XmlBufferDestroy(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        &( xmlParser->lastElem ) );

    pElement = xmlParser->pCurElement;
    while ( pElement != NULL )
    {
        XMLParserFreeElementStackItem(
#ifdef USE_CWMP_MEMORY_POOL
            pool ,
#endif
            pElement );

        pNsURI = pElement->pNsURI;
        while ( pNsURI != NULL )
        {
            pNextNsURI = pNsURI->nextNsURI;
            XMLParserFreeNsURI(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                pNsURI );
            PFREE( pNsURI );
            pNsURI = pNextNsURI;
        }

        pNextElement = pElement->nextElement;
        PFREE( pElement );
        pElement = pNextElement;
    }

    PFREE( xmlParser );

}

/*==============================================================================*
*
*   XMLParserParseReference
*       return XML_OK or not
*
*
*===============================================================================*/
static int
XMLParserParseReference( IN char *pStr )
{
    // place holder for future implementation
    return XML_OK;
}

/*==============================================================================*
*
*   XMLParserProcessAttribute
*       processes attribute.
*       Internal to parser only.
*       returns XML_OK or failure
*
*
*===============================================================================*/
static int
XMLParserProcessAttribute(
    Pool * pool,

    XMLParser * xmlParser,
    XmlNode *   node )
{

    char *strEndQuote = NULL;
    int tlen = 0;
    char *pCur = NULL;
    char *pCurToken = NULL;

    assert( xmlParser );
    if ( xmlParser == NULL )
    {
        return XML_FAILED;
    }

    pCurToken = ( xmlParser->tokenBuf ).buf;
    if ( pCurToken == NULL )
    {
        return XML_SYNTAX_ERR;
    }

    if ( XMLParserIsNameChar( Parser_UTF8ToInt( pCurToken, &tlen ), FALSE ) ==
            FALSE )
    {
        return XML_SYNTAX_ERR;
    }
    // copy in the attribute name
    node->nodeName = PSTRDUP( pCurToken );
    if ( node->nodeName == NULL )
    {
        return XML_INSUFFICIENT_MEMORY;
    }
    // read in the "=" sign
    if ( XMLParserGetNextToken(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser ) == 0 )
    {
        return XML_SYNTAX_ERR;
    }

    pCurToken = ( xmlParser->tokenBuf ).buf;
    if ( *pCurToken != EQUALS )
    {
        return XML_SYNTAX_ERR;
    }
    // read in the single quote or double quote
    if ( XMLParserGetNextToken(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif
                xmlParser ) == 0 )
    {
        return XML_SYNTAX_ERR;
    }
    // pCurToken is either quote or singlequote
    pCurToken = ( xmlParser->tokenBuf ).buf;
    if ( ( *pCurToken != QUOTE ) && ( *pCurToken != SINGLEQUOTE ) )
    {
        return XML_SYNTAX_ERR;
    }

    strEndQuote = strstr( xmlParser->curPtr, pCurToken );
    if ( strEndQuote == NULL )
    {
        return XML_SYNTAX_ERR;
    }
    // check between curPtr and strEndQuote, whether there are illegal chars.
    pCur = xmlParser->curPtr;
    while ( pCur < strEndQuote )
    {
        if ( *pCur == '<' )
        {
            return XML_SYNTAX_ERR;
        }

        if ( *pCur == '&' )
        {
            XMLParserParseReference( ++pCur );
        }
        pCur++;
    }
    //clear token buffer
    ParserClearTokenBuf(
#ifdef USE_CWMP_MEMORY_POOL
        pool ,
#endif
        xmlParser );
    if ( strEndQuote != xmlParser->curPtr )
    {
        if ( XMLParserCopyToken(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    xmlParser, xmlParser->curPtr,
                    strEndQuote - xmlParser->curPtr ) !=
                XML_OK )
        {
            return XML_SYNTAX_ERR;
        }
    }
    // skip the ending quote
    xmlParser->curPtr = strEndQuote + 1;

    pCurToken = ( xmlParser->tokenBuf ).buf;
    if ( pCurToken != NULL )    // attribute has value, like a="c"
    {
        node->nodeValue = XmlStrduptrim(
#ifdef USE_CWMP_MEMORY_POOL
                              pool ,
#endif
                              pCurToken );
        if ( node->nodeValue == NULL )
        {
            return XML_INSUFFICIENT_MEMORY;
        }
    }
    // if attribute doesn't have value, like a="", fine
    node->nodeType = XML_ATTRIBUTE_NODE;

    // check whether this is a new namespace definition
    if ( XMLParserXmlNamespace(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                xmlParser, node ) != XML_OK )
    {
        return XML_FAILED;
    }
    // read ahead to see whether we have more attributes
    xmlParser->savePtr = xmlParser->curPtr;
    if ( XMLParserGetNextToken(
#ifdef USE_CWMP_MEMORY_POOL
                pool ,
#endif

                xmlParser ) == 0 )
    {
        return XML_SYNTAX_ERR;
    }

    pCurToken = ( xmlParser->tokenBuf ).buf;
    if ( strcmp( pCurToken, "<" ) == 0 )
    {
        return XML_FAILED;
    }
    else if ( strcmp( pCurToken, ">" ) != 0 ) // more attribute?
    {
        // backup
        xmlParser->curPtr = xmlParser->savePtr;
    }
    else
    {
        xmlParser->state = E_CONTENT;
    }

    return XML_OK;
}

/*==============================================================================*
*
*   XMLParserGetNextNode
*       return next node
*   returns XML_OK or
*
*
*===============================================================================*/
static int
XMLParserGetNextNode(
    Pool * pool,
    XMLParser * xmlParser,
    OUT XmlNode *   node,
    OUT BOOL * bETag )
{
    char *pCurToken = NULL;
    char *lastElement = NULL;

    // endof file reached?
    if ( *( xmlParser->curPtr ) == '\0' )
    {
        *bETag = TRUE;
        return XML_FILE_DONE;
    }

    if ( xmlParser->state == E_CONTENT )
    {
        if ( ParserProcessContent(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    xmlParser, node ) != XML_OK )
        {
            return XML_FAILED;
        }
    }
    else
    {
        XMLParserSkipWhiteSpaces( xmlParser );

        if ( ( XMLParserGetNextToken(
#ifdef USE_CWMP_MEMORY_POOL
                    pool ,
#endif

                    xmlParser ) == 0 ) && ( xmlParser->pCurElement == NULL ) && ( *( xmlParser->curPtr ) == '\0' ) )    // comments after the xml doc
        {
            return XML_OK;
        }
        else if ( ( xmlParser->tokenBuf ).length == 0 )
        {
            return XML_SYNTAX_ERR;
        }

        pCurToken = ( xmlParser->tokenBuf ).buf;
        if ( *pCurToken == GREATERTHAN )
        {
            return XML_OK;
        }
        else if ( strcmp( pCurToken, ENDTAG ) == 0 )  //  we got </, read next element
        {
            return ParserProcessETag(
#ifdef USE_CWMP_MEMORY_POOL
                       pool ,
#endif

                       xmlParser, node, bETag );
        }
        else if ( *pCurToken == LESSTHAN )
        {
            return ParserProcessSTag(
#ifdef USE_CWMP_MEMORY_POOL
                       pool ,
#endif
                       xmlParser, node );
        }
        else if ( strcmp( pCurToken, COMPLETETAG ) == 0 )
        {
            lastElement = ( xmlParser->lastElem ).buf;
            if ( lastElement == NULL )
            {
                goto ErrorHandler;
            }

            node->nodeName = PSTRDUP( lastElement );
            if ( node->nodeName == NULL )
            {
                return XML_INSUFFICIENT_MEMORY;
            }
            node->nodeType = XML_ELEMENT_NODE;
            *bETag = TRUE;

            return XML_OK;
        }
        else if ( xmlParser->state == E_ATTRIBUTE )
        {
            if ( XMLParserProcessAttribute(
#ifdef USE_CWMP_MEMORY_POOL
                        pool ,
#endif
                        xmlParser, node ) !=
                    XML_OK )
            {
                return XML_SYNTAX_ERR;
            }
        }
        else
        {
            return XML_SYNTAX_ERR;
        }
    }

    return XML_OK;

ErrorHandler:

    return XML_SYNTAX_ERR;

}
