// bslstl_allocator.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALLOCATOR
#define INCLUDED_BSLSTL_ALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compatible proxy for 'bslma::Allocator' objects.
//
//@CLASSES:
//  bsl::allocator: STL-compatible allocator template
//
//@SEE_ALSO: bslma_allocator
//
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
//  // ...
//
//                             // =======================
//                             // class my_FixedSizeArray
//                             // =======================
//
//  template <class T, class ALLOC>
//  class my_FixedSizeArray {
//      // This class provides an array of the parameterized 'T' type passed of
//      // fixed length at construction, using an object of the parameterized
//      // 'ALLOC' type to supply memory.
//
//      // DATA
//      ALLOC  d_allocator;
//      int    d_length;
//      T     *d_array;
//
//    public:
//      // TYPES
//      typedef ALLOC  allocator_type;
//      typedef T      value_type;
//
//      // CREATORS
//      my_FixedSizeArray(int length, const ALLOC& allocator = ALLOC());
//          // Create a fixed-size array of the specified 'length', using the
//          // optionally specified 'allocator' to supply memory.  If
//          // 'allocator' is not specified, a default-constructed object of
//          // the parameterized 'ALLOC' type is used.  Note that all the
//          // elements in that array are default-constructed.
//
//      my_FixedSizeArray(const my_FixedSizeArray& original,
//                        const ALLOC&             allocator = ALLOC());
//          // Create a copy of the specified 'original' fixed-size array,
//          // using the optionally specified 'allocator' to supply memory.  If
//          // 'allocator' is not specified, a default-constructed object of
//          // the parameterized 'ALLOC' type is used.
//
//      ~my_FixedSizeArray();
//          // Destroy this fixed size array.
//
//      // MANIPULATORS
//      T& operator[](int index);
//          // Return a reference to the modifiable element at the specified
//          // 'index' position in this fixed size array.
//
//      // ACCESSORS
//      const T& operator[](int index) const;
//          // Return a reference to the modifiable element at the specified
//          // 'index' position in this fixed size array.
//
//      int length() const;
//          // Return the length specified at construction of this fixed size
//          // array.
//
//      const ALLOC& allocator() const;
//          // Return a reference to the non-modifiable allocator used by this
//          // fixed size array to supply memory.  This is here for
//          // illustrative purposes.  We should not generally have an accessor
//          // to return the allocator.
//  };
//
//  // FREE OPERATORS
//  template<class T, class ALLOC>
//  bool operator==(const my_FixedSizeArray<T,ALLOC>& lhs,
//                  const my_FixedSizeArray<T,ALLOC>& rhs)
//      // Return 'true' if the specified 'lhs' fixed-size array has the same
//      // value as the specified 'rhs' fixed-size array, and 'false'
//      // otherwise.  Two fixed-size arrays have the same value if they have
//      // the same length and if the element at any index in 'lhs' has the
//      // same value as the corresponding element at the same index in 'rhs'.
//..
// The implementation is straightforward
//..
//  // my_fixedsizearray.cpp
//  // ...
//                         // -----------------------
//                         // class my_FixedSizeArray
//                         // -----------------------
//
//  // CREATORS
//  template<class T, class ALLOC>
//  my_FixedSizeArray<T,ALLOC>::my_FixedSizeArray(int          length,
//                                                const ALLOC& allocator)
//  : d_allocator(allocator), d_length(length)
//  {
//      d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes
//
//      // Default construct each element of the array:
//      for (int i = 0; i < d_length; ++i) {
//          d_allocator.construct(&d_array[i], T());
//      }
//  }
//
//  template<class T, class ALLOC>
//  my_FixedSizeArray<T,ALLOC>::my_FixedSizeArray(
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
//  template<class T, class ALLOC>
//  my_FixedSizeArray<T,ALLOC>::~my_FixedSizeArray()
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
//  template<class T, class ALLOC>
//  inline T& my_FixedSizeArray<T,ALLOC>::operator[](int i)
//  {
//      return d_array[i];
//  }
//
//  // ACCESSORS
//  template<class T, class ALLOC>
//  inline
//  const T& my_FixedSizeArray<T,ALLOC>::operator[](int i) const
//  {
//      return d_array[i];
//  }
//
//  template<class T, class ALLOC>
//  inline int my_FixedSizeArray<T,ALLOC>::length() const
//  {
//      return d_length;
//  }
//
//  template<class T, class ALLOC>
//  inline
//  const ALLOC& my_FixedSizeArray<T,ALLOC>::allocator() const
//  {
//      return d_allocator;
//  }
//
//  // FREE OPERATORS
//  template<class T, class ALLOC>
//  bool operator==(const my_FixedSizeArray<T,ALLOC>& lhs,
//                  const my_FixedSizeArray<T,ALLOC>& rhs)
//  {
//      if (lhs.length() != rhs.length()) {
//          return false;
//      }
//      for (int i = 0; i < lhs.length(); ++i) {
//          if (lhs[i] != rhs[i]) {
//              return false;
//          }
//      }
//      return true;
//  }
//..
// Now we declare an allocator mechanism.  Our mechanism will be to simply call
// global 'operator new' and 'operator delete' functions, and count the number
// of blocks outstanding (allocated but not deallocated).  Note that a more
// reusable implementation would take an underlying mechanism at construction.
// We keep things simple only for the sake of this example.
//..
//  // my_countingallocator.h
//
//                           // ==========================
//                           // class my_CountingAllocator
//                           // ==========================
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
//      // Create a counting allocator that uses the operators 'new' and
//      // 'delete' to supply and free memory.
//
//      // MANIPULATORS
//      virtual void *allocate(size_type size);
//          // Return a pointer to an uninitialized memory of the specified
//          // 'size (in bytes).
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
//                           // --------------------------
//                           // class my_CountingAllocator
//                           // --------------------------
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
// values '[1 .. 5]':
//..
//  int main() {
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
//      assert(a1 == a2);
//      assert(a1.allocator() != a2.allocator());
//      assert(&countingAlloc == a2.allocator());
//      assert(1 == countingAlloc.blocksOutstanding())
//  }
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_memory.h> instead of <bslstl_allocator.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#include <bslmf_isbitwiseequalitycomparable.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_NEW
#include <new>
#define INCLUDED_NEW
#endif

#ifndef INCLUDED_CLIMITS
#include <climits>
#define INCLUDED_CLIMITS
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

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
    BSLMF_NESTED_TRAIT_DECLARATION(allocator,
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

    //! allocator& operator=(const allocator& rhs);
        // Assign to this object the value of the specified 'rhs'.
        // Postcondition: 'this->mechanism() == rhs->mechanism()'.  Note that
        // this does not delete the object pointed to by the previous value of
        // 'mechanism()'.  Also note that this method's definition is compiler
        // generated.

    // MANIPULATORS
    pointer allocate(size_type n, const void *hint = 0);
        // Allocate enough (properly aligned) space for the specified 'n'
        // objects of (template parameter) 'TYPE' by calling 'allocate' on the
        // mechanism object.  The optionally specified 'hint' argument is
        // ignored by this allocator type.  The behavior is undefined unless
        // 'n <= max_size()'.

    void deallocate(pointer p, size_type n = 1);
        // Return memory previously allocated with 'allocate' to the underlying
        // mechanism object by calling 'deallocate' on the the mechanism
        // object with the specified 'p'.  The optionally specified 'n'
        // argument is ignored by this allocator type.

    void construct(pointer p, const TYPE& val);
        // Copy-construct an object of (template parameter) 'TYPE' from the
        // specified 'val' at the memory address specified by 'p'.  Do not
        // directly allocate memory.  The behavior is undefined unless 'p' is
        // not properly aligned for objects of the given 'TYPE'.

    void destroy(pointer p);
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
};

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
    BSLMF_NESTED_TRAIT_DECLARATION(allocator,
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
};

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const allocator<T1>& lhs,
                const allocator<T2>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' are proxies for the same
    // 'bslma::Allocator' object.  This is a practical implementation of the
    // STL requirement that two allocators compare equal if and only if memory
    // allocated from one can be deallocated from the other.  Note that the two
    // allocators need not be instantiated on the same type in order to compare
    // equal.

template <class T1, class T2>
inline
bool operator!=(const allocator<T1>& lhs,
                const allocator<T2>& rhs);
    // Return 'true' unless the specified 'lhs' and 'rhs' are proxies for the
    // same 'bslma::Allocator' object, in which case return 'false'.  This is a
    // practical implementation of the STL requirement that two allocators
    // compare equal if and only if memory allocated from one can be
    // deallocated from the other.  Note that the two allocators need not be
    // instantiated on the same type in order to compare equal.


template <class TYPE>
inline
bool operator==(const allocator<TYPE>&               lhs,
                const BloombergLP::bslma::Allocator *rhs);
    // Return 'true' if the specified 'lhs' is a proxy for the specified 'rhs',
    // and 'false' otherwise.

template <class TYPE>
inline
bool operator!=(const allocator<TYPE>&               lhs,
                const BloombergLP::bslma::Allocator *rhs);
    // Return 'true' unless the specified 'lhs' is a proxy for the specified
    // 'rhs', in which case return 'false'.


template <class TYPE>
inline
bool operator==(const BloombergLP::bslma::Allocator *lhs,
                const allocator<TYPE>&               rhs);
    // Return 'true' if the specified 'rhs' is a proxy for the specified 'lhs',
    // and 'false' otherwise.

template <class TYPE>
inline
bool operator!=(const BloombergLP::bslma::Allocator *lhs,
                const allocator<TYPE>&               rhs);
    // Return 'true' unless the specified 'rhs' is a proxy for the specified
    // 'lhs', in which case return 'false'.

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
typename allocator<TYPE>::pointer
allocator<TYPE>::allocate(typename allocator::size_type  n,
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

template <class TYPE>
inline
void allocator<TYPE>::construct(typename allocator::pointer p,
                                const TYPE&                 val)
{
    new(static_cast<void*>(p)) TYPE(val);
}

template <class TYPE>
inline
void allocator<TYPE>::destroy(typename allocator::pointer p)
{
    p->~TYPE();
}

// ACCESSORS
template <class TYPE>
inline
typename allocator<TYPE>::const_pointer
allocator<TYPE>::address(const_reference x) const
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
#if defined(__xlC__) && __xlC__<0x900
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
    return ! (lhs == rhs);
}

template <class TYPE>
inline
bool operator==(const allocator<TYPE>&               lhs,
                const BloombergLP::bslma::Allocator *rhs)
{
    return lhs.mechanism() == rhs;
}

template <class TYPE>
inline
bool operator!=(const allocator<TYPE>&               lhs,
                const BloombergLP::bslma::Allocator *rhs)
{
    return ! (lhs == rhs);
}

template <class TYPE>
inline
bool operator==(const BloombergLP::bslma::Allocator *lhs,
                const allocator<TYPE>&               rhs)
{
    return lhs == rhs.mechanism();
}

template <class TYPE>
inline
bool operator!=(const BloombergLP::bslma::Allocator *lhs,
                const allocator<TYPE>&               rhs)
{
    return ! (lhs == rhs);
}

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace BloombergLP {
namespace bslma {

template <class TYPE>
struct UsesBslmaAllocator< ::bsl::allocator<TYPE> > : bsl::false_type {};

}  // close namespace bslma
}  // close enterprise namespace
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
