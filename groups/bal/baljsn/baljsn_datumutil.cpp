// baljsn_datumutil.cpp                                               -*-C++-*-
#include <baljsn_datumutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <baljsn_simpleformatter.h>
#include <baljsn_tokenizer.h>

#include <bdlb_chartype.h>
#include <bdlb_numericparseutil.h>
#include <bdld_datum.h>
#include <bdld_datumarraybuilder.h>
#include <bdld_datummapowningkeysbuilder.h>
#include <bdld_manageddatum.h>
#include <bdlde_utf8util.h>
#include <bdlma_bufferedsequentialallocator.h>
#include <bdlsb_memoutstreambuf.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>
#include <bsls_alignedbuffer.h>

namespace BloombergLP {
namespace baljsn {

namespace {

// LOCAL METHODS
static int decodeValue(bdld::ManagedDatum *result,
                       bsl::ostream       *errorStream,
                       baljsn::Tokenizer  *tokenizer,
                       int                 maxNestedDepth);
    // Decode into the specified '*result' the JSON object in the specified
    // '*tokenizer', updating the specified '*errorStream' if any errors are
    // detected, including if the specified 'maxNestedDepth' is exceeded.

static int encodeValue(SimpleFormatter    *formatter,
                       const bdld::Datum&  datum,
                       bool               *foundCheckFailures,
                       bslstl::StringRef  *name = 0);
    // Encode the specified 'datum' as JSON, and output it to the specified
    // 'formatter'.  Update '*foundCheckFailures' to 'true' if any types that
    // aren't fully supported are found.  Optionally specify the 'name' to be
    // used for this value.  Return 0 on success, and a negative value if
    // 'datum' cannot be encoded'.

static int decodeObject(bdld::ManagedDatum *result,
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

        bslstl::StringRef newKey;
        tokenizer->value(&newKey);
        key.assign(newKey);

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
            builder.pushBack(key, elementValue.release());
        }

        // Advance from e_ELEMENT_VALUE to e_ELEMENT_NAME or e_END_OBJECT
        tokenizer->advanceToNextToken();
    }

    result->adopt(builder.commit());
    return 0;
}

static int decodeArray(bdld::ManagedDatum *result,
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

        builder.pushBack(elementValue.release());

        // Advance from e_ELEMENT_VALUE to e_ELEMENT_VALUE or e_END_ARRAY
        tokenizer->advanceToNextToken();
    }

    result->adopt(builder.commit());
    return 0;
}

static int extractString(bsl::string       *stringValue,
                         bslstl::StringRef  readBuffer)
    // Extract into the specified '*stringValue' the interpreted value of the
    // string in the specified 'readBuffer'.
{
    bsl::size_t cursor = 0;
    if ('"' != readBuffer[cursor]) {
        return -1;                                                    // RETURN
    }
    else {
        ++cursor;
    }

    bool finished        = false;
    bool isEscapedChar   = false;
    bool isUnicodeEscape = false;
    int  unicodeValue    = 0;
    char unicodePos      = 0;

    while (!finished) {
        const bsl::size_t readSize = readBuffer.size();

        while (!finished && cursor < readSize) {
            const char currentChar = readBuffer[cursor];

            if (isUnicodeEscape) {
                if (currentChar >= '0' && currentChar <= '9') {
                    unicodeValue = (16 * unicodeValue) + (currentChar - '0');
                }
                else if (currentChar >= 'A' && currentChar <= 'F') {
                    unicodeValue =
                                (16 * unicodeValue) + (10 + currentChar - 'A');
                }
                else if (currentChar >= 'a' && currentChar <= 'f') {
                    unicodeValue =
                                (16 * unicodeValue) + (10 + currentChar - 'a');
                }
                else {
                    // Invalid unicode escape sequence
                    return -1;                                        // RETURN
                }

                ++unicodePos;

                if (4 == unicodePos) {
                    isUnicodeEscape = false;
                    int rc          = bdlde::Utf8Util::appendUtf8Character(
                                 stringValue, unicodeValue);
                    if (0 != rc) {
                        // Invalid UTF8 sequence
                        return -1;                                    // RETURN
                    }
                }
            }
            else if (isEscapedChar) {
                isEscapedChar = false;

                switch (currentChar) {
                  case '"':   // FALL THROUGH
                  case '\\':  // FALL THROUGH
                  case '/':
                    stringValue->push_back(currentChar);
                    break;
                  case 'b':
                    stringValue->push_back('\b');
                    break;
                  case 'f':
                    stringValue->push_back('\f');
                    break;
                  case 'n':
                    stringValue->push_back('\n');
                    break;
                  case 'r':
                    stringValue->push_back('\r');
                    break;
                  case 't':
                    stringValue->push_back('\t');
                    break;
                  case 'u':
                    isUnicodeEscape = true;
                    unicodeValue    = 0;
                    unicodePos      = 0;
                    break;
                  default:
                    // Invalid escape sequence
                    return -1;                                        // RETURN
                }
            }
            else if ('\\' == currentChar) {
                isEscapedChar = true;
            }
            else if ('"' == currentChar) {
                finished = true;
            }
            else {
                if (bdlb::CharType::isCntrl(currentChar)) {
                    // un-escaped control sequence
                    return -1;                                        // RETURN
                }

                stringValue->push_back(currentChar);
            }

            ++cursor;
        }

        // If we have no data, bail out
        if (!finished && cursor >= readBuffer.length()) {
            // Run out of data
            return -1;                                                // RETURN
        }
    }

    return 0;
}

static int extractValue(bdld::ManagedDatum *result,
                        baljsn::Tokenizer  *tokenizer)
    // Extract into the specified '*result' the current value in the specified
    // '*tokenizer'.
{
    bslstl::StringRef value;
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

        if (0 == extractString(&str, value)) {
            result->adopt(bdld::Datum::copyString(str, result->allocator()));
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

static int decodeValue(bdld::ManagedDatum *result,
                       bsl::ostream       *errorStream,
                       baljsn::Tokenizer  *tokenizer,
                       int                 maxNestedDepth)
{
    switch (tokenizer->tokenType()) {
      case baljsn::Tokenizer::e_START_OBJECT: {
        int rc =
            decodeObject(result, errorStream, tokenizer, maxNestedDepth - 1);
        if (0 != rc) {
            if (errorStream) {
                *errorStream << "decodeObject failed, rc = " << rc << '\n';
            }
            return -1;                                                // RETURN
        }
      } break;
      case baljsn::Tokenizer::e_START_ARRAY: {
        int rc =
            decodeArray(result, errorStream, tokenizer, maxNestedDepth - 1);
        if (0 != rc) {
            if (errorStream) {
                *errorStream << "decodeArray failed, rc = " << rc << '\n';
            }
            return -2;                                                // RETURN
        }
      } break;
      case baljsn::Tokenizer::e_ELEMENT_VALUE: {
        if (0 != extractValue(result, tokenizer)) {
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

static int encodeArray(SimpleFormatter            *formatter,
                       const bdld::DatumArrayRef&  datum,
                       bool                       *foundCheckFailures,
                       bslstl::StringRef          *name = 0)
    // Encode the specified 'datum' as a JSON array representation, and output
    // it to the specified 'formatter'.  Update '*foundCheckFailures' to 'true'
    // if any types that aren't fully supported are found.  Optionally specify
    // the 'name' to be used for this array.  Return 0 on success, and a
    // negative value if 'datum' cannot be encoded.
{
    if (name) {
        formatter->addMemberName(*name);
    }

    SimpleFormatter::ArrayFormattingStyle style =
        datum.length()
            ? SimpleFormatter::e_REGULAR_ARRAY_FORMAT
            : SimpleFormatter::e_EMPTY_ARRAY_FORMAT;

    formatter->openArray(style);

    int result = 0;
    for (bsl::size_t i = 0; 0 == result && i < datum.length(); ++i) {
        result = encodeValue(formatter, datum[i], foundCheckFailures);
    }

    formatter->closeArray(style);

    return result;
}

static int encodeObject(SimpleFormatter          *formatter,
                        const bdld::DatumMapRef&  datum,
                        bool                     *foundCheckFailures,
                        bslstl::StringRef        *name = 0)
    // Encode the specified 'datum' as a JSON object representation, and output
    // it to the specified 'formatter'.  Update '*foundCheckFailures' to 'true'
    // if any types that aren't fully supported are found.  Optionally specify
    // the 'name' to be used for this object.  Return 0 on success, and a
    // negative value if 'datum' cannot be encoded.
{
    if (name) {
        formatter->addMemberName(*name);
    }

    formatter->openObject();

    int result = 0;
    for (bsl::size_t i = 0; 0 == result && i < datum.size(); ++i) {
        bslstl::StringRef name(datum[i].key());
        result = encodeValue(formatter, datum[i].value(), foundCheckFailures,
                             &name);
    }
    formatter->closeObject();
    return result;
}

static int encodeValue(SimpleFormatter    *formatter,
                       const bdld::Datum&  datum,
                       bool               *foundCheckFailures,
                       bslstl::StringRef  *name)
{
    int                   result = -1;
    bdld::Datum::DataType type   = datum.type();

    if (name) {
        formatter->addMemberName(*name);
    }

    switch(type) {
      case bdld::Datum::e_MAP: {
        result = encodeObject(formatter, datum.theMap(), foundCheckFailures);
      } break;
      case bdld::Datum::e_ARRAY: {
        result = encodeArray(formatter, datum.theArray(), foundCheckFailures);
      } break;
      case bdld::Datum::e_INTEGER: {
        formatter->addValue(datum.theInteger());
        *foundCheckFailures = true;
        result = 0;
      } break;
      case bdld::Datum::e_DOUBLE: {
        formatter->addValue(datum.theDouble());
        result = 0;
      } break;
      case bdld::Datum::e_STRING: {
        formatter->addValue(datum.theString());
        result = 0;
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
        *foundCheckFailures = true;
        result = 0;
      } break;
      case bdld::Datum::e_TIME: {
        formatter->addValue(datum.theTime());
        *foundCheckFailures = true;
        result = 0;
      } break;
      case bdld::Datum::e_DATETIME: {
        formatter->addValue(datum.theDatetime());
        *foundCheckFailures = true;
        result = 0;
      } break;
      case bdld::Datum::e_DATETIME_INTERVAL: {
        formatter->addValue(datum.theDatetimeInterval());
        *foundCheckFailures = true;
        result = 0;
      } break;
      case bdld::Datum::e_INTEGER64: {
        formatter->addValue(static_cast<double>(datum.theInteger64()));
        *foundCheckFailures = true;
        result = 0;
      } break;
      case bdld::Datum::e_DECIMAL64: {
        formatter->addValue(datum.theDecimal64());
        *foundCheckFailures = true;
        result = 0;
      } break;
      default: {
        *foundCheckFailures = true;
        result = -1;
      } break;
    }
    return result;
}

}  // close unnamed namespace

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

    int rc =
        decodeValue(&value, errorStream, &tokenizer, options.maxNestedDepth());
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

    result->adopt(value.release());
    return 0;
}

int DatumUtil::encode(bsl::string                *result,
                      const bdld::Datum&          datum,
                      const DatumEncoderOptions&  options)
{
    bsls::AlignedBuffer<8 * 1024>      buffer;
    bdlma::BufferedSequentialAllocator bsa(
        buffer.buffer(), sizeof(buffer));

    bdlsb::MemOutStreamBuf streambuf(&bsa);
    bsl::ostream           stream(&streambuf);

    int rc = encode(stream, datum, options);

    if (0 <= rc) {
        result->assign(streambuf.data(), streambuf.length());
    }

    return rc;
}

int DatumUtil::encode(bsl::ostream&              stream,
                      const bdld::Datum&         datum,
                      const DatumEncoderOptions& options)
{
    EncoderOptions encoderOptions;

    encoderOptions.setEncodingStyle(options.encodingStyle());
    encoderOptions.setInitialIndentLevel(options.initialIndentLevel());
    encoderOptions.setSpacesPerLevel(options.spacesPerLevel());

    SimpleFormatter formatter(stream, encoderOptions);

    bool foundCheckFailures = false;

    int rc = encodeValue(&formatter, datum, &foundCheckFailures);

    if (foundCheckFailures && options.strictTypes() && 0 == rc) {
        rc = 1;
    }

    return rc;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
