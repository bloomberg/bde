// s_baltst_customstring.h          *DO NOT EDIT*          @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_CUSTOMSTRING
#define INCLUDED_S_BALTST_CUSTOMSTRING

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_customstring_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_typetraits.h>

#include <bdlat_valuetypefunctions.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_string.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class CustomString; }
namespace s_baltst {

                             // ==================
                             // class CustomString
                             // ==================

class CustomString {

    // INSTANCE DATA
    bsl::string d_value;

    // FRIENDS
    friend bool operator==(const CustomString& lhs, const CustomString& rhs);
    friend bool operator!=(const CustomString& lhs, const CustomString& rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS

    /// Create an object of type `CustomString` having the default value.
    /// Use the optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit CustomString(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `CustomString` having the value
    /// of the specified `original` object.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    CustomString(const CustomString& original,
                bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `CustomString` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    CustomString(CustomString&& original) = default;

    /// Create an object of type `CustomString` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    /// Use the optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    CustomString(CustomString&& original,
                bslma::Allocator *basicAllocator);
#endif

    /// Create an object of type `CustomString` having the specified
    /// `value`.  Use the optionally specified `basicAllocator` to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.
    explicit CustomString(const bsl::string& value,
                         bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~CustomString();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    CustomString& operator=(const CustomString& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    CustomString& operator=(CustomString&& rhs) = default;
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Convert from the specified `value` to this type.  Return 0 if
    /// successful and non-zero otherwise.  Note that, if `value` is an
    /// invalid UTF-8 string and "CustomString" has length restrictions,
    /// this function will fail and keep the underlying string unchanged.
    int fromString(const bsl::string& value);
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
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Convert this value to `bsl::string`.
    const bsl::string& toString() const;

    // PUBLIC CLASS METHODS

    /// Check if the specified `value` satisfies the restrictions of this
    /// class (i.e., "CustomString").  Return 0 if successful (i.e., the
    /// restrictions are satisfied) and non-zero otherwise.
    static int checkRestrictions(const bsl::string& value);
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const CustomString& lhs, const CustomString& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const CustomString& lhs, const CustomString& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const CustomString& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::CustomString)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                             // ------------------
                             // class CustomString
                             // ------------------

// CREATORS
inline
CustomString::CustomString(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomString::CustomString(const CustomString& original, bslma::Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
inline
CustomString::CustomString(CustomString&& original, bslma::Allocator *basicAllocator)
: d_value(bsl::move(original.d_value), basicAllocator)
{
}
#endif

inline
CustomString::CustomString(const bsl::string& value, bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
    BSLS_ASSERT(checkRestrictions(value) == 0);
}

inline
CustomString::~CustomString()
{
}

// MANIPULATORS
inline
CustomString& CustomString::operator=(const CustomString& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void CustomString::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomString::fromString(const bsl::string& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
inline
bsl::ostream& CustomString::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomString::toString() const
{
    return d_value;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::CustomString& lhs,
        const s_baltst::CustomString& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool s_baltst::operator!=(
        const s_baltst::CustomString& lhs,
        const s_baltst::CustomString& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::CustomString& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_customstring.xsd --mode msg --includedir . --msgComponent customstring --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
