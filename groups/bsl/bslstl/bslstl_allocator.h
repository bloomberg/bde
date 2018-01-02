// bslstl_allocator.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALLOCATOR
#define INCLUDED_BSLSTL_ALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compatible proxy for 'bslma::Allocator' objects.
//
//@DEPRECATED: Use 'bslma_stdallocator' instead.
//
//@CLASSES:
//  bsl::allocator: STL-compatible allocator template
//  bsl::allocator_traits<bsl::allocator>: specialization for 'bsl::allocator'
//
//@SEE_ALSO: bslma_stdallocator
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_STDALLOCATOR
#include <bslma_stdallocator.h>
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

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
