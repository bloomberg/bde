// bdlmtt_conditionimpl_pthread.cpp                                    -*-C++-*-
#include <bdlmtt_conditionimpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_conditionimpl_pthread_cpp,"$Id$ $CSID$")

#include <bdlmtt_saturatedtimeconversionimputil.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#ifdef BDLMTT_PLATFORM_POSIX_THREADS

namespace BloombergLP {

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
        (void) rc; BSLS_ASSERT(0 == rc);  // can only fail on ENOMEM

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
void initializeCondition(pthread_cond_t              *cond,
                         bsls::SystemClockType::Enum  clockType)
    // Initialize the specified 'cond' variable with the specified 'clockType'.
{
#ifdef BSLS_PLATFORM_OS_DARWIN
    (void) clockType;
    int rc = pthread_cond_init(cond, 0);
    (void) rc; BSLS_ASSERT(0 == rc); // 'pthread_cond_int' can only fail for
                                     // two possible reasons in this usage and
                                     // neither should ever occur:
                                     //: 1 lack of system resources
                                     //: 2 attempt to re-initialise 'cond'
#else
    CondAttr attr(clockType);
    int rc = pthread_cond_init(cond, &attr.conditonAttributes());
    (void) rc; BSLS_ASSERT(0 == rc); // 'pthread_cond_int' can only fail for
                                     // three possible reasons in this usage
                                     // and none should ever occur:
                                     //: 1 lack of system resources
                                     //: 2 attempt to re-initialise 'cond'
                                     //: 3 the attribute is invalid
#endif
}

namespace bdlmtt {
             // ------------------------------------------------------
             // class ConditionImpl<bdlmtt::Platform::PosixThreads>
             // ------------------------------------------------------

// CREATORS
ConditionImpl<bdlmtt::Platform::PosixThreads>::ConditionImpl(
                                         bsls::SystemClockType::Enum clockType)
#ifdef BSLS_PLATFORM_OS_DARWIN
: d_clockType(clockType)
#endif
{
    initializeCondition(&d_cond, clockType);
}

// MANIPULATORS
int ConditionImpl<bdlmtt::Platform::PosixThreads>::timedWait(
                                             Mutex              *mutex,
                                             const bsls::TimeInterval&  timeout)
{
#ifdef BSLS_PLATFORM_OS_DARWIN
    // This implementation is very sensitive to the 'd_clockType'.  For
    // safety, we will assert the value is one of the two currently expected
    // values.
    BSLS_ASSERT(bsls::SystemClockType::e_REALTIME == d_clockType ||
                bsls::SystemClockType::e_MONOTONIC == d_clockType);

    bsls::TimeInterval realTimeout(timeout);

    if (d_clockType != bsls::SystemClockType::e_REALTIME) {
        // since cond_timedwait operates only with the realtime clock, adjust
        // the timeout value to make it consistent with the realtime clock
        realTimeout += bsls::SystemTime::nowRealtimeClock()
                                          - bdlt::CurrentTime::now(d_clockType);
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
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
