// baljsn_encoderoptions.cpp                                          -*-C++-*-
#include <baljsn_encoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_encoderoptions_cpp,"$Id$ $CSID$")

#include <baljsn_encoderoptions.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>

#include <baljsn_encodingstyle.h>

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace baljsn {

                            // --------------------
                            // class EncoderOptions
                            // --------------------

// CONSTANTS

const char EncoderOptions::CLASS_NAME[] = "EncoderOptions";

const int EncoderOptions::DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL = 0;

const int EncoderOptions::DEFAULT_INITIALIZER_SPACES_PER_LEVEL = 0;

const baljsn::EncodingStyle::Value EncoderOptions::DEFAULT_INITIALIZER_ENCODING_STYLE = baljsn::EncodingStyle::e_COMPACT;

const bool EncoderOptions::DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS = false;

const bool EncoderOptions::DEFAULT_INITIALIZER_ENCODE_NULL_ELEMENTS = false;

const bool EncoderOptions::DEFAULT_INITIALIZER_ENCODE_INF_AND_NA_N_AS_STRINGS = false;

const int EncoderOptions::DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION = 3;

const int EncoderOptions::DEFAULT_INITIALIZER_MAX_FLOAT_PRECISION = bsl::numeric_limits<float>::digits10;

const int EncoderOptions::DEFAULT_INITIALIZER_MAX_DOUBLE_PRECISION = bsl::numeric_limits<double>::digits10;

const bdeat_AttributeInfo EncoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_INITIAL_INDENT_LEVEL,
        "InitialIndentLevel",
        sizeof("InitialIndentLevel") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_SPACES_PER_LEVEL,
        "SpacesPerLevel",
        sizeof("SpacesPerLevel") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ENCODING_STYLE,
        "EncodingStyle",
        sizeof("EncodingStyle") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS,
        "EncodeEmptyArrays",
        sizeof("EncodeEmptyArrays") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ENCODE_NULL_ELEMENTS,
        "EncodeNullElements",
        sizeof("EncodeNullElements") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ENCODE_INF_AND_NA_N_AS_STRINGS,
        "encodeInfAndNaNAsStrings",
        sizeof("encodeInfAndNaNAsStrings") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION,
        "DatetimeFractionalSecondPrecision",
        sizeof("DatetimeFractionalSecondPrecision") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_MAX_FLOAT_PRECISION,
        "maxFloatPrecision",
        sizeof("maxFloatPrecision") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_MAX_DOUBLE_PRECISION,
        "maxDoublePrecision",
        sizeof("maxDoublePrecision") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *EncoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 9; ++i) {
        const bdeat_AttributeInfo& attributeInfo =
                    EncoderOptions::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdeat_AttributeInfo *EncoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_INITIAL_INDENT_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL];
      case ATTRIBUTE_ID_SPACES_PER_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL];
      case ATTRIBUTE_ID_ENCODING_STYLE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE];
      case ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS];
      case ATTRIBUTE_ID_ENCODE_NULL_ELEMENTS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_NULL_ELEMENTS];
      case ATTRIBUTE_ID_ENCODE_INF_AND_NA_N_AS_STRINGS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_INF_AND_NA_N_AS_STRINGS];
      case ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION];
      case ATTRIBUTE_ID_MAX_FLOAT_PRECISION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FLOAT_PRECISION];
      case ATTRIBUTE_ID_MAX_DOUBLE_PRECISION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DOUBLE_PRECISION];
      default:
        return 0;
    }
}

// CREATORS

EncoderOptions::EncoderOptions()
: d_initialIndentLevel(DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL)
, d_spacesPerLevel(DEFAULT_INITIALIZER_SPACES_PER_LEVEL)
, d_datetimeFractionalSecondPrecision(DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION)
, d_maxFloatPrecision(DEFAULT_INITIALIZER_MAX_FLOAT_PRECISION)
, d_maxDoublePrecision(DEFAULT_INITIALIZER_MAX_DOUBLE_PRECISION)
, d_encodingStyle(DEFAULT_INITIALIZER_ENCODING_STYLE)
, d_encodeEmptyArrays(DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS)
, d_encodeNullElements(DEFAULT_INITIALIZER_ENCODE_NULL_ELEMENTS)
, d_encodeInfAndNaNAsStrings(DEFAULT_INITIALIZER_ENCODE_INF_AND_NA_N_AS_STRINGS)
{
}

EncoderOptions::EncoderOptions(const EncoderOptions& original)
: d_initialIndentLevel(original.d_initialIndentLevel)
, d_spacesPerLevel(original.d_spacesPerLevel)
, d_datetimeFractionalSecondPrecision(original.d_datetimeFractionalSecondPrecision)
, d_maxFloatPrecision(original.d_maxFloatPrecision)
, d_maxDoublePrecision(original.d_maxDoublePrecision)
, d_encodingStyle(original.d_encodingStyle)
, d_encodeEmptyArrays(original.d_encodeEmptyArrays)
, d_encodeNullElements(original.d_encodeNullElements)
, d_encodeInfAndNaNAsStrings(original.d_encodeInfAndNaNAsStrings)
{
}

EncoderOptions::~EncoderOptions()
{
}

// MANIPULATORS

EncoderOptions&
EncoderOptions::operator=(const EncoderOptions& rhs)
{
    if (this != &rhs) {
        d_initialIndentLevel = rhs.d_initialIndentLevel;
        d_spacesPerLevel = rhs.d_spacesPerLevel;
        d_encodingStyle = rhs.d_encodingStyle;
        d_encodeEmptyArrays = rhs.d_encodeEmptyArrays;
        d_encodeNullElements = rhs.d_encodeNullElements;
        d_encodeInfAndNaNAsStrings = rhs.d_encodeInfAndNaNAsStrings;
        d_datetimeFractionalSecondPrecision = rhs.d_datetimeFractionalSecondPrecision;
        d_maxFloatPrecision = rhs.d_maxFloatPrecision;
        d_maxDoublePrecision = rhs.d_maxDoublePrecision;
    }

    return *this;
}

void EncoderOptions::reset()
{
    d_initialIndentLevel = DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;
    d_spacesPerLevel = DEFAULT_INITIALIZER_SPACES_PER_LEVEL;
    d_encodingStyle = DEFAULT_INITIALIZER_ENCODING_STYLE;
    d_encodeEmptyArrays = DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS;
    d_encodeNullElements = DEFAULT_INITIALIZER_ENCODE_NULL_ELEMENTS;
    d_encodeInfAndNaNAsStrings = DEFAULT_INITIALIZER_ENCODE_INF_AND_NA_N_AS_STRINGS;
    d_datetimeFractionalSecondPrecision = DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION;
    d_maxFloatPrecision = DEFAULT_INITIALIZER_MAX_FLOAT_PRECISION;
    d_maxDoublePrecision = DEFAULT_INITIALIZER_MAX_DOUBLE_PRECISION;
}

// ACCESSORS

bsl::ostream& EncoderOptions::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("initialIndentLevel", d_initialIndentLevel);
    printer.printAttribute("spacesPerLevel", d_spacesPerLevel);
    printer.printAttribute("encodingStyle", d_encodingStyle);
    printer.printAttribute("encodeEmptyArrays", d_encodeEmptyArrays);
    printer.printAttribute("encodeNullElements", d_encodeNullElements);
    printer.printAttribute("encodeInfAndNaNAsStrings", d_encodeInfAndNaNAsStrings);
    printer.printAttribute("datetimeFractionalSecondPrecision", d_datetimeFractionalSecondPrecision);
    printer.printAttribute("maxFloatPrecision", d_maxFloatPrecision);
    printer.printAttribute("maxDoublePrecision", d_maxDoublePrecision);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_3.8.22 Tue Jan  3 11:23:49 2017
// USING bas_codegen.pl -m msg -p baljsn -E --noExternalization --noAggregateConversion baljsn.xsd
// SERVICE VERSION 
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
