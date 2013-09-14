// bslma_sharedptrrep.cpp                                             -*-C++-*-
#include <bslma_sharedptrrep.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

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

}  // close namespace bslma
}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
