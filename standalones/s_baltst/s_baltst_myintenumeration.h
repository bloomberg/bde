// s_baltst_myintenumeration.h        *DO NOT EDIT*        @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_MYINTENUMERATION
#define INCLUDED_S_BALTST_MYINTENUMERATION

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_myintenumeration_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bdlat_valuetypefunctions.h>

#include <bsls_objectbuffer.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace s_baltst { class MyIntEnumeration; }
namespace s_baltst { class MyIntEnumerationRecord; }
namespace s_baltst {

                           // ======================
                           // class MyIntEnumeration
                           // ======================

class MyIntEnumeration {

    // INSTANCE DATA
    int d_value;

    // FRIENDS
    friend bool operator==(const MyIntEnumeration& lhs, const MyIntEnumeration& rhs);
    friend bool operator!=(const MyIntEnumeration& lhs, const MyIntEnumeration& rhs);

  public:
    // TYPES
    typedef int BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int VALUE1;

    static const int VALUE2;

    // CREATORS
    MyIntEnumeration();
        // Create an object of type 'MyIntEnumeration' having the default
        // value.

    MyIntEnumeration(const MyIntEnumeration& original);
        // Create an object of type 'MyIntEnumeration' having the value of the
        // specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MyIntEnumeration(MyIntEnumeration&& original) = default;
        // Create an object of type 'MyIntEnumeration' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
#endif

    explicit MyIntEnumeration(const int& value);
        // Create an object of type 'MyIntEnumeration' having the specified
        // 'value'.

    ~MyIntEnumeration();
        // Destroy this object.

    // MANIPULATORS
    MyIntEnumeration& operator=(const MyIntEnumeration& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MyIntEnumeration& operator=(MyIntEnumeration&& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromInt(const int& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

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

    const int& toInt() const;
        // Convert this value to 'int'.

    // PUBLIC CLASS METHODS
    static int checkRestrictions(const int& value);
        // Check if the specified 'value' satisfies the restrictions of this
        // class (i.e., "MyIntEnumeration").  Return 0 if successful (i.e., the
        // restrictions are satisfied) and non-zero otherwise.
};

// FREE OPERATORS
inline
bool operator==(const MyIntEnumeration& lhs, const MyIntEnumeration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MyIntEnumeration& lhs, const MyIntEnumeration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MyIntEnumeration& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MyIntEnumeration)

namespace s_baltst {

                        // ============================
                        // class MyIntEnumerationRecord
                        // ============================

class MyIntEnumerationRecord {

    // INSTANCE DATA
    MyIntEnumeration  d_value;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_VALUE = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_VALUE = 0
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
    MyIntEnumerationRecord();
        // Create an object of type 'MyIntEnumerationRecord' having the default
        // value.

    MyIntEnumerationRecord(const MyIntEnumerationRecord& original);
        // Create an object of type 'MyIntEnumerationRecord' having the value
        // of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MyIntEnumerationRecord(MyIntEnumerationRecord&& original) = default;
        // Create an object of type 'MyIntEnumerationRecord' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.
#endif

    ~MyIntEnumerationRecord();
        // Destroy this object.

    // MANIPULATORS
    MyIntEnumerationRecord& operator=(const MyIntEnumerationRecord& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MyIntEnumerationRecord& operator=(MyIntEnumerationRecord&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    MyIntEnumeration& value();
        // Return a reference to the modifiable "Value" attribute of this
        // object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const MyIntEnumeration& value() const;
        // Return a reference offering non-modifiable access to the "Value"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const MyIntEnumerationRecord& lhs, const MyIntEnumerationRecord& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MyIntEnumerationRecord& lhs, const MyIntEnumerationRecord& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MyIntEnumerationRecord& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MyIntEnumerationRecord)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                           // ----------------------
                           // class MyIntEnumeration
                           // ----------------------

// CREATORS
inline
MyIntEnumeration::MyIntEnumeration()
: d_value()
{
}

inline
MyIntEnumeration::MyIntEnumeration(const MyIntEnumeration& original)
: d_value(original.d_value)
{
}

inline
MyIntEnumeration::MyIntEnumeration(const int& value)
: d_value(value)
{
    BSLS_ASSERT(checkRestrictions(value) == 0);
}

inline
MyIntEnumeration::~MyIntEnumeration()
{
}

// MANIPULATORS
inline
MyIntEnumeration& MyIntEnumeration::operator=(const MyIntEnumeration& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void MyIntEnumeration::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int MyIntEnumeration::fromInt(const int& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
inline
bsl::ostream& MyIntEnumeration::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const int& MyIntEnumeration::toInt() const
{
    return d_value;
}



                        // ----------------------------
                        // class MyIntEnumerationRecord
                        // ----------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MyIntEnumerationRecord::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MyIntEnumerationRecord::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_VALUE: {
        return manipulator(&d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MyIntEnumerationRecord::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
MyIntEnumeration& MyIntEnumerationRecord::value()
{
    return d_value;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MyIntEnumerationRecord::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MyIntEnumerationRecord::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_VALUE: {
        return accessor(d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MyIntEnumerationRecord::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const MyIntEnumeration& MyIntEnumerationRecord::value() const
{
    return d_value;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::MyIntEnumeration& lhs,
        const s_baltst::MyIntEnumeration& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool s_baltst::operator!=(
        const s_baltst::MyIntEnumeration& lhs,
        const s_baltst::MyIntEnumeration& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MyIntEnumeration& rhs)
{
    return rhs.print(stream, 0, -1);
}

inline
bool s_baltst::operator==(
        const s_baltst::MyIntEnumerationRecord& lhs,
        const s_baltst::MyIntEnumerationRecord& rhs)
{
    return  lhs.value() == rhs.value();
}

inline
bool s_baltst::operator!=(
        const s_baltst::MyIntEnumerationRecord& lhs,
        const s_baltst::MyIntEnumerationRecord& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MyIntEnumerationRecord& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2023.10.25
// USING bas_codegen.pl s_baltst_myintenumeration.xsd --mode msg --includedir . --msgComponent myintenumeration --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2023 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
