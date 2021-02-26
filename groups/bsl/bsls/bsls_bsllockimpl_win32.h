// bsls_bsllockimpl_win32.h                                           -*-C++-*-
#ifndef INCLUDED_BSLS_BSLLOCKIMPL_WIN32
#define INCLUDED_BSLS_BSLLOCKIMPL_WIN32

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mutex for use below 'bslmt' for windows platform.
//
//@CLASSES:
//  bsls::BslLockImpl_win32: windows mutex
//
//@SEE_ALSO: bslmt_mutex
//
//@DESCRIPTION: This component provides a mutually exclusive lock primitive
// ("mutex") by wrapping a suitable platform-specific mechanism.  The
// 'bsls::BslLockImpl_win32' class provides 'lock' and 'unlock' operations.
// Note that 'bsls::BslLockImpl_win32' is not intended for direct client use;
// see 'bslmt_mutex' instead.  Also note that 'bsls::BslLockImpl_win32' is not
// recursive.
//
///Usage
///-----
// This component is an implementation detail of 'bsls' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS

// This section replaces #include <windows.h> by forward declaring necessary
// structures and API. See {DRQS 147858260}.

struct _RTL_CRITICAL_SECTION;

typedef struct _RTL_CRITICAL_SECTION CRITICAL_SECTION, *LPCRITICAL_SECTION;
typedef int BOOL;
typedef unsigned long DWORD;

extern "C" {
    __declspec(dllimport) BOOL __stdcall InitializeCriticalSectionAndSpinCount(
                                    LPCRITICAL_SECTION lpCriticalSection,
                                    DWORD dwSpinCount);

    __declspec(dllimport) void __stdcall DeleteCriticalSection(
                                LPCRITICAL_SECTION lpCriticalSection);

    __declspec(dllimport) void __stdcall EnterCriticalSection(
                                LPCRITICAL_SECTION lpCriticalSection);

    __declspec(dllimport) void __stdcall LeaveCriticalSection(
                                LPCRITICAL_SECTION lpCriticalSection);

}  // extern "C"

namespace BloombergLP {
namespace bsls {

                              // =======================
                              // class BslLockImpl_win32
                              // =======================

class BslLockImpl_win32 {
    // This 'class' implements a light-weight wrapper of an OS-level mutex to
    // support intra-process synchronization.  The mutex implemented by this
    // class is *non*-recursive.  Note that 'BslLockImpl_win32' is *not*
    // intended for direct use by client code; it is meant for internal use
    // only.

  public:
    enum {
        // Size of the buffer allocated for the critical section, in
        // pointer-sized elements.  We have to make it public so we could
        // access it in a .cpp file to verify the size.

#ifdef BSLS_PLATFORM_CPU_64_BIT
        // 5*8 = 40 bytes
        k_CRITICAL_SECTION_BUFFER_SIZE = 5
#else
        // 6*4 = 24 bytes
        k_CRITICAL_SECTION_BUFFER_SIZE = 6
#endif
    };

  private:
    enum {
        // A Windows critical section has a configurable spin count.  A lock
        // operation spins this many iterations (on, presumably, some atomic
        // integer) before sleeping on the underlying primitive.

        k_SPIN_COUNT = 30
    };

    // DATA
    void *d_lock[k_CRITICAL_SECTION_BUFFER_SIZE];

    // NOT IMPLEMENTED
    BslLockImpl_win32(const BslLockImpl_win32&);             // = delete
    BslLockImpl_win32& operator=(const BslLockImpl_win32&);  // = delete

  public:
    // CREATORS
    BslLockImpl_win32();
        // Create a lock object initialized to the unlocked state.

    ~BslLockImpl_win32();
        // Destroy this lock object.  The behavior is undefined unless this
        // object is in the unlocked state.

    // MANIPULATORS
    void lock();
        // Acquire the lock on this object.  If the lock on this object is
        // currently held by another thread, then suspend execution of the
        // calling thread until the lock can be acquired.  The behavior is
        // undefined unless the calling thread does not already hold the lock
        // on this object.  Note that deadlock may result if this method is
        // invoked while the calling thread holds the lock on the object.

    void unlock();
        // Release the lock on this object that was previously acquired through
        // a call to 'lock', enabling another thread to acquire the lock.  The
        // behavior is undefined unless the calling thread holds the lock on
        // this object.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // -----------------------
                              // class BslLockImpl_win32
                              // -----------------------

// CREATORS
inline
BslLockImpl_win32::BslLockImpl_win32()
{
    InitializeCriticalSectionAndSpinCount(
          reinterpret_cast<_RTL_CRITICAL_SECTION *>(d_lock), k_SPIN_COUNT);
}

inline
BslLockImpl_win32::~BslLockImpl_win32()
{
    DeleteCriticalSection(
        reinterpret_cast<_RTL_CRITICAL_SECTION*>(d_lock));
}

// MANIPULATORS
inline
void BslLockImpl_win32::lock()
{
    EnterCriticalSection(
        reinterpret_cast<_RTL_CRITICAL_SECTION*>(d_lock));
}

inline
void BslLockImpl_win32::unlock()
{
    LeaveCriticalSection(
        reinterpret_cast<_RTL_CRITICAL_SECTION*>(d_lock));
}


}  // close package namespace
}  // close enterprise namespace

#endif  // BSLS_PLATFORM_OS_WINDOWS

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
