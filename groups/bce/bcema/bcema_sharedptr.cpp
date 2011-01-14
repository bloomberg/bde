// bcema_sharedptr.cpp                                                -*-C++-*-
#include <bcema_sharedptr.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_sharedptr_cpp,"$Id$ $CSID$")

#include <bcema_testallocator.h>   // for testing only

#include <bsls_alignmentutil.h>

///IMPLEMENTATION NOTES
///--------------------
// The following expression is a class invariant of 'bcema_SharedPtr' and shall
// always be 'true':
//..
//  !d_rep_p || d_ptr_p
//..

namespace BloombergLP {

                          // ------------------------
                          // class bcema_SharedPtrRep
                          // ------------------------

// MANIPULATORS
void bcema_SharedPtrRep::acquireWeakRef()
{
    BSLS_ASSERT(0 < numWeakReferences() || 0 < numReferences());

    d_adjustedWeakCount.relaxedAdd(2);                               // relaxed

    int sharedCount = d_adjustedSharedCount.relaxedLoad();           // relaxed
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

    const int sharedCount = d_adjustedSharedCount.add(-2);   // acquire/release
    if (0 == sharedCount) {
        disposeObject();
        disposeRep();
    }
    else if (1 == sharedCount) {
        disposeObject();

        const int weakCount = d_adjustedWeakCount.add(-1);   // acquire/release
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
                                      + (numWeakReferences ? 1 : 0));// relaxed
    d_adjustedWeakCount.relaxedStore(2 * numWeakReferences
                                    + (numSharedReferences ? 1 : 0));// relaxed
}

bool bcema_SharedPtrRep::tryAcquireRef()
{
    BSLS_ASSERT(0 < numWeakReferences() || 0 < numReferences());

    int sharedCount = d_adjustedSharedCount.relaxedLoad();           // relaxed
    while (sharedCount > 1) {
        int temp = d_adjustedSharedCount.testAndSwap(sharedCount,
                                                     sharedCount + 2);
                                                             // acquire/release
        if (temp == sharedCount) {
            return true;                                              // RETURN
        }
        sharedCount = temp;
    }
    return false;
}

                         // -------------------------
                         // class bcema_SharedPtrUtil
                         // -------------------------

// MANIPULATORS
bcema_SharedPtr<char>
bcema_SharedPtrUtil::createInplaceUninitializedBuffer(
                                                   bsl::size_t      bufferSize,
                                                   bslma_Allocator *allocator)
{
    allocator = bslma_Default::allocator(allocator);  // allocator is optional

    // We have alignment concerns here: there are no alignment issues with
    // 'bcema_SharedPtrRep', but the buffer address (i.e., the address
    // of 'd_instance' in the 'bcema_SharedPtr_InplaceRepImpl' object) must be
    // at least *naturally* *aligned* to 'bufferSize'.  (See 'bslma' package
    // documentation for a definition of natural alignment.)  We achieve this
    // in the simplest way by always maximally aligning the returned pointer.

    typedef
        bcema_SharedPtr_InplaceRepImpl<bsls_AlignmentUtil::MaxAlignedType> Rep;

    enum{
        ALIGNMENT_MASK = ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
    };

    bsl::size_t repSize = (sizeof(Rep) + bufferSize - 1) & ALIGNMENT_MASK;

    Rep *rep = new (allocator->allocate(repSize)) Rep(allocator);

    return bcema_SharedPtr<char>((char *)rep->ptr(), rep);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
