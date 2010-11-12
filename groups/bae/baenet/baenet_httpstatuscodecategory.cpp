// baenet_httpstatuscodecategory.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpstatuscodecategory_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httpstatuscodecategory.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_chartype.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>


#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                  // ---------------------------------------                   
                  // class baenet_HttpStatusCodeCategory                   
                  // ---------------------------------------                   

// CONSTANTS

const char baenet_HttpStatusCodeCategory::CLASS_NAME[]
                                        = "baenet_HttpStatusCodeCategory";

const bdeat_EnumeratorInfo
                     baenet_HttpStatusCodeCategory::ENUMERATOR_INFO_ARRAY[] = {
    {
        baenet_HttpStatusCodeCategory::BAENET_INFORMATIONAL,
        "Informational",
        sizeof("Informational") - 1,
        ""
    },
    {
        baenet_HttpStatusCodeCategory::BAENET_SUCCESS,
        "Success",
        sizeof("Success") - 1,
        ""
    },
    {
        baenet_HttpStatusCodeCategory::BAENET_REDIRECTION,
        "Redirection",
        sizeof("Redirection") - 1,
        ""
    },
    {
        baenet_HttpStatusCodeCategory::BAENET_CLIENT_ERROR,
        "Client Error",
        sizeof("Client Error") - 1,
        ""
    },
    {
        baenet_HttpStatusCodeCategory::BAENET_SERVER_ERROR,
        "Server Error",
        sizeof("Server Error") - 1,
        ""
    },
    {
        baenet_HttpStatusCodeCategory::BAENET_NONSTANDARD,
        "Nonstandard",
        sizeof("Nonstandard") - 1,
        ""
    }
};

// CLASS METHODS

int baenet_HttpStatusCodeCategory::fromInt(baenet_HttpStatusCodeCategory::Value *result, int number)
{
    switch (number) {
      case baenet_HttpStatusCodeCategory::BAENET_INFORMATIONAL:
      case baenet_HttpStatusCodeCategory::BAENET_SUCCESS:
      case baenet_HttpStatusCodeCategory::BAENET_REDIRECTION:
      case baenet_HttpStatusCodeCategory::BAENET_CLIENT_ERROR:
      case baenet_HttpStatusCodeCategory::BAENET_SERVER_ERROR:
      case baenet_HttpStatusCodeCategory::BAENET_NONSTANDARD:
        *result = (baenet_HttpStatusCodeCategory::Value)number;
        return 0;
      default:
        return -1;
    }
}

int baenet_HttpStatusCodeCategory::fromString(
                            baenet_HttpStatusCodeCategory::Value *result,
                            const char                           *string,
                            int                                   stringLength)
{
    switch(stringLength) {
        case 7: {
            if ((string[0]|0x20)=='s'
             && (string[1]|0x20)=='u'
             && (string[2]|0x20)=='c'
             && (string[3]|0x20)=='c'
             && (string[4]|0x20)=='e'
             && (string[5]|0x20)=='s'
             && (string[6]|0x20)=='s')
            {
                *result = baenet_HttpStatusCodeCategory::BAENET_SUCCESS;
                return 0;
            }
        } break;
        case 11: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'N': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='n'
                     && (string[3]|0x20)=='s'
                     && (string[4]|0x20)=='t'
                     && (string[5]|0x20)=='a'
                     && (string[6]|0x20)=='n'
                     && (string[7]|0x20)=='d'
                     && (string[8]|0x20)=='a'
                     && (string[9]|0x20)=='r'
                     && (string[10]|0x20)=='d')
                    {
                        *result = baenet_HttpStatusCodeCategory::BAENET_NONSTANDARD;
                        return 0;
                    }
                } break;
                case 'R': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='d'
                     && (string[3]|0x20)=='i'
                     && (string[4]|0x20)=='r'
                     && (string[5]|0x20)=='e'
                     && (string[6]|0x20)=='c'
                     && (string[7]|0x20)=='t'
                     && (string[8]|0x20)=='i'
                     && (string[9]|0x20)=='o'
                     && (string[10]|0x20)=='n')
                    {
                        *result =
                             baenet_HttpStatusCodeCategory::BAENET_REDIRECTION;
                        return 0;
                    }
                } break;
            }
        } break;
        case 12: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'C': {
                    if ((string[1]|0x20)=='l'
                     && (string[2]|0x20)=='i'
                     && (string[3]|0x20)=='e'
                     && (string[4]|0x20)=='n'
                     && (string[5]|0x20)=='t'
                     && (string[6])==' '
                     && (string[7]|0x20)=='e'
                     && (string[8]|0x20)=='r'
                     && (string[9]|0x20)=='r'
                     && (string[10]|0x20)=='o'
                     && (string[11]|0x20)=='r')
                    {
                        *result =
                            baenet_HttpStatusCodeCategory::BAENET_CLIENT_ERROR;
                        return 0;
                    }
                } break;
                case 'S': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='r'
                     && (string[3]|0x20)=='v'
                     && (string[4]|0x20)=='e'
                     && (string[5]|0x20)=='r'
                     && (string[6])==' '
                     && (string[7]|0x20)=='e'
                     && (string[8]|0x20)=='r'
                     && (string[9]|0x20)=='r'
                     && (string[10]|0x20)=='o'
                     && (string[11]|0x20)=='r')
                    {
                        *result =
                            baenet_HttpStatusCodeCategory::BAENET_SERVER_ERROR;
                        return 0;
                    }
                } break;
            }
        } break;
        case 13: {
            if ((string[0]|0x20)=='i'
             && (string[1]|0x20)=='n'
             && (string[2]|0x20)=='f'
             && (string[3]|0x20)=='o'
             && (string[4]|0x20)=='r'
             && (string[5]|0x20)=='m'
             && (string[6]|0x20)=='a'
             && (string[7]|0x20)=='t'
             && (string[8]|0x20)=='i'
             && (string[9]|0x20)=='o'
             && (string[10]|0x20)=='n'
             && (string[11]|0x20)=='a'
             && (string[12]|0x20)=='l')
            {
                *result = baenet_HttpStatusCodeCategory::BAENET_INFORMATIONAL;
                return 0;
            }
        } break;
    }
    
    return -1;
}

const char *baenet_HttpStatusCodeCategory::toString(baenet_HttpStatusCodeCategory::Value value)
{
    switch (value) {
      case BAENET_INFORMATIONAL: {
        return "Informational";
      } break;
      case BAENET_SUCCESS: {
        return "Success";
      } break;
      case BAENET_REDIRECTION: {
        return "Redirection";
      } break;
      case BAENET_CLIENT_ERROR: {
        return "Client Error";
      } break;
      case BAENET_SERVER_ERROR: {
        return "Server Error";
      } break;
      case BAENET_NONSTANDARD: {
        return "Nonstandard";
      } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Fri Feb 12 16:23:58 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
