// bcemt_threadutilimpl_pthread.h                                     -*-C++-*-
#ifndef INCLUDED_BCEMT_THREADUTILIMPL_PTHREAD
#define INCLUDED_BCEMT_THREADUTILIMPL_PTHREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bcemt_ThreadUtil'.
//
//@CLASSES:
//  bcemt_ThreadUtilImpl<PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bcemt_threadutil
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_ThreadUtil'
// for POSIX threads ("pthreads") via the template specialization:
//..
//  bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>
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

#ifdef BCES_PLATFORM__POSIX_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BCEMT_THREADATTRIBUTES
#include <bcemt_threadattributes.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

namespace BloombergLP {

class bdet_TimeInterval;

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
            // class bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>
            // =======================================================

template <>
struct bcemt_ThreadUtilImpl<bces_Platform::PosixThreads> {
    // This class provides a full specialization of 'bcemt_ThreadUtilImpl' for
    // pthreads.

    // TYPES
    typedef pthread_t Handle;        // thread handle type
    typedef pthread_t NativeHandle;  // native thread handle type
    typedef pthread_t Id;            // thread Id type
    typedef pthread_key_t Key;       // thread-specific storage key type

    // CLASS DATA
    static const pthread_t INVALID_HANDLE;

    // CLASS METHODS
    static int create(Handle                        *thread,
                      const bcemt_ThreadAttributes&  attributes,
                      bcemt_ThreadFunction           function,
                      void                          *userData);
        // Create a new thread of program control having the specified
        // 'attributes' that invokes the specified 'function' with a single
        // argument specified by 'userData', and load into the specified
        // 'threadHandle' an identifier that may be used to refer to the thread
        // in future calls to this utility.  Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined if 'thread' is
        // 0.  Note that unless explicitly "detached" (by 'detach'), or unless
        // the 'BCEMT_CREATE_DETACHED' attribute is specified, a call to 'join'
        // must be made once the thread terminates to reclaim any system
        // resources associated with the newly created identifier.

    static int create(Handle               *thread,
                      bcemt_ThreadFunction  function,
                      void                 *userData);
        // Create a new thread of program control having platform specific
        // default attributes (i.e., "stack size", "scheduling priority"), that
        // invokes the specified 'function' with a single argument specified
        // by 'userData', and load into the specified 'threadHandle', an
        // identifier that may be used to refer to the thread in future calls
        // to this utility.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined if 'thread' is 0.  Note that
        // unless explicitly "detached" ('detach'), a call to 'join' must be
        // made once the thread terminates to reclaim any system resources
        // associated with the newly created identifier.

    static int getMinSchedulingPriority(
                              bcemt_ThreadAttributes::SchedulingPolicy policy);
        // Return the minimum available priority for the 'policy', where
        // 'policy' is of type 'bcemt_ThreadAttributes::SchedulingPolicy'.
        // Note that for some platform / policy cominations,
        // 'getMinSchedulingPriority(policy)' and
        // 'getMaxSchedulingPriority(policy)' return the same value.  The
        // behavior is undefined unless 'policy' is a valid value of enum
        // 'bcemt_ThreadAttributes::SchedulingPolicy'.

    static int getMaxSchedulingPriority(
                              bcemt_ThreadAttributes::SchedulingPolicy policy);
        // Return the maximum available priority for the 'policy', where
        // 'policy' is of type 'bcemt_ThreadAttributes::SchedulingPolicy'.
        // Note that for some platform / policy cominations,
        // 'getMinSchedulingPriority(policy)' and
        // 'getMaxSchedulingPriority(policy)' return the same value.  The
        // behavior is undefined unless 'policy' is a valid value of enum
        // 'bcemt_ThreadAttributes::SchedulingPolicy'.

    static int join(Handle& threadHandle, void **status = (void**)0);
        // Suspend execution of the current thread until the thread specified
        // by 'threadHandle' terminates, and reclaim any system resources
        // associated with the specified 'threadHandle'.  If the specified
        // 'status' is not 0, load into the specified 'status', the value
        // returned by the specified 'thread'.

    static int sleep(const bdet_TimeInterval&  sleepTime,
                     bdet_TimeInterval        *unsleptTime = 0);
        // Suspend execution of the current thread for a period of at least
        // the specified 'sleepTime' (relative time), and optionally load into
        // the specified 'unsleptTime' the amount of time that was not slept
        // by this function if the operation was interrupted by a signal.
        // Return 0 on success, and a non-zero value if the operation was
        // interrupted by a signal.  Note that the actual time suspended
        // depends on many factors including system scheduling, and system
        // timer resolution.

    static void yield();
        // Put the current thread to the end of the scheduler's queue and
        // schedule another thread to run.  This allows cooperating threads of
        // the same priority to share CPU resources equally.

    static int microSleep(int                microseconds,
                          int                seconds = 0,
                          bdet_TimeInterval *unsleptTime = 0);
        // Suspend execution of the current thread for a period of at least the
        // specified 'seconds' and microseconds (relative time), and optionally
        // load into the specified 'unsleptTime' the amount of time that was
        // not slept by this function if the operation was interrupted by a
        // signal.  Return 0 on success, and non-zero if the operation was
        // interrupted by a signal.  Note that the actual time suspended
        // depends on many factors including system scheduling, and system
        // timer resolution.  Note that the actual time suspended depends on
        // many factors including system scheduling, and system timer
        // resolution.

    static void exit(void *status);
        // Exit the current thread and return the specified 'status'.  If
        // the current thread is not "detached", then a call to 'join' must be
        // made to reclaim any resources used by the thread, and to retrieve
        // the exit status.  Note that generally, the preferred method of
        // exiting a thread is to return form the entry point function.

    static Handle self();
        // Return an identifier that can be used to refer to the current thread
        // in future calls to this utility.

    static int detach(Handle& threadHandle);
        // "Detach" the thread identified by 'threadHandle', such that when
        // it terminates, the resources associated the thread will
        // automatically be reclaimed.  Note that once a thread is "detached",
        // it is no longer possible to 'join' the thread to retrieve the its
        // exit status.

    static NativeHandle nativeHandle(const Handle& threadHandle);
        // Return the platform specific identifier associated with the thread
        // specified by 'threadHandle'.

    static bool isEqual(const Handle& a, const Handle& b);
        // Return 'true' if the specified 'a' and 'b' thread handles, identify
        // the same thread, and 'false' otherwise.

    static Id selfId();
        // Return an identifier that can be used to uniquely identify the
        // current thread within the current process.  Note that the id is only
        // valid until the thread terminates and may be reused thereafter.

    static bsls_PlatformUtil::Uint64 selfIdAsInt();
        // Return an integer of the unique identifier of the current thread
        // within the current process.  This representation is particularly
        // useful for logging purposes.  Note that this value is only valid
        // until the thread terminates and may be reused thereafter.
        //
        // DEPRECATED: Use 'selfIdAsUint64' instead.

    static bsls_PlatformUtil::Uint64 selfIdAsUint64();
        // Return an integer of the unique identifier of the current thread
        // within the current process.  This representation is particularly
        // useful for logging purposes.  Note that this value is only valid
        // until the thread terminates and may be reused thereafter.

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

    static bool isEqualId(const Id& a, const Id& b);
        // Return 'true' if the specified 'a' and 'b' thread id identify the
        // same thread, and 'false' otherwise.

    static int createKey(Key *key, bcemt_KeyDestructorFunction destructor);
        // Load, into the specified 'key', an identifier that can be used to
        // store ('setSpecific') and retrieve ('getSpecific') a single
        // thread-specific pointer value.  Associate with the identifier, the
        // optional 'destructor' if a non-zero value is specified.  Return 0 on
        // success, and a non-zero value otherwise.

    static int deleteKey(Key& key);
        // Delete the specified thread-specific 'key'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that deleting a key does not
        // delete any data that is currently associated with the key in the
        // calling thread or any other thread.

    static void *getSpecific(const Key& key);
        // Return the value associated with the specified thread-specific
        // 'key'.  Note that if the key is not valid, a value of zero is
        // returned, which is indistinguishable from a valid key with a 0
        // value.

    static int setSpecific(const Key& key, const void *value);
        // Associate the specified 'value' with the specified thread-specific
        // 'key'.  Return 0 on success, and a non-zero value otherwise.
        // TBD elaborate on what 'value' represents
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

            // -------------------------------------------------------
            // class bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>
            // -------------------------------------------------------

// CLASS METHODS
inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::create(
           bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle *handle,
           bcemt_ThreadFunction                                       function,
           void                                                      *userData)
{
    return pthread_create(handle, 0, function, userData);
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::join(
           bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle&   thread,
           void                                                       **status)
{
    return pthread_join(thread, status);
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::yield()
{
    sched_yield();
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::exit(void *status)
{
    pthread_exit(status);
}

inline
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::self()
{
    return pthread_self();
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::detach(
             bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle& handle)
{
    return pthread_detach(handle);
}

inline
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::NativeHandle
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::nativeHandle(
       const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle& handle)
{
    return handle;
}

inline
bool bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::isEqual(
            const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle& a,
            const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle& b)
{
    return pthread_equal(a, b);
}

inline
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Id
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::selfId()
{
    return pthread_self();
}

inline
bsls_PlatformUtil::Uint64
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::selfIdAsInt()
{
    return (bsls_PlatformUtil::Uint64)selfId();
}

inline
bsls_PlatformUtil::Uint64
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::selfIdAsUint64()
{
    return (bsls_PlatformUtil::Uint64)selfId();
}

inline
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Id
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::handleToId(
                                                    const Handle& threadHandle)
{
    return threadHandle;
}

inline
int
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::idAsInt(const Id& threadId)
{
    // Our interface is not good if the id is a pointer.  The two casts will
    // avoid a compilation error though.  TBD

    return (int)(bsls_PlatformUtil::IntPtr)threadId;
}

inline
bool bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::isEqualId(
                const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Id& a,
                const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Id& b)
{
    return pthread_equal(a, b);
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::createKey(
                                       Key                         *key,
                                       bcemt_KeyDestructorFunction  destructor)
{
    return pthread_key_create(key,destructor);
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::deleteKey(Key& key)
{
    return pthread_key_delete(key);
}

inline
void *bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::getSpecific(
                                                                const Key& key)
{
    return pthread_getspecific(key);
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::setSpecific(
                                                             const Key&  key,
                                                             const void *value)
{
    return pthread_setspecific(key, value);
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM__POSIX_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
