// baem_publicationtype.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_publicationtype_cpp,"$Id$ $CSID$ $CCId$")

#include <baem_publicationtype.h>

#include <bdeat_valuetypefunctions.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>


#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                           // --------------------------
                           // class baem_PublicationType
                           // --------------------------

// CONSTANTS

// CLASS METHODS

int baem_PublicationType::fromInt(baem_PublicationType::Value *result,
                                  int                          number)
{
    switch (number) {
      case baem_PublicationType::BAEM_UNSPECIFIED:
      case baem_PublicationType::BAEM_TOTAL:
      case baem_PublicationType::BAEM_COUNT:
      case baem_PublicationType::BAEM_MIN:
      case baem_PublicationType::BAEM_MAX:
      case baem_PublicationType::BAEM_AVG:
      case baem_PublicationType::BAEM_RATE:
      case baem_PublicationType::BAEM_RATE_COUNT:
        *result = (baem_PublicationType::Value)number;
        return 0;
      default:
        return -1;
    }
}

int baem_PublicationType::fromString(baem_PublicationType::Value *result,
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
                            *result = baem_PublicationType::BAEM_AVG;
                            return 0;
                        }
                    } break;
                    case 'M': {
                        switch(string[6]) {
                            case 'A': {
                                if (string[7]=='X')
                                {
                                    *result = baem_PublicationType::BAEM_MAX;
                                    return 0;
                                }
                            } break;
                            case 'I': {
                                if (string[7]=='N')
                                {
                                    *result = baem_PublicationType::BAEM_MIN;
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
                *result = baem_PublicationType::BAEM_RATE;
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
                            *result = baem_PublicationType::BAEM_COUNT;
                            return 0;
                        }
                    } break;
                    case 'T': {
                        if (string[6]=='O'
                         && string[7]=='T'
                         && string[8]=='A'
                         && string[9]=='L')
                        {
                            *result = baem_PublicationType::BAEM_TOTAL;
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
                *result = baem_PublicationType::BAEM_RATE_COUNT;
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
                *result = baem_PublicationType::BAEM_UNSPECIFIED;
                return 0;
            }
        } break;
    }

    return -1;
}

const char *baem_PublicationType::toString(baem_PublicationType::Value value)
{
    switch (value) {
      case BAEM_UNSPECIFIED: {
        return "BAEM_UNSPECIFIED";
      } break;
      case BAEM_TOTAL: {
        return "BAEM_TOTAL";
      } break;
      case BAEM_COUNT: {
        return "BAEM_COUNT";
      } break;
      case BAEM_MIN: {
        return "BAEM_MIN";
      } break;
      case BAEM_MAX: {
        return "BAEM_MAX";
      } break;
      case BAEM_AVG: {
        return "BAEM_AVG";
      } break;
      case BAEM_RATE: {
        return "BAEM_RATE";
      } break;
      case BAEM_RATE_COUNT: {
        return "BAEM_RATE_COUNT";
      } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

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
