// bslma_allocatortraits.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMA_ALLOCATORTRAITS
#define INCLUDED_BSLMA_ALLOCATORTRAITS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a uniform interface to standard allocator types.
//
//@CLASSES:
//  bsl::allocator_traits: Uniform interface to standard allocator types
//
//@SEE_ALSO: bslma_allocator, bslma_stdallocator
//
// TBD: update component-level doc
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
///- - - - - - - - - - - - - -
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
//
//  using namespace BloombergLP;
//
//  template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
//  class MyContainer {
//      // This class provides a container that always holds exactly one
//      // element, dynamically allocated using the specified allocator.
//
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//          // Alias for the 'allocator_traits' instantiation to use for all
//          // memory management requests.
//
//      // DATA
//      ALLOC  d_allocator;
//      TYPE  *d_value_p;
//
//    public:
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
//
//      // etc.
//..
// Next we define the type traits for 'MyContainer' so that it is recognized as
// an STL *sequence* container:
//: o Defines STL iterators
//: o Is bitwise moveable if the allocator is bitwise moveable
//: o Uses 'bslma' allocators if the 'ALLOC' template parameter is convertible
//:   from 'bslma::Allocator*'.
//..
//      // TRAITS
//
//      // We would do the following if 'bslalg' was accessible.
//      // BSLMF_NESTED_TRAIT_DECLARATION(
//      //    MyContainer, bslalg::HasStlIterators);
//
//      BSLMF_NESTED_TRAIT_DECLARATION_IF(
//          MyContainer,
//          bslmf::IsBitwiseMoveable,
//          bslmf::IsBitwiseMoveable<ALLOC>::value);
//
//      BSLMF_NESTED_TRAIT_DECLARATION_IF(
//          MyContainer,
//          bslma::UsesBslmaAllocator,
//          (bsl::is_convertible<bslma::Allocator*, ALLOC>::value));
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
// Next, we perform the actual allocation and construction using the 'allocate'
// and 'construct' members of 'allocator_traits', which provide the correct
// semantic for passing the allocator to the constructed object when
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
///Example 2: C++03 Allocators
///- - - - - - - - - - - - - -
// This example shows that when 'MyContainer' is instantiated with a C++03
// allocator, that the allocator is a) copied on copy construction and b) is
// not propagated from the container to its elements.  Firstly we create a
// representative element class, 'MyType', that allocates memory using the
// bslma allocator protocol:
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
//      MyType(const MyType&)
//          : d_allocator_p(bslma::Default::allocator(0)) { /* ... */ }
//      MyType(const MyType&, bslma::Allocator* basicAlloc)
//         : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
//      // etc.
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const { return d_allocator_p; }
//
//      // etc.
//  };
//..
// Then we create a C++03-style allocator class template:
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
//      template <class OTHER>
//      struct rebind {
//          typedef MyCpp03Allocator<OTHER> other;
//      };
//
//      // CREATORS
//      explicit MyCpp03Allocator(int state = 0) : d_state(state) { }
//
//      // ALLOCATION FUNCTIONS
//      TYPE* allocate(size_type n, const void* = 0)
//          { return static_cast<TYPE *>(::operator new(sizeof(TYPE) * n)); }
//
//      void deallocate(TYPE* p, size_type) { ::operator delete(p); }
//
//      // ELEMENT CREATION FUNCTIONS
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
//      // ACCESSORS
//      static size_type max_size() { return UINT_MAX / sizeof(TYPE); }
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
// allocator is copied on copy-construction:
//..
//  int usageExample2()
//  {
//      typedef MyCpp03Allocator<MyType> MyTypeAlloc;
//
//      MyContainer<MyType, MyTypeAlloc> C1a(MyTypeAlloc(1));
//      assert((bsl::is_same<MyContainer<MyType, MyTypeAlloc>::allocator_type,
//                              MyTypeAlloc>::value));
//      assert(C1a.get_allocator() == MyTypeAlloc(1));
//      assert(C1a.front().allocator() == bslma::Default::defaultAllocator());
//
//      MyContainer<MyType, MyTypeAlloc> C2a(C1a);
//      assert(C2a.get_allocator() == C1a.get_allocator());
//      assert(C2a.get_allocator() != MyTypeAlloc());
//      assert(C2a.front().allocator() == bslma::Default::defaultAllocator());
//
//      MyType                           dummy;
//      MyContainer<MyType, MyTypeAlloc> C1b(dummy, MyTypeAlloc(1));
//      assert((bsl::is_same<MyContainer<MyType, MyTypeAlloc>::allocator_type,
//                              MyTypeAlloc>::value));
//      assert(C1b.get_allocator() == MyTypeAlloc(1));
//      assert(C1b.front().allocator() == bslma::Default::defaultAllocator());
//
//      MyContainer<MyType, MyTypeAlloc> C2b(C1b);
//      assert(C2b.get_allocator() == C1b.get_allocator());
//      assert(C2b.get_allocator() != MyTypeAlloc());
//      assert(C2b.front().allocator() == bslma::Default::defaultAllocator());
//
//      return 0;
//  }
//..

#include <bslscm_version.h>

#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isempty.h>
#include <bslmf_issame.h>
#include <bslmf_util.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Fri May 13 11:05:19 2022
// Command line: sim_cpp11_features.pl bslma_allocatortraits.h
# define COMPILING_BSLMA_ALLOCATORTRAITS_H
# include <bslma_allocatortraits_cpp03.h>
# undef COMPILING_BSLMA_ALLOCATORTRAITS_H
#else

#include <limits>

namespace BloombergLP {
namespace bslma {


                      // ================================
                      // AllocatorTraits_HasIsAlwaysEqual
                      // ================================

template <class ALLOC>
struct AllocatorTraits_HasIsAlwaysEqual {
    // This 'struct' template provides a mechanism for determining whether a
    // given (template parameter) 'ALLOCATOR_TYPE' defines a nested alias named
    //'is_always_equal'.  The static boolean member 'value' (nested alias
    // named 'type') is 'true' ('bsl::true_type') if 'ALLOCATOR_TYPE' defines
    // such an alias, and 'false' ('bsl::false_type') otherwise.

  private:
    // PRIVATE TYPES
    typedef struct { char d_a;    } yes_type;
    typedef struct { char d_a[2]; } no_type;

    // PRIVATE CLASS METHODS
    template <class U>
    static yes_type match(typename U::is_always_equal *);
    template <class U>
    static no_type match(...);
        // Return 'yes_type' if the (template parameter) 'TYPE' defines a
        // nested alias named 'is_always_equal', and 'no_type' otherwise.

  public:
    // PUBLIC CLASS DATA
    static const bool value = sizeof(match<ALLOC>(0)) == sizeof(yes_type);

    // PUBLIC TYPES
    typedef bsl::integral_constant<bool, value> type;
};

                       // =============================
                       // AllocatorTraits_IsAlwaysEqual
                       // =============================

template <class ALLOC, bool = AllocatorTraits_HasIsAlwaysEqual<ALLOC>::value>
struct AllocatorTraits_IsAlwaysEqual : public ALLOC::is_always_equal
{
    // This 'struct' template sets the boolean type for the attribute named
    // 'is_always_equal' to the nested type alias in the given (template
    // parameter) 'ALLOC' if 'ALLOC' defines such an alias (i.e., if
    // 'true == AllocatorTraits_HasIsAlwaysEqual<ALLOCATOR_TYPE>::value').
};

template <class ALLOC>
struct AllocatorTraits_IsAlwaysEqual<ALLOC, false>
                                  : public bsl::is_empty<ALLOC>
{
    // This 'struct' template sets the boolean type for the attribute named
    // 'is_always_equal' to 'bsl::is_empty<ALLOC>' if the given (template
    // parameter) 'ALLOC' does not define such an alias (i.e., if
    // 'false == AllocatorTraits_HasIsAlwaysEqual<ALLOCATOR_TYPE>::value').
};

                   // =====================================
                   // AllocatorTraits_HasSelectOnCopyMethod
                   // =====================================

template <class ALLOCATOR_TYPE>
struct AllocatorTraits_HasSelectOnCopyMethod {
    // This 'struct' template provides a mechanism for determining whether a
    // given (template parameter) 'ALLOCATOR_TYPE' defines a 'const' member
    // function named 'select_on_container_copy_construction' that takes no
    // arguments and returns an 'ALLOCATOR_TYPE' object by value.  The static
    // boolean 'value' (nested 'type' alias) is 'true' ('bsl::true_type') if
    // 'ALLOCATOR_TYPE' defines such a method, and 'false' ('bsl::false_type')
    // otherwise.

  private:
    typedef struct { char a;    } yes_type;
    typedef struct { char a[2]; } no_type;

    template <class T, T> struct MatchType { };
        // This 'struct' template provides a mechanism to check if a type
        // matches an instance within a SFINAE context.

    template <class T>
    struct MethodAlias { typedef T (T::*Method)() const; };

    template <class TYPE>
    static yes_type match(MatchType<typename MethodAlias<TYPE>::Method,
                          &TYPE::select_on_container_copy_construction> *);
    template <class TYPE>
    static no_type match(...);
        // Return 'yes_type' if the (template parameter) 'TYPE' defines a const
        // member function named 'select_on_container_copy_construction' taking
        // no arguments and returning a 'TYPE' object by value, and 'no_type'
        // otherwise.

  public:
    static const bool value =
                          sizeof(match<ALLOCATOR_TYPE>(0)) == sizeof(yes_type);
    typedef bsl::integral_constant<bool, value> type;
};

                    // ===================================
                    // AllocatorTraits_HasPropOnCopyAssign
                    // ===================================

template <class ALLOCATOR_TYPE>
struct AllocatorTraits_HasPropOnCopyAssign {
    // This 'struct' template provides a mechanism for determining whether a
    // given (template parameter) 'ALLOCATOR_TYPE' defines a nested alias named
    //'propagate_on_container_copy_assignment'.  The static boolean member
    // 'value' (nested alias named 'type') is 'true' ('bsl::true_type') if
    // 'ALLOCATOR_TYPE' defines such an alias, and 'false' ('bsl::false_type')
    // otherwise.

  private:
    typedef struct { char a;    } yes_type;
    typedef struct { char a[2]; } no_type;

    template <class U>
    static
    yes_type match(typename U::propagate_on_container_copy_assignment *);
    template <class U>
    static no_type match(...);
        // Return 'yes_type' if the (template parameter) 'TYPE' defines a
        // nested alias named 'propagate_on_container_copy_assignment', and
        // 'no_type' otherwise.

  public:
    static const bool value =
                          sizeof(match<ALLOCATOR_TYPE>(0)) == sizeof(yes_type);
    typedef bsl::integral_constant<bool, value> type;
};

                      // ================================
                      // AllocatorTraits_PropOnCopyAssign
                      // ================================

template <class ALLOCATOR_TYPE,
          bool = AllocatorTraits_HasPropOnCopyAssign<ALLOCATOR_TYPE>::value>
struct AllocatorTraits_PropOnCopyAssign : bsl::false_type
{
    // This 'struct' template sets the boolean type for the attribute named
    // 'propagate_on_container_copy_assignment' to 'bsl::false_type' if the
    // given (template parameter) 'ALLOCATOR_TYPE' does not define such an
    // alias (i.e.,
    // 'false == AllocatorTraits_HasPropOnCopyAssign<ALLOCATOR_TYPE>::value').
};

template <class ALLOC>
struct AllocatorTraits_PropOnCopyAssign<ALLOC, true>
                       : public ALLOC::propagate_on_container_copy_assignment
{
    // This 'struct' template sets the boolean type for the attribute named
    // 'propagate_on_container_copy_assignment' to the nested type alias in the
    // given (template parameter) 'ALLOCATOR_TYPE' if 'ALLOCATOR_TYPE' defines
    // such an alias (i.e.,
    // 'true == AllocatorTraits_HasPropOnCopyAssign<ALLOCATOR_TYPE>::value').
};

                    // ===================================
                    // AllocatorTraits_HasPropOnMoveAssign
                    // ===================================

template <class ALLOC>
struct AllocatorTraits_HasPropOnMoveAssign {
    // This 'struct' template provides a mechanism for determining whether a
    // given (template parameter) 'ALLOCATOR_TYPE' defines a nested alias named
    //'propagate_on_container_move_assignment'.  The static boolean member
    // 'value' (nested alias named 'type') is 'true' ('bsl::true_type') if
    // 'ALLOCATOR_TYPE' defines such an alias, and 'false' ('bsl::false_type')
    // otherwise.

  private:
    typedef struct { char a;    } yes_type;
    typedef struct { char a[2]; } no_type;

    template <class U>
    static
    yes_type match(typename U::propagate_on_container_move_assignment *);
    template <class U>
    static no_type match(...);
        // Return 'yes_type' if the (template parameter) 'TYPE' defines a
        // nested alias named 'propagate_on_container_move_assignment', and
        // 'no_type' otherwise.

  public:
    static const bool value = sizeof(match<ALLOC>(0)) == sizeof(yes_type);
    typedef bsl::integral_constant<bool, value> type;
};

                      // ================================
                      // AllocatorTraits_PropOnMoveAssign
                      // ================================

template <class ALLOC,
          bool = AllocatorTraits_HasPropOnMoveAssign<ALLOC>::value>
struct AllocatorTraits_PropOnMoveAssign : bsl::false_type
{
    // This 'struct' template sets the boolean type for the attribute named
    // 'propagate_on_container_move_assignment' to 'bsl::false_type' if the
    // given (template parameter) 'ALLOCATOR_TYPE' does not define such an
    // alias (i.e.,
    // 'false == AllocatorTraits_HasPropOnMoveAssign<ALLOCATOR_TYPE>::value').
};

template <class ALLOC>
struct AllocatorTraits_PropOnMoveAssign<ALLOC, true>
                       : public ALLOC::propagate_on_container_move_assignment
{
    // This 'struct' template sets the boolean type for the attribute named
    // 'propagate_on_container_move_assignment' to the nested type alias in the
    // given (template parameter) 'ALLOCATOR_TYPE' if 'ALLOCATOR_TYPE' defines
    // such an alias (i.e.,
    // 'true == AllocatorTraits_HasPropOnMoveAssign<ALLOCATOR_TYPE>::value').
};

                       // =============================
                       // AllocatorTraits_HasPropOnSwap
                       // =============================

template <class ALLOC>
struct AllocatorTraits_HasPropOnSwap {
    // This 'struct' template provides a mechanism for determining whether a
    // given (template parameter) 'ALLOCATOR_TYPE' defines a nested alias named
    //'propagate_on_container_swap'.  The static boolean member 'value' (nested
    // alias named 'type') is 'true' ('bsl::true_type') if 'ALLOCATOR_TYPE'
    // defines such an alias, and 'false' ('bsl::false_type') otherwise.

  private:
    typedef struct { char a;    } yes_type;
    typedef struct { char a[2]; } no_type;

    template <class U>
    static
    yes_type match(typename U::propagate_on_container_swap *);
    template <class U>
    static no_type match(...);
        // Return 'yes_type' if the (template parameter) 'TYPE' defines a
        // nested alias named 'propagate_on_container_swap', and 'no_type'
        // otherwise.

  public:
    static const bool value = sizeof(match<ALLOC>(0)) == sizeof(yes_type);
    typedef bsl::integral_constant<bool, value> type;
};

                         // ==========================
                         // AllocatorTraits_PropOnSwap
                         // ==========================

template <class ALLOC, bool = AllocatorTraits_HasPropOnSwap<ALLOC>::value>
struct AllocatorTraits_PropOnSwap : bsl::false_type
{
    // This 'struct' template sets the boolean type for the attribute named
    // 'propagate_on_container_swap' to 'bsl::false_type' if the given
    // (template parameter) 'ALLOCATOR_TYPE' does not define such an alias
    // (i.e., 'false == AllocatorTraits_HasPropOnSwap<ALLOCATOR_TYPE>::value').
};

template <class ALLOC>
struct AllocatorTraits_PropOnSwap<ALLOC, true>
                                  : public ALLOC::propagate_on_container_swap
{
    // This 'struct' template sets the boolean type for the attribute named
    // 'propagate_on_container_swap' to the nested type alias in the given
    // (template parameter) 'ALLOCATOR_TYPE' if 'ALLOCATOR_TYPE' defines such
    // an alias (i.e.,
    // 'false == AllocatorTraits_HasPropOnSwap<ALLOCATOR_TYPE>::value').
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) &&                        \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

                     // ==================================
                     // AllocatorTraits_HasConstructMethod
                     // ==================================

template <class T, class Return, class... Args>
struct AllocatorTraits_HasConstructMethod {
  private:
    template <class U>
    static auto match(U *) ->
        typename bsl::is_same<decltype(bslmf::Util::declval<U>().construct(
                                             bslmf::Util::declval<Args>()...)),
                              Return>::type;
    template <class>
    static bsl::false_type match(...);

  public:
    typedef decltype(match<T>(0)) type;
    static const bool value = type::value;
};

                      // ================================
                      // AllocatorTraits_HasDestroyMethod
                      // ================================

template <class T, class Return, class... Args>
struct AllocatorTraits_HasDestroyMethod {
  private:
    template <class U>
    static auto match(U *) ->
        typename bsl::is_same<decltype(bslmf::Util::declval<U>().destroy(
                                             bslmf::Util::declval<Args>()...)),
                              Return>::type;
    template <class>
    static bsl::false_type match(...);

  public:
    typedef decltype(match<T>(0)) type;
    static const bool value = type::value;
};

#endif

                        // ===========================
                        // AllocatorTraits_PointerType
                        // ===========================

template <class T, class = void>
struct AllocatorTraits_PointerType {
    typedef typename T::value_type *type;
};


template <class T>
struct AllocatorTraits_PointerType<T, BSLMF_VOIDTYPE(typename T::pointer)> {
    typedef typename T::pointer type;
};

                      // ================================
                      // AllocatorTraits_ConstPointerType
                      // ================================

template <class T, class = void>
struct AllocatorTraits_ConstPointerType {
    typedef const typename T::value_type *type;
        // should be pointer_traits::rebind of template above
};


template <class T>
struct AllocatorTraits_ConstPointerType<
                                   T,
                                   BSLMF_VOIDTYPE(typename T::const_pointer)> {
    typedef typename T::const_pointer type;
};


                      // ===============================
                      // AllocatorTraits_VoidPointerType
                      // ===============================

template <class T, class = void>
struct AllocatorTraits_VoidPointerType {
    typedef void *type;
        // should be pointer_traits::rebind of template above
};


template <class T>
struct AllocatorTraits_VoidPointerType<
                                   T,
                                   BSLMF_VOIDTYPE(typename T::void_pointer)> {
    typedef typename T::void_pointer type;
};

                    // ====================================
                    // AllocatorTraits_ConstVoidPointerType
                    // ====================================

template <class T, class = void>
struct AllocatorTraits_ConstVoidPointerType {
    typedef const void *type;
        // should be pointer_traits::rebind of template above
};


template <class T>
struct AllocatorTraits_ConstVoidPointerType<
                             T,
                             BSLMF_VOIDTYPE(typename T::const_void_pointer)> {
    typedef typename T::const_void_pointer type;
};

                          // ========================
                          // AllocatorTraits_SizeType
                          // ========================

template <class T, class = void>
struct AllocatorTraits_SizeType {
    typedef std::size_t type;
};


template <class T>
struct AllocatorTraits_SizeType<T, BSLMF_VOIDTYPE(typename T::size_type)> {
    typedef typename T::size_type type;
};

                       // ==============================
                       // AllocatorTraits_DifferenceType
                       // ==============================

template <class T, class = void>
struct AllocatorTraits_DifferenceType {
    // should be pointer_traits::rebind of template above
    typedef std::ptrdiff_t type;

};


template <class T>
struct AllocatorTraits_DifferenceType<
                             T,
                             BSLMF_VOIDTYPE(typename T::difference_type)> {
    typedef typename T::difference_type type;
};

                        // ===========================
                        // AllocatorTraits_RebindFront
                        // ===========================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
template <class T, class U>
struct AllocatorTraits_RebindFront {
    // There shall be no member named 'type' unless T is a class template with
    // only type parameters.
};

template <template <class, class...> class ALLOC,
	  class T,
	  class ...ARGS,
	  class U>
struct AllocatorTraits_RebindFront<ALLOC<T, ARGS...>, U> {
    using type = ALLOC<U, ARGS...>;
};
#else
template <class T, class U>
struct AllocatorTraits_RebindFront {
    // There shall be no member named 'type' unless T is a class template with
    // only type parameters.
};

template <template <class> class ALLOC,
	  class T,
	  class U>
struct AllocatorTraits_RebindFront<ALLOC<T>, U> {
    typedef ALLOC<U> type;
};
#endif

                        // ===========================
                        // AllocatorTraits_RebindAlloc
                        // ===========================

template <class T, class U, class = void>
struct AllocatorTraits_RebindAlloc {
    // should be pointer_traits::rebind of template above
    typedef typename AllocatorTraits_RebindFront<T, U>::type type;

};

template <class T, class U>
struct AllocatorTraits_RebindAlloc<
                      T,
                      U,
                      BSLMF_VOIDTYPE(typename T::template rebind<U>::other)> {
    typedef typename T::template rebind<U>::other type;
};

                        // ===========================
                        // AllocatorTraits_CallMaxSize
                        // ===========================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
template <class T, class = void>
struct AllocatorTraits_CallMaxSize {

    // PUBLIC TYPES
    typedef typename AllocatorTraits_SizeType<T>::type SizeType;

    // PUBLIC CLASS METHODS
    static SizeType max_size(const T &)
        // Return the maximum size of the specified (template) parameter 'T'.
        // Also note that this method is defined inline to work around a
        // Windows compiler bug with SFINAE functions.
    {
        return std::numeric_limits<SizeType>::max() /
               sizeof(typename T::value_type);
    }
};

// Due to the dependence on expression SFINAE to detect the presence of a
// 'max_size' member of the allocator, this is only done on more modern
// platforms.
template <class T>
struct AllocatorTraits_CallMaxSize<
    T,
    BSLMF_VOIDTYPE(decltype(bslmf::Util::declval<T>().max_size()))> {

    // PUBLIC TYPES
    typedef typename AllocatorTraits_SizeType<T>::type SizeType;

    // PUBLIC CLASS METHODS
    static SizeType max_size(const T &alloc)
        // Return the maximum size of the specified 'alloc'.  Also note that
        // this method is defined inline to work around a Windows compiler bug
        // with SFINAE functions.
    {
        return alloc.max_size();
    }
};
#endif

} // close namespace bslma
} // close enterprise namespace

namespace bsl {

                           // ======================
                           // class allocator_traits
                           // ======================

template <class ALLOCATOR_TYPE>
struct allocator_traits {
    // This class supports the complete interface of the C++11
    // 'allocator_traits' class template, which provides a uniform mechanism
    // for accessing nested types within, and operations on, any
    // standard-conforming allocator.  A specialization of this class template
    // for 'bsl::allocator' provides support for Bloomberg's 'bslma' allocator
    // model (see the 'bslma_stdallocator' component for more details).  In
    // C++11 compilation environments, the 'construct' methods forward to the
    // allocator's 'construct' method if such a method matching the (variable
    // number of) specified constructor arguments exists; otherwise, the
    // 'construct' method falls back to invoking the constructor of the element
    // type directly.  In C++03 compilation environments, there is no reliable
    // way to detect if the type provide a method that matches a (variable
    // number of) specified arguments; therefore, we require that standard
    // allocator types define 'construct' methods taking a variable number of
    // arguments in those environments.  This implementation is not
    // fully-standard-conforming in that it does not support deduce data types
    // that are not specified in the allocator.

  private:

    typedef typename BloombergLP::bslma::AllocatorTraits_HasSelectOnCopyMethod<
                                    ALLOCATOR_TYPE>::type DelegateSelectMethod;

    static
    ALLOCATOR_TYPE selectOnCopyConstruct(const ALLOCATOR_TYPE& stdAllocator,
                                         true_type);
        // Return the result of invoking the
        // 'select_on_container_copy_construction' method on the specified
        // 'stdAllocator'.

    static
    ALLOCATOR_TYPE selectOnCopyConstruct(const ALLOCATOR_TYPE& stdAllocator,
                                         false_type);
        // Return the specified 'stdAllocator'.  Note that this behavior
        // enforces a default policy of propagating the allocator on copy
        // construction when using a standard allocator.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) &&                        \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
    template <class ELEMENT_TYPE, class... Args>
    static typename bsl::enable_if<
        BloombergLP::bslma::AllocatorTraits_HasConstructMethod<ALLOCATOR_TYPE,
                                                               void,
                                                               ELEMENT_TYPE *,
                                                               Args...>::value,
        void>::type
    privateConstruct(ALLOCATOR_TYPE&  basicAllocator,
                     ELEMENT_TYPE    *elementAddr,
                     Args&&...        arguments);
    template <class ELEMENT_TYPE, class... Args>
    static typename bsl::enable_if<
        !BloombergLP::bslma::AllocatorTraits_HasConstructMethod<
            ALLOCATOR_TYPE,
            void,
            ELEMENT_TYPE *,
            Args...>::value,
        void>::type
    privateConstruct(ALLOCATOR_TYPE&  basicAllocator,
                     ELEMENT_TYPE    *elementAddr,
                     Args&&...        arguments);
        // Construct an object of (template parameter) type 'ELEMENT_TYPE' at
        // the specified 'elementAddr', either by 1) calling the 'construct'
        // method on 'basicAllocator' with 'elemAddr' and the specified
        // (variable number of) 'arguments' if the (template parameter) type
        // 'ALLOCATOR_TYPE' defines such a method, or 2) forwarding the
        // specified (variable number of) 'arguments' to the constructor of
        // 'ELEMENT_TYPE' directly (and ignoring 'basicAllocator') otherwise.
        // The behavior is undefined unless 'elementAddr' refers to valid,
        // uninitialized storage.

    template <class ELEMENT_TYPE>
    static typename bsl::enable_if<
        BloombergLP::bslma::AllocatorTraits_HasDestroyMethod<
            ALLOCATOR_TYPE,
            void,
            ELEMENT_TYPE *>::value,
        void>::type
    privateDestroy(ALLOCATOR_TYPE& basicAllocator, ELEMENT_TYPE *elementAddr);

    template <class ELEMENT_TYPE>
    static typename bsl::enable_if<
        !BloombergLP::bslma::AllocatorTraits_HasDestroyMethod<
            ALLOCATOR_TYPE,
            void,
            ELEMENT_TYPE *>::value,
        void>::type
    privateDestroy(ALLOCATOR_TYPE& basicAllocator, ELEMENT_TYPE *elementAddr);
        // Destroy the object of (template parameter) type 'ELEMENT_TYPE' at
        // the specified 'elementAddr', either by 1) calling the 'destroy'
        // method on 'basicAllocator' with 'elemAddr' as the sole argument if
        // the (template parameter) type 'ALLOCATOR_TYPE' defines such a
        // method, or 2) calling the destructor directly on 'elementAddr' (and
        // ignoring 'basicAllocator') otherwise.  The behavior is undefined
        // unless 'elementAddr' refers to a valid, constructed object.
#endif

  public:
    // PUBLIC TYPES
    typedef ALLOCATOR_TYPE                            allocator_type;
    typedef typename ALLOCATOR_TYPE::value_type       value_type;

    typedef typename
         BloombergLP::bslma::AllocatorTraits_PointerType<ALLOCATOR_TYPE>::type
	                                              pointer;
    typedef typename
    BloombergLP::bslma::AllocatorTraits_ConstPointerType<ALLOCATOR_TYPE>::type
	                                              const_pointer;
    typedef typename
     BloombergLP::bslma::AllocatorTraits_VoidPointerType<ALLOCATOR_TYPE>::type
	                                              void_pointer;
    typedef typename BloombergLP::bslma::
                    AllocatorTraits_ConstVoidPointerType<ALLOCATOR_TYPE>::type
	                                              const_void_pointer;

    typedef typename
      BloombergLP::bslma::AllocatorTraits_DifferenceType<ALLOCATOR_TYPE>::type
	                                              difference_type;
    typedef typename
	    BloombergLP::bslma::AllocatorTraits_SizeType<ALLOCATOR_TYPE>::type
	                                              size_type;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    template <class ELEMENT_TYPE>
    using rebind_alloc = typename
           BloombergLP::bslma::AllocatorTraits_RebindAlloc<ALLOCATOR_TYPE,
                                                           ELEMENT_TYPE>::type;

    template <class ELEMENT_TYPE>
    using rebind_traits = allocator_traits<rebind_alloc<ELEMENT_TYPE>>;
#else // !BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    template <class ELEMENT_TYPE>
    struct rebind_alloc
        : BloombergLP::bslma::AllocatorTraits_RebindAlloc<ALLOCATOR_TYPE,
                                                          ELEMENT_TYPE>::type
    {
        // Note that this class attempts to emulate an alias template, but is
        // not complete.  In general, code that must support C++03 should use
        // 'rebind_traits<ELEMENT_TYPE>::allocator_type' instead of
        // 'rebind_alloc<ELEMENT_TYPE>' because that nested typedef is the
        // preferred actual allocator type and not a subclass of the desired
        // type.

        typedef typename BloombergLP::bslma::
                AllocatorTraits_RebindAlloc<ALLOCATOR_TYPE, ELEMENT_TYPE>::type
	                                                        allocator_type;

        template <typename ARG>
        rebind_alloc(const ARG& allocatorArg)
            // Convert from anything that can be used to cosntruct the base
            // type.  This might be better if SFINAE-ed out using
            // 'is_convertible', but stressing older compilers more seems
            // unwise.
        : allocator_type(allocatorArg)
        {
        }
    };

    template <class ELEMENT_TYPE>
    struct rebind_traits : allocator_traits<typename allocator_traits::template
                                    rebind_alloc<ELEMENT_TYPE>::allocator_type>
    {
    };
#endif // !BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

    // Allocation functions

    static pointer allocate(ALLOCATOR_TYPE& basicAllocator, size_type n);
        // Return 'basicAllocator.allocate(n)'.

    static pointer allocate(ALLOCATOR_TYPE&    basicAllocator,
                            size_type          n,
                            const_void_pointer hint);
        // Return 'basicAllocator.allocate(n, hint)'.

    static void deallocate(ALLOCATOR_TYPE& basicAllocator,
                           pointer         elementAddr,
                           size_type       n);
        // Invoke 'basicAllocator.deallocate(elementAddr, n)'.  The behavior is
        // undefined unless the specified 'elementAddr' was returned from a
        // prior call to the 'allocate' method of an allocator that compares
        // equal to the specified 'allocator', and has not yet been passed to a
        // 'deallocate' call of such an allocator object.

    // Element creation functions

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class ELEMENT_TYPE, class... Args>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                          Args&&...        arguments);
        // Construct an object of (template parameter) type 'ELEMENT_TYPE' at
        // the specified 'elementAddr', either by 1) calling the 'construct'
        // method on 'basicAllocator' with 'elemAddr' and the specified
        // (variable number of) 'arguments' if the (template parameter) type
        // 'ALLOCATOR_TYPE' defines such a method, or 2) forwarding the
        // specified (variable number of) 'arguments' to the constructor of
        // 'ELEMENT_TYPE' directly (and ignoring 'basicAllocator') otherwise.
        // The behavior is undefined unless 'elementAddr' refers to valid,
        // uninitialized storage.
#endif

    template <class ELEMENT_TYPE>
    static void destroy(ALLOCATOR_TYPE&  basicAllocator,
                        ELEMENT_TYPE    *elementAddr);
        // Destroy the object of (template parameter) type 'ELEMENT_TYPE' at
        // the specified 'elementAddr', either by 1) calling the 'destroy'
        // method on 'basicAllocator' with 'elemAddr' as the sole argument if
        // the (template parameter) type 'ALLOCATOR_TYPE' defines such a
        // method, or 2) calling the destructor directly on 'elementAddr' (and
        // ignoring 'basicAllocator') otherwise.  The behavior is undefined
        // unless 'elementAddr' refers to a valid, constructed object.

    static size_type max_size(const ALLOCATOR_TYPE& basicAllocator)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return the largest number of 'value_type' objects that could
        // reasonably be returned by a single invocation of 'allocate' for the
        // specified 'allocator', i.e., 'allocator.max_size()'.

    // Allocator propagation traits
    static ALLOCATOR_TYPE
    select_on_container_copy_construction(const ALLOCATOR_TYPE& rhs);
        // Return a copy of the allocator that should be used to copy-
        // construct one container from another container whose allocator is
        // the specified 'rhs'.  If the parameterized 'ALLOCATOR_TYPE' defines
        // a method 'select_on_container_copy_construction', this function
        // returns the result of calling that method on 'rhs'; otherwise, this
        // method enforces the default policy of propagating the allocator on
        // copy construction, as is standard practice for standard allocators
        // (i.e., returns 'rhs').  Note that the specialization of this class
        // template for 'bsl::allocator' (in the 'bslma_stdallocator'
        // component) provides the alternate default behavior of *not*
        // propagating the allocator on copy construction (i.e., returning a
        // default-constructed allocator object).

    typedef typename BloombergLP::bslma::AllocatorTraits_IsAlwaysEqual<
                             ALLOCATOR_TYPE>::type is_always_equal;
        // Identical to, or derived from 'true_type' if two allocators of
        // parameterized 'ALLOCATOR_TYPE' always compare equal; otherwise
        // identical to or derived from 'false_type'.  This type is
        // 'ALLOCATOR_TYPE::is_always_equal' if such a type is defined, and
        // 'is_empty<ALLOCATOR_TYPE>' otherwise.

    typedef typename BloombergLP::bslma::AllocatorTraits_PropOnCopyAssign<
                  ALLOCATOR_TYPE>::type propagate_on_container_copy_assignment;
        // Identical to, or derived from 'true_type' if an allocator of
        // parameterized 'ALLOCATOR_TYPE' should be copied when a container
        // using that 'ALLOCATOR_TYPE' is copy-assigned; otherwise identical to
        // or derived from 'false_type'.  This type is
        // 'ALLOCATOR_TYPE::propagate_on_container_copy_assignment' if such a
        // type is defined, and 'false_type' otherwise.

    typedef typename BloombergLP::bslma::AllocatorTraits_PropOnMoveAssign<
                  ALLOCATOR_TYPE>::type propagate_on_container_move_assignment;
        // Identical to, or derived from 'true_type' if an allocator of
        // parameterized 'ALLOCATOR_TYPE' should be moved when a container
        // using that 'ALLOCATOR_TYPE' is move-assigned; otherwise identical to
        // or derived from 'false_type'.  This type is
        // 'ALLOCATOR_TYPE::propagate_on_container_move_assignment' if such a
        // type is defined, and 'false_type' otherwise.

    typedef typename BloombergLP::bslma::AllocatorTraits_PropOnSwap<
                             ALLOCATOR_TYPE>::type propagate_on_container_swap;
        // Identical to, or derived from 'true_type' if the allocators of
        // parameterized 'ALLOCATOR_TYPE' should be swapped when containers
        // using that 'ALLOCATOR_TYPE' are swapped; otherwise identical to or
        // derived from 'false_type'.  This type is
        // 'ALLOCATOR_TYPE::propagate_on_container_swap' if such a type is
        // defined, and 'false_type' otherwise.
};

                  // ========================================
                  // class allocator_traits<ALLOCATOR_TYPE *>
                  // ========================================

template <class ALLOCATOR_TYPE>
struct allocator_traits<ALLOCATOR_TYPE *> {
    // TBD: improve comment This is an empty class specialization of
    // 'allocator_traits' for pointer types that (intentionally) does not
    // define any of the traits typedefs.  It's needed in order make
    // unambiguous function overloads that take both a standard allocator by
    // value and a 'bslma::Allocator *'.  By using the typedefs defined in
    // 'allocator_traits' in the signature of functions taking standard
    // allocators, we can ensure that those overloads are not considered when
    // using 'bslma'-style allocators.
};

}  // close namespace bsl

// ============================================================================
//          INLINE AND TEMPLATE STATIC MEMBER FUNCTION DEFINITIONS
// ============================================================================


namespace bsl {

                           // ----------------------
                           // class allocator_traits
                           // ----------------------


template <class ALLOCATOR_TYPE>
inline
ALLOCATOR_TYPE allocator_traits<ALLOCATOR_TYPE>::selectOnCopyConstruct(
                                            const ALLOCATOR_TYPE& stdAllocator,
                                            true_type)
{
    return stdAllocator.select_on_container_copy_construction();
}

template <class ALLOCATOR_TYPE>
inline
ALLOCATOR_TYPE allocator_traits<ALLOCATOR_TYPE>::selectOnCopyConstruct(
                                            const ALLOCATOR_TYPE& stdAllocator,
                                            false_type)
{
    return stdAllocator;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) &&                        \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class... Args>
inline
typename bsl::enable_if<
    BloombergLP::bslma::AllocatorTraits_HasConstructMethod<ALLOCATOR_TYPE,
                                                           void,
                                                           ELEMENT_TYPE *,
                                                           Args...>::value,
    void>::type
allocator_traits<ALLOCATOR_TYPE>::privateConstruct(
                                               ALLOCATOR_TYPE&  basicAllocator,
                                               ELEMENT_TYPE    *elementAddr,
                                               Args&&...        arguments)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class... Args>
inline
typename bsl::enable_if<
    !BloombergLP::bslma::AllocatorTraits_HasConstructMethod<ALLOCATOR_TYPE,
                                                            void,
                                                            ELEMENT_TYPE *,
                                                            Args...>::value,
    void>::type
allocator_traits<ALLOCATOR_TYPE>::privateConstruct(ALLOCATOR_TYPE&,
                                                   ELEMENT_TYPE *elementAddr,
                                                   Args&&...     arguments)
{
    ::new (static_cast<void *>(elementAddr))
        ELEMENT_TYPE(BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE>
inline
typename bsl::enable_if<BloombergLP::bslma::AllocatorTraits_HasDestroyMethod<
                            ALLOCATOR_TYPE,
                            void,
                            ELEMENT_TYPE *>::value,
                        void>::type
allocator_traits<ALLOCATOR_TYPE>::privateDestroy(
                                               ALLOCATOR_TYPE&  basicAllocator,
                                               ELEMENT_TYPE    *elementAddr)
{
    basicAllocator.destroy(elementAddr);
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE>
inline
typename bsl::enable_if<!BloombergLP::bslma::AllocatorTraits_HasDestroyMethod<
                            ALLOCATOR_TYPE,
                            void,
                            ELEMENT_TYPE *>::value,
                        void>::type
allocator_traits<ALLOCATOR_TYPE>::privateDestroy(ALLOCATOR_TYPE&,
                                                 ELEMENT_TYPE *elementAddr)
{
    elementAddr->~ELEMENT_TYPE();
}
#endif

// Allocation functions

template <class ALLOCATOR_TYPE>
inline
typename allocator_traits<ALLOCATOR_TYPE>::pointer
allocator_traits<ALLOCATOR_TYPE>::allocate(ALLOCATOR_TYPE& basicAllocator,
                                           size_type n)
{
    return basicAllocator.allocate(n);
}

template <class ALLOCATOR_TYPE>
inline
typename allocator_traits<ALLOCATOR_TYPE>::pointer
allocator_traits<ALLOCATOR_TYPE>::allocate(ALLOCATOR_TYPE&    basicAllocator,
                                           size_type          n,
                                           const_void_pointer hint)
{
    return basicAllocator.allocate(n, hint);
}

template <class ALLOCATOR_TYPE>
inline
void
allocator_traits<ALLOCATOR_TYPE>::deallocate(ALLOCATOR_TYPE& basicAllocator,
                                             pointer         elementAddr,
                                             size_type       n)
{
    basicAllocator.deallocate(elementAddr, n);
}

// ELEMENT CREATION FUNCTIONS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class... Args>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                                            Args&&...        arguments)
{
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
    privateConstruct(basicAllocator,
                     elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
#else
    // Cannot sniff out whether 'basicAllocator.construct(...)' is valid in
    // C++03, but allocators are required to have a 'construct' method, so just
    // call it.
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
#endif
}
#endif

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE>
inline
void
allocator_traits<ALLOCATOR_TYPE>::destroy(ALLOCATOR_TYPE& stdAllocator,
                                          ELEMENT_TYPE    *elementAddr)
{
//  For full C++11 compatibility, this should check for the well-formedness of
//  the allocator-specific code that is commented out below (via some SFINAE
//  trickery), and switch to the 'DestructionUtil' implementation only if the
// 'destroy' member function is not available.

//    allocator.destroy(elementAddr);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) &&                        \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    privateDestroy(stdAllocator, elementAddr);
#else
    elementAddr->~ELEMENT_TYPE();
    (void) stdAllocator;
#endif
}

template <class ALLOCATOR_TYPE>
inline
typename allocator_traits<ALLOCATOR_TYPE>::size_type
allocator_traits<ALLOCATOR_TYPE>::max_size(
                    const ALLOCATOR_TYPE& basicAllocator) BSLS_KEYWORD_NOEXCEPT
{
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)
    return BloombergLP::bslma::
	 AllocatorTraits_CallMaxSize<ALLOCATOR_TYPE>::max_size(basicAllocator);
#else
    // Cannot sniff out whether 'basicAllocator.max_size()' is valid in C++03,
    // but for now require that allocators have a 'max_size' method and just
    // call it.
    return basicAllocator.max_size();
#endif
}

template <class ALLOCATOR_TYPE>
inline
ALLOCATOR_TYPE
allocator_traits<ALLOCATOR_TYPE>::select_on_container_copy_construction(
                                                     const ALLOCATOR_TYPE& rhs)
{
    return selectOnCopyConstruct(rhs, DelegateSelectMethod());
}

}  // close namespace bsl

#endif // End C++11 code

#endif // ! defined(INCLUDED_BSLMA_ALLOCATORTRAITS)

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
