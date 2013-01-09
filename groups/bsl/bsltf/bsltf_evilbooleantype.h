// bsltf_evilbooleantype.h                                            -*-C++-*-
#ifndef INCLUDED_BSLTF_EVILBOOLEANTYPE
#define INCLUDED_BSLTF_EVILBOOLEANTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumerated test type.
//
//@CLASSES:
//   bsltf::EnumeratedTestType: enumerated test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides a type that is convertible-to-'bool',
// and so may be used to satisfy many C++ standard library requirements, but is
// perversely implemented to provide the most awkward interface that meets the
// requirements.  This type is not intented for use in production code, but is
// most useful when implementing test drivers for generic components that must
// accept predicates, or other expressions, that yield a type that is merely
// convertible to' bool'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: (TBD)
/// - - - - - - - - - - - - - - - -
// First, we ...

namespace BloombergLP {

namespace bsltf {

                            // =====================
                            // class EvilBooleanType
                            // =====================

struct EvilBooleanType {
    // This class provides a test type for predicates returning a type that is
    // convertible-to-bool.  It makes life reasonably difficult by disabling
    // the address-of and comma operators, but deliberately does not overload
    // the '&&' and '||' operators, as we hope the standard will be updated to
    // no longer require such support.  Once C++11 becomes available, this
    // class would use an 'explicit operator bool()' conversion operator, and
    // explicitly supply the '==' and '!=' operators, but we use the
    // convertible-to-pointer-to-member idiom in the meantime.  Implicitly
    // defined operations fill out the API as needed.

  private:
    // PRIVATE TYPES
    struct ImpDetail { int d_member; };

    typedef int ImpDetail::* BoolResult;

    // DATA
    BoolResult d_value;

  private:
    void operator=(const EvilBooleanType&);  // = delete;
        // not implemented

    void operator&();  // = delete;
        // not implemented

    template<class T>
    void operator,(const T&); // = delete;
        // not implemented

    template<class T>
    void operator,(T&); // = delete;
        // not implemented


  public:
    // CREATORS
    EvilBooleanType(bool value);                                    // IMPLICIT

    // ACCESSORS
    operator BoolResult() const;

    EvilBooleanType operator!() const;
};

EvilBooleanType operator==(const EvilBooleanType& lhs,
                           const EvilBooleanType& rhs);

EvilBooleanType operator!=(const EvilBooleanType& lhs,
                           const EvilBooleanType& rhs);


// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                       // ---------------------
                       // class EvilBooleanType
                       // ---------------------

// CREATORS
inline
EvilBooleanType::EvilBooleanType(bool value)
: d_value(!value ? 0 : &ImpDetail::d_member)
{
}

// ACCESSORS
inline
EvilBooleanType::operator BoolResult() const
{
    return d_value;
}

inline
EvilBooleanType EvilBooleanType::operator!() const
{
    return !d_value;
}

}  // close package namespace

inline
bsltf::EvilBooleanType bsltf::operator==(const EvilBooleanType& lhs,
                                         const EvilBooleanType& rhs)
{
    return static_cast<bool>(lhs) == static_cast<bool>(rhs);
}

inline
bsltf::EvilBooleanType bsltf::operator!=(const EvilBooleanType& lhs,
                                         const EvilBooleanType& rhs)
{
    return static_cast<bool>(lhs) != static_cast<bool>(rhs);
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
