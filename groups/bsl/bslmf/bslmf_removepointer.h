// bslmf_removepointer.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEPOINTER
#define INCLUDED_BSLMF_REMOVEPOINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to transform pointer type to referent type.
//
//@CLASSES:
//  bsl::remove_pointer: transform a pointer type to its referent pointer
//  bsl::remove_pointer_t: alias to the return type of the meta-function
//
//@SEE_ALSO: bslmf_addpointer
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_pointer',
// that may be used to obtain the type pointed to by a pointer type.
//
// 'bsl::remove_pointer' and 'bsl::remove_pointer_t' meet the requirements of
// the 'remove_pointer' template defined in the C++11 standard
// [meta.trans.ptr].
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
// Finally, if the current compiler supports alias templates C++11 feature, we
// get the type pointed to by 'MyPtrType' using 'bsl::remove_pointer_t' and
// verify that the resulting type is the same as 'MyType':
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  assert((bsl::is_same<bsl::remove_pointer_t<MyPtrType>, MyType>::value));
//#endif
//..
// Note, that the 'bsl::remove_pointer_t' avoids the '::type' suffix and
// 'typename' prefix when we want to use the result of the
// 'bsl::remove_pointer' meta-function in templates.

#include <bslscm_version.h>

#include <bslmf_functionpointertraits.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
    // Permit reliance on transitive includes within robo.
#include <bslmf_removecvq.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslmf {

                         // ========================
                         // struct RemovePointer_Imp
                         // ========================

template <class t_TYPE>
struct RemovePointer_Imp {
    // This 'struct' template provides an alias 'Type' that refers to the type
    // pointed to by the (template parameter) 't_TYPE' if 't_TYPE' is a
    // (non-cv-qualified) pointer type; otherwise, 'Type' refers to 't_TYPE'.
    // This generic default template's 'Type' always refers to 't_TYPE'.  A
    // template specialization (below) handles the case where 't_TYPE' is a
    // pointer type.

    // PUBLIC TYPES
    typedef t_TYPE Type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE'.
};

                     // ==================================
                     // struct RemovePointer_Imp<t_TYPE *>
                     // ==================================

template <class t_TYPE>
struct RemovePointer_Imp<t_TYPE *> {
    // This partial specialization of 'RemovePointer_Imp', for when the
    // (template parameter) 't_TYPE' is a pointer type, provides an alias
    // 'Type' that refers to the type pointed to by 't_TYPE'.

    // PUBLIC TYPES
    typedef t_TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 't_TYPE'.
};

template <class t_TYPE>
struct RemovePointer_Imp<t_TYPE *const> {
    // This partial specialization of 'RemovePointer_Imp', for when the
    // (template parameter) 't_TYPE' is a 'const'-qualified pointer type,
    // provides an alias 'Type' that refers to the type pointed to by 't_TYPE'.
    // Note that this is specifically for 'const'-qualified pointers, and not
    // for pointers-to-'const'-type.

    // PUBLIC TYPES
    typedef t_TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 't_TYPE'.
};

template <class t_TYPE>
struct RemovePointer_Imp<t_TYPE *volatile> {
    // This partial specialization of 'RemovePointer_Imp', for when the
    // (template parameter) 't_TYPE' is a 'volatile'-qualified pointer type,
    // provides an alias 'Type' that refers to the type pointed to by 't_TYPE'.
    // Note that this is specifically for 'volatile'-qualified pointers, and
    // not for pointers-to-'volatile'-type.

    // PUBLIC TYPES
    typedef t_TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 't_TYPE'.
};

template <class t_TYPE>
struct RemovePointer_Imp<t_TYPE *const volatile> {
    // This partial specialization of 'RemovePointer_Imp', for when the
    // (template parameter) 't_TYPE' is a 'const volatile'-qualified pointer
    // type, provides an alias 'Type' that refers to the type pointed to by
    // 't_TYPE'.  Note that this is specifically for 'const volatile'-qualified
    // pointers, and not for pointers-to-'const volatile'-type.

    // PUBLIC TYPES
    typedef t_TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 't_TYPE'.
};

#if defined(BSLS_PLATFORM_CMP_IBM)
template <class t_TYPE, bool isFunctionPtr>
struct RemovePointer_Aix : RemovePointer_Imp<t_TYPE> {
    // The implementation of the 'RemovePointer_Imp' for the AIX xlC 11
    // compiler uses the generic mechanism for non-function types, but see
    // below for a specialization for pointer-to-function types.
};

template <class t_TYPE>
struct RemovePointer_Aix<t_TYPE, true> {
    // The implementation of the 'RemovePointer_Imp' for the AIX xlC 11
    // compiler when 't_TYPE' is a pointer-to-function.  xlC 11 has a bug
    // specializing on 't_TYPE*' where 't_TYPE' is was deduced from the address
    // of a function with default arguments. To workaround the bug, this
    // specialization uses 'FunctionPointerTraits' to obtain the function type
    // by somewhat more complicated means.

    typedef typename FunctionPointerTraits<t_TYPE>::FuncType Type;
};
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
template <class t_TYPE, bool isFunctionPtr = IsFunctionPointer<t_TYPE>::VALUE>
struct RemovePointer_Msvc : RemovePointer_Imp<t_TYPE> {
    // The implementation of the 'RemovePointer_Imp' for the Microsoft Visual
    // C++ compiler which has a bug matching a 'T * const' template parameter
    // with a cv-qualified function pointer type.  To workaround the bug, we
    // provide additional partial specializations predicated on 't_TYPE' being
    // a function pointer type to first strip the cv-qualifier, and then
    // delegate to the regular 'RemovePointer_Imp'.  This yields the correct
    // result withough accidentally stripping the cv-qualifier from non-pointer
    // types.
};

template <class t_TYPE>
struct RemovePointer_Msvc<t_TYPE const, true> : RemovePointer_Imp<t_TYPE> {
};

template <class t_TYPE>
struct RemovePointer_Msvc<t_TYPE volatile, true> : RemovePointer_Imp<t_TYPE> {
};

template <class t_TYPE>
struct RemovePointer_Msvc<t_TYPE const volatile, true>
: RemovePointer_Imp<t_TYPE> {
};
#endif

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // =====================
                         // struct remove_pointer
                         // =====================

template <class t_TYPE>
struct remove_pointer {
    // This 'struct' template implements the 'remove_pointer' meta-function
    // defined in the C++11 standard [meta.trans.ptr], providing an alias,
    // 'type', that returns the result.  If the (template parameter) 't_TYPE'
    // is a (possibly cv-qualified) pointer type, then 'type' is an alias to
    // the type pointed to by 't_TYPE'; otherwise, 'type' is an alias to
    // 't_TYPE'.

#if defined(BSLS_PLATFORM_CMP_IBM)
    typedef typename BloombergLP::bslmf::RemovePointer_Aix<
        t_TYPE,
        BloombergLP::bslmf::IsFunctionPointer<t_TYPE>::VALUE>::Type type;
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    typedef typename BloombergLP::bslmf::RemovePointer_Msvc<t_TYPE>::Type type;
#else
    typedef typename BloombergLP::bslmf::RemovePointer_Imp<t_TYPE>::Type type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 't_TYPE' if 't_TYPE' is a (possibly cv-qualified) pointer
        // type; otherwise, 'type' is an alias to 't_TYPE'.

#endif
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using remove_pointer_t = typename remove_pointer<t_TYPE>::type;
    // 'remove_pointer_t' is an alias to the return type of the
    // 'bsl::remove_pointer' meta-function.  Note, that the 'remove_pointer_t'
    // avoids the '::type' suffix and 'typename' prefix when we want to use the
    // result of the meta-function in templates.

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

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
