// bdlqq_once.cpp                                                     -*-C++-*-
#include <bdlqq_once.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_once_cpp,"$Id$ $CSID$")

#include <bdlqq_threadattributes.h>    // for testing only
#include <bdlqq_threadutil.h>          // for testing only
#include <bdlqq_barrier.h>             // for testing only

#include <bsl_exception.h>

#include <bsls_assert.h>

namespace BloombergLP {

                                // ----------
                                // class Once
                                // ----------

bool bdlqq::Once::enter(Once::OnceLock *onceLock)
{
    if (e_DONE == bsls::AtomicOperations::getInt(&d_state)) {
        return false;                                                 // RETURN
    }

    onceLock->lock(&d_mutex);  // Lock the mutex
    switch (bsls::AtomicOperations::getInt(&d_state)) {

      case e_NOT_ENTERED:
        bsls::AtomicOperations::setInt(&d_state, e_IN_PROGRESS);
        return true;  // Leave mutex locked                           // RETURN

      case e_IN_PROGRESS:
        BSLS_ASSERT(! "Can't get here!");
        break;

      case e_DONE:
        onceLock->unlock();
        return false;                                                 // RETURN
    }

    return false;
}

void bdlqq::Once::leave(Once::OnceLock *onceLock)
{
    BSLS_ASSERT(e_IN_PROGRESS == bsls::AtomicOperations::getInt(&d_state));

    bsls::AtomicOperations::setInt(&d_state, e_DONE);
    onceLock->unlock();
}

void bdlqq::Once::cancel(Once::OnceLock *onceLock)
{
    BSLS_ASSERT(e_IN_PROGRESS == bsls::AtomicOperations::getInt(&d_state));

    bsls::AtomicOperations::setInt(&d_state, e_NOT_ENTERED);
    onceLock->unlock();
}

                            // ---------------
                            // class OnceGuard
                            // ---------------

bdlqq::OnceGuard::~OnceGuard()
{
    if (e_IN_PROGRESS != d_state) {
        return;                                                       // RETURN
    }
#if ! defined(BSLS_PLATFORM_CMP_MSVC)
    else if (bsl::uncaught_exception()) {
        d_once->cancel(&d_onceLock);
    }
#endif
    else {
        d_once->leave(&d_onceLock);
    }
}

bool bdlqq::OnceGuard::enter()
{
    if (e_DONE == d_state) {
        return false;                                                 // RETURN
    }

    BSLS_ASSERT(e_IN_PROGRESS != d_state);
    BSLS_ASSERT(d_once);

    if (d_once->enter(&d_onceLock)) {
        d_state = e_IN_PROGRESS;
        return true;                                                  // RETURN
    }
    else {
        return false;                                                 // RETURN
    }
}

void bdlqq::OnceGuard::leave()
{
    if (e_IN_PROGRESS == d_state) {
        d_once->leave(&d_onceLock);
        d_state = e_DONE;
    }
}

void bdlqq::OnceGuard::cancel()
{
    if (e_IN_PROGRESS == d_state) {
        d_once->cancel(&d_onceLock);
        d_state = e_NOT_ENTERED;
    }
}

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
