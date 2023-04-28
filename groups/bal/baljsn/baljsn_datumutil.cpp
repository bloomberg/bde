// baljsn_datumutil.cpp                                               -*-C++-*-
#include <baljsn_datumutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <baljsn_parserutil.h>
#include <baljsn_simpleformatter.h>
#include <baljsn_tokenizer.h>

#include <bdlb_chartype.h>
#include <bdlb_float.h>
#include <bdlb_numericparseutil.h>

#include <bdld_datum.h>
#include <bdld_datumarraybuilder.h>
#include <bdld_datummapowningkeysbuilder.h>
#include <bdld_manageddatum.h>

#include <bdlde_utf8util.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bdlsb_memoutstreambuf.h>

#include <bsls_alignedbuffer.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>

namespace {
namespace u {

using namespace BloombergLP;

// LOCAL METHODS
int decodeValue(bdld::ManagedDatum *result,
                bsl::ostream       *errorStream,
                baljsn::Tokenizer  *tokenizer,
                int                 maxNestedDepth);
    // Decode into the specified '*result' the JSON object in the specified
    // '*tokenizer', updating the specified '*errorStream' if any errors are
    // detected, including if the specified 'maxNestedDepth' is exceeded.

int encodeValue(baljsn::SimpleFormatter    *formatter,
                const bdld::Datum&          datum,
                int                        *strictTypesCheckStatus,
                bsl::string_view           *name = 0);
    // Encode the specified 'datum' as JSON, and output it to the specified
    // 'formatter'.  Update '*strictTypesCheckStatus' to the appropriate
    // positve integer value if any types or singular double values that aren't
    // fully supported are found, allowing further encoding to proceed.
    // '*strictTypesCheckStatus' remains unchanged if no unsupported types or
    // values are encountered.  Optionally specify the 'name' to be used for
    // this value.  Return 0 on success, and a negative value if 'datum' cannot
    // be encoded', which should stop further encoding.

int decodeObject(bdld::ManagedDatum *result,
                 bsl::ostream       *errorStream,
                 baljsn::Tokenizer  *tokenizer,
                 int                 maxNestedDepth)
    // Decode into the specified '*result' the JSON object in the specified
    // '*tokenizer', updating the specified '*errorStream' if any errors are
    // detected, including if the specified 'maxNestedDepth' is exceeded.
{
    if (maxNestedDepth < 0) {
        if (errorStream) {
            *errorStream << "Maximum nesting depth exceeded";
        }
        return -4;                                                    // RETURN
    }

    // Advance from e_START_OBJECT
    tokenizer->advanceToNextToken();
    if (baljsn::Tokenizer::e_ERROR == tokenizer->tokenType()) {
        if (errorStream) {
            *errorStream << "Unexpected token";
        }
        return -1;                                                    // RETURN
    }

    bsl::unordered_set<bsl::string> keys;
    bsl::string                     key;

    bdld::DatumMapOwningKeysBuilder builder(result->allocator());

    while (baljsn::Tokenizer::e_END_OBJECT != tokenizer->tokenType()) {
        // If not e_END_OBJECT, we expect e_ELEMENT_NAME
        if (baljsn::Tokenizer::e_ELEMENT_NAME != tokenizer->tokenType()) {
            return -2;                                                // RETURN
        }

        bsl::string_view tokenContents;
        tokenizer->value(&tokenContents);

        bsl::string key;
        if (0 != baljsn::ParserUtil::getUnquotedString(&key, tokenContents))
        {
            if (errorStream) {
                *errorStream
                    << "decodeValue: getUnquotedString for key failed\n";
            }
            return -3;                                                // RETURN
        }

        // Advance from e_ELEMENT_NAME.  decodeValue checks the token, so we
        // don't need to do it here.
        tokenizer->advanceToNextToken();

        bdld::ManagedDatum elementValue(result->allocator());

        int rc =
            decodeValue(&elementValue, errorStream, tokenizer, maxNestedDepth);

        if (0 != rc) {
            if (errorStream) {
                *errorStream << "decodeValue failed, rc = " << rc << '\n';
            }
            return -3;                                                // RETURN
        }

        // Keep the FIRST instance of any duplicate keys.
        if (keys.find(key) == keys.end()) {
            keys.insert(key);
            builder.pushBack(key, elementValue.datum());
            elementValue.release();
        }

        // Advance from e_ELEMENT_VALUE to e_ELEMENT_NAME or e_END_OBJECT
        tokenizer->advanceToNextToken();
    }

    result->adopt(builder.commit());
    return 0;
}

int decodeArray(bdld::ManagedDatum *result,
                bsl::ostream       *errorStream,
                baljsn::Tokenizer  *tokenizer,
                int                 maxNestedDepth)
    // Decode into the specified '*result' the JSON array in the specified
    // '*tokenizer', updating the specified '*errorStream' if any errors are
    // detected, including if the specified 'maxNestedDepth' is exceeded.
{
    if (maxNestedDepth < 0) {
        if (errorStream) {
            *errorStream << "Maximum nesting depth exceeded";
        }
        return -4;                                                    // RETURN
    }

    // Advance from e_START_ARRAY
    tokenizer->advanceToNextToken();
    if (baljsn::Tokenizer::e_ERROR == tokenizer->tokenType()) {
        if (errorStream) {
            *errorStream << "Unexpected token";
        }
        return -1;                                                    // RETURN
    }

    bdld::DatumArrayBuilder builder(result->allocator());

    while (baljsn::Tokenizer::e_END_ARRAY != tokenizer->tokenType()) {
        // decodeValue checks the token, so we don't need to do it here.
        bdld::ManagedDatum elementValue(result->allocator());

        int rc =
            decodeValue(&elementValue, errorStream, tokenizer, maxNestedDepth);

        if (0 != rc) {
            if (errorStream) {
                *errorStream << "decodeValue failed, rc = " << rc << '\n';
            }
            return -2;                                                // RETURN
        }

        builder.pushBack(elementValue.datum());
        elementValue.release();

        // Advance from e_ELEMENT_VALUE to e_ELEMENT_VALUE or e_END_ARRAY
        tokenizer->advanceToNextToken();
    }

    result->adopt(builder.commit());
    return 0;
}

template <class STRING>
inline
int encodeImp(STRING                             *result,
              const bdld::Datum&                  datum,
              const baljsn::DatumEncoderOptions&  options)
{
    bsls::AlignedBuffer<8 * 1024>      buffer;
    bdlma::BufferedSequentialAllocator bsa(
        buffer.buffer(), sizeof(buffer));

    bdlsb::MemOutStreamBuf streambuf(&bsa);
    bsl::ostream           stream(&streambuf);

    int rc = baljsn::DatumUtil::encode(stream, datum, options);

    if (0 <= rc) {
        result->assign(streambuf.data(), streambuf.length());
    }

    return rc;
}

int extractValue(bdld::ManagedDatum *result,
                 baljsn::Tokenizer  *tokenizer)
    // Extract into the specified '*result' the current value in the specified
    // '*tokenizer'.
{
    bsl::string_view value;
    tokenizer->value(&value);

    if ("true" == value || "false" == value) {
        result->adopt(bdld::Datum::createBoolean("true" == value));
        return 0;                                                     // RETURN
    }

    if ("null" == value) {
        result->adopt(bdld::Datum::createNull());
        return 0;                                                     // RETURN
    }

    if ('"' == value[0]) {
        bsl::string str(result->allocator());

        if (0 == baljsn::ParserUtil::getValue(&str, value)) {
            result->adopt(bdld::Datum::copyString(str, result->allocator()));
        }
        else {
            return -1;                                                // RETURN
        }

        return 0;                                                     // RETURN
    }

    double            d;
    bslstl::StringRef remainder;
    if (0 == bdlb::NumericParseUtil::parseDouble(&d, &remainder, value) &&
        0 == remainder.length()) {
        result->adopt(bdld::Datum::createDouble(d));
        return 0;                                                     // RETURN
    }

    return -1;
}

int decodeValue(bdld::ManagedDatum *result,
                bsl::ostream       *errorStream,
                baljsn::Tokenizer  *tokenizer,
                int                 maxNestedDepth)
{
    switch (tokenizer->tokenType()) {
      case baljsn::Tokenizer::e_START_OBJECT: {
        int rc = u::decodeObject(
                           result, errorStream, tokenizer, maxNestedDepth - 1);
        if (0 != rc) {
            if (errorStream) {
                *errorStream << "decodeObject failed, rc = " << rc << '\n';
            }
            return -1;                                                // RETURN
        }
      } break;
      case baljsn::Tokenizer::e_START_ARRAY: {
        int rc = u::decodeArray(
                           result, errorStream, tokenizer, maxNestedDepth - 1);
        if (0 != rc) {
            if (errorStream) {
                *errorStream << "decodeArray failed, rc = " << rc << '\n';
            }
            return -2;                                                // RETURN
        }
      } break;
      case baljsn::Tokenizer::e_ELEMENT_VALUE: {
        if (0 != u::extractValue(result, tokenizer)) {
            return -3;                                                // RETURN
        }
      } break;
      default: {
        if (errorStream) {
            *errorStream << "Unexpected token: "
                         << tokenizer->tokenType() << '\n';
        }
        return -3;                                                    // RETURN
      } break;
    }

    return 0;
}

int encodeArray(baljsn::SimpleFormatter    *formatter,
                const bdld::DatumArrayRef&  datum,
                int                        *strictTypesCheckStatus,
                bsl::string_view           *name = 0)
    // Encode the specified 'datum' as a JSON array representation, and output
    // it to the specified 'formatter'.  Update '*strictTypesCheckStatus' to
    // the appropriate positve integer value if any types or singular double
    // values that aren't fully supported are found, allowing further encoding
    // to proceed.  '*strictTypesCheckStatus' remains unchanged if no
    // unsupported types or values are encountered.  Optionally specify the
    // 'name' to be used for this array.  Return 0 on success, and a negative
    // value if 'datum' cannot be encoded, which should stop further encoding.
{
    if (name) {
        formatter->addMemberName(*name);
    }

    baljsn::SimpleFormatter::ArrayFormattingStyle style =
        datum.length()
            ? baljsn::SimpleFormatter::e_REGULAR_ARRAY_FORMAT
            : baljsn::SimpleFormatter::e_EMPTY_ARRAY_FORMAT;

    formatter->openArray(style);

    int result = 0;
    for (bsl::size_t i = 0; 0 == result && i < datum.length(); ++i) {
        result = u::encodeValue(
            formatter, datum[i], strictTypesCheckStatus);
    }

    formatter->closeArray(style);

    return result;
}

int encodeObject(baljsn::SimpleFormatter  *formatter,
                 const bdld::DatumMapRef&  datum,
                 int                      *strictTypesCheckStatus,
                 bsl::string_view         *name = 0)
    // Encode the specified 'datum' as a JSON object representation, and output
    // it to the specified 'formatter'.  Update '*strictTypesCheckStatus' to
    // the appropriate positve integer value if any types or singular double
    // values that aren't fully supported are found, allowing further encoding
    // to proceed.  '*strictTypesCheckStatus' remains unchanged if no
    // unsupported types or values are encountered.Optionally specify the
    // 'name' to be used for this object.  Return 0 on success, and a negative
    // value if 'datum' cannot be encoded, which should stop further encoding.
{
    if (name) {
        formatter->addMemberName(*name);
    }

    formatter->openObject();

    int result = 0;
    for (bsl::size_t i = 0; 0 == result && i < datum.size(); ++i) {
        bsl::string_view name(datum[i].key());
        result = u::encodeValue(formatter,
                                datum[i].value(),
                                strictTypesCheckStatus,
                                &name);
    }
    formatter->closeObject();
    return result;
}

int encodeValue(baljsn::SimpleFormatter    *formatter,
                const bdld::Datum&          datum,
                int                        *strictTypesCheckStatus,
                bsl::string_view           *name)
{
    int                   result = -1;
    bdld::Datum::DataType type   = datum.type();

    if (name) {
        formatter->addMemberName(*name);
    }

    switch(type) {
      case bdld::Datum::e_MAP: {
        result = u::encodeObject(
            formatter, datum.theMap(), strictTypesCheckStatus);
      } break;
      case bdld::Datum::e_ARRAY: {
        result = u::encodeArray(
            formatter, datum.theArray(), strictTypesCheckStatus);
      } break;
      case bdld::Datum::e_INTEGER: {
        formatter->addValue(datum.theInteger());
        *strictTypesCheckStatus = 1;
        result = 0;
      } break;
      case bdld::Datum::e_DOUBLE: {
        double theDouble = datum.theDouble();
        int rc = formatter->addValue(theDouble);
        if (0 != rc) {
            result = 2;
        }
        else {
            switch (bdlb::Float::classifyFine(theDouble)) {
              case bdlb::Float::k_POSITIVE_INFINITY:            // FALL-THROUGH
              case bdlb::Float::k_NEGATIVE_INFINITY:            // FALL-THROUGH
              case bdlb::Float::k_QNAN:                         // FALL-THROUGH
              case bdlb::Float::k_SNAN: {
                *strictTypesCheckStatus   = 2;
                result              = 0;
              } break;
              default: {
                result = 0;
              } break;
            }
        }
      } break;
      case bdld::Datum::e_STRING: {
        if (0 != formatter->addValue(datum.theString())) {
            result = -1;
        }
        else {
            result = 0;
        }
      } break;
      case bdld::Datum::e_BOOLEAN: {
        formatter->addValue(datum.theBoolean());
        result = 0;
      } break;
      case bdld::Datum::e_NIL: {
        formatter->addNullValue();
        result = 0;
      } break;
      case bdld::Datum::e_DATE: {
        formatter->addValue(datum.theDate());
        *strictTypesCheckStatus   = 1;
        result = 0;
      } break;
      case bdld::Datum::e_TIME: {
        formatter->addValue(datum.theTime());
        *strictTypesCheckStatus   = 1;
        result = 0;
      } break;
      case bdld::Datum::e_DATETIME: {
        formatter->addValue(datum.theDatetime());
        *strictTypesCheckStatus   = 1;
        result = 0;
      } break;
      case bdld::Datum::e_DATETIME_INTERVAL: {
        formatter->addValue(datum.theDatetimeInterval());
        *strictTypesCheckStatus   = 1;
        result = 0;
      } break;
      case bdld::Datum::e_INTEGER64: {
        formatter->addValue(static_cast<double>(datum.theInteger64()));
        *strictTypesCheckStatus   = 1;
        result = 0;
      } break;
      case bdld::Datum::e_DECIMAL64: {
        formatter->addValue(datum.theDecimal64());
        *strictTypesCheckStatus   = 1;
        result = 0;
      } break;
      default: {
        *strictTypesCheckStatus   = 1;
        result = -1;
      } break;
    }
    return result;
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace baljsn {

                              // ----------------
                              // struct DatumUtil
                              // ----------------

// CLASS METHODS
int DatumUtil::decode(bdld::ManagedDatum         *result,
                      bsl::ostream               *errorStream,
                      bsl::streambuf             *jsonBuffer,
                      const DatumDecoderOptions&  options)
{
    bsls::AlignedBuffer<8 * 1024>      buffer;
    bdlma::BufferedSequentialAllocator bsa(
        buffer.buffer(), sizeof(buffer));

    baljsn::Tokenizer tokenizer(&bsa);
    tokenizer.setAllowNonUtf8StringLiterals(false);
    tokenizer.reset(jsonBuffer);

    // Advance from e_BEGIN
    tokenizer.advanceToNextToken();
    if (baljsn::Tokenizer::e_ERROR == tokenizer.tokenType()) {
        if (errorStream) {
            *errorStream << "Unexpected token";
        }
        return -1;                                                    // RETURN
    }

    bdld::ManagedDatum value(result->allocator());

    int rc = u::decodeValue(
                    &value, errorStream, &tokenizer, options.maxNestedDepth());
    if (0 != rc) {
        if (errorStream) {
            *errorStream << "decodeValue failed, rc = " << rc << '\n';
        }
        return -2;                                                    // RETURN
    }

    if (0 == tokenizer.advanceToNextToken()) {
        if (errorStream) {
            *errorStream << "decodeValue failed, extra token detected after "
                            "value, rc = "
                         << -3 << '\n';
        }
        return -3;                                                    // RETURN
    }

    result->adopt(value.datum());
    value.release();
    return 0;
}

int DatumUtil::encode(bsl::string                *result,
                      const bdld::Datum&          datum,
                      const DatumEncoderOptions&  options)
{
    return u::encodeImp(result, datum, options);
}

int DatumUtil::encode(std::string                *result,
                      const bdld::Datum&          datum,
                      const DatumEncoderOptions&  options)
{
    return u::encodeImp(result, datum, options);
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
int DatumUtil::encode(std::pmr::string           *result,
                      const bdld::Datum&          datum,
                      const DatumEncoderOptions&  options)
{
    return u::encodeImp(result, datum, options);
}
#endif

int DatumUtil::encode(bsl::ostream&              stream,
                      const bdld::Datum&         datum,
                      const DatumEncoderOptions& options)
{
    EncoderOptions encoderOptions;

    encoderOptions.setEncodingStyle(options.encodingStyle());
    encoderOptions.setEncodeQuotedDecimal64(options.encodeQuotedDecimal64());
    encoderOptions.setInitialIndentLevel(options.initialIndentLevel());
    encoderOptions.setSpacesPerLevel(options.spacesPerLevel());

    encoderOptions.setEncodeInfAndNaNAsStrings(true);

    SimpleFormatter formatter(stream, encoderOptions);

    int  strictTypesCheckStatus   = 0;

    int rc = u::encodeValue(&formatter, datum, &strictTypesCheckStatus);

    if (0 != strictTypesCheckStatus && options.strictTypes() && 0 == rc) {
        rc = strictTypesCheckStatus;
    }

    return rc;
}

}  // close package namespace
}  // close enterprise namespace

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
