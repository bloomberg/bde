// bael_fixedsizerecordbuffer.cpp                -*-C++-*-
#include <bael_fixedsizerecordbuffer.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_fixedsizerecordbuffer_cpp,"$Id$ $CSID$")

#if 0
#include <bael_severity.h>
#include <bcemt_barrier.h>
#endif

namespace BloombergLP {

// CREATORS
bael_FixedSizeRecordBuffer::~bael_FixedSizeRecordBuffer()
{
    removeAll();
}

// MANIPULATORS
void bael_FixedSizeRecordBuffer::popBack()
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);

    d_currentTotalSize -=
        (d_deque.back())->numAllocatedBytes();

    d_currentTotalSize -=
        bsls_PlatformUtil::roundUpToMaximalAlignment(sizeof(bael_Record));

    d_deque.pop_back();
}

void bael_FixedSizeRecordBuffer::popFront()
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);

    d_currentTotalSize -=
        (d_deque.front())->numAllocatedBytes();

    d_currentTotalSize -=
        bsls_PlatformUtil::roundUpToMaximalAlignment(sizeof(bael_Record));

    d_deque.pop_front();
}

int bael_FixedSizeRecordBuffer::pushBack(
                         const bcema_SharedPtr<bael_Record>& handle)
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);


    int size = handle->numAllocatedBytes() +
            bsls_PlatformUtil::roundUpToMaximalAlignment(sizeof(bael_Record));

    if (size + d_allocator.numBytesTotal() > d_maxTotalSize) {
        // impossible to accommodate this record
        return -1;
    }

    int returnValue = 0;
    d_deque.push_back(handle); // This operation may cause 'd_deque' to grow,
                               // so test again.

    if (size + d_allocator.numBytesTotal() > d_maxTotalSize) { // impossible
                                                               // to
                                                               // accommodate
        d_deque.pop_back();
        returnValue = -1;
    }
    else { // possible to accommodate this record
        d_currentTotalSize += size;
    }


    while(d_currentTotalSize + d_allocator.numBytesTotal() > d_maxTotalSize) {
        d_currentTotalSize -=
            (d_deque.front())->numAllocatedBytes();
        d_currentTotalSize -=
            bsls_PlatformUtil::roundUpToMaximalAlignment(sizeof(bael_Record));
        d_deque.pop_front();
    }
    return returnValue;
}

int bael_FixedSizeRecordBuffer::pushFront(
                         const bcema_SharedPtr<bael_Record>& handle)
{
    bcemt_LockGuard<bcemt_RecursiveMutex> guard(&d_mutex);


    int size = handle->numAllocatedBytes() +
            bsls_PlatformUtil::roundUpToMaximalAlignment(sizeof(bael_Record));
    if (size + d_allocator.numBytesTotal() > d_maxTotalSize) {
        // impossible to accommodate this record
        return -1;
    }

    int returnValue = 0;
    d_deque.push_front(handle); // This operation may cause 'd_deque' to grow,
                                // so test again.

    if (size + d_allocator.numBytesTotal() > d_maxTotalSize) { // impossible
                                                               // to
                                                               // accommodate
        d_deque.pop_front();
        returnValue = -1;
    }
    else { // possible to accommodate this record
        d_currentTotalSize += size;
    }


    while(d_currentTotalSize + d_allocator.numBytesTotal() > d_maxTotalSize) {
        d_currentTotalSize -=
            (d_deque.back())->numAllocatedBytes();
        d_currentTotalSize -=
            bsls_PlatformUtil::roundUpToMaximalAlignment(sizeof(bael_Record));
        d_deque.pop_back();
    }
    return returnValue;
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
