// bslstl_allocatortraits.h                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#define INCLUDED_BSLSTL_ALLOCATORTRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a uniform interface to standard allocator types
//
//@CLASSES:
//  bsl::allocator_traits: Uniform interface to standard allocator types
//
//@SEE_ALSO: bslstl_allocator
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: The standard 'allocator_traits' class template is defined in
// the C++11 standard as a uniform mechanism for accessing nested types
// within, and operations on, any standard-conforming allocator.  An
// 'allocator_traits' specialization is stateless and all of its member
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
// Specifically, this implemention does not provide defaults for C++03 types
// and functions and has hard-wired implementations of the new C++11 features.
// Thus, the 'allocator_traits' template cannot be instantiated on an
// allocator type that does not provide a full compliment of C++03 types and
// functions, and it will ignore any special C++11 features specified in
// 'ALLOC'.  The reason for this limitation is that Bloomberg does not need
// the full functionality of the C++11 model, but only needs to distinguish
// between C++03 allocators, and allocators that implement the BSLMA allocator
// model (see {'bslstd_allocator'}).  The full feature set of
// 'allocator_traits' would require a lot of resources for implementation and
// (epecially) testing.  Moreover, a full implementation would require
// metaprogramming that is too advanced for the feature set of the compilers
// currently in use at Bloomberg.  This interface is useful, however, as a way
// to future-proof containers against the eventual implementation of the full
// feature set, and to take advantage of the Bloomberg-specific features
// described below.
//
// There are two important (new) C++11 features provided by the
// 'allocator_traits' interface: The 'construct' function with a variadic
// argument list (limited to 5 constructor arguments on compilers that don't
// support variadic templates), and the allocator-propagation traits.  The
// implementations of these features within this component are tuned to
// Bloomberg's needs.  The 'construct' member function will automatically
// forward the iterator to the constructed object iff the 'ALLOC' parameter is
// convertible from 'bslma_Allocator*' and the object being constructed has
// the 'bslalg_TypeTraitUsesBslmaAllocator' type trait, as per standard
// Bloomberg practice.  The 'select_on_container_copy_construction' static
// member will return a default-constructed allocator iff 'ALLOC' is
// convertible to from 'bslma_Allocator*' because bslma allocators should not
// be copied when a container is copy-constructed; otherwise it will return a
// copy of the allocator, as per C++03 container rules.  The other propagation
// traits all have a 'false' value, so allocators are not propagated on
// assignment or swap.
//
///Usage
///-----
// This example demonstrates the intended use of 'allocator_traits' to
// implement a standard-conforming container class. We create a container
// class that holds a single object and which meets the requirements both of a
// standard container and of a Bloomberg container.  I.e., when instantiated
// with an allocator argument it uses the standard allocator model;
// otheriwe it uses the 'bslma' model.
//..
//  #include <bslstl_allocatortraits.h>
//  #include <bslstl_allocator.h>
//  #include <bslalg_typetraitsgroupstlsequence.h>
//
//  template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
//  class my_Container {
//      ALLOC  d_allocator;
//      TYPE  *d_valueptr;
//    
//    public:
//      typedef BloombergLP::bslalg_TypeTraitsGroupStlSequence<TYPE,ALLOC>
//          MyTypeTraits;
//      BSLALG_DECLARE_NESTED_TRAITS(my_Container, MyTypeTraits);
//          // Declare nested type traits for this class.
//    
//      typedef TYPE  value_type;
//      typedef ALLOC allocator_type;
//      // etc.
//    
//      explicit my_Container(const ALLOC& a = ALLOC());
//      explicit my_Container(const TYPE& v, const ALLOC& a = ALLOC());
//      my_Container(const my_Container& other);
//      my_Container(const my_Container& other, const ALLOC& a);
//      ~my_Container();
//    
//      ALLOC get_allocator() const { return d_allocator; }
//    
//      TYPE&       front()       { return *d_valueptr; }
//      const TYPE& front() const { return *d_valueptr; }
//      // etc.
//  };
//..
// The implementation of the constructors needs to allocate memory and
// construct an object of type 'TYPE' in the allocated memory.  Because the
// allocation and construction are done in two separate steps, we need to
// create a proctor that will deallocate the allocated memory in case the
// constructor throws an exception.  The proctor uses the uniform interface
// provided by 'allocator_traits' to access the 'pointer' and
// 'deallocate' members of 'ALLOC':
//..
//  template <class ALLOC>
//  class my_Proctor {
//      typedef typename bsl::allocator_traits<ALLOC>::pointer pointer;
//      ALLOC   d_alloc;
//      pointer d_ptr;
//    public:
//      my_Proctor(const ALLOC& a, pointer p) : d_alloc(a), d_ptr(p) { }
//      ~my_Proctor() {
//          if (d_ptr) {
//              bsl::allocator_traits<ALLOC>::deallocate(d_alloc, d_ptr, 1);
//          }
//      }
//      void release() { d_ptr = pointer(); }
//  };
//..
// The actual allocation and construction are done using the 'allocate' and
// 'construct' members of 'allocator_traits', which provide the correct
// semantic for passing the allocator to the constructed object when
// appropriate:
//..
//  template <class TYPE, class ALLOC>
//  my_Container<TYPE, ALLOC>::my_Container(const ALLOC& a)
//      : d_allocator(a)
//  {
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//      d_valueptr = AllocTraits::allocate(d_allocator, 1);
//      my_Proctor<ALLOC> proctor(a, d_valueptr);
//      // Call 'construct' with no constructor arguments
//      AllocTraits::construct(d_allocator, d_valueptr);
//      proctor.release();
//  }
//    
//  template <class TYPE, class ALLOC>
//  my_Container<TYPE, ALLOC>::my_Container(const TYPE& v, const ALLOC& a)
//      : d_allocator(a)
//  {
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//      d_valueptr = AllocTraits::allocate(d_allocator, 1);
//      my_Proctor<ALLOC> proctor(a, d_valueptr);
//      // Call 'construct' with one constructor argument of type 'TYPE'
//      AllocTraits::construct(d_allocator, d_valueptr, v);
//      proctor.release();
//  }
//..
// The copy constructor for 'my_Container' needs to conditinally copy the
// allocator from the 'other' container.  It uses
// 'allocator_traits::select_on_container_copy_construction' to decide whether
// to copy the 'other' allocator (for non-bslma allocators) or to
// default-construct the allocator (for bslma allocators).
//..
//  template <class TYPE, class ALLOC>
//  my_Container<TYPE, ALLOC>::my_Container(const my_Container& other)
//      : d_allocator(bsl::allocator_traits<ALLOC>::
//                    select_on_container_copy_construction(other.d_allocator))
//  {
//      // NOTE: This implementation is not exception-safe
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//      d_valueptr = AllocTraits::allocate(d_allocator, 1);
//      my_Proctor<ALLOC> proctor(d_allocator, d_valueptr);
//      AllocTraits::construct(d_allocator, d_valueptr, *other.d_valueptr);
//      proctor.release();
//  }
//..
// Finally, the destructor uses 'allocator_traits' functions to destroy and
// deallocate the value object:
//..
//  template <class TYPE, class ALLOC>
//  my_Container<TYPE, ALLOC>::~my_Container()
//  {
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//      AllocTraits::destroy(d_allocator, d_valueptr);
//      AllocTraits::deallocate(d_allocator, d_valueptr, 1);
//  }
//..
// We create a representative class, 'my_Type', which allocates memory using
// the 'bslma_Allocator' protocol:
//..
//  #include <bslma_default.h>
//
//  class my_Type {
//    
//      bslma_Allocator *d_allocator;
//      // etc.
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(my_Type,
//                                   bslalg_TypeTraitUsesBslmaAllocator);
//    
//      // CREATORS
//      explicit my_Type(bslma_Allocator* basicAlloc = 0)
//          : d_allocator(bslma_Default::allocator(basicAlloc)) { /* ... */ }
//      my_Type(const my_Type& other)
//          : d_allocator(bslma_Default::allocator(0)) { /* ... */ }
//      my_Type(const my_Type& other, bslma_Allocator* basicAlloc)
//          : d_allocator(bslma_Default::allocator(basicAlloc)) { /* ... */ }
//      // etc.
//    
//      // ACCESSORS
//      bslma_Allocator *allocator() const { return d_allocator; }
//      // etc.
//  };
//..
// Finally, instantiationg 'my_Container' using 'my_Type', we verify that when
// we provide a the address of an allocator to the constructor of the
// container, the same address is passed to the constructor of the container's
// element.  When the container is copy-constructed, the copy uses the default
// allocator, not the allocator from the original.  Moreover, the element
// stored in the copy also uses the default allocator.
//..
//  #include <bslmf_issame.h>
//
//  int main()
//  {
//      bslma_TestAllocator testAlloc;
//      my_Container<my_Type> C1(&testAlloc);
//      assert((bslmf_IsSame<my_Container<my_Type>::allocator_type,
//              bsl::allocator<my_Type> >::VALUE));
//      assert(C1.get_allocator() == bsl::allocator<my_Type>(&testAlloc));
//      assert(C1.front().allocator() == &testAlloc);
//      my_Container<my_Type> C2(C1);
//      assert(C2.get_allocator() != C1.get_allocator());
//      assert(C2.get_allocator() == bsl::allocator<my_Type>());
//      assert(C2.front().allocator() != &testAlloc);
//      assert(C2.front().allocator() == bslma_Default::defaultAllocator());
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif


namespace bsl {

                        // ======================
                        // class allocator_traits
                        // ======================

template <class ALLOC>
struct allocator_traits {
    // This class supports the complete interface for of the C++11
    // 'allocator_traits' class, which provides a uniform mechanism for
    // accessing nested types within, and operations on, any
    // standard-conforming allocator.  This version of 'allocator_traits'
    // supports Bloombergs 'bslma' allocator model by automatically detecting
    // when the specified allocator type, 'ALLOC', is convertable from
    // 'bslma_Allocator' (called a bslma-compatible allocator).  For
    // bslma-compatible allocators, the 'construct' methods forwards the
    // allocator to the element's constructor, when possible, and
    // 'select_on_container_copy_constructor' returns a default-constructed
    // 'ALLOC'.  Otherwise, 'construct' simply forwards its arguments to the
    // element's constructor unchanged and
    // 'select_on_container_copy_constructor' returns its argument unchanged,
    // as per C++03 rules.  This implemention supports C++03 allocators and
    // bslma-compatible allocators; it is not fully-standard-conforming in
    // that it does not support every combination of propagation traits and
    // does not deduce data types that are not specified in the allocator.

  private:
    typedef BloombergLP::bslmf_MetaInt<0> FalseType;
    typedef BloombergLP::bslmf_MetaInt<1> TrueType;

    // 'IsBslma' is 'TrueType' if 'ALLOC' is constructable from
    // 'bslma_Allocator*'.  In other words, its 'VALUE' is true if 'ALLOC' is
    // a wrapper around 'bslma_Allocator*'.
    typedef
    typename BloombergLP::bslmf_IsConvertible<BloombergLP::bslma_Allocator*,
                                              ALLOC>::Type IsBslma;

    // 'IsScoped::VALUE' is true iff 'IsBslma::VALUE' is true and 'T' has the
    // 'bslalg_TypeTraitUsesBslmaAllocator' trait, i.e., if the allocator
    // should be passed to 'T's constructor.
    template <class T>
    struct IsScoped
    {
        enum {
            USES_BDEMA = BloombergLP::bslalg_HasTrait<T,
                   BloombergLP::bslalg_TypeTraitUsesBslmaAllocator>::VALUE
        };

        typedef BloombergLP::bslmf_MetaInt<IsBslma::VALUE && USES_BDEMA> Type;
    };

    // construct object, passing the allocator in if scoped and ignoring
    // the allocator otherwise.
#ifdef BDE_CXX0X_VARIADIC_TEMPLATES
    template <class T, class... Args>
    static void doConstruct(TrueType   /*scoped*/,
                            ALLOC&     a,
                            T         *p,
                            Args&&...  args);

    template <class T, class... Args>
    static void doConstruct(FalseType  /*scoped*/,
                            ALLOC&     a,
                            T         *p,
                            Args&&...  args);
#else
    template <class T>
    static void doConstruct(TrueType     /*scoped*/,
                            ALLOC&       a,
                            T           *p);
    template <class T, class ARG1>
    static void doConstruct(TrueType     /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1);
    template <class T, class ARG1, class ARG2>
    static void doConstruct(TrueType     /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1,
                            const ARG2&  a2);
    template <class T, class ARG1, class ARG2, class ARG3>
    static void doConstruct(TrueType     /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1,
                            const ARG2&  a2,
                            const ARG3&  a3);
    template <class T, class ARG1, class ARG2, class ARG3, class ARG4>
    static void doConstruct(TrueType     /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1,
                            const ARG2&  a2,
                            const ARG3&  a3,
                            const ARG4&  a4);
    template <class T, class ARG1, class ARG2, class ARG3, class ARG4,
              class ARG5>
    static void doConstruct(TrueType     /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1,
                            const ARG2&  a2,
                            const ARG3&  a3,
                            const ARG4&  a4,
                            const ARG5&  a5);

    template <class T>
    static void doConstruct(FalseType    /*scoped*/,
                            ALLOC&       a,
                            T           *p);
    template <class T, class ARG1>
    static void doConstruct(FalseType    /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1);
    template <class T, class ARG1, class ARG2>
    static void doConstruct(FalseType    /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1,
                            const ARG2&  a2);
    template <class T, class ARG1, class ARG2, class ARG3>
    static void doConstruct(FalseType    /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1,
                            const ARG2&  a2,
                            const ARG3&  a3);
    template <class T, class ARG1, class ARG2, class ARG3, class ARG4>
    static void doConstruct(FalseType    /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1,
                            const ARG2&  a2,
                            const ARG3&  a3,
                            const ARG4&  a4);
    template <class T, class ARG1, class ARG2, class ARG3, class ARG4,
              class ARG5>
    static void doConstruct(FalseType    /*scoped*/,
                            ALLOC&       a,
                            T           *p,
                            const ARG1&  a1,
                            const ARG2&  a2,
                            const ARG3&  a3,
                            const ARG4&  a4,
                            const ARG5&  a5);
#endif

    static ALLOC selectOnCopyConstruct(const ALLOC& a, FalseType);
        // Return allocator 'a'.  This function is called when 'ALLOC' is
        // not a bslma allocator.

    static ALLOC selectOnCopyConstruct(const ALLOC&, TrueType);
        // Return a default constructed 'ALLOC' object.  This function is
        // called when 'ALLOC' is bslma allocator.

  public:
    // PUBLIC TYPES
    typedef ALLOC                            allocator_type;
    typedef typename ALLOC::value_type       value_type;

    typedef typename ALLOC::pointer          pointer;
    typedef typename ALLOC::const_pointer    const_pointer;
    typedef void*                            void_pointer;
    typedef void const*                      const_void_pointer;
    typedef typename ALLOC::difference_type  difference_type;
    typedef typename ALLOC::size_type        size_type;

#ifdef BDE_CXX0X_TEMPLATE_ALIASES
    template <class T> using rebind_alloc =
        typename ALLOC::template rebind<Alloc,T>::other;
    template <class T> using rebind_traits =
        allocator_traits<rebind_alloc<T> >;
#else // !BDE_CXX0X_TEMPLATE_ALIASES
    template <class T>
    struct rebind_alloc : ALLOC::template rebind<T>::other { };

    template <class T>
    struct rebind_traits :
        allocator_traits<typename ALLOC::template rebind<T>::other>
    {
    };
#endif // !BDE_CXX0X_TEMPLATE_ALIASES

    static pointer allocate(ALLOC& a, size_type n);
        // Return 'a.allocate(n)'.

    static pointer allocate(ALLOC& a, size_type n, const_void_pointer hint);
        // Return 'a.allocate(n, hint)'.

    static void deallocate(ALLOC& a, pointer p, size_type n);
        // Call 'a.deallocate(p, n)'

#ifdef BDE_CXX0X_VARIADIC_TEMPLATES
    template <class T, class... Args>
    static void construct(ALLOC& a, T *p, Args&&... args);
        // Construct a 'T' object at the specified 'p' address using the
        // specified 'args' constructor arguments.  If 'ALLOC' is a
        // bslma-compatible allocator and 'T' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass the specified
        // allocator 'a' as an additional constructor argument (at the end of
        // the argument list).  The behavior is undefined unless 'p' points to
        // an object that was allocated from (a copy of) 'a'.
#else
    template <class T>
    static void construct(ALLOC&       a,
                          T           *p);
        // Construct a 'T' object at the specified 'p' address.  If 'ALLOC' is
        // a bslma-compatible allocator and 'T' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass the specified
        // allocator 'a' as the sole constructor argument.  Otherwise, use the
        // default constructor for 'T'.  The behavior is undefined unless 'p'
        // points to an object that was allocated from (a copy of) 'a'.

    template <class T, class ARG1>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1);
        // Construct a 'T' object at the specified 'p' address using the
        // specified 'a1' constructor argument.  If 'ALLOC' is a
        // bslma-compatible allocator and 'T' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass the specified
        // allocator 'a' as an additional constructor argument (at the end of
        // the argument list).  The behavior is undefined unless 'p' points to
        // an object that was allocated from (a copy of) 'a'.

    template <class T, class ARG1, class ARG2>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1,
                          const ARG2&  a2);
        // Construct a 'T' object at 'p' using the specified 'a1' and 'a2'
        // constructor arguments.  If 'ALLOC' is a bslma-compatible allocator
        // and 'T' has the 'bslalg_TypeTraitUsesBslmaAllocator' trait, then
        // pass the specified allocator 'a' as an additional constructor
        // argument (at the end of the argument list).  The behavior is
        // undefined unless 'p' points to an object that was allocated from (a
        // copy of) 'a'.

    template <class T, class ARG1, class ARG2, class ARG3>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1,
                          const ARG2&  a2,
                          const ARG3&  a3);
        // Construct a 'T' object at 'p' using the specified 'a1', 'a2' and
        // 'a3' constructor arguments.  If 'ALLOC' is a bslma-compatible
        // allocator and 'T' has the 'bslalg_TypeTraitUsesBslmaAllocator'
        // trait, then pass the specified allocator 'a' as an additional
        // constructor argument (at the end of the argument list).  The
        // behavior is undefined unless 'p' points to an object that was
        // allocated from (a copy of) 'a'.

    template <class T, class ARG1, class ARG2, class ARG3, class ARG4>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1,
                          const ARG2&  a2,
                          const ARG3&  a3,
                          const ARG4&  a4);
        // Construct a 'T' object at 'p' using the specified 'a1', 'a2', 'a3'
        // and 'a4' constructor arguments.  If 'ALLOC' is a bslma-compatible
        // allocator and 'T' has the 'bslalg_TypeTraitUsesBslmaAllocator'
        // trait, then pass the specified allocator 'a' as an additional
        // constructor argument (at the end of the argument list).  The
        // behavior is undefined unless 'p' points to an object that was
        // allocated from (a copy of) 'a'.

    template <class T, class ARG1, class ARG2, class ARG3, class ARG4,
              class ARG5>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1,
                          const ARG2&  a2,
                          const ARG3&  a3,
                          const ARG4&  a4,
                          const ARG5&  a5);
        // Construct a 'T' object at 'p' using the specified 'a1', 'a2', 'a3',
        // 'a4' and 'a5' constructor arguments.  If 'ALLOC' is a
        // bslma-compatible allocator and 'T' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass the specified
        // allocator 'a' as an additional constructor argument (at the end of
        // the argument list).  The behavior is undefined unless 'p' points to
        // an object that was allocated from (a copy of) 'a'.

#endif

    template <class T>
    static void destroy(ALLOC& a, T* p);
        // Call the destructor for 'p'.  The specified allocator 'a' is
        // ignored.  The behavior is undefined unless 'p' points to a valid,
        // constructed object.

    static size_type max_size(const ALLOC& a);
        // Return 'a.max_size()'.

    // Allocator propagation traits
    static ALLOC select_on_container_copy_construction(const ALLOC& rhs);
        // Return a copy of the allocator that should be used to copy-
        // construct one container from another container whose allocator is
        // the specified 'rhs'.  If 'ALLOC' is a bslma-compatible allocator
        // type, then return 'ALLOC()' (i.e., do not copy the allocator to the
        // newly-constructed container); otherwise, return 'rhs' (i.e., do
        // propagate the allocator to the newly-constructed container).

    typedef FalseType propagate_on_container_copy_assignment;
        // Identical to, or derived from 'bslmf_MetaInt<1>' if an allocator of
        // type 'ALLOC' should be copied when a container using that 'ALLOC'
        // is copy-assigned; otherwise identical to or derived from
        // 'bslmf_MetaInt<0>'.  In the current implementation, this type is
        // always 'bslmf_MetaInt<0>'.  In a fully standard-compliant
        // implementation this type would be
        // 'ALLOC::propagate_on_container_copy_assignment' if such a type is
        // defined, and 'false_type' otherwise.

    typedef FalseType propagate_on_container_move_assignment;
        // Identical to, or derived from 'bslmf_MetaInt<1>' if an allocator of
        // type 'ALLOC' should be moved when a container using that 'ALLOC' is
        // move-assigned; otherwise identical to or derived from
        // 'bslmf_MetaInt<0>'.  In the current implementation, this type is
        // always 'bslmf_MetaInt<0>'.  In a fully standard-compliant
        // implementation this type would be
        // 'ALLOC::propagate_on_container_move_assignment' if such a type is
        // defined, and 'false_type' otherwise.

    typedef FalseType propagate_on_container_swap;
        // Identical to, or derived from 'bslmf_MetaInt<1>' if the allocators
        // of type 'ALLOC' should be swapped when containers using that
        // 'ALLOC' are swapped; otherwise identical to or derived from
        // 'bslmf_MetaInt<0>'.  In the current implementation, this type is
        // always 'bslmf_MetaInt<0>'.  In a fully standard-compliant
        // implementation this type would be
        // 'ALLOC::propagate_on_container_swap' if such a type is defined, and
        // 'false_type' otherwise.
};

#ifdef BDE_CXX0X_VARIADIC_TEMPLATES
template <class ALLOC>
    template <class T, class... Args>
inline
void allocator_traits<ALLOC>::doConstruct(TrueType  /*scoped*/,
                                          ALLOC&    a,
                                          T        *p,
                                          Args&&... args)
{
    ::new((void*) p) T(std::forward<Args>(args)..., a.mechanism());
}

template <class ALLOC>
    template <class T, class... Args>
inline
void allocator_traits<ALLOC>::doConstruct(FalseType  /*scoped*/,
                                          ALLOC&    a,
                                          T        *p,
                                          Args&&... args)
{
    ::new((void*) p) T(std::forward<Args>(args)...);
}
#else

template <class ALLOC>
    template <class T>
inline
void allocator_traits<ALLOC>::doConstruct(TrueType /*scoped*/, ALLOC& a, T* p)
{
    ::new((void*) p) T(a.mechanism());
}

template <class ALLOC>
    template <class T, class ARG1>
inline
void allocator_traits<ALLOC>::doConstruct(TrueType     /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1)
{
    ::new((void*) p) T(a1, a.mechanism());
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2>
inline
void allocator_traits<ALLOC>::doConstruct(TrueType     /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1,
                                          const ARG2&  a2)
{
    ::new((void*) p) T(a1, a2, a.mechanism());
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2, class ARG3>
inline
void allocator_traits<ALLOC>::doConstruct(TrueType     /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1,
                                          const ARG2&  a2,
                                          const ARG3&  a3)
{
    ::new((void*) p) T(a1, a2, a3, a.mechanism());
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2, class ARG3, class ARG4>
inline
void allocator_traits<ALLOC>::doConstruct(TrueType     /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1,
                                          const ARG2&  a2,
                                          const ARG3&  a3,
                                          const ARG4&  a4)
{
    ::new((void*) p) T(a1, a2, a3, a4, a.mechanism());
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2, class ARG3, class ARG4,
              class ARG5>
inline
void allocator_traits<ALLOC>::doConstruct(TrueType     /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1,
                                          const ARG2&  a2,
                                          const ARG3&  a3,
                                          const ARG4&  a4,
                                          const ARG5&  a5)
{
    ::new((void*) p) T(a1, a2, a3, a4, a5, a.mechanism());
}

template <class ALLOC>
    template <class T>
inline
void allocator_traits<ALLOC>::doConstruct(FalseType /*scoped*/, ALLOC& a, T* p)
{
    ::new((void*) p) T();
}

template <class ALLOC>
    template <class T, class ARG1>
inline
void allocator_traits<ALLOC>::doConstruct(FalseType    /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1)
{
    ::new((void*) p) T(a1);
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2>
inline
void allocator_traits<ALLOC>::doConstruct(FalseType    /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1,
                                          const ARG2&  a2)
{
    ::new((void*) p) T(a1, a2);
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2, class ARG3>
inline
void allocator_traits<ALLOC>::doConstruct(FalseType    /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1,
                                          const ARG2&  a2,
                                          const ARG3&  a3)
{
    ::new((void*) p) T(a1, a2, a3);
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2, class ARG3, class ARG4>
inline
void allocator_traits<ALLOC>::doConstruct(FalseType    /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1,
                                          const ARG2&  a2,
                                          const ARG3&  a3,
                                          const ARG4&  a4)
{
    ::new((void*) p) T(a1, a2, a3, a4);
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2, class ARG3, class ARG4,
              class ARG5>
inline
void allocator_traits<ALLOC>::doConstruct(FalseType    /*scoped*/,
                                          ALLOC&       a,
                                          T           *p,
                                          const ARG1&  a1,
                                          const ARG2&  a2,
                                          const ARG3&  a3,
                                          const ARG4&  a4,
                                          const ARG5&  a5)
{
    ::new((void*) p) T(a1, a2, a3, a4, a5);
}
#endif // !BDE_CXX0X_VARIADIC_TEMPLATES

template <class ALLOC>
inline
ALLOC allocator_traits<ALLOC>::selectOnCopyConstruct(const ALLOC& a, FalseType)
{
    return a;
}

template <class ALLOC>
inline
ALLOC allocator_traits<ALLOC>::selectOnCopyConstruct(const ALLOC&, TrueType)
{
    return ALLOC();
}

template <class ALLOC>
inline typename allocator_traits<ALLOC>::pointer
allocator_traits<ALLOC>::allocate(ALLOC& a, size_type n)
{
    return a.allocate(n, 0);
}

template <class ALLOC>
inline typename allocator_traits<ALLOC>::pointer
allocator_traits<ALLOC>::allocate(ALLOC&             a,
                                  size_type          n,
                                  const_void_pointer hint)
{
    return a.allocate(n, hint);
}

template <class ALLOC>
inline void
allocator_traits<ALLOC>::deallocate(ALLOC& a, pointer p, size_type n)
{
    a.deallocate(p, n);
}

#ifdef BDE_CXX0X_VARIADIC_TEMPLATES
template <class ALLOC>
template <class T, class... Args>
inline void
allocator_traits<ALLOC>::construct(ALLOC& a, T* p, Args&&... args)
{
    doConstruct(typename IsScoped<T>::Type(), a, p,
                std::forward<Args>(args)...);
}
#else
template <class ALLOC>
template <class T>
inline void
allocator_traits<ALLOC>::construct(ALLOC&  a,
                                   T      *p)
{
    doConstruct(typename IsScoped<T>::Type(), a, p);
}

template <class ALLOC>
template <class T, class ARG1>
inline void
allocator_traits<ALLOC>::construct(ALLOC&       a,
                                   T           *p,
                                   const ARG1&  a1)
{
    doConstruct(typename IsScoped<T>::Type(), a, p, a1);
}

template <class ALLOC>
    template <class T, class ARG1, class ARG2>
inline void
allocator_traits<ALLOC>::construct(ALLOC&       a,
                                   T           *p,
                                   const ARG1&  a1,
                                   const ARG2&  a2)
{
    doConstruct(typename IsScoped<T>::Type(), a, p, a1, a2);
}

template <class ALLOC>
template <class T, class ARG1, class ARG2, class ARG3>
inline void
allocator_traits<ALLOC>::construct(ALLOC&       a,
                                   T           *p,
                                   const ARG1&  a1,
                                   const ARG2&  a2,
                                   const ARG3&  a3)
{
    doConstruct(typename IsScoped<T>::Type(), a, p, a1, a2, a3);
}

template <class ALLOC>
template <class T, class ARG1, class ARG2, class ARG3, class ARG4>
inline void
allocator_traits<ALLOC>::construct(ALLOC&       a,
                                   T           *p,
                                   const ARG1&  a1,
                                   const ARG2&  a2,
                                   const ARG3&  a3,
                                   const ARG4&  a4)
{
    doConstruct(typename IsScoped<T>::Type(), a, p, a1, a2, a3, a4);
}

template <class ALLOC>
template <class T, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
inline void
allocator_traits<ALLOC>::construct(ALLOC&       a,
                                   T           *p,
                                   const ARG1&  a1,
                                   const ARG2&  a2,
                                   const ARG3&  a3,
                                   const ARG4&  a4,
                                   const ARG5&  a5)
{
    doConstruct(typename IsScoped<T>::Type(), a, p, a1, a2, a3, a4, a5);
}

#endif // ! BDE_CXX0X_VARIADIC_TEMPLATES

template <class ALLOC>
template <class T>
inline void
allocator_traits<ALLOC>::destroy(ALLOC& a, T* p)
{
    p->~T();
}

template <class ALLOC>
inline
typename allocator_traits<ALLOC>::size_type
allocator_traits<ALLOC>::max_size(const ALLOC& a)
{
    return a.max_size();
}

template <class ALLOC>
inline ALLOC
allocator_traits<ALLOC>::select_on_container_copy_construction(
    const ALLOC& rhs)
{
    return selectOnCopyConstruct(rhs, IsBslma());
}

// FREE OPERATORS

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CLASS METHODS

// CREATORS

// MANIPULATORS

// ACCESSORS

// FREE OPERATORS

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
