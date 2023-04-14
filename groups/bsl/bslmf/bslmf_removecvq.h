// bslmf_removecvq.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECVQ
#define INCLUDED_BSLMF_REMOVECVQ

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing 'const'/'volatile' qualifiers.
//
//@CLASSES:
//  bslmf::RemoveCvq: meta-function for stripping 'const'/'volatile' qualifiers
//
//@DESCRIPTION: This component defines a simple template structure used to
// strip of any top-level 'const'/'volatile' qualifiers from it's single
// template parameter.  The un-qualified type can be accessed via the 'Type'
// member defined in 'bslmf::RemoveCvq'.
//..
//  struct MyType {};
//
//  bslmf::RemoveCvq<int              >::Type i1; // int i1;
//  bslmf::RemoveCvq<const int        >::Type i2; // int i2;
//  bslmf::RemoveCvq<volatile int     >::Type i3; // int i3;
//  bslmf::RemoveCvq<int *            >::Type i4; // int *i4;
//  bslmf::RemoveCvq<int **           >::Type i5; // int **i5;
//  bslmf::RemoveCvq<int *const       >::Type i6; // int *i6;
//  bslmf::RemoveCvq<int *const *     >::Type i7; // int *const *i7;
//  bslmf::RemoveCvq<int *const *const>::Type i8; // int *const *i8;
//  bslmf::RemoveCvq<MyType           >::Type m1; // MyType m1;
//  bslmf::RemoveCvq<MyType const     >::Type m2; // MyType m2;
//..
//
///Usage
///-----
// We can make a simple template function that shows whether two objects are
// the same type, ignoring 'const' and 'volatile' qualifiers.
//
// First, we create a template that will determine whether two objects are
// EXACTLY the same type:
//..
//  template <class t_TYPE>
//  bool isSame(t_TYPE& a, t_TYPE& b) { return true; }
//  template <class TYPEA, class TYPEB>
//  bool isSame(TYPEA& a, TYPEB& b) { return false; }
//..
// Next, we combine that template function with the use of 'bslmf::RemoveCvq'
// to create a template that will determine whether two objects are the same
// type, ignoring 'const' and 'volatile' qualifiers:
//..
//  template <class TYPEA, class TYPEB>
//  bool isSortaSame(TYPEA& a, TYPEB& b)
//  {
//      typename bslmf::RemoveCvq<TYPEA>::Type aa = a;
//      typename bslmf::RemoveCvq<TYPEB>::Type bb = b;
//
//      return isSame(aa, bb);
//  }
//..
// Next, we use the templates
//..
//  int i = 0, j = 0;
//  const int ci = 0, cj = 0;
//  volatile int vi = 0, vj = 0;
//  const volatile int cvi = 0, cvj = 0;
//
//  double x = 0, y = 0;
//
//  assert(  isSame(i, j));
//  assert(  isSame(ci, cj));
//  assert(  isSame(vi, vj));
//  assert(  isSame(cvi, cvj));
//  assert(  isSame(x, y));
//
//  assert(! isSame(i, x));
//  assert(! isSame(i, ci));
//  assert(! isSame(i, vi));
//  assert(! isSame(i, cvi));
//  assert(! isSame(ci, vi));
//  assert(! isSame(ci, cvi));
//  assert(! isSame(vi, cvi));
//
//  assert(! isSortaSame(i, x));
//  assert(  isSortaSame(i, ci));
//  assert(  isSortaSame(i, vi));
//  assert(  isSortaSame(i, cvi));
//  assert(  isSortaSame(ci, vi));
//  assert(  isSortaSame(ci, vi));
//  assert(  isSortaSame(ci, cvi));
//  assert(  isSortaSame(vi, cvi));
//..

#include <bslscm_version.h>

#include <bslmf_removecv.h>

namespace BloombergLP {

namespace bslmf {

                           // ================
                           // struct RemoveCvq
                           // ================

template <class t_TYPE>
struct RemoveCvq
{
    // This class implements a meta-function for stripping top-level
    // const/volatile qualifiers from it's parameter type.

    typedef typename bsl::remove_cv<t_TYPE>::type Type;
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_RemoveCvq
#undef bslmf_RemoveCvq
#endif
#define bslmf_RemoveCvq bslmf::RemoveCvq
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
