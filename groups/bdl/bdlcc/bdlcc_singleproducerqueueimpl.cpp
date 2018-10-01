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
// writable state.  While enqueueing, if the next node to the 'd_lastWritten'
// node is not writable, a new node is allocated and inserted into the circular
// linked list.  'd_numElements' is, essentially, part of a counting semphore
// used to ensure the consumers do not attempt to read from an unwritten node.
//
// Here is an illustration of the structure, with "R" denoting 'e_READABLE' and
// "W" denoting 'e_WRITABLE':
//..
//      d_nextRead        d_lastWritten       3 == d_numElements
//          |                   |
//          V                   V
//        +---+     +---+     +---+     +---+     +---+
//  ,-->  | R | --> | R | --> | R | --> | W | --> | W | --,
//  |     +---+     +---+     +---+     +---+     +---+   |
//  |                                                     |
//  '-----------------------------------------------------'
//..

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
