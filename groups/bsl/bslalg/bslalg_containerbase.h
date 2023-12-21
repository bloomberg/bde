// bslalg_containerbase.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_CONTAINERBASE
#define INCLUDED_BSLALG_CONTAINERBASE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper for STL allocators, for container use.
//
//@CLASSES:
//  bslalg::ContainerBase: base class for allocator-aware STL-style containers
//
//@SEE_ALSO: bslma_bslallocator
//
//@DESCRIPTION: This component provides a single mechanism class,
// 'bslalg::ContainerBase', that can used as a base class by STL-style
// containers for storing the container's allocator.  If instantiated with an
// empty allocator type, 'ContainerBase' will itself be an empty class type.
// Thus, a container class derived from 'ContainerBase' can benefit from the
// base-class optimization in that an empty allocator object will not add to
// the size of the container's footprint.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Fixed-Size Array with 'bslalg::ContainerBase'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to implement a fixed-size array that allocates memory
// from a user-supplied allocator at construction.
//
// First, we define the interface of the container, 'MyFixedSizeArray'.  We
// privately derive from 'ContainerBase' to take advantage of the
// empty-base-class optimization (in case 'ALLOCATOR' is an empty class) and to
// take advantage of implementation conveniences 'ContainerBase' provides:
//..
//  #include <bslalg_containerbase.h>
//  #include <bslma_allocatorutil.h>
//  #include <bslma_autodestructor.h>
//  #include <bslma_constructionutil.h>
//  #include <bslma_deallocateobjectproctor.h>
//  #include <bslma_destructionutil.h>
//  #include <bslma_bslallocator.h>
//  #include <bslma_testallocator.h>
//  #include <bslmf_isempty.h>
//
//  template <class VALUE, class ALLOCATOR>
//  class MyFixedSizeArray : private bslalg::ContainerBase<ALLOCATOR>
//      // This class implements a container that contains a fixed number of
//      // elements of the parameterized type 'VALUE' using the parameterized
//      // 'ALLOCATOR' to allocate memory.  The number of elements is specified
//      // on construction.
//  {
//      // PRIVATE TYPES
//      typedef bslalg::ContainerBase<ALLOCATOR> Base;
//
//      // DATA
//      VALUE     *d_array;  // head pointer to the array of elements
//      const int  d_size;   // (fixed) number of elements in 'd_array'
//
//    public:
//      // TYPES
//      typedef ALLOCATOR allocator_type;
//
//      // CREATORS
//      explicit MyFixedSizeArray(int              size,
//                                const ALLOCATOR& allocator = ALLOCATOR());
//          // Create a 'MyFixedSizeArray' object having the specified 'size'
//          // elements, and using the optionally specified 'allocator' to
//          // supply memory.  Each element of the array is value initialized.
//
//      MyFixedSizeArray(const MyFixedSizeArray& original,
//                       const ALLOCATOR&        allocator = ALLOCATOR());
//          // Create a 'MyFixedSizeArray' object having same number of
//          // elements as that of the specified 'original', the same value of
//          // each element as that of corresponding element in 'original', and
//          // using the optionally specified 'allocator' to supply memory.
//
//      ~MyFixedSizeArray();
//          // Destroy this object.
//
//      // MANIPULATORS
//      VALUE& operator[](int i) { return d_array[i]; }
//          // Return a modifiable reference to the specified 'i'th element of
//          // this object.  The behavior is undefined unless 'i < size()'.
//
//      // ACCESSORS
//      const VALUE& operator[](int i) const { return d_array[i]; }
//          // Return a const reference to the specified 'i'th element of this
//          // object.  The behavior is undefined unless 'i < size()'.
//
//      ALLOCATOR get_allocator() const;
//          // Return the allocator used by this object to allocate memory.
//
//      int size() const { return d_size; }
//          // Return the number of elements contained in this object.
//  };
//..
// Next, we define the 'get_allocator' accessor, which extracts the allocator
// from the 'ContainerBase' base class using its 'allocatorRef' method:
//..
//  template<class VALUE, class ALLOCATOR>
//  inline
//  ALLOCATOR
//  MyFixedSizeArray<VALUE,ALLOCATOR>::get_allocator() const {
//      return Base::allocatorRef();
//  }
//..
// Next, we define the first constructor, beginning with the initialization the
// 'ContainerBase' base class with the supplied 'allocator':
//..
//  template<class VALUE, class ALLOCATOR>
//  MyFixedSizeArray<VALUE,ALLOCATOR>::MyFixedSizeArray(
//                                                  int              size,
//                                                  const ALLOCATOR& allocator)
//  : Base(allocator)
//  , d_size(size)
//  {
//..
// Then, we allocate the specified number of array elements using the allocator
// returned by the 'get_allocator()' method.  Once allocated, we protect the
// array memory with a 'bslma::DeallocateObjectProctor' object:
//..
//      d_array =
//          bslma::AllocatorUtil::allocateObject<VALUE>(get_allocator(),
//                                                      d_size);
//      bslma::DeallocateObjectProctor<ALLOCATOR, VALUE>
//          deallocateProctor(get_allocator(), d_array, d_size);
//..
// Then, we invoke the constructor for each array element using the
// 'bslma::ConstructionUtil::construct' method.  We use a
// 'bslma::AutoDestuctor' proctor to unwind these constructions if an exception
// is thrown:
//..
//      bslma::AutoDestructor<VALUE> autoDtor(d_array, 0);
//      // Default construct each element of the array:
//      for (int i = 0; i < d_size; ++i) {
//          bslma::ConstructionUtil::construct(&d_array[i], get_allocator());
//          ++autoDtor;
//      }
//..
// Then, when every element has been constructed, we free the proctors:
//..
//      autoDtor.release();
//      deallocateProctor.release();
//  }
//..
// Next we implement the destructor as the reverse of the constructor, invoking
// 'bslma::DestructionUtil::destroy' on each element then deallocating them
// with 'bslma::AllocatorUtil::deallocateObject':
//..
//  template<class VALUE, class ALLOCATOR>
//  MyFixedSizeArray<VALUE,ALLOCATOR>::~MyFixedSizeArray()
//  {
//      // Call destructor for each element
//      for (int i = 0; i < d_size; ++i) {
//          bslma::DestructionUtil::destroy(&d_array[i]);
//      }
//
//      // Return memory to allocator.
//      bslma::AllocatorUtil::deallocateObject(get_allocator(),
//                                             d_array, d_size);
//  }
//..
// Next, for testing purposes, we create a 'StatelessAllocator' template that
// simply allocates a global test allocator:
//..
//  bslma::TestAllocator g_testAllocator;
//
//  template <class TYPE>
//  class StatelessAllocator {
//      // Allocator that allocates from the default 'bslma::Allocator'
//      // resource.
//
//    public:
//      typedef TYPE value_type;
//
//      value_type *allocate(std::size_t n, void * = 0) {
//          return bslma::AllocatorUtil::allocateObject<value_type>(
//                                                        &g_testAllocator, n);
//      }
//
//      void deallocate(value_type *p, std::size_t n) {
//          bslma::AllocatorUtil::deallocateObject(&g_testAllocator, p, n);
//      }
//  };
//..
// Finally, we create two 'MyFixedSizeArray' objects, one using
// 'StatelessAllocator', and the other using 'bsl::allocator', and we verify
// that memory is allocated from the correct allocator for each.  Because
// 'StatelessAllocator' is an empty class, the first object is smaller than the
// second object by at least the size of a 'bsl::allocator'.
//..
//  int main()
//  {
//      assert(bsl::is_empty<StatelessAllocator<int> >::value);
//
//      MyFixedSizeArray<int, StatelessAllocator<int> > fixedArray1(3);
//      assert(3               == fixedArray1.size());
//      assert(1               == g_testAllocator.numBlocksInUse());
//      assert(3 * sizeof(int) == g_testAllocator.numBytesInUse());
//
//      bslma::TestAllocator                            ta;
//      MyFixedSizeArray<int, bsl::allocator<int> >     fixedArray2(3, &ta);
//      assert(3               == fixedArray2.size());
//      assert(&ta             == fixedArray2.get_allocator());
//      assert(1               == ta.numBlocksInUse());
//      assert(3 * sizeof(int) == ta.numBytesInUse());
//
//      assert(sizeof(fixedArray2) - sizeof(fixedArray1) >=
//             sizeof(bsl::allocator<int>));
//  }
//..

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_isempty.h>

#include <bsls_keyword.h>

#include <new>  // Placement new

namespace BloombergLP {

namespace bslalg {

                        // ================================
                        // class ContainerBase_NonEmptyBase
                        // ================================

template <class ALLOCATOR>
class ContainerBase_NonEmptyBase {
    // One of two possible base classes for 'ContainerBase'.  This class should
    // be used only for allocators with size > 0 (Inheritance from this type
    // can cause ambiguous conversions and should be avoided or insulated.)

    ALLOCATOR d_allocator;

  private:
    // NOT IMPLEMENTED
    ContainerBase_NonEmptyBase(const ContainerBase_NonEmptyBase& original)
                                                          BSLS_KEYWORD_DELETED;
    ContainerBase_NonEmptyBase& operator=(const ContainerBase_NonEmptyBase&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef ALLOCATOR AllocatorType;

    // CREATORS
    explicit ContainerBase_NonEmptyBase(const ALLOCATOR& basicAllocator);
        // Construct this object to hold a copy of the specified
        // 'basicAllocator' of the (template parameter) type 'ALLOCATOR'.

    ~ContainerBase_NonEmptyBase();
        // Destroy this object.

    // MANIPULATORS
    ALLOCATOR& allocatorRef();
        // Return a reference to this object's allocator, which is typically a
        // copy of the allocator used to construct this object.

    // ACCESSORS
    const ALLOCATOR& allocatorRef() const;
        // Return a non-modifiable reference to this object's allocator, which
        // is typically a copy of the allocator used to construct this object.
};

                        // =============================
                        // class ContainerBase_EmptyBase
                        // =============================

template <class ALLOCATOR>
class ContainerBase_EmptyBase {
    // One of two possible base classes for 'ContainerBase'.  This class is for
    // stateless allocators (i.e., that have size 0), including
    // 'std::allocator' (but not 'bsl::allocator').  Provides access to the
    // allocator.
    //
    // This class does **not** have a subobject of type 'ALLOCATOR'.  Instead,
    // a (zero-sized) 'ALLOCATOR' is implicitly sited at the address of the
    // start of an object of this class.  This approach was chosen in
    // preference to inheriting from 'ALLOCATOR' so as to avoid inheriting the
    // interface for 'ALLOCATOR' and thus making this class (and all classes
    // derived from it) improperly categorized as an allocator when using the
    // 'bslma::IsStdAllocator' metafunction.  'ContainerBase_EmptyBase' is an
    // empty class and will not increase the footprint of a derived class.

  private:
    // NOT IMPLEMENTED
    ContainerBase_EmptyBase(const ContainerBase_EmptyBase& original)
                                                          BSLS_KEYWORD_DELETED;
    ContainerBase_EmptyBase& operator=(const ContainerBase_EmptyBase&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef ALLOCATOR AllocatorType;

    // CREATORS
    explicit ContainerBase_EmptyBase(const ALLOCATOR& basicAllocator);
        // Construct this object to hold a copy of the specified
        // 'basicAllocator' of the parameterized 'ALLOCATOR' type.

    ~ContainerBase_EmptyBase();
        // Destroy this object.

    // MANIPULATORS
    ALLOCATOR& allocatorRef();
        // Return a reference to the modifiable allocator used to construct
        // this object.

    // ACCESSORS
    const ALLOCATOR& allocatorRef() const;
        // Return a reference to the non-modifiable allocator used to construct
        // this object.
};

                        // ====================
                        // class ContainerBase
                        // ====================

template <class ALLOCATOR>
class ContainerBase : public
    bsl::conditional<bsl::is_empty<ALLOCATOR>::value,
                     ContainerBase_EmptyBase<ALLOCATOR>,
                     ContainerBase_NonEmptyBase<ALLOCATOR> >::type {
    // Allocator proxy class for STL-style containers.  Provides access to the
    // allocator.  Implements the entire STL allocator interface, redirecting
    // allocation and deallocation calls to the proxied allocator.  One of two
    // possible base classes is chosen depending on whether 'ALLOCATOR' is
    // an empty class.

    // PRIVATE TYPES
    typedef typename
        bsl::conditional<bsl::is_empty<ALLOCATOR>::value,
                         ContainerBase_EmptyBase<ALLOCATOR>,
                         ContainerBase_NonEmptyBase<ALLOCATOR> >::type Base;

    // NOT IMPLEMENTED
    ContainerBase(const ContainerBase& original) BSLS_KEYWORD_DELETED;
    ContainerBase& operator=(const ContainerBase&) BSLS_KEYWORD_DELETED;

  public:
    // PUBLIC TYPES
    typedef typename Base::AllocatorType AllocatorType;

    // CREATORS
    explicit ContainerBase(const ALLOCATOR& basicAllocator);
        // Construct this object using the specified 'basicAllocator' of the
        // parameterized 'ALLOCATOR' type.

    ~ContainerBase();
        // Destroy this object.

    // ACCESSORS
    bool equalAllocator(const ContainerBase& rhs) const;
        // Returns true if this object and 'rhs' have allocators that compare
        // equal.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // --------------------------------
                  // class ContainerBase_NonEmptyBase
                  // --------------------------------

// CREATORS
template <class ALLOCATOR>
inline
ContainerBase_NonEmptyBase<ALLOCATOR>::
ContainerBase_NonEmptyBase(const ALLOCATOR& basicAllocator)
: d_allocator(basicAllocator)
{
}

template <class ALLOCATOR>
inline
ContainerBase_NonEmptyBase<ALLOCATOR>::~ContainerBase_NonEmptyBase()
{
}

// MANIPULATORS
template <class ALLOCATOR>
inline
ALLOCATOR& ContainerBase_NonEmptyBase<ALLOCATOR>::allocatorRef()
{
    return d_allocator;
}

// ACCESSORS
template <class ALLOCATOR>
inline
const ALLOCATOR& ContainerBase_NonEmptyBase<ALLOCATOR>::allocatorRef() const
{
    return d_allocator;
}

                        // -----------------------------
                        // class ContainerBase_EmptyBase
                        // -----------------------------

// CREATORS
template <class ALLOCATOR>
inline
ContainerBase_EmptyBase<ALLOCATOR>::
ContainerBase_EmptyBase(const ALLOCATOR& basicAllocator)
{
    BSLMF_ASSERT(1 == sizeof(ALLOCATOR));
    BSLMF_ASSERT(1 == sizeof(*this));

    // Construct the allocator at this location.  Because 'ALLOCATOR' is an
    // empty class, the constructor is almost certainly a no-op, but it could
    // have an external side effect such as logging or inserting the allocator
    // address into a global map.
    ::new(this) ALLOCATOR(basicAllocator);
}

template <class ALLOCATOR>
inline
ContainerBase_EmptyBase<ALLOCATOR>::~ContainerBase_EmptyBase()
{
    // Destroy the allocator at this location.  Because 'ALLOCATOR' is an
    // empty class, the destructor is almost certainly a no-op, but it could
    // have an external side effect such as logging or removing the allocator
    // address from a global map.
    allocatorRef().~ALLOCATOR();
}

// MANIPULATORS
template <class ALLOCATOR>
inline
ALLOCATOR& ContainerBase_EmptyBase<ALLOCATOR>::allocatorRef()
{
    return *reinterpret_cast<ALLOCATOR *>(this);
}

// ACCESSORS
template <class ALLOCATOR>
inline
const ALLOCATOR&
ContainerBase_EmptyBase<ALLOCATOR>::allocatorRef() const
{
    return *reinterpret_cast<const ALLOCATOR *>(this);
}

                        // --------------------
                        // class ContainerBase
                        // --------------------

// CREATORS
template <class ALLOCATOR>
inline
ContainerBase<ALLOCATOR>::ContainerBase(const ALLOCATOR& basicAllocator)
: Base(basicAllocator)
{
}

template <class ALLOCATOR>
inline
ContainerBase<ALLOCATOR>::~ContainerBase()
{
}

// ACCESSORS
template <class ALLOCATOR>
inline
bool ContainerBase<ALLOCATOR>::equalAllocator(const ContainerBase& rhs) const
{
    return this->allocatorRef() == rhs.allocatorRef();
}

}  // close package namespace
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
