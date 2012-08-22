// bcemt_readerwriterlock.t.cpp    -*-C++-*-

#include <bcemt_readerwriterlock.h>

#include <bcemt_barrier.h>     // for testing only
#include <bdetu_systemtime.h>  // for testing only
#include <bdet_datetime.h>     // for testing only
#include <bces_atomictypes.h>  // for testing only

#include <bsl_map.h>           // for usage example

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM__OS_UNIX
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
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error "__FILE__"(%d): %s"
               "    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcemt_ReaderWriterLock Obj;
enum { NTHREADS = 5 };

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------

class my_Condition {
    // This class implements a condition variable object which compensates for
    // spurious returns from the wait functions in bcemt_Condition.  The
    // versions of the 'wait' and 'timedWait' functions provided here only
    // return when the condition was actually signaled.  The class is used to
    // simplify testing.
    bcemt_Condition       d_cond;
    bcemt_Mutex           d_mutex;
    volatile int          d_sigState;
    volatile int          d_bcastCount;
  public:
    my_Condition();
        // Construct a my_Condition object.

    ~my_Condition();
        // Destroy a my_Condition object.

    void wait(bcemt_Mutex *mutex);
        // Block until this condition is signaled by a call to 'signal' or
        // 'broadcast'.
    int  timedWait(bcemt_Mutex *mutex, const bdet_TimeInterval &timeout);
        // Block until this condition is signaled by a call to 'signal' or
        // 'broadcast', or until the specified timeout(in abs time).  Return
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

void my_Condition::wait(bcemt_Mutex *mutex)
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

int  my_Condition::timedWait(bcemt_Mutex *mutex,
                             const bdet_TimeInterval &timeout)
{
    d_mutex.lock();
    int bcastCount = d_bcastCount;
    d_sigState = 0;
    mutex->unlock();
    while (!d_sigState && d_bcastCount == bcastCount) {
        if (d_cond.timedWait(&d_mutex,timeout)) {
            d_mutex.unlock();
            mutex->lock();
            return -1;
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
    bcemt_Mutex      d_mutex;
    volatile int     d_startSigCount;
    volatile int     d_stopSigCount;
    volatile int     d_iterations;
    bces_AtomicInt   d_count;
    bces_AtomicInt   d_readCount;
    bces_AtomicInt   d_writeCount;
    bcemt_Barrier    d_barrierAll;     // barrier for all threads
    bcemt_Barrier    d_barrier2;       // barrier for two threads
  public:
    TestArguments(int iterations=0, int nThreads=NTHREADS);
        // Construct a 'TestArguments' object and initialize all counters to
        // zero, 'd_iterations' to the specified 'iterations' and
        // 'd_barrierAll' to 'nThreads' + 1.

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
    // This function is used to test the 'lockRead' and 'unlock' methods.
    // It begins by invoking 'wait' on the barrier for all the threads.
    // Then it signals the provided condition variable 'd_cond1'.
    // Next it calls 'lockRead' on the provided reader/writer lock.  Once it
    // has acquired the lock, it again signals 'd_cond1' and waits for
    // 'd_cond2' to be signaled.  Finally it releases the reader/writer lock
    // using the 'unlock' method.
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
    // This function is used to test the 'lockRead' and 'unlock' methods.
    // It begins by invoking 'wait' on the barrier for all the threads.
    // Then it signals the provided condition variable 'd_cond1'.
    // Next it calls 'lockRead' on the provided reader/writer lock.  Once it
    // has acquired the lock, it again signals 'd_cond1' and waits for
    // 'd_cond2' to be signaled.  Finally it releases the reader/writer lock
    // using the 'unlock' method.
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
    // This function is used to test the 'lockWrite' and 'unlock' methods.
    // It begins by invoking 'wait' on the barrier for all the threads.
    // Then it signals the provided condition variable 'd_cond1'.
    // Next it calls 'lockWrite' on the provided reader/writer lock.  Once it
    // has acquired the lock, it again signals 'd_cond1' and waits for
    // 'd_cond2' to be signaled.  Finally it releases the reader/writer lock
    // using the 'unlockWrite' method.
{
    static bcemt_Mutex m;
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
    // This function is used to test the 'lockReadReserveWrite' and
    // 'unlock' methods.  It begins by invoking 'wait' on the
    // barrier for all the threads.  Then it signals the provided condition
    // variable 'd_cond1'.  Next it calls 'lockReadReserveWrite' on the
    // provided reader/writer lock.  Once it has acquired the lock, it again
    // signals 'd_cond1' and waits for 'd_cond2' to be signaled.  Finally it
    // releases the reader/writer lock using the 'unlock'
    // method.
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
    // 'upgradeToWriteLock' methods.  It begins by invoking 'wait' on
    // the barrier for all the threads.  Then it signals the provided condition
    // variable 'd_cond1'.  Next it calls 'lockReadReserveWrite' on the
    // provided reader/writer lock.  Once it has acquired the lock, it again
    // signals 'd_cond1' and waits for 'd_cond2' to be signaled.  Finally it
    // upgrades the lock to a write lock using the 'upgradeToWriteLock'
    // method and releases the lock using the 'unlock' method.
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
    // This function is used to test the 'lockWrite' and 'unlock' methods.
    // It begins by invoking 'wait' on the barrier for all the threads.
    // Then it signals the provided condition variable 'd_cond1'.
    // Next it calls 'lockWrite' on the provided reader/writer lock.  Once it
    // has acquired the lock, it again signals 'd_cond1' and waits for
    // 'd_cond2' to be signaled.  Finally it releases the reader/writer lock
    // using the 'unlockWrite' method.
{
    static bcemt_Mutex m;
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
    // This function is used to test the '' upgradeToWriteLock'
    // methods.  It begins by invoking 'wait' on the barrier for all the
    // threads.  Then it signals the provided condition variable 'd_cond1'.
    // Next it calls 'lockReadReserveWrite' on the provided reader/writer lock.
    // Once it has acquired the lock, it again signals 'd_cond1' and waits for
    // 'd_cond2' to be signaled.  Finally it upgrades the lock to a write lock
    // using the 'upgradeToWriteLock' method and releases the lock
    // using the 'unlock' method.
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

//=============================================================================
//                         USAGE EXAMPLE
//-----------------------------------------------------------------------------

#define MAX_USER_NAME 40
#define MAX_BADGE_LOCATION 40

struct UserInfo{
    long d_UserId;
    char d_UserName[MAX_USER_NAME];
    char d_badge_location[MAX_BADGE_LOCATION];
    bdet_Datetime d_badgeTime;
};

class UserInfoCache {
    typedef bsl::map<int, UserInfo> InfoMap;

    Obj     d_lock;
    InfoMap d_infoMap;
  public:
    UserInfoCache();
    ~UserInfoCache();

    int getUserInfo(int userId, UserInfo *userInfo);
    int updateUserInfo(int userId,UserInfo *userInfo);
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
int UserInfoCache::updateUserInfo(int userId, UserInfo *newInfo)
{
    int ret = 1;
    d_lock.lockRead();
    InfoMap::iterator it = d_infoMap.find(userId);
    if (d_infoMap.end() != it) {
        if (d_lock.upgradeToWriteLock()) {
            it = d_infoMap.find(userId);
        }
        if (d_infoMap.end() != it) {
            it->second = *newInfo;
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

struct my_ThreadArgument
{
    bcemt_Barrier            *d_barrier_p;
    bcemt_ReaderWriterLock   *d_lock_p;
    bces_AtomicInt           *d_shared_p;
};

    static bces_AtomicInt   numConcurrent;
    static bces_AtomicInt   numReaders;

extern "C" void *case11ThreadRW(void *arg)
{
    my_ThreadArgument    a = *static_cast<my_ThreadArgument*>(arg);
    bces_AtomicInt&      k = *a.d_shared_p;

    a.d_barrier_p->wait();

    enum { NUM_ITERATIONS = 2000 };

    for(int i=0; i < NUM_ITERATIONS; ++i){
        a.d_lock_p->lockReadReserveWrite();
        int nc = ++numConcurrent;
        LOOP_ASSERT(i, 1 == nc);
        if (1 != nc) {
            NL(); P(nc);
        }
        LOOP_ASSERT(i, 0 == k);
        if (0 != k) {
            NL(); P(k);
        }
        // Only this thread should be able to change the value since
        // it reserved the write lock.
        ++k;
        LOOP_ASSERT(i, 1 == k);

        if(i & 1){
            --k;
            LOOP_ASSERT(i, 0 == k);
            nc = --numConcurrent;
            LOOP_ASSERT(i, 0 == nc);
            if (0 != nc) {
                NL(); P(nc);
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
                NL(); P(nc);
            }
            a.d_lock_p->unlock();
        }
    }
    return 0;
}

extern "C" void *case11ThreadRO(void *arg)
{
    my_ThreadArgument    a = *static_cast<my_ThreadArgument*>(arg);
    bces_AtomicInt&      k = *a.d_shared_p;

    a.d_barrier_p->wait();
    register int v;

    enum { NUM_ITERATIONS = 2000 };

    for(int i=0; i < NUM_ITERATIONS; ++i){
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
    bcemt_ReaderWriterLock *mX = (Obj*) arg;
    ASSERT(mX);

    int NUM_ITERATIONS = 10;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
    }
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
    }
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->upgradeToWriteLock();
        mX->unlock();
    }
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
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
    bcemt_ReaderWriterLock *mX = (Obj*) arg;
    ASSERT(mX);

    int NUM_ITERATIONS = 10;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->unlock();

    }
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->unlock();

    }
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // 1
        mX->lockReadReserveWrite();
        mX->unlock();
        // 2
        mX->lockReadReserveWrite();
        mX->unlock();

    }
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
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
    bcemt_ReaderWriterLock *mX = (Obj*) arg;
    ASSERT(mX);

    int NUM_ITERATIONS = 800;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
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
    bcemt_ReaderWriterLock *mX = (Obj*) arg;
    ASSERT(mX);

    int NUM_ITERATIONS = 10;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
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

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
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

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
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
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    // int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 11: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: reserve & upgrade 2"
                 << endl
                 << "================================================="
                 << endl;
        {
            enum { NUM_THREADS = 5 };
            bces_AtomicInt             shared;
            bcemt_Barrier              barrier(NUM_THREADS);
            bcemt_ReaderWriterLock     lock;
            bcemt_ThreadUtil::Handle   workers[NUM_THREADS];
            my_ThreadArgument          argument = { &barrier, &lock, &shared };

            for (int i = 0; i < NUM_THREADS; ++i) {
                bcemt_ThreadUtil::ThreadFunction  threadMain = (i % 2)
                                                             ? case11ThreadRW
                                                             : case11ThreadRO;
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_ThreadAttributes(),
                                                  threadMain,
                                                  &argument);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      }  break;
      case 10: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: reserve & upgrade" << endl
                 << "===============================================" << endl;
        {
            enum { NUM_THREADS = 10 };
            bcemt_ReaderWriterLock mX;
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_ThreadAttributes(),
                                                  &case10Thread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      } break;
      case 9: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: reserve/unreserve" << endl
                 << "===============================================" << endl;
        {
            enum { NUM_THREADS = 10 };
            bcemt_ReaderWriterLock mX;
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_ThreadAttributes(),
                                                  &case9Thread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      } break;

      case 8: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: with upgrade" << endl
                 << "==========================================" << endl;
        {
            enum { NUM_THREADS = 3 };
            bcemt_ReaderWriterLock mX;
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_ThreadAttributes(),
                                                  &case8Thread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      } break;
      case 7: {
        if (veryVerbose)
            cout << "Highly-parallel RW lock test: no upgrade" << endl
                 << "========================================" << endl;
        {
            enum { NUM_THREADS = 10 };
            bcemt_ReaderWriterLock mX;
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_ThreadAttributes(),
                                                  &case7Thread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST: USAGE EXAMPLE
        //   The usage example demonstrates a sample class which uses
        //   a 'bcemt_ReaderWriterLock' object.  By integrating the code
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

            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadAttributes attributes;

            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread2, &args );
            }
            args.d_barrierAll.wait();

            for ( int i=0; i<NTHREADS;i++) {
                args.d_barrier2.wait();
                LOOP_ASSERT( i, (i+1) == args.d_count );
                if (veryVeryVerbose) {
                    T_(); P(args.d_count);
                }
                args.signalStop();
                args.waitStart();
            }
            ASSERT( NTHREADS == args.d_count );
            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
            }

            if (veryVerbose) {
                T_(); P_(args.d_count); P(NTHREADS);
            }
        }
        if (veryVerbose) cout << endl
                              << "\tTesting: after unlock" << endl
                              << "\t---------------------" << endl;
        {
            TestArguments          args;

            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadAttributes attributes;

            args.d_lock.lockWrite();

            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread2, &args );
            }

            args.d_barrierAll.wait();
            ASSERT(0 == args.d_count);

            args.d_lock.unlock();
            for (int i=0; i<NTHREADS;i++) {
                args.d_barrier2.wait();
                if (veryVeryVerbose) {
                    T_(); P(args.d_count);
                }
                args.signalStop();
                args.waitStart();
            }

            ASSERT(NTHREADS == args.d_count);
            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
            }
        }
        if (veryVerbose) cout << endl
                              << "Testing: 'unlock'" << endl
                              << "-----------------" << endl;
        {
            TestArguments          args;

            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadAttributes attributes;

            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread2, &args );
            }
            args.d_barrierAll.wait();

            for ( int i=0; i<NTHREADS;i++) {
                args.d_barrier2.wait();
                LOOP_ASSERT( i, (i+1) == args.d_count );
                if (veryVeryVerbose) {
                    T_(); P(args.d_count);
                }
                args.signalStop();
                args.waitStart();
            }

            ASSERT( NTHREADS == args.d_count );
            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
            }

            if (veryVerbose) {
                T_(); P_(args.d_count); P(NTHREADS);
            }
        }
        if (veryVerbose) cout << endl
                              << "Testing: 'upgradeToWriteLock'"
                              << "-----------------------------"
                              << endl;
        {
            const int NRLOCKS  = 5;
            TestArguments          args;

            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadAttributes attributes;

            for (int i=0; i<NRLOCKS; ++i) {
                args.d_lock.lockRead();
            }

            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread3, &args );

            }

            args.d_barrierAll.wait();

            for (int i=0; i<NRLOCKS; ++i) {
                if (veryVeryVerbose) {
                    T_(); P(args.d_count);
                }
                args.d_lock.unlock();
            }

            for ( int i=0; i<NTHREADS;i++) {
                LOOP_ASSERT( i, i == args.d_count );
                if (veryVeryVerbose) {
                    T_(); P(args.d_count);
                }
                args.signalStop();
                if ( i < NTHREADS ) {
                    args.waitStart();
                }
            }

            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
            }

            if (veryVerbose) {
                T_(); P_(args.d_count); P(NTHREADS);
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
            TestArguments          args(NTHREADS+1);

            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadAttributes attributes;

            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         TestUpgradeThread1, (void*)&args);
            }
            args.d_barrierAll.wait();
            if (veryVeryVerbose) {
                T_(); P(args.d_count);
            }

            for (int i=0; i<NTHREADS;i++) {
                while (i != args.d_count) {
                    // Some highly optimized implementations like AIX will spin
                    // and prevent other threads from preempting.

                    bcemt_ThreadUtil::yield();
                }
                if (veryVeryVerbose) {
                    T_(); P(args.d_count);
                }
                args.signalStop();
            }
            args.signalStop();

            for (int i=0; i<NTHREADS; ++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
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
            TestArguments          args(NTHREADS+1);

            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadUtil::Handle writerHandle;
            bcemt_ThreadAttributes attributes;

            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         TestReaderThread2, (void*)&args);
            }
            args.d_barrierAll.wait();
            args.d_barrierAll.wait();
            ASSERT(NTHREADS == args.d_readCount);

            bcemt_ThreadUtil::create(&writerHandle, attributes,
                                     TestWriterThread4, (void*)&args);
            args.d_barrier2.wait();

            for (int i=0; i<NTHREADS;i++) {
                LOOP_ASSERT(i, 0 != args.d_lock.tryLockWrite());
                LOOP_ASSERT(i, 0 == args.d_writeCount);
                if (veryVeryVerbose || 0 != args.d_writeCount) {
                    T_();
                    P_(args.d_writeCount);
                    P(args.d_count);
                    T_(); P_(args.d_count); P(args.d_readCount);
                }
                args.signalStop();
            }

            args.d_barrier2.wait();
            ASSERT(1 == args.d_writeCount);
            args.signalStop();
            bcemt_ThreadUtil::join(writerHandle);
            for (int i=0; i<NTHREADS; ++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
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
            TestArguments          args(NTHREADS+1);

            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadAttributes attributes;

            for (int i=0; i<NTHREADS; ++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         TestWriterThread1, (void*)&args);
            }

            args.d_barrierAll.wait();

            for (int i=0; i < NTHREADS; ++i) {
                args.d_barrier2.wait();
                LOOP_ASSERT(i, (i+1) == args.d_count);
                if (veryVeryVerbose) {
                    T_(); P_(i); P(args.d_count);
                }

                args.signalStop();
                if (i < (NTHREADS - 1)) {
                    args.waitStart();
                }
            }

            ASSERT(NTHREADS == args.d_count);
            for (int i=0; i<NTHREADS;++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
            }

            if (veryVerbose) {
                T_(); P_(args.d_count); P(NTHREADS);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
