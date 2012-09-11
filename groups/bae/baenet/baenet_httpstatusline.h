// baenet_httpstatusline.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAENET_HTTPSTATUSLINE
#define INCLUDED_BAENET_HTTPSTATUSLINE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baenet_httpstatusline_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)

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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif
#define INCLUDED_BSL_IOSFWD
#endif

namespace BloombergLP {


                            // ===========================
                            // class baenet_HttpStatusLine
                            // ===========================

class baenet_HttpStatusLine {

    // INSTANCE DATA
    bsl::string  d_reasonPhrase;
    int          d_majorVersion;
    int          d_minorVersion;
    int          d_statusCode;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_MAJOR_VERSION = 0
      , ATTRIBUTE_ID_MINOR_VERSION = 1
      , ATTRIBUTE_ID_STATUS_CODE   = 2
      , ATTRIBUTE_ID_REASON_PHRASE = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_MAJOR_VERSION = 0
      , ATTRIBUTE_INDEX_MINOR_VERSION = 1
      , ATTRIBUTE_INDEX_STATUS_CODE   = 2
      , ATTRIBUTE_INDEX_REASON_PHRASE = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_MAJOR_VERSION;

    static const int DEFAULT_INITIALIZER_MINOR_VERSION;

    static const int DEFAULT_INITIALIZER_STATUS_CODE;

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
    explicit baenet_HttpStatusLine(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpStatusLine' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    baenet_HttpStatusLine(const baenet_HttpStatusLine&  original,
                          bslma_Allocator              *basicAllocator = 0);
        // Create an object of type 'baenet_HttpStatusLine' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~baenet_HttpStatusLine();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpStatusLine& operator=(const baenet_HttpStatusLine& rhs);
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

    int& majorVersion();
        // Return a reference to the modifiable "MajorVersion" attribute of
        // this object.

    int& minorVersion();
        // Return a reference to the modifiable "MinorVersion" attribute of
        // this object.

    int& statusCode();
        // Return a reference to the modifiable "StatusCode" attribute of this
        // object.

    bsl::string& reasonPhrase();
        // Return a reference to the modifiable "ReasonPhrase" attribute of
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

    int majorVersion() const;
        // Return a reference to the non-modifiable "MajorVersion" attribute of
        // this object.

    int minorVersion() const;
        // Return a reference to the non-modifiable "MinorVersion" attribute of
        // this object.

    int statusCode() const;
        // Return a reference to the non-modifiable "StatusCode" attribute of
        // this object.

    const bsl::string& reasonPhrase() const;
        // Return a reference to the non-modifiable "ReasonPhrase" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const baenet_HttpStatusLine& lhs,
                const baenet_HttpStatusLine& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baenet_HttpStatusLine& lhs,
                const baenet_HttpStatusLine& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baenet_HttpStatusLine& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baenet_HttpStatusLine)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                            // ---------------------------
                            // class baenet_HttpStatusLine
                            // ---------------------------

// CLASS METHODS
inline
int baenet_HttpStatusLine::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& baenet_HttpStatusLine::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_majorVersion, 1);
            bdex_InStreamFunctions::streamIn(stream, d_minorVersion, 1);
            bdex_InStreamFunctions::streamIn(stream, d_statusCode, 1);
            bdex_InStreamFunctions::streamIn(stream, d_reasonPhrase, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baenet_HttpStatusLine::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_majorVersion, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAJOR_VERSION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_minorVersion, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MINOR_VERSION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_statusCode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS_CODE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_reasonPhrase, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REASON_PHRASE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int baenet_HttpStatusLine::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAJOR_VERSION: {
        return manipulator(&d_majorVersion, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAJOR_VERSION]);
      } break;
      case ATTRIBUTE_ID_MINOR_VERSION: {
        return manipulator(&d_minorVersion, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MINOR_VERSION]);
      } break;
      case ATTRIBUTE_ID_STATUS_CODE: {
        return manipulator(&d_statusCode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS_CODE]);
      } break;
      case ATTRIBUTE_ID_REASON_PHRASE: {
        return manipulator(&d_reasonPhrase, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REASON_PHRASE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int baenet_HttpStatusLine::manipulateAttribute(
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
int& baenet_HttpStatusLine::majorVersion()
{
    return d_majorVersion;
}

inline
int& baenet_HttpStatusLine::minorVersion()
{
    return d_minorVersion;
}

inline
int& baenet_HttpStatusLine::statusCode()
{
    return d_statusCode;
}

inline
bsl::string& baenet_HttpStatusLine::reasonPhrase()
{
    return d_reasonPhrase;
}

// ACCESSORS
template <class STREAM>
STREAM& baenet_HttpStatusLine::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_majorVersion, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_minorVersion, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_statusCode, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_reasonPhrase, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int baenet_HttpStatusLine::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_majorVersion, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAJOR_VERSION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_minorVersion, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MINOR_VERSION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_statusCode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS_CODE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_reasonPhrase, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REASON_PHRASE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int baenet_HttpStatusLine::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAJOR_VERSION: {
        return accessor(d_majorVersion, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAJOR_VERSION]);
      } break;
      case ATTRIBUTE_ID_MINOR_VERSION: {
        return accessor(d_minorVersion, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MINOR_VERSION]);
      } break;
      case ATTRIBUTE_ID_STATUS_CODE: {
        return accessor(d_statusCode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATUS_CODE]);
      } break;
      case ATTRIBUTE_ID_REASON_PHRASE: {
        return accessor(d_reasonPhrase, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REASON_PHRASE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int baenet_HttpStatusLine::accessAttribute(
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
int baenet_HttpStatusLine::majorVersion() const
{
    return d_majorVersion;
}

inline
int baenet_HttpStatusLine::minorVersion() const
{
    return d_minorVersion;
}

inline
int baenet_HttpStatusLine::statusCode() const
{
    return d_statusCode;
}

inline
const bsl::string& baenet_HttpStatusLine::reasonPhrase() const
{
    return d_reasonPhrase;
}


// FREE FUNCTIONS

inline
bool operator==(
        const baenet_HttpStatusLine& lhs,
        const baenet_HttpStatusLine& rhs)
{
    return  lhs.majorVersion() == rhs.majorVersion()
         && lhs.minorVersion() == rhs.minorVersion()
         && lhs.statusCode() == rhs.statusCode()
         && lhs.reasonPhrase() == rhs.reasonPhrase();
}

inline
bool operator!=(
        const baenet_HttpStatusLine& lhs,
        const baenet_HttpStatusLine& rhs)
{
    return  lhs.majorVersion() != rhs.majorVersion()
         || lhs.minorVersion() != rhs.minorVersion()
         || lhs.statusCode() != rhs.statusCode()
         || lhs.reasonPhrase() != rhs.reasonPhrase();
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baenet_HttpStatusLine& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.6.2 Fri Feb  4 15:08:20 2011
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
