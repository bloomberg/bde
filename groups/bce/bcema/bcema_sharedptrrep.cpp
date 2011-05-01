// bcema_sharedptrrep.cpp                                             -*-C++-*-
#include <bcema_sharedptrrep.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_sharedptrrep_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {

                          // ------------------------
                          // class bcema_SharedPtrRep
                          // ------------------------

// MANIPULATORS
void bcema_SharedPtrRep::acquireWeakRef()
{
    BSLS_ASSERT(0 < numWeakReferences() || 0 < numReferences());

    d_adjustedWeakCount.relaxedAdd(2);          // minimum consistency: relaxed

    int sharedCount = d_adjustedSharedCount.relaxedLoad();
                                                // minimum consistency: relaxed
    while (!(sharedCount & 1)) {
        int temp = d_adjustedSharedCount.testAndSwap(sharedCount,
                                                     sharedCount | 1);
                                                // minimum consistency: relaxed
        if (temp == sharedCount) {
            return;                                                   // RETURN
        }
        sharedCount = temp;
    }
}

void bcema_SharedPtrRep::releaseRef()
{
    BSLS_ASSERT_SAFE(0 < numReferences());

    const int sharedCount = d_adjustedSharedCount.add(-2);
                                        // release consistency: acquire/release
    if (0 == sharedCount) {
        disposeObject();
        disposeRep();
    }
    else if (1 == sharedCount) {
        disposeObject();

        const int weakCount = d_adjustedWeakCount.add(-1);
                                        // release consistency: acquire/release
        if (0 == weakCount) {
            disposeRep();
        }
    }
}

void bcema_SharedPtrRep::resetCountsRaw(int numSharedReferences,
                                        int numWeakReferences)
{
    BSLS_ASSERT_SAFE(0 <= numSharedReferences);
    BSLS_ASSERT_SAFE(0 <= numWeakReferences);

    // These reference counts can be relaxed because access to this
    // 'bcema_SharedPtrRep' must be serialized when calling this function (as
    // specified in the function-level doc).

    d_adjustedSharedCount.relaxedStore(2 * numSharedReferences
                                       + (numWeakReferences ? 1 : 0));
                                                // minimum consistency: relaxed

    d_adjustedWeakCount.relaxedStore(2 * numWeakReferences
                                     + (numSharedReferences ? 1 : 0));
                                                // minimum consistency: relaxed
}

bool bcema_SharedPtrRep::tryAcquireRef()
{
    BSLS_ASSERT(0 < numWeakReferences() || 0 < numReferences());

    int sharedCount = d_adjustedSharedCount.relaxedLoad();
                                                // minimum consistency: relaxed
    while (sharedCount > 1) {
        int temp = d_adjustedSharedCount.testAndSwap(sharedCount,
                                                     sharedCount + 2);
                                        // release consistency: acquire/release
        if (temp == sharedCount) {
            return true;                                              // RETURN
        }
        sharedCount = temp;
    }
    return false;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
