// btlmt_tcptimereventmanager.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlmt_tcptimereventmanager.h>
#include <btlso_socketimputil.h>
#include <btlso_eventmanagertester.h>
#include <btlso_inetstreamsocketfactory.h>
#include <btlso_ipv4address.h>
#include <btlso_streamsocket.h>

#include <btlso_defaulteventmanager.h>

#include <bslma_testallocator.h>
#include <bdlmt_threadpool.h>
#include <bslmt_barrier.h>
#include <bslmt_threadattributes.h>
#include <bsls_atomic.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocatorexception.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bdlt_currenttime.h>
#include <bdlt_time.h>
#include <bdlt_currenttime.h>
#include <btlso_socketimputil.h>

#include <bdlf_bind.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <sys/resource.h>   // getrlimit()
#endif

using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a mechanism that implements a pure protocol.
//-----------------------------------------------------------------------------
// CREATORS
// [12] TcpTimerEventManager(bslma::Allocator *basicAllocator = 0);
// [12] TcpTimerEventManager(collectTimeMetrics, *basicAllocator = 0);
// [12] TcpTimerEventManager(collectTimeMetrics, poolTimer, *ba = 0);
// [  ] TcpTimerEventManager(rawEventManager, *basicAllocator = 0);
// [12] ~TcpTimerEventManager();
//
// MANIPULATORS
// [ 7] void deregisterAllSocketEvents();
// [  ] void deregisterAllTimers();
// [  ] void deregisterAll();
// [ 8] void deregisterSocketEvent(handle, event);
// [  ] void deregisterSocket(const btlso::SocketHandle::Handle& handle);
// [  ] void deregisterTimer(const void *handle);
// [ 9] int disable();
// [11] int enable();
// [11] int enable(const bslmt::ThreadAttributes& attribute);
// [10] void execute(const bsl::function<void()>& functor);
// [  ] void clearExecuteQueue();
// [ 6] int registerSocketEvent(handle, event, callback);
// [ 4] void *registerTimer(expiryTime, callback);
// [13] int rescheduleTimer(timerId, expiryTime);
//
// ACCESSORS
// [14] bool hasLimitedSocketCapacity() const;
// [  ] int  isRegistered(const Handle& handle, const Type event);
// [  ] void numTimers();
// [  ] void numEvents();
// [  ] void numSocketEvents(const btlso::SocketHandle::Handle& handle);
// [  ] int numTotalSocketEvents() const;
// [  ] btlso::TimeMetrics *timeMetrics() const;
// [  ] bslmt::ThreadUtil::Handle dispatcherThreadHandle() const;
// [11] int isEnabled() const;
// [12] bool hasTimeMetrics() const;
//
// BUG FIXES
// [15] TEST closure of control channel sockets
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] USAGE EXAMPLE
//=============================================================================

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static bool         verbose = false,
                veryVerbose = false,
            veryVeryVerbose = false,
        veryVeryVeryVerbose = false;

bslmt::Mutex         coutMutex;
bslma::TestAllocator defaultAllocator("default");

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        printf("Error %s (%d): %s    (failed)\n", __FILE__, i, s);
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//-----------------------------------------------------------------------------
//            SEMI-STANDARD NEGATIVE TESTING CONVENIENCE MACROS
//-----------------------------------------------------------------------------

#define ASSERT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

typedef btlmt::TcpTimerEventManager Obj;

//=============================================================================
//           TEST: 'collectTimeMetrics' configuration flag
//-----------------------------------------------------------------------------

namespace TEST_CASE_COLLECT_TIME_METRICS {

void waitForSomeTime()
{
    bslmt::ThreadUtil::microSleep(10000); // 10 ms
}

}  // close namespace TEST_CASE_COLLECT_TIME_METRICS

//=============================================================================
//       ADDITIONAL 'enable' and 'isEnabled' TEST:
//-----------------------------------------------------------------------------

namespace TEST_CASE_ENABLE_TEST {

void testIsEnabled(btlmt::TcpTimerEventManager *manager,
                   bsls::AtomicInt             *complete)
    // Test whether the specified 'manager' is enabled and then set 'complete'
    // to a non-zero value.
{
    ASSERT(manager->isEnabled());
    *complete = 1;
}

}  // close namespace TEST_CASE_ENABLE_TEST

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Helper function for the test driver

static int maxOpenFiles()
    // Return the maximum number of file descriptors allowed to be opened
    // by this process on success and a negative value on failure.
{
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    return (1 << sizeof(int)) * 16 - 1;
#endif
#if defined(BSLS_PLATFORM_OS_UNIX)
    struct ::rlimit result;
    int s = ::getrlimit(RLIMIT_NOFILE, &result);
    return s ? s : result.rlim_cur;
#endif
}

enum { NUM_TASKS = 10 };

static int nbytes[NUM_TASKS];

void
timerCb(int taskId, Obj *mX)
{
    bsl::function<void()> functor(bdlf::BindUtil::bind(&timerCb,
                                                       taskId,
                                                       mX));

    bsls::TimeInterval now(bdlt::CurrentTime::now());
    bsls::TimeInterval timeout(now);
    timeout.addSeconds(1000);

    void *tmrId = mX->registerTimer(timeout, functor);
    mX->deregisterTimer(tmrId);

    timeout = now;timeout.addMilliseconds(10);
    tmrId = mX->registerTimer(timeout, functor);
}

void
socketCb(btlmt::TcpTimerEventManager                *,
         btlmt::TcpTimerEventManager_ControlChannel *c_p,
         int                                         id)
{
    nbytes[id] += c_p->serverRead();
}

btlmt::TcpTimerEventManager *pEventManager = NULL;

extern "C" void *monitorThread(void *arg)
{
    cout << "Monitor thread has started." << endl;
    while (1) {

    }
    return arg;
}

extern "C" void * caseStressTestEntryPoint(void *arg)
{
    int j = (int)(bsls::Types::IntPtr)arg;
    printf("Thread %d has started\n", j);
    bsl::function<void()> mgrReinitFunctor;
    btlmt::TcpTimerEventManager& em = *pEventManager;
    btlmt::TcpTimerEventManager_ControlChannel chnl;

    bsl::function<void()> socketFunctor(bdlf::BindUtil::bind(&socketCb,
                                                                  &em,
                                                                  &chnl,
                                                                  j));

    bsl::function<void()> timerFunctor(bdlf::BindUtil::bind(&timerCb,
                                                                 j,
                                                                 &em));

    bsls::TimeInterval timeout(bdlt::CurrentTime::now());
    timeout.addMilliseconds(10);
    em.registerTimer(timeout, timerFunctor);

    for (int i = 0; i < 50000000; ++i) {
        if (i % 100 == 0 && 0 == j && i > 0 && 0 == (i * NUM_TASKS) % 1000) {
            int dispatchEnabled = pEventManager->isEnabled();
            if (dispatchEnabled) {
                pEventManager->disable();
                puts("Disabling event manager");
            }
            else {
                pEventManager->enable();
                puts("Enabling event manager");
            }
        }

        em.registerSocketEvent(chnl.serverFd(),
                               btlso::EventType::e_READ,
                               socketFunctor);
        ASSERT(1 == em.isRegistered(chnl.serverFd(),
                                    btlso::EventType::e_READ));
        ASSERT(1 == em.numSocketEvents(chnl.serverFd()));

        em.deregisterSocket(chnl.serverFd());
        ASSERT(0 == em.isRegistered(chnl.serverFd(),
                                    btlso::EventType::e_READ));
        ASSERT(0 == em.numSocketEvents(chnl.serverFd()));

        em.registerSocketEvent(chnl.serverFd(),
                               btlso::EventType::e_READ,
                               socketFunctor);
        ASSERT(1 == em.isRegistered(chnl.serverFd(),
                                    btlso::EventType::e_READ));
        ASSERT(1 == em.numSocketEvents(chnl.serverFd()));

        ASSERT(1 == chnl.clientWrite());

        em.deregisterSocketEvent(chnl.serverFd(), btlso::EventType::e_READ);
        ASSERT(0 == em.isRegistered(chnl.serverFd(),
                                    btlso::EventType::e_READ));
        ASSERT(0 == em.numSocketEvents(chnl.serverFd()));
    }
    printf("Thread %d is exiting\n", j);
    return arg;
}

// ----------------------------------------------------------------------------
static  void disableCb(Obj *mX)
    // Disable dispatching for the specified 'mX' event manager.
{
    if (veryVerbose) {
        Q("disableCb")
    }
    ASSERT(mX);
    ASSERT(mX->isEnabled());
    ASSERT(0 != mX->disable());
    ASSERT(mX->isEnabled());
}

// ----------------------------------------------------------------------------
// Helper function for the usage example

static void producer(bdlcc::Queue<int>           *workQueue,
                     btlmt::TcpTimerEventManager *manager,
                     bsls::TimeInterval           nextTime)
    // Enqueue a work item onto the specified 'workQueue' and register this
    // function with the specified 'manager' to be invoked after the specified
    // 'nextTime' absolute time interval.
{
    ASSERT(workQueue);
    ASSERT(manager);

    enum { TIME_OFFSET = 50 };   // invoke timer every 50 milliseconds

    int item = bdlt::CurrentTime::now().nanoseconds() / 1000;

    if (verbose) {
        bsl::cout << bdlt::CurrentTime::now()
            << ": Pushing " << item << bsl::endl;
    }
    workQueue->pushBack(item);

    bsls::TimeInterval nextNextTime(nextTime);
    nextNextTime.addMilliseconds(TIME_OFFSET);
    bsl::function<void()> callback(
            bdlf::BindUtil::bind(&producer, workQueue, manager, nextNextTime));

    void *timerId = manager->registerTimer(nextTime, callback);
    ASSERT(timerId);
}

// ----------------------------------------------------------------------------
bslma::Allocator *testAllocator_p;

static void dummyFunction(void) {

}

enum { NUM_REGISTER_PAIRS = 10 };

bslmt::Barrier *globalBarrier;

extern "C"
void *registerThread(void *arg)
    // Perform registration requests for the event manager specified by 'arg'.
{
    Obj *mX = (Obj*) arg;

    ASSERT(mX);
    ASSERT(globalBarrier);

    btlso::EventManagerTestPair *testPairs;
    testPairs = (btlso::EventManagerTestPair *) (
                  new char[NUM_REGISTER_PAIRS *
                                         sizeof(btlso::EventManagerTestPair)]);

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        new (&testPairs[i]) btlso::EventManagerTestPair(veryVerbose);
    }

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        LOOP_ASSERT(i, testPairs[i].isValid());
    }

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        btlso::SocketHandle::Handle fd = testPairs[i].observedFd();
        bslma::TestAllocator ta("TestAllocator: registerThread",
                                veryVeryVeryVerbose);
        const btlso::TimerEventManager::Callback callback(
                       bsl::allocator_arg_t(),
                       bsl::allocator<btlso::TimerEventManager::Callback>(&ta),
                       &dummyFunction);
        if (veryVerbose) {
            printf("Thread %llu: Iteration (O) %d\n",
                   bslmt::ThreadUtil::selfIdAsUint64(),
                   i);
        }
        mX->registerSocketEvent(fd,
                                btlso::EventType::e_READ, callback);
        LOOP_ASSERT(i, mX->isRegistered(fd,
                                        btlso::EventType::e_READ));
    }

    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        btlso::SocketHandle::Handle fd = testPairs[i].controlFd();
        bslma::TestAllocator ta("TestAllocator: registerThread loop",
                                veryVeryVeryVerbose);
        const btlso::TimerEventManager::Callback callback(
                       bsl::allocator_arg_t(),
                       bsl::allocator<btlso::TimerEventManager::Callback>(&ta),
                       &dummyFunction);
        mX->registerSocketEvent(fd,
                                btlso::EventType::e_READ, callback);
        if (veryVerbose) {
            printf("Thread %llu: Iteration (C)%d\n",
                   bslmt::ThreadUtil::selfIdAsUint64(),
                   i);
        }
        LOOP_ASSERT(i, mX->isRegistered(fd,
                                        btlso::EventType::e_READ));
    }

    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());

    btlso::SocketHandle::Handle ofd, cfd;
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        ofd = testPairs[i].observedFd();
        mX->deregisterSocket(ofd);
        cfd = testPairs[i].controlFd();
        mX->deregisterSocket(cfd);
    }

    // 'testPairs' cannot be destroyed until ALL threads are done.

    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());


    bsls::Stopwatch watch;
    watch.start();
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        testPairs[i].~EventManagerTestPair();
    }
    delete [] (char*)testPairs;
    watch.stop();
    double elapsedTime = watch.elapsedTime();
    double limit = 0.1 * NUM_REGISTER_PAIRS;
    if (elapsedTime > limit) {
        P_(elapsedTime);
        P(limit);
    }
    ASSERT(elapsedTime < limit);
    return arg;
}

extern "C"
void *deregisterThread(void *arg)
    // Perform registration requests for the event manager specified by 'arg'.
{
    Obj *mX = (Obj*) arg;
    ASSERT(mX); ASSERT(globalBarrier);

    btlso::EventManagerTestPair testPairs[NUM_REGISTER_PAIRS];
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        LOOP_ASSERT(i, testPairs[i].isValid());
    }
    bslma::TestAllocator ta("TestAllocator: deregisterThread",
                            veryVeryVeryVerbose);
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        btlso::SocketHandle::Handle fd = testPairs[i].observedFd();
        const btlso::TimerEventManager::Callback callback(
                       bsl::allocator_arg_t(),
                       bsl::allocator<btlso::TimerEventManager::Callback>(&ta),
                       &dummyFunction);
        mX->registerSocketEvent(fd, btlso::EventType::e_READ, callback);
        LOOP_ASSERT(i, mX->isRegistered(fd, btlso::EventType::e_READ));
        mX->deregisterSocketEvent(fd, btlso::EventType::e_READ);
        LOOP_ASSERT(i, 0 == mX->isRegistered(fd, btlso::EventType::e_READ));
    }
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        btlso::SocketHandle::Handle fd = testPairs[i].controlFd();
        bsl::function<void()> callback(&dummyFunction);
        mX->registerSocketEvent(fd, btlso::EventType::e_READ, callback);
        LOOP_ASSERT(i, mX->isRegistered(fd, btlso::EventType::e_READ));

        mX->deregisterSocketEvent(fd, btlso::EventType::e_READ);
        LOOP_ASSERT(i, 0 == mX->isRegistered(fd, btlso::EventType::e_READ));
    }

    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        btlso::SocketHandle::Handle fd = testPairs[i].controlFd();
        const btlso::TimerEventManager::Callback callback(
                       bsl::allocator_arg_t(),
                       bsl::allocator<btlso::TimerEventManager::Callback>(&ta),
                       &dummyFunction);
    }

    // 'testPairs' cannot be destroyed until ALL threads are done.
    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());

    return arg;
}

// ----------------------------------------------------------------------------
static
void timerCallback(int                *isInvokedFlag,
                   bsls::TimeInterval *registrationTime,
                   bsls::TimeInterval  expDelta,
                   int                 sequenceNumber,
                   bool                checkDeltaFlag)
{
    ASSERT(isInvokedFlag); ASSERT(registrationTime);
    ASSERT(0 == *isInvokedFlag);
    *isInvokedFlag = 1;
    bsls::TimeInterval now = bdlt::CurrentTime::now();
    ASSERT(now >= *registrationTime);
    if (checkDeltaFlag) {
        bsls::TimeInterval delta = now - *registrationTime;
        const double TOLERANCE = 0.15;  // allow 15% inaccuracy
        bsls::TimeInterval deltaLowerBound =
           bsls::TimeInterval(delta.totalSecondsAsDouble() * (1. - TOLERANCE));
        ASSERT(delta >= deltaLowerBound);
        if (veryVerbose) {
            Q("timerCallback");
            P_(sequenceNumber); P_(now); P(*registrationTime);
            P_(deltaLowerBound); P_(delta); P(expDelta);
        }
    }
}

extern "C" void *testTimersThread(void *arg) {
    Obj *mX = (Obj *)arg;
    ASSERT(mX);

    const Obj& X = *mX;
    enum { NUM_TIMERS = 10 };
    int flags[NUM_TIMERS];
    bsls::TimeInterval timeValues[NUM_TIMERS];
    bsls::TimeInterval now = bdlt::CurrentTime::now();
    void *timerIds[NUM_TIMERS];

    ASSERT(0 == defaultAllocator.numBytesInUse());

    bslma::TestAllocator scratchAllocator("scratch", veryVeryVeryVerbose);
    for (int i = 0; i < NUM_TIMERS; ++i) {
        int offset = i % 2 ? i : -i;
        offset *= 10 * 1000 * 1000;  // make it milliseconds, at most +/- 100ms
        bsls::TimeInterval delta = bsls::TimeInterval(0, offset);
        timeValues[i] = now + delta;
        flags[i] = 0;
        bsls::TimeInterval expDelta = i % 2 ? delta : bsls::TimeInterval(0);
        expDelta += bsls::TimeInterval(0.25);
        bsl::function<void()> functor(
                     bsl::allocator_arg_t(),
                     bsl::allocator<bsl::function<void()> >(&scratchAllocator),
                     bdlf::BindUtil::bind(&timerCallback,
                                          &flags[i],
                                          &timeValues[i],
                                           expDelta,
                                           -i,
                                           true));

        timerIds[i] = mX->registerTimer(timeValues[i], functor);
    }

    globalBarrier->wait();
    ASSERT(0 == defaultAllocator.numBytesInUse());

    if (X.isEnabled()) {
        bsls::TimeInterval delta(0.5);  // 500ms
        bslmt::ThreadUtil::sleep(delta);
        mX->disable();
        int numNotInvoked = 0;
        for (int i = 0; i < NUM_TIMERS; ++i) {
            if (veryVerbose) {
                P(i);
                P(timeValues[i]);
                P(flags[i]);
                P(delta);
            }
            if (i % 2 == 0) {
                LOOP_ASSERT(i, flags[i]);
                continue;
            }
            if (1 == flags[i]) {
                LOOP_ASSERT(i, timeValues[i] <= now + delta);
            }
            else {
                ++numNotInvoked;
            }
        }
        if (veryVerbose) {
            P(numNotInvoked);
        }
    }
    else {
        for (int i = 0; i < NUM_TIMERS; ++i) {
            LOOP_ASSERT(i, 0 == flags[i]);
        }
    }
    return arg;
}

static void executeInParallel(bslmt_ThreadFunction  func,
                              void                 *arg,
                              int                   numThreads)
    // Execute the specified 'func' with the specified 'arg' in 'numThreads'
    // threads and wait until all threads complete.  The concurrency level
    // adjusted automatically.
{
    vector<bslmt::ThreadUtil::Handle> threads(numThreads,
                                             bslmt::ThreadUtil::Handle());

    for (int i = 0; i < numThreads; ++i) {
        ASSERT(0 == bslmt::ThreadUtil::create(&threads[i],
                                              bslmt::ThreadAttributes(),
                                              func,
                                              arg));
    }
    for (int i = 0; i < numThreads; ++i) {
        ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
    }

}

struct my_Event {
    int         d_thread;    // calling thread
    bdlt::Time  d_timestamp; // invocation time
    void       *d_context;   // user data
};

static void recordCb(void *context,
                     bsl::vector<my_Event> *result) {
    my_Event event;
    event.d_thread = bslmt::ThreadUtil::selfIdAsInt();
    event.d_timestamp = bdlt::CurrentTime::utc().time();
    event.d_context = context;
    result->push_back(event);
}

void *executeCPPTest(void *arg) {
    enum { NUM_EXECUTES = 1000 };
    Obj *mX = (Obj*)arg;
    ASSERT(mX);

    vector<my_Event> results;
    for (int i = 0; i < NUM_EXECUTES; ++i) {
        bsl::function<void()> functor(
                bdlf::BindUtil::bind(&recordCb, (void*)i, &results));
        mX->execute(functor);
    }

    while(results.size() != NUM_EXECUTES) {
        bslmt::ThreadUtil::microSleep(1000, 0);
    }

    int objHandle = results[0].d_thread;
    for (int i = 1; i < NUM_EXECUTES; ++i) {
        LOOP_ASSERT(i, results[i].d_thread == objHandle);
        LOOP_ASSERT(i, results[i - 1].d_context < results[i].d_context);
    }

    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> lock(&coutMutex);
        P_(bslmt::ThreadUtil::selfIdAsInt());
        P_(results[0].d_timestamp);
        P(results[NUM_EXECUTES - 1].d_timestamp);
    }
    return arg;
}

extern "C" void *executeTest(void *arg) {
    return executeCPPTest(arg);
}

namespace CASE_TEST_FORCED_SOCKET_CLOSURES {

struct ReadDataType {
    btlso::SocketHandle::Handle  d_handle;
    int                         d_totalBytesToRead;
    int                         d_numBytesRead;
    bsls::AtomicInt            *d_numConnsDone_p;
};

struct WriteDataType {
    btlso::SocketHandle::Handle d_handle;
    int                        d_totalBytesToWrite;
    int                        d_numBytesWritten;
};

void readData(ReadDataType *readDataArgs)
{
    const int SIZE = 1024;
    char readBuffer[SIZE]  = { 'x' };
    char expBuffer[SIZE]   = { 'z' };
    int errorCode = 0;
    int rc = btlso::SocketImpUtil::read(readBuffer,
                                        readDataArgs->d_handle,
                                        SIZE,
                                        &errorCode);
    if (rc <= 0) {
        cout << "ReadData rc: " << rc << " errorCode: " << errorCode << endl;
        return;                                                       // RETURN
    }

    ASSERT(rc > 0);
    readDataArgs->d_numBytesRead += rc;
//    ASSERT(0 == bsl::memcmp(readBuffer, expBuffer, rc));

    if (readDataArgs->d_numBytesRead >= readDataArgs->d_totalBytesToRead) {
        ++(*readDataArgs->d_numConnsDone_p);
    }
    bslmt::ThreadUtil::microSleep(800, 0);
}

void writeData(WriteDataType *writeDataArgs)
{
    if (writeDataArgs->d_numBytesWritten >=
                                          writeDataArgs->d_totalBytesToWrite) {
        return;                                                       // RETURN
    }

    const int SIZE = 1024;
    char writeBuffer[SIZE] = { 'z' };

    int rc = btlso::SocketImpUtil::write(writeDataArgs->d_handle,
                                         writeBuffer,
                                         SIZE);

    if (rc <= 0) {
        cout << "WriteData rc: " << rc << endl;
        return;                                                       // RETURN
    }

    ASSERT(rc > 0);
    writeDataArgs->d_numBytesWritten += rc;
//    if (0 == (writeDataArgs->d_numBytesWritten % (1024 * 1024)))
//        cout << "Written Data: " << writeDataArgs->d_numBytesWritten << endl;
    bslmt::ThreadUtil::microSleep(800, 0);
}

}  // close namespace CASE_TEST_FORCED_SOCKET_CLOSURES

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test
         << " STARTED " << bdlt::CurrentTime::utc() << endl;;

    bslma::TestAllocator testAllocator("testAllocator", veryVeryVeryVerbose);

    testAllocator_p = &testAllocator;
    int rc = btlso::SocketImpUtil::startup();
    ASSERT(0 == rc);

    enum { MIN_REQUIRED_OPEN_FILES = 200 };
    int maxNumOpenFiles = maxOpenFiles();
    ASSERT(MIN_REQUIRED_OPEN_FILES <= maxNumOpenFiles);

    if (MIN_REQUIRED_OPEN_FILES > maxNumOpenFiles) {
        LOOP2_ASSERT(maxNumOpenFiles, MIN_REQUIRED_OPEN_FILES,
                     "Not enough system resources.");
        return testStatus;                                            // RETURN
    }
    if (veryVerbose) {
        P_(maxNumOpenFiles);
        P(MIN_REQUIRED_OPEN_FILES);
    }

    switch (test) { case 0:
      case 16: {
        // ----------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove
        //   leading comment characters, and replace 'assert' with
        //   'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // ----------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        enum {
            TIME_OFFSET         = 50, // milliseconds
            INITIAL_TIME_OFFSET = 10  // milliseconds
        };

        bdlcc::Queue<int> workQueue(&testAllocator);
        btlmt::TcpTimerEventManager manager(&testAllocator);

        bsls::TimeInterval now = bdlt::CurrentTime::now();
        now.addMilliseconds(INITIAL_TIME_OFFSET);
        bsls::TimeInterval nextTime(now);
        nextTime.addMilliseconds(TIME_OFFSET);

        bsl::function<void()> callback(bdlf::BindUtil::bind(&producer,
                                                                 &workQueue,
                                                                 &manager,
                                                                 nextTime));

        void *timerId = manager.registerTimer(now, callback);

        ASSERT(timerId);
        int rc = manager.enable();
        ASSERT(0 == rc);

        for (int i = 0; i < 10; ++i) {
            int item = workQueue.popFront();
            if (verbose) {
                bsl::cout << bdlt::CurrentTime::now()
                          << ": Consuming " << item << bsl::endl;
            }
        }
      } break;

      case 15: {
        // -----------------------------------------------------------------
        // TEST closure of control channel sockets
        //
        // Concern:
        //: 1 The closure of the control channel sockets does not cause an
        //:   object to spin.
        //:
        //: 2 The closure of both the client and the server socket is handled
        //:   gracefully.
        //:
        //: 3 Upto three socket closures should result in an attempt to
        //:   recreate the control channel.
        //:
        //: 4 Any read or write operation that is underway should be
        //:   unaffected by the socket closures and should complete
        //:   successfully.
        //
        // Plan:
        //: 1 Create a 'btlmt::TcpTimerEventManager' object, mX, and create the
        //:   control channel by calling 'enable'.
        //:
        //: 2 Create a large number of socket pairs.  For each socket pair,
        //:   register their server socket for a read event with mX and the
        //:   client socket for a write event.  This will result in the
        //:   initiation of data transfer between the connections.
        //:
        //: 3 Close the server socket of the control channel three times and
        //:   confirm that the control channel is recreated each time and that
        //:   all of the outstanding data exchanges between the created
        //:   connections completes succeessfully.
        //:
        //: 4 Repeat the same steps by closing the client socket.
        //
        // Testing:
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST closure of control channel sockets" << endl
                          << "=======================================" << endl;

#ifdef BTLSO_PLATFORM_WIN_SOCKETS
        using namespace CASE_TEST_FORCED_SOCKET_CLOSURES;

        for (int i = 0; i < 2; ++i) {
            if (verbose) {
                if (1 == i) {
                    cout << "Killing the client handle" << endl;
                }
                else {
                    cout << "Killing the server handle" << endl;
                }
            }

            Obj mX;  const Obj& X = mX;
            mX.enable();

            const int NUM_CONNS = 4;
            bsls::AtomicInt numConnsDone(0);

            ReadDataType  readDataArgs[NUM_CONNS];
            WriteDataType writeDataArgs[NUM_CONNS];

            const int NUM_BYTES = 1024 * 1024 * 20;

            for (int j = 0; j < NUM_CONNS/2; ++j) {
                btlso::SocketHandle::Handle fds[2];
                rc = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     fds,
                                     btlso::SocketImpUtil::k_SOCKET_STREAM);

                ASSERT(0 == rc);

                readDataArgs[j].d_handle           = fds[0];
                readDataArgs[j].d_totalBytesToRead = NUM_BYTES;
                readDataArgs[j].d_numBytesRead     = 0;
                readDataArgs[j].d_numConnsDone_p   = &numConnsDone;

                writeDataArgs[j].d_handle            = fds[1];
                writeDataArgs[j].d_totalBytesToWrite = NUM_BYTES;
                writeDataArgs[j].d_numBytesWritten   = 0;

                bsl::function<void()> readFunctor, writeFunctor;
                readFunctor = bdlf::BindUtil::bind(readData,
                                                  &readDataArgs[j]);
                do {
                    rc = mX.registerSocketEvent(fds[0],
                                                btlso::EventType::e_READ,
                                                readFunctor);
                } while (0 != rc);

                ASSERT(0 == rc);

                writeFunctor = bdlf::BindUtil::bind(writeData,
                                                   &writeDataArgs[j]);
                do {
                    rc = mX.registerSocketEvent(fds[1],
                                                btlso::EventType::e_WRITE,
                                                writeFunctor);
                } while (0 != rc);

                ASSERT(0 == rc);

                bslmt::ThreadUtil::microSleep(1000, 0);
            }

            if (veryVerbose) {
                cout << "Created first set of connections, numConnections: "
                     << NUM_CONNS/2 << endl;
            }

            btlmt::TcpTimerEventManager_ControlChannel *controlChannel =
                const_cast<btlmt::TcpTimerEventManager_ControlChannel *>(
                   btlmt::TcpTimerEventManager_TestUtil::getControlChannel(X));

            int numKills = 0;
            while (numKills < 1) {
                int errorCode = 0;
                btlso::SocketHandle::Handle handle;
                if (1 == i) {
                    handle = controlChannel->clientFd();
                }
                else {
                    handle = controlChannel->serverFd();
                }

                rc = btlso::SocketImpUtil::close(handle, &errorCode);
                if (rc < 0) {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    continue;
                }
                LOOP2_ASSERT(rc, errorCode, 0 == rc);

                ++numKills;
            }

            if (veryVerbose) {
                cout << "Completed first set of kills, numKills: "
                     << numKills << endl;
            }
            //bslmt::ThreadUtil::microSleep(0, 3);

            for (int j = NUM_CONNS/2; j < NUM_CONNS; ++j) {
                btlso::SocketHandle::Handle fds[2];
                do {
                    rc = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     fds,
                                     btlso::SocketImpUtil::k_SOCKET_STREAM);
                } while (0 != rc);

                LOOP2_ASSERT(j, rc, 0 == rc);

                readDataArgs[j].d_handle           = fds[0];
                readDataArgs[j].d_totalBytesToRead = NUM_BYTES;
                readDataArgs[j].d_numBytesRead     = 0;
                readDataArgs[j].d_numConnsDone_p   = &numConnsDone;

                writeDataArgs[j].d_handle            = fds[1];
                writeDataArgs[j].d_totalBytesToWrite = NUM_BYTES;
                writeDataArgs[j].d_numBytesWritten   = 0;

                bsl::function<void()> readFunctor, writeFunctor;
                readFunctor = bdlf::BindUtil::bind(readData,
                                                  &readDataArgs[j]);
                do {
                    rc = mX.registerSocketEvent(fds[0],
                                                btlso::EventType::e_READ,
                                                readFunctor);
                } while (0 != rc);

                ASSERT(0 == rc);

                writeFunctor = bdlf::BindUtil::bind(writeData,
                                                   &writeDataArgs[j]);

                do {
                    rc = mX.registerSocketEvent(fds[1],
                                                btlso::EventType::e_WRITE,
                                                writeFunctor);
                } while (0 != rc);

                ASSERT(0 == rc);

                bslmt::ThreadUtil::microSleep(1000, 0);
            }

            if (veryVerbose) {
                cout << "Created second set of connections, numConnections: "
                     << NUM_CONNS << endl;
            }

            while (numKills < 3) {
                int errorCode = 0;
                btlso::SocketHandle::Handle handle;
                if (1 == i) {
                    handle = controlChannel->clientFd();
                }
                else {
                    handle = controlChannel->serverFd();
                }

                rc = btlso::SocketImpUtil::close(handle, &errorCode);
                if (rc < 0) {
                    bslmt::ThreadUtil::microSleep(0, 1);
                    continue;
                }
                LOOP2_ASSERT(rc, errorCode, 0 == rc);

                bslmt::ThreadUtil::microSleep(0, 1);
                ++numKills;
            }

            if (veryVerbose) {
                cout << "Completed second set of kills, numKills: "
                     << numKills << endl;
            }

            while (numConnsDone < NUM_CONNS) {
                if (veryVerbose) {
                    cout << "Waiting for conns to be done: "
                         << numConnsDone << endl;
                    bslmt::ThreadUtil::microSleep(0, 1);
                }
            }
            ASSERT(0 == mX.disable());
            //mX.deregisterAll();
            //
            //for (int j = 0; j < NUM_CONNS; ++j) {
            // btlso::SocketHandle::Handle serverFd = readDataArgs[j].d_handle;
            // btlso::SocketHandle::Handle clientFd =
            //                                       writeDataArgs[j].d_handle;

            //    ASSERT(0 == btlso::SocketImpUtil::close(serverFd));
            //    ASSERT(0 == btlso::SocketImpUtil::close(clientFd));
            //}

        }
#endif
      } break;

      case 14: {
        // -----------------------------------------------------------------
        // TESTING 'hasLimitedSocketCapacity'
        //
        // Concern:
        //: 1 'hasLimitiedSocketCapacity' returns 'true' if the underlying
        //:   event manager returns 'true' and 'false' otherwise.
        //
        // Plan:
        //: 1 Assert that 'hasLimitiedSocketCapacity' returns 'true' if the
        //:   underlying event manager returns 'true' and 'false' otherwise.
        //
        // Testing:
        //   bool hasLimitedSocketCapacity() const;
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'hasLimitedSocketCapacity" << endl
                          << "=================================" << endl;

#ifdef BSLS_PLATFORM_OS_WINDOWS
        const bool HLSC = true;
#else
        const bool HLSC = false;
#endif

        if (verbose) cout << "Testing 'hasLimitedSocketCapacity'" << endl;
        {
            Obj mX;  const Obj& X = mX;
            bool hlsc = X.hasLimitedSocketCapacity();
            LOOP2_ASSERT(HLSC, hlsc, HLSC == hlsc);
        }
      } break;

      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'rescheduleTimer' METHODS
        // Concerns:
        //   o rescheduling works as expected
        // Plan:
        //   Register a large number of timers to be invoked at the time of the
        //   registration and verify that each is invoked within a hard limit.
        // Testing:
        //  int rescheduleTimer(timerId, expiryTime);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESING 'rescheduleTimer'" << endl
                          << "=======================" << endl;
        {
            Obj mX(&testAllocator);   const Obj& X = mX;
            ASSERT(0 == mX.enable()); ASSERT(mX.isEnabled());

            enum { NUM_TIMERS  = 10000 };
            bsls::TimeInterval  timeValues[NUM_TIMERS];

            // DELTA had to be increased from 0.5 for when built in safe mode
            // on Solaris

            const double       DELTA = 1.25;
            bsls::TimeInterval  delta(DELTA);
            int                flags[NUM_TIMERS];
            void              *ids[NUM_TIMERS];

            bsls::TimeInterval  start = bdlt::CurrentTime::now();

            for (int i = 0; i < NUM_TIMERS; ++i) {
                flags[i] = 0;
                timeValues[i] = bdlt::CurrentTime::now() + delta;
                bsl::function<void()> functor(
                        bdlf::BindUtil::bind(&timerCallback,
                                            &flags[i],
                                            &timeValues[i],
                                            delta,
                                            i,
                                            false));
                ids[i] = mX.registerTimer(timeValues[i],
                                          functor);
                LOOP_ASSERT(i, ids[i]);
            }

            if (veryVerbose) {
                cout << "\t\tRegistered " << NUM_TIMERS << " timers." << endl;
            }

            double soFar =
                     (bdlt::CurrentTime::now() - start).totalSecondsAsDouble();
            LOOP_ASSERT(soFar, soFar < DELTA);
            if (verbose) { P_(DELTA); P(soFar); }

            for (int i = 0; i < NUM_TIMERS; ++i) {
                LOOP_ASSERT(i, 0 == flags[i]);
            }

            soFar = (bdlt::CurrentTime::now() - start).totalSecondsAsDouble();
            LOOP_ASSERT(soFar, soFar < DELTA);

            for (int i = 0; i < NUM_TIMERS; ++i) {
                bslma::TestAllocator da("Default for case 13",
                                        veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                const int rc = mX.rescheduleTimer(
                                             ids[i],
                                             bdlt::CurrentTime::now() + delta);

                LOOP_ASSERT(i, !rc);
                LOOP_ASSERT(i, 0 == da.numBytesInUse());
            }

            if (veryVerbose) {
                cout << "\t\tRescheduled " << NUM_TIMERS << " timers." << endl;
            }

            bslmt::ThreadUtil::sleep(delta);

            ASSERT(0 == mX.disable());
            for (int i = 0; i < NUM_TIMERS; ++i) {
                LOOP_ASSERT(i, 1 == flags[i]);

            }
            ASSERT(0 == X.numTimers());
            ASSERT(0 == X.numEvents());
        }
      } break;
      case 12: {
        // ----------------------------------------------------------------
        // TEST: 'collectTimeMetrics' configuration flag
        //
        // Concerns:
        //   o The optional 'collectTimeMetrics' construction parameter is
        //   correctly applied.
        //
        // Plan:
        //   (Block Box) 1) Use all variants of the constructor to create a
        //          tcp timer event manager, and verify the status of the
        //         'hasTimeMetrics' flag.
        //
        //   (Black Box) 2) Configure a tcp timer event manager to collect
        //           metrics, and provide a callback that blocks for a
        //           long period.  Verify that 'timeMetrics' reflects a
        //           CPU bound operation.
        //
        //   (White Box) 3) Configure a tcp timer event manager to not
        //           collect metrics.  Set 'timeMetrics' to IO_BOUND.
        //           Supply a callback that blocks for a long period.
        //           Verify 'timeMetrics' reflects a I/O bound (the default
        //           state).
        //
        // Testing:
        //  TcpTimerEventManager(bslma::Allocator *basicAllocator = 0);
        //  TcpTimerEventManager(collectTimeMetrics, *basicAllocator = 0);
        //  TcpTimerEventManager(collectTimeMetrics, poolTimer, *ba = 0);
        //  ~TcpTimerEventManager();
        //  bool hasTimeMetrics() const;
        // ----------------------------------------------------------------

        if (verbose)
              cout << "TESTING: 'collectTimeMetrics' and 'hasTimeMetrics'\n"
                   << "====================================================\n";

        using namespace TEST_CASE_COLLECT_TIME_METRICS;

        {
            if (veryVerbose) {
                cout << "\tVerify the configuration option is set correctly'"
                     << endl;
            }

            btlso::DefaultEventManager<btlso::Platform::SELECT>
                                        dummyEventManager(0, &testAllocator);
            Obj mA(&testAllocator);
            Obj mG(&dummyEventManager, &testAllocator);
            Obj mH(true, &testAllocator);
            Obj mI(false, &testAllocator);
            Obj mJ(false, true, &testAllocator);
            Obj mK(false, false, &testAllocator);

            const Obj& A = mA;
            const Obj& G = mG;
            const Obj& H = mH;
            const Obj& I = mI;
            const Obj& J = mJ;
            const Obj& K = mK;
            ASSERT(true  == A.hasTimeMetrics());
            ASSERT(false == G.hasTimeMetrics());
            ASSERT(true  == H.hasTimeMetrics());
            ASSERT(false == I.hasTimeMetrics());
            ASSERT(false == J.hasTimeMetrics());
            ASSERT(false == K.hasTimeMetrics());
        }
        {
            if (veryVerbose) {
                cout << "\tConfigure metrics to be collected" << endl;
            }

            enum { BUFFER_SIZE = 50 };
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            btlso::SocketHandle::Handle handles[2];
            ASSERT(0 == btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                         handles, btlso::SocketImpUtil::k_SOCKET_STREAM));
            ASSERT(0 !=
                btlso::SocketImpUtil::write(handles[0], buffer, BUFFER_SIZE));

            Obj mX(&testAllocator);
            bsl::function<void()> callback(&waitForSomeTime);
            ASSERT(0 == mX.registerSocketEvent(handles[1],
                                               btlso::EventType::e_READ,
                                               callback));
            ASSERT(0 == mX.enable());

            bslmt::ThreadUtil::microSleep(10000); // 10 ms
            ASSERT(0 == mX.disable());
            int percent = mX.timeMetrics()->percentage(
                                         btlso::TimeMetrics::e_CPU_BOUND);
            LOOP_ASSERT(percent, percent > 80);

            if (veryVerbose) {
                P(percent);
            }
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);

        }
        {
            if (veryVerbose) {
                cout << "\tConfigure metrics to not be collected" << endl;
            }

            enum { BUFFER_SIZE = 50 };
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            btlso::SocketHandle::Handle handles[2];
            ASSERT(0 == btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                         handles, btlso::SocketImpUtil::k_SOCKET_STREAM));
            ASSERT(0 !=
                btlso::SocketImpUtil::write(handles[0], buffer, BUFFER_SIZE));

            Obj mX(false, &testAllocator);
            const Obj& X = mX;
            bsl::function<void()> callback(&waitForSomeTime);
            ASSERT(0 == mX.registerSocketEvent(handles[1],
                                               btlso::EventType::e_READ,
                                               callback));
            ASSERT(0 == mX.enable());

            bslmt::ThreadUtil::microSleep(10000); // 10 ms
            ASSERT(0 == mX.disable());
            int percent = mX.timeMetrics()->percentage(
                                         btlso::TimeMetrics::e_CPU_BOUND);
            LOOP_ASSERT(percent, percent <= 1);

            if (veryVerbose) {
                P(percent);
            }

            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);

        }
      } break;
      case 11: {
        // ----------------------------------------------------------------
        // ADDITIONAL 'enable' and 'isEnabled' TEST
        //
        // Concerns:
        //   o that callbacks dispatched by events registed prior to invoking
        //   'enable', find 'isEnabled' to be 'true'.
        //
        // Plan:
        //   Create a socket pair and write data to both ends of the open
        //   connection, such that a 'btlso::EventManager' created for these
        //   sockets would dispatch a 'READ' event on each socket.  Register
        //   these socket handles with the tcp event manager under dispatch,
        //   such that they will dispatch 'READ' events to the test callback
        //   'testIsEnabled'.  Then call 'enable'.
        //
        // Testing:
        //  int enable();
        //  int isEnabled() const;
        //  int enable(const bslmt::ThreadAttributes& attribute);
        // ----------------------------------------------------------------

        if (verbose)
              cout << "TESTING enable() and isEnabled()" << endl
                   << "================================" << endl;

        using namespace TEST_CASE_ENABLE_TEST;

        enum { BUFFER_SIZE = 50 };
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        btlso::SocketHandle::Handle handles[2];
        ASSERT(0 == btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                           handles, btlso::SocketImpUtil::k_SOCKET_STREAM));

        ASSERT(0 != btlso::SocketImpUtil::write(handles[0],
                                                buffer,
                                                BUFFER_SIZE));
        ASSERT(0 != btlso::SocketImpUtil::write(handles[1],
                                                buffer,
                                                BUFFER_SIZE));

        bsls::AtomicInt complete(0);
        btlmt::TcpTimerEventManager manager(&testAllocator);
        bsl::function<void()> callback(
                bdlf::BindUtil::bind(&testIsEnabled, &manager, &complete));

        ASSERT(0 == manager.registerSocketEvent(handles[0],
                                                btlso::EventType::e_READ,
                                                callback));
        ASSERT(0 == manager.registerSocketEvent(handles[1],
                                                btlso::EventType::e_READ,
                                                callback));
        manager.enable();
        manager.disable();
        ASSERT(0 != complete);

        btlso::SocketImpUtil::close(handles[0]);
        btlso::SocketImpUtil::close(handles[1]);
      } break;
      case 10: {
        // ----------------------------------------------------------------
        // TESTING 'execute' METHOD
        // Concerns:
        //   o execute correctly invokes the installed functor
        //
        // Plan:
        //   Execute a recording functor from multiple threads, both when
        //   channel pool is running and when it is not.
        //
        // Testing:
        //   void execute(const bsl::function<void()>& functor);
        // ----------------------------------------------------------------

        if (verbose) cout << "TESTING execute METHOD." << endl
                          << "=======================" << endl;

        enum { NUM_THREADS = 16 };

        if (verbose) cout << "\tOn Enabled object" << endl;
        {
            Obj mX(&testAllocator);
            mX.enable();
            ASSERT(1 == mX.isEnabled());
            executeInParallel(executeTest, (void*)&mX, NUM_THREADS);
            mX.disable();
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'enable' and 'disable' methods in a callback
        // Concern:
        //   Verify that 'enable', 'disable' and 'isEnabled' work as
        //   advertised when invoked in a callback.
        //
        // Testing:
        //  int enable();
        //  int disable();
        //  int isEnabled() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'enable' AND 'disable' METHODS" << endl
                 << "======================================" << endl;
        {
            enum { NUM_TESTS = 10 };

            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj mX(&testAllocator);
                mX.enable();
                LOOP_ASSERT(i, mX.isEnabled());

                btlso::EventManagerTestPair testPair;
                bsl::function<void()> callback(
                       bdlf::BindUtil::bind(&disableCb, &mX));

                mX.registerSocketEvent(testPair.observedFd(),
                                       btlso::EventType::e_WRITE,
                                       callback);

                bslmt::ThreadUtil::microSleep(10000); // 10 ms
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'deregisterSocketEvent' METHOD
        // Concerns:
        //   o deregistration from different threads on disabled object works
        //     correctly
        //   o deregistration from different threads on enabled object works
        //     correctly
        //   o deregistration from an invoked callback works correctly
        //
        // Testing:
        //  void deregisterSocketEvent(handle, event);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'deregisterSocketEvent' METHOD" << endl
                 << "======================================" << endl;

        if (verbose)
            cout << "\tConcern #1: Deregistration on disabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(&testAllocator);
            ASSERT(0 == mX.isEnabled());
            bslmt::ThreadUtil::Handle workers[NUM_THREADS];
            bslmt::Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma::DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                   bslmt::ThreadAttributes(),
                                                   &deregisterThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            int numTotalSocketEvents = mX.numTotalSocketEvents();
            if (veryVerbose) {
                P(numTotalSocketEvents);
            }
            LOOP_ASSERT(numTotalSocketEvents, 0 == numTotalSocketEvents);
        }

        if (verbose)
            cout << "\tConcern #2: Deregistration on enabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(&testAllocator);
            mX.enable();
            ASSERT(mX.isEnabled());
            bslmt::ThreadUtil::Handle workers[NUM_THREADS];
            bslmt::Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma::DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                   bslmt::ThreadAttributes(),
                                                   &deregisterThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            int numTotalSocketEvents = mX.numTotalSocketEvents();
            if (veryVerbose) {
                P(numTotalSocketEvents);
            }
            LOOP_ASSERT(numTotalSocketEvents, 0 == numTotalSocketEvents);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'deregisterAllSocketEvents' METHOD
        // Concerns:
        //   o deregistration from different threads on disabled object works
        //     correctly
        //   o deregistration from different threads on enabled object works
        //     correctly
        //   o deregistration from an invoked callback works correctly
        //
        // Testing:
        //  void deregisterAllSocketEvents();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'deregisterSocketEvent' METHOD" << endl
                 << "======================================" << endl;

        if (verbose)
            cout << "\tConcern #1: Deregistration on disabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(&testAllocator);
            ASSERT(0 == mX.isEnabled());
            bslmt::ThreadUtil::Handle workers[NUM_THREADS];
            bslmt::Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;
            if (veryVerbose) {
                Q(before deregisterThread);
                P(mX.numTotalSocketEvents());
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma::DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                   bslmt::ThreadAttributes(),
                                                   &deregisterThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            int numTotalSocketEvents = mX.numTotalSocketEvents();
            if (veryVerbose) {
                Q(after deregisterThread);
                P(mX.numTotalSocketEvents());
            }
            LOOP_ASSERT(numTotalSocketEvents, 0 == numTotalSocketEvents);
        }

        if (verbose)
            cout << "\tConcern #2: Deregistration on enabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(&testAllocator);
            mX.enable();
            ASSERT(mX.isEnabled());
            bslmt::ThreadUtil::Handle workers[NUM_THREADS];
            bslmt::Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            if (veryVerbose) {
                Q(before deregisterThread);
                P(mX.numTotalSocketEvents());
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma::DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                   bslmt::ThreadAttributes(),
                                                   &deregisterThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            int numTotalSocketEvents = mX.numTotalSocketEvents();
            if (veryVerbose) {
                Q(after deregisterThread);
                P(numTotalSocketEvents);
            }
            LOOP_ASSERT(numTotalSocketEvents, 0 == numTotalSocketEvents);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'registerSocketEvent' METHOD
        // Concerns:
        //   o registration from different threads on disabled object works
        //     correctly
        //   o registration from different threads on enabled object works
        //     correctly
        //   o registration from an invoked callback works correctly
        //
        // Testing:
        //  int registerSocketEvent(handle, event, callback);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'registerSocketEvent' METHOD" << endl
                 << "====================================" << endl;

        if (verbose)
            cout << "\tConcern #1: Registration on disabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(&testAllocator);
            ASSERT(0 == mX.isEnabled());
            bslmt::ThreadUtil::Handle workers[NUM_THREADS];

            bslmt::Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma::DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                   bslmt::ThreadAttributes(),
                                                   &registerThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            int numTotalSocketEvents = mX.numTotalSocketEvents();
            if (verbose) {
                P(numTotalSocketEvents);
            }
            ASSERT(0 == numTotalSocketEvents);
        }

        if (verbose)
            cout << "\tConcern #2: Registration on enabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(&testAllocator);
            mX.enable();
            ASSERT(mX.isEnabled());
            bslmt::ThreadUtil::Handle workers[NUM_THREADS];

            bslmt::Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma::DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                   bslmt::ThreadAttributes(),
                                                   &registerThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            // Call isRegistered() because it will do a waitForResult() which
            // will allow all pending deregistration events to complete, which
            // will give us the right result from numTotalSocketEvents()

            btlso::SocketHandle::Handle nilHandle = 0;
            mX.isRegistered(nilHandle, btlso::EventType::e_READ);
            int numTotalSocketEvents = mX.numTotalSocketEvents();
            LOOP_ASSERT(numTotalSocketEvents, 0 == numTotalSocketEvents);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'disable' METHOD
        // Concerns:
        //   o disable doesn't remove registered timers
        // Plan:
        //   Register a large number of timers to be invoked at some time in
        //   the future.  The disable and enable the event manager a few times.
        //   Then sleep until the future time + delta and verify that
        //   all callbacks are invoked.  Due to the disable/enable, we cannot
        //   guarantee that they are invoked within a hard limit; disable that
        //   check.
        // Testing:
        //  int disable();
        //  int enable();
        //  void *registerTimer(expiryTime, callback);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESING 'enable', 'disable' AND 'registerTimer'" << endl
                 << "==============================================" << endl;
        {
            for (int k = 0; k < 2; ++k) {
                const bool TIMER = static_cast<bool>(k);

                Obj mX(false, TIMER, &testAllocator);
                const Obj& X = mX;

                ASSERT(0 == mX.enable()); ASSERT(mX.isEnabled());

                enum { NUM_TIMERS  = 100, NUM_ATTEMPTS = 10 };
                bsl::vector<bsls::TimeInterval> timeValues(NUM_TIMERS);
                bsls::TimeInterval offset(0, 200000);
                bsls::TimeInterval delta(0, 100000);
                int flags[NUM_TIMERS];

                for (int i = 0; i < NUM_TIMERS; ++i) {
                    flags[i] = 0;
                    timeValues[i] = bdlt::CurrentTime::now();
                    timeValues[i] += offset;

                    bsl::function<void()> functor(
                        bdlf::BindUtil::bind(&timerCallback,
                                            &flags[i], &timeValues[i],
                                            delta, i, false));
                    mX.registerTimer(timeValues[i], functor);
                }
                if (veryVerbose) {
                    cout << "\t\tRegistered " << NUM_TIMERS
                         << " timers." << endl;
                }

                bslma::TestAllocator da("Default for case 5",
                                        veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                    LOOP_ASSERT(i, 0 == mX.disable());
                    LOOP_ASSERT(i, 0 == X.isEnabled());
                    LOOP_ASSERT(i, 0 == mX.enable());
                    LOOP_ASSERT(i, 1 == X.isEnabled());

                    LOOP_ASSERT(i, 0 == da.numBytesInUse());
                }
                bslmt::ThreadUtil::sleep(
                timeValues[NUM_TIMERS - 1] - bdlt::CurrentTime::now() + delta);

                ASSERT(0 == mX.disable());
                for (int i = 0; i < NUM_TIMERS; ++i) {
                    LOOP_ASSERT(i, 1 == flags[i]);
                }
                ASSERT(0 == X.numTimers());
                ASSERT(0 == X.numEvents());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'registerTimer' METHODS
        // Concerns:
        //   o delay for invocation is limited
        // Plan:
        //   Register a large number of timers to be invoked at the time of the
        //   registration and verify that each is invoked within a hard limit.
        // Testing:
        //  void *registerTimer(expiryTime, callback);
        // --------------------------------------------------------------------
        if (verbose)
            cout << "TESING 'registerTimer'" << endl
                 << "======================" << endl;
        {
            for (int k = 0; k < 2; ++k) {
                const bool TIMER = static_cast<bool>(k);

                Obj mX(false, TIMER, &testAllocator);
                const Obj& X = mX;

                ASSERT(0 == mX.enable()); ASSERT(mX.isEnabled());

                enum { NUM_TIMERS  = 100 };
                bsls::TimeInterval timeValues[NUM_TIMERS];
                bsls::TimeInterval delta(0, 100000); // 100 milliseconds
                int flags[NUM_TIMERS];

                for (int i = 0; i < NUM_TIMERS; ++i) {
                    flags[i] = 0;
                    timeValues[i] = bdlt::CurrentTime::now();
                    bsl::function<void()> functor(
                        bdlf::BindUtil::bind(&timerCallback,
                                            &flags[i], &timeValues[i],
                                            delta, i, true));

                    void *id = mX.registerTimer(timeValues[i],
                                                functor);
                    LOOP_ASSERT(i, id);
                }
                if (veryVerbose) {
                    cout << "\t\tRegistered " << NUM_TIMERS
                         << " timers." << endl;
                }
                bslmt::ThreadUtil::sleep(delta);

                ASSERT(0 == mX.disable());
                for (int i = 0; i < NUM_TIMERS; ++i) {
                    LOOP_ASSERT(i, 1 == flags[i]);
                }

                ASSERT(0 == X.numTimers());
                ASSERT(0 == X.numEvents());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'registerTimer' METHOD
        // Concerns:
        //   o registration from different threads on disabled object works
        //     correctly
        //   o registration from different threads on enabled object works
        //     correctly
        //   o delay for invocation is limited
        //
        // Testing:
        //  void *registerTimer(expiryTime, callback);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'registerTimer' METHOD" << endl
                 << "==============================" << endl;

        if (verbose)
            cout << "\tConcern #1: Registration on disabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(false, true, &testAllocator);
            const Obj& X = mX;

            ASSERT(0 == mX.isEnabled());
            bslmt::ThreadUtil::Handle workers[NUM_THREADS];

            bslmt::Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma::DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                  bslmt::ThreadAttributes(),
                                                  &testTimersThread,
                                                  &mX);
                LOOP_ASSERT(i, 0 == rc);
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            if (veryVerbose) {
                P(X.numTimers());
            }

            int numTimers = mX.numTimers();
            LOOP_ASSERT(numTimers, 100 == mX.numTimers());
            ASSERT(0 == mX.numTotalSocketEvents());
        }

        if (verbose)
            cout << "\tConcern #2: Registration on enabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(false, true, &testAllocator);
            const Obj& X = mX;
            ASSERT(0 == mX.enable()); ASSERT(X.isEnabled());
            bslmt::ThreadUtil::Handle workers[NUM_THREADS];

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslmt::Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            bslma::DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::create(&workers[i],
                                                  bslmt::ThreadAttributes(),
                                                  &testTimersThread,
                                                  &mX);
                LOOP_ASSERT(i, 0 == rc);
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bslmt::ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            if (veryVerbose) {
                P(X.numTimers());
            }
            ASSERT(0 == mX.numTimers());
            ASSERT(0 == mX.numTotalSocketEvents());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'enable' AND 'disable' METHODS
        //   Verify that 'enable' and 'disable' methods work as advertised.
        // Particularly verify that
        //   o an object can be enabled from any state (i.e., enabled or not)
        //   o an object can be disabled in any state (i.e., enabled or not)
        //   o an object can be destroyed, whether enabled or not
        //   Use 'isEnabled' to verify state.
        // Testing:
        //  int enable();
        //  int disable();
        //  int isEnabled() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'enable' AND 'disable' METHODS" << endl
                 << "======================================" << endl;

        if (verbose) cout << "\tBasic test for 'isEnabled'" << endl;
        {
            enum { NUM_TESTS = 10 };
            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj mX(&testAllocator);
                LOOP_ASSERT(i, 0 == mX.isEnabled());
            }
        }

        if (verbose) cout << "\tEnabling disabled object." << endl;
        {
            enum { NUM_TESTS = 10 };
            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj mX(&testAllocator);
                LOOP_ASSERT(i, 0 == mX.isEnabled());

                bslma::TestAllocator da("Test case 2, da: 1",
                                        veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                LOOP_ASSERT(i, 0 == mX.enable());

                LOOP2_ASSERT(i, da.numBytesInUse(), 0 == da.numBytesInUse());

                LOOP_ASSERT(i, mX.isEnabled());
            }
        }
        if (verbose) cout << "\tDisabling already disabled object." << endl;
        {
            enum { NUM_TESTS = 10 };
            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj mX(&testAllocator);
                LOOP_ASSERT(i, 0 == mX.isEnabled());

                bslma::TestAllocator da("Test case 2, da: 2",
                                        veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                LOOP_ASSERT(i, 0 == mX.disable());

                LOOP2_ASSERT(i, da.numBytesInUse(), 0 == da.numBytesInUse());

                LOOP_ASSERT(i, 0 == mX.isEnabled());
            }
        }
        if (verbose) cout << "\tDisabling enabled object." << endl;
        {
            enum { NUM_TESTS = 10 };
            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj mX(&testAllocator);
                LOOP_ASSERT(i, 0 ==  mX.isEnabled());

                bslma::TestAllocator da("Test case 2, da: 3",
                                        veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                LOOP_ASSERT(i, 0 == mX.enable());
                LOOP_ASSERT(i, 1 == mX.isEnabled());
                LOOP_ASSERT(i, 0 == mX.disable());

                LOOP2_ASSERT(i, da.numBytesInUse(), 0 == da.numBytesInUse());

                LOOP_ASSERT(i, 0 == mX.isEnabled());
            }
        }
        if (verbose) cout << "\tEnabling already enabled object." << endl;
        {
            enum { NUM_TESTS = 10 };
            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj mX(&testAllocator);
                LOOP_ASSERT(i, 0 == mX.isEnabled());

                bslma::TestAllocator da("Test case 2, da: 4",
                                        veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                LOOP_ASSERT(i, 0 == mX.enable());
                LOOP_ASSERT(i, 1 == mX.isEnabled());
                LOOP_ASSERT(i, 0 == mX.enable());

                LOOP2_ASSERT(i, da.numBytesInUse(), 0 == da.numBytesInUse());

                LOOP_ASSERT(i, 1 == mX.isEnabled());
            }
        }

        if (verbose) cout << "\tDestroying enabled object." << endl;
        {
            enum { NUM_TESTS = 10 };
            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj mX(&testAllocator);
                LOOP_ASSERT(i, 0 == mX.isEnabled());

                bslma::TestAllocator da("Test case 2, da: 5",
                                        veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                LOOP_ASSERT(i, 0 == mX.enable());

                LOOP2_ASSERT(i, da.numBytesInUse(), 0 == da.numBytesInUse());
            }
        }

        if (verbose) cout << "\tDestroying disabled object." << endl;
        {
            enum { NUM_TESTS = 10 };
            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj mX(&testAllocator);
                LOOP_ASSERT(i, 0 == mX.isEnabled());
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Ensure the basic liveness of an event manager object.
        //
        // Testing:
        //   Create an object of this event manager under test.  Perform
        //   some basic operations on it.
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX(&testAllocator); const Obj& X = mX;

        ASSERT(0 == mX.isEnabled());
        if (veryVerbose) {
            P(X.numTotalSocketEvents());
            P(X.numTimers());
            P(X.numEvents());
        }
#ifdef BSLS_PLATFORM_OS_UNIX
        ASSERT((bslmt::ThreadUtil::Handle) -1 == X.dispatcherThreadHandle());
#endif

        ASSERT(0 == X.numTotalSocketEvents());
        ASSERT(0 == X.numTimers());
        ASSERT(0 == X.numEvents());
        ASSERT(0 == mX.enable());
        ASSERT(1 == mX.isEnabled());
#ifdef BSLS_PLATFORM_OS_UNIX
        ASSERT((bslmt::ThreadUtil::Handle) -1 != X.dispatcherThreadHandle());
#endif
        ASSERT(0 == mX.disable());

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // HIGHLY-PARALLEL ALL-OPERATIONS TEST
        //
        // Plan:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "HIGHLY-PARRALLEL COMBINATION TEST" << endl
                          << "=================================" << endl;

        btlmt::TcpTimerEventManager em(&testAllocator);
        em.enable();
        pEventManager = &em;

        enum {
            MIN_THREADS = NUM_TASKS,
            MAX_THREADS = NUM_TASKS,
            MAX_IDLE_TIME = 100
        };

        bdlmt::ThreadPool threadPool(bslmt::ThreadAttributes(),
                                   MIN_THREADS,
                                   MAX_THREADS,
                                   MAX_IDLE_TIME);
        if (verbose)
            cout << "\tStarting thread pool." << endl;
        threadPool.start();

        if (verbose)
            cout << "\tEnqueueing " << NUM_TASKS << " jobs." << endl;

        for (int i = 0; i < NUM_TASKS; ++i) {
            LOOP_ASSERT(i, 0 == threadPool.enqueueJob(
                   (bdlmt::ThreadPoolJobFunc)caseStressTestEntryPoint,
                   (void*)i));
        }

        btlso::TimeMetrics *metrics = pEventManager->timeMetrics();
        ASSERT(metrics);
        while (threadPool.numActiveThreads()) {
         printf("=========================================================\n"
                "| Load = %d%%\n"
                "| numEvents = %d\n"
                "| numTotalSocketEvents = %d\n"
                "| numTimers = %d\n"
                "=========================================================\n",
                metrics->percentage(btlso::TimeMetrics::e_CPU_BOUND),
                pEventManager->numEvents(),
                pEventManager->numTotalSocketEvents(),
                pEventManager->numTimers()
               );

            metrics->resetAll();
            metrics->resetStartTimes();
            bslmt::ThreadUtil::microSleep(0, 5);  // 5 seconds
        }

        if (verbose)
            cout << "\tStopping thread pool." << endl;
        threadPool.stop();
        if (verbose)
            bsl::cout << "The test is complete." << bsl::endl;

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    rc = btlso::SocketImpUtil::cleanup();
    ASSERT(0 == rc);

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    cout << "TEST CASE " << test << " ENDED "
         << bdlt::CurrentTime::utc() << endl;

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
