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
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: The standard 'allocator_traits' class template is defined in
// the C++11 standard as a uniform mechanism for accessing nested types
// within, and operations on, any standard-conforming allocator.  An
// 'allocator_traits' specialization is stateless, and all of its member
// functions are static.  In most cases, facilities of 'allocator_traits'
// are straight pass-throughs for the same facilities from the 'ALLOC'
// template parameter.  For example, 'allocator_traits<X>::pointer' is the
// same as 'X::pointer' and 'allocator_traits<X>::allocate(x, n)' is the same
// as 'x.allocate(n)'.  The advantage of using 'allocator_traits' instead of
// directly using the allocator is that the 'allocator_traits' interface can
// supply parts of the interface that are missing from 'ALLOC'.  In fact, the
// most important purpose of 'allocator_traits' is to provide implementations
// of C++11 allocator features that were absent in C++03, thus allowing a
// C++03 allocator to work with C++11 containers.
//
// This component provides a full C++11 interface for 'allocator_traits', but
// constrains the set of allocator types on which it may be instantiated.
// Specifically, this implementation does not provide defaults for C++03 types
// and functions, and has hard-wired implementations of the new C++11
// features.  Thus, the 'allocator_traits' template cannot be instantiated on
// an allocator type that does not provide a full compliment of C++03 types
// and functions, and it will ignore any special C++11 features specified in
// 'ALLOC'.  This limitation exists because Bloomberg does not need the full
// functionality of the C++11 model, but needs only to distinguish between
// C++03 allocators and allocators that implement the BSLMA allocator model
// (see {'bslstl_allocator'}).  The full feature set of 'allocator_traits'
// would require a lot of resources for implementation and (especially)
// testing.  Moreover, a full implementation would require metaprogramming
// that is too advanced for the feature set of the compilers currently in use
// at Bloomberg.  This interface is useful, however, as a way to future-proof
// containers against the eventual implementation of the full feature set, and
// to take advantage of the Bloomberg-specific features described below.
//
// There are two important (new) C++11 features provided by the
// 'allocator_traits' interface: the 'construct' function having a
// variable-length argument list (limited to 5 constructor arguments on
// compilers that don't support variadic templates) and the
// allocator-propagation traits.  The implementations of these features within
// this component are tuned to Bloomberg's needs.  The 'construct' member
// function will automatically forward the iterator to the constructed object
// iff the 'ALLOC' parameter is convertible from 'bslma_Allocator*' and the
// object being constructed has the 'bslalg_TypeTraitUsesBslmaAllocator' type
// trait, as per standard Bloomberg practice.  The
// 'select_on_container_copy_construction' static member will return a
// default-constructed allocator iff 'ALLOC' is convertible to from
// 'bslma_Allocator*' because bslma allocators should not be copied when a
// container is copy-constructed; otherwise this function will return a copy
// of the allocator, as per C++03 container rules.  The other propagation
// traits all have a 'false' value, so allocators are not propagated on
// assignment or swap.
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
// both of a standard container and of a Bloomberg container -- i.e., when
// instantiated with an allocator argument it uses the standard allocator
// model; otherwise it uses the 'bslma' model.
//..
//  #include <bslstl_allocatortraits.h>
//  #include <bslstl_allocator.h>
//  #include <bslalg_typetraitsgroupstlsequence.h>
//
//  using namespace BloombergLP;
//
//  template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
//  class MyContainer {
//      ALLOC  d_allocator;
//      TYPE  *d_value_p;
//
//    public:
//      typedef bslalg_TypeTraitsGroupStlSequence<TYPE,ALLOC> TypeTraits;
//      BSLALG_DECLARE_NESTED_TRAITS(MyContainer, TypeTraits);
//          // Declare nested type traits for this class.
//
//      typedef TYPE  value_type;
//      typedef ALLOC allocator_type;
//      // etc.
//
//      explicit MyContainer(const ALLOC& a = ALLOC());
//      explicit MyContainer(const TYPE& v, const ALLOC& a = ALLOC());
//      MyContainer(const MyContainer& other);
//      MyContainer(const MyContainer& other, const ALLOC& a);
//      ~MyContainer();
//
//      ALLOC get_allocator() const { return d_allocator; }
//
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
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
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
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
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
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
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
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//      AllocTraits::destroy(d_allocator, d_value_p);
//      AllocTraits::deallocate(d_allocator, d_value_p, 1);
//  }
//..
// Finally, we perform a simple test of 'MyContainer', instantiating it with
// element type 'int':
//..
//  int main()
//  {
//      bslma_TestAllocator testAlloc;
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
///Example 2: bslma Allocator Propagation
///- - - - - - - - - - - - - - - - - - -
// To exercise the propagation of the allocator of 'MyContainer' to its
// elements, we first create a representative element class, 'MyType', that
// allocates memory using the bslma allocator protocol:
//..
//  #include <bslma_default.h>
//
//  class MyType {
//
//      bslma_Allocator *d_allocator_p;
//      // etc.
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(MyType,
//                                   bslalg_TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      explicit MyType(bslma_Allocator* basicAlloc = 0)
//          : d_allocator_p(bslma_Default::allocator(basicAlloc)) { /* ... */ }
//      MyType(const MyType& other)
//          : d_allocator_p(bslma_Default::allocator(0)) { /* ... */ }
//      MyType(const MyType& other, bslma_Allocator* basicAlloc)
//          : d_allocator_p(bslma_Default::allocator(basicAlloc)) { /* ... */ }
//      // etc.
//
//      // ACCESSORS
//      bslma_Allocator *allocator() const { return d_allocator_p; }
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
//  int main()
//  {
//      bslma_TestAllocator testAlloc;
//      MyContainer<MyType> C1(&testAlloc);
//      assert((bslmf_IsSame<MyContainer<MyType>::allocator_type,
//              bsl::allocator<MyType> >::VALUE));
//      assert(C1.get_allocator() == bsl::allocator<MyType>(&testAlloc));
//      assert(C1.front().allocator() == &testAlloc);
//
//      MyContainer<MyType> C2(C1);
//      assert(C2.get_allocator() != C1.get_allocator());
//      assert(C2.get_allocator() == bsl::allocator<MyType>());
//      assert(C2.front().allocator() != &testAlloc);
//      assert(C2.front().allocator() == bslma_Default::defaultAllocator());
//
//      return 0;
//  }
//..
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
//  public:
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
//      int state() const { return d_state; }
//  };
//
//  template <class TYPE1, class TYPE2>
//  inline
//  bool operator==(const MyCpp03Allocator<TYPE1>& a,
//                  const MyCpp03Allocator<TYPE2>& b)
//  {
//      return a.state() == b.state();
//  }
//
//  template <class TYPE1, class TYPE2>
//  inline
//  bool operator!=(const MyCpp03Allocator<TYPE1>& a,
//                  const MyCpp03Allocator<TYPE2>& b)
//  {
//      return ! (a == b);
//  }
//..
// Then we instantiate 'MyContainer' using this allocator type and verify that
// elements are constructed using the default allocator (because the allocator
// is not propagated from the container).  We also verify that the allocator
// is copied on copy-construction:
//..
//  int main()
//  {
//      typedef MyCpp03Allocator<MyType> MyTypeAlloc;
//      MyContainer<MyType, MyTypeAlloc> C1(MyTypeAlloc(1));
//      assert((bslmf_IsSame<MyContainer<MyType, MyTypeAlloc>::allocator_type,
//                           MyTypeAlloc>::VALUE));
//      assert(C1.get_allocator() == MyTypeAlloc(1));
//      assert(C1.front().allocator() == bslma_Default::defaultAllocator());
//
//      MyContainer<MyType, MyTypeAlloc> C2(C1);
//      assert(C2.get_allocator() == C1.get_allocator());
//      assert(C2.get_allocator() != MyTypeAlloc())
//      assert(C2.front().allocator() == bslma_Default::defaultAllocator());
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace bsl {

                        // ======================
                        // class allocator_traits
                        // ======================

template <class ALLOCATOR_TYPE>
struct allocator_traits {
    // This class supports the complete interface for of the C++11
    // 'allocator_traits' class, which provides a uniform mechanism for
    // accessing nested types within, and operations on, any
    // standard-conforming allocator.  This version of 'allocator_traits'
    // supports Bloomberg's 'bslma' allocator model by automatically detecting
    // when the parameterized, 'ALLOCATOR_TYPE', is convertible from
    // 'bslma_Allocator' (called a bslma-compatible allocator).  For
    // bslma-compatible allocators, the 'construct' methods forwards the
    // allocator to the element's constructor, when possible, and
    // 'select_on_container_copy_constructor' returns a default-constructed
    // 'ALLOCATOR_TYPE'.  Otherwise, 'construct' simply forwards its arguments
    // to the element's constructor unchanged and
    // 'select_on_container_copy_constructor' returns its argument unchanged,
    // as per C++03 rules.  This implementation supports C++03 allocators and
    //
    // bslma-compatible allocators; it is not fully-standard-conforming in
    // that it does not support every combination of propagation traits and
    // does not deduce data types that are not specified in the allocator.

  private:
    typedef BloombergLP::bslmf::MetaInt<0> FalseType;
    typedef BloombergLP::bslmf::MetaInt<1> TrueType;

    // 'IsBslma' is 'TrueType' if the parameterized 'ALLOCATOR_TYPE' is
    // constructible from 'bslma_Allocator*'.  In other words, its 'VALUE' is
    // true if 'ALLOCATOR_TYPE' is a wrapper around 'bslma_Allocator*'.
    typedef
    typename BloombergLP::bslmf::IsConvertible<BloombergLP::bslma::Allocator*,
                                               ALLOCATOR_TYPE>::Type IsBslma;

    static
    ALLOCATOR_TYPE selectOnCopyConstruct(const ALLOCATOR_TYPE& allocator,
                                         FalseType);
        // Return 'allocator'.  Note that this function is called only when
        // 'ALLOCATOR_TYPE' is not a bslma allocator.

    static ALLOCATOR_TYPE selectOnCopyConstruct(const ALLOCATOR_TYPE&,
                                                TrueType);
        // Return a default constructed 'ALLOCATOR_TYPE' object.  Note that
        // this function is called only when 'ALLOCATOR_TYPE' is bslma
        // allocator.

    static void *mechanism(const ALLOCATOR_TYPE&, FalseType);
        // Return a null pointer.  Note that this function is called only when
        // 'ALLOCATOR_TYPE' is not a bslma allocator.

    static
    BloombergLP::bslma::Allocator *mechanism(const ALLOCATOR_TYPE& allocator,
                                             TrueType);
        // Return the address of the 'bslma_Allocator' that implements the
        // mechanism for the specified 'allocator', i.e.,
        // 'allocator.mechanism()'. Note that this function is called only
        // when 'ALLOCATOR_TYPE' is bslma allocator.

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
           typename ALLOCATOR_TYPE::template rebind<Alloc,ELEMENT_TYPE>::other;

    template <class ELEMENT_TYPE>
    using rebind_traits = allocator_traits<rebind_alloc<ELEMENT_TYPE> >;
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

    static pointer allocate(ALLOCATOR_TYPE& allocator, size_type n);
        // Return 'allocator.allocate(n)'.

    static pointer allocate(ALLOCATOR_TYPE&    allocator,
                            size_type          n,
                            const_void_pointer hint);
        // Return 'allocator.allocate(n, hint)'.

    static void deallocate(ALLOCATOR_TYPE& allocator,
                           pointer         elementAddr,
                           size_type       n);
        // Invoke 'allocator.deallocate(elementAddr, n)'.  The behavior is
        // undefined unless the specified 'elementAddr' was returned from
        // a prior call to the 'allocate' method of the specified 'allocator',
        // and has not yet been passed to a 'deallocate' call of the same
        // object.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#  ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    template <class ELEMENT_TYPE, class... CTOR_ARGS>
    static void construct(ALLOCATOR_TYPE&  allocator,
                          ELEMENT_TYPE    *elementAddr,
                          CTOR_ARGS&&...   ctorArgs);
        // Construct an object of the parameterized 'ELEMENT_TYPE' at the
        // specified 'elementAddr' using the specified 'ctorArgs'.  If the
        // parameterized 'ALLOCATOR_TYPE' is bslma-compatible and
        // 'ELEMENT_TYPE' has the 'bslalg_TypeTraitUsesBslmaAllocator' trait,
        // then pass the mechanism from the specified 'allocator' as an
        // additional constructor argument (at the end of the argument list).
        // The behavior is undefined unless 'elementAddr' refers to valid,
        // uninitialized storage.
#  else
    template <class ELEMENT_TYPE, class...     CTOR_ARGS>
    static void construct(ALLOCATOR_TYPE&      allocator,
                          ELEMENT_TYPE        *elementAddr,
                          const CTOR_ARGS&...  ctorArgs);
        // Construct an object of the parameterized 'ELEMENT_TYPE' at the
        // specified 'elementAddr' using the specified 'ctorArgs'.  If the
        // parameterized 'ALLOCATOR_TYPE' is bslma-compatible and
        // 'ELEMENT_TYPE' has the 'bslalg_TypeTraitUsesBslmaAllocator' trait,
        // then pass the mechanism from the specified 'allocator' as an
        // additional constructor argument (at the end of the argument list).
        // The behavior is undefined unless 'elementAddr' refers to valid,
        // uninitialized storage.
#  endif
#else
    template <class ELEMENT_TYPE>
    static void construct(ALLOCATOR_TYPE&   allocator,
                          ELEMENT_TYPE     *elementAddr);
        // Construct an object of the parameterized 'ELEMENT_TYPE'.  If the
        // parameterized 'ALLOCATOR_TYPE' is bslma-compatible and
        // 'ELEMENT_TYPE' has the 'bslalg_TypeTraitUsesBslmaAllocator' trait,
        // then pass the mechanism from the specified 'allocator' as the sole
        // constructor argument; otherwise, invoke the default constructor for
        // 'ELEMENT_TYPE' (passing no arguments).  The behavior is undefined
        // unless 'elementAddr' refers to valid, uninitialized storage.

    template <class ELEMENT_TYPE, class CTOR_ARG1>
    static void construct(ALLOCATOR_TYPE&   allocator,
                          ELEMENT_TYPE     *elementAddr,
                          const CTOR_ARG1&  ctorArg1);
        // Construct an object of the parameterized 'ELEMENT_TYPE' at the
        // specified 'elementAddr' using the specified 'ctorArg1' constructor
        // argument.  If the parameterized 'ALLOCATOR_TYPE' is
        // bslma-compatible and 'ELEMENT_TYPE' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass the mechanism
        // from the specified 'allocator' as an additional constructor
        // argument (at the end of the argument list).  The behavior is
        // undefined unless 'elementAddr' refers to valid, uninitialized
        // storage.

    template <class ELEMENT_TYPE, class CTOR_ARG1, class CTOR_ARG2>
    static void construct(ALLOCATOR_TYPE&   allocator,
                          ELEMENT_TYPE     *elementAddr,
                          const CTOR_ARG1&  ctorArg1,
                          const CTOR_ARG2&  ctorArg2);
        // Construct an object of the parameterized 'ELEMENT_TYPE' at the
        // specified 'elementAddr' using the specified 'ctorArg1', and
        // 'ctorArg2' constructor arguments.  If the parameterized
        // 'ALLOCATOR_TYPE' is bslma-compatible and 'ELEMENT_TYPE' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass the mechanism
        // from the specified 'allocator' as an additional constructor
        // argument (at the end of the argument list).  The behavior is
        // undefined unless 'elementAddr' refers to valid, uninitialized
        // storage.

    template <class ELEMENT_TYPE, class CTOR_ARG1, class CTOR_ARG2,
              class CTOR_ARG3>
    static void construct(ALLOCATOR_TYPE&   allocator,
                          ELEMENT_TYPE     *elementAddr,
                          const CTOR_ARG1&  ctorArg1,
                          const CTOR_ARG2&  ctorArg2,
                          const CTOR_ARG3&  ctorArg3);
        // Construct an object of the parameterized 'ELEMENT_TYPE' at the
        // specified 'elementAddr' using the specified 'ctorArg1', 'ctorArg2',
        // and 'ctorArg3' constructor arguments.  If the parameterized
        // 'ALLOCATOR_TYPE' is bslma-compatible and 'ELEMENT_TYPE' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass the mechanism
        // from the specified 'allocator' as an additional constructor
        // argument (at the end of the argument list).  The behavior is
        // undefined unless 'elementAddr' refers to valid, uninitialized
        // storage.

    template <class ELEMENT_TYPE, class CTOR_ARG1, class CTOR_ARG2,
              class CTOR_ARG3, class CTOR_ARG4>
    static void construct(ALLOCATOR_TYPE&   allocator,
                          ELEMENT_TYPE     *elementAddr,
                          const CTOR_ARG1&  ctorArg1,
                          const CTOR_ARG2&  ctorArg2,
                          const CTOR_ARG3&  ctorArg3,
                          const CTOR_ARG4&  ctorArg4);
        // Construct an object of the parameterized 'ELEMENT_TYPE' at the
        // specified 'elementAddr' using the specified 'ctorArg1', 'ctorArg2',
        // 'ctorArg3' and 'ctorArg4' constructor arguments.  If the
        // parameterized 'ALLOCATOR_TYPE' is bslma-compatible and
        // 'ELEMENT_TYPE' has the 'bslalg_TypeTraitUsesBslmaAllocator' trait,
        // then pass the mechanism from the specified 'allocator' as an
        // additional constructor argument (at the end of the argument list).
        // The behavior is undefined unless 'elementAddr' refers to valid,
        // uninitialized storage.

    template <class ELEMENT_TYPE, class CTOR_ARG1, class CTOR_ARG2,
              class CTOR_ARG3, class CTOR_ARG4, class CTOR_ARG5>
    static void construct(ALLOCATOR_TYPE&   allocator,
                          ELEMENT_TYPE     *elementAddr,
                          const CTOR_ARG1&  ctorArg1,
                          const CTOR_ARG2&  ctorArg2,
                          const CTOR_ARG3&  ctorArg3,
                          const CTOR_ARG4&  ctorArg4,
                          const CTOR_ARG5&  ctorArg5);
        // Construct an object of the parameterized 'ELEMENT_TYPE' at the
        // specified 'elementAddr' using the specified 'ctorArg1', 'ctorArg2',
        // 'ctorArg3', 'ctorArg4' and 'ctorArg5' constructor arguments.  If
        // the parameterized 'ALLOCATOR_TYPE' is bslma-compatible and
        // 'ELEMENT_TYPE' has the 'bslalg_TypeTraitUsesBslmaAllocator' trait,
        // then pass the mechanism from the specified 'allocator' as an
        // additional constructor argument (at the end of the argument list).
        // The behavior is undefined unless 'elementAddr' refers to valid,
        // uninitialized storage.

#endif

    template <class ELEMENT_TYPE>
    static void destroy(ALLOCATOR_TYPE& allocator, ELEMENT_TYPE *elementAddr);
        // Invoke the destructor for the object at the specified
        // 'elementAddr'; the specified 'allocator' (of parameterized
        // 'ALLOCATOR_TYPE') is ignored.  The behavior is undefined unless
        // 'elementAddr' refers to a valid, constructed object.

    static size_type max_size(const ALLOCATOR_TYPE& allocator);
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
        // the allocator to the newly-constructed container); otherwise,
        // return 'rhs' (i.e., do propagate the allocator to the
        // newly-constructed container).

    typedef FalseType propagate_on_container_copy_assignment;
        // Identical to, or derived from 'bslmf_MetaInt<1>' if an allocator of
        // parameterized 'ALLOCATOR_TYPE' should be copied when a container
        // using that 'ALLOCATOR_TYPE' is copy-assigned; otherwise identical
        // to or derived from 'bslmf_MetaInt<0>'.  In the current
        // implementation, this type is always 'bslmf_MetaInt<0>'.  In a fully
        // standard-compliant implementation, this type would be
        // 'ALLOCATOR_TYPE::propagate_on_container_copy_assignment' if such a
        // type is defined, and 'false_type' otherwise.

    typedef FalseType propagate_on_container_move_assignment;
        // Identical to, or derived from 'bslmf_MetaInt<1>' if an allocator of
        // parameterized 'ALLOCATOR_TYPE' should be moved when a container
        // using that 'ALLOCATOR_TYPE' is move-assigned; otherwise identical
        // to or derived from 'bslmf_MetaInt<0>'.  In the current
        // implementation, this type is always 'bslmf_MetaInt<0>'.  In a fully
        // standard-compliant implementation, this type would be
        // 'ALLOCATOR_TYPE::propagate_on_container_move_assignment' if such a
        // type is defined, and 'false_type' otherwise.

    typedef FalseType propagate_on_container_swap;
        // Identical to, or derived from 'bslmf_MetaInt<1>' if the allocators
        // of parameterized 'ALLOCATOR_TYPE' should be swapped when containers
        // using that 'ALLOCATOR_TYPE' are swapped; otherwise identical to or
        // derived from 'bslmf_MetaInt<0>'.  In the current implementation,
        // this type is always 'bslmf_MetaInt<0>'.  In a fully
        // standard-compliant implementation, this type would be
        // 'ALLOCATOR_TYPE::propagate_on_container_swap' if such a type is
        // defined, and 'false_type' otherwise.
};

// ===========================================================================
//          INLINE AND TEMPLATE STATIC MEMBER FUNCTION DEFINITIONS
// ===========================================================================

template <class ALLOCATOR_TYPE>
inline
ALLOCATOR_TYPE allocator_traits<ALLOCATOR_TYPE>::selectOnCopyConstruct(
                                               const ALLOCATOR_TYPE& allocator,
                                               FalseType)
{
    return allocator;
}

template <class ALLOCATOR_TYPE>
inline
ALLOCATOR_TYPE allocator_traits<ALLOCATOR_TYPE>::selectOnCopyConstruct(
                                                         const ALLOCATOR_TYPE&,
                                                         TrueType)
{
    return ALLOCATOR_TYPE();
}

template <class ALLOCATOR_TYPE>
inline
void *
allocator_traits<ALLOCATOR_TYPE>::mechanism(const ALLOCATOR_TYPE&,
                                            FalseType)
{
    return 0;
}

template <class ALLOCATOR_TYPE>
inline
BloombergLP::bslma::Allocator *
allocator_traits<ALLOCATOR_TYPE>::mechanism(const ALLOCATOR_TYPE& allocator,
                                            TrueType)
{
    return allocator.mechanism();
}

template <class ALLOCATOR_TYPE>
inline
typename allocator_traits<ALLOCATOR_TYPE>::pointer
allocator_traits<ALLOCATOR_TYPE>::allocate(ALLOCATOR_TYPE& allocator,
                                           size_type n)
{
    return allocator.allocate(n, 0);
}

template <class ALLOCATOR_TYPE>
inline
typename allocator_traits<ALLOCATOR_TYPE>::pointer
allocator_traits<ALLOCATOR_TYPE>::allocate(ALLOCATOR_TYPE&    allocator,
                                           size_type          n,
                                           const_void_pointer hint)
{
    return allocator.allocate(n, hint);
}

template <class ALLOCATOR_TYPE>
inline
void
allocator_traits<ALLOCATOR_TYPE>::deallocate(ALLOCATOR_TYPE& allocator,
                                             pointer         elementAddr,
                                             size_type       n)
{
    allocator.deallocate(elementAddr, n);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#  ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class... CTOR_ARGS>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  allocator,
                                   ELEMENT_TYPE             *elementAddr,
                                   CTOR_ARGS&&...            ctorArgs)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                                          elementAddr,
                                          std::forward<CTOR_ARGS>(ctorArgs)...,
                                          mechanism(allocator, IsBslma()));
}
#  else
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class... CTOR_ARGS>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  allocator,
                                   ELEMENT_TYPE             *elementAddr,
                                   const CTOR_ARGS&...       ctorArgs)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                                              elementAddr,
                                              ctorArgs...,
                                              mechanism(allocator, IsBslma()));
}
#  endif
#else
template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&  allocator,
                                            ELEMENT_TYPE    *elementAddr)
{
    BloombergLP::bslalg::ScalarPrimitives::defaultConstruct(
        elementAddr, mechanism(allocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARG1>
inline void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&   allocator,
                                            ELEMENT_TYPE     *elementAddr,
                                            const CTOR_ARG1&  ctorArg1)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                       elementAddr, ctorArg1, mechanism(allocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE, class CTOR_ARG1, class CTOR_ARG2>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&   allocator,
                                            ELEMENT_TYPE     *elementAddr,
                                            const CTOR_ARG1&  ctorArg1,
                                            const CTOR_ARG2&  ctorArg2)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
             elementAddr, ctorArg1, ctorArg2, mechanism(allocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE,
          class CTOR_ARG1,
          class CTOR_ARG2,
          class CTOR_ARG3>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&   allocator,
                                            ELEMENT_TYPE     *elementAddr,
                                            const CTOR_ARG1&  ctorArg1,
                                            const CTOR_ARG2&  ctorArg2,
                                            const CTOR_ARG3&  ctorArg3)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                                              elementAddr,
                                              ctorArg1,
                                              ctorArg2,
                                              ctorArg3,
                                              mechanism(allocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE,
          class CTOR_ARG1,
          class CTOR_ARG2,
          class CTOR_ARG3,
          class CTOR_ARG4>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&   allocator,
                                            ELEMENT_TYPE     *elementAddr,
                                            const CTOR_ARG1&  ctorArg1,
                                            const CTOR_ARG2&  ctorArg2,
                                            const CTOR_ARG3&  ctorArg3,
                                            const CTOR_ARG4&  ctorArg4)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                                              elementAddr,
                                              ctorArg1,
                                              ctorArg2,
                                              ctorArg3,
                                              ctorArg4,
                                              mechanism(allocator, IsBslma()));
}

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE,
          class CTOR_ARG1,
          class CTOR_ARG2,
          class CTOR_ARG3,
          class CTOR_ARG4,
          class CTOR_ARG5>
inline
void
allocator_traits<ALLOCATOR_TYPE>::construct(ALLOCATOR_TYPE&   allocator,
                                            ELEMENT_TYPE     *elementAddr,
                                            const CTOR_ARG1&  ctorArg1,
                                            const CTOR_ARG2&  ctorArg2,
                                            const CTOR_ARG3&  ctorArg3,
                                            const CTOR_ARG4&  ctorArg4,
                                            const CTOR_ARG5&  ctorArg5)
{
    BloombergLP::bslalg::ScalarPrimitives::construct(
                                              elementAddr,
                                              ctorArg1,
                                              ctorArg2,
                                              ctorArg3,
                                              ctorArg4,
                                              ctorArg5,
                                              mechanism(allocator, IsBslma()));
}

#endif // ! BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

template <class ALLOCATOR_TYPE>
template <class ELEMENT_TYPE>
inline
void
allocator_traits<ALLOCATOR_TYPE>::destroy(ALLOCATOR_TYPE&  allocator,
                                          ELEMENT_TYPE    *elementAddr)
{
    BloombergLP::bslalg::ScalarDestructionPrimitives::destroy(elementAddr);
}

template <class ALLOCATOR_TYPE>
inline
typename allocator_traits<ALLOCATOR_TYPE>::size_type
allocator_traits<ALLOCATOR_TYPE>::max_size(const ALLOCATOR_TYPE& allocator)
{
    return allocator.max_size();
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
