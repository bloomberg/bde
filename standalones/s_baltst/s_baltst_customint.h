// s_baltst_customint.h           *DO NOT EDIT*            @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_CUSTOMINT
#define INCLUDED_S_BALTST_CUSTOMINT

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_customint_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_typetraits.h>

#include <bdlat_valuetypefunctions.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace s_baltst { class CustomInt; }
namespace s_baltst {

                              // ===============
                              // class CustomInt
                              // ===============

class CustomInt {

    // INSTANCE DATA
    int d_value;

  public:
    // TYPES
    typedef int BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS

    /// Create an object of type `CustomInt` having the default value.
    CustomInt();

    /// Create an object of type `CustomInt` having the value of the
    /// specified `original` object.
    CustomInt(const CustomInt& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `CustomInt` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    CustomInt(CustomInt&& original) = default;
#endif

    /// Create an object of type `CustomInt` having the specified `value`.
    explicit CustomInt(const int& value);

    /// Destroy this object.
    ~CustomInt();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    CustomInt& operator=(const CustomInt& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    CustomInt& operator=(CustomInt&& rhs) = default;
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Convert from the specified `value` to this type.  Return 0 if
    /// successful and non-zero otherwise.
    int fromInt(const int& value);

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

    /// Convert this value to `int`.
    const int& toInt() const;

    // PUBLIC CLASS METHODS

    /// Check if the specified `value` satisfies the restrictions of this
    /// class (i.e., "CustomInt").  Return 0 if successful (i.e., the
    /// restrictions are satisfied) and non-zero otherwise.
    static int checkRestrictions(const int& value);

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const CustomInt& lhs, const CustomInt& rhs)
    {
        return lhs.d_value == rhs.d_value;
    }

    /// Return `true` if the specified `lhs` and `rhs` attribute objects do
    /// not have the same value, and `false` otherwise.  Two attribute
    /// objects do not have the same value if one or more respective
    /// attributes differ in values.
    friend bool operator!=(const CustomInt& lhs, const CustomInt& rhs)
    {
        return lhs.d_value != rhs.d_value;
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream& stream, const CustomInt& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::CustomInt)

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                              // ---------------
                              // class CustomInt
                              // ---------------

// CREATORS
inline
CustomInt::CustomInt()
: d_value()
{
}

inline
CustomInt::CustomInt(const CustomInt& original)
: d_value(original.d_value)
{
}

inline
CustomInt::CustomInt(const int& value)
: d_value(value)
{
    BSLS_ASSERT(checkRestrictions(value) == 0);
}

inline
CustomInt::~CustomInt()
{
}

// MANIPULATORS
inline
CustomInt& CustomInt::operator=(const CustomInt& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void CustomInt::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomInt::fromInt(const int& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
inline
bsl::ostream& CustomInt::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const int& CustomInt::toInt() const
{
    return d_value;
}

}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2025.08.21
// USING bas_codegen.pl s_baltst_customint.xsd --mode msg --includedir . --msgComponent customint --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
