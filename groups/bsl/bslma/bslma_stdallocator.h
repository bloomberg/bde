// bslma_stdallocator.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMA_STDALLOCATOR
#define INCLUDED_BSLMA_STDALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compatible proxy for 'bslma::Allocator' objects.
//
//@CLASSES:
//  bsl::allocator: STL-compatible allocator template
//  bsl::allocator_traits<bsl::allocator>: specialization for 'bsl::allocator'
//
//@CANONICAL_HEADER: bsl_memory.h
//
//@SEE_ALSO: bslma_allocator
//
// TBD: update component-level doc
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_memory.h>' instead and use 'bsl::allocator' directly.  This component
// provides an STL-compatible proxy for any allocator class derived from
// 'bslma::Allocator'.  The proxy class, 'bsl::allocator' is a template that
// adheres to the allocator requirements defined in section 20.1.5
// [lib.allocator.requirements] of the C++ standard.  'bsl::allocator' may be
// used to instantiate any class template that is parameterized by a standard
// allocator.  The container is expected to allocate memory for its own use
// through the allocator.  Different types of allocator use different
// allocation mechanisms, so this mechanism gives the programmer control over
// how the container obtains memory.
//
// The 'bsl::allocator' template is intended to solve a problem created by the
// C++ standard allocator protocol.  Since, in STL, the allocator type is
// specified as a container template parameter, the allocation mechanism
// becomes an explicit part of the resulting container type.  Two containers
// cannot be of the same type unless they are instantiated with the same
// allocator type, and therefore the same allocation mechanism.
// 'bsl::allocator' breaks the connection between allocator type and allocation
// mechanism.  The allocation mechanism is chosen at *run-time* by
// *initializing* (contrast with *instantiating*) the 'bsl::allocator' with a
// pointer to a *mechanism* *object* derived from 'bslma::Allocator'.  Each
// class derived from 'bslma::Allocator' implements a specific allocation
// mechanism and is thus called a *mechanism* *class* within this component.
// The 'bsl::allocator' object forwards calls made through the standard
// allocator interface to the mechanism object with which it was initialized.
// In this way, two containers instantiated with 'bsl::allocator' can utilize
// different allocation mechanisms even though they have the same compile-time
// type.  The default mechanism object, if none is supplied to the
// 'bsl::allocator' constructor, is 'bslma::Default::defaultAllocator()'.
//
// Instantiations of 'bsl::allocator' have full value semantics (well-behaved
// copy construction, assignment, and tests for equality).  Note, however, that
// a 'bsl::allocator' object does not "own" the 'bslma::Allocator' with which
// it is initialized.  In practice , this means that copying a 'bsl::allocator'
// object does not copy its mechanism object and destroying a 'bsl::allocator'
// does not destroy its mechanism object.  Two 'bsl::allocator' objects compare
// equal if and only if they share the same mechanism object.
//
///Restrictions on Allocator Usage
///-------------------------------
// The allocator requirements section of the C++ standard (section 20.1.5
// [lib.allocator.requirements]) permits containers to assume that two
// allocators of the same type always compare equal.  This assumption is
// incorrect for instantiations of 'bsl::allocator'.  Therefore, any container
// (or other facility) that can use 'bsl::allocator' must operate correctly in
// the presence of non-equal 'bsl::allocator' objects.  In practice, this means
// that a container cannot transfer ownership of allocated memory to another
// container unless the two containers use equal allocators.  Two
// 'bsl::allocator' objects will compare equal if and only if they were
// initialized with the same mechanism object.
//
///Usage
///-----
// We first show how to define a container type parameterized with an STL-style
// allocator template parameter.  For simplicity, we choose a fixed-size array
// to avoid issues concerning reallocation, dynamic growth, etc.  Furthermore,
// we do not assume the 'bslma' allocation protocol, which would dictate that
// we pass-through the allocator to the parameterized 'T' contained type (see
// the 'bslma_allocator' component and 'bslalg' package).  The interface would
// be as follows:
//..
//  // my_fixedsizearray.h
//
//                          // =======================
//                          // class my_FixedSizeArray
//                          // =======================
//
//  template <class TYPE, class ALLOC>
//  class my_FixedSizeArray {
//      // This class provides an array of (the template parameter) 'TYPE'
//      // passed of fixed length at construction, using an instance of the
//      // parameterized 'ALLOC' type to supply memory.
//
//      // DATA
//      ALLOC  d_allocator;
//      int    d_length;
//      TYPE  *d_array;
//
//      // INVARIANTS
//
//    public:
//      // TYPES
//      typedef ALLOC  allocator_type;
//      typedef TYPE   value_type;
//
//      // CREATORS
//      explicit my_FixedSizeArray(int length,
//                                 const ALLOC& allocator = ALLOC());
//          // Create a fixed-size array of the specified 'length', using the
//          // optionally specified 'allocator' to supply memory.  If
//          // 'allocator' is not specified, a default-constructed instance of
//          // the parameterized 'ALLOC' type is used.  Note that all the
//          // elements in that array are default-constructed.
//
//      my_FixedSizeArray(const my_FixedSizeArray& original,
//                        const ALLOC&             allocator = ALLOC());
//          // Create a copy of the specified 'original' fixed-size array,
//          // using the optionally specified 'allocator' to supply memory.  If
//          // 'allocator' is not specified, a default-constructed instance of
//          // the parameterized 'ALLOC' type is used.
//
//      ~my_FixedSizeArray();
//          // Destroy this fixed size array.
//
//      // MANIPULATORS
//      TYPE& operator[](int index);
//          // Return a reference to the modifiable element at the specified
//          // 'index' position in this fixed size array.
//
//      // ACCESSORS
//      const TYPE& operator[](int index) const;
//          // Return a reference to the modifiable element at the specified
//          // 'index' position in this fixed size array.
//
//      const ALLOC& allocator() const;
//          // Return a reference to the non-modifiable allocator used by this
//          // fixed size array to supply memory.  This is here for
//          // illustrative purposes.  We should not generally have an accessor
//          // to return the allocator.
//
//      int length() const;
//          // Return the length specified at construction of this fixed size
//          // array.
//  };
//
//  // FREE OPERATORS
//  template<class TYPE, class ALLOC>
//  bool operator==(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
//                  const my_FixedSizeArray<TYPE, ALLOC>& rhs);
//      // Return 'true' if the specified 'lhs' fixed-size array has the same
//      // value as the specified 'rhs' fixed-size array, and 'false'
//      // otherwise.  Two fixed-size arrays have the same value if they have
//      // the same length and if the element at any index in 'lhs' has the
//      // same value as the corresponding element at the same index in 'rhs'.
//
//  namespace BloombergLP {
//  namespace bslma {
//
//  template <class TYPE, class ALLOC>
//  struct UsesBslmaAllocator< my_FixedSizeArray<TYPE, ALLOC> >
//  : bsl::is_convertible<Allocator*, ALLOC>::type
//  {
//  };
//
//  }  // close namespace bslma
//  }  // close enterprise namespace
//
//..
// The implementation is straightforward
//..
//                          // -----------------------
//                          // class my_FixedSizeArray
//                          // -----------------------
//
//  // CREATORS
//  template<class TYPE, class ALLOC>
//  my_FixedSizeArray<TYPE, ALLOC>::my_FixedSizeArray(int          length,
//                                                    const ALLOC& allocator)
//  : d_allocator(allocator), d_length(length)
//  {
//      d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes
//
//      // Default construct each element of the array:
//      for (int i = 0; i < d_length; ++i) {
//          d_allocator.construct(&d_array[i], TYPE());
//      }
//  }
//
//  template<class TYPE, class ALLOC>
//  my_FixedSizeArray<TYPE, ALLOC>::my_FixedSizeArray(
//                                          const my_FixedSizeArray& original,
//                                          const ALLOC&             allocator)
//  : d_allocator(allocator), d_length(original.d_length)
//  {
//      d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes
//
//      // copy construct each element of the array:
//      for (int i = 0; i < d_length; ++i) {
//          d_allocator.construct(&d_array[i], original.d_array[i]);
//      }
//  }
//
//  template<class TYPE, class ALLOC>
//  my_FixedSizeArray<TYPE, ALLOC>::~my_FixedSizeArray()
//  {
//      // Call destructor for each element
//      for (int i = 0; i < d_length; ++i) {
//          d_allocator.destroy(&d_array[i]);
//      }
//
//      // Return memory to allocator.
//      d_allocator.deallocate(d_array, d_length);
//  }
//
//  // MANIPULATORS
//  template<class TYPE, class ALLOC>
//  inline TYPE& my_FixedSizeArray<TYPE, ALLOC>::operator[](int index)
//  {
//      return d_array[index];
//  }
//
//  // ACCESSORS
//  template<class TYPE, class ALLOC>
//  inline
//  const TYPE& my_FixedSizeArray<TYPE, ALLOC>::operator[](int index) const
//  {
//      return d_array[index];
//  }
//
//  template<class TYPE, class ALLOC>
//  inline
//  const ALLOC& my_FixedSizeArray<TYPE, ALLOC>::allocator() const
//  {
//      return d_allocator;
//  }
//
//  // FREE OPERATORS
//  template<class TYPE, class ALLOC>
//  bool operator==(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
//                  const my_FixedSizeArray<TYPE, ALLOC>& rhs)
//  {
//      if (lhs.length() != rhs.length()) {
//          return false;                                             // RETURN
//      }
//      for (int i = 0; i < lhs.length(); ++i) {
//          if (lhs[i] != rhs[i]) {
//              return false;                                         // RETURN
//          }
//      }
//      return true;
//  }
//
//  template<class TYPE, class ALLOC>
//  inline int my_FixedSizeArray<TYPE, ALLOC>::length() const
//  {
//      return d_length;
//  }
//
//..
// Now we declare an allocator mechanism.  Our mechanism will be to simply call
// the global 'operator new' and 'operator delete' functions, and count the
// number of blocks outstanding (allocated but not deallocated).  Note that a
// more reusable implementation would take an underlying mechanism at
// construction.  We keep things simple only for the sake of this example.
//..
//  // my_countingallocator.h
//
//                         // ==========================
//                         // class my_CountingAllocator
//                         // ==========================
//
//  class my_CountingAllocator : public bslma::Allocator {
//      // This concrete implementation of the 'bslma::Allocator' protocol
//      // maintains some statistics of the number of blocks outstanding (i.e.,
//      // allocated but not yet deallocated).
//
//      // DATA
//      int d_blocksOutstanding;
//
//    public:
//      // CREATORS
//      my_CountingAllocator();
//          // Create a counting allocator that uses the operators 'new' and
//          // 'delete' to supply and free memory.
//
//      // MANIPULATORS
//      virtual void *allocate(size_type size);
//          // Return a pointer to an uninitialized memory of the specified
//          // size (in bytes).
//
//      virtual void deallocate(void *address);
//          // Return the memory at the specified 'address' to this allocator.
//
//      // ACCESSORS
//      int blocksOutstanding() const;
//          // Return the number of blocks outstanding (i.e., allocated but not
//          // yet deallocated by this counting allocator).
//  };
//..
// The implementation is really straightforward:
//..
//  // my_countingallocator.cpp
//
//                         // --------------------------
//                         // class my_CountingAllocator
//                         // --------------------------
//
//  // CREATORS
//  my_CountingAllocator::my_CountingAllocator()
//  : d_blocksOutstanding(0)
//  {
//  }
//
//  // MANIPULATORS
//  void *my_CountingAllocator::allocate(size_type size)
//  {
//      ++d_blocksOutstanding;
//      return operator new(size);
//  }
//
//  void my_CountingAllocator::deallocate(void *address)
//  {
//      --d_blocksOutstanding;
//      operator delete(address);
//  }
//
//  // ACCESSORS
//  int my_CountingAllocator::blocksOutstanding() const
//  {
//      return d_blocksOutstanding;
//  }
//..
// Now we can create array objects with different allocator mechanisms.  First
// we create an array, 'a1', using the default allocator and fill it with the
// values '1 .. 5':
//..
//  void usageExample() {
//
//      my_FixedSizeArray<int, bsl::allocator<int> > a1(5);
//      assert(5 == a1.length());
//      assert(bslma::Default::defaultAllocator() == a1.allocator());
//
//      for (int i = 0; i < a1.length(); ++i) {
//          a1[i] = i + 1;
//      }
//..
// Then we create a copy of 'a1' using the counting allocator.  The values of
// 'a1' and 'a2' are equal, even though they have different allocation
// mechanisms.
//..
//      my_CountingAllocator countingAlloc;
//      my_FixedSizeArray<int, bsl::allocator<int> > a2(a1,&countingAlloc);
//                 assert(a1 == a2);
//                 assert(a1.allocator() != a2.allocator());
//                 assert(&countingAlloc == a2.allocator());
//                 assert(1 == countingAlloc.blocksOutstanding());
//..
// Then we create a copy of 'a2' using the default allocator.  The values of
// 'a1', 'a2' and 'a3' are equal, even though they have different allocation
// mechanisms.
//..
//     my_FixedSizeArray<int, bsl::allocator<int> > a3(a2);
//                 assert(a1 == a3);
//                 assert(a1 == a2);
//                 assert(a2 == a3);
//                 assert(a1.allocator() == a3.allocator());
//                 assert(a1.allocator() != a2.allocator());
//                 assert(a2.allocator() != a3.allocator());
//                 assert(bsl::allocator<int>() == a3.allocator());
// }
//..
// To exercise the propagation of the allocator of 'MyContainer' to its
// elements, we first create a representative element class, 'MyType', that
// allocates memory using the bslma allocator protocol:
//..
// #include <bslma_default.h>
//
// class MyType {
//
//     bslma::Allocator *d_allocator_p;
//     // etc.
//   public:
//     // TRAITS
//     BSLMF_NESTED_TRAIT_DECLARATION(MyType, bslma::UsesBslmaAllocator);
//
//     // CREATORS
//     explicit MyType(bslma::Allocator* basicAlloc = 0)
//         : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
//     MyType(const MyType&)
//         : d_allocator_p(bslma::Default::allocator(0)) { /* ... */ }
//     MyType(const MyType&, bslma::Allocator* basicAlloc)
//         : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
//     // etc.
//
//     // ACCESSORS
//     bslma::Allocator *allocator() const { return d_allocator_p; }
//
//     // etc.
//};
//..
// Finally, we instantiate 'my_FixedSizeArray' using 'MyType' and verify that,
// when we provide the address of an allocator to the constructor of the
// container, the same address is passed to the constructor of the container's
// element.  We also verify that, when the container is copy-constructed, the
// copy uses the default allocator, not the allocator from the original;
// moreover, we verify that the element stored in the copy also uses the
// default allocator.
//..
// #include <bslmf_issame.h>
//
// void usageExample2()
// {
//    bslma::TestAllocator                               testAlloc;
//    my_FixedSizeArray<MyType, bsl::allocator<MyType> > C1a(7, &testAlloc);
//    ASSERT((bsl::is_same<
//        my_FixedSizeArray<MyType, bsl::allocator<MyType> >::allocator_type,
//        bsl::allocator<MyType> >::value));
//    ASSERT(C1a.allocator() == bsl::allocator<MyType>(&testAlloc));
//    ASSERT(C1a[0].allocator() == &testAlloc);
//
//    my_FixedSizeArray<MyType, bsl::allocator<MyType> > C2a(C1a);
//    ASSERT(C2a.allocator() != C1a.allocator());
//    ASSERT(C2a.allocator() == bsl::allocator<MyType>());
//    ASSERT(C2a[0].allocator() != &testAlloc);
//    ASSERT(C2a[0].allocator() == bslma::Default::defaultAllocator());
//
//    MyType                                             dummy;
//    my_FixedSizeArray<MyType, bsl::allocator<MyType> > C1b(7, &testAlloc);
//    ASSERT((bsl::is_same<
//        my_FixedSizeArray<MyType, bsl::allocator<MyType> >::allocator_type,
//        bsl::allocator<MyType> >::value));
//    ASSERT(C1b.allocator() == bsl::allocator<MyType>(&testAlloc));
//    ASSERT(C1b[0].allocator() == &testAlloc);
//
//    my_FixedSizeArray<MyType, bsl::allocator<MyType> > C2b(C1b);
//    ASSERT(C2b.allocator() != C1b.allocator());
//    ASSERT(C2b.allocator() == bsl::allocator<MyType>());
//    ASSERT(C2b[0].allocator() != &testAlloc);
//    ASSERT(C2b[0].allocator() == bslma::Default::defaultAllocator());
//}
//..

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <climits>
#include <cstddef>
#include <new>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Fri May 13 11:05:19 2022
// Command line: sim_cpp11_features.pl bslma_stdallocator.h
# define COMPILING_BSLMA_STDALLOCATOR_H
# include <bslma_stdallocator_cpp03.h>
# undef COMPILING_BSLMA_STDALLOCATOR_H
#else

namespace bsl {

                              // ===============
                              // class allocator
                              // ===============

template <class TYPE>
class allocator {
    // An STL-compatible allocator that forwards allocation calls to an
    // underlying mechanism object of a type derived from 'bslma::Allocator'.
    // This class template adheres to the allocator requirements defined in
    // section 20.1.5 [lib.allocator.requirements] of the C++ standard and may
    // be used to instantiate any [container] class template that follows the
    // STL allocator protocol.  The allocation mechanism is chosen at run-time,
    // giving the programmer run-time control over how a container allocates
    // and frees memory.

    // DATA
    BloombergLP::bslma::Allocator *d_mechanism;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(allocator, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(allocator,
                                   BloombergLP::bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(
        allocator,
        BloombergLP::bslmf::IsBitwiseEqualityComparable);
        // Declare nested type traits for this class.

    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef TYPE           *pointer;
    typedef const TYPE     *const_pointer;
    typedef TYPE&           reference;
    typedef const TYPE&     const_reference;
    typedef TYPE            value_type;

    template <class ANY_TYPE>
    struct rebind {
        // This nested 'struct' template, parameterized by 'ANY_TYPE', provides
        // a namespace for an 'other' type alias, which is an allocator type
        // following the same template as this one but that allocates elements
        // of 'ANY_TYPE'.  Note that this allocator type is convertible to and
        // from 'other' for any type, including 'void'.

        typedef allocator<ANY_TYPE> other;
    };

    // CREATORS
    allocator();
        // Create a proxy object which will forward allocation calls to the
        // object pointed to by 'bslma::Default::defaultAllocator()'.
        // Postcondition:
        //..
        //  this->mechanism() == bslma::Default::defaultAllocator();
        //..

    allocator(BloombergLP::bslma::Allocator *mechanism);            // IMPLICIT
        // Convert a 'bslma::Allocator' pointer to an 'allocator' object which
        // forwards allocation calls to the object pointed to by the specified
        // 'mechanism'.  If 'mechanism' is 0, then the currently installed
        // default allocator is used instead.  Postcondition:
        // '0 == mechanism || this->mechanism() == mechanism'.

    allocator(const allocator& original);
        // Create a proxy object using the same mechanism as the specified
        // 'original'.  Postcondition: 'this->mechanism() == rhs.mechanism()'.

    template <class ANY_TYPE>
    allocator(const allocator<ANY_TYPE>& rhs);
        // Create a proxy object sharing the same mechanism object as the
        // specified 'rhs'.  The newly constructed allocator will compare equal
        // to 'rhs', even though they are instantiated on different types.
        // Postcondition: 'this->mechanism() == rhs.mechanism()'.

    //! ~allocator();
        // Destroy this object.  Note that this does not delete the object
        // pointed to by 'mechanism()'.  Also note that this method's
        // definition is compiler generated.

    // MANIPULATORS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    allocator& operator=(const allocator& rhs) = default;
        // Assign to this object the value of the specified 'rhs'.
        // Postcondition: 'this->mechanism() == rhs->mechanism()'.  Note that
        // this does not delete the object pointed to by the previous value of
        // 'mechanism()'.  Also note that this method's definition is compiler
        // generated.  Also note that this must be explicitly defaulted to
        // silence compiler warnings on later versions of C++.
#else
    //! allocator& operator=(const allocator& rhs) = default;
#endif

    pointer allocate(size_type n, const void *hint = 0);
        // Allocate enough (properly aligned) space for the specified 'n'
        // objects of (template parameter) 'TYPE' by calling 'allocate' on the
        // mechanism object.  The optionally specified 'hint' argument is
        // ignored by this allocator type.  The behavior is undefined unless
        // 'n <= max_size()'.

    void deallocate(pointer p, size_type n = 1);
        // Return memory previously allocated with 'allocate' to the underlying
        // mechanism object by calling 'deallocate' on the mechanism object
        // with the specified 'p'.  The optionally specified 'n' argument is
        // ignored by this allocator type.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class ELEMENT_TYPE, class... Args>
    void construct(ELEMENT_TYPE *address, Args&&... arguments);
        // Construct an object of (template parameter) 'TYPE', by forwarding
        // the specified (variable number of) 'arguments' to the corresponding
        // constructor of 'ELEMENT_TYPE', at the specified uninitialized memory
        // 'address'.  The behavior is undefined unless 'address' is properly
        // aligned for objects of 'ELEMENT_TYPE'.
#endif

template <class ELEMENT_TYPE>
void destroy(ELEMENT_TYPE *address);
        // Call the 'TYPE' destructor for the object pointed to by the
        // specified 'p'.  Do not directly deallocate any memory.

    // ACCESSORS
pointer address(reference x) const;
        // Return the address of the object referred to by the specified 'x',
        // even if the (template parameter) 'TYPE' overloads the unary
        // 'operator&'.

const_pointer address(const_reference x) const;
        // Return the address of the object referred to by the specified 'x',
        // even if the (template parameter) 'TYPE' overloads the unary
        // 'operator&'.

size_type max_size() const;
        // Return the maximum number of elements of (template parameter) 'TYPE'
        // that can be allocated using this allocator.  Note that there is no
        // guarantee that attempts at allocating fewer elements than the value
        // returned by 'max_size' will not throw.

BloombergLP::bslma::Allocator *mechanism() const;
        // Return a pointer to the mechanism object to which this proxy
        // forwards allocation and deallocation calls.

allocator<TYPE> select_on_container_copy_construction() const;
        // TBD: add comment
}
;

                           // =====================
                           // class allocator<void>
                           // =====================

template <>
class allocator<void> {
    // Specialization of 'allocator<T>' where 'T' is 'void'.  Does not contain
    // members that are unrepresentable for 'void'

    // DATA
    BloombergLP::bslma::Allocator *d_mechanism;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(allocator, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(allocator,
                                   BloombergLP::bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(
        allocator,
        BloombergLP::bslmf::IsBitwiseEqualityComparable);
        // Declare nested type traits for this class.

    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef void           *pointer;
    typedef const void     *const_pointer;
    typedef void            value_type;

    template <class ANY_TYPE>
    struct rebind {
        typedef allocator<ANY_TYPE> other;
    };

    // CREATORS
    allocator();
        // Construct a proxy object which will forward allocation calls to the
        // object pointed to by 'bslma::Default::defaultAllocator()'.

    allocator(BloombergLP::bslma::Allocator *mechanism);            // IMPLICIT
        // Convert a 'bslma::Allocator' pointer to an 'allocator' object which
        // forwards allocation calls to the object pointed to by the specified
        // 'mechanism'.  If 'mechanism' is 0, then the current default
        // allocator is used instead.  Postcondition:
        // '0 == mechanism || this->mechanism() == mechanism'.

    allocator(const allocator& original);
        // Create a proxy object using the same mechanism as the specified
        // 'original'.  Postcondition: 'this->mechanism() == rhs.mechanism()'.

    template <class ANY_TYPE>
    allocator(const allocator<ANY_TYPE>& rhs);
        // Construct a proxy object sharing the same mechanism object as the
        // specified 'rhs'.  The newly constructed allocator will compare equal
        // to 'rhs', even though they are instantiated on different types.
        // Postcondition: 'this->mechanism() == rhs.mechanism()'.

    //! ~allocator();
        // Destroy this object.  Note that this does not delete the object
        // pointed to by 'mechanism()'.  Also note that this method's
        // definition is compiler generated.

    // MANIPULATORS
    //! allocator& operator=(const allocator& rhs);
        // Assign this object the value of the specified 'rhs'.  Postcondition:
        // 'this->mechanism() == rhs->mechanism()'.  Note that this does not
        // delete the object pointed to by the previous value of 'mechanism()'.
        // Also note that this method's definition is compiler generated.

    // ACCESSORS
    BloombergLP::bslma::Allocator *mechanism() const;
        // Return a pointer to the mechanism object to which this proxy
        // forwards allocation and deallocation calls.

    allocator<void> select_on_container_copy_construction() const;
        // TBD: add comment
};

                  // ========================================
                  // class allocator_traits<allocator<TYPE> >
                  // ========================================

template <class TYPE>
struct allocator_traits<allocator<TYPE> > {
    // This 'struct' template provides a specialization of the
    // 'allocator_traits' class template for 'bsl::allocator'.

    // PUBLIC TYPES
    typedef allocator<TYPE> allocator_type;
    typedef TYPE            value_type;

    typedef TYPE           *pointer;
    typedef const TYPE     *const_pointer;
    typedef void           *void_pointer;
    typedef const void     *const_void_pointer;
    typedef std::ptrdiff_t  difference_type;
    typedef std::size_t     size_type;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    template <class ELEMENT_TYPE>
    using rebind_alloc = allocator<ELEMENT_TYPE>;

    template <class ELEMENT_TYPE>
    using rebind_traits = allocator_traits<allocator<ELEMENT_TYPE> >;
#else
    template <class ELEMENT_TYPE>
    struct rebind_alloc : allocator<ELEMENT_TYPE> {
        rebind_alloc()
        : allocator<ELEMENT_TYPE>()
        {
        }

        template <typename ARG>
        rebind_alloc(const ARG& allocatorArg)
            // Convert from anything that can be used to cosntruct the base
            // type.  This might be better if SFINAE-ed out using
            // 'is_convertible', but stressing older compilers more seems
            // unwise.
        : allocator<ELEMENT_TYPE>(allocatorArg)
        {
        }
    };

    template <class ELEMENT_TYPE>
    struct rebind_traits : allocator_traits<allocator<ELEMENT_TYPE> > {
    };
#endif

    static pointer allocate(allocator<TYPE>& m, size_type n)
    {
        return m.allocate(n);
    }

    static pointer allocate(allocator<TYPE>&   m,
                            size_type          n,
                            const_void_pointer hint)
    {
        return m.allocate(n, hint);
    }

    static void deallocate(allocator<TYPE>& m, pointer p, size_type n)
    {
        m.deallocate(p, n);
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class ELEMENT_TYPE, class... Args>
    static void construct(allocator<TYPE>&  m,
                          ELEMENT_TYPE     *p,
                          Args&&...         arguments)
    {
        m.construct(p, BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
#endif

    template <class ELEMENT_TYPE>
    static void destroy(allocator<TYPE>& m, ELEMENT_TYPE *p)
    {
        m.destroy(p);
    }

    static size_type max_size(const allocator<TYPE>& m)
    {
        return m.max_size();
    }

    // Allocator propagation traits
    static allocator<TYPE> select_on_container_copy_construction(
                                                        const allocator<TYPE>&)
    {
        return allocator<TYPE>();
    }

    typedef false_type is_always_equal;

    typedef false_type propagate_on_container_copy_assignment;

    typedef false_type propagate_on_container_move_assignment;

    typedef false_type propagate_on_container_swap;
};

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const allocator<T1>& lhs, const allocator<T2>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' are proxies for the same
    // 'bslma::Allocator' object.  This is a practical implementation of the
    // STL requirement that two allocators compare equal if and only if memory
    // allocated from one can be deallocated from the other.  Note that the two
    // allocators need not be instantiated on the same type in order to compare
    // equal.

template <class T1, class T2>
inline
bool operator!=(const allocator<T1>& lhs, const allocator<T2>& rhs);
    // Return 'true' unless the specified 'lhs' and 'rhs' are proxies for the
    // same 'bslma::Allocator' object, in which case return 'false'.  This is a
    // practical implementation of the STL requirement that two allocators
    // compare equal if and only if memory allocated from one can be
    // deallocated from the other.  Note that the two allocators need not be
    // instantiated on the same type in order to compare equal.

template <class TYPE>
inline
bool operator==(const allocator<TYPE>&         lhs,
                BloombergLP::bslma::Allocator *rhs);
    // Return 'true' if the specified 'lhs' is a proxy for the specified 'rhs',
    // and 'false' otherwise.  Note that 'rhs' points to a *modifiable*
    // 'bslma::Allocator', as 'bsl::allocator' cannot proxy a 'const
    // bslma::Allocator *'

template <class TYPE>
inline
bool operator!=(const allocator<TYPE>&         lhs,
                BloombergLP::bslma::Allocator *rhs);
    // Return 'true' unless the specified 'lhs' is a proxy for the specified
    // 'rhs', in which case return 'false'.  Note that 'rhs' points to a
    // *modifiable* 'bslma::Allocator', as 'bsl::allocator' cannot proxy a
    // 'const bslma::Allocator *'
template <class TYPE>
inline
bool operator==(BloombergLP::bslma::Allocator *lhs,
                const allocator<TYPE>&         rhs);
    // Return 'true' if the specified 'rhs' is a proxy for the specified 'lhs',
    // and 'false' otherwise.  Note that 'lhs' points to a *modifiable*
    // 'bslma::Allocator', as 'bsl::allocator' cannot proxy a 'const
    // bslma::Allocator *'

template <class TYPE>
inline
bool operator!=(BloombergLP::bslma::Allocator *lhs,
                const allocator<TYPE>&         rhs);
    // Return 'true' unless the specified 'rhs' is a proxy for the specified
    // 'lhs', in which case return 'false'.  Note that 'lhs' points to a
    // *modifiable* 'bslma::Allocator', as 'bsl::allocator' cannot proxy a
    // 'const bslma::Allocator *'

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // ---------------
                              // class allocator
                              // ---------------

// LOW-LEVEL ACCESSORS
template <class TYPE>
inline
BloombergLP::bslma::Allocator *allocator<TYPE>::mechanism() const
{
    return d_mechanism;
}

// CREATORS
template <class TYPE>
inline
allocator<TYPE>::allocator()
: d_mechanism(BloombergLP::bslma::Default::defaultAllocator())
{
}

template <class TYPE>
inline
allocator<TYPE>::allocator(BloombergLP::bslma::Allocator *mechanism)
: d_mechanism(BloombergLP::bslma::Default::allocator(mechanism))
{
}

template <class TYPE>
inline
allocator<TYPE>::allocator(const allocator& original)
: d_mechanism(original.mechanism())
{
}

template <class TYPE>
template <class ANY_TYPE>
inline
allocator<TYPE>::allocator(const allocator<ANY_TYPE>& rhs)
: d_mechanism(rhs.mechanism())
{
}

// MANIPULATORS
template <class TYPE>
inline
typename allocator<TYPE>::pointer allocator<TYPE>::allocate(
                                           typename allocator::size_type  n,
                                           const void                    *hint)
{
    BSLS_ASSERT_SAFE(n <= this->max_size());

    (void) hint;  // suppress unused parameter warning
    return static_cast<pointer>(d_mechanism->allocate(n * sizeof(TYPE)));
}

template <class TYPE>
inline
void allocator<TYPE>::deallocate(typename allocator::pointer   p,
                                 typename allocator::size_type n)
{
    (void) n;  // suppress unused parameter warning
    d_mechanism->deallocate(p);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class ELEMENT_TYPE, class... Args>
inline
void allocator<TYPE>::construct(ELEMENT_TYPE *address, Args&&... arguments)
{
    BloombergLP::bslma::ConstructionUtil::construct(
                            address,
                            d_mechanism,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
}
#endif

template <class TYPE>
template <class ELEMENT_TYPE>
inline
void allocator<TYPE>::destroy(ELEMENT_TYPE *address)
{
    BloombergLP::bslma::DestructionUtil::destroy(address);
}

// ACCESSORS
template <class TYPE>
inline
typename allocator<TYPE>::const_pointer allocator<TYPE>::address(
                                                       const_reference x) const
{
    return BSLS_UTIL_ADDRESSOF(x);
}

template <class TYPE>
inline
typename allocator<TYPE>::pointer allocator<TYPE>::address(reference x) const
{
    return BSLS_UTIL_ADDRESSOF(x);
}

template <class TYPE>
inline
typename allocator<TYPE>::size_type allocator<TYPE>::max_size() const
{
    // Return the largest value, 'v', such that 'v * sizeof(T)' fits in a
    // 'size_type'.

    // We will calculate MAX_NUM_BYTES based on our knowledge that
    // 'bslma::Allocator::size_type' is just an alias for 'std::size_t'.  First
    // demonstrate that is true:

    BSLMF_ASSERT((bsl::is_same<BloombergLP::bslma::Allocator::size_type,
                               std::size_t>::value));

    static const std::size_t MAX_NUM_BYTES    = ~std::size_t(0);
    static const std::size_t MAX_NUM_ELEMENTS = MAX_NUM_BYTES / sizeof(TYPE);

    return MAX_NUM_ELEMENTS;
}

template <class TYPE>
inline
allocator<TYPE> allocator<TYPE>::select_on_container_copy_construction() const
{
    return allocator<TYPE>();
}

                           // ---------------------
                           // class allocator<void>
                           // ---------------------

// LOW-LEVEL ACCESSORS
inline
BloombergLP::bslma::Allocator *allocator<void>::mechanism() const
{
    return d_mechanism;
}

// CREATORS
inline
allocator<void>::allocator()
: d_mechanism(BloombergLP::bslma::Default::defaultAllocator())
{
}

inline
allocator<void>::allocator(BloombergLP::bslma::Allocator *mechanism)
: d_mechanism(BloombergLP::bslma::Default::allocator(mechanism))
{
}

// 'template <>' is needed only for versions of xlC prior to 9
#if defined(__xlC__) && __xlC__ < 0x900
template <>
#endif
inline
allocator<void>::allocator(const allocator<void>& original)
: d_mechanism(original.mechanism())
{
}

template <class ANY_TYPE>
inline
allocator<void>::allocator(const allocator<ANY_TYPE>& rhs)
: d_mechanism(rhs.mechanism())
{
}

inline
allocator<void> allocator<void>::select_on_container_copy_construction() const
{
    return allocator<void>();
}

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const allocator<T1>& lhs, const allocator<T2>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T1, class T2>
inline
bool operator!=(const allocator<T1>& lhs, const allocator<T2>& rhs)
{
    return !(lhs == rhs);
}

template <class TYPE>
inline
bool operator==(const allocator<TYPE>&         lhs,
                BloombergLP::bslma::Allocator *rhs)
{
    return lhs.mechanism() == rhs;
}

template <class TYPE>
inline
bool operator!=(const allocator<TYPE>&         lhs,
                BloombergLP::bslma::Allocator *rhs)
{
    return !(lhs == rhs);
}

template <class TYPE>
inline
bool operator==(BloombergLP::bslma::Allocator *lhs,
                const allocator<TYPE>&         rhs)
{
    return lhs == rhs.mechanism();
}

template <class TYPE>
inline
bool operator!=(BloombergLP::bslma::Allocator *lhs,
                const allocator<TYPE>&         rhs)
{
    return !(lhs == rhs);
}

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace BloombergLP {
namespace bslma {

template <class TYPE>
struct UsesBslmaAllocator< ::bsl::allocator<TYPE> > : bsl::false_type {
};

}  // close namespace bslma
}  // close enterprise namespace

#endif // End C++11 code

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
