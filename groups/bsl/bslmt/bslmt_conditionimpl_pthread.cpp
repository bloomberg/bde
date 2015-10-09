// bslmt_conditionimpl_pthread.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_conditionimpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_conditionimpl_pthread_cpp,"$Id$ $CSID$")

#include <bslmt_saturatedtimeconversionimputil.h>

#include <bsls_assert.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#ifdef BSLMT_PLATFORM_POSIX_THREADS

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

        rc = pthread_condattr_setclock(&d_attr, clockId);
        (void) rc; BSLS_ASSERT(0 == rc);  // only documented failure is for bad
                                          // input
    }

    ~CondAttr()
        // Destroy the 'pthread_condattr_t' structure.
    {
        int rc = pthread_condattr_destroy(&d_attr);
        (void) rc; BSLS_ASSERT(0 == rc);  // can only fail on invalid 'd_attr'
    }

    const pthread_condattr_t & conditonAttributes() const
    {
        return d_attr;
    }
};

#endif

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

}  // close unnamed namespace

               // -------------------------------------------
               // class ConditionImpl<Platform::PosixThreads>
               // -------------------------------------------

// CREATORS
bslmt::ConditionImpl<bslmt::Platform::PosixThreads>::ConditionImpl(
                                         bsls::SystemClockType::Enum clockType)
#ifdef BSLS_PLATFORM_OS_DARWIN
: d_clockType(clockType)
#endif
{
    initializeCondition(&d_cond, clockType);
}

// MANIPULATORS
int bslmt::ConditionImpl<bslmt::Platform::PosixThreads>::timedWait(
                                            Mutex                     *mutex,
                                            const bsls::TimeInterval&  timeout)
{
#ifdef BSLS_PLATFORM_OS_DARWIN
    // This implementation is very sensitive to the 'd_clockType'.  For safety,
    // we will assert the value is one of the two currently expected values.
    BSLS_ASSERT(bsls::SystemClockType::e_REALTIME == d_clockType ||
                bsls::SystemClockType::e_MONOTONIC == d_clockType);

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
    int status = pthread_cond_timedwait(&d_cond, &mutex->nativeMutex(), &ts);

    return 0 == status ? 0 : (ETIMEDOUT == status ? -1 : -2);
}

}  // close enterprise namespace

#endif

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
