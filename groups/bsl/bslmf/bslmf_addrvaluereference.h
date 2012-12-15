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
//  bsl::add_rvalue_reference: standard meta-function for transforming type
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_addlvaluereference
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::add_rvalue_reference', which may be used to transform a type to its
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
// Suppose that we want to transform a couple of types to rvalue reference
// types.
//
// Now, we instantiate the 'bsl::add_rvalue_reference' template for these
// types, and use the 'bsl::is_same' meta-function to assert the 'type' static
// data member of each instantiation:
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
// Note that rvalue is introduced in C++11 and may not be supported by all
// compilers.

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
    typedef TYPE&& type;
};

#define BSL_DEFINE_ADD_RVALUE_REFERENCE(TYPE, REF_TYPE) \
template <>                                             \
struct add_rvalue_reference<TYPE> {                     \
    typedef REF_TYPE type;                              \
}                                                       \

BSL_DEFINE_ADD_RVALUE_REFERENCE(void, void);
BSL_DEFINE_ADD_RVALUE_REFERENCE(void const, void const);
BSL_DEFINE_ADD_RVALUE_REFERENCE(void volatile, void volatile);
BSL_DEFINE_ADD_RVALUE_REFERENCE(void const volatile, void const volatile);

#undef BSL_DEFINE_ADD_RVALUE_REFERENCE

#endif

}  // close namespace bsl

#endif

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
