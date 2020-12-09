// bdlcc_singleproducersingleconsumerboundedqueue.cpp                 -*-C++-*-

#include <bdlcc_singleproducersingleconsumerboundedqueue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlcc_singleproducersingleconsumerboundedqueue_cpp,
                 "$Id$$CSID$")

namespace BloombergLP {

///Implementation Note
///===================
// This component is implemented using a ring buffer data structure.  A ring
// buffer is a fixed size buffer that logically wraps around itself.  It is the
// ideal container for a fixed sized queue, since this structure imposes a
// strict upper bound on it's internal capacity.
//
// Here is an illustration representing a ring buffer that can hold at most
// twenty items at any instant.
//..
// +---------------------------------------------------------------------+
// | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9| 10| 11| 12| 13| 14| 15| 16| 17| 18| 19|
// +---------------------------------------------------------------------+
//                                 |
//                                 |
//                                 V
//                            +-------+
//                        +---|  9| 10|---+
//                    +---|  8|---+---| 11|---+
//                    |  7|---+       +---| 12|
//                   +----                +------+
//                   |  6|                   | 13|
//                  +----+                   +----+
//                  |  5|                     | 14|
//                  +---+                     +---+
//                  |  4|                     | 15|
//                  +----+                    ----+
//                   |  3|                   | 16|
//                   +-----               +------+
//                    |  2|---+       +---| 17|
//                    +---|  1|---+---| 18|---+
//                        +---|  0| 19|---+
//                            +-------+
//..
// This component defines an implementation of a ring buffer that allows
// concurrent access from a single producer and a single consumer thread.  The
// component was designed to minimize contention between these threads.
//
// The state for the single producer is primarily 'd_pushIndex', that stores
// the index of the next enqueue location.  Likewise, 'd_popIndex' stores the
// index of the next dequeue location.  Furthermore, within the ring buffer,
// the state of the element is maintained:
//   * readable ('e_READABLE'),
//   * readable and the single producer is waiting to write here
//         ('e_READABLE_AND_BLOCKED'),
//   * writable ('e_WRITABLE'),
//   * *first* writable node and the queue is empty
//         ('e_WRITABLE_AND_EMPTY'), and
//   * *first* writable node, queue is empty, and the single consumer is
//         waiting to read here ('e_WRITABLE_AND_BLOCKED').
//
// Finally, a generation count for the number of times this queue has been
// empty is maintained ('d_emptyGeneration').  This count is incremented
// whenever the queue becomes empty and whenever the queue leaves the empty
// state.  When this count is even, the queue is empty.  When this count is
// odd, the queue is not empty.  Note that the consuming thread and producing
// thread may both increment this value concurrently, and it is the combination
// of the consumer marking the queue as empty by setting a node's state *to*
// 'e_WRITABLE_AND_EMPTY' (and incrementing 'd_emptyGeneration') and the
// producer marking the queue as no longer empty by setting a node's state
// *from* 'e_WRITABLE_AND_EMPTY' (and incrementing 'd_emptyGeneration') that
// together form an "empty generation" for this queue (as a combined increment
// of 2).

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
