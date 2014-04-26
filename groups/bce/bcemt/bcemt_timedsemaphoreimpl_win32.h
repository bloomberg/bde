// bcemt_timedsemaphoreimpl_win32.h                                   -*-C++-*-
#ifndef INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_WIN32
#define INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_WIN32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bcemt_TimedSemaphore'.
//
//@CLASSES:
//  bcemt_TimedSemaphoreImpl<Win32TimedSemaphore>: win32 specialization
//
//@SEE_ALSO: bcemt_timedsemaphore
//
//@AUTHOR: David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides an implementation of
// 'bcemt_TimedSemaphore' for Windows (win32) via the template specialization:
//..
//  bcemt_TimedSemaphoreImpl<bces_Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_TimedSemaphore'.
//
///Supported Clock-Types
///-------------------------
// The component 'bdetu_SystemClockType' supplies the enumeration indicating
// the system clock on which timeouts supplied to other methods should be
// based.  If the clock type indicated at construction is
// 'bdetu_SystemClockType::e_REALTIME', the timeout should be expressed as an
// absolute offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch
// used in 'bdetu_SystemTime::now(bdetu_SystemClockType::e_REALTIME)'.  If the
// clock type indicated at construction is
// 'bdetu_SystemClockType::e_MONOTONIC', the timeout should be expressed as an
// absolute offset since the epoch of this clock (which matches the epoch used
// in 'bdetu_SystemTime::now(bdetu_SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifdef BCES_PLATFORM_WIN32_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDETU_SYSTEMTIME
#include <bdetu_systemtime.h>
#endif

#ifndef INCLUDED_BDETU_SYSTEMCLOCKTYPE
#include <bdetu_systemclocktype.h>
#endif

#ifndef INCLUDED_BSL_C_LIMITS
#include <bsl_c_limits.h>
#endif

struct _SECURITY_ATTRIBUTES;
typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
typedef long LONG, *LPLONG;
typedef int BOOL;
typedef void *HANDLE;
typedef const char *LPCSTR;
typedef unsigned long DWORD;

extern "C" {

    __declspec(dllimport) HANDLE __stdcall CreateSemaphoreA(
                LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
                LONG lInitialCount,
                LONG lMaximumCount,
                LPCSTR lpName
    );

    __declspec(dllimport) BOOL __stdcall CloseHandle(
                HANDLE hObject
    );

    __declspec(dllimport) BOOL __stdcall ReleaseSemaphore(
                HANDLE hSemaphore,
                LONG lReleaseCount,
                LPLONG lpPreviousCount
    );

    __declspec(dllimport) DWORD __stdcall WaitForSingleObject(
                HANDLE hHandle,
                DWORD dwMilliseconds
    );

};

namespace BloombergLP {

template <class TIMED_SEMAPHORE_POLICY>
class bcemt_TimedSemaphoreImpl;

           // ===================================================
           // class bcemt_TimedSemaphoreImpl<Win32TimedSemaphore>
           // ===================================================

template <>
class bcemt_TimedSemaphoreImpl<bces_Platform::Win32TimedSemaphore> {
    // This class implements a timed semaphore in terms of Windows semaphores.

    // DATA
    void                        *d_handle;    // handle to Window's
                                              // implementation of timed
                                              // semaphore

    bdetu_SystemClockType::Enum  d_clockType; // clock used in timedWait

    // NOT IMPLEMENTED
    bcemt_TimedSemaphoreImpl(const bcemt_TimedSemaphoreImpl&);
    bcemt_TimedSemaphoreImpl& operator=(const bcemt_TimedSemaphoreImpl&);

  public:
    // CREATORS
    explicit
    bcemt_TimedSemaphoreImpl(bdetu_SystemClockType::Enum clockType
                                          = bdetu_SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having a count of 0.  Optionally
        // specify a 'clockType' indicating the type of the system clock
        // against which the 'bdet_TimeInterval' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    explicit
    bcemt_TimedSemaphoreImpl(int                         count,
                             bdetu_SystemClockType::Enum clockType
                                          = bdetu_SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having the specified 'count'.
        // Optionally specify a 'clockType' indicating the type of the system
        // clock against which the 'bdet_TimeInterval' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    ~bcemt_TimedSemaphoreImpl();
        // Destroy this semaphore object.

    // MANIPULATORS
    void post();
        // Atomically increment the count of the semaphore.

    void post(int number);
        // Atomically increment the count by the specified 'number' of the
        // semaphore.  The behavior is undefined unless 'number' is a positive
        // value.

    int timedWait(const bdet_TimeInterval& timeout);
        // Block until the count of this semaphore is a positive value, or
        // until the specified 'timeout' expires.  The 'timeout' is an absolute
        // time represented as an interval from some epoch, which is detemined
        // by the clock indicated at construction (see
        // {'Supported Clock-Types'} in the component documentation).  If the
        // 'timeout' did not expire before the count attained a positive value,
        // atomically decrement the count and return 0; otherwise, return a
        // non-zero value with no effect on the count.

    int tryWait();
        // Decrement the count of this semaphore if it is positive and return
        // 0.  Return a non-zero value otherwise.

    void wait();
        // Block until the count is a positive value and atomically decrement
        // it.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

           // ---------------------------------------------------
           // class bcemt_TimedSemaphoreImpl<Win32TimedSemaphore>
           // ---------------------------------------------------

// CREATORS
inline
bcemt_TimedSemaphoreImpl<bces_Platform::Win32TimedSemaphore>::
                bcemt_TimedSemaphoreImpl(bdetu_SystemClockType::Enum clockType)
: d_clockType(clockType)
{
    d_handle = CreateSemaphoreA(NULL, 0, INT_MAX, NULL);
}

inline
bcemt_TimedSemaphoreImpl<bces_Platform::Win32TimedSemaphore>::
     bcemt_TimedSemaphoreImpl(int count, bdetu_SystemClockType::Enum clockType)
: d_clockType(clockType)
{
    d_handle = CreateSemaphoreA(NULL, count, INT_MAX, NULL);
}

inline
bcemt_TimedSemaphoreImpl<bces_Platform::Win32TimedSemaphore>::
                                                    ~bcemt_TimedSemaphoreImpl()
{
    CloseHandle(d_handle);
}

// MANIPULATORS
inline
void bcemt_TimedSemaphoreImpl<bces_Platform::Win32TimedSemaphore>::post()
{
    ReleaseSemaphore(d_handle, 1, NULL);
}

inline
void bcemt_TimedSemaphoreImpl<bces_Platform::Win32TimedSemaphore>::post(
                                                                    int number)
{
    ReleaseSemaphore(d_handle, number, NULL);
}

inline
int bcemt_TimedSemaphoreImpl<bces_Platform::Win32TimedSemaphore>::tryWait()
{
    return WaitForSingleObject(d_handle, 0);  // 0 means timeout immediately.
}

inline
void bcemt_TimedSemaphoreImpl<bces_Platform::Win32TimedSemaphore>::wait()
{
    WaitForSingleObject(d_handle, 0xFFFFFFFF /* INFINITE */);
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_WIN32_THREADS

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
