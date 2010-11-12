// baexml_decoderoptions.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_decoderoptions_cpp,"$Id$ $CSID$ $CCId$")

#include <baexml_decoderoptions.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>


#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                      // -------------------------------                       
                      // class baexml_DecoderOptions                       
                      // -------------------------------                       

// CONSTANTS

const char baexml_DecoderOptions::CLASS_NAME[] = "baexml_DecoderOptions";

const int baexml_DecoderOptions::DEFAULT_MAX_DEPTH = 32;

const int baexml_DecoderOptions::DEFAULT_FORMATTING_MODE = 0;

const bool baexml_DecoderOptions::DEFAULT_SKIP_UNKNOWN_ELEMENTS = true;

const bdeat_AttributeInfo baexml_DecoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_MAX_DEPTH,
        "MaxDepth",
        sizeof("MaxDepth") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_FORMATTING_MODE,
        "FormattingMode",
        sizeof("FormattingMode") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS,
        "SkipUnknownElements",
        sizeof("SkipUnknownElements") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *baexml_DecoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if ((name[0]|0x20)=='m'
             && (name[1]|0x20)=='a'
             && (name[2]|0x20)=='x'
             && (name[3]|0x20)=='d'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='p'
             && (name[6]|0x20)=='t'
             && (name[7]|0x20)=='h')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH];
            }
        } break;
        case 14: {
            if ((name[0]|0x20)=='f'
             && (name[1]|0x20)=='o'
             && (name[2]|0x20)=='r'
             && (name[3]|0x20)=='m'
             && (name[4]|0x20)=='a'
             && (name[5]|0x20)=='t'
             && (name[6]|0x20)=='t'
             && (name[7]|0x20)=='i'
             && (name[8]|0x20)=='n'
             && (name[9]|0x20)=='g'
             && (name[10]|0x20)=='m'
             && (name[11]|0x20)=='o'
             && (name[12]|0x20)=='d'
             && (name[13]|0x20)=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE];
            }
        } break;
        case 19: {
            if ((name[0]|0x20)=='s'
             && (name[1]|0x20)=='k'
             && (name[2]|0x20)=='i'
             && (name[3]|0x20)=='p'
             && (name[4]|0x20)=='u'
             && (name[5]|0x20)=='n'
             && (name[6]|0x20)=='k'
             && (name[7]|0x20)=='n'
             && (name[8]|0x20)=='o'
             && (name[9]|0x20)=='w'
             && (name[10]|0x20)=='n'
             && (name[11]|0x20)=='e'
             && (name[12]|0x20)=='l'
             && (name[13]|0x20)=='e'
             && (name[14]|0x20)=='m'
             && (name[15]|0x20)=='e'
             && (name[16]|0x20)=='n'
             && (name[17]|0x20)=='t'
             && (name[18]|0x20)=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *baexml_DecoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH];
      case ATTRIBUTE_ID_FORMATTING_MODE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE];
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
      default:
        return 0;
    }
}

// CREATORS

baexml_DecoderOptions::baexml_DecoderOptions()
: d_maxDepth(DEFAULT_MAX_DEPTH)
, d_formattingMode(DEFAULT_FORMATTING_MODE)
, d_skipUnknownElements(DEFAULT_SKIP_UNKNOWN_ELEMENTS)
{
}

baexml_DecoderOptions::baexml_DecoderOptions(const baexml_DecoderOptions& original)
: d_maxDepth(original.d_maxDepth)
, d_formattingMode(original.d_formattingMode)
, d_skipUnknownElements(original.d_skipUnknownElements)
{
}

baexml_DecoderOptions::~baexml_DecoderOptions()
{
}

// MANIPULATORS

baexml_DecoderOptions&
baexml_DecoderOptions::operator=(const baexml_DecoderOptions& rhs)
{
    if (this != &rhs) {
        d_maxDepth = rhs.d_maxDepth;
        d_formattingMode = rhs.d_formattingMode;
        d_skipUnknownElements = rhs.d_skipUnknownElements;
    }
    return *this;
}

void baexml_DecoderOptions::reset()
{
    d_maxDepth = DEFAULT_MAX_DEPTH;
    d_formattingMode = DEFAULT_FORMATTING_MODE;
    d_skipUnknownElements = DEFAULT_SKIP_UNKNOWN_ELEMENTS;
}

// ACCESSORS

bsl::ostream& baexml_DecoderOptions::print(
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
        stream << "FormattingMode = ";
        bdeu_PrintMethods::print(stream, d_formattingMode,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SkipUnknownElements = ";
        bdeu_PrintMethods::print(stream, d_skipUnknownElements,
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
        stream << "FormattingMode = ";
        bdeu_PrintMethods::print(stream, d_formattingMode,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SkipUnknownElements = ";
        bdeu_PrintMethods::print(stream, d_skipUnknownElements,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}


}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.5.0 Tue May  4 08:22:33 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
