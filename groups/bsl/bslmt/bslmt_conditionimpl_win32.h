// bslmt_conditionimpl_win32.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_CONDITIONIMPL_WIN32
#define INCLUDED_BSLMT_CONDITIONIMPL_WIN32

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bslmt::Condition'.
//
//@CLASSES:
//  bslmt::ConditionImpl<Win32Threads>: win32 specialization
//
//@SEE_ALSO: bslmt_condition
//
//@DESCRIPTION: This component provides an implementation of 'bslmt::Condition'
// for Windows (win32), 'bslmt::ConditionImpl<Win32Threads>', via the template
// specialization:
//..
//  bslmt::ConditionImpl<Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bslmt::Condition'.
//
///Supported Clock-Types
///---------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock on which timeouts supplied to other methods should be
// based.  If the clock type indicated at construction is
// 'bsls::SystemClockType::e_REALTIME', the timeout should be expressed as an
// absolute offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch
// used in 'bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)'.  If the
// clock type indicated at construction is
// 'bsls::SystemClockType::e_MONOTONIC', the timeout should be expressed as an
// absolute offset since the epoch of this clock (which matches the epoch used
// in 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// This component is an implementation detail of 'bslmt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <bslscm_version.h>

#include <bslmt_platform.h>

#ifdef BSLMT_PLATFORM_WIN32_THREADS

// Platform-specific implementation starts here.

#include <bslma_mallocfreeallocator.h>

#include <bsls_systemclocktype.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmt_sluice.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

// Rather than setting 'WINVER' or 'NTDDI_VERSION', just forward declare the
// Windows 2000 functions that are used.

struct _RTL_CONDITION_VARIABLE;
struct _RTL_CRITICAL_SECTION;

typedef struct _RTL_CONDITION_VARIABLE  CONDITION_VARIABLE,
                                       *PCONDITION_VARIABLE;
typedef struct _RTL_CRITICAL_SECTION CRITICAL_SECTION, *LPCRITICAL_SECTION;
typedef int BOOL;
typedef unsigned long DWORD;

extern "C" {
    __declspec(dllimport) BOOL __stdcall SleepConditionVariableCS(
        PCONDITION_VARIABLE  ConditionVariable,
        LPCRITICAL_SECTION   CriticalSection,
        DWORD                dwMilliseconds);


    __declspec(dllimport) void __stdcall InitializeConditionVariable(
        PCONDITION_VARIABLE ConditionVariable);

    __declspec(dllimport) void __stdcall WakeConditionVariable(
        PCONDITION_VARIABLE ConditionVariable);

    __declspec(dllimport) void __stdcall WakeAllConditionVariable(
        PCONDITION_VARIABLE ConditionVariable);

    __declspec(dllimport) DWORD __stdcall GetLastError();

}  // extern "C"

namespace BloombergLP {
namespace bslmt {

template <class THREAD_POLICY>
class ConditionImpl;

class Mutex;

               // ===========================================
               // class ConditionImpl<Platform::Win32Threads>
               // ===========================================

template <>
class ConditionImpl<Platform::Win32Threads> {
    // This class provides a full specialization of 'ConditionImpl' for win32.
    // The implementation provided here defines an efficient POSIX like
    // condition variable.

  private:
    // DATA
    void                        *d_cond;      // Condition variable is the size
                                              // of a pointer.

    bsls::SystemClockType::Enum  d_clockType; // clock type

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
        // Signal this condition object, by waking up *all* threads that are
        // currently waiting on this condition.

    void signal();
        // Signal this condition object, by waking up a single thread that is
        // currently waiting on this condition.

    int timedWait(Mutex *mutex, const bsls::TimeInterval& timeout);
        // Atomically unlock the specified 'mutex' and suspend execution of the
        // current thread until this condition object is "signaled" (i.e., one
        // of the 'signal' or 'broadcast' methods is invoked on this object) or
        // until the specified 'timeout', then re-acquire a lock on the
        // 'mutex'.  The 'timeout' is an absolute time represented as an
        // interval from some epoch, which is determined by the clock indicated
        // at construction (see {Supported Clock-Types} in the component
        // documentation).  Return 0 on success, -1 on timeout, and a non-zero
        // value different from -1 if an error occurs.  The behavior is
        // undefined unless 'mutex' is locked by the calling thread prior to
        // calling this method.  Note that 'mutex' remains locked by the
        // calling thread upon returning from this function.  Also note that
        // spurious wakeups are rare but possible, i.e., this method may
        // succeed (return 0) and return control to the thread without the
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
        // locked by the calling thread upon return from this function.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

               // -------------------------------------------
               // class ConditionImpl<Platform::Win32Threads>
               // -------------------------------------------

// CREATORS
inline
bslmt::ConditionImpl<bslmt::Platform::Win32Threads>::ConditionImpl(
                                         bsls::SystemClockType::Enum clockType)
: d_clockType(clockType)
{
    InitializeConditionVariable(
                         reinterpret_cast<_RTL_CONDITION_VARIABLE *>(&d_cond));
}

inline
bslmt::ConditionImpl<bslmt::Platform::Win32Threads>::~ConditionImpl()
{
}

// MANIPULATORS
inline
void bslmt::ConditionImpl<bslmt::Platform::Win32Threads>::broadcast()
{
    WakeAllConditionVariable(
                         reinterpret_cast<_RTL_CONDITION_VARIABLE *>(&d_cond));
}

inline
void bslmt::ConditionImpl<bslmt::Platform::Win32Threads>::signal()
{
    WakeConditionVariable(
                         reinterpret_cast<_RTL_CONDITION_VARIABLE *>(&d_cond));
}

}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_WIN32_THREADS

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
