// baenet_httprequestheaderfields.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAENET_HTTPREQUESTHEADERFIELDS
#define INCLUDED_BAENET_HTTPREQUESTHEADERFIELDS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baenet_httprequestheaderfields_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@SEE_ALSO: RFC 2616
//
//@DESCRIPTION:  This component provides a value-semantic container for HTTP
// request header fields, as defined in RFC 2616.

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BAENET_HTTPHOST
#include <baenet_httphost.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bslma_Allocator;


                  // ========================================                  
                  // class baenet_HttpRequestHeaderFields                  
                  // ========================================                  

class baenet_HttpRequestHeaderFields {

    // INSTANCE DATA
    bsl::vector<bsl::string>                  d_accept;
    bsl::vector<bsl::string>                  d_acceptCharset;
    bsl::vector<bsl::string>                  d_acceptEncoding;
    bsl::vector<bsl::string>                  d_acceptLanguage;
    bdeut_NullableValue<bsl::string>          d_authorization;
    bdeut_NullableValue<bsl::string>          d_expect;
    bdeut_NullableValue<bsl::string>          d_from;
    bdeut_NullableValue<bsl::string>          d_ifMatch;
    bdeut_NullableValue<bsl::string>          d_ifNoneMatch;
    bdeut_NullableValue<bsl::string>          d_ifRange;
    bdeut_NullableValue<bsl::string>          d_proxyAuthorization;
    bdeut_NullableValue<bsl::string>          d_range;
    bdeut_NullableValue<bsl::string>          d_referer;
    bdeut_NullableValue<bsl::string>          d_te;
    bdeut_NullableValue<bsl::string>          d_userAgent;
    bdeut_NullableValue<bdet_DatetimeTz>      d_ifModifiedSince;
    bdeut_NullableValue<bdet_DatetimeTz>      d_ifUnmodifiedSince;
    bdeut_NullableValue<baenet_HttpHost>  d_host;
    bdeut_NullableValue<int>                  d_maxForwards;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ACCEPT              = 0
      , ATTRIBUTE_ID_ACCEPT_CHARSET      = 1
      , ATTRIBUTE_ID_ACCEPT_ENCODING     = 2
      , ATTRIBUTE_ID_ACCEPT_LANGUAGE     = 3
      , ATTRIBUTE_ID_AUTHORIZATION       = 4
      , ATTRIBUTE_ID_EXPECT              = 5
      , ATTRIBUTE_ID_FROM                = 6
      , ATTRIBUTE_ID_HOST                = 7
      , ATTRIBUTE_ID_IF_MATCH            = 8
      , ATTRIBUTE_ID_IF_MODIFIED_SINCE   = 9
      , ATTRIBUTE_ID_IF_NONE_MATCH       = 10
      , ATTRIBUTE_ID_IF_RANGE            = 11
      , ATTRIBUTE_ID_IF_UNMODIFIED_SINCE = 12
      , ATTRIBUTE_ID_MAX_FORWARDS        = 13
      , ATTRIBUTE_ID_PROXY_AUTHORIZATION = 14
      , ATTRIBUTE_ID_RANGE               = 15
      , ATTRIBUTE_ID_REFERER             = 16
      , ATTRIBUTE_ID_TE                  = 17
      , ATTRIBUTE_ID_USER_AGENT          = 18
    };

    enum {
        NUM_ATTRIBUTES = 19
    };

    enum {
        ATTRIBUTE_INDEX_ACCEPT              = 0
      , ATTRIBUTE_INDEX_ACCEPT_CHARSET      = 1
      , ATTRIBUTE_INDEX_ACCEPT_ENCODING     = 2
      , ATTRIBUTE_INDEX_ACCEPT_LANGUAGE     = 3
      , ATTRIBUTE_INDEX_AUTHORIZATION       = 4
      , ATTRIBUTE_INDEX_EXPECT              = 5
      , ATTRIBUTE_INDEX_FROM                = 6
      , ATTRIBUTE_INDEX_HOST                = 7
      , ATTRIBUTE_INDEX_IF_MATCH            = 8
      , ATTRIBUTE_INDEX_IF_MODIFIED_SINCE   = 9
      , ATTRIBUTE_INDEX_IF_NONE_MATCH       = 10
      , ATTRIBUTE_INDEX_IF_RANGE            = 11
      , ATTRIBUTE_INDEX_IF_UNMODIFIED_SINCE = 12
      , ATTRIBUTE_INDEX_MAX_FORWARDS        = 13
      , ATTRIBUTE_INDEX_PROXY_AUTHORIZATION = 14
      , ATTRIBUTE_INDEX_RANGE               = 15
      , ATTRIBUTE_INDEX_REFERER             = 16
      , ATTRIBUTE_INDEX_TE                  = 17
      , ATTRIBUTE_INDEX_USER_AGENT          = 18
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit baenet_HttpRequestHeaderFields(
                                          bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpRequestHeaderFields' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    baenet_HttpRequestHeaderFields(
                   const baenet_HttpRequestHeaderFields&  original,
                   bslma_Allocator                       *basicAllocator = 0);
        // Create an object of type 'baenet_HttpRequestHeaderFields' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~baenet_HttpRequestHeaderFields();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpRequestHeaderFields& operator=(
                                    const baenet_HttpRequestHeaderFields& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    bsl::vector<bsl::string>& accept();
        // Return a reference to the modifiable "Accept" attribute of this
        // object.

    bsl::vector<bsl::string>& acceptCharset();
        // Return a reference to the modifiable "AcceptCharset" attribute of
        // this object.

    bsl::vector<bsl::string>& acceptEncoding();
        // Return a reference to the modifiable "AcceptEncoding" attribute of
        // this object.

    bsl::vector<bsl::string>& acceptLanguage();
        // Return a reference to the modifiable "AcceptLanguage" attribute of
        // this object.

    bdeut_NullableValue<bsl::string>& authorization();
        // Return a reference to the modifiable "Authorization" attribute of
        // this object.

    bdeut_NullableValue<bsl::string>& expect();
        // Return a reference to the modifiable "Expect" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& from();
        // Return a reference to the modifiable "From" attribute of this
        // object.

    bdeut_NullableValue<baenet_HttpHost>& host();
        // Return a reference to the modifiable "Host" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& ifMatch();
        // Return a reference to the modifiable "IfMatch" attribute of this
        // object.

    bdeut_NullableValue<bdet_DatetimeTz>& ifModifiedSince();
        // Return a reference to the modifiable "IfModifiedSince" attribute of
        // this object.

    bdeut_NullableValue<bsl::string>& ifNoneMatch();
        // Return a reference to the modifiable "IfNoneMatch" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& ifRange();
        // Return a reference to the modifiable "IfRange" attribute of this
        // object.

    bdeut_NullableValue<bdet_DatetimeTz>& ifUnmodifiedSince();
        // Return a reference to the modifiable "IfUnmodifiedSince" attribute
        // of this object.

    bdeut_NullableValue<int>& maxForwards();
        // Return a reference to the modifiable "MaxForwards" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& proxyAuthorization();
        // Return a reference to the modifiable "ProxyAuthorization" attribute
        // of this object.

    bdeut_NullableValue<bsl::string>& range();
        // Return a reference to the modifiable "Range" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& referer();
        // Return a reference to the modifiable "Referer" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& te();
        // Return a reference to the modifiable "Te" attribute of this object.

    bdeut_NullableValue<bsl::string>& userAgent();
        // Return a reference to the modifiable "UserAgent" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::vector<bsl::string>& accept() const;
        // Return a reference to the non-modifiable "Accept" attribute of this
        // object.

    const bsl::vector<bsl::string>& acceptCharset() const;
        // Return a reference to the non-modifiable "AcceptCharset" attribute
        // of this object.

    const bsl::vector<bsl::string>& acceptEncoding() const;
        // Return a reference to the non-modifiable "AcceptEncoding" attribute
        // of this object.

    const bsl::vector<bsl::string>& acceptLanguage() const;
        // Return a reference to the non-modifiable "AcceptLanguage" attribute
        // of this object.

    const bdeut_NullableValue<bsl::string>& authorization() const;
        // Return a reference to the non-modifiable "Authorization" attribute
        // of this object.

    const bdeut_NullableValue<bsl::string>& expect() const;
        // Return a reference to the non-modifiable "Expect" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& from() const;
        // Return a reference to the non-modifiable "From" attribute of this
        // object.

    const bdeut_NullableValue<baenet_HttpHost>& host() const;
        // Return a reference to the non-modifiable "Host" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& ifMatch() const;
        // Return a reference to the non-modifiable "IfMatch" attribute of this
        // object.

    const bdeut_NullableValue<bdet_DatetimeTz>& ifModifiedSince() const;
        // Return a reference to the non-modifiable "IfModifiedSince" attribute
        // of this object.

    const bdeut_NullableValue<bsl::string>& ifNoneMatch() const;
        // Return a reference to the non-modifiable "IfNoneMatch" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& ifRange() const;
        // Return a reference to the non-modifiable "IfRange" attribute of this
        // object.

    const bdeut_NullableValue<bdet_DatetimeTz>& ifUnmodifiedSince() const;
        // Return a reference to the non-modifiable "IfUnmodifiedSince"
        // attribute of this object.

    const bdeut_NullableValue<int>& maxForwards() const;
        // Return a reference to the non-modifiable "MaxForwards" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& proxyAuthorization() const;
        // Return a reference to the non-modifiable "ProxyAuthorization"
        // attribute of this object.

    const bdeut_NullableValue<bsl::string>& range() const;
        // Return a reference to the non-modifiable "Range" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& referer() const;
        // Return a reference to the non-modifiable "Referer" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& te() const;
        // Return a reference to the non-modifiable "Te" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& userAgent() const;
        // Return a reference to the non-modifiable "UserAgent" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const baenet_HttpRequestHeaderFields& lhs,
                const baenet_HttpRequestHeaderFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baenet_HttpRequestHeaderFields& lhs,
                const baenet_HttpRequestHeaderFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const baenet_HttpRequestHeaderFields&  rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baenet_HttpRequestHeaderFields)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                  // ----------------------------------------                  
                  // class baenet_HttpRequestHeaderFields                  
                  // ----------------------------------------                  

// CLASS METHODS
inline
int baenet_HttpRequestHeaderFields::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& baenet_HttpRequestHeaderFields::bdexStreamIn(STREAM&    stream,
                                                     int        version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_accept, 1);
            bdex_InStreamFunctions::streamIn(stream, d_acceptCharset, 1);
            bdex_InStreamFunctions::streamIn(stream, d_acceptEncoding, 1);
            bdex_InStreamFunctions::streamIn(stream, d_acceptLanguage, 1);
            bdex_InStreamFunctions::streamIn(stream, d_authorization, 1);
            bdex_InStreamFunctions::streamIn(stream, d_expect, 1);
            bdex_InStreamFunctions::streamIn(stream, d_from, 1);
            bdex_InStreamFunctions::streamIn(stream, d_host, 1);
            bdex_InStreamFunctions::streamIn(stream, d_ifMatch, 1);
            bdex_InStreamFunctions::streamIn(stream, d_ifModifiedSince, 1);
            bdex_InStreamFunctions::streamIn(stream, d_ifNoneMatch, 1);
            bdex_InStreamFunctions::streamIn(stream, d_ifRange, 1);
            bdex_InStreamFunctions::streamIn(stream, d_ifUnmodifiedSince, 1);
            bdex_InStreamFunctions::streamIn(stream, d_maxForwards, 1);
            bdex_InStreamFunctions::streamIn(stream, d_proxyAuthorization, 1);
            bdex_InStreamFunctions::streamIn(stream, d_range, 1);
            bdex_InStreamFunctions::streamIn(stream, d_referer, 1);
            bdex_InStreamFunctions::streamIn(stream, d_te, 1);
            bdex_InStreamFunctions::streamIn(stream, d_userAgent, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baenet_HttpRequestHeaderFields::manipulateAttributes(
                                                      MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_accept, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_acceptCharset,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_CHARSET]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_acceptEncoding,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_ENCODING]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_acceptLanguage,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_LANGUAGE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_authorization,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AUTHORIZATION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_expect, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPECT]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_from, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FROM]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_host, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOST]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_ifMatch,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MATCH]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_ifModifiedSince,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MODIFIED_SINCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_ifNoneMatch,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_NONE_MATCH]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_ifRange,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_RANGE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_ifUnmodifiedSince,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_UNMODIFIED_SINCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_maxForwards,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FORWARDS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_proxyAuthorization,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHORIZATION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_range, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RANGE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_referer,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REFERER]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_te, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_userAgent,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USER_AGENT]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int baenet_HttpRequestHeaderFields::manipulateAttribute(
                    MANIPULATOR&  manipulator,
                    int           id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ACCEPT: {
        return manipulator(&d_accept,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT]);
      } break;
      case ATTRIBUTE_ID_ACCEPT_CHARSET: {
        return manipulator(&d_acceptCharset,
                         ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_CHARSET]);
      } break;
      case ATTRIBUTE_ID_ACCEPT_ENCODING: {
        return manipulator(&d_acceptEncoding,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_ENCODING]);
      } break;
      case ATTRIBUTE_ID_ACCEPT_LANGUAGE: {
        return manipulator(&d_acceptLanguage,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_LANGUAGE]);
      } break;
      case ATTRIBUTE_ID_AUTHORIZATION: {
        return manipulator(&d_authorization,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AUTHORIZATION]);
      } break;
      case ATTRIBUTE_ID_EXPECT: {
        return manipulator(&d_expect,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPECT]);
      } break;
      case ATTRIBUTE_ID_FROM: {
        return manipulator(&d_from,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FROM]);
      } break;
      case ATTRIBUTE_ID_HOST: {
        return manipulator(&d_host,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOST]);
      } break;
      case ATTRIBUTE_ID_IF_MATCH: {
        return manipulator(&d_ifMatch,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MATCH]);
      } break;
      case ATTRIBUTE_ID_IF_MODIFIED_SINCE: {
        return manipulator(&d_ifModifiedSince,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MODIFIED_SINCE]);
      } break;
      case ATTRIBUTE_ID_IF_NONE_MATCH: {
        return manipulator(&d_ifNoneMatch,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_NONE_MATCH]);
      } break;
      case ATTRIBUTE_ID_IF_RANGE: {
        return manipulator(&d_ifRange,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_RANGE]);
      } break;
      case ATTRIBUTE_ID_IF_UNMODIFIED_SINCE: {
        return manipulator(&d_ifUnmodifiedSince,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_UNMODIFIED_SINCE]);
      } break;
      case ATTRIBUTE_ID_MAX_FORWARDS: {
        return manipulator(&d_maxForwards,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FORWARDS]);
      } break;
      case ATTRIBUTE_ID_PROXY_AUTHORIZATION: {
        return manipulator(&d_proxyAuthorization,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHORIZATION]);
      } break;
      case ATTRIBUTE_ID_RANGE: {
        return manipulator(&d_range,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RANGE]);
      } break;
      case ATTRIBUTE_ID_REFERER: {
        return manipulator(&d_referer,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REFERER]);
      } break;
      case ATTRIBUTE_ID_TE: {
        return manipulator(&d_te,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TE]);
      } break;
      case ATTRIBUTE_ID_USER_AGENT: {
        return manipulator(&d_userAgent,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USER_AGENT]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int baenet_HttpRequestHeaderFields::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::vector<bsl::string>& baenet_HttpRequestHeaderFields::accept()
{
    return d_accept;
}

inline
bsl::vector<bsl::string>& baenet_HttpRequestHeaderFields::acceptCharset()
{
    return d_acceptCharset;
}

inline
bsl::vector<bsl::string>& baenet_HttpRequestHeaderFields::acceptEncoding()
{
    return d_acceptEncoding;
}

inline
bsl::vector<bsl::string>& baenet_HttpRequestHeaderFields::acceptLanguage()
{
    return d_acceptLanguage;
}

inline
bdeut_NullableValue<bsl::string>&
                                baenet_HttpRequestHeaderFields::authorization()
{
    return d_authorization;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::expect()
{
    return d_expect;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::from()
{
    return d_from;
}

inline
bdeut_NullableValue<baenet_HttpHost>& baenet_HttpRequestHeaderFields::host()
{
    return d_host;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::ifMatch()
{
    return d_ifMatch;
}

inline
bdeut_NullableValue<bdet_DatetimeTz>&
                              baenet_HttpRequestHeaderFields::ifModifiedSince()
{
    return d_ifModifiedSince;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::ifNoneMatch()
{
    return d_ifNoneMatch;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::ifRange()
{
    return d_ifRange;
}

inline
bdeut_NullableValue<bdet_DatetimeTz>&
                            baenet_HttpRequestHeaderFields::ifUnmodifiedSince()
{
    return d_ifUnmodifiedSince;
}

inline
bdeut_NullableValue<int>& baenet_HttpRequestHeaderFields::maxForwards()
{
    return d_maxForwards;
}

inline
bdeut_NullableValue<bsl::string>&
                           baenet_HttpRequestHeaderFields::proxyAuthorization()
{
    return d_proxyAuthorization;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::range()
{
    return d_range;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::referer()
{
    return d_referer;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::te()
{
    return d_te;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpRequestHeaderFields::userAgent()
{
    return d_userAgent;
}

// ACCESSORS
template <class STREAM>
STREAM& baenet_HttpRequestHeaderFields::bdexStreamOut(STREAM&  stream,
                                                      int      version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_accept, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_acceptCharset, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_acceptEncoding, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_acceptLanguage, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_authorization, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_expect, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_from, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_host, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_ifMatch, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_ifModifiedSince, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_ifNoneMatch, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_ifRange, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_ifUnmodifiedSince, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_maxForwards, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_proxyAuthorization, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_range, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_referer, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_te, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_userAgent, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int baenet_HttpRequestHeaderFields::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_accept, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_acceptCharset,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_CHARSET]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_acceptEncoding,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_ENCODING]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_acceptLanguage,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_LANGUAGE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_authorization,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AUTHORIZATION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_expect, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPECT]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_from, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FROM]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_host, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOST]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_ifMatch, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MATCH]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_ifModifiedSince,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MODIFIED_SINCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_ifNoneMatch,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_NONE_MATCH]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_ifRange, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_RANGE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_ifUnmodifiedSince,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_UNMODIFIED_SINCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_maxForwards,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FORWARDS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_proxyAuthorization,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHORIZATION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_range, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RANGE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_referer, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REFERER]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_te, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_userAgent,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USER_AGENT]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int baenet_HttpRequestHeaderFields::accessAttribute(ACCESSOR&  accessor,
                                                    int        id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ACCEPT: {
        return accessor(d_accept,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT]);
      } break;
      case ATTRIBUTE_ID_ACCEPT_CHARSET: {
        return accessor(d_acceptCharset,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_CHARSET]);
      } break;
      case ATTRIBUTE_ID_ACCEPT_ENCODING: {
        return accessor(d_acceptEncoding,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_ENCODING]);
      } break;
      case ATTRIBUTE_ID_ACCEPT_LANGUAGE: {
        return accessor(d_acceptLanguage,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_LANGUAGE]);
      } break;
      case ATTRIBUTE_ID_AUTHORIZATION: {
        return accessor(d_authorization,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AUTHORIZATION]);
      } break;
      case ATTRIBUTE_ID_EXPECT: {
        return accessor(d_expect,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPECT]);
      } break;
      case ATTRIBUTE_ID_FROM: {
        return accessor(d_from,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FROM]);
      } break;
      case ATTRIBUTE_ID_HOST: {
        return accessor(d_host,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOST]);
      } break;
      case ATTRIBUTE_ID_IF_MATCH: {
        return accessor(d_ifMatch,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MATCH]);
      } break;
      case ATTRIBUTE_ID_IF_MODIFIED_SINCE: {
        return accessor(d_ifModifiedSince,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_MODIFIED_SINCE]);
      } break;
      case ATTRIBUTE_ID_IF_NONE_MATCH: {
        return accessor(d_ifNoneMatch,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_NONE_MATCH]);
      } break;
      case ATTRIBUTE_ID_IF_RANGE: {
        return accessor(d_ifRange,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_RANGE]);
      } break;
      case ATTRIBUTE_ID_IF_UNMODIFIED_SINCE: {
        return accessor(d_ifUnmodifiedSince,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_IF_UNMODIFIED_SINCE]);
      } break;
      case ATTRIBUTE_ID_MAX_FORWARDS: {
        return accessor(d_maxForwards,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_FORWARDS]);
      } break;
      case ATTRIBUTE_ID_PROXY_AUTHORIZATION: {
        return accessor(d_proxyAuthorization,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHORIZATION]);
      } break;
      case ATTRIBUTE_ID_RANGE: {
        return accessor(d_range, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RANGE]);
      } break;
      case ATTRIBUTE_ID_REFERER: {
        return accessor(d_referer,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REFERER]);
      } break;
      case ATTRIBUTE_ID_TE: {
        return accessor(d_te, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TE]);
      } break;
      case ATTRIBUTE_ID_USER_AGENT: {
        return accessor(d_userAgent,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USER_AGENT]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int baenet_HttpRequestHeaderFields::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::vector<bsl::string>& baenet_HttpRequestHeaderFields::accept() const
{
    return d_accept;
}

inline
const bsl::vector<bsl::string>&
                          baenet_HttpRequestHeaderFields::acceptCharset() const
{
    return d_acceptCharset;
}

inline
const bsl::vector<bsl::string>&
                         baenet_HttpRequestHeaderFields::acceptEncoding() const
{
    return d_acceptEncoding;
}

inline
const bsl::vector<bsl::string>&
                         baenet_HttpRequestHeaderFields::acceptLanguage() const
{
    return d_acceptLanguage;
}

inline
const bdeut_NullableValue<bsl::string>&
                          baenet_HttpRequestHeaderFields::authorization() const
{
    return d_authorization;
}

inline
const bdeut_NullableValue<bsl::string>&
                                 baenet_HttpRequestHeaderFields::expect() const
{
    return d_expect;
}

inline
const bdeut_NullableValue<bsl::string>&
                                   baenet_HttpRequestHeaderFields::from() const
{
    return d_from;
}

inline
const bdeut_NullableValue<baenet_HttpHost>&
                                   baenet_HttpRequestHeaderFields::host() const
{
    return d_host;
}

inline
const bdeut_NullableValue<bsl::string>&
                                baenet_HttpRequestHeaderFields::ifMatch() const
{
    return d_ifMatch;
}

inline
const bdeut_NullableValue<bdet_DatetimeTz>&
                        baenet_HttpRequestHeaderFields::ifModifiedSince() const
{
    return d_ifModifiedSince;
}

inline
const bdeut_NullableValue<bsl::string>&
                            baenet_HttpRequestHeaderFields::ifNoneMatch() const
{
    return d_ifNoneMatch;
}

inline
const bdeut_NullableValue<bsl::string>&
                                baenet_HttpRequestHeaderFields::ifRange() const
{
    return d_ifRange;
}

inline
const bdeut_NullableValue<bdet_DatetimeTz>&
                      baenet_HttpRequestHeaderFields::ifUnmodifiedSince() const
{
    return d_ifUnmodifiedSince;
}

inline
const bdeut_NullableValue<int>&
                            baenet_HttpRequestHeaderFields::maxForwards() const
{
    return d_maxForwards;
}

inline
const bdeut_NullableValue<bsl::string>&
                     baenet_HttpRequestHeaderFields::proxyAuthorization() const
{
    return d_proxyAuthorization;
}

inline
const bdeut_NullableValue<bsl::string>&
                                  baenet_HttpRequestHeaderFields::range() const
{
    return d_range;
}

inline
const bdeut_NullableValue<bsl::string>&
                                baenet_HttpRequestHeaderFields::referer() const
{
    return d_referer;
}

inline
const bdeut_NullableValue<bsl::string>&
                                     baenet_HttpRequestHeaderFields::te() const
{
    return d_te;
}

inline
const bdeut_NullableValue<bsl::string>&
                              baenet_HttpRequestHeaderFields::userAgent() const
{
    return d_userAgent;
}


// FREE FUNCTIONS

inline
bool operator==(
        const baenet_HttpRequestHeaderFields& lhs,
        const baenet_HttpRequestHeaderFields& rhs)
{
    return  lhs.accept() == rhs.accept()
         && lhs.acceptCharset() == rhs.acceptCharset()
         && lhs.acceptEncoding() == rhs.acceptEncoding()
         && lhs.acceptLanguage() == rhs.acceptLanguage()
         && lhs.authorization() == rhs.authorization()
         && lhs.expect() == rhs.expect()
         && lhs.from() == rhs.from()
         && lhs.host() == rhs.host()
         && lhs.ifMatch() == rhs.ifMatch()
         && lhs.ifModifiedSince() == rhs.ifModifiedSince()
         && lhs.ifNoneMatch() == rhs.ifNoneMatch()
         && lhs.ifRange() == rhs.ifRange()
         && lhs.ifUnmodifiedSince() == rhs.ifUnmodifiedSince()
         && lhs.maxForwards() == rhs.maxForwards()
         && lhs.proxyAuthorization() == rhs.proxyAuthorization()
         && lhs.range() == rhs.range()
         && lhs.referer() == rhs.referer()
         && lhs.te() == rhs.te()
         && lhs.userAgent() == rhs.userAgent();
}

inline
bool operator!=(
        const baenet_HttpRequestHeaderFields& lhs,
        const baenet_HttpRequestHeaderFields& rhs)
{
    return  lhs.accept() != rhs.accept()
         || lhs.acceptCharset() != rhs.acceptCharset()
         || lhs.acceptEncoding() != rhs.acceptEncoding()
         || lhs.acceptLanguage() != rhs.acceptLanguage()
         || lhs.authorization() != rhs.authorization()
         || lhs.expect() != rhs.expect()
         || lhs.from() != rhs.from()
         || lhs.host() != rhs.host()
         || lhs.ifMatch() != rhs.ifMatch()
         || lhs.ifModifiedSince() != rhs.ifModifiedSince()
         || lhs.ifNoneMatch() != rhs.ifNoneMatch()
         || lhs.ifRange() != rhs.ifRange()
         || lhs.ifUnmodifiedSince() != rhs.ifUnmodifiedSince()
         || lhs.maxForwards() != rhs.maxForwards()
         || lhs.proxyAuthorization() != rhs.proxyAuthorization()
         || lhs.range() != rhs.range()
         || lhs.referer() != rhs.referer()
         || lhs.te() != rhs.te()
         || lhs.userAgent() != rhs.userAgent();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baenet_HttpRequestHeaderFields& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Wed Feb 10 17:14:02 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
