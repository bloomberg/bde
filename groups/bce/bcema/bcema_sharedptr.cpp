// bcema_sharedptr.cpp               -*-C++-*-
#include <bcema_sharedptr.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_sharedptr_cpp,"$Id$ $CSID$")

#include <bcema_testallocator.h>   // for testing only

#include <bsls_alignmentutil.h>

namespace BloombergLP {

                          // ------------------------
                          // class bcema_SharedPtrRep
                          // ------------------------

// CLASS METHODS
void bcema_SharedPtrRep::managedPtrDeleter(void *, bcema_SharedPtrRep *rep)
{
    if (!rep->decrementRefs()) {
        rep->release();
    }
}

                         // -------------------------
                         // class bcema_SharedPtrUtil
                         // -------------------------

// MANIPULATORS
bcema_SharedPtr<char>
bcema_SharedPtrUtil::
    createInplaceUninitializedBuffer(bsl::size_t      bufferSize,
                                     bslma_Allocator *allocator)
{
    // Allocator is optional.
    allocator = bslma_Default::allocator(allocator);

    // We have alignment problems here: no alignment issues with
    // 'bcema_SharedPtrRep', but the buffer address (i.e., the address
    // of 'd_instance' in the 'bcema_SharedPtr_InplaceRepImpl' object) must be
    // at least *naturally* *aligned* to 'bufferSize'.  (See 'bdema' package
    // documentation for a definition of natural alignment.)  We achieve this
    // in the simplest way by always maximally aligning the returned pointer.

    typedef
        bcema_SharedPtr_InplaceRepImpl<bsls_AlignmentUtil::MaxAlignedType> Rep;

    enum{
        ALIGNMENT_MASK = ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
    };

    // Compute required size, rounded up to the next multiple of 'ALIGNMENT'.
    // Note that 'ALIGNMENT' is a power of two, hence rounding up can be made
    // more efficient than the usual:
    //..
    //  '((x + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT
    //..
    bsl::size_t repSize = (sizeof(Rep) + bufferSize - 1) & ALIGNMENT_MASK;

    Rep *rep = new(allocator->allocate(repSize)) Rep(allocator);

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
