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

                         // -------------------------
                         // class bcema_SharedPtrUtil
                         // -------------------------

// MANIPULATORS
bcema_SharedPtr<char>
bcema_SharedPtrUtil::createInplaceUninitializedBuffer(
                                               bsl::size_t      bufferSize,
                                               bslma_Allocator *basicAllocator)
{
    basicAllocator = bslma_Default::allocator(basicAllocator);
                                                       // allocator is optional

    // We have alignment concerns here: there are no alignment issues with
    // 'bcema_SharedPtrRep', but the buffer address (i.e., the address of
    // 'd_instance' in the 'bcema_SharedPtrInplaceRep' object) must be at
    // least *naturally* *aligned* to 'bufferSize'.  (See 'bslma' package
    // documentation for a definition of natural alignment.)  We achieve this
    // in the simplest way by always maximally aligning the returned pointer.

    typedef
        bcema_SharedPtrInplaceRep<bsls_AlignmentUtil::MaxAlignedType> Rep;

    enum{
        ALIGNMENT_MASK = ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
    };

    bsl::size_t repSize = (sizeof(Rep) + bufferSize - 1) & ALIGNMENT_MASK;

    Rep *rep = new (basicAllocator->allocate(repSize)) Rep(basicAllocator);

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
