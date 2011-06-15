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
// 'bslma_TestAllocatorMonitor', that is used to summarize changes in three
// 'bslma_TestAllocator' statistics, relative to their values at the
// construction of the monitor.  The statistics are 'numBlocksInUse',
// 'numBlocksMax', and 'numBlocksTotal'.  For each of these statistics, this
// class provides a several predicate accessor methods: one reports 'true' if
// there is an increase in its value, the other reports 'true' if its value has
// not changed, and, if the static value might decrease over time, one that
// reports 'true' if its value has decreased. 
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

namespace BloombergLP {

                        // ================================
                        // class bslma_TestAllocatorMonitor
                        // ================================

class bslma_TestAllocatorMonitor {
    // TBD

    // DATA
    const int                        d_initialInUse;    // 'numBlocksInUse'
    const int                        d_initialMax;      // 'numBlocksMax'
    const int                        d_initialTotal;    // 'numBlocksTotal'
    const bslma_TestAllocator *const d_testAllocator_p; // held, not owned

  public:
    // CREATORS
    explicit bslma_TestAllocatorMonitor(const bslma_TestAllocator&
                                                                testAllocator);
        // Create a 'bslma_TestAllocatorMonitor' object to track changes in
        // statistics of the specified 'testAllocator'.

    // ACCESSORS
    bool isInUseDown() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked
        // test allocator has decreased since construction of this monitor,
        // and 'false' otherwise.

    bool isInUseSame() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked
        // test allocator has not changed since construction of this monitor,
        // and 'false' otherwise.

    bool isInUseUp() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked
        // test allocator has increased since construction of this monitor,
        // and 'false' otherwise.

    bool isMaxSame() const;
        // Return 'true' if the 'numBlocksMax' statistic of the tracked
        // test allocator has not changed since construction of this monitor,
        // and 'false' otherwise.

    bool isMaxUp() const;
        // Return 'true' if the 'numBlocksMax' statistic of the tracked
        // test allocator has increased since construction of this monitor,
        // and 'false' otherwise.

    bool isTotalSame() const;
        // Return 'true' if the 'numBlocksTotal' statistic of the tracked
        // test allocator has not changed since construction of this monitor,
        // and 'false' otherwise.

    bool isTotalUp() const;
        // Return 'true' if the 'numBlocksTotal' statistic of the tracked
        // test allocator has increased since construction of this monitor,
        // and 'false' otherwise.
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
