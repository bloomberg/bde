// bslma_sharedptrrep.cpp                                             -*-C++-*-
#include <bslma_sharedptrrep.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_default.h>                      // for testing only
#include <bslma_testallocator.h>                // for testing only

#include <bsls_assert.h>

namespace BloombergLP {
namespace bslma {
                          // ------------------
                          // class SharedPtrRep
                          // ------------------

// MANIPULATORS
void SharedPtrRep::acquireWeakRef()
{
    BSLS_ASSERT(0 < numWeakReferences() || 0 < numReferences());

    d_adjustedWeakCount.addRelaxed(2);          // minimum consistency: relaxed

    int sharedCount = d_adjustedSharedCount.loadRelaxed();
                                                // minimum consistency: relaxed
    while (!(sharedCount & 1)) {
        int temp = d_adjustedSharedCount.testAndSwap(sharedCount,
                                                     sharedCount | 1);
                                                // minimum consistency: relaxed
        if (temp == sharedCount) {
            return;                                                   // RETURN
        }
        sharedCount = temp;
    }
}

void SharedPtrRep::releaseRef()
{
    BSLS_ASSERT_SAFE(0 < numReferences());

    const int sharedCount = d_adjustedSharedCount.add(-2);
                                        // release consistency: acquire/release
    if (0 == sharedCount) {
        disposeObject();
        disposeRep();
    }
    else if (1 == sharedCount) {
        disposeObject();

        const int weakCount = d_adjustedWeakCount.add(-1);
                                        // release consistency: acquire/release
        if (0 == weakCount) {
            disposeRep();
        }
    }
}

void SharedPtrRep::resetCountsRaw(int numSharedReferences,
                                  int numWeakReferences)
{
    BSLS_ASSERT_SAFE(0 <= numSharedReferences);
    BSLS_ASSERT_SAFE(0 <= numWeakReferences);

    // These reference counts can be relaxed because access to this
    // 'SharedPtrRep' must be serialized when calling this function (as
    // specified in the function-level doc).

    d_adjustedSharedCount.storeRelaxed(2 * numSharedReferences
                                       + (numWeakReferences ? 1 : 0));
                                                // minimum consistency: relaxed

    d_adjustedWeakCount.storeRelaxed(2 * numWeakReferences
                                     + (numSharedReferences ? 1 : 0));
                                                // minimum consistency: relaxed
}

bool SharedPtrRep::tryAcquireRef()
{
    BSLS_ASSERT(0 < numWeakReferences() || 0 < numReferences());

    int sharedCount = d_adjustedSharedCount.loadRelaxed();
                                                // minimum consistency: relaxed
    while (sharedCount > 1) {
        int temp = d_adjustedSharedCount.testAndSwap(sharedCount,
                                                     sharedCount + 2);
                                        // release consistency: acquire/release
        if (temp == sharedCount) {
            return true;                                              // RETURN
        }
        sharedCount = temp;
    }
    return false;
}

}  // close package namespace
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
