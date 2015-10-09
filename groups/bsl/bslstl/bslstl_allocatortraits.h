// bslstl_allocatortraits.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#define INCLUDED_BSLSTL_ALLOCATORTRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a uniform interface to standard allocator types.
//
//@CLASSES:
//  bsl::allocator_traits: Uniform interface to standard allocator types
//
//@SEE_ALSO: bslma_allocator, bslstl_allocator
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
// allocator model (see {'bslstl_allocator'}).  The full feature set of
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
//  #include <bslstl_allocatortraits.h>
//  #include <bslstl_allocator.h>
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
//      void construct(pointer p, const TYPE& value)
//          { new((void *)p) TYPE(value); }
//
//      void destroy(pointer p) { p->~TYPE(); }
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

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_allocatortraits.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_UTILITY
#include <utility>
#define INCLUDED_UTILITY
#endif

#endif


namespace bsl {

                        // ======================
                        // class allocator_traits
                        // ======================

template <class ALLOCATOR_TYPE>
struct allocator_traits {
    // This class supports the complete interface of the C++11
    // 'allocator_traits' class template, which provides a uniform mechanism
    // for accessing nested types within, and operations on, any
    // standard-conforming allocator.  This version of 'allocator_traits'
    // supports Bloomberg's 'bslma' allocator model by automatically detecting
    // when the parameterized 'ALLOCATOR_TYPE' is convertible from
    // 'bslma::Allocator' (called a bslma-compatible allocator).  For
    // bslma-compatible allocators, the 'construct' methods forward the
    // allocator to the new element's constructor, when possible, and
    // 'select_on_container_copy_constructor' returns a default-constructed
    // 'ALLOCATOR_TYPE'.  Otherwise, 'construct' simply forwards its arguments
    // to the new element's constructor unchanged and
    // 'select_on_container_copy_constructor' returns its argument unchanged,
    // as per C++03 rules.  This implementation supports C++03 allocators and
    // bslma-compatible allocators; it is not fully-standard-conforming in that
    // it does not support every combination of propagation traits and does not
    // deduce data types that are not specified in the allocator.

  private:
    // 'IsBslma' is 'true_type' if the parameterized 'ALLOCATOR_TYPE' is
    // constructible from 'bslma::Allocator*'.  In other words, its 'VALUE' is
    // 'true' if 'ALLOCATOR_TYPE' is a wrapper around 'bslma::Allocator *'.
    typedef typename is_convertible<BloombergLP::bslma::Allocator*,
                                    ALLOCATOR_TYPE>::type IsBslma;

    static void *mechanism(const ALLOCATOR_TYPE&, false_type);
        // Return a null pointer.  Note that this function is called only when
        // 'ALLOCATOR_TYPE' is not a bslma allocator.

    static
    BloombergLP::bslma::Allocator *mechanism(
                                           const ALLOCATOR_TYPE& bslAllocator,
                                           true_type);
        // Return the address of the 'bslma::Allocator' that implements the
        // mechanism for the specified 'bslAllocator', i.e.,
        // 'allocator.mechanism()'.  Note that this function is called only
        // when 'ALLOCATOR_TYPE' is bslma allocator.

    static
    ALLOCATOR_TYPE selectOnCopyConstruct(const ALLOCATOR_TYPE& stdAllocator,
                                         false_type);
        // Return the specified 'stdAllocator'.  Note that this function is
        // called only when the (template parameter) 'ALLOCATOR_TYPE' is not a
        // bslma allocator.

    static
    ALLOCATOR_TYPE selectOnCopyConstruct(const ALLOCATOR_TYPE&, true_type);
        // Return a default constructed 'ALLOCATOR_TYPE' object.  Note that
        // this function is called only when 'ALLOCATOR_TYPE' is bslma
        // allocator.

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
    struct rebind_alloc : ALLOCATOR_TYPE::template rebind<ELEMENT_TYPE>::other
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

    template <class ELEMENT_TYPE>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr);
        // Default construct an object of the parameterized 'ELEMENT_TYPE' at
        // the specified 'elementAddr'.  If the parameterized 'ALLOCATOR_TYPE'
        // is bslma-compatible and 'ELEMENT_TYPE' has the
        // 'bslma::UsesBslmaAllocator' trait, then pass the mechanism from the
        // specified 'basicAllocator' as an additional constructor argument (at
        // the end of the argument list).  The behavior is undefined unless
        // 'elementAddr' refers to valid, uninitialized storage.  Note that
        // this overload of 'construct' is implemented using
        // 'bslalg::ScalarPrimitives::defaultConstruct' whereas those overloads
        // that take at least one additional constructor argument are
        // implemented in terms of 'bslalg::ScalarPrimitives::construct'

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class... CTOR_ARGS>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                          CTOR_ARGS_0&&    ctorArgs_0,
                          CTOR_ARGS&&...   ctorArgs);
        // Construct an object of the parameterized 'ELEMENT_TYPE' at the
        // specified 'elementAddr' using a constructor argument list comprising
        // the specified 'ctorArgs_0' and 'ctorArgs'.  If the parameterized
        // 'ALLOCATOR_TYPE' is bslma-compatible and 'ELEMENT_TYPE' has the
        // 'bslma::UsesBslmaAllocator' trait, then pass the mechanism from the
        // specified 'basicAllocator' as an additional constructor argument (at
        // the end of the argument list).  The behavior is undefined unless
        // 'elementAddr' refers to valid, uninitialized storage.  Note that
        // this overload of 'construct' takes at least one constructor argument
        // in addition to the allocator argument.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_allocatortraits.h
    template <class ELEMENT_TYPE, class CTOR_ARGS_0>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06,
                                                     class CTOR_ARGS_07>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06,
                                                     class CTOR_ARGS_07,
                                                     class CTOR_ARGS_08>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06,
                                                     class CTOR_ARGS_07,
                                                     class CTOR_ARGS_08,
                                                     class CTOR_ARGS_09>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06,
                                                     class CTOR_ARGS_07,
                                                     class CTOR_ARGS_08,
                                                     class CTOR_ARGS_09,
                                                     class CTOR_ARGS_10>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06,
                                                     class CTOR_ARGS_07,
                                                     class CTOR_ARGS_08,
                                                     class CTOR_ARGS_09,
                                                     class CTOR_ARGS_10,
                                                     class CTOR_ARGS_11>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_11) ctorArgs_11);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06,
                                                     class CTOR_ARGS_07,
                                                     class CTOR_ARGS_08,
                                                     class CTOR_ARGS_09,
                                                     class CTOR_ARGS_10,
                                                     class CTOR_ARGS_11,
                                                     class CTOR_ARGS_12>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_11) ctorArgs_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_12) ctorArgs_12);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06,
                                                     class CTOR_ARGS_07,
                                                     class CTOR_ARGS_08,
                                                     class CTOR_ARGS_09,
                                                     class CTOR_ARGS_10,
                                                     class CTOR_ARGS_11,
                                                     class CTOR_ARGS_12,
                                                     class CTOR_ARGS_13>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_11) ctorArgs_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_12) ctorArgs_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_13) ctorArgs_13);

    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                     class CTOR_ARGS_02,
                                                     class CTOR_ARGS_03,
                                                     class CTOR_ARGS_04,
                                                     class CTOR_ARGS_05,
                                                     class CTOR_ARGS_06,
                                                     class CTOR_ARGS_07,
                                                     class CTOR_ARGS_08,
                                                     class CTOR_ARGS_09,
                                                     class CTOR_ARGS_10,
                                                     class CTOR_ARGS_11,
                                                     class CTOR_ARGS_12,
                                                     class CTOR_ARGS_13,
                                                     class CTOR_ARGS_14>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_11) ctorArgs_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_12) ctorArgs_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_13) ctorArgs_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_14) ctorArgs_14);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class ELEMENT_TYPE, class CTOR_ARGS_0, class... CTOR_ARGS>
    static void construct(ALLOCATOR_TYPE&  basicAllocator,
                          ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS)... ctorArgs);
// }}} END GENERATED CODE
#endif

    template <class ELEMENT_TYPE>
    static void destroy(ALLOCATOR_TYPE&  basicAllocator,
                        ELEMENT_TYPE    *elementAddr);
        // Invoke the destructor for the object at the specified 'elementAddr';
        // the specified 'basicAllocator' (of parameterized 'ALLOCATOR_TYPE')
        // is ignored.  The behavior is undefined unless 'elementAddr' refers
        // to a valid, constructed object.

    static size_type max_size(const ALLOCATOR_TYPE& basicAllocator);
        // Return the largest number of 'value_type' objects that could
        // reasonably be returned by a single invocation of 'allocate' for the
        // specified 'allocator', i.e., 'allocator.max_size()'.

    // Allocator propagation traits
    static ALLOCATOR_TYPE
    select_on_container_copy_construction(const ALLOCATOR_TYPE& rhs);
        // Return a copy of the allocator that should be used to copy-
        // construct one container from another container whose allocator is
        // the specified 'rhs'.  If the parameterized 'ALLOCATOR_TYPE' is
        // bslma-compatible, then return 'ALLOCATOR_TYPE()' (i.e., do not copy
        // the allocator to the newly-constructed container); otherwise, return
        // 'rhs' (i.e., do propagate the allocator to the newly-constructed
        // container).

    typedef false_type propagate_on_container_copy_assignment;
        // Identical to, or derived from 'true_type' if an allocator of
        // parameterized 'ALLOCATOR_TYPE' should be copied when a container
        // using that 'ALLOCATOR_TYPE' is copy-assigned; otherwise identical to
        // or derived from 'false_type'.  In the current implementation, this
        // type is always 'false_type'.  In a fully standard-compliant
        // implementation, this type would be
        // 'ALLOCATOR_TYPE::propagate_on_container_copy_assignment' if such a
        // type is defined, and 'false_type' otherwise.

    typedef false_type propagate_on_container_move_assignment;
        // Identical to, or derived from 'true_type' if an allocator of
        // parameterized 'ALLOCATOR_TYPE' should be moved when a container
        // using that 'ALLOCATOR_TYPE' is move-assigned; otherwise identical to
        // or derived from 'false_type'.  In the current implementation, this
        // type is always 'false_type'.  In a fully standard-compliant
        // implementation, this type would be
        // 'ALLOCATOR_TYPE::propagate_on_container_move_assignment' if such a
        // type is defined, and 'false_type' otherwise.

    typedef false_type propagate_on_container_swap;
        // Identical to, or derived from 'true_type' if the allocators of
        // parameterized 'ALLOCATOR_TYPE' should be swapped when containers
        // using that 'ALLOCATOR_TYPE' are swapped; otherwise identical to or
        // derived from 'false_type'.  In the current implementation, this type
        // is always 'false_type'.  In a fully standard-compliant
        // implementation, this type would be
        // 'ALLOCATOR_TYPE::propagate_on_container_swap' if such a type is
        // defined, and 'false_type' otherwise.
};

// ============================================================================
//          INLINE AND TEMPLATE STATIC MEMBER FUNCTION DEFINITIONS
// ============================================================================

template <class ALLOCATOR_TYPE>
inline
void *
allocator_traits<ALLOCATOR_TYPE>::mechanism(const ALLOCATOR_TYPE&,
                                            false_type)
{
    return 0;
}

template <class ALLOCATOR_TYPE>
inline
BloombergLP::bslma::Allocator *
allocator_traits<ALLOCATOR_TYPE>::mechanism(const ALLOCATOR_TYPE& bslAllocator,
                                            true_type)
{
    return bslAllocator.mechanism();
}

template <class ALLOCATOR_TYPE>
inline
ALLOCATOR_TYPE allocator_traits<ALLOCATOR_TYPE>::selectOnCopyConstruct(
                                            const ALLOCATOR_TYPE& stdAllocator,
                                            false_type)
{
    return stdAllocator;
}

template <class ALLOCATOR_TYPE>
inline
ALLOCATOR_TYPE allocator_traits<ALLOCATOR_TYPE>::selectOnCopyConstruct(
                                                         const ALLOCATOR_TYPE&,
                                                         true_type)
{
    return ALLOCATOR_TYPE();
}

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

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr)
{
    BloombergLP::bslalg::ScalarPrimitives::defaultConstruct(
                                         elementAddr,
                                         mechanism(basicAllocator, IsBslma()));
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class... CTOR_ARGS>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                                            CTOR_ARGS_0&&    ctorArgs_0,
                                            CTOR_ARGS&&...   ctorArgs)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS,ctorArgs)...,
                         mechanism(basicAllocator, IsBslma()));
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_allocatortraits.h
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06,
                                                 class CTOR_ARGS_07>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_07,ctorArgs_07),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06,
                                                 class CTOR_ARGS_07,
                                                 class CTOR_ARGS_08>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_07,ctorArgs_07),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_08,ctorArgs_08),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06,
                                                 class CTOR_ARGS_07,
                                                 class CTOR_ARGS_08,
                                                 class CTOR_ARGS_09>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_07,ctorArgs_07),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_08,ctorArgs_08),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_09,ctorArgs_09),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06,
                                                 class CTOR_ARGS_07,
                                                 class CTOR_ARGS_08,
                                                 class CTOR_ARGS_09,
                                                 class CTOR_ARGS_10>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_07,ctorArgs_07),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_08,ctorArgs_08),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_09,ctorArgs_09),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_10,ctorArgs_10),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06,
                                                 class CTOR_ARGS_07,
                                                 class CTOR_ARGS_08,
                                                 class CTOR_ARGS_09,
                                                 class CTOR_ARGS_10,
                                                 class CTOR_ARGS_11>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_11) ctorArgs_11)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_07,ctorArgs_07),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_08,ctorArgs_08),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_09,ctorArgs_09),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_10,ctorArgs_10),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_11,ctorArgs_11),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06,
                                                 class CTOR_ARGS_07,
                                                 class CTOR_ARGS_08,
                                                 class CTOR_ARGS_09,
                                                 class CTOR_ARGS_10,
                                                 class CTOR_ARGS_11,
                                                 class CTOR_ARGS_12>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_11) ctorArgs_11,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_12) ctorArgs_12)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_07,ctorArgs_07),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_08,ctorArgs_08),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_09,ctorArgs_09),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_10,ctorArgs_10),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_11,ctorArgs_11),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_12,ctorArgs_12),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06,
                                                 class CTOR_ARGS_07,
                                                 class CTOR_ARGS_08,
                                                 class CTOR_ARGS_09,
                                                 class CTOR_ARGS_10,
                                                 class CTOR_ARGS_11,
                                                 class CTOR_ARGS_12,
                                                 class CTOR_ARGS_13>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_11) ctorArgs_11,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_12) ctorArgs_12,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_13) ctorArgs_13)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_07,ctorArgs_07),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_08,ctorArgs_08),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_09,ctorArgs_09),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_10,ctorArgs_10),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_11,ctorArgs_11),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_12,ctorArgs_12),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_13,ctorArgs_13),
                         mechanism(basicAllocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class CTOR_ARGS_01,
                                                 class CTOR_ARGS_02,
                                                 class CTOR_ARGS_03,
                                                 class CTOR_ARGS_04,
                                                 class CTOR_ARGS_05,
                                                 class CTOR_ARGS_06,
                                                 class CTOR_ARGS_07,
                                                 class CTOR_ARGS_08,
                                                 class CTOR_ARGS_09,
                                                 class CTOR_ARGS_10,
                                                 class CTOR_ARGS_11,
                                                 class CTOR_ARGS_12,
                                                 class CTOR_ARGS_13,
                                                 class CTOR_ARGS_14>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_01) ctorArgs_01,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_02) ctorArgs_02,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_03) ctorArgs_03,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_04) ctorArgs_04,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_05) ctorArgs_05,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_06) ctorArgs_06,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_07) ctorArgs_07,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_08) ctorArgs_08,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_09) ctorArgs_09,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_10) ctorArgs_10,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_11) ctorArgs_11,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_12) ctorArgs_12,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_13) ctorArgs_13,
                   BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_14) ctorArgs_14)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_01,ctorArgs_01),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_02,ctorArgs_02),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_03,ctorArgs_03),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_04,ctorArgs_04),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_05,ctorArgs_05),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_06,ctorArgs_06),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_07,ctorArgs_07),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_08,ctorArgs_08),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_09,ctorArgs_09),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_10,ctorArgs_10),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_11,ctorArgs_11),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_12,ctorArgs_12),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_13,ctorArgs_13),
                       BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_14,ctorArgs_14),
                         mechanism(basicAllocator, IsBslma()));
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARGS_0, class... CTOR_ARGS>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  basicAllocator,
                                            ELEMENT_TYPE    *elementAddr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS_0) ctorArgs_0,
                      BSLS_COMPILERFEATURES_FORWARD_REF(CTOR_ARGS)... ctorArgs)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                         elementAddr,
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS_0,ctorArgs_0),
                         BSLS_COMPILERFEATURES_FORWARD(CTOR_ARGS,ctorArgs)...,
                         mechanism(basicAllocator, IsBslma()));
}
// }}} END GENERATED CODE
#endif

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE>
inline
void
allocator_traits<ALLOCATOR_TYPE>::destroy(ALLOCATOR_TYPE&  /*basicAllocator*/,
                                          ELEMENT_TYPE    *elementAddr)
{
//  For full C++11 compatibility, this should check for the well-formedness of
//  the allocator-specific code that is commented out below (via some SFINAE
//  trickery), and switch to the ScalarDestructionPrimitives implementation
//  only if the 'destroy' member function is not available.

//    allocator.destroy(elementAddr);
    BloombergLP::bslalg::ScalarDestructionPrimitives::destroy(elementAddr);
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
    return selectOnCopyConstruct(rhs, IsBslma());
}

}  // close namespace bsl

#endif // ! defined(INCLUDED_BSLSTL_ALLOCATORTRAITS)

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
