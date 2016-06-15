// bslstp_exfunctional.t.cpp                                          -*-C++-*-
#ifndef BDE_OPENSOURCE_PUBLICATION // STP

#include <bslstp_exfunctional.h>

#include <bslstl_pair.h>

#include <bslmf_assert.h>
#include <bslmf_istriviallycopyable.h>

typedef bsl::pair<int, int> pair_type;

BSLMF_ASSERT((bsl::is_trivially_copyable<bsl::identity<int> >::value));
BSLMF_ASSERT((bsl::is_trivially_copyable<bsl::select1st<pair_type> >::value));
BSLMF_ASSERT((bsl::is_trivially_copyable<bsl::select2nd<pair_type> >::value));
BSLMF_ASSERT((bsl::is_trivially_copyable<bsl::StringComparator>::value));

#endif  // BDE_OPENSOURCE_PUBLICATION -- STP

int main() { return -1; }   // empty test driver

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
