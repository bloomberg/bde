// bslstl_allocatortraits.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#define INCLUDED_BSLSTL_ALLOCATORTRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a uniform interface to standard allocator types.
//
//@DEPRECATED: Use 'bslma_allocatortraits' instead.
//
//@CLASSES:
//  bsl::allocator_traits: Uniform interface to standard allocator types
//
//@SEE_ALSO: bslma_allocator, bslma_stdallocator
//
//@DESCRIPTION: The standard 'allocator_traits' class template is defined in
// the C++11 standard ([allocator.traits]) as a uniform mechanism for accessing
// nested types within, and operations on, any standard-conforming allocator.
// An 'allocator_traits' specialization is stateless, and all of its member
// functions are static.  In most cases, facilities of 'allocator_traits' are
// straight pass-throughs for the same facilities from the 'ALLOC' template
// parameter.  For example, 'allocator_traits<X>::pointer' is the same as
// 'X::pointer' and 'allocator_traits<X>::allocate(x, n)' is the same as
// 'x.allocate(n)'.  The advantage of using 'allocator_traits' instead of
// directly using the allocator is that the 'allocator_traits' interface can
// supply parts of the interface that are missing from 'ALLOC'.  In fact, the
// most important purpose of 'allocator_traits' is to provide implementations
// of C++11 allocator features that were absent in C++03, thus allowing a C++03
// allocator to work with C++11 containers.
//
// This component provides a full C++11 interface for 'allocator_traits', but
// constrains the set of allocator types on which it may be instantiated.
// Specifically, this implementation does not provide defaults for C++03 types
// and functions, and has hard-wired implementations of the new C++11 features.
// Thus, the 'allocator_traits' template cannot be instantiated on an allocator
// type that does not provide a full compliment of types and functions required
// by the C++03 standard, and it will ignore any special C++11 features
// specified in 'ALLOC'.  This limitation exists because Bloomberg does not
// need the full functionality of the C++11 model, but needs only to
// distinguish between C++03 allocators and allocators that implement the BSLMA
// allocator model (see {'bslma_stdallocator'}).  The full feature set of
// 'allocator_traits' would require a lot of resources for implementation and
// (especially) testing.  Moreover, a full implementation would require
// metaprogramming that is too advanced for the feature set of the compilers
// currently in use at Bloomberg.  This interface is useful, however, as a way
// to future-proof containers against the eventual implementation of the full
// feature set, and to take advantage of the Bloomberg-specific features
// described below.
//
// There are two important (new) C++11 features provided by the
// 'allocator_traits' interface: the 'construct' function having a
// variable-length argument list (limited to 5 constructor arguments on
// compilers that don't support variadic templates) and the
// allocator-propagation traits.  The implementations of these features within
// this component are tuned to Bloomberg's needs.  The 'construct' member
// function will automatically forward the allocator to the constructed object
// iff the 'ALLOC' parameter is convertible from 'bslma::Allocator*' and the
// object being constructed has the 'bslma::UsesBslmaAllocator' type trait, as
// per standard Bloomberg practice.  The
// 'select_on_container_copy_construction' static member will return a
// default-constructed allocator iff 'ALLOC' is convertible from
// 'bslma::Allocator *' because bslma allocators should not be copied when a
// container is copy-constructed; otherwise this function will return a copy of
// the allocator, as per C++03 container rules.  The other propagation traits
// all have a 'false' value, so allocators are not propagated on assignment or
// swap.
//
// Note that use of this component will differ from a strict following of the
// C++03 standard, as the 'construct' and 'destroy' methods of the
// parameterized allocator type will not be called.  Rather, the target object
// will always be constructed at the address specified by the user, by calling
// the constructor in-place.  Similarly, the destructor will always be called
// directly, rather than using a parameterized allocator's 'destroy' method.
// Otherwise, this implementation will fully support the C++03 model, including
// use of allocators returning "smart pointers" from 'allocate'.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: A Container Class
/// - - - - - - - - - - - - - -
// This example demonstrates the intended use of 'allocator_traits' to
// implement a standard-conforming container class.  First, we create a
// container class that holds a single object and which meets the requirements
// both of a standard container and of a Bloomberg container.  I.e., when
// instantiated with an allocator argument it uses the standard allocator
// model; otherwise it uses the 'bslma' model.  We provide an alias,
// 'AllocTraits', to the specific 'allocator_traits' instantiation to simplify
// the implementation of each method that must allocate memory, or create or
// destroy elements.
//..
//  #include <bslma_allocatortraits.h>
//  #include <bslma_stdallocator.h>
//
//  using namespace BloombergLP;
//
//  template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
//  class MyContainer {
//      // This class provides a container that always holds exactly one
//      // element, dynamically allocated using the specified allocator.
//
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//          // Alias for the 'allocator_traits' instantiation to use for
//          // all memory management requests.
//
//      // DATA
//      ALLOC  d_allocator;
//      TYPE  *d_value_p;
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(MyContainer, bslalg::HasStlIterators);
//      BSLMF_NESTED_TRAIT_DECLARATION_IF(MyContainer,
//                                        bslma::UsesBslmaAllocator,
//                                        (bsl::is_convertible<Allocator*,
//                                                             ALLOC>::value));
//      BSLMF_NESTED_TRAIT_DECLARATION_IF(MyContainer,
//                                     bslmf::IsBitwiseMoveable,
//                                     bslmf::IsBitwiseMoveable<ALLOC>::value);
//          // Declare nested type traits for this class.
//
//      typedef TYPE  value_type;
//      typedef ALLOC allocator_type;
//      // etc.
//
//      // CREATORS
//      explicit MyContainer(const ALLOC& a = ALLOC());
//      explicit MyContainer(const TYPE& v, const ALLOC& a = ALLOC());
//      MyContainer(const MyContainer& other);
//      MyContainer(const MyContainer& other, const ALLOC& a);
//      ~MyContainer();
//
//      // MANIPULATORS
//      ALLOC get_allocator() const { return d_allocator; }
//
//      // ACCESSORS
//      TYPE&       front()       { return *d_value_p; }
//      const TYPE& front() const { return *d_value_p; }
//      // etc.
//  };
//..
// Then we implement the constructors, which allocate memory and construct a
// 'TYPE' object in the allocated memory.  Because the allocation and
// construction are done in two separate steps, we need to create a proctor
// that will deallocate the allocated memory in case the constructor throws an
// exception.  The proctor uses the uniform interface provided by
// 'allocator_traits' to access the 'pointer' and 'deallocate' members of
// 'ALLOC':
//..
//  template <class ALLOC>
//  class MyContainerProctor {
//      // This class implements a proctor to release memory allocated during
//      // the construction of a 'MyContainer' object if the constructor for
//      // the container's data element throws an exception.  Such a proctor
//      // should be 'release'd once the element is safely constructed.
//
//      typedef typename bsl::allocator_traits<ALLOC>::pointer pointer;
//      ALLOC   d_alloc;
//      pointer d_data_p;
//
//    public:
//      MyContainerProctor(const ALLOC& a, pointer p)
//          : d_alloc(a), d_data_p(p) { }
//
//      ~MyContainerProctor() {
//          if (d_data_p) {
//              bsl::allocator_traits<ALLOC>::deallocate(d_alloc, d_data_p, 1);
//          }
//      }
//
//      void release() { d_data_p = pointer(); }
//  };
//..
// Next, we perform the actual allocation and construction using the
// 'allocate' and 'construct' members of 'allocator_traits', which provide the
// correct semantic for passing the allocator to the constructed object when
// appropriate:
//..
//  template <class TYPE, class ALLOC>
//  MyContainer<TYPE, ALLOC>::MyContainer(const ALLOC& a)
//      : d_allocator(a)
//  {
//      d_value_p = AllocTraits::allocate(d_allocator, 1);
//      MyContainerProctor<ALLOC> proctor(a, d_value_p);
//      // Call 'construct' with no constructor arguments
//      AllocTraits::construct(d_allocator, d_value_p);
//      proctor.release();
//  }
//
//  template <class TYPE, class ALLOC>
//  MyContainer<TYPE, ALLOC>::MyContainer(const TYPE& v, const ALLOC& a)
//      : d_allocator(a)
//  {
//      d_value_p = AllocTraits::allocate(d_allocator, 1);
//      MyContainerProctor<ALLOC> proctor(a, d_value_p);
//      // Call 'construct' with one constructor argument of type 'TYPE'
//      AllocTraits::construct(d_allocator, d_value_p, v);
//      proctor.release();
//  }
//..
// Next, the copy constructor for 'MyContainer' needs to conditionally copy the
// allocator from the 'other' container.  The copy constructor uses
// 'allocator_traits::select_on_container_copy_construction' to decide whether
// to copy the 'other' allocator (for non-bslma allocators) or to
// default-construct the allocator (for bslma allocators).
//..
//  template <class TYPE, class ALLOC>
//  MyContainer<TYPE, ALLOC>::MyContainer(const MyContainer& other)
//      : d_allocator(bsl::allocator_traits<ALLOC>::
//                    select_on_container_copy_construction(other.d_allocator))
//  {
//      d_value_p = AllocTraits::allocate(d_allocator, 1);
//      MyContainerProctor<ALLOC> proctor(d_allocator, d_value_p);
//      AllocTraits::construct(d_allocator, d_value_p, *other.d_value_p);
//      proctor.release();
//  }
//..
// Now, the destructor uses 'allocator_traits' functions to destroy and
// deallocate the value object:
//..
//  template <class TYPE, class ALLOC>
//  MyContainer<TYPE, ALLOC>::~MyContainer()
//  {
//      AllocTraits::destroy(d_allocator, d_value_p);
//      AllocTraits::deallocate(d_allocator, d_value_p, 1);
//  }
//..
// Finally, we perform a simple test of 'MyContainer', instantiating it with
// element type 'int':
//..
//  int usageExample1()
//  {
//      bslma::TestAllocator testAlloc;
//      MyContainer<int> C1(123, &testAlloc);
//      assert(C1.get_allocator() == bsl::allocator<int>(&testAlloc));
//      assert(C1.front() == 123);
//
//      MyContainer<int> C2(C1);
//      assert(C2.get_allocator() == bsl::allocator<int>());
//      assert(C2.front() == 123);
//
//      return 0;
//  }
//..
//
///Example 2: 'bslma' Allocator Propagation
/// - - - - - - - - - - - - - - - - - - - -
// To exercise the propagation of the allocator of 'MyContainer' to its
// elements, we first create a representative element class, 'MyType', that
// allocates memory using the bslma allocator protocol:
//..
//  #include <bslma_default.h>
//
//  class MyType {
//
//      bslma::Allocator *d_allocator_p;
//      // etc.
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(MyType, bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit MyType(bslma::Allocator* basicAlloc = 0)
//         : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
//      MyType(const MyType& other)
//          : d_allocator_p(bslma::Default::allocator(0)) { /* ... */ }
//      MyType(const MyType& other, bslma::Allocator* basicAlloc)
//         : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
//      // etc.
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const { return d_allocator_p; }
//      // etc.
//  };
//..
// Finally, we instantiate 'MyContainer' using 'MyType' and verify that, when
// we provide a the address of an allocator to the constructor of the
// container, the same address is passed to the constructor of the container's
// element.  We also verify that, when the container is copy-constructed, the
// copy uses the default allocator, not the allocator from the original;
// moreover, we verify that the element stored in the copy also uses the
// default allocator.
//..
//  #include <bslmf_issame.h>
//
//  int usageExample2()
//  {
//      bslma::TestAllocator testAlloc;
//      MyContainer<MyType> C1(&testAlloc);
//      assert((bsl::is_same<MyContainer<MyType>::allocator_type,
//              bsl::allocator<MyType> >::value));
//      assert(C1.get_allocator() == bsl::allocator<MyType>(&testAlloc));
//      assert(C1.front().allocator() == &testAlloc);
//
//      MyContainer<MyType> C2(C1);
//      assert(C2.get_allocator() != C1.get_allocator());
//      assert(C2.get_allocator() == bsl::allocator<MyType>());
//      assert(C2.front().allocator() != &testAlloc);
//      assert(C2.front().allocator() == bslma::Default::defaultAllocator());
//
//      return 0;
//  }
//..
//
///Example 3: C++03 Allocators
///- - - - - - - - - - - - - -
// This example shows that when 'MyContainer' is instantiated with a C++03
// allocator, that the allocator is a) copied on copy construction and b) is
// not propagated from the container to its elements.  First, we create a
// C++03-style allocator class template:
//..
//  template <class TYPE>
//  class MyCpp03Allocator {
//      int d_state;
//
//    public:
//      typedef TYPE        value_type;
//      typedef TYPE       *pointer;
//      typedef const TYPE *const_pointer;
//      typedef unsigned    size_type;
//      typedef int         difference_type;
//
//      template <class U>
//      struct rebind {
//          typedef MyCpp03Allocator<U> other;
//      };
//
//      explicit MyCpp03Allocator(int state = 0) : d_state(state) { }
//
//      TYPE* allocate(size_type n, const void* = 0)
//          { return (TYPE*) ::operator new(sizeof(TYPE) * n); }
//
//      void deallocate(TYPE* p, size_type) { ::operator delete(p); }
//
//      static size_type max_size() { return UINT_MAX / sizeof(TYPE); }
//
//      template <class ELEMENT_TYPE>
//      void construct(ELEMENT_TYPE *p)
//      {
//          ::new (static_cast<void *>(p)) ELEMENT_TYPE();
//      }
//      template <class ELEMENT_TYPE, class A1>
//      void construct(ELEMENT_TYPE *p,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A1) a1)
//      {
//          ::new (static_cast<void *>(p))
//              ELEMENT_TYPE(BSLS_COMPILERFEATURES_FORWARD(A1, a1));
//      }
//      template <class ELEMENT_TYPE, class A1, class A2>
//      void construct(ELEMENT_TYPE *p,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A1) a1,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A2) a2)
//      {
//          ::new (static_cast<void *>(p))
//              ELEMENT_TYPE(BSLS_COMPILERFEATURES_FORWARD(A1, a1),
//                           BSLS_COMPILERFEATURES_FORWARD(A2, a2));
//      }
//  
//      template <class ELEMENT_TYPE, class A1, class A2, class A3>
//      void construct(ELEMENT_TYPE *p,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A1) a1,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A2) a2,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A3) a3)
//      {
//          ::new (static_cast<void *>(p))
//              ELEMENT_TYPE(BSLS_COMPILERFEATURES_FORWARD(A1, a1),
//                           BSLS_COMPILERFEATURES_FORWARD(A2, a2),
//                           BSLS_COMPILERFEATURES_FORWARD(A3, a3));
//      }
//  
//      template <class ELEMENT_TYPE, class A1, class A2, class A3, class A4>
//      void construct(ELEMENT_TYPE *p,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A1) a1,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A2) a2,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A3) a3,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A4) a4)
//      {
//          ::new (static_cast<void *>(p))
//              ELEMENT_TYPE(BSLS_COMPILERFEATURES_FORWARD(A1, a1),
//                           BSLS_COMPILERFEATURES_FORWARD(A2, a2),
//                           BSLS_COMPILERFEATURES_FORWARD(A3, a3),
//                           BSLS_COMPILERFEATURES_FORWARD(A4, a4));
//      }
//  
//      template <class ELEMENT_TYPE,
//                class A1,
//                class A2,
//                class A3,
//                class A4,
//                class A5>
//      void construct(ELEMENT_TYPE *p,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A1) a1,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A2) a2,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A3) a3,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A4) a4,
//                     BSLS_COMPILERFEATURES_FORWARD_REF(A5) a5)
//      {
//          ::new (static_cast<void *>(p))
//              ELEMENT_TYPE(BSLS_COMPILERFEATURES_FORWARD(A1, a1),
//                           BSLS_COMPILERFEATURES_FORWARD(A2, a2),
//                           BSLS_COMPILERFEATURES_FORWARD(A3, a3),
//                           BSLS_COMPILERFEATURES_FORWARD(A4, a4),
//                           BSLS_COMPILERFEATURES_FORWARD(A5, a5));
//      }
//  
//      template <class ELEMENT_TYPE>
//      void destroy(ELEMENT_TYPE *p) { p->~ELEMENT_TYPE(); }
//  
//      int state() const { return d_state; }
//  };
//
//  template <class TYPE1, class TYPE2>
//  inline
//  bool operator==(const MyCpp03Allocator<TYPE1>& lhs,
//                  const MyCpp03Allocator<TYPE2>& rhs)
//  {
//      return lhs.state() == rhs.state();
//  }
//
//  template <class TYPE1, class TYPE2>
//  inline
//  bool operator!=(const MyCpp03Allocator<TYPE1>& lhs,
//                  const MyCpp03Allocator<TYPE2>& rhs)
//  {
//      return ! (lhs == rhs);
//  }
//..
// Finally we instantiate 'MyContainer' using this allocator type and verify
// that elements are constructed using the default allocator (because the
// allocator is not propagated from the container).  We also verify that the
// allocator is copied on copy construction:
//..
//  int usageExample3()
//  {
//      typedef MyCpp03Allocator<MyType> MyTypeAlloc;
//      MyContainer<MyType, MyTypeAlloc> C1(MyTypeAlloc(1));
//      assert((bsl::is_same<MyContainer<MyType, MyTypeAlloc>::allocator_type,
//                           MyTypeAlloc>::value));
//      assert(C1.get_allocator() == MyTypeAlloc(1));
//      assert(C1.front().allocator() == bslma::Default::defaultAllocator());
//
//      MyContainer<MyType, MyTypeAlloc> C2(C1);
//      assert(C2.get_allocator() == C1.get_allocator());
//      assert(C2.get_allocator() != MyTypeAlloc())
//      assert(C2.front().allocator() == bslma::Default::defaultAllocator());
//
//      return 0;
//   }
//..

// TBD: We are going to go ahead and remove this for now as an alternative to
// duplicating the 'bslalg_arrayprimitives' component since this component is
// included by 'bslalg_arrayprimitives' and we have clients that include
// directly the 'bslalg_arrayprimitives' component in 'BSL_OVERRIDES_STD' mode.
#if 0
// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslma_allocatortraits.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATORTRAITS
#include <bslma_allocatortraits.h>
#endif

namespace BloombergLP {

} // close enterprise namespace

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
