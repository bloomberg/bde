// ball_fixedsizerecordbuffer.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_fixedsizerecordbuffer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_fixedsizerecordbuffer_cpp,"$Id$ $CSID$")

#include <bsls_alignmentutil.h>

#include <bslmt_lockguard.h>
#include <bslmt_recursivemutex.h>

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
    bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);

    d_currentTotalSize -=
        (d_deque.back())->numAllocatedBytes();

    d_currentTotalSize -=
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));

    d_deque.pop_back();
}

void FixedSizeRecordBuffer::popFront()
{
    bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);

    d_currentTotalSize -=
        (d_deque.front())->numAllocatedBytes();

    d_currentTotalSize -=
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));

    d_deque.pop_front();
}

int FixedSizeRecordBuffer::pushBack(
                         const bsl::shared_ptr<Record>& handle)
{
    bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);


    int size = handle->numAllocatedBytes() +
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));

    if (size + d_allocator.numBytesTotal() > d_maxTotalSize) {
        // impossible to accommodate this record
        return -1;                                                    // RETURN
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
    bslmt::LockGuard<bslmt::RecursiveMutex> guard(&d_mutex);


    int size = handle->numAllocatedBytes() +
           bsls::AlignmentUtil::roundUpToMaximalAlignment(sizeof(Record));
    if (size + d_allocator.numBytesTotal() > d_maxTotalSize) {
        // impossible to accommodate this record
        return -1;                                                    // RETURN
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

}  // close enterprise namespace

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
