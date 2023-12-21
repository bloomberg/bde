// bslma_hasallocatortype.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_HASALLOCATORTYPE
#define INCLUDED_BSLMA_HASALLOCATORTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for an 'allocator_type' typedef
//
//@CLASSES:
//  bslma::HasAllocatorType: Metafunction for detecting nested 'allocator_type'
//
//@SEE_ALSO: bslmf_usesallocator
//
//@DESCRIPTION: This component defines a meta-function,
// 'bslma::HasAllocatorType' that may be used to query whether a type
// advertizes, through the use of an 'allocator_type' typedef, its use of an
// allocator.  An 'allocator_type' typedef is deemed to indicate that a type
// meets the requirements for being considered allocator aware (AA) following
// the C++ Standard Library conventions for AA containers.
//
// An instantiation 'bslma::HasAllocatorType<TYPE>' is derived from
// 'bsl::true_type' if 'TYPE' has a nested type named 'allocator_type' that
// meets the requirements for a C++11 allocator, and 'bsl::false_type'
// otherwise.  In particular, an 'allocator_type' of 'void' does not meet these
// requirements; class templates idiomatically use (public) 'typedef void
// allocator_type;' to prevent accidentally inheriting this trait from one of
// its base classes.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Conditional 'allocator_type'
///- - - - - - - - - - - - - - - - - - - -
// In this example, we create a wrapper class, 'Wrapper', that defines a nested
// 'allocator_type' if and only if the type it wraps has an 'allocator_type'.
// First, we forward-declare a base class template, 'Wrapper_CondAllocType',
// parameterized on whether or the wrapped class is allocator-aware (AA).
//..
//  template <class TYPE, bool IS_AA> struct Wrapper_CondAllocType;
//..
// Next, we specialize the base class for the non-AA ('false') case, providing
// no 'allocator_type' nested member:
//..
//  template <class TYPE>
//  struct Wrapper_CondAllocType<TYPE, false> {
//  };
//..
// Then, we specialize it for the AA ('true') case, using the 'allocator_type'
// from the wrapped type:
//..
//  template <class TYPE>
//  struct Wrapper_CondAllocType<TYPE, true> {
//      typedef typename TYPE::allocator_type allocator_type;
//  };
//..
// Next, we define our wrapper class to inherit the correct specialization of
// 'Wrapper_CondAllocType' by specifying the result of 'HasAllocatorType' for
// the 'IS_AA' argument:
//..
//  template <class TYPE>
//  class Wrapper
//      : public Wrapper_CondAllocType<TYPE,
//                                     bslma::HasAllocatorType<TYPE>::value> {
//      // ...
//  };
//..
// Now, to test our work, we define a minimal allocator type, 'MyAlloc':
//..
//  template <class TYPE>
//  struct MyAlloc {
//      BSLMF_NESTED_TRAIT_DECLARATION(MyAlloc, bslma::IsStdAllocator);
//
//      typedef TYPE value_type;
//
//      MyAlloc();
//      template<class T> MyAlloc(const MyAlloc<T>& other);
//
//      TYPE* allocate(std::size_t n);
//      void deallocate(TYPE* p, std::size_t n);
//  };
//..
// Next, we define a class type, 'AAType', that uses 'MyAlloc':
//..
//  struct AAType {
//      typedef MyAlloc<int> allocator_type;
//      //..
//  };
//..
// Finally, we can verify that any instantiation of 'Wrapper' on 'AAType' is
// itself AA, whereas instantiations on, e.g., 'int' or pointer types are not:
//..
//  int main()
//  {
//      assert(bslma::HasAllocatorType<        AAType  >::value);
//      assert(bslma::HasAllocatorType<Wrapper<AAType> >::value);
//      assert((bsl::is_same<Wrapper<AAType>::allocator_type,
//                           MyAlloc<int> >::value));
//
//      assert(! bslma::HasAllocatorType<        int     >::value);
//      assert(! bslma::HasAllocatorType<Wrapper<int   > >::value);
//
//      assert(! bslma::HasAllocatorType<        char *  >::value);
//      assert(! bslma::HasAllocatorType<Wrapper<char *> >::value);
//  }
//..


#include <bslscm_version.h>

#include <bslma_isstdallocator.h>

#include <bslmf_integralconstant.h>
#include <bslmf_voidtype.h>

namespace BloombergLP {
namespace bslma {

                    // =================================
                    // struct template HasHallocatorType
                    // =================================


template <class TYPE, class = void>
struct HasAllocatorType : bsl::false_type {
    // This metafunction yields 'bsl::true_type' if the specified
    // template-parameter 'TYPE' has a nested type named 'allocator_type' that
    // meets the requirements for a C++11 allocator, and 'bsl::false_type'
    // otherwise.  This primary template, yielding 'false_type', is
    // instantiated for types for which 'allocator_type' does not exist or is
    // not an allocator type.  Note that instantiation will fail if
    // 'allocator_type' exists but is not public within 'TYPE' or if any of the
    // required members of 'TYPE::allocator_type' exist but are not public
    // (though only 'value_type' and 'allocate' are tested at present).
};

template <class TYPE>
struct HasAllocatorType<TYPE,
                        BSLMF_VOIDTYPE(typename TYPE::allocator_type)>
    : bslma::IsStdAllocator<typename TYPE::allocator_type>::type {
    // Partial specialization of 'HasAllocatorType' yielding 'true_type' is
    // instantiated for 'TYPE's that *do* have an 'allocator_type' member that
    // meets the requirements of a C++11 allocator type.
};

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_HASALLOCATORTYPE)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
