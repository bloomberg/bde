// s_baltst_mysequencewithattributes.h    *DO NOT EDIT*    @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_MYSEQUENCEWITHATTRIBUTES
#define INCLUDED_S_BALTST_MYSEQUENCEWITHATTRIBUTES

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithattributes_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlb_nullablevalue.h>

#include <bsl_string.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class MySequenceWithAttributes; }
namespace s_baltst {

                       // ==============================
                       // class MySequenceWithAttributes
                       // ==============================

class MySequenceWithAttributes {

    // INSTANCE DATA
    bdlb::NullableValue<bsl::string>  d_attribute2;
    bdlb::NullableValue<bsl::string>  d_element2;
    int                               d_attribute1;
    bdlb::NullableValue<int>          d_element1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
      , ATTRIBUTE_ID_ATTRIBUTE2 = 1
      , ATTRIBUTE_ID_ELEMENT1   = 2
      , ATTRIBUTE_ID_ELEMENT2   = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
      , ATTRIBUTE_INDEX_ATTRIBUTE2 = 1
      , ATTRIBUTE_INDEX_ELEMENT1   = 2
      , ATTRIBUTE_INDEX_ELEMENT2   = 3
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
    explicit MySequenceWithAttributes(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAttributes' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MySequenceWithAttributes(const MySequenceWithAttributes& original,
                             bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'MySequenceWithAttributes' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MySequenceWithAttributes(MySequenceWithAttributes&& original) noexcept;
        // Create an object of type 'MySequenceWithAttributes' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.

    MySequenceWithAttributes(MySequenceWithAttributes&& original,
                             bslma::Allocator *basicAllocator);
        // Create an object of type 'MySequenceWithAttributes' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
#endif

    ~MySequenceWithAttributes();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithAttributes& operator=(const MySequenceWithAttributes& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MySequenceWithAttributes& operator=(MySequenceWithAttributes&& rhs);
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

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
        // object.

    bdlb::NullableValue<int>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
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

    int attribute1() const;
        // Return the value of the "Attribute1" attribute of this object.

    const bdlb::NullableValue<bsl::string>& attribute2() const;
        // Return a reference offering non-modifiable access to the
        // "Attribute2" attribute of this object.

    const bdlb::NullableValue<int>& element1() const;
        // Return a reference offering non-modifiable access to the "Element1"
        // attribute of this object.

    const bdlb::NullableValue<bsl::string>& element2() const;
        // Return a reference offering non-modifiable access to the "Element2"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithAttributes& lhs, const MySequenceWithAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithAttributes& lhs, const MySequenceWithAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithAttributes& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithAttributes)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                       // ------------------------------
                       // class MySequenceWithAttributes
                       // ------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int MySequenceWithAttributes::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int MySequenceWithAttributes::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
      }
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int MySequenceWithAttributes::manipulateAttribute(
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
int& MySequenceWithAttributes::attribute1()
{
    return d_attribute1;
}

inline
bdlb::NullableValue<bsl::string>& MySequenceWithAttributes::attribute2()
{
    return d_attribute2;
}

inline
bdlb::NullableValue<int>& MySequenceWithAttributes::element1()
{
    return d_element1;
}

inline
bdlb::NullableValue<bsl::string>& MySequenceWithAttributes::element2()
{
    return d_element2;
}

// ACCESSORS
template <class ACCESSOR>
int MySequenceWithAttributes::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int MySequenceWithAttributes::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
      }
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int MySequenceWithAttributes::accessAttribute(
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
int MySequenceWithAttributes::attribute1() const
{
    return d_attribute1;
}

inline
const bdlb::NullableValue<bsl::string>& MySequenceWithAttributes::attribute2() const
{
    return d_attribute2;
}

inline
const bdlb::NullableValue<int>& MySequenceWithAttributes::element1() const
{
    return d_element1;
}

inline
const bdlb::NullableValue<bsl::string>& MySequenceWithAttributes::element2() const
{
    return d_element2;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::MySequenceWithAttributes& lhs,
        const s_baltst::MySequenceWithAttributes& rhs)
{
    return  lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2()
         && lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2();
}

inline
bool s_baltst::operator!=(
        const s_baltst::MySequenceWithAttributes& lhs,
        const s_baltst::MySequenceWithAttributes& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MySequenceWithAttributes& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_mysequencewithattributes.xsd --mode msg --includedir . --msgComponent mysequencewithattributes --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
