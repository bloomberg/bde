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
//   * writable ('e_WRITABLE'), and
//   * writable and the single consumer is waiting to read here
//         ('e_WRITABLE_AND_BLOCKED'),

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
