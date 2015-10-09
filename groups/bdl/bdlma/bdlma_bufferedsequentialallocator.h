// bdlma_bufferedsequentialallocator.h                                -*-C++-*-
#ifndef INCLUDED_BDLMA_BUFFEREDSEQUENTIALALLOCATOR
#define INCLUDED_BDLMA_BUFFEREDSEQUENTIALALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an efficient managed allocator using an external buffer.
//
//@CLASSES:
//  bdlma::BufferedSequentialAllocator: allocator using an external buffer
//
//@SEE_ALSO: bdlma_bufferedsequentialpool, bdlma_sequentialallocator
//
//@DESCRIPTION: This component provides a concrete mechanism,
// 'bdlma::BufferedSequentialAllocator', that implements the
// 'bdlma::ManagedAllocator' protocol to very efficiently allocate
// heterogeneous memory blocks (of varying, user-specified sizes) from an
// external buffer supplied at construction:
//..
//   ,----------------------------------.
//  ( bdlma::BufferedSequentialAllocator )
//   `----------------------------------'
//                   |        ctor/dtor
//                   |
//                   V
//       ,-----------------------.
//      ( bdlma::ManagedAllocator )
//       `-----------------------'
//                   |        release
//                   V
//          ,----------------.
//         ( bslma::Allocator )
//          `----------------'
//                            allocate
//                            deallocate
//..
// If an allocation request exceeds the remaining free memory space in the
// external buffer, the allocator will fall back to a sequence of
// dynamically-allocated buffers.  Users can optionally specify a growth
// strategy at construction that governs the growth rate of the
// dynamically-allocated buffers.  If no growth strategy is specified at
// construction, geometric growth is used.  Users can also optionally specify
// an alignment strategy at construction that governs the alignment of
// allocated memory blocks.  If no alignment strategy is specified at
// construction, natural alignment is used.  The 'release' method releases all
// memory allocated through the allocator, as does the destructor.  Note that,
// even though a 'deallocate' method is available, it has no effect:
// Individually allocated memory blocks cannot be separately deallocated.
//
// 'bdlma::BufferedSequentialAllocator' is typically used when users have a
// reasonable estimation of the amount of memory needed.  This amount of memory
// would typically be created directly on the program stack, and used as the
// initial external buffer of the allocator for very fast memory allocation.
// While the buffer has sufficient capacity, memory allocations using the pool
// will not trigger *any* dynamic memory allocation, will have optimal locality
// of reference, and will not require deallocation upon destruction.
//
// Once the external buffer is exhausted, subsequent allocation requests
// require dynamic memory allocation, and the performance of the allocator
// degrades.
//
// The main difference between a 'bdlma::BufferedSequentialAllocator' and a
// 'bdlma::BufferedSequentialPool' is that, very often, the allocator is
// maintained through a 'bslma::Allocator' pointer - hence, every call to
// 'allocate' is a virtual function call, which is slower than invoking
// 'allocate' with the pool directly.  However, the allocator interface is much
// more widely accepted across objects, and hence more general purpose.
//
///Optional 'maxBufferSize' Parameter
/// - - - - - - - - - - - - - - - - -
// An optional 'maxBufferSize' parameter can be supplied at construction to
// specify the maximum size (in bytes) of the dynamically-allocated buffers for
// geometric growth.  Once the internal buffer grows up to the 'maxBufferSize',
// further requests that exceed this size will be served by a separate memory
// block instead of the internal buffer.  The behavior is undefined unless
// 'size <= maxBufferSize', where 'size' is the extent (in bytes) of the
// external buffer supplied at construction.
//
///Warning
///-------
// Note that, even when a buffer having 'n' bytes of memory is supplied at
// construction, it does *not* mean that 'n' bytes of memory are available
// before dynamic memory allocation is triggered.  This is due to memory
// alignment requirements.  If the buffer supplied is not aligned, the first
// call to the 'allocate' method will automatically skip one or more bytes such
// that the memory allocated is properly aligned.  The number of bytes that are
// wasted depends on whether natural alignment, maximum alignment, or 1-byte
// alignment is used (see 'bsls_alignment' for more details).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlma::BufferedSequentialAllocator' with Exact Calculation
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to implement a method, 'calculate', that performs
// calculations (where the specifics are not important to illustrate the use of
// this component), which require three vectors of 'double' values.
// Furthermore, suppose we know that we need to store at most 100 values for
// each vector:
//..
//  double calculate(const bsl::vector<double>& data)
//  {
//..
// Since the amount of memory needed is known in advance, we can optimize the
// memory allocation by using a 'bdlma::BufferedSequentialAllocator' to supply
// memory for the vectors.  We can also prevent the vectors from resizing
// (which triggers more allocations) by reserving for the specific capacity we
// need:
//..
//      enum { k_SIZE = 3 * 100 * sizeof(double) };
//..
// In the above calculation, we assume that the only memory allocation
// requested by the vector is the allocation for the array that stores the
// 'double' values.  Furthermore, we assume that the 'reserve' method allocates
// the exact amount of memory for the number of items specified (in this case,
// of type 'double').  Note that both of these assumptions are true for BDE's
// implementation of 'bsl::vector'.
//
// To avoid alignment issues described in the "Warning" section (above), we
// create a 'bsls::AlignedBuffer':
//..
//      bsls::AlignedBuffer<k_SIZE> bufferStorage;
//
//      bdlma::BufferedSequentialAllocator alloc(bufferStorage.buffer(),
//                                               k_SIZE);
//
//      bsl::vector<double> v1(&alloc);     v1.reserve(100);
//      bsl::vector<double> v2(&alloc);     v2.reserve(100);
//      bsl::vector<double> v3(&alloc);     v3.reserve(100);
//
//      return data.empty() ? 0.0 : data.front();
//  }
//..
// By making use of a 'bdlma::BufferedSequentialAllocator', *all* dynamic
// memory allocation is eliminated in the above example.
//
///Example 2: Using 'bdlma::BufferedSequentialAllocator' with Fallback
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are receiving updates for price quotes for a list of securities
// through the following function:
//..
//  void receivePriceQuotes(bsl::map<bsl::string, double> *updateMap);
//      // Load into the specified 'updateMap' updates for price quotes for a
//      // list of securities.
//..
// Furthermore, suppose the number of securities we are interested in is
// limited.  We can then use a 'bdlma::BufferedSequentialAllocator' to optimize
// memory allocation for the 'bsl::map'.  We first create a buffer on the
// stack:
//..
//  enum {
//      k_NUM_SECURITIES = 100,
//
//      k_TREE_NODE_SIZE = sizeof(bsl::map<bsl::string, double>::value_type)
//                       + sizeof(void *) * 4,
//
//      k_AVERAGE_SECURITY_LENGTH = 5,
//
//      k_TOTAL_SIZE = k_NUM_SECURITIES *
//                               (k_TREE_NODE_SIZE + k_AVERAGE_SECURITY_LENGTH)
//  };
//
//  bsls::AlignedBuffer<k_TOTAL_SIZE> bufferStorage;
//..
// The calculation of the amount of memory needed is just an estimate, as we
// used the average security size instead of the maximum security size.  We
// also assume that a 'bsl::map's node size is roughly the size of 4 pointers.
//..
//  bdlma::BufferedSequentialAllocator bsa(bufferStorage.buffer(),
//                                         k_TOTAL_SIZE,
//                                         &objectAllocator);
//  bsl::map<bsl::string, double> updateMap(&bsa);
//
//  receivePriceQuotes(&updateMap);
//..
// With the use of a 'bdlma::BufferedSequentialAllocator', we can be reasonably
// assured that the memory allocation performance is optimized (i.e., minimal
// use of dynamic allocation).

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_BUFFEREDSEQUENTIALPOOL
#include <bdlma_bufferedsequentialpool.h>
#endif

#ifndef INCLUDED_BDLMA_MANAGEDALLOCATOR
#include <bdlma_managedallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_BLOCKGROWTH
#include <bsls_blockgrowth.h>
#endif

namespace BloombergLP {
namespace bdlma {

                    // =================================
                    // class BufferedSequentialAllocator
                    // =================================

class BufferedSequentialAllocator : public ManagedAllocator {
    // This class implements the 'ManagedAllocator' protocol to provide a fast
    // allocator that dispenses heterogeneous blocks of memory (of varying,
    // user-specified sizes) from an external buffer whose address and size (in
    // bytes) are supplied at construction.  If an allocation request exceeds
    // the remaining free memory space in the external buffer, memory will be
    // supplied by an (optional) allocator also supplied at construction; if no
    // allocator is supplied, the currently installed default allocator is
    // used.  This class is *exception* *neutral*: If memory cannot be
    // allocated, the behavior is defined by the (optional) allocator supplied
    // at construction.  Note that in no case will the buffered sequential
    // allocator attempt to deallocate the external buffer.

    // DATA
    BufferedSequentialPool d_pool;  // manager for allocated memory blocks

  private:
    // NOT IMPLEMENTED
    BufferedSequentialAllocator(const BufferedSequentialAllocator&);
    BufferedSequentialAllocator& operator=(const BufferedSequentialAllocator&);

  public:
    // CREATORS
    BufferedSequentialAllocator(char             *buffer,
                                int               size,
                                bslma::Allocator *basicAllocator = 0);
    BufferedSequentialAllocator(
                              char                        *buffer,
                              int                          size,
                              bsls::BlockGrowth::Strategy  growthStrategy,
                              bslma::Allocator            *basicAllocator = 0);
    BufferedSequentialAllocator(char                      *buffer,
                                int                        size,
                                bsls::Alignment::Strategy  alignmentStrategy,
                                bslma::Allocator          *basicAllocator = 0);
    BufferedSequentialAllocator(
                              char                        *buffer,
                              int                          size,
                              bsls::BlockGrowth::Strategy  growthStrategy,
                              bsls::Alignment::Strategy    alignmentStrategy,
                              bslma::Allocator            *basicAllocator = 0);
        // Create a buffered sequential allocator for allocating memory blocks
        // from the specified external 'buffer' having the specified 'size' (in
        // bytes).  Optionally specify a 'growthStrategy' used to control
        // buffer growth.  If a 'growthStrategy' is not specified, geometric
        // growth is used.  Optionally specify an 'alignmentStrategy' used to
        // align allocated memory blocks.  If an 'alignmentStrategy' is not
        // specified, natural alignment is used.  Optionally specify a
        // 'basicAllocator' used to supply memory should the capacity of
        // 'buffer' be exhausted.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless '0 < size', and 'buffer' has at least 'size' bytes.  Note
        // that, due to alignment effects, it is possible that not all 'size'
        // bytes of memory in 'buffer' can be used for allocation.  Also note
        // that no limit is imposed on the size of the internal buffers when
        // geometric growth is used.  Also note that when constant growth is
        // used, the size of the internal buffers will always be the same as
        // 'size'.

    BufferedSequentialAllocator(char             *buffer,
                                int               size,
                                int               maxBufferSize,
                                bslma::Allocator *basicAllocator = 0);
    BufferedSequentialAllocator(
                              char                        *buffer,
                              int                          size,
                              int                          maxBufferSize,
                              bsls::BlockGrowth::Strategy  growthStrategy,
                              bslma::Allocator            *basicAllocator = 0);
    BufferedSequentialAllocator(char                      *buffer,
                                int                        size,
                                int                        maxBufferSize,
                                bsls::Alignment::Strategy  alignmentStrategy,
                                bslma::Allocator          *basicAllocator = 0);
    BufferedSequentialAllocator(
                              char                        *buffer,
                              int                          size,
                              int                          maxBufferSize,
                              bsls::BlockGrowth::Strategy  growthStrategy,
                              bsls::Alignment::Strategy    alignmentStrategy,
                              bslma::Allocator            *basicAllocator = 0);
        // Create a buffered sequential allocator for allocating memory blocks
        // from the specified external 'buffer' having the specified 'size' (in
        // bytes), or from an internal buffer (after the external 'buffer' is
        // exhausted) where the buffer growth is limited to the specified
        // 'maxBufferSize' (in bytes).  Optionally specify a 'growthStrategy'
        // used to control buffer growth.  If a 'growthStrategy' is not
        // specified, geometric growth is used.  Optionally specify an
        // 'alignmentStrategy' used to align allocated memory blocks.  If an
        // 'alignmentStrategy' is not specified, natural alignment is used.
        // Optionally specify a 'basicAllocator' used to supply memory should
        // the capacity of 'buffer' be exhausted.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 < size', 'size <= maxBufferSize', and 'buffer'
        // has at least 'size' bytes.  Note that, due to alignment effects, it
        // is possible that not all 'size' bytes of memory in 'buffer' can be
        // used for allocation.  Also note that when constant growth is used,
        // the size of the internal buffers will always be the same as 'size'.

    virtual ~BufferedSequentialAllocator();
        // Destroy this buffered sequential allocator.  All memory allocated
        // from this allocator is released.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) according to the alignment strategy specified at
        // construction.  If 'size' is 0, no memory is allocated and 0 is
        // returned.  If the allocation request exceeds the remaining free
        // memory space in the external buffer supplied at construction, use
        // memory obtained from the allocator supplied at construction.

    virtual void deallocate(void *address);
        // This method has no effect on the memory block at the specified
        // 'address' as all memory allocated by this allocator is managed.  The
        // behavior is undefined unless 'address' is 0, or was allocated by
        // this allocator and has not already been deallocated.

    virtual void release();
        // Release all memory currently allocated through this allocator.  This
        // method deallocates all memory (if any) allocated with the allocator
        // provided at construction, and makes the memory from the entire
        // external buffer supplied at construction available for subsequent
        // allocations, but has no effect on the contents of the buffer.  Note
        // that this allocator is reset to its initial state by this method.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                    // ---------------------------------
                    // class BufferedSequentialAllocator
                    // ---------------------------------

// CREATORS
inline
BufferedSequentialAllocator::BufferedSequentialAllocator(
                                              char             *buffer,
                                              int               size,
                                              bslma::Allocator *basicAllocator)
: d_pool(buffer, size, basicAllocator)
{
}

inline
BufferedSequentialAllocator::BufferedSequentialAllocator(
                                   char                        *buffer,
                                   int                          size,
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
: d_pool(buffer, size, growthStrategy, basicAllocator)
{
}

inline
BufferedSequentialAllocator::BufferedSequentialAllocator(
                                  char                      *buffer,
                                  int                        size,
                                  bsls::Alignment::Strategy  alignmentStrategy,
                                  bslma::Allocator          *basicAllocator)
: d_pool(buffer, size, alignmentStrategy, basicAllocator)
{
}

inline
BufferedSequentialAllocator::BufferedSequentialAllocator(
                                char                        *buffer,
                                int                          size,
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                bsls::Alignment::Strategy    alignmentStrategy,
                                bslma::Allocator            *basicAllocator)
: d_pool(buffer, size, growthStrategy, alignmentStrategy, basicAllocator)
{
}

inline
BufferedSequentialAllocator::BufferedSequentialAllocator(
                                              char             *buffer,
                                              int               size,
                                              int               maxBufferSize,
                                              bslma::Allocator *basicAllocator)
: d_pool(buffer, size, maxBufferSize, basicAllocator)
{
}

inline
BufferedSequentialAllocator::BufferedSequentialAllocator(
                                   char                        *buffer,
                                   int                          size,
                                   int                          maxBufferSize,
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
: d_pool(buffer, size, maxBufferSize, growthStrategy, basicAllocator)
{
}

inline
BufferedSequentialAllocator::BufferedSequentialAllocator(
                                  char                      *buffer,
                                  int                        size,
                                  int                        maxBufferSize,
                                  bsls::Alignment::Strategy  alignmentStrategy,
                                  bslma::Allocator          *basicAllocator)
: d_pool(buffer, size, maxBufferSize, alignmentStrategy, basicAllocator)
{
}

inline
BufferedSequentialAllocator::BufferedSequentialAllocator(
                                char                        *buffer,
                                int                          size,
                                int                          maxBufferSize,
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                bsls::Alignment::Strategy    alignmentStrategy,
                                bslma::Allocator            *basicAllocator)
: d_pool(buffer,
         size,
         maxBufferSize,
         growthStrategy,
         alignmentStrategy,
         basicAllocator)
{
}

// MANIPULATORS
inline
void BufferedSequentialAllocator::deallocate(void *)
{
}

inline
void BufferedSequentialAllocator::release()
{
    d_pool.release();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
