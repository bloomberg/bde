// bslalg_typetraits.cpp                                              -*-C++-*-
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
// - Fundamental, pointer, and enumeration types are assigned the traits in the
// 'bslalg::TypeTraitGroupPod' (that is, 'bslalg::TypeTraitBitwiseCopyable',
// 'bslalg::TypeTraitBitwiseMoveable',
// 'bslalg::TypeTraitHasTrivialDefaultConstructor'), as well as
// 'bslalg::TypeTraitBitwiseEqualityComparable'.
//
// - Class types that contain an invocation of the
// 'BSLALG_DECLARE_NESTED_TRAITS' macros have traits assigned as per that
// macro.  The macro is a simple way to assign traits and avoids the need to
// use extensive partial specialization to define 'bslalg_TypeTraits<T>'.
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
// - Conversion constructor from special type: Too likely to conflict with a
// templated conversion constructor.
//
// - Conversion operator from special type: Internal error on MSVC.
//
// - Friend function declaration: requires that class actually be instantiated
// to work with Koenig lookup on most compilers.  Did not work at all (no
// Koenig lookup) on Sun CC 5.2.  Friend function was not always instantiated
// on most other compilers.
//
// - Other operators: Too likely to collide with use within class, including
// hiding base-class operator.
//
// Correspondingly, 'bslalg::TypeTraits_NestedTest::VALUE' invokes the comma
// operator to check for the invasive probe.  The result of the comma operator
// is either 'bslalg::TypeTraits_NestedYes' or some other type, which is then
// passed to 'match' to see if the comma operator returns
// 'bslalg::TypeTraits_NestedYes'.

namespace BloombergLP {

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
