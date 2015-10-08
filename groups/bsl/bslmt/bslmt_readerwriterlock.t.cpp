// bslmt_readerwriterlock.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_readerwriterlock.h>

#include <bslmt_barrier.h>           // for testing only
#include <bslmt_threadattributes.h>

#include <bslim_testutil.h>

#include <bsls_timeinterval.h>       // for testing only
#include <bsls_atomic.h>             // for testing only

#include <bsl_map.h>                 // for usage example

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>
#include <bsl_c_signal.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//-----------------------------------------------------------------------------
// [ 3] void lockRead();
// [ 5] void lockReadReserveWrite();
// [ 3] void lockWrite();
// [ 3] int tryLockRead();
// [ 2] int tryLockWrite();
// [ 5] int upgradeToWriteLock();
// [ 4] int upgradeToWriteLock();
// [ 4] int tryUpgradeToWriteLock();
// [ 3] void unlock();
// [ 5] void unlock();
// [ 2] void unlock();
//-----------------------------------------------------------------------------
// [ 1] Breathing test
// [ 6] USAGE Example

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

#define NL cout << endl;

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::ReaderWriterLock Obj;
enum { k_NTHREADS = 5 };

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

class my_Condition {
    // This class implements a condition variable object which compensates for
    // spurious returns from the wait functions in bslmt_Condition.  The
    // versions of the 'wait' and 'timedWait' functions provided here only
    // return when the condition was actually signaled.  The class is used to
    // simplify testing.
    bslmt::Condition d_cond;
    bslmt::Mutex     d_mutex;
    volatile int     d_sigState;
    volatile int     d_bcastCount;
  public:
    my_Condition();
        // Construct a my_Condition object.

    ~my_Condition();
        // Destroy a my_Condition object.

    void wait(bslmt::Mutex *mutex);
        // Block until this condition is signaled by a call to 'signal' or
        // 'broadcast'.

    int timedWait(bslmt::Mutex *mutex, const bsls::TimeInterval &timeout);
        // Block until this condition is signaled by a call to 'signal' or
        // 'broadcast', or until the specified 'timeout' (in abs time).  Return
        // 0 if the condition was signaled, and a value of -1 if a timeout
        // occurred.

    void signal();
        // Unblock a single thread that is waiting on this condition.

    void broadcast();
        // Unblock all threads that are waiting on this condition.
};

my_Condition::my_Condition()
: d_sigState(0)
, d_bcastCount(0)
{
}

my_Condition::~my_Condition()
{
}

void my_Condition::wait(bslmt::Mutex *mutex)
{
    d_mutex.lock();
    int bcastCount = d_bcastCount;
    d_sigState = 0;
    mutex->unlock();
    while (!d_sigState && d_bcastCount == bcastCount)
        d_cond.wait(&d_mutex);
    if (d_sigState) --d_sigState;
    d_mutex.unlock();
    mutex->lock();
}

int my_Condition::timedWait(bslmt::Mutex              *mutex,
                            const bsls::TimeInterval&  timeout)
{
    d_mutex.lock();
    int bcastCount = d_bcastCount;
    d_sigState = 0;
    mutex->unlock();
    while (!d_sigState && d_bcastCount == bcastCount) {
        if (d_cond.timedWait(&d_mutex,timeout)) {
            d_mutex.unlock();
            mutex->lock();
            return -1;                                                // RETURN
        }
    }
    if (d_sigState) --d_sigState;
    d_mutex.unlock();
    mutex->lock();
    return 0;
}

void my_Condition::signal()
{
    d_mutex.lock();
    ++d_sigState;
    d_cond.signal();
    d_mutex.unlock();
}

void my_Condition::broadcast()
{
    d_mutex.lock();
    ++d_bcastCount;
    d_cond.broadcast();
    d_mutex.unlock();
}

struct TestArguments {
    // This structure provides a set of objects and counters used by various
    // thread functions in this test driver.  It also provides a set of wait
    // and signal methods.

    Obj              d_lock;
    my_Condition     d_cond1;
    my_Condition     d_cond2;
    bslmt::Mutex      d_mutex;
    volatile int     d_startSigCount;
    volatile int     d_stopSigCount;
    volatile int     d_iterations;
    bsls::AtomicInt   d_count;
    bsls::AtomicInt   d_readCount;
    bsls::AtomicInt   d_writeCount;
    bslmt::Barrier    d_barrierAll;     // barrier for all threads
    bslmt::Barrier    d_barrier2;       // barrier for two threads
  public:
    TestArguments(int iterations = 0, int nThreads = k_NTHREADS);
        // Construct a 'TestArguments' object and initialize all counters to
        // zero, 'd_iterations' to the optionally specified 'iterations' and
        // 'd_barrierAll' to the optionally specified 'nThreads' + 1.

    void clearStart();
        // clear the start indicator flag.

    void waitStart();
        // Block until the start condition is signaled('signalStart').

    void signalStart();
        // Unblock a single thread that is waiting on the start condition(
        // 'waitStart').

    void clearStop();
        // Clear the stop indicator flag.

    void waitStop();
        // Block until the stop condition is signaled('signalStop')

    void signalStop();
        // Unblock a single thread that is waiting on the stop condition(
        // 'waitStop').
};

TestArguments::TestArguments(int iterations, int nThreads)
: d_startSigCount(0)
, d_stopSigCount(0)
, d_iterations(iterations)
, d_barrierAll(nThreads + 1)
, d_barrier2(2)
{
}

void TestArguments::clearStart()
{
  d_mutex.lock();
  d_startSigCount = 0;
  d_mutex.unlock();
}

void TestArguments::waitStart()
{
    d_mutex.lock();
    while (!d_startSigCount) d_cond1.wait(&d_mutex);
    --d_startSigCount;
    d_mutex.unlock();
}

void TestArguments::signalStart()
{
    d_mutex.lock();
    ++d_startSigCount;
    d_cond1.signal();
    d_mutex.unlock();
}

void TestArguments::clearStop()
{
    d_mutex.lock();
    d_stopSigCount = 0;
    d_mutex.unlock();
}

void TestArguments::waitStop()
{
    d_mutex.lock();
    while (!d_stopSigCount) d_cond2.wait(&d_mutex);
    --d_stopSigCount;
    d_mutex.unlock();
}

void TestArguments::signalStop()
{
    d_mutex.lock();
    ++d_stopSigCount;
    d_cond2.signal();
    d_mutex.unlock();
}

void* TestReaderThread1(void *ptr)
    // This function is used to test the 'lockRead' and 'unlock' methods.  It
    // begins by invoking 'wait' on the barrier for all the threads.  Then it
    // signals the provided condition variable 'd_cond1'.  Next it calls
    // 'lockRead' on the provided reader/writer lock.  Once it has acquired the
    // lock, it again signals 'd_cond1' and waits for 'd_cond2' to be signaled.
    // Finally it releases the reader/writer lock using the 'unlock' method.
{
    TestArguments *args=(TestArguments*)ptr;
    args->d_barrierAll.wait();

    args->waitStop();
    args->d_lock.lockRead();
    ++args->d_count;
    ++args->d_readCount;
    args->d_lock.unlock();
    return 0;
}

extern "C" {
void* TestReaderThread2(void *ptr)
    // This function is used to test the 'lockRead' and 'unlock' methods.  It
    // begins by invoking 'wait' on the barrier for all the threads.  Then it
    // signals the provided condition variable 'd_cond1'.  Next it calls
    // 'lockRead' on the provided reader/writer lock.  Once it has acquired the
    // lock, it again signals 'd_cond1' and waits for 'd_cond2' to be signaled.
    // Finally it releases the reader/writer lock using the 'unlock' method.
{
    TestArguments *args=(TestArguments*)ptr;
    args->d_barrierAll.wait();

    args->d_lock.lockRead();
    ++args->d_count;
    ++args->d_readCount;
    args->d_barrierAll.wait();
    args->waitStop();
    args->d_lock.unlock();
    return 0;
}

void* TestWriterThread1(void *ptr)
    // This function is used to test the 'lockWrite' and 'unlock' methods.  It
    // begins by invoking 'wait' on the barrier for all the threads.  Then it
    // signals the provided condition variable 'd_cond1'.  Next it calls
    // 'lockWrite' on the provided reader/writer lock.  Once it has acquired
    // the lock, it again signals 'd_cond1' and waits for 'd_cond2' to be
    // signaled.  Finally it releases the reader/writer lock using the
    // 'unlockWrite' method.
{
    static bslmt::Mutex m;
    TestArguments *args=(TestArguments*)ptr;
    args->d_barrierAll.wait();

    args->d_lock.lockWrite();
    ++args->d_count;
    args->d_barrier2.wait();
    ++args->d_writeCount;
    args->waitStop();
    args->signalStart();
    args->d_lock.unlock();
    return 0;
}

void* TestWriterThread2(void *ptr)
    // This function is used to test the 'lockReadReserveWrite' and 'unlock'
    // methods.  It begins by invoking 'wait' on the barrier for all the
    // threads.  Then it signals the provided condition variable 'd_cond1'.
    // Next it calls 'lockReadReserveWrite' on the provided reader/writer lock.
    // Once it has acquired the lock, it again signals 'd_cond1' and waits for
    // 'd_cond2' to be signaled.  Finally it releases the reader/writer lock
    // using the 'unlock' method.
{
    TestArguments *args=(TestArguments*)ptr;
    args->d_barrierAll.wait();

    //    args->d_lock.lockReadReserveWrite();
    args->d_lock.lockWrite();
    ++args->d_count;
    args->d_barrier2.wait();
    ++args->d_writeCount;
    args->waitStop();
    args->signalStart();
    args->d_lock.unlock();

    return 0;
}

void* TestWriterThread3(void *ptr)
    // This function is used to test the 'lockReadReserveWrite' and
    // 'upgradeToWriteLock' methods.  It begins by invoking 'wait' on the
    // barrier for all the threads.  Then it signals the provided condition
    // variable 'd_cond1'.  Next it calls 'lockReadReserveWrite' on the
    // provided reader/writer lock.  Once it has acquired the lock, it again
    // signals 'd_cond1' and waits for 'd_cond2' to be signaled.  Finally it
    // upgrades the lock to a write lock using the 'upgradeToWriteLock' method
    // and releases the lock using the 'unlock' method.
{
    TestArguments *args=(TestArguments*)ptr;
    args->d_barrierAll.wait();

    args->d_lock.lockReadReserveWrite();
    args->waitStop();
    args->d_mutex.lock();
    args->d_lock.upgradeToWriteLock();
    ++args->d_count;
    ++args->d_writeCount;
    args->d_lock.unlock();
    args->d_mutex.unlock();
    args->signalStart();

    return 0;
}

void* TestWriterThread4(void *ptr)
    // This function is used to test the 'lockWrite' and 'unlock' methods.  It
    // begins by invoking 'wait' on the barrier for all the threads.  Then it
    // signals the provided condition variable 'd_cond1'.  Next it calls
    // 'lockWrite' on the provided reader/writer lock.  Once it has acquired
    // the lock, it again signals 'd_cond1' and waits for 'd_cond2' to be
    // signaled.  Finally it releases the reader/writer lock using the
    // 'unlockWrite' method.
{
    static bslmt::Mutex m;
    TestArguments *args=(TestArguments*)ptr;
    args->d_barrier2.wait();

    args->d_lock.lockWrite();
    ++args->d_count;
    ++args->d_writeCount;
    args->d_barrier2.wait();
    args->waitStop();
    args->signalStart();

    args->d_lock.unlock();
    return 0;
}

void* TestUpgradeThread1(void *ptr)
    // This function is used to test the '' upgradeToWriteLock' methods.  It
    // begins by invoking 'wait' on the barrier for all the threads.  Then it
    // signals the provided condition variable 'd_cond1'.  Next it calls
    // 'lockReadReserveWrite' on the provided reader/writer lock.  Once it has
    // acquired the lock, it again signals 'd_cond1' and waits for 'd_cond2' to
    // be signaled.  Finally it upgrades the lock to a write lock using the
    // 'upgradeToWriteLock' method and releases the lock using the 'unlock'
    // method.
{
    TestArguments *args=(TestArguments*)ptr;
    args->d_barrierAll.wait();

    args->d_lock.lockRead();
    args->d_lock.upgradeToWriteLock();
    ++args->d_writeCount;
    args->waitStop();
    ++args->d_count;
    args->d_lock.unlock();
    return 0;
}

}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

#define MAX_USER_NAME 40
#define MAX_BADGE_LOCATION 40

///Usage
///-----
// The following snippet of code demonstrates a typical use of a reader/writer
// lock.  The sample implements a simple cache mechanism for user information.
// We expect that the information is read very frequently, but only modified
// when a user "badges" in or out, which should be relatively infrequent.
//..
    struct UserInfo{
        long               d_UserId;
        char               d_UserName[MAX_USER_NAME];
        char               d_badge_location[MAX_BADGE_LOCATION];
        int                d_inOutStatus;
        bsls::TimeInterval d_badgeTime;
    };

    class UserInfoCache {
        typedef bsl::map<int, UserInfo> InfoMap;

        bslmt::ReaderWriterLock d_lock;
        InfoMap                 d_infoMap;

      public:
        UserInfoCache();
        ~UserInfoCache();

        int getUserInfo(int userId, UserInfo *userInfo);
        int updateUserInfo(int userId, UserInfo *userInfo);
        int addUserInfo(int userId, UserInfo *userInfo);
        void removeUser(int userId);
    };

    inline
    UserInfoCache::UserInfoCache()
    {
    }

    inline
    UserInfoCache::~UserInfoCache()
    {
    }

    inline
    int UserInfoCache::getUserInfo(int userId, UserInfo *userInfo)
    {
        int ret = 1;
//..
// Getting the user info does not require any write access.  We do, however,
// need read access to 'd_infoMap', which is controlled by 'd_lock'.  (Note
// that writers *will* block until this *read* *lock* is released, but
// concurrent reads are allowed.)  The user info is copied into the
// caller-owned location 'userInfo'.
//..
        d_lock.lockRead();
        InfoMap::iterator it = d_infoMap.find(userId);
        if (d_infoMap.end() != it) {
            *userInfo = it->second;
            ret = 0;
        }
        d_lock.unlock();
        return ret;
    }

    inline
    int UserInfoCache::updateUserInfo(int userId, UserInfo *userInfo)
    {
        int ret = 1;
//..
// Although we intend to update the information, we first acquire a *read*
// *lock* to locate the item.  This allows other threads to read the list while
// we find the item.  If we do not locate the item we can simply release the
// *read* *lock* and return an error without causing any other *reading* thread
// to block.  (Again, other writers *will* block until this *read* *lock* is
// released.)
//..
        d_lock.lockRead();
        InfoMap::iterator it = d_infoMap.find(userId);
        if (d_infoMap.end() != it) {
//..
// Since 'it != end()', we found the item.  Now we need to upgrade to a *write*
// *lock*.  If we can't do this atomically, then we need to locate the item
// again.  This is because another thread may have changed 'd_infoMap' during
// the time between our *read* and *write* locks.
//..
            if (d_lock.upgradeToWriteLock()) {
                it = d_infoMap.find(userId);
            }
//..
// This is a little more costly, but since we don't expect many concurrent
// writes, it should not happen often.  In the (likely) event that we do
// upgrade to a *write* *lock* atomically, then the second lookup above is not
// performed.  In any case, we can now update the information and release the
// lock, since we already have a pointer to the item and we know that the list
// could not have been changed by anyone else.
//..
            if (d_infoMap.end() != it) {
                it->second = *userInfo;
                ret = 0;
            }
            d_lock.unlock();
        }
        else {
            d_lock.unlock();
        }
        return ret;
    }

    inline
    int UserInfoCache::addUserInfo(int userId, UserInfo *userInfo)
    {
        d_lock.lockRead();
        bool found = !! d_infoMap.count(userId);
        if (! found) {
            if (d_lock.upgradeToWriteLock()) {
                found = !! d_infoMap.count(userId);
            }
            if (! found) {
                d_infoMap[userId] = *userInfo;
            }
            d_lock.unlock();
        }
        else {
            d_lock.unlock();
        }
        return found ? 1 : 0;
    }

    inline
    void UserInfoCache::removeUser(int userId)
    {
        d_lock.lockWrite();
        d_infoMap.erase(userId);
        d_lock.unlock();
    }
//..

struct my_ThreadArgument
{
    bslmt::Barrier            *d_barrier_p;
    bslmt::ReaderWriterLock   *d_lock_p;
    bsls::AtomicInt           *d_shared_p;
};

    static bsls::AtomicInt   numConcurrent;
    static bsls::AtomicInt   numReaders;

extern "C" void *case11ThreadRW(void *arg)
{
    my_ThreadArgument    a = *static_cast<my_ThreadArgument*>(arg);
    bsls::AtomicInt&      k(*a.d_shared_p);

    a.d_barrier_p->wait();

    enum { k_NUM_ITERATIONS = 2000 };

    for(int i=0; i < k_NUM_ITERATIONS; ++i){
        a.d_lock_p->lockReadReserveWrite();
        int nc = ++numConcurrent;
        LOOP_ASSERT(i, 1 == nc);
        if (1 != nc) {
            NL; P(nc);
        }
        LOOP_ASSERT(i, 0 == k);
        if (0 != k) {
            NL; P(k);
        }
        // Only this thread should be able to change the value since it
        // reserved the write lock.
        ++k;
        LOOP_ASSERT(i, 1 == k);

        if(i & 1){
            --k;
            LOOP_ASSERT(i, 0 == k);
            nc = --numConcurrent;
            LOOP_ASSERT(i, 0 == nc);
            if (0 != nc) {
                NL; P(nc);
            }
            a.d_lock_p->unlock();
        } else{
            LOOP_ASSERT(i, 0 == a.d_lock_p->upgradeToWriteLock());
            ++k;
            LOOP_ASSERT(i, 2 == k);
            k -= 2;
            LOOP_ASSERT(i, 0 == k);
            nc = --numConcurrent;
            LOOP_ASSERT(i, 0 == nc);
            if (0 != nc) {
                NL; P(nc);
            }
            a.d_lock_p->unlock();
        }
    }
    return 0;
}

extern "C" void *case11ThreadRO(void *arg)
{
    my_ThreadArgument    a = *static_cast<my_ThreadArgument*>(arg);
    bsls::AtomicInt&      k(*a.d_shared_p);

    a.d_barrier_p->wait();
    register int v;

    enum { k_NUM_ITERATIONS = 2000 };

    for(int i=0; i < k_NUM_ITERATIONS; ++i){
        a.d_lock_p->lockRead();
        v = k; LOOP_ASSERT(i, 0 == v || 1 == v );
        a.d_lock_p->unlock();

        a.d_lock_p->lockRead();
        v = k; LOOP_ASSERT(i, 0 == v || 1 == v );
        a.d_lock_p->unlock();

        a.d_lock_p->lockRead();
        v = k; LOOP_ASSERT(i, 0 == v || 1 == v );
        a.d_lock_p->unlock();
    }
    return 0;
}

extern "C" void *case10Thread(void *arg)
{
    bslmt::ReaderWriterLock *mX = (Obj*) arg;
    ASSERT(mX);

    int k_NUM_ITERATIONS = 10;
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
    }
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
    }
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
    }
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
    }
    return arg;
}

extern "C" void *case9Thread(void *arg)
{
    bslmt::ReaderWriterLock *mX = (Obj*) arg;
    ASSERT(mX);

    int k_NUM_ITERATIONS = 10;
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->unlock();

    }
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->unlock();

    }
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->unlock();

    }
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->unlock();

    }
    return arg;
}

extern "C" void *case8Thread(void *arg)
{
    bslmt::ReaderWriterLock *mX = (Obj*) arg;
    ASSERT(mX);

    int k_NUM_ITERATIONS = 800;
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockRead();
        mX->upgradeToWriteLock();
        mX->unlock();
        // 2
        mX->lockRead();
        mX->upgradeToWriteLock();
        mX->unlock();
    }

    return arg;
}

extern "C" void *case7Thread(void *arg)
{
    bslmt::ReaderWriterLock *mX = (Obj*) arg;
    ASSERT(mX);

    int k_NUM_ITERATIONS = 10;
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockRead();
        mX->unlock();
        mX->lockWrite();
        mX->unlock();
        // 2
        mX->lockRead();
        mX->unlock();
        mX->lockWrite();
        mX->unlock();
    }

    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockRead();
        mX->unlock();
        mX->lockWrite();
        mX->unlock();
        // 2
        mX->lockRead();
        mX->unlock();
        mX->lockWrite();
        mX->unlock();
    }

    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockRead();
        mX->unlock();
        mX->lockWrite();
        mX->unlock();
        // 2
        mX->lockRead();
        mX->unlock();
        mX->lockWrite();
        mX->unlock();
    }
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        // 1
        mX->lockRead();
        mX->unlock();
        mX->lockWrite();
        mX->unlock();
        // 2
        mX->lockRead();
        mX->unlock();
        mX->lockWrite();
        mX->unlock();
    }
    return arg;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void)verbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 11: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: reserve & upgrade 2"
                 << endl
                 << "================================================="
                 << endl;
        {
            enum { k_NUM_THREADS = 5 };
            bsls::AtomicInt             shared;
            bslmt::Barrier              barrier(k_NUM_THREADS);
            bslmt::ReaderWriterLock     lock;
            bslmt::ThreadUtil::Handle   workers[k_NUM_THREADS];
            my_ThreadArgument          argument = { &barrier, &lock, &shared };

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                bslmt::ThreadUtil::ThreadFunction  threadMain = (i % 2)
                                                             ? case11ThreadRW
                                                             : case11ThreadRO;
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                  bslmt::ThreadAttributes(),
                                                  threadMain,
                                                  &argument);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      }  break;
      case 10: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: reserve & upgrade" << endl
                 << "===============================================" << endl;
        {
            enum { k_NUM_THREADS = 10 };
            bslmt::ReaderWriterLock mX;
            bslmt::ThreadUtil::Handle workers[k_NUM_THREADS];

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                  bslmt::ThreadAttributes(),
                                                  &case10Thread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      } break;
      case 9: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: reserve/unreserve" << endl
                 << "===============================================" << endl;
        {
            enum { k_NUM_THREADS = 10 };
            bslmt::ReaderWriterLock mX;
            bslmt::ThreadUtil::Handle workers[k_NUM_THREADS];

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                  bslmt::ThreadAttributes(),
                                                  &case9Thread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      } break;

      case 8: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: with upgrade" << endl
                 << "==========================================" << endl;
        {
            enum { k_NUM_THREADS = 3 };
            bslmt::ReaderWriterLock mX;
            bslmt::ThreadUtil::Handle workers[k_NUM_THREADS];

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                  bslmt::ThreadAttributes(),
                                                  &case8Thread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      } break;
      case 7: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: no upgrade" << endl
                 << "========================================" << endl;
        {
            enum { k_NUM_THREADS = 10 };
            bslmt::ReaderWriterLock mX;
            bslmt::ThreadUtil::Handle workers[k_NUM_THREADS];

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                  bslmt::ThreadAttributes(),
                                                  &case7Thread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < k_NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST: USAGE EXAMPLE
        //   The usage example demonstrates a sample class which uses
        //   a 'bslmt::ReaderWriterLock' object.  By integrating the code
        //   into this test driver, we have verified that it compiles.
        //   Next verify that the class can be instantiated.
        // Plan:
        //   Construct an instance of the example class to assert that the
        //   the object can be instantiated.
        //
        // Testing:
        // --------------------------------------------------------------------
        if (veryVerbose) cout << endl
                              << "Testing: USAGE EXAMPLE" << endl
                              << "=============================" << endl;
        {

        }
      }break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST: Reservered write locks
        //
        // Plan:
        //
        // Testing:
        //   void unlock()
        //   void upgradeToWriteLock();
        //   void lockReadReserveWrite();
        // --------------------------------------------------------------------
        if (veryVerbose) cout << endl
                              << "Testing: Reserved write locks" << endl
                              << "=============================" << endl;
        {
            Obj lock;
            lock.lockReadReserveWrite();
            ASSERT(0 == lock.tryLockRead());
            lock.unlock();
            ASSERT(0 != lock.tryLockWrite());
            lock.unlock();
            ASSERT(0 == lock.tryLockWrite());
            ASSERT(0 != lock.tryLockRead());
            lock.unlock();
        }
        if (veryVerbose) cout << endl
                              << "Testing: 'lockReadReserveWrite'" << endl
                              << "-------------------------------" << endl;
        {
            TestArguments          args;

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];
            bslmt::ThreadAttributes attributes;

            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread2, &args );
            }
            args.d_barrierAll.wait();

            for ( int i=0; i<k_NTHREADS;i++) {
                args.d_barrier2.wait();
                LOOP_ASSERT( i, (i+1) == args.d_count );
                if (veryVeryVerbose) {
                    T_; P(args.d_count);
                }
                args.signalStop();
                args.waitStart();
            }
            ASSERT( k_NTHREADS == args.d_count );
            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }

            if (veryVerbose) {
                T_; P_(args.d_count); P(k_NTHREADS);
            }
        }
        if (veryVerbose) cout << endl
                              << "\tTesting: after unlock" << endl
                              << "\t---------------------" << endl;
        {
            TestArguments          args;

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];
            bslmt::ThreadAttributes attributes;

            args.d_lock.lockWrite();

            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread2, &args );
            }

            args.d_barrierAll.wait();
            ASSERT(0 == args.d_count);

            args.d_lock.unlock();
            for (int i=0; i<k_NTHREADS;i++) {
                args.d_barrier2.wait();
                if (veryVeryVerbose) {
                    T_; P(args.d_count);
                }
                args.signalStop();
                args.waitStart();
            }

            ASSERT(k_NTHREADS == args.d_count);
            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }
        }
        if (veryVerbose) cout << endl
                              << "Testing: 'unlock'" << endl
                              << "-----------------" << endl;
        {
            TestArguments          args;

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];
            bslmt::ThreadAttributes attributes;

            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread2, &args );
            }
            args.d_barrierAll.wait();

            for ( int i=0; i<k_NTHREADS;i++) {
                args.d_barrier2.wait();
                LOOP_ASSERT( i, (i+1) == args.d_count );
                if (veryVeryVerbose) {
                    T_; P(args.d_count);
                }
                args.signalStop();
                args.waitStart();
            }

            ASSERT( k_NTHREADS == args.d_count );
            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }

            if (veryVerbose) {
                T_; P_(args.d_count); P(k_NTHREADS);
            }
        }
        if (veryVerbose) cout << endl
                              << "Testing: 'upgradeToWriteLock'"
                              << "-----------------------------"
                              << endl;
        {
            const int NRLOCKS  = 5;
            TestArguments          args;

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];
            bslmt::ThreadAttributes attributes;

            for (int i=0; i<NRLOCKS; ++i) {
                args.d_lock.lockRead();
            }

            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread3, &args );

            }

            args.d_barrierAll.wait();

            for (int i=0; i<NRLOCKS; ++i) {
                if (veryVeryVerbose) {
                    T_; P(args.d_count);
                }
                args.d_lock.unlock();
            }

            for ( int i=0; i<k_NTHREADS;i++) {
                LOOP_ASSERT( i, i == args.d_count );
                if (veryVeryVerbose) {
                    T_; P(args.d_count);
                }
                args.signalStop();
                if ( i < k_NTHREADS ) {
                    args.waitStart();
                }
            }

            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }

            if (veryVerbose) {
                T_; P_(args.d_count); P(k_NTHREADS);
            }
        }
      } break;

      case 4: {
        // TEST: lock upgrade
        //
        // Plan:
        //
        // Testing:
        //   int tryUpgradeToWriteLock();
        //   int upgradeToWriteLock();
        // --------------------------------------------------------------------
        if (veryVerbose) cout << endl
                              << "Testing: lock upgrade" << endl
                              << "====================="
                              << endl;

        {
            Obj lock;
            lock.lockRead();
            ASSERT(0 == lock.tryUpgradeToWriteLock());
            ASSERT(0 != lock.tryUpgradeToWriteLock());
            ASSERT(0 != lock.tryLockWrite());
            ASSERT(0 != lock.tryLockRead());
            lock.unlock();
            ASSERT(0 == lock.tryLockRead());
            lock.unlock();
        }

        if (veryVerbose) cout << endl
                              << "\tTesting: 'upgradeToWriteLock'" << endl
                              << "\t-----------------------------"
                              << endl;
        {
            TestArguments          args(k_NTHREADS+1);

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];
            bslmt::ThreadAttributes attributes;

            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                         TestUpgradeThread1, (void*)&args);
            }
            args.d_barrierAll.wait();
            if (veryVeryVerbose) {
                T_; P(args.d_count);
            }

            for (int i=0; i<k_NTHREADS;i++) {
                while (i != args.d_count) {
                    // Some highly optimized implementations like AIX will spin
                    // and prevent other threads from preempting.

                    bslmt::ThreadUtil::yield();
                }
                if (veryVeryVerbose) {
                    T_; P(args.d_count);
                }
                args.signalStop();
            }
            args.signalStop();

            for (int i=0; i<k_NTHREADS; ++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }
        }
      }break;
      case 3: {
        // TEST: basic read locks
        //   Assert that basic read lock and unlock functions work properly.
        //
        // Plan:
        //
        // Testing:
        //   void lockRead();
        //   int tryLockRead();
        //   void unlock();
        // --------------------------------------------------------------------
        {
            Obj lock;
            lock.lockRead();
            ASSERT(0 == lock.tryLockRead());
            lock.unlock();
            ASSERT(0 != lock.tryLockWrite());
            lock.unlock();
            ASSERT(0 == lock.tryLockWrite());
            ASSERT(0 != lock.tryLockRead());
            lock.unlock();
        }

        if (veryVerbose) cout << endl
                              << "Testing: 'lockRead', and 'unlock'"
                              << "---------------------------------"
                              << endl;

        {
            TestArguments          args(k_NTHREADS+1);

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];
            bslmt::ThreadUtil::Handle writerHandle;
            bslmt::ThreadAttributes attributes;

            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                         TestReaderThread2, (void*)&args);
            }
            args.d_barrierAll.wait();
            args.d_barrierAll.wait();
            ASSERT(k_NTHREADS == args.d_readCount);

            bslmt::ThreadUtil::create(&writerHandle, attributes,
                                     TestWriterThread4, (void*)&args);
            args.d_barrier2.wait();

            for (int i=0; i<k_NTHREADS;i++) {
                LOOP_ASSERT(i, 0 != args.d_lock.tryLockWrite());
                LOOP_ASSERT(i, 0 == args.d_writeCount);
                if (veryVeryVerbose || 0 != args.d_writeCount) {
                    T_;
                    P_(args.d_writeCount);
                    P(args.d_count);
                    T_; P_(args.d_count); P(args.d_readCount);
                }
                args.signalStop();
            }

            args.d_barrier2.wait();
            ASSERT(1 == args.d_writeCount);
            args.signalStop();
            bslmt::ThreadUtil::join(writerHandle);
            for (int i=0; i<k_NTHREADS; ++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST: basic write locks
        //
        // Plan:
        //
        //
        // Testing:
        //   void lockWrite();
        //   void tryLockWrite();
        //   void unlock();
        // --------------------------------------------------------------------
        if (veryVerbose) cout << endl
                              << "Testing: basic write locks" << endl
                              << "=========================="
                              << endl;
        {
            Obj lock;
            lock.lockRead();
            ASSERT(0 != lock.tryLockWrite());
            lock.lockRead();
            ASSERT(0 != lock.tryLockWrite());
            lock.unlock();
            ASSERT(0 != lock.tryLockWrite());
            lock.unlock();
            ASSERT(0 == lock.tryLockWrite());
        }

        if (veryVerbose) cout << endl
                              << "Testing: 'lockWrite', and 'unlock'"
                              << endl
                              << "----------------------------------"
                              << endl;

        {
            TestArguments          args(k_NTHREADS+1);

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];
            bslmt::ThreadAttributes attributes;

            for (int i=0; i<k_NTHREADS; ++i) {
                bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread1, (void*)&args);
            }

            args.d_barrierAll.wait();

            for (int i=0; i < k_NTHREADS; ++i) {
                args.d_barrier2.wait();
                LOOP_ASSERT(i, (i+1) == args.d_count);
                if (veryVeryVerbose) {
                    T_; P_(i); P(args.d_count);
                }

                args.signalStop();
                if (i < (k_NTHREADS - 1)) {
                    args.waitStart();
                }
            }

            ASSERT(k_NTHREADS == args.d_count);
            for (int i=0; i<k_NTHREADS;++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }

            if (veryVerbose) {
                T_; P_(args.d_count); P(k_NTHREADS);
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad selection of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 8], and
        //   assignment operator without [9, 10] and with [11] aliasing.  Use
        //   the direct accessors to verify the expected results.  Display
        //   object values frequently in verbose mode.  Note that 'VA', 'VB',
        //   and 'VC' denote unique, but otherwise arbitrary, object values,
        //   while '0' denotes the default object value.
        //
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        Obj x1;

        ASSERT(0 == x1.tryLockWrite());
        ASSERT(0 != x1.tryLockWrite());
        ASSERT(0 != x1.tryLockRead());
        x1.unlock();

        ASSERT(0 == x1.tryLockRead());
        ASSERT(0 == x1.tryLockRead());
        ASSERT(0 != x1.tryLockWrite());

        x1.unlock();
        ASSERT(0 != x1.tryLockWrite());
        ASSERT(0 == x1.upgradeToWriteLock());

        ASSERT(0 != x1.tryLockRead());
        x1.unlock();

        x1.lockRead();
        ASSERT(0 == x1.tryLockRead());
        x1.unlock();
        ASSERT(0 != x1.tryLockWrite());
        x1.unlock();

        x1.lockWrite();
        ASSERT(0 != x1.tryLockRead());
        ASSERT(0 != x1.tryLockWrite());
        x1.unlock();

        Obj x2;

        x2.lockReadReserveWrite();
        ASSERT(0 != x2.tryLockWrite());
        ASSERT(0 == x2.tryLockRead());
        x2.unlock();
        ASSERT(0 != x2.tryLockWrite());
        x2.unlock();
        ASSERT(0 == x2.tryLockWrite());
        x2.unlock();

        Obj x3;

        x3.lockReadReserveWrite();
        ASSERT(0 != x3.tryLockWrite());
        x3.upgradeToWriteLock();
        ASSERT(0 != x3.tryLockWrite());
        ASSERT(0 != x3.tryLockRead());
        x3.unlock();

        ASSERT(0 == x3.tryLockWrite());
        ASSERT(0 != x3.tryLockRead());
        x3.unlock();
        ASSERT(0 == x3.tryLockRead());
        ASSERT(0 != x3.tryLockWrite());
        x3.unlock();

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

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
