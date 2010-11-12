// baenet_httprequestline.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httprequestline_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httprequestline.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <baenet_httprequestmethod.h>
#include <bsl_string.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                      // --------------------------------                      
                      // class baenet_HttpRequestLine                      
                      // --------------------------------                      

// CONSTANTS

const char baenet_HttpRequestLine::CLASS_NAME[] = "baenet_HttpRequestLine";

const int baenet_HttpRequestLine::DEFAULT_INITIALIZER_MAJOR_VERSION = 1;

const int baenet_HttpRequestLine::DEFAULT_INITIALIZER_MINOR_VERSION = 1;

const bdeat_AttributeInfo baenet_HttpRequestLine::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_METHOD,
        "Method",
        sizeof("Method") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_REQUEST_URI,
        "Request-URI",
        sizeof("Request-URI") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_MAJOR_VERSION,
        "MajorVersion",
        sizeof("MajorVersion") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_MINOR_VERSION,
        "MinorVersion",
        sizeof("MinorVersion") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *baenet_HttpRequestLine::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 6: {
            if ((name[0]|0x20)=='m'
             && (name[1]|0x20)=='e'
             && (name[2]|0x20)=='t'
             && (name[3]|0x20)=='h'
             && (name[4]|0x20)=='o'
             && (name[5]|0x20)=='d')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_METHOD];
            }
        } break;
        case 11: {
            if ((name[0]|0x20)=='r'
             && (name[1]|0x20)=='e'
             && (name[2]|0x20)=='q'
             && (name[3]|0x20)=='u'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='s'
             && (name[6]|0x20)=='t'
             && (name[7])=='-'
             && (name[8]|0x20)=='u'
             && (name[9]|0x20)=='r'
             && (name[10]|0x20)=='i')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REQUEST_URI];
            }
        } break;
        case 12: {
            if ((name[0]|0x20)=='m')
            {
                switch(bdeu_CharType::toUpper(name[1])) {
                    case 'A': {
                        if ((name[2]|0x20)=='j'
                         && (name[3]|0x20)=='o'
                         && (name[4]|0x20)=='r'
                         && (name[5]|0x20)=='v'
                         && (name[6]|0x20)=='e'
                         && (name[7]|0x20)=='r'
                         && (name[8]|0x20)=='s'
                         && (name[9]|0x20)=='i'
                         && (name[10]|0x20)=='o'
                         && (name[11]|0x20)=='n')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[
                                                ATTRIBUTE_INDEX_MAJOR_VERSION];
                        }
                    } break;
                    case 'I': {
                        if ((name[2]|0x20)=='n'
                         && (name[3]|0x20)=='o'
                         && (name[4]|0x20)=='r'
                         && (name[5]|0x20)=='v'
                         && (name[6]|0x20)=='e'
                         && (name[7]|0x20)=='r'
                         && (name[8]|0x20)=='s'
                         && (name[9]|0x20)=='i'
                         && (name[10]|0x20)=='o'
                         && (name[11]|0x20)=='n')
                        {
                            return &ATTRIBUTE_INFO_ARRAY[
                                                ATTRIBUTE_INDEX_MINOR_VERSION];
                        }
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *baenet_HttpRequestLine::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_METHOD:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_METHOD];
      case ATTRIBUTE_ID_REQUEST_URI:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REQUEST_URI];
      case ATTRIBUTE_ID_MAJOR_VERSION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAJOR_VERSION];
      case ATTRIBUTE_ID_MINOR_VERSION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MINOR_VERSION];
      default:
        return 0;
    }
}

// CREATORS

baenet_HttpRequestLine::baenet_HttpRequestLine(bslma_Allocator *basicAllocator)
: d_requestUri(basicAllocator)
, d_majorVersion(DEFAULT_INITIALIZER_MAJOR_VERSION)
, d_minorVersion(DEFAULT_INITIALIZER_MINOR_VERSION)
, d_method(static_cast<baenet_HttpRequestMethod::Value>(0))
{
}

baenet_HttpRequestLine::baenet_HttpRequestLine(const baenet_HttpRequestLine& original,
                                                       bslma_Allocator *basicAllocator)
: d_requestUri(original.d_requestUri, basicAllocator)
, d_majorVersion(original.d_majorVersion)
, d_minorVersion(original.d_minorVersion)
, d_method(original.d_method)
{
}

baenet_HttpRequestLine::~baenet_HttpRequestLine()
{
}

// MANIPULATORS

baenet_HttpRequestLine&
baenet_HttpRequestLine::operator=(const baenet_HttpRequestLine& rhs)
{
    if (this != &rhs) {
        d_method = rhs.d_method;
        d_requestUri = rhs.d_requestUri;
        d_majorVersion = rhs.d_majorVersion;
        d_minorVersion = rhs.d_minorVersion;
    }
    return *this;
}

void baenet_HttpRequestLine::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_method);
    bdeat_ValueTypeFunctions::reset(&d_requestUri);
    d_majorVersion = DEFAULT_INITIALIZER_MAJOR_VERSION;
    d_minorVersion = DEFAULT_INITIALIZER_MINOR_VERSION;
}

// ACCESSORS

bsl::ostream& baenet_HttpRequestLine::print(
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
        stream << "Method = ";
        bdeu_PrintMethods::print(stream, d_method,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "RequestUri = ";
        bdeu_PrintMethods::print(stream, d_requestUri,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MajorVersion = ";
        bdeu_PrintMethods::print(stream, d_majorVersion,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MinorVersion = ";
        bdeu_PrintMethods::print(stream, d_minorVersion,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Method = ";
        bdeu_PrintMethods::print(stream, d_method,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "RequestUri = ";
        bdeu_PrintMethods::print(stream, d_requestUri,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MajorVersion = ";
        bdeu_PrintMethods::print(stream, d_majorVersion,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MinorVersion = ";
        bdeu_PrintMethods::print(stream, d_minorVersion,
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
