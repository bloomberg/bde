// bslmf_removepointer.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEPOINTER
#define INCLUDED_BSLMF_REMOVEPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to transform pointer type to referent type.
//
//@CLASSES:
//  bsl::remove_pointer: transform a pointer type to its referent pointer
//
//@SEE_ALSO: bslmf_addpointer
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_pointer',
// that may be used to obtain the type pointed to by a pointer type.
//
// 'bsl::remove_pointer' meets the requirements of the 'remove_pointer'
// template defined in the C++11 standard [meta.trans.ptr].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Get the Type Pointed to by a Pointer Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to get the type pointed to by a pointer type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType') and the type
// pointed to by the pointer type ('MyType'):
//..
//  typedef int  MyType;
//  typedef int *MyPtrType;
//..
// Now, we get the type pointed to by 'MyPtrType' using 'bsl::remove_pointer'
// and verify that the resulting type is the same as 'MyType':
//..
//  assert((bsl::is_same<bsl::remove_pointer<MyPtrType>::type,
//                       MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
    // Permit reliance on transitive includes within robo.
#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslmf {

                         // ========================
                         // struct RemovePointer_Imp
                         // ========================

template <class TYPE>
struct RemovePointer_Imp {
    // This 'struct' template provides an alias 'Type' that refers to the type
    // pointed to by the (template parameter) 'TYPE' if 'TYPE' is a
    // (non-cv-qualified) pointer type; otherwise, 'Type' refers to 'TYPE'.
    // This generic default template's 'Type' always refers to 'TYPE'.  A
    // template specialization (below) handles the case where 'TYPE' is a
    // pointer type.

    // PUBLIC TYPES
    typedef TYPE Type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE'.
};

                         // ================================
                         // struct RemovePointer_Imp<TYPE *>
                         // ================================

template <class TYPE>
struct RemovePointer_Imp<TYPE *> {
     // This partial specialization of 'RemovePointer_Imp', for when the
     // (template parameter) 'TYPE' is a pointer type, provides an alias 'Type'
     // that refers to the type pointed to by 'TYPE'.

    // PUBLIC TYPES
    typedef TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE'.
};

template <class TYPE>
struct RemovePointer_Imp<TYPE *const> {
     // This partial specialization of 'RemovePointer_Imp', for when the
     // (template parameter) 'TYPE' is a 'const'-qualified pointer type,
     // provides an alias 'Type' that refers to the type pointed to by 'TYPE'.
     // Note that this is specifically for 'const'-qualified pointers, and not
     // for pointers-to-'const'-type.

    // PUBLIC TYPES
    typedef TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE'.
};

template <class TYPE>
struct RemovePointer_Imp<TYPE *volatile> {
     // This partial specialization of 'RemovePointer_Imp', for when the
     // (template parameter) 'TYPE' is a 'volatile'-qualified pointer type,
     // provides an alias 'Type' that refers to the type pointed to by 'TYPE'.
     // Note that this is specifically for 'volatile'-qualified pointers, and
     // not for pointers-to-'volatile'-type.

    // PUBLIC TYPES
    typedef TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE'.
};

template <class TYPE>
struct RemovePointer_Imp<TYPE *const volatile> {
     // This partial specialization of 'RemovePointer_Imp', for when the
     // (template parameter) 'TYPE' is a 'const volatile'-qualified pointer
     // type, provides an alias 'Type' that refers to the type pointed to by
     // 'TYPE'.  Note that this is specifically for 'const volatile'-qualified
     // pointers, and not for pointers-to-'const volatile'-type.

    // PUBLIC TYPES
    typedef TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE'.
};

#if defined(BSLS_PLATFORM_CMP_IBM)
template <class TYPE, bool isFunctionPtr>
struct RemovePointer_Aix : RemovePointer_Imp<TYPE> {
    // The implementation of the 'RemovePointer_Imp' for the AIX xlC 11
    // compiler uses the generic mechanism for non-function types, but see
    // below for a specialization for pointer-to-function types.
};

template <class TYPE>
struct RemovePointer_Aix<TYPE, true> {
    // The implementation of the 'RemovePointer_Imp' for the AIX xlC 11
    // compiler when 'TYPE' is a pointer-to-function.  xlC 11 has a bug
    // specializing on 'TYPE*' where 'TYPE' is was deduced from the address of
    // a function with default arguments. To workaround the bug, this
    // specialization uses 'FunctionPointerTraits' to obtain the function
    // type by somewhat more complicated means.

    typedef typename FunctionPointerTraits<TYPE>::FuncType Type;
};
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
template <class TYPE, bool isFunctionPtr = IsFunctionPointer<TYPE>::VALUE>
struct RemovePointer_Msvc : RemovePointer_Imp<TYPE> {
    // The implementation of the 'RemovePointer_Imp' for the Microsoft Visual
    // C++ compiler which has a bug matching a 'T * const' template parameter
    // with a cv-qualified function pointer type.  To workaround the bug, we
    // provide additional partial specializations predicated on 'TYPE' being a
    // function pointer type to first strip the cv-qualifier, and then delegate
    // to the regular 'RemovePointer_Imp'.  This yields the correct result
    // withough accidentally stripping the cv-qualifier from non-pointer types.
};

template <class TYPE>
struct RemovePointer_Msvc<TYPE const, true>
     : RemovePointer_Imp<TYPE> {
};

template <class TYPE>
struct RemovePointer_Msvc<TYPE volatile, true>
     : RemovePointer_Imp<TYPE> {
};

template <class TYPE>
struct RemovePointer_Msvc<TYPE const volatile, true>
     : RemovePointer_Imp<TYPE> {
};
#endif

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // =====================
                         // struct remove_pointer
                         // =====================

template <class TYPE>
struct remove_pointer {
    // This 'struct' template implements the 'remove_pointer' meta-function
    // defined in the C++11 standard [meta.trans.ptr], providing an alias,
    // 'type', that returns the result.  If the (template parameter) 'TYPE' is
    // a (possibly cv-qualified) pointer type, then 'type' is an alias to the
    // type pointed to by 'TYPE'; otherwise, 'type' is an alias to 'TYPE'.

#if defined(BSLS_PLATFORM_CMP_IBM)
    typedef typename BloombergLP::bslmf::RemovePointer_Aix<TYPE,
                BloombergLP::bslmf::IsFunctionPointer<TYPE>::VALUE>::Type type;
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    typedef typename BloombergLP::bslmf::RemovePointer_Msvc<TYPE>::Type type;
#else
    typedef typename BloombergLP::bslmf::RemovePointer_Imp<TYPE>::Type type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE' if 'TYPE' is a (possibly cv-qualified) pointer
        // type; otherwise, 'type' is an alias to 'TYPE'.

#endif
};

}  // close namespace bsl

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
