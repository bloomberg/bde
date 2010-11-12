// bcemt_threadimpl_pthread.h                -*-C++-*-

#ifndef INCLUDED_BCEMT_THREADIMPL_PTHREAD
#define INCLUDED_BCEMT_THREADIMPL_PTHREAD

//@PURPOSE: Provide a full specialization of bcemt_thread for POSIX threads.
//
//@CLASSES:
//      bcemt_AttributeImpl: pthreads specialization of bcemt_AttributeImpl
//     bcemt_ThreadUtilImpl: pthreads specialization of bcemt_ThreadUtilImpl
//      bcemt_ConditionImpl: pthreads specialization of bcemt_ConditionImpl
//          bcemt_MutexImpl: pthreads specialization of bcemt_MutexImpl
// bcemt_RecursiveMutexImpl: pthreads full spec. of bcemt_RecursiveMutexImpl
//
//@SEE_ALSO: bcemt_thread
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a full specialization of the
// bcemt_thread component for POSIX threads ("pthreads").  Since the
// bcemt_thread implements the POSIX threading semantics, the operations
// provided here are mostly efficient proxies to their respective POSIX
// operations.
//
// This component is a platform specific implementation detail and is not
// intended to be used directly.  Clients should use the 'bcemt_thread'
// component instead.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifdef BCES_PLATFORM__POSIX_THREADS
    // Platform specific implementation starts here.

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

#ifndef INCLUDED_SCHED
#include <sched.h>
#define INCLUDED_SCHED
#endif

#ifndef INCLUDED_CTIME
#include <ctime>
#define INCLUDED_CTIME
#endif

#ifndef INCLUDED_CERRNO
#include <cerrno>
#define INCLUDED_CERRNO
#endif

#ifndef PTHREAD_MUTEX_RECURSIVE
#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>  // For recursive mutex
#endif
#endif

#ifndef INCLUDED_SEMAPHORE
#include <semaphore.h>
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
class bcemt_AttributeImpl<bces_Platform::PosixThreads> {
    // This class provides a full specialization of bcemt_AttributeImpl for
    // pthreads.  It is a simple wrapper for a 'pthread_attr_t' and its
    // respective operations.  The default 'SchedulingPolicy' is
    // system-dependent: it is BCEMT_SCHED_FIFO on Cygwin and BCEMT_SCHED_OTHER
    // on all other platforms.

    pthread_attr_t d_imp;   // Thread attribute structure.

    friend class bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>;

  public:
    typedef pthread_attr_t NativeAttribute;

    enum DetachedState {
        CREATE_JOINABLE, // The current thread can be 'join'ed with.
        CREATE_DETACHED  // The current thread can not be 'join'ed with.
    };

    enum SchedulingPolicy {
        BCEMT_SCHED_OTHER,  // Default OS scheduling policy.
        BCEMT_SCHED_FIFO,   // First-In-First-Out scheduling policy.
        BCEMT_SCHED_RR      // Round-Robin scheduling policy.
    };

    // CREATORS
    bcemt_AttributeImpl();
        // Create a thread attribute object with the following default values:
        //..
        //      Attribute             Default Value
        //      -------------------+----------------------------
        //      DetachedState      |  CREATE_JOINABLE
        //      SchedulingPolicy   |  *BDE's* *default* *value* (see above)
        //      SchedulingPriority |  *platform's* *default* *value*
        //      InheritSchedule    |  TRUE
        //      StackSize          |  *platform's* *default* *value*
        //      GuardSize          |  *platform's* *default* *value*
        //..

    bcemt_AttributeImpl(const NativeAttribute& original);
        // Create a thread attribute object initialized with the values in the
        // specified 'original' native attribute object.

    bcemt_AttributeImpl(const bcemt_AttributeImpl& attribute);
        // Create a thread attribute object initialized to the value of the
        // specified 'original' attribute object.

    ~bcemt_AttributeImpl();
        // Destroy this 'bcemt_AttributeImpl' object.

    bcemt_AttributeImpl& operator=(const bcemt_AttributeImpl& rhs);
        // Assign to this attribute object the value of the specified 'rhs'
        // attribute object and return a reference to this modifiable
        // attribute object.

    // MANIPULATORS
    void setDetachedState(DetachedState detachedState);
        // Set the value of the detached state attribute to the specified
        // 'detachedState'.

    void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        // Set the value of the scheduling policy attribute to the specified
        // 'schedulingPolicy'.

    void setSchedulingPriority(int schedulingPriority);
        // Set the value of the scheduling priority attribute to the specified
        // 'schedulingPriority'.

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

    NativeAttribute nativeAttribute();
        // Return a copy of the native 'pthread_attr_t' object used by this
        // attribute object.

    // ACCESSORS
    DetachedState detachedState() const;
        // Return the value of the detached state attribute.

    SchedulingPolicy schedulingPolicy() const;
        // Return the value of the scheduling policy attribute.

    int schedulingPriority() const;
        // Return the value of the scheduling priority attribute.  A value of -1
        // indicates that the scheduling priority is set the othe platform's
        // default value.

    int inheritSchedule() const;
        // Return 1 if threads (created with this object) will inherit the
        // "parent" threads scheduling policy and priority, and 0 if they do
        // not.

    int stackSize() const;
        // Return the value of the stack size attribute.

    int guardSize() const;
        // Return the value of the guard size attribute.

    static int getMinSchedPriority(int policy = -1);
        // Return the non-negative minimun available priority for the
        // optionnally-specified 'policy' on success.  If no policy is
        // specified, the minimum priority for the current policy is returned.
        // Return a negative value on error.

    static int getMaxSchedPriority(int policy = -1);
        // Return the non-negative maximum available priority for the
        // optionnally-specified 'policy' on success.  If no policy is
        // specified, the maximum priority for the current policy is returned.
        // Return a negative value on error.
};

inline
int operator==(const bcemt_AttributeImpl<bces_Platform::PosixThreads>& lhs,
               const bcemt_AttributeImpl<bces_Platform::PosixThreads>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects have the
    // same value, and a 0 value otherwise.

inline
int operator!=(const bcemt_AttributeImpl<bces_Platform::PosixThreads>& lhs,
               const bcemt_AttributeImpl<bces_Platform::PosixThreads>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' attribute objects do not have
    // the same value, and a 0 value otherwise.

            // ====================================================
            // class bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>
            // ====================================================

extern "C" {
    typedef void* (*bcemt_ThreadFunction)(void *);
        // This type declares the prototype for functions that are suiteable
        // to be specified as thread entrypoint functions to
        // 'bcemt_ThreadUtilImpl::create'.

    typedef void (*bcemt_KeyDestructorFunction)(void *);
        // This type declares the prototype for functions that are suiteable
        // to be specified as thread specific key destructor functions to
        // "bcemt_ThreadUtilImpl::createKey".
}

template <>
struct bcemt_ThreadUtilImpl<bces_Platform::PosixThreads> {
    // This class provides a full specializaion of 'bcemt_ThreadUtilImpl' for
    // pthreads.

    typedef pthread_t Handle;        // thread handle type
    typedef pthread_t NativeHandle;  // native thread handle type
    typedef pthread_t Id;            // thread Id type
    typedef pthread_key_t Key;       // thread-specific storage key type

    static const pthread_t INVALID_HANDLE;

    static int create(
             Handle *thread,
             const bcemt_AttributeImpl<bces_Platform::PosixThreads>& attribute,
             bcemt_ThreadFunction function,
             void *userData);
        // Create a new thread of program control having the attributes
        // specified by 'attribute', that invokes the specified 'function'
        // with a single argument specified by 'userData' and  load into the
        // specified 'threadHandle', an identifier that may be used to refer
        // to the thread in future calls to this utility.  Return 0 on success,
        // and a non-zero value otherwise.  The behavior is undefined if
        // 'thread' is 0.  Note that unless explicitly "detached"('detach'),
        // or unless the CREATE_DETACHED attribute is specified, a call to
        // 'join' must be made once the thread terminates to reclaim any system
        // resources associated with the newly created identifier.

    static int create(Handle *thread, bcemt_ThreadFunction function,
                      void *userData);
        // Create a new thread of program control having platform specific
        // default attributes(i.e. "stack size", "scheduling priority"), that
        // invokes the specified 'function' with a single argument specified
        // by 'userData', and load into the  specified 'threadHandle', an
        // identifier that may be used to refer to the thread in future calls
        // to this utility.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined if 'thread' is 0.  Note that
        // unless explicitly "detached"('detach'), a call to 'join' must be
        // made once the thread terminates to reclaim any system resources
        // associated with the newly created identifier.

    static int join(Handle &threadHandle, void **status = (void**)0);
        // Suspend execution of the current thread until the thread specified
        // by 'threadHandle' terminates, and reclaim any system resources
        // associated with the specified 'threadHandle'.  If the specified
        // 'status' is not 0, load into the specified 'status', the value
        // returned by the specified 'thread'.

    static int sleep(const bdet_TimeInterval &sleepTime,
                     bdet_TimeInterval *unsleptTime=0);
        // Suspend execution of the current thread for a period of at least
        // the specified 'sleepTime'(relative time) and optionanlly load into
        // the specified 'unsleptTime', the amount of time that was not sleep
        // by this function if the operation was interrupted by a signal.
        // Return 0 on sucess, and non-zero if the operation was interrupted,
        // by a signal.  Note that the actual time suspended depends on many
        // factors including system scheduling, and system timer resolution.

    static void yield();
        // Put the current thread to the end of the scheduler's queue and
        // schedule another thread to run.  This allows cooperating threads of
        // the same priority to share CPU ressources equally.

    static int microSleep(int microseconds, int seconds=0,
                          bdet_TimeInterval *unsleptTime=0);
        // Suspend execution of the current thread for a period of at least the
        // specified 'seconds' and microseconds(relative time) and optionanlly
        // load into the specified 'unsleptTime', the amount of time that was
        // not sleep by this function if the operation was interrupted by a
        // signal.  Return 0 on sucess, and non-zero if the operation was
        // interrupted, by a signal.  Note that the actual time suspended
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

    static int createKey(Key *key, bcemt_KeyDestructorFunction destructor );
        // Store into the specified 'key', an identifier which can be used to
        // associate('setSpecific') and retrieve('getSpecific') a single
        // thread-specific pointer value.  Associated with the identifier,the
        // optional 'destructor' if a non-zero value is specified.  Return 0 on
        // success, and a non-zero value otherwise.

    static int deleteKey(Key &key);
        // Delete the specified thread-specific 'key'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that deleting a key does not
        // delete any data that is currently associated with the key in the
        // calling thread or any other thread.

    static void* getSpecific(const Key &key);
        // Return the value associated with the specified thread-specific
        // 'key'.  Note that if the key is not valid, a value of zero is
        // returned, which is indistinguishable from a valid key with a 0
        // value.

    static int setSpecific(const Key &key, const void * value);
        // Associate the specified 'value' with the specified thread-specific
        // 'key'.  Return 0 on success, and a non-zero value otherwise.

    static int getConcurrency();
        // Return the current threads implementation concurrency level or -1 if
        // not supported.

    static void setConcurrency(int newLevel);
        // Set the desired thread concurrency level to the specified
        // 'newLevel'.  Some threads implementations may not run actually run
        // run threads concurrently or only run a small number of
        // threads concurrently regardless of the number of processors
        // available.  This method informs the threads implementation of the
        // desired number of concurrent threads.  Note that the value is only a
        // hint to the threads implementation.  The actual number of concurrent
        // threads used may be greater or less that than the requested value.
};

               // ==================================================
               // class bcemt_MutexImpl<bces_Platform::PosixThreads>
               // ==================================================

template <>
class bcemt_MutexImpl<bces_Platform::PosixThreads> {
    // This class provides a full specializaion of 'bcemt_MutexImpl' for
    // pthreads.  It provides a efficient proxy for the 'pthread_mutex_t'
    // pthreads type, and releated operations.  Note that mutex implemented
    // here classs, is non error checking, and non-recursive.

    pthread_mutex_t d_lock;

    // not implemented
    bcemt_MutexImpl(const bcemt_MutexImpl&);
    bcemt_MutexImpl& operator=(const bcemt_MutexImpl&);

    friend class bcemt_ConditionImpl<bces_Platform::PosixThreads>;

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

        // ===========================================================
        // class bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>
        // ===========================================================

template <>
class bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads> {
    // This class provides a full specializaion of 'bcemt_RecursiveMutexImpl'
    // for pthreads.  If the pthreads implementation supports the "recursive"
    // attribute, then the native implemenation is used, otherwise, a portable,
    // efficient implementation is provided.

    pthread_mutex_t d_lock;

#ifndef PTHREAD_MUTEX_RECURSIVE
    bces_SpinLock  d_spin;   // Spinlock used to control access to this object
    pthread_t      d_owner;  // Thread id of the thread that currently owns
                             // this lock object
    volatile int   d_lockCount; // current lock recursion level
#endif

    // not implemented
    bcemt_RecursiveMutexImpl(const bcemt_RecursiveMutexImpl&);
    bcemt_RecursiveMutexImpl& operator=(const bcemt_RecursiveMutexImpl&);

    friend class bcemt_ConditionImpl<bces_Platform::PosixThreads>;
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
        // the lock immediately and return 0.  Otherwise If this object is
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

             // ======================================================
             // class bcemt_ConditionImpl<bces_Platform::PosixThreads>
             // ======================================================

template <>
class bcemt_ConditionImpl<bces_Platform::PosixThreads> {
    // This class provides a full specializaion of 'bcemt_RecursiveMutexImpl'
    // for pthreads.  The implementation provided here defines an efficient
    // proxy for the 'pthread_cond_t' pthread type, and related operations.

    pthread_cond_t d_cond;

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
    int wait(bcemt_MutexImpl<bces_Platform::PosixThreads> *mutex);
        // Atomically unlock the specified 'mutex' and suspend execution of
        // current thread until this condition object is "signaled"('signal',
        // 'broadcast') then re-acquire the lock on the specified 'mutex', and
        // return 0 upon success and non-zero otherwise.  Note that the behavior
        // is undefined unless specified 'mutex' is locked by the calling
        // thread prior to calling this method.

    int timedWait(bcemt_MutexImpl<bces_Platform::PosixThreads> *mutex,
                  const bdet_TimeInterval& timeout);
        // Atomically unlock the specified 'mutex' and suspend execution of
        // current thread until this condition object is "signaled"('signal',
        // 'broadcast') or until the specified 'timeout' (expressed as the
        // absolute time from 00:00:00 UTC, January 1, 1970), then re-acquire
        // the lock on the specified 'mutex', and return 0 upon success and
        // non-zero if an error or timeout occured.  Note that the behavior is
        // undefined unless specified 'mutex' is locked by the calling thread
        // prior to calling this method.

    void signal();
        // Signal this condition object; wake up a single thread that is
        // currently waiting on this condition.

    void broadcast();
        // Signal this condition object; wake up all threads that are currently
        // waiting on this condition.
};

             // ======================================================
             // class bcemt_SemaphoreImpl<bces_Platform::PosixThreads>
             // ======================================================

template <>
class bcemt_SemaphoreImpl<bces_Platform::PosixThreads> {
    //
    
    sem_t d_sem;

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

                              // ===================
                              // bcemt_AttributeImpl
                              // ===================

inline
bcemt_AttributeImpl<bces_Platform::PosixThreads>::bcemt_AttributeImpl(
          const bcemt_AttributeImpl<bces_Platform::PosixThreads>& attribute)
{
    pthread_attr_init(&d_imp);
    *this = attribute;
}

inline
bcemt_AttributeImpl<bces_Platform::PosixThreads>::~bcemt_AttributeImpl()
{
    pthread_attr_destroy(&d_imp);
}

inline
void bcemt_AttributeImpl<bces_Platform::PosixThreads>::setDetachedState(
                bcemt_AttributeImpl<bces_Platform::PosixThreads>::DetachedState
                                                                 detachedState)
{
    pthread_attr_setdetachstate(&d_imp,
                                (detachedState == CREATE_DETACHED)
                                ? PTHREAD_CREATE_DETACHED
                                : PTHREAD_CREATE_JOINABLE);
}

inline
void bcemt_AttributeImpl<bces_Platform::PosixThreads>::setSchedulingPolicy(
    bcemt_AttributeImpl<bces_Platform::PosixThreads>::SchedulingPolicy
                                                              schedulingPolicy)
{
    switch(schedulingPolicy) {
      case BCEMT_SCHED_FIFO:
        pthread_attr_setschedpolicy(&d_imp, SCHED_FIFO);
      break;
      case BCEMT_SCHED_RR:
        pthread_attr_setschedpolicy(&d_imp, SCHED_RR);
      break;
      default:
        pthread_attr_setschedpolicy(&d_imp, SCHED_OTHER);
      break;
    }
}

inline void
bcemt_AttributeImpl<bces_Platform::PosixThreads>::setSchedulingPriority(
                                                        int schedulingPriority)
{
    struct sched_param sched;
    pthread_attr_getschedparam(&d_imp, &sched);
    sched.sched_priority = schedulingPriority;
    pthread_attr_setschedparam(&d_imp, &sched);
}

inline void
bcemt_AttributeImpl<bces_Platform::PosixThreads>::
                                        setInheritSchedule(int inheritSchedule)
{
    pthread_attr_setinheritsched(&d_imp, inheritSchedule
                                 ? PTHREAD_INHERIT_SCHED
                                 : PTHREAD_EXPLICIT_SCHED);

}

inline
void bcemt_AttributeImpl<bces_Platform::PosixThreads>::setStackSize(
                                                                 int stackSize)
{
    pthread_attr_setstacksize(&d_imp, stackSize);
}

inline
void bcemt_AttributeImpl<bces_Platform::PosixThreads>::setGuardSize(
                                                                 int guardSize)
{
#if !defined(BDES_PLATFORM__OS_DGUX) && !defined(BDES_PLATFORM__OS_CYGWIN)
    pthread_attr_setguardsize(&d_imp, guardSize);
#endif
}

inline bcemt_AttributeImpl<bces_Platform::PosixThreads>::NativeAttribute
bcemt_AttributeImpl<bces_Platform::PosixThreads>::nativeAttribute()
{
    return d_imp;
}

inline bcemt_AttributeImpl<bces_Platform::PosixThreads>::DetachedState
bcemt_AttributeImpl<bces_Platform::PosixThreads>::detachedState() const
{
    int i;
    pthread_attr_getdetachstate(&d_imp, &i);
    return ((i == PTHREAD_CREATE_DETACHED)
            ? CREATE_DETACHED
            : CREATE_JOINABLE);
}

inline bcemt_AttributeImpl<bces_Platform::PosixThreads>::SchedulingPolicy
bcemt_AttributeImpl<bces_Platform::PosixThreads>::schedulingPolicy() const
{
    int i;
    pthread_attr_getschedpolicy(&d_imp, &i);
    switch(i) {
      case SCHED_FIFO:
        return BCEMT_SCHED_FIFO;
      case SCHED_RR:
        return BCEMT_SCHED_RR;
      default:
        return BCEMT_SCHED_OTHER;
    }
}

inline
int bcemt_AttributeImpl<bces_Platform::PosixThreads>::stackSize() const
{
    std::size_t size;
    pthread_attr_getstacksize(&d_imp, &size);
    return size;
}

inline int
bcemt_AttributeImpl<bces_Platform::PosixThreads>::schedulingPriority() const
{
    struct sched_param sched;
    pthread_attr_getschedparam(&d_imp, &sched);
    return sched.sched_priority;
}

inline int
bcemt_AttributeImpl<bces_Platform::PosixThreads>::inheritSchedule() const
{
    int i;
    pthread_attr_getinheritsched(&d_imp, &i);
    return((i == PTHREAD_INHERIT_SCHED) ? 1 : 0);
}

inline
int bcemt_AttributeImpl<bces_Platform::PosixThreads>::guardSize() const
{
#if !defined(BDES_PLATFORM__OS_DGUX) && !defined(BDES_PLATFORM__OS_CYGWIN)
    std::size_t size;
    pthread_attr_getguardsize(&d_imp, &size);
    return size;
#else
    return -1;
#endif
}

inline int
bcemt_AttributeImpl<bces_Platform::PosixThreads>::
    getMinSchedPriority(int policy)
{
    if (policy == -1) {
        policy = sched_getscheduler(0);
    }
    return sched_get_priority_min(policy);
}

inline int
bcemt_AttributeImpl<bces_Platform::PosixThreads>::
    getMaxSchedPriority(int policy)
{
    if (policy == -1) {
        policy = sched_getscheduler(0);
    }
    return sched_get_priority_max(policy);
}

inline
int operator==(const bcemt_AttributeImpl<bces_Platform::PosixThreads>& lhs,
               const bcemt_AttributeImpl<bces_Platform::PosixThreads>& rhs)
{
    return(lhs.stackSize() == rhs.stackSize() &&
           lhs.guardSize() == rhs.guardSize() &&
           lhs.schedulingPolicy() == rhs.schedulingPolicy() &&
           lhs.schedulingPriority() == rhs.schedulingPriority() &&
           lhs.inheritSchedule() == rhs.inheritSchedule() &&
           lhs.detachedState() == rhs.detachedState());
}

inline
int operator!=(const bcemt_AttributeImpl<bces_Platform::PosixThreads>& lhs,
               const bcemt_AttributeImpl<bces_Platform::PosixThreads>& rhs)
{
    return (!(lhs == rhs));
}

                             // ====================
                             // bcemt_ThreadUtilImpl
                             // ====================

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::create(
         bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle *handle,
         const bcemt_AttributeImpl<bces_Platform::PosixThreads>& attribute,
         bcemt_ThreadFunction function,
         void *userData)
{
    return pthread_create(handle, &attribute.d_imp, function, userData);
}

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::create(
         bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle *handle,
         bcemt_ThreadFunction function,
         void *userData)
{
    return pthread_create(handle, 0, function, userData);
}

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::join(
           bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle &thread,
           void **status)
{
    return pthread_join(thread, status);
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::yield()
{
    sched_yield();
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::sleep(
                                            const bdet_TimeInterval &sleepTime,
                                            bdet_TimeInterval *unsleeptTime)

{
    timespec naptime;
    timespec unslept_time;
    naptime.tv_sec = sleepTime.seconds();
    naptime.tv_nsec = sleepTime.nanoseconds();

#if defined(BDES_PLATFORM__OS_DGUX)
    do {
        __d10_dg_pthread_sleep(&naptime, &unslept_time);
        if (!unslept_time.tv_sec && !unslept_time.tv_nsec) break;
        naptime.tv_sec = unslept_time.tv_sec;
        naptime.tv_nsec = unslept_time.tv_nsec;
    } while (1);
    return 0;
#else
    int res = nanosleep(&naptime, unsleeptTime ? &unslept_time : 0);
    if (res && unsleeptTime)
        unsleeptTime->setInterval(unslept_time.tv_sec, unslept_time.tv_nsec);
    return res;
#endif
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::microSleep(
                                            int microsecs,
                                            int seconds,
                                            bdet_TimeInterval *unsleeptTime)
{
    timespec naptime;
    timespec unslept_time;
    naptime.tv_sec = microsecs / 1000000 + seconds;
    naptime.tv_nsec = (microsecs % 1000000) * 1000;
#if defined(BDES_PLATFORM__OS_DGUX)
    do {
        __d10_dg_pthread_sleep(&naptime, &unslept_time);
        if (!unslept_time.tv_sec && !unslept_time.tv_nsec) break;
        naptime.tv_sec = unslept_time.tv_sec;
        naptime.tv_nsec = unslept_time.tv_nsec;
    } while (1);
    return 0;
#else
    int res = nanosleep (&naptime, unsleeptTime ? &unslept_time : 0);
    if (res && unsleeptTime)
        unsleeptTime->setInterval(unslept_time.tv_sec, unslept_time.tv_nsec);
    return res;
#endif
}

inline void bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::
                                                             exit(void *status)
{
    pthread_exit(status);
}

inline bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::self()
{
    return pthread_self();
}

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::detach(
             bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle &handle)
{
    return pthread_detach(handle);
}

inline bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::NativeHandle
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::nativeHandle(
    const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle &handle)
{
    return handle;
}

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::isEqual(
       const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle &lhs,
       const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle &rhs)
{
    return pthread_equal(lhs, rhs);
}

inline bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Id
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::selfId()
{
    return pthread_self();
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::selfIdAsInt()
{
#ifndef BDES_PLATFORM__OS_CYGWIN
    return selfId();
#else
    return (int) selfId();
#endif
}

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::isEqualId(
       const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Id &lhs,
       const bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Id &rhs)
{
    return pthread_equal(lhs, rhs);
}

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::createKey(
                                                                      Key *key,
                                        bcemt_KeyDestructorFunction destructor)
{
    return pthread_key_create(key,destructor);
}

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::
                                                            deleteKey(Key &key)
{
    return pthread_key_delete(key);
}

inline void* bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::getSpecific(
                                                                const Key &key)
{
    return pthread_getspecific(key);
}

inline int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::setSpecific(
                                                                const Key &key,
                                                            const void * value)
{
#if defined(BDES_PLATFORM__OS_DGUX)
    return pthread_setspecific(key, (void*)value);
#else
    return pthread_setspecific(key, value);
#endif
}

inline
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::getConcurrency()
{
#if defined(BDES_PLATFORM__OS_AIX) || defined(BDES_PLATFORM__OS_SOLARIS)
    return pthread_getconcurrency();
#else
    return -1;
#endif
}

inline
void bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::
                                                   setConcurrency(int newLevel)
{
#if defined(BDES_PLATFORM__OS_AIX) || defined(BDES_PLATFORM__OS_SOLARIS)
    pthread_setconcurrency(newLevel);
#endif
}

                             // ===============
                             // bcemt_MutexImpl
                             // ===============

inline bcemt_MutexImpl<bces_Platform::PosixThreads>::bcemt_MutexImpl()
{
    pthread_mutex_init(&d_lock, 0);
}

inline bcemt_MutexImpl<bces_Platform::PosixThreads>::~bcemt_MutexImpl()
{
    pthread_mutex_destroy(&d_lock);
}

inline void bcemt_MutexImpl<bces_Platform::PosixThreads>::lock()
{
    pthread_mutex_lock(&d_lock);
}

inline int bcemt_MutexImpl<bces_Platform::PosixThreads>::tryLock()
{
    return pthread_mutex_trylock(&d_lock);
}

inline void bcemt_MutexImpl<bces_Platform::PosixThreads>::unlock()
{
    pthread_mutex_unlock(&d_lock);
}

                           // ========================
                           // bcemt_RecursiveMutexImpl
                           // ========================

inline
bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>
                                         ::bcemt_RecursiveMutexImpl()
{
    pthread_mutexattr_t attribute;
    pthread_mutexattr_init(&attribute);
#ifdef PTHREAD_MUTEX_RECURSIVE
    pthread_mutexattr_settype(&attribute,PTHREAD_MUTEX_RECURSIVE);
#else
    d_lockCount = 0;
#endif
    pthread_mutex_init(&d_lock, &attribute);
    pthread_mutexattr_destroy(&attribute);
}

inline
bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>
                                          ::~bcemt_RecursiveMutexImpl()
{
    pthread_mutex_destroy(&d_lock);
}

inline void bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>::lock()
{
#ifdef PTHREAD_MUTEX_RECURSIVE
    pthread_mutex_lock(&d_lock);
#else
    if (pthread_mutex_trylock(&d_lock)) {
        d_spin.lock();
        if (d_lockCount && pthread_equal(d_owner,pthread_self())) {
            ++d_lockCount;
            d_spin.unlock();
            return;
        }
        d_spin.unlock();
        pthread_mutex_lock(&d_lock);
    }
    d_spin.lock();
    d_owner = pthread_self();
    d_lockCount = 1;
    d_spin.unlock();
#endif
}

inline int bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>::tryLock()
{
#ifdef PTHREAD_MUTEX_RECURSIVE
    return pthread_mutex_trylock(&d_lock);
#else
    if (pthread_mutex_trylock(&d_lock)) {
        d_spin.lock();
        if (d_lockCount && pthread_equal(d_owner,pthread_self())) {
            ++d_lockCount;
            d_spin.unlock();
            return 0;
        }
        else {
            d_spin.unlock();
            return 1;
        }
    }
    else {
        d_spin.lock();
        d_owner = pthread_self();
        d_lockCount = 1;
        d_spin.unlock();
        return 0;
    }
#endif
}

inline void bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>::unlock()
{
#ifdef PTHREAD_MUTEX_RECURSIVE
    pthread_mutex_unlock(&d_lock);
#else
    d_spin.lock();
    if (d_lockCount && pthread_equal(d_owner,pthread_self())) {
        if (!--d_lockCount) {
            d_spin.unlock();
            pthread_mutex_unlock(&d_lock);
            return;
        }
    }
    d_spin.unlock();
#endif
}

                           // ===================
                           // bcemt_ConditionImpl
                           // ===================

inline bcemt_ConditionImpl<bces_Platform::PosixThreads>::bcemt_ConditionImpl()
{
    pthread_cond_init(&d_cond, 0);
}

inline bcemt_ConditionImpl<bces_Platform::PosixThreads>::~bcemt_ConditionImpl()
{
    pthread_cond_destroy(&d_cond);
}

inline int bcemt_ConditionImpl<bces_Platform::PosixThreads>::wait(
                           bcemt_MutexImpl<bces_Platform::PosixThreads> *mutex)
{
    return pthread_cond_wait(&d_cond, &mutex->d_lock);
}

inline int bcemt_ConditionImpl<bces_Platform::PosixThreads>::timedWait(
                           bcemt_MutexImpl<bces_Platform::PosixThreads> *mutex,
                           const bdet_TimeInterval& timeout)
{
    timespec ts;
    ts.tv_sec = timeout.seconds();
    ts.tv_nsec = timeout.nanoseconds();
    int status = pthread_cond_timedwait(&d_cond, &mutex->d_lock, &ts);
    return 0 == status ? 0 : (ETIMEDOUT == status ? -1 : -2);
}

inline void bcemt_ConditionImpl<bces_Platform::PosixThreads>::signal()
{
    pthread_cond_signal(&d_cond);
}

inline void bcemt_ConditionImpl<bces_Platform::PosixThreads>::broadcast()
{
    pthread_cond_broadcast(&d_cond);
}

                           // ===================
                           // bcemt_SemaphoreImpl
                           // ===================


inline bcemt_SemaphoreImpl<bces_Platform::PosixThreads>::bcemt_SemaphoreImpl(int count) {
    ::sem_init(&d_sem, 0, count);
}

inline bcemt_SemaphoreImpl<bces_Platform::PosixThreads>::~bcemt_SemaphoreImpl()
{
    ::sem_destroy(&d_sem);
}
    // Destroy a semaphore

// MANIPULATORS
inline void 
bcemt_SemaphoreImpl<bces_Platform::PosixThreads>::post()
{
    ::sem_post(&d_sem);
}


inline void 
bcemt_SemaphoreImpl<bces_Platform::PosixThreads>::post(int number)
{
    for(int i=0; i<number; i++) {
        post();
    }
}

inline int
bcemt_SemaphoreImpl<bces_Platform::PosixThreads>::tryWait()
{
    return ::sem_trywait(&d_sem);
}

inline void
bcemt_SemaphoreImpl<bces_Platform::PosixThreads>::wait(int *signalInterrupted)
{
    while(::sem_wait(&d_sem) != 0) {
        if (errno == EINTR) {
            if (signalInterrupted) {
                *signalInterrupted = 1;
                return;
            } else {
                continue;
            }
        } else {
            return;
        }
    }
}


}  // close namespace BloombergLP

#endif // BCES_PLATFORM__POSIX_THREADS

#endif // INCLUDED_BCEMT_THREADIMPL_PTHREAD

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
