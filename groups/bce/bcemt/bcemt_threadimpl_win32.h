// bcemt_threadimpl_win32.h        -*-C++-*-

#ifndef INCLUDED_BCEMT_THREADIMPL_WIN32
#define INCLUDED_BCEMT_THREADIMPL_WIN32

//@PURPOSE: Provide a full specialization of bcemt_thread for WIN32.
//
//@CLASSES:
//      bcemt_AttributeImpl: WIN32 specialization of bcemt_AttributeImpl
//     bcemt_ThreadUtilImpl: WIN32 specialization of bcemt_ThreadUtilImpl
//      bcemt_ConditionImpl: WIN32 specialization of bcemt_ConditionImpl
//          bcemt_MutexImpl: WIN32 specialization of bcemt_MutexImpl
// bcemt_RecursiveMutexImpl: WIN32 specialization of bcemt_RecursiveMutexImpl
//
//@SEE_ALSO: bcemt_thread
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a full specialization of the
// bcemt_thread component for the WIN32 platform.  This component is a platform
// specific implementation detail and is not intended to be used directly.
// Clients should use the 'bcemt_threadimpl' component.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BDETU_SYSTEMTIME
#include <bdetu_systemtime.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifndef INCLUDED_BDES_ASSERT
#include <bdes_assert.h>
#endif

#ifdef BCES_PLATFORM__WIN32_THREADS

// The following include declarations include 'windows.h' without including
// many of the unnecessary "junk" that windows includes.  The
// 'WIN32_LEAN_AND_MEAN' macro is not already defined by the client, then
// temporarily define it before including windows, and then undefine it again.
#ifndef INCLUDED_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
        #define INCLUDED_WINDOWS
        #undef  WIN32_LEAN_AND_MEAN
    #else
        #include <windows.h>
        #define INCLUDED_WINDOWS
    #endif
#endif

#ifndef INCLUDED_CTIME
#include <ctime>
#define INCLUDED_CTIME
#endif

#ifndef INCLUDED_CERRNO
#include <cerrno>
#define INCLUDED_CERRNO
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_AttributeImpl;

template <typename THREAD_POLICY>
struct bcemt_ThreadUtilImpl;

template <typename THREAD_POLICY>
class bcemt_MutexImpl;

template <typename THREAD_POLICY>
class bcemt_RecursiveMutexImpl;

template <typename THREAD_POLICY>
class bcemt_ConditionImpl;

template <typename THREAD_POLICY>
class bcemt_SemaphoreImpl;

                           // =========================
                           // class bcemt_AttributeImpl
                           // =========================


template <>
class bcemt_AttributeImpl<bces_Platform::Win32Threads> {
    // This class provides a full specialization of bcemt_AttributeImpl for
    // the WIN32 platform.  This class contains attributes used to create
    // threads.  Note that many attributes specified here have no effect on
    // the WIN32 platform.

  public:
    enum DetachedState {
        CREATE_JOINABLE, // The current thread can be 'join'ed with
        CREATE_DETACHED  // The current thread can not be 'join'ed with
    };

    enum SchedulingPolicy {
        BCEMT_SCHED_OTHER,  // Default OS scheduling policy
        BCEMT_SCHED_FIFO,   // First-In-First-Out scheduling policy
        BCEMT_SCHED_RR      // Round-Robin scheduling policy
    };

  private:
    friend struct bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>;

    int              d_stackSize;        // stack size attribute
    int              d_priority;         // scheduling priority
    int              d_guardSize;        // guard size(not used on WIN32)
    SchedulingPolicy d_schedulingPolicy; // scheduling policy
                                         // (not used in WIN32)
    DetachedState    d_detachedState;    // detached attribute
    int              d_inheritSchedule;  // inherit schedule attribute(not used
                                         // in WIN32)


  public:
    // CREATORS
    bcemt_AttributeImpl();
        // Create a thread attribute object with the following default values:
        //..
        //      Attribute             Default Value
        //      -------------------+----------------------------
        //      DetachedState      |  CREATE_JOINABLE
        //      SchedulingPolicy   |  BCEMTI_SCHED_OTHER
        //      SchedulingPriority |  *platform's default value*
        //      InheritSchedule    |  TRUE
        //      StackSize          |  *platform's default value*
        //      GuardSize          |  *platform's default value*
        //..

    bcemt_AttributeImpl(const bcemt_AttributeImpl& original);
        // Create a thread attribute object initialized with the values in the
        // specified 'original' attribute object.

    ~bcemt_AttributeImpl();

     bcemt_AttributeImpl& operator=(const bcemt_AttributeImpl& rhs);
        // Assign to this attribute object the value of the specified 'rhs'
        // attribute object and return a reference to this modifiable
        // attribute object.

    // MANIPULATORS
    void setDetachedState(DetachedState detachedState);
        // Set the value of the detached state attribute in this object to the
        // specified 'detachedState'.

    void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        // Set the value of the scheduling policy attribute in this object to
        // the specified 'schedulingPolicy'.

    void setSchedulingPriority(int schedulingPriority);
        // Set the value of the scheduling priority attribute in this object
        // to the specified 'schedulingPriority'.

    void setInheritSchedule(int inheritSchedule);
        // Set the value of the inherit schedule attribute to 0 if
        // 'inheritSchedule' is 0 and set it to 1 otherwise.  A value of 0 for
        // this attribute will indicate that a thread should *not* inherit the
        // scheduling policy and priority of the thread that created it, and a
        // value of 1 indicates that the thread should inherit these
        // attributes.  Note that if the attributes are inherited, the values
        // of the scheduling policy and priority attributes in the
        // 'bcemt_AttributeImpl' object (supplied to
        // 'bcemt_ThreadUtilImpl::create' to spawn the thread) are ignored.

    void setStackSize(int stackSize);
        // Set the value of the stack size attribute in this object
        // to the specified 'stackSize' (in bytes).  Note that if
        // 'stackSize' < 0, the default value as defined by the platform is
        // used.

    void setGuardSize(int guardSize);
        // Set the value of the guard size attribute in this object
        // to the specified 'guardSize' (in bytes).  Note that if
        // 'guardSize' < 0, the default value as defined by the platform is
        // used (which is typically the size of a page).

    // ACCESSORS
    DetachedState detachedState() const;
        // Return the value of the detached state attribute in this object.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the scheduling policy attribute in this object.

    int schedulingPriority() const;
        // Return the value of the scheduling priority attribute in this
        // object.  A value of -1 indicates that the scheduling priority is
        // set the othe platform's default value.

    int inheritSchedule() const;
        // Return 1 if threads (created with this object) will inherit the
        // "parent" threads scheduling policy and priority, and 0 if they do
        // not.

    int stackSize() const;
        // Return the value of the stack size attribute in this object.

    int guardSize() const;
        // Return the value of the guard size attribute in this object.
    
    static int getMinSchedPriority(int policy = -1);
        // Return the non-negative minimun available priority for the
        // optionnally-specified 'policy' on success. If no policy is
        // specified, the minimum priority for the current policy is returned.
        // Return a negative value on error.
        // On Win32, this methods always returns -1.

    static int getMaxSchedPriority(int policy = -1);
        // Return the non-negative maximum available priority for the
        // optionnally-specified 'policy' on success. If no policy is
        // specified, the maximum priority for the current policy is returned.
        // Return a negative value on error.
        // On Win32, this methods always returns -1.
};

inline
int operator==(const bcemt_AttributeImpl<bces_Platform::Win32Threads>& lhs,
               const bcemt_AttributeImpl<bces_Platform::Win32Threads>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects have the
    // same value, and a 0 value otherwise.

inline
int operator!=(const bcemt_AttributeImpl<bces_Platform::Win32Threads>& lhs,
               const bcemt_AttributeImpl<bces_Platform::Win32Threads>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects do not have
    // the same value, and a 0 value otherwise.

            // ====================================================
            // class bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>
            // ====================================================

extern "C" {
    typedef void* (*bcemt_ThreadFunction)(void *);
        // This type declares the prototype for functions that are suiteable
        // to be specified as thread entrypoint functions to
        // 'bcemt_ThreadUtil::create'.

    typedef void (*bcemt_KeyDestructorFunction)(void *);
        // This type declares the prototype for functions that are suiteable
        // to be specified as thread specific key destructor functions to
        // "bcemt_ThreadUtil::createKey".
}

template <>
struct bcemt_ThreadUtilImpl<bces_Platform::Win32Threads> {
    // This class provides a full specializaion of 'bcemt_ThreadUtilImpl' for
    // WIN32 threads.

    struct Handle {
        // Representation of a thread handle.  If a thread is created as
        // joinable, then a duplicate of to original handle is created
        // to be used in calls to 'join' and 'detach'.

        HANDLE d_handle;  // Actual WIN32 thread handle
        DWORD  d_id;      // Duplicate of thread handle used for joinable
                          // threads
    };

    typedef HANDLE NativeHandle;
        // Native WIN32 thread handle type

    typedef DWORD  Id;
        // Win32 thread Id type

    typedef DWORD  Key;
        // Win32 thread specific key(TLS index)

    static const Handle INVALID_HANDLE;

    static int create(
             Handle *thread,
             const bcemt_AttributeImpl<bces_Platform::Win32Threads>& attribute,
             bcemt_ThreadFunction function,
             void *userData);
        // Create a new thread of program control having the attributes
        // specified by 'attribute', that invokes the specified 'function'
        // with a single argument specified by 'userData' and  load into the
        // specified 'threadHandle', an identifier that may be used to refer
        // to the thread in future calls to this utility. Return 0 on success,
        // and a non-zero value otherwise.  The behavior is undefined if
        // 'thread' is 0. Note that unless explicitly "detached"('detach'),
        // or unless the CREATE_DETACHED attribute is specified,  a call to
        // 'join' must be made once the thread terminates to reclaim any system
        // resources associated with the newly created identifier.

    static int create(Handle *thread,
                      bcemt_ThreadFunction function,
                      void *userData);
        // Create a new thread of program control having platform specific
        // default attributes(i.e. "stack size", "scheduling priority"), that
        // invokes the specified 'function' with a single argument specified
        // by 'userData', and load into the  specified 'threadHandle', an
        // identifier that may be used to refer to the thread in future calls
        // to this utility. Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined if 'thread' is 0.  Note that
        // unless explicitly "detached"('detach'), a call to 'join' must be
        // made once the thread terminates to reclaim any system resources
        // associated with the newly created identifier.

    static int join(Handle &thread, void **status = 0);
        // Suspend execution of the current thread until the thread specified
        // by 'threadHandle' terminates, and reclaim any system resources
        // associated with the specified 'threadHandle'.  If the specified
        // 'status' is not 0, load into the specified 'status', the value
        // returned by the specified 'thread'.

    static void yield();
        // Put the current thread to the end of the scheduler's queue and
        // schedule another thread to run. This allows cooperating threads of
        // the same priority to share CPU ressources equally.

    static void sleep(const bdet_TimeInterval &sleepTime);
        // Suspend execution of the current thread for a period of at least
        // the specified 'sleepTime'(relative time).  Note that the actual
        // time suspended depends on many factors including system scheduling,
        // and system timer resolution.

    static void microSleep(int microseconds, int seconds=0);
        // Suspend execution of the current thread for a period of at least
        // the specified 'seconds' and microseconds(relative time).
        // Note that the actual time suspended depends on many factors
        // including system scheduling, and system timer resolution. On the
        // WIN32 platform the sleep timer has a resolution 1 millisecond.

    static void exit(void *status);
        // Exit the current thread and return the specified 'status'.  If
        // the current thread is not "detached", then a call to 'join' must be
        // made to reclaim any resources used by the thread, and to retrieve
        // the exit status.  Note that generally, the preferred method of
        // exiting a thread is to return form the entry point function.

    static Handle self();
        // Return a thread 'Handle' which can be used to refer to the current
        // thread.  The handle can be specified to any function which supports
        // operations on itself calling(e.g.,'detach','isEqual').  Note that
        // the returned handle is only valid in the context of the calling
        // thread.

    static int detach(Handle &threadHandle);
        // "Detach" the thread identified by 'threadHandle', such that when
        // it terminates, the resources associated the thread will
        // automatically be reclaimed.  Note that once a thread is "detached",
        // it is no longer possible to 'join' the thread to retrieve the its
        // exit status.

    static NativeHandle nativeHandle(const Handle &threadHandle);
        // Return the platform specific identifier associated with the thread
        // specified by 'threadHandle'.

    static int isEqual(const Handle &lhs, const Handle &rhs);
        // Return 1 if the specified 'lhs' and 'rhs' thread handles identify
        // the same thread and a zero value otherwise.

    static Id selfId();
        // Return an identifier that can be used to uniquely identify the
        // current thread within the current process.  Note that the id is only
        // valid until the thread terminates and may be reused thereafter.
 
    static int selfIdAsInt();
        // Return an integer of the unique identifier of the current thread
        // within the current process.  This representation is particularly
        // useful for logging purposes.  Note that this value is only valid
        // until the thread terminates and may be reused thereafter.

    static int isEqualId(const Id &lhs, const Id &rhs);
        // Return 1 if the specified 'lhs' and 'rhs' thread id identify
        // the same thread and 0 otherwise.

    static int createKey(Key *key, bcemt_KeyDestructorFunction destructor);
        // Store into the specified 'key', an identifier which can be used to
        // associate('setSpecific') and retrieve('getSpecific') a single
        // thread-specific pointer value. Associated with the identifier,the
        // optional 'destructor' if a non-zero value is specified. Return 0 on
        // success, and a non-zero value otherwise.

    static int deleteKey(Key &key);
        // Delete the specified thread-specific 'key'.  Note that deleting
        // a key does not delete any data that is currently associated with
        // the key in the calling thread or any other thread.

    static void* getSpecific(const Key &key);
        // Return the value associated with the specified thread-specific
        // 'key'. Note that if the key is not valid, a value of zero is
        // returned, which is indistinguishable from a valid key with a 0
        // value.

    static int setSpecific(const Key &key, const void * value);
        // Associate the specified 'value' with the specified thread-specific
        // 'key'. Return 0 on success, and a non-zero value otherwise.

    static int getConcurrency();
        // Return the current threads implementation concurrency level or -1
        // if not supported. Note that since all windows threads are kernel
        // threads, this method always returns -1.

    static void setConcurrency(int newLevel);
        // Set the desired thread concurrency level to the specified
        // 'newLevel'.  Note that this method has no effect on the windows
        // platform since all threads are kernel threads.

};


               // ===============================================
               // class bcemt_MutexImpl<bces_Platform::Win32Threads>
               // ===============================================

template <>
class bcemt_MutexImpl<bces_Platform::Win32Threads> {
    // This class provides a full specializaion of 'bcemt_MutexImpl' for
    // WIN32 threads.  Note that mutex implemented here classs, is non error
    // checking, and non-recursive.

    volatile LONG d_lockCount; // atomic count of number of actve/lock requests
    HANDLE d_semaphore;        // semaphore used for blocking

    // not implemented
    bcemt_MutexImpl(const bcemt_MutexImpl&);
    bcemt_MutexImpl& operator=(const bcemt_MutexImpl&);

  public:
    // CREATORS
    bcemt_MutexImpl();
        // Create a mutex initialized to an unlocked state.

    ~bcemt_MutexImpl();
        // Destroy this mutex object.

    // MANIPULATORS
    void lock();
        // Acquire a lock on this mutex object.  If this object is currently
        // locked, then suspend execution of the current thread until a
        // lock can be acquired.  Note that the behaviour is undefined if the
        // calling thread already owns the lock on this mutex, and will likely
        // result in a deadlock.

    int tryLock();
        // Attempt to aquire a lock on this mutex object.  Return 0 on success,
        // and a non-zero value of this object is already locked, or if an
        // error occurs.

    void unlock();
        // Release a lock on this mutex that was previously acquired through a
        // successful call to 'lock', or 'tryLock'.  The behaviour is
        // undefined, unless the calling thread currenly owns the lock on this
        // mutex.
};


template <>
class bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads> {
    // This class provides a full specializaion of 'bcemt_RecursiveMutexImpl'
    // for pthreads.  If the pthreads implementation supports the "recursive"
    // attribute, then the native implemenation is used, otherwise, a portable,
    // efficient implementation is provided.

    LONG   d_lockCount;     // atomic count of number of actve/lock requests
    DWORD  d_ownerId;       // id of the thread that currently owns this lock
    HANDLE d_semaphore;     // semaphore used for blocking
    int    d_instanceCount; // current lock recursion level

    // not implemented
    bcemt_RecursiveMutexImpl(const bcemt_RecursiveMutexImpl&);
    bcemt_RecursiveMutexImpl& operator=(const bcemt_RecursiveMutexImpl&);


  public:
    // CREATORS
    bcemt_RecursiveMutexImpl();
        // Create a recursive mutex initialized to unlocked state.

    ~bcemt_RecursiveMutexImpl();
        // Destroy this recursive mutex.

    // MANIPULATORS
    void lock();
        // Acquire a lock on this mutex object.  If this object is currently
        // locked by a different thread, then suspend execution of the current
        // thread until a lock can be acquired.  Otherwise, if it unlocked,
        // or locked by the calling thread, then grant ownership of the lock
        // immediately and return.  Note that when this object is recursively
        // locked by a thread, 'unlock' must be called an equal number of
        // times before the lock is actually released.

    int tryLock();
        // Attempt to acquire a lock on this mutex object.  If this object is
        // unlocked, or locked by the calling thread, then grant ownership of
        // the lock immediately and return 0. Otherwise If this object is
        // currently locked by a different thread or if an error occurs, then
        // return a non-zero value.  Note that when this object is recursively
        // locked by a thread, 'unlock' must be called an equal number of times
        //  before the lock is actually released.

    void unlock();
        // Release a lock on this mutex that was previously acquired through a
        // successful call to 'lock', or 'tryLock'.  The behaviour is
        // undefined, unless the calling thread currenly owns the lock on this
        // mutex.  Note that when this object is recursively locked by a
        // thread, 'unlock' must be called an equal number of times before the
        // lock is actually released.
};


             // ===================================================
             // class bcemt_ConditionImpl<bces_Platform::Win32Threads>
             // ===================================================

template <>
class bcemt_ConditionImpl<bces_Platform::Win32Threads> {
    // This class provides a full specializaion of 'bcemt_RecursiveMutexImpl'
    // for WIN32.  The implemenation provided here defines an efficient
    // POSIX like condition variable.

    CRITICAL_SECTION d_crit;      // critical section used to control access
                                  // the condition
    HANDLE           d_semaphore; // Handle to a semaphore used for the actual
                                  // signalling.
    volatile LONG    d_waitCount; // Count of how many threads are currently
                                  // waiting for this condition

    // not implemented
    bcemt_ConditionImpl(const bcemt_ConditionImpl&);
    bcemt_ConditionImpl& operator=(const bcemt_ConditionImpl&);

  public:
    // CREATORS
    bcemt_ConditionImpl();
        // Create a condition variable.

    ~bcemt_ConditionImpl();
        // Destroy condition variable this object.

    // MANIPULATORS
    int wait(bcemt_MutexImpl<bces_Platform::Win32Threads> *mutex);
        // Atomically unlock the specified 'mutex' and suspend execution of
        // current thread until this condition object is "signaled"('signal',
        // 'broadcast') then re-acquire the lock on the specified 'mutex', and
        // return 0 upon success and non-zero otherwise. Note that the behavior
        // is undefined unless specified 'mutex' is locked by the calling
        // thread prior to calling this method.

    int timedWait(bcemt_MutexImpl<bces_Platform::Win32Threads> *mutex,
                  const bdet_TimeInterval& timeout);
        // Atomically unlock the specified 'mutex' and suspend execution of
        // current thread until this condition object is "signaled"('signal',
        // 'broadcast') or until the specified 'timeout' (expressed as the
        // absolute time from 00:00:00 UTC, January 1, 1970), then re-acquire
        // the lock on the specified 'mutex', and return 0 upon success and
        // non-zero if an error or timeout occured. Note that the behavior is
        // undefined unless specified 'mutex' is locked by the calling thread
        // prior to calling this method.

    void signal();
        // Signal this condition object; wake up a single thread that is
        // currently waiting on this condition.

    void broadcast();
        // Signal this condition object; wake up all threads that are currently
        // waiting on this condition.
};

template <>
class bcemt_SemaphoreImpl<bces_Platform::Win32Threads> {
    //

    HANDLE d_handle;

    // not implemented
    bcemt_SemaphoreImpl(const bcemt_SemaphoreImpl&);
    bcemt_SemaphoreImpl& operator=(const bcemt_SemaphoreImpl&);

public:
    // CREATORS
    bcemt_SemaphoreImpl(int count = 0);
    // Create a semaphore.

    ~bcemt_SemaphoreImpl();
    // Destroy a semaphore

    // MANIPULATORS
    void post();

    void post(int number);

    int tryWait();

    void wait(int *signalInterrupted=0);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                              // -------------------
                              // bcemt_AttributeImpl
                              // -------------------

inline
bcemt_AttributeImpl<bces_Platform::Win32Threads>::bcemt_AttributeImpl(
          const bcemt_AttributeImpl<bces_Platform::Win32Threads>& attribute)
: d_stackSize(attribute.d_stackSize)
, d_priority(attribute.d_priority)
, d_guardSize(attribute.d_guardSize)
, d_schedulingPolicy(attribute.d_schedulingPolicy)
, d_detachedState(attribute.d_detachedState)
, d_inheritSchedule(attribute.d_inheritSchedule)
{
}

inline
bcemt_AttributeImpl<bces_Platform::Win32Threads>::bcemt_AttributeImpl()
: d_stackSize(0)
, d_priority(0)
, d_guardSize(0)
, d_schedulingPolicy(BCEMT_SCHED_OTHER)
, d_detachedState(CREATE_JOINABLE)
, d_inheritSchedule(1)
{

}

inline
bcemt_AttributeImpl<bces_Platform::Win32Threads>::~bcemt_AttributeImpl()
{

}

inline
bcemt_AttributeImpl<bces_Platform::Win32Threads>&
bcemt_AttributeImpl<bces_Platform::Win32Threads>::operator=(
                   const bcemt_AttributeImpl<bces_Platform::Win32Threads>& rhs)
{
  d_stackSize = rhs.d_stackSize;
  d_priority = rhs.d_priority;
  d_guardSize = rhs.d_guardSize;
  d_schedulingPolicy = rhs.d_schedulingPolicy;
  d_detachedState = rhs.d_detachedState;
  d_inheritSchedule = rhs.d_inheritSchedule;
  return *this;
}

inline
void bcemt_AttributeImpl<bces_Platform::Win32Threads>::setDetachedState(
                bcemt_AttributeImpl<bces_Platform::Win32Threads>::DetachedState
                                                                 detachedState)
{
    d_detachedState = detachedState;
}

inline
void bcemt_AttributeImpl<bces_Platform::Win32Threads>::setSchedulingPolicy(
    bcemt_AttributeImpl<bces_Platform::Win32Threads>::SchedulingPolicy
                                                              schedulingPolicy)
{
    d_schedulingPolicy = schedulingPolicy;
}

inline void
bcemt_AttributeImpl<bces_Platform::Win32Threads>::setSchedulingPriority(
                                                        int schedulingPriority)
{
    d_priority = schedulingPriority;
}

inline void
bcemt_AttributeImpl<bces_Platform::Win32Threads>::setInheritSchedule(
                                                           int inheritSchedule)
{
    d_inheritSchedule = inheritSchedule ? 1 : 0;
}

inline
void bcemt_AttributeImpl<bces_Platform::Win32Threads>::setStackSize(
                                                                 int stackSize)
{
    d_stackSize = stackSize;
}

inline
void bcemt_AttributeImpl<bces_Platform::Win32Threads>::setGuardSize(
                                                                 int guardSize)
{
    d_guardSize = guardSize;

}

inline bcemt_AttributeImpl<bces_Platform::Win32Threads>::DetachedState
bcemt_AttributeImpl<bces_Platform::Win32Threads>::detachedState() const
{
    return d_detachedState;
}

inline bcemt_AttributeImpl<bces_Platform::Win32Threads>::SchedulingPolicy
bcemt_AttributeImpl<bces_Platform::Win32Threads>::schedulingPolicy() const
{
    return d_schedulingPolicy;
}

inline
int bcemt_AttributeImpl<bces_Platform::Win32Threads>::stackSize() const
{
    return d_stackSize;
}

inline int
bcemt_AttributeImpl<bces_Platform::Win32Threads>::schedulingPriority() const
{
    return d_priority;
}

inline int
bcemt_AttributeImpl<bces_Platform::Win32Threads>::inheritSchedule() const
{
    return d_inheritSchedule;
}

inline
int bcemt_AttributeImpl<bces_Platform::Win32Threads>::guardSize() const
{
    return d_guardSize;
}

inline int bcemt_AttributeImpl<bces_Platform::Win32Threads>::
    getMinSchedPriority(int policy)
{
    return -1;
}

inline int bcemt_AttributeImpl<bces_Platform::Win32Threads>::
    getMaxSchedPriority(int policy)
{
    return -1;
}

inline
int operator==(const bcemt_AttributeImpl<bces_Platform::Win32Threads>& lhs,
               const bcemt_AttributeImpl<bces_Platform::Win32Threads>& rhs)
{
    return(lhs.stackSize() == rhs.stackSize() &&
           lhs.guardSize() == rhs.guardSize() &&
           lhs.schedulingPolicy() == rhs.schedulingPolicy() &&
           lhs.schedulingPriority() == rhs.schedulingPriority() &&
           lhs.inheritSchedule() == rhs.inheritSchedule() &&
           lhs.detachedState() == rhs.detachedState());
}

inline
int operator!=(const bcemt_AttributeImpl<bces_Platform::Win32Threads>& lhs,
               const bcemt_AttributeImpl<bces_Platform::Win32Threads>& rhs)
{
    return (!(lhs == rhs));
}

                             // --------------------
                             // bcemt_ThreadUtilImpl
                             // --------------------


inline
int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::create(Handle *thread,
                                              bcemt_ThreadFunction function,
                                                              void *userData)
{
    bcemt_AttributeImpl<bces_Platform::Win32Threads> attribute;
    return create(thread, attribute, function, userData);
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::yield()
{
    ::SleepEx(0, 0);
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::sleep(
                                            const bdet_TimeInterval &sleepTime)

{
    ::Sleep((DWORD)sleepTime.seconds()*1000 +
            (DWORD)(sleepTime.nanoseconds() / (1000 * 1000)));
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::microSleep(
                                                               int microsecs,
                                                               int seconds)
{
    ::Sleep(microsecs/1000 + (seconds * 1000));
}

inline bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::self()
{
    Handle h;
    h.d_id = GetCurrentThreadId();
    h.d_handle = GetCurrentThread();
    return h;
}

inline bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::NativeHandle
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::nativeHandle(
       const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle &handle)
{
    return handle.d_handle;
}

inline bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Id
bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::selfId()
{
    return GetCurrentThreadId();
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::selfIdAsInt()
{
    return (int) selfId();
}

inline int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::isEqualId(
          const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Id &lhs,
          const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Id &rhs)
{
    return lhs == rhs;
}

inline
void* bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::getSpecific(
                                                                const Key &key)
{
    return TlsGetValue(key);
}

inline int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::
                                                 setSpecific(const Key&  key,
                                                             const void *value)
{
    return TlsSetValue(key, (LPVOID)value) == FALSE ? 1 : 0;
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::getConcurrency()
{
    return -1;
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::setConcurrency(int)
{
}

                             // ---------------
                             // bcemt_MutexImpl
                             // ---------------

inline bcemt_MutexImpl<bces_Platform::Win32Threads>::bcemt_MutexImpl()
: d_lockCount(0)
{
    d_semaphore = CreateSemaphore(0,0,0x1000000,0);
}

inline bcemt_MutexImpl<bces_Platform::Win32Threads>::~bcemt_MutexImpl()
{
    CloseHandle(d_semaphore);
}

inline void bcemt_MutexImpl<bces_Platform::Win32Threads>::lock()
{
    if (InterlockedIncrement(&d_lockCount) != 1) {
        WaitForSingleObject(d_semaphore, INFINITE);
    }
}

inline int bcemt_MutexImpl<bces_Platform::Win32Threads>::tryLock()
{
    if (InterlockedCompareExchange(&d_lockCount,1,0)) return 1;
    else return 0;
}

inline void bcemt_MutexImpl<bces_Platform::Win32Threads>::unlock()
{
    if (InterlockedDecrement(&d_lockCount)) {
        ReleaseSemaphore(d_semaphore,1,0);
    }
}


                             // ------------------------
                             // bcemt_RecursiveMutexImpl
                             // ------------------------

inline
bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads>::
                                                     bcemt_RecursiveMutexImpl()
: d_lockCount(0)
, d_ownerId(0)
{
    d_semaphore = CreateSemaphore(0,0,0x1000000,0);
}

inline
bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads>::
                                                    ~bcemt_RecursiveMutexImpl()
{
    CloseHandle(d_semaphore);
}

inline void bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads>::lock()
{
    DWORD tid = GetCurrentThreadId();
    if (InterlockedIncrement(&d_lockCount) != 1) {
        if (tid != d_ownerId) {
            WaitForSingleObject(d_semaphore, INFINITE);
            d_ownerId = tid;
            d_instanceCount = 1;
        }
        else {
            d_instanceCount++;
        }
    }
    else {
        d_ownerId = tid;
        d_instanceCount = 1;
    }

}

inline int bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads>::tryLock()
{
    DWORD tid = GetCurrentThreadId();

    if (d_ownerId == tid) {
        InterlockedIncrement(&d_lockCount);
        ++d_instanceCount;
        return 0;
    }
    else if(InterlockedCompareExchange(&d_lockCount,1,0)) return 1;
    else {
        d_ownerId = tid;
        d_instanceCount = 1;
        return 0;
    }
}

inline void bcemt_RecursiveMutexImpl<bces_Platform::Win32Threads>::unlock()
{
    int postInstanceCount;

    if ((postInstanceCount = --d_instanceCount) == 0) d_ownerId = 0;
    if (InterlockedDecrement(&d_lockCount) && !postInstanceCount ) {
        ReleaseSemaphore(d_semaphore,1,0);
    }
}

                             // -------------------
                             // bcemt_ConditionImpl
                             // -------------------

inline bcemt_ConditionImpl<bces_Platform::Win32Threads>::bcemt_ConditionImpl()
: d_waitCount(0)
{
    d_semaphore = CreateSemaphore(0,0,0x1000000,0);
    InitializeCriticalSection(&d_crit);
}

inline bcemt_ConditionImpl<bces_Platform::Win32Threads>::~bcemt_ConditionImpl()
{
    EnterCriticalSection(&d_crit);
    CloseHandle(d_semaphore);
    LeaveCriticalSection(&d_crit);
    DeleteCriticalSection(&d_crit);
}

inline int bcemt_ConditionImpl<bces_Platform::Win32Threads>::wait(
                           bcemt_MutexImpl<bces_Platform::Win32Threads> *mutex)
{
    EnterCriticalSection(&d_crit);
    ++d_waitCount;
    mutex->unlock();
    LeaveCriticalSection(&d_crit);
    DWORD res = WaitForSingleObject(d_semaphore,INFINITE);
    mutex->lock();
    return res == WAIT_OBJECT_0  ? 0 : 1;
}

inline int bcemt_ConditionImpl<bces_Platform::Win32Threads>::timedWait(
                           bcemt_MutexImpl<bces_Platform::Win32Threads> *mutex,
                           const bdet_TimeInterval& timeout)
{

    DWORD milliTimeout;
    DWORD res;

    bdet_TimeInterval now = bdetu_SystemTime::now();
    bdet_TimeInterval reltime = timeout - now;
    if (timeout > now) {
        now -= timeout;
        milliTimeout = ((DWORD)reltime.seconds() * 1000 ) +
                       reltime.nanoseconds() / 1000000;
    }
    else milliTimeout = 0;

    EnterCriticalSection(&d_crit);
    d_waitCount++;
    mutex->unlock();
    LeaveCriticalSection(&d_crit);
    res = WaitForSingleObject(d_semaphore,milliTimeout);
    if (res != WAIT_OBJECT_0) {
        EnterCriticalSection(&d_crit);
        res = WaitForSingleObject(d_semaphore,0);
        if (res != WAIT_OBJECT_0) d_waitCount--;
        LeaveCriticalSection(&d_crit);
    }
    mutex->lock();
    return res == WAIT_OBJECT_0  ? 0 : (res == WAIT_TIMEOUT ? -1 : 1);
}

inline void bcemt_ConditionImpl<bces_Platform::Win32Threads>::signal()
{
    EnterCriticalSection(&d_crit);
    if (d_waitCount) {
        ReleaseSemaphore(d_semaphore, 1, 0);
        --d_waitCount;
    }
    LeaveCriticalSection(&d_crit);
}

inline void bcemt_ConditionImpl<bces_Platform::Win32Threads>::broadcast()
{
    EnterCriticalSection(&d_crit);
    if (d_waitCount) {
        ReleaseSemaphore(d_semaphore, d_waitCount, 0);
        d_waitCount = 0;
    }
    LeaveCriticalSection(&d_crit);
}

// ===================
// bcemt_SemaphoreImpl
// ===================

inline bcemt_SemaphoreImpl<bces_Platform::Win32Threads>::
                bcemt_SemaphoreImpl(int count) 
{
    d_handle = CreateSemaphore(NULL, count, 0x7FFFFFFF, NULL);
}

inline bcemt_SemaphoreImpl<bces_Platform::Win32Threads>::
               ~bcemt_SemaphoreImpl()
{
    CloseHandle(d_handle);
}
// Destroy a semaphore

// MANIPULATORS
inline void
bcemt_SemaphoreImpl<bces_Platform::Win32Threads>::post()
{
    ReleaseSemaphore(d_handle, 1, NULL);
}

inline void
bcemt_SemaphoreImpl<bces_Platform::Win32Threads>::post(int number)
{
    ReleaseSemaphore(d_handle, number, NULL);
}

inline int
bcemt_SemaphoreImpl<bces_Platform::Win32Threads>::tryWait()
{
    return WaitForSingleObject(d_handle, 0)
                == WAIT_OBJECT_0 ? 0 : -1;
}

inline void
bcemt_SemaphoreImpl<bces_Platform::Win32Threads>::wait(int *signalInterrupted)
{
    WaitForSingleObject(d_handle, INFINITE);
}

}  // close namespace BloombergLP

#endif // BCES_PLATFORM__WIN32_THREADS

#endif // INCLUDED_BCEMT_THREADIMPL_WIN32

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
