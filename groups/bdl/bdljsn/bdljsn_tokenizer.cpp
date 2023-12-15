// bdljsn_tokenizer.cpp                                               -*-C++-*-
#include <bdljsn_tokenizer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_tokenizer_cpp, "$Id$ $CSID$")

#include <bdlb_chartype.h>
#include <bdlde_utf8util.h>
#include <bdlsb_fixedmemoutstreambuf.h>

#include <bsl_cstring.h>
#include <bsl_ios.h>

// IMPLEMENTATION NOTES
// --------------------
// The following table provides the various transitions that need to be handled
// with the tokenizer.
//
//   Current Token             Curr Char    Next Char         Following Token
//   -------------             ---------    ---------         ---------------
//   BEGIN                       BEGIN        '{'              START_OBJECT
//   NAME                         ':'         '{'              START_OBJECT
//   START_ARRAY                  '['         '{'              START_OBJECT
//   END_OBJECT                   ','         '{'              START_OBJECT
//
//   START_OBJECT                 '{'         '"'              NAME
//   VALUE                        ','         '"'              NAME
//   END_OBJECT                   ','         '"'              NAME
//   END_ARRAY                    ','         '"'              NAME
//
//   NAME                         ':'         '"'              VALUE (string)
//   NAME                         ':'        Number            VALUE (number)
//   START_ARRAY                  '['         '"'              VALUE (string)
//   START_ARRAY                  '['        Number            VALUE (number)
//   VALUE                        ','         '"'              VALUE (string)
//   VALUE                        ','        Number            VALUE (number)
//
//   START_OBJECT                 '{'         '}'              END_OBJECT
//   VALUE (number)              Number       '}'              END_OBJECT
//   VALUE (string)               '"'         '}'              END_OBJECT
//   END_OBJECT                   '}'         '}'              END_OBJECT
//   END_ARRAY                    ']'         '}'              END_OBJECT
//
//   NAME                         ':'         '['              START_ARRAY
//   START_ARRAY                  '['         '['              START_ARRAY
//   END_ARRAY                    ','         '['              START_ARRAY
//
//   START_ARRAY                  '['         ']'              END_ARRAY
//   VALUE (number)              Number       ']'              END_ARRAY
//   VALUE (string)               '"'         ']'              END_ARRAY
//   END_OBJECT                   '}'         ']'              END_ARRAY
//   END_ARRAY                    ']'         ']'              END_ARRAY
//..
//
// Note that the implementation must allow changes to tokenizer options after
// tokenization has begun.  And in no case should a situation arise in which
// the same state of tokenizer is legal with one combination of options and
// illegal with others.

namespace BloombergLP {
namespace {

static const char *g_WHITESPACE = " \n\t\v\f\r";
static const char *g_TOKENS     = "{}[]:,\"";

}  // close unnamed namespace

namespace bdljsn {

                              // ---------------
                              // class Tokenizer
                              // ---------------

// PRIVATE MANIPULATORS
int Tokenizer::expandBufferForLargeValue()
{
    const bsl::string::size_type currLength = d_stringBuffer.length();
    d_stringBuffer.resize(currLength + k_MAX_STRING_SIZE);

    bsl::size_t numRead;
    if (d_readStatus || d_bufEndStatus) {
        numRead = 0;
    }
    else if (d_allowNonUtf8StringLiterals) {
        numRead = static_cast<bsl::size_t>(d_streambuf_p->sgetn(
                                                  &d_stringBuffer[d_valueIter],
                                                  k_MAX_STRING_SIZE));
    }
    else {
        int sts = 0;
        numRead = bdlde::Utf8Util::readIfValid(&sts,
                                               &d_stringBuffer[d_valueIter],
                                               k_MAX_STRING_SIZE,
                                               d_streambuf_p);
        if (sts < 0) {
            d_bufEndStatus = sts;
        }
        else {
            // should be success or buffer full

            BSLS_ASSERT(0 == sts || k_MAX_STRING_SIZE < numRead + 4);
        }
    }

    if (0 == d_readStatus && 0 == numRead) {
        d_readStatus = 0 == d_bufEndStatus ? k_EOF : d_bufEndStatus;
    }

    d_readOffset += numRead;
    d_stringBuffer.resize(currLength + numRead);
    return numRead ? 0 : -1;
}

int Tokenizer::extractStringValue()
{
    bool firstTime    = true;
    char previousChar = 0;

    while (true) {
        while (d_valueIter < d_stringBuffer.length() &&
               '"' != d_stringBuffer[d_valueIter]) {
            if ('\\' == d_stringBuffer[d_valueIter] && '\\' == previousChar) {
                previousChar = 0;
            }
            else {
                previousChar = d_stringBuffer[d_valueIter];
            }

            ++d_valueIter;
        }

        if (d_valueIter >= d_stringBuffer.length()) {
            // There isn't enough room in the internal buffer to hold the
            // value.  If this is the first time through the loop, we move the
            // current sequence of characters being processed to the front of
            // the internal buffer, otherwise we must expand the internal
            // buffer to hold additional characters.  If we are at the
            // beginning of the string buffer then we don't need to move any
            // characters and we simply expand the string buffer.

            if (0 == d_valueBegin) {
                firstTime = false;
            }

            if (firstTime) {
                const int numRead = moveValueCharsToStartAndReloadBuffer();
                if (0 == numRead) {
                    return -1;                                        // RETURN
                }

                firstTime = false;
            }
            else {
                const int rc = expandBufferForLargeValue();
                if (rc) {
                    return rc;                                        // RETURN
                }
            }
        }
        else {
            if ('\\' == previousChar) {
                ++d_valueIter;
                previousChar = 0;
                continue;
            }
            d_valueEnd = d_valueIter;
            return 0;                                                 // RETURN
        }
    }
    return 0;
}

int Tokenizer::moveValueCharsToStartAndReloadBuffer()
{
    d_stringBuffer.erase(d_stringBuffer.begin(),
                         d_stringBuffer.begin() + d_valueBegin);
    d_stringBuffer.resize(k_MAX_STRING_SIZE);

    d_valueIter  = d_valueIter - d_valueBegin;
    d_valueBegin = 0;

    bsl::size_t numRead;
    if (d_readStatus || d_bufEndStatus) {
        numRead = 0;
    }
    else if (d_allowNonUtf8StringLiterals) {
        numRead = static_cast<bsl::size_t>(d_streambuf_p->sgetn(
                                             &d_stringBuffer[d_valueIter],
                                             k_MAX_STRING_SIZE - d_valueIter));
    }
    else {
        int sts = 0;
        numRead = bdlde::Utf8Util::readIfValid(&sts,
                                               &d_stringBuffer[d_valueIter],
                                               k_MAX_STRING_SIZE - d_valueIter,
                                               d_streambuf_p);
        if (sts < 0) {
            d_bufEndStatus = sts;
        }
        else {
            // should be success or buffer full

            BSLS_ASSERT(0 == sts || k_MAX_STRING_SIZE - d_valueIter < numRead +
                                                                          4);
        }
    }

    if (0 == d_readStatus && 0 == numRead) {
        d_readStatus = 0 == d_bufEndStatus ? k_EOF : d_bufEndStatus;
    }

    d_readOffset += numRead;
    d_stringBuffer.resize(d_valueIter + numRead);

    return static_cast<int>(numRead);
}

int Tokenizer::reloadStringBuffer()
{
    d_stringBuffer.resize(k_MAX_STRING_SIZE);

    bsl::size_t numRead;
    if (d_readStatus || d_bufEndStatus) {
        numRead = 0;
    }
    else if (d_allowNonUtf8StringLiterals) {
        numRead = static_cast<bsl::size_t>(
                  d_streambuf_p->sgetn(&d_stringBuffer[0], k_MAX_STRING_SIZE));
    }
    else {
        int sts = 0;
        numRead = bdlde::Utf8Util::readIfValid(&sts,
                                               &d_stringBuffer[0],
                                               k_MAX_STRING_SIZE,
                                               d_streambuf_p);

        if (sts < 0) {
            d_bufEndStatus = sts;
        }
        else {
            // should be success or buffer full

            BSLS_ASSERT(0 == sts || k_MAX_STRING_SIZE < numRead + 4);
        }
    }

    if (0 == d_readStatus && 0 == numRead) {
        d_readStatus = 0 == d_bufEndStatus ? k_EOF : d_bufEndStatus;
    }

    d_readOffset += numRead;
    d_cursor = 0;
    d_stringBuffer.resize(numRead);
    return static_cast<int>(numRead);
}

int Tokenizer::skipNonWhitespaceOrTillToken()
{
    bool firstTime = true;

    while (true) {
        while (d_valueIter < d_stringBuffer.length() &&
               !bdlb::CharType::isSpace(d_stringBuffer[d_valueIter]) &&
               !bsl::strchr(g_TOKENS, d_stringBuffer[d_valueIter])) {
            ++d_valueIter;
        }

        if (d_valueIter >= d_stringBuffer.length()) {
            // There isn't enough room in the internal buffer to hold the
            // value.  If this is the first time through the loop, we move the
            // current sequence of characters being processed to the front of
            // the internal buffer, otherwise we must expand the internal
            // buffer to hold additional characters.

            if (firstTime) {
                const int numRead = moveValueCharsToStartAndReloadBuffer();
                if (0 == numRead) {
                    if (d_readStatus < 0) {
                        return -1;                                    // RETURN
                    }

                    d_valueEnd = d_valueIter;
                    return 0;                                         // RETURN
                }
                firstTime = false;
            }
            else {
                const int rc = expandBufferForLargeValue();
                if (rc) {
                    return rc;                                        // RETURN
                }
            }
        }
        else {
            d_valueEnd = d_valueIter;
            return 0;                                                 // RETURN
        }
    }
    return 0;
}

int Tokenizer::skipWhitespace()
{
    while (true) {
        bsl::size_t pos = d_stringBuffer.find_first_not_of(g_WHITESPACE,
                                                           d_cursor);
        if (bsl::string::npos != pos) {
            d_cursor = pos;
            break;
        }

        const int numRead = reloadStringBuffer();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}

// MANIPULATORS
int Tokenizer::advanceToNextToken()
{
    if (e_ERROR == d_tokenType) {
        return -1;                                                    // RETURN
    }

    if (d_cursor >= d_stringBuffer.size()) {
        const int numRead = reloadStringBuffer();
        if (0 == numRead) {
            d_tokenType = e_ERROR;
            return -1;                                                // RETURN
        }
    }

    bool continueFlag;
    char previousChar = 0;
    do {
        continueFlag = false;

        const int rc = skipWhitespace();
        if (rc) {
            d_tokenType = e_ERROR;
            return -1;                                                // RETURN
        }

        switch (d_stringBuffer[d_cursor]) {
          case '{': {
            if ((e_ELEMENT_NAME == d_tokenType && ':' == previousChar)
             || e_START_ARRAY   == d_tokenType
             || (e_END_OBJECT   == d_tokenType && ',' == previousChar)
             || (d_allowHeterogenousArrays
                 && e_ARRAY_CONTEXT == context()
                 && ','             == previousChar)
             || e_BEGIN         == d_tokenType) {

                d_tokenType  = e_START_OBJECT;
                pushContext(e_OBJECT_CONTEXT);
                previousChar = '{';

                ++d_cursor;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }
          } break;

          case '}': {
            if ((e_ELEMENT_VALUE == d_tokenType && ',' != previousChar)
             || e_START_OBJECT   == d_tokenType
             || e_END_OBJECT     == d_tokenType
             || e_END_ARRAY      == d_tokenType) {

                // Soldier onward on mismatches - '{DRQS 162368278}'
                if (e_NO_CONTEXT != context()) {
                    popContext();
                }
                d_tokenType  = e_END_OBJECT;
                previousChar = '}';

                ++d_cursor;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }
          } break;

          case '[': {
            if ((e_ELEMENT_NAME == d_tokenType && ':' == previousChar)
             || e_START_ARRAY   == d_tokenType
             || (e_END_ARRAY    == d_tokenType && ',' == previousChar)
             || (d_allowHeterogenousArrays
                 && e_ARRAY_CONTEXT == context()
                 && ','             == previousChar)
             || e_BEGIN         == d_tokenType) {

                d_tokenType  = e_START_ARRAY;
                pushContext(e_ARRAY_CONTEXT);
                previousChar = '[';

                ++d_cursor;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }
          } break;

          case ']': {
            if ((e_ELEMENT_VALUE == d_tokenType && ',' != previousChar)
             || e_START_ARRAY    == d_tokenType
             || (e_END_ARRAY     == d_tokenType && ',' != previousChar)
             || (e_END_OBJECT    == d_tokenType && ',' != previousChar)) {

                // Soldier onward on mismatches - '{DRQS 162368278}'
                if (e_NO_CONTEXT != context()) {
                    popContext();
                }
                d_tokenType = e_END_ARRAY;
                previousChar = ']';

                ++d_cursor;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }
          } break;

          case ',': {
            if (!d_allowTrailingTopLevelComma && e_NO_CONTEXT == context()) {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }

            if (e_ELEMENT_VALUE == d_tokenType
             || e_END_OBJECT    == d_tokenType
             || e_END_ARRAY     == d_tokenType) {

                previousChar = ',';
                continueFlag = true;

                ++d_cursor;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }
          } break;

          case ':': {
            if (e_ELEMENT_NAME == d_tokenType) {

                previousChar = ':';
                continueFlag = true;

                ++d_cursor;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }
          } break;

          case '"': {
            // Here are the scenarios for a '"':
            //
            // CURRENT TOKEN           CONTEXT                    NEXT TOKEN
            // -------------           -------                    ----------
            // BEGIN         (   )     n/a                        ELEMENT_VALUE
            //                                 NOTE: req. allowStandAloneValues
            // START_OBJECT  ('{')     OBJECT_CONTEXT             ELEMENT_NAME
            //
            // END_OBJECT    ('}')     OBJECT_CONTEXT             ELEMENT_NAME
            //                         ARRAY_CONTEXT              ELEMENT_VALUE
            //                                 NOTE: req. prevChar==','
            //
            // START_ARRAY   ('[')              ARRAY_CONTEXT     ELEMENT_VALUE
            //
            // END_ARRAY     (']')     OBJECT_CONTEXT             ELEMENT_NAME
            //                         ARRAY_CONTEXT              ELEMENT_VALUE
            //                                 NOTE: req. prevChar==','
            //
            // ELEMENT_NAME  (':')     OBJECT_CONTEXT             ELEMENT_VALUE
            //
            // ELEMENT_VALUE (   )     OBJECT_CONTEXT             ELEMENT_NAME
            //                         ARRAY_CONTEXT              ELEMENT_VALUE
            //                                 NOTE: req. prevChar==','

            switch (d_tokenType) {
              case e_BEGIN: {
                if (!d_allowStandAloneValues) {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }

                d_tokenType = e_ELEMENT_VALUE;
              } break;

              case e_START_OBJECT: {
                if (e_OBJECT_CONTEXT != context()) {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }

                d_tokenType = e_ELEMENT_NAME;
              } break;

              case e_ELEMENT_VALUE: {
                if (previousChar != ',') {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }

                if (e_OBJECT_CONTEXT == context()) {
                    d_tokenType = e_ELEMENT_NAME;
                }
                else if (e_ARRAY_CONTEXT == context()) {
                    d_tokenType = e_ELEMENT_VALUE;
                }
                else {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }
              } break;

              case e_END_ARRAY:                                  // FALLTHROUGH
              case e_END_OBJECT: {
                if (previousChar != ',') {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }

                if (e_OBJECT_CONTEXT == context()) {
                    d_tokenType = e_ELEMENT_NAME;
                }
                else if (e_ARRAY_CONTEXT == context()) {
                    d_tokenType = e_ELEMENT_VALUE;
                }
                else {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }
              } break;

              case e_START_ARRAY: {
                if (e_ARRAY_CONTEXT != context()) {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }

                d_tokenType = e_ELEMENT_VALUE;
              } break;

              case e_ELEMENT_NAME: {
                if (previousChar != ':') {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }

                if (e_OBJECT_CONTEXT != context()) {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }
                else {
                    d_tokenType = e_ELEMENT_VALUE;
                }
              } break;

              default: {
                d_tokenType = e_ERROR;                                // RETURN
              } break;
            }

            if (e_ELEMENT_NAME == d_tokenType) {
                d_valueBegin = d_cursor + 1;
                d_valueIter  = d_valueBegin;
            }

            if (e_ELEMENT_VALUE == d_tokenType) {
                d_valueBegin = d_cursor;
                d_valueIter  = d_valueBegin + 1;
            }

            d_valueEnd = 0;
            int rc     = extractStringValue();
            if (rc) {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }

            if (e_ELEMENT_NAME == d_tokenType) {
                d_cursor = d_valueEnd + 1;
            }
            else {
                // Advance past the end '"'.

                ++d_valueEnd;
                d_cursor = d_valueEnd;
            }

            previousChar = '"';
          } break;

          default: {
            // Here are the scenarios for what could be a value:
            //
            // CURRENT TOKEN           CONTEXT                    NEXT TOKEN
            // -------------           -------                    ----------
            // e_BEGIN                 N/A                        ELEMENT_VALUE
            //                                 NOTE: req. allowStandAloneValues
            // START_OBJECT  ('{')     OBJECT_CONTEXT             **ERROR**
            //
            // END_OBJECT    ('}')     OBJECT_CONTEXT             **ERROR**
            //                         ARRAY_CONTEXT              ELEMENT_VALUE
            //
            // START_ARRAY   ('[')     ARRAY_CONTEXT              ELEMENT_VALUE
            //
            // END_ARRAY     (']')     OBJECT_CONTEXT             **ERROR**
            //                         ARRAY_CONTEXT              ELEMENT_VALUE
            //
            // ELEMENT_NAME  (':')     OBJECT_CONTEXT             ELEMENT_VALUE
            //                                 NOTE: req. prevChar==':'
            //
            // ELEMENT_VALUE (   )     OBJECT_CONTEXT             **ERROR**
            //                         ARRAY_CONTEXT              ELEMENT_VALUE
            //                                 NOTE: req. prevChar==','

            bool validObjectValue = e_OBJECT_CONTEXT == context() &&
                                    e_ELEMENT_NAME == d_tokenType &&
                                    ':' == previousChar;
            bool validArrayValue =
                e_ARRAY_CONTEXT == context() &&
                ((e_END_OBJECT == d_tokenType) ||
                 (e_START_ARRAY == d_tokenType) ||
                 (e_END_ARRAY == d_tokenType) ||
                 (e_ELEMENT_VALUE == d_tokenType && ',' == previousChar));

            bool validStandaloneValue =
                (e_BEGIN == d_tokenType && d_allowStandAloneValues);

            if (validObjectValue || validArrayValue || validStandaloneValue) {
                d_tokenType = e_ELEMENT_VALUE;

                d_valueBegin = d_cursor;
                d_valueEnd   = 0;
                d_valueIter  = d_valueBegin + 1;

                const int rc = skipNonWhitespaceOrTillToken();
                if (rc) {
                    d_tokenType = e_ERROR;
                    return -1;                                        // RETURN
                }
                d_cursor     = d_valueEnd;
                previousChar = 0;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }
          } break;
        }
    } while (continueFlag);

    return 0;
}

int Tokenizer::resetStreamBufGetPointer()
{
    if (d_cursor >= d_stringBuffer.size()) {
        return 0;                                                     // RETURN
    }

    const int numExtraCharsRead = static_cast<int>(d_stringBuffer.size()
                                                                   - d_cursor);
    const bsl::streamoff newPos = d_streambuf_p->pubseekoff(-numExtraCharsRead,
                                                            bsl::ios_base::cur,
                                                            bsl::ios_base::in);
    if (numExtraCharsRead) {
        d_readStatus = 0;
        d_bufEndStatus = 0;
    }

    return newPos >= 0 ? 0 : -1;
}

// ACCESSORS
int Tokenizer::value(bsl::string_view *data) const
{
    if ((e_ELEMENT_NAME == d_tokenType || e_ELEMENT_VALUE == d_tokenType) &&
        d_valueBegin != d_valueEnd) {

        *data = bsl::string_view(d_stringBuffer).substr(
                                                    d_valueBegin,
                                                    d_valueEnd - d_valueBegin);

        return 0;                                                     // RETURN
    }
    return -1;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
