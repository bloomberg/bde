// bdlcc_stripedunorderedcontainerimpl.cpp                            -*-C++-*-
#include <bdlcc_stripedunorderedcontainerimpl.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlcc_stripedunorderedcontainerimpl,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// Rehashing requires locking all the stripes and reallocating the entire hash
// table, but the stripes are locked one at a time.
//
// It is implemented as a vector of custom lists for the elements.
//
// The locks are kept in an array, as a vector requires copy constructor.
//
// Rehashing can be disabled or enabled dynamically.
//
// Rehashing requires locking all the stripes and reallocating the entire hash
// table, but the stripes are locked one at a time.
//
// The number of stripes must not be bigger than the number of buckets.

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
