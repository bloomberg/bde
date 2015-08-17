// ball_severity.cpp                                                  -*-C++-*-
#include <ball_severity.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_severity_cpp,"$Id$ $CSID$")

#include <bdlb_chartype.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
// PRIVATE CLASS METHODS

void Severity::print(bsl::ostream& stream, Severity::Level value)
{
    stream << Severity::toAscii(value);
}

// CLASS METHODS
int Severity::fromAscii(Severity::Level *level,
                             const char           *string,
                             int                   stringLength)
{
    switch(stringLength) {
      case 3: {
        if ('o' == (string[0] | 0x20)
         && 'f' == (string[1] | 0x20)
         && 'f' == (string[2] | 0x20)) {
            *level = Severity::e_OFF;
            return 0;                                                 // RETURN
        }
      } break;
      case 4: {
        switch(bdlb::CharType::toUpper(string[0])) {
          case 'I': {
            if ('n' == (string[1] | 0x20)
             && 'f' == (string[2] | 0x20)
             && 'o' == (string[3] | 0x20)) {
                *level = Severity::e_INFO;
                return 0;                                             // RETURN
            }
          } break;
          case 'N': {
            if ('o' == (string[1] | 0x20)
             && 'n' == (string[2] | 0x20)
             && 'e' == (string[3] | 0x20)) {
                *level = Severity::e_NONE;
                return 0;                                             // RETURN
            }
          } break;
          case 'W': {
            if ('a' == (string[1] | 0x20)
             && 'r' == (string[2] | 0x20)
             && 'n' == (string[3] | 0x20)) {
                *level = Severity::e_WARN;
                return 0;                                             // RETURN
            }
          } break;
        }
      } break;
      case 5: {
            switch(bdlb::CharType::toUpper(string[0])) {
              case 'D': {
                if ('e' == (string[1] | 0x20)
                 && 'b' == (string[2] | 0x20)
                 && 'u' == (string[3] | 0x20)
                 && 'g' == (string[4] | 0x20)) {
                    *level = Severity::e_DEBUG;
                    return 0;                                         // RETURN
                }
              } break;
              case 'E': {
                if ('r' == (string[1] | 0x20)
                 && 'r' == (string[2] | 0x20)
                 && 'o' == (string[3] | 0x20)
                 && 'r' == (string[4] | 0x20)) {
                    *level = Severity::e_ERROR;
                    return 0;                                         // RETURN
                }
              } break;
              case 'F': {
                if ('a' == (string[1] | 0x20)
                 && 't' == (string[2] | 0x20)
                 && 'a' == (string[3] | 0x20)
                 && 'l' == (string[4] | 0x20)) {
                    *level = Severity::e_FATAL;
                    return 0;                                         // RETURN
                }
              } break;
              case 'T': {
                if ('r' == (string[1] | 0x20)
                 && 'a' == (string[2] | 0x20)
                 && 'c' == (string[3] | 0x20)
                 && 'e' == (string[4] | 0x20)) {
                    *level = Severity::e_TRACE;
                    return 0;                                         // RETURN
                }
              } break;
            }
      } break;
    }

    return -1;
}

const char *Severity::toAscii(Severity::Level value)
{
#define CASE(X) return #X;

    switch(value) {
      case   0: CASE(OFF)
      case  32: CASE(FATAL)
      case  64: CASE(ERROR)
      case  96: CASE(WARN)
      case 128: CASE(INFO)
      case 160: CASE(DEBUG)
      case 192: CASE(TRACE)
      case 224: CASE(NONE)
      default:  return "(* UNKNOWN *)";
    }

#undef CASE
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
