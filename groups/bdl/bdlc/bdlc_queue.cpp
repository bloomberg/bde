// bdlc_queue.cpp                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlc_queue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlc_queue_cpp,"$Id$ $CSID$")

//
///IMPLEMENTATION NOTES
///--------------------
// This component implements a double-ended queue.  The queue is stored in
// memory as an array with 'd_front' indicating the position before the first
// element and 'd_back' the position after the last element; this implies two
// extra array positions are required beyond what is strictly necessary.  This
// technique was chosen since it avoids all ambiguities involving empty and
// full queues, and simplifies efficient implementation via symmetry.
//
// Since a queue stored in this manner is circular, there are two general
// scenarios to consider during every operation (the numbers in the cells are
// index positions):
//..
//                         +---+---+---+---+---+---+---+---+---+---+
//    d_front < d_back     |   |   | 0 | 1 | 2 | 3 |   |   |   |   |
//                         +---+---+---+---+---+---+---+---+---+---+
//                               ^                   ^
//                               |___ d_front        |___ d_back
//
//                         +---+---+---+---+---+---+---+---+---+---+
//    d_front > d_back     | 3 |   |   |   |   |   |   | 0 | 1 | 2 |
//                         +---+---+---+---+---+---+---+---+---+---+
//                               ^                   ^
//                               |___ d_back         |___ d_front
//..

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
