// bslalg_dequeprimitives.cpp                                         -*-C++-*-
#include <bslalg_dequeprimitives.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//
// IMPLEMENTATION NOTES:
//
// In case of aliasing, we make a copy of the object through a constructor
// proxy.  This is because the copy constructor might take more than one
// argument.  We cannot just check whether the range is between 'fromEnd' and
// 'position' because there's no way of checking whether 'value' is located
// inside a 'bslstl_Deque' or not.


namespace BloombergLP {

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
