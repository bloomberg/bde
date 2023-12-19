// balxml_encoderoptions.cpp         *DO NOT EDIT*         @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_encoderoptions_cpp,"$Id$ $CSID$")

#include <balxml_encoderoptions.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <balxml_encodingstyle.h>
#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace balxml {

                            // --------------------
                            // class EncoderOptions
                            // --------------------

// CONSTANTS

const char EncoderOptions::CLASS_NAME[] = "EncoderOptions";

const int EncoderOptions::DEFAULT_INITIALIZER_FORMATTING_MODE = 0;

const int EncoderOptions::DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL = 0;

const int EncoderOptions::DEFAULT_INITIALIZER_SPACES_PER_LEVEL = 4;

const int EncoderOptions::DEFAULT_INITIALIZER_WRAP_COLUMN = 80;

const EncodingStyle::Value EncoderOptions::DEFAULT_INITIALIZER_ENCODING_STYLE = EncodingStyle::COMPACT;

const bool EncoderOptions::DEFAULT_INITIALIZER_ALLOW_CONTROL_CHARACTERS = false;

const bool EncoderOptions::DEFAULT_INITIALIZER_OUTPUT_X_M_L_HEADER = true;

const bool EncoderOptions::DEFAULT_INITIALIZER_OUTPUT_X_S_I_ALIAS = true;

const int EncoderOptions::DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION = 6;

const bool EncoderOptions::DEFAULT_INITIALIZER_USE_Z_ABBREVIATION_FOR_UTC = false;

const bdlat_AttributeInfo EncoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_OBJECT_NAMESPACE,
        "ObjectNamespace",
        sizeof("ObjectNamespace") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_SCHEMA_LOCATION,
        "SchemaLocation",
        sizeof("SchemaLocation") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_TAG,
        "Tag",
        sizeof("Tag") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_FORMATTING_MODE,
        "FormattingMode",
        sizeof("FormattingMode") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_INITIAL_INDENT_LEVEL,
        "InitialIndentLevel",
        sizeof("InitialIndentLevel") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SPACES_PER_LEVEL,
        "SpacesPerLevel",
        sizeof("SpacesPerLevel") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_WRAP_COLUMN,
        "WrapColumn",
        sizeof("WrapColumn") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS,
        "MaxDecimalTotalDigits",
        sizeof("MaxDecimalTotalDigits") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS,
        "MaxDecimalFractionDigits",
        sizeof("MaxDecimalFractionDigits") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS,
        "SignificantDoubleDigits",
        sizeof("SignificantDoubleDigits") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ENCODING_STYLE,
        "EncodingStyle",
        sizeof("EncodingStyle") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS,
        "AllowControlCharacters",
        sizeof("AllowControlCharacters") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_OUTPUT_X_M_L_HEADER,
        "OutputXMLHeader",
        sizeof("OutputXMLHeader") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_OUTPUT_X_S_I_ALIAS,
        "OutputXSIAlias",
        sizeof("OutputXSIAlias") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION,
        "DatetimeFractionalSecondPrecision",
        sizeof("DatetimeFractionalSecondPrecision") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_USE_Z_ABBREVIATION_FOR_UTC,
        "UseZAbbreviationForUtc",
        sizeof("UseZAbbreviationForUtc") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 16; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderOptions::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_OBJECT_NAMESPACE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OBJECT_NAMESPACE];
      case ATTRIBUTE_ID_SCHEMA_LOCATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SCHEMA_LOCATION];
      case ATTRIBUTE_ID_TAG:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TAG];
      case ATTRIBUTE_ID_FORMATTING_MODE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE];
      case ATTRIBUTE_ID_INITIAL_INDENT_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_INITIAL_INDENT_LEVEL];
      case ATTRIBUTE_ID_SPACES_PER_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SPACES_PER_LEVEL];
      case ATTRIBUTE_ID_WRAP_COLUMN:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRAP_COLUMN];
      case ATTRIBUTE_ID_MAX_DECIMAL_TOTAL_DIGITS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_TOTAL_DIGITS];
      case ATTRIBUTE_ID_MAX_DECIMAL_FRACTION_DIGITS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DECIMAL_FRACTION_DIGITS];
      case ATTRIBUTE_ID_SIGNIFICANT_DOUBLE_DIGITS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIGNIFICANT_DOUBLE_DIGITS];
      case ATTRIBUTE_ID_ENCODING_STYLE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODING_STYLE];
      case ATTRIBUTE_ID_ALLOW_CONTROL_CHARACTERS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONTROL_CHARACTERS];
      case ATTRIBUTE_ID_OUTPUT_X_M_L_HEADER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OUTPUT_X_M_L_HEADER];
      case ATTRIBUTE_ID_OUTPUT_X_S_I_ALIAS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_OUTPUT_X_S_I_ALIAS];
      case ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION];
      case ATTRIBUTE_ID_USE_Z_ABBREVIATION_FOR_UTC:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USE_Z_ABBREVIATION_FOR_UTC];
      default:
        return 0;
    }
}

// CREATORS

EncoderOptions::EncoderOptions(bslma::Allocator *basicAllocator)
: d_objectNamespace(basicAllocator)
, d_schemaLocation(basicAllocator)
, d_tag(basicAllocator)
, d_formattingMode(DEFAULT_INITIALIZER_FORMATTING_MODE)
, d_initialIndentLevel(DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL)
, d_spacesPerLevel(DEFAULT_INITIALIZER_SPACES_PER_LEVEL)
, d_wrapColumn(DEFAULT_INITIALIZER_WRAP_COLUMN)
, d_datetimeFractionalSecondPrecision(DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION)
, d_maxDecimalTotalDigits()
, d_maxDecimalFractionDigits()
, d_significantDoubleDigits()
, d_encodingStyle(DEFAULT_INITIALIZER_ENCODING_STYLE)
, d_allowControlCharacters(DEFAULT_INITIALIZER_ALLOW_CONTROL_CHARACTERS)
, d_outputXMLHeader(DEFAULT_INITIALIZER_OUTPUT_X_M_L_HEADER)
, d_outputXSIAlias(DEFAULT_INITIALIZER_OUTPUT_X_S_I_ALIAS)
, d_useZAbbreviationForUtc(DEFAULT_INITIALIZER_USE_Z_ABBREVIATION_FOR_UTC)
{
}

EncoderOptions::EncoderOptions(const EncoderOptions& original,
                               bslma::Allocator *basicAllocator)
: d_objectNamespace(original.d_objectNamespace, basicAllocator)
, d_schemaLocation(original.d_schemaLocation, basicAllocator)
, d_tag(original.d_tag, basicAllocator)
, d_formattingMode(original.d_formattingMode)
, d_initialIndentLevel(original.d_initialIndentLevel)
, d_spacesPerLevel(original.d_spacesPerLevel)
, d_wrapColumn(original.d_wrapColumn)
, d_datetimeFractionalSecondPrecision(original.d_datetimeFractionalSecondPrecision)
, d_maxDecimalTotalDigits(original.d_maxDecimalTotalDigits)
, d_maxDecimalFractionDigits(original.d_maxDecimalFractionDigits)
, d_significantDoubleDigits(original.d_significantDoubleDigits)
, d_encodingStyle(original.d_encodingStyle)
, d_allowControlCharacters(original.d_allowControlCharacters)
, d_outputXMLHeader(original.d_outputXMLHeader)
, d_outputXSIAlias(original.d_outputXSIAlias)
, d_useZAbbreviationForUtc(original.d_useZAbbreviationForUtc)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderOptions::EncoderOptions(EncoderOptions&& original) noexcept
: d_objectNamespace(bsl::move(original.d_objectNamespace))
, d_schemaLocation(bsl::move(original.d_schemaLocation))
, d_tag(bsl::move(original.d_tag))
, d_formattingMode(bsl::move(original.d_formattingMode))
, d_initialIndentLevel(bsl::move(original.d_initialIndentLevel))
, d_spacesPerLevel(bsl::move(original.d_spacesPerLevel))
, d_wrapColumn(bsl::move(original.d_wrapColumn))
, d_datetimeFractionalSecondPrecision(bsl::move(original.d_datetimeFractionalSecondPrecision))
, d_maxDecimalTotalDigits(bsl::move(original.d_maxDecimalTotalDigits))
, d_maxDecimalFractionDigits(bsl::move(original.d_maxDecimalFractionDigits))
, d_significantDoubleDigits(bsl::move(original.d_significantDoubleDigits))
, d_encodingStyle(bsl::move(original.d_encodingStyle))
, d_allowControlCharacters(bsl::move(original.d_allowControlCharacters))
, d_outputXMLHeader(bsl::move(original.d_outputXMLHeader))
, d_outputXSIAlias(bsl::move(original.d_outputXSIAlias))
, d_useZAbbreviationForUtc(bsl::move(original.d_useZAbbreviationForUtc))
{
}

EncoderOptions::EncoderOptions(EncoderOptions&& original,
                               bslma::Allocator *basicAllocator)
: d_objectNamespace(bsl::move(original.d_objectNamespace), basicAllocator)
, d_schemaLocation(bsl::move(original.d_schemaLocation), basicAllocator)
, d_tag(bsl::move(original.d_tag), basicAllocator)
, d_formattingMode(bsl::move(original.d_formattingMode))
, d_initialIndentLevel(bsl::move(original.d_initialIndentLevel))
, d_spacesPerLevel(bsl::move(original.d_spacesPerLevel))
, d_wrapColumn(bsl::move(original.d_wrapColumn))
, d_datetimeFractionalSecondPrecision(bsl::move(original.d_datetimeFractionalSecondPrecision))
, d_maxDecimalTotalDigits(bsl::move(original.d_maxDecimalTotalDigits))
, d_maxDecimalFractionDigits(bsl::move(original.d_maxDecimalFractionDigits))
, d_significantDoubleDigits(bsl::move(original.d_significantDoubleDigits))
, d_encodingStyle(bsl::move(original.d_encodingStyle))
, d_allowControlCharacters(bsl::move(original.d_allowControlCharacters))
, d_outputXMLHeader(bsl::move(original.d_outputXMLHeader))
, d_outputXSIAlias(bsl::move(original.d_outputXSIAlias))
, d_useZAbbreviationForUtc(bsl::move(original.d_useZAbbreviationForUtc))
{
}
#endif

EncoderOptions::~EncoderOptions()
{
}

// MANIPULATORS

EncoderOptions&
EncoderOptions::operator=(const EncoderOptions& rhs)
{
    if (this != &rhs) {
        d_objectNamespace = rhs.d_objectNamespace;
        d_schemaLocation = rhs.d_schemaLocation;
        d_tag = rhs.d_tag;
        d_formattingMode = rhs.d_formattingMode;
        d_initialIndentLevel = rhs.d_initialIndentLevel;
        d_spacesPerLevel = rhs.d_spacesPerLevel;
        d_wrapColumn = rhs.d_wrapColumn;
        d_maxDecimalTotalDigits = rhs.d_maxDecimalTotalDigits;
        d_maxDecimalFractionDigits = rhs.d_maxDecimalFractionDigits;
        d_significantDoubleDigits = rhs.d_significantDoubleDigits;
        d_encodingStyle = rhs.d_encodingStyle;
        d_allowControlCharacters = rhs.d_allowControlCharacters;
        d_outputXMLHeader = rhs.d_outputXMLHeader;
        d_outputXSIAlias = rhs.d_outputXSIAlias;
        d_datetimeFractionalSecondPrecision = rhs.d_datetimeFractionalSecondPrecision;
        d_useZAbbreviationForUtc = rhs.d_useZAbbreviationForUtc;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderOptions&
EncoderOptions::operator=(EncoderOptions&& rhs)
{
    if (this != &rhs) {
        d_objectNamespace = bsl::move(rhs.d_objectNamespace);
        d_schemaLocation = bsl::move(rhs.d_schemaLocation);
        d_tag = bsl::move(rhs.d_tag);
        d_formattingMode = bsl::move(rhs.d_formattingMode);
        d_initialIndentLevel = bsl::move(rhs.d_initialIndentLevel);
        d_spacesPerLevel = bsl::move(rhs.d_spacesPerLevel);
        d_wrapColumn = bsl::move(rhs.d_wrapColumn);
        d_maxDecimalTotalDigits = bsl::move(rhs.d_maxDecimalTotalDigits);
        d_maxDecimalFractionDigits = bsl::move(rhs.d_maxDecimalFractionDigits);
        d_significantDoubleDigits = bsl::move(rhs.d_significantDoubleDigits);
        d_encodingStyle = bsl::move(rhs.d_encodingStyle);
        d_allowControlCharacters = bsl::move(rhs.d_allowControlCharacters);
        d_outputXMLHeader = bsl::move(rhs.d_outputXMLHeader);
        d_outputXSIAlias = bsl::move(rhs.d_outputXSIAlias);
        d_datetimeFractionalSecondPrecision = bsl::move(rhs.d_datetimeFractionalSecondPrecision);
        d_useZAbbreviationForUtc = bsl::move(rhs.d_useZAbbreviationForUtc);
    }

    return *this;
}
#endif

void EncoderOptions::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_objectNamespace);
    bdlat_ValueTypeFunctions::reset(&d_schemaLocation);
    bdlat_ValueTypeFunctions::reset(&d_tag);
    d_formattingMode = DEFAULT_INITIALIZER_FORMATTING_MODE;
    d_initialIndentLevel = DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;
    d_spacesPerLevel = DEFAULT_INITIALIZER_SPACES_PER_LEVEL;
    d_wrapColumn = DEFAULT_INITIALIZER_WRAP_COLUMN;
    bdlat_ValueTypeFunctions::reset(&d_maxDecimalTotalDigits);
    bdlat_ValueTypeFunctions::reset(&d_maxDecimalFractionDigits);
    bdlat_ValueTypeFunctions::reset(&d_significantDoubleDigits);
    d_encodingStyle = DEFAULT_INITIALIZER_ENCODING_STYLE;
    d_allowControlCharacters = DEFAULT_INITIALIZER_ALLOW_CONTROL_CHARACTERS;
    d_outputXMLHeader = DEFAULT_INITIALIZER_OUTPUT_X_M_L_HEADER;
    d_outputXSIAlias = DEFAULT_INITIALIZER_OUTPUT_X_S_I_ALIAS;
    d_datetimeFractionalSecondPrecision = DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION;
    d_useZAbbreviationForUtc = DEFAULT_INITIALIZER_USE_Z_ABBREVIATION_FOR_UTC;
}

// ACCESSORS

bsl::ostream& EncoderOptions::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("objectNamespace", this->objectNamespace());
    printer.printAttribute("schemaLocation", this->schemaLocation());
    printer.printAttribute("tag", this->tag());
    printer.printAttribute("formattingMode", this->formattingMode());
    printer.printAttribute("initialIndentLevel", this->initialIndentLevel());
    printer.printAttribute("spacesPerLevel", this->spacesPerLevel());
    printer.printAttribute("wrapColumn", this->wrapColumn());
    printer.printAttribute("maxDecimalTotalDigits", this->maxDecimalTotalDigits());
    printer.printAttribute("maxDecimalFractionDigits", this->maxDecimalFractionDigits());
    printer.printAttribute("significantDoubleDigits", this->significantDoubleDigits());
    printer.printAttribute("encodingStyle", this->encodingStyle());
    printer.printAttribute("allowControlCharacters", this->allowControlCharacters());
    printer.printAttribute("outputXMLHeader", this->outputXMLHeader());
    printer.printAttribute("outputXSIAlias", this->outputXSIAlias());
    printer.printAttribute("datetimeFractionalSecondPrecision", this->datetimeFractionalSecondPrecision());
    printer.printAttribute("useZAbbreviationForUtc", this->useZAbbreviationForUtc());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// BAS_CODEGEN RUN BY code_from_xsd.pl RUN ON Thu Sep 24 20:40:02 EDT 2020
// GENERATED BY BLP_BAS_CODEGEN_2020.09.14
// USING bas_codegen.pl -m msg -p balxml -E --noExternalization --noAggregateConversion --noHashSupport balxml.xsd

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
