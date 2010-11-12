// bcemt_qlock.h                                                      -*-C++-*-
#ifndef INCLUDED_BCEMT_QLOCK
#define INCLUDED_BCEMT_QLOCK

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide small, statically-initializable mutex lock.
//
//@CLASSES:
//       bcemt_QLock: Small, statically-initializable intra-process mutex
//  bcemt_QLockGuard: Automatic locking-unlocking of bcemt_QLock
//
//@SEE_ALSO: bcemt_thread, bcemt_atomictypes, bcemt_lockguard, bcemt_once
//
//@AUTHOR: Vlad Kliatchko (vkliatchko@bloomberg.net) (design)
//         Alex Libman (alibman1@bloomberg.net) (integration with BCE)
//
//@DESCRIPTION: This component defines a portable and efficient lock for
// ensuring that only one thread at a time enters a specific "critical
// region" -- a section of code that accesses a shared resource.  The
// functionality of the 'bcemt_QLock' class overlaps those of the
// 'bcemt_Mutex' and 'bces_SpinLock' classes, but with different usage and
// performance characteristics, as shown in the following grid:
//..
//                                    | QLock | Mutex | SpinLock
// -----------------------------------+-------+-------+---------
// Memory footprint                   | small | large | small
// Cost of construction/destruction   | cheap | costly| cheap
// Statically initializable           | yes   | no    | yes
// Speed at low contention            | fast  | fast  | fast
// Speed at high contention           | slow  | fast  | very slow
// Suitable for long critical regions | yes   | yes   | no
// Fair                               | yes   | no    | no
//..
// The performance trade-offs for a QLock are quite different than those for a
// conventional mutex.  QLocks are best suited for low-contention applications
// where large numbers of locks may be needed.  For example, a node-based data
// structure that needs a lock for each node can benefit from the small size
// and low initialization cost of a QLock compared to that of a conventional
// mutex.  A 'bcemt_Mutex' object cannot be initialized statically because
// some platforms (e.g., Windows XP) do not have a native
// statically-initializable mutex type.  A 'bcemt_QLock' object, in contrast
// is statically initializable on all platforms.
//
// The performance characteristics of a QLock are very similar to those of a
// SpinLock.  However, a QLock is much more suitable than a SpinLock for
// situations where the critical region is more than a few instructions long.
// Also, although QLocks are best for low-contention situations, they do not
// degrade nearly as badly as SpinLocks if there is a lot of contention for
// the lock.  They also use significantly fewer CPU cycles in high-contention
// situations than do SpinLocks.
//
// A unique characteristic of QLocks is that they are fair.  If there is
// contention for a lock, each thread is given the lock in the order in
// which it requested it.  Consequently, every thread competing for the lock
// will get a chance before any other thread can have a second turn; no
// thread is ever "starved" out of the critical region.  This fairness comes
// at a cost, however, in that the scheduler is given less leeway to schedule
// threads in the most efficient manner.
//
///The 'bcemt_QLockGuard' Class
///----------------------------
// A 'bcemt_QLock' is different from other locking classes such as
// 'bcemt_Mutex' and 'bces_SpinLock' in that it cannot be manipulated except
// through the auxiliary 'bcemt_QLockGuard' class.  The reason for this
// limited interface is that a QLock requires a small amount of additional
// storage for each thread that is holding or waiting for the lock.  The
// 'bcemt_QLockGuard' provides this extra storage efficiently on the stack.
//
// In typical usage, a 'bcemt_QLockGuard' is created as a local (stack)
// variable, acquires the lock in its constructor and releases the lock in
// its destructor.  If the lock is in use at construction time, then the
// current thread blocks until the lock becomes available.  Although the
// QLock itself is intended to be shared among multiple threads, the guard
// object must never be used by more than one thread at a time.  When multiple
// threads want to acquire the same QLock, each must use its own
// 'bcemt_QLockGuard' object.
//
// 'bcemt_QLockGuard' also provides the following manipulators typical of
// locking classes:
//..
//  void lock();    // Acquire the lock, waiting if necessary
//  int tryLock();  // Acquire the lock if possible.  Fail if lock is in use.
//  void unlock();  // Free the lock.
//..
// As with other types of mutexes, only one thread my hold the lock at a time.
// Other threads attempting to call 'lock' will block until the lock becomes
// available.  However, it is important to remember that the manipulators
// listed above are only pass-through operations on the shared 'bcemt_QLock'
// object.  In other words, upon return from calling 'lock' on a
// 'bcemt_QLockGuard' object, a thread has actually acquired the lock to the
// underlying 'bcemt_QLock'.
//
// Although it is only a proxy for the actual QLock, 'lock'/'unlock'/'tryLock'
// interface of 'bcemt_QLockGuard' allows it to be treated as though it were
// itself a lock.  In particular, it is possible to instantiate the
// 'bcemt_LockGuard' and 'bcemt_LockGuardUnlock' class templates using
// 'bcemt_QLockGuard'.  This layering of guard classes is useful for creating
// regions where the QLock is locked or unlocked.  For example, if a thread
// acquires a QLock and then needs to temporarily relinquish it, it could use
// a 'bcemt_LockGuardUnlock' as follows:
//..
//  void Node::update()
//  {
//     bcemt_QLockGuard qguard(&d_qlock);  // 'd_qlock' is a 'bcemt_QLock'.
//     readLunarState();
//     if (d_moonIsFull) {
//         // Free lock while we sleep
//         bcemt_LockGuardUnlock<bcemt_QLockGuard> unlock(&qguard)
//         sleep(TWENTY_FOUR_HOURS);
//     }
//     // Lock has been re-acquired
//     ...
//  }
//..
// The behavior is undefined if 'unlock' is invoked from a thread that did not
// successfully acquire the lock, or if 'lock' is called twice in a thread
// without an intervening call to 'unlock' (i.e., 'bcemt_QLockGuard' is
// non-recursive).
//
///Usage
///-----
//
///Using 'bcemt_QLock' to Implement a Thread-Safe Singleton
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// For this example, assume that we have the need to use the string "Hello"
// repeatedly in the form of an 'bsl::string' object.  Rather than construct
// the string each time we use it, it would be nice to have only one copy so
// that we can amortize the memory allocation and construction cost over all
// the uses of the string.  It is thus logical to have a single, static
// variable (a singleton) of type 'bsl::string' initialized with the value,
// "Hello".  Unfortunately, as this is a multithreaded application, there is
// the danger that more than one thread will attempt to initialize the
// singleton simultaneously, causing a memory leak at best and memory
// corruption at worse.  To solve this problem, we use a 'bcemt_QLock' to
// synchronize access to the singleton.
//
// We begin by wrapping the singleton in a function:
//..
//  const bsl::string& helloString()
//  {
//..
// This function defines two static variables, a pointer to the singleton, and
// a QLock to control access to the singleton.  Note that both of these
// variables are statically initialized, so there is no need for a run-time
// constructor and hence no danger of a race condition among threads.  The
// need for static initialization is the main reason we choose to use
// 'bcemt_QLock' over 'bcemt_Mutex':
//..
//      static const bsl::string *singletonPtr = 0;
//      static bcemt_QLock qlock = BCEMT_QLOCK_INITIALIZER;
//..
// Before checking the status of the singleton pointer, we must make sure that
// we are not accessing the pointer at the same time that some other thread is
// modifying the pointer.  We do this by acquiring the lock by constructing a
// 'bcemt_QLockGuard' object:
//..
//      bcemt_QLockGuard qlockGuard(&qlock);
//..
// Now we are inside the critical region.  If the pointer has not already been
// set, we can initialize the singleton knowing that no other thread is
// manipulating or accessing these variables at the same time.  Note that this
// critical region involves constructing a variable of type 'bsl::string'.
// This operation, while not ultra-expensive, is too lengthy for comfortably
// holding a spinlock.  Again, the characteristics of 'bcemt_QLock' are
// superior to the alternatives for this application.  (It is worth noting that
// the QLock concept was created specifically to permit this kind of one-time
// processing.  See also 'bcemt_once'.)
//..
//      if (! singletonPtr) {
//          static bsl::string singleton("Hello");
//          singletonPtr = &singleton;
//      }
//..
// Finally, we return a reference to the singleton.  The destructor for
// 'bcemt_QLockGuard' will automatically unlock the QLock and allow another
// thread into the critical region.
//..
//      return *singletonPtr;
//  }
//..
// The following test program shows how our singleton function can be called.
// Note that 'hello1' and 'hello2' have the same address, demonstrating that
// there was only one string created.
//..
//  int main()
//  {
//      const bsl::string& hello1 = helloString();
//      assert(hello1 == "Hello");
//
//      const bsl::string& hello2 = helloString();
//      assert(hello2 == "Hello");
//      assert(&hello2 == &hello1);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef  INCLUDED_BCES_ATOMICUTIL
#include <bces_atomicutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

class bcemt_Semaphore;

#define BCEMT_QLOCK_INITIALIZER  { {0} }
    // Use this macro as the value for initializing an object of type
    // 'bcemt_QLock'  For example:
    //..
    //  bcemt_QLock mylock = BCEMT_QLOCK_INITIALIZER;
    //..

                           // ==================
                           // struct bcemt_QLock
                           // ==================

struct bcemt_QLock {
    // An efficient statically-initializable synchronization primitive that
    // enables serialized access to shared resources.  Objects of this class
    // can only be manipulated through the use of a 'bcemt_QLockGuard'.  The
    // following idiom is used to initialize objects of type 'bcemt_QLock':
    //..
    //  bcemt_QLock mylock = BCEMT_QLOCK_INITIALIZER;
    //..

  private:
    // NOT IMPLEMENTED
    bcemt_QLock& operator=(const bcemt_QLock&);

    // We would like to prohibit copy construction, but then this class would
    // not be a POD and we would lose the ability to initialize objects of
    // this class statically:
    // bcemt_QLock(const bcemt_QLock&);

  public:
    bces_AtomicUtil::Pointer d_guardQueueTail;
        // Pointer to the last guard in the queue of guards waiting for this
        // lock, or 0 if the lock is unlocked.
        //
        // Note that the first guard in the queue owns the lock so that
        // 'd_guardQueueTail' points to the owner of the lock when the lock
        // is locked and there are no additional guards waiting.
        //
        // It would have been preferable for this member to be private, but
        // then this class would not be statically initializable.  Also, it
        // would have been preferable to make this member an instance of
        // 'bces_AtomicPointer<>', but again, we would lose the ability
        // to initialize statically.

  public:
    // MANIPULATORS
    void initialize();
        // Set this lock into the initial unlocked state.

    // ACCESSORS
    bool isLocked() const;
        // Return true if this lock is locked and false otherwise.
};

                         // ===========================
                         // class bcemt_QLock_EventFlag
                         // ===========================

class bcemt_QLock_EventFlag {
    // [!PRIVATE!] This class provides a thread-safe mechanism for one thread
    // to inform another thread that some event has occurred.  A flag provides
    // two primary manipulators, 'set', which indicates the event has occurred,
    // and 'waitUntilSet', which waits until that event has occurred (or
    // returns immediately if it has already occurred).  A flag is intended to
    // be used by only two threads: a thread setting the flag, and a thread
    // waiting for the flag to be set, and the behavior is undefined if 'set'
    // is called while the flag is already set, or if 'waitUntilSet' is called
    // while another thread is waiting for the flag.
    //
    // This class is an implementation detail of the 'bcemt_qlock', and must
    // not be used by client code.

  private:
    // PRIVATE TYPES
    typedef bcemt_Semaphore               Semaphore;
    typedef bces_AtomicPointer<Semaphore> AtomicSemaphorePtr;

    // DATA
    AtomicSemaphorePtr d_status;  // status of this flag

  private:
    // NOT IMPLEMENTED
    bcemt_QLock_EventFlag(const bcemt_QLock_EventFlag&);
    bcemt_QLock_EventFlag& operator=(const bcemt_QLock_EventFlag&);

  public:

    // CREATORS
    bcemt_QLock_EventFlag();
        // Create an unset flag.

    ~bcemt_QLock_EventFlag();
        // Destroy this flag.  The behavior is undefined if a thread is
        // currently waiting for the flag to be set.

    // MANIPULATORS
    void reset();
        // Reset this flag to the unset state.  The behavior is undefined if a
        // thread is waiting for this flag to be set.

    void set();
        // Set this flag, and if a thread is waiting for it, signal the waiting
        // thread.  The behavior is undefined if this flag is already set.

    void waitUntilSet(int spinRetryCount);
        // Wait until this flag has been set (returning immediately if this
        // flag is already set), and, if this flag is not already set, spin for
        // the specified 'spinCount' iterations before waiting on a semaphore.
        // The behavior is undefined unless there are no other threads waiting
        // for this flag to be set.
};

                           // ======================
                           // class bcemt_QLockGuard
                           // ======================

class bcemt_QLockGuard  {
    // This class provides the means to acquire and release the lock on a
    // 'bcemt_QLock' object.  Typically, the lock is acquired at construction
    // and released automatically on destruction.  This class also provides
    // explicit 'lock', 'tryLock', and 'unlock' primitives.

  private:

    bcemt_QLock           *d_qlock_p;    // points to tail of the queue.

    bcemt_QLockGuard      *d_next;       // next object in queue.

    bcemt_QLock_EventFlag  d_readyFlag;  // flag indicating when the lock is
                                         // released by its predecessor in the
                                         // queue

    bcemt_QLock_EventFlag  d_nextFlag;   // flag indicating 'd_next' is set by
                                         // its successor.

    bool                   d_locked;     // 'true' if this guard holds the lock

  private:
    // NOT IMPLEMENTED
    bcemt_QLockGuard(const bcemt_QLockGuard&);
    bcemt_QLockGuard& operator=(const bcemt_QLockGuard&);

    // PRIVATE MANIPULATORS
    void unlockRaw();
        // Free the lock but do not clear the member variables of this class.

  public:
    // CREATORS
    bcemt_QLockGuard();
        // Create a guard in the unlocked state, not associated with any
        // 'bcemt_QLock' objects.

    explicit bcemt_QLockGuard(bcemt_QLock *qlock, bool doLock = true);
        // Create a guard associated with the specified 'qlock'.  Acquire the
        // lock unless the (optionally) specified 'doLock' is false.  If the
        // 'lock' is not free, block until it can be acquired.

    ~bcemt_QLockGuard();
        // Destroy this object.  If this object holds a lock, automatically
        // free it.

    // MANIPULATORS
    void setQLock(bcemt_QLock *qlock);
        // Associate this guard with the specified 'qlock'.  The behavior is
        // undefined if this object is already in a locked state.

    void lock();
        // Acquire a lock on the associated 'bcemt_QLock' object.  If the
        // 'lock' is not free, block until it can be acquired.  The behavior
        // is undefined if the calling thread already owns the lock on the
        // QLock.

    void lock(bcemt_QLock *qlock);
        // Associate this guard with the specified 'qlock' and acquire the
        // lock.  If the 'lock' is not free, block until it can be acquired.
        // The behavior is undefined if the calling thread already owns the
        // lock on 'qlock' or if this object is in the locked state.

    int tryLock();
        // Attempt to acquire a lock on the associated 'bcemt_QLock' object.
        // Return 0 on success, a positive value of the associated QLock object
        // is already locked, or a negative value if an error occurs.

    void unlock();
        // Release the lock on the associated 'bcemt_QLock'.  The behavior
        // is undefined unless this guard previously acquired the lock and has
        // not already released it.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // -----------------
                           // class bcemt_QLock
                           // -----------------

// MANIPULATORS
inline
void bcemt_QLock::initialize()
{
    bces_AtomicUtil::setPtrRelaxed(&d_guardQueueTail, 0);
}

// ACCESSORS
inline
bool bcemt_QLock::isLocked() const
{
    return bces_AtomicUtil::getPtr(d_guardQueueTail) != 0;
}

                           // ---------------------------
                           // class bcemt_QLock_EventFlag
                           // ---------------------------

// CREATORS
inline
bcemt_QLock_EventFlag::bcemt_QLock_EventFlag()
: d_status(0)
{
}

inline
bcemt_QLock_EventFlag::~bcemt_QLock_EventFlag()
{
}

// MANIPULATORS
inline
void bcemt_QLock_EventFlag::reset()
{
    d_status = 0;
}

                           // ----------------------
                           // class bcemt_QLockGuard
                           // ----------------------

// CREATORS
inline
bcemt_QLockGuard::bcemt_QLockGuard()
: d_qlock_p   (0)
, d_next      (0)
, d_readyFlag ()
, d_nextFlag  ()
, d_locked    (false)
{
}

inline
bcemt_QLockGuard::bcemt_QLockGuard(bcemt_QLock *qlock, bool doLock)
: d_qlock_p   (qlock)
, d_next      (0)
, d_readyFlag ()
, d_nextFlag  ()
, d_locked    (false)
{
    if (doLock) {
        lock();
    }
}

inline
bcemt_QLockGuard::~bcemt_QLockGuard()
{
    if (d_locked) {
        unlockRaw();
    }
}

// MANIPULATORS
inline
void bcemt_QLockGuard::setQLock(bcemt_QLock *qlock)
{
    BSLS_ASSERT_SAFE(!d_locked);

    d_qlock_p = qlock;
}

inline
void bcemt_QLockGuard::lock(bcemt_QLock *qlock)
{
    BSLS_ASSERT_SAFE(!d_locked);
    BSLS_ASSERT_SAFE(qlock);

    d_qlock_p = qlock;
    lock();
}

inline
void bcemt_QLockGuard::unlock()
{
    if (d_locked) {
        // Release the lock, and reset the state variables so it can be
        // relocked.

        unlockRaw();

        d_locked = false;
        d_next   = 0;
        d_readyFlag.reset();
        d_nextFlag.reset();
    }
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
