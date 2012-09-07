// baenet_httpbasicheaderfields.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAENET_HTTPBASICHEADERFIELDS
#define INCLUDED_BAENET_HTTPBASICHEADERFIELDS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baenet_httpbasicheaderfields_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@SEE_ALSO: RFC 2616
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@DESCRIPTION:  This component provides a value-semantic container for basic
// HTTP header fields.  These fields are a combination of the fields from the
// HTTP General fields and the fields from the HTTP Entity fields (as defined
// in RFC 2616).

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

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

#ifndef INCLUDED_BAENET_HTTPCONTENTTYPE
#include <baenet_httpcontenttype.h>
#endif

#ifndef INCLUDED_BAENET_HTTPREQUESTMETHOD
#include <baenet_httprequestmethod.h>
#endif

#ifndef INCLUDED_BAENET_HTTPTRANSFERENCODING
#include <baenet_httptransferencoding.h>
#endif

#ifndef INCLUDED_BAENET_HTTPVIARECORD
#include <baenet_httpviarecord.h>
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

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {


                   // ======================================                   
                   // class baenet_HttpBasicHeaderFields                   
                   // ======================================                   

class baenet_HttpBasicHeaderFields {

    // INSTANCE DATA
    bsl::vector<bsl::string>                             d_contentEncoding;
    bsl::vector<bsl::string>                             d_contentLanguage;
    bsl::vector<baenet_HttpViaRecord>                d_via;
    bdeut_NullableValue<bsl::string>                     d_cacheControl;
    bdeut_NullableValue<bsl::string>                     d_connection;
    bdeut_NullableValue<bsl::string>                     d_pragma;
    bdeut_NullableValue<bsl::string>                     d_trailer;
    bdeut_NullableValue<bsl::string>                     d_upgrade;
    bdeut_NullableValue<bsl::string>                     d_warning;
    bdeut_NullableValue<bsl::string>                     d_contentLocation;
    bdeut_NullableValue<bsl::string>                     d_contentMd5;
    bdeut_NullableValue<bsl::string>                     d_contentRange;
    bdeut_NullableValue<bdet_DatetimeTz>                 d_date;
    bdeut_NullableValue<bdet_DatetimeTz>                 d_expires;
    bdeut_NullableValue<bdet_DatetimeTz>                 d_lastModified;
    bdeut_NullableValue<baenet_HttpContentType>      d_contentType;
    bsl::vector<baenet_HttpTransferEncoding::Value>  d_transferEncoding;
    bsl::vector<baenet_HttpRequestMethod::Value>     d_allow;
    bdeut_NullableValue<int>                             d_contentLength;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_CACHE_CONTROL     = 0
      , ATTRIBUTE_ID_CONNECTION        = 1
      , ATTRIBUTE_ID_DATE              = 2
      , ATTRIBUTE_ID_PRAGMA            = 3
      , ATTRIBUTE_ID_TRAILER           = 4
      , ATTRIBUTE_ID_TRANSFER_ENCODING = 5
      , ATTRIBUTE_ID_UPGRADE           = 6
      , ATTRIBUTE_ID_VIA               = 7
      , ATTRIBUTE_ID_WARNING           = 8
      , ATTRIBUTE_ID_ALLOW             = 9
      , ATTRIBUTE_ID_CONTENT_ENCODING  = 10
      , ATTRIBUTE_ID_CONTENT_LANGUAGE  = 11
      , ATTRIBUTE_ID_CONTENT_LENGTH    = 12
      , ATTRIBUTE_ID_CONTENT_LOCATION  = 13
      , ATTRIBUTE_ID_CONTENT_MD5       = 14
      , ATTRIBUTE_ID_CONTENT_RANGE     = 15
      , ATTRIBUTE_ID_CONTENT_TYPE      = 16
      , ATTRIBUTE_ID_EXPIRES           = 17
      , ATTRIBUTE_ID_LAST_MODIFIED     = 18
    };

    enum {
        NUM_ATTRIBUTES = 19
    };

    enum {
        ATTRIBUTE_INDEX_CACHE_CONTROL     = 0
      , ATTRIBUTE_INDEX_CONNECTION        = 1
      , ATTRIBUTE_INDEX_DATE              = 2
      , ATTRIBUTE_INDEX_PRAGMA            = 3
      , ATTRIBUTE_INDEX_TRAILER           = 4
      , ATTRIBUTE_INDEX_TRANSFER_ENCODING = 5
      , ATTRIBUTE_INDEX_UPGRADE           = 6
      , ATTRIBUTE_INDEX_VIA               = 7
      , ATTRIBUTE_INDEX_WARNING           = 8
      , ATTRIBUTE_INDEX_ALLOW             = 9
      , ATTRIBUTE_INDEX_CONTENT_ENCODING  = 10
      , ATTRIBUTE_INDEX_CONTENT_LANGUAGE  = 11
      , ATTRIBUTE_INDEX_CONTENT_LENGTH    = 12
      , ATTRIBUTE_INDEX_CONTENT_LOCATION  = 13
      , ATTRIBUTE_INDEX_CONTENT_MD5       = 14
      , ATTRIBUTE_INDEX_CONTENT_RANGE     = 15
      , ATTRIBUTE_INDEX_CONTENT_TYPE      = 16
      , ATTRIBUTE_INDEX_EXPIRES           = 17
      , ATTRIBUTE_INDEX_LAST_MODIFIED     = 18
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
    explicit baenet_HttpBasicHeaderFields(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpBasicHeaderFields' having
        // the default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    baenet_HttpBasicHeaderFields(const baenet_HttpBasicHeaderFields& original,
                                     bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpBasicHeaderFields' having
        // the value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~baenet_HttpBasicHeaderFields();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpBasicHeaderFields& operator=(
                                      const baenet_HttpBasicHeaderFields& rhs);
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

    bdeut_NullableValue<bsl::string>& cacheControl();
        // Return a reference to the modifiable "CacheControl" attribute of
        // this object.

    bdeut_NullableValue<bsl::string>& connection();
        // Return a reference to the modifiable "Connection" attribute of this
        // object.

    bdeut_NullableValue<bdet_DatetimeTz>& date();
        // Return a reference to the modifiable "Date" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& pragma();
        // Return a reference to the modifiable "Pragma" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& trailer();
        // Return a reference to the modifiable "Trailer" attribute of this
        // object.

    bsl::vector<baenet_HttpTransferEncoding::Value>& transferEncoding();
        // Return a reference to the modifiable "TransferEncoding" attribute of
        // this object.

    bdeut_NullableValue<bsl::string>& upgrade();
        // Return a reference to the modifiable "Upgrade" attribute of this
        // object.

    bsl::vector<baenet_HttpViaRecord>& via();
        // Return a reference to the modifiable "Via" attribute of this object.

    bdeut_NullableValue<bsl::string>& warning();
        // Return a reference to the modifiable "Warning" attribute of this
        // object.

    bsl::vector<baenet_HttpRequestMethod::Value>& allow();
        // Return a reference to the modifiable "Allow" attribute of this
        // object.

    bsl::vector<bsl::string>& contentEncoding();
        // Return a reference to the modifiable "ContentEncoding" attribute of
        // this object.

    bsl::vector<bsl::string>& contentLanguage();
        // Return a reference to the modifiable "ContentLanguage" attribute of
        // this object.

    bdeut_NullableValue<int>& contentLength();
        // Return a reference to the modifiable "ContentLength" attribute of
        // this object.

    bdeut_NullableValue<bsl::string>& contentLocation();
        // Return a reference to the modifiable "ContentLocation" attribute of
        // this object.

    bdeut_NullableValue<bsl::string>& contentMd5();
        // Return a reference to the modifiable "ContentMd5" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& contentRange();
        // Return a reference to the modifiable "ContentRange" attribute of
        // this object.

    bdeut_NullableValue<baenet_HttpContentType>& contentType();
        // Return a reference to the modifiable "ContentType" attribute of this
        // object.

    bdeut_NullableValue<bdet_DatetimeTz>& expires();
        // Return a reference to the modifiable "Expires" attribute of this
        // object.

    bdeut_NullableValue<bdet_DatetimeTz>& lastModified();
        // Return a reference to the modifiable "LastModified" attribute of
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

    const bdeut_NullableValue<bsl::string>& cacheControl() const;
        // Return a reference to the non-modifiable "CacheControl" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& connection() const;
        // Return a reference to the non-modifiable "Connection" attribute of
        // this object.

    const bdeut_NullableValue<bdet_DatetimeTz>& date() const;
        // Return a reference to the non-modifiable "Date" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& pragma() const;
        // Return a reference to the non-modifiable "Pragma" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& trailer() const;
        // Return a reference to the non-modifiable "Trailer" attribute of this
        // object.

    const bsl::vector<baenet_HttpTransferEncoding::Value>&
                                                      transferEncoding() const;
        // Return a reference to the non-modifiable "TransferEncoding"
        // attribute of this object.

    const bdeut_NullableValue<bsl::string>& upgrade() const;
        // Return a reference to the non-modifiable "Upgrade" attribute of this
        // object.

    const bsl::vector<baenet_HttpViaRecord>& via() const;
        // Return a reference to the non-modifiable "Via" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& warning() const;
        // Return a reference to the non-modifiable "Warning" attribute of this
        // object.

    const bsl::vector<baenet_HttpRequestMethod::Value>& allow() const;
        // Return a reference to the non-modifiable "Allow" attribute of this
        // object.

    const bsl::vector<bsl::string>& contentEncoding() const;
        // Return a reference to the non-modifiable "ContentEncoding" attribute
        // of this object.

    const bsl::vector<bsl::string>& contentLanguage() const;
        // Return a reference to the non-modifiable "ContentLanguage" attribute
        // of this object.

    const bdeut_NullableValue<int>& contentLength() const;
        // Return a reference to the non-modifiable "ContentLength" attribute
        // of this object.

    const bdeut_NullableValue<bsl::string>& contentLocation() const;
        // Return a reference to the non-modifiable "ContentLocation" attribute
        // of this object.

    const bdeut_NullableValue<bsl::string>& contentMd5() const;
        // Return a reference to the non-modifiable "ContentMd5" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& contentRange() const;
        // Return a reference to the non-modifiable "ContentRange" attribute of
        // this object.

    const bdeut_NullableValue<baenet_HttpContentType>& contentType() const;
        // Return a reference to the non-modifiable "ContentType" attribute of
        // this object.

    const bdeut_NullableValue<bdet_DatetimeTz>& expires() const;
        // Return a reference to the non-modifiable "Expires" attribute of this
        // object.

    const bdeut_NullableValue<bdet_DatetimeTz>& lastModified() const;
        // Return a reference to the non-modifiable "LastModified" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const baenet_HttpBasicHeaderFields& lhs,
                const baenet_HttpBasicHeaderFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baenet_HttpBasicHeaderFields& lhs,
                const baenet_HttpBasicHeaderFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const baenet_HttpBasicHeaderFields& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baenet_HttpBasicHeaderFields)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                   // --------------------------------------                   
                   // class baenet_HttpBasicHeaderFields                   
                   // --------------------------------------                   

// CLASS METHODS
inline
int baenet_HttpBasicHeaderFields::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& baenet_HttpBasicHeaderFields::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_cacheControl, 1);
            bdex_InStreamFunctions::streamIn(stream, d_connection, 1);
            bdex_InStreamFunctions::streamIn(stream, d_date, 1);
            bdex_InStreamFunctions::streamIn(stream, d_pragma, 1);
            bdex_InStreamFunctions::streamIn(stream, d_trailer, 1);
            bdex_InStreamFunctions::streamIn(stream, d_transferEncoding, 1);
            bdex_InStreamFunctions::streamIn(stream, d_upgrade, 1);
            bdex_InStreamFunctions::streamIn(stream, d_via, 1);
            bdex_InStreamFunctions::streamIn(stream, d_warning, 1);
            bdex_InStreamFunctions::streamIn(stream, d_allow, 1);
            bdex_InStreamFunctions::streamIn(stream, d_contentEncoding, 1);
            bdex_InStreamFunctions::streamIn(stream, d_contentLanguage, 1);
            bdex_InStreamFunctions::streamIn(stream, d_contentLength, 1);
            bdex_InStreamFunctions::streamIn(stream, d_contentLocation, 1);
            bdex_InStreamFunctions::streamIn(stream, d_contentMd5, 1);
            bdex_InStreamFunctions::streamIn(stream, d_contentRange, 1);
            bdex_InStreamFunctions::streamIn(stream, d_contentType, 1);
            bdex_InStreamFunctions::streamIn(stream, d_expires, 1);
            bdex_InStreamFunctions::streamIn(stream, d_lastModified, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baenet_HttpBasicHeaderFields::manipulateAttributes(
                                                      MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_cacheControl,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CACHE_CONTROL]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_connection,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONNECTION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_date, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_pragma, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_trailer,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRAILER]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_transferEncoding,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSFER_ENCODING]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_upgrade,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UPGRADE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_via, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VIA]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_warning,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WARNING]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_allow, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_contentEncoding,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_ENCODING]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_contentLanguage,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LANGUAGE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_contentLength,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LENGTH]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_contentLocation,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LOCATION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_contentMd5,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_MD5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_contentRange,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_RANGE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_contentType,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_TYPE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_expires,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPIRES]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_lastModified,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_MODIFIED]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int baenet_HttpBasicHeaderFields::manipulateAttribute(MANIPULATOR& manipulator,
                                                      int          id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CACHE_CONTROL: {
        return manipulator(&d_cacheControl,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CACHE_CONTROL]);
      } break;
      case ATTRIBUTE_ID_CONNECTION: {
        return manipulator(&d_connection,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONNECTION]);
      } break;
      case ATTRIBUTE_ID_DATE: {
        return manipulator(&d_date,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE]);
      } break;
      case ATTRIBUTE_ID_PRAGMA: {
        return manipulator(&d_pragma,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA]);
      } break;
      case ATTRIBUTE_ID_TRAILER: {
        return manipulator(&d_trailer,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRAILER]);
      } break;
      case ATTRIBUTE_ID_TRANSFER_ENCODING: {
        return manipulator(&d_transferEncoding,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSFER_ENCODING]);
      } break;
      case ATTRIBUTE_ID_UPGRADE: {
        return manipulator(&d_upgrade,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UPGRADE]);
      } break;
      case ATTRIBUTE_ID_VIA: {
        return manipulator(&d_via, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VIA]);
      } break;
      case ATTRIBUTE_ID_WARNING: {
        return manipulator(&d_warning,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WARNING]);
      } break;
      case ATTRIBUTE_ID_ALLOW: {
        return manipulator(&d_allow,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW]);
      } break;
      case ATTRIBUTE_ID_CONTENT_ENCODING: {
        return manipulator(&d_contentEncoding,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_ENCODING]);
      } break;
      case ATTRIBUTE_ID_CONTENT_LANGUAGE: {
        return manipulator(&d_contentLanguage,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LANGUAGE]);
      } break;
      case ATTRIBUTE_ID_CONTENT_LENGTH: {
        return manipulator(&d_contentLength,
                         ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LENGTH]);
      } break;
      case ATTRIBUTE_ID_CONTENT_LOCATION: {
        return manipulator(&d_contentLocation,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LOCATION]);
      } break;
      case ATTRIBUTE_ID_CONTENT_MD5: {
        return manipulator(&d_contentMd5,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_MD5]);
      } break;
      case ATTRIBUTE_ID_CONTENT_RANGE: {
        return manipulator(&d_contentRange,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_RANGE]);
      } break;
      case ATTRIBUTE_ID_CONTENT_TYPE: {
        return manipulator(&d_contentType,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_TYPE]);
      } break;
      case ATTRIBUTE_ID_EXPIRES: {
        return manipulator(&d_expires,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPIRES]);
      } break;
      case ATTRIBUTE_ID_LAST_MODIFIED: {
        return manipulator(&d_lastModified,
                          ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_MODIFIED]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int baenet_HttpBasicHeaderFields::manipulateAttribute(
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
bdeut_NullableValue<bsl::string>& baenet_HttpBasicHeaderFields::cacheControl()
{
    return d_cacheControl;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpBasicHeaderFields::connection()
{
    return d_connection;
}

inline
bdeut_NullableValue<bdet_DatetimeTz>& baenet_HttpBasicHeaderFields::date()
{
    return d_date;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpBasicHeaderFields::pragma()
{
    return d_pragma;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpBasicHeaderFields::trailer()
{
    return d_trailer;
}

inline
bsl::vector<baenet_HttpTransferEncoding::Value>&
                               baenet_HttpBasicHeaderFields::transferEncoding()
{
    return d_transferEncoding;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpBasicHeaderFields::upgrade()
{
    return d_upgrade;
}

inline
bsl::vector<baenet_HttpViaRecord>& baenet_HttpBasicHeaderFields::via()
{
    return d_via;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpBasicHeaderFields::warning()
{
    return d_warning;
}

inline
bsl::vector<baenet_HttpRequestMethod::Value>&
                                          baenet_HttpBasicHeaderFields::allow()
{
    return d_allow;
}

inline
bsl::vector<bsl::string>& baenet_HttpBasicHeaderFields::contentEncoding()
{
    return d_contentEncoding;
}

inline
bsl::vector<bsl::string>& baenet_HttpBasicHeaderFields::contentLanguage()
{
    return d_contentLanguage;
}

inline
bdeut_NullableValue<int>& baenet_HttpBasicHeaderFields::contentLength()
{
    return d_contentLength;
}

inline
bdeut_NullableValue<bsl::string>&
                                baenet_HttpBasicHeaderFields::contentLocation()
{
    return d_contentLocation;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpBasicHeaderFields::contentMd5()
{
    return d_contentMd5;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpBasicHeaderFields::contentRange()
{
    return d_contentRange;
}

inline
bdeut_NullableValue<baenet_HttpContentType>&
                                   baenet_HttpBasicHeaderFields::contentType()
{
    return d_contentType;
}

inline
bdeut_NullableValue<bdet_DatetimeTz>& baenet_HttpBasicHeaderFields::expires()
{
    return d_expires;
}

inline
bdeut_NullableValue<bdet_DatetimeTz>&
                                   baenet_HttpBasicHeaderFields::lastModified()
{
    return d_lastModified;
}

// ACCESSORS
template <class STREAM>
STREAM& baenet_HttpBasicHeaderFields::bdexStreamOut(STREAM& stream,
                                                    int     version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_cacheControl, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_connection, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_date, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_pragma, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_trailer, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_transferEncoding, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_upgrade, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_via, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_warning, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_allow, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_contentEncoding, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_contentLanguage, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_contentLength, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_contentLocation, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_contentMd5, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_contentRange, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_contentType, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_expires, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_lastModified, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int baenet_HttpBasicHeaderFields::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_cacheControl,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CACHE_CONTROL]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_connection,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONNECTION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_date, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_pragma, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_trailer, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRAILER]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_transferEncoding,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSFER_ENCODING]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_upgrade, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UPGRADE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_via, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VIA]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_warning, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WARNING]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_allow, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_contentEncoding,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_ENCODING]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_contentLanguage,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LANGUAGE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_contentLength,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LENGTH]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_contentLocation,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LOCATION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_contentMd5,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_MD5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_contentRange,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_RANGE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_contentType,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_TYPE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_expires, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPIRES]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_lastModified,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_MODIFIED]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int baenet_HttpBasicHeaderFields::accessAttribute(ACCESSOR& accessor,
                                                  int       id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CACHE_CONTROL: {
        return accessor(d_cacheControl,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CACHE_CONTROL]);
      } break;
      case ATTRIBUTE_ID_CONNECTION: {
        return accessor(d_connection,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONNECTION]);
      } break;
      case ATTRIBUTE_ID_DATE: {
        return accessor(d_date, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATE]);
      } break;
      case ATTRIBUTE_ID_PRAGMA: {
        return accessor(d_pragma,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA]);
      } break;
      case ATTRIBUTE_ID_TRAILER: {
        return accessor(d_trailer,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRAILER]);
      } break;
      case ATTRIBUTE_ID_TRANSFER_ENCODING: {
        return accessor(d_transferEncoding,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSFER_ENCODING]);
      } break;
      case ATTRIBUTE_ID_UPGRADE: {
        return accessor(d_upgrade,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UPGRADE]);
      } break;
      case ATTRIBUTE_ID_VIA: {
        return accessor(d_via,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VIA]);
      } break;
      case ATTRIBUTE_ID_WARNING: {
        return accessor(d_warning,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WARNING]);
      } break;
      case ATTRIBUTE_ID_ALLOW: {
        return accessor(d_allow, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOW]);
      } break;
      case ATTRIBUTE_ID_CONTENT_ENCODING: {
        return accessor(d_contentEncoding,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_ENCODING]);
      } break;
      case ATTRIBUTE_ID_CONTENT_LANGUAGE: {
        return accessor(d_contentLanguage,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LANGUAGE]);
      } break;
      case ATTRIBUTE_ID_CONTENT_LENGTH: {
        return accessor(d_contentLength,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LENGTH]);
      } break;
      case ATTRIBUTE_ID_CONTENT_LOCATION: {
        return accessor(d_contentLocation,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_LOCATION]);
      } break;
      case ATTRIBUTE_ID_CONTENT_MD5: {
        return accessor(d_contentMd5,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_MD5]);
      } break;
      case ATTRIBUTE_ID_CONTENT_RANGE: {
        return accessor(d_contentRange,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_RANGE]);
      } break;
      case ATTRIBUTE_ID_CONTENT_TYPE: {
        return accessor(d_contentType,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTENT_TYPE]);
      } break;
      case ATTRIBUTE_ID_EXPIRES: {
        return accessor(d_expires,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_EXPIRES]);
      } break;
      case ATTRIBUTE_ID_LAST_MODIFIED: {
        return accessor(d_lastModified,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LAST_MODIFIED]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int baenet_HttpBasicHeaderFields::accessAttribute(
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
                             baenet_HttpBasicHeaderFields::cacheControl() const
{
    return d_cacheControl;
}

inline
const bdeut_NullableValue<bsl::string>&
                               baenet_HttpBasicHeaderFields::connection() const
{
    return d_connection;
}

inline
const bdeut_NullableValue<bdet_DatetimeTz>&
                                     baenet_HttpBasicHeaderFields::date() const
{
    return d_date;
}

inline
const bdeut_NullableValue<bsl::string>&
                                   baenet_HttpBasicHeaderFields::pragma() const
{
    return d_pragma;
}

inline
const bdeut_NullableValue<bsl::string>&
                                  baenet_HttpBasicHeaderFields::trailer() const
{
    return d_trailer;
}

inline
const bsl::vector<baenet_HttpTransferEncoding::Value>&
                         baenet_HttpBasicHeaderFields::transferEncoding() const
{
    return d_transferEncoding;
}

inline
const bdeut_NullableValue<bsl::string>&
                                  baenet_HttpBasicHeaderFields::upgrade() const
{
    return d_upgrade;
}

inline
const bsl::vector<baenet_HttpViaRecord>&
                                      baenet_HttpBasicHeaderFields::via() const
{
    return d_via;
}

inline
const bdeut_NullableValue<bsl::string>&
                                  baenet_HttpBasicHeaderFields::warning() const
{
    return d_warning;
}

inline
const bsl::vector<baenet_HttpRequestMethod::Value>&
                                    baenet_HttpBasicHeaderFields::allow() const
{
    return d_allow;
}

inline
const bsl::vector<bsl::string>&
                          baenet_HttpBasicHeaderFields::contentEncoding() const
{
    return d_contentEncoding;
}

inline
const bsl::vector<bsl::string>&
                          baenet_HttpBasicHeaderFields::contentLanguage() const
{
    return d_contentLanguage;
}

inline
const bdeut_NullableValue<int>&
                            baenet_HttpBasicHeaderFields::contentLength() const
{
    return d_contentLength;
}

inline
const bdeut_NullableValue<bsl::string>&
                          baenet_HttpBasicHeaderFields::contentLocation() const
{
    return d_contentLocation;
}

inline
const bdeut_NullableValue<bsl::string>&
                               baenet_HttpBasicHeaderFields::contentMd5() const
{
    return d_contentMd5;
}

inline
const bdeut_NullableValue<bsl::string>&
                             baenet_HttpBasicHeaderFields::contentRange() const
{
    return d_contentRange;
}

inline
const bdeut_NullableValue<baenet_HttpContentType>&
                              baenet_HttpBasicHeaderFields::contentType() const
{
    return d_contentType;
}

inline
const bdeut_NullableValue<bdet_DatetimeTz>&
                                  baenet_HttpBasicHeaderFields::expires() const
{
    return d_expires;
}

inline
const bdeut_NullableValue<bdet_DatetimeTz>&
                             baenet_HttpBasicHeaderFields::lastModified() const
{
    return d_lastModified;
}


// FREE FUNCTIONS

inline
bool operator==(
        const baenet_HttpBasicHeaderFields& lhs,
        const baenet_HttpBasicHeaderFields& rhs)
{
    return  lhs.cacheControl() == rhs.cacheControl()
         && lhs.connection() == rhs.connection()
         && lhs.date() == rhs.date()
         && lhs.pragma() == rhs.pragma()
         && lhs.trailer() == rhs.trailer()
         && lhs.transferEncoding() == rhs.transferEncoding()
         && lhs.upgrade() == rhs.upgrade()
         && lhs.via() == rhs.via()
         && lhs.warning() == rhs.warning()
         && lhs.allow() == rhs.allow()
         && lhs.contentEncoding() == rhs.contentEncoding()
         && lhs.contentLanguage() == rhs.contentLanguage()
         && lhs.contentLength() == rhs.contentLength()
         && lhs.contentLocation() == rhs.contentLocation()
         && lhs.contentMd5() == rhs.contentMd5()
         && lhs.contentRange() == rhs.contentRange()
         && lhs.contentType() == rhs.contentType()
         && lhs.expires() == rhs.expires()
         && lhs.lastModified() == rhs.lastModified();
}

inline
bool operator!=(
        const baenet_HttpBasicHeaderFields& lhs,
        const baenet_HttpBasicHeaderFields& rhs)
{
    return  lhs.cacheControl() != rhs.cacheControl()
         || lhs.connection() != rhs.connection()
         || lhs.date() != rhs.date()
         || lhs.pragma() != rhs.pragma()
         || lhs.trailer() != rhs.trailer()
         || lhs.transferEncoding() != rhs.transferEncoding()
         || lhs.upgrade() != rhs.upgrade()
         || lhs.via() != rhs.via()
         || lhs.warning() != rhs.warning()
         || lhs.allow() != rhs.allow()
         || lhs.contentEncoding() != rhs.contentEncoding()
         || lhs.contentLanguage() != rhs.contentLanguage()
         || lhs.contentLength() != rhs.contentLength()
         || lhs.contentLocation() != rhs.contentLocation()
         || lhs.contentMd5() != rhs.contentMd5()
         || lhs.contentRange() != rhs.contentRange()
         || lhs.contentType() != rhs.contentType()
         || lhs.expires() != rhs.expires()
         || lhs.lastModified() != rhs.lastModified();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baenet_HttpBasicHeaderFields& rhs)
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
