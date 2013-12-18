// bslmf_addrvaluereference.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDRVALUEREFERENCE
#define INCLUDED_BSLMF_ADDRVALUEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time type transformation to rvalue reference.
//
//@CLASSES:
//  bsl::add_rvalue_reference: standard meta-function for transforming types
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_addlvaluereference
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::add_rvalue_reference', that may be used to transform a type to its
// rvalue reference type.
//
// 'bsl::add_rvalue_reference' meets the requirements of the
// 'add_rvalue_reference' template defined in the C++11 standard
// [meta.trans.ref].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform to Rvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform some types to rvalue reference types.
//
// Now, for a set of types, we transform each type to the corresponding rvalue
// reference of that type using 'bsl::add_rvalue_reference' and verify the
// result:
//..
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int&&>::value));
//  assert(false ==
//       (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int  >::value));
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int&>::type,  int& >::value));
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int&&>::type, int&&>::value));
//#endif
//..
// Note that rvalue reference was introduced in C++11 and may not be supported
// by all compilers.  Note also that according to 'reference collapsing'
// semantics [8.3.2], 'add_rvalue_reference' does not transform 'TYPE' to
// rvalue reference type if 'TYPE' is an lvalue reference type.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace bsl {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <class TYPE>
struct add_rvalue_reference {
    // This 'struct' template implements a meta-function to transform the
    // (template parameter) 'TYPE' to its rvalue reference type.

    // PUBLIC TYPES
    typedef TYPE&& type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

template <>
struct add_rvalue_reference<void> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void' type.

    // PUBLIC TYPES
    typedef void type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

template <>
struct add_rvalue_reference<void const> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void const' type.

    // PUBLIC TYPES
    typedef void const type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

template <>
struct add_rvalue_reference<void volatile> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void volatile' type.

    // PUBLIC TYPES
    typedef void volatile type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

template <>
struct add_rvalue_reference<void const volatile> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void const volatile' type.

    // PUBLIC TYPES
    typedef void const volatile type;
        // This 'typedef' is an alias to the return value of this
        // meta-function.
};

#endif

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
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
