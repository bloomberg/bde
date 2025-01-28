// bsls_fuzztest_testutil.cpp                                         -*-C++-*-

#include <bsls_fuzztest_testutil.h>

#include <bsls_preconditions.h>

namespace BloombergLP {
namespace bsls {

void FuzzTest_TestUtil::triggerAssert()
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_INVOKE("triggered an assertion violation");
    BSLS_PRECONDITIONS_END();
}

void FuzzTest_TestUtil::triggerReview()
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_REVIEW_INVOKE("triggered a review violation");
    BSLS_PRECONDITIONS_END();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
