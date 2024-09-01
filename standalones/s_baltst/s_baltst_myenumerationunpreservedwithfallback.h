// s_baltst_myenumerationunpreservedwithfallback.h *DO NOT EDIT* @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_MYENUMERATIONUNPRESERVEDWITHFALLBACK
#define INCLUDED_S_BALTST_MYENUMERATIONUNPRESERVEDWITHFALLBACK

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_myenumerationunpreservedwithfallback_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_enumeratorinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace s_baltst { class MyEnumerationUnpreservedWithFallbackRecord; }
namespace s_baltst {

                 // ==========================================
                 // class MyEnumerationUnpreservedWithFallback
                 // ==========================================

struct MyEnumerationUnpreservedWithFallback {

  public:
    // TYPES
    enum Value {
        UNKNOWN = 0
      , VALUE1  = 1
      , VALUE2  = 2
    };

    enum {
        NUM_ENUMERATORS = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration `value`.
    static const char *toString(Value value);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string` of the specified `stringLength`.  Return 0 on
    /// success, and a non-zero value with no effect on `result` otherwise
    /// (i.e., `string` does not match any enumerator).
    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);

    /// Load into the specified `result` the enumerator matching the
    /// specified `string`.  Return 0 on success, and a non-zero value with
    /// no effect on `result` otherwise (i.e., `string` does not match any
    /// enumerator).
    static int fromString(Value              *result,
                          const bsl::string&  string);

    /// Load into the specified `result` the enumerator matching the
    /// specified `number`.  Return 0 on success, and a non-zero value with
    /// no effect on `result` otherwise (i.e., `number` does not match any
    /// enumerator).
    static int fromInt(Value *result, int number);

    /// Load into the specified `result` the fallback enumerator value and
    /// return 0 to indicate success.
    static int makeFallback(Value *result);

    /// Return `true` to indicate that this type supports a fallback
    /// enumerator.
    static bool hasFallback(Value);

    /// Return `true` if the specified `value` equals the fallback
    /// enumerator, and `false` otherwise.
    static bool isFallback(Value value);

    /// Write to the specified `stream` the string representation of
    /// the specified enumeration `value`.  Return a reference to
    /// the modifiable `stream`.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

// FREE OPERATORS

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, MyEnumerationUnpreservedWithFallback::Value rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_ENUMERATION_WITH_FALLBACK_TRAITS(s_baltst::MyEnumerationUnpreservedWithFallback)


namespace s_baltst {

              // ================================================
              // class MyEnumerationUnpreservedWithFallbackRecord
              // ================================================

class MyEnumerationUnpreservedWithFallbackRecord {

    // INSTANCE DATA
    MyEnumerationUnpreservedWithFallback::Value  d_value;

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

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type
    /// `MyEnumerationUnpreservedWithFallbackRecord` having the default
    /// value.
    MyEnumerationUnpreservedWithFallbackRecord();

    /// Create an object of type
    /// `MyEnumerationUnpreservedWithFallbackRecord` having the value of the
    /// specified `original` object.
    MyEnumerationUnpreservedWithFallbackRecord(const MyEnumerationUnpreservedWithFallbackRecord& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type
    /// `MyEnumerationUnpreservedWithFallbackRecord` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    MyEnumerationUnpreservedWithFallbackRecord(MyEnumerationUnpreservedWithFallbackRecord&& original) = default;
#endif

    /// Destroy this object.
    ~MyEnumerationUnpreservedWithFallbackRecord();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MyEnumerationUnpreservedWithFallbackRecord& operator=(const MyEnumerationUnpreservedWithFallbackRecord& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MyEnumerationUnpreservedWithFallbackRecord& operator=(MyEnumerationUnpreservedWithFallbackRecord&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Value" attribute of this
    /// object.
    MyEnumerationUnpreservedWithFallback::Value& value();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return the value of the "Value" attribute of this object.
    MyEnumerationUnpreservedWithFallback::Value value() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const MyEnumerationUnpreservedWithFallbackRecord& lhs, const MyEnumerationUnpreservedWithFallbackRecord& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const MyEnumerationUnpreservedWithFallbackRecord& lhs, const MyEnumerationUnpreservedWithFallbackRecord& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const MyEnumerationUnpreservedWithFallbackRecord& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MyEnumerationUnpreservedWithFallbackRecord)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                 // ------------------------------------------
                 // class MyEnumerationUnpreservedWithFallback
                 // ------------------------------------------

// CLASS METHODS
inline
int MyEnumerationUnpreservedWithFallback::fromString(Value *result, const bsl::string& string)
{
    return fromString(result, string.c_str(), static_cast<int>(string.length()));
}

inline
bsl::ostream& MyEnumerationUnpreservedWithFallback::print(bsl::ostream&      stream,
                                 MyEnumerationUnpreservedWithFallback::Value value)
{
    return stream << toString(value);
}

inline
int MyEnumerationUnpreservedWithFallback::makeFallback(MyEnumerationUnpreservedWithFallback::Value *result)
{
    *result = UNKNOWN;
    return 0;
}

inline
bool MyEnumerationUnpreservedWithFallback::hasFallback(MyEnumerationUnpreservedWithFallback::Value)
{
    return true;
}

inline
bool MyEnumerationUnpreservedWithFallback::isFallback(MyEnumerationUnpreservedWithFallback::Value value)
{
    return value == UNKNOWN;
}



              // ------------------------------------------------
              // class MyEnumerationUnpreservedWithFallbackRecord
              // ------------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MyEnumerationUnpreservedWithFallbackRecord::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MyEnumerationUnpreservedWithFallbackRecord::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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
int MyEnumerationUnpreservedWithFallbackRecord::manipulateAttribute(
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
MyEnumerationUnpreservedWithFallback::Value& MyEnumerationUnpreservedWithFallbackRecord::value()
{
    return d_value;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MyEnumerationUnpreservedWithFallbackRecord::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MyEnumerationUnpreservedWithFallbackRecord::accessAttribute(t_ACCESSOR& accessor, int id) const
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
int MyEnumerationUnpreservedWithFallbackRecord::accessAttribute(
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
MyEnumerationUnpreservedWithFallback::Value MyEnumerationUnpreservedWithFallbackRecord::value() const
{
    return d_value;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        s_baltst::MyEnumerationUnpreservedWithFallback::Value rhs)
{
    return s_baltst::MyEnumerationUnpreservedWithFallback::print(stream, rhs);
}

inline
bool s_baltst::operator==(
        const s_baltst::MyEnumerationUnpreservedWithFallbackRecord& lhs,
        const s_baltst::MyEnumerationUnpreservedWithFallbackRecord& rhs)
{
    return  lhs.value() == rhs.value();
}

inline
bool s_baltst::operator!=(
        const s_baltst::MyEnumerationUnpreservedWithFallbackRecord& lhs,
        const s_baltst::MyEnumerationUnpreservedWithFallbackRecord& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MyEnumerationUnpreservedWithFallbackRecord& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2023.09.30
// USING bas_codegen.pl s_baltst_myenumerationunpreservedwithfallback.xsd --mode msg --includedir . --msgComponent myenumerationunpreservedwithfallback --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2023 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
