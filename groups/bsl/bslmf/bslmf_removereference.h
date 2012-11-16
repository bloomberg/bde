// bslmf_removereference.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#define INCLUDED_BSLMF_REMOVEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing reference-ness from types.
//
//@CLASSES:
//  bsl::remove_reference: standard meta-function for stripping reference-ness
//  bslmf::RemoveReference: meta-function for stripping reference-ness
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::remove_reference' and 'BloombergLP::bslmf::RemoveReference', both of
// which may be used to strip reference-ness from a type.
//
// 'bsl::remove_reference' meets the requirements of the 'remove_reference'
// template defined in the C++11 standard [meta.trans.ref], while
// 'bslmf::RemoveReference' was devised before 'remove_reference' was
// standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::remove_reference' is indicated by
// the class member 'type', while the result for 'bslmf::RemoveReference' is
// indicated by the class member 'Type'.
//
// Note that 'bsl::remove_reference' should be preferred over
// 'bslmf::RemoveReference', and in general, should be used by new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Pointer Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to remove reference-ness on a couple of types.
//
// Now, we instantiate the 'bsl::remove_reference' template for each of these
// types, and use 'bsl::is_same' meta-function to assert the 'type' static data
// member of each instantiation:
//..
//  assert(true  ==
//             (bsl::is_same<bsl::remove_reference<int&>::type, int >::value));
//  assert(false ==
//             (bsl::is_same<bsl::remove_reference<int&>::type, int&>::value));
//  assert(true  ==
//             (bsl::is_same<bsl::remove_reference<int >::type, int >::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

                         // ======================
                         // struct RemoveReference
                         // ======================

namespace bsl {

template <typename TYPE>
struct remove_reference
{
    typedef TYPE type;
};

template <typename TYPE>
struct remove_reference<TYPE &>
{
    typedef TYPE type;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <typename TYPE>
struct remove_reference<TYPE &&>
{
    typedef TYPE type;
};

#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct RemoveReference
{
    typedef typename bsl::remove_reference<TYPE>::type Type;
};

}  // close package namespace
}  // close enterprise namespace


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
