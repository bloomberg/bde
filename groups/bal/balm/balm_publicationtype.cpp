// balm_publicationtype.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_publicationtype_cpp,"$Id$ $CSID$ $CCId$")

#include <balm_publicationtype.h>

#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

namespace balm {
                           // ---------------------
                           // class PublicationType
                           // ---------------------

// CONSTANTS

// CLASS METHODS

int PublicationType::fromInt(PublicationType::Value *result,
                             int                     number)
{
    switch (number) {
      case PublicationType::e_UNSPECIFIED:
      case PublicationType::e_TOTAL:
      case PublicationType::e_COUNT:
      case PublicationType::e_MIN:
      case PublicationType::e_MAX:
      case PublicationType::e_AVG:
      case PublicationType::e_RATE:
      case PublicationType::e_RATE_COUNT:
        *result = (PublicationType::Value)number;
        return 0;                                                     // RETURN
      default:
        return -1;                                                    // RETURN
    }
    return -1;
}

int PublicationType::fromString(PublicationType::Value *result,
                                const char             *string,
                                int                     stringLength)
{

    switch(stringLength) {
        case 8: {
            if (string[0]=='B'
             && string[1]=='A'
             && string[2]=='E'
             && string[3]=='M'
             && string[4]=='_')
            {
                switch(string[5]) {
                    case 'A': {
                        if (string[6]=='V'
                         && string[7]=='G')
                        {
                            *result = PublicationType::e_AVG;
                            return 0;                                 // RETURN
                        }
                    } break;
                    case 'M': {
                        switch(string[6]) {
                            case 'A': {
                                if (string[7]=='X')
                                {
                                    *result = PublicationType::e_MAX;
                                    return 0;                         // RETURN
                                }
                            } break;
                            case 'I': {
                                if (string[7]=='N')
                                {
                                    *result = PublicationType::e_MIN;
                                    return 0;                         // RETURN
                                }
                            } break;
                        }
                    } break;
                }
            }
        } break;
        case 9: {
            if (string[0]=='B'
             && string[1]=='A'
             && string[2]=='E'
             && string[3]=='M'
             && string[4]=='_'
             && string[5]=='R'
             && string[6]=='A'
             && string[7]=='T'
             && string[8]=='E')
            {
                *result = PublicationType::e_RATE;
                return 0;                                             // RETURN
            }
        } break;
        case 10: {
            if (string[0]=='B'
             && string[1]=='A'
             && string[2]=='E'
             && string[3]=='M'
             && string[4]=='_')
            {
                switch(string[5]) {
                    case 'C': {
                        if (string[6]=='O'
                         && string[7]=='U'
                         && string[8]=='N'
                         && string[9]=='T')
                        {
                            *result = PublicationType::e_COUNT;
                            return 0;                                 // RETURN
                        }
                    } break;
                    case 'T': {
                        if (string[6]=='O'
                         && string[7]=='T'
                         && string[8]=='A'
                         && string[9]=='L')
                        {
                            *result = PublicationType::e_TOTAL;
                            return 0;                                 // RETURN
                        }
                    } break;
                }
            }
        } break;
        case 15: {
            if (string[0]=='B'
             && string[1]=='A'
             && string[2]=='E'
             && string[3]=='M'
             && string[4]=='_'
             && string[5]=='R'
             && string[6]=='A'
             && string[7]=='T'
             && string[8]=='E'
             && string[9]=='_'
             && string[10]=='C'
             && string[11]=='O'
             && string[12]=='U'
             && string[13]=='N'
             && string[14]=='T')
            {
                *result = PublicationType::e_RATE_COUNT;
                return 0;                                             // RETURN
            }
        } break;
        case 16: {
            if (string[0]=='B'
             && string[1]=='A'
             && string[2]=='E'
             && string[3]=='M'
             && string[4]=='_'
             && string[5]=='U'
             && string[6]=='N'
             && string[7]=='S'
             && string[8]=='P'
             && string[9]=='E'
             && string[10]=='C'
             && string[11]=='I'
             && string[12]=='F'
             && string[13]=='I'
             && string[14]=='E'
             && string[15]=='D')
            {
                *result = PublicationType::e_UNSPECIFIED;
                return 0;                                             // RETURN
            }
        } break;
    }

    return -1;
}

const char *PublicationType::toString(PublicationType::Value value)
{
    switch (value) {
      case e_UNSPECIFIED: {
        return "UNSPECIFIED";                                         // RETURN
      } break;
      case e_TOTAL: {
        return "TOTAL";                                               // RETURN
      } break;
      case e_COUNT: {
        return "COUNT";                                               // RETURN
      } break;
      case e_MIN: {
        return "MIN";                                                 // RETURN
      } break;
      case e_MAX: {
        return "MAX";                                                 // RETURN
      } break;
      case e_AVG: {
        return "AVG";                                                 // RETURN
      } break;
      case e_RATE: {
        return "RATE";                                                // RETURN
      } break;
      case e_RATE_COUNT: {
        return "RATE_COUNT";                                          // RETURN
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
