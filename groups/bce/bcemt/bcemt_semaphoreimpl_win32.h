// bcemt_semaphoreimpl_win32.h                                        -*-C++-*-
#ifndef INCLUDED_BCEMT_SEMAPHOREIMPL_WIN32
#define INCLUDED_BCEMT_SEMAPHOREIMPL_WIN32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bcemt_Semaphore'.
//
//@CLASSES:
//  bcemt_SemaphoreImpl<Win32Semaphore>: win32 specialization
//
//@SEE_ALSO: bcemt_semaphore
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_Semaphore'
// for Windows (win32) via the template specialization:
//..
//  bcemt_SemaphoreImpl<bces_Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_Semaphore'.
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

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
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

template <typename SEMAPHORE_POLICY>
class bcemt_SemaphoreImpl;

           // ========================================================
           // class bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>
           // ========================================================

template <>
class bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore> {
    // This class provides a full specialization of 'bcemt_SemaphoreImpl'
    // for win32.  The implementation provided here defines an efficient
    // POSIX like semaphore.

    // DATA
    void          *d_handle;     // TBD doc

    bces_AtomicInt d_resources;  // if positive, number of available resources
                                 // if negative: number of waiting threads
                                 // (need this because Windows does not provide
                                 //  a mechanism to get the semaphore count)

    // NOT IMPLEMENTED
    bcemt_SemaphoreImpl(const bcemt_SemaphoreImpl&);
    bcemt_SemaphoreImpl& operator=(const bcemt_SemaphoreImpl&);

  public:
    // CREATORS
    bcemt_SemaphoreImpl(int count);
        // Create a semaphore initially having the specified 'count'.

    ~bcemt_SemaphoreImpl();
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
           // class bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>
           // --------------------------------------------------------

// CREATORS
inline
bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>::bcemt_SemaphoreImpl(
                                                                     int count)
: d_resources(count)
{
    // Create a semaphore with a 0 count, since the count is actually
    // maintained in 'd_resources'.

    d_handle = CreateSemaphoreA(NULL, 0, INT_MAX, NULL);
}

inline
bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>::~bcemt_SemaphoreImpl()
{
    CloseHandle(d_handle);
}

// MANIPULATORS
inline
void bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>::post()
{
    if (++d_resources <= 0) {
        ReleaseSemaphore(d_handle, 1, NULL);
    }
}

inline
void bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>::wait()
{
    if (--d_resources >= 0) {
        return;
    }
    WaitForSingleObject(d_handle, 0xFFFFFFFF /*INFINITE*/);
}

// ACCESSORS
inline
int bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>::getValue() const
{
    const int v = d_resources;
    return v > 0 ? v : 0;
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_WIN32_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
