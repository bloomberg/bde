// bslmf_arraytopointer.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_ARRAYTOPOINTER
#define INCLUDED_BSLMF_ARRAYTOPOINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to convert array types to pointer types.
//
//@CLASSES:
//  bslmf::ArrayToPointer: convert an array type to a pointer type
//  bslmf::ArrayToConstPointer: convert an array type to a 'const' pointer type
//
//@SEE_ALSO: bslmf_isarray, bslmf_forwardingtype
//
//@DESCRIPTION: This component provides a meta function for converting array
// types to pointer types.  The utility is generally used for in templates that
// require forwarding or storage of arguments that are passed a arrays(e.g.,
// "string literals").  For non array types, the type is left unmodified.  Note
// that 'bslmf::ArrayToPointer' and 'bslmf::ArrayToConstPointer' retain the CV
// qualifiers of the original type.  In other words, if the original array type
// was 'const' or 'volatile', or 'const volatile', the converted pointer type
// will also be 'const', 'volatile', or 'const volatile' respectively.
//
// When an explicit const pointer pointer type is needed(such as when accepting
// as argument, then 'bslmf::ArrayToConstPointer' should be used.
//
///Usage
///-----
// For example:
//..
//  assert(1 == bsl::is_same<bslmf::ArrayToPointer<int[5]>::Type
//                          , int *>::value);
//  assert(1 == bsl::is_same<bslmf::ArrayToPointer<int *>::Type
//                          , int *>::value);
//  assert(0 == bsl::is_same<bslmf::ArrayToPointer<int (*)[5]>::Type]
//                          , int **>::value);
//..

#include <bslscm_version.h>

#include <cstddef>       // for 'std::size_t'

namespace BloombergLP {

namespace bslmf {

                            // =====================
                            // struct ArrayToPointer
                            // =====================

template <class t_TYPE, class t_ORIGINAL_TYPE>
struct ArrayToPointer_Imp;

template <class t_TYPE>
struct ArrayToPointer {
    typedef typename ArrayToPointer_Imp<t_TYPE, t_TYPE>::Type Type;
};

template <class t_TYPE>
struct ArrayToPointer<t_TYPE&> {
    typedef typename ArrayToPointer_Imp<t_TYPE, t_TYPE&>::Type Type;
};

                          // ==========================
                          // struct ArrayToConstPointer
                          // ==========================

template <class t_TYPE>
struct ArrayToConstPointer {
    typedef typename ArrayToPointer_Imp<const t_TYPE, t_TYPE>::Type Type;
};

template <class t_TYPE>
struct ArrayToConstPointer<t_TYPE&> {
    typedef typename ArrayToPointer_Imp<const t_TYPE, t_TYPE&>::Type Type;
};

                         // =========================
                         // struct ArrayToPointer_Imp
                         // =========================

template <class t_TYPE, class t_ORIGINAL_TYPE>
struct ArrayToPointer_Imp {
    typedef t_ORIGINAL_TYPE Type;
};

template <class t_TYPE, std::size_t t_NUM_ELEMENTS, class t_UNUSED>
struct ArrayToPointer_Imp<t_TYPE[t_NUM_ELEMENTS], t_UNUSED> {
    typedef t_TYPE *Type;
};

template <class t_TYPE, class t_UNUSED>
struct ArrayToPointer_Imp<t_TYPE[], t_UNUSED> {
    typedef t_TYPE *Type;
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_ArrayToConstPointer
#undef bslmf_ArrayToConstPointer
#endif
#define bslmf_ArrayToConstPointer bslmf::ArrayToConstPointer
    // This alias is defined for backward compatibility.

#ifdef bslmf_ArrayToPointer
#undef bslmf_ArrayToPointer
#endif
#define bslmf_ArrayToPointer bslmf::ArrayToPointer
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
