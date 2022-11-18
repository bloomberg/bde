// bdljsn_stringutil.cpp                                              -*-C++-*-
#include <bdljsn_stringutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_stringutil_cpp, "$Id$ $CSID$")

#include <bdlde_charconvertutf32.h>
#include <bdlde_utf8util.h>

#include <bdlb_numericparseutil.h>

#include <bsls_annotation.h>
#include <bsls_types.h>

namespace {
namespace u {

inline
static
void writeEscapedChar(bsl::ostream&   stream,
                      const char    **currentStart,
                      const char     *iter,
                      const char      value)
    // Write the sequence of characters in the range specified by
    // '[*currentStart, iter)' to the specified 'stream' followed by the escape
    // character ('\\') and the specified 'value' character.  After that update
    // '*currentStart' to point to the address following 'iter'.
{
    stream.write(*currentStart, iter - *currentStart);
    stream.put('\\');
    stream.put(value);
    *currentStart = iter + 1;
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdljsn {

                             // -----------------
                             // struct StringUtil
                             // -----------------

// CLASS METHODS
int StringUtil::readUnquotedString(bsl::string             *value,
                                   const bsl::string_view&  string,
                                   int                      flags)
{
    BSLS_ASSERT(value);

    value->clear();
    value->reserve(string.length());

    const bool acceptCapitalUnicodeEscape =
                flags & static_cast<unsigned>(e_ACCEPT_CAPITAL_UNICODE_ESCAPE);

    for (const char *iter = string.data(),
                    *end  = string.data() + string.length();
                     iter < end; ++iter) {
        if ('\\' == *iter) {
            ++iter;
            if (iter >= end) {
                return -1;                                            // RETURN
            }

            switch (*iter) {
              case 'b': {
                *value += '\b';
              } break;
              case 'f': {
                *value += '\f';
              } break;
              case 'n': {
                *value += '\n';
              } break;
              case 'r': {
                *value += '\r';
              } break;
              case 't': {
                *value += '\t';
              } break;
              case '"':
                BSLS_ANNOTATION_FALLTHROUGH;
              case '\\':
                BSLS_ANNOTATION_FALLTHROUGH;
              case '/': {
                // printable characters

                *value += *iter;
              } break;
              case 'U': {
                if (!acceptCapitalUnicodeEscape) {
                    return -1;                                        // RETURN
                }
              }                                                 // FALL THROUGH
              case 'u': {
                enum { k_NUM_UNICODE_DIGITS = 4 };

                if (k_NUM_UNICODE_DIGITS >= end - iter) {
                    return -1;                                        // RETURN
                }

                ++iter;

                bsls::Types::Uint64 digits;
                bsl::string_view    rest;
                if (0 != bdlb::NumericParseUtil::parseUnsignedInteger(
                                  &digits,
                                  &rest,
                                  bsl::string_view(iter, k_NUM_UNICODE_DIGITS),
                                  16,
                                  0xFFFFULL,
                                  k_NUM_UNICODE_DIGITS)
                 || 0 < rest.size()) {
                    return -1;                                        // RETURN
                }

                unsigned int utf32input = static_cast<unsigned int>(digits);

                // Value by which to increment 'iter'.  (3 instead of 4 because
                // 'iter' is incremented at the end of the function.)
                int increment = 3;

                // Check for supplementary plane encodings.  These consist of a
                // pair of Unicode 16-bit values, the first in 'D800..DBFF' and
                // the second in 'DC00..DFFF'.

                unsigned int first = utf32input;

                if (0xD800 <= first && first <= 0xDBFF) {
                    // Check that another Unicode escape sequence follows.
                    if (iter + 2 * k_NUM_UNICODE_DIGITS + 1 >= end) {
                        return -1;                                    // RETURN
                    }
                    if ('\\' != iter[k_NUM_UNICODE_DIGITS]) {
                        return -1;                                    // RETURN
                    }
                    if ('u' != iter[k_NUM_UNICODE_DIGITS + 1] &&
                        'U' != iter[k_NUM_UNICODE_DIGITS + 1]) {
                        return -1;                                    // RETURN
                    }

                    if (0 != bdlb::NumericParseUtil::parseUnsignedInteger(
                              &digits,
                              &rest,
                              bsl::string_view(iter + k_NUM_UNICODE_DIGITS + 2,
                                               k_NUM_UNICODE_DIGITS),
                              16,
                              0xDFFFULL,
                              k_NUM_UNICODE_DIGITS)
                     || 0      < rest.size()
                     || digits < 0xDC00) {
                        return -1;                                    // RETURN
                    }

                    unsigned int second = static_cast<unsigned int>(digits);

                    // Combine the two 16-bit halves into one 21-bit whole.
                    utf32input = 0x010000
                               + ((first - 0xD800) << 10)
                               + (second - 0xDC00);

                    // Increment 'increment' to skip past second character.
                    increment += 6;
                }

                // Due to the short string optimization there won't be a memory
                // allocation here.

                bsl::string utf8String;
                const int   rc = bdlde::CharConvertUtf32::utf32ToUtf8(
                                                                   &utf8String,
                                                                   &utf32input,
                                                                   1);

                if (rc) {
                    return rc;                                        // RETURN
                }

                value->append(utf8String);

                iter += increment;
              } break;
              default: {
                return -1;                                            // RETURN
              } break;
            }
        }
        else if ('"' == *iter) {
            // Do not allow early '"' in strings.
            return -1;                                                // RETURN
        }
        else {
            *value += *iter;
        }
    }

    return 0;
}

int StringUtil::writeString(bsl::ostream&           stream,
                            const bsl::string_view& string)
{
    if (!bdlde::Utf8Util::isValid(string.data(),
                                  static_cast<int>(string.length()))) {
        return -2;                                                    // RETURN
    }

    stream.put('"');

    const char *       currentStart = string.data();
    const char * const end          = string.data() + string.length();

    for (const char *iter = string.data(); iter < end; ++iter) {
        switch (*iter) {
          case '"':  BSLS_ANNOTATION_FALLTHROUGH;
          case '\\': BSLS_ANNOTATION_FALLTHROUGH;
          case '/': {
            u::writeEscapedChar(stream, &currentStart, iter, *iter);
          } break;
          case '\b': {
            u::writeEscapedChar(stream, &currentStart, iter, 'b');
          } break;
          case '\f': {
            u::writeEscapedChar(stream, &currentStart, iter, 'f');
          } break;
          case '\n': {
            u::writeEscapedChar(stream, &currentStart, iter, 'n');
          } break;
          case '\r': {
            u::writeEscapedChar(stream, &currentStart, iter, 'r');
          } break;
          case '\t': {
            u::writeEscapedChar(stream, &currentStart, iter, 't');
          } break;

          // control characters

          case '\x00': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x01': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x02': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x03': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x04': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x05': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x06': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x07': BSLS_ANNOTATION_FALLTHROUGH;
          // Backspace       ('\b') handled above
//        case '\x08': BSLS_ANNOTATION_FALLTHROUGH;
          // Horizontal tab  ('\t') handled above
//        case '\x09': BSLS_ANNOTATION_FALLTHROUGH;
          // New line        ('\n') handled above
//        case '\x0A': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x0B': BSLS_ANNOTATION_FALLTHROUGH;
          // Form feed       ('\f') handled above
//        case '\x0C': BSLS_ANNOTATION_FALLTHROUGH;
          // Carriage return ('\r') handled above
//        case '\x0D': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x0E': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x0F': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x10': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x11': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x12': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x13': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x14': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x15': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x16': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x17': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x18': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x19': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x1A': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x1B': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x1C': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x1D': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x1E': BSLS_ANNOTATION_FALLTHROUGH;
          case '\x1F': {
            u::writeEscapedChar(stream, &currentStart, iter, 'u');

            const char *k_HEX_BYTES = "0123456789abcdef";

            const int k_BUF_SIZE = 4;
            char      buffer[k_BUF_SIZE] ;

            buffer[0] = '0';
            buffer[1] = '0';
            buffer[2] = k_HEX_BYTES[(*iter & 0xF0) >> 4];
            buffer[3] = k_HEX_BYTES[ *iter & 0x0F];

            stream.write(buffer, k_BUF_SIZE);
            currentStart = iter + 1;
          }
        }
    }

    stream.write(currentStart, end - currentStart);
    stream.put('"');

    return stream.good() ? 0 : -1;
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
