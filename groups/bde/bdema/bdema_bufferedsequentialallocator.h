// bdema_bufferedsequentialallocator.h                                -*-C++-*-
#ifndef INCLUDED_BDEMA_BUFFEREDSEQUENTIALALLOCATOR
#define INCLUDED_BDEMA_BUFFEREDSEQUENTIALALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an efficient managed allocator using an external buffer.
//
//@CLASSES:
//  bdema_BufferedSequentialAllocator: allocator using an external buffer
//
//@SEE_ALSO: bdema_bufferedsequentialpool, bdema_sequentialallocator
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides a concrete mechanism,
// 'bdema_BufferedSequentialAllocator', that implements the
// 'bslma_ManagedAllocator' protocol to very efficiently allocate heterogeneous
// memory blocks (of varying, user-specified sizes) from an external buffer
// supplied at construction:
//..
//   ,---------------------------------.
//  ( bdema_BufferedSequentialAllocator )
//   `---------------------------------'
//                   |        ctor/dtor
//                   |
//                   V
//       ,----------------------.
//      ( bslma_ManagedAllocator )
//       `----------------------'
//                   |        release
//                   V
//          ,----------------.
//         (  bslma_Allocator )
//          `----------------'
//                            allocate
//                            deallocate
//..
// If an allocation request exceeds the remaining free memory space in
// the external buffer, the allocator will fall back to a sequence of
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
// 'bdema_BufferedSequentialAllocator' is typically used when users have a
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
// The main difference between a 'bdema_BufferedSequentialAllocator' and a
// 'bdema_BufferedSequentialPool' is that, very often, the allocator is
// maintained through a 'bslma_Allocator' pointer - hence, every call to
// 'allocate' is a virtual function call, which is slower than invoking
// 'allocate' with the pool directly.  However, the allocator interface is much
// more widely accepted across objects, and hence more general purpose.
//
///Warning
///-------
// Note that, even when a buffer having 'n' bytes of memory is supplied at
// construction, it does *not* mean that 'n' bytes of memory are available
// before dynamic memory allocation is triggered.  This is due to memory
// alignment requirements.  If the buffer supplied is not aligned, the first
// call to the 'allocate' method will automatically skip one or more bytes such
// that the memory allocated is properly aligned.  The number of bytes that are
// wasted depends on whether natural alignment or maximum alignment is used
// (see 'bsls_alignment' for more details).
//
///Usage
///-----
///Example 1: Using 'bdema_BufferedSequentialAllocator' with Exact Calculation
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to implement a method, 'calculate', that performs
// calculations (where the specifics are not important to illustrate the use of
// this component), which requires three vectors of 'double' values.
// Furthermore, suppose we know that we need to store at most 100 values for
// each vector:
//..
//  double calculate(const bsl::vector<double>& data)
//  {
//..
// Since the amount of memory needed is known in advance, we can optimize the
// memory allocation by using a 'bdema_BufferedSequentialAllocator' to supply
// memory for the vectors.  We can also prevent the vectors from resizing
// (which triggers more allocations) by reserving for the specific capacity we
// need:
//..
//      enum { SIZE = 3 * 100 * sizeof(double) };
//..
// In the above calculation, we assume that the only memory allocation
// requested by the vector is the allocation for the array that stores the
// 'double' values.  Furthermore, we assume that the 'reserve' method allocates
// the exact amount of memory for the number of items (in this case, 'double')
// specified.  Note that both of these assumptions are true in BDE's
// implementation of 'bsl::vector'.
//
// To avoid alignment issues described in the "Warning" section (above), we can
// create a 'bsls_AlignedBuffer':
//..
//      bsls_AlignedBuffer<SIZE> bufferStorage;
//
//      bdema_BufferedSequentialAllocator alloc(bufferStorage.buffer(), SIZE);
//
//      bsl::vector<double> v1(&alloc);     v1.reserve(100);
//      bsl::vector<double> v2(&alloc);     v2.reserve(100);
//      bsl::vector<double> v3(&alloc);     v3.reserve(100);
//
//      // ...
//  }
//..
// By making use of a 'bdema_BufferedSequentialAllocator', *all* dynamic memory
// allocation is eliminated in the above example.
//
///Example 2: Using 'bdema_BufferedSequentialAllocator' with Fallback
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are receiving updates for price quotes for a list of securities
// through the following function:
//..
//  void receivePriceQuotes(bsl::map<bsl::string, double> *updateMap);
//      // Load into the specified 'updateMap' updates for price quotes for a
//      // list of securities.
//..
// Furthermore, suppose the number of securities we are interested in is
// limited.  We can then use a 'bdema_BufferedSequentialAllocator' to optimize
// memory allocation for the 'bsl::map'.  We first create a buffer on the
// stack:
//..
//  enum {
//      NUM_SECURITIES = 100,
//
//      TREE_NODE_SIZE = sizeof(bsl::map<bsl::string, double>::value_type)
//                       + sizeof(void *) * 4,
//
//      AVERAGE_SECURITY_LENGTH = 5,
//
//      TOTAL_SIZE = NUM_SECURITIES *
//                   (TREE_NODE_SIZE + AVERAGE_SECURITY_LENGTH )
//  };
//
//  char buffer[TOTAL_SIZE];
//..
// The calculation of the amount of memory needed is just an estimate, as we
// used the average security size instead of the maximum security size.  We
// also assume that a 'bsl::map's node size is roughly the size of 4 pointers.
//..
//  bdema_BufferedSequentialAllocator bsa(buffer, TOTAL_SIZE);
//  bsl::map<bsl::string, double> updateMap(&bsa);
//
//  receivePriceQuotes(&updateMap);
//..
// With the use of a 'bdema_BufferedSequentialAllocator', we can be reasonably
// assured that the memory allocation performance is optimized (i.e., minimal
// use of dynamic allocation).

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_BUFFEREDSEQUENTIALPOOL
#include <bdema_bufferedsequentialpool.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_BLOCKGROWTH
#include <bsls_blockgrowth.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                    // =======================================
                    // class bdema_BufferedSequentialAllocator
                    // =======================================

class bdema_BufferedSequentialAllocator : public bslma_ManagedAllocator {
    // This class implements the 'bslma_ManagedAllocator' protocol to provide
    // a fast allocator that dispenses heterogeneous blocks of memory (of
    // varying, user-specified sizes) from an external buffer whose address and
    // size (in bytes) are supplied at construction.  If an allocation request
    // exceeds the remaining free memory space in the external buffer, memory
    // will be supplied by an (optional) allocator also supplied at
    // construction; if no allocator is supplied, the currently installed
    // default allocator is used.  This class is *exception* *neutral*: If
    // memory cannot be allocated, the behavior is defined by the (optional)
    // allocator supplied at construction.  Note that in no cases will the
    // buffered sequential allocator attempt to deallocate the external buffer.

    // DATA
    bdema_BufferedSequentialPool d_pool;  // manager for allocated memory
                                          // blocks

  private:
    // NOT IMPLEMENTED
    bdema_BufferedSequentialAllocator(
                                     const bdema_BufferedSequentialAllocator&);
    bdema_BufferedSequentialAllocator& operator=(
                                     const bdema_BufferedSequentialAllocator&);

  public:
    // CREATORS
    bdema_BufferedSequentialAllocator(
                               char                       *buffer,
                               int                         size,
                               bslma_Allocator            *basicAllocator = 0);
    bdema_BufferedSequentialAllocator(
                               char                       *buffer,
                               int                         size,
                               bsls_BlockGrowth::Strategy  growthStrategy,
                               bslma_Allocator            *basicAllocator = 0);
    bdema_BufferedSequentialAllocator(
                               char                       *buffer,
                               int                         size,
                               bsls_Alignment::Strategy    alignmentStrategy,
                               bslma_Allocator            *basicAllocator = 0);
    bdema_BufferedSequentialAllocator(
                               char                       *buffer,
                               int                         size,
                               bsls_BlockGrowth::Strategy  growthStrategy,
                               bsls_Alignment::Strategy    alignmentStrategy,
                               bslma_Allocator            *basicAllocator = 0);
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
        // bytes of memory in 'buffer' can be used for allocation.

    bdema_BufferedSequentialAllocator(
                               char                       *buffer,
                               int                         size,
                               int                         maxBufferSize,
                               bslma_Allocator            *basicAllocator = 0);
    bdema_BufferedSequentialAllocator(
                               char                       *buffer,
                               int                         size,
                               int                         maxBufferSize,
                               bsls_BlockGrowth::Strategy  growthStrategy,
                               bslma_Allocator            *basicAllocator = 0);
    bdema_BufferedSequentialAllocator(
                               char                       *buffer,
                               int                         size,
                               int                         maxBufferSize,
                               bsls_Alignment::Strategy    alignmentStrategy,
                               bslma_Allocator            *basicAllocator = 0);
    bdema_BufferedSequentialAllocator(
                               char                       *buffer,
                               int                         size,
                               int                         maxBufferSize,
                               bsls_BlockGrowth::Strategy  growthStrategy,
                               bsls_Alignment::Strategy    alignmentStrategy,
                               bslma_Allocator            *basicAllocator = 0);
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
        // used for allocation.

    virtual ~bdema_BufferedSequentialAllocator();
        // Destroy this buffered sequential allocator.  All memory allocated
        // from this allocator is released.

    // MANIPULATORS
    virtual void *allocate(bsls_Types::size_type size);
        // Return the address of a contiguous block of memory of the specified
        // 'size' (in bytes) according to the alignment strategy specified at
        // construction.  If 'size' is 0, no memory is allocated and 0 is
        // returned.  If the allocation request exceeds the remaining free
        // memory space in the external buffer supplied at construction, use
        // memory obtained from the allocator supplied at construction.  The
        // behavior is undefined unless '0 <= size'.

    virtual void deallocate(void *address);
        // This method has no effect on the memory block at the specified
        // 'address' as all memory allocated by this allocator is managed.  The
        // behavior is undefined unless 'address' was allocated by this
        // allocator, and has not already been deallocated.

    virtual void release();
        // Release all memory currently allocated through this allocator.  This
        // method deallocates all memory (if any) allocated with the allocator
        // provided at construction, and make available memory from the entire
        // external buffer supplied at construction for subsequent allocations,
        // but has no effect on the contents of the buffer.  Note that this
        // allocator is reset to its initial state by this method.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                    // ---------------------------------------
                    // class bdema_BufferedSequentialAllocator
                    // ---------------------------------------

// CREATORS
inline
bdema_BufferedSequentialAllocator::bdema_BufferedSequentialAllocator(
                                 char            *buffer,
                                 int              size,
                                 bslma_Allocator *basicAllocator)
: d_pool(buffer, size, basicAllocator)
{
}

inline
bdema_BufferedSequentialAllocator::bdema_BufferedSequentialAllocator(
                                 char                       *buffer,
                                 int                         size,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_pool(buffer, size, growthStrategy, basicAllocator)
{
}

inline
bdema_BufferedSequentialAllocator::bdema_BufferedSequentialAllocator(
                                 char                     *buffer,
                                 int                       size,
                                 bsls_Alignment::Strategy  alignmentStrategy,
                                 bslma_Allocator          *basicAllocator)
: d_pool(buffer, size, alignmentStrategy, basicAllocator)
{
}

inline
bdema_BufferedSequentialAllocator::bdema_BufferedSequentialAllocator(
                                 char                       *buffer,
                                 int                         size,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bsls_Alignment::Strategy    alignmentStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_pool(buffer, size, growthStrategy, alignmentStrategy, basicAllocator)
{
}

inline
bdema_BufferedSequentialAllocator::bdema_BufferedSequentialAllocator(
                                 char            *buffer,
                                 int              size,
                                 int              maxBufferSize,
                                 bslma_Allocator *basicAllocator)
: d_pool(buffer, size, maxBufferSize, basicAllocator)
{
}

inline
bdema_BufferedSequentialAllocator::bdema_BufferedSequentialAllocator(
                                 char                       *buffer,
                                 int                         size,
                                 int                         maxBufferSize,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_pool(buffer, size, maxBufferSize, growthStrategy, basicAllocator)
{
}

inline
bdema_BufferedSequentialAllocator::bdema_BufferedSequentialAllocator(
                                 char                     *buffer,
                                 int                       size,
                                 int                       maxBufferSize,
                                 bsls_Alignment::Strategy  alignmentStrategy,
                                 bslma_Allocator          *basicAllocator)
: d_pool(buffer, size, maxBufferSize, alignmentStrategy, basicAllocator)
{
}

inline
bdema_BufferedSequentialAllocator::bdema_BufferedSequentialAllocator(
                                 char                       *buffer,
                                 int                         size,
                                 int                         maxBufferSize,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bsls_Alignment::Strategy    alignmentStrategy,
                                 bslma_Allocator            *basicAllocator)
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
void bdema_BufferedSequentialAllocator::deallocate(void *)
{
}

inline
void bdema_BufferedSequentialAllocator::release()
{
    d_pool.release();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
