// bslmt_fastpostsemaphoreimpl.cpp                                    -*-C++-*-

#include <bslmt_fastpostsemaphoreimpl.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_fastpostsemaphoreimpl_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bslmt {

///Implementation Note
///===================
// Typical implementations of a counting semaphore maintain an available count,
// defined as the total amount of the resource available less the total number
// of threads wanting the resource.  This resultant value is the amount of the
// resource available when non-negative, and the negative of the number of
// threads waiting for additional resource.  As such, the 'post' implementation
// has to signal whenever the original value of available count is negative.
//
// This implementation minimizes the number of times 'post' signals.  To
// accomplish this goal, the number of blocked threads will be added to the
// available count (undoing the initial subtraction at the start of a 'wait'
// operation) and tracked separately, allowing 'post' to signal only when there
// are blocked threads and the count of available resources was zero prior to
// the invocation of 'post'.  In all other scenarios, there is another thread
// capable of signalling (typically in a 'wait' operation).  The following
// table depicts some (possible transcient) scenarios where, to make the
// correct signal decision, the new state representation is needed (note that
// the values are sampled near the end of the 'post' method, while making the
// decision to signal):
//..
//  # Post  # Wait  Typical Available  Available  Blocked  Signal in Post?
//  ------  ------  -----------------  ---------  -------  ---------------
//     2        2           0             0         0            no
//     2        2           0             1         1           YES
//     2        2           0             2         2            no
//
//     5        3           2             2         0            no
//     5        3           2             3         1            no
//     5        3           2             4         2            no
//     5        3           2             5         3            no
//
//     3        5          -2            -2         0            no
//     3        5          -2            -1         1            no
//     3        5          -2             0         2            no
//     3        5          -2             1         3           YES
//     3        5          -2             2         4            no
//     3        5          -2             3         5            no
//..
//
// The state of the semaphore is stored in 'd_state' as a bit pattern that
// represents three attribute values:
//: o count of available ('(d_state & k_AVAILABLE_MASK) >> k_AVAILABLE_SHIFT'),
//: o disabled generation ('d_state & k_DISABLED_GEN_MASK'), and
//: o count of blocked threads ('d_state & k_BLOCKED_MASK').
//
// The count of available is the semaphore's count; the number of wait requests
// to allow to acquire the semaphore.
//
// The disabled generation is used to track the disabled state of the semaphore
// with the intent of rapid disable/enable requests not causing the disable to
// be ineffective.  Specifically, a rapid disable and enable should release all
// blocked threads.  Odd values in this field indicate the semaphore is
// disabled.
//
// The count of blocked threads is used, in part, to determine if a thread
// should block after decrementing the available count (see
// 'willHaveBlockedThread').

}  // close package namespace
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
