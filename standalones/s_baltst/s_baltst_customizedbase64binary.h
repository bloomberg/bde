// s_baltst_customizedbase64binary.h     *DO NOT EDIT*     @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_CUSTOMIZEDBASE64BINARY
#define INCLUDED_S_BALTST_CUSTOMIZEDBASE64BINARY

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_customizedbase64binary_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_typetraits.h>

#include <bdlat_valuetypefunctions.h>

#include <bsls_assert.h>

#include <bsl_vector.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace s_baltst { class CustomizedBase64Binary; }
namespace s_baltst {

                        // ============================
                        // class CustomizedBase64Binary
                        // ============================

class CustomizedBase64Binary {

    // INSTANCE DATA
    bsl::vector<char> d_value;

    // FRIENDS
    friend bool operator==(const CustomizedBase64Binary& lhs, const CustomizedBase64Binary& rhs);
    friend bool operator!=(const CustomizedBase64Binary& lhs, const CustomizedBase64Binary& rhs);

  public:
    // TYPES
    typedef bsl::vector<char> BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS

    /// Create an object of type `CustomizedBase64Binary` having the default
    /// value.
    CustomizedBase64Binary();

    /// Create an object of type `CustomizedBase64Binary` having the value
    /// of the specified `original` object.
    CustomizedBase64Binary(const CustomizedBase64Binary& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `CustomizedBase64Binary` having the value
    /// of the specified `original` object.  After performing this action,
    /// the `original` object will be left in a valid, but unspecified
    /// state.
    CustomizedBase64Binary(CustomizedBase64Binary&& original) = default;
#endif

    /// Create an object of type `CustomizedBase64Binary` having the
    /// specified `value`.
    explicit CustomizedBase64Binary(const bsl::vector<char>& value);

    /// Destroy this object.
    ~CustomizedBase64Binary();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    CustomizedBase64Binary& operator=(const CustomizedBase64Binary& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    CustomizedBase64Binary& operator=(CustomizedBase64Binary&& rhs) = default;
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Convert from the specified `value` to this type.  Return 0 if
    /// successful and non-zero otherwise.
    int fromVector(const bsl::vector<char>& value);

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

    /// Convert this value to `bsl::vector<char>`.
    const bsl::vector<char>& toVector() const;

    // PUBLIC CLASS METHODS

    /// Check if the specified `value` satisfies the restrictions of this
    /// class (i.e., "CustomizedBase64Binary").  Return 0 if successful
    /// (i.e., the restrictions are satisfied) and non-zero otherwise.
    static int checkRestrictions(const bsl::vector<char>& value);
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const CustomizedBase64Binary& lhs, const CustomizedBase64Binary& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const CustomizedBase64Binary& lhs, const CustomizedBase64Binary& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const CustomizedBase64Binary& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::CustomizedBase64Binary)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                        // ----------------------------
                        // class CustomizedBase64Binary
                        // ----------------------------

// PRIVATE CLASS METHODS
inline
int CustomizedBase64Binary::checkRestrictions(const bsl::vector<char>& value)
{
    (void)value;
    return 0;
}

// CREATORS
inline
CustomizedBase64Binary::CustomizedBase64Binary()
: d_value()
{
}

inline
CustomizedBase64Binary::CustomizedBase64Binary(const CustomizedBase64Binary& original)
: d_value(original.d_value)
{
}

inline
CustomizedBase64Binary::CustomizedBase64Binary(const bsl::vector<char>& value)
: d_value(value)
{
    BSLS_ASSERT(checkRestrictions(value) == 0);
}

inline
CustomizedBase64Binary::~CustomizedBase64Binary()
{
}

// MANIPULATORS
inline
CustomizedBase64Binary& CustomizedBase64Binary::operator=(const CustomizedBase64Binary& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void CustomizedBase64Binary::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomizedBase64Binary::fromVector(const bsl::vector<char>& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
inline
bsl::ostream& CustomizedBase64Binary::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::vector<char>& CustomizedBase64Binary::toVector() const
{
    return d_value;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::CustomizedBase64Binary& lhs,
        const s_baltst::CustomizedBase64Binary& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool s_baltst::operator!=(
        const s_baltst::CustomizedBase64Binary& lhs,
        const s_baltst::CustomizedBase64Binary& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::CustomizedBase64Binary& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_customizedbase64binary.xsd --mode msg --includedir . --msgComponent customizedbase64binary --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2023 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
