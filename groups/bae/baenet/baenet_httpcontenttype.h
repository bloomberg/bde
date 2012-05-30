// baenet_httpcontenttype.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAENET_HTTPCONTENTTYPE
#define INCLUDED_BAENET_HTTPCONTENTTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baenet_httpcontenttype_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@SEE_ALSO: RFC 2616
//
//@DESCRIPTION:  This component provides a value-semantic type for representing
// the HTTP Content-Type field value, as described in RFC 2616.

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

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {


                      // ================================                      
                      // class baenet_HttpContentType                      
                      // ================================                      

class baenet_HttpContentType {

    // INSTANCE DATA
    bsl::string                       d_type;
    bsl::string                       d_subType;
    bdeut_NullableValue<bsl::string>  d_charset;
    bdeut_NullableValue<bsl::string>  d_boundary;
    bdeut_NullableValue<bsl::string>  d_id;
    bdeut_NullableValue<bsl::string>  d_name;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_TYPE     = 0
      , ATTRIBUTE_ID_SUB_TYPE = 1
      , ATTRIBUTE_ID_CHARSET  = 2
      , ATTRIBUTE_ID_BOUNDARY = 3
      , ATTRIBUTE_ID_ID       = 4
      , ATTRIBUTE_ID_NAME     = 5
    };

    enum {
        NUM_ATTRIBUTES = 6
    };

    enum {
        ATTRIBUTE_INDEX_TYPE     = 0
      , ATTRIBUTE_INDEX_SUB_TYPE = 1
      , ATTRIBUTE_INDEX_CHARSET  = 2
      , ATTRIBUTE_INDEX_BOUNDARY = 3
      , ATTRIBUTE_INDEX_ID       = 4
      , ATTRIBUTE_INDEX_NAME     = 5
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
    explicit baenet_HttpContentType(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpContentType' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    baenet_HttpContentType(const baenet_HttpContentType& original,
                               bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpContentType' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~baenet_HttpContentType();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpContentType& operator=(const baenet_HttpContentType& rhs);
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

    bsl::string& type();
        // Return a reference to the modifiable "Type" attribute of this
        // object.

    bsl::string& subType();
        // Return a reference to the modifiable "SubType" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& charset();
        // Return a reference to the modifiable "Charset" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& boundary();
        // Return a reference to the modifiable "Boundary" attribute of this
        // object.

    bdeut_NullableValue<bsl::string>& id();
        // Return a reference to the modifiable "Id" attribute of this object.

    bdeut_NullableValue<bsl::string>& name();
        // Return a reference to the modifiable "Name" attribute of this
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

    const bsl::string& type() const;
        // Return a reference to the non-modifiable "Type" attribute of this
        // object.

    const bsl::string& subType() const;
        // Return a reference to the non-modifiable "SubType" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& charset() const;
        // Return a reference to the non-modifiable "Charset" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& boundary() const;
        // Return a reference to the non-modifiable "Boundary" attribute of
        // this object.

    const bdeut_NullableValue<bsl::string>& id() const;
        // Return a reference to the non-modifiable "Id" attribute of this
        // object.

    const bdeut_NullableValue<bsl::string>& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const baenet_HttpContentType& lhs,
                const baenet_HttpContentType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baenet_HttpContentType& lhs,
                const baenet_HttpContentType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const baenet_HttpContentType& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baenet_HttpContentType)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                      // --------------------------------                      
                      // class baenet_HttpContentType                      
                      // --------------------------------                      

// CLASS METHODS
inline
int baenet_HttpContentType::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& baenet_HttpContentType::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_type, 1);
            bdex_InStreamFunctions::streamIn(stream, d_subType, 1);
            bdex_InStreamFunctions::streamIn(stream, d_charset, 1);
            bdex_InStreamFunctions::streamIn(stream, d_boundary, 1);
            bdex_InStreamFunctions::streamIn(stream, d_id, 1);
            bdex_InStreamFunctions::streamIn(stream, d_name, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baenet_HttpContentType::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_subType,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SUB_TYPE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_charset,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARSET]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_boundary,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BOUNDARY]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_id, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int baenet_HttpContentType::manipulateAttribute(MANIPULATOR& manipulator,
                                                int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_TYPE: {
        return manipulator(&d_type,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
      } break;
      case ATTRIBUTE_ID_SUB_TYPE: {
        return manipulator(&d_subType,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SUB_TYPE]);
      } break;
      case ATTRIBUTE_ID_CHARSET: {
        return manipulator(&d_charset,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARSET]);
      } break;
      case ATTRIBUTE_ID_BOUNDARY: {
        return manipulator(&d_boundary,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BOUNDARY]);
      } break;
      case ATTRIBUTE_ID_ID: {
        return manipulator(&d_id, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
      } break;
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int baenet_HttpContentType::manipulateAttribute(
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
bsl::string& baenet_HttpContentType::type()
{
    return d_type;
}

inline
bsl::string& baenet_HttpContentType::subType()
{
    return d_subType;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpContentType::charset()
{
    return d_charset;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpContentType::boundary()
{
    return d_boundary;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpContentType::id()
{
    return d_id;
}

inline
bdeut_NullableValue<bsl::string>& baenet_HttpContentType::name()
{
    return d_name;
}

// ACCESSORS
template <class STREAM>
STREAM& baenet_HttpContentType::bdexStreamOut(STREAM& stream,
                                              int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_type, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_subType, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_charset, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_boundary, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_id, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_name, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int baenet_HttpContentType::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_subType, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SUB_TYPE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_charset, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARSET]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_boundary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BOUNDARY]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_id, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int baenet_HttpContentType::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_TYPE: {
        return accessor(d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
      } break;
      case ATTRIBUTE_ID_SUB_TYPE: {
        return accessor(d_subType,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SUB_TYPE]);
      } break;
      case ATTRIBUTE_ID_CHARSET: {
        return accessor(d_charset,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARSET]);
      } break;
      case ATTRIBUTE_ID_BOUNDARY: {
        return accessor(d_boundary,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BOUNDARY]);
      } break;
      case ATTRIBUTE_ID_ID: {
        return accessor(d_id, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
      } break;
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int baenet_HttpContentType::accessAttribute(
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
const bsl::string& baenet_HttpContentType::type() const
{
    return d_type;
}

inline
const bsl::string& baenet_HttpContentType::subType() const
{
    return d_subType;
}

inline
const bdeut_NullableValue<bsl::string>& baenet_HttpContentType::charset() const
{
    return d_charset;
}

inline
const bdeut_NullableValue<bsl::string>&
                                       baenet_HttpContentType::boundary() const
{
    return d_boundary;
}

inline
const bdeut_NullableValue<bsl::string>& baenet_HttpContentType::id() const
{
    return d_id;
}

inline
const bdeut_NullableValue<bsl::string>& baenet_HttpContentType::name() const
{
    return d_name;
}


// FREE FUNCTIONS

inline
bool operator==(
        const baenet_HttpContentType& lhs,
        const baenet_HttpContentType& rhs)
{
    return  lhs.type() == rhs.type()
         && lhs.subType() == rhs.subType()
         && lhs.charset() == rhs.charset()
         && lhs.boundary() == rhs.boundary()
         && lhs.id() == rhs.id()
         && lhs.name() == rhs.name();
}

inline
bool operator!=(
        const baenet_HttpContentType& lhs,
        const baenet_HttpContentType& rhs)
{
    return  lhs.type() != rhs.type()
         || lhs.subType() != rhs.subType()
         || lhs.charset() != rhs.charset()
         || lhs.boundary() != rhs.boundary()
         || lhs.id() != rhs.id()
         || lhs.name() != rhs.name();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baenet_HttpContentType& rhs)
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
