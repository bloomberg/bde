// bslma_testallocatormonitor.h                                       -*-C++-*-
#ifndef INCLUDED_BSLMA_TESTALLOCATORMONITOR
#define INCLUDED_BSLMA_TESTALLOCATORMONITOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to summarize 'bslma_TestAllocator' object use.
//
//@CLASSES:
//  bslma_TestAllocatorMonitor: summarizes 'bslma_TestAllocator' use
//
//@AUTHOR: Rohan Bhindwale (rbhindwale), Steven Breitstein (sbreitstein)
//
//@SEE_ALSO: bslma_testallocator
//
//@DESCRIPTION: This component provides a single mechanism class,
// 'bslma_TestAllocatorMonitor', which is used, in concert with
// 'bslma_TestAllocator', in the implementation of test drivers.  The
// 'bslma_TestAllocatorMonitor' class provides several tell-tale indicators
// (accessors) that certain states (i.e., statistics) of the test allocator
// have changed (or not) since constuction of the monitor.  Using
// 'bslma_TestAllocatorMonitor' objects results test cases that more concise,
// easier to read, and less error prone than test cases that directly access
// the test allocator for state information.
//
///Statistics
/// - - - - -
// The test allocator statistics tracked and the corresponding test allocator
// monitor (Boolean) accessor methods are shown in the table below.  The change
// (or lack of change) reported by these accessors are relative the the value
// of each statistic on construction of the monitor.  Note that each of these
// statistics count blocks of memory (i.e., number of allocations from the
// allocator), and do not depend on the number of bytes in those allocated
// blocks.
//..
//  Statistic        Is-Same Method Is-Up Method Is-Down Method
//  --------------   -------------- ------------ --------------
//  numBlocksInUse   isInUseSame    isInUseUp    isInUseDown
//  numBlocksMax     isMaxSame      isMaxUp      none
//  numBlocksTotal   isTotalSame    isTotalUp    none
//..
// The 'numBlocksMax' and 'numBlocksTotal' statistics have values that are
// monotonically non-decreasing; hence, they have no "Is-Down" methods.  If a
// monitor is created for an allocator with outstanding blocks, then method
// there are scenarios of deallocations and allocations where the number of
// outstanding blocks drops below the value at monitor creation (i.e.,
// 'true == monitor.isInUseDown()').
//
///Usage
///-----
// See 'bslma_testallocatormonitor.t.cpp' usage case for now.

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

namespace BloombergLP {

                        // ================================
                        // class bslma_TestAllocatorMonitor
                        // ================================

class bslma_TestAllocatorMonitor {
    // This mechanism provides a set of accessor methods providing simple, tell
    // tale indications of a change (or of no change), since construction of
    // the monitor, in the state (statics) of the 'bslma_TestAllocator' object
    // provided at construction.  See the Statics section of @DESCRIPTION for
    // the statics tracked.

    // DATA
    const bsls_Types::Int64          d_initialInUse;    // 'numBlocksInUse'
    const bsls_Types::Int64          d_initialMax;      // 'numBlocksMax'
    const bsls_Types::Int64          d_initialTotal;    // 'numBlocksTotal'
    const bslma_TestAllocator *const d_testAllocator_p; // held, not owned

  public:
    // CREATORS
    explicit bslma_TestAllocatorMonitor(const bslma_TestAllocator&
                                                                testAllocator);
        // Create a 'bslma_TestAllocatorMonitor' object to track changes in
        // statistics of the specified 'testAllocator'.

    //! ~bslma_TestAllocatorMonitor() = default;
        // Destroy this object.

    // ACCESSORS
    bool isInUseDown() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has decreased since construction of this monitor, and
        // 'false' otherwise.

    bool isInUseSame() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isInUseUp() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.

    bool isMaxSame() const;
        // Return 'true' if the 'numBlocksMax' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isMaxUp() const;
        // Return 'true' if the 'numBlocksMax' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.

    bool isTotalSame() const;
        // Return 'true' if the 'numBlocksTotal' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isTotalUp() const;
        // Return 'true' if the 'numBlocksTotal' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------------------
                        // class bslma_TestAllocatorMonitor
                        // --------------------------------

// CREATORS
inline
bslma_TestAllocatorMonitor::bslma_TestAllocatorMonitor(
                                      const bslma_TestAllocator& testAllocator)
: d_initialInUse(testAllocator.numBlocksInUse())
, d_initialMax(testAllocator.numBlocksMax())
, d_initialTotal(testAllocator.numBlocksTotal())
, d_testAllocator_p(&testAllocator)
{
}

// ACCESSORS
inline
bool bslma_TestAllocatorMonitor::isInUseDown() const
{
    return d_initialInUse > d_testAllocator_p->numBlocksInUse();
}

inline
bool bslma_TestAllocatorMonitor::isInUseSame() const
{
    return d_initialInUse == d_testAllocator_p->numBlocksInUse();
}

inline
bool bslma_TestAllocatorMonitor::isInUseUp() const
{
    return d_initialInUse < d_testAllocator_p->numBlocksInUse();
}

inline
bool bslma_TestAllocatorMonitor::isMaxSame() const
{
    return d_initialMax == d_testAllocator_p->numBlocksMax();
}

inline
bool bslma_TestAllocatorMonitor::isMaxUp() const
{
    return d_initialMax != d_testAllocator_p->numBlocksMax();
}

inline
bool bslma_TestAllocatorMonitor::isTotalSame() const
{
    return d_initialTotal == d_testAllocator_p->numBlocksTotal();
}

inline
bool bslma_TestAllocatorMonitor::isTotalUp() const
{
    return d_initialTotal != d_testAllocator_p->numBlocksTotal();
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
