// baenet_httprequestheaderfields.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httprequestheaderfields_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httprequestheaderfields.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <baenet_httphost.h>
#include <bdet_datetimetz.h>
#include <bdeut_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                  // ----------------------------------------                  
                  // class baenet_HttpRequestHeaderFields                  
                  // ----------------------------------------                  

// CONSTANTS

const char baenet_HttpRequestHeaderFields::CLASS_NAME[]
                                            = "baenet_HttpRequestHeaderFields";
const bdeat_AttributeInfo
                     baenet_HttpRequestHeaderFields::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ACCEPT,
        "Accept",
        sizeof("Accept") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ACCEPT_CHARSET,
        "Accept-Charset",
        sizeof("Accept-Charset") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ACCEPT_ENCODING,
        "Accept-Encoding",
        sizeof("Accept-Encoding") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ACCEPT_LANGUAGE,
        "Accept-Language",
        sizeof("Accept-Language") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_AUTHORIZATION,
        "Authorization",
        sizeof("Authorization") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_EXPECT,
        "Expect",
        sizeof("Expect") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_FROM,
        "From",
        sizeof("From") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_HOST,
        "Host",
        sizeof("Host") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_IF_MATCH,
        "If-Match",
        sizeof("If-Match") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_IF_MODIFIED_SINCE,
        "If-Modified-Since",
        sizeof("If-Modified-Since") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_IF_NONE_MATCH,
        "If-None-Match",
        sizeof("If-None-Match") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_IF_RANGE,
        "If-Range",
        sizeof("If-Range") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_IF_UNMODIFIED_SINCE,
        "If-Unmodified-Since",
        sizeof("If-Unmodified-Since") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_MAX_FORWARDS,
        "Max-Forwards",
        sizeof("Max-Forwards") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_PROXY_AUTHORIZATION,
        "Proxy-Authorization",
        sizeof("Proxy-Authorization") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_RANGE,
        "Range",
        sizeof("Range") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_REFERER,
        "Referer",
        sizeof("Referer") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_TE,
        "TE",
        sizeof("TE") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_USER_AGENT,
        "User-Agent",
        sizeof("User-Agent") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *baenet_HttpRequestHeaderFields::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 2: {
            if ((name[0]|0x20)=='t'
             && (name[1]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TE];
            }
        } break;
        case 4: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'F': {
                    if ((name[1]|0x20)=='r'
                     && (name[2]|0x20)=='o'
                     && (name[3]|0x20)=='m')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FROM];
                    }
                } break;
                case 'H': {
                    if ((name[1]|0x20)=='o'
                     && (name[2]|0x20)=='s'
                     && (name[3]|0x20)=='t')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOST];
                    }
                } break;
            }
        } break;
        case 5: {
            if ((name[0]|0x20)=='r'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='n'
             && (name[3]|0x20)=='g'
             && (name[4]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RANGE];
            }
        } break;
        case 6: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'A': {
                    if ((name[1]|0x20)=='c'
                     && (name[2]|0x20)=='c'
                     && (name[3]|0x20)=='e'
                     && (name[4]|0x20)=='p'
                     && (name[5]|0x20)=='t')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT];
                    }
                } break;
                case 'E': {
                    if ((name[1]|0x20)=='x'
                     && (name[2]|0x20)=='p'
                     && (name[3]|0x20)=='e'
                     && (name[4]|0x20)=='c'
                     && (name[5]|0x20)=='t')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPECT];
                    }
                } break;
            }
        } break;
        case 7: {
            if ((name[0]|0x20)=='r'
             && (name[1]|0x20)=='e'
             && (name[2]|0x20)=='f'
             && (name[3]|0x20)=='e'
             && (name[4]|0x20)=='r'
             && (name[5]|0x20)=='e'
             && (name[6]|0x20)=='r')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REFERER];
            }
        } break;
        case 8: {
            if ((name[0]|0x20)=='i'
             && (name[1]|0x20)=='f'
             && (name[2])=='-')
            {
                switch(bdeu_CharType::toUpper(name[3])) {
                    case 'M': {
                        if ((name[4]|0x20)=='a'
                         && (name[5]|0x20)=='t'
                         && (name[6]|0x20)=='c'
                         && (name[7]|0x20)=='h')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[
                                                     ATTRIBUTE_INDEX_IF_MATCH];
                        }
                    } break;
                    case 'R': {
                        if ((name[4]|0x20)=='a'
                         && (name[5]|0x20)=='n'
                         && (name[6]|0x20)=='g'
                         && (name[7]|0x20)=='e')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[
                                                     ATTRIBUTE_INDEX_IF_RANGE];
                        }
                    } break;
                }
            }
        } break;
        case 10: {
            if ((name[0]|0x20)=='u'
             && (name[1]|0x20)=='s'
             && (name[2]|0x20)=='e'
             && (name[3]|0x20)=='r'
             && (name[4])=='-'
             && (name[5]|0x20)=='a'
             && (name[6]|0x20)=='g'
             && (name[7]|0x20)=='e'
             && (name[8]|0x20)=='n'
             && (name[9]|0x20)=='t')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USER_AGENT];
            }
        } break;
        case 12: {
            if ((name[0]|0x20)=='m'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='x'
             && (name[3])=='-'
             && (name[4]|0x20)=='f'
             && (name[5]|0x20)=='o'
             && (name[6]|0x20)=='r'
             && (name[7]|0x20)=='w'
             && (name[8]|0x20)=='a'
             && (name[9]|0x20)=='r'
             && (name[10]|0x20)=='d'
             && (name[11]|0x20)=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FORWARDS];
            }
        } break;
        case 13: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'A': {
                    if ((name[1]|0x20)=='u'
                     && (name[2]|0x20)=='t'
                     && (name[3]|0x20)=='h'
                     && (name[4]|0x20)=='o'
                     && (name[5]|0x20)=='r'
                     && (name[6]|0x20)=='i'
                     && (name[7]|0x20)=='z'
                     && (name[8]|0x20)=='a'
                     && (name[9]|0x20)=='t'
                     && (name[10]|0x20)=='i'
                     && (name[11]|0x20)=='o'
                     && (name[12]|0x20)=='n')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[
                                                ATTRIBUTE_INDEX_AUTHORIZATION];
                    }
                } break;
                case 'I': {
                    if ((name[1]|0x20)=='f'
                     && (name[2])=='-'
                     && (name[3]|0x20)=='n'
                     && (name[4]|0x20)=='o'
                     && (name[5]|0x20)=='n'
                     && (name[6]|0x20)=='e'
                     && (name[7])=='-'
                     && (name[8]|0x20)=='m'
                     && (name[9]|0x20)=='a'
                     && (name[10]|0x20)=='t'
                     && (name[11]|0x20)=='c'
                     && (name[12]|0x20)=='h')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[
                                                ATTRIBUTE_INDEX_IF_NONE_MATCH];
                    }
                } break;
            }
        } break;
        case 14: {
            if ((name[0]|0x20)=='a'
             && (name[1]|0x20)=='c'
             && (name[2]|0x20)=='c'
             && (name[3]|0x20)=='e'
             && (name[4]|0x20)=='p'
             && (name[5]|0x20)=='t'
             && (name[6])=='-'
             && (name[7]|0x20)=='c'
             && (name[8]|0x20)=='h'
             && (name[9]|0x20)=='a'
             && (name[10]|0x20)=='r'
             && (name[11]|0x20)=='s'
             && (name[12]|0x20)=='e'
             && (name[13]|0x20)=='t')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_CHARSET];
            }
        } break;
        case 15: {
            if ((name[0]|0x20)=='a'
             && (name[1]|0x20)=='c'
             && (name[2]|0x20)=='c'
             && (name[3]|0x20)=='e'
             && (name[4]|0x20)=='p'
             && (name[5]|0x20)=='t'
             && (name[6])=='-')
            {
                switch(bdeu_CharType::toUpper(name[7])) {
                    case 'E': {
                        if ((name[8]|0x20)=='n'
                         && (name[9]|0x20)=='c'
                         && (name[10]|0x20)=='o'
                         && (name[11]|0x20)=='d'
                         && (name[12]|0x20)=='i'
                         && (name[13]|0x20)=='n'
                         && (name[14]|0x20)=='g')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[
                                              ATTRIBUTE_INDEX_ACCEPT_ENCODING];
                        }
                    } break;
                    case 'L': {
                        if ((name[8]|0x20)=='a'
                         && (name[9]|0x20)=='n'
                         && (name[10]|0x20)=='g'
                         && (name[11]|0x20)=='u'
                         && (name[12]|0x20)=='a'
                         && (name[13]|0x20)=='g'
                         && (name[14]|0x20)=='e')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[
                                              ATTRIBUTE_INDEX_ACCEPT_LANGUAGE];
                        }
                    } break;
                }
            }
        } break;
        case 17: {
            if ((name[0]|0x20)=='i'
             && (name[1]|0x20)=='f'
             && (name[2])=='-'
             && (name[3]|0x20)=='m'
             && (name[4]|0x20)=='o'
             && (name[5]|0x20)=='d'
             && (name[6]|0x20)=='i'
             && (name[7]|0x20)=='f'
             && (name[8]|0x20)=='i'
             && (name[9]|0x20)=='e'
             && (name[10]|0x20)=='d'
             && (name[11])=='-'
             && (name[12]|0x20)=='s'
             && (name[13]|0x20)=='i'
             && (name[14]|0x20)=='n'
             && (name[15]|0x20)=='c'
             && (name[16]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                                            ATTRIBUTE_INDEX_IF_MODIFIED_SINCE];
            }
        } break;
        case 19: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'I': {
                    if ((name[1]|0x20)=='f'
                     && (name[2])=='-'
                     && (name[3]|0x20)=='u'
                     && (name[4]|0x20)=='n'
                     && (name[5]|0x20)=='m'
                     && (name[6]|0x20)=='o'
                     && (name[7]|0x20)=='d'
                     && (name[8]|0x20)=='i'
                     && (name[9]|0x20)=='f'
                     && (name[10]|0x20)=='i'
                     && (name[11]|0x20)=='e'
                     && (name[12]|0x20)=='d'
                     && (name[13])=='-'
                     && (name[14]|0x20)=='s'
                     && (name[15]|0x20)=='i'
                     && (name[16]|0x20)=='n'
                     && (name[17]|0x20)=='c'
                     && (name[18]|0x20)=='e')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[
                                          ATTRIBUTE_INDEX_IF_UNMODIFIED_SINCE];
                    }
                } break;
                case 'P': {
                    if ((name[1]|0x20)=='r'
                     && (name[2]|0x20)=='o'
                     && (name[3]|0x20)=='x'
                     && (name[4]|0x20)=='y'
                     && (name[5])=='-'
                     && (name[6]|0x20)=='a'
                     && (name[7]|0x20)=='u'
                     && (name[8]|0x20)=='t'
                     && (name[9]|0x20)=='h'
                     && (name[10]|0x20)=='o'
                     && (name[11]|0x20)=='r'
                     && (name[12]|0x20)=='i'
                     && (name[13]|0x20)=='z'
                     && (name[14]|0x20)=='a'
                     && (name[15]|0x20)=='t'
                     && (name[16]|0x20)=='i'
                     && (name[17]|0x20)=='o'
                     && (name[18]|0x20)=='n')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[
                                          ATTRIBUTE_INDEX_PROXY_AUTHORIZATION];
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *
                    baenet_HttpRequestHeaderFields::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ACCEPT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT];
      case ATTRIBUTE_ID_ACCEPT_CHARSET:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_CHARSET];
      case ATTRIBUTE_ID_ACCEPT_ENCODING:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_ENCODING];
      case ATTRIBUTE_ID_ACCEPT_LANGUAGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_LANGUAGE];
      case ATTRIBUTE_ID_AUTHORIZATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AUTHORIZATION];
      case ATTRIBUTE_ID_EXPECT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPECT];
      case ATTRIBUTE_ID_FROM:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FROM];
      case ATTRIBUTE_ID_HOST:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOST];
      case ATTRIBUTE_ID_IF_MATCH:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MATCH];
      case ATTRIBUTE_ID_IF_MODIFIED_SINCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MODIFIED_SINCE];
      case ATTRIBUTE_ID_IF_NONE_MATCH:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_NONE_MATCH];
      case ATTRIBUTE_ID_IF_RANGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_RANGE];
      case ATTRIBUTE_ID_IF_UNMODIFIED_SINCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_UNMODIFIED_SINCE];
      case ATTRIBUTE_ID_MAX_FORWARDS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FORWARDS];
      case ATTRIBUTE_ID_PROXY_AUTHORIZATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHORIZATION];
      case ATTRIBUTE_ID_RANGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RANGE];
      case ATTRIBUTE_ID_REFERER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REFERER];
      case ATTRIBUTE_ID_TE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TE];
      case ATTRIBUTE_ID_USER_AGENT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USER_AGENT];
      default:
        return 0;
    }
}

// CREATORS

baenet_HttpRequestHeaderFields::baenet_HttpRequestHeaderFields(bslma_Allocator *basicAllocator)
: d_accept(basicAllocator)
, d_acceptCharset(basicAllocator)
, d_acceptEncoding(basicAllocator)
, d_acceptLanguage(basicAllocator)
, d_authorization(basicAllocator)
, d_expect(basicAllocator)
, d_from(basicAllocator)
, d_ifMatch(basicAllocator)
, d_ifNoneMatch(basicAllocator)
, d_ifRange(basicAllocator)
, d_proxyAuthorization(basicAllocator)
, d_range(basicAllocator)
, d_referer(basicAllocator)
, d_te(basicAllocator)
, d_userAgent(basicAllocator)
, d_ifModifiedSince()
, d_ifUnmodifiedSince()
, d_host(basicAllocator)
, d_maxForwards()
{
}

baenet_HttpRequestHeaderFields::baenet_HttpRequestHeaderFields(const baenet_HttpRequestHeaderFields& original,
                                                                       bslma_Allocator *basicAllocator)
: d_accept(original.d_accept, basicAllocator)
, d_acceptCharset(original.d_acceptCharset, basicAllocator)
, d_acceptEncoding(original.d_acceptEncoding, basicAllocator)
, d_acceptLanguage(original.d_acceptLanguage, basicAllocator)
, d_authorization(original.d_authorization, basicAllocator)
, d_expect(original.d_expect, basicAllocator)
, d_from(original.d_from, basicAllocator)
, d_ifMatch(original.d_ifMatch, basicAllocator)
, d_ifNoneMatch(original.d_ifNoneMatch, basicAllocator)
, d_ifRange(original.d_ifRange, basicAllocator)
, d_proxyAuthorization(original.d_proxyAuthorization, basicAllocator)
, d_range(original.d_range, basicAllocator)
, d_referer(original.d_referer, basicAllocator)
, d_te(original.d_te, basicAllocator)
, d_userAgent(original.d_userAgent, basicAllocator)
, d_ifModifiedSince(original.d_ifModifiedSince)
, d_ifUnmodifiedSince(original.d_ifUnmodifiedSince)
, d_host(original.d_host, basicAllocator)
, d_maxForwards(original.d_maxForwards)
{
}

baenet_HttpRequestHeaderFields::~baenet_HttpRequestHeaderFields()
{
}

// MANIPULATORS

baenet_HttpRequestHeaderFields&
baenet_HttpRequestHeaderFields::operator=(const baenet_HttpRequestHeaderFields& rhs)
{
    if (this != &rhs) {
        d_accept = rhs.d_accept;
        d_acceptCharset = rhs.d_acceptCharset;
        d_acceptEncoding = rhs.d_acceptEncoding;
        d_acceptLanguage = rhs.d_acceptLanguage;
        d_authorization = rhs.d_authorization;
        d_expect = rhs.d_expect;
        d_from = rhs.d_from;
        d_host = rhs.d_host;
        d_ifMatch = rhs.d_ifMatch;
        d_ifModifiedSince = rhs.d_ifModifiedSince;
        d_ifNoneMatch = rhs.d_ifNoneMatch;
        d_ifRange = rhs.d_ifRange;
        d_ifUnmodifiedSince = rhs.d_ifUnmodifiedSince;
        d_maxForwards = rhs.d_maxForwards;
        d_proxyAuthorization = rhs.d_proxyAuthorization;
        d_range = rhs.d_range;
        d_referer = rhs.d_referer;
        d_te = rhs.d_te;
        d_userAgent = rhs.d_userAgent;
    }
    return *this;
}

void baenet_HttpRequestHeaderFields::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_accept);
    bdeat_ValueTypeFunctions::reset(&d_acceptCharset);
    bdeat_ValueTypeFunctions::reset(&d_acceptEncoding);
    bdeat_ValueTypeFunctions::reset(&d_acceptLanguage);
    bdeat_ValueTypeFunctions::reset(&d_authorization);
    bdeat_ValueTypeFunctions::reset(&d_expect);
    bdeat_ValueTypeFunctions::reset(&d_from);
    bdeat_ValueTypeFunctions::reset(&d_host);
    bdeat_ValueTypeFunctions::reset(&d_ifMatch);
    bdeat_ValueTypeFunctions::reset(&d_ifModifiedSince);
    bdeat_ValueTypeFunctions::reset(&d_ifNoneMatch);
    bdeat_ValueTypeFunctions::reset(&d_ifRange);
    bdeat_ValueTypeFunctions::reset(&d_ifUnmodifiedSince);
    bdeat_ValueTypeFunctions::reset(&d_maxForwards);
    bdeat_ValueTypeFunctions::reset(&d_proxyAuthorization);
    bdeat_ValueTypeFunctions::reset(&d_range);
    bdeat_ValueTypeFunctions::reset(&d_referer);
    bdeat_ValueTypeFunctions::reset(&d_te);
    bdeat_ValueTypeFunctions::reset(&d_userAgent);
}

// ACCESSORS

bsl::ostream& baenet_HttpRequestHeaderFields::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Accept = ";
        bdeu_PrintMethods::print(stream, d_accept,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "AcceptCharset = ";
        bdeu_PrintMethods::print(stream, d_acceptCharset,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "AcceptEncoding = ";
        bdeu_PrintMethods::print(stream, d_acceptEncoding,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "AcceptLanguage = ";
        bdeu_PrintMethods::print(stream, d_acceptLanguage,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Authorization = ";
        bdeu_PrintMethods::print(stream, d_authorization,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Expect = ";
        bdeu_PrintMethods::print(stream, d_expect,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "From = ";
        bdeu_PrintMethods::print(stream, d_from,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Host = ";
        bdeu_PrintMethods::print(stream, d_host,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "IfMatch = ";
        bdeu_PrintMethods::print(stream, d_ifMatch,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "IfModifiedSince = ";
        bdeu_PrintMethods::print(stream, d_ifModifiedSince,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "IfNoneMatch = ";
        bdeu_PrintMethods::print(stream, d_ifNoneMatch,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "IfRange = ";
        bdeu_PrintMethods::print(stream, d_ifRange,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "IfUnmodifiedSince = ";
        bdeu_PrintMethods::print(stream, d_ifUnmodifiedSince,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MaxForwards = ";
        bdeu_PrintMethods::print(stream, d_maxForwards,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ProxyAuthorization = ";
        bdeu_PrintMethods::print(stream, d_proxyAuthorization,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Range = ";
        bdeu_PrintMethods::print(stream, d_range,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Referer = ";
        bdeu_PrintMethods::print(stream, d_referer,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Te = ";
        bdeu_PrintMethods::print(stream, d_te,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "UserAgent = ";
        bdeu_PrintMethods::print(stream, d_userAgent,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Accept = ";
        bdeu_PrintMethods::print(stream, d_accept,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "AcceptCharset = ";
        bdeu_PrintMethods::print(stream, d_acceptCharset,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "AcceptEncoding = ";
        bdeu_PrintMethods::print(stream, d_acceptEncoding,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "AcceptLanguage = ";
        bdeu_PrintMethods::print(stream, d_acceptLanguage,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Authorization = ";
        bdeu_PrintMethods::print(stream, d_authorization,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Expect = ";
        bdeu_PrintMethods::print(stream, d_expect,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "From = ";
        bdeu_PrintMethods::print(stream, d_from,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Host = ";
        bdeu_PrintMethods::print(stream, d_host,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "IfMatch = ";
        bdeu_PrintMethods::print(stream, d_ifMatch,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "IfModifiedSince = ";
        bdeu_PrintMethods::print(stream, d_ifModifiedSince,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "IfNoneMatch = ";
        bdeu_PrintMethods::print(stream, d_ifNoneMatch,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "IfRange = ";
        bdeu_PrintMethods::print(stream, d_ifRange,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "IfUnmodifiedSince = ";
        bdeu_PrintMethods::print(stream, d_ifUnmodifiedSince,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MaxForwards = ";
        bdeu_PrintMethods::print(stream, d_maxForwards,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ProxyAuthorization = ";
        bdeu_PrintMethods::print(stream, d_proxyAuthorization,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Range = ";
        bdeu_PrintMethods::print(stream, d_range,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Referer = ";
        bdeu_PrintMethods::print(stream, d_referer,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Te = ";
        bdeu_PrintMethods::print(stream, d_te,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "UserAgent = ";
        bdeu_PrintMethods::print(stream, d_userAgent,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Wed Feb 10 17:14:02 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
