// bsls_bslonce.cpp                                                   -*-C++-*-
#include <bsls_bslonce.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bsls_bslonce_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_asserttest.h>   // for testing only
#include <bsls_bsltestutil.h>  // for testing only

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#else
#include <sched.h>  // sched_yield
#endif

namespace BloombergLP {
namespace bsls {

namespace {

static void yield()
    // Schedule another thread to run.
{
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    ::SleepEx(0, 0);
#else
    sched_yield();
#endif
}

}  // close unnamed namespace

                        // -------------
                        // class BslOnce
                        // -------------

// MANIPULATORS
bool BslOnce::doEnter()
{
    int state = bsls::AtomicOperations::testAndSwapInt(&d_onceState,
                                                       e_NOT_ENTERED,
                                                       e_IN_PROGRESS);

    // Verify that 'd_onceState' was correctly initialized.
    BSLS_ASSERT(e_NOT_ENTERED == state ||
                e_IN_PROGRESS == state ||
                e_DONE        == state);


    if (state != e_NOT_ENTERED) {
        // Someone had previously entered the one time block, block until it is
        // complete.

        while (state != e_DONE) {
            yield();
            state = bsls::AtomicOperations::getIntAcquire(&d_onceState);

            BSLS_ASSERT(e_IN_PROGRESS == state ||
                        e_DONE        == state);
        }
        return false;                                                 // RETURN
    }
    return true;
};

}  // close package namespace
}  // close enterprise namespace


// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
