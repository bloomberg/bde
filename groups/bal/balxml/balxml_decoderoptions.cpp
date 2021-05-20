// balxml_decoderoptions.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_decoderoptions_cpp,"$Id$ $CSID$ $CCId$")

#include <balxml_decoderoptions.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // ----------------------------
                        // class balxml::DecoderOptions
                        // ----------------------------

// CONSTANTS

const char balxml::DecoderOptions::CLASS_NAME[] = "balxml::DecoderOptions";

const int balxml::DecoderOptions::DEFAULT_INITIALIZER_MAX_DEPTH = 32;

const int balxml::DecoderOptions::DEFAULT_INITIALIZER_FORMATTING_MODE = 0;

const bool balxml::DecoderOptions::DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS = true;

const bdlat_AttributeInfo balxml::DecoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_MAX_DEPTH,
        "MaxDepth",
        sizeof("MaxDepth") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_FORMATTING_MODE,
        "FormattingMode",
        sizeof("FormattingMode") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS,
        "SkipUnknownElements",
        sizeof("SkipUnknownElements") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

namespace balxml {
// CLASS METHODS

const bdlat_AttributeInfo *DecoderOptions::lookupAttributeInfo(
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
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH];
                                                                      // RETURN
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
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE];
                                                                      // RETURN
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
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *DecoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_MAX_DEPTH:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_DEPTH];
      case e_ATTRIBUTE_ID_FORMATTING_MODE:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_FORMATTING_MODE];
      case e_ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
      default:
        return 0;
    }
}

// CREATORS

DecoderOptions::DecoderOptions()
: d_maxDepth(DEFAULT_INITIALIZER_MAX_DEPTH)
, d_formattingMode(DEFAULT_INITIALIZER_FORMATTING_MODE)
, d_skipUnknownElements(DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS)
{
}

DecoderOptions::DecoderOptions(const DecoderOptions& original)
: d_maxDepth(original.d_maxDepth)
, d_formattingMode(original.d_formattingMode)
, d_skipUnknownElements(original.d_skipUnknownElements)
{
}

DecoderOptions::~DecoderOptions()
{
}

// MANIPULATORS

DecoderOptions&
DecoderOptions::operator=(const DecoderOptions& rhs)
{
    if (this != &rhs) {
        d_maxDepth = rhs.d_maxDepth;
        d_formattingMode = rhs.d_formattingMode;
        d_skipUnknownElements = rhs.d_skipUnknownElements;
    }
    return *this;
}

void DecoderOptions::reset()
{
    d_maxDepth = DEFAULT_INITIALIZER_MAX_DEPTH;
    d_formattingMode = DEFAULT_INITIALIZER_FORMATTING_MODE;
    d_skipUnknownElements = DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;
}

// ACCESSORS

bsl::ostream& DecoderOptions::print(
    bsl::ostream& stream,
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
        bdlb::PrintMethods::print(stream, d_maxDepth,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "FormattingMode = ";
        bdlb::PrintMethods::print(stream, d_formattingMode,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SkipUnknownElements = ";
        bdlb::PrintMethods::print(stream, d_skipUnknownElements,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "MaxDepth = ";
        bdlb::PrintMethods::print(stream, d_maxDepth,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "FormattingMode = ";
        bdlb::PrintMethods::print(stream, d_formattingMode,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SkipUnknownElements = ";
        bdlb::PrintMethods::print(stream, d_skipUnknownElements,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}
}  // close package namespace


}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
