// bdlcc_singleconsumerqueueimpl.cpp                                  -*-C++-*-

#include <bdlcc_singleconsumerqueueimpl.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlcc_singleconsumerqueueimpl_cpp,"$Id$$CSID$")

namespace BloombergLP {

///Implementation Note
///===================
// This component is implemented using a circular linked list of at least one
// node.  There is always one more node in the list than indicated by
// 'd_capacity' or indicated by the available attribute of 'd_state'.  Each
// node stores the next node, the state of the node (readable, writable,
// writable and the read thread is blocked waiting for the write, and the node
// needs to be reclaimed due to a prior thrown exception), and possibly a
// value.  While enqueueing, if the available attribute of 'd_state' indicates
// there are no available nodes, a new node is allocated and inserted into the
// circular linked list, but the pointed 'd_nextWrite' node is used to store
// the pushed value (since there is always an extra node than what is indicated
// by the available count).
//
// Here is an illustration of the structure, with "R" denoting 'e_READABLE' and
// "W" denoting 'e_WRITABLE':
//..
//      d_nextRead        d_lastWritten     2 == available count
//          |                   |
//          V                   V
//        +---+     +---+     +---+     +---+     +---+
//  ,-->  | R | --> | R | --> | W | --> | W | --> | W | --,
//  |     +---+     +---+     +---+     +---+     +---+   |
//  |                                                     |
//  '-----------------------------------------------------'
//..
//
// The state of the queue is stored in 'd_state' as a bit pattern that
// represents three attribute values:
//   * count of nodes available for use
//         ('(d_state & k_AVAILABLE_MASK) >> k_AVAILABLE_SHIFT'),
//   * count of threads attempting to use an available node (this value is not
//         accessed except as a check for zero or one ('d_state & k_USE_MASK')
//   * and number of threads allocating new nodes
//         ('d_state & k_ALLOCATE_MASK').
//
// The dequeueing of a node is governed by the state value in the node.
//
// The key difficulties in enqueueing elements are:
//   * deciding to use an existing node or allocate a new node,
//   * and inserting a new node.
// The approach to resolving these issues is to maintain an available count in
// 'd_state' and, when inserting, allow only one thread to have access to
// 'd_nextWrite'.  To accomplish the single access to 'd_nextWrite' goal, the
// count of the number of threads attempting to use and allocate nodes are
// tracked in 'd_state'.
//
// The available count is a semaphore-like count.  When the available count is
// greater than zero and there are no threads attempting to allocate, a push
// thread will be able to follow a fast execution path.  Otherwise, the thread
// will yield until the determination to allocate or use and existing node is
// possible.  To allocate, there must be no threads attempting to use a node.
// To use an existing node, there must be an available node and no threads
// attempting to allocate.  Note that, for this logic to work correctly, a
// fast-track thread must include itself as a thread using an existing node
// when initially modifying 'd_state'.
//
// When writing to a node, if an exception is thrown during the copy/move of
// the data, the node is considered "removed" from the available nodes by
// marking the node 'e_RECLAIM' and decreasing the capacity of the queue.  When
// a reader sees a 'e_RECLAIM' node, the node is returned to normal use, the
// capacity of the queue is increased, and the reader advances to the next
// node.
//
// The disabled states are tracked with 'd_popFrontDisabled' and
// 'd_pushBackDisabled'.  Both are a generation count plus one bit indicating
// if the queue is disabled for the related operation.  Given that the state
// modification methods can be called concurrently, some care must be taken in
// updating these values.  The generation count is used to ensure threads
// awoken by a disablement do not return to sleep if an enablement quickly
// follows.  This allows the user of the queue to quickly call
// 'disablePopFront' and then 'enablePopFront' to release all threads blocked
// on a dequeue operation.

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
