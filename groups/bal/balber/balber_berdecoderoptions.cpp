// balber_berdecoderoptions.cpp                                       -*-C++-*-
#include <balber_berdecoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berdecoderoptions_cpp,"$Id$ $CSID$")

#include <bdlat_formattingmode.h>

#include <bsls_assert.h>
#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                          // -------------------------------
                          // class balber::BerDecoderOptions
                          // -------------------------------

// CONSTANTS

const char balber::BerDecoderOptions::CLASS_NAME[]                  =
                                                   "balber::BerDecoderOptions";
const int  balber::BerDecoderOptions::DEFAULT_MAX_DEPTH             =      32;
const bool balber::BerDecoderOptions::DEFAULT_SKIP_UNKNOWN_ELEMENTS = true;
const int  balber::BerDecoderOptions::DEFAULT_TRACE_LEVEL           =       0;
const int  balber::BerDecoderOptions::DEFAULT_MAX_SEQUENCE_SIZE     = 8388608;

const bdeat_AttributeInfo balber::BerDecoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_MAX_DEPTH,
        "MaxDepth",                        // name
        sizeof("MaxDepth") - 1,            // name length
        "maximum recursion depth",         // annotation
        bdeat_FormattingMode::BDEAT_DEC    // formatting mode
    },
    {
        e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS,
        "SkipUnknownElements",             // name
        sizeof("SkipUnknownElements") - 1, // name length
        "Option to skip unknown elements", // annotation
        bdeat_FormattingMode::BDEAT_TEXT   // formatting mode
    },
    {
        e_ATTRIBUTE_ID_TRACE_LEVEL,
        "TraceLevel",                      // name
        sizeof("TraceLevel") - 1,          // name length
        "trace (verbosity) level",         // annotation
        bdeat_FormattingMode::BDEAT_DEC    // formatting mode
    },
    {
        e_ATTRIBUTE_ID_MAX_SEQUENCE_SIZE,
        "MaxSequenceSize",                 // name
        sizeof("MaxSequenceSize") - 1,     // name length
        "maximum sequence size",           // annotation
        bdeat_FormattingMode::BDEAT_DEC    // formatting mode
    }
};

namespace balber {

// CLASS METHODS
const bdeat_AttributeInfo *BerDecoderOptions::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (bdlb::CharType::toUpper(name[0])=='M'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='X'
             && bdlb::CharType::toUpper(name[3])=='D'
             && bdlb::CharType::toUpper(name[4])=='E'
             && bdlb::CharType::toUpper(name[5])=='P'
             && bdlb::CharType::toUpper(name[6])=='T'
             && bdlb::CharType::toUpper(name[7])=='H')
            {
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH];
            }
        } break;
        case 10: {
            if (bdlb::CharType::toUpper(name[0])=='T'
             && bdlb::CharType::toUpper(name[1])=='R'
             && bdlb::CharType::toUpper(name[2])=='A'
             && bdlb::CharType::toUpper(name[3])=='C'
             && bdlb::CharType::toUpper(name[4])=='E'
             && bdlb::CharType::toUpper(name[5])=='L'
             && bdlb::CharType::toUpper(name[6])=='E'
             && bdlb::CharType::toUpper(name[7])=='V'
             && bdlb::CharType::toUpper(name[8])=='E'
             && bdlb::CharType::toUpper(name[9])=='L')
            {
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL];
            }
        } break;
        case 15: {
            if (bdlb::CharType::toUpper(name[0])=='M'
             && bdlb::CharType::toUpper(name[1])=='A'
             && bdlb::CharType::toUpper(name[2])=='X'
             && bdlb::CharType::toUpper(name[3])=='S'
             && bdlb::CharType::toUpper(name[4])=='E'
             && bdlb::CharType::toUpper(name[5])=='Q'
             && bdlb::CharType::toUpper(name[6])=='U'
             && bdlb::CharType::toUpper(name[7])=='E'
             && bdlb::CharType::toUpper(name[8])=='N'
             && bdlb::CharType::toUpper(name[9])=='C'
             && bdlb::CharType::toUpper(name[10])=='E'
             && bdlb::CharType::toUpper(name[11])=='S'
             && bdlb::CharType::toUpper(name[12])=='I'
             && bdlb::CharType::toUpper(name[13])=='Z'
             && bdlb::CharType::toUpper(name[14])=='E')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                                          e_ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE];
            }
        } break;
        case 19: {
            if (bdlb::CharType::toUpper(name[0])=='S'
             && bdlb::CharType::toUpper(name[1])=='K'
             && bdlb::CharType::toUpper(name[2])=='I'
             && bdlb::CharType::toUpper(name[3])=='P'
             && bdlb::CharType::toUpper(name[4])=='U'
             && bdlb::CharType::toUpper(name[5])=='N'
             && bdlb::CharType::toUpper(name[6])=='K'
             && bdlb::CharType::toUpper(name[7])=='N'
             && bdlb::CharType::toUpper(name[8])=='O'
             && bdlb::CharType::toUpper(name[9])=='W'
             && bdlb::CharType::toUpper(name[10])=='N'
             && bdlb::CharType::toUpper(name[11])=='E'
             && bdlb::CharType::toUpper(name[12])=='L'
             && bdlb::CharType::toUpper(name[13])=='E'
             && bdlb::CharType::toUpper(name[14])=='M'
             && bdlb::CharType::toUpper(name[15])=='E'
             && bdlb::CharType::toUpper(name[16])=='N'
             && bdlb::CharType::toUpper(name[17])=='T'
             && bdlb::CharType::toUpper(name[18])=='S')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                                      e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *BerDecoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_MAX_DEPTH:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH];
      case e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
      case e_ATTRIBUTE_ID_TRACE_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL];
      case e_ATTRIBUTE_ID_MAX_SEQUENCE_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE];
      default:
        return 0;
    }
}

// ACCESSORS

bsl::ostream& BerDecoderOptions::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);

        stream << "MaxDepth = ";
        bdlb::PrintMethods::print(stream,
                                  d_maxDepth,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SkipUnknownElements = ";
        bdlb::PrintMethods::print(stream,
                                  d_skipUnknownElements,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "TraceLevel = ";
        bdlb::PrintMethods::print(stream,
                                  d_traceLevel,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MaxSequenceSize = ";
        bdlb::PrintMethods::print(stream,
                                  d_maxSequenceSize,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "MaxDepth = ";
        bdlb::PrintMethods::print(stream,
                                  d_maxDepth,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << ' ';
        stream << "SkipUnknownElements = ";
        bdlb::PrintMethods::print(stream,
                                  d_skipUnknownElements,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << ' ';
        stream << "TraceLevel = ";
        bdlb::PrintMethods::print(stream,
                                  d_traceLevel,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << ' ';
        stream << "MaxSequenceSize = ";
        bdlb::PrintMethods::print(stream,
                                  d_maxSequenceSize,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2.1.11 Tue Mar 11 14:28:56 2008
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
