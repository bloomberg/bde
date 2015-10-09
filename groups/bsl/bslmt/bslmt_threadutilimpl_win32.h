// bslmt_threadutilimpl_win32.h                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_THREADUTILIMPL_WIN32
#define INCLUDED_BSLMT_THREADUTILIMPL_WIN32

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bslmt::ThreadUtil'.
//
//@CLASSES:
//  bslmt::ThreadUtilImpl<Win32Threads>: win32 specialization
//
//@SEE_ALSO: bslmt_threadutil
//
//@DESCRIPTION: This component provides an implementation of
// 'bslmt::ThreadUtil' for Windows (win32),
// 'bslmt::ThreadUtilImpl<Win32Threads>', via the template specialization:
//..
//  bslmt::ThreadUtilImpl<Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bslmt::ThreadUtil'.
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_PLATFORM
#include <bslmt_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifdef BSLMT_PLATFORM_WIN32_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BSLMT_SATURATEDTIMECONVERSIONIMPUTIL
#include <bslmt_saturatedtimeconversionimputil.h>
#endif

#ifndef INCLUDED_BSLMT_THREADATTRIBUTES
#include <bslmt_threadattributes.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

typedef unsigned long DWORD;
typedef int BOOL;
typedef void *HANDLE;

extern "C" {

    __declspec(dllimport) void __stdcall Sleep(
                DWORD dwMilliseconds
    );

    __declspec(dllimport) DWORD __stdcall SleepEx(
                DWORD dwMilliseconds,
                BOOL bAlertable
    );

    __declspec(dllimport) DWORD __stdcall GetCurrentThreadId(
                void
    );

    __declspec(dllimport) HANDLE __stdcall GetCurrentThread(
                void
    );

    __declspec(dllimport) void* __stdcall TlsGetValue(
                DWORD dwTlsIndex
    );

    __declspec(dllimport) BOOL __stdcall TlsSetValue(
                DWORD dwTlsIndex,
                void *lpTlsValue
    );

};

namespace BloombergLP {

extern "C" {
    typedef void *(*bslmt_ThreadFunction)(void *);
        // 'bslmt_ThreadFunction' is an alias for a function type taking a
        // single 'void' pointer argument and returning 'void *'.  Such
        // functions are suitable to be specified as thread entry point
        // functions to 'bslmt::ThreadUtil::create'.

    typedef void (*bslmt_KeyDestructorFunction)(void *);
        // 'bslmt_KeyDestructorFunction' is an alias for a function type taking
        // a single 'void' pointer argument and returning 'void'.  Such
        // functions are suitable to be specified as thread-specific key
        // destructor functions to 'bslmt::ThreadUtil::createKey'.
}

namespace bslmt {

template <class THREAD_POLICY>
struct ThreadUtilImpl;

               // ============================================
               // class ThreadUtilImpl<Platform::Win32Threads>
               // ============================================

template <>
struct ThreadUtilImpl<Platform::Win32Threads> {
    // This class provides a full specialization of 'ThreadUtilImpl' for
    // Windows.

    // TYPES
    struct Handle {
        // Representation of a thread handle.  If a thread is created as
        // joinable, then a duplicate of to original handle is created to be
        // used in calls to 'join' and 'detach'.

        HANDLE d_handle;  // win32 thread handle

        DWORD  d_id;      // duplicate of thread handle used for joinable
                          // threads
    };

    typedef HANDLE NativeHandle;
        // Native WIN32 thread handle type

    typedef DWORD  Id;
        // Win32 thread Id type

    typedef DWORD  Key;
        // Win32 thread specific key(TLS index)

    // CLASS METHODS
    static const Handle INVALID_HANDLE;

    static int create(Handle                  *thread,
                      const ThreadAttributes&  attribute,
                      bslmt_ThreadFunction     function,
                      void                    *userData);
        // Create a new thread of program control having the attributes
        // specified by 'attribute', that invokes the specified 'function' with
        // a single argument specified by 'userData' and load into the
        // specified 'threadHandle', an identifier that may be used to refer to
        // the thread in future calls to this utility.  Return 0 on success,
        // and a non-zero value otherwise.  The behavior is undefined if
        // 'thread' is 0.  Note that unless explicitly "detached"('detach'), or
        // unless the 'BSLMT_CREATE_DETACHED' attribute is specified, a call to
        // 'join' must be made once the thread terminates to reclaim any system
        // resources associated with the newly created identifier.

    static int create(Handle               *thread,
                      bslmt_ThreadFunction  function,
                      void                 *userData);
        // Create a new thread of program control having platform specific
        // default attributes(i.e., "stack size", "scheduling priority"), that
        // invokes the specified 'function' with a single argument specified by
        // 'userData', and load into the specified 'threadHandle', an
        // identifier that may be used to refer to the thread in future calls
        // to this utility.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined if 'thread' is 0.  Note that
        // unless explicitly "detached"('detach'), a call to 'join' must be
        // made once the thread terminates to reclaim any system resources
        // associated with the newly created identifier.

    static int getMinSchedulingPriority(
                                    ThreadAttributes::SchedulingPolicy policy);
        // Return the minimum available priority for the 'policy', where
        // 'policy' is of type 'ThreadAttributes::SchedulingPolicy'.  Return
        // 'ThreadAttributes::BSLMT_UNSET_PRIORITY' if the minimum scheduling
        // priority cannot be determined.  Note that, for some platform /
        // policy combinations, 'getMinSchedulingPriority(policy)' and
        // 'getMaxSchedulingPriority(policy)' return the same value.

    static int getMaxSchedulingPriority(
                                    ThreadAttributes::SchedulingPolicy policy);
        // Return the maximum available priority for the 'policy', where
        // 'policy' is of type 'ThreadAttributes::SchedulingPolicy'.  Return
        // 'ThreadAttributes::BSLMT_UNSET_PRIORITY' if the maximum scheduling
        // priority cannot be determined.  Note that, for some platform /
        // policy combinations, 'getMinSchedulingPriority(policy)' and
        // 'getMaxSchedulingPriority(policy)' return the same value.

    static int join(Handle& thread, void **status = 0);
        // Suspend execution of the current thread until the thread specified
        // by 'threadHandle' terminates, and reclaim any system resources
        // associated with the specified 'threadHandle'.  If the specified
        // 'status' is not 0, load into the specified 'status', the value
        // returned by the specified 'thread'.

    static void yield();
        // Put the current thread to the end of the scheduler's queue and
        // schedule another thread to run.  This allows cooperating threads of
        // the same priority to share CPU resources equally.

    static void sleep(const bsls::TimeInterval& sleepTime);
        // Suspend execution of the current thread for a period of at least the
        // specified 'sleepTime' (relative time).  Note that the actual time
        // suspended depends on many factors including system scheduling, and
        // system timer resolution.  On the win32 platform the sleep timer has
        // a resolution of 1 millisecond.

    static void microSleep(int microseconds, int seconds = 0);
        // Suspend execution of the current thread for a period of at least the
        // specified 'seconds' and 'microseconds' (relative time).  Note that
        // the actual time suspended depends on many factors including system
        // scheduling, and system timer resolution.  On the win32 platform the
        // sleep timer has a resolution of 1 millisecond.

    static int sleepUntil(const bsls::TimeInterval&   absoluteTime,
                          bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);
        // Suspend execution of the current thread until the specified
        // 'absoluteTime'.  Optionally specify 'clockType' which determines the
        // epoch from which the interval 'absoluteTime' is measured (see
        // {'Supported Clock-Types'} in the component documentation).  Return 0
        // on success, and a non-zero value otherwise.  The behavior is
        // undefined unless 'absoluteTime' represents a time after January 1,
        // 1970 and before the end of December 31, 9999 (i.e., a time interval
        // greater than or equal to 0, and less than 253,402,300,800 seconds).
        // Note that the actual time suspended depends on many factors
        // including system scheduling and system timer resolution.

    static void exit(void *status);
        // Exit the current thread and return the specified 'status'.  If the
        // current thread is not "detached", then a call to 'join' must be made
        // to reclaim any resources used by the thread, and to retrieve the
        // exit status.  Note that generally, the preferred method of exiting a
        // thread is to return form the entry point function.

    static Handle self();
        // Return a thread 'Handle' that can be used to refer to the current
        // thread.  The handle can be specified to any function that supports
        // operations on itself (e.g., 'detach', 'areEqual').  Note that the
        // returned handle is only valid in the context of the calling thread.

    static int detach(Handle& threadHandle);
        // "Detach" the thread identified by 'threadHandle', such that when it
        // terminates, the resources associated the thread will automatically
        // be reclaimed.  Note that once a thread is "detached", it is no
        // longer possible to 'join' the thread to retrieve the its exit
        // status.

    static NativeHandle nativeHandle(const Handle& threadHandle);
        // Return the platform specific identifier associated with the thread
        // specified by 'threadHandle'.  Note that the returned native handle
        // may not be a globally unique identifier for the thread (see
        // 'selfIdAsUint').

    static bool areEqual(const Handle& a, const Handle& b);
        // Return 'true' if the specified 'a' and 'b' thread handles identify
        // the same thread and a 'false' value otherwise.

    static Id selfId();
        // Return an identifier that can be used to uniquely identify the
        // current thread within the current process.  Note that the id is only
        // valid until the thread terminates and may be reused thereafter.

    static bsls::Types::Uint64 selfIdAsInt();
        // Return an integral identifier that can be used to uniquely identify
        // the current thread within the current process.  This representation
        // is particularly useful for logging purposes.  Note that this value
        // is only valid until the thread terminates and may be reused
        // thereafter.
        //
        // DEPRECATED: Use 'selfIdAsUint64' instead.

    static bsls::Types::Uint64 selfIdAsUint64();
        // Return an integral identifier that can be used to uniquely identify
        // the current thread within the current process.  This representation
        // is particularly useful for logging purposes.  Note that this value
        // is only valid until the thread terminates and may be reused
        // thereafter.

    static Id handleToId(const Handle& threadHandle);
        // Return the unique identifier of the thread having the specified
        // 'threadHandle' within the current process.  Note that this value is
        // only valid until the thread terminates and may be reused thereafter.

    static bsls::Types::Uint64 idAsUint64(const Id& threadId);
        // Return the unique integral identifier of a thread uniquely
        // identified by the specified 'threadId' within the current process.
        // Note that this representation is particularly useful for logging
        // purposes.  Also note that this value is only valid until the thread
        // terminates and may be reused thereafter.

    static int idAsInt(const Id& threadId);
        // Return the unique integral identifier of a thread uniquely
        // identified by the specified 'threadId' within the current process.
        // Note that this representation is particularly useful for logging
        // purposes.  Also note that this value is only valid until the thread
        // terminates and may be reused thereafter.
        //
        // DEPRECATED: use 'idAsUint64'.

    static bool areEqualId(const Id& a, const Id& b);
        // Return 'true' if the specified 'a' and 'b' thread id identify the
        // same thread and 'false' otherwise.

    static int createKey(Key *key, bslmt_KeyDestructorFunction destructor);
        // Store into the specified 'key', an identifier that can be used to
        // associate('setSpecific') and retrieve('getSpecific') a single
        // thread-specific pointer value.  Associated with the identifier,the
        // optional 'destructor' if a non-zero value is specified.  Return 0 on
        // success, and a non-zero value otherwise.

    static int deleteKey(Key& key);
        // Delete the specified thread-specific 'key'.  Note that deleting a
        // key does not delete any data that is currently associated with the
        // key in the calling thread or any other thread.

    static void *getSpecific(const Key& key);
        // Return the value associated with the specified thread-specific
        // 'key'.  Note that if the key is not valid, a value of zero is
        // returned, which is indistinguishable from a valid key with a 0
        // value.

    static int setSpecific(const Key& key, const void *value);
        // Associate the specified 'value' with the specified thread-specific
        // 'key'.  Return 0 on success, and a non-zero value otherwise.
};

// FREE OPERATORS
bool operator==(const ThreadUtilImpl<Platform::Win32Threads>::Handle& lhs,
                const ThreadUtilImpl<Platform::Win32Threads>::Handle& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' thread handles have the
    // same value, and 'false' otherwise.

bool operator!=(const ThreadUtilImpl<Platform::Win32Threads>::Handle& lhs,
                const ThreadUtilImpl<Platform::Win32Threads>::Handle& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' thread handles do not
    // have the same value, and 'false' otherwise.

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // ThreadUtilImpl::Handle
                          // ----------------------

// FREE OPERATORS
inline
bool bslmt::operator==(
              const ThreadUtilImpl<bslmt::Platform::Win32Threads>::Handle& lhs,
              const ThreadUtilImpl<bslmt::Platform::Win32Threads>::Handle& rhs)
{
    return ThreadUtilImpl<Platform::Win32Threads>::areEqual(lhs, rhs);
}

inline
bool bslmt::operator!=(
              const ThreadUtilImpl<bslmt::Platform::Win32Threads>::Handle& lhs,
              const ThreadUtilImpl<bslmt::Platform::Win32Threads>::Handle& rhs)
{
    return !(lhs == rhs);
}

               // --------------------------------------------
               // class ThreadUtilImpl<Platform::Win32Threads>
               // --------------------------------------------

// CLASS METHODS
inline
int bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::
    getMinSchedulingPriority(ThreadAttributes::SchedulingPolicy)
{
    return ThreadAttributes::e_UNSET_PRIORITY;
}

inline
int bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::
    getMaxSchedulingPriority(ThreadAttributes::SchedulingPolicy)
{
    return ThreadAttributes::e_UNSET_PRIORITY;
}

inline
void bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::yield()
{
    ::SleepEx(0, 0);
}

inline
void bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::sleep(
                                           const bsls::TimeInterval& sleepTime)

{
    DWORD milliSeconds;
    SaturatedTimeConversionImpUtil::toMillisec(&milliSeconds, sleepTime);

    ::Sleep(milliSeconds);
}

inline
void bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::microSleep(
                                                                 int microsecs,
                                                                 int seconds)
{
    enum { k_MILLION = 1000 * 1000 };

    bsls::TimeInterval ti((microsecs / k_MILLION) + seconds,
                         (microsecs % k_MILLION) * 1000);
    DWORD milliSeconds;
    SaturatedTimeConversionImpUtil::toMillisec(&milliSeconds, ti);

    ::Sleep(milliSeconds);
}

inline
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::Handle
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::self()
{
    Handle h;
    h.d_id     = GetCurrentThreadId();
    h.d_handle = GetCurrentThread();
    return h;
}

inline
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::NativeHandle
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::nativeHandle(
                                                          const Handle& handle)
{
    return handle.d_handle;
}

inline
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::Id
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::selfId()
{
    return GetCurrentThreadId();
}

inline
bsls::Types::Uint64
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::selfIdAsInt()
{
    return idAsInt(selfId());
}

inline
bsls::Types::Uint64
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::selfIdAsUint64()
{
    return idAsUint64(selfId());
}

inline
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::Id
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::handleToId(
                                                    const Handle& threadHandle)
{
    return threadHandle.d_id;
}

inline
bsls::Types::Uint64
bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::idAsUint64(
                                                            const Id& threadId)
{
    return static_cast<bsls::Types::Uint64>(threadId);
}

inline
int bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::idAsInt(
                                                            const Id& threadId)
{
    return static_cast<int>(threadId);
}

inline
bool bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::areEqualId(
                                                                   const Id& a,
                                                                   const Id& b)
{
    return a == b;
}

inline
void *bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::getSpecific(
                                                                const Key& key)
{
    return TlsGetValue(key);
}

inline
int bslmt::ThreadUtilImpl<bslmt::Platform::Win32Threads>::setSpecific(
                                                             const Key&  key,
                                                             const void *value)
{
    return 0 == TlsSetValue(key, (void*)value) ? 1 : 0;
}

}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_WIN32_THREADS

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
