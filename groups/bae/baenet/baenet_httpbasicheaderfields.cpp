// baenet_httpbasicheaderfields.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpbasicheaderfields_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httpbasicheaderfields.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <baenet_httpcontenttype.h>
#include <baenet_httprequestmethod.h>
#include <baenet_httptransferencoding.h>
#include <baenet_httpviarecord.h>
#include <bdet_datetimetz.h>
#include <bdeut_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                   // --------------------------------------                   
                   // class baenet_HttpBasicHeaderFields                   
                   // --------------------------------------                   

// CONSTANTS

const char baenet_HttpBasicHeaderFields::CLASS_NAME[] =
                                                "baenet_HttpBasicHeaderFields";
const bdeat_AttributeInfo
                       baenet_HttpBasicHeaderFields::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CACHE_CONTROL,
        "Cache-Control",
        sizeof("Cache-Control") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_CONNECTION,
        "Connection",
        sizeof("Connection") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_DATE,
        "Date",
        sizeof("Date") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_PRAGMA,
        "Pragma",
        sizeof("Pragma") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_TRAILER,
        "Trailer",
        sizeof("Trailer") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_TRANSFER_ENCODING,
        "Transfer-Encoding",
        sizeof("Transfer-Encoding") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_UPGRADE,
        "Upgrade",
        sizeof("Upgrade") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_VIA,
        "Via",
        sizeof("Via") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_WARNING,
        "Warning",
        sizeof("Warning") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ALLOW,
        "Allow",
        sizeof("Allow") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_CONTENT_ENCODING,
        "Content-Encoding",
        sizeof("Content-Encoding") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_CONTENT_LANGUAGE,
        "Content-Language",
        sizeof("Content-Language") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_CONTENT_LENGTH,
        "Content-Length",
        sizeof("Content-Length") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_CONTENT_LOCATION,
        "Content-Location",
        sizeof("Content-Location") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_CONTENT_MD5,
        "Content-MD5",
        sizeof("Content-MD5") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_CONTENT_RANGE,
        "Content-Range",
        sizeof("Content-Range") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_CONTENT_TYPE,
        "Content-Type",
        sizeof("Content-Type") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_EXPIRES,
        "Expires",
        sizeof("Expires") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_LAST_MODIFIED,
        "Last-Modified",
        sizeof("Last-Modified") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *baenet_HttpBasicHeaderFields::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 3: {
            if ((name[0]|0x20)=='v'
             && (name[1]|0x20)=='i'
             && (name[2]|0x20)=='a')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VIA];
            }
        } break;
        case 4: {
            if ((name[0]|0x20)=='d'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='t'
             && (name[3]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE];
            }
        } break;
        case 5: {
            if ((name[0]|0x20)=='a'
             && (name[1]|0x20)=='l'
             && (name[2]|0x20)=='l'
             && (name[3]|0x20)=='o'
             && (name[4]|0x20)=='w')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW];
            }
        } break;
        case 6: {
            if ((name[0]|0x20)=='p'
             && (name[1]|0x20)=='r'
             && (name[2]|0x20)=='a'
             && (name[3]|0x20)=='g'
             && (name[4]|0x20)=='m'
             && (name[5]|0x20)=='a')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA];
            }
        } break;
        case 7: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'E': {
                    if ((name[1]|0x20)=='x'
                     && (name[2]|0x20)=='p'
                     && (name[3]|0x20)=='i'
                     && (name[4]|0x20)=='r'
                     && (name[5]|0x20)=='e'
                     && (name[6]|0x20)=='s')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPIRES];
                    }
                } break;
                case 'T': {
                    if ((name[1]|0x20)=='r'
                     && (name[2]|0x20)=='a'
                     && (name[3]|0x20)=='i'
                     && (name[4]|0x20)=='l'
                     && (name[5]|0x20)=='e'
                     && (name[6]|0x20)=='r')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRAILER];
                    }
                } break;
                case 'U': {
                    if ((name[1]|0x20)=='p'
                     && (name[2]|0x20)=='g'
                     && (name[3]|0x20)=='r'
                     && (name[4]|0x20)=='a'
                     && (name[5]|0x20)=='d'
                     && (name[6]|0x20)=='e')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UPGRADE];
                    }
                } break;
                case 'W': {
                    if ((name[1]|0x20)=='a'
                     && (name[2]|0x20)=='r'
                     && (name[3]|0x20)=='n'
                     && (name[4]|0x20)=='i'
                     && (name[5]|0x20)=='n'
                     && (name[6]|0x20)=='g')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WARNING];
                    }
                } break;
            }
        } break;
        case 10: {
            if ((name[0]|0x20)=='c'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='n'
             && (name[3]|0x20)=='n'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='c'
             && (name[6]|0x20)=='t'
             && (name[7]|0x20)=='i'
             && (name[8]|0x20)=='o'
             && (name[9]|0x20)=='n')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONNECTION];
            }
        } break;
        case 11: {
            if ((name[0]|0x20)=='c'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='n'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='n'
             && (name[6]|0x20)=='t'
             && (name[7])=='-'
             && (name[8]|0x20)=='m'
             && (name[9]|0x20)=='d'
             && (name[10])=='5')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_MD5];
            }
        } break;
        case 12: {
            if ((name[0]|0x20)=='c'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='n'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='n'
             && (name[6]|0x20)=='t'
             && (name[7])=='-'
             && (name[8]|0x20)=='t'
             && (name[9]|0x20)=='y'
             && (name[10]|0x20)=='p'
             && (name[11]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_TYPE];
            }
        } break;
        case 13: {
            switch(bdeu_CharType::toUpper(name[0])) {
                case 'C': {
                    switch(bdeu_CharType::toUpper(name[1])) {
                        case 'A': {
                            if ((name[2]|0x20)=='c'
                             && (name[3]|0x20)=='h'
                             && (name[4]|0x20)=='e'
                             && (name[5])=='-'
                             && (name[6]|0x20)=='c'
                             && (name[7]|0x20)=='o'
                             && (name[8]|0x20)=='n'
                             && (name[9]|0x20)=='t'
                             && (name[10]|0x20)=='r'
                             && (name[11]|0x20)=='o'
                             && (name[12]|0x20)=='l')
                            {
                                 return &ATTRIBUTE_INFO_ARRAY[
                                                ATTRIBUTE_INDEX_CACHE_CONTROL];
                            }
                        } break;
                        case 'O': {
                            if ((name[2]|0x20)=='n'
                             && (name[3]|0x20)=='t'
                             && (name[4]|0x20)=='e'
                             && (name[5]|0x20)=='n'
                             && (name[6]|0x20)=='t'
                             && (name[7])=='-'
                             && (name[8]|0x20)=='r'
                             && (name[9]|0x20)=='a'
                             && (name[10]|0x20)=='n'
                             && (name[11]|0x20)=='g'
                             && (name[12]|0x20)=='e')
                            {
                                return &ATTRIBUTE_INFO_ARRAY[
                                                ATTRIBUTE_INDEX_CONTENT_RANGE];
                            }
                        } break;
                    }
                } break;
                case 'L': {
                    if ((name[1]|0x20)=='a'
                     && (name[2]|0x20)=='s'
                     && (name[3]|0x20)=='t'
                     && (name[4])=='-'
                     && (name[5]|0x20)=='m'
                     && (name[6]|0x20)=='o'
                     && (name[7]|0x20)=='d'
                     && (name[8]|0x20)=='i'
                     && (name[9]|0x20)=='f'
                     && (name[10]|0x20)=='i'
                     && (name[11]|0x20)=='e'
                     && (name[12]|0x20)=='d')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[
                                                ATTRIBUTE_INDEX_LAST_MODIFIED];
                    }
                } break;
            }
        } break;
        case 14: {
            if ((name[0]|0x20)=='c'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='n'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='n'
             && (name[6]|0x20)=='t'
             && (name[7])=='-'
             && (name[8]|0x20)=='l'
             && (name[9]|0x20)=='e'
             && (name[10]|0x20)=='n'
             && (name[11]|0x20)=='g'
             && (name[12]|0x20)=='t'
             && (name[13]|0x20)=='h')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LENGTH];
            }
        } break;
        case 16: {
            if ((name[0]|0x20)=='c'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='n'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='n'
             && (name[6]|0x20)=='t'
             && (name[7])=='-')
            {
                switch(bdeu_CharType::toUpper(name[8])) {
                    case 'E': {
                        if ((name[9]|0x20)=='n'
                         && (name[10]|0x20)=='c'
                         && (name[11]|0x20)=='o'
                         && (name[12]|0x20)=='d'
                         && (name[13]|0x20)=='i'
                         && (name[14]|0x20)=='n'
                         && (name[15]|0x20)=='g')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[
                                             ATTRIBUTE_INDEX_CONTENT_ENCODING];
                        }
                    } break;
                    case 'L': {
                        switch(bdeu_CharType::toUpper(name[9])) {
                            case 'A': {
                                if ((name[10]|0x20)=='n'
                                 && (name[11]|0x20)=='g'
                                 && (name[12]|0x20)=='u'
                                 && (name[13]|0x20)=='a'
                                 && (name[14]|0x20)=='g'
                                 && (name[15]|0x20)=='e')
                                {
                                    return &ATTRIBUTE_INFO_ARRAY[
                                             ATTRIBUTE_INDEX_CONTENT_LANGUAGE];
                                }
                            } break;
                            case 'O': {
                                if ((name[10]|0x20)=='c'
                                 && (name[11]|0x20)=='a'
                                 && (name[12]|0x20)=='t'
                                 && (name[13]|0x20)=='i'
                                 && (name[14]|0x20)=='o'
                                 && (name[15]|0x20)=='n')
                                {
                                    return &ATTRIBUTE_INFO_ARRAY[
                                             ATTRIBUTE_INDEX_CONTENT_LOCATION];
                                }
                            } break;
                        }
                    } break;
                }
            }
        } break;
        case 17: {
            if ((name[0]|0x20)=='t'
             && (name[1]|0x20)=='r'
             && (name[2]|0x20)=='a'
             && (name[3]|0x20)=='n'
             && (name[4]|0x20)=='s'
             && (name[5]|0x20)=='f'
             && (name[6]|0x20)=='e'
             && (name[7]|0x20)=='r'
             && (name[8])=='-'
             && (name[9]|0x20)=='e'
             && (name[10]|0x20)=='n'
             && (name[11]|0x20)=='c'
             && (name[12]|0x20)=='o'
             && (name[13]|0x20)=='d'
             && (name[14]|0x20)=='i'
             && (name[15]|0x20)=='n'
             && (name[16]|0x20)=='g')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                                            ATTRIBUTE_INDEX_TRANSFER_ENCODING];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *baenet_HttpBasicHeaderFields::lookupAttributeInfo(
                                                                        int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CACHE_CONTROL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CACHE_CONTROL];
      case ATTRIBUTE_ID_CONNECTION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONNECTION];
      case ATTRIBUTE_ID_DATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE];
      case ATTRIBUTE_ID_PRAGMA:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA];
      case ATTRIBUTE_ID_TRAILER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRAILER];
      case ATTRIBUTE_ID_TRANSFER_ENCODING:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSFER_ENCODING];
      case ATTRIBUTE_ID_UPGRADE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UPGRADE];
      case ATTRIBUTE_ID_VIA:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VIA];
      case ATTRIBUTE_ID_WARNING:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WARNING];
      case ATTRIBUTE_ID_ALLOW:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW];
      case ATTRIBUTE_ID_CONTENT_ENCODING:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_ENCODING];
      case ATTRIBUTE_ID_CONTENT_LANGUAGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LANGUAGE];
      case ATTRIBUTE_ID_CONTENT_LENGTH:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LENGTH];
      case ATTRIBUTE_ID_CONTENT_LOCATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LOCATION];
      case ATTRIBUTE_ID_CONTENT_MD5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_MD5];
      case ATTRIBUTE_ID_CONTENT_RANGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_RANGE];
      case ATTRIBUTE_ID_CONTENT_TYPE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_TYPE];
      case ATTRIBUTE_ID_EXPIRES:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPIRES];
      case ATTRIBUTE_ID_LAST_MODIFIED:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_MODIFIED];
      default:
        return 0;
    }
}

// CREATORS

baenet_HttpBasicHeaderFields::baenet_HttpBasicHeaderFields(bslma_Allocator *basicAllocator)
: d_contentEncoding(basicAllocator)
, d_contentLanguage(basicAllocator)
, d_via(basicAllocator)
, d_cacheControl(basicAllocator)
, d_connection(basicAllocator)
, d_pragma(basicAllocator)
, d_trailer(basicAllocator)
, d_upgrade(basicAllocator)
, d_warning(basicAllocator)
, d_contentLocation(basicAllocator)
, d_contentMd5(basicAllocator)
, d_contentRange(basicAllocator)
, d_date()
, d_expires()
, d_lastModified()
, d_contentType(basicAllocator)
, d_transferEncoding(basicAllocator)
, d_allow(basicAllocator)
, d_contentLength()
{
}

baenet_HttpBasicHeaderFields::baenet_HttpBasicHeaderFields(const baenet_HttpBasicHeaderFields& original,
                                                                   bslma_Allocator *basicAllocator)
: d_contentEncoding(original.d_contentEncoding, basicAllocator)
, d_contentLanguage(original.d_contentLanguage, basicAllocator)
, d_via(original.d_via, basicAllocator)
, d_cacheControl(original.d_cacheControl, basicAllocator)
, d_connection(original.d_connection, basicAllocator)
, d_pragma(original.d_pragma, basicAllocator)
, d_trailer(original.d_trailer, basicAllocator)
, d_upgrade(original.d_upgrade, basicAllocator)
, d_warning(original.d_warning, basicAllocator)
, d_contentLocation(original.d_contentLocation, basicAllocator)
, d_contentMd5(original.d_contentMd5, basicAllocator)
, d_contentRange(original.d_contentRange, basicAllocator)
, d_date(original.d_date)
, d_expires(original.d_expires)
, d_lastModified(original.d_lastModified)
, d_contentType(original.d_contentType, basicAllocator)
, d_transferEncoding(original.d_transferEncoding, basicAllocator)
, d_allow(original.d_allow, basicAllocator)
, d_contentLength(original.d_contentLength)
{
}

baenet_HttpBasicHeaderFields::~baenet_HttpBasicHeaderFields()
{
}

// MANIPULATORS

baenet_HttpBasicHeaderFields&
baenet_HttpBasicHeaderFields::operator=(const baenet_HttpBasicHeaderFields& rhs)
{
    if (this != &rhs) {
        d_cacheControl = rhs.d_cacheControl;
        d_connection = rhs.d_connection;
        d_date = rhs.d_date;
        d_pragma = rhs.d_pragma;
        d_trailer = rhs.d_trailer;
        d_transferEncoding = rhs.d_transferEncoding;
        d_upgrade = rhs.d_upgrade;
        d_via = rhs.d_via;
        d_warning = rhs.d_warning;
        d_allow = rhs.d_allow;
        d_contentEncoding = rhs.d_contentEncoding;
        d_contentLanguage = rhs.d_contentLanguage;
        d_contentLength = rhs.d_contentLength;
        d_contentLocation = rhs.d_contentLocation;
        d_contentMd5 = rhs.d_contentMd5;
        d_contentRange = rhs.d_contentRange;
        d_contentType = rhs.d_contentType;
        d_expires = rhs.d_expires;
        d_lastModified = rhs.d_lastModified;
    }
    return *this;
}

void baenet_HttpBasicHeaderFields::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_cacheControl);
    bdeat_ValueTypeFunctions::reset(&d_connection);
    bdeat_ValueTypeFunctions::reset(&d_date);
    bdeat_ValueTypeFunctions::reset(&d_pragma);
    bdeat_ValueTypeFunctions::reset(&d_trailer);
    bdeat_ValueTypeFunctions::reset(&d_transferEncoding);
    bdeat_ValueTypeFunctions::reset(&d_upgrade);
    bdeat_ValueTypeFunctions::reset(&d_via);
    bdeat_ValueTypeFunctions::reset(&d_warning);
    bdeat_ValueTypeFunctions::reset(&d_allow);
    bdeat_ValueTypeFunctions::reset(&d_contentEncoding);
    bdeat_ValueTypeFunctions::reset(&d_contentLanguage);
    bdeat_ValueTypeFunctions::reset(&d_contentLength);
    bdeat_ValueTypeFunctions::reset(&d_contentLocation);
    bdeat_ValueTypeFunctions::reset(&d_contentMd5);
    bdeat_ValueTypeFunctions::reset(&d_contentRange);
    bdeat_ValueTypeFunctions::reset(&d_contentType);
    bdeat_ValueTypeFunctions::reset(&d_expires);
    bdeat_ValueTypeFunctions::reset(&d_lastModified);
}

// ACCESSORS

bsl::ostream& baenet_HttpBasicHeaderFields::print(
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
        stream << "CacheControl = ";
        bdeu_PrintMethods::print(stream, d_cacheControl,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Connection = ";
        bdeu_PrintMethods::print(stream, d_connection,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Date = ";
        bdeu_PrintMethods::print(stream, d_date,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Pragma = ";
        bdeu_PrintMethods::print(stream, d_pragma,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Trailer = ";
        bdeu_PrintMethods::print(stream, d_trailer,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "TransferEncoding = ";
        bdeu_PrintMethods::print(stream, d_transferEncoding,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Upgrade = ";
        bdeu_PrintMethods::print(stream, d_upgrade,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Via = ";
        bdeu_PrintMethods::print(stream, d_via,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Warning = ";
        bdeu_PrintMethods::print(stream, d_warning,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Allow = ";
        bdeu_PrintMethods::print(stream, d_allow,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ContentEncoding = ";
        bdeu_PrintMethods::print(stream, d_contentEncoding,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ContentLanguage = ";
        bdeu_PrintMethods::print(stream, d_contentLanguage,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ContentLength = ";
        bdeu_PrintMethods::print(stream, d_contentLength,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ContentLocation = ";
        bdeu_PrintMethods::print(stream, d_contentLocation,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ContentMd5 = ";
        bdeu_PrintMethods::print(stream, d_contentMd5,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ContentRange = ";
        bdeu_PrintMethods::print(stream, d_contentRange,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ContentType = ";
        bdeu_PrintMethods::print(stream, d_contentType,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Expires = ";
        bdeu_PrintMethods::print(stream, d_expires,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "LastModified = ";
        bdeu_PrintMethods::print(stream, d_lastModified,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "CacheControl = ";
        bdeu_PrintMethods::print(stream, d_cacheControl,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Connection = ";
        bdeu_PrintMethods::print(stream, d_connection,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Date = ";
        bdeu_PrintMethods::print(stream, d_date,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Pragma = ";
        bdeu_PrintMethods::print(stream, d_pragma,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Trailer = ";
        bdeu_PrintMethods::print(stream, d_trailer,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "TransferEncoding = ";
        bdeu_PrintMethods::print(stream, d_transferEncoding,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Upgrade = ";
        bdeu_PrintMethods::print(stream, d_upgrade,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Via = ";
        bdeu_PrintMethods::print(stream, d_via,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Warning = ";
        bdeu_PrintMethods::print(stream, d_warning,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Allow = ";
        bdeu_PrintMethods::print(stream, d_allow,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ContentEncoding = ";
        bdeu_PrintMethods::print(stream, d_contentEncoding,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ContentLanguage = ";
        bdeu_PrintMethods::print(stream, d_contentLanguage,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ContentLength = ";
        bdeu_PrintMethods::print(stream, d_contentLength,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ContentLocation = ";
        bdeu_PrintMethods::print(stream, d_contentLocation,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ContentMd5 = ";
        bdeu_PrintMethods::print(stream, d_contentMd5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ContentRange = ";
        bdeu_PrintMethods::print(stream, d_contentRange,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ContentType = ";
        bdeu_PrintMethods::print(stream, d_contentType,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Expires = ";
        bdeu_PrintMethods::print(stream, d_expires,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "LastModified = ";
        bdeu_PrintMethods::print(stream, d_lastModified,
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
