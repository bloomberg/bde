// bcemt_meteredmutex.h      -*-C++-*-
#ifndef INCLUDED_BCEMT_METEREDMUTEX
#define INCLUDED_BCEMT_METEREDMUTEX

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a mutex capable of keeping track of wait and hold time.
//
//@CLASSES:
// bcemt_MeteredMutex: mutex capable of keeping track of wait and hold time
//
//@SEE_ALSO:
//
//@AUTHOR: Ujjwal Bhoota (ubhoota)
//
//@DESCRIPTION: This component provides a class, 'bcemt_MeteredMutex', that
// functions as a mutex and has additional capability to keep track of wait
// time and hold time.  This class can be used, for example, in evaluating
// the performance of an application, based on its lock contention behavior.
//
///Precise definitions of wait and hold time
///-----------------------------------------
// Wait time is defined as the sum of the time intervals between each call
// to 'lock' (or 'tryLock') on the underlying mutex and the return of that
// call.  Note that if one or more threads are waiting for the lock at
// the point when `waitTime` is called, those waiting time intervals
// are *not* included in the returned wait time.  Hold time is defined as
// the sum of the time intervals between return from each call to 'lock'
// (or a successful call to 'tryLock') on the underlying mutex and the
// subsequent call to 'unlock'.  Note that if a thread is holding the lock
// at the point when `holdTime` is called, then that holding time is *not*
// included in the returned hold time.
//
///Performance
///-----------
// It should be noted that the overhead in keeping track of wait and hold
// time is very small.  We do not use additional mutexes to manipulate
// these times, instead, we use atomic data types (which have very small
// overhead compared to a mutex) to update these times atomically.
//
///Inaccuracy of 'waitTime' and 'holdTime'
///--------------------------------------
// Times reported by 'waitTime' and 'holdTime' are (close) approximate
// times and *not* 100% accurate.  This inaccuracy can sometime cause
// surprising behavior.  For example, one can incorrectly assume 'lock()'
// and 'while (tryLock() != 0);' to be effectively the same (both
// disallowing the thread to advance until the lock is acquired) but the
// wait time reported in the first case can be much more accurate than
// that of the second because the 'lock' is called only once (and thus
// computation error is introduced only once) in the first case.
//
///Usage
///-----
// In the following example, we have 'NUM_THREADS' threads (that are
// sequentially numbered from '0' to 'NUM_THREADS-1') and two counters
// 'evenCount' and 'oddCount'.  'evenCount' is incremented by the even
// numbered threads and 'oddCount' is incremented by the odd ones.
// We considers two strategies to increment these counters.  In
// the first strategy (strategy1), we use two mutexes (one for each
// counter) and in the second strategy (strategy2), we use a single
// mutex for both counters.
//..
//    int oddCount = 0;
//    int evenCount = 0;
//
//    typedef bcemt_MeteredMutex Obj;
//    Obj oddMutex;
//    Obj evenMutex;
//    Obj globalMutex;
//
//    enum { NUM_THREADS = 4, SLEEP_TIME = 4 };
//    bcemt_Barrier barrier(NUM_THREADS);
//
//    void executeInParallel(int numThreads,
//                           bcemt_ThreadUtil::ThreadFunction func)
//       // Create the specified 'numThreads', each executing the
//       // specified 'func'.  Number each thread (sequentially from 0
//       // to 'numThreads-1') by passing i to i'th thread.  Finally
//       // join all the threads.
//    {
//        bcemt_ThreadUtil::Handle *threads =
//                           new bcemt_ThreadUtil::Handle[numThreads];
//        ASSERT(threads);
//
//        for (int i = 0; i < numThreads; ++i) {
//            bcemt_ThreadUtil::create(&threads[i], func, (void*)i);
//        }
//        for (int i = 0; i < numThreads; ++i) {
//            bcemt_ThreadUtil::join(threads[i]);
//        }
//
//        delete [] threads;
//    }
//
//    extern "C" {
//      void *strategy1(void *arg)
//        {
//            barrier.wait();
//            int remainder = (int)arg % 2;
//            if (remainder == 1) {
//                oddMutex.lock();
//                ++oddCount;
//                bcemt_ThreadUtil::microSleep(0, SLEEP_TIME);
//                oddMutex.unlock();
//            }
//            else {
//                evenMutex.lock();
//                ++evenCount;
//                bcemt_ThreadUtil::microSleep(0, SLEEP_TIME);
//                evenMutex.unlock();
//            }
//            return NULL;
//        }
//    } //extern "C"
//
//    extern "C" {
//        void *strategy2(void *arg)
//        {
//            barrier.wait();
//            int remainder = (int)arg % 2;
//            if (remainder == 1) {
//                globalMutex.lock();
//                ++oddCount;
//                bcemt_ThreadUtil::microSleep(0, SLEEP_TIME);
//                globalMutex.unlock();
//            }
//            else {
//                globalMutex.lock();
//                ++evenCount;
//                bcemt_ThreadUtil::microSleep(0, SLEEP_TIME);
//                globalMutex.unlock();
//            }
//            return NULL;
//        }
//    } //extern "C"
//
//    int main()
//    {
//        executeInParallel(NUM_THREADS, strategy1);
//        bsls_PlatformUtil::Int64 waitTimeForStrategy1 =
//                oddMutex.waitTime() + evenMutex.waitTime();
//
//        executeInParallel(NUM_THREADS, strategy2);
//        bsls_PlatformUtil::Int64 waitTimeForStrategy2 =
//                                 globalMutex.waitTime();
//
//        ASSERT(waitTimeForStrategy2 > waitTimeForStrategy1);
//        if (veryVerbose) {
//            P(waitTimeForStrategy1);
//            P(waitTimeForStrategy2);
//        }
//    }
//..
// We measured the wait times for each strategy.  Intuitively, the wait
// time for the second strategy should be greater than that of the first.
// The output was consistent with our expectation.
//..
// waitTimeForStrategy1 = 400787000
// waitTimeForStrategy2 = 880765000
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSLS_TIMEUTIL
#include <bsls_timeutil.h>
#endif

namespace BloombergLP {

                           // ========================
                           // class bcemt_MeteredMutex
                           // ========================

class bcemt_MeteredMutex {
    // This class implements a mutex, that has the additional capability
    // to keep track of hold time and wait time.  The hold time is defined
    // as the cumulative duration for which the mutex was in the locked
    // state.  The wait time is defined as the duration for which threads
    // waited for the mutex.

    // DATA
    bcemt_Mutex              d_mutex;          // underlying mutex
    bces_AtomicInt64         d_waitTime;       // wait time
    bces_AtomicInt64         d_holdTime;       // hold time
    bsls_PlatformUtil::Int64 d_startHoldTime;  // starting point of hold time
    bces_AtomicInt64         d_lastResetTime;  // last reset time

    // NOT IMPLEMENTED
    bcemt_MeteredMutex(const bcemt_MeteredMutex&);
    bcemt_MeteredMutex& operator=(const bcemt_MeteredMutex&);

  public:
    // CREATORS
    bcemt_MeteredMutex();
        // Create a metered mutex in the unlocked state.

    ~bcemt_MeteredMutex();
        // Destroy this metered mutex.

    // MANIPULATORS
    void lock();
        // Acquire the lock on this metered mutex.  If this mutex is
        // currently locked, suspend the execution of the current
        // thread until the lock can be acquired.  Update the wait and
        // hold time appropriately.  The behavior is undefined if the
        // calling thread already owns the lock.

    void resetMetrics();
        // Reset the wait and hold time to zero and record the current
        // time.  All subsequent calls (that are made before a subsequent
        // call to 'resetMetrics') to 'waitTime' (or 'holdTime') will
        // return the wait (or hold) time, accumulated since this call.
        // Also, all subsequent calls (that are made before a subsequent
        // call to 'resetMetrics') to 'lastResetTime' will return the time
        // of this call.

    int tryLock();
        // Attempt to acquire the lock on this metered mutex.  Return 0 on
        // success, and a non-zero value if this mutex is already locked,
        // or if an error occurs.  Update the wait and hold time
        // appropriately.  The behavior is undefined if the calling thread
        // already owns the lock.

    void unlock();
        // Release the lock on this mutex that was previously acquired
        // through a successful call to 'lock' or 'tryLock'.  Update the
        // hold time appropriately.  The behavior is undefined unless the
        // calling thread currently owns the lock.

    // ACCESSORS
    bsls_PlatformUtil::Int64 holdTime() const;
        // Return the hold time (in nanoseconds) accumulated since the
        // most recent call to 'resetMetrics' (or `bcemt_MeteredMutex`
        // if 'resetMetrics' was never called).

    bsls_PlatformUtil::Int64 lastResetTime() const;
        // Return the time in nanoseconds (referenced to an arbitrary but
        // fixed origin) of the most recent invocation to 'resetMetrics' (or
        // creation time if 'resetMetrics' was never invoked).  User can
        // calculate the difference (in nanoseconds) between the current time
        // and the last reset time by expression
        // 'bsls_TimeUtil::getTimer() - clientMutex.lastResetTime()'.

    bsls_PlatformUtil::Int64 waitTime() const;
        // Return the wait time (in nanoseconds), accumulated since the
        // most recent call to 'resetMetrics' (or `bcemt_MeteredMutex`
        // if 'resetMetrics' was never called).
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                               // ------------------
                               // bcemt_MeteredMutex
                               // ------------------
// CREATORS
inline
bcemt_MeteredMutex::bcemt_MeteredMutex()
: d_lastResetTime(bsls_TimeUtil::getTimer())
{
}

inline
bcemt_MeteredMutex::~bcemt_MeteredMutex()
{
}

// MANIPULATORS
inline
void bcemt_MeteredMutex::lock()
{
    bsls_PlatformUtil::Int64 t1 = bsls_TimeUtil::getTimer();
    d_mutex.lock();
    d_startHoldTime = bsls_TimeUtil::getTimer();
    d_waitTime += (d_startHoldTime - t1);
}

inline
int bcemt_MeteredMutex::tryLock()
{
    bsls_PlatformUtil::Int64 t1 = bsls_TimeUtil::getTimer();
    int returnStatus = d_mutex.tryLock();
    bsls_PlatformUtil::Int64 t2 = bsls_TimeUtil::getTimer();
    d_waitTime += t2 - t1;
    if (returnStatus == 0) {
        d_startHoldTime = t2;
    }
    return returnStatus;
}

inline
void bcemt_MeteredMutex::unlock()
{
    d_holdTime += (bsls_TimeUtil::getTimer() - d_startHoldTime);
    d_mutex.unlock();
}

// ACCESSORS
inline
bsls_PlatformUtil::Int64 bcemt_MeteredMutex::holdTime() const
{
    return d_holdTime;
}

inline
bsls_PlatformUtil::Int64 bcemt_MeteredMutex::lastResetTime() const
{
    return d_lastResetTime;
}

inline
bsls_PlatformUtil::Int64 bcemt_MeteredMutex::waitTime() const
{
    return d_waitTime;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
