// bdlcc_fixedqueue.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlcc_fixedqueue.h>

#include <bdlcc_queue.h>
#include <bdlcc_skiplist.h>

#include <bslim_testutil.h>

#include <bslmt_barrier.h>
#include <bslmt_condition.h>
#include <bslmt_configuration.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_qlock.h>
#include <bslmt_recursivemutex.h>
#include <bslma_testallocator.h>
#include <bslmt_timedsemaphore.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bslmt_turnstile.h>

#include <bdlf_bind.h>
#include <bdlt_currenttime.h>
#include <bdlb_random.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocatormonitor.h>
#include <bsls_stopwatch.h>
#include <bsls_timeutil.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_ctime.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>

#include <bsl_c_stdlib.h>            // 'atoi'

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   THREAD-SAFE OUTPUT AND ASSERT MACROS
// ----------------------------------------------------------------------------
typedef bslmt::LockGuard<bslmt::Mutex> LockGuard;
bslmt::Mutex coutMutex;

#define PT(X)  { LockGuard guard(&printMutex); P(X);  }
#define PT_(X) { LockGuard guard(&printMutex); P_(X); }

#define ASSERTT(X) { \
   if (!(X)) {                                                                \
       LockGuard guard(&coutMutex);                                           \
       aSsErT(1, #X, __LINE__); } }

#define LOOP_ASSERTT(I,X) { \
   if (!(X)) {                                                                \
       LockGuard guard(&coutMutex);                                           \
       cout << #I << ": " << I << "\n";                                       \
       aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERTT(I,J,X) {                                                \
   if (!(X)) { LockGuard guard(&coutMutex);                                   \
       cout << #I << ": " << I << "\t"                                        \
       << #J << ": " << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERTT(I,J,K,X) { \
   if (!(X)) { LockGuard guard(&coutMutex); cout << #I << ": " << I << "\t"   \
       << #J << ": " << J << "\t" << #K << ": " << K << "\n";                 \
       aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERTT(I,J,K,L,X) { \
   if (!(X)) { LockGuard guard(&coutMutex); cout << #I << ": " << I << "\t"   \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L            \
       << ": " << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERTT(I,J,K,L,M,X) { \
   if (!(X)) { LockGuard guard(&coutMutex); cout << #I << ": " << I << "\t"   \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L            \
       << ": " << L << "\t" << #M << ": " << M << "\n";                       \
       aSsErT(1, #X, __LINE__); } }

#define V(X) { if (verbose) P(X) }            // Print in verbose mode

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef void Element;
typedef bdlcc::FixedQueue<Element*> Obj;

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

namespace Backoff {

void hardwork(int* item, int spin)
{
    for (int i=0; i<spin; ++i) {
        bdlb::Random::generate15(item);
    }
}

void pushpopThread(bdlcc::FixedQueue<int> *queue,
                   bslmt::Barrier         *barrier,
                   int                     numIterations,
                   int                     workSpin)
{
    barrier->wait();

    for (int i = 0; i < numIterations; ++i) {

        int value;
        // work, push, work, pop
        for (int j = 0; j < 100; ++j) {
            hardwork(&value, workSpin);
            queue->pushBack(value);
        }

        for (int j = 0; j < 100; ++j) {
            hardwork(&value, workSpin);
            queue->popFront();
        }
    }
}

}  // close namespace Backoff

void rolloverPusher(bdlcc::FixedQueue<int> *queue,
                    bsls::AtomicInt        *doneFlag,
                    bslmt::Turnstile       *turnstile,
                    int                     threadId)
{
    enum {
        k_NUM_ITEMS = 50000 // number to push in this thread
    };

    const int base = 1000000 * threadId;

    for (int i = 0; i < k_NUM_ITEMS; ++i) {
        turnstile->waitTurn();

        queue->pushBack(base + i);
    }
    (*doneFlag)++;
}

void rolloverLengthChecker(bdlcc::FixedQueue<int> *queue,
                           bsls::AtomicInt        *doneFlag)
{
    const int minLength = queue->size() / 2;
    const int maxLength = queue->size();

    for (unsigned i = 1; 2 > doneFlag->loadRelaxed(); ++i) {
        int length = queue->length();

        LOOP_ASSERTT(length, minLength <= length && maxLength >= length);
        if (0 == i % 10) {
            bslmt::ThreadUtil::yield();
        }
    }
}

void rolloverPopper(bdlcc::FixedQueue<int> *queue,
                    bsls::AtomicInt        *doneFlag,
                    bslmt::Turnstile       *turnstile)
{
    const int minLength = queue->size() / 2;

    int lastSeen1 = minLength;
    int lastSeen2 = minLength;

    while (2 > doneFlag->loadRelaxed()) {
        turnstile->waitTurn();

        if (queue->length() > minLength + 1) {
            int value = queue->popFront();

            if (value >= 2000000) {
                ASSERTT(value > lastSeen2);
                lastSeen2 = value;
            }
            else if (value >= 1000000) {
                ASSERTT(value > lastSeen1);
                lastSeen1 = value;
            }
            else {
                ASSERTT(value >= minLength && value < queue->size());
            }
        }
    }
    ASSERTT(lastSeen1 > minLength);
    ASSERTT(lastSeen2 > minLength);
}

void pushpopThread(bdlcc::FixedQueue<int> *queue, bsls::AtomicInt *stop)
{
    while (!stop->loadRelaxed()) {
        queue->pushBack(1);
        queue->popFront();

        queue->pushBack(1);
        queue->popFront();

        queue->pushBack(1);
        queue->popFront();
    }
}

class ExceptionTester
{
public:
    static bsls::AtomicInt64 s_throwFrom;

    ExceptionTester() {}

    ExceptionTester(const ExceptionTester&) {
        if (s_throwFrom &&
            bslmt::ThreadUtil::selfIdAsUint64() ==
            static_cast<bsls::Types::Uint64>(s_throwFrom.load())) {
            throw 1;
        }
    }

    ExceptionTester& operator=(const ExceptionTester&) {
        if (s_throwFrom &&
            bslmt::ThreadUtil::selfIdAsUint64() ==
            static_cast<bsls::Types::Uint64>(s_throwFrom.load())) {

            throw 1;
        }
        return *this;
    }
};

bsls::AtomicInt64 ExceptionTester::s_throwFrom(0);

void exceptionProducer(bdlcc::FixedQueue<ExceptionTester> *tester,
                       bslmt::TimedSemaphore              *semaphore,
                       bsls::AtomicInt                    *numCaught) {
    enum { k_NUM_ITERATIONS = 3 };

    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        try {
            tester->pushBack(ExceptionTester());
        } catch (...) {
            ++(*numCaught);
        }
        semaphore->post();
    }
}

class TestType
{
    int *d_arg_p;

public:

    TestType(int *arg1) : d_arg_p(arg1)
    {
        ++(*d_arg_p);
    }

    TestType(const TestType& rhs) : d_arg_p(rhs.d_arg_p)
    {
        ++(*d_arg_p);
    }

    TestType& operator=(const TestType& rhs) {
        --(*d_arg_p);
        d_arg_p = rhs.d_arg_p;
        ++(*d_arg_p);
        return *this;
    }

    ~TestType() {
        --(*d_arg_p);
    }
};

struct ThreadArgs {
    bslmt::Condition  d_startCond;
    bslmt::Condition  d_goCond;
    bslmt::Mutex      d_mutex;
    Obj              d_queue;
    volatile int     d_iterations;
    volatile int     d_count;
    volatile int     d_startSig;
    volatile int     d_goSig;
    volatile int     d_stopSig;
    char            *d_reserved;

    ThreadArgs(int iterations, int size=20*1000);

    ~ThreadArgs()
    { delete [] d_reserved; }
};

ThreadArgs::ThreadArgs(int iterations, int size)
: d_queue(size)
, d_iterations(iterations)
, d_count(0)
, d_startSig(0)
, d_goSig(0)
, d_stopSig(0)
{
    d_reserved = new char[iterations+2];
}

extern "C" {

void* pushBackTestThread(void *ptr)
    // This function is used to simulate a thread pool job.  It accepts a
    // pointer to a pointer to a structure containing a mutex and a conditional
    // variable.  The function simply blocks until the conditional variable is
    // signaled.
{
    ThreadArgs *args = (ThreadArgs*)ptr;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&args->d_mutex);
        ++args->d_count;
        args->d_startCond.signal();
        while ( !args->d_goSig ) args->d_goCond.wait(&args->d_mutex);
    }

    char* reserved = args->d_reserved;
    for (int i=1; i<args->d_iterations+1; ++i) {
        if (veryVerbose && i%10000 == 0) {
            LockGuard guard(&coutMutex);
            cout << "Thread " << bslmt::ThreadUtil::selfIdAsInt()
                 << " pushing i=" << i << endl;
        }

        args->d_queue.pushBack((Element*)(reserved + i));
    }

    if (veryVerbose) {
        LockGuard guard(&coutMutex);
        cout << "Thread " << bslmt::ThreadUtil::selfIdAsInt()
             << "done, pushing sentinel value..." << endl;
    }

    args->d_queue.pushBack((Element*)(0xFFFFFFFF));

    return ptr;
}

void* popFrontTestThread(void *ptr)
    // This function is used to simulate a thread pool job.  It accepts a
    // pointer to a pointer to a structure containing a mutex and a conditional
    // variable.  The function simply blocks until the conditional variable is
    // signaled.
{
    ThreadArgs *args = (ThreadArgs*)ptr;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&args->d_mutex);
        ++args->d_count;
        args->d_startCond.signal();
        while ( !args->d_goSig ) args->d_goCond.wait(&args->d_mutex);
    }
    while (1) {
        Element *e = args->d_queue.popFront();
        if ((Element*)0xffffffff == e) break;
    }
    return ptr;
}

}

void case9pusher(bdlcc::FixedQueue<int> *queue, volatile bool *done)
{
    while (!*done) {
        queue->pushBack(static_cast<int>(bslmt::ThreadUtil::selfIdAsInt()));
    }
}

void case9drainer(bdlcc::FixedQueue<int> *queue,
                  bslmt::Barrier         *drainDoneBarrier)
{
    int result;
    while (0 == queue->tryPopFront(&result)) ;
    ASSERT(queue->isEmpty());
    ASSERT(queue->isEmpty());
    drainDoneBarrier->wait();
}

void case9disabler(bdlcc::FixedQueue<int> *queue,
                   bslmt::Barrier         *drainDoneBarrier,
                   bslmt::Barrier         *reEnableBarrier)
{
    queue->disable();

    enum {
        k_NUM_PUSH_TRIES=5000
    };

    for (int i = 0; i < k_NUM_PUSH_TRIES; ++i) {
        ASSERT(0 != queue->pushBack(i));
    }
    drainDoneBarrier->wait();
    queue->enable();
    queue->disable();
    queue->enable();
    reEnableBarrier->wait();
    ASSERT(queue->isEnabled());
    ASSERT(0 == queue->tryPushBack(1));
    ASSERT(0 == queue->pushBack(2));
}

struct StressNode {
    int thread;
    int value;
};

struct StressData {
    bslmt::ThreadUtil::Handle            handle;
    bdlcc::FixedQueue<StressNode>        *queue;
    int                                *counts;
    int                                *checksums;
    int                                 maxCount;
    int                                 thread;
    bool                               *stopProd;
};

extern "C" void *stressConsumer1(void* arg) {
    StressData *data = (StressData*)arg;
    while (true) {
        StressNode sn = data->queue->popFront();
        if (sn.thread<0) return 0;                                    // RETURN
        ++data->counts[sn.thread];
        data->checksums[sn.thread] += sn.value;
    }
}

extern "C" void *stressConsumer2(void* arg) {
    StressData *data = (StressData*)arg;
    while (true) {
        StressNode sn;
        if (!data->queue->tryPopFront(&sn)) {
            if (sn.thread<0) return 0;                                // RETURN
            ++data->counts[sn.thread];
            data->checksums[sn.thread] += sn.value;
        }
    }
}

int stressrand() {
    static int v = 0;
    return v = (v*1664525 + 1013904223)&0xFFFFFFFF;
}

extern "C" void *stressProducer1(void* arg) {
    StressData *data = (StressData*)arg;
    int maxCount = data->maxCount;
    for (int i=0; i<maxCount; ++i) {
        if (*data->stopProd) return 0;                                // RETURN
        StressNode sn;
        sn.thread = data->thread;
        sn.value = stressrand();
        ++data->counts[0];
        data->checksums[0] += sn.value;
        data->queue->pushBack(sn);
    }
    return 0;
}

extern "C" void* stressProducer2(void* arg) {
    StressData *data = (StressData*)arg;
    int maxCount = data->maxCount;
    for (int i=0; i<maxCount; ++i) {
        if (*data->stopProd) return 0;                                // RETURN
        StressNode sn;
        sn.thread = data->thread;
        sn.value = stressrand();
        if (!data->queue->tryPushBack(sn)) {
            ++data->counts[0];
            data->checksums[0] += sn.value;
        }
    }
    return 0;
}

struct BenchData {
    bslmt::ThreadUtil::Handle    handle;
    bdlcc::FixedQueue<void*>     *queue;
    int                         count;
    bool                        stop;
};

extern "C" void *benchConsumer(void* arg) {
    BenchData *data = (BenchData*)arg;
    while (true) {
        data->queue->popFront();
        if (data->stop) return 0;                                     // RETURN
        ++data->count;
    }
}

extern "C" void *benchProducer(void* arg) {
    BenchData *data = (BenchData*)arg;
    int reserved;
    while (true) {
        data->queue->pushBack((void*)&reserved);
        if (data->stop) return 0;                                     // RETURN
        ++data->count;
    }
    return 0;
}

static const char* fmt(int n) {
    int f; char c=0;
    if (n>=1000) { f=n/10; n/=1000; f-=n*100; c='K'; }
    if (n>=1000) { f=n/10; n/=1000; f-=n*100; c='M'; }
    if (n>=1000) { f=n/10; n/=1000; f-=n*100; c='G'; }
    static char buf[0x100];
    if (c) {
        sprintf(buf, "%d.%02d%c", n, f, c);
    } else {
        sprintf(buf, "%d", n);
    }
    return buf;
}

void test9PushBack(bdlcc::FixedQueue<int> *queue,
                   double                  rate,
                   int                     threshold,
                   bool                   *stop,
                   bool                   *thresholdExceeded,
                   bslmt::Condition       *thresholdExceededCondition)
{
    bslmt::Turnstile turnstile(rate);

    while (!*stop) {
        turnstile.waitTurn();
        bslmt::ThreadUtil::microSleep(10 * 1000);
        queue->pushBack(0);

        if (queue->length() >= threshold) {
            *thresholdExceeded = true;
            thresholdExceededCondition->signal();
        }
    }
}

void sleepAndWait(int numMicroseconds, bslmt::Barrier *barrier)
{
    bslmt::ThreadUtil::microSleep(numMicroseconds);
    barrier->wait();
}

void abaThread(char                     *firstValue,
               char                     *lastValue,
               bdlcc::FixedQueue<char*> *queue,
               bslmt::Barrier           *barrier,
               bool                      sendSentinel)
{
    barrier->wait();
    for (char* value = firstValue; value <= lastValue; ++value) {
        queue->pushBack(value);
    }
    if (sendSentinel) {
        if (veryVerbose) {
            LockGuard guard(&coutMutex);
            cout << "Thread " << bslmt::ThreadUtil::selfIdAsInt()
                 << " done, pushing sentinel value" << endl;
        }
        queue->pushBack((char*)0xffffffff);
    }
}

namespace nonblocktst {

struct Item {
    int                   d_threadId;
    int                   d_sequenceNum;
    bsl::function<void()> d_funct;
};

struct Control {
    bslmt::Barrier          *d_barrier;

    bdlcc::FixedQueue<Item> *d_queue;

    int                      d_numExpectedThreads;
    int                      d_iterations;
    int                      d_queueSize;
    int                      d_pushCount;

    bsls::AtomicInt          d_numThreads;

    bsls::AtomicInt          d_numPushed;
    bsls::AtomicInt          d_numPopped;
};

void f(const bsl::shared_ptr<int>&)
{
}

void workerThread(Control *control)
{
    bsl::shared_ptr<int> sp;
    sp.createInplace(bslma::Default::allocator(), 12345);

    bsl::function<void()> funct = bdlf::BindUtil::bind(&f, sp);

    bsl::vector<int> seq(control->d_numExpectedThreads, -1);

    bdlcc::FixedQueue<Item> *queue = control->d_queue;

    int iterations = control->d_iterations;
    int pushCount = control->d_pushCount;

    bsls::AtomicInt& numPushed(control->d_numPushed);
    bsls::AtomicInt& numPopped(control->d_numPopped);

    int threadId = control->d_numThreads++;

    control->d_barrier->wait();

    int i = numPushed++;

    Item push;
    push.d_threadId = threadId;
    push.d_sequenceNum = i;

    Item pop;

    while (i<iterations) {
        for (int p=0; p<pushCount && i<iterations; p++) {
            if (queue->tryPushBack(push)) break;
            i = numPushed++;
            push.d_threadId = threadId;
            push.d_funct = funct;
            push.d_sequenceNum = i;
        }
        while (queue->tryPopFront(&pop) == 0) {
            ASSERT(seq[pop.d_threadId] < pop.d_sequenceNum);
            seq[pop.d_threadId] = pop.d_sequenceNum;
            numPopped++;
        }
    }
}

void runtest(int numIterations, int numThreads, int queueSize, int pushCount)
{
    bdlcc::FixedQueue<Item> queue(queueSize);

    ASSERT(queue.isEnabled());

    bslmt::Barrier barrier(numThreads);

    Control control;

    control.d_numExpectedThreads = numThreads;
    control.d_iterations = numIterations;
    control.d_pushCount = pushCount;

    control.d_queue = &queue;

    control.d_numThreads = 0;
    control.d_numPushed = 0;
    control.d_numPopped = 0;

    control.d_barrier = &barrier;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&workerThread,&control),
            numThreads);

    tg.joinAll();
    ASSERT(queue.isEmpty());
    ASSERT(control.d_numPushed >= numIterations);
    ASSERT(control.d_numPopped == numIterations);
}
}  // close namespace nonblocktst

namespace disabletst {

struct Control {
    bslmt::Barrier         *d_barrier;
    bdlcc::FixedQueue<int>  *d_queue;
};

void pusherThread(Control *control)
{
    bdlcc::FixedQueue<int> *queue = control->d_queue;
    control->d_barrier->wait();

    while (queue->pushBack(123) == 0) {}
}

void runtest(int numPushers, int queueSize, bool doDrain, bool doSleep = false)
{
    bdlcc::FixedQueue<int> queue(queueSize);

    ASSERT(queueSize == queue.size());
    ASSERT(queue.isEnabled());

    bslmt::Barrier barrier(numPushers);

    Control control;

    control.d_queue = &queue;
    control.d_barrier = &barrier;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&pusherThread,&control),
            numPushers);

    while (!queue.isFull()) {
        bslmt::ThreadUtil::yield();
    }

    ASSERT(queue.isEnabled());
    ASSERT(queue.isFull());

    if (doSleep) {
        // Sleep for 5ms to allow other threads to block on the
        // implementation's semaphore.

        bslmt::ThreadUtil::microSleep(5000);
    }

    queue.disable();
    ASSERT(!queue.isEnabled());

    tg.joinAll();

    if (doDrain) {
        int result;
        while (0 == queue.tryPopFront(&result)) {
            ASSERT(123 == result);
        }
        ASSERT(queue.isEmpty());
        ASSERT(!queue.isFull());
    }
    else {
        ASSERT(!queue.isEmpty());
        ASSERT(queue.isFull());
    }
    ASSERT(!queue.isEnabled());
}
}  // close namespace disabletst

namespace seqtst {

struct Item {
    int  d_threadId;
    int  d_sequenceNum;
    bsl::function<void()> d_funct;
};

struct Control {
    bslmt::Barrier         *d_barrier;

    bdlcc::FixedQueue<Item> *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bsls::AtomicInt         d_numPushers;
    bsls::AtomicInt         d_numPopped;
};

void f(const bsl::shared_ptr<int>&)
{
}

void pusherThread(Control *control)
{
    bsl::shared_ptr<int> sp;
    sp.createInplace(bslma::Default::allocator(), 12345);

    bsl::function<void()> funct = bdlf::BindUtil::bind(&f, sp);

    bdlcc::FixedQueue<Item> *queue = control->d_queue;

    int threadId = control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        Item item;
        item.d_threadId = threadId;
        item.d_sequenceNum = i;
        item.d_funct = funct;
        queue->pushBack(item);
    }
}

void popperThread(Control *control)
{
    bsl::vector<int> seq(control->d_numExpectedPushers, -1);

    bdlcc::FixedQueue<Item> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while ((numPopped = control->d_numPopped++) < totalToPop) {

        Item item = queue->popFront();

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        k_QUEUE_SIZE = 2047
    };

    bdlcc::FixedQueue<Item> queue(k_QUEUE_SIZE);

    ASSERT(k_QUEUE_SIZE == queue.size());
    ASSERT(queue.isEnabled());

    bslmt::Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdlf::BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
    ASSERT(queue.isEmpty());
}
}  // close namespace seqtst

namespace zerotst {

struct Control {
    bslmt::Barrier           *d_barrier;

    bdlcc::FixedQueue<void *> *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bsls::AtomicInt         d_numPushers;
    bsls::AtomicInt         d_numPopped;
};

void pusherThread(Control *control)
{
    bdlcc::FixedQueue<void *> *queue = control->d_queue;

    control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        queue->pushBack((void *)0);
    }
}

void popperThread(Control *control)
{
    bdlcc::FixedQueue<void *> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while ((numPopped = control->d_numPopped++) < totalToPop) {

        queue->popFront();
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        k_QUEUE_SIZE = 2047
    };

    bdlcc::FixedQueue<void *> queue(k_QUEUE_SIZE);

    ASSERT(k_QUEUE_SIZE == queue.size());
    ASSERT(queue.isEnabled());

    bslmt::Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdlf::BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
    ASSERT(queue.isEmpty());
}
}  // close namespace zerotst

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Thread Pool
///- - - - - - - - - - - - - - - -
// In the following example a 'bdlcc::FixedQueue' is used to communicate
// between a single "producer" thread and multiple "consumer" threads.  The
// "producer" will push work requests onto the queue, and each "consumer" will
// iteratively take a work request from the queue and service the request.
// This example shows a partial, simplified implementation of the
// 'bdlmt::FixedThreadPool' class.  See component 'bdlmt_fixedthreadpool' for
// more information.
//
// First, we define a utility classes that handles a simple "work item":
//..
    struct my_WorkData {
        // Work data...
    };

    struct my_WorkRequest {
        enum RequestType {
            e_WORK = 1,
            e_STOP = 2
        };

        RequestType d_type;
        my_WorkData d_data;
        // Work data...
    };
//..
// Next, we provide a simple function to service an individual work item.  The
// details are unimportant for this example:
//..
    void myDoWork(my_WorkData& data)
    {
        // do some stuff...
        (void)data;
    }
//..
// Then, we define a 'myConsumer' function that will pop elements off the queue
// and process them.  Note that the call to 'queue->popFront()' will block
// until there is an element available on the queue.  This function will be
// executed in multiple threads, so that each thread waits in
// 'queue->popFront()', and 'bdlcc::FixedQueue' guarantees that each thread
// gets a unique element from the queue:
//..
    void myConsumer(bdlcc::FixedQueue<my_WorkRequest> *queue)
    {
        while (1) {
            // 'popFront()' will wait for a 'my_WorkRequest' until available.

            my_WorkRequest item = queue->popFront();
            if (item.d_type == my_WorkRequest::e_STOP) { break; }
            myDoWork(item.d_data);
        }
    }
//..
// Finally, we define a 'myProducer' function that serves multiple roles: it
// creates the 'bdlcc::FixedQueue', starts the consumer threads, and then
// produces and enqueues work items.  When work requests are exhausted, this
// function enqueues one 'e_STOP' item for each consumer queue.  This 'e_STOP'
// item indicates to the consumer thread to terminate its thread-handling
// function.
//
// Note that, although the producer cannot control which thread 'pop's a
// particular work item, it can rely on the knowledge that each consumer thread
// will read a single 'e_STOP' item and then terminate.
//..
    void myProducer(int numThreads)
    {
        enum {
            k_MAX_QUEUE_LENGTH = 100,
            k_NUM_WORK_ITEMS   = 1000
        };

        bdlcc::FixedQueue<my_WorkRequest> queue(k_MAX_QUEUE_LENGTH);

        bslmt::ThreadGroup consumerThreads;
        consumerThreads.addThreads(bdlf::BindUtil::bind(&myConsumer, &queue),
                                   numThreads);

        for (int i = 0; i < k_NUM_WORK_ITEMS; ++i) {
            my_WorkRequest item;
            item.d_type = my_WorkRequest::e_WORK;
            item.d_data = my_WorkData(); // some stuff to do
            queue.pushBack(item);
        }

        for (int i = 0; i < numThreads; ++i) {
            my_WorkRequest item;
            item.d_type = my_WorkRequest::e_STOP;
            queue.pushBack(item);
        }
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslmt::Configuration::setDefaultThreadStackSize(
                    bslmt::Configuration::recommendedDefaultThreadStackSize());

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
        // ---------------------------------------------------------
        // Usage example test
        //
        // Test that the usage example compiles and runs as provided.
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "Usage example test" << endl
                          << "==================" << endl;

        enum { k_NUM_THREADS = 4 };
        myProducer(k_NUM_THREADS);
        break;
      }

      case 17: {
#ifdef BDE_BUILD_TARGET_EXC
        // ---------------------------------------------------------
        // Exception safety test
        //
        // Test that the queue provides the Basic exception safety guarantee.
        // After an exception on pushBack, the queue is emptied.  After an
        // exception on popFront, the object is removed and the queue behaves
        // normally.
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "Basic exception guarantee test" << endl
                          << "==============================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) {
            cout << endl
                 << "Testing exception safety queue of size 1" << endl;
        }
        {
            bdlcc::FixedQueue<ExceptionTester> queue(1, &ta);

            ExceptionTester::s_throwFrom = static_cast<bsls::Types::Int64>(
                                          bslmt::ThreadUtil::selfIdAsUint64());

            bool caught = false;
            try {
                queue.pushBack(ExceptionTester());
            }
            catch (...) {
                caught = true;
            }
            ASSERT(caught);

            ExceptionTester::s_throwFrom = static_cast<bsls::Types::Int64>(0);

        }

        if (verbose) {
            cout << endl
                 << "Testing exception safety for mutiple threads" << endl;
        }
        {
            // b. popping from a queue with exception operates normally.

            enum { k_QUEUE_LENGTH = 3 };
            bdlcc::FixedQueue<ExceptionTester> queue(k_QUEUE_LENGTH,
                                                         &ta);
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 != queue.tryPushBack(ExceptionTester()));

            bslmt::TimedSemaphore sema;
            bsls::AtomicInt numCaught(0);

            bslmt::ThreadUtil::Handle producer;
            int rc = bslmt::ThreadUtil::create(&producer,
                                              bdlf::BindUtil::bind(
                                                         &exceptionProducer,
                                                         &queue, &sema,
                                                         &numCaught));
            BSLS_ASSERT_OPT(0 == rc); // test invariant

            ExceptionTester::s_throwFrom = static_cast<bsls::Types::Int64>(
                                          bslmt::ThreadUtil::selfIdAsUint64());

            if (veryVerbose) {
                cout << endl
                     << "Exception during assignment" << endl;
            }

            bool caught = false;
            try {
                ExceptionTester test;
                queue.popFront(&test);
            } catch (...) {
                caught = true;
            }
            ASSERT(caught);

            // Now the queue should become non-full and the pusher should be
            // woken up and able to push into it
            ASSERT(0 ==
                   sema.timedWait(bdlt::CurrentTime::now().addSeconds(1)));

            ASSERT(k_QUEUE_LENGTH == queue.length());

            if (veryVerbose) {
                cout << endl
                     << "Exception during copy constructor (pop)" << endl;
            }

            caught = false;
            try {
                queue.popFront();
            } catch (...) {
                caught = true;
            }
            ASSERT(caught);

            // Now the queue should become non-full and the pusher should be
            // woken up and able to push into it
            ASSERT(0 ==
                   sema.timedWait(bdlt::CurrentTime::now().addSeconds(1)));

            ASSERT(k_QUEUE_LENGTH == queue.length());

            if (veryVerbose) {
                cout << endl
                     << "Exception during copy constructor (push)" << endl;
            }

            // b. pushing into a queue with exception empties the queue.
            ExceptionTester::s_throwFrom = bslmt::ThreadUtil::idAsUint64(
                                      bslmt::ThreadUtil::handleToId(producer));

            // pop an item to unblock the pusher
            ExceptionTester test = queue.popFront();
            ASSERT(0 ==
                   sema.timedWait(bdlt::CurrentTime::now().addSeconds(1)));
            LOOP_ASSERT(numCaught, 1 == numCaught);

            LOOP_ASSERT(queue.length(), 0 == queue.length());
            ASSERT(!queue.isFull());

            // c. after a push exception, the queue is still functional. run
            // two generations through it.
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 != queue.tryPushBack(ExceptionTester()));
            ASSERT(0 == queue.tryPopFront(&test));
            ASSERT(0 == queue.tryPopFront(&test));
            ASSERT(0 == queue.tryPopFront(&test));
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 == queue.pushBack(ExceptionTester()));
            ASSERT(0 != queue.tryPushBack(ExceptionTester()));
            ASSERT(0 == queue.tryPopFront(&test));
            ASSERT(0 == queue.tryPopFront(&test));
            ASSERT(0 == queue.tryPopFront(&test));
            ASSERT(queue.isEmpty());

            bslmt::ThreadUtil::join(producer);
        }
        ASSERT(0 == ta.numBytesInUse());
#endif //  BDE_BUILD_TARGET_EXC
      } break;
      case 16: {
        // ---------------------------------------------------------
        // Template Requirements Test
        //
        // bdlcc::FixedQueue<T> should work for types T that have no default
        // constructor and a 1-arg copy constructor.
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "Template Requirements Test" << endl
                          << "==========================" << endl;

        bdlcc::FixedQueue<TestType> q(10);

        int count = 0;
        {
            TestType t(&count);
            q.pushBack(t);

            TestType t2 = q.popFront();
        }
        ASSERT(0 == count);
        {
            TestType t(&count);
            q.pushBack(t);

            TestType t2(&count);
            q.popFront(&t2);
        }
        ASSERT(0 == count);
      } break;
      case 15: {
        // ---------------------------------------------------------
        // length() stress-test
        //
        // plan: in N threads, push one element and pop one element in a tight
        // loop.  In the main thread, invoke length() repeatedly; the real
        // length of the queue will always be between 0 and N, so verify that
        // the reported length is always in this range.
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "'length()' Stress Test" << endl
                          << "======================" << endl;

        enum {
            k_NUM_PUSHPOP_THREADS = 6,
            k_TEST_DURATION = 3 // in seconds
        };

        bdlcc::FixedQueue<int> queue (k_NUM_PUSHPOP_THREADS*2);

        bsls::AtomicInt stop(0);

        bslmt::ThreadGroup tg;
        tg.addThreads(bdlf::BindUtil::bind(&pushpopThread,
                                          &queue,
                                          &stop),
                      k_NUM_PUSHPOP_THREADS);
        bsls::Stopwatch timer;
        timer.start();

        while (timer.elapsedTime() < k_TEST_DURATION) {
            int length;

            length = queue.length();
            LOOP_ASSERT(length,
                        0 <= length && length <= k_NUM_PUSHPOP_THREADS);

            length = queue.length();
            LOOP_ASSERT(length,
                        0 <= length && length <= k_NUM_PUSHPOP_THREADS);

            length = queue.length();
            LOOP_ASSERT(length,
                        0 <= length && length <= k_NUM_PUSHPOP_THREADS);

            length = queue.length();
            LOOP_ASSERT(length,
                        0 <= length && length <= k_NUM_PUSHPOP_THREADS);

            length = queue.length();
            LOOP_ASSERT(length,
                        0 <= length && length <= k_NUM_PUSHPOP_THREADS);
        }
        stop.storeRelaxed(1);
        tg.joinAll();
      } break;

      case 14: {
        // ---------------------------------------------------------
        // Disable while pushing into a full queue
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "Disable while pushing into a full queue" << endl
                          << "=======================================" << endl;
        enum {
            k_NUM_THREADS = 4
        };

        for (int numPushers=1; numPushers<=k_NUM_THREADS; numPushers++) {
            disabletst::runtest(numPushers, 8, false);   // queue size, doDrain
            disabletst::runtest(numPushers, 8, true);    // queue size, doDrain
            disabletst::runtest(numPushers, 2047, false);
                                                         // queue size, doDrain
            disabletst::runtest(numPushers, 2047, true); // queue size, doDrain
        }

        // Additional verification that threads are blocked on the
        // implementation's semaphore when 'disable' is invoked (and that they
        // are immediately released).

        disabletst::runtest(k_NUM_THREADS, 8, true, true);

      } break;
      case 13: {
        // ---------------------------------------------------------
        // TESTING sequence constraints
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "sequence constraint test" << endl
                          << "========================" << endl;
        enum {
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 50000
        };

        for (int numPushers=1; numPushers<=k_NUM_THREADS; numPushers++) {
        for (int numPoppers=1; numPoppers<=k_NUM_THREADS; numPoppers++) {

            zerotst::runtest(k_NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;
      case 12: {
        // ---------------------------------------------------------
        // TESTING sequence constraints
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "sequence constraint test" << endl
                          << "========================" << endl;
        enum {
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 50000
        };

        for (int numPushers=1; numPushers<=k_NUM_THREADS; numPushers++) {
        for (int numPoppers=1; numPoppers<=k_NUM_THREADS; numPoppers++) {

            seqtst::runtest(k_NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;
      case 11: {
        // ---------------------------------------------------------
        // TESTING CONCERN: Verify the size calculations performed in the
        // constructor.
        // ---------------------------------------------------------

        {
            bdlcc::FixedQueue<int> mX(100);
            V(mX.size());
            ASSERT(mX.size() == 100);
        }
        {
            bdlcc::FixedQueue<int> mX(0x7FFF);
            V(mX.size());
            ASSERT(mX.size() == (0x7FFF));
        }
        {
            bdlcc::FixedQueue<int> mX(2070);
            V(mX.size());
            ASSERT(mX.size() == 2070);
        }
      } break;

      case 10: {
        // ---------------------------------------------------------
        // TESTING CONCERN: Memory leak if pushing while disabled
        //
        // Plan: In a single thread, push an item while disabled and assert
        // that memory allocated does not increase.
        //
        // ---------------------------------------------------------

        bsl::vector<int> a; const bsl::vector<int> &A = a;
        a.resize(1);

        bslma::TestAllocator oa(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

        {

            bdlcc::FixedQueue<bsl::vector<int> > mX(100, &oa);
            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 != oa.numBytesInUse());

            {
                bslma::TestAllocatorMonitor oam(&oa), dam(&da);

                ASSERT(0 == mX.pushBack(A));

                ASSERT(oam.isInUseUp());
                ASSERT(dam.isInUseSame());
            }

            {
                bslma::TestAllocatorMonitor oam(&oa), dam(&da);

                mX.popFront();

                ASSERT(oam.isInUseDown());
                ASSERT(dam.isInUseSame());
            }

            mX.disable();
            ASSERT(0 == da.numBytesInUse());
            {
                bslma::TestAllocatorMonitor oam(&oa), dam(&da);

                ASSERT(0 != mX.pushBack(A));
                ASSERT(oam.isInUseSame());
                ASSERT(dam.isInUseSame());

            }

            mX.enable();
            {
                bslma::TestAllocatorMonitor oam(&oa), dam(&da);

                ASSERT(0 == mX.pushBack(A));
                ASSERT(oam.isInUseUp());
                ASSERT(dam.isInUseSame());
            }

        }
        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 == oa.numBytesInUse());

      } break;
      case 9: {
        // ---------------------------------------------------------
        // TESTING enable/disable
        //
        // Plan:
        //   * Create a queue.  Test that isEnabled() is true and an
        //     item may be pushed onto the queue.
        //   * Disable the queue.  Ensure that pushBack and tryPushBack
        //     fail.  (Single-threaded).
        //   * Re-enable the queue.  Ensure that pushBack and tryPushBack
        //     succeed.  (Single-threaded).
        //   * Fill up the queue.  In two threads, disable and then
        //     repeatedly invoke pushBack; in another thread, drain the queue.
        //     Ensure that no pushBack attempts are successful; that
        //     the queue may then be re-enabled and disabled and re-enabled;
        //     and that pushBack then succeeds.  Repeat this test many times.
        //   * In many threads, push items onto the queue irrespective of the
        //     success of pushBack.  In one thread, disable, empty the queue,
        //     and then assert that it remains empty for a short while;
        //     then re-enable and ensure that the queue begins to fill up
        //     and items may be dequeued.
        //   * Re-run the last test with fewer threads and a much larger queue
        //     so that it is *not* full at any point during the test.
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "enable/disable test" << endl
                          << "==================="   << endl;

        if (veryVerbose) cout << "...Initial queue state test" << endl;

        enum {
            k_QUEUE_SIZE_SINGLETHREAD = 4,
            k_NUM_FULL_ITERATIONS = 1200,
            k_QUEUE_SIZE_MT = 30,
            k_NUM_PUSHERS = 12,
            k_QUEUE_SIZE_LARGE = 500000,
            k_NUM_PUSHERS_MORE = 20,
            k_NUM_PUSHERS_LESS = 2,
            k_EMPTY_VERIFY_MS = 1500
        };

        bdlcc::FixedQueue<int> queue(k_QUEUE_SIZE_SINGLETHREAD);
        ASSERT(queue.isEnabled());
        ASSERT(0 == queue.tryPushBack(1));
        ASSERT(0 == queue.pushBack(2));

        if (veryVerbose) cout << "...Simple disable test" << endl;
        queue.disable();
        ASSERT(!queue.isEnabled());
        ASSERT(0 != queue.tryPushBack(1));
        ASSERT(0 != queue.pushBack(2));

        if (veryVerbose) cout << "...Simple enable test" << endl;
        queue.enable();
        ASSERT(queue.isEnabled());
        ASSERT(0 == queue.tryPushBack(1));
        ASSERT(0 == queue.pushBack(2));

        if (veryVerbose) cout << "...Full-queue disable test" << endl;

        bdlcc::FixedQueue<int> mtQueue(k_QUEUE_SIZE_MT);
        for (int i = 0; i < k_NUM_FULL_ITERATIONS; ++i) {
            if (veryVerbose && (0 == i % 100)) {
                cout << "     ...filling(" << i << ")...";
            }
            while (0 == mtQueue.tryPushBack(1)) ;
            if (veryVerbose && (0 == i % 100)) {
                cout << "filled." << endl;
            }

            bslmt::Barrier drainDoneBarrier(3);
            bslmt::Barrier reEnableBarrier(2);
            bslmt::ThreadGroup tg;
            tg.addThreads(bdlf::BindUtil::bind(&case9disabler,
                                              &mtQueue,
                                              &drainDoneBarrier,
                                              &reEnableBarrier),
                          2);
            tg.addThread(bdlf::BindUtil::bind(&case9drainer,
                                             &mtQueue,
                                             &drainDoneBarrier));
            tg.joinAll();
            LOOP_ASSERT(mtQueue.length(), 4 == mtQueue.length());
        }

        if (veryVerbose) cout << "...Disable-while-pushing test" << endl;
        {
            volatile bool done = false;
            bslmt::ThreadGroup pusherGroup;
            pusherGroup.addThreads(bdlf::BindUtil::bind(&case9pusher,
                                                       &mtQueue,
                                                       &done),
                                   k_NUM_PUSHERS);
            mtQueue.disable();
            mtQueue.removeAll();
            bsls::Stopwatch timer;
            timer.start();
            while (timer.elapsedTime() * 1000 < k_EMPTY_VERIFY_MS) {
                ASSERT(mtQueue.isEmpty());
            }
            mtQueue.enable();
            if (veryVerbose) {
                cout << "     ...waiting for queue to become nonempty..."
                     << flush;
            }
            while (mtQueue.isEmpty()) ;
            if (veryVerbose) {
                cout << "done" << endl;
            }

            // TBD: RACE!!! ASSERT(0 == mtQueue.tryPopFront(&result));
            mtQueue.popFront();
            done = true;
            mtQueue.removeAll();
            pusherGroup.joinAll();
        }

        if (veryVerbose) cout << "...Non-full-queue test (less)" << endl;
        {
            volatile bool done = false;
            bdlcc::FixedQueue<int> queue(k_QUEUE_SIZE_LARGE);

            bslmt::ThreadGroup pusherGroup;
            pusherGroup.addThreads(bdlf::BindUtil::bind(&case9pusher,
                                                       &queue,
                                                       &done),
                                   k_NUM_PUSHERS_LESS);

            queue.disable();
            if (veryVerbose) {
                cout << "     (after disabling, queue has " << queue.length()
                     << " items)" << endl;
            }

            queue.removeAll();
            bsls::Stopwatch timer;
            timer.start();
            double elapsed;
            while ((elapsed = timer.elapsedTime() * 1000)
                 < k_EMPTY_VERIFY_MS) {
                ASSERT(queue.isEmpty());
                if (!queue.isEmpty()) {
                    cout << "TEST FAILURE: disabled queue became non-empty "
                         << "after " << elapsed << " ms.  Has "
                         << queue.length() << " items." << endl;
                    break;
                }
            }
            queue.enable();
            if (veryVerbose) {
                cout << "     ...waiting for queue to become nonempty...";
            }
            while (queue.isEmpty()) ;
            if (veryVerbose) {
                cout << "done" << endl;
            }
            // TBD: RACE!!! ASSERT(0 == queue.tryPopFront(&result));
            queue.popFront();
            done = true;
            queue.disable();
            queue.removeAll();
            pusherGroup.joinAll();
        }
        if (veryVerbose) cout << "...Non-full-queue test (more)" << endl;
        {
            volatile bool done = false;
            bdlcc::FixedQueue<int> queue(k_QUEUE_SIZE_LARGE);

            bslmt::ThreadGroup pusherGroup;
            pusherGroup.addThreads(bdlf::BindUtil::bind(&case9pusher,
                                                       &queue,
                                                       &done),
                                   k_NUM_PUSHERS_MORE);

            // Sleep for up to 2 ms just to let some stuff get into the queue
            bslmt::ThreadUtil::microSleep(rand() % 2000);
            queue.disable();
            if (veryVerbose) {
                cout << "     (after disabling, queue has " << queue.length()
                     << " items)" << endl;
            }

            queue.removeAll();
            bsls::Stopwatch timer;
            timer.start();
            double elapsed;
            for (int i = 0; (elapsed = timer.elapsedTime() * 1000) <
                     k_EMPTY_VERIFY_MS; ++i) {
                ASSERT(queue.isEmpty());
                if (!queue.isEmpty()) {
                    cout << "TEST FAILURE: disabled queue became non-empty"
                         << " after " << elapsed << " ms (" << i
                         << " iters).  Has " << queue.length()
                         << " items." << endl;
                    break;
                }
            }
            queue.enable();
            if (veryVerbose) {
                cout << "     ...waiting for queue to become nonempty...";
            }
            while (queue.isEmpty()) ;
            if (veryVerbose) {
                cout << "done" << endl;
            }
            // TBD: RACE! 'isEmpty' does not guarantee that the semaphore used
            //            by 'tryPopFront' is updated.
            //ASSERT(0 == queue.tryPopFront(&result));
            queue.popFront();
            done = true;
            queue.disable();
            queue.removeAll();
            pusherGroup.joinAll();
        }

      } break;

      case 8: {
        // ---------------------------------------------------------
        // CONCERN: Generation count logic
        //
        // Run enough objects through a small enough queue that the
        // generation-count rollover logic is invoked.  The smallest number of
        // generation counts supported is 508; for a queue size of 3, this
        // implies that we need only run more than 3048 objects through the
        // queue successfully to trigger the rollover logic and loop again.
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "Generation count logic test" << endl
                          << "==========================="   << endl;

        enum {
            k_NUM_PUSHER_THREADS = 6,
            k_QUEUE_SIZE = 3,
            k_NUM_VALUES = 3060 / k_NUM_PUSHER_THREADS
        };

        enum { k_NUM_ENTRIES = k_NUM_VALUES * k_NUM_PUSHER_THREADS };
        char reserved[k_NUM_ENTRIES];

        bslma::TestAllocator ta(veryVeryVerbose);

        bslmt::Barrier barrier(k_NUM_PUSHER_THREADS+1);

        {
            bdlcc::FixedQueue<char*> mX(3, &ta);
            bslmt::ThreadGroup tg;

            char* nextValue[k_NUM_PUSHER_THREADS];
            char* lastValue[k_NUM_PUSHER_THREADS];

            for (int j = 0; j < k_NUM_PUSHER_THREADS; ++j) {
                nextValue[j] = reserved + k_NUM_VALUES*j;
                lastValue[j] = reserved + k_NUM_VALUES*(j+1) - 1;
                tg.addThread(bdlf::BindUtil::bind(&abaThread,
                                                 nextValue[j], lastValue[j],
                                                 &mX, &barrier, false));
            }
            tg.addThread(bdlf::BindUtil::bind(&sleepAndWait, 100, &barrier));

            for (int numReceived = 0; numReceived < k_NUM_ENTRIES;
                 ++numReceived) {
                char* value = mX.popFront();
                int k;
                for (k = 0; k < k_NUM_PUSHER_THREADS; ++k) {
                    if (value == nextValue[k]) {
                        nextValue[k] += (value == lastValue[k] ? 0 : 1);
                        LOOP2_ASSERT((void*)nextValue[k],
                                     (void*)lastValue[k],
                                     nextValue[k] <= lastValue[k]);
                        break;
                    }
                }
                LOOP_ASSERT(numReceived, k < k_NUM_PUSHER_THREADS);
            }
            tg.joinAll();

            ASSERT(mX.isEmpty());
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;

      case 7: {
        // ---------------------------------------------------------
        // CONCERN: ABA 'empty' PROBLEM
        //
        // If several threads attempt to push an object into the same position
        // in the queue (in this test, it will be the first position in an
        // empty queue), and there is a thread waiting to pop from that
        // position, data can be pushed into it and popped from it while
        // threads are still reading from that position.  When that happens,
        // the pusher threads can see an "empty" value in the cell and push
        // into it, even though it's not the same "empty" that it was before
        // one of the threads got data into it.
        //
        // The component solves this with a generation-count-based "empty"
        // value.  This test was written before that change was implemented,
        // and failed.
        //
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "ABA \"empty\" value test" << endl
                          << "======================"   << endl;

        enum {
            k_NUM_PUSHER_THREADS = 40,
            k_NUM_VALUES = 6,
            k_NUM_ITERATIONS = 2000
        };

        enum { k_NUM_ENTRIES = k_NUM_VALUES * k_NUM_PUSHER_THREADS };
        char reserved[k_NUM_ENTRIES];

        bslma::TestAllocator ta(veryVeryVerbose);

        for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
            bslmt::Barrier barrier(k_NUM_PUSHER_THREADS+1);

            bdlcc::FixedQueue<char*> mX(k_NUM_ENTRIES+1, &ta);
            bslmt::ThreadGroup tg;

            char* nextValue[k_NUM_PUSHER_THREADS];
            char* lastValue[k_NUM_PUSHER_THREADS];

            for (int j = 0; j < k_NUM_PUSHER_THREADS; ++j) {
                nextValue[j] = reserved + k_NUM_VALUES*j;
                lastValue[j] = reserved + k_NUM_VALUES*(j+1) - 1;
                tg.addThread(bdlf::BindUtil::bind(&abaThread,
                                                 nextValue[j], lastValue[j],
                                                 &mX, &barrier, false));
            }
            tg.addThread(bdlf::BindUtil::bind(&sleepAndWait, 100, &barrier));

            for (int numReceived = 0; numReceived < k_NUM_ENTRIES;
                 ++numReceived) {
                char* value = mX.popFront();
                int k;
                for (k = 0; k < k_NUM_PUSHER_THREADS; ++k) {
                    if (value == nextValue[k]) {
                        nextValue[k] += (value == lastValue[k] ? 0 : 1);
                        LOOP2_ASSERT((void*)nextValue[k],
                                     (void*)lastValue[k],
                                     nextValue[k] <= lastValue[k]);
                        break;
                    }
                }
                LOOP_ASSERT(numReceived, k < k_NUM_PUSHER_THREADS);
            }
            tg.joinAll();

            ASSERT(mX.isEmpty());
         }
         ASSERT(0 < ta.numAllocations());
         ASSERT(0 == ta.numBytesInUse());
      } break;

      case 6: {
        // --------------------------------------------------------------------
        // CONCERN: REMOVE_ALL
        //   We want to ensure that 'removeAll' behaves correctly for both
        //  'bdlcc::FixedQueue<T>' and its specialization
        //  'bdlcc::FixedQueue<T*>' in a single threaded environment.
        //
        // Single-Threaded Test Plan:
        //   Instantiate a queue.  Push a known number of elements on to the
        //   queue.  Verify the queue length.  Invoke 'removeAll' and verify
        //   the queue length is zero.
        //
        // Multi-Threaded Test Plan:
        //   Have several threads pushing at a known rate.  Wait until the
        //   queue reaches a certain size.  Call 'removeAll'.  The pushing
        //   threads will continue to push while the calling thread processes
        //   'removeAll'.  When the calling thread returns from 'removeAll'
        //   take a snapshot of the number of elements in the queue, and
        //   ensure there a fewer elements in the queue than before the call
        //   to 'removeAll'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "removeAll test" << endl
                          << "==================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Single thread, queue of T

        {
            enum { k_NUM_ELEMENTS = 100 };
            bdlcc::FixedQueue<int> mX(k_NUM_ELEMENTS);

            for (int i = 0; i < k_NUM_ELEMENTS; ++i) {

                ASSERT(0 == mX.pushBack(0));
            }
            ASSERT(k_NUM_ELEMENTS == mX.length());
            mX.removeAll();
            ASSERT(0 == mX.length());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Single thread, queue of T*

        {
            enum { k_NUM_ELEMENTS = 100 };
            bdlcc::FixedQueue<int*> mX(k_NUM_ELEMENTS);

            int element = 0;

            for (int i = 0; i < k_NUM_ELEMENTS; ++i) {
                ASSERT(0 == mX.pushBack(&element));
            }

            ASSERT(k_NUM_ELEMENTS == mX.length());
            mX.removeAll();
            ASSERT(0 == mX.length());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Basic Multi-threaded test
        {
            const double PUSH_RATE = 100.0; // per sec
            const int    THRESHOLD = 200;
            const int    MAX_QUEUE_SIZE = THRESHOLD;

            bdlcc::FixedQueue<int> mX(MAX_QUEUE_SIZE);

            bool stop = false;

            bool            thresholdExceeded = false;
            bslmt::Condition thresholdExceededCondition;
            bslmt::Mutex     thresholdExceededMutex;

            bslmt::ThreadGroup tg;

            tg.addThread(bdlf::BindUtil::bind(&test9PushBack,
                                             &mX,
                                             PUSH_RATE,
                                             THRESHOLD,
                                             &stop,
                                             &thresholdExceeded,
                                             &thresholdExceededCondition));

            while (!thresholdExceeded) {
                bslmt::LockGuard<bslmt::Mutex> guard(&thresholdExceededMutex);
                thresholdExceededCondition.wait(&thresholdExceededMutex);
            }

            int oldLength = mX.length();
            ASSERT(THRESHOLD <= oldLength);

            mX.removeAll();

            int newLength = mX.length();
            ASSERT(newLength < oldLength);

            stop = true;

            tg.joinAll();
        }

      } break;
      case 5: {
         if (verbose) cout << endl
                           << "Stress test 2" << endl
                           << "==================" << endl;

          bdlcc::FixedQueue<StressNode> *queue =
                                      new bdlcc::FixedQueue<StressNode>(4);
          StressNode sn;
          queue->pushBack(sn);
          queue->pushBack(sn);
          queue->pushBack(sn);
          delete queue;
      } break;
      case 4: {
         if (verbose) cout << endl
                           << "Stress test 1" << endl
                           << "==================" << endl;

        int numProducers = 20;
        int numConsumers = 20;
        int maxCount = INT_MAX;
        int queueSize = 4;
        int seconds = 5;
        if (verbose) cout << endl
                          << "Stress testing for " << seconds
                              << " seconds with "
                              << numProducers << " producers, "
                              << numConsumers << " consumers" << endl
                          << "==============" << endl;

        StressData* producerData = new StressData[numProducers];
        StressData* consumerData = new StressData[numConsumers];
        memset(producerData, 0, sizeof(StressData)*numProducers);
        memset(consumerData, 0, sizeof(StressData)*numConsumers);
        bdlcc::FixedQueue<StressNode> queue(queueSize);
        bool stopProducers = false;
        for (int i=0; i<numProducers; i++) {
            producerData[i].counts = new int[1];
            producerData[i].checksums = new int[1];
            producerData[i].counts[0] = 0;
            producerData[i].checksums[0] = 0;
            producerData[i].queue = &queue;
            producerData[i].maxCount = maxCount;
            producerData[i].thread = i;
            producerData[i].stopProd = &stopProducers;
            bslmt::ThreadUtil::create(&producerData[i].handle,
                    ((i%2) ? stressProducer2 : stressProducer1),
                    (void*)(producerData+i));
        }
        for (int i=0; i<numConsumers; i++) {
            consumerData[i].counts = new int[numProducers];
            consumerData[i].checksums = new int[numProducers];
            for (int j=0; j<numProducers; j++) {
                consumerData[i].counts[j] = 0;
                consumerData[i].checksums[j] = 0;
            }
            consumerData[i].queue = &queue;
            producerData[i].maxCount = maxCount;
            consumerData[i].thread = i;
            bslmt::ThreadUtil::create(&consumerData[i].handle,
                    ((i%2) ? stressConsumer2 : stressConsumer1),
                    (void*)(consumerData+i));
        }
        for (int i=0; i<seconds; i++) {
            bslmt::ThreadUtil::microSleep(1000000);
            if (verbose) cout << "." << flush;
        }
        if (verbose) cout << endl;
        // First, stop producers and join them.  Consumers are still running so
        // the producers must be looping continuously.
        if (seconds) stopProducers = true;
        for (int i=0; i<numProducers; i++) {
            bslmt::ThreadUtil::join(producerData[i].handle);
        }
        if (verbose) cout << "Producers stopped." << flush;
        // push messages to the queue to stop consumers
        for (int i=0; i<numConsumers; i++) {
            StressNode sn;
            sn.thread=-1;
            queue.pushBack(sn);
        }
        // then join consumers
        for (int i=0; i<numConsumers;i++) {
            bslmt::ThreadUtil::join(consumerData[i].handle);
        }
        if (verbose) cout << "Consumers stopped." << flush;
        // now, verify the results
        for (int i=0; i<numProducers;i++) {
            int count = 0;
            int checksum = 0;
            for (int j=0; j<numConsumers; j++) {
                count += consumerData[j].counts[i];
                checksum += consumerData[j].checksums[i];
            }
//            printf("%d:%08x:",count,checksum);
            ASSERT(producerData[i].counts[0] == count);
            ASSERT(producerData[i].checksums[0] == checksum);
        }
        for (int i=0; i<numProducers; i++) {
            delete[] producerData[i].counts;
            delete[] producerData[i].checksums;
        }
        for (int i=0; i<numConsumers; i++) {
            delete[] consumerData[i].counts;
            delete[] consumerData[i].checksums;
        }
        if (verbose) cout << "Checks completed.  Queue.length()=="
                          << queue.length() << endl;
        delete[] producerData;
        delete[] consumerData;
      } break;
      case 3: {
         // -----------------------------------------------------------------
          if (verbose) cout << endl
                            << "THREAD test 2" << endl
                            << "==============" << endl;

          // TEST constructors
          const int NITERATIONS = 100000;
          const int NTHREADS    = 5;

          ThreadArgs args(NITERATIONS);
          bslmt::ThreadAttributes attr;

          bslmt::ThreadUtil::Handle handles[NTHREADS*2];

          args.d_mutex.lock();
          args.d_iterations = NITERATIONS;

          for (int i=0; i<NTHREADS; ++i) {
              bslmt::ThreadUtil::create(&handles[i], attr,
                      popFrontTestThread, &args );
              while (args.d_count != (i+1) )
                  args.d_startCond.wait(&args.d_mutex);
          }

          if (veryVerbose) {
              cout << "All pop threads created..." << endl;
          }

          for (int i=0; i<NTHREADS; ++i) {
              bslmt::ThreadUtil::create(&handles[i+NTHREADS], attr,
                      pushBackTestThread, &args );
              while (args.d_count != (NTHREADS+i+1) )
                  args.d_startCond.wait(&args.d_mutex);
          }

          if (veryVerbose) {
              cout << "All push threads created..." << endl;
          }

          args.d_goSig++;
          args.d_goCond.broadcast();
          args.d_mutex.unlock();

          for (int i=0; i<(NTHREADS*2); ++i) {
              bslmt::ThreadUtil::join(handles[i]);
          }
          //ASSERT(0 == args.d_queue.length());
          //P(args.d_queue.length());
      } break;
      case 2: {
          // -----------------------------------------------------------------
          if (verbose) cout << endl
              << "THREAD test" << endl
                  << "==============" << endl;

          // TEST constructors
          const int NITERATIONS = 100000;
          const int NTHREADS    = 10;

          ThreadArgs args(NITERATIONS);
          bslmt::ThreadAttributes attr;

          bslmt::ThreadUtil::Handle handles[NTHREADS*2];

          args.d_mutex.lock();
          args.d_iterations = NITERATIONS;

          for (int i=0; i<NTHREADS; ++i) {
              bslmt::ThreadUtil::create(&handles[i], attr,
                      popFrontTestThread, &args );
              while (args.d_count != (i+1) )
                  args.d_startCond.wait(&args.d_mutex);
          }

          args.d_goSig++;
          args.d_goCond.broadcast();
          args.d_mutex.unlock();

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
// disable the larger than warning for this buffer
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlarger-than="
#endif

          char reserved[NITERATIONS+1];

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

          for (int i=0; i<NITERATIONS; ++i) {
              //bslmt::LockGuard<bslmt::Mutex> lock(&args.d_mutex);
              //        while ( args.d_queue.length() < 1 ) {
              //        args.d_startCond.wait(&args.d_mutex);
              //        }
              Element *e = (Element*)(reserved+i);
              args.d_queue.pushBack(e);
          }
          for ( int i=0; i<NTHREADS; ++i)
              args.d_queue.pushBack((Element*)(0xffffffff));

          for (int i=0; i<NTHREADS; ++i) {
              bslmt::ThreadUtil::join(handles[i]);
          }
          //    ASSERT(0 == args.d_queue.length());
          //P(args.d_queue.length());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj x1(10*1000);
        double reserved[3];
        double *VA=reserved;
        double *VB=reserved+1;
        double *VC=reserved+2;

        ASSERT(0 ==  x1.length());
        ASSERT(0 == x1.pushBack(VA));
        ASSERT(0 == x1.pushBack(VB));
        ASSERT(0 == x1.pushBack(VC));

        ASSERT( VA == x1.popFront() );
        ASSERT( VB == x1.popFront() );
        ASSERT( VC == x1.popFront() );

     } break;
      case -2: {
        // --------------------------------------------------------------------
        // A SIMPLE BENCHMARK
        //
        // imitates a producer-consumer system with a fixed size queue using
        // two semaphores
        //
        // TBD: coredump on sundev31 -- fix
        int numProducers = argc > 2 ? atoi(argv[2]) : 1;
        int numConsumers = argc > 3 ? atoi(argv[3]) : 1;
        int queueSize = argc > 4 ? atoi(argv[4]) : 1;
        int samples = argc > 5 ? atoi(argv[5]) : 2;
        int seconds = 5;
        if (verbose) cout << endl
                          << "Benchmarking....." << endl
                          << "=================" << endl
                          << "producers=" << numProducers << endl
                          << "consumers=" << numConsumers << endl
                          << "queue size=" << queueSize << endl;
        BenchData* producerData = new BenchData[numProducers];
        BenchData* consumerData = new BenchData[numConsumers];
        bdlcc::FixedQueue<void*> queue(queueSize);
        //bsls::Types::Int64 timeStart = bsls::TimeUtil::getTimer();
        for (int i=0; i<numConsumers; i++) {
            consumerData[i].queue = &queue;
            consumerData[i].count = 0;
            consumerData[i].stop = false;
            bslmt::ThreadUtil::create(&consumerData[i].handle,
                                     benchConsumer,
                                     (void*)(consumerData+i));
        }
        for (int i=0; i<numProducers; i++) {
            producerData[i].queue = &queue;
            producerData[i].stop = false;
            producerData[i].count = 0;
            bslmt::ThreadUtil::create(&producerData[i].handle,
                                     benchProducer,
                                     (void*)(producerData+i));
        }
        for (int j=0; j<samples; j++) {
            bsls::Types::Int64 timeStart = bsls::TimeUtil::getTimer();
            bsls::Types::Int64 timeStartCPU = ::clock();
            int* consumerCount = new int[numConsumers];
            for (int i=0; i<numConsumers; i++) {
                consumerCount[i] = consumerData[i].count;
            }
            bsls::Types::Int64 throughput;
            bsls::Types::Int64 throughputCPU;
            for (int i=0; i<seconds; i++) {
                bslmt::ThreadUtil::microSleep(1000000);
                bsls::Types::Int64 totalMessages = 0;
                for (int i=0; i<numConsumers;i++) {
                    totalMessages += (consumerData[i].count-consumerCount[i]);
                }
                bsls::Types::Int64 elapsed_us =
                               (bsls::TimeUtil::getTimer() - timeStart) / 1000;
                bsls::Types::Int64 elapsed_usCPU =
                                                      ::clock() - timeStartCPU;
                throughput = (totalMessages*1000000/elapsed_us);
                throughputCPU = (totalMessages*1000000/elapsed_usCPU);
                cout << "testing: "
                     << elapsed_us/1000              << " ms, "
                     << elapsed_usCPU*100/elapsed_us << " CPU%, "
                     << totalMessages                << " msg, "
                     << fmt((int)throughput)         << " msg/s, "
                     << fmt((int)throughputCPU)      << " msg/CPUs"
                     << endl;
            }
            cout << "====== final:"
                 << fmt((int)throughput)    << " msg/s, "
                 << fmt((int)throughputCPU) << " msg/CPUs\n"
                 << endl;
        }
        cout << "stopping: " << flush;
        for (int i=0; i<numProducers; i++) {
            producerData[i].stop = true;
        }
        for (int i=0; i<numProducers; i++) {
            bslmt::ThreadUtil::join(producerData[i].handle);
            cout << 'p' << flush;
        }
        for (int i=0; i<numConsumers; i++) {
            consumerData[i].stop = true;
        }
        for (int i=0; i<numConsumers; i++) {
            queue.pushBack((void*)0xBAADF00D);
        }
        for (int i=0; i<numConsumers;i++) {
            bslmt::ThreadUtil::join(consumerData[i].handle);
            cout << 'c' << flush;
        }
        cout << endl;
        delete[] producerData;
        delete[] consumerData;
      } break;

      case -3: {
        enum {
            k_QUEUE_SIZE_LARGE = 500000,
            k_NUM_PUSHERS_MORE = 20,
            k_EMPTY_VERIFY_MS = 1500
        };

        cout << "...Non-full-queue test (more)" << endl;
        int numIterations = verbose ? atoi(argv[2]) : 1000;
        for (int a = 0; a < numIterations; ++a)
        {
            volatile bool done = false;
            bdlcc::FixedQueue<int> queue(k_QUEUE_SIZE_LARGE);

            bslmt::ThreadGroup pusherGroup;
            pusherGroup.addThreads(bdlf::BindUtil::bind(&case9pusher,
                                                       &queue,
                                                       &done),
                                   k_NUM_PUSHERS_MORE);

            // Sleep for up to 2 ms just to let some stuff get into the queue
            bslmt::ThreadUtil::microSleep(rand() % 2000);
            queue.disable();
            if (veryVerbose) {
                cout << "     (after disabling, queue has " << queue.length()
                     << " items)" << endl;
            }

            int result;
            queue.removeAll();
            bsls::Stopwatch timer;
            timer.start();
            double elapsed;
            for (int i = 0; (elapsed = timer.elapsedTime() * 1000) <
                     k_EMPTY_VERIFY_MS; ++i) {
                BSLS_ASSERT(queue.isEmpty());
                ASSERT(queue.isEmpty());
                if (!queue.isEmpty()) {
                    cout << "TEST FAILURE: disabled queue became non-empty"
                         << " after " << elapsed << " ms (" << i
                         << " iters).  Has " << queue.length()
                         << " items." << endl;
                    break;
                }
            }
            queue.enable();
            if (veryVerbose) {
                cout << "     ...waiting for queue to become nonempty...";
            }
            while (queue.isEmpty()) ;
            if (veryVerbose) {
                cout << "done" << endl;
            }
            ASSERT(0 == queue.tryPopFront(&result));
            queue.popFront();
            done = true;
            queue.disable();
            queue.removeAll();
            pusherGroup.joinAll();
        }
      } break;

      case -4: {
        // --------------------------------------------------------------------
        // STRESS TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "STRESS TEST -4" << endl
                          << "==============" << endl;
        enum {
            k_NUM_THREADS = 6,
            k_NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : k_NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : k_NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : k_NUM_THREADS;

        if (verbose) cout << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "NUM PUSHERS: " << numPushers << endl
                          << "NUM POPPERS: " << numPoppers << endl;
        seqtst::runtest(numIterations, numPushers, numPoppers);
      } break;

      case -5: {
        // --------------------------------------------------------------------
        // STRESS TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "STRESS TEST -5" << endl
                          << "==============" << endl;
        enum {
            k_NUM_THREADS = 6,
            k_NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : k_NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : k_NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : k_NUM_THREADS;

        if (verbose) cout << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "NUM PUSHERS: " << numPushers << endl
                          << "NUM POPPERS: " << numPoppers << endl;
        zerotst::runtest(numIterations, numPushers, numPoppers);
      } break;

      case -7: {
        // --------------------------------------------------------------------
        // DISABLE TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "DISABLE TEST -7" << endl
                          << "===============" << endl;
        enum {
            k_NUM_THREADS = 6
        };

        int numPushers = argc > 2 ? atoi(argv[2]) : k_NUM_THREADS;

        if (verbose) cout << endl
                          << "NUM PUSHERS: " << numPushers << endl;

        disabletst::runtest(numPushers, 4099, false);
      } break;
      case -8: {
        // --------------------------------------------------------------------
        // NON BLOCKING STRESS TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "NON BLOCKING STRESS TEST -8" << endl
                          << "===========================" << endl;
        enum {
            k_NUM_THREADS = 6,
            k_NUM_ITERATIONS = 1000000,
            k_QUEUE_SIZE = 2047,
            k_PUSH_COUNT = 100
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : k_NUM_ITERATIONS;
        int numThreads = argc > 3 ? atoi(argv[3]) : k_NUM_THREADS;
        int queueSize = argc > 4 ? atoi(argv[4]) : k_QUEUE_SIZE;
        int pushCount = argc > 5 ? atoi(argv[5]) : k_PUSH_COUNT;

        if (verbose) cout << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "NUM THREADS: " << numThreads << endl
                          << "QUEUE SIZE: " << queueSize << endl
                          << "PUSH COUNT: " << pushCount << endl;

        nonblocktst::runtest(numIterations, numThreads, queueSize, pushCount);
      } break;
      case -9: {
        // ---------------------------------------------------------
        // tests after 32-bit index rollover
        //
        // The indices into the queue are 32-bit indexes that count up
        // continuously.  They're interpreted modulo the queue size, but the
        // indexes themselves will abruptly roll over to 0 after 2^32
        // operations on the queue.  For a period of time thereafter, 'front'
        // will be vastly larger than 'back' while the queue size remains
        // normal.
        //
        // Perform various multithreaded tests on a queue AFTER it has reached
        // this condition.  First, in a single thread, spin these indices up;
        // then, using multiple threads, stress-test operations that compare
        // front to back, such as pushing into a full queue or calculating the
        // length.
        //
        // ---------------------------------------------------------
        enum {
            k_QUEUE_SIZE = 30000  // fairly large to facilitate certain
                                  // non-empty tests
        };

        // First, run our tests on a non-rolled-over queue
        bsl::cout << "Testing newly created queue..." << bsl::endl;
        bslmt::ThreadGroup workThreads;
        bsls::AtomicInt doneFlag(0);
        bdlcc::FixedQueue<int> youngQueue(k_QUEUE_SIZE);
        // Fill the queue up halfway.
        for (unsigned i = k_QUEUE_SIZE/2; i < k_QUEUE_SIZE; ++i) {
            youngQueue.pushBack(i);
        }

        workThreads.addThreads(bdlf::BindUtil::bind(&rolloverLengthChecker,
                                                   &youngQueue,
                                                   &doneFlag),
                               2);

        bslmt::Turnstile pushTurnstile(6000.0);
        workThreads.addThread(bdlf::BindUtil::bind(&rolloverPusher,
                                                  &youngQueue,
                                                  &doneFlag,
                                                  &pushTurnstile,
                                                  1));

        workThreads.addThread(bdlf::BindUtil::bind(&rolloverPusher,
                                                  &youngQueue,
                                                  &doneFlag,
                                                  &pushTurnstile,
                                                  2));

        bslmt::Turnstile popTurnstile(3000.0);
        workThreads.addThreads(bdlf::BindUtil::bind(&rolloverPopper,
                                                   &youngQueue,
                                                   &doneFlag,
                                                   &popTurnstile),
                               2);

        workThreads.joinAll();

        doneFlag = 0;

        bsl::cout << "\nIncrementing";

        bdlcc::FixedQueue<int> queue (k_QUEUE_SIZE);
        for (unsigned i = 0; i < 0xFFFFFFFF - k_QUEUE_SIZE; i += 5) {
            queue.pushBack(i);
            queue.pushBack(i);
            queue.pushBack(i);
            queue.pushBack(i);
            queue.pushBack(i);
            queue.popFront();
            queue.popFront();
            queue.popFront();
            queue.popFront();
            queue.popFront();

            if (0 == i % 4000000) {
                bsl::cout << '.' << bsl::flush;
            }
        }

        bsl::cout << bsl::endl
                  << "Done incrementing.  Filling halfway."
                  << bsl::endl;

        // Fill the queue up halfway.
        for (unsigned i = k_QUEUE_SIZE/2; i < k_QUEUE_SIZE; ++i) {
            queue.pushBack(i);
        }

        bsl::cout << "Done filling.  Starting test threads."
                  << bsl::endl;

        pushTurnstile.reset(6000.0);
        popTurnstile.reset(3000.0);

        // Now the indices should be ready to roll over.  Start some
        // stress-test threads:
        //  * Have two threads asserting continuously that the queue is
        //    always between halfway and entirely full, according to length().
        //  * Have another two threads pushing into the queue at some
        //    metered rate R.
        //  * At a metered rate R/2, have another two threads popping from
        //    the queue.
        workThreads.addThreads(bdlf::BindUtil::bind(&rolloverLengthChecker,
                                                   &queue,
                                                   &doneFlag),
                               2);

        workThreads.addThread(bdlf::BindUtil::bind(&rolloverPusher,
                                                  &queue,
                                                  &doneFlag,
                                                  &pushTurnstile,
                                                  1));

        workThreads.addThread(bdlf::BindUtil::bind(&rolloverPusher,
                                                  &queue,
                                                  &doneFlag,
                                                  &pushTurnstile,
                                                  2));

        workThreads.addThreads(bdlf::BindUtil::bind(&rolloverPopper,
                                                   &queue,
                                                   &doneFlag,
                                                   &popTurnstile),
                               2);

        workThreads.joinAll();
        bsl::cout << "Done.  testStatus = " << testStatus << bsl::endl;
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
