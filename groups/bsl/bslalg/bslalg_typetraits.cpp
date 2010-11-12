// bslalg_typetraits.cpp                  -*-C++-*-
#include <bslalg_typetraits.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

// IMPLEMENTATION NOTES:
//
///Automatic Traits Deduction:
///---------------------------
// The classes and macros in this section exist to automatically deduce the
// traits for a given type without requiring explicit specialization of
// 'bslalg_TypeTraits' for that type.  The following traits are assigned
// automatically:
//
// - Fundamental, pointer, and enumeration types are assigned the traits in
//   the 'bslalg_TypeTraitGroupPod' (that is,
//   'bslalg_TypeTraitBitwiseCopyable', 'bslalg_TypeTraitBitwiseMoveable',
//   'bslalg_TypeTraitHasTrivialDefaultConstructor'), as well as
//   'bslalg_TypeTraitBitwiseEqualityComparable'.
//
// - Class types that contain an invocation of the
//   'BSLALG_DECLARE_NESTED_TRAITS' macros have traits assigned as per that
//   macro.  The macro is a simple way to assign traits and avoids the need to
//   use extensive partial specialization to define 'bslalg_TypeTraits<T>'.
//
///Nested Probe:
///-------------
// The comma operator was chosen as the invasive probe function because it has
// a default meaning for all types and because is the least likely to be
// declared explicitly within the class.  When checked at compile-time, the
// comma operator will return a specific value of a type which can be tested
// within a metafunction.  Any comma operator which does not take the exact
// arguments and return the exact type of the one declared by this macro will
// not cause a match.  Even a templated comma operator will not trigger a false
// positive.  Other solutions that were considered and the reasons they were
// eliminated are:
//
// - Conversion constructor from special type: Too likely to conflict with
//   a templated conversion constructor.
//
// - Conversion operator from special type: Internal error on MSVC.
//
// - Friend function declaration: requires that class actually be
//   instantiated to work with Koenig lookup on most compilers.  Did not
//   work at all (no Koenig lookup) on Sun CC 5.2.  Friend function was
//   not always instantiated on most other compilers.
//
// - Other operators: Too likely to collide with use within class,
//   including hiding base-class operator.
//
// Correspondingly, 'bslalg_TypeTraits_NestedTest::VALUE' invokes the comma
// operator to check for the invasive probe.  The result of the comma operator
// is either 'bslalg_TypeTraits_NestedYes' or some other type, which is then
// passed to 'match' to see if the comma operator returns
// 'bslalg_TypeTraits_NestedYes'.

namespace BloombergLP {

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
