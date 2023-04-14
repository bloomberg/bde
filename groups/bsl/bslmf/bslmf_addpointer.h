// bslmf_addpointer.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDPOINTER
#define INCLUDED_BSLMF_ADDPOINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide meta-function to transform a type to pointer to that type.
//
//@CLASSES:
//  bsl::add_pointer: meta-function to transform a type to a pointer type
//  bsl::add_pointer_t: alias to the return type of the 'bsl::add_pointer'
//
//@SEE_ALSO: bslmf_removepointer
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_pointer',
// that may be used to transform a type to a pointer to that type.
//
// 'bsl::add_pointer' meets the requirements of the 'add_pointer' template
// defined in the C++11 standard [meta.trans.ptr].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform Type to Pointer Type to that Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a type to a pointer type to that type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType') and the type
// pointed to by the pointer type ('MyType'):
//..
//  typedef int   MyType;
//  typedef int * MyPtrType;
//..
// Now, we transform 'MyType' to a pointer type using 'bsl::add_pointer' and
// verify that the resulting type is the same as 'MyPtrType':
//..
//  assert((bsl::is_same<bsl::add_pointer<MyType>::type, MyPtrType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// transform 'MyType' to a pointer type using 'bsl::add_pointer_t' and verify
// that the resulting type is the same as 'MyPtrType':
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  assert((bsl::is_same<bsl::add_pointer_t<MyType>, MyPtrType>::value));
//#endif
//..
// Note, that the 'bsl::add_pointer_t' avoids the '::type' suffix and
// 'typename' prefix when we want to use the result of the 'bsl::add_pointer'
// meta-function in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stddef.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_removereference.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslmf {

struct AddPointer_Compute {
    // This utility 'struct' is a private implementation detail that hosts an
    // overloaded pair of functions that, through SFINAE, can determine whether
    // it is legal to form a pointer to a specified 't_TYPE'.

    struct LargeResult {
        char d_dummy[99];
    };

    template <class t_TYPE>
    static LargeResult canFormPointer(t_TYPE *);

    template <class t_TYPE>
    static char canFormPointer(...);
};

template <class t_TYPE,
          size_t = sizeof(AddPointer_Compute::canFormPointer<t_TYPE>(0))>
struct AddPointer_Impl {
    // For the majority of types, it is perfectly reasonable to form the type
    // 't_TYPE *'.

    typedef t_TYPE *type;  // A pointer to the original 't_TYPE'.
};

template <class t_TYPE>
struct AddPointer_Impl<t_TYPE, 1u> {
    // For special cases, such as references and "abominable" functions, it is
    // not legal to form a pointer, and this parital specialization will be
    // chosen by the computed default template parameter.

    typedef t_TYPE type;  // Do not modify the type if a pointer is not valid.
};

#if defined(BSLS_PLATFORM_CMP_IBM)
template <class t_TYPE>
struct AddPointer_Impl<t_TYPE[], 1u> {
    // IBM miscomputes the SFINAE condition for arrays of unknown bound, so we
    // provide an additional partial specialization for this platform.

    typedef t_TYPE (*type)[];     // A pointer to the original 't_TYPE[]'.
};
#endif

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ==================
                         // struct add_pointer
                         // ==================

template <class t_TYPE>
struct add_pointer {
    // This 'struct' template implements the 'add_pointer' meta-function
    // defined in the C++11 standard [meta.trans.ptr], providing an alias,
    // 'type', that returns the result.  If the (template parameter) 't_TYPE'
    // is not a reference type, then 'type' is an alias to a pointer type that
    // points to 't_TYPE'; otherwise, 'type' is an alias to a pointer type that
    // points to the type referred to by the reference 't_TYPE', unless it is
    // not legal to form such a pointer type, in which case 'type' is an alias
    // for 't_TYPE'.

    typedef typename BloombergLP::bslmf::AddPointer_Impl<t_TYPE>::type type;
        // This 'typedef' is an alias to a pointer type that points to the
        // (template parameter) 't_TYPE' if it is not a reference type;
        // otherwise, this 'typedef' is an alias to a pointer type that points
        // to the type referred to by the reference 't_TYPE'.
};

template <class t_TYPE>
struct add_pointer<t_TYPE&> {
    // If we can form a reference to 't_TYPE', then we can also form a pointer
    // to it.  In particular, we know that it is not 'void' (which should still
    // work) or an "abominable" function type with a trailing cv-qualifier.
    // Note that this partial specialization is necessary to avoid falling into
    // degenerate (non-compiling) cases in the implementation meta-program.

    typedef t_TYPE * type;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
struct add_pointer<t_TYPE&&> {
    // If we can form a reference to 't_TYPE', then we can also form a pointer
    // to it.  In particular, we know that it is not 'void' (which should still
    // work) or an "abominable" function type with a trailing cv-qualifier.
    // Note that this partial specialization is necessary to avoid falling into
    // degenerate (non-compiling) cases in the implementation meta-program.

    typedef t_TYPE * type;
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

//ALIASES
template <class t_TYPE>
using add_pointer_t = typename add_pointer<t_TYPE>::type;
    // 'add_pointer_t' is an alias to the return type of the 'bsl::add_pointer'
    // meta-function.  Note, that the 'remove_pointer_t' avoids the '::type'
    // suffix and 'typename' prefix when we want to use the result of the
    // meta-function in templates.

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
