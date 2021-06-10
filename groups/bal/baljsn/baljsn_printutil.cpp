// baljsn_printutil.cpp                                               -*-C++-*-
#include <baljsn_printutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_printutil_cpp,"$Id$ $CSID$")

#include <bdlde_base64encoder.h>
#include <bdlde_utf8util.h>

#include <bsls_annotation.h>

#include <bsl_sstream.h>

namespace BloombergLP {
namespace {

inline
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

}  // close unnamed namespace

namespace baljsn {

                              // ---------------
                              // class PrintUtil
                              // ---------------

int PrintUtil::printString(bsl::ostream&           stream,
                           const bsl::string_view& value)
{
    if (!bdlde::Utf8Util::isValid(value.data(),
                                  static_cast<int>(value.length()))) {
        return -1;                                                    // RETURN
    }

    stream.put('"');

    const char *currentStart = value.data();
    const char *iter      = value.data();
    const char *end       = value.data() + value.length();

    while (iter < end) {
        switch (*iter) {
          case '"':  BSLS_ANNOTATION_FALLTHROUGH;
          case '\\': BSLS_ANNOTATION_FALLTHROUGH;
          case '/': {
            writeEscapedChar(stream, &currentStart, iter, *iter);
          } break;
          case '\b': {
            writeEscapedChar(stream, &currentStart, iter, 'b');
          } break;
          case '\f': {
            writeEscapedChar(stream, &currentStart, iter, 'f');
          } break;
          case '\n': {
            writeEscapedChar(stream, &currentStart, iter, 'n');
          } break;
          case '\r': {
            writeEscapedChar(stream, &currentStart, iter, 'r');
          } break;
          case '\t': {
            writeEscapedChar(stream, &currentStart, iter, 't');
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
            writeEscapedChar(stream, &currentStart, iter, 'u');

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
        ++iter;
    }

    stream.write(currentStart, end - currentStart);
    stream.put('"');

    return 0;
}

int PrintUtil::printValue(bsl::ostream&         stream,
                          bdldfp::Decimal64     value,
                          const EncoderOptions *options)
{
    switch (bdldfp::DecimalUtil::classify(value)) {
      case FP_INFINITE: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream <<
                  (value == bsl::numeric_limits<bdldfp::Decimal64>::infinity()
                  ? "\"+inf\""
                  : "\"-inf\"");
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      case FP_NAN: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream << "\"nan\"";
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      default: {
        if (options && options->encodeQuotedDecimal64()) {
            stream.put('"');
            stream << value;
            stream.put('"');
        }
        else {
            stream << value;
        }
        if (stream.bad()) {
            return -1;                                                // RETURN
        }
      }
    }
    return 0;
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
