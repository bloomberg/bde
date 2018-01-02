// baljsn_printutil.cpp                                               -*-C++-*-
#include <baljsn_printutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_printutil_cpp,"$Id$ $CSID$")

#include <bdlde_base64encoder.h>
#include <bdlde_utf8util.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace {

inline
void writeEscapedChar(bsl::ostream&   stream,
                      const char    **currBegin,
                      const char     *iter,
                      const char      value)
    // Write the sequence of characters in the range [*currBegin, iter) to the
    // specified 'ostream' followed by the escape character ('\\') and the
    // specified 'value' character.  After that update '*currBegin' to point to
    // the address following 'iter'.
{
    stream.write(*currBegin, iter - *currBegin);
    stream.put('\\');
    stream.put(value);
    *currBegin = iter + 1;
}

}  // close unnamed namespace

namespace baljsn {

                              // ---------------
                              // class PrintUtil
                              // ---------------

int PrintUtil::printString(bsl::ostream&            stream,
                           const bslstl::StringRef& value)
{
    if (!bdlde::Utf8Util::isValid(value.data(),
                                  static_cast<int>(value.length()))) {
        return -1;                                                    // RETURN
    }

    stream.put('"');

    const char *currBegin = value.data();
    const char *iter      = value.data();
    const char *end       = value.data() + value.length();

    while (iter < end) {
        switch (*iter) {
          case '"':                                             // FALL THROUGH
          case '\\':                                            // FALL THROUGH
          case '/': {
            writeEscapedChar(stream, &currBegin, iter, *iter);
          } break;
          case '\b': {
            writeEscapedChar(stream, &currBegin, iter, 'b');
          } break;
          case '\f': {
            writeEscapedChar(stream, &currBegin, iter, 'f');
          } break;
          case '\n': {
            writeEscapedChar(stream, &currBegin, iter, 'n');
          } break;
          case '\r': {
            writeEscapedChar(stream, &currBegin, iter, 'r');
          } break;
          case '\t': {
            writeEscapedChar(stream, &currBegin, iter, 't');
          } break;

          // control characters

          case '\x00':                                          // FALL THROUGH
          case '\x01':                                          // FALL THROUGH
          case '\x02':                                          // FALL THROUGH
          case '\x03':                                          // FALL THROUGH
          case '\x04':                                          // FALL THROUGH
          case '\x05':                                          // FALL THROUGH
          case '\x06':                                          // FALL THROUGH
          case '\x07':                                          // FALL THROUGH
          // Backspace       ('\b') handled above
//        case '\x08':                                          // FALL THROUGH
          // Horizontal tab  ('\t') handled above
//        case '\x09':                                          // FALL THROUGH
          // New line        ('\n') handled above
//        case '\x0A':                                          // FALL THROUGH
          case '\x0B':                                          // FALL THROUGH
          // Form feed       ('\f') handled above
//        case '\x0C':                                          // FALL THROUGH
          // Carriage return ('\r') handled above
//        case '\x0D':                                          // FALL THROUGH
          case '\x0E':                                          // FALL THROUGH
          case '\x0F':                                          // FALL THROUGH
          case '\x10':                                          // FALL THROUGH
          case '\x11':                                          // FALL THROUGH
          case '\x12':                                          // FALL THROUGH
          case '\x13':                                          // FALL THROUGH
          case '\x14':                                          // FALL THROUGH
          case '\x15':                                          // FALL THROUGH
          case '\x16':                                          // FALL THROUGH
          case '\x17':                                          // FALL THROUGH
          case '\x18':                                          // FALL THROUGH
          case '\x19':                                          // FALL THROUGH
          case '\x1A':                                          // FALL THROUGH
          case '\x1B':                                          // FALL THROUGH
          case '\x1C':                                          // FALL THROUGH
          case '\x1D':                                          // FALL THROUGH
          case '\x1E':                                          // FALL THROUGH
          case '\x1F': {
            writeEscapedChar(stream, &currBegin, iter, 'u');

            const char *HEX_BYTES = "0123456789abcdef";

            const int BUF_SIZE = 4;
            char      buffer[BUF_SIZE] ;

            buffer[0] = '0';
            buffer[1] = '0';
            buffer[2] = HEX_BYTES[(*iter & 0xF0) >> 4];
            buffer[3] = HEX_BYTES[ *iter & 0x0F];

            stream.write(buffer, BUF_SIZE);
            currBegin = iter + 1;
          }
        }
        ++iter;

    }

    stream.write(currBegin, end - currBegin);
    stream.put('"');

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
