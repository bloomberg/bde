// ball_severity.cpp            -*-C++-*-
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
            *level = Severity::BAEL_OFF;
            return 0;
        }
      } break;
      case 4: {
        switch(bdlb::CharType::toUpper(string[0])) {
          case 'I': {
            if ('n' == (string[1] | 0x20)
             && 'f' == (string[2] | 0x20)
             && 'o' == (string[3] | 0x20)) {
                *level = Severity::BAEL_INFO;
                return 0;
            }
          } break;
          case 'N': {
            if ('o' == (string[1] | 0x20)
             && 'n' == (string[2] | 0x20)
             && 'e' == (string[3] | 0x20)) {
                *level = Severity::BAEL_NONE;
                return 0;
            }
          } break;
          case 'W': {
            if ('a' == (string[1] | 0x20)
             && 'r' == (string[2] | 0x20)
             && 'n' == (string[3] | 0x20)) {
                *level = Severity::BAEL_WARN;
                return 0;
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
                    *level = Severity::BAEL_DEBUG;
                    return 0;
                }
              } break;
              case 'E': {
                if ('r' == (string[1] | 0x20)
                 && 'r' == (string[2] | 0x20)
                 && 'o' == (string[3] | 0x20)
                 && 'r' == (string[4] | 0x20)) {
                    *level = Severity::BAEL_ERROR;
                    return 0;
                }
              } break;
              case 'F': {
                if ('a' == (string[1] | 0x20)
                 && 't' == (string[2] | 0x20)
                 && 'a' == (string[3] | 0x20)
                 && 'l' == (string[4] | 0x20)) {
                    *level = Severity::BAEL_FATAL;
                    return 0;
                }
              } break;
              case 'T': {
                if ('r' == (string[1] | 0x20)
                 && 'a' == (string[2] | 0x20)
                 && 'c' == (string[3] | 0x20)
                 && 'e' == (string[4] | 0x20)) {
                    *level = Severity::BAEL_TRACE;
                    return 0;
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
