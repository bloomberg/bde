// btemt_tcptimereventmanager.t.cpp                                   -*-C++-*-

#include <btemt_tcptimereventmanager.h>
#include <bteso_socketimputil.h>
#include <bteso_eventmanagertester.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_ipv4address.h>
#include <bteso_streamsocket.h>

#include <bteso_defaulteventmanager.h>

#include <bcema_testallocator.h>                // for testing only
#include <bcep_threadpool.h>
#include <bcemt_barrier.h>
#include <bces_atomictypes.h>

#include <bslma_testallocatorexception.h>       // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bsls_stopwatch.h>
#include <bsls_platform.h>
#include <bdetu_systemtime.h>
#include <bdet_time.h>
#include <bdetu_systemtime.h>
#include <bdetu_time.h>
#include <bteso_socketimputil.h>

#include <bdef_function.h>
#include <bdef_bind.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_vector.h>

#ifdef BSLS_PLATFORM__OS_UNIX
#include <sys/resource.h>   // getrlimit()
#endif

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//    [TBD - Overview of the test]
//-----------------------------------------------------------------------------
// [XX] [TBD - Method name]

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static int verbose = 0,
           veryVerbose = 0,
           veryVeryVerbose = 0;

bcemt_Mutex         coutMutex;
bcema_TestAllocator defaultAllocator;

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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

typedef btemt_TcpTimerEventManager Obj;

void noopFunction()
{
}

//=============================================================================
//           TEST: 'collectTimeMetrics' configuration flag
//-----------------------------------------------------------------------------

namespace TEST_CASE_COLLECT_TIME_METRICS {

void waitForASec()
    // Delay for 1 second
{
    bcemt_ThreadUtil::sleep(bdet_TimeInterval(1, 0));
}

} // close namespace TEST_CASE_COLLECT_TIME_METRICS

//=============================================================================
//       ADDITIONAL 'enable' and 'isEnabled' TEST: DRQS 15212134
//-----------------------------------------------------------------------------

namespace TEST_CASE_DRQS15212134 {

void testIsEnabled(btemt_TcpTimerEventManager *manager,
                   bces_AtomicInt             *complete)
    // Test whether the specified 'manager' is enabled and then set 'complete'
    // to a non-zero value.
{
    ASSERT(manager->isEnabled());
    *complete = 1;
}

} // close namespace TEST_CASE_DRQS15212134

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Helper function for the test driver

static int maxOpenFiles()
    // Return the maximum number of file descriptors allowed to be opened
    // by this process on success and a negative value on failure.
{
#if defined(BSLS_PLATFORM__OS_WINDOWS)
    return (1 << sizeof(int)) * 16 - 1;
#endif
#if defined(BSLS_PLATFORM__OS_UNIX)
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
    bdef_Function<void (*)()> functor(
            bdef_BindUtil::bind(&timerCb, taskId, mX));

    bdet_TimeInterval now(bdetu_SystemTime::now());
    bdet_TimeInterval timeout(now);
    timeout.addSeconds(1000);

    void *tmrId = mX->registerTimer(timeout, functor);
    mX->deregisterTimer(tmrId);

    timeout = now;timeout.addMilliseconds(10);
    tmrId = mX->registerTimer(timeout, functor);
}

void
socketCb(
    btemt_TcpTimerEventManager *pEventManager,
    btemt_TcpTimerEventManager_ControlChannel *c_p,
    int id)
{
    nbytes[id] += c_p->serverRead();
}

btemt_TcpTimerEventManager *pEventManager = NULL;

extern "C" void *monitorThread(void *arg)
{
    cout << "Monitor thread has started." << endl;
    while (1) {

    }
    return arg;
}

extern "C" void * case100EntryPoint(void *arg)
{
    int j = (int)(bsls_PlatformUtil::IntPtr)arg;
    printf("Thread %d has started\n", j);

    btemt_TcpTimerEventManager& em = *pEventManager;
    btemt_TcpTimerEventManager_ControlChannel chnl;

    bdef_Function<void (*)()> socketFunctor(
            bdef_BindUtil::bind(&socketCb, &em, &chnl, j));

    bdef_Function<void (*)()> timerFunctor(
            bdef_BindUtil::bind(&timerCb, j, &em));

    bdet_TimeInterval timeout(bdetu_SystemTime::now());
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

        em.registerSocketEvent(
            chnl.serverFd(), bteso_EventType::BTESO_READ, socketFunctor);
        ASSERT(1 == em.isRegistered(chnl.serverFd(),
                                    bteso_EventType::BTESO_READ));
        ASSERT(1 == em.numSocketEvents(chnl.serverFd()));

        em.deregisterSocket(chnl.serverFd());
        ASSERT(0 == em.isRegistered(chnl.serverFd(),
                                    bteso_EventType::BTESO_READ));
        ASSERT(0 == em.numSocketEvents(chnl.serverFd()));

        em.registerSocketEvent(
            chnl.serverFd(), bteso_EventType::BTESO_READ, socketFunctor);
        ASSERT(1 == em.isRegistered(chnl.serverFd(),
                                    bteso_EventType::BTESO_READ));
        ASSERT(1 == em.numSocketEvents(chnl.serverFd()));

        ASSERT(1 == chnl.clientWrite());

        em.deregisterSocketEvent(
            chnl.serverFd(), bteso_EventType::BTESO_READ);
        ASSERT(0 == em.isRegistered(chnl.serverFd(),
                                    bteso_EventType::BTESO_READ));
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
static void producer(bcec_Queue<int>            *workQueue,
                     btemt_TcpTimerEventManager *manager,
                     bdet_TimeInterval           nextTime)
    // Enqueue a work item onto the specified 'workQueue' and register
    // this function with the specified 'manager' to be invoked after
    // the specified 'nextTime' absolute time interval.
{
    ASSERT(workQueue);
    ASSERT(manager);

    enum { TIME_OFFSET = 5 };   // invoke timer every 5 seconds

    int item = bdetu_SystemTime::now().nanoseconds() / 1000;

    if (verbose) {
        bsl::cout << bdetu_SystemTime::now()
            << ": Pushing " << item << bsl::endl;
    }
    workQueue->pushBack(item);

    bdet_TimeInterval nextNextTime(nextTime);
    nextNextTime.addSeconds(TIME_OFFSET);
    bdef_Function<void (*)()> callback(
            bdef_BindUtil::bind(&producer, workQueue, manager, nextNextTime));

    void *timerId = manager->registerTimer(nextTime, callback);
    ASSERT(timerId);
}

// ----------------------------------------------------------------------------
bslma_Allocator *testAllocator_p;
static void dummyFunction(void) {

}

enum { NUM_REGISTER_PAIRS = 10 };

bcemt_Barrier *globalBarrier;

extern "C"
void *registerThread(void *arg)
    // Perform registration requests for the event manager specified by 'arg'.
{
    Obj *mX = (Obj*) arg;
    ASSERT(mX);
    ASSERT(globalBarrier);

    bteso_EventManagerTestPair *testPairs;
    testPairs = (bteso_EventManagerTestPair*) (new char[NUM_REGISTER_PAIRS *
                         sizeof(bteso_EventManagerTestPair)]);

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        new (&testPairs[i]) bteso_EventManagerTestPair(veryVerbose);
    }

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        LOOP_ASSERT(i, testPairs[i].isValid());
    }

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        bteso_SocketHandle::Handle fd = testPairs[i].observedFd();
        bslma_TestAllocator ta;
        const bteso_TimerEventManager::Callback callback(&dummyFunction, &ta);
        if (veryVerbose) {
            printf("Thread %d: Iteration (O) %d\n",
                   bcemt_ThreadUtil::self(),
                   i);
        }
        mX->registerSocketEvent(fd,
                                bteso_EventType::BTESO_READ, callback);
        LOOP_ASSERT(i, mX->isRegistered(fd,
                                        bteso_EventType::BTESO_READ));
    }

    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        bteso_SocketHandle::Handle fd = testPairs[i].controlFd();
        bslma_TestAllocator ta;
        const bteso_TimerEventManager::Callback callback(&dummyFunction, &ta);
        mX->registerSocketEvent(fd,
                                bteso_EventType::BTESO_READ, callback);
        if (veryVerbose) {
            printf("Thread %d: Iteration (C)%d\n",
                   bcemt_ThreadUtil::self(),
                   i);
        }
        LOOP_ASSERT(i, mX->isRegistered(fd,
                                        bteso_EventType::BTESO_READ));
    }

    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());

    bteso_SocketHandle::Handle ofd, cfd;
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


    bsls_Stopwatch watch;
    watch.start();
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        testPairs[i].~bteso_EventManagerTestPair();
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

    bteso_EventManagerTestPair testPairs[NUM_REGISTER_PAIRS];
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        LOOP_ASSERT(i, testPairs[i].isValid());
    }
    bslma_TestAllocator ta;
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        bteso_SocketHandle::Handle fd = testPairs[i].observedFd();
        const bteso_TimerEventManager::Callback callback(&dummyFunction, &ta);
        mX->registerSocketEvent(fd,
                                bteso_EventType::BTESO_READ, callback);
        LOOP_ASSERT(i, mX->isRegistered(fd,
                                        bteso_EventType::BTESO_READ));
        mX->deregisterSocketEvent(fd,
                                bteso_EventType::BTESO_READ);
        LOOP_ASSERT(i, 0 == mX->isRegistered(fd,
                                             bteso_EventType::BTESO_READ));
    }
    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        bteso_SocketHandle::Handle fd = testPairs[i].controlFd();
        bdef_Function<void (*)()> callback(&dummyFunction);
        mX->registerSocketEvent(fd,
                                bteso_EventType::BTESO_READ, callback);
        LOOP_ASSERT(i, mX->isRegistered(fd,
                                        bteso_EventType::BTESO_READ));

        mX->deregisterSocketEvent(fd,
                                bteso_EventType::BTESO_READ);
        LOOP_ASSERT(i, 0 == mX->isRegistered(fd,
                                             bteso_EventType::BTESO_READ));
    }

    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());

    for (int i = 0; i < NUM_REGISTER_PAIRS; ++i) {
        bteso_SocketHandle::Handle fd = testPairs[i].controlFd();
        const bteso_TimerEventManager::Callback callback(&dummyFunction, &ta);
    }

    // 'testPairs' cannot be destroyed until ALL threads are done.
    globalBarrier->wait();
    LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                0 == defaultAllocator.numBytesInUse());

    return arg;
}

// ----------------------------------------------------------------------------
static
void timerCallback(int               *isInvokedFlag,
                   bdet_TimeInterval *registrationTime,
                   bdet_TimeInterval  expDelta,
                   int                sequenceNumber,
                   bool               checkDeltaFlag)
{
    ASSERT(isInvokedFlag); ASSERT(registrationTime);
    ASSERT(0 == *isInvokedFlag);
    *isInvokedFlag = 1;
    bdet_TimeInterval now = bdetu_SystemTime::now();
    ASSERT(now >= *registrationTime);
    if (checkDeltaFlag) {
        bdet_TimeInterval delta = now - *registrationTime;
        const double TOLERANCE = 0.15;  // allow 15% inaccuracy
        bdet_TimeInterval deltaLowerBound =
            bdet_TimeInterval(delta.totalSecondsAsDouble() * (1. - TOLERANCE));
        ASSERT(delta >= deltaLowerBound);
        if (expDelta < delta || veryVerbose) {
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
    bdet_TimeInterval timeValues[NUM_TIMERS];
    bdet_TimeInterval now = bdetu_SystemTime::now();
    void *timerIds[NUM_TIMERS];

    for (int i = 0; i < NUM_TIMERS; ++i) {
        int offset = i % 2 ? i : -i;
        offset *= 10 * 1000 * 1000;  // make it milliseconds, at most +/- 100ms
        bdet_TimeInterval delta = bdet_TimeInterval(0, offset);
        timeValues[i] = now + delta;
        flags[i] = 0;
        bdet_TimeInterval expDelta = i % 2 ? delta : bdet_TimeInterval(0);
        expDelta += bdet_TimeInterval(0.25);
        bdef_Function<void (*)()> functor(
                bdef_BindUtil::bind(&timerCallback, &flags[i],
                                    &timeValues[i], expDelta, -i, true));

        timerIds[i] = mX->registerTimer(timeValues[i], functor);
    }

    globalBarrier->wait();
// TBD: Uncomment
//     ASSERT(0 == defaultAllocator.numBytesInUse());

    if (X.isEnabled()) {
        bdet_TimeInterval delta(0.5);  // 500ms
        bcemt_ThreadUtil::sleep(delta);
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

static void executeInParallel(bcemt_ThreadFunction  func,
                              void                 *arg,
                              int                   numThreads)
    // Execute the specified 'func' with the specified 'arg' in 'numThreads'
    // threads and wait until all threads complete.  The concurrency level
    // adjusted automatically.
{
    vector<bcemt_ThreadUtil::Handle> threads(numThreads,
                                             bcemt_ThreadUtil::Handle());

    for (int i = 0; i < numThreads; ++i) {
        ASSERT(0 == bcemt_ThreadUtil::create(&threads[i], bcemt_Attribute(),
                                             func, arg));
    }
    for (int i = 0; i < numThreads; ++i) {
        ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
    }

}

struct my_Event {
    int       d_thread;    // calling thread
    bdet_Time d_timestamp; // invocation time
    void      *d_context;   // user data
};

static void recordCb(void *context,
                     bsl::vector<my_Event> *result) {
    my_Event event;
    event.d_thread = bcemt_ThreadUtil::selfIdAsInt();
    event.d_timestamp = bdetu_SystemTime::nowAsDatetimeGMT().time();
    event.d_context = context;
    result->push_back(event);
}

extern "C" void *executeTest(void *arg) {
    enum { NUM_EXECUTES = 1000 };
    Obj *mX = (Obj*)arg;
    ASSERT(mX);

    vector<my_Event> results;
    for (int i = 0; i < NUM_EXECUTES; ++i) {
        bdef_Function<void (*)()> functor(
                bdef_BindUtil::bindA(testAllocator_p, &recordCb, (void*)i,
                                     &results));
        mX->execute(functor);
    }

    while(results.size() != NUM_EXECUTES) {
        bcemt_ThreadUtil::microSleep(1000, 0);
    }

    int objHandle = results[0].d_thread;
    for (int i = 1; i < NUM_EXECUTES; ++i) {
        LOOP_ASSERT(i, results[i].d_thread == objHandle);
        LOOP_ASSERT(i, results[i - 1].d_context < results[i].d_context);
    }

    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> lock(&coutMutex);
        P_(bcemt_ThreadUtil::selfIdAsInt());
        P_(results[0].d_timestamp);
        P(results[NUM_EXECUTES - 1].d_timestamp);
    }
    return arg;
}

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test
         << " STARTED " << bdetu_SystemTime::nowAsDatetimeGMT() << endl;;

    bcema_TestAllocator testAllocator(veryVeryVerbose);

    testAllocator_p = &testAllocator;
    int rc = bteso_SocketImpUtil::startup();
    ASSERT(0 == rc);

    enum { MIN_REQUIRED_OPEN_FILES = 200 };
    int maxNumOpenFiles = maxOpenFiles();
    ASSERT(MIN_REQUIRED_OPEN_FILES <= maxNumOpenFiles);
    if (MIN_REQUIRED_OPEN_FILES > maxNumOpenFiles) {
        LOOP2_ASSERT(maxNumOpenFiles, MIN_REQUIRED_OPEN_FILES,
                     "Not enough system resources.");
        return testStatus;
    }
    if (veryVerbose) {
        P_(maxNumOpenFiles);
        P(MIN_REQUIRED_OPEN_FILES);
    }

    switch (test) { case 0:
      case 15: {
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

          if (verbose)
              cout << "TESTING USAGE EXAMPLE" << endl
                   << "=====================" << endl;

          enum {
              TIME_OFFSET         = 5 , // seconds
              INITIAL_TIME_OFFSET = 1   // seconds
          };

          bcec_Queue<int> workQueue(&testAllocator);;
          btemt_TcpTimerEventManager manager(&testAllocator);;

          bdet_TimeInterval now = bdetu_SystemTime::now();
          now.addSeconds(INITIAL_TIME_OFFSET);
          bdet_TimeInterval nextTime(now);
          nextTime.addSeconds(TIME_OFFSET);

          bdef_Function<void (*)()> callback(
                  bdef_BindUtil::bind(&producer, &workQueue, &manager,
                                      nextTime));

          void *timerId = manager.registerTimer(now, callback);

          // TBD
          ASSERT(timerId);
          int rc = manager.enable();
          ASSERT(0 == rc);

          for (int i = 0; i < 10; ++i) {
              int item = workQueue.popFront();
              if (verbose) {
                  bsl::cout << bdetu_SystemTime::now()
                      << ": Consuming " << item << bsl::endl;
              }
          }
      } break;

      case 14: {
        // -----------------------------------------------------------------
        // TESTING 'canRegisterSockets' and 'hasLimitedSocketCapacity'
        //
        // Concern:
        //: 1 'hasLimitiedSocketCapacity' returns 'true' if the underlying
        //:   event manager returns 'true' and 'false' otherwise.
        //:
        //: 2 'canRegisterSockets' always returns 'true' if
        //:   'hasLimitedSocketCapacity' is 'false'.
        //:
        //: 3 If 'hasLimitedSocketCapacity' is 'true' then
        //:   'canRegisterSockets' returns 'true' upto 'BTESO_MAX_NUM_HANDLES'
        //:   handles are registered and 'false' after that.
        //
        // Plan:
        //: 1 Assert that 'hasLimitiedSocketCapacity' returns 'true' if the
        //:   underlying event manager returns 'true' and 'false' otherwise.
        //:
        //: 2 Register socket events upto 'BTESO_MAX_NUM_HANDLES'.  Verify
        //:   that 'canRegisterSockets' always returns 'true'.  After that
        //:   limit confirm that 'canRegisterSockets' returns 'false'.
        //
        // Testing:
        //   bool canRegisterSockets() const;
        //   bool hasLimitedSocketCapacity() const;
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                << "TESTING 'canRegisterSockets' and 'hasLimitedSocketCapacity"
                << endl
                << "=========================================================="
                << endl;

#ifdef BSLS_PLATFORM__OS_WINDOWS
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

        if (verbose) cout << "Testing 'canRegisterSockets'" << endl;
        {
            for (int i = 0; i < 2; ++i) {
                Obj mX;  const Obj& X = mX;

                if (i) {
                    mX.enable();
                }

                // As the internal event manager listens on a socket that
                // reduces the number of available sockets.

                const int MAX_NUM_HANDLES = FD_SETSIZE - 1;

                if (veryVerbose) { P(MAX_NUM_HANDLES) }

                bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
                vector<bteso_StreamSocket<bteso_IPv4Address> *>  sockets;
                sockets.reserve(MAX_NUM_HANDLES);

                bteso_StreamSocket<bteso_IPv4Address> *socket;
                bdef_Function<void (*)()> cb1(&noopFunction),
                                          cb2(&noopFunction);

                int  numRead           = 1;
                bool socketAllocFailed = false;
                for (; numRead < MAX_NUM_HANDLES; ++numRead) {
                    socket = factory.allocate();
                    if (!socket) {
                        socketAllocFailed = true;
                        break;
                    }
                    sockets.push_back(socket);

                    if (veryVerbose) { P_(numRead) P(socket->handle()) }

                    int rc = mX.registerSocketEvent(
                                                   socket->handle(),
                                                   bteso_EventType::BTESO_READ,
                                                   cb1);
                    ASSERT(!rc);

                    rc = mX.canRegisterSockets();
                    ASSERT(rc);

                    rc = mX.registerSocketEvent(socket->handle(),
                                                bteso_EventType::BTESO_WRITE,
                                                cb2);
                    ASSERT(!rc);

                    rc = mX.canRegisterSockets();
                    ASSERT(rc);
                }

                if (!socketAllocFailed) {
                    ASSERT(numRead == MAX_NUM_HANDLES);

                    int rc = mX.canRegisterSockets();
                    ASSERT(rc);

                    socket = factory.allocate();
                    sockets.push_back(socket);

                    rc = mX.registerSocketEvent(socket->handle(),
                                                bteso_EventType::BTESO_READ,
                                                cb1);
                    ASSERT(!rc);

                    rc = mX.canRegisterSockets();

#ifdef BSLS_PLATFORM__OS_WINDOWS
                    ASSERT(!rc);
#else
                    ASSERT(rc);
#endif

                    mX.deregisterSocketEvent(socket->handle(),
                                             bteso_EventType::BTESO_READ);

                    rc = mX.canRegisterSockets();
                    ASSERT(rc);

                    rc = mX.registerSocketEvent(socket->handle(),
                                                bteso_EventType::BTESO_READ,
                                                cb1);
                    ASSERT(!rc);

                    rc = mX.canRegisterSockets();
#ifdef BSLS_PLATFORM__OS_WINDOWS
                    ASSERT(!rc);
#else
                    ASSERT(rc);
#endif
                }

                mX.disable();

                const int NUM_SOCKETS = sockets.size();
                for (int i = 0; i < NUM_SOCKETS; ++i) {
                    factory.deallocate(sockets[i]);
                }
            }
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
        //   rescheduleTimer
        // --------------------------------------------------------------------
        if (verbose)
            cout << "TESING 'rescheduleTimer'" << endl
                 << "=======================" << endl;
        {
            Obj mX(&testAllocator);   const Obj& X = mX;
            ASSERT(0 == mX.enable()); ASSERT(mX.isEnabled());

            enum { NUM_TIMERS  = 10000 };
            bdet_TimeInterval  timeValues[NUM_TIMERS];

            // DELTA had to be increased from 0.5 for when built in safe mode
            // on Solaris

            const double       DELTA = 1.25;
            bdet_TimeInterval  delta(DELTA);
            int                flags[NUM_TIMERS];
            void              *ids[NUM_TIMERS];

            bdet_TimeInterval  start = bdetu_SystemTime::now();

            for (int i = 0; i < NUM_TIMERS; ++i) {
                flags[i] = 0;
                timeValues[i] = bdetu_SystemTime::now() + delta;
                bdef_Function<void (*)()> functor(
                        bdef_BindUtil::bind(&timerCallback,
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
                      (bdetu_SystemTime::now() - start).totalSecondsAsDouble();
            LOOP_ASSERT(soFar, soFar < DELTA);
            if (verbose) { P_(DELTA); P(soFar); }

            for (int i = 0; i < NUM_TIMERS; ++i) {
                LOOP_ASSERT(i, 0 == flags[i]);
            }

            soFar = (bdetu_SystemTime::now() - start).totalSecondsAsDouble();
            LOOP_ASSERT(soFar, soFar < DELTA);

            for (int i = 0; i < NUM_TIMERS; ++i) {
                bslma_TestAllocator da;
                bslma_DefaultAllocatorGuard dag(&da);

                const int rc = mX.rescheduleTimer(
                                              ids[i],
                                              bdetu_SystemTime::now() + delta);

                LOOP_ASSERT(i, !rc);
                LOOP_ASSERT(i, 0 == da.numBytesInUse());
            }

            if (veryVerbose) {
                cout << "\t\tRescheduled " << NUM_TIMERS << " timers." << endl;
            }

            bcemt_ThreadUtil::sleep(delta);

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
          //   o DRQS 16796407.  That the optional 'collectTimeMetrics'
          //       construction parameter is correctly applied.
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
          //  btemt_TcpTimerEventManager(Hint , bool, bslma_Allocator *);
          //  btemt_TcpTimerEventManager(bteso_EventManager  *,
          //                             bslma_Allocator     *);
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

              bteso_DefaultEventManager<bteso_Platform::SELECT>
                                          dummyEventManager(0, &testAllocator);
              Obj mA(&testAllocator);
              Obj mB(Obj::BTEMT_NO_HINT, &testAllocator);
              Obj mC(Obj::BTEMT_NO_HINT, true, &testAllocator);
              Obj mD(Obj::BTEMT_NO_HINT, false, &testAllocator);
              Obj mE(Obj::BTEMT_NO_HINT, false, true, &testAllocator);
              Obj mF(Obj::BTEMT_NO_HINT, false, false, &testAllocator);
              Obj mG(&dummyEventManager, &testAllocator);

              const Obj& A = mA;
              const Obj& B = mB;
              const Obj& C = mC;
              const Obj& D = mD;
              const Obj& E = mE;
              const Obj& F = mF;
              const Obj& G = mG;
              ASSERT(true  == A.hasTimeMetrics());
              ASSERT(true  == B.hasTimeMetrics());
              ASSERT(true  == C.hasTimeMetrics());
              ASSERT(false == D.hasTimeMetrics());
              ASSERT(false == E.hasTimeMetrics());
              ASSERT(false == F.hasTimeMetrics());
              ASSERT(false == G.hasTimeMetrics());
          }
          {
              if (veryVerbose) {
                  cout << "\tConfigure metrics to be collected" << endl;
              }

              enum { BUFFER_SIZE = 50 };
              char buffer[BUFFER_SIZE];
              memset(buffer, 0, BUFFER_SIZE);

              bteso_SocketHandle::Handle handles[2];
              ASSERT(0 == bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                           handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM));
              ASSERT(0 !=
                  bteso_SocketImpUtil::write(handles[0], buffer, BUFFER_SIZE));

              Obj mX(&testAllocator); const Obj& X = mX;
              bdef_Function<void (*)()> callback(&waitForASec);
              ASSERT(0 == mX.registerSocketEvent(handles[1],
                                                 bteso_EventType::BTESO_READ,
                                                 callback));
              ASSERT(0 == mX.enable());

              bcemt_ThreadUtil::microSleep(10000); // 10 ms
              ASSERT(0 == mX.disable());
              int percent = mX.timeMetrics()->percentage(
                                           bteso_TimeMetrics::BTESO_CPU_BOUND);
              LOOP_ASSERT(percent, percent > 80);

              if (veryVerbose) {
                  P(percent);
              }
              bteso_SocketImpUtil::close(handles[0]);
              bteso_SocketImpUtil::close(handles[1]);

          }
          {
              if (veryVerbose) {
                  cout << "\tConfigure metrics to not be collected" << endl;
              }

              enum { BUFFER_SIZE = 50 };
              char buffer[BUFFER_SIZE];
              memset(buffer, 0, BUFFER_SIZE);

              bteso_SocketHandle::Handle handles[2];
              ASSERT(0 == bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                           handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM));
              ASSERT(0 !=
                  bteso_SocketImpUtil::write(handles[0], buffer, BUFFER_SIZE));

              Obj mX(Obj::BTEMT_NO_HINT, false, &testAllocator);
              const Obj& X = mX;
              bdef_Function<void (*)()> callback(&waitForASec);
              ASSERT(0 == mX.registerSocketEvent(handles[1],
                                                 bteso_EventType::BTESO_READ,
                                                 callback));
              ASSERT(0 == mX.enable());

              bcemt_ThreadUtil::microSleep(10000); // 10 ms
              ASSERT(0 == mX.disable());
              int percent = mX.timeMetrics()->percentage(
                                           bteso_TimeMetrics::BTESO_CPU_BOUND);
              LOOP_ASSERT(percent, percent <= 1);

              if (veryVerbose) {
                  P(percent);
              }

              bteso_SocketImpUtil::close(handles[0]);
              bteso_SocketImpUtil::close(handles[1]);

          }
      } break;
      case 11: {
          // ----------------------------------------------------------------
          // ADDITIONAL 'enable' and 'isEnabled' TEST: DRQS 15212134
          //
          // Concerns:
          //   o DRQS 151212134 -
          //           that callbacks dispatched by events registed prior
          //           to invoking 'enable', find 'isEnabled' to be 'true'.
          //
          // Plan:
          //   Create a socket pair and write data to both ends of the open
          //   connection, such that a 'bteso_EventManager' created for these
          //   sockets would dispatch a 'READ' event on each socket.  Register
          //   these socket handles with the tcp event manager under dispatch,
          //   such that they will dispatch 'READ' events to the test callback
          //   'testIsEnabled'.  Then call 'enable'.
          //
          // Testing:
          //    int enable();
          //    int isEnabled() const;
          // ----------------------------------------------------------------

          if (verbose)
              cout << "TESTING enable() and isEnabled()" << endl
                   << "================================" << endl;

          using namespace TEST_CASE_DRQS15212134;

          enum { BUFFER_SIZE = 50 };
          char buffer[BUFFER_SIZE];
          memset(buffer, 0, BUFFER_SIZE);

          bteso_SocketHandle::Handle handles[2];
          ASSERT(0 == bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                           handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM));

          ASSERT(0 !=
                 bteso_SocketImpUtil::write(handles[0], buffer, BUFFER_SIZE));
          ASSERT(0 !=
                 bteso_SocketImpUtil::write(handles[1], buffer, BUFFER_SIZE));

          bces_AtomicInt complete(0);
          btemt_TcpTimerEventManager manager(&testAllocator);
          bdef_Function<void (*)()> callback(
                  bdef_BindUtil::bind(&testIsEnabled, &manager, &complete));
          ASSERT(0 == manager.registerSocketEvent(handles[0],
                                                  bteso_EventType::BTESO_READ,
                                                  callback));
          ASSERT(0 == manager.registerSocketEvent(handles[1],
                                                  bteso_EventType::BTESO_READ,
                                                  callback));
          manager.enable();
          manager.disable();
          ASSERT(0 != complete);

          bteso_SocketImpUtil::close(handles[0]);
          bteso_SocketImpUtil::close(handles[1]);
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
          //   void execute(const bdef_Function<void (*)()>& functor);
          // ----------------------------------------------------------------

          if (verbose)
              cout << "TESTING execute METHOD." << endl
                   << "=======================" << endl;

          enum { NUM_THREADS = 16 };

          if (verbose)
              cout << "\tOn Enabled object" << endl;
          {
              Obj mX(&testAllocator);
              mX.enable();
              ASSERT(1 == mX.isEnabled());
              executeInParallel(executeTest, (void*)&mX, NUM_THREADS);
              mX.disable();
          }
#if 0
    // TBD -- currently, the behavior is undefined if the object is disabled
          if (verbose)
              cout << "\tOn disabled object" << endl;
          {
              Obj mX(&testAllocator);
              ASSERT(0 == mX.isEnabled());
              executeInParallel(executeTest, (void*)&mX, NUM_THREADS);
          }
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'enable' and 'disable' methods in a callback
        // Concern:
        //   Verify that 'enable', 'disable' and 'isEnabled' work as
        //   advertised when invoked in a callback.
        //
        // Testing:
        //   enable()
        //   disable()
        //   isEnabled
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

                bteso_EventManagerTestPair testPair;
                bdef_Function<void (*)()> callback(
                        bdef_BindUtil::bindA(&testAllocator, &disableCb, &mX));

                mX.registerSocketEvent(testPair.observedFd(),
                                       bteso_EventType::BTESO_WRITE, callback);

                bcemt_ThreadUtil::microSleep(10000); // 10 ms
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
        //   deregisterSocketEvent
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
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];
            bcemt_Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma_DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_Attribute(),
                                                  &deregisterThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
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
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];
            bcemt_Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma_DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_Attribute(),
                                                  &deregisterThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
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
        //   deregisterSocketEvent
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
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];
            bcemt_Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;
            if (veryVerbose) {
                Q(before deregisterThread);
                P(mX.numTotalSocketEvents());
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma_DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_Attribute(),
                                                  &deregisterThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
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
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];
            bcemt_Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            if (veryVerbose) {
                Q(before deregisterThread);
                P(mX.numTotalSocketEvents());
            }

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma_DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_Attribute(),
                                                  &deregisterThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
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
        //   registerSocketEvent
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
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];

            bcemt_Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma_DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_Attribute(),
                                                  &registerThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
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
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];

            bcemt_Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bslma_DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_Attribute(),
                                                  &registerThread, &mX);
                LOOP_ASSERT(i, 0 == rc);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
                LOOP_ASSERT(i, 0 == rc);
            }

            LOOP_ASSERT(defaultAllocator.numBytesInUse(),
                        0 == defaultAllocator.numBytesInUse());

            // Call isRegistered() because it will do a waitForResult() which
            // will allow all pending deregistration events to complete, which
            // will give us the right result from numTotalSocketEvents()
            bteso_SocketHandle::Handle nilHandle = 0;
            mX.isRegistered(nilHandle, bteso_EventType::BTESO_READ);
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
        //   disable
        //   enable
        //   registerTimer
        // --------------------------------------------------------------------
        if (verbose)
            cout << "TESING 'enable', 'disable' AND 'registerTimer'" << endl
                 << "==============================================" << endl;
        {
            for (int k = 0; k < 2; ++k) {
                Obj mX(Obj::BTEMT_NO_HINT, false, (bool) k, &testAllocator);
                const Obj& X = mX;

                ASSERT(0 == mX.enable()); ASSERT(mX.isEnabled());

                enum { NUM_TIMERS  = 100000, NUM_ATTEMPTS = 10 };
                bsl::vector<bdet_TimeInterval> timeValues(NUM_TIMERS);
                bdet_TimeInterval offset(3.0);
                bdet_TimeInterval delta(0.5); // 1/2 seconds
                int flags[NUM_TIMERS];

                for (int i = 0; i < NUM_TIMERS; ++i) {
                    flags[i] = 0;
                    timeValues[i] = bdetu_SystemTime::now();
                    timeValues[i] += offset;

                    bdef_Function<void (*)()> functor(
                        bdef_BindUtil::bind(&timerCallback,
                                            &flags[i], &timeValues[i],
                                            delta, i, false));
                    mX.registerTimer(timeValues[i], functor);
                }
                if (veryVerbose) {
                    cout << "\t\tRegistered " << NUM_TIMERS
                         << " timers." << endl;
                }

                bslma_TestAllocator da;
                bslma_DefaultAllocatorGuard dag(&da);

                for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                    LOOP_ASSERT(i, 0 == mX.disable());
                    LOOP_ASSERT(i, 0 == X.isEnabled());
                    LOOP_ASSERT(i, 0 == mX.enable());
                    LOOP_ASSERT(i, 1 == X.isEnabled());

                    LOOP_ASSERT(i, 0 == da.numBytesInUse());
                }
                bcemt_ThreadUtil::sleep(
                timeValues[NUM_TIMERS - 1] - bdetu_SystemTime::now() + delta);

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
        //   registerTimer
        // --------------------------------------------------------------------
        if (verbose)
            cout << "TESING 'registerTimer'" << endl
                 << "======================" << endl;
        {
            for (int k = 0; k < 2; ++k) {
                Obj mX(Obj::BTEMT_NO_HINT, false, (bool) k, &testAllocator);
                const Obj& X = mX;

                ASSERT(0 == mX.enable()); ASSERT(mX.isEnabled());

                enum { NUM_TIMERS  = 10000 };
                bdet_TimeInterval timeValues[NUM_TIMERS];
                bdet_TimeInterval delta(0.5); // 1/2 seconds
                int flags[NUM_TIMERS];

                for (int i = 0; i < NUM_TIMERS; ++i) {
                    flags[i] = 0;
                    timeValues[i] = bdetu_SystemTime::now();
                    bdef_Function<void (*)()> functor(
                        bdef_BindUtil::bind(&timerCallback,
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
                bcemt_ThreadUtil::sleep(delta);

                ASSERT(0 == mX.disable());
                for (int i = 0; i < NUM_TIMERS; ++i) {
                    LOOP_ASSERT(i, 1 == flags[i]);
                }

                bcemt_ThreadUtil::sleep(bdet_TimeInterval(5));
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
        //   registerTimer
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'registerTimer' METHOD" << endl
                 << "==============================" << endl;

        if (verbose)
            cout << "\tConcern #1: Registration on disabled object."
                 << endl;
        {
            enum { NUM_THREADS = 10 };
            Obj mX(Obj::BTEMT_NO_HINT, false, true, &testAllocator);
            const Obj& X = mX;

            ASSERT(0 == mX.isEnabled());
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];

            bcemt_Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            bslma_DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_Attribute(),
                                                  &testTimersThread,
                                                  &mX);
                LOOP_ASSERT(i, 0 == rc);
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
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
            Obj mX(Obj::BTEMT_NO_HINT, false, true, &testAllocator);
            const Obj& X = mX;
            ASSERT(0 == mX.enable()); ASSERT(X.isEnabled());
            bcemt_ThreadUtil::Handle workers[NUM_THREADS];

            ASSERT(0 == defaultAllocator.numBytesInUse());
            bcemt_Barrier barrier(NUM_THREADS);
            globalBarrier = &barrier;

            bslma_DefaultAllocatorGuard dag(&defaultAllocator);

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::create(&workers[i],
                                                  bcemt_Attribute(),
                                                  &testTimersThread,
                                                  &mX);
                LOOP_ASSERT(i, 0 == rc);
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                int rc = bcemt_ThreadUtil::join(workers[i]);
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
        //   enable
        //   disable
        //   isEnabled
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

                bslma_TestAllocator da;
                bslma_DefaultAllocatorGuard dag(&da);

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

                bslma_TestAllocator da;
                bslma_DefaultAllocatorGuard dag(&da);

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

                bslma_TestAllocator da;
                bslma_DefaultAllocatorGuard dag(&da);

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

                bslma_TestAllocator da;
                bslma_DefaultAllocatorGuard dag(&da);

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

                bslma_TestAllocator da;
                bslma_DefaultAllocatorGuard dag(&da);

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
        if (verbose)
            cout << "BREATHING TEST" << endl
                 << "==============" << endl;

        Obj mX(&testAllocator); const Obj& X = mX;

        ASSERT(0 == mX.isEnabled());
        if (veryVerbose) {
            P(X.numTotalSocketEvents());
            P(X.numTimers());
            P(X.numEvents());
        }
#ifdef BSLS_PLATFORM__OS_UNIX
        ASSERT((bcemt_ThreadUtil::Handle) -1 == X.dispatcherThreadHandle());
#endif

        ASSERT(0 == X.numTotalSocketEvents());
        ASSERT(0 == X.numTimers());
        ASSERT(0 == X.numEvents());
        ASSERT(0 == mX.enable());
        ASSERT(1 == mX.isEnabled());
#ifdef BSLS_PLATFORM__OS_UNIX
        ASSERT((bcemt_ThreadUtil::Handle) -1 != X.dispatcherThreadHandle());
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

        btemt_TcpTimerEventManager em(&testAllocator);
        em.enable();
        pEventManager = &em;

        enum {
            MIN_THREADS = NUM_TASKS,
            MAX_THREADS = NUM_TASKS,
            MAX_IDLE_TIME = 100
        };

        bcep_ThreadPool threadPool(bcemt_Attribute(),
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
                   (bcep_ThreadPoolJobFunc)case100EntryPoint,
                   (void*)i));
        }

        bteso_TimeMetrics *metrics = pEventManager->timeMetrics();
        ASSERT(metrics);
        while (threadPool.numActiveThreads()) {
         printf("=========================================================\n"
                "| Load = %d%%\n"
                "| numEvents = %d\n"
                "| numTotalSocketEvents = %d\n"
                "| numTimers = %d\n"
                "=========================================================\n",
                metrics->percentage(bteso_TimeMetrics::BTESO_CPU_BOUND),
                pEventManager->numEvents(),
                pEventManager->numTotalSocketEvents(),
                pEventManager->numTimers()
               );

            metrics->resetAll();
            metrics->resetStartTimes();
            bcemt_ThreadUtil::microSleep(0, 5);  // 5 seconds
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

    rc = bteso_SocketImpUtil::cleanup();
    ASSERT(0 == rc);

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    cout << "TEST CASE " << test << " ENDED "
         << bdetu_SystemTime::nowAsDatetimeGMT() << endl;

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
