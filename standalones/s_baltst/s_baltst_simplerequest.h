// s_baltst_simplerequest.h         *DO NOT EDIT*          @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_SIMPLEREQUEST
#define INCLUDED_S_BALTST_SIMPLEREQUEST

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_simplerequest_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_string.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class SimpleRequest; }
namespace s_baltst {

                            // ===================
                            // class SimpleRequest
                            // ===================

class SimpleRequest {

    // INSTANCE DATA
    bsl::string  d_data;
    int          d_responseLength;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_DATA            = 0
      , ATTRIBUTE_ID_RESPONSE_LENGTH = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_DATA            = 0
      , ATTRIBUTE_INDEX_RESPONSE_LENGTH = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    explicit SimpleRequest(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SimpleRequest' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    SimpleRequest(const SimpleRequest& original,
                  bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SimpleRequest' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SimpleRequest(SimpleRequest&& original) noexcept;
        // Create an object of type 'SimpleRequest' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    SimpleRequest(SimpleRequest&& original,
                  bslma::Allocator *basicAllocator);
        // Create an object of type 'SimpleRequest' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~SimpleRequest();
        // Destroy this object.

    // MANIPULATORS
    SimpleRequest& operator=(const SimpleRequest& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SimpleRequest& operator=(SimpleRequest&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bsl::string& data();
        // Return a reference to the modifiable "Data" attribute of this
        // object.

    int& responseLength();
        // Return a reference to the modifiable "ResponseLength" attribute of
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

    const bsl::string& data() const;
        // Return a reference offering non-modifiable access to the "Data"
        // attribute of this object.

    int responseLength() const;
        // Return the value of the "ResponseLength" attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const SimpleRequest& lhs, const SimpleRequest& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const SimpleRequest& lhs, const SimpleRequest& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SimpleRequest& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::SimpleRequest)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                            // -------------------
                            // class SimpleRequest
                            // -------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int SimpleRequest::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_data, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_responseLength, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int SimpleRequest::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_DATA: {
        return manipulator(&d_data, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA]);
      }
      case ATTRIBUTE_ID_RESPONSE_LENGTH: {
        return manipulator(&d_responseLength, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int SimpleRequest::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
                                         lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& SimpleRequest::data()
{
    return d_data;
}

inline
int& SimpleRequest::responseLength()
{
    return d_responseLength;
}

// ACCESSORS
template <class ACCESSOR>
int SimpleRequest::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_data, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_responseLength, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int SimpleRequest::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_DATA: {
        return accessor(d_data, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA]);
      }
      case ATTRIBUTE_ID_RESPONSE_LENGTH: {
        return accessor(d_responseLength, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int SimpleRequest::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& SimpleRequest::data() const
{
    return d_data;
}

inline
int SimpleRequest::responseLength() const
{
    return d_responseLength;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::SimpleRequest& lhs,
        const s_baltst::SimpleRequest& rhs)
{
    return  lhs.data() == rhs.data()
         && lhs.responseLength() == rhs.responseLength();
}

inline
bool s_baltst::operator!=(
        const s_baltst::SimpleRequest& lhs,
        const s_baltst::SimpleRequest& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::SimpleRequest& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_simplerequest.xsd --mode msg --includedir . --msgComponent simplerequest --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
