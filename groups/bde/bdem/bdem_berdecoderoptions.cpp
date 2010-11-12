// bdem_berdecoderoptions.cpp                                         -*-C++-*-
#include <bdem_berdecoderoptions.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_berdecoderoptions_cpp,"$Id$ $CSID$")

#include <bdeat_formattingmode.h>

#include <bsls_assert.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                          // -----------------------
                          // class bdem_BerDecoderOptions
                          // -----------------------

// CONSTANTS

const char bdem_BerDecoderOptions::CLASS_NAME[] = "bdem_BerDecoderOptions";
    // the name of this class
const int bdem_BerDecoderOptions::DEFAULT_MAX_DEPTH = 32;
    // default value of 'MaxDepth' attribute
const bool bdem_BerDecoderOptions::DEFAULT_SKIP_UNKNOWN_ELEMENTS = true;
    // default value of 'SkipUnknownElements' attribute
const int bdem_BerDecoderOptions::DEFAULT_TRACE_LEVEL = 0;
    // default value of 'TraceLevel' attribute
const int bdem_BerDecoderOptions::DEFAULT_MAX_SEQUENCE_SIZE = 8388608;
    // default value of 'MaxSequenceSize' attribute

const bdeat_AttributeInfo bdem_BerDecoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_MAX_DEPTH,
        "MaxDepth",             // name
        sizeof("MaxDepth") - 1, // name length
        "maximum recursion depth",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS,
        "SkipUnknownElements",             // name
        sizeof("SkipUnknownElements") - 1, // name length
        "Option to skip unknown elements",  // annotation
        bdeat_FormattingMode::BDEAT_TEXT // formatting mode
    },
    {
        ATTRIBUTE_ID_TRACE_LEVEL,
        "TraceLevel",             // name
        sizeof("TraceLevel") - 1, // name length
        "trace (verbosity) level",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    },
    {
        ATTRIBUTE_ID_MAX_SEQUENCE_SIZE,
        "MaxSequenceSize",             // name
        sizeof("MaxSequenceSize") - 1, // name length
        "maximum sequence size",  // annotation
        bdeat_FormattingMode::BDEAT_DEC // formatting mode
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *bdem_BerDecoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (bdeu_CharType::toUpper(name[0])=='M'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='X'
             && bdeu_CharType::toUpper(name[3])=='D'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='P'
             && bdeu_CharType::toUpper(name[6])=='T'
             && bdeu_CharType::toUpper(name[7])=='H')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH];
            }
        } break;
        case 10: {
            if (bdeu_CharType::toUpper(name[0])=='T'
             && bdeu_CharType::toUpper(name[1])=='R'
             && bdeu_CharType::toUpper(name[2])=='A'
             && bdeu_CharType::toUpper(name[3])=='C'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='L'
             && bdeu_CharType::toUpper(name[6])=='E'
             && bdeu_CharType::toUpper(name[7])=='V'
             && bdeu_CharType::toUpper(name[8])=='E'
             && bdeu_CharType::toUpper(name[9])=='L')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL];
            }
        } break;
        case 15: {
            if (bdeu_CharType::toUpper(name[0])=='M'
             && bdeu_CharType::toUpper(name[1])=='A'
             && bdeu_CharType::toUpper(name[2])=='X'
             && bdeu_CharType::toUpper(name[3])=='S'
             && bdeu_CharType::toUpper(name[4])=='E'
             && bdeu_CharType::toUpper(name[5])=='Q'
             && bdeu_CharType::toUpper(name[6])=='U'
             && bdeu_CharType::toUpper(name[7])=='E'
             && bdeu_CharType::toUpper(name[8])=='N'
             && bdeu_CharType::toUpper(name[9])=='C'
             && bdeu_CharType::toUpper(name[10])=='E'
             && bdeu_CharType::toUpper(name[11])=='S'
             && bdeu_CharType::toUpper(name[12])=='I'
             && bdeu_CharType::toUpper(name[13])=='Z'
             && bdeu_CharType::toUpper(name[14])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE];
            }
        } break;
        case 19: {
            if (bdeu_CharType::toUpper(name[0])=='S'
             && bdeu_CharType::toUpper(name[1])=='K'
             && bdeu_CharType::toUpper(name[2])=='I'
             && bdeu_CharType::toUpper(name[3])=='P'
             && bdeu_CharType::toUpper(name[4])=='U'
             && bdeu_CharType::toUpper(name[5])=='N'
             && bdeu_CharType::toUpper(name[6])=='K'
             && bdeu_CharType::toUpper(name[7])=='N'
             && bdeu_CharType::toUpper(name[8])=='O'
             && bdeu_CharType::toUpper(name[9])=='W'
             && bdeu_CharType::toUpper(name[10])=='N'
             && bdeu_CharType::toUpper(name[11])=='E'
             && bdeu_CharType::toUpper(name[12])=='L'
             && bdeu_CharType::toUpper(name[13])=='E'
             && bdeu_CharType::toUpper(name[14])=='M'
             && bdeu_CharType::toUpper(name[15])=='E'
             && bdeu_CharType::toUpper(name[16])=='N'
             && bdeu_CharType::toUpper(name[17])=='T'
             && bdeu_CharType::toUpper(name[18])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *bdem_BerDecoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH];
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
      case ATTRIBUTE_ID_TRACE_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL];
      case ATTRIBUTE_ID_MAX_SEQUENCE_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& bdem_BerDecoderOptions::print(
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
        stream << "MaxDepth = ";
        bdeu_PrintMethods::print(stream, d_maxDepth,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SkipUnknownElements = ";
        bdeu_PrintMethods::print(stream, d_skipUnknownElements,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "TraceLevel = ";
        bdeu_PrintMethods::print(stream, d_traceLevel,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MaxSequenceSize = ";
        bdeu_PrintMethods::print(stream, d_maxSequenceSize,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "MaxDepth = ";
        bdeu_PrintMethods::print(stream, d_maxDepth,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SkipUnknownElements = ";
        bdeu_PrintMethods::print(stream, d_skipUnknownElements,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "TraceLevel = ";
        bdeu_PrintMethods::print(stream, d_traceLevel,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MaxSequenceSize = ";
        bdeu_PrintMethods::print(stream, d_maxSequenceSize,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_2.1.11 Tue Mar 11 14:28:56 2008
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
