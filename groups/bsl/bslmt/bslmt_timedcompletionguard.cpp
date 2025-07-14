// bslmt_timedcompletionguard.cpp                                     -*-C++-*-
#include <bslmt_timedcompletionguard.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_timedcompletionguard_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_assertimputil.h>
#include <bsls_log.h>
#include <bsls_systemtime.h>

#include <limits.h>

namespace BloombergLP {
namespace bslmt {

// CLASS DATA
const bsls::TimeInterval TimedCompletionGuard::k_DISABLED =
                                                          bsls::TimeInterval();
const bsls::TimeInterval TimedCompletionGuard::k_WAITING  =
                                          bsls::TimeInterval(LLONG_MAX / 2, 0);

// PRIVATE CLASS METHODS

void *TimedCompletionGuard::guardThreadFunction(void *arg)
{
    TimedCompletionGuard& obj = *static_cast<TimedCompletionGuard *>(arg);

    bslmt::LockGuard<bslmt::Mutex> guardData(&obj.d_dataMutex);

    while (k_DISABLED != obj.d_expiration) {
        // Note that a spurious wakeup returns success.

        int rv = obj.d_condition.timedWait(&obj.d_dataMutex, obj.d_expiration);

        if (bslmt::Condition::e_TIMED_OUT == rv) {
            obj.d_handler(obj.d_text.c_str());

            obj.d_expiration = k_WAITING;
            obj.d_text       = "previously expired";
        }
    }

    return 0;
}

// CLASS METHODS

BSLS_ANNOTATION_NORETURN
void TimedCompletionGuard::failByAbort(const char *text)
{
    if (!text) {
        text = "(* Unspecified Text *)";
    }
    else if (!*text) {
        text = "(* Empty Text *)";
    }

    BSLS_LOG_FATAL("TimedCompletionGuard failure: %s", text);

    bsls::AssertImpUtil::failByAbort();
}

// MANIPULATORS

int TimedCompletionGuard::guard(const bsls::TimeInterval& duration,
                                const bsl::string_view&   text)
{
    BSLS_ASSERT(bsls::TimeInterval(0, 0) < duration);

    bslmt::LockGuard<bslmt::Mutex> guardHandle(&d_handleMutex);

    {
        bslmt::LockGuard<bslmt::Mutex> guardData(&d_dataMutex);

        d_expiration = bsls::SystemTime::nowMonotonicClock() + duration;
        d_text       = text;
    }

    int rv = 0;

    if (d_threadHandle != bslmt::ThreadUtil::invalidHandle()) {
        d_condition.signal();
    }
    else {
        rv = bslmt::ThreadUtil::create(&d_threadHandle,
                                       &guardThreadFunction,
                                       this);
    }

    return rv;
}

void TimedCompletionGuard::release()
{
    bslmt::LockGuard<bslmt::Mutex> guardHandle(&d_handleMutex);

    {
        bslmt::LockGuard<bslmt::Mutex> guardData(&d_dataMutex);

        d_expiration = k_DISABLED;
        d_text       = "";
    }

    if (d_threadHandle != bslmt::ThreadUtil::invalidHandle()) {
        d_condition.signal();

        bslmt::ThreadUtil::join(d_threadHandle);

        d_threadHandle = bslmt::ThreadUtil::invalidHandle();
    }
}

int TimedCompletionGuard::updateText(const bsl::string_view& text)
{
    bslmt::LockGuard<bslmt::Mutex> guardHandle(&d_handleMutex);
    bslmt::LockGuard<bslmt::Mutex> guardData(&d_dataMutex);

    if (k_DISABLED == d_expiration || k_WAITING == d_expiration) {
        return -1;                                                    // RETURN
    }

    d_text = text;

    return 0;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
