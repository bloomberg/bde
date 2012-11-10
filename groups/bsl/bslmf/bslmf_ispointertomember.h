// bslmf_ispointertomember.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#define INCLUDED_BSLMF_ISPOINTERTOMEMBER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer-to-member types.
//
//@CLASSES:
//  bslmf::IsPointerToMember: check for pointer-to-member types
//  bslmf::IsPointerToMemberData: check for pointer-to-member data types
//  bslmf::IsPointerToMemberFunction: check for pointer-to-member function types
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer to member, optionally qualified
// with 'const' or 'volatile'.  'bslmf::IsPointerToMember' defines a 'value'
// member that is initialized (at compile-time) to 1 if the parameter is a
// pointer to member (or a reference to such a type), and to 0 otherwise.  The
// meta-functions 'bslmf::IsPointerToMemberData' and
// 'bslmf::IsPointerToMemberFunction' are also provided to test specifically
// for pointers to (non-'static') data members and pointers to (non-'static')
// function members, respectively.  Note that, consistent with TR1, these
// metafunctions evaluate to 0 (false) for reference types (i.e.,
// reference-to-pointer-to-member types).
//
///Usage
///-----
// For example:
//..
//  struct MyStruct {};
//  enum   MyEnum {};
//  class  MyClass {};
//
//  typedef int (MyClass::* PMFdRi)(double, int&);
//  typedef int (MyClass::* PMFCe)(MyEnum) const;
//
//  assert(0 == bslmf::IsPointerToMember<int             *>::value);
//  assert(0 == bslmf::IsPointerToMember<    MyStruct    *>::value);
//  assert(1 == bslmf::IsPointerToMember<int MyStruct::*  >::value);
//  assert(0 == bslmf::IsPointerToMember<int MyStruct::*& >::value);
//  assert(0 == bslmf::IsPointerToMember<int MyStruct::* *>::value);
//
//  assert(1 == bslmf::IsPointerToMemberData<int MyStruct::*>::value);
//  assert(0 == bslmf::IsPointerToMemberData<PMFdRi         >::value);
//
//  assert(1 == bslmf::IsPointerToMember<PMFdRi >::value);
//  assert(0 == bslmf::IsPointerToMember<PMFdRi&>::value);
//  assert(1 == bslmf::IsPointerToMember<PMFCe  >::value);
//  assert(0 == bslmf::IsPointerToMember<PMFCe& >::value);
//
//  assert(1 == bslmf::IsPointerToMemberFunction<PMFdRi        >::value);
//  assert(0 == bslmf::IsPointerToMemberFunction<int MyClass::*>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER
#include <bslmf_ismemberfunctionpointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISMEMBEROBJECTPOINTER
#include <bslmf_ismemberobjectpointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISMEMBERPOINTER
#include <bslmf_ismemberpointer.h>
#endif

namespace BloombergLP {
namespace bslmf {

                      // ================================
                      // struct IsPointerToMemberFunction
                      // ================================

template <typename TYPE>
struct IsPointerToMemberFunction : bsl::is_member_function_pointer<TYPE>::type
{
    // Metafunction to test if 'TYPE' is a pointer to a member function.  Note
    // that the result is false if 'TYPE' is a reference.
};

                      // ============================
                      // struct IsPointerToMemberData
                      // ============================

template <typename TYPE>
struct IsPointerToMemberData : bsl::is_member_object_pointer<TYPE>::type
{
    // Metafunction to test if 'TYPE' is a pointer to a member object.  Note
    // that the result is false if 'TYPE' is a reference.
};

                      // ========================
                      // struct IsPointerToMember
                      // ========================

template <typename TYPE>
struct IsPointerToMember : bsl::is_member_pointer<TYPE>::type
{
    // Metafunction to test if 'TYPE' is a pointer to member (function or
    // object).  The result is false if 'TYPE' is a reference.
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
