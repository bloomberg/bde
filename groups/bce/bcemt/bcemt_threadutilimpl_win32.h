// bcemt_threadutilimpl_win32.h                                       -*-C++-*-
#ifndef INCLUDED_BCEMT_THREADUTILIMPL_WIN32
#define INCLUDED_BCEMT_THREADUTILIMPL_WIN32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a win32 implementation of 'bcemt_ThreadUtil'.
//
//@CLASSES:
//  bcemt_ThreadUtilImpl<Win32Threads>: win32 specialization
//
//@SEE_ALSO: bcemt_threadutil
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_ThreadUtil'
// for Windows (win32) via the template specialization:
//..
//  bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_ThreadUtil'.
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

#ifdef BCES_PLATFORM__WIN32_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
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

class bcemt_ThreadAttributes;

template <typename THREAD_POLICY>
struct bcemt_ThreadUtilImpl;

extern "C" {
    typedef void *(*bcemt_ThreadFunction)(void *);
        // 'bcemt_ThreadFunction' is an alias for a function type taking a
        // single 'void' pointer argument and returning 'void *'.  Such
        // functions are suitable to be specified as thread entry point
        // functions to 'bcemt_ThreadUtil::create'.  Note that 'create'
        // also
        // accepts 'bdef_Function<void(*)()>' objects as well.

    typedef void (*bcemt_KeyDestructorFunction)(void *);
        // 'bcemt_KeyDestructorFunction' is an alias for a function type taking
        // a single 'void' pointer argument and returning 'void'.  Such
        // functions are suitable to be specified as thread-specific key
        // destructor functions to 'bcemt_ThreadUtil::createKey'.
}

            // =======================================================
            // class bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>
            // =======================================================

template <>
struct bcemt_ThreadUtilImpl<bces_Platform::Win32Threads> {
    // This class provides a full specialization of 'bcemt_ThreadUtilImpl' for
    // Windows.

    // TYPES
    struct Handle {
        // Representation of a thread handle.  If a thread is created as
        // joinable, then a duplicate of to original handle is created
        // to be used in calls to 'join' and 'detach'.

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

    static int create(Handle                        *thread,
                      const bcemt_ThreadAttributes&  attribute,
                      bcemt_ThreadFunction           function,
                      void                          *userData);
        // Create a new thread of program control having the attributes
        // specified by 'attribute', that invokes the specified 'function'
        // with a single argument specified by 'userData' and load into the
        // specified 'threadHandle', an identifier that may be used to refer
        // to the thread in future calls to this utility.  Return 0 on success,
        // and a non-zero value otherwise.  The behavior is undefined if
        // 'thread' is 0.  Note that unless explicitly "detached"('detach'),
        // or unless the 'BCEMT_CREATE_DETACHED' attribute is specified, a
        // call to 'join' must be made once the thread terminates to reclaim
        // any system resources associated with the newly created identifier.

    static int create(Handle               *thread,
                      bcemt_ThreadFunction  function,
                      void                 *userData);
        // Create a new thread of program control having platform specific
        // default attributes(i.e., "stack size", "scheduling priority"), that
        // invokes the specified 'function' with a single argument specified
        // by 'userData', and load into the specified 'threadHandle', an
        // identifier that may be used to refer to the thread in future calls
        // to this utility.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined if 'thread' is 0.  Note that
        // unless explicitly "detached"('detach'), a call to 'join' must be
        // made once the thread terminates to reclaim any system resources
        // associated with the newly created identifier.

    static int getMinSchedPriority(int policy);
        // Return the non-negative minimum available priority for the
        // optionally-specified 'policy' on success, where 'policy' is of type
        // 'bcemt_ThreadAttributes::SchedulingPolicy'.  Return 'INT_MIN' on
        // error.  Note that for some platform / policy cominations,
        // 'getMinSchedPriority(policy) == getMaxSchedPriority(policy)'.

    static int getMaxSchedPriority(int policy);
        // Return the non-negative maximum available priority for the
        // optionally-specified 'policy' on success, where 'policy' is of type
        // 'bcemt_ThreadAttributes::SchedulingPolicy'.  Return 'INT_MIN' on
        // error.  Note that for some platform / policy cominations,
        // 'getMinSchedPriority(policy) == getMaxSchedPriority(policy)'.

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

    static void sleep(const bdet_TimeInterval& sleepTime);
        // Suspend execution of the current thread for a period of at least
        // the specified 'sleepTime' (relative time).  Note that the actual
        // time suspended depends on many factors including system scheduling,
        // and system timer resolution.  On the win32 platform the sleep timer
        // has a resolution of 1 millisecond.

    static void microSleep(int microseconds, int seconds = 0);
        // Suspend execution of the current thread for a period of at least
        // the specified 'seconds' and 'microseconds' (relative time).
        // Note that the actual time suspended depends on many factors
        // including system scheduling, and system timer resolution.  On the
        // win32 platform the sleep timer has a resolution of 1 millisecond.

    static void exit(void *status);
        // Exit the current thread and return the specified 'status'.  If
        // the current thread is not "detached", then a call to 'join' must be
        // made to reclaim any resources used by the thread, and to retrieve
        // the exit status.  Note that generally, the preferred method of
        // exiting a thread is to return form the entry point function.

    static Handle self();
        // Return a thread 'Handle' that can be used to refer to the current
        // thread.  The handle can be specified to any function that supports
        // operations on itself (e.g., 'detach', 'isEqual').  Note that the
        // returned handle is only valid in the context of the calling thread.

    static int detach(Handle& threadHandle);
        // "Detach" the thread identified by 'threadHandle', such that when
        // it terminates, the resources associated the thread will
        // automatically be reclaimed.  Note that once a thread is "detached",
        // it is no longer possible to 'join' the thread to retrieve the its
        // exit status.

    static NativeHandle nativeHandle(const Handle& threadHandle);
        // Return the platform specific identifier associated with the thread
        // specified by 'threadHandle'.

    static bool isEqual(const Handle& lhs, const Handle& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' thread handles
        // identify the same thread and a 'false' value otherwise.

    static Id selfId();
        // Return an identifier that can be used to uniquely identify the
        // current thread within the current process.  Note that the id is only
        // valid until the thread terminates and may be reused thereafter.

    static bsls_PlatformUtil::Uint64 selfIdAsInt();
        // Return an integral identifier that can be used to uniquely identify
        // the current thread within the current process.  This representation
        // is particularly useful for logging purposes.  Note that this value
        // is only valid until the thread terminates and may be reused
        // thereafter.
        //
        // DEPRECATED: Use 'selfIdAsUint64' instead.

    static bsls_PlatformUtil::Uint64 selfIdAsUint64();
        // Return an integral identifier that can be used to uniquely identify
        // the current thread within the current process.  This representation
        // is particularly useful for logging purposes.  Note that this value
        // is only valid until the thread terminates and may be reused
        // thereafter.

    static Id handleToId(const Handle& threadHandle);
        // Return the unique identifier of the thread having the specified
        // 'threadHandle' within the current process.  Note that this value is
        // only valid until the thread terminates and may be reused thereafter.

    static int idAsInt(const Id& threadId);
        // Return the unique integral identifier of a thread uniquely
        // identified by the specified 'threadId' within the current process.
        // This representation is particularly useful for logging purposes.
        // Note that this value is only valid until the thread terminates and
        // may be reused thereafter.

    static bool isEqualId(const Id& lhs, const Id& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' thread id identify
        // the same thread and 'false' otherwise.

    static int createKey(Key *key, bcemt_KeyDestructorFunction destructor);
        // Store into the specified 'key', an identifier that can be used to
        // associate('setSpecific') and retrieve('getSpecific') a single
        // thread-specific pointer value.  Associated with the identifier,the
        // optional 'destructor' if a non-zero value is specified.  Return 0 on
        // success, and a non-zero value otherwise.

    static int deleteKey(Key& key);
        // Delete the specified thread-specific 'key'.  Note that deleting
        // a key does not delete any data that is currently associated with
        // the key in the calling thread or any other thread.

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
bool operator==(
         const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& lhs,
         const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' thread handles have the
    // same value, and 'false' otherwise.

bool operator!=(
         const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& lhs,
         const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' thread handles do not
    // have the same value, and 'false' otherwise.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                             // ----------------------------
                             // bcemt_ThreadUtilImpl::Handle
                             // ----------------------------

// FREE OPERATORS
inline
bool operator==(
          const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& lhs,
          const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& rhs)
{
    return bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::isEqual(lhs,
                                                                      rhs);
}

inline
bool operator!=(
          const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& lhs,
          const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& rhs)
{
    return !(lhs == rhs);
}

            // -------------------------------------------------------
            // class bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>
            // -------------------------------------------------------

// CLASS METHODS
inline
int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::
                                                getMinSchedPriority(int policy)
{
    return -1;    // priorities not supported on Windows
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::
                                                getMaxSchedPriority(int policy)
{
    return -1;    // priorities not supported on Windows
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::yield()
{
    ::SleepEx(0, 0);
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::sleep(
                                            const bdet_TimeInterval& sleepTime)

{
    ::Sleep(static_cast<DWORD>(sleepTime.seconds()) * 1000 +
            static_cast<DWORD>(sleepTime.nanoseconds() / (1000 * 1000)));
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::microSleep(
                                                                 int microsecs,
                                                                 int seconds)
{
    ::Sleep(microsecs / 1000 + (seconds * 1000));
}

inline
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::self()
{
    Handle h;
    h.d_id     = GetCurrentThreadId();
    h.d_handle = GetCurrentThread();
    return h;
}

inline
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::NativeHandle
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::nativeHandle(
       const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& handle)
{
    return handle.d_handle;
}

inline
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Id
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::selfId()
{
    return GetCurrentThreadId();
}

inline
bsls_PlatformUtil::Uint64
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::selfIdAsInt()
{
    return static_cast<bsls_PlatformUtil::Uint64>(selfId());
}

inline
bsls_PlatformUtil::Uint64
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::selfIdAsUint64()
{
    return static_cast<bsls_PlatformUtil::Uint64>(selfId());
}

inline
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Id
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::handleToId(
                                                    const Handle& threadHandle)
{
    return threadHandle.d_id;
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::idAsInt(
                                                            const Id& threadId)
{
    return static_cast<int>(threadId);
}

inline
bool bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::isEqualId(
              const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Id& lhs,
              const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Id& rhs)
{
    return lhs == rhs;
}

inline
void *bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::getSpecific(
                                                                const Key& key)
{
    return TlsGetValue(key);
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::
                                                 setSpecific(const Key&  key,
                                                             const void *value)
{
    return 0 == TlsSetValue(key, (void*)value) ? 1 : 0;
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM__WIN32_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
