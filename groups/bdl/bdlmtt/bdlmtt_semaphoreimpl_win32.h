// bdlmtt_semaphoreimpl_win32.h                                        -*-C++-*-
#ifndef INCLUDED_BDLMTT_SEMAPHOREIMPL_WIN32
#define INCLUDED_BDLMTT_SEMAPHOREIMPL_WIN32

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bdlmtt::Semaphore'.
//
//@CLASSES:
//  bdlmtt::SemaphoreImpl<Win32Semaphore>: win32 specialization
//
//@SEE_ALSO: bdlmtt_semaphore
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bdlmtt::Semaphore'
// for Windows (win32) via the template specialization:
//..
//  bdlmtt::SemaphoreImpl<bdlmtt::Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bdlmtt::Semaphore'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMTT_PLATFORM
#include <bdlmtt_platform.h>
#endif

#ifdef BDLMTT_PLATFORM_WIN32_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BDLMTT_XXXATOMICTYPES
#include <bdlmtt_xxxatomictypes.h>
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


namespace bdlmtt {template <typename SEMAPHORE_POLICY>
class SemaphoreImpl;

           // ========================================================
           // class SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>
           // ========================================================

template <>
class SemaphoreImpl<bdlmtt::Platform::Win32Semaphore> {
    // This class provides a full specialization of 'SemaphoreImpl'
    // for win32.  The implementation provided here defines an efficient
    // POSIX like semaphore.

    // DATA
    void          *d_handle;     // TBD doc

    bdlmtt::AtomicInt d_resources;  // if positive, number of available resources
                                 // if negative: number of waiting threads
                                 // (need this because Windows does not provide
                                 //  a mechanism to get the semaphore count)

    // NOT IMPLEMENTED
    SemaphoreImpl(const SemaphoreImpl&);
    SemaphoreImpl& operator=(const SemaphoreImpl&);

  public:
    // CREATORS
    SemaphoreImpl(int count);
        // Create a semaphore initially having the specified 'count'.

    ~SemaphoreImpl();
        // Destroy a semaphore

    // MANIPULATORS
    void post();
        // Atomically increment the count of this semaphore.

    void post(int number);
        // Atomically increment the count of this semaphore by the specified
        // 'number'.  The behavior is undefined unless 'number > 0'.

    int tryWait();
        // Decrement the count of this semaphore if it is positive and return
        // 0.  Return a non-zero value otherwise.

    void wait();
        // Block until the count of this semaphore is a positive value and
        // atomically decrement it.

    // ACCESSORS
    int getValue() const;
        // Return the current value of this semaphore.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

           // --------------------------------------------------------
           // class SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>
           // --------------------------------------------------------

// CREATORS
inline
SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>::SemaphoreImpl(
                                                                     int count)
: d_resources(count)
{
    // Create a semaphore with a 0 count, since the count is actually
    // maintained in 'd_resources'.

    d_handle = CreateSemaphoreA(NULL, 0, INT_MAX, NULL);
}

inline
SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>::~SemaphoreImpl()
{
    CloseHandle(d_handle);
}

// MANIPULATORS
inline
void SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>::post()
{
    if (++d_resources <= 0) {
        ReleaseSemaphore(d_handle, 1, NULL);
    }
}

inline
void SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>::wait()
{
    if (--d_resources >= 0) {
        return;
    }
    WaitForSingleObject(d_handle, 0xFFFFFFFF /*INFINITE*/);
}

// ACCESSORS
inline
int SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>::getValue() const
{
    const int v = d_resources;
    return v > 0 ? v : 0;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLMTT_PLATFORM_WIN32_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
