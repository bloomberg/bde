// balxml_decoderoptions.cpp         *DO NOT EDIT*         @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_decoderoptions_cpp, "$Id$ $CSID$")

#include <balxml_decoderoptions.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace balxml {

                            // --------------------
                            // class DecoderOptions
                            // --------------------

// CONSTANTS

const char DecoderOptions::CLASS_NAME[] = "DecoderOptions";

const int DecoderOptions::DEFAULT_INITIALIZER_MAX_DEPTH = 512;

const int DecoderOptions::DEFAULT_INITIALIZER_FORMATTING_MODE = 0;

const bool DecoderOptions::DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS = true;

const bool DecoderOptions::DEFAULT_INITIALIZER_VALIDATE_INPUT_IS_UTF8 = false;

const bool DecoderOptions::DEFAULT_INITIALIZER_VALIDATE_ROOT_TAG = false;

const bdlat_AttributeInfo DecoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_MAX_DEPTH,
        "MaxDepth",
        sizeof("MaxDepth") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_FORMATTING_MODE,
        "FormattingMode",
        sizeof("FormattingMode") - 1,
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
        ATTRIBUTE_ID_VALIDATE_ROOT_TAG,
        "ValidateRootTag",
        sizeof("ValidateRootTag") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *DecoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 5; ++i) {
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
      case ATTRIBUTE_ID_FORMATTING_MODE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FORMATTING_MODE];
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
      case ATTRIBUTE_ID_VALIDATE_INPUT_IS_UTF8:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_INPUT_IS_UTF8];
      case ATTRIBUTE_ID_VALIDATE_ROOT_TAG:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALIDATE_ROOT_TAG];
      default:
        return 0;
    }
}

// CREATORS

DecoderOptions::DecoderOptions()
: d_maxDepth(DEFAULT_INITIALIZER_MAX_DEPTH)
, d_formattingMode(DEFAULT_INITIALIZER_FORMATTING_MODE)
, d_skipUnknownElements(DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS)
, d_validateInputIsUtf8(DEFAULT_INITIALIZER_VALIDATE_INPUT_IS_UTF8)
, d_validateRootTag(DEFAULT_INITIALIZER_VALIDATE_ROOT_TAG)
{
}

DecoderOptions::DecoderOptions(const DecoderOptions& original)
: d_maxDepth(original.d_maxDepth)
, d_formattingMode(original.d_formattingMode)
, d_skipUnknownElements(original.d_skipUnknownElements)
, d_validateInputIsUtf8(original.d_validateInputIsUtf8)
, d_validateRootTag(original.d_validateRootTag)
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
        d_validateInputIsUtf8 = rhs.d_validateInputIsUtf8;
        d_validateRootTag = rhs.d_validateRootTag;
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
        d_formattingMode = bsl::move(rhs.d_formattingMode);
        d_skipUnknownElements = bsl::move(rhs.d_skipUnknownElements);
        d_validateInputIsUtf8 = bsl::move(rhs.d_validateInputIsUtf8);
        d_validateRootTag = bsl::move(rhs.d_validateRootTag);
    }

    return *this;
}
#endif

void DecoderOptions::reset()
{
    d_maxDepth = DEFAULT_INITIALIZER_MAX_DEPTH;
    d_formattingMode = DEFAULT_INITIALIZER_FORMATTING_MODE;
    d_skipUnknownElements = DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;
    d_validateInputIsUtf8 = DEFAULT_INITIALIZER_VALIDATE_INPUT_IS_UTF8;
    d_validateRootTag = DEFAULT_INITIALIZER_VALIDATE_ROOT_TAG;
}

// ACCESSORS

bsl::ostream& DecoderOptions::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("maxDepth", this->maxDepth());
    printer.printAttribute("formattingMode", this->formattingMode());
    printer.printAttribute("skipUnknownElements", this->skipUnknownElements());
    printer.printAttribute("validateInputIsUtf8", this->validateInputIsUtf8());
    printer.printAttribute("validateRootTag", this->validateRootTag());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl -m msg -p balxml -E --noExternalization --noAggregateConversion --noHashSupport balxml.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2023 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
