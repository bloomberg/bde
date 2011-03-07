// baenet_httpstatuscode.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpstatuscode_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httpstatuscode.h>

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

                      // -------------------------------
                      // class baenet_HttpStatusCode
                      // -------------------------------

// CONSTANTS

const char baenet_HttpStatusCode::CLASS_NAME[] = "baenet_HttpStatusCode";

const bdeat_EnumeratorInfo baenet_HttpStatusCode::ENUMERATOR_INFO_ARRAY[] = {
    {
        baenet_HttpStatusCode::BAENET_CONTINUE,
        "Continue",
        sizeof("Continue") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_SWITCHING_PROTOCOLS,
        "Switching Protocols",
        sizeof("Switching Protocols") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_OK,
        "OK",
        sizeof("OK") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_CREATED,
        "Created",
        sizeof("Created") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_ACCEPTED,
        "Accepted",
        sizeof("Accepted") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_NON_AUTHORITATIVE_INFORMATION,
        "Non-Authoritative Information",
        sizeof("Non-Authoritative Information") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_NO_CONTENT,
        "No Content",
        sizeof("No Content") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_RESET_CONTENT,
        "Reset Content",
        sizeof("Reset Content") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_PARTIAL_CONTENT,
        "Partial Content",
        sizeof("Partial Content") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_MULTIPLE_CHOICES,
        "Multiple Choices",
        sizeof("Multiple Choices") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_MOVED_PERMANENTLY,
        "Moved Permanently",
        sizeof("Moved Permanently") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_FOUND,
        "Found",
        sizeof("Found") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_SEE_OTHER,
        "See Other",
        sizeof("See Other") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_NOT_MODIFIED,
        "Not Modified",
        sizeof("Not Modified") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_USE_PROXY,
        "Use Proxy",
        sizeof("Use Proxy") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_TEMPORARY_REDIRECT,
        "Temporary Redirect",
        sizeof("Temporary Redirect") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_BAD_REQUEST,
        "Bad Request",
        sizeof("Bad Request") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_UNAUTHORIZED,
        "Unauthorized",
        sizeof("Unauthorized") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_PAYMENT_REQUIRED,
        "Payment Required",
        sizeof("Payment Required") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_FORBIDDEN,
        "Forbidden",
        sizeof("Forbidden") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_NOT_FOUND,
        "Not Found",
        sizeof("Not Found") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_METHOD_NOT_ALLOWED,
        "Method Not Allowed",
        sizeof("Method Not Allowed") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_NOT_ACCEPTABLE,
        "Not Acceptable",
        sizeof("Not Acceptable") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_PROXY_AUTHENTICATION_REQUIRED,
        "Proxy Authentication Required",
        sizeof("Proxy Authentication Required") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_REQUEST_TIME_OUT,
        "Request Time-out",
        sizeof("Request Time-out") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_CONFLICT,
        "Conflict",
        sizeof("Conflict") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_GONE,
        "Gone",
        sizeof("Gone") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_LENGTH_REQUIRED,
        "Length Required",
        sizeof("Length Required") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_PRECONDITION_FAILED,
        "Precondition Failed",
        sizeof("Precondition Failed") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_REQUEST_ENTITY_TOO_LARGE,
        "Request Entity Too Large",
        sizeof("Request Entity Too Large") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_REQUEST_URI_TOO_LARGE,
        "Request-URI Too Large",
        sizeof("Request-URI Too Large") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_UNSUPPORTED_MEDIA_TYPE,
        "Unsupported Media Type",
        sizeof("Unsupported Media Type") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_REQUESTED_RANGE_NOT_SATISFIABLE,
        "Requested range not satisfiable",
        sizeof("Requested range not satisfiable") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_EXPECTATION_FAILED,
        "Expectation Failed",
        sizeof("Expectation Failed") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_INTERNAL_SERVER_ERROR,
        "Internal Server Error",
        sizeof("Internal Server Error") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_NOT_IMPLEMENTED,
        "Not Implemented",
        sizeof("Not Implemented") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_BAD_GATEWAY,
        "Bad Gateway",
        sizeof("Bad Gateway") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_SERVICE_UNAVAILABLE,
        "Service Unavailable",
        sizeof("Service Unavailable") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_GATEWAY_TIME_OUT,
        "Gateway Time-out",
        sizeof("Gateway Time-out") - 1,
        ""
    },
    {
        baenet_HttpStatusCode::BAENET_HTTP_VERSION_NOT_SUPPORTED,
        "HTTP Version not supported",
        sizeof("HTTP Version not supported") - 1,
        ""
    }
};

// CLASS METHODS

int baenet_HttpStatusCode::fromInt(baenet_HttpStatusCode::Value *result,
                                   int                           number)
{
    switch (number) {
      case baenet_HttpStatusCode::BAENET_CONTINUE:
      case baenet_HttpStatusCode::BAENET_SWITCHING_PROTOCOLS:
      case baenet_HttpStatusCode::BAENET_OK:
      case baenet_HttpStatusCode::BAENET_CREATED:
      case baenet_HttpStatusCode::BAENET_ACCEPTED:
      case baenet_HttpStatusCode::BAENET_NON_AUTHORITATIVE_INFORMATION:
      case baenet_HttpStatusCode::BAENET_NO_CONTENT:
      case baenet_HttpStatusCode::BAENET_RESET_CONTENT:
      case baenet_HttpStatusCode::BAENET_PARTIAL_CONTENT:
      case baenet_HttpStatusCode::BAENET_MULTIPLE_CHOICES:
      case baenet_HttpStatusCode::BAENET_MOVED_PERMANENTLY:
      case baenet_HttpStatusCode::BAENET_FOUND:
      case baenet_HttpStatusCode::BAENET_SEE_OTHER:
      case baenet_HttpStatusCode::BAENET_NOT_MODIFIED:
      case baenet_HttpStatusCode::BAENET_USE_PROXY:
      case baenet_HttpStatusCode::BAENET_TEMPORARY_REDIRECT:
      case baenet_HttpStatusCode::BAENET_BAD_REQUEST:
      case baenet_HttpStatusCode::BAENET_UNAUTHORIZED:
      case baenet_HttpStatusCode::BAENET_PAYMENT_REQUIRED:
      case baenet_HttpStatusCode::BAENET_FORBIDDEN:
      case baenet_HttpStatusCode::BAENET_NOT_FOUND:
      case baenet_HttpStatusCode::BAENET_METHOD_NOT_ALLOWED:
      case baenet_HttpStatusCode::BAENET_NOT_ACCEPTABLE:
      case baenet_HttpStatusCode::BAENET_PROXY_AUTHENTICATION_REQUIRED:
      case baenet_HttpStatusCode::BAENET_REQUEST_TIME_OUT:
      case baenet_HttpStatusCode::BAENET_CONFLICT:
      case baenet_HttpStatusCode::BAENET_GONE:
      case baenet_HttpStatusCode::BAENET_LENGTH_REQUIRED:
      case baenet_HttpStatusCode::BAENET_PRECONDITION_FAILED:
      case baenet_HttpStatusCode::BAENET_REQUEST_ENTITY_TOO_LARGE:
      case baenet_HttpStatusCode::BAENET_REQUEST_URI_TOO_LARGE:
      case baenet_HttpStatusCode::BAENET_UNSUPPORTED_MEDIA_TYPE:
      case baenet_HttpStatusCode::BAENET_REQUESTED_RANGE_NOT_SATISFIABLE:
      case baenet_HttpStatusCode::BAENET_EXPECTATION_FAILED:
      case baenet_HttpStatusCode::BAENET_INTERNAL_SERVER_ERROR:
      case baenet_HttpStatusCode::BAENET_NOT_IMPLEMENTED:
      case baenet_HttpStatusCode::BAENET_BAD_GATEWAY:
      case baenet_HttpStatusCode::BAENET_SERVICE_UNAVAILABLE:
      case baenet_HttpStatusCode::BAENET_GATEWAY_TIME_OUT:
      case baenet_HttpStatusCode::BAENET_HTTP_VERSION_NOT_SUPPORTED:
        *result = (baenet_HttpStatusCode::Value)number;
        return 0;
      default:
        return -1;
    }
}

int
baenet_HttpStatusCode::fromString(baenet_HttpStatusCode::Value *result,
                                  const char                   *string,
                                  int                           stringLength)
{
    switch(stringLength) {
        case 2: {
            if ((string[0]|0x20)=='o'
             && (string[1]|0x20)=='k')
            {
                *result = baenet_HttpStatusCode::BAENET_OK;
                return 0;
            }
        } break;
        case 4: {
            if ((string[0]|0x20)=='g'
             && (string[1]|0x20)=='o'
             && (string[2]|0x20)=='n'
             && (string[3]|0x20)=='e')
            {
                *result = baenet_HttpStatusCode::BAENET_GONE;
                return 0;
            }
        } break;
        case 5: {
            if ((string[0]|0x20)=='f'
             && (string[1]|0x20)=='o'
             && (string[2]|0x20)=='u'
             && (string[3]|0x20)=='n'
             && (string[4]|0x20)=='d')
            {
                *result = baenet_HttpStatusCode::BAENET_FOUND;
                return 0;
            }
        } break;
        case 7: {
            if ((string[0]|0x20)=='c'
             && (string[1]|0x20)=='r'
             && (string[2]|0x20)=='e'
             && (string[3]|0x20)=='a'
             && (string[4]|0x20)=='t'
             && (string[5]|0x20)=='e'
             && (string[6]|0x20)=='d')
            {
                *result = baenet_HttpStatusCode::BAENET_CREATED;
                return 0;
            }
        } break;
        case 8: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'A': {
                    if ((string[1]|0x20)=='c'
                     && (string[2]|0x20)=='c'
                     && (string[3]|0x20)=='e'
                     && (string[4]|0x20)=='p'
                     && (string[5]|0x20)=='t'
                     && (string[6]|0x20)=='e'
                     && (string[7]|0x20)=='d')
                    {
                        *result = baenet_HttpStatusCode::BAENET_ACCEPTED;
                        return 0;
                    }
                } break;
                case 'C': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='n')
                    {
                        switch(bdeu_CharType::toUpper(string[3])) {
                            case 'F': {
                                if ((string[4]|0x20)=='l'
                                 && (string[5]|0x20)=='i'
                                 && (string[6]|0x20)=='c'
                                 && (string[7]|0x20)=='t')
                                {
                                    *result =
                                        baenet_HttpStatusCode::BAENET_CONFLICT;
                                    return 0;
                                }
                            } break;
                            case 'T': {
                                if ((string[4]|0x20)=='i'
                                 && (string[5]|0x20)=='n'
                                 && (string[6]|0x20)=='u'
                                 && (string[7]|0x20)=='e')
                                {
                                    *result =
                                        baenet_HttpStatusCode::BAENET_CONTINUE;
                                    return 0;
                                }
                            } break;
                        }
                    }
                } break;
            }
        } break;
        case 9: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'F': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='r'
                     && (string[3]|0x20)=='b'
                     && (string[4]|0x20)=='i'
                     && (string[5]|0x20)=='d'
                     && (string[6]|0x20)=='d'
                     && (string[7]|0x20)=='e'
                     && (string[8]|0x20)=='n')
                    {
                        *result = baenet_HttpStatusCode::BAENET_FORBIDDEN;
                        return 0;
                    }
                } break;
                case 'N': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='t'
                     && (string[3])==' '
                     && (string[4]|0x20)=='f'
                     && (string[5]|0x20)=='o'
                     && (string[6]|0x20)=='u'
                     && (string[7]|0x20)=='n'
                     && (string[8]|0x20)=='d')
                    {
                        *result = baenet_HttpStatusCode::BAENET_NOT_FOUND;
                        return 0;
                    }
                } break;
                case 'S': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='e'
                     && (string[3])==' '
                     && (string[4]|0x20)=='o'
                     && (string[5]|0x20)=='t'
                     && (string[6]|0x20)=='h'
                     && (string[7]|0x20)=='e'
                     && (string[8]|0x20)=='r')
                    {
                        *result = baenet_HttpStatusCode::BAENET_SEE_OTHER;
                        return 0;
                    }
                } break;
                case 'U': {
                    if ((string[1]|0x20)=='s'
                     && (string[2]|0x20)=='e'
                     && (string[3])==' '
                     && (string[4]|0x20)=='p'
                     && (string[5]|0x20)=='r'
                     && (string[6]|0x20)=='o'
                     && (string[7]|0x20)=='x'
                     && (string[8]|0x20)=='y')
                    {
                        *result = baenet_HttpStatusCode::BAENET_USE_PROXY;
                        return 0;
                    }
                } break;
            }
        } break;
        case 10: {
            if ((string[0]|0x20)=='n'
             && (string[1]|0x20)=='o'
             && (string[2])==' '
             && (string[3]|0x20)=='c'
             && (string[4]|0x20)=='o'
             && (string[5]|0x20)=='n'
             && (string[6]|0x20)=='t'
             && (string[7]|0x20)=='e'
             && (string[8]|0x20)=='n'
             && (string[9]|0x20)=='t')
            {
                *result = baenet_HttpStatusCode::BAENET_NO_CONTENT;
                return 0;
            }
        } break;
        case 11: {
            if ((string[0]|0x20)=='b'
             && (string[1]|0x20)=='a'
             && (string[2]|0x20)=='d'
             && (string[3])==' ')
            {
                switch(bdeu_CharType::toUpper(string[4])) {
                    case 'G': {
                        if ((string[5]|0x20)=='a'
                         && (string[6]|0x20)=='t'
                         && (string[7]|0x20)=='e'
                         && (string[8]|0x20)=='w'
                         && (string[9]|0x20)=='a'
                         && (string[10]|0x20)=='y')
                        {
                            *result =
                                     baenet_HttpStatusCode::BAENET_BAD_GATEWAY;
                            return 0;
                        }
                    } break;
                    case 'R': {
                        if ((string[5]|0x20)=='e'
                         && (string[6]|0x20)=='q'
                         && (string[7]|0x20)=='u'
                         && (string[8]|0x20)=='e'
                         && (string[9]|0x20)=='s'
                         && (string[10]|0x20)=='t')
                        {
                            *result =
                                     baenet_HttpStatusCode::BAENET_BAD_REQUEST;
                            return 0;
                        }
                    } break;
                }
            }
        } break;
        case 12: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'N': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='t'
                     && (string[3])==' '
                     && (string[4]|0x20)=='m'
                     && (string[5]|0x20)=='o'
                     && (string[6]|0x20)=='d'
                     && (string[7]|0x20)=='i'
                     && (string[8]|0x20)=='f'
                     && (string[9]|0x20)=='i'
                     && (string[10]|0x20)=='e'
                     && (string[11]|0x20)=='d')
                    {
                        *result = baenet_HttpStatusCode::BAENET_NOT_MODIFIED;
                        return 0;
                    }
                } break;
                case 'U': {
                    if ((string[1]|0x20)=='n'
                     && (string[2]|0x20)=='a'
                     && (string[3]|0x20)=='u'
                     && (string[4]|0x20)=='t'
                     && (string[5]|0x20)=='h'
                     && (string[6]|0x20)=='o'
                     && (string[7]|0x20)=='r'
                     && (string[8]|0x20)=='i'
                     && (string[9]|0x20)=='z'
                     && (string[10]|0x20)=='e'
                     && (string[11]|0x20)=='d')
                    {
                        *result = baenet_HttpStatusCode::BAENET_UNAUTHORIZED;
                        return 0;
                    }
                } break;
            }
        } break;
        case 13: {
            if ((string[0]|0x20)=='r'
             && (string[1]|0x20)=='e'
             && (string[2]|0x20)=='s'
             && (string[3]|0x20)=='e'
             && (string[4]|0x20)=='t'
             && (string[5])==' '
             && (string[6]|0x20)=='c'
             && (string[7]|0x20)=='o'
             && (string[8]|0x20)=='n'
             && (string[9]|0x20)=='t'
             && (string[10]|0x20)=='e'
             && (string[11]|0x20)=='n'
             && (string[12]|0x20)=='t')
            {
                *result = baenet_HttpStatusCode::BAENET_RESET_CONTENT;
                return 0;
            }
        } break;
        case 14: {
            if ((string[0]|0x20)=='n'
             && (string[1]|0x20)=='o'
             && (string[2]|0x20)=='t'
             && (string[3])==' '
             && (string[4]|0x20)=='a'
             && (string[5]|0x20)=='c'
             && (string[6]|0x20)=='c'
             && (string[7]|0x20)=='e'
             && (string[8]|0x20)=='p'
             && (string[9]|0x20)=='t'
             && (string[10]|0x20)=='a'
             && (string[11]|0x20)=='b'
             && (string[12]|0x20)=='l'
             && (string[13]|0x20)=='e')
            {
                *result = baenet_HttpStatusCode::BAENET_NOT_ACCEPTABLE;
                return 0;
            }
        } break;
        case 15: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'L': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='n'
                     && (string[3]|0x20)=='g'
                     && (string[4]|0x20)=='t'
                     && (string[5]|0x20)=='h'
                     && (string[6])==' '
                     && (string[7]|0x20)=='r'
                     && (string[8]|0x20)=='e'
                     && (string[9]|0x20)=='q'
                     && (string[10]|0x20)=='u'
                     && (string[11]|0x20)=='i'
                     && (string[12]|0x20)=='r'
                     && (string[13]|0x20)=='e'
                     && (string[14]|0x20)=='d')
                    {
                        *result =
                                 baenet_HttpStatusCode::BAENET_LENGTH_REQUIRED;
                        return 0;
                    }
                } break;
                case 'N': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='t'
                     && (string[3])==' '
                     && (string[4]|0x20)=='i'
                     && (string[5]|0x20)=='m'
                     && (string[6]|0x20)=='p'
                     && (string[7]|0x20)=='l'
                     && (string[8]|0x20)=='e'
                     && (string[9]|0x20)=='m'
                     && (string[10]|0x20)=='e'
                     && (string[11]|0x20)=='n'
                     && (string[12]|0x20)=='t'
                     && (string[13]|0x20)=='e'
                     && (string[14]|0x20)=='d')
                    {
                        *result =
                                 baenet_HttpStatusCode::BAENET_NOT_IMPLEMENTED;
                        return 0;
                    }
                } break;
                case 'P': {
                    if ((string[1]|0x20)=='a'
                     && (string[2]|0x20)=='r'
                     && (string[3]|0x20)=='t'
                     && (string[4]|0x20)=='i'
                     && (string[5]|0x20)=='a'
                     && (string[6]|0x20)=='l'
                     && (string[7])==' '
                     && (string[8]|0x20)=='c'
                     && (string[9]|0x20)=='o'
                     && (string[10]|0x20)=='n'
                     && (string[11]|0x20)=='t'
                     && (string[12]|0x20)=='e'
                     && (string[13]|0x20)=='n'
                     && (string[14]|0x20)=='t')
                    {
                        *result =
                                 baenet_HttpStatusCode::BAENET_PARTIAL_CONTENT;
                        return 0;
                    }
                } break;
            }
        } break;
        case 16: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'G': {
                    if ((string[1]|0x20)=='a'
                     && (string[2]|0x20)=='t'
                     && (string[3]|0x20)=='e'
                     && (string[4]|0x20)=='w'
                     && (string[5]|0x20)=='a'
                     && (string[6]|0x20)=='y'
                     && (string[7])==' '
                     && (string[8]|0x20)=='t'
                     && (string[9]|0x20)=='i'
                     && (string[10]|0x20)=='m'
                     && (string[11]|0x20)=='e'
                     && (string[12])=='-'
                     && (string[13]|0x20)=='o'
                     && (string[14]|0x20)=='u'
                     && (string[15]|0x20)=='t')
                    {
                        *result =
                                baenet_HttpStatusCode::BAENET_GATEWAY_TIME_OUT;
                        return 0;
                    }
                } break;
                case 'M': {
                    if ((string[1]|0x20)=='u'
                     && (string[2]|0x20)=='l'
                     && (string[3]|0x20)=='t'
                     && (string[4]|0x20)=='i'
                     && (string[5]|0x20)=='p'
                     && (string[6]|0x20)=='l'
                     && (string[7]|0x20)=='e'
                     && (string[8])==' '
                     && (string[9]|0x20)=='c'
                     && (string[10]|0x20)=='h'
                     && (string[11]|0x20)=='o'
                     && (string[12]|0x20)=='i'
                     && (string[13]|0x20)=='c'
                     && (string[14]|0x20)=='e'
                     && (string[15]|0x20)=='s')
                    {
                        *result =
                                baenet_HttpStatusCode::BAENET_MULTIPLE_CHOICES;
                        return 0;
                    }
                } break;
                case 'P': {
                    if ((string[1]|0x20)=='a'
                     && (string[2]|0x20)=='y'
                     && (string[3]|0x20)=='m'
                     && (string[4]|0x20)=='e'
                     && (string[5]|0x20)=='n'
                     && (string[6]|0x20)=='t'
                     && (string[7])==' '
                     && (string[8]|0x20)=='r'
                     && (string[9]|0x20)=='e'
                     && (string[10]|0x20)=='q'
                     && (string[11]|0x20)=='u'
                     && (string[12]|0x20)=='i'
                     && (string[13]|0x20)=='r'
                     && (string[14]|0x20)=='e'
                     && (string[15]|0x20)=='d')
                    {
                        *result =
                                baenet_HttpStatusCode::BAENET_PAYMENT_REQUIRED;
                        return 0;
                    }
                } break;
                case 'R': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='q'
                     && (string[3]|0x20)=='u'
                     && (string[4]|0x20)=='e'
                     && (string[5]|0x20)=='s'
                     && (string[6]|0x20)=='t'
                     && (string[7])==' '
                     && (string[8]|0x20)=='t'
                     && (string[9]|0x20)=='i'
                     && (string[10]|0x20)=='m'
                     && (string[11]|0x20)=='e'
                     && (string[12])=='-'
                     && (string[13]|0x20)=='o'
                     && (string[14]|0x20)=='u'
                     && (string[15]|0x20)=='t')
                    {
                        *result =
                                baenet_HttpStatusCode::BAENET_REQUEST_TIME_OUT;
                        return 0;
                    }
                } break;
            }
        } break;
        case 17: {
            if ((string[0]|0x20)=='m'
             && (string[1]|0x20)=='o'
             && (string[2]|0x20)=='v'
             && (string[3]|0x20)=='e'
             && (string[4]|0x20)=='d'
             && (string[5])==' '
             && (string[6]|0x20)=='p'
             && (string[7]|0x20)=='e'
             && (string[8]|0x20)=='r'
             && (string[9]|0x20)=='m'
             && (string[10]|0x20)=='a'
             && (string[11]|0x20)=='n'
             && (string[12]|0x20)=='e'
             && (string[13]|0x20)=='n'
             && (string[14]|0x20)=='t'
             && (string[15]|0x20)=='l'
             && (string[16]|0x20)=='y')
            {
                *result = baenet_HttpStatusCode::BAENET_MOVED_PERMANENTLY;
                return 0;
            }
        } break;
        case 18: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'E': {
                    if ((string[1]|0x20)=='x'
                     && (string[2]|0x20)=='p'
                     && (string[3]|0x20)=='e'
                     && (string[4]|0x20)=='c'
                     && (string[5]|0x20)=='t'
                     && (string[6]|0x20)=='a'
                     && (string[7]|0x20)=='t'
                     && (string[8]|0x20)=='i'
                     && (string[9]|0x20)=='o'
                     && (string[10]|0x20)=='n'
                     && (string[11])==' '
                     && (string[12]|0x20)=='f'
                     && (string[13]|0x20)=='a'
                     && (string[14]|0x20)=='i'
                     && (string[15]|0x20)=='l'
                     && (string[16]|0x20)=='e'
                     && (string[17]|0x20)=='d')
                    {
                        *result =
                              baenet_HttpStatusCode::BAENET_EXPECTATION_FAILED;
                        return 0;
                    }
                } break;
                case 'M': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='t'
                     && (string[3]|0x20)=='h'
                     && (string[4]|0x20)=='o'
                     && (string[5]|0x20)=='d'
                     && (string[6])==' '
                     && (string[7]|0x20)=='n'
                     && (string[8]|0x20)=='o'
                     && (string[9]|0x20)=='t'
                     && (string[10])==' '
                     && (string[11]|0x20)=='a'
                     && (string[12]|0x20)=='l'
                     && (string[13]|0x20)=='l'
                     && (string[14]|0x20)=='o'
                     && (string[15]|0x20)=='w'
                     && (string[16]|0x20)=='e'
                     && (string[17]|0x20)=='d')
                    {
                        *result =
                              baenet_HttpStatusCode::BAENET_METHOD_NOT_ALLOWED;
                        return 0;
                    }
                } break;
                case 'T': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='m'
                     && (string[3]|0x20)=='p'
                     && (string[4]|0x20)=='o'
                     && (string[5]|0x20)=='r'
                     && (string[6]|0x20)=='a'
                     && (string[7]|0x20)=='r'
                     && (string[8]|0x20)=='y'
                     && (string[9])==' '
                     && (string[10]|0x20)=='r'
                     && (string[11]|0x20)=='e'
                     && (string[12]|0x20)=='d'
                     && (string[13]|0x20)=='i'
                     && (string[14]|0x20)=='r'
                     && (string[15]|0x20)=='e'
                     && (string[16]|0x20)=='c'
                     && (string[17]|0x20)=='t')
                    {
                        *result =
                              baenet_HttpStatusCode::BAENET_TEMPORARY_REDIRECT;
                        return 0;
                    }
                } break;
            }
        } break;
        case 19: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'P': {
                    if ((string[1]|0x20)=='r'
                     && (string[2]|0x20)=='e'
                     && (string[3]|0x20)=='c'
                     && (string[4]|0x20)=='o'
                     && (string[5]|0x20)=='n'
                     && (string[6]|0x20)=='d'
                     && (string[7]|0x20)=='i'
                     && (string[8]|0x20)=='t'
                     && (string[9]|0x20)=='i'
                     && (string[10]|0x20)=='o'
                     && (string[11]|0x20)=='n'
                     && (string[12])==' '
                     && (string[13]|0x20)=='f'
                     && (string[14]|0x20)=='a'
                     && (string[15]|0x20)=='i'
                     && (string[16]|0x20)=='l'
                     && (string[17]|0x20)=='e'
                     && (string[18]|0x20)=='d')
                    {
                        *result =
                             baenet_HttpStatusCode::BAENET_PRECONDITION_FAILED;
                        return 0;
                    }
                } break;
                case 'S': {
                    switch(bdeu_CharType::toUpper(string[1])) {
                        case 'E': {
                            if ((string[2]|0x20)=='r'
                             && (string[3]|0x20)=='v'
                             && (string[4]|0x20)=='i'
                             && (string[5]|0x20)=='c'
                             && (string[6]|0x20)=='e'
                             && (string[7])==' '
                             && (string[8]|0x20)=='u'
                             && (string[9]|0x20)=='n'
                             && (string[10]|0x20)=='a'
                             && (string[11]|0x20)=='v'
                             && (string[12]|0x20)=='a'
                             && (string[13]|0x20)=='i'
                             && (string[14]|0x20)=='l'
                             && (string[15]|0x20)=='a'
                             && (string[16]|0x20)=='b'
                             && (string[17]|0x20)=='l'
                             && (string[18]|0x20)=='e')
                            {
                                *result =
                             baenet_HttpStatusCode::BAENET_SERVICE_UNAVAILABLE;
                                return 0;
                            }
                        } break;
                        case 'W': {
                            if ((string[2]|0x20)=='i'
                             && (string[3]|0x20)=='t'
                             && (string[4]|0x20)=='c'
                             && (string[5]|0x20)=='h'
                             && (string[6]|0x20)=='i'
                             && (string[7]|0x20)=='n'
                             && (string[8]|0x20)=='g'
                             && (string[9])==' '
                             && (string[10]|0x20)=='p'
                             && (string[11]|0x20)=='r'
                             && (string[12]|0x20)=='o'
                             && (string[13]|0x20)=='t'
                             && (string[14]|0x20)=='o'
                             && (string[15]|0x20)=='c'
                             && (string[16]|0x20)=='o'
                             && (string[17]|0x20)=='l'
                             && (string[18]|0x20)=='s')
                            {
                                *result =
                             baenet_HttpStatusCode::BAENET_SWITCHING_PROTOCOLS;
                                return 0;
                            }
                        } break;
                    }
                } break;
            }
        } break;
        case 21: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'I': {
                    if ((string[1]|0x20)=='n'
                     && (string[2]|0x20)=='t'
                     && (string[3]|0x20)=='e'
                     && (string[4]|0x20)=='r'
                     && (string[5]|0x20)=='n'
                     && (string[6]|0x20)=='a'
                     && (string[7]|0x20)=='l'
                     && (string[8])==' '
                     && (string[9]|0x20)=='s'
                     && (string[10]|0x20)=='e'
                     && (string[11]|0x20)=='r'
                     && (string[12]|0x20)=='v'
                     && (string[13]|0x20)=='e'
                     && (string[14]|0x20)=='r'
                     && (string[15])==' '
                     && (string[16]|0x20)=='e'
                     && (string[17]|0x20)=='r'
                     && (string[18]|0x20)=='r'
                     && (string[19]|0x20)=='o'
                     && (string[20]|0x20)=='r')
                    {
                        *result =
                           baenet_HttpStatusCode::BAENET_INTERNAL_SERVER_ERROR;
                        return 0;
                    }
                } break;
                case 'R': {
                    if ((string[1]|0x20)=='e'
                     && (string[2]|0x20)=='q'
                     && (string[3]|0x20)=='u'
                     && (string[4]|0x20)=='e'
                     && (string[5]|0x20)=='s'
                     && (string[6]|0x20)=='t'
                     && (string[7])=='-'
                     && (string[8]|0x20)=='u'
                     && (string[9]|0x20)=='r'
                     && (string[10]|0x20)=='i'
                     && (string[11])==' '
                     && (string[12]|0x20)=='t'
                     && (string[13]|0x20)=='o'
                     && (string[14]|0x20)=='o'
                     && (string[15])==' '
                     && (string[16]|0x20)=='l'
                     && (string[17]|0x20)=='a'
                     && (string[18]|0x20)=='r'
                     && (string[19]|0x20)=='g'
                     && (string[20]|0x20)=='e')
                    {
                        *result =
                           baenet_HttpStatusCode::BAENET_REQUEST_URI_TOO_LARGE;
                        return 0;
                    }
                } break;
            }
        } break;
        case 22: {
            if ((string[0]|0x20)=='u'
             && (string[1]|0x20)=='n'
             && (string[2]|0x20)=='s'
             && (string[3]|0x20)=='u'
             && (string[4]|0x20)=='p'
             && (string[5]|0x20)=='p'
             && (string[6]|0x20)=='o'
             && (string[7]|0x20)=='r'
             && (string[8]|0x20)=='t'
             && (string[9]|0x20)=='e'
             && (string[10]|0x20)=='d'
             && (string[11])==' '
             && (string[12]|0x20)=='m'
             && (string[13]|0x20)=='e'
             && (string[14]|0x20)=='d'
             && (string[15]|0x20)=='i'
             && (string[16]|0x20)=='a'
             && (string[17])==' '
             && (string[18]|0x20)=='t'
             && (string[19]|0x20)=='y'
             && (string[20]|0x20)=='p'
             && (string[21]|0x20)=='e')
            {
                *result = baenet_HttpStatusCode::BAENET_UNSUPPORTED_MEDIA_TYPE;
                return 0;
            }
        } break;
        case 24: {
            if ((string[0]|0x20)=='r'
             && (string[1]|0x20)=='e'
             && (string[2]|0x20)=='q'
             && (string[3]|0x20)=='u'
             && (string[4]|0x20)=='e'
             && (string[5]|0x20)=='s'
             && (string[6]|0x20)=='t'
             && (string[7])==' '
             && (string[8]|0x20)=='e'
             && (string[9]|0x20)=='n'
             && (string[10]|0x20)=='t'
             && (string[11]|0x20)=='i'
             && (string[12]|0x20)=='t'
             && (string[13]|0x20)=='y'
             && (string[14])==' '
             && (string[15]|0x20)=='t'
             && (string[16]|0x20)=='o'
             && (string[17]|0x20)=='o'
             && (string[18])==' '
             && (string[19]|0x20)=='l'
             && (string[20]|0x20)=='a'
             && (string[21]|0x20)=='r'
             && (string[22]|0x20)=='g'
             && (string[23]|0x20)=='e')
            {
                *result =
                        baenet_HttpStatusCode::BAENET_REQUEST_ENTITY_TOO_LARGE;
                return 0;
            }
        } break;
        case 26: {
            if ((string[0]|0x20)=='h'
             && (string[1]|0x20)=='t'
             && (string[2]|0x20)=='t'
             && (string[3]|0x20)=='p'
             && (string[4])==' '
             && (string[5]|0x20)=='v'
             && (string[6]|0x20)=='e'
             && (string[7]|0x20)=='r'
             && (string[8]|0x20)=='s'
             && (string[9]|0x20)=='i'
             && (string[10]|0x20)=='o'
             && (string[11]|0x20)=='n'
             && (string[12])==' '
             && (string[13]|0x20)=='n'
             && (string[14]|0x20)=='o'
             && (string[15]|0x20)=='t'
             && (string[16])==' '
             && (string[17]|0x20)=='s'
             && (string[18]|0x20)=='u'
             && (string[19]|0x20)=='p'
             && (string[20]|0x20)=='p'
             && (string[21]|0x20)=='o'
             && (string[22]|0x20)=='r'
             && (string[23]|0x20)=='t'
             && (string[24]|0x20)=='e'
             && (string[25]|0x20)=='d')
            {
                *result =
                      baenet_HttpStatusCode::BAENET_HTTP_VERSION_NOT_SUPPORTED;
                return 0;
            }
        } break;
        case 29: {
            switch(bdeu_CharType::toUpper(string[0])) {
                case 'N': {
                    if ((string[1]|0x20)=='o'
                     && (string[2]|0x20)=='n'
                     && (string[3])=='-'
                     && (string[4]|0x20)=='a'
                     && (string[5]|0x20)=='u'
                     && (string[6]|0x20)=='t'
                     && (string[7]|0x20)=='h'
                     && (string[8]|0x20)=='o'
                     && (string[9]|0x20)=='r'
                     && (string[10]|0x20)=='i'
                     && (string[11]|0x20)=='t'
                     && (string[12]|0x20)=='a'
                     && (string[13]|0x20)=='t'
                     && (string[14]|0x20)=='i'
                     && (string[15]|0x20)=='v'
                     && (string[16]|0x20)=='e'
                     && (string[17])==' '
                     && (string[18]|0x20)=='i'
                     && (string[19]|0x20)=='n'
                     && (string[20]|0x20)=='f'
                     && (string[21]|0x20)=='o'
                     && (string[22]|0x20)=='r'
                     && (string[23]|0x20)=='m'
                     && (string[24]|0x20)=='a'
                     && (string[25]|0x20)=='t'
                     && (string[26]|0x20)=='i'
                     && (string[27]|0x20)=='o'
                     && (string[28]|0x20)=='n')
                    {
                        *result =
                   baenet_HttpStatusCode::BAENET_NON_AUTHORITATIVE_INFORMATION;
                        return 0;
                    }
                } break;
                case 'P': {
                    if ((string[1]|0x20)=='r'
                     && (string[2]|0x20)=='o'
                     && (string[3]|0x20)=='x'
                     && (string[4]|0x20)=='y'
                     && (string[5])==' '
                     && (string[6]|0x20)=='a'
                     && (string[7]|0x20)=='u'
                     && (string[8]|0x20)=='t'
                     && (string[9]|0x20)=='h'
                     && (string[10]|0x20)=='e'
                     && (string[11]|0x20)=='n'
                     && (string[12]|0x20)=='t'
                     && (string[13]|0x20)=='i'
                     && (string[14]|0x20)=='c'
                     && (string[15]|0x20)=='a'
                     && (string[16]|0x20)=='t'
                     && (string[17]|0x20)=='i'
                     && (string[18]|0x20)=='o'
                     && (string[19]|0x20)=='n'
                     && (string[20])==' '
                     && (string[21]|0x20)=='r'
                     && (string[22]|0x20)=='e'
                     && (string[23]|0x20)=='q'
                     && (string[24]|0x20)=='u'
                     && (string[25]|0x20)=='i'
                     && (string[26]|0x20)=='r'
                     && (string[27]|0x20)=='e'
                     && (string[28]|0x20)=='d')
                    {
                        *result =
                   baenet_HttpStatusCode::BAENET_PROXY_AUTHENTICATION_REQUIRED;
                        return 0;
                    }
                } break;
            }
        } break;
        case 31: {
            if ((string[0]|0x20)=='r'
             && (string[1]|0x20)=='e'
             && (string[2]|0x20)=='q'
             && (string[3]|0x20)=='u'
             && (string[4]|0x20)=='e'
             && (string[5]|0x20)=='s'
             && (string[6]|0x20)=='t'
             && (string[7]|0x20)=='e'
             && (string[8]|0x20)=='d'
             && (string[9])==' '
             && (string[10]|0x20)=='r'
             && (string[11]|0x20)=='a'
             && (string[12]|0x20)=='n'
             && (string[13]|0x20)=='g'
             && (string[14]|0x20)=='e'
             && (string[15])==' '
             && (string[16]|0x20)=='n'
             && (string[17]|0x20)=='o'
             && (string[18]|0x20)=='t'
             && (string[19])==' '
             && (string[20]|0x20)=='s'
             && (string[21]|0x20)=='a'
             && (string[22]|0x20)=='t'
             && (string[23]|0x20)=='i'
             && (string[24]|0x20)=='s'
             && (string[25]|0x20)=='f'
             && (string[26]|0x20)=='i'
             && (string[27]|0x20)=='a'
             && (string[28]|0x20)=='b'
             && (string[29]|0x20)=='l'
             && (string[30]|0x20)=='e')
            {
                *result =
                 baenet_HttpStatusCode::BAENET_REQUESTED_RANGE_NOT_SATISFIABLE;
                return 0;
            }
        } break;
    }

    return -1;
}

const char *baenet_HttpStatusCode::toString(baenet_HttpStatusCode::Value value)
{
    switch (value) {
      case BAENET_CONTINUE: {
        return "Continue";
      } break;
      case BAENET_SWITCHING_PROTOCOLS: {
        return "Switching Protocols";
      } break;
      case BAENET_OK: {
        return "OK";
      } break;
      case BAENET_CREATED: {
        return "Created";
      } break;
      case BAENET_ACCEPTED: {
        return "Accepted";
      } break;
      case BAENET_NON_AUTHORITATIVE_INFORMATION: {
        return "Non-Authoritative Information";
      } break;
      case BAENET_NO_CONTENT: {
        return "No Content";
      } break;
      case BAENET_RESET_CONTENT: {
        return "Reset Content";
      } break;
      case BAENET_PARTIAL_CONTENT: {
        return "Partial Content";
      } break;
      case BAENET_MULTIPLE_CHOICES: {
        return "Multiple Choices";
      } break;
      case BAENET_MOVED_PERMANENTLY: {
        return "Moved Permanently";
      } break;
      case BAENET_FOUND: {
        return "Found";
      } break;
      case BAENET_SEE_OTHER: {
        return "See Other";
      } break;
      case BAENET_NOT_MODIFIED: {
        return "Not Modified";
      } break;
      case BAENET_USE_PROXY: {
        return "Use Proxy";
      } break;
      case BAENET_TEMPORARY_REDIRECT: {
        return "Temporary Redirect";
      } break;
      case BAENET_BAD_REQUEST: {
        return "Bad Request";
      } break;
      case BAENET_UNAUTHORIZED: {
        return "Unauthorized";
      } break;
      case BAENET_PAYMENT_REQUIRED: {
        return "Payment Required";
      } break;
      case BAENET_FORBIDDEN: {
        return "Forbidden";
      } break;
      case BAENET_NOT_FOUND: {
        return "Not Found";
      } break;
      case BAENET_METHOD_NOT_ALLOWED: {
        return "Method Not Allowed";
      } break;
      case BAENET_NOT_ACCEPTABLE: {
        return "Not Acceptable";
      } break;
      case BAENET_PROXY_AUTHENTICATION_REQUIRED: {
        return "Proxy Authentication Required";
      } break;
      case BAENET_REQUEST_TIME_OUT: {
        return "Request Time-out";
      } break;
      case BAENET_CONFLICT: {
        return "Conflict";
      } break;
      case BAENET_GONE: {
        return "Gone";
      } break;
      case BAENET_LENGTH_REQUIRED: {
        return "Length Required";
      } break;
      case BAENET_PRECONDITION_FAILED: {
        return "Precondition Failed";
      } break;
      case BAENET_REQUEST_ENTITY_TOO_LARGE: {
        return "Request Entity Too Large";
      } break;
      case BAENET_REQUEST_URI_TOO_LARGE: {
        return "Request-URI Too Large";
      } break;
      case BAENET_UNSUPPORTED_MEDIA_TYPE: {
        return "Unsupported Media Type";
      } break;
      case BAENET_REQUESTED_RANGE_NOT_SATISFIABLE: {
        return "Requested range not satisfiable";
      } break;
      case BAENET_EXPECTATION_FAILED: {
        return "Expectation Failed";
      } break;
      case BAENET_INTERNAL_SERVER_ERROR: {
        return "Internal Server Error";
      } break;
      case BAENET_NOT_IMPLEMENTED: {
        return "Not Implemented";
      } break;
      case BAENET_BAD_GATEWAY: {
        return "Bad Gateway";
      } break;
      case BAENET_SERVICE_UNAVAILABLE: {
        return "Service Unavailable";
      } break;
      case BAENET_GATEWAY_TIME_OUT: {
        return "Gateway Time-out";
      } break;
      case BAENET_HTTP_VERSION_NOT_SUPPORTED: {
        return "HTTP Version not supported";
      } break;
      default: {
        // NOTE: IF THIS COMPONENT IS REGENERATED, THE FOLLOWING *MUST* BE
        // REINSTATED (SEE DRQS 23241856).

        return "(* Custom HTTP Status Code *)";
      } break;
    }

    return 0;
}

}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Thu Feb 11 13:00:11 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
