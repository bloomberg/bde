// bslma_deleteobjectproctor.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMA_DELETEOBJECTPROCTOR
#define INCLUDED_BSLMA_DELETEOBJECTPROCTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor to conditionally unwind new object creation.
//
//@CLASSES:
//  bslma::DeleteObjectProctor: proctor to conditionally unwind object creation
//
//@SEE_ALSO: bslma_deallocatebytesproctor, bslma_dealocatebjectproctor
//
//@DESCRIPTION: This component provides a proctor class template,
// `bslma::DeleteObjectProctor`, that conditionally reverses the effect of
// `bslma::AllocatorUtil::newObject<TYPE>` or `new (bslmAllocator) TYPE`.  Upon
// destruction, this proctor invokes the `TYPE` destructor and deallocates the
// object from the specified allocator or pool.  The proctor's constructor
// takes the same arguments as `bslma::AllocatorUtil::deleteObject`,
// making it straightforward to allocate an object using `newObject` and
// protect it using `DeleteObjectProctor`.
//
// As with all proctors, this class is used to automatically reclaim a resource
// in the event that a function ends prematurely, e.g., via an exception.
// After allocating and constructing an object, an exception-safe function
// would create a `DeleteObjectProctor` to manage the new object's lifetime.
// If the operation completes successfully, the code calls the proctor's
// `release` method, which disengages the proctor.  If, however, the function
// or constructor exits while the proctor is still engaged, the proctor's
// destructor will destroy and deallocate the managed object.
//
// The `DeleteObjectProctor` template has two template parameters: the
// `ALLOCATOR` type used to reclaim storage and the object `TYPE` managed by
// the proctor.  If `ALLOCATOR` is a non-pointer type and `TYPE` is omitted, it
// is deduced as `ALLOCATOR::value_type`.  However, if `TYPE` is supplied, it
// overrides `ALLOCATOR::value_type`.
//
// If `ALLOCATOR` is a non-pointer type, the proctor uses
// `bslma::AllocatorUtil::deleteObject(a, ptr)` to destroy the managed object
// and reclaim its storage, where `a` is the allocator, and `ptr` is the
// address of the managed object.  Otherwise, if `ALLOCATOR` is a pointer
// type, the proctor invokes the `TYPE` destructor followed by
// `a->deallocate(ptr)`.  Instantiating `DeleteObjectProctor` with a
// pointer-type `ALLOCATOR` is appropriate for classes derived from
// `bslma::Allocator` and for BDE-style pool types, i.e., classes for which
// `a->deallocate(ptr)` is well formed.
//
///Usage
///-----
// These examples illustrate the intended use of this component.
//
//Example 1: Class having an owning pointer
//- - - - - - - - - - - - - - - - - - - - -
// In this example, we create a class, `my_Manager`, having an owning pointer
// to an object of another class, `my_Data`.  Because it owns the `my_Data`
// object, `my_Manager` is responsible for allocating, constructing,
// deallocating, and destroying it.
//
// First, we define the `my_Data` class, which holds an integer value and
// counts how many times its constructor and destructor have been called. It
// also has a manipulator, `mightThrow`, that throws an exception if the
// integer value equals the number of constructor calls:
// ```
// #include <bslma_allocatorutil.h>
// #include <bslma_bslallocator.h>
// #include <bslma_testallocator.h>
//
// class my_Data {
//
//     // DATA
//     int d_value;
//
//     // CLASS DATA
//     static int s_numConstructed;
//     static int s_numDestroyed;
//
//   public:
//     // CLASS METHODS
//     static int numConstructed() { return s_numConstructed; }
//     static int numDestroyed()   { return s_numDestroyed;   }
//
//     // CREATORS
//     explicit my_Data(int v) : d_value(v) { ++s_numConstructed; }
//     my_Data(const my_Data& original);
//     ~my_Data() { ++s_numDestroyed; }
//
//     // MANIPULATORS
//     void mightThrow()
//         { if (s_numConstructed == d_value) { throw --d_value; } }
// };
//
// int my_Data::s_numConstructed = 0;
// int my_Data::s_numDestroyed   = 0;
// ```
// Next, we define `my_Manager` as an allocator-aware class holding a pointer
// to `my_Data` and maintaining its own count of constructor invocations:
// ```
// class my_Manager {
//
//     // DATA
//     bsl::allocator<my_Data>  d_allocator;
//     my_Data                 *d_data_p;
//
//     // CLASS DATA
//     static int s_numConstructed;
//
//   public:
//     // TYPES
//     typedef bsl::allocator<> allocator_type;
//
//     // CLASS METHODS
//     static int numConstructed() { return s_numConstructed; }
//
//     // CREATORS
//     explicit my_Manager(int                   v,
//                       const allocator_type& allocator = allocator_type());
//     my_Manager(const my_Manager& original);
//     ~my_Manager();
//
//     // ...
// };
//
// int my_Manager::s_numConstructed = 0;
// ```
// Next, we define the constructor for `my_Manager`, which begins by allocating
// and constructing a `my_Data` object:
// ```
// my_Manager::my_Manager(int v, const allocator_type& allocator)
//     : d_allocator(allocator), d_data_p(0)
// {
//     d_data_p = bslma::AllocatorUtil::newObject<my_Data>(allocator, v);
// ```
// Then, the `my_Manager` constructor invokes the `mightThrow` manipulator on
// the new data object, but first, it protects the object with a
// `bslma::DeleteObjectProctor`:
// ```
//     bslma::DeleteObjectProctor<bsl::allocator<my_Data> >
//                                             proctor(d_allocator, d_data_p);
//     d_data_p->mightThrow();
// ```
// Then, once the `mightThrow` operation completes successfully, we can release
// the data object from the proctor.  Only then do we increment the
// construction count:
// ```
//     proctor.release();
//     ++s_numConstructed;
// }
// ```
// Next, we define the `my_Manager` destructor, which destroys and deallocates
// its data object:
// ```
// my_Manager::~my_Manager()
// {
//     bslma::AllocatorUtil::deleteObject(d_allocator, d_data_p);
// }
// ```
// Now, we use a `bslma::TestAllocator` to verify that, under normal (non
// exceptional) circumstances, constructing a `my_Manager` object will result
// in one block of memory being allocated and one invocation of the `my_Data`
// constructor:
// ```
// int main()
// {
//     bslma::TestAllocator ta;
//
//     {
//         my_Manager obj1(7, &ta);
//         assert(1 == ta.numBlocksInUse());
//         assert(1 == ta.numBlocksTotal());
//         assert(1 == my_Data::numConstructed());
//         assert(0 == my_Data::numDestroyed());
//         assert(1 == my_Manager::numConstructed());
//     }
//     assert(0 == ta.numBlocksInUse());
//     assert(1 == ta.numBlocksTotal());
//     assert(1 == my_Data::numConstructed());
//     assert(1 == my_Data::numDestroyed());
//     assert(1 == my_Manager::numConstructed());
// ```
// Finally, when `mightThrow` does throw, a block is allocated and a `my_Data`
// constructor is invoked, but we verify that the `my_Manager` constructor did
// not complete, the `my_Data` destructor was called and the block was
// deallocated, resulting in no leaks:
// ```
//     try {
//         my_Manager obj2(2, &ta);
//         assert(false && "Can't get here");
//     }
//     catch (int e) {
//         assert(1 == e);
//         assert(0 == ta.numBlocksInUse());
//         assert(2 == ta.numBlocksTotal());
//         assert(2 == my_Data::numConstructed());
//         assert(2 == my_Data::numDestroyed());   //
//         assert(1 == my_Manager::numConstructed());
//     }
//     assert(1 == my_Manager::numConstructed());
// }
// ```

#include <bslscm_version.h>

#include <bslma_allocatortraits.h>
#include <bslma_allocatorutil.h>

#include <bslmf_integralconstant.h>
#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>

#include <cstdlib>  // std::size_t

namespace BloombergLP {
namespace bslma {

// FORWARD DECLARATIONS
template <class ALLOCATOR, class TYPE>
struct DeleteObjectProctor_PtrType;

                    // ==================================
                    // class template DeleteObjectProctor
                    // ==================================

/// This class implements a proctor that, unless its `release` method has
/// previously been invoked, automatically deletes a managed object upon
/// destruction by first invoking the object's destructor, and then invoking
/// the `deallocate` method of an allocator (or pool) of parameterized
/// `ALLOCATOR` type supplied to it at construction.  The managed object of
/// parameterized `TYPE` must have been created using memory provided by
/// this allocator (or pool), which must remain valid throughout the
/// lifetime of the proctor object.  If `ALLOCATOR` is a non-pointer type,
/// it is assumed to be STL compatible; otherwise, it is assumed have a
/// `deallocate` method with a single `void *` parameter, i.e., the same
/// deallocation interface as `bslma::Allocator`.
template <class ALLOCATOR, class TYPE = typename ALLOCATOR::value_type>
class DeleteObjectProctor {

  private:
    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil                                       MoveUtil;
    typedef typename DeleteObjectProctor_PtrType<ALLOCATOR, TYPE>::type PtrType;

    // DATA
    ALLOCATOR   d_allocator;  // allocator object (might be a pointer)
    PtrType     d_object_p;   // managed object (null if disengaged)

    // PRIVATE MANIPULATORS
    void doDelete(bsl::false_type);
    void doDelete(bsl::true_type);
       // Destroy and deallocate 'd_object_p'.  The first overload is selected
       // for non-pointer-type 'ALLOCATOR' and invokes
       // 'AllocatorUtil::deleteObject'.  The second overload is selected for
       // pointer-type 'ALLOCATOR' and first invokes ~TYPE(), then invokes
       // 'd_allocator->deallocate(d_object_p)'.

    // NOT IMPLEMENTED
    DeleteObjectProctor(const DeleteObjectProctor&) BSLS_KEYWORD_DELETED;
    DeleteObjectProctor& operator=(const DeleteObjectProctor&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // CREATORS

    /// Create a proctor to manage the specified `p` object, which has been
    /// allocated from the specified `allocator`.  If `p` is null, then the
    /// created proctor is disengaged (manages no object).  The behavior is
    /// undefined if `p` is non-null but not allocated from `allocator`.
    DeleteObjectProctor(const ALLOCATOR& allocator, PtrType p);

    /// This move constructor creates a proctor managing the same object as
    /// `original`.  After the move, `original` is disengaged.
    DeleteObjectProctor(bslmf::MovableRef<DeleteObjectProctor> original);

    /// Destroy and deallocate the managed object, if any.
    ~DeleteObjectProctor();

    // MANIPULATORS

    /// Disengage this allocator and return a pointer to the formerly
    /// managed object.
    PtrType release();

    /// Release the managed object and reset this proctor to manage the
    /// specified `p` object.  If `p` is null, the proctor will be
    /// disengaged.  Note that the previously managed object is not
    /// destroyed or deallocated.
    void reset(PtrType p);

    // ACCESSORS
    PtrType ptr() const;
         // Return the address of the currently managed object, if engaged;
         // otherwise return null.
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                // -------------------------------------------
                // struct template DeleteObjectProctor_PtrType
                // -------------------------------------------

/// This metafunction has a nested `type` that indicates the correct pointer
/// type to use to manage an object of the specified `TYPE` template
/// parameter allocated from an allocator having the specified `ALLOCATOR`
/// template parameter type.  This primary template yields the pointer type
/// specified by `ALLOCATOR`, after rebinding to the specified `TYPE`.
template <class ALLOCATOR, class TYPE>
struct DeleteObjectProctor_PtrType
{

    typedef typename bsl::allocator_traits<ALLOCATOR>::
                                    template rebind_traits<TYPE>::pointer type;
};

/// This partial specialization is chosen when `ALLOCATOR` is a pointer type
/// (i.e., `bslma::Allocator *` or a pointer to a pool class).  It yields
/// `TYPE *` for `type`.
template <class ALLOCATOR, class TYPE>
struct DeleteObjectProctor_PtrType<ALLOCATOR *, TYPE>
{

    typedef TYPE *type;
};

                    // ----------------------------------
                    // class template DeleteObjectProctor
                    // ----------------------------------

// PRIVATE MANIPULATORS
template <class ALLOCATOR, class TYPE>
inline
void DeleteObjectProctor<ALLOCATOR, TYPE>::doDelete(bsl::false_type)
{
    AllocatorUtil::deleteObject(d_allocator, d_object_p);
}

template <class ALLOCATOR, class TYPE>
inline
void DeleteObjectProctor<ALLOCATOR, TYPE>::doDelete(bsl::true_type)
{
    BSLS_ASSERT_SAFE(0 != d_allocator);

    // When 'ALLOCATOR' is a pointer type, assume that 'deallocate' can be
    // called with a single pointer argument.  We cannot use
    // 'AllocatorUtil::deleteObject' because 'AllocatorUtil' does not work with
    // pool types, which are neither STL allocators nor derived from
    // 'bslma::Allocator'.
    d_object_p->~TYPE();
    d_allocator->deallocate(d_object_p);
}

// CREATORS
template <class ALLOCATOR, class TYPE>
inline
DeleteObjectProctor<ALLOCATOR, TYPE>::DeleteObjectProctor(
                                                    const ALLOCATOR& allocator,
                                                    PtrType          p)
    : d_allocator(allocator), d_object_p(p)
{
}

template <class ALLOCATOR, class TYPE>
inline
DeleteObjectProctor<ALLOCATOR, TYPE>::DeleteObjectProctor(
                           bslmf::MovableRef<DeleteObjectProctor> original)
    : d_allocator(MoveUtil::access(original).d_allocator)
    , d_object_p(MoveUtil::access(original).d_object_p)
{
    MoveUtil::access(original).release();
}

template <class ALLOCATOR, class TYPE>
inline
DeleteObjectProctor<ALLOCATOR, TYPE>::~DeleteObjectProctor()
{
    if (d_object_p) {
        // Dispatch to the appropriate version of 'doDelete', depending on
        // whether or not 'ALLOCATOR' is a pointer type.  We cannot simply call
        // 'AllocatorUtil::deleteObject' unconditionally because
        // 'AllocatorUtil' does not work with pool types, which are neither STL
        // allocators nor derived from 'bslma::Allocator'.
        doDelete(bsl::is_pointer<ALLOCATOR>());
    }
}

// MANIPULATORS
template <class ALLOCATOR, class TYPE>
inline
typename DeleteObjectProctor<ALLOCATOR, TYPE>::PtrType
DeleteObjectProctor<ALLOCATOR, TYPE>::release()
{
    PtrType ret = d_object_p;
    d_object_p = PtrType();
    return ret;
}

template <class ALLOCATOR, class TYPE>
inline
void DeleteObjectProctor<ALLOCATOR, TYPE>::reset(PtrType p)
{
    d_object_p = p;
}

// ACCESSORS
template <class ALLOCATOR, class TYPE>
inline
typename DeleteObjectProctor<ALLOCATOR, TYPE>::PtrType
DeleteObjectProctor<ALLOCATOR, TYPE>::ptr() const
{
    return d_object_p;
}

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_DELETEOBJECTPROCTOR)

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
