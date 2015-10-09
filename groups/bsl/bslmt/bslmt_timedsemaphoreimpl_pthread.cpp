// bslmt_timedsemaphoreimpl_pthread.cpp                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_timedsemaphoreimpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_timedsemaphoreimpl_pthread_cpp,"$Id$ $CSID$")

#include <bslmt_muteximpl_pthread.h>   // for testing only
#include <bslmt_saturatedtimeconversionimputil.h>
#include <bslmt_threadutil.h>

#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#include <bsls_assert.h>

#ifdef BSLMT_PLATFORM_POSIX_THREADS

// Platform-specific implementation starts here.

#include <bsl_ctime.h>
#include <bsl_c_errno.h>

namespace BloombergLP {
namespace {

#if !defined(BSLS_PLATFORM_OS_DARWIN)
// Set the condition clock type, except on Darwin which doesn't support it.

class CondAttr {
    // This class is a thin wrapper over 'pthread_condattr_t' structure which
    // gets configured with the proper clock type for the purpose of
    // initializing the 'pthread_cond_t' object.

    // DATA
    pthread_condattr_t d_attr;

    // NOT IMPLEMENTED
    CondAttr();
    CondAttr(const CondAttr&);
    CondAttr& operator=(const CondAttr&);

public:
    CondAttr(bsls::SystemClockType::Enum clockType)
        // Create the 'pthread_condattr_t' structure and initialize it with the
        // specified 'clockType'.
    {
        int rc = pthread_condattr_init(&d_attr);
        (void) rc; BSLS_ASSERT(0 == rc);  // can only fail on 'ENOMEM'

        clockid_t clockId;
        switch (clockType) {
          case bsls::SystemClockType::e_REALTIME: {
            clockId = CLOCK_REALTIME;
          } break;
          case bsls::SystemClockType::e_MONOTONIC: {
            clockId = CLOCK_MONOTONIC;
          } break;
          default:
            BSLS_ASSERT_OPT("Invalid ClockType parameter value" && 0);
        }

        pthread_condattr_setclock(&d_attr, clockId);
    }

    ~CondAttr()
        // Destroy the 'pthread_condattr_t' structure.
    {
        int rc = pthread_condattr_destroy(&d_attr);
        (void) rc; BSLS_ASSERT(0 == rc);  // can only fail on invalid 'd_attr'
    }

    const pthread_condattr_t& conditonAttributes() const
    {
        return d_attr;
    }
};

#endif

// STATIC HELPER FUNCTIONS

static
void initializeCondition(pthread_cond_t              *condition,
                         bsls::SystemClockType::Enum  clockType)
    // Initialize the specified 'condition' variable with the specified
    // 'clockType'.
{
#ifdef BSLS_PLATFORM_OS_DARWIN
    (void) clockType;
    int rc = pthread_cond_init(condition, 0);
    (void) rc; BSLS_ASSERT(0 == rc); // 'pthread_cond_int' can only fail for
                                     // two possible reasons in this usage and
                                     // neither should ever occur:
                                     //: 1 lack of system resources
                                     //: 2 attempt to re-initialise 'condition'
#else
    CondAttr attr(clockType);
    int rc = pthread_cond_init(condition, &attr.conditonAttributes());
    (void) rc; BSLS_ASSERT(0 == rc); // 'pthread_cond_int' can only fail for
                                     // three possible reasons in this usage
                                     // and none should ever occur:
                                     //: 1 lack of system resources
                                     //: 2 attempt to re-initialise 'condition'
                                     //: 3 the attribute is invalid
#endif
}

static
int decrementIfPositive(bsls::AtomicInt *a)
    // Try to decrement the specified atomic integer 'a' if positive.  Return 0
    // on success and a non-zero value otherwise.
{
    int i = *a;

    while (i > 0) {
        if (i == a->testAndSwap(i, i - 1)) {
            return 0;                                                 // RETURN
        }
        i = *a;
    }
    return -1;
}

}  // close unnamed namespace

             // -----------------------------------------------
             // class TimedSemaphoreImpl<PthreadTimedSemaphore>
             // -----------------------------------------------

// CREATORS
bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>::
                TimedSemaphoreImpl(bsls::SystemClockType::Enum clockType)
: d_resources(0)
, d_waiters(0)
#ifdef BSLS_PLATFORM_OS_DARWIN
, d_clockType(clockType)
#endif
{
    pthread_mutex_init(&d_lock, 0);
    initializeCondition(&d_condition, clockType);
}

bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>::
     TimedSemaphoreImpl(int count, bsls::SystemClockType::Enum clockType)
: d_resources(count)
, d_waiters(0)
#ifdef BSLS_PLATFORM_OS_DARWIN
, d_clockType(clockType)
#endif
{
    pthread_mutex_init(&d_lock, 0);
    initializeCondition(&d_condition, clockType);
}

bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>::
                                                    ~TimedSemaphoreImpl()
{
    pthread_mutex_lock(&d_lock);
    pthread_mutex_destroy(&d_lock);

    pthread_cond_destroy(&d_condition);

    BSLS_ASSERT(d_resources >= 0);
    BSLS_ASSERT(d_waiters >= 0);
}

// MANIPULATORS
void bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>::post()
{
    ++d_resources;
    // barrier
    if (d_waiters > 0) {
        pthread_mutex_lock(&d_lock);
        pthread_cond_signal(&d_condition);
        pthread_mutex_unlock(&d_lock);
    }
}

void
bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>::post(
                                                                    int number)
{
    BSLS_ASSERT(number > 0);

    d_resources += number;
    // barrier
    if (d_waiters > 0) {
        pthread_mutex_lock(&d_lock);
        pthread_cond_broadcast(&d_condition);
        pthread_mutex_unlock(&d_lock);
    }
}

int bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>::
    timedWait(const bsls::TimeInterval& timeout)
{
    if (0 == decrementIfPositive(&d_resources)) {
        return 0;                                                     // RETURN
    }

    int ret = 0;
    pthread_mutex_lock(&d_lock);
    ++d_waiters;

    while (0 != decrementIfPositive(&d_resources)) {
        int status = timedWaitImp(timeout);
        if (0 != status) {
            BSLS_ASSERT(-1 == status);    // timeout and not an error
            ret = 1;
            break;
        }
    }

    --d_waiters;
    pthread_mutex_unlock(&d_lock);
    return ret;
}

int bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>
                              ::timedWaitImp(const bsls::TimeInterval& timeout)
{
#ifdef BSLS_PLATFORM_OS_DARWIN
    // Darwin supports only realtime clock for the condition variable.

    bsls::TimeInterval realTimeout(timeout);

    if (d_clockType != bsls::SystemClockType::e_REALTIME) {
        // since cond_timedwait operates only with the realtime clock, adjust
        // the timeout value to make it consistent with the realtime clock
        realTimeout += bsls::SystemTime::nowRealtimeClock() -
                       bsls::SystemTime::now(d_clockType);
    }

    timespec ts;
    SaturatedTimeConversionImpUtil::toTimeSpec(&ts, realTimeout);

#else  // !DARWIN
    timespec ts;
    SaturatedTimeConversionImpUtil::toTimeSpec(&ts, timeout);
#endif

    int status = pthread_cond_timedwait(&d_condition, &d_lock, &ts);
    return status == 0 ? 0 : (status == ETIMEDOUT ? -1 : -2);
}

int bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>
    ::tryWait()
{
    const int newValue = decrementIfPositive(&d_resources);
    if (newValue >= 0) {
        if (newValue != 0) {
            pthread_cond_signal(&d_condition);
        }
        return 0;                                                     // RETURN
    }
    return 1;
}

void bslmt::TimedSemaphoreImpl<bslmt::Platform::PthreadTimedSemaphore>::wait()
{
    if (0 == decrementIfPositive(&d_resources)) {
        return;                                                       // RETURN
    }

    pthread_mutex_lock(&d_lock);
    ++d_waiters;
    while (0 != decrementIfPositive(&d_resources)) {
        pthread_cond_wait(&d_condition, &d_lock);
    }
    --d_waiters;
    pthread_mutex_unlock(&d_lock);
}

}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_POSIX_THREADS

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
