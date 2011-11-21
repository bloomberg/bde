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
// the C++11 standard as a uniform mechanism for accessing types of, and
// operations on, any standard-conforming allocator.  A 'allocator_traits'
// specialization is stateless and all of its member functions are static.  In
// most cases, facilities of 'allocator_traits' is are straight pass-throughs
// for the same facilities from the 'ALLOC' template parameter.  For example,
// 'allocator_traits<X>::pointer' is the same as 'X::pointer' and
// 'allocator_traits<X>::allocate(x, n)' is the same as 'x.allocate(n)'.  The
// advantage of using 'allocator_traits' instead of directly using the
// allocator is that the 'allocator_traits' interface can supply parts of the
// interface that are missing from 'ALLOC'.  In fact, the most important
// purpose of 'allocator_traits' is to provide implementations of C++11
// allocator features that were absent in C++03, thus allowing a C++03
// allocator to work with C++11 containers.
//
// This component provides a full C++11 interface for 'allocator_traits', but
// constrains the set of allocator types on which it may be instantiated.
// Specifically, this implemention does not provide defaults for C++03 types
// and functions and has hard-wired implementations of the new C++11 features.
// Thus, the 'allocator_traits' template cannot be instantiated on an
// allocator type that does not provide a full compliment of C++03 types and
// functions, and it will ignore any special C++11 features specified in
// 'ALLOC'.  The reason for this limitation is that Bloomberg's compilers do
// not implement all of the features needed for the metaprogramming that would
// be required to fully implement the C++11 standard.  This interface is
// useful, however, as a way to future-proof containers against the eventual
// implementation of the full feature set, and to take advantage of the
// Bloomberg-specific features described below.
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
// be copied when a container is copy-constructed.  The other propagation
// traits all have a 'false' value, so allocators are not propagated on
// assignment and swap.
//
///Usage
///-----
// Let's create a container class that holds a single object and which meets
// the requirements of an STL container and of a Bloomberg container:
//..
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
//      ~my_Container();
//
//      TYPE&       front()       { return *d_valueptr; }
//      const TYPE& front() const { return *d_valueptr; }
//      // etc.
//  };
//..
// The implementation of the constructors needs to allocate memory and
// construct an object of type 'TYPE' in the allocated memory.  Rather than
// allocating the memory directly, we use the 'allocate' member of
// 'allocator_traits'.  More importantly, we construct the object using the
// 'construct' member of 'allocator_traits', which provides the correct
// semantic for passing the allocator to the constructed object when
// appropriate:
//..
//  template <class TYPE, class ALLOC>
//  my_Container<TYPE, ALLOC>::my_Container(const ALLOC& a)
//      : d_allocator(a)
//  {
//      // NOTE: This implementation is not exception-safe
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//      d_valueptr = AllocTraits::allocate(d_allocator, 1);
//      AllocTraits::construct(d_allocator, d_valueptr);
//  }
//
//  template <class TYPE, class ALLOC>
//  my_Container<TYPE, ALLOC>::my_Container(const TYPE& v, const ALLOC& a)
//      : d_allocator(a)
//  {
//      // NOTE: This implementation is not exception-safe
//      typedef bsl::allocator_traits<ALLOC> AllocTraits;
//      d_valueptr = AllocTraits::allocate(d_allocator, 1);
//      AllocTraits::construct(d_allocator, d_valueptr, v);
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
//      AllocTraits::construct(d_allocator, d_valueptr, *other.d_valueptr);
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
// Now, given a value type that uses a 'bslma_Allocator' to allocate memory:
//..
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
// We can see that the allocator is propagated to the container's element and
// that it is not copied on copy construction of the container:
//..
//  int main()
//  {
//      bslma_TestAllocator testAlloc;
//      my_Container<my_Type> C1(&testAlloc);
//      assert(C1.front().allocator() == &testAlloc);
//      my_Container<my_Type> C2(C1);
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
    struct rebind_alloc : ALLOC::template rebind<T> { };

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
        // Construct a 'T' object at 'p' using the specified constructor
        // 'args'.  If 'ALLOC' is a bslma-compatible allocator and 'T' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass 'a' as an
        // additional constructor argument (at the end of the argument list).
        // The behavior is undefined unless 'p' points to an object that was
        // allocated from (a copy of) 'a'.
#else
    template <class T>
    static void construct(ALLOC&       a,
                          T           *p);
        // Construct a 'T' object at 'p'.  If 'ALLOC' is a bslma-compatible
        // allocator and 'T' has the 'bslalg_TypeTraitUsesBslmaAllocator'
        // trait, then pass 'a' as the sole constructor argument.  Otherwise,
        // use the default constructor for 'T'.  The behavior is undefined
        // unless 'p' points to an object that was allocated from (a copy of)
        // 'a'.

    template <class T, class ARG1>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1);
        // Construct a 'T' object at 'p' using the specified constructor
        // argument, 'a1'.  If 'ALLOC' is a bslma-compatible allocator and 'T'
        // has the 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass 'a'
        // as an additional constructor argument (at the end of the argument
        // list).  The behavior is undefined unless 'p' points to an object
        // that was allocated from (a copy of) 'a'.

    template <class T, class ARG1, class ARG2>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1,
                          const ARG2&  a2);
        // Construct a 'T' object at 'p' using the specified constructor
        // arguments, 'a1' and 'a2'.  If 'ALLOC' is a bslma-compatible
        // allocator and 'T' has the 'bslalg_TypeTraitUsesBslmaAllocator'
        // trait, then pass 'a' as an additional constructor argument (at the
        // end of the argument list).  The behavior is undefined unless 'p'
        // points to an object that was allocated from (a copy of) 'a'.

    template <class T, class ARG1, class ARG2, class ARG3>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1,
                          const ARG2&  a2,
                          const ARG3&  a3);
        // Construct a 'T' object at 'p' using the specified constructor
        // arguments, 'a1', 'a2' and 'a3'.  If 'ALLOC' is a bslma-compatible
        // allocator and 'T' has the 'bslalg_TypeTraitUsesBslmaAllocator'
        // trait, then pass 'a' as an additional constructor argument (at the
        // end of the argument list).  The behavior is undefined unless 'p'
        // points to an object that was allocated from (a copy of) 'a'.

    template <class T, class ARG1, class ARG2, class ARG3, class ARG4>
    static void construct(ALLOC&       a,
                          T           *p,
                          const ARG1&  a1,
                          const ARG2&  a2,
                          const ARG3&  a3,
                          const ARG4&  a4);
        // Construct a 'T' object at 'p' using the specified constructor
        // arguments, 'a1', 'a2', 'a3' and 'a4'.  If 'ALLOC' is a
        // bslma-compatible allocator and 'T' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass 'a' as an
        // additional constructor argument (at the end of the argument list).
        // The behavior is undefined unless 'p' points to an object that was
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
        // Construct a 'T' object at 'p' using the specified constructor
        // arguments, 'a1', 'a2', 'a3', 'a4' and 'a5'.  If 'ALLOC' is a
        // bslma-compatible allocator and 'T' has the
        // 'bslalg_TypeTraitUsesBslmaAllocator' trait, then pass 'a' as an
        // additional constructor argument (at the end of the argument list).
        // The behavior is undefined unless 'p' points to an object that was
        // allocated from (a copy of) 'a'.

#endif

    template <class T>
    static void destroy(ALLOC& a, T* p);
        // Call the destructor for 'p'.  The 'a' argument is ignored.  The
        // behavior is undefined unless 'p' points to a valid, constructed
        // object.

    static size_type max_size(const ALLOC& a);
        // Returns 'a.max_size()'.

    // Allocator propagation traits
    static ALLOC select_on_container_copy_construction(const ALLOC& rhs);
        // Return a copy of the allocator that should be used to copy-
        // construct one container from another container whose allocator is
        // 'rhs'.  If 'ALLOC' is a bslma-compatible allocator, then return
        // 'ALLOC()' (i.e., do not copy the allocator to the newly-constructed
        // container); otherwise, return 'rhs' (i.e., do propagate the
        // allocator to the newly-constructed container).

    typedef FalseType propagate_on_container_copy_assignment;
        // 'propagate_on_container_copy_assignment::VALUE' indicates whether
        // the allocator object should be copied when a container using that
        // allocator is copy-assigned.  In the current implementation, it is
        // always 'false'.

    typedef FalseType propagate_on_container_move_assignment;
        // 'propagate_on_container_move_assignment::VALUE' indicates whether
        // the allocator object should be moved when a container using that
        // allocator is move-assigned.  In the current implementation, it is
        // always 'false'.

    typedef FalseType propagate_on_container_swap;
        // 'propagate_on_container_move_assignment::VALUE' indicates whether
        // the allocator objects should be swapped when containers using
        // allocators of type 'ALLOC' are move-assigned.  In the current
        // implementation, it is always 'false'.
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
