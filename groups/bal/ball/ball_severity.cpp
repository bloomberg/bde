// ball_severity.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

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
                        const char      *string,
                        int              stringLength)
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
    switch(value) {
      case   0: return "OFF";
      case  32: return "FATAL";
      case  64: return "ERROR";
      case  96: return "WARN";
      case 128: return "INFO";
      case 160: return "DEBUG";
      case 192: return "TRACE";
      default:  return "(* UNKNOWN *)";
    }
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
