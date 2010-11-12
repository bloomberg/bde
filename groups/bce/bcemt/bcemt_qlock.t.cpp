// bcemt_qlock.t.cpp              -*-C++-*-
#include <bcemt_qlock.h>

#include <bcemt_barrier.h>
#include <bcemt_lockguard.h>
#include <bcemt_thread.h>
#include <bcemt_semaphore.h>

#include <bces_atomicutil.h>

#include <bsls_stopwatch.h>
#include <bsls_timeutil.h>

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strcmp()
#include <bsl_c_stdlib.h>     // rand_r()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The basic idea of testing bcemt_QLock and bcemt_QLock is to create
// thread poll and manipulate various test data in parallel, then to
// check the the integrity of data is preserved.
//
// The helper class  MyTask provides thread pool and its management.
// This class creates thread pool, executes in each thread specified
// function and join all threads to make sure that there are no
// unfinished threads left and there is no leak of resources.
// The class also is a convenient place holder to common data
// used by thread function.
//
// The helper class Rand provides generation of pseudo-random numbers
// for each thread.
//-----------------------------------------------------------------------------
// CLASS bcemt_QLock
// CLASS bcemt_QLockGuard
// [ 5] Contention test: many threads - many bcemt_QLocks
// [ 4] Test of internal QLock core primitives:
//      setFlag and waitOnFlag
// [ 3] Aggressive contention test: many threads - one bcemt_QLock
// [ 2] USAGE EXAMPLE:  MT-Safe Singleton.
//      Design is based on combination of bces_AtomicInt
//      and bcemt_QLocks.
// [ 1] BREATHING TEST

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose = 0;
int veryVerbose = 0;

//-----------------------------------------------------------------------------
//   Class providing thread pool to run testCase functions
//-----------------------------------------------------------------------------

class MyTask {

  public:
    typedef void* (*TestFunc)(int threadNum, const MyTask& task);

  private:
    enum {MAX_THREADS = 100};

    bcemt_ThreadAttributes    d_attr;
    TestFunc                  d_f;
    void                     *d_arg;
    bcemt_ThreadUtil::Handle  d_handles[MAX_THREADS];
    bcemt_Mutex               d_logMutex;

    bces_AtomicInt            d_numThreadsStarted;
     // number of threads started since last call start ()

    bces_AtomicInt            d_numThreadsFinished;
     // number of threads finished since last call start ()

    bces_AtomicInt            d_totalThreadsStarted;
     // total number of threads started since task creation

    bces_AtomicInt            d_totalThreadsFinished;
     // number of threads finished since task creation

    bcemt_Barrier            *d_barrier;
     // Barrier to provide hard contention.
     // It is created on each start()call and
     // it is destroyed on each stop() call

  public:
    MyTask(TestFunc f, void *arg);
    virtual ~MyTask();

    int start(int numThreads);
    int stop();

    void *arg() const
    {
        return d_arg;
    }

    bcemt_Barrier *barrier() const
    {
        return d_barrier;
    }

    int numThreadsStarted() const
    {
        return d_numThreadsStarted;
    }

    int numThreadsFinished() const
    {
        return d_numThreadsStarted;
    }

    int totalThreadsStarted() const
    {
        return d_totalThreadsStarted;
    }

    int totalThreadsFinished() const
    {
        return d_totalThreadsStarted;
    }

    virtual void *run(void);
};

extern "C"
void *threadFunc(void *data)
{
    MyTask *task = (MyTask*) data;
    return task->run ();
}

MyTask::MyTask(TestFunc f, void *arg)
: d_attr()
, d_f(f)
, d_arg(arg)
, d_numThreadsStarted(0)
, d_numThreadsFinished(0)
, d_totalThreadsStarted(0)
, d_totalThreadsFinished(0)
, d_barrier(0)
{
    d_attr.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_JOINABLE);
    d_attr.setStackSize (1024*128);

    for (int i=0; i < MAX_THREADS; ++i) {
        d_handles[i] = bcemt_ThreadUtil::invalidHandle();
    }

    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> lg(&d_logMutex);
        bsl::cout << "MyTask CTOR  ThrId="
                    << bcemt_ThreadUtil::selfId()
                    << bsl::endl;
    }
}

MyTask::~MyTask()
{
    stop();

    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> lg(&d_logMutex);
        bsl::cout << "MyTask DTOR  ThrId="
                  << bcemt_ThreadUtil::selfId()
                  << bsl::endl;
    }
}

int MyTask::start(int numThreads)
{
    ASSERT(d_numThreadsStarted == d_numThreadsFinished);

    d_numThreadsStarted  = 0;
    d_numThreadsFinished = 0;

    ASSERT(d_barrier == 0);
    d_barrier = new  bcemt_Barrier(numThreads);

    for (int i=0; i < MAX_THREADS && numThreads > 0; ++i)
    {
        if (bcemt_ThreadUtil::isEqual(d_handles[i],
                                      bcemt_ThreadUtil::invalidHandle())) {

            int rc = bcemt_ThreadUtil::create(&d_handles[i],
                                              d_attr,
                                              threadFunc,
                                              this);
            --numThreads;

            ASSERT(rc == 0);
            ASSERT(!bcemt_ThreadUtil::isEqual(
                                           d_handles[i],
                                           bcemt_ThreadUtil::invalidHandle()));
        }
    }

    return 0 == numThreads ? 0 : -1;
}

int MyTask::stop()
{
    for (int i=0; i < MAX_THREADS; ++i) {

        if (!bcemt_ThreadUtil::isEqual(d_handles[i],
                                       bcemt_ThreadUtil::invalidHandle())) {

            int rc = bcemt_ThreadUtil::join (d_handles[i], 0);
            ASSERT(rc == 0);
            d_handles[i] = bcemt_ThreadUtil::invalidHandle();
        }
    }
    ASSERT (d_numThreadsStarted == d_numThreadsFinished);

    delete d_barrier;
    d_barrier = 0;

    return 0;
}

void *MyTask::run()
{
    void *ret = 0;

    int   threadNum = ++d_numThreadsStarted;
    ++d_totalThreadsStarted;

    if (veryVerbose) {
       bcemt_LockGuard<bcemt_Mutex> lg(&d_logMutex);
       bsl::cout << "***Enter Thread: Id="
                 << bcemt_ThreadUtil::selfId()
                 << " Num="
                 << threadNum
                 << bsl::endl;
    }

    // This will increase the contention
    barrier()->wait();

    if (d_f) {
        ret = d_f(threadNum, *this);
    }

    ++d_numThreadsFinished;
    ++d_totalThreadsFinished;

    if (veryVerbose) {
       bcemt_LockGuard<bcemt_Mutex> lg(&d_logMutex);
       bsl::cout << "***Leave Thread: Id="
                 << bcemt_ThreadUtil::selfId()
                 << " Num="
                 << threadNum
                 << bsl::endl;
    }

    return ret;
}

//-----------------------------------------------------------------------------
//   Class generating random numbers
//-----------------------------------------------------------------------------
class Rand {

    // DATA
    unsigned int d_seed;

  public:
    // CREATORS
    Rand(int seedParm) : d_seed(seedParm)
    {
        d_seed ^= (int)(bsl::size_t) this;

        if (veryVerbose) {
            bsl::cout << "Rand seed=" << d_seed << bsl::endl;
        }
        srand(d_seed);
    }

    int get()
    {
        int next;
#if defined BSLS_PLATFORM__OS_WINDOWS
        next = rand();
#else
        next = rand_r(&d_seed);
#endif
        return next;
    }
};

void printMetrics(bsl::ostream&   out,
                  const char      *name,
                  bsl::size_t           numTotal,
                  bsls_Stopwatch&  sw)
{
    out << name
        << " NumIter="  << numTotal
        << " Time="  << sw.elapsedTime()
        << bsl::endl;
}

typedef bcemt_Semaphore Semaphore;

// --------------------------------------------------------------------
// TestCase 7. Multiple-threads, 2-qlocks
//
// Concerns:
//   Verify that each thread obtains the lock in the order in
//   which it requested it.
//
// Plan:
// The two qlocks are completely independent.  A thread locking one QLock
// does not wait for a thread that holds the lock to the other.  Two context
// structures, each with a QLock, a few threads on each context.
// In the critical region, each thread sets a flag in its context,
// reads the other context's flag, sleeps briefly, clears its flag,
// exits the critical region, and loops.  When every thread's
// flag has been represented in the other context structure, all threads
// exit.  An error is reported if the maximum loop count is reached.
//
// --------------------------------------------------------------------

struct ContextCase7 {
    bcemt_QLock     *d_qlock;
    bces_AtomicInt   d_owner;
    bsl::vector<int> d_slots;
};

struct DataCase7 {
    bcemt_Barrier    *d_barrier;  // common barrier
    ContextCase7     *d_myContext;
    ContextCase7     *d_otherContext;
};

void *testCase7(int threadNum, const MyTask& task)
{
     DataCase7 *data = reinterpret_cast<DataCase7 *> (task.arg());

     ASSERT(data != 0);
     ASSERT(threadNum > 0);

     data->d_barrier->wait();

     bool flgExit = false;

     while(!flgExit) {

        // Lock own context
        bcemt_QLockGuard guard(data->d_myContext->d_qlock);

        // set owner of this context
        data->d_myContext->d_owner = threadNum-1;

        // Read other context owner
        int otherOwner = data->d_otherContext->d_owner;

        // if other owner is valid, mark it visible in this context
        if (otherOwner != -1) {
            ASSERT(otherOwner < data->d_myContext->d_slots.size());
            data->d_myContext->d_slots[otherOwner] = otherOwner;
        }

        // lets threads of other context to see me
        bcemt_ThreadUtil::yield();

        // exit flag is OK
        // if this thread has been seen in other context
        // and we have seen all threads in other context
        flgExit = (data->d_otherContext->d_slots[threadNum-1] != -1);

        for (bsl::size_t i=0;  i < data->d_myContext->d_slots.size(); ++i) {
            if (data->d_myContext->d_slots[i] == -1) {
                flgExit = false;
                break;
            }
        }

        // Clear the owner of my context
        data->d_myContext->d_owner = -1;
     }
     return 0;
}

// --------------------------------------------------------------------
// TestCase 6. Fairness Test
//
// Concerns:
//   Verify that each thread obtains the lock in the order in
//   which it requested it.
//
// Plan:
//  A bunch of threads wait on a barrier, then compete for a QLock.
//  Upon acquiring the lock, each thread sets a thread-specific flag,
//  then sleeps very briefly, releases the lock, and loops.  If the
//  flag has already been set (i.e., second time in for that thread),
//  assert that all of the other flags are also set, i.e., every other
//  thread has had a turn before any thread has a second chance.
//  Each thread exits after its second time in the critical region.
//
//   Repeat above scenario several times, incrementing the number of
//   threads by one on each iteration.
// --------------------------------------------------------------------

struct DataCase6 {
    bsl::vector<int>  d_slots;
    bcemt_QLock      *d_qlock;
};

void *testCase6(int threadNum, const MyTask& task)
{
     DataCase6 *data = reinterpret_cast<DataCase6 *> (task.arg());

     ASSERT(data != 0);
     ASSERT(threadNum > 0);

     bsl::size_t i = 0;

     for (i=0; i < data->d_slots.size(); ++i) {
         ASSERT(data->d_slots[i] == 0);
     }

     task.barrier()->wait();

     {
        bcemt_QLockGuard guard(data->d_qlock);

        //sleep enough to allow other threads be enqueued
        bcemt_ThreadUtil::microSleep(1000*100);

        ASSERT (data->d_slots[threadNum-1] == 0);
        data->d_slots[threadNum-1] = 1;
     }

     {
         bcemt_QLockGuard guard(data->d_qlock);

         //bcemt_ThreadUtil::microSleep(1000*250);

         for (i=0; i < data->d_slots.size(); ++i) {
            ASSERT(data->d_slots[i] != 0);
        }
     }

     return 0;
}

// -------------------------------------------------------------------
// Case 5
// Multiple threads - multiple QLocks Test.  At the same time
// this test can be used for performance evaluation for such
// scenario.
//
// Concerns:
//   To test behavior and performance in case of many threads
//   and many QLocks.
//
// Plan:
//  Create array of data slots.  Each element-slot is protected by
//  its own QLock.  Create multiple threads.  Each thread in the loop
//  obtains a lock for the random slot.  Upon acquiring the lock for
//  the slot, the thread stores locally the slot data, manipulates
//  them with slot data, ensures the slot data is not modified by
//  other threads, restores the original slot data, and releases
//  the slot lock.  Repeat above action in the loop.
//
//  Repeat above scenario several times, incrementing the number of
//  threads by one on each iteration.
// -------------------------------------------------------------------
struct CaseData5 {
    int           d_numIter;
    int           d_numElements;
    bcemt_QLock  *d_qlocks;
    bcemt_Mutex  *d_mutexes;
    int          *d_slots;
};

void *testCase5_fn1(int threadNum, const MyTask& task)
{
    CaseData5 *data = reinterpret_cast< CaseData5 *> (task.arg());

    Rand rand (threadNum);

    for (int i=0; i < data->d_numIter; ++i) {

        // get random slot
        int slotIndex = rand.get() % data->d_numElements;

        bcemt_QLockGuard guard(&data->d_qlocks[slotIndex]);

        data->d_slots[slotIndex] = 0;
        ASSERT(data->d_slots[slotIndex] == 0);

        data->d_slots[slotIndex] |= 1;
        ASSERT(data->d_slots[slotIndex] == 1);

        data->d_slots[slotIndex] <<= 1;
        ASSERT(data->d_slots[slotIndex] == 2);

        data->d_slots[slotIndex] ^= 2;
        ASSERT(data->d_slots[slotIndex] == 0);

        guard.unlock();

    }

    return 0;
}

void *testCase5_fn2(int threadNum, const MyTask& task)
{
    CaseData5 *data = reinterpret_cast< CaseData5 *> (task.arg());

    Rand rand (threadNum);

    for (int i=0; i < data->d_numIter; ++i) {

        // get random slot
        int slotIndex = rand.get() % data->d_numElements;

        bcemt_LockGuard<bcemt_Mutex> guard(&data->d_mutexes[slotIndex]);

        data->d_slots[slotIndex] = 0;
        ASSERT(data->d_slots[slotIndex] == 0);

        data->d_slots[slotIndex] |= 1;
        ASSERT(data->d_slots[slotIndex] == 1);

        data->d_slots[slotIndex] <<= 1;
        ASSERT(data->d_slots[slotIndex] == 2);

        data->d_slots[slotIndex] ^= 2;
        ASSERT(data->d_slots[slotIndex] == 0);

    }

    return 0;
}

// --------------------------------------------------------------------
// Case 4.
// Set/Wait Flags Test
//
// Concerns:
//   To test internal primitives setFlags/waitOnFlag on which QLock
//   is based.
//
// Plan:
//  Create two threads and two flags.  First thread in the loop sets
//  the first flag and waits on second flag.  The second thread sets
//  the second flag and waits on the first.  Ensure that threads
//  never have a deadlock.
//  'Flag' means an atomic pointer to the synchronization object and
//  can be in one of three states:
//  0    nor 'setFlag' neither 'waitOnFlag' have been called yet
//  -1   'setFlag' was called before 'waitOnFlag', so there is no
//        need for 'waitOnFlag' to wait on synchronization object
//  XXX  'waitOnFlag' was called before 'setFlag' and waits on
//        synchronization object with address 'XXX'.  'setFlag' must
//        post/signal/notify that synchronization object.
//
// --------------------------------------------------------------------
static Semaphore *const dummySemaphorePtr =
      reinterpret_cast<Semaphore *const> (-1L);

struct DataCase4 {
    bces_AtomicPointer<Semaphore>  d_event1;
    bces_AtomicPointer<Semaphore>  d_event2;
    bces_AtomicInt                 d_flagStart;
    bces_AtomicInt                 d_flagEnd;

    void reset ()
    {
        d_event1.swap(0);
        d_event2.swap(0);
        d_flagStart.swap(0);
        d_flagEnd.swap(0);

        ASSERT (d_flagStart == 0);
        ASSERT (d_flagEnd == 0);
    }
};

void setFlag(bces_AtomicPointer<Semaphore> *flag)
{
    // set '-1' if it was 0, and get the original value
    Semaphore *event = flag->testAndSwap(0, dummySemaphorePtr);

    if (event) {

        ASSERT (event != dummySemaphorePtr);

        // Another thread has already stored
        // an event handle in the flag.
        event->post();
    }
}

void waitOnFlag(bces_AtomicPointer<Semaphore> *flag, int spinCount)
{
    Semaphore *event = 0;
    int        i     = 0;

    do {
        // read flag value with memory barrier
        event = *flag;
    }
    while ((event == 0) && (++i < spinCount));

    if (event == 0) {

        // The flag is not set; create event.
        Semaphore localSemaphore;

        event = flag->testAndSwap(0,&localSemaphore);
        if (0 == event ) {
            // The event handle has been stored in the flag;
            // wait on it now.
            localSemaphore.wait();
            return;
        }
    }
    ASSERT(event == dummySemaphorePtr);
}

void *testCase4_fn3(int threadNum, const MyTask& task)
{
    DataCase4 *data = reinterpret_cast<DataCase4 *> (task.arg());

    for (int i=0; i < 1000; ++i) {

        setFlag(&data->d_event1);

        waitOnFlag(&data->d_event2, 100);
        ASSERT (data->d_event2 != 0);

        data->d_event2 = 0;
        ASSERT (data->d_event2 == 0);

    }

    return 0;
}

void *testCase4_fn4(int threadNum, const MyTask& task)
{
    DataCase4 *data = reinterpret_cast<DataCase4 *> (task.arg());

    for (int i=0; i < 1000; ++i) {

        waitOnFlag(&data->d_event1, 100);
        ASSERT (data->d_event1 != 0);

        data->d_event1 = 0;
        ASSERT (data->d_event1 == 0);

        setFlag(&data->d_event2);
    }

    return 0;
}

void *testCase4_fn1(int threadNum, const MyTask& task)
{
    DataCase4 *data = reinterpret_cast<DataCase4 *> (task.arg());

    for (int i=0; i < 1000; i++) {

        ASSERT (data->d_flagEnd == i);
        ASSERT (data->d_flagStart == i);

        data->d_event1.swap(0);
        data->d_event2.swap(0);

        // open gate for the second thread
        ASSERT(data->d_flagStart.testAndSwap(i, i+1) == i);
        ASSERT(data->d_flagStart == (i+1));

        setFlag(&data->d_event1);
        waitOnFlag(&data->d_event2, 100);

        // barrier wait till other thread set next value
        while ((i+1) != data->d_flagEnd) {
           bcemt_ThreadUtil::yield();
        }
    }

    return 0;
}

void *testCase4_fn2(int threadNum, const MyTask& task)
{
    DataCase4 *data = reinterpret_cast<DataCase4 *> (task.arg());

    for (int i=0; i < 1000; i++) {

        ASSERT (data->d_flagEnd == i);

        // barrier wait till other thread set next value

        while ((i+1) != data->d_flagStart) {
           bcemt_ThreadUtil::yield();
        }

        setFlag(&data->d_event2);
        waitOnFlag(&data->d_event1, 100);

        // make sure the first thread is waiting for us
        ASSERT(data->d_flagStart == (i+1));

        // open gate for the first thread
        ASSERT(data->d_flagEnd.testAndSwap(i, i+1) == i);
        ASSERT(data->d_flagEnd == (i+1));
    }

    return 0;
}

// ----------------------------------------------------------------------------
// Test Case 3. Hard Contention Test: many threads - one QLock
//
// Concerns:
//   Verify that single QLock allows only one thread at a time to
//   execute the critical region
//
// Plan:
//   Set the global count to zero value.  Create several M threads.
//   Each of them repeats N times execution of the critical region:
//   - read the global count value and store its locally
//   - put the random number into count
//   - sleep random number microseconds
//   - increment by one the original global count value and update
//     the global count with the incremented value
//   The execution of critical region is protected by global QLock.
//   Before exit each thread waits on the barrier to be synchronized with
//   other similar threads, acquires the global QLock, and checks that
//   global count has value equal to N*M, where M the number of threads
//   and N is number of iterations made by each thread.
//   Join all threads and ensure that:
//   - the value of count is still equal N*M.
//   - global QLock is immediately available, i.e tryLock() returns 0.
//
//   Repeat above scenario several times, incrementing the number of
//   threads by one on each iteration.
// ----------------------------------------------------------------------------
struct DataCase3 {
    int           d_numIter;
    int           d_count;
    bcemt_QLock  *d_qlock;
    bcemt_Mutex  *d_mutex;
};

void *testCase3(int threadNum, const MyTask& task)
{
     DataCase3 *data = reinterpret_cast<DataCase3 *> (task.arg());

     ASSERT(data->d_count == 0);

     Rand rand (threadNum);

     task.barrier()->wait();

     bcemt_QLockGuard guard;

     for (int i=0; i < data->d_numIter; ++i) {

         // Critical region
         // set mutex-qlock and lock
         guard.lock(data->d_qlock);

         int  original = data->d_count;
         int  sleepTime = rand.get() % 1000;

         // This will take too long time
         //int  sleepTime = rand.get() % 1000;
         //bcemt_ThreadUtil::microSleep(++sleepTime);

         for (int j=0; j < 20; ++j) {
             data->d_count = rand.get(); // put random value
         }

         ++original;
         data->d_count = original;   // restore incremented value

         guard.unlock();
     }

     task.barrier()->wait();
     {
        guard.lock(data->d_qlock);
        ASSERT(data->d_count == data->d_numIter*task.numThreadsStarted());
     }

     // qlock must be unlocked automatically on destructor
    return 0;
}

void *testCase3a(int threadNum, const MyTask& task)
{
     DataCase3 *data = reinterpret_cast<DataCase3 *> (task.arg());

     ASSERT(data->d_count == 0);

     Rand rand (threadNum);

     task.barrier()->wait();

     bcemt_QLockGuard guard;

     for (int i=0; i < data->d_numIter; ++i) {

         // Critical region
         // set mutex-qlock and lock
         bcemt_LockGuard<bcemt_Mutex> guard(data->d_mutex);

         int  original = data->d_count;

         // This will take too long time
         //int  sleepTime = rand.get() % 1000;
         //bcemt_ThreadUtil::microSleep(++sleepTime);

         for (int j=0; j < 20; ++j) {
             data->d_count = rand.get(); // put random value
         }

         ++original;
         data->d_count = original;   // restore incremented value
     }

     task.barrier()->wait();
     {
        bcemt_LockGuard<bcemt_Mutex> guard(data->d_mutex);
        ASSERT(data->d_count == data->d_numIter*task.numThreadsStarted());
     }

    return 0;
}

// ----------------------------------------------------------------------------
// USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
    const bsl::string& helloString()
    {
//..
// This function defines two static variables, a pointer to the singleton, and
// a QLock to control access to the singleton.  Note that both of these
// variables are statically initialized, so there is no need for a run-time
// constructor and hence no danger of a race condition among threads.  The
// need for static initialization is the main reason we choose to use
// 'bcemt_QLock' over 'bcemt_Mutex':
//..
        static const bsl::string *singletonPtr = 0;
        static bcemt_QLock qlock = BCEMT_QLOCK_INITIALIZER;
//..
// Before checking the status of the singleton pointer, we must make sure that
// we are not accessing the pointer at the same time that some other thread is
// modifying the pointer.  We do this by acquiring the lock by constructing a
// 'bcemt_QLockGuard' object:
//..
        bcemt_QLockGuard qlockGuard(&qlock);
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
        if (! singletonPtr) {
            static bsl::string singleton("Hello");
            singletonPtr = &singleton;
        }
//..
// Finally, we return a reference to the singleton.  The destructor for
// 'bcemt_QLockGuard' will automatically unlock the QLock and allow another
// thread into the critical region.
//..
        return *singletonPtr;
    }
//..
// The following test program shows how our singleton function can be called.
// Note that 'hello1' and 'hello2' have the same address, demonstrating that
// there was only one string created.
//..
    int usageExample1()
    {
        const bsl::string EXPECTED("Hello");

        const bsl::string& hello1 = helloString();
        ASSERT(hello1 == EXPECTED);

        const bsl::string& hello2 = helloString();
        ASSERT(hello2  == EXPECTED);
        ASSERT(&hello2 == &hello1);

        return 0;
    }
//..

extern "C" void *testCase2(void *)
{
    usageExample1();
    return const_cast<bsl::string*>(&helloString());
}

// ----------------------------------------------------------------------------
// Test Case 1. BREATHING Test - Thread Function
//   Check that the global count value is zero on enter,
//   wait on the barrier to synchronize competition for the lock.
//   Execute the critical region: reads the global count
//   value , increments its value, sleep several milliseconds, updates
//   the global count with incremented value.  The execution of critical
//   region is protected by global QLock.
// ----------------------------------------------------------------------------
static bcemt_QLock qMutex1  = BCEMT_QLOCK_INITIALIZER;
void *testCase1(int threadNum, const MyTask& task)
{
    int *count = (int*)(task.arg());

    ASSERT(*count == 0);

    task.barrier()->wait();

    // Critical region
    {
        bcemt_QLockGuard qlock(&qMutex1);

        int val = *count;

        ASSERT(val < task.numThreadsStarted());
        ++val;

        // Sleep 10 milliseconds
        bcemt_ThreadUtil::microSleep(1000*10);

        *count = val;
    }

    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // Testing bcemt_QLock::initialize and bcemt_QLock::isLocked
        // --------------------------------------------------------------------

        if (verbose) cout << "Initialization test"  << bsl::endl;

        bcemt_QLock qlock;
        qlock.initialize();
        ASSERT(!qlock.isLocked());

        bcemt_QLockGuard g(&qlock);
        ASSERT(qlock.isLocked());
        g.unlock();
        ASSERT(!qlock.isLocked());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Multiple-threads, 2-qlocks
        //
        // Concerns:
        //   Verify that each thread has chance to obtain the lock
        //
        // Plan:
        //   The two qlocks are completely independent.  A thread locking one
        //   QLock does not wait for a thread that holds the lock to the other.
        //   Two context structures, each with a QLock, a few threads on each
        //   context.  In the critical region, each thread sets a flag in its
        //   context, reads the other context's flag, sleeps briefly, clears
        //   its flag, exits the critical region, and loops.  When every
        //   thread's flag has been represented in the other context structure,
        //   all threads exit.  An error is reported if the maximum loop count
        //   is reached.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "Two locks Test"  << bsl::endl;

        bcemt_QLock qlock1 = BCEMT_QLOCK_INITIALIZER;
        bcemt_QLock qlock2 = BCEMT_QLOCK_INITIALIZER;

        ContextCase7 context1;
        ContextCase7 context2;

        context1.d_qlock = &qlock1;
        context1.d_owner = -1;
        context1.d_slots.clear();

        context2.d_qlock = &qlock2;
        context2.d_owner = -1;
        context2.d_slots.clear();

        DataCase7 data1 = { 0, &context1, &context2 };
        DataCase7 data2 = { 0, &context2, &context1 };

        MyTask task71(testCase7, &data1);
        MyTask task72(testCase7, &data2);

        for (int i=0; i < 10; ++i) {

            bcemt_Barrier  barrier(i*2);

            data1.d_barrier = &barrier;
            data2.d_barrier = &barrier;

            ASSERT(context1.d_owner == -1);
            ASSERT(context2.d_owner == -1);

            context1.d_slots.assign(i, -1);
            ASSERT(context1.d_slots.size()== i);

            context2.d_slots.assign(i, -1);
            ASSERT(context2.d_slots.size()== i);

            ASSERT(0 == task71.start(i));
            ASSERT(0 == task72.start(i));

            ASSERT(0 == task71.stop());
            ASSERT(0 == task72.stop());

            for (int j=0; j < i; ++j) {
                ASSERT(context1.d_slots[j] != -1);
                ASSERT(context2.d_slots[j] != -1);
            }

            bcemt_QLockGuard guard1(&qlock1, false);
            ASSERT(guard1.tryLock() == 0);

            bcemt_QLockGuard guard2(&qlock2, false);
            ASSERT(guard2.tryLock() == 0);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // Fairness Test
        //
        // Concerns:
        //   Verify that each thread obtains the lock in the order in
        //   which it requested it.
        //
        // Plan:
        //  A bunch of threads wait on a barrier, then compete for a QLock.
        //  Upon acquiring the lock, each thread sets a thread-specific flag,
        //  then sleeps very briefly, releases the lock, and loops.  If the
        //  flag has already been set (i.e., second time in for that thread),
        //  assert that all of the other flags are also set, i.e., every other
        //  thread has had a turn before any thread has a second chance.
        //  Each thread exits after its second time in the critical region.
        //
        //   Repeat above scenario several times, incrementing the number of
        //   threads by one on each iteration.
        // --------------------------------------------------------------------

        if (verbose) cout << "Fairness Test"  << bsl::endl;

        bcemt_QLock qlock = BCEMT_QLOCK_INITIALIZER;
        DataCase6   data;
        data.d_qlock = &qlock;
        data.d_slots.clear();

        MyTask task6(testCase6, &data);

        for (int i=0; i < 10; ++i) {

            data.d_slots.assign(i,0);

            ASSERT(0 == task6.start(i));
            ASSERT(0 == task6.stop());

            bcemt_QLockGuard guard(data.d_qlock, false);
            ASSERT(guard.tryLock() == 0);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // Multiple threads - multiple QLocks Test.  At the same time
        // this test can be used for performance evaluation for such
        // scenario.
        //
        // Concerns:
        //   To test behavior and performance in case of many threads
        //   and many QLocks.
        //
        // Plan:
        //  Create array of data slots.  Each element-slot is protected by
        //  its own QLock.  Create multiple threads.  Each thread in the loop
        //  obtains a lock for the random slot.  Upon acquiring the lock for
        //  the slot, the thread stores locally the slot data, manipulates
        //  them with slot data, ensures the slot data is not modified by
        //  other threads, restores the original slot data, and releases
        //  the slot lock.  Repeat above action in the loop.
        //
        //  Repeat above scenario several times, incrementing the number of
        //  threads by one on each iteration.
        // --------------------------------------------------------------------

        enum {
            MAX_SLOTS   = 1000,
            MAX_ITER    = 100000,
            MAX_THREADS = 20
        };

        {
            bsls_Stopwatch sw;
            sw.start();

            CaseData5 data;
            MyTask task51(testCase5_fn1, &data);

            for (int i=0; i < MAX_THREADS; ++i) {

                data.d_numIter = MAX_ITER;
                data.d_numElements = MAX_SLOTS;

                data.d_mutexes = 0;
                data.d_qlocks = new bcemt_QLock [MAX_SLOTS];
                data.d_slots = new int [MAX_SLOTS];

                for (int j=0; j < MAX_SLOTS; ++j) {
                    data.d_slots[j] = 0;
                    data.d_qlocks[j].initialize();
                }
                bcemt_ThreadUtil::yield();

                ASSERT(0 == task51.start(i));
                ASSERT(0 == task51.stop());
                if (veryVerbose) {
                    bsl::cout << i << bsl::endl << bsl::flush;
                }
                delete [] data.d_qlocks;
                delete [] data.d_slots;
            }

            sw.stop();
            if (verbose) printMetrics(bsl::cout, "QLock", MAX_THREADS, sw);
       }

       {
            bsls_Stopwatch sw;
            sw.start();

            CaseData5 data;
            MyTask task52(testCase5_fn2, &data);

            for (int i=0; i < MAX_THREADS; ++i) {

                data.d_numIter = MAX_ITER;
                data.d_numElements = MAX_SLOTS;

                data.d_qlocks = 0;
                data.d_mutexes = new bcemt_Mutex [MAX_SLOTS];
                data.d_slots = new int [MAX_SLOTS];

                for (int j=0; j < MAX_SLOTS; ++j) {
                    data.d_slots[j] = 0;
                }

                ASSERT(0 == task52.start(i));
                ASSERT(0 == task52.stop());

                delete [] data.d_mutexes;
                delete [] data.d_slots;
            }

            sw.stop();
            if (verbose) printMetrics(bsl::cout, "Mutex", MAX_THREADS, sw);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // Set/Wait Flags Test
        //
        // Concerns:
        //   To test internal primitives setFlags/waitOnFlag on which QLock
        //   is based.
        //
        // Plan:
        //  Create two threads and two flags.  First thread in the loop sets
        //  the first flag and waits on second flag.  The second thread sets
        //  the second flag and waits on the first.  Ensure that threads
        //  never have a deadlock.
        //  'Flag' means an atomic pointer to the synchronization object and
        //  can be in one of three states:
        //  0    nor 'setFlag' neither 'waitOnFlag' have been called yet
        //  -1   'setFlag' was called before 'waitOnFlag', so there is no
        //        need for 'waitOnFlag' to wait on synchronization object
        //  XXX  'waitOnFlag' was called before 'setFlag' and waits on
        //        synchronization object with address 'XXX'.  'setFlag' must
        //        post/signal/notify that synchronization object.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "Set-Wait Flags Test" << endl;

            bces_AtomicPointer<Semaphore> flag = 0;

            DataCase4 data;

            MyTask task41(testCase4_fn1, &data);
            MyTask task42(testCase4_fn2, &data);

            for (int i=0; i < 20; ++i) {
                data.reset();
                ASSERT(0 == task41.start(1));
                ASSERT(0 == task42.start(1));
                ASSERT(0 == task41.stop());
                ASSERT(0 == task42.stop());
            }

            MyTask task43(testCase4_fn3, &data);
            MyTask task44(testCase4_fn4, &data);

            for (int i=0; i < 20; ++i) {
                data.reset();
                ASSERT(0 == task43.start(1));
                ASSERT(0 == task44.start(1));
                ASSERT(0 == task43.stop());
                ASSERT(0 == task44.stop());
            }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Contention Test: many threads - one QLock
        //
        // Concerns:
        //   Verify that single QLock allows only one thread at a time to
        //   execute the critical region
        //
        // Plan:
        //   Set the global count to zero value.  Create several threads.
        //   Each of them repeats N times execution of the critical region:
        //   - read the global count value and store its locally
        //   - put the random number into count
        //   - sleep random number microseconds
        //   - increment by one the original global count value and update
        //     the global count with the incremented value
        //   The execution of critical region is protected by global QLock.
        //   Before exit each thread waits on the barrier to be synchronized
        //   with other similar threads, acquires the global QLock, and checks
        //   that global count has value equal to N*M, where M the number of
        //   threads and N is number of iterations made by each thread.
        //   Join all threads and ensure that:
        //   - the value of count is still equal N*M.
        //   - global QLock is immediately available, i.e tryLock() returns 0.
        //
        //   Repeat above scenario several times, incrementing the number of
        //   threads by one on each iteration.
        // --------------------------------------------------------------------

        if (verbose) cout << "Contention Test: many threads - one QLock"
                          << endl;

        bcemt_QLock qlock = BCEMT_QLOCK_INITIALIZER;;
        bcemt_Mutex mutex;
        DataCase3 data;

        data.d_count = 0;
        data.d_numIter = 1000;
        data.d_qlock = &qlock;
        data.d_mutex = &mutex;

        {
            bsls_Stopwatch sw;
            MyTask task3(testCase3, &data);

            sw.start();
            for (int i=0; i < 10; ++i) {

                data.d_count = 0;
                ASSERT(0 == task3.start(i));
                ASSERT(0 == task3.stop());
                ASSERT(data.d_count == i*data.d_numIter);

                bcemt_QLockGuard guard(data.d_qlock, false);
                ASSERT(guard.tryLock() == 0);
            }
            sw.stop();
            if (verbose) printMetrics(bsl::cout, "QLock", 10, sw);
       }

       // repeat the same test with bcemt_mutex
       {
            bsls_Stopwatch sw;

            MyTask task3a(testCase3a, &data);

            sw.start();
            for (int i=0; i < 10; ++i) {

                data.d_count = 0;
                ASSERT(0 == task3a.start(i));
                ASSERT(0 == task3a.stop());
                ASSERT(data.d_count == i*data.d_numIter);

                ASSERT(data.d_mutex->tryLock() == 0);
                data.d_mutex->unlock();
            }
            sw.stop();
            if (verbose) printMetrics(bsl::cout, "StdMutex", 10, sw);
       }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE: Singleton
        // Concerns:
        //   Demonstrate common technique of singleton creation based on
        //   combination of three-state atomic variable and statically
        //   initializable mutex (QLock).
        //
        //   Verify that Singleton is created only once regardless of which
        //   thread called its first.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUsage example: Singleton" << endl;

        bcemt_ThreadUtil::Handle handle = bcemt_ThreadUtil::invalidHandle();
        bcemt_ThreadUtil::create(&handle, testCase2, 0);

        const bsl::string& s1 = helloString();
        void              *s2 = 0;

        bcemt_ThreadUtil::join(handle, &s2);

        ASSERT(s2 != 0);
        ASSERT(s2 == &s1);
        ASSERT(s1 == "Hello");

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Verify that single QLock allows only one thread at a time to
        //   execute the critical region
        //
        // Plan:
        //   Set the global count to zero value.  Create several threads.
        //   Each of them executes critical region: reads the global count
        //   value, increments its value, sleep several milliseconds, updates
        //   the global count with incremented value.  The execution of
        //   critical region is protected by global QLock.  Join all threads
        //   and ensure the value of count is equal to the number of created
        //   threads.  Also ensure the global QLock is immediately available,
        //   i.e., tryLock() returns 0 after all threads are joined.  Repeat
        //   above scenario several times, incrementing the number of threads
        //   by one on each iteration.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBasic Test" << endl;

        int count1 = 0;
        MyTask task1(testCase1, &count1);

        for (int i=0; i < 10; ++i) {

            count1 = 0;
            ASSERT(0 == task1.start(i));
            ASSERT(0 == task1.stop());
            ASSERT(count1 == i);

            bcemt_QLockGuard guard(&qMutex1, false);
            ASSERT(guard.tryLock() == 0);
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
