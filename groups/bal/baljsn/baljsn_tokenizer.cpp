// baljsn_tokenizer.cpp                                               -*-C++-*-
#include <baljsn_tokenizer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_tokenizer_cpp,"$Id$ $CSID$")

#include <bdlb_chartype.h>

#include <bsl_cstring.h>
#include <bsl_ios.h>
#include <bsl_streambuf.h>

#include <baljsn_parserutil.h>                 // for testing only

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

namespace BloombergLP {
namespace {

    static const char *WHITESPACE = " \n\t\v\f\r";
    static const char *TOKENS     = "{}[]:,";

}  // close unnamed namespace

namespace baljsn {

                              // ----------------
                              // struct Tokenizer
                              // ----------------

// PRIVATE MANIPULATORS
int Tokenizer::reloadStringBuffer()
{
    d_stringBuffer.resize(k_MAX_STRING_SIZE);
    const int numRead =
                     static_cast<int>(d_streambuf_p->sgetn(&d_stringBuffer[0],
                                                           k_MAX_STRING_SIZE));
    d_cursor = 0;
    d_stringBuffer.resize(numRead);
    return numRead;
}

int Tokenizer::expandBufferForLargeValue()
{
    d_stringBuffer.resize(d_stringBuffer.length() + k_MAX_STRING_SIZE);

    const int numRead =
            static_cast<int>(d_streambuf_p->sgetn(&d_stringBuffer[d_valueIter],
                                                  k_MAX_STRING_SIZE));
    return numRead ? 0 : -1;
}

int Tokenizer::moveValueCharsToStartAndReloadBuffer()
{
    d_stringBuffer.erase(d_stringBuffer.begin(),
                         d_stringBuffer.begin() + d_valueBegin);
    d_stringBuffer.resize(k_MAX_STRING_SIZE);

    d_valueIter = d_valueIter - d_valueBegin;

    const int numRead =
       static_cast<int>(d_streambuf_p->sgetn(&d_stringBuffer[d_valueIter],
                                             k_MAX_STRING_SIZE - d_valueIter));

    if (numRead > 0) {
        d_stringBuffer.resize(d_valueIter + numRead);
        d_valueBegin = 0;
    }

    return numRead;
}

int Tokenizer::skipWhitespace()
{
    while (true) {
        bsl::size_t pos = d_stringBuffer.find_first_not_of(WHITESPACE,
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

int Tokenizer::extractStringValue()
{
    bool firstTime    = true;
    char previousChar = 0;

    while (true) {
        while (d_valueIter < d_stringBuffer.length()
            && '"' != d_stringBuffer[d_valueIter]) {

            if ('\\' == d_stringBuffer[d_valueIter]
             && '\\' == previousChar) {
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
            // buffer to hold additional characters.

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

int Tokenizer::skipNonWhitespaceOrTillToken()
{
    bool firstTime = true;

    while (true) {
        while (d_valueIter < d_stringBuffer.length()
            && !bdlb::CharType::isSpace(d_stringBuffer[d_valueIter])
            && !bsl::strchr(TOKENS, d_stringBuffer[d_valueIter])) {
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
             || e_BEGIN         == d_tokenType) {

                d_tokenType  = e_START_OBJECT;
                d_context    = e_OBJECT_CONTEXT;
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

                d_tokenType  = e_END_OBJECT;
                d_context    = e_OBJECT_CONTEXT;
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
             || e_BEGIN         == d_tokenType) {

                d_tokenType  = e_START_ARRAY;
                d_context    = e_ARRAY_CONTEXT;
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

                d_tokenType = e_END_ARRAY;
                d_context   = e_OBJECT_CONTEXT;
                previousChar = ']';

                ++d_cursor;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }
          } break;

          case ',': {
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
            // CURRENT TOKEN           CONTEXT           NEXT TOKEN
            // -------------           -------           ----------
            // START_OBJECT  ('{')                       ELEMENT_NAME
            // END_OBJECT    ('}')                       ELEMENT_NAME
            // START_ARRAY   ('[')                       ELEMENT_VALUE
            // END_ARRAY     (']')                       ELEMENT_VALUE
            // ELEMENT_NAME  (':')                       ELEMENT_VALUE
            // ELEMENT_VALUE (   )     OBJECT_CONTEXT    ELEMENT_NAME
            // ELEMENT_VALUE (   )     ARRAY_CONTEXT     ELEMENT_VALUE

            if (e_START_OBJECT   == d_tokenType
             || (e_END_OBJECT    == d_tokenType && ',' == previousChar)
             || (e_END_ARRAY     == d_tokenType && ',' == previousChar)
             || (e_ELEMENT_VALUE   == d_tokenType
               && ','                   == previousChar
               && e_OBJECT_CONTEXT == d_context)) {
                d_tokenType  = e_ELEMENT_NAME;
                d_valueBegin = d_cursor + 1;
                d_valueIter  = d_valueBegin;
            }
            else if (e_START_ARRAY    == d_tokenType
                  || (e_ELEMENT_NAME  == d_tokenType
                                                        && ':' == previousChar)
                  || (e_ELEMENT_VALUE == d_tokenType
                   && ','                  == previousChar
                   && e_ARRAY_CONTEXT == d_context)
                 || (e_BEGIN == d_tokenType && d_allowStandAloneValues)) {
                d_tokenType  = e_ELEMENT_VALUE;
                d_valueBegin = d_cursor;
                d_valueIter  = d_valueBegin + 1;
            }
            else {
                d_tokenType = e_ERROR;
                return -1;                                            // RETURN
            }

            d_valueEnd = 0;
            int rc = extractStringValue();
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
            if (e_START_ARRAY    == d_tokenType
             || (e_ELEMENT_NAME  == d_tokenType && ':' == previousChar)
             || (e_ELEMENT_VALUE == d_tokenType
              && ','                  == previousChar
              && e_ARRAY_CONTEXT == d_context)
             || (e_BEGIN == d_tokenType && d_allowStandAloneValues)) {

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
                                                            bsl::ios_base::end,
                                                            bsl::ios_base::in);

    return newPos >= 0 ? 0 : -1;
}

// ACCESSORS
int Tokenizer::value(bslstl::StringRef *data) const
{
    if ((e_ELEMENT_NAME == d_tokenType
                                        || e_ELEMENT_VALUE == d_tokenType)
     && d_valueBegin != d_valueEnd) {
        data->assign(&d_stringBuffer[d_valueBegin],
                     &d_stringBuffer[d_valueEnd]);
        return 0;                                                     // RETURN
    }
    return -1;
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
