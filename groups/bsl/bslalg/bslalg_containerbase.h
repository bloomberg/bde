// bslalg_containerbase.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_CONTAINERBASE
#define INCLUDED_BSLALG_CONTAINERBASE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper for STL allocators, respecting 'bslma' semantics.
//
//@CLASSES:
//  bslalg::ContainerBase: proxy class for STL-style containers
//
//@SEE_ALSO: bslstl_allocator
//
//@DESCRIPTION: This component provides a single, mechanism class,
// 'bslalg::ContainerBase', that can used as a common base class for all
// STL-style containers.  A container should derive from this class to take
// advantage of empty-base optimization when a non-'bslma' allocator is used.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Fixed-Size Array with 'bslalg::ContainerBase'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to implement a fixed-size array that allocates memory
// on the heap at construction.
//
// First, we define the interface of the container, 'MyFixedSizeArray', that
// derives from 'ContainerBase'.  The implementation is elided for brevity:
//..
//  template <class VALUE, class ALLOCATOR>
//  class MyFixedSizeArray : private bslalg::ContainerBase<ALLOCATOR>
//      // This class implements a container that contains a fixed number of
//      // elements of the parameterized type 'VALUE' using the parameterized
//      // 'ALLOCATOR' to allocate memory.  The number of elements is specified
//      // on construction.
//  {
//..
// Notice that to use this component, a class should derive from
// 'ContainerBase' in order to take advantage of empty-base optimization.
//..
//      // DATA
//      VALUE     *d_array;  // head pointer to the array of elements
//      const int  d_size;   // (fixed) number of elements in 'd_array'
//
//    public:
//      // CREATORS
//      MyFixedSizeArray(int size, const ALLOCATOR& allocator = ALLOCATOR());
//          // Create a 'MyFixedSizeArray' object having the specified 'size'
//          // elements, and using the specified 'allocator' to supply memory.
//
//      MyFixedSizeArray(const MyFixedSizeArray& original,
//                       const ALLOCATOR&        allocator = ALLOCATOR());
//          // Create a 'MyFixedSizeArray' object having same number of
//          // elements as that of the specified 'original', the same value of
//          // each element as that of corresponding element in 'original', and
//          // using the specified 'allocator' to supply memory.
//
//      ~MyFixedSizeArray();
//          // Destroy this object.
//
//      // MANIPULATORS
//      VALUE& operator[](int i);
//          // Return the reference of the specified 'i'th element of this
//          // object.  The behavior is undefined unless 'i < size()'.
//
//      // ACCESSORS
//      int size() const;
//          // Return the number of elements contained in this object.
//  };
//..
// Finally, assuming we have a STL compliant allocator named 'Allocator', we
// can create a 'MyFixedSizeArray' object and populate it with data.
//..
//  MyFixedSizeArray<int, Allocator<int> > fixedArray(3);
//  fixedArray[0] = 1;
//  fixedArray[1] = 2;
//  fixedArray[2] = 3;
//
//  assert(fixedArray[0] == 1);
//  assert(fixedArray[1] == 2);
//  assert(fixedArray[2] == 3);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bslalg {

                        // =============================
                        // class ContainerBase_BslmaBase
                        // =============================

template <class ALLOCATOR>
class ContainerBase_BslmaBase {
    // One of two possible base classes for 'ContainerBase'.  This class
    // should only be used for allocators that are based on 'bslma::Allocator'.
    // Provides access to the allocator.  Since 'ALLOCATOR' always has state
    // (at least a 'bslma::Allocator *'), there is no empty-base initialization
    // opportunity, so we don't inherit from 'ALLOCATOR' the way
    // 'ContainerBase_NonBslmaBase' does, below.  (Inheritance of this type can
    // cause ambiguous conversions and should be avoided or insulated.)

    ALLOCATOR d_allocator;

  private:
    // NOT IMPLEMENTED
    ContainerBase_BslmaBase& operator=(const ContainerBase_BslmaBase&);

  public:
    // TYPES
    typedef ALLOCATOR AllocatorType;

    typedef bslma::Allocator *bslmaAllocatorPtr;
        // Pointer type returned by 'ALLOCATOR::mechanism'.  This type differs
        // in specializations of this class for non 'bslma'-based allocators.

    // CREATORS
    explicit
    ContainerBase_BslmaBase(const ALLOCATOR& basicAllocator);
        // Construct this object using the specified 'basicAllocator' of the
        // 'ALLOCATOR' parameterized type.

    ContainerBase_BslmaBase(const ContainerBase_BslmaBase& original);
        // Construct this object using the default allocator.  The 'original'
        // argument is ignored.  NOTE: This is obviously not a copy constructor
        // as is does not do any copying.  It does implement BSL-style
        // allocator semantics, whereby a newly created object must either have
        // an explicitly-specified allocator or else it uses the default
        // allocator object.  Under no circumstances is a BSL-style allocator
        // copied during copy construction or assignment.

    ~ContainerBase_BslmaBase();
        // Destroy this object.

    // MANIPULATORS
    ALLOCATOR& allocator();
        // Return a reference to the modifiable allocator used to construct
        // this object.

    // ACCESSORS
    const ALLOCATOR& allocator() const;
        // Return a reference to the non-modifiable allocator used to construct
        // this object.

    bslmaAllocatorPtr bslmaAllocator() const;
        // Return 'allocator().mechanism()'.
};

                        // ================================
                        // class ContainerBase_NonBslmaBase
                        // ================================

template <class ALLOCATOR>
class ContainerBase_NonBslmaBase : public ALLOCATOR {
    // One of two possible base classes for 'ContainerBase'.  This class is
    // for allocators that are not based on 'bslma::Allocator'.  Provides
    // access to the allocator.
    //
    // Because this class inherits from 'ALLOCATOR' it can take advantage of
    // empty-base optimization.  In other words, if 'ALLOCATOR' has no state
    // then it will not contribute to the footprint of the
    // 'ContainerBase_NonBslmaBase' object.  'ContainerBase_NonBslmaBase'
    // itself adds no state and will not increase the footprint of subsequently
    // derived classes.

  private:
    // NOT IMPLEMENTED
    ContainerBase_NonBslmaBase& operator=(const ContainerBase_NonBslmaBase&);

  public:
    // TYPES
    typedef ALLOCATOR AllocatorType;

    typedef void *bslmaAllocatorPtr;
        // Generically, a pointer to the 'bslma::Allocator' mechanism type for
        // 'ALLOCATOR'.  Since the 'ALLOCATOR' type specified in this template
        // does not use the 'bslma::Allocator' mechanism, 'bslmaAllocatorPtr'
        // is simply 'void*'.  This can be used for overloading functions based
        // on whether a container uses a 'bslma'-based allocator or not.  (See
        // the 'bslalg_scalarprimitives' and 'bslalg_arrayprimitives'
        // components.)

    // CREATORS
    ContainerBase_NonBslmaBase(const ALLOCATOR& basicAllocator);
        // Construct this object using the specified 'basicAllocator' or the
        // parameterized 'ALLOCATOR' type.

    ContainerBase_NonBslmaBase(const ContainerBase_NonBslmaBase& rhs);
        // Construct this object by copying the allocator from rhs.  NOTE:
        // Although this constructor does copy the allocator, the copy
        // constructor in the 'bslma'-specific 'ContainerBase_BslmaBase' class
        // (above) does not.

    ~ContainerBase_NonBslmaBase();
        // Destroy this object.

    // MANIPULATORS
    ALLOCATOR& allocator();
        // Return a reference to the modifiable allocator used to construct
        // this object.

    // ACCESSORS
    const ALLOCATOR& allocator() const;
        // Return a reference to the non-modifiable allocator used to construct
        // this object.

    bslmaAllocatorPtr bslmaAllocator() const;
        // Return a null pointer.  (This container's allocator does not use
        // 'bslma').  Note that the return type for this function differs from
        // the (typedef'ed) return type for the same function in the
        // 'ContainerBase_BslmaBase' class and can thus be used to choose one
        // overloaded function over another.
};

                        // ====================
                        // class ContainerBase
                        // ====================

template <class ALLOCATOR>
class ContainerBase : public
    bsl::conditional<bsl::is_convertible<bslma::Allocator*, ALLOCATOR>::value,
                     ContainerBase_BslmaBase<ALLOCATOR>,
                     ContainerBase_NonBslmaBase<ALLOCATOR> >::type {
    // Allocator proxy class for STL-style containers.  Provides access to the
    // allocator.  Implements the entire STL allocator interface, redirecting
    // allocation and deallocation calls to the proxied allocator.  One of two
    // possible base classes is chosen depending on whether 'ALLOCATOR' is
    // constructed from 'bslma::Allocator*'.

    // PRIVATE TYPES
    typedef typename
        bsl::conditional<
                  bsl::is_convertible<bslma::Allocator*, ALLOCATOR>::value,
                  ContainerBase_BslmaBase<ALLOCATOR>,
                  ContainerBase_NonBslmaBase<ALLOCATOR> >::type Base;

  private:
    // NOT IMPLEMENTED
    ContainerBase& operator=(const ContainerBase&);

  private:
    // PRIVATE MANIPULATORS
    template <class T>
    typename ALLOCATOR::template rebind<T>::other
    rebindAllocator(T*)
        // Return 'this->allocator()' rebound for type 'T'.  The 'T*' argument
        // is used only for template parameter deduction and is ignored.
    {
        typedef typename ALLOCATOR::template rebind<T>::other Rebound;
        return Rebound(this->allocator());
    }

  public:
    // PUBLIC TYPES
    typedef typename Base::AllocatorType            AllocatorType;

    typedef typename AllocatorType::size_type       size_type;
    typedef typename AllocatorType::difference_type difference_type;
    typedef typename AllocatorType::pointer         pointer;
    typedef typename AllocatorType::const_pointer   const_pointer;
    typedef typename AllocatorType::reference       reference;
    typedef typename AllocatorType::const_reference const_reference;
    typedef typename AllocatorType::value_type      value_type;
        // Restate required allocator types.  (Reduces use of 'typename' in
        // interface.)

    // CREATORS
    ContainerBase(const ALLOCATOR& basicAllocator);
        // Construct this object using the specified 'basicAllocator' of the
        // parameterized 'ALLOCATOR' type.

    ContainerBase(const ContainerBase& rhs);
        // Initialize this container base with rhs.  NOTE: This is not a true
        // copy constructor.  The allocator does not get copied if the
        // allocator is 'bslma'-based.  Using BSL allocator semantics, the
        // 'bslma'-style allocator must be supplied explicitly (i.e., not
        // copied from rhs) or else it is given a default value.  Non-'bslma'
        // allocators ARE copied because that is the way the ISO standard is
        // currently written.

    ~ContainerBase();
        // Destroy this object.

    // MANIPULATORS
    pointer allocate(size_type n, const void *hint = 0);
        // Allocate enough (properly aligned) space for 'n' objects of type 'T'
        // by calling 'allocate' on the mechanism object.  The 'hint' argument
        // is ignored by this allocator type.

    template <class T>
    T *allocateN(T* p, size_type n)
        // Allocate (but do not initialize) 'n' objects of type 'T' using the
        // allocator returned by 'allocator'.  Return a pointer to the raw
        // memory that was allocated.  The 'p' argument is used only to
        // determine the type of object being allocated; its value (usually
        // null) is not used.
    {
        return rebindAllocator(p).allocate(n);
    }

    void construct(pointer p, const value_type& val);
        // Copy-construct a 'T' object at the memory address specified by 'p'.
        // Do not directly allocate memory.  The behavior is undefined if 'p'
        // is not properly aligned for 'T'.

    void deallocate(pointer p, size_type n = 1);
        // Return memory previously allocated with 'allocate' to the underlying
        // mechanism object by calling 'deallocate' on the mechanism object.
        // The 'n' argument is ignored by this allocator type.

    template <class T>
    void deallocateN(T *p, size_type n)
        // Return 'n' objects of type 'T', starting at 'p' to the allocator
        // returned by 'allocator'.  Does not call destructors on the
        // deallocated objects.
    {
        rebindAllocator(p).deallocate(p, n);
    }

    void destroy(pointer p);
        // Call the 'T' destructor for the object pointed to by 'p'.  Do not
        // directly deallocate any memory.

    // ACCESSORS
    bool equalAllocator(const ContainerBase& rhs) const;
        // Returns 'this->allocator() == rhs.allocator()'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // -----------------------------
                  // class ContainerBase_BslmaBase
                  // -----------------------------

// CREATORS
template <class ALLOCATOR>
inline
ContainerBase_BslmaBase<ALLOCATOR>::
ContainerBase_BslmaBase(const ALLOCATOR& basicAllocator)
: d_allocator(basicAllocator)
{
}

template <class ALLOCATOR>
inline
ContainerBase_BslmaBase<ALLOCATOR>::
ContainerBase_BslmaBase(const ContainerBase_BslmaBase&)
: d_allocator()
{
}

template <class ALLOCATOR>
inline
ContainerBase_BslmaBase<ALLOCATOR>::~ContainerBase_BslmaBase()
{
}

// MANIPULATORS
template <class ALLOCATOR>
inline
ALLOCATOR& ContainerBase_BslmaBase<ALLOCATOR>::allocator()
{
    return d_allocator;
}

// ACCESSORS
template <class ALLOCATOR>
inline
const ALLOCATOR& ContainerBase_BslmaBase<ALLOCATOR>::allocator() const
{
    return d_allocator;
}

template <class ALLOCATOR>
inline
typename ContainerBase_BslmaBase<ALLOCATOR>::bslmaAllocatorPtr
ContainerBase_BslmaBase<ALLOCATOR>::bslmaAllocator() const
{
    return d_allocator.mechanism();
}

                        // --------------------------------
                        // class ContainerBase_NonBslmaBase
                        // --------------------------------

// CREATORS
template <class ALLOCATOR>
inline
ContainerBase_NonBslmaBase<ALLOCATOR>::
ContainerBase_NonBslmaBase(const ALLOCATOR& basicAllocator)
: ALLOCATOR(basicAllocator)
{
}

template <class ALLOCATOR>
inline
ContainerBase_NonBslmaBase<ALLOCATOR>::
ContainerBase_NonBslmaBase(const ContainerBase_NonBslmaBase& rhs)
: ALLOCATOR(rhs.allocator())
{
}

template <class ALLOCATOR>
inline
ContainerBase_NonBslmaBase<ALLOCATOR>::~ContainerBase_NonBslmaBase()
{
}

// MANIPULATORS
template <class ALLOCATOR>
inline
ALLOCATOR& ContainerBase_NonBslmaBase<ALLOCATOR>::allocator()
{
    return *this;
}

// ACCESSORS
template <class ALLOCATOR>
inline
const ALLOCATOR&
ContainerBase_NonBslmaBase<ALLOCATOR>::allocator() const
{
    return *this;
}

template <class ALLOCATOR>
inline
typename ContainerBase_NonBslmaBase<ALLOCATOR>::bslmaAllocatorPtr
ContainerBase_NonBslmaBase<ALLOCATOR>::bslmaAllocator() const
{
    return 0;
}

                        // --------------------
                        // class ContainerBase
                        // --------------------

// CREATORS
template <class ALLOCATOR>
inline
ContainerBase<ALLOCATOR>::
ContainerBase(const ALLOCATOR& basicAllocator)
: Base(basicAllocator)
{
}

template <class ALLOCATOR>
inline
ContainerBase<ALLOCATOR>::
ContainerBase(const ContainerBase<ALLOCATOR>& rhs)
: Base(rhs)
{
}

template <class ALLOCATOR>
inline
ContainerBase<ALLOCATOR>::~ContainerBase()
{
}

// MANIPULATORS
template <class ALLOCATOR>
inline
typename ContainerBase<ALLOCATOR>::pointer
ContainerBase<ALLOCATOR>::allocate(size_type n, const void *hint)
{
    return this->allocator().allocate(n, hint);
}

template <class ALLOCATOR>
inline
void ContainerBase<ALLOCATOR>::deallocate(pointer p, size_type n)
{
    this->allocator().deallocate(p, n);
}

template <class ALLOCATOR>
inline
void ContainerBase<ALLOCATOR>::construct(pointer           p,
                                         const value_type& val)
{
    this->allocator().construct(p, val);
}

template <class ALLOCATOR>
inline
void ContainerBase<ALLOCATOR>::destroy(pointer p)
{
    this->allocator().destroy(p);
}

// ACCESSORS
template <class ALLOCATOR>
inline
bool ContainerBase<ALLOCATOR>::equalAllocator(const ContainerBase& rhs) const
{
    return this->allocator() == rhs.allocator();
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
