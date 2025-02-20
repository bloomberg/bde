// baljsn_decoderoptions.cpp                                          -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_decoderoptions_cpp, "$Id$ $CSID$")

#include <baljsn_decoderoptions.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>  // 'bsl::memcpy'
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>  // 'bsl::move'

///Implementation Note
///- - - - - - - - - -
// This file was generated from a script and was subsequently modified to add
// documentation and to make other changes.  The steps to generate and update
// this file can be found in the 'doc/generating_codec_options.txt' file.

namespace BloombergLP {
namespace baljsn {

                            // --------------------
                            // class DecoderOptions
                            // --------------------

// CONSTANTS

const char DecoderOptions::CLASS_NAME[] = "DecoderOptions";

const int DecoderOptions::DEFAULT_INITIALIZER_MAX_DEPTH = 512;

const bool DecoderOptions::DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS = true;

const bool DecoderOptions::DEFAULT_INITIALIZER_VALIDATE_INPUT_IS_UTF8 = false;

const bool DecoderOptions::DEFAULT_INITIALIZER_ALLOW_CONSECUTIVE_SEPARATORS = true;

const bool DecoderOptions::DEFAULT_INITIALIZER_ALLOW_FORM_FEED_AS_WHITESPACE = true;

const bool DecoderOptions::DEFAULT_INITIALIZER_ALLOW_UNESCAPED_CONTROL_CHARACTERS = true;

const bdlat_AttributeInfo DecoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_MAX_DEPTH,
        "MaxDepth",
        sizeof("MaxDepth") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS,
        "SkipUnknownElements",
        sizeof("SkipUnknownElements") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8,
        "ValidateInputIsUtf8",
        sizeof("ValidateInputIsUtf8") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ALLOW_CONSECUTIVE_SEPARATORS,
        "AllowConsecutiveSeparators",
        sizeof("AllowConsecutiveSeparators") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ALLOW_FORM_FEED_AS_WHITESPACE,
        "AllowFormFeedAsWhitespace",
        sizeof("AllowFormFeedAsWhitespace") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ALLOW_UNESCAPED_CONTROL_CHARACTERS,
        "AllowUnescapedControlCharacters",
        sizeof("AllowUnescapedControlCharacters") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *DecoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 6; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    DecoderOptions::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *DecoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH];
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
      case ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8];
      case ATTRIBUTE_ID_ALLOW_CONSECUTIVE_SEPARATORS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_CONSECUTIVE_SEPARATORS];
      case ATTRIBUTE_ID_ALLOW_FORM_FEED_AS_WHITESPACE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_FORM_FEED_AS_WHITESPACE];
      case ATTRIBUTE_ID_ALLOW_UNESCAPED_CONTROL_CHARACTERS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW_UNESCAPED_CONTROL_CHARACTERS];
      default:
        return 0;
    }
}

// CREATORS

DecoderOptions::DecoderOptions()
: d_maxDepth(DEFAULT_INITIALIZER_MAX_DEPTH)
, d_skipUnknownElements(DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS)
, d_validateInputIsUtf8(DEFAULT_INITIALIZER_VALIDATE_INPUT_IS_UTF8)
, d_allowConsecutiveSeparators(DEFAULT_INITIALIZER_ALLOW_CONSECUTIVE_SEPARATORS)
, d_allowFormFeedAsWhitespace(DEFAULT_INITIALIZER_ALLOW_FORM_FEED_AS_WHITESPACE)
, d_allowUnescapedControlCharacters(DEFAULT_INITIALIZER_ALLOW_UNESCAPED_CONTROL_CHARACTERS)
{
}

DecoderOptions::DecoderOptions(const DecoderOptions& original)
: d_maxDepth(original.d_maxDepth)
, d_skipUnknownElements(original.d_skipUnknownElements)
, d_validateInputIsUtf8(original.d_validateInputIsUtf8)
, d_allowConsecutiveSeparators(original.d_allowConsecutiveSeparators)
, d_allowFormFeedAsWhitespace(original.d_allowFormFeedAsWhitespace)
, d_allowUnescapedControlCharacters(original.d_allowUnescapedControlCharacters)
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
        d_skipUnknownElements = rhs.d_skipUnknownElements;
        d_validateInputIsUtf8 = rhs.d_validateInputIsUtf8;
        d_allowConsecutiveSeparators = rhs.d_allowConsecutiveSeparators;
        d_allowFormFeedAsWhitespace = rhs.d_allowFormFeedAsWhitespace;
        d_allowUnescapedControlCharacters =
                                         rhs.d_allowUnescapedControlCharacters;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
DecoderOptions&
DecoderOptions::operator=(DecoderOptions&& rhs)
{
    if (this != &rhs) {
        d_maxDepth = bsl::move(rhs.d_maxDepth);
        d_skipUnknownElements = bsl::move(rhs.d_skipUnknownElements);
        d_validateInputIsUtf8 = bsl::move(rhs.d_validateInputIsUtf8);

        d_allowConsecutiveSeparators =
                                    bsl::move(rhs.d_allowConsecutiveSeparators);
        d_allowFormFeedAsWhitespace =
                                     bsl::move(rhs.d_allowFormFeedAsWhitespace);
        d_allowUnescapedControlCharacters =
                               bsl::move(rhs.d_allowUnescapedControlCharacters);
    }
    return *this;
}
#endif

void DecoderOptions::reset()
{
    d_maxDepth = DEFAULT_INITIALIZER_MAX_DEPTH;
    d_skipUnknownElements = DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;
    d_validateInputIsUtf8 = DEFAULT_INITIALIZER_VALIDATE_INPUT_IS_UTF8;
    d_allowConsecutiveSeparators = DEFAULT_INITIALIZER_ALLOW_CONSECUTIVE_SEPARATORS;
    d_allowFormFeedAsWhitespace = DEFAULT_INITIALIZER_ALLOW_FORM_FEED_AS_WHITESPACE;
    d_allowUnescapedControlCharacters = DEFAULT_INITIALIZER_ALLOW_UNESCAPED_CONTROL_CHARACTERS;
}

// ACCESSORS

bsl::ostream& DecoderOptions::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("maxDepth", this->maxDepth());
    printer.printAttribute("skipUnknownElements", this->skipUnknownElements());
    printer.printAttribute("validateInputIsUtf8", this->validateInputIsUtf8());
    printer.printAttribute("allowConsecutiveSeparators", this->allowConsecutiveSeparators());
    printer.printAttribute("allowFormFeedAsWhitespace", this->allowFormFeedAsWhitespace());
    printer.printAttribute("allowUnescapedControlCharacters", this->allowUnescapedControlCharacters());
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2024.03.02
// USING bas_codegen.pl -m msg -p baljsn -E --noExternalization --noAggregateConversion ../baljsn.xsd
// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
