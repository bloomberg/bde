// ball_countingallocator.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_COUNTINGALLOCATOR
#define INCLUDED_BALL_COUNTINGALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concrete allocator keep a count of allocated bytes.
//
//@CLASSES:
//  ball::CountingAllocator: maximally-aligning, instrumented allocator adaptor
//
//@SEE_ALSO: ball_fixedsizerecordbuffer
//
//@DESCRIPTION: This component provides a special-purpose instrumented
// allocator, 'ball::CountingAllocator', that implements the 'bslma::Allocator'
// protocol and guarantees maximal alignment of allocated blocks, even when
// the allocator supplied at construction guarantees only natural alignment
// (or no alignment at all).  'ball::CountingAllocator' maintains a
// user-resettable running sum of the total number of bytes
// allocated (called byte count, see below).
//..
//   ,----------------------.
//  ( ball::CountingAllocator )
//   `----------------------'
//               |         ctor/dtor
//               |         numBytesTotal
//               |         resetNumBytesTotal
//               V
//       ,-----------------.
//      ( bslma::Allocator )
//       `-----------------'
//                       allocate
//                       deallocate
//..
//
///Byte Count
///----------
// The byte count maintained by 'ball::CountingAllocator' is set to 0 upon
// construction and after each call to 'resetNumBytesTotal'.  Each call of
// 'allocate(size)' increases the byte count by the sum of the least
// multiple of 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT' that is greater than
// or equal to 'size' and 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'.  Each call
// of 'deallocate' decrements the byte count by the same amount by which the
// byte count was incremented on matching 'allocate' call.  The current
// value of the byte count is returned by the 'numBytesTotal' accessor.
//
///Usage
///-----
// In the following example we demonstrate how the counting allocator can
// be used to know the amount of dynamic memory allocated by a
// 'vector<int>' after pushing one integer.  Let us assume that memory for
// the vector comes from a 'bslma::Allocator' named 'allocator'.
//..
//    // First create the counting allocator using 'allocator'.
//    BloombergLP::ball::CountingAllocator countingAllocator(allocator);
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
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



namespace BloombergLP {

namespace ball {
                        // ============================
                        // class CountingAllocator
                        // ============================

class CountingAllocator : public bslma::Allocator {
    // This class maintains a count of the total number of allocated bytes.
    // The running byte count is initialized to 0 upon construction, is
    // increased by the 'allocate' method, and may be reset to 0 by the
    // 'resetNumBytesTotal' method.  The 'deallocate' method appropriately
    // decrement the byte count.  The precise definition of byte count is
    // described in the "Byte Count" section of the component-level
    // documentation.

    // DATA
    int               d_byteCount;    // byte count
    bslma::Allocator *d_allocator_p;  // holds (but does not own) allocator

    // NOT IMPLEMENTED
    CountingAllocator(const CountingAllocator&);
    CountingAllocator& operator=(const CountingAllocator&);

  public:
    // CREATORS
    CountingAllocator(bslma::Allocator *basicAllocator = 0);
        // Create a counting allocator having an initial byte count of 0.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    virtual ~CountingAllocator();
        // Destroy this counting allocator.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (bytes) and increment the byte count maintained by
        // this counting allocator by the sum of the least multiple of
        // 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT' that is greater than or
        // equal to 'size' and 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'.
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
CountingAllocator::CountingAllocator(
                                              bslma::Allocator *basicAllocator)
: d_byteCount(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
inline
void *CountingAllocator::allocate(size_type size)
{
    size_type paddedSize =
                          bsls::AlignmentUtil::roundUpToMaximalAlignment(size);
    size_type totalSize = paddedSize + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    d_byteCount += totalSize;
    void *address = d_allocator_p->allocate(totalSize);
    *((int *)address) = static_cast<int>(totalSize);
    return (char *)address + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
}

inline
void CountingAllocator::deallocate(void *address)
{
    if (address) {
        address = (char *)address - bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
        d_byteCount -= *((int *)address);
        d_allocator_p->deallocate(address);
    }
}

inline
void CountingAllocator::resetNumBytesTotal()
{
    d_byteCount = 0;
}

// ACCESSORS
inline
int CountingAllocator::numBytesTotal() const
{
    return d_byteCount;
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
