// baenet_httprequestmethod.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httprequestmethod_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httprequestmethod.h>

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

                     // ----------------------------------                     
                     // class baenet_HttpRequestMethod                     
                     // ----------------------------------                     

// CONSTANTS

const char baenet_HttpRequestMethod::CLASS_NAME[] = "baenet_HttpRequestMethod";

const bdeat_EnumeratorInfo
                          baenet_HttpRequestMethod::ENUMERATOR_INFO_ARRAY[] = {
    {
        baenet_HttpRequestMethod::BAENET_CONNECT,
        "CONNECT",
        sizeof("CONNECT") - 1,
        ""
    },
    {
        baenet_HttpRequestMethod::BAENET_DELETE,
        "DELETE",
        sizeof("DELETE") - 1,
        ""
    },
    {
        baenet_HttpRequestMethod::BAENET_GET,
        "GET",
        sizeof("GET") - 1,
        ""
    },
    {
        baenet_HttpRequestMethod::BAENET_HEAD,
        "HEAD",
        sizeof("HEAD") - 1,
        ""
    },
    {
        baenet_HttpRequestMethod::BAENET_OPTIONS,
        "OPTIONS",
        sizeof("OPTIONS") - 1,
        ""
    },
    {
        baenet_HttpRequestMethod::BAENET_POST,
        "POST",
        sizeof("POST") - 1,
        ""
    },
    {
        baenet_HttpRequestMethod::BAENET_PUT,
        "PUT",
        sizeof("PUT") - 1,
        ""
    },
    {
        baenet_HttpRequestMethod::BAENET_TRACE,
        "TRACE",
        sizeof("TRACE") - 1,
        ""
    }
};

// CLASS METHODS

int baenet_HttpRequestMethod::fromInt(baenet_HttpRequestMethod::Value *result, int number)
{
    switch (number) {
      case baenet_HttpRequestMethod::BAENET_CONNECT:
      case baenet_HttpRequestMethod::BAENET_DELETE:
      case baenet_HttpRequestMethod::BAENET_GET:
      case baenet_HttpRequestMethod::BAENET_HEAD:
      case baenet_HttpRequestMethod::BAENET_OPTIONS:
      case baenet_HttpRequestMethod::BAENET_POST:
      case baenet_HttpRequestMethod::BAENET_PUT:
      case baenet_HttpRequestMethod::BAENET_TRACE:
        *result = (baenet_HttpRequestMethod::Value)number;
        return 0;
      default:
        return -1;
    }
}

int baenet_HttpRequestMethod::fromString(baenet_HttpRequestMethod::Value *result,
                            const char         *string,
                            int                 stringLength)
{
    
    switch(stringLength) {
        case 3: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'G': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='t')
                    {
                        *result = baenet_HttpRequestMethod::BAENET_GET;
                        return 0;
                    }
                } break;
                case 'P': {
                    if ((string[1]|0x20)=='u'
                     && (string[2]|0x20)=='t')
                    {
                        *result = baenet_HttpRequestMethod::BAENET_PUT;
                        return 0;
                    }
                } break;
            }
        } break;
        case 4: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'H': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='a'
                     && (string[3]|0x20)=='d')
                    {
                        *result = baenet_HttpRequestMethod::BAENET_HEAD;
                        return 0;
                    }
                } break;
                case 'P': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='s'
                     && (string[3]|0x20)=='t')
                    {
                        *result = baenet_HttpRequestMethod::BAENET_POST;
                        return 0;
                    }
                } break;
            }
        } break;
        case 5: {
            if ((string[0]|0x20)=='t'
             && (string[1]|0x20)=='r'
             && (string[2]|0x20)=='a'
             && (string[3]|0x20)=='c'
             && (string[4]|0x20)=='e')
            {
                *result = baenet_HttpRequestMethod::BAENET_TRACE;
                return 0;
            }
        } break;
        case 6: {
            if ((string[0]|0x20)=='d'
             && (string[1]|0x20)=='e'
             && (string[2]|0x20)=='l'
             && (string[3]|0x20)=='e'
             && (string[4]|0x20)=='t'
             && (string[5]|0x20)=='e')
            {
                *result = baenet_HttpRequestMethod::BAENET_DELETE;
                return 0;
            }
        } break;
        case 7: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'C': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='n'
                     && (string[3]|0x20)=='n'
                     && (string[4]|0x20)=='e'
                     && (string[5]|0x20)=='c'
                     && (string[6]|0x20)=='t')
                    {
                        *result = baenet_HttpRequestMethod::BAENET_CONNECT;
                        return 0;
                    }
                } break;
                case 'O': {
                    if ((string[1]|0x20)=='p'
                     && (string[2]|0x20)=='t'
                     && (string[3]|0x20)=='i'
                     && (string[4]|0x20)=='o'
                     && (string[5]|0x20)=='n'
                     && (string[6]|0x20)=='s')
                    {
                        *result = baenet_HttpRequestMethod::BAENET_OPTIONS;
                        return 0;
                    }
                } break;
            }
        } break;
    }

    return -1;
}

const char *baenet_HttpRequestMethod::toString(baenet_HttpRequestMethod::Value value)
{
    switch (value) {
      case BAENET_CONNECT: {
        return "CONNECT";
      } break;
      case BAENET_DELETE: {
        return "DELETE";
      } break;
      case BAENET_GET: {
        return "GET";
      } break;
      case BAENET_HEAD: {
        return "HEAD";
      } break;
      case BAENET_OPTIONS: {
        return "OPTIONS";
      } break;
      case BAENET_POST: {
        return "POST";
      } break;
      case BAENET_PUT: {
        return "PUT";
      } break;
      case BAENET_TRACE: {
        return "TRACE";
      } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Fri Feb 12 15:50:30 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
