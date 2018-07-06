// bdlb_literalutil.cpp                                               -*-C++-*-
#include <bdlb_literalutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_literalutil_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlb {

                            // ----------------
                            // class LiteralUtil
                            // ----------------

// CLASS METHODS
void LiteralUtil::createQuotedEscapedCString(bsl::string              *result,
                                             const bslstl::StringRef&  input)
{
    result->clear();
    result->reserve(input.length() + 2);
    *result += '"';

    typedef bslstl::StringRef::const_iterator const_iter_t;

    const const_iter_t end = input.end();

    for (const_iter_t i = input.begin(); i < end; ++i) {
        const char ch = *i;

        switch (ch) {
          case '0': case '1': case '2': case '3': case '4':// NUMBERS
          case '5': case '6': case '7': case '8': case '9':

          case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
          case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
          case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
          case 'V': case 'W': case 'X': case 'Y': case 'Z':
          case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
          case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
          case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
          case 'v': case 'w': case 'x': case 'y': case 'z':

          case ' ': case '!': case '#': case '$': case '%': case '&': case '(':
          case ')': case '*': case '+': case ',': case '-': case '.': case '/':
          case ':': case ';': case '<': case '=': case '>': case '?': case '@':
          case '[': case ']': case '^': case '_': case '`': case '{': case '|':
          case '}': case '~': case '\'':
            *result += ch;
          break;

          case '\a':  *result += "\\a";  break;
          case '\b':  *result += "\\b";  break;
          case '\n':  *result += "\\n";  break;
          case '\f':  *result += "\\f";  break;
          case '\r':  *result += "\\r";  break;
          case '\t':  *result += "\\t";  break;
          case '\v':  *result += "\\v";  break;
          case '"': *result += "\\\"";   break;
          case '\\':  *result += "\\\\"; break;

          default:
            unsigned char uch = ch;  // Singed char makes conversion harder

                                // 0_123_
            char octalBuffer[5] = "\\___";
            octalBuffer[1] = static_cast<char>('0' + (uch / 64));
            octalBuffer[2] = static_cast<char>('0' + ((uch & 070) / 8));
            octalBuffer[3] = static_cast<char>('0' + (uch & 07));

            *result += octalBuffer;
            break;
        }
    }

    *result += '"';
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
