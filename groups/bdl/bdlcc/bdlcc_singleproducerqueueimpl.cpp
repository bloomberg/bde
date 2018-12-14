// bdlcc_singleproducerqueueimpl.cpp                                  -*-C++-*-

#include <bdlcc_singleproducerqueueimpl.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlcc_singleproducerqueueimpl_cpp,"$Id$$CSID$")

namespace BloombergLP {

///Implementation Note
///===================
// This component is implemented using a circular linked list of at least two
// nodes.  Each node stores the next node, the state of the node (readable or
// writable), and possibly a value.  Writing is restricted to nodes in the
// writable state.  While enqueueing, if the next node to the 'd_nextWrite'
// node is not writable, a new node is allocated and inserted into the circular
// linked list.  'd_numElements' is, essentially, part of a counting semphore
// used to ensure the consumers do not attempt to read from an unwritten node.
//
// Here is an illustration of the structure, with "R" denoting 'e_READABLE' and
// "W" denoting 'e_WRITABLE':
//..
//      d_nextRead        d_nextWrite       2 == d_numElements
//          |                   |
//          V                   V
//        +---+     +---+     +---+     +---+     +---+
//  ,-->  | R | --> | R | --> | W | --> | W | --> | W | --,
//  |     +---+     +---+     +---+     +---+     +---+   |
//  |                                                     |
//  '-----------------------------------------------------'
//..
//
// The state of the queue is stored in 'd_state' as a bit pattern which
// represents two values:
//   * available count, and
//   * number of blocked pop threads.
// The available count is a semaphore-like count.  When the available count is
// greater than the number of blocked pop threads, a new pop thread will be
// able to follow a fast execution path.
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
