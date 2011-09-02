// bcemt_threadutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BCEMT_THREADUTIL
#define INCLUDED_BCEMT_THREADUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent utilities related to threading.
//
//@CLASSES:
//  bcemt_ThreadUtil: namespace for portable thread management utilities
//
//@SEE_ALSO: bcemt_threadattributes, bcemt_default
//
//@DESCRIPTION: This component defineds a utility 'struct', 'bcemt_ThreadUtil',
// that serves as a name space for suit of pure functions to create threads,
// join them (make one thread block and wait for another thread to exit),
// manipulate thread handles, manipulate the current thread, and (on some
// platforms) access thread-local storage.
//
///Creating a Simple Thread with Default Attributes
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Clients call 'bcemt_ThreadUtil::create()' to create threads.  This function
// requires a thread entry point as either a "C" linkage function pointer (of a
// type defined by 'bcemt_ThreadUtil::ThreadFunction') or an "invokable" based
// on 'bdef_Function' (of a type defined by 'bcemt_ThreadUtil::Invokable'), and
// a 'void' pointer to 'userData'.  Upon creation of the thread, the supplied
// thread entry point is invoked by passing it the 'userData'.  The invoked
// function becomes the main driver for the new thread; when it returns, the
// thread terminates.
//
///Thread Identity
///- - - - - - - -
// A thread is identified by an object of the opaque type
// 'bcemt_ThreadUtil::Handle'.  A handle of this type is returned when a thread
// is created (with 'bcemt_ThreadUtil::create').  A client can also retrieve a
// 'Handle' for the "current" thread via the 'self' method:
//..
//  bcemt_ThreadUtil::Handle myHandle = bcemt_ThreadUtil::self();
//..
// Several thread manipulation functions in 'bcemt_ThreadUtil' take a thread
// handle, or pointer to a thread handle, as an argument.  To facilitate
// compatibility with existing systems and allow for non-portable operations,
// clients also have access to the 'bcemt_ThreadUtil::NativeHandle' type, which
// exposes the underlying, platform-specific thread identifier type:
//..
//  bcemt_ThreadUtil::NativeHandle myNativeHandle;
//  myNativeHandle = bcemt_ThreadUtil::nativeHandle();
//..
//
///Setting Thread Priorities
///- - - - - - - - - - - - -
// 'bcemt_ThreadUtil' allows clients to configure the priority of newly created
// threads by setting the 'inheritSchedule', 'schedulingPolicy', and
// 'schedulingPriority' of a thread attributes object supplied to the 'create'
// method.  The range of legal values for 'schedulingPriority' depends on both
// the platform and the value of 'schedulingPolicy'; and can be obtained from
// the 'getMinSchedulingPriority' and 'getMaxSchedulingPriority' methods.  Both
// 'schedulingPolicy' and 'schedulingPriority' are ignored unless
// 'inheritSchedule' is 'false' (the default value is 'true').  Note that not
// only is effective setting of thread priorities only workable on some
// combinations of platforms and user priviledges, but setting the thread
// policy and priority appropriately for one platform may cause thread creation
// to fail on another platform.  Also note that an unset thread
// priority may be interpreted as being outside the valid range defined by
// '[ getMinSchedulingPriority(policy), getMaxSchedulingPriority(policy) ]'.
//..
// Platform  Restrictions
// --------  ------------------------------------------------------------------
// Solaris   None.
//
// AIX       For non-priviledged clients, spawning of threads fails if
//           'schedulingPolicy' is 'BCEMT_SCHED_FIFO' or 'BCEMT_SCHED_RR'.
//
// Linux     Non-priviledged clients *can* *not* make effective use of thread
//           priorities -- spawning of threads fails if 'schedulingPolicy' is
//           'BCEMT_SCHED_FIFO' or 'BCEMT_SCHED_RR', and
//           'getMinSchedulingPriority == getMaxSchedulingPriority' if the
//           policy has any other value.
//
// HPUX      Non-priviledged clients *can* *not* make effective use of thread
//           priorities -- spawning of threads fails if 'inheritSchedule'
//           is 'false'.
//
// Windows   Clients *can* *not* make effective use of thread priorities --
//           'schedulingPolicy', 'schedulingPriority', and 'inheritSchedule'
//           are ignored for all clients.
//..
//
///Usage
///-----
// This section illustrates the usage of this component.
//
///Example 1: Creating a Simple Thread with Default Attributes
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example creates a thread using the default attribute settings.  Upon
// creation, the thread executes the user-supplied C-linkage function
// 'myThreadFunction' that counts 5 seconds before terminating:
//..
//  extern "C" void *myThreadFunction(void *)
//      // Print to standard output "Another second has passed" every second
//      // for five seconds, and return 0.
//  {
//      for (int i = 0; i < 5; ++i) {
//          bcemt_ThreadUtil::microSleep(0, 1);
//          bsl::cout << "Another second has passed." << bsl::endl;
//      }
//      return 0;
//  }
//..
// After creating the thread, the 'main' routine *joins* the thread; in effect,
// this causes 'main' to wait for execution of 'myThreadFunction' to complete,
// and this guarantees that the output from 'main' will follow the last output
// from the user-supplied function:
//..
//  int main()
//  {
//      bcemt_ThreadAttributes attributes;
//      bcemt_ThreadUtil::Handle handle;
//      bcemt_ThreadUtil::create(&handle, attributes, myThreadFunction, 0);
//      bcemt_ThreadUtil::join(handle);
//
//      bsl::cout << "A five second interval has elapsed.\n";
//      return 0;
//  }
//..
// The output of this program is:
//..
//  Another second has passed.
//  Another second has passed.
//  Another second has passed.
//  Another second has passed.
//  Another second has passed.
//  A five second interval has elapsed.
//..
///Example 2: Creating a Simple Thread with User-Specified Attributes
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively, the attributes of a thread can be explicitly specified by
// supplying a 'bcemt_ThreadAttributes' object to the 'create' method.  For
// instance, we could specify a smaller stack size for a thread to conserve
// system resources if we know that we will require not require the platform's
// default stack size:
//..
//  extern "C" void *mySmallStackThreadFunction(void *threadArg)
//      // Initialize a small object on the stack and do some work.
//  {
//      char *initValue = (char *)threadArg;
//      char Small[8];
//      bsl::memset(&Small[0], *initValue, 8);
//      // do some work ...
//      return 0;
//  }
//
//  void createSmallStackSizeThread()
//      // Create a detached thread with a small stack size and perform some
//      // work.
//  {
//      enum { STACK_SIZE = 16384 };
//      bcemt_ThreadAttributes attributes;
//      attributes.setDetachedState(
//                              bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
//      attributes.setStackSize(STACK_SIZE);
//
//      char initValue = 1;
//      bcemt_ThreadUtil::Handle handle;
//      int status = bcemt_ThreadUtil::create(&handle,
//                                            attributes,
//                                            mySmallStackThreadFunction,
//                                            &initValue);
//  }
//..
//
///Example 3: Setting Thread Priorities
/// - - - - - - - - - - - - - - - - - -
// In this example we demonstrate creating 3 threads with different priorities.
// We use the 'convertToSchedulingPriority' function to translate a normalized,
// floating-point priority in the range '[ 0.0, 1.0 ]' to an integer priority
// in the range '[ getMinSchedulingPriority, getMaxSchedulingPriority ]' to set
// the 'schedulingPriority' attribute.
//..
//  void runSeveralThreads()
//      // Create 3 threads with different priorities and then wait for them
//      // all to finish.
//  {
//      enum { NUM_THREADS = 3 };
//
//      bcemt_ThreadUtil::Handle handles[NUM_THREADS];
//      bcemt_ThreadUtil::Invokable functions[NUM_THREADS] = {
//                                                MostUrgentThreadFunctor(),
//                                                FairlyUrgentThreadFunctor(),
//                                                LeastUrgentThreadFunctor() };
//      double priorities[NUM_THREADS] = { 1.0, 0.5, 0.0 };
//
//      bcemt_ThreadAttributes attributes;
//      attributes.setInheritSchedule(false);
//      const bcemt_ThreadAttributes::SchedulingPolicy policy =
//                                   bcemt_ThreadAttributes::BCEMT_SCHED_OTHER;
//      attributes.setSchedulingPolicy(policy);
//
//      for (int i = 0; i < NUM_THREADS; ++i) {
//          attributes.setSchedulingPriority(
//               bcemt_ThreadUtil::convertToSchedulingPriority(policy,
//                                                             priorities[i]));
//          int rc = bcemt_ThreadUtil::create(&handles[i],
//                                            attributes,
//                                            functions[i]);
//          assert(0 == rc);
//      }
//
//      for (int i = 0; i < NUM_THREADS; ++i) {
//          int rc = bcemt_ThreadUtil::join(handles[i]);
//          assert(0 == rc);
//      }
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_THREADATTRIBUTES
#include <bcemt_threadattributes.h>
#endif

#ifndef INCLUDED_BCEMT_THREADUTILIMPL_PTHREAD
#include <bcemt_threadutilimpl_pthread.h>
#endif

#ifndef INCLUDED_BCEMT_THREADUTILIMPL_WIN32
#include <bcemt_threadutilimpl_win32.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

namespace BloombergLP {

class bdet_TimeInterval;

template <typename THREAD_POLICY>
struct bcemt_ThreadUtilImpl;

extern "C" {
    typedef void *(*bcemt_ThreadFunction)(void *);
        // 'bcemt_ThreadFunction' is an alias for a function type taking a
        // single 'void' pointer argument and returning 'void *'.  Such
        // functions are suitable to be specified as thread entry-point
        // functions to 'bcemt_ThreadUtil::create'.  Note that 'create' also
        // accepts 'bdef_Function<void(*)()>' objects as well.

    typedef void (*bcemt_KeyDestructorFunction)(void *);
        // 'bcemt_KeyDestructorFunction' is an alias for a function type taking
        // a single 'void' pointer argument and returning 'void'.  Such
        // functions are suitable to be specified as thread-specific key
        // destructor functions to 'bcemt_ThreadUtil::createKey'.
}  // extern "C"

                           // =======================
                           // struct bcemt_ThreadUtil
                           // =======================

struct bcemt_ThreadUtil {
    // This 'struct' provides a suite of portable utility functions for
    // managing threads.

    // TYPES
    typedef bcemt_ThreadUtilImpl<bces_Platform::ThreadPolicy> Imp;
        // Platform-specific implementation type.  Do not use directly.

    typedef Imp::Handle                                       Handle;
        // Thread handle type.  Use this type to refer to a thread in a
        // platform-independent way.

    typedef Imp::NativeHandle                                 NativeHandle;
        // Platform-specific thread handle type.

    typedef Imp::Id                                           Id;
        // Thread identifier type - distinguished from a 'Handle' in that
        // it does not have any resources associated with it, whereas 'Handle'
        // may, depending on platform.

    typedef bcemt_ThreadFunction                              ThreadFunction;
        // Prototype for thread entry-point functions.

    typedef bdef_Function<void(*)()>                          Invokable;
        // Invokable functor wrapper, usable also as a thread entry point.

    typedef Imp::Key                                          Key;
        // Thread-specific key type, used to refer to thread-specific storage.

    typedef bcemt_KeyDestructorFunction                       Destructor;
        // Prototype for thread-specific key destructors.

    // CLASS METHODS
                         // *** Thread Management ***

    static int convertToSchedulingPriority(
        bcemt_ThreadAttributes::SchedulingPolicy policy,
        double                                   normalizedSchedulingPriority);
        // Return an integer scheduling priority appropriate for the specified
        // 'normalizedSchedulingPriority' and the specified 'policy'.  Higher
        // values of 'normalizedSchedulingPriority' are considered to represent
        // more urgent priorities.  The behavior is undefined unless 'policy'
        // is a valid 'bcemt_ThreadAttributes::SchedulingPolicy' and unless
        // 'normalizedSchedulingPriority' is in the range '[ 0.0, 1.0 ]'.

    static int create(Handle                        *handle,
                      const bcemt_ThreadAttributes&  attributes,
                      ThreadFunction                 function,
                      void                          *userData);
        // Create a new thread of program control having the specified
        // 'attributes' that invokes the specified 'function' with a single
        // argument specified by 'userData', and load into the specified
        // 'handle' an identifier that may be used to refer to this thread in
        // calls to other 'bcemt_ThreadUtil' methods.  Return 0 on success, and
        // a non-zero value otherwise.  The behavior is undefined unless
        // 'handle != 0'.  Note that unless explicitly "detached" (by invoking
        // the 'detach' class method with 'handle'), or unless the
        // 'BCEMT_CREATE_DETACHED' attribute is specified, a call to 'join'
        // must be made once the thread terminates to reclaim any system
        // resources associated with the newly-created thread.

    static int create(Handle         *handle,
                      ThreadFunction  function,
                      void           *userData);
        // Create a new thread of program control having platform-specific
        // default attributes (i.e., "stack size", "scheduling priority", etc.)
        // that invokes the specified 'function' with a single argument
        // specified by 'userData', and load into the specified 'handle' an
        // identifier that may be used to refer to this thread in calls to
        // other 'bcemt_ThreadUtil' methods.  Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined unless
        // 'handle != 0'.  Note that unless explicitly "detached" (by invoking
        // 'detach(*handle)'), a call to 'join' must be made once the thread
        // terminates to reclaim any system resources associated with the
        // newly-created thread.

    static int create(Handle                        *handle,
                      const bcemt_ThreadAttributes&  attributes,
                      const Invokable&               function);
        // Create a new thread of program control having the specified
        // 'attributes' that invokes the specified 'function' object, and load
        // into the specified 'handle' an identifier that may be used to refer
        // to this thread in calls to other 'bcemt_ThreadUtil' methods.  Return
        // 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless 'handle != 0'.  Note that unless explicitly
        // "detached" (by invoking 'detach(*handle)'), or unless the
        // 'BCEMT_CREATE_DETACHED' attribute is specified, a call to 'join'
        // must be made once the thread terminates to reclaim any system
        // resources associated with the newly-created thread.

    static int create(Handle *handle, const Invokable& function);
        // Create a new thread of program control having platform-specific
        // default attributes (i.e., "stack size", "scheduling priority", etc.)
        // that invokes the specified 'function' object, and load into the
        // specified 'handle' an identifier that may be used to refer to this
        // thread in calls to other 'bcemt_ThreadUtil' methods.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless 'handle != 0'.  Note that unless explicitly "detached" (by
        // invoking 'detach(*handle)'), a call to 'join' must be made once the
        // thread terminates to reclaim any system resources associated with
        // the newly-created thread.

    static int detach(Handle& handle);
        // "Detach" the thread identified by the specified 'handle' such that
        // when it terminates, the resources associated with that thread will
        // automatically be reclaimed.  The behavior is undefined unless
        // 'handle' was obtained by a call to 'create' or 'self'.  Note that
        // once a thread is "detached", it is no longer possible to 'join' the
        // thread to retrieve its exit status.

    static void exit(void *status);
        // Exit the current thread and return the specified 'status'.  If
        // the current thread is not "detached", then a call to 'join' must be
        // made to reclaim any resources used by the thread, and to retrieve
        // the exit status.  Note that the preferred method of exiting a thread
        // is to return from the entry point function.

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

    static int join(Handle& handle, void **status = 0);
        // Suspend execution of the current thread until the thread referred to
        // by the specified 'handle' terminates, and reclaim any system
        // resources associated with 'handle'.  If the specified 'status' is
        // not 0, load into '*status' the value returned by the function
        // supplied at the creation of the thread identified by 'handle'.  The
        // behavior is undefined unless 'handle' was obtained by a call to
        // 'create'.

    static void microSleep(int microseconds, int seconds = 0);
        // Suspend execution of the current thread for a period of at least the
        // specified 'seconds' and 'microseconds' (relative time).  Note that
        // the actual time suspended depends on many factors including system
        // scheduling and system timer resolution.

    static void sleep(const bdet_TimeInterval& time);
        // Suspend execution of the current thread for a period of at least the
        // specified 'time' (relative time).  Note that the actual time
        // suspended depends on many factors including system scheduling and
        // system timer resolution.

    static void yield();
        // Move the current thread to the end of the scheduler's queue and
        // schedule another thread to run.  Note that this allows cooperating
        // threads of the same priority to share CPU resources equally.

                       // *** Thread Identification ***

    static bool isEqual(const Handle& a, const Handle& b);
        // Return 'true' if the specified 'lhs' thread handle identifies the
        // same thread as the specified 'rhs' thread handle, or if both 'lhs'
        // and 'rhs' are invalid handles, and 'false' otherwise.  Note that if
        // *either* of 'lhs' or 'rhs' is an invalid handle, but not both, this
        // method returns 'false'.

    static bool isEqualId(const Id& a, const Id& b);
        // Return 'true' if the specified 'lhs' thread identifier is associated
        // with the same thread as the specified 'rhs' thread identifier, and
        // 'false' otherwise.

    static const Handle& invalidHandle();
        // Return a reference to the non-modifiable value that is guaranteed
        // never to be used as a valid thread handle by this implementation.

    static NativeHandle nativeHandle(const Handle& handle);
        // Return the platform-specific identifier associated with the thread
        // referred to by the specified 'handle'.  The behavior is undefined
        // unless 'handle' was obtained by a call to 'create' or 'self'.

    static Handle self();
        // Return an identifier that can be used to refer to the current thread
        // in calls to other 'bcemt_ThreadUtil' methods.

    static Id selfId();
        // Return an identifier that can be used to uniquely identify the
        // current thread within the current process.  Note that the identifier
        // is only valid until the thread terminates and may be reused
        // thereafter.

    static bsls_PlatformUtil::Uint64 selfIdAsInt();
        // Return an integral identifier that can be used to uniquely identify
        // the current thread within the current process.  Note that this
        // representation is particularly useful for logging purposes.  Also
        // note that this value is only valid until the thread terminates and
        // may be reused thereafter.
        //
        // DEPRECATED: use 'selfIdAsUint64' instead.

    static bsls_PlatformUtil::Uint64 selfIdAsUint64();
        // Return an integral identifier that can be used to uniquely identify
        // the current thread within the current process.  Note that this
        // representation is particularly useful for logging purposes.  Also
        // note that this value is only valid until the thread terminates and
        // may be reused thereafter.

    static Id handleToId(const Handle& threadHandle);
        // Return the unique identifier of the thread having the specified
        // 'threadHandle' within the current process.  The behavior is
        // undefined unless 'handle' was obtained by a call to 'create' or
        // 'self'.  Note that this value is only valid until the thread
        // terminates and may be reused thereafter.

    static int idAsInt(const Id& threadId);
        // Return the unique integral identifier of a thread uniquely
        // identified by the specified 'threadId' within the current process.
        // Note that this representation is particularly useful for logging
        // purposes.  Also note that this value is only valid until the thread
        // terminates and may be reused thereafter.

                // *** Thread-Specific (Local) Storage (TSS or TLS) ***

    static int createKey(Key *key, Destructor threadKeyCleanupFunction);
        // Load into the specified 'key' a new process-wide identifier that can
        // be used to store (via 'setSpecific') and retrieve (via
        // 'getSpecific') a pointer value local to each thread, and associate
        // with the new key the specified 'threadKeyCleanupFunction' that may
        // typically be used to cleanup resources associated with the key at
        // thread termination.  Return 0 on success, and a non-zero value
        // otherwise.  'threadKeyCleanupFunction' will be called by each
        // thread, if 'threadKeyCleanupFunction' is non-zero and the value
        // associated with 'key' for that thread is non-zero, with the
        // associated value as an argument, after the function passed to
        // 'create' has returned and before the thread terminates.  Note that
        // multiple keys can be defined, and this can result in multiple thread
        // key cleanup functions being called for a given thread.

    static int deleteKey(Key& key);
        // Delete the specified 'key' from the calling process, and
        // disassociate all threads from the thread key cleanup function
        // supplied when 'key' was created (see 'createKey').  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless 'key' was obtained from a successful call to 'createKey' and
        // has not already been deleted.  Note that deleting a key does not
        // delete any data pointed at by the pointer values associated with
        // that key in any thread.

    static void *getSpecific(const Key& key);
        // Return the thread-local value associated with the specified 'key'.
        // A 'key' is shared among all threads and the value associated with
        // 'key' for each thread is 0 until it is set by that thread using
        // 'setSpecific'.  The behavior is undefined if 'key' was not obtained
        // from a successful call to 'createKey', if 'key' has been deleted, or
        // if this method is called inside a thread key cleanup function
        // associated with any key by 'createKey'.

    static int setSpecific(const Key& key, const void *value);
        // Associate the specified thread-local 'value' with the specified
        // process-wide 'key'.  Return 0 on success, and a non-zero value
        // otherwise.  The value associated with a thread for a given key is 0
        // until it has been set by that thread using 'setSpecific'.  The
        // behavior is undefined if 'key' was not obtained from a successful
        // call to 'createKey', if 'key' has been deleted, or if this method is
        // called inside a thread key cleanup function associated with any key
        // by 'createKey'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // -----------------------
                           // struct bcemt_ThreadUtil
                           // -----------------------

// CLASS METHODS
inline
int bcemt_ThreadUtil::create(bcemt_ThreadUtil::Handle      *handle,
                             const bcemt_ThreadAttributes&  attribute,
                             ThreadFunction                 function,
                             void                          *userData)
{
    return Imp::create(handle, attribute, function, userData);
}

inline
int bcemt_ThreadUtil::create(bcemt_ThreadUtil::Handle *handle,
                             ThreadFunction            function,
                             void                     *userData)
{
    return Imp::create(handle, function, userData);
}

inline
int bcemt_ThreadUtil::getMinSchedulingPriority(
                               bcemt_ThreadAttributes::SchedulingPolicy policy)
{
    return Imp::getMinSchedulingPriority(policy);
}

inline
int bcemt_ThreadUtil::getMaxSchedulingPriority(
                               bcemt_ThreadAttributes::SchedulingPolicy policy)
{
    return Imp::getMaxSchedulingPriority(policy);
}

inline
int bcemt_ThreadUtil::join(bcemt_ThreadUtil::Handle&   thread,
                           void                      **status)
{
    return Imp::join(thread, status);
}

inline
void bcemt_ThreadUtil::yield()
{
    Imp::yield();
}

inline
void bcemt_ThreadUtil::sleep(const bdet_TimeInterval& sleepTime)
{
    Imp::sleep(sleepTime);
}

inline
void bcemt_ThreadUtil::microSleep(int microseconds, int seconds)
{
    Imp::microSleep(microseconds, seconds);
}

inline
void bcemt_ThreadUtil::exit(void *status)
{
    Imp::exit(status);
}

inline
bcemt_ThreadUtil::Handle bcemt_ThreadUtil::self()
{
    return Imp::self();
}

inline
int bcemt_ThreadUtil::detach(Handle& handle)
{
    return Imp::detach(handle);
}

inline
const bcemt_ThreadUtil::Handle& bcemt_ThreadUtil::invalidHandle()
{
    return Imp::INVALID_HANDLE;
}

inline
bcemt_ThreadUtil::NativeHandle
bcemt_ThreadUtil::nativeHandle(const bcemt_ThreadUtil::Handle& handle)
{
    return Imp::nativeHandle(handle);
}

inline
bool bcemt_ThreadUtil::isEqual(const bcemt_ThreadUtil::Handle& a,
                               const bcemt_ThreadUtil::Handle& b)
{
    // Some implementations (notably pthreads) do not define the result of
    // comparing invalid handles.  In practice, they work by comparing the
    // values of the handles explicitly, so we are not relying on undefined
    // behavior.  For uniform behavior across platforms, we explicitly check
    // for invalid handles.

    return invalidHandle() == a
           ? (invalidHandle() == b)
           : (invalidHandle() == b ? false : Imp::isEqual(a, b));
}

inline
bcemt_ThreadUtil::Id bcemt_ThreadUtil::selfId()
{
    return Imp::selfId();
}

inline
bsls_PlatformUtil::Uint64 bcemt_ThreadUtil::selfIdAsInt()
{
    return Imp::selfIdAsInt();
}

inline
bsls_PlatformUtil::Uint64 bcemt_ThreadUtil::selfIdAsUint64()
{
    return Imp::selfIdAsUint64();
}

inline
bcemt_ThreadUtil::Id bcemt_ThreadUtil::handleToId(
                                        const bcemt_ThreadUtil::Handle& handle)
{
    return Imp::handleToId(handle);
}

inline
int bcemt_ThreadUtil::idAsInt(const bcemt_ThreadUtil::Id& id)
{
    return Imp::idAsInt(id);
}

inline
bool bcemt_ThreadUtil::isEqualId(const bcemt_ThreadUtil::Id& a,
                                 const bcemt_ThreadUtil::Id& b)
{
    return Imp::isEqualId(a, b);
}

inline
int bcemt_ThreadUtil::createKey(
                        bcemt_ThreadUtil::Key        *key,
                        bcemt_ThreadUtil::Destructor  threadKeyCleanupFunction)
{
    return Imp::createKey(key, threadKeyCleanupFunction);
}

inline
int bcemt_ThreadUtil::deleteKey(bcemt_ThreadUtil::Key& key)
{
    return Imp::deleteKey(key);
}

inline
void *bcemt_ThreadUtil::getSpecific(const bcemt_ThreadUtil::Key& key)
{
    return Imp::getSpecific(key);
}

inline
int bcemt_ThreadUtil::setSpecific(const bcemt_ThreadUtil::Key&  key,
                                  const void                   *value)
{
    return Imp::setSpecific(key, value);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
