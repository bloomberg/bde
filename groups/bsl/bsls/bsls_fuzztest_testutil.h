// bsls_fuzztest_testutil.h                                           -*-C++-*-
#ifndef INCLUDED_BSLS_FUZZTEST_TESTUTIL
#define INCLUDED_BSLS_FUZZTEST_TESTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for `bsls_fuzztest` component.
//
//@DESCRIPTION: The `bsls_fuzztest_testutil` component provides utilities to
// facilitate testing the `bsls_fuzztest` component.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Basic Usage of Functions
///- - - - - - - - - - - - - - - - -
// First, we install an `AssertTestHandlerGuard` to catch the violation.
// ```
//  bsls::AssertTestHandlerGuard g;
// ```
// Then, we trigger an `AssertViolation` by calling `triggerAssert` inside
// `ASSERT_FAIL` and verify that a violation occurs as expected.
// ```
//  ASSERT_FAIL(bsls::FuzzTest_TestUtil::triggerAssert());
// ```
// Next, we trigger a `ReviewViolation` by calling `triggerReview` inside
// `ASSERT_FAIL` and verify that a violation occurs as expected.
// ```
//  ASSERT_FAIL(bsls::FuzzTest_TestUtil::triggerReview());
// ```

#include <bsls_assert.h>
#include <bsls_review.h>

namespace BloombergLP {
namespace bsls {

                      // =======================
                      // class FuzzTest_TestUtil
                      // =======================

/// This utility class provides sample functions to demonstrate an
/// `AssertViolation` or a `ReviewViolation` originating from a different
/// component to facilitate testing of `bsls_fuzztest` component.
struct FuzzTest_TestUtil {

  public:
    // CLASS METHODS

    /// Trigger an `AssertViolation` by calling `BSLS_ASSERT_INVOKE`.
    static void triggerAssert();

    /// Trigger a `ReviewViolation` by calling `BSLS_REVIEW_INVOKE`.
    static void triggerReview();
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
