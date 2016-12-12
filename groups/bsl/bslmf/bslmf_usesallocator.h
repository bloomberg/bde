// bslmf_usesallocator.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_USESALLOCATOR
#define INCLUDED_BSLMF_USESALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to determine if a type uses an allocator.
//
//@CLASSES:
//  bsl::uses_allocator: meta-fn to check if a type uses a certain allocator
//
//@SEE_ALSO: bslmf_isconvertible
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::uses_allocator',
// that may be used to query whether a given type uses a given allocator type.
//
// 'bsl::uses_allocator' meets the requirements of the 'uses_allocator'
// template defined in the C++11 standard [allocator.uses.trait], in addition
// to providing a welcome availability in both C++03 and C++11 compilation
// environments.
//
// A type 'T' uses an allocator type 'A' if 'A' has a nested alias named
// 'allocator_type' and 'A' is convertible to 'allocator_type' (as defined in
// the 'bslmf_isconvertible' component).  If a type 'T' uses an allocator type
// 'A', then 'T' has a constructor that takes either 1) 'allocator_arg_t' as a
// first argument and 'A' as a second argument, or 2) 'A' as the last argument.
// Alternatively, the 'uses_allocator' template may be specialized for a type
// 'T' that does not have a nested alias named 'allocator_type', where 'T'
// can be constructed with 'A' as detailed above.
//
///Usage
///-----
// In this section we show intended use of this component.
//
// TBD: finish up usage example, add to test driver.
//
///Example 1: Determine If a Type Uses an Allocator
///- - - - - - - - - - - - - - - - - - - - - - - - 
//
//..
// template <class CONTAINER>
// class ContainerAdaptor {
//     // ...
//   public:
//     ContainerAdaptor();
//         // Create an empty container adaptor.  No allocator will be provided
//         // to the underlying container, and the container's memory
//         // allocation will be provided by whatever is the default for the
//         // container type.
//
//     template <class ALLOC>
//     explicit
//     ContainerAdaptor(const ALLOC& basicAllocator,
//                      typename bsl::enable_if<
//                                bsl::uses_allocator<CONTAINER, ALLOC>::value,
//                                ALLOC>::type * = 0);
//         // Create an empty container adaptor, and use the specified
//         // 'basicAllocator' to supply memory.  Note that this constructor is
//         // available only when the type of the argument is compatible with
//         // the allocator type associated with the container.
//
//     // ...
// };
//..
//

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace BloombergLP {
namespace bslmf {

                 // =====================================
                 // struct UsesAllocator_HasAllocatorType
                 // =====================================
 
template <class TYPE>
struct UsesAllocator_HasAllocatorType {
    // This 'struct' template provides a mechanism for determining whether a
    // given type defines a nested type named 'allocator_type'.  The 'value' is
    // 'true' if the (template parameter) 'TYPE' provides such an alias, and
    // 'false' otherwise.

  private:
    typedef struct { char a;    } yes_type;
    typedef struct { char a[2]; } no_type;

    template <class BDE_OTHER_TYPE>
    static yes_type match(typename BDE_OTHER_TYPE::allocator_type *);
    template <class BDE_OTHER_TYPE>
    static no_type match(...);
        // Return 'yes_type' if the (template parameter) 'BDE_OTHER_TYPE'
        // defines a type named 'allocator_type', and 'no_type' otherwise.

  public:
    static const bool value = sizeof(match<TYPE>(0)) == sizeof(yes_type);
    typedef bsl::integral_constant<bool, value> type;
};

template <class TYPE,
          class ALLOC,
          bool = UsesAllocator_HasAllocatorType<TYPE>::value>
struct UsesAllocator_Imp : bsl::false_type
    // This 'struct' template derives from 'bsl::false_type' when the (template
    // parameter) type 'TYPE' does not have a nested alias 'allocator_type'.
{
};

template <class TYPE, class ALLOC>
struct UsesAllocator_Imp<TYPE, ALLOC, true>
    // This 'struct' template derives from 'bsl::true_type' when the (template
    // parameter) 'TYPE' has a nested alias 'allocator_type' and the (template
    // parameter) type 'ALLOC' is convertible to 'TYPE::allocator_type', and
    // 'bsl::false_type' otherwise.
{
    static const bool value =
              bsl::is_convertible<ALLOC, typename TYPE::allocator_type>::value;
    typedef bsl::integral_constant<bool, value> type;
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                        // =====================
                        // struct uses_allocator
                        // =====================

template <class TYPE, class ALLOCATOR_TYPE>
struct uses_allocator
: BloombergLP::bslmf::UsesAllocator_Imp<TYPE, ALLOCATOR_TYPE>::type {
    // This 'struct' template implements a meta-function to determine whether
    // a (template parameter) 'TYPE' uses a given (template parameter) 
    // 'ALLOCATOR_TYPE'.  This 'struct' derives from 'bsl::true_type' if
    // 'TYPE' uses 'ALLOCATOR_TYPE' and from 'bsl::false_type' otherwise.  This
    // meta-function has the same syntax as the 'uses_allocator' meta-function
    // defined in the C++11 standard [allocator.uses.trait].
 
};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
