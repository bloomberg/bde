// baenet_httpresponseheaderfields.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpresponseheaderfields_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httpresponseheaderfields.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bdeut_nullablevalue.h>
#include <bsl_string.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                 // -----------------------------------------                  
                 // class baenet_HttpResponseHeaderFields                  
                 // -----------------------------------------                  

// CONSTANTS

const char baenet_HttpResponseHeaderFields::CLASS_NAME[]
                                           = "baenet_HttpResponseHeaderFields";
const bdeat_AttributeInfo
                    baenet_HttpResponseHeaderFields::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ACCEPT_RANGES,
        "Accept-Ranges",
        sizeof("Accept-Ranges") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_AGE,
        "Age",
        sizeof("Age") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_E_TAG,
        "ETag",
        sizeof("ETag") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_LOCATION,
        "Location",
        sizeof("Location") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_PROXY_AUTHENTICATE,
        "Proxy-Authenticate",
        sizeof("Proxy-Authenticate") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_RETRY_AFTER,
        "Retry-After",
        sizeof("Retry-After") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_SERVER,
        "Server",
        sizeof("Server") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_VARY,
        "Vary",
        sizeof("Vary") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_WWW_AUTHENTICATE,
        "WWW-Authenticate",
        sizeof("WWW-Authenticate") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo
*baenet_HttpResponseHeaderFields::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if ((name[0]|0x20)=='a'
             && (name[1]|0x20)=='g'
             && (name[2]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
            }
        } break;
        case 4: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'E': {
                    if ((name[1]|0x20)=='t'
                     && (name[2]|0x20)=='a'
                     && (name[3]|0x20)=='g')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_E_TAG];
                    }
                } break;
                case 'V': {
                    if ((name[1]|0x20)=='a'
                     && (name[2]|0x20)=='r'
                     && (name[3]|0x20)=='y')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VARY];
                    }
                } break;
            }
        } break;
        case 6: {
            if ((name[0]|0x20)=='s'
             && (name[1]|0x20)=='e'
             && (name[2]|0x20)=='r'
             && (name[3]|0x20)=='v'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='r')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER];
            }
        } break;
        case 8: {
            if ((name[0]|0x20)=='l'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='c'
             && (name[3]|0x20)=='a'
             && (name[4]|0x20)=='t'
             && (name[5]|0x20)=='i'
             && (name[6]|0x20)=='o'
             && (name[7]|0x20)=='n')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LOCATION];
            }
        } break;
        case 11: {
            if ((name[0]|0x20)=='r'
             && (name[1]|0x20)=='e'
             && (name[2]|0x20)=='t'
             && (name[3]|0x20)=='r'
             && (name[4]|0x20)=='y'
             && (name[5])=='-'
             && (name[6]|0x20)=='a'
             && (name[7]|0x20)=='f'
             && (name[8]|0x20)=='t'
             && (name[9]|0x20)=='e'
             && (name[10]|0x20)=='r')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RETRY_AFTER];
            }
        } break;
        case 13: {
            if ((name[0]|0x20)=='a'
             && (name[1]|0x20)=='c'
             && (name[2]|0x20)=='c'
             && (name[3]|0x20)=='e'
             && (name[4]|0x20)=='p'
             && (name[5]|0x20)=='t'
             && (name[6])=='-'
             && (name[7]|0x20)=='r'
             && (name[8]|0x20)=='a'
             && (name[9]|0x20)=='n'
             && (name[10]|0x20)=='g'
             && (name[11]|0x20)=='e'
             && (name[12]|0x20)=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_RANGES];
            }
        } break;
        case 16: {
            if ((name[0]|0x20)=='w'
             && (name[1]|0x20)=='w'
             && (name[2]|0x20)=='w'
             && (name[3])=='-'
             && (name[4]|0x20)=='a'
             && (name[5]|0x20)=='u'
             && (name[6]|0x20)=='t'
             && (name[7]|0x20)=='h'
             && (name[8]|0x20)=='e'
             && (name[9]|0x20)=='n'
             && (name[10]|0x20)=='t'
             && (name[11]|0x20)=='i'
             && (name[12]|0x20)=='c'
             && (name[13]|0x20)=='a'
             && (name[14]|0x20)=='t'
             && (name[15]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WWW_AUTHENTICATE];
            }
        } break;
        case 18: {
            if ((name[0]|0x20)=='p'
             && (name[1]|0x20)=='r'
             && (name[2]|0x20)=='o'
             && (name[3]|0x20)=='x'
             && (name[4]|0x20)=='y'
             && (name[5])=='-'
             && (name[6]|0x20)=='a'
             && (name[7]|0x20)=='u'
             && (name[8]|0x20)=='t'
             && (name[9]|0x20)=='h'
             && (name[10]|0x20)=='e'
             && (name[11]|0x20)=='n'
             && (name[12]|0x20)=='t'
             && (name[13]|0x20)=='i'
             && (name[14]|0x20)=='c'
             && (name[15]|0x20)=='a'
             && (name[16]|0x20)=='t'
             && (name[17]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                                           ATTRIBUTE_INDEX_PROXY_AUTHENTICATE];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *
                   baenet_HttpResponseHeaderFields::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ACCEPT_RANGES:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_RANGES];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
      case ATTRIBUTE_ID_E_TAG:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_E_TAG];
      case ATTRIBUTE_ID_LOCATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LOCATION];
      case ATTRIBUTE_ID_PROXY_AUTHENTICATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHENTICATE];
      case ATTRIBUTE_ID_RETRY_AFTER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RETRY_AFTER];
      case ATTRIBUTE_ID_SERVER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER];
      case ATTRIBUTE_ID_VARY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VARY];
      case ATTRIBUTE_ID_WWW_AUTHENTICATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WWW_AUTHENTICATE];
      default:
        return 0;
    }
}

// CREATORS

baenet_HttpResponseHeaderFields::baenet_HttpResponseHeaderFields(bslma_Allocator *basicAllocator)
: d_acceptRanges(basicAllocator)
, d_eTag(basicAllocator)
, d_location(basicAllocator)
, d_proxyAuthenticate(basicAllocator)
, d_retryAfter(basicAllocator)
, d_server(basicAllocator)
, d_vary(basicAllocator)
, d_wwwAuthenticate(basicAllocator)
, d_age()
{
}

baenet_HttpResponseHeaderFields::baenet_HttpResponseHeaderFields(const baenet_HttpResponseHeaderFields& original,
                                                                         bslma_Allocator *basicAllocator)
: d_acceptRanges(original.d_acceptRanges, basicAllocator)
, d_eTag(original.d_eTag, basicAllocator)
, d_location(original.d_location, basicAllocator)
, d_proxyAuthenticate(original.d_proxyAuthenticate, basicAllocator)
, d_retryAfter(original.d_retryAfter, basicAllocator)
, d_server(original.d_server, basicAllocator)
, d_vary(original.d_vary, basicAllocator)
, d_wwwAuthenticate(original.d_wwwAuthenticate, basicAllocator)
, d_age(original.d_age)
{
}

baenet_HttpResponseHeaderFields::~baenet_HttpResponseHeaderFields()
{
}

// MANIPULATORS

baenet_HttpResponseHeaderFields&
baenet_HttpResponseHeaderFields::operator=(const baenet_HttpResponseHeaderFields& rhs)
{
    if (this != &rhs) {
        d_acceptRanges = rhs.d_acceptRanges;
        d_age = rhs.d_age;
        d_eTag = rhs.d_eTag;
        d_location = rhs.d_location;
        d_proxyAuthenticate = rhs.d_proxyAuthenticate;
        d_retryAfter = rhs.d_retryAfter;
        d_server = rhs.d_server;
        d_vary = rhs.d_vary;
        d_wwwAuthenticate = rhs.d_wwwAuthenticate;
    }
    return *this;
}

void baenet_HttpResponseHeaderFields::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_acceptRanges);
    bdeat_ValueTypeFunctions::reset(&d_age);
    bdeat_ValueTypeFunctions::reset(&d_eTag);
    bdeat_ValueTypeFunctions::reset(&d_location);
    bdeat_ValueTypeFunctions::reset(&d_proxyAuthenticate);
    bdeat_ValueTypeFunctions::reset(&d_retryAfter);
    bdeat_ValueTypeFunctions::reset(&d_server);
    bdeat_ValueTypeFunctions::reset(&d_vary);
    bdeat_ValueTypeFunctions::reset(&d_wwwAuthenticate);
}

// ACCESSORS

bsl::ostream& baenet_HttpResponseHeaderFields::print(
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
        stream << "AcceptRanges = ";
        bdeu_PrintMethods::print(stream, d_acceptRanges,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ETag = ";
        bdeu_PrintMethods::print(stream, d_eTag,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Location = ";
        bdeu_PrintMethods::print(stream, d_location,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ProxyAuthenticate = ";
        bdeu_PrintMethods::print(stream, d_proxyAuthenticate,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "RetryAfter = ";
        bdeu_PrintMethods::print(stream, d_retryAfter,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Server = ";
        bdeu_PrintMethods::print(stream, d_server,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Vary = ";
        bdeu_PrintMethods::print(stream, d_vary,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "WwwAuthenticate = ";
        bdeu_PrintMethods::print(stream, d_wwwAuthenticate,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "AcceptRanges = ";
        bdeu_PrintMethods::print(stream, d_acceptRanges,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Age = ";
        bdeu_PrintMethods::print(stream, d_age,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ETag = ";
        bdeu_PrintMethods::print(stream, d_eTag,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Location = ";
        bdeu_PrintMethods::print(stream, d_location,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ProxyAuthenticate = ";
        bdeu_PrintMethods::print(stream, d_proxyAuthenticate,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "RetryAfter = ";
        bdeu_PrintMethods::print(stream, d_retryAfter,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Server = ";
        bdeu_PrintMethods::print(stream, d_server,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Vary = ";
        bdeu_PrintMethods::print(stream, d_vary,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "WwwAuthenticate = ";
        bdeu_PrintMethods::print(stream, d_wwwAuthenticate,
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
