// bdlf_bind_test.cpp                                                 -*-C++-*-
#include <bdlf_bind_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlf_bind_test_cpp,"$Id$ $CSID$")

#include <bdlf_placeholder.h>    // for testing only

namespace BloombergLP {

namespace bdlf {
                         // -------------------------
                         // class Bind_TestSlotsAlloc
                         // -------------------------

// CLASS METHODS
bool Bind_TestSlotsAlloc::verifySlots(
        const bslma::Allocator* const *EXPECTED,
        bool                           verboseFlag)
{
    bool equalFlag = true;
#if !defined(BSLS_PLATFORM_CMP_MSVC)
    // Note: the calls to 'verifyAllocSlots' are disabled on Windows.  Their
    // success depends on the "Return Value Optimization" (RVO) which Windows
    // does not seem to be applying.

    for (int i = 0; i < k_NUM_SLOTS; ++i) {
        if (EXPECTED[i] != getSlot(i)) {
            equalFlag = false;
            break;
        }
    }
#endif // !defined(BSLS_PLATFORM_CMP_MSVC)

    if (verboseFlag || !equalFlag) {
        bsl::printf("\tAllocSlots:");
        for (int i = 0; i < k_NUM_SLOTS; ++i) {
            bsl::printf(" %s", (getSlot(i) == getZ0()) ? "Z0"
                            : ((getSlot(i) == getZ1()) ? "Z1"
                            : ((getSlot(i) == getZ2()) ? "Z2"
                            : "Z?")));
        }
        bsl::printf("\n");
    }

    return equalFlag;
}

}  // close package namespace
}  // close enterprise namespace

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
