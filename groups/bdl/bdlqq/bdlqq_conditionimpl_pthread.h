// bdlqq_conditionimpl_pthread.h                                      -*-C++-*-
#ifndef INCLUDED_BDLQQ_CONDITIONIMPL_PTHREAD
#define INCLUDED_BDLQQ_CONDITIONIMPL_PTHREAD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bdlqq::Condition'.
//
//@CLASSES:
//  bdlqq::ConditionImpl<PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bdlqq_condition
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bdlqq::Condition'
// for POSIX threads ("pthreads") via the template specialization:
//..
//  bdlqq::ConditionImpl<bdlqq::Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bdlqq::Condition'.
//
///Supported Clock-Types
///-------------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock on which timeouts supplied to other methods should be
// based.  If the clock type indicated at construction is
// 'bsls::SystemClockType::e_REALTIME', the timeout should be expressed as an
// absolute offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch
// used in 'bdlt::CurrentTime::now(bsls::SystemClockType::e_REALTIME)'.  If the
// clock type indicated at construction is
// 'bsls::SystemClockType::e_MONOTONIC', the timeout should be expressed as an
// absolute offset since the epoch of this clock (which matches the epoch used
// in 'bdlt::CurrentTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLQQ_MUTEX
#include <bdlqq_mutex.h>
#endif

#ifndef INCLUDED_BDLQQ_PLATFORM
#include <bdlqq_platform.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

#ifdef BDLQQ_PLATFORM_POSIX_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>
#endif

#ifndef INCLUDED_BSL_C_ERRNO
#include <bsl_c_errno.h>
#endif

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

namespace BloombergLP {


namespace bdlqq {template <typename THREAD_POLICY>
class ConditionImpl;
}  // close package namespace



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bsls { class TimeInterval; }                          // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bsls::TimeInterval TimeInterval;    // bdet -> bdlt
}  // close package namespace

namespace bdlqq {
             // ======================================================
             // class ConditionImpl<bdlqq::Platform::PosixThreads>
             // ======================================================

template <>
class ConditionImpl<bdlqq::Platform::PosixThreads> {
    // This class provides a full specialization of 'Condition' for
    // pthreads.  The implementation provided here defines an efficient proxy
    // for the 'pthread_cond_t' pthread type, and related operations.

    // DATA
    pthread_cond_t d_cond;  // provides post/wait for condition

#ifdef BSLS_PLATFORM_OS_DARWIN
    bsls::SystemClockType::Enum d_clockType; // clock type used in 'timedWait'
#endif

    // NOT IMPLEMENTED
    ConditionImpl(const ConditionImpl&);
    ConditionImpl& operator=(const ConditionImpl&);

  public:
    // CREATORS
    explicit
    ConditionImpl(bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Create a condition variable object.  Optionally specify a
        // 'clockType' indicating the type of the system clock against which
        // the 'bsls::TimeInterval' timeouts passed to the 'timedWait' method
        // are to be interpreted.  If 'clockType' is not specified then the
        // realtime system clock is used.

    ~ConditionImpl();
        // Destroy condition variable this object.

    // MANIPULATORS
    void broadcast();
        // Signal this condition object; wake up all threads that are currently
        // waiting on this condition.

    void signal();
        // Signal this condition object; wake up a single thread that is
        // currently waiting on this condition.

    int timedWait(Mutex *mutex, const bsls::TimeInterval& timeout);
        // Atomically unlock the specified 'mutex' and suspend execution of the
        // current thread until this condition object is "signaled" (i.e., one
        // of the 'signal' or 'broadcast' methods is invoked on this object) or
        // until the specified 'timeout', then re-acquire a lock on the
        // 'mutex'.  The 'timeout' is an absolute time represented as an
        // interval from some epoch, which is detemined by the clock indicated
        // at construction (see {'Supported Clock-Types'} in the component
        // documentation).  Return 0 on success, -1 on timeout, and a non-zero
        // value different from -1 if an error occurs.  The behavior is
        // undefined unless 'mutex' is locked by the calling thread prior to
        // calling this method.  Note that 'mutex' remains locked by the
        // calling thread upon returning from this function with success or
        // timeout, but is *not* guaranteed to remain locked otherwise.  Also
        // note that spurious wakeups are rare but possible, i.e., this method
        // may succeed (return 0) and return control to the thread without the
        // condition object being signaled.

    int wait(Mutex *mutex);
        // Atomically unlock the specified 'mutex' and suspend execution of the
        // current thread until this condition object is "signaled" (i.e.,
        // either 'signal' or 'broadcast' is invoked on this object in another
        // thread), then re-acquire a lock on the 'mutex'.  Return 0 on
        // success, and a non-zero value otherwise.  Spurious wakeups are rare
        // but possible; i.e., this method may succeed (return 0), and return
        // control to the thread without the condition object being signaled.
        // The behavior is undefined unless 'mutex' is locked by the calling
        // thread prior to calling this method.  Note that 'mutex' remains
        // locked by the calling thread upon successfully returning from this
        // function, but is *not* guaranteed to remain locked if an error
        // occurs.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

             // ------------------------------------------------------
             // class ConditionImpl<bdlqq::Platform::PosixThreads>
             // ------------------------------------------------------

// CREATORS
inline
ConditionImpl<bdlqq::Platform::PosixThreads>::~ConditionImpl()
{
    pthread_cond_destroy(&d_cond);
}

// MANIPULATORS
inline
void ConditionImpl<bdlqq::Platform::PosixThreads>::broadcast()
{
    pthread_cond_broadcast(&d_cond);
}

inline
void ConditionImpl<bdlqq::Platform::PosixThreads>::signal()
{
    pthread_cond_signal(&d_cond);
}

inline
int ConditionImpl<bdlqq::Platform::PosixThreads>::wait(Mutex *mutex)
{
    return pthread_cond_wait(&d_cond, &mutex->nativeMutex());
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLQQ_PLATFORM_POSIX_THREADS

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
