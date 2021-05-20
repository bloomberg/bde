// balxml_encodingstyle.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_encodingstyle_cpp,"$Id$ $CSID$ $CCId$")

#include <balxml_encodingstyle.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_chartype.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // ---------------------------
                        // class balxml::EncodingStyle
                        // ---------------------------

// CONSTANTS

const char balxml::EncodingStyle::CLASS_NAME[] = "balxml::EncodingStyle";

const bdlat_EnumeratorInfo balxml::EncodingStyle::ENUMERATOR_INFO_ARRAY[] = {
    {
        balxml::EncodingStyle::e_COMPACT,
        "COMPACT",
        sizeof("COMPACT") - 1,
        ""
    },
    {
        balxml::EncodingStyle::e_PRETTY,
        "PRETTY",
        sizeof("PRETTY") - 1,
        ""
    }
};

namespace balxml {
// CLASS METHODS

int EncodingStyle::fromInt(EncodingStyle::Value *result, int number)
{
    switch (number) {
      case EncodingStyle::e_COMPACT:
      case EncodingStyle::e_PRETTY:
        *result = (EncodingStyle::Value)number;
        return 0;                                                     // RETURN
      default:
        return -1;                                                    // RETURN
    }
}

int EncodingStyle::fromString(EncodingStyle::Value *result,
                            const char         *string,
                            int                 stringLength)
{

    switch(stringLength) {
        case 6: {
            if ((string[0]|0x20)=='p'
             && (string[1]|0x20)=='r'
             && (string[2]|0x20)=='e'
             && (string[3]|0x20)=='t'
             && (string[4]|0x20)=='t'
             && (string[5]|0x20)=='y')
            {
                *result = EncodingStyle::e_PRETTY;
                return 0;                                             // RETURN
            }
        } break;
        case 7: {
            if ((string[0]|0x20)=='c'
             && (string[1]|0x20)=='o'
             && (string[2]|0x20)=='m'
             && (string[3]|0x20)=='p'
             && (string[4]|0x20)=='a'
             && (string[5]|0x20)=='c'
             && (string[6]|0x20)=='t')
            {
                *result = EncodingStyle::e_COMPACT;
                return 0;                                             // RETURN
            }
        } break;
        case 13: {
            if ((string[0]|0x20)=='b'
             && (string[1]|0x20)=='a'
             && (string[2]|0x20)=='e'
             && (string[3]|0x20)=='x'
             && (string[4]|0x20)=='m'
             && (string[5]|0x20)=='l'
             &&  string[6]      =='_'
             && (string[7]|0x20)=='p'
             && (string[8]|0x20)=='r'
             && (string[9]|0x20)=='e'
             && (string[10]|0x20)=='t'
             && (string[11]|0x20)=='t'
             && (string[12]|0x20)=='y')
            {
                *result = EncodingStyle::e_PRETTY;
                return 0;                                             // RETURN
            }
        } break;
        case 14: {
            if ((string[0]|0x20)=='b'
             && (string[1]|0x20)=='a'
             && (string[2]|0x20)=='e'
             && (string[3]|0x20)=='x'
             && (string[4]|0x20)=='m'
             && (string[5]|0x20)=='l'
             &&  string[6]      =='_'
             && (string[7]|0x20)=='c'
             && (string[8]|0x20)=='o'
             && (string[9]|0x20)=='m'
             && (string[10]|0x20)=='p'
             && (string[11]|0x20)=='a'
             && (string[12]|0x20)=='c'
             && (string[13]|0x20)=='t')
            {
                *result = EncodingStyle::e_COMPACT;
                return 0;                                             // RETURN
            }
        } break;
    }

    return -1;
}

const char *EncodingStyle::toString(EncodingStyle::Value value)
{
    switch (value) {
      case e_COMPACT: {
        return "COMPACT";                                             // RETURN
      } break;
      case e_PRETTY: {
        return "PRETTY";                                              // RETURN
      } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
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
