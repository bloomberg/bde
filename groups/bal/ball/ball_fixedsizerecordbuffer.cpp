// ball_fixedsizerecordbuffer.cpp                -*-C++-*-
#include <ball_fixedsizerecordbuffer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_fixedsizerecordbuffer_cpp,"$Id$ $CSID$")

#include <bsls_alignmentutil.h>

#if 0
#include <ball_severity.h>
#include <bdlmtt_barrier.h>
#endif

namespace BloombergLP {

namespace ball {
// CREATORS
FixedSizeRecordBuffer::~FixedSizeRecordBuffer()
{
    removeAll();
}

// MANIPULATORS
void FixedSizeRecordBuffer::popBack()
{
    bdlmtt::LockGuard<bdlmtt::RecursiveMutex> guard(&d_mutex);

    d_currentTotalSize -=
        (d_deque.back())->numAllocatedBytes();

    d_currentTotalSize -=
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));

    d_deque.pop_back();
}

void FixedSizeRecordBuffer::popFront()
{
    bdlmtt::LockGuard<bdlmtt::RecursiveMutex> guard(&d_mutex);

    d_currentTotalSize -=
        (d_deque.front())->numAllocatedBytes();

    d_currentTotalSize -=
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));

    d_deque.pop_front();
}

int FixedSizeRecordBuffer::pushBack(
                         const bsl::shared_ptr<Record>& handle)
{
    bdlmtt::LockGuard<bdlmtt::RecursiveMutex> guard(&d_mutex);


    int size = handle->numAllocatedBytes() +
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));

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
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));
        d_deque.pop_front();
    }
    return returnValue;
}

int FixedSizeRecordBuffer::pushFront(
                         const bsl::shared_ptr<Record>& handle)
{
    bdlmtt::LockGuard<bdlmtt::RecursiveMutex> guard(&d_mutex);


    int size = handle->numAllocatedBytes() +
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));
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
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));
        d_deque.pop_back();
    }
    return returnValue;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
