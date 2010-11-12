// baenet_httpstatusline.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpstatusline_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httpstatusline.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpstatusline_cpp,"$Id$ $CSID$")

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <baenet_httpstatuscode.h>
#include <bsl_string.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                      // -------------------------------                       
                      // class baenet_HttpStatusLine                       
                      // -------------------------------                       

// CONSTANTS

const char baenet_HttpStatusLine::CLASS_NAME[] = "baenet_HttpStatusLine";

const int baenet_HttpStatusLine::DEFAULT_INITIALIZER_MAJOR_VERSION = 1;

const int baenet_HttpStatusLine::DEFAULT_INITIALIZER_MINOR_VERSION = 1;

const baenet_HttpStatusCode::Value baenet_HttpStatusLine::DEFAULT_INITIALIZER_STATUS_CODE = baenet_HttpStatusCode::BAENET_CONTINUE;

const bdeat_AttributeInfo baenet_HttpStatusLine::ATTRIBUTE_INFO_ARRAY[] = {
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
    },
    {
        ATTRIBUTE_ID_STATUS_CODE,
        "Status-Code",
        sizeof("Status-Code") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_REASON_PHRASE,
        "Reason-Phrase",
        sizeof("Reason-Phrase") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *baenet_HttpStatusLine::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 11: {
            if ((name[0]|0x20)=='s'
             && (name[1]|0x20)=='t'
             && (name[2]|0x20)=='a'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='u'
             && (name[5]|0x20)=='s'
             && (name[6])=='-'
             && (name[7]|0x20)=='c'
             && (name[8]|0x20)=='o'
             && (name[9]|0x20)=='d'
             && (name[10]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS_CODE];
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
                                        ATTRIBUTE_INDEX_MAJOR_VERSION
                                   ];
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
                                        ATTRIBUTE_INDEX_MINOR_VERSION
                                   ];
                        }
                    } break;
                }
            }
        } break;
        case 13: {
            if ((name[0]|0x20)=='r'
             && (name[1]|0x20)=='e'
             && (name[2]|0x20)=='a'
             && (name[3]|0x20)=='s'
             && (name[4]|0x20)=='o'
             && (name[5]|0x20)=='n'
             && (name[6])=='-'
             && (name[7]|0x20)=='p'
             && (name[8]|0x20)=='h'
             && (name[9]|0x20)=='r'
             && (name[10]|0x20)=='a'
             && (name[11]|0x20)=='s'
             && (name[12]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REASON_PHRASE];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *baenet_HttpStatusLine::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_MAJOR_VERSION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAJOR_VERSION];
      case ATTRIBUTE_ID_MINOR_VERSION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MINOR_VERSION];
      case ATTRIBUTE_ID_STATUS_CODE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS_CODE];
      case ATTRIBUTE_ID_REASON_PHRASE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REASON_PHRASE];
      default:
        return 0;
    }
}

// CREATORS

baenet_HttpStatusLine::baenet_HttpStatusLine(bslma_Allocator *basicAllocator)
: d_reasonPhrase(basicAllocator)
, d_majorVersion(DEFAULT_INITIALIZER_MAJOR_VERSION)
, d_minorVersion(DEFAULT_INITIALIZER_MINOR_VERSION)
, d_statusCode(DEFAULT_INITIALIZER_STATUS_CODE)
{
}

baenet_HttpStatusLine::baenet_HttpStatusLine(
                               const baenet_HttpStatusLine&  original,
                               bslma_Allocator              *basicAllocator)
: d_reasonPhrase(original.d_reasonPhrase, basicAllocator)
, d_majorVersion(original.d_majorVersion)
, d_minorVersion(original.d_minorVersion)
, d_statusCode(original.d_statusCode)
{
}

baenet_HttpStatusLine::~baenet_HttpStatusLine()
{
}

// MANIPULATORS

baenet_HttpStatusLine&
baenet_HttpStatusLine::operator=(const baenet_HttpStatusLine& rhs)
{
    if (this != &rhs) {
        d_majorVersion = rhs.d_majorVersion;
        d_minorVersion = rhs.d_minorVersion;
        d_statusCode = rhs.d_statusCode;
        d_reasonPhrase = rhs.d_reasonPhrase;
    }
    return *this;
}

void baenet_HttpStatusLine::reset()
{
    d_majorVersion = DEFAULT_INITIALIZER_MAJOR_VERSION;
    d_minorVersion = DEFAULT_INITIALIZER_MINOR_VERSION;
    d_statusCode = DEFAULT_INITIALIZER_STATUS_CODE;
    bdeat_ValueTypeFunctions::reset(&d_reasonPhrase);
}

// ACCESSORS

bsl::ostream& baenet_HttpStatusLine::print(
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
        stream << "MajorVersion = ";
        bdeu_PrintMethods::print(stream, d_majorVersion,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MinorVersion = ";
        bdeu_PrintMethods::print(stream, d_minorVersion,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "StatusCode = ";
        bdeu_PrintMethods::print(stream, d_statusCode,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ReasonPhrase = ";
        bdeu_PrintMethods::print(stream, d_reasonPhrase,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "MajorVersion = ";
        bdeu_PrintMethods::print(stream, d_majorVersion,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MinorVersion = ";
        bdeu_PrintMethods::print(stream, d_minorVersion,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "StatusCode = ";
        bdeu_PrintMethods::print(stream, d_statusCode,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ReasonPhrase = ";
        bdeu_PrintMethods::print(stream, d_reasonPhrase,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}


}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Thu Feb 11 13:00:11 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
