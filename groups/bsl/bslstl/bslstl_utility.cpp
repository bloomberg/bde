// bslstl_utility.cpp                                                 -*-C++-*-
#include <bslstl_utility.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///Implementation Notes
///--------------------
// The C++03 implementation of `bsl::exchange` is trickier than it appears.
// Suppose the first overload were implemented roughly as follows:
// ```
// template <class T, class U>
// T exchange(T& lhs, const U& rhs)
// {
//     T tmp = move(lhs);
//     lhs = rhs;
//     return tmp;
// }
// ```
// In this case, we would get named return value optimization (NRVO) when the
// build configuration supports it, but when NRVO is not in effect, the return
// value would be copy-constructed from `tmp`, resulting in the allocator not
// being propagated from `lhs`.  On the other hand, an explicit move in the
// return statement would always prevent NRVO, pessimizing the common case
// where NRVO is supported.
//
// To solve this problem, we construct the return value immediately, thus
// eliminating `tmp` entirely, and then overwrite `lhs`.  To make code run
// after the return value is constructed, we have to put it in the destructor
// of a local variable.

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
