// bslma_deallocatebytesproctor.h                                    -*-C++-*-
#ifndef INCLUDED_BSLMA_DEALLOCATEBYTESPROCTOR
#define INCLUDED_BSLMA_DEALLOCATEBYTESPROCTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor to conditionally unwind memory block allocation.
//
//@CLASSES:
//  bslma::DeallocateBytesProctor: proctor to conditionally unwind allocation.
//
//@SEE_ALSO: bslma_deallocateobjectproctor, bslma_deleteobjectproctor
//
//@DESCRIPTION: This component provides a proctor class template,
// `bslma::DeallocateBytesProctor`, that conditionally reverses the effect of
// `bslma::AllocatorUtil::allocateBytes(a, n, align)` or `a->allocate(n)`,
// where `a` is a memory resource or pool that allocates, `n` is the number of
// bytes to allocate, and align is the desired alignment.  Upon destruction,
// this proctor deallocates the block from the specified allocator or pool
// without calling any destructors.  The proctor's constructor takes the same
// arguments as `bslma::AllocatorUtil::deallocateBytes`, making it
// straightforward to allocate an object using `allocateBytes` and protect it
// using `DeallocateBytesProctor`.
//
// As with all proctors, this class is used to automatically reclaim a resource
// in the event that a function ends prematurely, e.g., via an exception.
// After allocating a block of memory, an exception-safe function
// would create a `DeallocateBytesProctor` to manage the new memory block.
// If the operation completes successfully, the code calls the proctor's
// `release` method, which disengages the proctor.  If, however, the function
// exits while the proctor is still engaged, the proctor's destructor will
// deallocate the managed memory block.
//
// The `DeallocateBytesProctor` template has one template parameter: the
// `ALLOCATOR` type used to reclaim storage managed by the proctor.  If
// `ALLOCATOR` is a non-pointer type, the proctor uses
// `bslma::AllocatorUtil::deallocateBytes(a, ptr, n, align)` to reclaim
// storage, where `a` is the allocator, `ptr` is the address of the managed
// memory block, `n` is the number of managed bytes, and `align` is an optional
// argument specifying the alignment of the block (max-aligned by default).
// Otherwise, if `ALLOCATOR` is a pointer type, the proctor reclaims memory by
// calling `a->deallocate(ptr)`.  Instantiating `DeallocateBytesProctor` with a
// pointer-type `ALLOCATOR` is appropriate for classes derived from
// `bslma::Allocator` and for BDE-style pool types, i.e., classes for which
// `a->deallocate(ptr)` is well formed.
//
///Usage
///-----
//Example 1: Class having an owning pointer
//- - - - - - - - - - - - - - - - - - - - -
// In this example, we create a class, `my_Manager`, having an owning pointer
// to an object of another class, `my_Data`.  Because it owns the `my_Data`
// object, `my_Manager` is responsible for allocating, constructing,
// deallocating, and destroying it.
//
// First, we define the `my_Data` class, which holds an integer value and
// counts how many times its constructor and destructor have been called. Its
// constructor will throw an exception if its integer argument equals the
// number of constructor calls before construction:
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
//     explicit my_Data(int v) : d_value(v)
//     {
//         if (v == s_numConstructed) throw s_numConstructed;
//         ++s_numConstructed;
//     }
//     my_Data(const my_Data& original);
//     ~my_Data() { ++s_numDestroyed; }
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
//                        const allocator_type& allocator = allocator_type());
//     my_Manager(const my_Manager& original);
//     ~my_Manager();
//
//     // ...
// };
//
// int my_Manager::s_numConstructed = 0;
// ```
// Next, we define the constructor for `my_Manager`, which begins by allocating
// a `my_Data` object:
// ```
// my_Manager::my_Manager(int v, const allocator_type& allocator)
//     : d_allocator(allocator), d_data_p(0)
// {
//     d_data_p = static_cast<my_Data*>(
//         bslma::AllocatorUtil::allocateBytes(allocator, sizeof(my_Data)));
// ```
// Then, the `my_Manager` constructor constructs the `my_Data` object in the
// allocated memory.  However, as the constructor might throw, it first
// protects the data object with a `bslma::DeallocateBytesProctor`:
// ```
//     bslma::DeallocateBytesProctor<allocator_type>
//                            proctor(d_allocator, d_data_p, sizeof(my_Data));
//     bslma::ConstructionUtil::construct(d_data_p, d_allocator, v);
// ```
// Then, once the `construct` operation completes successfully, we can release
// the data object from the proctor.  Only then do we increment the
// construction count, as the constructor is now complete:
// ```
//     proctor.release();
//     ++s_numConstructed;
// }
// ```
// Next, we define the `my_Manager` destructor, which destroys and deallocates
// its data object.  Note that the arguments to `deallocateBytes` is identical
// to the constructor arguments to the `DeallocateBytesProctor`, above:
// ```
// my_Manager::~my_Manager()
// {
//     d_data_p->~my_Data();
//     bslma::AllocatorUtil::deallocateBytes(d_allocator, d_data_p,
//                                           sizeof(my_Data));
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
//         assert(1 == my_Manager::numConstructed());
//     }
//     assert(0 == ta.numBlocksInUse());
//     assert(1 == ta.numBlocksTotal());
//     assert(1 == my_Manager::numConstructed());
// ```
// Finally, when the `my_Data` constructor does throw, a block is allocated but
// we verify that the `my_Manager` constructor did not complete and that the
// block is automatically deallocated, resulting in no leaks:
// ```
//     try {
//         my_Manager obj2(1, &ta);  // Will throw an exception
//         assert(false && "Can't get here");
//     }
//     catch (int e) {
//         assert(1 == e);
//         assert(2 == ta.numBlocksTotal());  // A 2nd block was allocated...
//         assert(0 == ta.numBlocksInUse());  // ...but was then deallocated
//         assert(1 == my_Manager::numConstructed());
//     }
//     assert(1 == my_Manager::numConstructed());
// }
// ```

#include <bslscm_version.h>

#include <bslma_allocatorutil.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_alignmentutil.h>
#include <bsls_keyword.h>

#include <cstdlib>  // std::size_t

namespace BloombergLP {
namespace bslma {

// FORWARD DECLARATIONS
template <class ALLOCATOR>
struct DeallocateBytesProctor_PtrType;

                    // ======================================
                    // class template DeallocateBytesProctor
                    // ======================================

/// This class implements a proctor that, unless its `release` method has
/// previously been invoked, automatically deallocates a managed block of
/// memory upon destruction by invoking the `deallocate` method of an
/// allocator (or pool) of parameterized `ALLOCATOR` type supplied to it at
/// construction.  The managed memory block must have been provided by the
/// supplied allocator (or pool); the allocator (or pool) must remain valid
/// throughout the lifetime of the proctor object.  If `ALLOCATOR` is a
/// non-pointer type, it is assumed to be STL compatible; otherwise it is
/// assumed have the same deallocation interface as `bslma::Allocator`.
template <class ALLOCATOR>
class DeallocateBytesProctor {

  private:
    // PRIVATE CONSTANTS
    enum { k_MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

    // PRIVATE TYPES
    typedef bslmf::MovableRefUtil                                    MoveUtil;
    typedef typename DeallocateBytesProctor_PtrType<ALLOCATOR>::type PtrType;

    // DATA
    ALLOCATOR   d_allocator;  // allocator object (might be a pointer)
    PtrType     d_bytes_p;    // managed block (null if disengaged)
    std::size_t d_nbytes;     // number of managed bytes
    std::size_t d_alignment;  // alignment of managed blocks

    // PRIVATE MANIPULATORS

    /// Deallocate the block at `d_bytes_p`.  The first overload is selected
    /// for non-pointer-type `ALLOCATOR` and invokes
    /// `bslma::AllocatorUtil::deallocateBytes(d_allocator, d_bytes,
    /// d_nbytes, d_alignment)`.  The second overload is selected for a
    /// pointer-type `ALLOCATOR` and invokes
    /// `d_allocator->deallocate(d_object_p)`.
    void doDeallocate(bsl::false_type);
    void doDeallocate(bsl::true_type);

    // NOT IMPLEMENTED
    DeallocateBytesProctor(const DeallocateBytesProctor&) BSLS_KEYWORD_DELETED;
    DeallocateBytesProctor&
    operator=(const DeallocateBytesProctor&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS

    /// Create a proctor to manage the specified `nbytes` bytes at the
    /// specified `p` address, which have been allocated from the specified
    /// `allocator`.  Optionally specify the `alignment` used to allocate
    /// the bytes.  If `p` is null, then the created proctor is disengaged
    /// (manages no memory).  The behavior is undefined unless `p` is null
    /// or was allocated from `allocator` with a size of `nbytes` and
    /// alignment of `alignment`.
    DeallocateBytesProctor(const ALLOCATOR& allocator,
                           PtrType          p,
                           std::size_t      nbytes,
                           std::size_t      alignment = k_MAX_ALIGNMENT);

    /// This move constructor creates a proctor managing the same block as
    /// `original`.  After the move, `original` is disengaged.
    DeallocateBytesProctor(bslmf::MovableRef<DeallocateBytesProctor> original);

    /// Deallocate the managed memory, if any, without invoking any other
    /// destructors.
    ~DeallocateBytesProctor();

    // MANIPULATORS

    /// Disengage this allocator and return a pointer to the formerly
    /// managed memory block.
    PtrType release();

    /// Release the managed memory and reset this proctor to manage the
    /// specified `nbytes` bytes at the specified `p` address.  Optionally
    /// specify the `alignment` used to allocate `p`.  If `p` is null, then
    /// the proctor is disengaged (manages no memory).  The behavior is
    /// undefined unless `p` is null or was allocated with a size of
    /// `nbytes` and alignment of `alignment` from the same allocator used
    /// to construct this proctor.  Note that the previously managed block
    /// is not deallocated.
    void reset(PtrType     p,
               std::size_t nbytes,
               std::size_t alignment = k_MAX_ALIGNMENT);

    // ACCESSORS

    /// Return the address of the currently managed memory block, if
    /// engaged; otherwise return a null pointer.
    PtrType ptr() const;
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                // ----------------------------------------------
                // struct template DeallocateBytesProctor_PtrType
                // ----------------------------------------------

/// This metafunction has a nested `type` that indicates the correct `void`
/// pointer type to use to manage bytes allocated from an allocator having
/// the specified `ALLOCATOR` template parameter type.  This primary
/// template yields the `void_pointer` type specified by the
/// `allocator_traits` for `ALLOCATOR`.
template <class ALLOCATOR>
struct DeallocateBytesProctor_PtrType
{

    typedef typename bsl::allocator_traits<ALLOCATOR>::void_pointer type;
};

/// This partial specialization is chosen when `ALLOCATOR` is a pointer type
/// (i.e., `bslma::Allocator *` or a pointer to a pool class).  It yields
/// a `type` of `void *`.
template <class ALLOCATOR>
struct DeallocateBytesProctor_PtrType<ALLOCATOR *>
{

    typedef void *type;
};

                    // --------------------------------------
                    // class template DeallocateBytesProctor
                    // --------------------------------------

// PRIVATE MANIPULATORS
template <class ALLOCATOR>
inline
void DeallocateBytesProctor<ALLOCATOR>::doDeallocate(bsl::false_type)
{
    bslma::AllocatorUtil::deallocateBytes(d_allocator, d_bytes_p,
                                          d_nbytes, d_alignment);
}

template <class ALLOCATOR>
inline
void DeallocateBytesProctor<ALLOCATOR>::doDeallocate(bsl::true_type)
{
    d_allocator->deallocate(d_bytes_p);
}

// CREATORS
template <class ALLOCATOR>
inline
DeallocateBytesProctor<ALLOCATOR>::DeallocateBytesProctor(
                                                    const ALLOCATOR& allocator,
                                                    PtrType          p,
                                                    std::size_t      nbytes,
                                                    std::size_t      alignment)
    : d_allocator(allocator)
    , d_bytes_p(p)
    , d_nbytes(nbytes)
    , d_alignment(alignment)
{
}

template <class ALLOCATOR>
inline
DeallocateBytesProctor<ALLOCATOR>::DeallocateBytesProctor(
                           bslmf::MovableRef<DeallocateBytesProctor> original)
    : d_allocator(MoveUtil::access(original).d_allocator)
    , d_bytes_p(MoveUtil::access(original).d_bytes_p)
    , d_nbytes(MoveUtil::access(original).d_nbytes)
    , d_alignment(MoveUtil::access(original).d_alignment)
{
    MoveUtil::access(original).release();
}

template <class ALLOCATOR>
inline
DeallocateBytesProctor<ALLOCATOR>::~DeallocateBytesProctor()
{
    if (d_bytes_p) {
        doDeallocate(bsl::is_pointer<ALLOCATOR>());
    }
}

// MANIPULATORS
template <class ALLOCATOR>
inline
typename DeallocateBytesProctor<ALLOCATOR>::PtrType
DeallocateBytesProctor<ALLOCATOR>::release()
{
    PtrType ret = d_bytes_p;
    d_bytes_p = PtrType();
    return ret;
}

template <class ALLOCATOR>
inline
void DeallocateBytesProctor<ALLOCATOR>::reset(PtrType     p,
                                              std::size_t nbytes,
                                              std::size_t alignment)
{
    d_bytes_p   = p;
    d_nbytes    = nbytes;
    d_alignment = alignment;
}

// ACCESSORS
template <class ALLOCATOR>
inline
typename DeallocateBytesProctor<ALLOCATOR>::PtrType
DeallocateBytesProctor<ALLOCATOR>::ptr() const
{
    return d_bytes_p;
}

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_DEALLOCATEBYTESPROCTOR)

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
