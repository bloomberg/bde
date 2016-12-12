// bslma_allocatortraits.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_ALLOCATORTRAITS
#define INCLUDED_BSLMA_ALLOCATORTRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_UTIL
#include <bslmf_util.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace BloombergLP {
namespace bslma {

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
        // matches an instance within a sfinae context.

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
    // 'ALLOCATOR_TYPE' defines such an alias, and 'false'
    // ('bsl::false_type') otherwise.

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
    // 'ALLOCATOR_TYPE' defines such an alias, and 'false'
    // ('bsl::false_type') otherwise.
 
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)                           \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

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
    // number of) specified constuctor arguments exists; otherwise, the
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)                           \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
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
        // method, or 2) calling the destructor directly on 'elementAddr'
        // (and ignoring 'basicAllocator') otherwise.  The behavior is
        // undefined unless 'elementAddr' refers to a valid, constructed
        // object.
#endif

  public:
    // PUBLIC TYPES
    typedef ALLOCATOR_TYPE                            allocator_type;
    typedef typename ALLOCATOR_TYPE::value_type       value_type;

    typedef typename ALLOCATOR_TYPE::pointer          pointer;
    typedef typename ALLOCATOR_TYPE::const_pointer    const_pointer;
    typedef void                                     *void_pointer;
    typedef void const                               *const_void_pointer;
    typedef typename ALLOCATOR_TYPE::difference_type  difference_type;
    typedef typename ALLOCATOR_TYPE::size_type        size_type;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    template <class ELEMENT_TYPE>
    using rebind_alloc =
                 typename ALLOCATOR_TYPE::template rebind<ELEMENT_TYPE>::other;

    template <class ELEMENT_TYPE>
    using rebind_traits = allocator_traits<rebind_alloc<ELEMENT_TYPE>>;
#else // !BDE_CXX11_TEMPLATE_ALIASES
    template <class ELEMENT_TYPE>
    struct rebind_alloc : public ALLOCATOR_TYPE::template rebind<ELEMENT_TYPE>::other
    {
    };

    template <class ELEMENT_TYPE>
    struct rebind_traits : allocator_traits<typename ALLOCATOR_TYPE::template
                                            rebind<ELEMENT_TYPE>::other>
    {
    };
#endif // !BDE_CXX11_TEMPLATE_ALIASES

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
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_allocatortraits.h
    template <class ELEMENT_TYPE>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr);

    template <class ELEMENT_TYPE, class Args_01>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class ELEMENT_TYPE, class Args_01,
                                  class Args_02,
                                  class Args_03,
                                  class Args_04,
                                  class Args_05,
                                  class Args_06,
                                  class Args_07,
                                  class Args_08,
                                  class Args_09,
                                  class Args_10,
                                  class Args_11,
                                  class Args_12,
                                  class Args_13,
                                  class Args_14>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class ELEMENT_TYPE, class... Args>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif

    template <class ELEMENT_TYPE>
    static void destroy(ALLOCATOR_TYPE&  basicAllocator,
                        ELEMENT_TYPE    *elementAddr);
        // Destroy the object of (template parameter) type 'ELEMENT_TYPE' at
        // the specified 'elementAddr', either by 1) calling the 'destroy'
        // method on 'basicAllocator' with 'elemAddr' as the sole argument if
        // the (template parameter) type 'ALLOCATOR_TYPE' defines such a
        // method, or 2) calling the destructor directly on 'elementAddr'
        // (and ignoring 'basicAllocator') otherwise.  The behavior is
        // undefined unless 'elementAddr' refers to a valid, constructed
        // object.

    static size_type max_size(const ALLOCATOR_TYPE& basicAllocator);
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
    // TBD: improve comment
    // This is an empty class specialization of 'allocator_traits' for pointer
    // types that (intentionally) does not define any of the traits typedefs.
    // It's needed in order make unambiguous function overloads that take both
    // a standard allocator by value and a 'blmsa::Allocator *'.  By using
    // the typedefs defined in 'allocator_traits' in the signature of functions
    // taking standard allocators, we can ensure that those overloads are not
    // considered when using 'bslma'-style allocators.
};

// ============================================================================
//          INLINE AND TEMPLATE STATIC MEMBER FUNCTION DEFINITIONS
// ============================================================================

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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)                           \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
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
    privateConstruct(basicAllocator,
                     elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    // basicAllocator.construct(
    //     elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_allocatortraits.h
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr)
{
    basicAllocator.construct(
        elementAddr);
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                     BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                     BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                     BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                     BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                     BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                     BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                     BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                     BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                     BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                     BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                     BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                     BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                     BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                     BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
                     BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                     BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                     BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                     BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                     BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
                     BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
                     BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                     BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                     BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                     BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                     BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
                     BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
                     BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
                     BSLS_COMPILERFEATURES_FORWARD(Args_13, arguments_13));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class Args_01,
                              class Args_02,
                              class Args_03,
                              class Args_04,
                              class Args_05,
                              class Args_06,
                              class Args_07,
                              class Args_08,
                              class Args_09,
                              class Args_10,
                              class Args_11,
                              class Args_12,
                              class Args_13,
                              class Args_14>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                     BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                     BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                     BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                     BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                     BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                     BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                     BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                     BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                     BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10),
                     BSLS_COMPILERFEATURES_FORWARD(Args_11, arguments_11),
                     BSLS_COMPILERFEATURES_FORWARD(Args_12, arguments_12),
                     BSLS_COMPILERFEATURES_FORWARD(Args_13, arguments_13),
                     BSLS_COMPILERFEATURES_FORWARD(Args_14, arguments_14));
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class... Args>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    basicAllocator.construct(
        elementAddr, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}
// }}} END GENERATED CODE
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
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)                           \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
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
                                          const ALLOCATOR_TYPE& basicAllocator)
{
    return basicAllocator.max_size();
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
