// balber_berencoderoptions.cpp       *DO NOT EDIT*        @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berencoderoptions_cpp, "$Id$ $CSID$")

#include <balber_berencoderoptions.h>

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
namespace balber {

                          // -----------------------
                          // class BerEncoderOptions
                          // -----------------------

// CONSTANTS

const char BerEncoderOptions::CLASS_NAME[] = "BerEncoderOptions";

const int BerEncoderOptions::DEFAULT_INITIALIZER_TRACE_LEVEL = 0;

const int BerEncoderOptions::DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE = 10500;

const bool BerEncoderOptions::DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS = true;

const bool BerEncoderOptions::DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY = false;

const int BerEncoderOptions::DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION = 3;

const bool BerEncoderOptions::DEFAULT_INITIALIZER_DISABLE_UNSELECTED_CHOICE_ENCODING = false;

const bool BerEncoderOptions::DEFAULT_INITIALIZER_PRESERVE_SIGN_OF_NEGATIVE_ZERO = false;

const bool BerEncoderOptions::DEFAULT_INITIALIZER_ENCODE_ARRAY_LENGTH_HINTS = false;

const bdlat_AttributeInfo BerEncoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_TRACE_LEVEL,
        "TraceLevel",
        sizeof("TraceLevel") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE,
        "BdeVersionConformance",
        sizeof("BdeVersionConformance") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS,
        "EncodeEmptyArrays",
        sizeof("EncodeEmptyArrays") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY,
        "EncodeDateAndTimeTypesAsBinary",
        sizeof("EncodeDateAndTimeTypesAsBinary") - 1,
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
        ATTRIBUTE_ID_DISABLE_UNSELECTED_CHOICE_ENCODING,
        "DisableUnselectedChoiceEncoding",
        sizeof("DisableUnselectedChoiceEncoding") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_PRESERVE_SIGN_OF_NEGATIVE_ZERO,
        "PreserveSignOfNegativeZero",
        sizeof("PreserveSignOfNegativeZero") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ENCODE_ARRAY_LENGTH_HINTS,
        "EncodeArrayLengthHints",
        sizeof("EncodeArrayLengthHints") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BerEncoderOptions::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 8; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BerEncoderOptions::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BerEncoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_TRACE_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL];
      case ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE];
      case ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS];
      case ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY];
      case ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION];
      case ATTRIBUTE_ID_DISABLE_UNSELECTED_CHOICE_ENCODING:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DISABLE_UNSELECTED_CHOICE_ENCODING];
      case ATTRIBUTE_ID_PRESERVE_SIGN_OF_NEGATIVE_ZERO:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_SIGN_OF_NEGATIVE_ZERO];
      case ATTRIBUTE_ID_ENCODE_ARRAY_LENGTH_HINTS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_ARRAY_LENGTH_HINTS];
      default:
        return 0;
    }
}

// CREATORS

BerEncoderOptions::BerEncoderOptions()
: d_traceLevel(DEFAULT_INITIALIZER_TRACE_LEVEL)
, d_bdeVersionConformance(DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE)
, d_datetimeFractionalSecondPrecision(DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION)
, d_encodeEmptyArrays(DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS)
, d_encodeDateAndTimeTypesAsBinary(DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY)
, d_disableUnselectedChoiceEncoding(DEFAULT_INITIALIZER_DISABLE_UNSELECTED_CHOICE_ENCODING)
, d_preserveSignOfNegativeZero(DEFAULT_INITIALIZER_PRESERVE_SIGN_OF_NEGATIVE_ZERO)
, d_encodeArrayLengthHints(DEFAULT_INITIALIZER_ENCODE_ARRAY_LENGTH_HINTS)
{
}

// MANIPULATORS

void BerEncoderOptions::reset()
{
    d_traceLevel = DEFAULT_INITIALIZER_TRACE_LEVEL;
    d_bdeVersionConformance = DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE;
    d_encodeEmptyArrays = DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS;
    d_encodeDateAndTimeTypesAsBinary = DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY;
    d_datetimeFractionalSecondPrecision = DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION;
    d_disableUnselectedChoiceEncoding = DEFAULT_INITIALIZER_DISABLE_UNSELECTED_CHOICE_ENCODING;
    d_preserveSignOfNegativeZero = DEFAULT_INITIALIZER_PRESERVE_SIGN_OF_NEGATIVE_ZERO;
    d_encodeArrayLengthHints = DEFAULT_INITIALIZER_ENCODE_ARRAY_LENGTH_HINTS;
}

// ACCESSORS

bsl::ostream& BerEncoderOptions::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("traceLevel", this->traceLevel());
    printer.printAttribute("bdeVersionConformance", this->bdeVersionConformance());
    printer.printAttribute("encodeEmptyArrays", this->encodeEmptyArrays());
    printer.printAttribute("encodeDateAndTimeTypesAsBinary", this->encodeDateAndTimeTypesAsBinary());
    printer.printAttribute("datetimeFractionalSecondPrecision", this->datetimeFractionalSecondPrecision());
    printer.printAttribute("disableUnselectedChoiceEncoding", this->disableUnselectedChoiceEncoding());
    printer.printAttribute("preserveSignOfNegativeZero", this->preserveSignOfNegativeZero());
    printer.printAttribute("encodeArrayLengthHints", this->encodeArrayLengthHints());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2025.05.15
// USING bas_codegen.pl -m msg --msgExpand -p balber --noAggregateConversion --noHashSupport -c berencoderoptions balber.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
