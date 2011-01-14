// baenet_httpstatuscode.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAENET_HTTPSTATUSCODE
#define INCLUDED_BAENET_HTTPSTATUSCODE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baenet_httpstatuscode_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@SEE_ALSO: RFC 2616
//
//@DESCRIPTION:  This component provides an enumeration
// 'baenet_HttpStatusCode::Value' for all the possible HTTP status codes, as
// defined in RFC 2616.

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMERATORINFO
#include <bdeat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {


                      // ===============================                       
                      // class baenet_HttpStatusCode                       
                      // ===============================                       

struct baenet_HttpStatusCode {

  public:
    // TYPES
    enum Value {
        BAENET_CONTINUE                        = 100
      , BAENET_SWITCHING_PROTOCOLS             = 101
      , BAENET_OK                              = 200
      , BAENET_CREATED                         = 201
      , BAENET_ACCEPTED                        = 202
      , BAENET_NON_AUTHORITATIVE_INFORMATION   = 203
      , BAENET_NO_CONTENT                      = 204
      , BAENET_RESET_CONTENT                   = 205
      , BAENET_PARTIAL_CONTENT                 = 206
      , BAENET_MULTIPLE_CHOICES                = 300
      , BAENET_MOVED_PERMANENTLY               = 301
      , BAENET_FOUND                           = 302
      , BAENET_SEE_OTHER                       = 303
      , BAENET_NOT_MODIFIED                    = 304
      , BAENET_USE_PROXY                       = 305
      , BAENET_TEMPORARY_REDIRECT              = 307
      , BAENET_BAD_REQUEST                     = 400
      , BAENET_UNAUTHORIZED                    = 401
      , BAENET_PAYMENT_REQUIRED                = 402
      , BAENET_FORBIDDEN                       = 403
      , BAENET_NOT_FOUND                       = 404
      , BAENET_METHOD_NOT_ALLOWED              = 405
      , BAENET_NOT_ACCEPTABLE                  = 406
      , BAENET_PROXY_AUTHENTICATION_REQUIRED   = 407
      , BAENET_REQUEST_TIME_OUT                = 408
      , BAENET_CONFLICT                        = 409
      , BAENET_GONE                            = 410
      , BAENET_LENGTH_REQUIRED                 = 411
      , BAENET_PRECONDITION_FAILED             = 412
      , BAENET_REQUEST_ENTITY_TOO_LARGE        = 413
      , BAENET_REQUEST_URI_TOO_LARGE           = 414
      , BAENET_UNSUPPORTED_MEDIA_TYPE          = 415
      , BAENET_REQUESTED_RANGE_NOT_SATISFIABLE = 416
      , BAENET_EXPECTATION_FAILED              = 417
      , BAENET_INTERNAL_SERVER_ERROR           = 500
      , BAENET_NOT_IMPLEMENTED                 = 501
      , BAENET_BAD_GATEWAY                     = 502
      , BAENET_SERVICE_UNAVAILABLE             = 503
      , BAENET_GATEWAY_TIME_OUT                = 504
      , BAENET_HTTP_VERSION_NOT_SUPPORTED      = 505
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , CONTINUE                        = BAENET_CONTINUE
      , SWITCHING_PROTOCOLS             = BAENET_SWITCHING_PROTOCOLS
      , OK                              = BAENET_OK
      , CREATED                         = BAENET_CREATED
      , ACCEPTED                        = BAENET_ACCEPTED
      , NON_AUTHORITATIVE_INFORMATION   = BAENET_NON_AUTHORITATIVE_INFORMATION
      , NO_CONTENT                      = BAENET_NO_CONTENT
      , RESET_CONTENT                   = BAENET_RESET_CONTENT
      , PARTIAL_CONTENT                 = BAENET_PARTIAL_CONTENT
      , MULTIPLE_CHOICES                = BAENET_MULTIPLE_CHOICES
      , MOVED_PERMANENTLY               = BAENET_MOVED_PERMANENTLY
      , FOUND                           = BAENET_FOUND
      , SEE_OTHER                       = BAENET_SEE_OTHER
      , NOT_MODIFIED                    = BAENET_NOT_MODIFIED
      , USE_PROXY                       = BAENET_USE_PROXY
      , TEMPORARY_REDIRECT              = BAENET_TEMPORARY_REDIRECT
      , BAD_REQUEST                     = BAENET_BAD_REQUEST
      , UNAUTHORIZED                    = BAENET_UNAUTHORIZED
      , PAYMENT_REQUIRED                = BAENET_PAYMENT_REQUIRED
      , FORBIDDEN                       = BAENET_FORBIDDEN
      , NOT_FOUND                       = BAENET_NOT_FOUND
      , METHOD_NOT_ALLOWED              = BAENET_METHOD_NOT_ALLOWED
      , NOT_ACCEPTABLE                  = BAENET_NOT_ACCEPTABLE
      , PROXY_AUTHENTICATION_REQUIRED   = BAENET_PROXY_AUTHENTICATION_REQUIRED
      , REQUEST_TIME_OUT                = BAENET_REQUEST_TIME_OUT
      , CONFLICT                        = BAENET_CONFLICT
      , GONE                            = BAENET_GONE
      , LENGTH_REQUIRED                 = BAENET_LENGTH_REQUIRED
      , PRECONDITION_FAILED             = BAENET_PRECONDITION_FAILED
      , REQUEST_ENTITY_TOO_LARGE        = BAENET_REQUEST_ENTITY_TOO_LARGE
      , REQUEST_URI_TOO_LARGE           = BAENET_REQUEST_URI_TOO_LARGE
      , UNSUPPORTED_MEDIA_TYPE          = BAENET_UNSUPPORTED_MEDIA_TYPE
      , REQUESTED_RANGE_NOT_SATISFIABLE =
                                         BAENET_REQUESTED_RANGE_NOT_SATISFIABLE
      , EXPECTATION_FAILED              = BAENET_EXPECTATION_FAILED
      , INTERNAL_SERVER_ERROR           = BAENET_INTERNAL_SERVER_ERROR
      , NOT_IMPLEMENTED                 = BAENET_NOT_IMPLEMENTED
      , BAD_GATEWAY                     = BAENET_BAD_GATEWAY
      , SERVICE_UNAVAILABLE             = BAENET_SERVICE_UNAVAILABLE
      , GATEWAY_TIME_OUT                = BAENET_GATEWAY_TIME_OUT
      , HTTP_VERSION_NOT_SUPPORTED      = BAENET_HTTP_VERSION_NOT_SUPPORTED
#endif
    };

    enum {
        BAENET_LENGTH = 40
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).

    static int fromString(Value              *result,
                          const bsl::string&  string);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'string' does not match any
        // enumerator).

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&  stream,
                                Value&   value,
                                int      version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.  Return a reference to
        // the modifiable 'stream'.

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&  stream,
                                 Value    value,
                                 int      version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         baenet_HttpStatusCode::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_ENUMERATION_TRAITS(baenet_HttpStatusCode)


// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                      // -------------------------------                       
                      // class baenet_HttpStatusCode                       
                      // -------------------------------                       

// CLASS METHODS
inline
int baenet_HttpStatusCode::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1
}

inline
int baenet_HttpStatusCode::fromString(Value *result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline
bsl::ostream& baenet_HttpStatusCode::print(bsl::ostream&      stream,
                                 baenet_HttpStatusCode::Value value)
{
    return stream << toString(value);
}

template <class STREAM>
STREAM& baenet_HttpStatusCode::bdexStreamIn(STREAM&             stream,
                                   baenet_HttpStatusCode::Value& value,
                                   int                 version)
{
    switch(version) {
      case 1: {
        int readValue;
        stream.getInt32(readValue);
        if (stream) {
            if (fromInt(&value, readValue)) {
               stream.invalidate();   // bad value in stream
            }
        }
      } break;
      default: {
        stream.invalidate();          // unrecognized version number
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& baenet_HttpStatusCode::bdexStreamOut(STREAM&              stream,
                                    baenet_HttpStatusCode::Value value,
                                    int                version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}


// FREE FUNCTIONS

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        baenet_HttpStatusCode::Value rhs)
{
    return baenet_HttpStatusCode::print(stream, rhs);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Thu Feb 11 13:00:11 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
