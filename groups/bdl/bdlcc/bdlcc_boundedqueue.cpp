// bdlcc_boundedqueue.cpp                                             -*-C++-*-

#include <bdlcc_boundedqueue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlcc_boundedqueue_cpp,"$Id$$CSID$")

namespace BloombergLP {

///Implementation Note
///===================
// This component is implemented as a circular array of nodes.  A node stores
// the value in a 'bsls::ObjectBuffer<T>' and a boolean 'd_reclaim' used to
// handle exceptions while writing into the queue.  The next index to enqueue
// into is stored in 'd_pushIndex' and the next index to dequeue from is stored
// in 'd_popIndex'.  To ensure elements are not enqueued or dequeued prior to
// the completion of the previous operation, the queue waits for a quiescent
// state before marking elements as readable or writable.  Specifically,
// written elements are marked readable when there are no active writers (users
// of "push" methods) and read elements are marked writable when there are no
// active readers (users of "pop" methods).  The quiescent state occurs when
// the number of operations started equals the number of operations completed.
// This detection is accomplished using the private constants:
//   * 'k_STARTED_INC' to indicate an operation has started,
//   * 'k_FINISHED_INC' to indicate an operation has completed, and
//   * 'k_STARTED_MASK' and 'k_FINISHED_SHIFT' for determining quiescent state.

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
