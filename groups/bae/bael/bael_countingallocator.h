// bael_countingallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BAEL_COUNTINGALLOCATOR
#define INCLUDED_BAEL_COUNTINGALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a concrete allocator keep a count of allocated bytes.
//
//@CLASSES:
//   bael_CountingAllocator: maximally-aligning, instrumented allocator adaptor
//
//@SEE_ALSO: bael_fixedsizerecordbuffer
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component provides a special-purpose instrumented
// allocator, 'bael_CountingAllocator', that implements the 'bslma_Allocator'
// protocol and guarantees maximal alignment of allocated blocks, even when
// the allocator supplied at construction guarantees only natural alignment
// (or no alignment at all).  'bael_CountingAllocator' maintains a
// user-resettable running sum of the total number of bytes
// allocated (called byte count, see below).
//..
//   ,----------------------.
//  ( bael_CountingAllocator )
//   `----------------------'
//               |         ctor/dtor
//               |         numBytesTotal
//               |         resetNumBytesTotal
//               V
//       ,---------------.
//      ( bslma_Allocator )
//       `---------------'
//                       allocate
//                       deallocate
//..
///Byte Count
///----------
// The byte count maintained by 'bael_CountingAllocator' is set to 0 upon
// construction and after each call to 'resetNumBytesTotal'.  Each call of
// 'allocate(size)' increases the byte count by the sum of the least
// multiple of 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT' that is greater than or
// equal to 'size' and 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'.  Each call of
// 'deallocate' decrements the byte count by the same amount by which the
// byte count was incremented on matching 'allocate' call.  The current
// value of the byte count is returned by the 'numBytesTotal' accessor.
//
///Usage
///-----
// In the following example we demonstrate how the counting allocator can
// be used to know the amount of dynamic memory allocated by a
// 'vector<int>' after pushing one integer.  Let us assume that memory for
// the vector comes from a 'bslma_Allocator' named 'allocator'.
//..
//    // First create the counting allocator using 'allocator'.
//    BloombergLP::bael_CountingAllocator countingAllocator(allocator);
//
//    // Now create the vector using the counting allocator.
//    bsl::vector<int> vec(&countingAllocator);
//
//    vec.push_back(1);
//    // The following will print the memory consumed by the
//    // vector and the counting allocator.
//    bsl::cout << "dynamic memory after first push back: "
//              << countingAllocator.numBytesTotal() << bsl::endl;
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class bael_CountingAllocator
                        // ============================

class bael_CountingAllocator : public bslma_Allocator {
    // This class maintains a count of the total number of allocated bytes.
    // The running byte count is initialized to 0 upon construction, is
    // increased by the 'allocate' method, and may be reset to 0 by the
    // 'resetNumBytesTotal' method.  The 'deallocate' method appropriately
    // decrement the byte count.  The precise definition of byte count is
    // described in the "Byte Count" section of the component-level
    // documentation.

    // DATA
    int              d_byteCount;    // byte count
    bslma_Allocator *d_allocator_p;  // holds (but does not own) allocator

    // NOT IMPLEMENTED
    bael_CountingAllocator(const bael_CountingAllocator&);
    bael_CountingAllocator& operator=(const bael_CountingAllocator&);

  public:
    // CREATORS
    bael_CountingAllocator(bslma_Allocator *basicAllocator = 0);
        // Create a counting allocator having an initial byte count of 0.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    virtual ~bael_CountingAllocator();
        // Destroy this counting allocator.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (bytes) and increment the byte count maintained by
        // this counting allocator by the sum of the least multiple of
        // 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT' that is greater than or
        // equal to 'size' and 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'.
        // The behavior is undefined unless 0 <= size.  Note that the alignment
        // of the address returned is the maximum alignment for any fundamental
        // type defined for the calling platform, even if the supplied
        // allocator guarantees only natural alignment.

    virtual void deallocate(void *address);
        // Return the memory at the specified 'address' back to this allocator
        // and update the byte count maintained by this counting allocator
        // appropriately.  If 'address' is 0, this function has no effect.  The
        // behavior is undefined unless 'address' was allocated using this
        // allocator and has not already been deallocated.

    void resetNumBytesTotal();
        // Reset the byte count maintained by this counting allocator to
        // 0.

    // ACCESSORS
    int numBytesTotal() const;
        // Return the byte count maintained by this counting allocator.
        // The precise definition of byte count is described in the "Byte
        // Count" section of the component-level documentation.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
bael_CountingAllocator::bael_CountingAllocator(bslma_Allocator *basicAllocator)
: d_byteCount(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

// MANIPULATORS
inline
void *bael_CountingAllocator::allocate(size_type size)
{
    int paddedSize = bsls_PlatformUtil::roundUpToMaximalAlignment((int)size);
    int totalSize = paddedSize + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

    d_byteCount += totalSize;
    void *address = d_allocator_p->allocate(totalSize);
    *((int *)address) = totalSize;
    return (char *)address + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
}

inline
void bael_CountingAllocator::deallocate(void *address)
{
    if (address) {
        address = (char *)address - bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
        d_byteCount -= *((int *)address);
        d_allocator_p->deallocate(address);
    }
}

inline
void bael_CountingAllocator::resetNumBytesTotal()
{
    d_byteCount = 0;
}

// ACCESSORS
inline
int bael_CountingAllocator::numBytesTotal() const
{
    return d_byteCount;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
