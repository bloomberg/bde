// balm_publicationtype.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

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
                           // --------------------------
                           // class PublicationType
                           // --------------------------

// CONSTANTS

// CLASS METHODS

int PublicationType::fromInt(PublicationType::Value *result,
                                  int                          number)
{
    switch (number) {
      case PublicationType::e_BALM_UNSPECIFIED:
      case PublicationType::e_BALM_TOTAL:
      case PublicationType::e_BALM_COUNT:
      case PublicationType::e_BALM_MIN:
      case PublicationType::e_BALM_MAX:
      case PublicationType::e_BALM_AVG:
      case PublicationType::e_BALM_RATE:
      case PublicationType::e_BALM_RATE_COUNT:
        *result = (PublicationType::Value)number;
        return 0;
      default:
        return -1;
    }
}

int PublicationType::fromString(PublicationType::Value *result,
                                     const char                  *string,
                                     int                          stringLength)
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
                            *result = PublicationType::e_BALM_AVG;
                            return 0;
                        }
                    } break;
                    case 'M': {
                        switch(string[6]) {
                            case 'A': {
                                if (string[7]=='X')
                                {
                                    *result = PublicationType::e_BALM_MAX;
                                    return 0;
                                }
                            } break;
                            case 'I': {
                                if (string[7]=='N')
                                {
                                    *result = PublicationType::e_BALM_MIN;
                                    return 0;
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
                *result = PublicationType::e_BALM_RATE;
                return 0;
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
                            *result = PublicationType::e_BALM_COUNT;
                            return 0;
                        }
                    } break;
                    case 'T': {
                        if (string[6]=='O'
                         && string[7]=='T'
                         && string[8]=='A'
                         && string[9]=='L')
                        {
                            *result = PublicationType::e_BALM_TOTAL;
                            return 0;
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
                *result = PublicationType::e_BALM_RATE_COUNT;
                return 0;
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
                *result = PublicationType::e_BALM_UNSPECIFIED;
                return 0;
            }
        } break;
    }

    return -1;
}

const char *PublicationType::toString(PublicationType::Value value)
{
    switch (value) {
      case e_BALM_UNSPECIFIED: {
        return "BALM_UNSPECIFIED";
      } break;
      case e_BALM_TOTAL: {
        return "BALM_TOTAL";
      } break;
      case e_BALM_COUNT: {
        return "BALM_COUNT";
      } break;
      case e_BALM_MIN: {
        return "BALM_MIN";
      } break;
      case e_BALM_MAX: {
        return "BALM_MAX";
      } break;
      case e_BALM_AVG: {
        return "BALM_AVG";
      } break;
      case e_BALM_RATE: {
        return "BALM_RATE";
      } break;
      case e_BALM_RATE_COUNT: {
        return "BALM_RATE_COUNT";
      } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}
}  // close package namespace

}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.3.x_DEV Tue Aug  4 13:07:24 2009
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
