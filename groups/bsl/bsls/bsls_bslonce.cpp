// bsls_bslonce.cpp                                                   -*-C++-*-
#include <bsls_bslonce.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bsls_bsldoonce_cpp,"$Id$ $CSID$")

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
// Copyright (C) 2014 Bloomberg Finance L.P.
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
