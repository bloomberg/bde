// baenet_httpresponseheaderfields.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAENET_HTTPRESPONSEHEADERFIELDS
#define INCLUDED_BAENET_HTTPRESPONSEHEADERFIELDS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baenet_httpresponseheaderfields_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@SEE_ALSO: RFC 2616
//
//@DESCRIPTION:  This component provides a value-semantic container for HTTP
// response header fields, as defined in RFC 2616.

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

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bslma_Allocator;


                 // =========================================                  
                 // class baenet_HttpResponseHeaderFields                  
                 // =========================================                  

class baenet_HttpResponseHeaderFields {

    // INSTANCE DATA
    bdeut_NullableValue<bsl::string>  d_acceptRanges;
    bdeut_NullableValue<bsl::string>  d_eTag;
    bdeut_NullableValue<bsl::string>  d_location;
    bdeut_NullableValue<bsl::string>  d_proxyAuthenticate;
    bdeut_NullableValue<bsl::string>  d_retryAfter;
    bdeut_NullableValue<bsl::string>  d_server;
    bdeut_NullableValue<bsl::string>  d_vary;
    bdeut_NullableValue<bsl::string>  d_wwwAuthenticate;
    bdeut_NullableValue<int>          d_age;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ACCEPT_RANGES      = 0
      , ATTRIBUTE_ID_AGE                = 1
      , ATTRIBUTE_ID_E_TAG              = 2
      , ATTRIBUTE_ID_LOCATION           = 3
      , ATTRIBUTE_ID_PROXY_AUTHENTICATE = 4
      , ATTRIBUTE_ID_RETRY_AFTER        = 5
      , ATTRIBUTE_ID_SERVER             = 6
      , ATTRIBUTE_ID_VARY               = 7
      , ATTRIBUTE_ID_WWW_AUTHENTICATE   = 8
    };

    enum {
        NUM_ATTRIBUTES = 9
    };

    enum {
        ATTRIBUTE_INDEX_ACCEPT_RANGES      = 0
      , ATTRIBUTE_INDEX_AGE                = 1
      , ATTRIBUTE_INDEX_E_TAG              = 2
      , ATTRIBUTE_INDEX_LOCATION           = 3
      , ATTRIBUTE_INDEX_PROXY_AUTHENTICATE = 4
      , ATTRIBUTE_INDEX_RETRY_AFTER        = 5
      , ATTRIBUTE_INDEX_SERVER             = 6
      , ATTRIBUTE_INDEX_VARY               = 7
      , ATTRIBUTE_INDEX_WWW_AUTHENTICATE   = 8
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
    explicit baenet_HttpResponseHeaderFields(
                                          bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpResponseHeaderFields' having
        // the default value.  Use the optionally specified 'basicAllocator'
        // to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    baenet_HttpResponseHeaderFields(
                const baenet_HttpResponseHeaderFields&  original,
                bslma_Allocator                        *basicAllocator = 0);
        // Create an object of type 'baenet_HttpResponseHeaderFields' having
        // the value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator'
        // is 0, the currently installed default allocator is used.

    ~baenet_HttpResponseHeaderFields();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpResponseHeaderFields& operator=(
                                   const baenet_HttpResponseHeaderFields& rhs);
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

    bdeut_NullableValue<bsl::string>& acceptRanges();
        // Return a reference to the modifiable "AcceptRanges" attribute of
        // this object.

    bdeut_NullableValue<int>& age();
        // Return a reference to the modifiable "Age" attribute of this object.

    bdeut_NullableValue<bsl::string>& eTag();
        // Return a reference to the modifiable "ETag" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& location();
        // Return a reference to the modifiable "Location" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& proxyAuthenticate();
        // Return a reference to the modifiable "ProxyAuthenticate" attribute
        // of this object.

    bdeut_NullableValue<bsl::string>& retryAfter();
        // Return a reference to the modifiable "RetryAfter" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& server();
        // Return a reference to the modifiable "Server" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& vary();
        // Return a reference to the modifiable "Vary" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& wwwAuthenticate();
        // Return a reference to the modifiable "WwwAuthenticate" attribute of
        // this object.

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

    const bdeut_NullableValue<bsl::string>& acceptRanges() const;
        // Return a reference to the non-modifiable "AcceptRanges" attribute of
        // this object.

    const bdeut_NullableValue<int>& age() const;
        // Return a reference to the non-modifiable "Age" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& eTag() const;
        // Return a reference to the non-modifiable "ETag" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& location() const;
        // Return a reference to the non-modifiable "Location" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& proxyAuthenticate() const;
        // Return a reference to the non-modifiable "ProxyAuthenticate"
        // attribute of this object.

    const bdeut_NullableValue<bsl::string>& retryAfter() const;
        // Return a reference to the non-modifiable "RetryAfter" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& server() const;
        // Return a reference to the non-modifiable "Server" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& vary() const;
        // Return a reference to the non-modifiable "Vary" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& wwwAuthenticate() const;
        // Return a reference to the non-modifiable "WwwAuthenticate" attribute
        // of this object.
};

// FREE OPERATORS
inline
bool operator==(const baenet_HttpResponseHeaderFields& lhs,
                const baenet_HttpResponseHeaderFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baenet_HttpResponseHeaderFields& lhs,
                const baenet_HttpResponseHeaderFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const baenet_HttpResponseHeaderFields& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baenet_HttpResponseHeaderFields)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                 // -----------------------------------------                  
                 // class baenet_HttpResponseHeaderFields                  
                 // -----------------------------------------                  

// CLASS METHODS
inline
int baenet_HttpResponseHeaderFields::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& baenet_HttpResponseHeaderFields::bdexStreamIn(STREAM&  stream,
                                                      int      version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_acceptRanges, 1);
            bdex_InStreamFunctions::streamIn(stream, d_age, 1);
            bdex_InStreamFunctions::streamIn(stream, d_eTag, 1);
            bdex_InStreamFunctions::streamIn(stream, d_location, 1);
            bdex_InStreamFunctions::streamIn(stream, d_proxyAuthenticate, 1);
            bdex_InStreamFunctions::streamIn(stream, d_retryAfter, 1);
            bdex_InStreamFunctions::streamIn(stream, d_server, 1);
            bdex_InStreamFunctions::streamIn(stream, d_vary, 1);
            bdex_InStreamFunctions::streamIn(stream, d_wwwAuthenticate, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baenet_HttpResponseHeaderFields::manipulateAttributes(
                                                      MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_acceptRanges,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_RANGES]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_eTag, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_E_TAG]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_location,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LOCATION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_proxyAuthenticate,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHENTICATE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_retryAfter,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RETRY_AFTER]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_server, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_vary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VARY]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_wwwAuthenticate,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WWW_AUTHENTICATE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int baenet_HttpResponseHeaderFields::manipulateAttribute(
                            MANIPULATOR&  manipulator,
                            int           id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ACCEPT_RANGES: {
        return manipulator(&d_acceptRanges,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_RANGES]);
      } break;
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
      } break;
      case ATTRIBUTE_ID_E_TAG: {
        return manipulator(&d_eTag,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_E_TAG]);
      } break;
      case ATTRIBUTE_ID_LOCATION: {
        return manipulator(&d_location,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LOCATION]);
      } break;
      case ATTRIBUTE_ID_PROXY_AUTHENTICATE: {
        return manipulator(&d_proxyAuthenticate,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHENTICATE]);
      } break;
      case ATTRIBUTE_ID_RETRY_AFTER: {
        return manipulator(&d_retryAfter,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RETRY_AFTER]);
      } break;
      case ATTRIBUTE_ID_SERVER: {
        return manipulator(&d_server,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER]);
      } break;
      case ATTRIBUTE_ID_VARY: {
        return manipulator(&d_vary,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VARY]);
      } break;
      case ATTRIBUTE_ID_WWW_AUTHENTICATE: {
        return manipulator(&d_wwwAuthenticate,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WWW_AUTHENTICATE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int baenet_HttpResponseHeaderFields::manipulateAttribute(
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
bdeut_NullableValue<bsl::string>&
                                baenet_HttpResponseHeaderFields::acceptRanges()
{
    return d_acceptRanges;
}

inline
bdeut_NullableValue<int>& baenet_HttpResponseHeaderFields::age()
{
    return d_age;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpResponseHeaderFields::eTag()
{
    return d_eTag;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpResponseHeaderFields::location()
{
    return d_location;
}

inline
bdeut_NullableValue<bsl::string>&
                           baenet_HttpResponseHeaderFields::proxyAuthenticate()
{
    return d_proxyAuthenticate;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpResponseHeaderFields::retryAfter()
{
    return d_retryAfter;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpResponseHeaderFields::server()
{
    return d_server;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpResponseHeaderFields::vary()
{
    return d_vary;
}

inline
bdeut_NullableValue<bsl::string>&
                             baenet_HttpResponseHeaderFields::wwwAuthenticate()
{
    return d_wwwAuthenticate;
}

// ACCESSORS
template <class STREAM>
STREAM& baenet_HttpResponseHeaderFields::bdexStreamOut(STREAM&  stream,
                                                       int      version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_acceptRanges, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_age, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_eTag, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_location, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_proxyAuthenticate, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_retryAfter, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_server, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_vary, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_wwwAuthenticate, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int baenet_HttpResponseHeaderFields::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_acceptRanges,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_RANGES]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_eTag, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_E_TAG]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_location, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LOCATION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_proxyAuthenticate,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHENTICATE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_retryAfter,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RETRY_AFTER]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_server, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_vary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VARY]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_wwwAuthenticate,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WWW_AUTHENTICATE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int baenet_HttpResponseHeaderFields::accessAttribute(ACCESSOR&  accessor,
                                                     int        id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ACCEPT_RANGES: {
        return accessor(d_acceptRanges,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_RANGES]);
      } break;
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
      } break;
      case ATTRIBUTE_ID_E_TAG: {
        return accessor(d_eTag, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_E_TAG]);
      } break;
      case ATTRIBUTE_ID_LOCATION: {
        return accessor(d_location,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LOCATION]);
      } break;
      case ATTRIBUTE_ID_PROXY_AUTHENTICATE: {
        return accessor(d_proxyAuthenticate,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PROXY_AUTHENTICATE]);
      } break;
      case ATTRIBUTE_ID_RETRY_AFTER: {
        return accessor(d_retryAfter,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RETRY_AFTER]);
      } break;
      case ATTRIBUTE_ID_SERVER: {
        return accessor(d_server,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER]);
      } break;
      case ATTRIBUTE_ID_VARY: {
        return accessor(d_vary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VARY]);
      } break;
      case ATTRIBUTE_ID_WWW_AUTHENTICATE: {
        return accessor(d_wwwAuthenticate,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WWW_AUTHENTICATE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int baenet_HttpResponseHeaderFields::accessAttribute(
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
const bdeut_NullableValue<bsl::string>&
                          baenet_HttpResponseHeaderFields::acceptRanges() const
{
    return d_acceptRanges;
}

inline
const bdeut_NullableValue<int>& baenet_HttpResponseHeaderFields::age() const
{
    return d_age;
}

inline
const bdeut_NullableValue<bsl::string>&
                                  baenet_HttpResponseHeaderFields::eTag() const
{
    return d_eTag;
}

inline
const bdeut_NullableValue<bsl::string>&
                              baenet_HttpResponseHeaderFields::location() const
{
    return d_location;
}

inline
const bdeut_NullableValue<bsl::string>&
                     baenet_HttpResponseHeaderFields::proxyAuthenticate() const
{
    return d_proxyAuthenticate;
}

inline
const bdeut_NullableValue<bsl::string>&
                            baenet_HttpResponseHeaderFields::retryAfter() const
{
    return d_retryAfter;
}

inline
const bdeut_NullableValue<bsl::string>&
                                baenet_HttpResponseHeaderFields::server() const
{
    return d_server;
}

inline
const bdeut_NullableValue<bsl::string>&
                                  baenet_HttpResponseHeaderFields::vary() const
{
    return d_vary;
}

inline
const bdeut_NullableValue<bsl::string>&
                       baenet_HttpResponseHeaderFields::wwwAuthenticate() const
{
    return d_wwwAuthenticate;
}


// FREE FUNCTIONS

inline
bool operator==(
        const baenet_HttpResponseHeaderFields& lhs,
        const baenet_HttpResponseHeaderFields& rhs)
{
    return  lhs.acceptRanges() == rhs.acceptRanges()
         && lhs.age() == rhs.age()
         && lhs.eTag() == rhs.eTag()
         && lhs.location() == rhs.location()
         && lhs.proxyAuthenticate() == rhs.proxyAuthenticate()
         && lhs.retryAfter() == rhs.retryAfter()
         && lhs.server() == rhs.server()
         && lhs.vary() == rhs.vary()
         && lhs.wwwAuthenticate() == rhs.wwwAuthenticate();
}

inline
bool operator!=(
        const baenet_HttpResponseHeaderFields& lhs,
        const baenet_HttpResponseHeaderFields& rhs)
{
    return  lhs.acceptRanges() != rhs.acceptRanges()
         || lhs.age() != rhs.age()
         || lhs.eTag() != rhs.eTag()
         || lhs.location() != rhs.location()
         || lhs.proxyAuthenticate() != rhs.proxyAuthenticate()
         || lhs.retryAfter() != rhs.retryAfter()
         || lhs.server() != rhs.server()
         || lhs.vary() != rhs.vary()
         || lhs.wwwAuthenticate() != rhs.wwwAuthenticate();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baenet_HttpResponseHeaderFields& rhs)
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
