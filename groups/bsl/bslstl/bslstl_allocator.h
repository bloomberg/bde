// bslstl_allocator.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALLOCATOR
#define INCLUDED_BSLSTL_ALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compatible proxy for 'bslma_Allocator' objects.
//
//@CLASSES:
//  bsl::allocator: STL-compatible allocator template
//  bsl::Allocator_BslalgTypeTraits: type traits for 'bsl::allocator'
//
//@SEE_ALSO:
//  bslma_allocator
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<memory>' instead and use 'bsl::allocator' directly.  This component
// provides an STL-compatible proxy for any allocator class derived from
// 'bslma_Allocator'.  The proxy class, 'bsl::allocator' is a template that
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
// pointer to a *mechanism* *object* derived from 'bslma_Allocator'.  Each
// class derived from 'bslma_Allocator' implements a specific allocation
// mechanism and is thus called a *mechanism* *class* within this component.
// The 'bsl::allocator' object forwards calls made through the standard
// allocator interface to the mechanism object with which it was initialized.
// In this way, two containers instantiated with 'bsl::allocator' can utilize
// different allocation mechanisms even though they have the same compile-time
// type.  The default mechanism object, if none is supplied to the
// 'bsl::allocator' constructor, is 'bslma_Default::defaultAllocator()'.
//
// Instantiations of 'bsl::allocator' have full value semantics (well-behaved
// copy construction, assignment, and tests for equality).  Note, however, that
// a 'bsl::allocator' object does not "own" the 'bslma_Allocator' with which it
// is initialized.  In practice , this means that copying a 'bsl::allocator'
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
//                       const ALLOC&            allocator = ALLOC());
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
//                                                const ALLOC& alloc)
//  : d_allocator(alloc), d_length(length)
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
//                                              const my_FixedSizeArray& rhs,
//                                              const ALLOC&             alloc)
//  : d_allocator(alloc), d_length(rhs.d_length)
//  {
//      d_array = d_allocator.allocate(d_length);  // sizeof(T)*d_length bytes
//
//      // copy construct each element of the array:
//      for (int i = 0; i < d_length; ++i) {
//          d_allocator.construct(&d_array[i], rhs.d_array[i]);
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
//  class my_CountingAllocator : public bslma_Allocator {
//      // This concrete implementation of the 'bslma_Allocator' protocol
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
// values '[ 1 .. 5 ]':
//..
//  int main() {
//
//      my_FixedSizeArray<int, bsl::allocator<int> > a1(5);
//                 assert(5 == a1.length());
//                 assert(bslma_Default::defaultAllocator() == a1.allocator());
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
//                 assert(1 == countingAlloc.blocksOutstanding())
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
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

                      // ================================
                      // class Allocator_BslalgTypeTraits
                      // ================================

struct Allocator_BslalgTypeTraits
    : BloombergLP::bslalg_TypeTraitBitwiseCopyable
    , BloombergLP::bslalg_TypeTraitBitwiseMoveable
    , BloombergLP::bslalg_TypeTraitBitwiseEqualityComparable
{
    // Type traits for 'bsl::allocator'.  This class cannot be nested within
    // 'allocator' because doing so confuses the AIX xlC 6 compiler, which
    // sometimes thinks that the nested struct is private even when it's in the
    // private section of the enclosing class.
};

                             // ===============
                             // class allocator
                             // ===============

template <class T>
class allocator {
    // An STL-compatible allocator that forwards allocation calls to an
    // underlying mechanism object of a type derived from 'bslma_Allocator'.
    // This class template adheres to the allocator requirements defined in
    // section 20.1.5 [lib.allocator.requirements] of the C++ standard and may
    // be used to instantiate any [container] class template that follows the
    // STL allocator protocol.  The allocation mechanism is chosen at run-time,
    // giving the programmer run-time control over how a container allocates
    // and frees memory.

    // DATA
    BloombergLP::bslma_Allocator *d_mechanism;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(allocator, Allocator_BslalgTypeTraits);
        // Declare nested type traits for this class.

    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T              *pointer;
    typedef const T        *const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template <class U> struct rebind
    {
        // This nested 'struct' template, parameterized by some type 'U',
        // provides a namespace for an 'other' type alias, which is an
        // allocator type following the same template as this one but that
        // allocates elements of type 'U'.  Note that this allocator type is
        // convertible to and from 'other' for any type 'U' including 'void'.

        typedef allocator<U> other;
    };

    // CREATORS
    allocator();
        // Construct a proxy object which will forward allocation calls to the
        // object pointed to by 'bslma_Default::defaultAllocator()'.
        // Postcondition:
        //..
        //  this->mechanism() == bslma_Default::defaultAllocator();
        //..

    allocator(BloombergLP::bslma_Allocator *mechanism);
        // Convert a 'bslma_Allocator' pointer to a 'allocator' object which
        // forwards allocation calls to the object pointed to by the specified
        // 'mechanism'.  If 'mechanism' is 0, then the currently installed
        // default allocator is used instead.  Postcondition:
        // '0 == mechanism || this->mechanism() == mechanism'.

    allocator(const allocator& rhs);
        // Copy construct a proxy object using the same mechanism as rhs.
        // Postcondition: 'this->mechanism() == rhs.mechanism()'.

    template <class U>
    allocator(const allocator<U>& rhs);
        // Construct a proxy object sharing the same mechanism object as 'rhs'.
        // The newly constructed allocator will compare equal to rhs, even
        // though they are instantiated on different types.  Postcondition:
        // 'this->mechanism() == rhs.mechanism()'.

    //! ~allocator();
        // Destroy this object.  Note that this does not delete the object
        // pointed to by 'mechanism()'.  Also note that this method's
        // definition is compiler generated.

    //! allocator& operator=(const allocator& rhs);
        // Assign this object the value of the specified 'rhs'.  Postcondition:
        // 'this->mechanism() == rhs->mechanism()'.  Note that this does not
        // delete the object pointed to by the previous value of 'mechanism()'.
        // Also note that this method's definition is compiler generated.

    // MANIPULATORS
    pointer allocate(size_type n, const void *hint = 0);
        // Allocate enough (properly aligned) space for 'n' objects of type 'T'
        // by calling 'allocate' on the mechanism object.  The 'hint' argument
        // is ignored by this allocator type.  The behavior is undefined unless
        // 'n <= max_size()'.

    void deallocate(pointer p, size_type n = 1);
        // Return memory previously allocated with 'allocate' to the underlying
        // mechanism object by calling 'deallocate' on the the mechanism
        // object.  The 'n' argument is ignored by this allocator type.

    void construct(pointer p, const T& val);
        // Copy-construct a 'T' object at the memory address specified by 'p'.
        // Do not directly allocate memory.  Undefined if 'p' is not properly
        // aligned for 'T'.

    void destroy(pointer p);
        // Call the 'T' destructor for the object pointed to by 'p'.  Do not
        // directly deallocate any memory.

    // ACCESSORS
    BloombergLP::bslma_Allocator *mechanism() const;
        // Return a pointer to the mechanism object to which this proxy
        // forwards allocation and deallocation calls.

    pointer address(reference x) const;
        // Return the address of 'x'.

    const_pointer address(const_reference x) const;
        // Return the address of 'x'.

    size_type max_size() const;
        // Return the maximum number of elements of type 'T' that can be
        // allocated using this allocator.  Note that there is no guarantee
        // that attempts at allocating less elements than the value returned by
        // 'max_size' will not throw.
};

                          // =====================
                          // class allocator<void>
                          // =====================

template <>
class allocator<void> {
    // Specialization of 'allocator<T>' where 'T' is 'void'.  Does not contain
    // members that are unrepresentable for 'void'

    // DATA
    BloombergLP::bslma_Allocator *d_mechanism;

  public:
    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef void           *pointer;
    typedef const void     *const_pointer;
    typedef void            value_type;

    template <class U> struct rebind
    {
        typedef allocator<U> other;
    };

    BSLALG_DECLARE_NESTED_TRAITS(allocator, Allocator_BslalgTypeTraits);
        // Declare nested type traits for this class.

    // CREATORS
    allocator();
        // Construct a proxy object which will forward allocation calls to the
        // object pointed to by 'bslma_Default::defaultAllocator()'.

    allocator(BloombergLP::bslma_Allocator *mechanism);
        // Convert a 'bslma_Allocator' pointer to a 'allocator' object which
        // forwards allocation calls to the object pointed to by the specified
        // 'mechanism'.  If 'mechanism' is 0, then the current default
        // allocator is used instead.  Postcondition:
        // '0 == mechanism || this->mechanism() == mechanism'.

    allocator(const allocator& rhs);
        // Copy construct a proxy object using the same mechanism as rhs.
        // Postcondition: 'this->mechanism() == rhs.mechanism()'.

    template <class U>
    allocator(const allocator<U>& rhs);
        // Construct a proxy object sharing the same mechanism object as 'rhs'.
        // The newly constructed allocator will compare equal to rhs, even
        // though they are instantiated on different types.  Postcondition:
        // 'this->mechanism() == rhs.mechanism()'.

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
    BloombergLP::bslma_Allocator *mechanism() const;
        // Return a pointer to the mechanism object to which this proxy
        // forwards allocation and deallocation calls.
};

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const allocator<T1>& lhs,
                const allocator<T2>& rhs);
    // Return true 'lhs' and 'rhs' are proxies for the same 'bslma_Allocator'
    // object.  This is a practical implementation of the STL requirement that
    // two allocators compare equal if and only if memory allocated from one
    // can be deallocated from the other.  Note that the two allocators need
    // not be instantiated on the same type in order to compare equal.

template <class T1, class T2>
inline
bool operator!=(const allocator<T1>& lhs,
                const allocator<T2>& rhs);
    // Return '!(lhs == rhs)'.

template <class T>
inline
bool operator==(const allocator<T>&                 lhs,
                const BloombergLP::bslma_Allocator *rhs);
    // Return true if 'lhs.mechanism() == rhs'.

template <class T>
inline
bool operator!=(const allocator<T>&                 lhs,
                const BloombergLP::bslma_Allocator *rhs);
    // Return '!(lhs == rhs)'.

template <class T>
inline
bool operator==(const BloombergLP::bslma_Allocator *lhs,
                const allocator<T>&                 rhs);
    // Return true if 'lhs == rhs.mechanism()'.

template <class T>
inline
bool operator!=(const BloombergLP::bslma_Allocator *lhs,
                const allocator<T>&                 rhs);
    // Return '!(lhs == rhs)'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                             // ---------------
                             // class allocator
                             // ---------------

// LOW-LEVEL ACCESSORS
template <class T>
inline
BloombergLP::bslma_Allocator *allocator<T>::mechanism() const
{
    return d_mechanism;
}

// CREATORS
template <class T>
inline
allocator<T>::allocator()
: d_mechanism(BloombergLP::bslma_Default::defaultAllocator())
{
}

template <class T>
inline
allocator<T>::allocator(BloombergLP::bslma_Allocator *mechanism)
: d_mechanism(BloombergLP::bslma_Default::allocator(mechanism))
{
}

template <class T>
inline
allocator<T>::allocator(const allocator& rhs)
: d_mechanism(rhs.mechanism())
{
}

template <class T>
template <class U>
inline
allocator<T>::allocator(const allocator<U>& rhs)
: d_mechanism(rhs.mechanism())
{
}

// MANIPULATORS
template <class T>
inline
typename allocator<T>::pointer
allocator<T>::allocate(typename allocator::size_type  n,
                       const void                    *hint)
{
    // Both 'bslma_Allocator::size_type' and 'allocator<T>::size_type' have the
    // same width; however, the former is signed, but the latter is not.  Hence
    // the cast in the argument of 'allocate' below.

    (void) hint;  // suppress warning
    return static_cast<pointer>(d_mechanism->allocate(
        BloombergLP::bslma_Allocator::size_type(n * sizeof(T))));
}

template <class T>
inline
void allocator<T>::deallocate(typename allocator::pointer   p,
                              typename allocator::size_type n)
{
    (void) n;  // suppress warning
    d_mechanism->deallocate(p);
}

template <class T>
inline
void allocator<T>::construct(typename allocator::pointer p,
                             const T&                    val)
{
    new(static_cast<void*>(p)) T(val);
}

template <class T>
inline
void allocator<T>::destroy(typename allocator::pointer p)
{
    p->~T();
}

// ACCESSORS
template <class T>
inline
typename allocator<T>::size_type allocator<T>::max_size() const
{
    // Return the largest value, 'v', such that 'v * sizeof(T)' fits in a
    // 'size_type'.

    // TBD Should these 'const' variables be declared 'static'?
    static const bool BSLMA_SIZE_IS_SIGNED =
                              ~BloombergLP::bslma_Allocator::size_type(0) < 0;
    static const std::size_t MAX_NUM_BYTES        =
                              ~std::size_t(0) / (BSLMA_SIZE_IS_SIGNED ? 2 : 1);
    static const std::size_t MAX_NUM_ELEMENTS     =
                              std::size_t(MAX_NUM_BYTES) / sizeof(T);

    return MAX_NUM_ELEMENTS;
}

template <class T>
inline
typename allocator<T>::pointer allocator<T>::address(reference x) const
{
    return &x;
}

template <class T>
inline
typename allocator<T>::const_pointer allocator<T>::address(const_reference x)
                                                                          const
{
    return &x;
}

                          // ---------------------
                          // class allocator<void>
                          // ---------------------

// LOW-LEVEL ACCESSORS
inline
BloombergLP::bslma_Allocator *allocator<void>::mechanism() const
{
    return d_mechanism;
}

// CREATORS
inline
allocator<void>::allocator()
: d_mechanism(BloombergLP::bslma_Default::defaultAllocator())
{
}

inline
allocator<void>::allocator(BloombergLP::bslma_Allocator *mechanism)
: d_mechanism(BloombergLP::bslma_Default::allocator(mechanism))
{
}

// 'template <>' is needed only for versions of xlC prior to 9
#if defined(__xlC__) && __xlC__<0x900
template <>
#endif
inline
allocator<void>::allocator(const allocator<void>& rhs)
: d_mechanism(rhs.mechanism())
{
}

template <class U>
inline
allocator<void>::allocator(const allocator<U>& rhs)
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

template <class T>
inline
bool operator==(const allocator<T>&                 lhs,
                const BloombergLP::bslma_Allocator *rhs)
{
    return lhs.mechanism() == rhs;
}

template <class T>
inline
bool operator!=(const allocator<T>&                 lhs,
                const BloombergLP::bslma_Allocator *rhs)
{
    return ! (lhs == rhs);
}

template <class T>
inline
bool operator==(const BloombergLP::bslma_Allocator *lhs,
                const allocator<T>&                 rhs)
{
    return lhs == rhs.mechanism();
}

template <class T>
inline
bool operator!=(const BloombergLP::bslma_Allocator *lhs,
                const allocator<T>&                 rhs)
{
    return ! (lhs == rhs);
}

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
