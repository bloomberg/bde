// bslmf_removecvq.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECVQ
#define INCLUDED_BSLMF_REMOVECVQ

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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
///Usage Example
///-------------
// We can make a simple template function that shows whether two objects are
// the same type, ignoring 'const' and 'volatile' qualifiers.
//
// First, we create a template that will determine whether two objects are
// EXACTLY the same type:
//..
//  template <typename TYPE>
//  bool isSame(TYPE& a, TYPE& b) { return true; }
//  template <typename TYPEA, typename TYPEB>
//  bool isSame(TYPEA& a, TYPEB& b) { return false; }
//..
// Next, we combine that template function with the use of 'bslmf::RemoveCvq'
// to create a template that will determine whether two objects are the same
// type, ignoring 'const' and 'volatile' qualifiers:
//..
//  template <typename TYPEA, typename TYPEB>
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {

namespace bslmf {

template <typename TYPE>
struct Remove_NonCvPointer
{
    typedef TYPE Type;
};

template <typename TYPE>
struct Remove_NonCvPointer<TYPE *>
{
    typedef TYPE Type;
};

                              // ================
                              // struct RemoveCvq
                              // ================

template <typename TYPE>
struct RemovePtrCvq
    // This class implements a meta-function for stripping 'const' and
    // 'volatile' qualifiers from the type pointed to by it's parameter type.
    // It also returns, in 'ValueType', the (non-cvq) type pointed to by its
    // parameter.  This class has no body -- it is specialized below for
    // pointers to const, volatile, and const volatile.  IMPLEMENTATION NOTE:
    // The Sun Workshop 6 C++ 5.2 compiler works with these pointer
    // specializations, but does not work with direct specialization on T, T
    // const, and T volatile.  Thus, 'RemoveCvq', below, is implemented
    // indirectly in terms of 'RemovePtrCvq'.
{
    typedef typename bsl::remove_cv<TYPE>::type                      Type;
    typedef typename Remove_NonCvPointer<typename bsl::remove_cv<TYPE>::type>::Type
                                                                     ValueType;
};

                           // ================
                           // struct RemoveCvq
                           // ================

template <typename TYPE>
struct RemoveCvq
{
    // This class implements a meta-function for stripping top-level
    // const/volatile qualifiers from it's parameter type.

    typedef typename bsl::remove_cv<TYPE>::type Type;
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
