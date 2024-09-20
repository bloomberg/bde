// bslmt_semaphoreimpl_win32.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMT_SEMAPHOREIMPL_WIN32
#define INCLUDED_BSLMT_SEMAPHOREIMPL_WIN32

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of `bslmt::Semaphore`.
//
//@CLASSES:
//  bslmt::SemaphoreImpl<Win32Semaphore>: win32 specialization
//
//@SEE_ALSO: bslmt_semaphore
//
//@DESCRIPTION: This component provides an implementation of `bslmt::Semaphore`
// for Windows (win32), `bslmt::SemaphoreImpl<Win32Semaphore>`, via the
// template specialization:
// ```
// bslmt::SemaphoreImpl<Platform::Win32Threads>
// ```
// This template class should not be used (directly) by client code.  Clients
// should instead use `bslmt::Semaphore`.
//
///Usage
///-----
// This component is an implementation detail of `bslmt` and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <bslscm_version.h>

#include <bslmt_platform.h>

#ifdef BSLMT_PLATFORM_WIN32_THREADS

// Platform-specific implementation starts here.

#include <bsls_assert.h>
#include <bsls_atomic.h>

#include <bsl_c_limits.h>

struct _SECURITY_ATTRIBUTES;
typedef struct _SECURITY_ATTRIBUTES  SECURITY_ATTRIBUTES;
typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;
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
namespace bslmt {

template <class SEMAPHORE_POLICY>
class SemaphoreImpl;

              // =============================================
              // class SemaphoreImpl<Platform::Win32Semaphore>
              // =============================================

/// This class provides a full specialization of `SemaphoreImpl` for win32.
/// The implementation provided here defines an efficient POSIX like
/// semaphore.
template <>
class SemaphoreImpl<Platform::Win32Semaphore> {

    // DATA
    void            *d_handle;     // TBD doc

    bsls::AtomicInt  d_resources;  // if positive, number of available
                                   // resources if negative: number of waiting
                                   // threads (need this because Windows does
                                   // not provide a mechanism to get the
                                   // semaphore count)

    // NOT IMPLEMENTED
    SemaphoreImpl(const SemaphoreImpl&);
    SemaphoreImpl& operator=(const SemaphoreImpl&);

  public:
    // CREATORS

    /// Create a semaphore initially having the specified `count`.  This
    /// method does not return normally unless there are sufficient system
    /// resources to construct the object.
    SemaphoreImpl(int count);

    /// Destroy a semaphore
    ~SemaphoreImpl();

    // MANIPULATORS

    /// Atomically increment the count of this semaphore.
    void post();

    /// Atomically increment the count of this semaphore by the specified
    /// `number`.  The behavior is undefined unless `number > 0`.
    void post(int number);

    /// Decrement the count of this semaphore if it is positive and return
    /// 0.  Return a non-zero value otherwise.
    int tryWait();

    /// Block until the count of this semaphore is a positive value and
    /// atomically decrement it.
    void wait();

    // ACCESSORS

    /// Return the current value of this semaphore.
    int getValue() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

              // ---------------------------------------------
              // class SemaphoreImpl<Platform::Win32Semaphore>
              // ---------------------------------------------

// CREATORS
inline
SemaphoreImpl<bslmt::Platform::Win32Semaphore>::SemaphoreImpl(int count)
: d_resources(count)
{
    // Create a semaphore with a 0 count, since the count is actually
    // maintained in 'd_resources'.

    d_handle = CreateSemaphoreA(NULL, 0, INT_MAX, NULL);
    if (NULL == d_handle) {
        BSLS_ASSERT_INVOKE_NORETURN("'CreateSemaphoreA' failed");
    }
}

inline
SemaphoreImpl<bslmt::Platform::Win32Semaphore>::~SemaphoreImpl()
{
    CloseHandle(d_handle);
}

// MANIPULATORS
inline
void SemaphoreImpl<bslmt::Platform::Win32Semaphore>::post()
{
    if (++d_resources <= 0) {
        ReleaseSemaphore(d_handle, 1, NULL);
    }
}

inline
void SemaphoreImpl<bslmt::Platform::Win32Semaphore>::wait()
{
    if (--d_resources >= 0) {
        return;
    }
    WaitForSingleObject(d_handle, 0xFFFFFFFF /*INFINITE*/);
}

// ACCESSORS
inline
int SemaphoreImpl<bslmt::Platform::Win32Semaphore>::getValue() const
{
    const int v = d_resources;
    return v > 0 ? v : 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_WIN32_THREADS

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
