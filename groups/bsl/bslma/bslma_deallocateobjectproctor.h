// bslma_deallocateobjectproctor.h                                    -*-C++-*-
#ifndef INCLUDED_BSLMA_DEALLOCATEOBJECTPROCTOR
#define INCLUDED_BSLMA_DEALLOCATEOBJECTPROCTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor to conditionally unwind object allocation.
//
//@CLASSES:
//  bslma::DeallocateObjectProctor: proctor to conditionally unwind allocation.
//
//@SEE_ALSO: bslma_deallocatebytesproctor, bslma_deleteobjectproctor
//
//@DESCRIPTION: This component provides a proctor class template,
// 'bslma::DeallocateObjectProctor', that conditionally reverses the effect of
// 'bslma::AllocatorUtil::allocateObject<TYPE>' or '(TYPE *)
// bslmAllocator->allocate(n)'.  Upon destruction, this proctor deallocates the
// object from the specified allocator or pool without calling the object's
// destructor.  The proctor's constructor takes the same arguments as
// 'bslma::AllocatorUtil::deallocateObject', making it straightforward to
// allocate an object using 'allocateObject' and protect it using
// 'DeallocateObjectProctor'.
//
// As with all proctors, this class is used to automatically reclaim a resource
// in the event that a function ends prematurely, e.g., via an exception.
// After allocating an object, an exception-safe function would create a
// 'DeallocateObjectProctor' to manage the new object's memory.  If the
// operation completes successfully, the code calls the proctor's 'release'
// method, which disengages the proctor.  If, however, the function exits while
// the proctor is still engaged, the proctor's destructor will deallocate the
// managed memory.
//
// The 'DeallocateObjectProctor' template has two template parameters: the
// 'ALLOCATOR' type used to reclaim storage and the object 'TYPE' managed by
// the proctor.  If 'ALLOCATOR' is a non-pointer type and 'TYPE' is omitted, it
// is deduced as 'ALLOCATOR::value_type'.  However, if 'TYPE' is supplied, it
// overrides 'ALLOCATOR::value_type'.
//
// If 'ALLOCATOR' is a non-pointer type, the proctor uses
// 'bslma::AllocatorUtil::deleteObject(a, ptr, n)' to reclaim storage, where
// 'a' is the allocator, 'ptr' is the address of the managed object, and 'n' is
// the number of managed objects.  Otherwise, if 'ALLOCATOR' is a pointer type,
// the proctor reclaims memory by calling 'a->deallocate(ptr)'.  Instantiating
// 'DeallocateObjectProctor' with a pointer-type 'ALLOCATOR' is appropriate for
// classes derived from 'bslma::Allocator' and for BDE-style pool types, i.e.,
// classes for which 'a->deallocate(ptr)' is well formed.
//
///Usage
///-----
// These examples illustrate the intended use of this component.
//
//Example 1: Class having an owning pointer
//- - - - - - - - - - - - - - - - - - - - -
// In this example, we create a class, 'my_Manager', having an owning pointer
// to an object of another class, 'my_Data'.  Because it owns the 'my_Data'
// object, 'my_Manager' is responsible for allocating, constructing,
// deallocating, and destroying it.
//
// First, we define the 'my_Data' class, which holds an integer value and
// counts how many times its constructor and destructor have been called. Its
// constructor will throw an exception if its integer argument equals the
// number of constructor calls before construction:
//..
//  #include <bslma_allocatorutil.h>
//  #include <bslma_bslallocator.h>
//  #include <bslma_testallocator.h>
//
//  class my_Data {
//
//      // DATA
//      int d_value;
//
//      // CLASS DATA
//      static int s_numConstructed;
//      static int s_numDestroyed;
//
//    public:
//      // CLASS METHODS
//      static int numConstructed() { return s_numConstructed; }
//      static int numDestroyed()   { return s_numDestroyed;   }
//
//      // CREATORS
//      explicit my_Data(int v) : d_value(v)
//      {
//          if (v == s_numConstructed) throw s_numConstructed;
//          ++s_numConstructed;
//      }
//      my_Data(const my_Data& original);
//      ~my_Data() { ++s_numDestroyed; }
//  };
//
//  int my_Data::s_numConstructed = 0;
//  int my_Data::s_numDestroyed   = 0;
//..
// Next, we define 'my_Manager' as an allocator-aware class holding a pointer
// to 'my_Data' and maintaining its own count of constructor invocations:
//..
//  class my_Manager {
//
//      // DATA
//      bsl::allocator<my_Data>  d_allocator;
//      my_Data                 *d_data_p;
//
//      // CLASS DATA
//      static int s_numConstructed;
//
//    public:
//      // TYPES
//      typedef bsl::allocator<> allocator_type;
//
//      // CLASS METHODS
//      static int numConstructed() { return s_numConstructed; }
//
//      // CREATORS
//      explicit my_Manager(int                   v,
//                        const allocator_type& allocator = allocator_type());
//      my_Manager(const my_Manager& original);
//      ~my_Manager();
//
//      // ...
//  };
//
//  int my_Manager::s_numConstructed = 0;
//..
// Next, we define the constructor for 'my_Manager', which begins by allocating
// a 'my_Data' object:
//..
//  my_Manager::my_Manager(int v, const allocator_type& allocator)
//      : d_allocator(allocator), d_data_p(0)
//  {
//      d_data_p = bslma::AllocatorUtil::allocateObject<my_Data>(allocator);
//..
// Then, the 'my_Manager' constructor constructs the 'my_Data' object in the
// allocated memory.  However, as the constructor might throw it first protects
// the data object with a 'bslma::DeallocateObjectProctor':
//..
//      bslma::DeallocateObjectProctor<allocator_type, my_Data>
//                                              proctor(d_allocator, d_data_p);
//      bslma::ConstructionUtil::construct(d_data_p, d_allocator, v);
//..
// Then, once the 'construct' operation completes successfully, we can release
// the data object from the proctor.  Only then do we increment the
// construction count:
//..
//      proctor.release();
//      ++s_numConstructed;
//  }
//..
// Next, we define the 'my_Manager' destructor, which destroys and deallocates
// its data object:
//..
//  my_Manager::~my_Manager()
//  {
//      d_data_p->~my_Data();
//      bslma::AllocatorUtil::deallocateObject(d_allocator, d_data_p);
//  }
//..
// Now, we use a 'bslma::TestAllocator' to verify that, under normal (non
// exceptional) circumstances, constructing a 'my_Manager' object will result
// in one block of memory being allocated and one invocation of the 'my_Data'
// constructor:
//..
//  int main()
//  {
//      bslma::TestAllocator ta;
//
//      {
//          my_Manager obj1(7, &ta);
//          assert(1 == ta.numBlocksInUse());
//          assert(1 == ta.numBlocksTotal());
//          assert(1 == my_Manager::numConstructed());
//      }
//      assert(0 == ta.numBlocksInUse());
//      assert(1 == ta.numBlocksTotal());
//      assert(1 == my_Manager::numConstructed());
//..
// Finally, when the 'my_Data' constructor does throw, a block is allocated but
// we verify that the 'my_Manager' constructor did not complete and that the
// block was deallocated, resulting in no leaks:
//..
//      try {
//          my_Manager obj2(1, &ta);
//          assert(false && "Can't get here");
//      }
//      catch (int e) {
//          assert(1 == e);
//          assert(0 == ta.numBlocksInUse());
//          assert(2 == ta.numBlocksTotal());
//          assert(1 == my_Manager::numConstructed());
//      }
//      assert(1 == my_Manager::numConstructed());
//  }
//..

#include <bslscm_version.h>

#include <bslma_allocatortraits.h>
#include <bslma_allocatorutil.h>

#include <bslmf_integralconstant.h>
#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_keyword.h>

#include <cstdlib>  // std::size_t

namespace BloombergLP {
namespace bslma {

// FORWARD DECLARATIONS
template <class ALLOCATOR, class TYPE>
struct DeallocateObjectProctor_PtrType;

                    // ======================================
                    // class template DeallocateObjectProctor
                    // ======================================

template <class ALLOCATOR, class TYPE = typename ALLOCATOR::value_type>
class DeallocateObjectProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically deallocates a managed object upon
    // destruction by invoking the 'deallocate' method of an allocator (or
    // pool) of parameterized 'ALLOCATOR' type supplied to it at construction.
    // The managed object of parameterized 'TYPE' must be memory that was
    // provided by this allocator (or pool); the allocator (or pool) must
    // remain valid throughout the lifetime of the proctor object.  If
    // 'ALLOCATOR' is a non-pointer type, it is assumed to be STL compatible;
    // otherwise, it is assumed have a 'deallocate' method with a single 'void
    // *' parameter, i.e., the same deallocation interface as
    // 'bslma::Allocator'.

  private:
    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil                                MoveUtil;
    typedef typename DeallocateObjectProctor_PtrType<ALLOCATOR,
                                                     TYPE>::type PtrType;

    // DATA
    ALLOCATOR   d_allocator;   // allocator object (might be a pointer)
    PtrType     d_object_p;    // managed object (null if disengaged)
    std::size_t d_numObjects;  // number of managed objects

    // PRIVATE MANIPULATORS
    void doDeallocate(bsl::false_type);
    void doDeallocate(bsl::true_type);
       // Deallocate the object at 'd_object_p'.  The first overload is
       // selected for non-pointer-type 'ALLOCATOR' and invokes
       // 'AllocatorUtil::deallocateObject'.  The second overload is selected
       // for a pointer-type 'ALLOCATOR' and invokes
       // 'd_allocator->deallocate(d_object_p)'.

    // NOT IMPLEMENTED
    DeallocateObjectProctor(const DeallocateObjectProctor&)
                                                          BSLS_KEYWORD_DELETED;
    DeallocateObjectProctor& operator=(const DeallocateObjectProctor&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    DeallocateObjectProctor(const ALLOCATOR& allocator,
                            PtrType          p,
                            std::size_t      n = 1);
        // Create a proctor to manage the optionally specified 'n' objects at
        // the specified 'p' address, which have been allocated from the
        // specified 'allocator'.  If 'p' is null, then the created proctor is
        // disengaged (manages no objects).  The behavior is undefined if 'p'
        // is non-null but not allocated from 'allocator' or if 'p' does not
        // point to an array of 'n' objects.

    DeallocateObjectProctor(
        bslmf::MovableRef<DeallocateObjectProctor> original);
        // This move constructor creates a proctor managing the same object as
        // 'original'.  After the move, 'original' is disengaged.

    ~DeallocateObjectProctor();
        // Deallocate the managed objects, if any.  Note that the managed
        // objects' destructors are not invoked.

    // MANIPULATORS
    PtrType release();
        // Disengage this allocator and return a pointer to the formerly
        // managed object(s).

    void reset(PtrType p, std::size_t n = 1);
        // Release the managed object and reset this proctor to manage the
        // specified 'p' pointer to optionally specified 'n' objects.  If 'p'
        // is null, then the proctor will be disenaged.  Note that the
        // previously managed objects are not deallocated.

    // ACCESSORS
    PtrType ptr() const;
         // Return the address of the currently managed objects, if engaged;
         // otherwise return a null pointer.
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                // -----------------------------------------------
                // struct template DeallocateObjectProctor_PtrType
                // -----------------------------------------------

template <class ALLOCATOR, class TYPE>
struct DeallocateObjectProctor_PtrType
{
    // This metafunction has a nested 'type' that indicates the correct pointer
    // type to use to manage an object of the specified 'TYPE' template
    // parameter allocated from an allocator having the specified 'ALLOCATOR'
    // template parameter type.  This primary template yields the pointer type
    // specified by 'ALLOCATOR', after rebinding to the specified 'TYPE'.

    typedef typename bsl::allocator_traits<ALLOCATOR>::
                                    template rebind_traits<TYPE>::pointer type;
};

template <class ALLOCATOR, class TYPE>
struct DeallocateObjectProctor_PtrType<ALLOCATOR *, TYPE>
{
    // This partial specialization is chosen when 'ALLOCATOR' is a pointer type
    // (i.e., 'bslma::Allocator *' or a pointer to a pool class).  It yields
    // 'TYPE *' for 'type'.

    typedef TYPE *type;
};

                    // --------------------------------------
                    // class template DeallocateObjectProctor
                    // --------------------------------------

// PRIVATE MANIPULATORS
template <class ALLOCATOR, class TYPE>
inline
void DeallocateObjectProctor<ALLOCATOR, TYPE>::doDeallocate(bsl::false_type)
{
    AllocatorUtil::deallocateObject(d_allocator, d_object_p, d_numObjects);
}

template <class ALLOCATOR, class TYPE>
inline
void DeallocateObjectProctor<ALLOCATOR, TYPE>::doDeallocate(bsl::true_type)
{
    // When 'ALLOCATOR' is a pointer type, assume that 'deallocate' can be
    // called with a single pointer argument.  We cannot use
    // 'AllocatorUtil::deallocateObject' because 'AllocatorUtil' does not work
    // with pool types, which are neither STL allocators nor derived from
    // 'bslma::Allocator'.
    d_allocator->deallocate(d_object_p);
}

// CREATORS
template <class ALLOCATOR, class TYPE>
inline
DeallocateObjectProctor<ALLOCATOR, TYPE>::DeallocateObjectProctor(
                                                    const ALLOCATOR& allocator,
                                                    PtrType          p,
                                                    std::size_t      n)
    : d_allocator(allocator), d_object_p(p), d_numObjects(n)
{
}

template <class ALLOCATOR, class TYPE>
inline
DeallocateObjectProctor<ALLOCATOR, TYPE>::DeallocateObjectProctor(
                           bslmf::MovableRef<DeallocateObjectProctor> original)
    : d_allocator(MoveUtil::access(original).d_allocator)
    , d_object_p(MoveUtil::access(original).d_object_p)
    , d_numObjects(MoveUtil::access(original).d_numObjects)
{
    MoveUtil::access(original).release();
}

template <class ALLOCATOR, class TYPE>
inline
DeallocateObjectProctor<ALLOCATOR, TYPE>::~DeallocateObjectProctor()
{
    if (d_object_p) {
        // Dispatch to the appropriate version of 'doDelete', depending on
        // whether or not 'ALLOCATOR' is a pointer type.  We cannot simply call
        // 'AllocatorUtil::dealocateObject' unconditionally because
        // 'AllocatorUtil' does not work with pool types, which are neither STL
        // allocators nor derived from 'bslma::Allocator'.
        doDeallocate(bsl::is_pointer<ALLOCATOR>());
    }
}

// MANIPULATORS
template <class ALLOCATOR, class TYPE>
inline
typename DeallocateObjectProctor<ALLOCATOR, TYPE>::PtrType
DeallocateObjectProctor<ALLOCATOR, TYPE>::release()
{
    PtrType ret = d_object_p;
    d_object_p = PtrType();
    return ret;
}

template <class ALLOCATOR, class TYPE>
inline
void DeallocateObjectProctor<ALLOCATOR, TYPE>::reset(PtrType p, size_t n)
{
    d_object_p = p;
    d_numObjects = n;
}

// ACCESSORS
template <class ALLOCATOR, class TYPE>
inline
typename DeallocateObjectProctor<ALLOCATOR, TYPE>::PtrType
DeallocateObjectProctor<ALLOCATOR, TYPE>::ptr() const
{
    return d_object_p;
}

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_DEALLOCATEOBJECTPROCTOR)

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
