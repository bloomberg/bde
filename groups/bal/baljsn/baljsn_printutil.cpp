// baljsn_printutil.cpp                                               -*-C++-*-
#include <baljsn_printutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_printutil_cpp,"$Id$ $CSID$")

#include <bdlde_base64encoder.h>
#include <bdlde_utf8util.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace {

char getEscapeChar(char value)
    // Return the JSON escape character for the specified 'value' if 'value'
    // needs to be escaped, and '\0' otherwise.
{
    switch (value) {
      case '"':                                                 // FALL THROUGH
      case '\\':                                                // FALL THROUGH
      case '/': {
        return value;                                                 // RETURN
      }
      case '\b': {
        return 'b';                                                   // RETURN
      }
      case '\f': {
        return 'f';                                                   // RETURN
      }
      case '\n': {
        return 'n';                                                   // RETURN
      }
      case '\r': {
        return 'r';                                                   // RETURN
      }
      case '\t': {
        return 't';                                                   // RETURN
      }
      default: {
        if (0 <= value && value < 32) {
            // control characters

            return 'u';                                               // RETURN
        }
      }
    }
    return 0;
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

    stream << '"';

    for (bslstl::StringRef::const_iterator it = value.begin();
         it != value.end();
         ++it) {
        bsl::string str;

        char ch = getEscapeChar(*it);
        if (0 != ch) {
            str += '\\';
            str += ch;
            if ('u' == ch) {
                bsl::ostringstream oss;

                oss << "00"
                    << bsl::hex << bsl::setfill('0') << bsl::setw(2)
                    << (static_cast<unsigned int>(*it) & 0xff);
                str += oss.str();
            }
        }
        else {
            str += *it;
        }

        stream << str;
    }

    stream << '"';
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
