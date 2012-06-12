// bcec_fixedqueue.t.cpp       -*-C++-*-

#include <bcec_fixedqueue.h>
#include <bcec_queue.h>
#include <bcec_skiplist.h>

#include <bcemt_barrier.h>
#include <bcemt_configuration.h>
#include <bcemt_lockguard.h>
#include <bcemt_qlock.h>
#include <bcema_sharedptr.h>
#include <bcema_testallocator.h>
#include <bcemt_threadgroup.h>
#include <bcemt_turnstile.h>

#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdetu_systemtime.h>
#include <bdeu_random.h>

#include <bslma_defaultallocatorguard.h>
#include <bsls_stopwatch.h>
#include <bsls_timeutil.h>

#include <bsl_iostream.h>
#include <bsl_algorithm.h>

#include <bsl_c_stdlib.h>            // atoi()

#if defined(BDES_PLATFORMUTIL__NO_LONG_HEADER_NAMES)
#include <strstrea.h>
#else
#include <bsl_strstream.h>
#endif
using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static bcemt_QLock coutMutex;

static void aSsErTT(int c, const char *s, int i)
{
    if (c) {
        bcemt_QLockGuard guard(&coutMutex);
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define ASSERTT(X) { aSsErTT(!(X), #X, __LINE__); }

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP_ASSERTT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErTT(1, #X, __LINE__); }}

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
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define V(X) { if (verbose) P(X) }            // Print in verbose mode

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef void Element;
typedef bcec_FixedQueue<Element*> Obj;

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------
namespace Backoff {

void hardwork(int* item, int spin)
{
    for (int i=0; i<spin; ++i) {
        bdeu_Random::generate15(item);
    }
}

void pushpopThread(bcec_FixedQueue<int> *queue,
                   bcemt_Barrier *barrier,
                   int numIterations,
                   int workSpin)
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
            int queuedValue = queue->popFront();
        }
    }
}

} // close namespace Backoff

void rolloverPusher(bcec_FixedQueue<int> *queue,
                    bces_AtomicInt *doneFlag,
                    bcemt_Turnstile *turnstile,
                    int threadId)
{
    enum {
        NUM_ITEMS = 50000 // num to push in this thread
    };

    const int base = 1000000 * threadId;

    for (int i = 0; i < NUM_ITEMS; ++i) {
        turnstile->waitTurn();

        queue->pushBack(base + i);
    }
    (*doneFlag)++;
}

void rolloverLengthChecker(bcec_FixedQueue<int> *queue,
                           bces_AtomicInt *doneFlag)
{
    const int minLength = queue->size() / 2;
    const int maxLength = queue->size();

    for (unsigned i = 1; 2 > doneFlag->relaxedLoad(); ++i) {
        int length = queue->length();

        LOOP_ASSERTT(length, minLength <= length && maxLength >= length);
        if (0 == i % 10) {
            bcemt_ThreadUtil::yield();
        }
    }
}

void rolloverPopper(bcec_FixedQueue<int> *queue,
                    bces_AtomicInt *doneFlag,
                    bcemt_Turnstile *turnstile)
{
    const int minLength = queue->size() / 2;

    int lastSeen1 = minLength;
    int lastSeen2 = minLength;

    while (2 > doneFlag->relaxedLoad()) {
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

void pushpopThread(bcec_FixedQueue<int> *queue,
                   bces_AtomicInt *stop)
{
    while (!stop->relaxedLoad()) {
        queue->pushBack(1);
        queue->popFront();

        queue->pushBack(1);
        queue->popFront();

        queue->pushBack(1);
        queue->popFront();
    }
}

class TestType
{
public:

    TestType(int arg1, int arg2)
    {}

    TestType(const TestType& rhs)
    {}

    TestType& operator=(const TestType& rhs) {
        return *this;
    }
};

struct CountedDelete
{
    static bces_AtomicInt s_numDeletes;

public:
    ~CountedDelete()
    { ++s_numDeletes; }

    static int numDeletes()
    { return s_numDeletes; }
};

bces_AtomicInt CountedDelete::s_numDeletes = 0;

struct ThreadArgs {
    bcemt_Condition  d_startCond;
    bcemt_Condition  d_goCond;
    bcemt_Mutex      d_mutex;
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
    // This function is used to simulate a thread pool job.  It
    // accepts a pointer to a pointer to a structure containing
    // a mutex and a conditional variable.  The function simply
    // blocks until the conditional variable is signaled.
{
    ThreadArgs *args = (ThreadArgs*)ptr;
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&args->d_mutex);
        ++args->d_count;
        args->d_startCond.signal();
        while( !args->d_goSig ) args->d_goCond.wait(&args->d_mutex);
    }

    char* reserved = args->d_reserved;
    for (int i=1; i<args->d_iterations+1; ++i) {
        if (veryVerbose && i%10000 == 0) {
            bcemt_QLockGuard guard(&coutMutex);
            cout << "Thread " << bcemt_ThreadUtil::selfIdAsInt()
                 << " pushing i=" << i << endl;
        }

        args->d_queue.pushBack((Element*)(reserved + i));
    }

    if (veryVerbose) {
        bcemt_QLockGuard guard(&coutMutex);
        cout << "Thread " << bcemt_ThreadUtil::selfIdAsInt()
             << "done, pushing sentinal value..." << endl;
    }

    args->d_queue.pushBack((Element*)(0xFFFFFFFF));

    return ptr;
}

void* popFrontTestThread(void *ptr)
    // This function is used to simulate a thread pool job.  It
    // accepts a pointer to a pointer to a structure containing
    // a mutex and a conditional variable.  The function simply
    // blocks until the conditional variable is signaled.
{
    ThreadArgs *args = (ThreadArgs*)ptr;
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&args->d_mutex);
        ++args->d_count;
        args->d_startCond.signal();
        while( !args->d_goSig ) args->d_goCond.wait(&args->d_mutex);
    }
    while (1) {
        Element *e = args->d_queue.popFront();
        if ((Element*)0xffffffff == e) break;
    }
    return ptr;
}

}

void case9pusher(bcec_FixedQueue<int> *queue,
                 volatile bool *done)
{
    while (!*done) {
        queue->pushBack(bcemt_ThreadUtil::selfIdAsInt());
    }
}

void case9drainer(bcec_FixedQueue<int> *queue,
                  bcemt_Barrier *drainDoneBarrier)
{
    int result;
    while (0 == queue->tryPopFront(&result)) ;
    ASSERT(queue->isEmpty());
    ASSERT(queue->isEmpty());
    drainDoneBarrier->wait();
}

void case9disabler(bcec_FixedQueue<int> *queue,
                   bcemt_Barrier *drainDoneBarrier,
                   bcemt_Barrier *reEnableBarrier)
{
    queue->disable();

    enum {
        NUM_PUSH_TRIES=5000
    };

    for (int i = 0; i < NUM_PUSH_TRIES; ++i) {
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
    bcemt_ThreadUtil::Handle            handle;
    bcec_FixedQueue<StressNode>        *queue;
    int                                *counts;
    int                                *checksums;
    int                                 maxCount;
    int                                 thread;
    bool                               *stopProd;
};

extern "C" void *stressConsumer1(void* arg) {
    StressData *data = (StressData*)arg;
    while(true) {
        StressNode sn = data->queue->popFront();
        if (sn.thread<0) return 0;
        ++data->counts[sn.thread];
        data->checksums[sn.thread] += sn.value;
    }
}

extern "C" void *stressConsumer2(void* arg) {
    StressData *data = (StressData*)arg;
    while(true) {
        StressNode sn;
        if (!data->queue->tryPopFront(&sn)) {
            if (sn.thread<0) return 0;
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
        if (*data->stopProd) return 0;
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
        if (*data->stopProd) return 0;
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
    bcemt_ThreadUtil::Handle    handle;
    bcec_FixedQueue<void*>     *queue;
    int                         count;
    bool                        stop;
};

extern "C" void *benchConsumer(void* arg) {
    BenchData *data = (BenchData*)arg;
    while(true) {
        data->queue->popFront();
        if (data->stop) return 0;
        ++data->count;
    }
}

extern "C" void *benchProducer(void* arg) {
    BenchData *data = (BenchData*)arg;
    int reserved;
    while(true) {
        data->queue->pushBack((void*)&reserved);
        if (data->stop) return 0;
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

void test9PushBack(bcec_FixedQueue<int> *queue,
                   double                rate,
                   int                   threshold,
                   bool                 *stop,
                   bool                 *thresholdExceeded,
                   bcemt_Condition      *thresholdExceededCondition)
{
    bcemt_Turnstile turnstile(rate);

    while (!*stop) {
        turnstile.waitTurn();
        bcemt_ThreadUtil::microSleep(10 * 1000);
        queue->pushBack(0);

        if (queue->length() >= threshold) {
            *thresholdExceeded = true;
            thresholdExceededCondition->signal();
        }
    }
}

void sleepAndWait(int numMicroseconds, bcemt_Barrier *barrier)
{
    bcemt_ThreadUtil::microSleep(numMicroseconds);
    barrier->wait();
}

void abaThread(char *firstValue, char *lastValue,
               bcec_FixedQueue<char*> *queue, bcemt_Barrier *barrier,
               bool sendSentinal)
{
    barrier->wait();
    for (char* value = firstValue; value <= lastValue; ++value) {
        queue->pushBack(value);
    }
    if (sendSentinal) {
        if (veryVerbose) {
            bcemt_QLockGuard guard(&coutMutex);
            cout << "Thread " << bcemt_ThreadUtil::selfIdAsInt()
                 << " done, pushing sentinal value" << endl;
        }
        queue->pushBack((char*)0xffffffff);
    }
}

namespace nonblocktst {

struct Item {
    int  d_threadId;
    int  d_sequenceNum;
    bdef_Function<void(*)()> d_funct;
};

struct Control {
    bcemt_Barrier         *d_barrier;

    bcec_FixedQueue<Item> *d_queue;

    int                    d_numExpectedThreads;
    int                    d_iterations;
    int                    d_queueSize;
    int                    d_pushCount;

    bces_AtomicInt         d_numThreads;

    bces_AtomicInt         d_numPushed;
    bces_AtomicInt         d_numPopped;
};

void f(const bcema_SharedPtr<int>&)
{
}

void workerThread(Control *control)
{
    bcema_SharedPtr<int> sp;
    sp.createInplace(bslma_Default::allocator(), 12345);

    bdef_Function<void(*)()> funct = bdef_BindUtil::bind(&f, sp);

    bsl::vector<int> seq(control->d_numExpectedThreads, -1);

    bcec_FixedQueue<Item> *queue = control->d_queue;

    int iterations = control->d_iterations;
    int pushCount = control->d_pushCount;

    bces_AtomicInt& numPushed = control->d_numPushed;
    bces_AtomicInt& numPopped = control->d_numPopped;

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
    bcec_FixedQueue<Item> queue(queueSize);

    ASSERT(queue.isEnabled());

    bcemt_Barrier barrier(numThreads);

    Control control;

    control.d_numExpectedThreads = numThreads;
    control.d_iterations = numIterations;
    control.d_pushCount = pushCount;

    control.d_queue = &queue;

    control.d_numThreads = 0;
    control.d_numPushed = 0;
    control.d_numPopped = 0;

    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&workerThread,&control),
            numThreads);

    tg.joinAll();
    ASSERT(queue.isEmpty());
    ASSERT(control.d_numPushed >= numIterations);
    ASSERT(control.d_numPopped == numIterations);
}
}

namespace disabletst {

struct Control {
    bcemt_Barrier         *d_barrier;
    bcec_FixedQueue<int>  *d_queue;
};

void pusherThread(Control *control)
{
    bcec_FixedQueue<int> *queue = control->d_queue;
    control->d_barrier->wait();

    while (queue->pushBack(123) == 0) {}
}

void runtest(int numPushers, int queueSize, bool doDrain)
{
    bcec_FixedQueue<int> queue(queueSize);

    ASSERT(queueSize == queue.size());
    ASSERT(queue.isEnabled());

    bcemt_Barrier barrier(numPushers);

    Control control;

    control.d_queue = &queue;
    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&pusherThread,&control),
            numPushers);

    while (!queue.isFull()) {
        bcemt_ThreadUtil::yield();
    }

    ASSERT(queue.isEnabled());
    ASSERT(queue.isFull());

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
}

namespace seqtst {

struct Item {
    int  d_threadId;
    int  d_sequenceNum;
    bdef_Function<void(*)()> d_funct;
};

struct Control {
    bcemt_Barrier         *d_barrier;

    bcec_FixedQueue<Item> *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bces_AtomicInt         d_numPushers;
    bces_AtomicInt         d_numPopped;
};

void f(const bcema_SharedPtr<int>&)
{
}

void pusherThread(Control *control)
{
    bcema_SharedPtr<int> sp;
    sp.createInplace(bslma_Default::allocator(), 12345);

    bdef_Function<void(*)()> funct = bdef_BindUtil::bind(&f, sp);

    bcec_FixedQueue<Item> *queue = control->d_queue;

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

    bcec_FixedQueue<Item> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while((numPopped = control->d_numPopped++) < totalToPop) {

        Item item = queue->popFront();

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        QUEUE_SIZE = 2047
    };

    bcec_FixedQueue<Item> queue(QUEUE_SIZE);

    ASSERT(QUEUE_SIZE == queue.size());
    ASSERT(queue.isEnabled());

    bcemt_Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdef_BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
    ASSERT(queue.isEmpty());
}
}

namespace indexqueue_zerotst {

struct Control {
    bcemt_Barrier              *d_barrier;

    bcec_FixedQueue_IndexQueue *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bces_AtomicInt         d_numPushers;

    bces_AtomicInt         d_numPopped;
    bces_AtomicInt         d_numOverflow;
    bces_AtomicInt         d_numUnderflow;
};

void pusherThread(Control *control)
{

    bcec_FixedQueue_IndexQueue *queue = control->d_queue;
    bces_AtomicInt& numOverflow = control->d_numOverflow;

    control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        for(;;) {
            int ret = queue->tryPushBack(0);
            if (ret) {
                numOverflow++;
                bcemt_ThreadUtil::yield();
            }
            else {
                break;
            }
        }
    }
}

void popperThread(Control *control)
{
    bcec_FixedQueue_IndexQueue *queue = control->d_queue;
    bces_AtomicInt& numPopped = control->d_numPopped;
    bces_AtomicInt& numUnderflow = control->d_numUnderflow;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int p;
    while((p = numPopped++) < totalToPop) {

        for (;;) {
            int data;
            int ret = queue->tryPopFront(&data);
            if (ret) {
                numUnderflow++;
                bcemt_ThreadUtil::yield();
            }
            else {
                ASSERT(data == 0);
                break;
            }
        }
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    if (verbose) cout << endl
        << "NUM ITERATIONS: " << numIterations << endl
            << "NUM PUSHERS: " << numPushers << endl
            << "NUM POPPERS: " << numPoppers << endl;

    enum {
        QUEUE_SIZE = 256,
        INDEX_RANGE = 8
    };

    bcec_FixedQueue_IndexQueue queue(QUEUE_SIZE, INDEX_RANGE);

    ASSERT(queue.isEnabled());

    bcemt_Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPushers = 0;
    control.d_numPopped = 0;
    control.d_numOverflow = 0;
    control.d_numUnderflow = 0;

    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdef_BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
    ASSERT(queue.length() == 0);

    V(control.d_numOverflow);
    V(control.d_numUnderflow);

}
}

namespace zerotst {

struct Control {
    bcemt_Barrier           *d_barrier;

    bcec_FixedQueue<void *> *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bces_AtomicInt         d_numPushers;
    bces_AtomicInt         d_numPopped;
};

void pusherThread(Control *control)
{
    int threadId;

    bcec_FixedQueue<void *> *queue = control->d_queue;

    threadId = control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        queue->pushBack((void *)0);
    }
}

void popperThread(Control *control)
{
    bcec_FixedQueue<void *> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while((numPopped = control->d_numPopped++) < totalToPop) {

        queue->popFront();
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        QUEUE_SIZE = 2047
    };

    bcec_FixedQueue<void *> queue(QUEUE_SIZE);

    ASSERT(QUEUE_SIZE == queue.size());
    ASSERT(queue.isEnabled());

    bcemt_Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdef_BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
    ASSERT(queue.isEmpty());
}
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bcemt_Configuration::setDefaultThreadStackSize(
                     bcemt_Configuration::recommendedDefaultThreadStackSize());

    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
        // ---------------------------------------------------------
        // Template Requirements Test
        //
        // bcec_FixedQueue<T> should work for types T that have no default
        // constructor and a 1-arg copy constructor.
        // ---------------------------------------------------------

        bcec_FixedQueue<TestType> q(10);

        TestType t(1, 2);
        q.pushBack(t);

         TestType t2 = q.popFront();
      } break;

      case 19: {
        // ---------------------------------------------------------
        // length() stress-test
        //
        // plan: in N threads, push one element and pop one element in
        // a tight loop.  In the main thread, invoke length() repeatedly;
        // the real length of the queue will always be between 0 and N,
        // so verify that the reported length is always in this range.
        // ---------------------------------------------------------

        enum {
            NUM_PUSHPOP_THREADS = 6,
            TEST_DURATION = 3 // in seconds
        };

        bcec_FixedQueue<int> queue (NUM_PUSHPOP_THREADS*2);

        bces_AtomicInt stop = 0;

        bcemt_ThreadGroup tg;
        tg.addThreads(bdef_BindUtil::bind(&pushpopThread,
                                          &queue,
                                          &stop),
                      NUM_PUSHPOP_THREADS);
        bsls_Stopwatch timer;
        timer.start();

        while (timer.elapsedTime() < TEST_DURATION) {
            int length;

            length = queue.length();
            LOOP_ASSERT(length, 0 <= length && length <= NUM_PUSHPOP_THREADS);

            length = queue.length();
            LOOP_ASSERT(length, 0 <= length && length <= NUM_PUSHPOP_THREADS);

            length = queue.length();
            LOOP_ASSERT(length, 0 <= length && length <= NUM_PUSHPOP_THREADS);

            length = queue.length();
            LOOP_ASSERT(length, 0 <= length && length <= NUM_PUSHPOP_THREADS);

            length = queue.length();
            LOOP_ASSERT(length, 0 <= length && length <= NUM_PUSHPOP_THREADS);
        }
        stop.relaxedStore(1);
        tg.joinAll();
      } break;

      case 18: {
        // ---------------------------------------------------------
        // IndexQueue size and range test
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "IndexQueue size and range test" << endl
                          << "==============================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);

        {
            enum {
                QUEUE_SIZE = 6
            };

            bcec_FixedQueue<int> mX(QUEUE_SIZE, &ta);
            ASSERT(0 == da.numBytesInUse());
            V(mX.size());
            V(mX.length());
            ASSERT(0 == mX.length());
            ASSERT(6 == mX.size());

            ASSERT(!mX.isFull());
            ASSERT(0 == mX.tryPushBack(0));
            ASSERT(!mX.isFull());
            ASSERT(0 == mX.tryPushBack(1));
            ASSERT(!mX.isFull());
            ASSERT(0 == mX.tryPushBack(2));
            ASSERT(!mX.isFull());
            ASSERT(0 == mX.tryPushBack(3));
            ASSERT(!mX.isFull());
            ASSERT(0 == mX.tryPushBack(4));
            ASSERT(!mX.isFull());
            ASSERT(0 == mX.tryPushBack(5));
            ASSERT(mX.isFull());
            ASSERT(0 != mX.tryPushBack(6));
        }

        {
            enum {
                QUEUE_SIZE = 6
            };

            bcec_FixedQueue<int> mX(QUEUE_SIZE, &ta);
            ASSERT(0 == da.numBytesInUse());
            V(mX.size());
            V(mX.length());
            ASSERT(0 == mX.length());
            ASSERT(6 == mX.size());

            ASSERT(0 == mX.tryPushBack(0));
            ASSERT(0 == mX.tryPushBack(1));
            ASSERT(0 == mX.tryPushBack(0));
            ASSERT(0 == mX.tryPushBack(1));
            int val;
            ASSERT(0 == mX.tryPopFront(&val));
            ASSERT(0 == val);
            ASSERT(0 == mX.tryPopFront(&val));
            ASSERT(1 == val);
            ASSERT(0 == mX.tryPopFront(&val));
            ASSERT(0 == val);
            ASSERT(0 == mX.tryPopFront(&val));
            ASSERT(1 == val);
        }

        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;

      case 17: {
        // ---------------------------------------------------------
        // Disable while pushing into a full queue
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "Disable while pushing into a full queue" << endl
                          << "=======================================" << endl;
        enum {
            NUM_THREADS = 4
        };

        for(int numPushers=1; numPushers<=NUM_THREADS; numPushers++) {
            disabletst::runtest(numPushers, 8, false);   // queue size, doDrain
            disabletst::runtest(numPushers, 8, true);    // queue size, doDrain
            disabletst::runtest(numPushers, 2047, false);
                                                         // queue size, doDrain
            disabletst::runtest(numPushers, 2047, true); // queue size, doDrain
        }

      } break;
      case 16: {
        // ---------------------------------------------------------
        // Pushing zeros through the IndexQueue
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "Pushing zeros through the IndexQueue" << endl
                          << "====================================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 50000
        };

        for(int numPushers=1; numPushers<=NUM_THREADS; numPushers++) {
        for(int numPoppers=1; numPoppers<=NUM_THREADS; numPoppers++) {

            indexqueue_zerotst::runtest(
                    NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;
      case 15: {
        // ---------------------------------------------------------
        // IndexQueue disable/enable test
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "IndexQueue disable/enable test" << endl
                          << "==============================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);

        {
            enum {
                QUEUE_SIZE = 3
            };

            bcec_FixedQueue<int> mX(QUEUE_SIZE, &ta);
            const bcec_FixedQueue<int>& X = mX;

            ASSERT(0 == da.numBytesInUse());
            V(X.size());
            V(X.length());
            ASSERT(0 == mX.length());
            ASSERT(3 == mX.size());
            ASSERT(X.isEnabled());

            enum { D1 = 5, D2 = 8, D3 = 3 };

            mX.disable();
            ASSERT(!X.isEnabled());
            int data = D1;
            ASSERT(-2 == mX.tryPushBack(data));
            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 == mX.length());

            mX.enable();
            ASSERT(X.isEnabled());
            data = D1;
            ASSERT(0 == mX.tryPushBack(data));
            ASSERT(1 == mX.length());
            data = D2;
            ASSERT(0 == mX.tryPushBack(data));
            ASSERT(2 == mX.length());
            data = D3;
            ASSERT(0 == mX.tryPushBack(data));
            ASSERT(3 == mX.length());
            ASSERT(-1 == mX.tryPushBack(data));
            ASSERT(0 == da.numBytesInUse());
            ASSERT(3 == mX.length());

            mX.disable();
            ASSERT(!X.isEnabled());
            ASSERT(-1 == mX.tryPushBack(data));

            ASSERT(0 == mX.tryPopFront(&data));
            ASSERT(D1 == data);

            ASSERT(-2 == mX.tryPushBack(data));
            ASSERT(0 == mX.tryPopFront(&data));
            ASSERT(D2 == data);
            ASSERT(0 == mX.tryPopFront(&data));
            ASSERT(D3 == data);

            ASSERT(-1 == mX.tryPopFront(&data));
        }

        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;

      case 14: {
        // ---------------------------------------------------------
        // IndexQueue breathing test
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "IndexQueue breathing test" << endl
                          << "=========================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);

        {
            enum {
                QUEUE_SIZE = 1
            };

            bcec_FixedQueue<int> mX(QUEUE_SIZE, &ta);
            ASSERT(0 == da.numBytesInUse());
            V(mX.size());
            V(mX.length());
            ASSERT(0 == mX.length());
            ASSERT(1 == mX.size());

            enum { D1 = 5, D2 = 8 };

            int data = D1;
            ASSERT(0 == mX.tryPushBack(data));
            ASSERT(0 == da.numBytesInUse());

            ASSERT(0 == mX.tryPopFront(&data));
            ASSERT(0 == da.numBytesInUse());
            V(data);
            ASSERT(D1 == data);

            data = D2;
            ASSERT(0 == mX.tryPushBack(data));
            ASSERT(0 == da.numBytesInUse());
            ASSERT(1 == mX.length());
            ASSERT(1 == mX.size());
            data = D1;
            ASSERT(-1 == mX.tryPushBack(data));
            ASSERT(0 == da.numBytesInUse());
            ASSERT(1 == mX.length());
            ASSERT(1 == mX.size());

            ASSERT(0 == mX.tryPopFront(&data));
            ASSERT(0 == da.numBytesInUse());
            V(data);
            ASSERT(D2 == data);

            data = D1;
            ASSERT(0 == mX.tryPushBack(data));
            ASSERT(0 == da.numBytesInUse());
            ASSERT(1 == mX.length());
            ASSERT(1 == mX.size());

            ASSERT(0 == mX.tryPopFront(&data));
            ASSERT(0 == da.numBytesInUse());
            V(data);
            ASSERT(D1 == data);
        }

        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;

      case 13: {
        // ---------------------------------------------------------
        // TESTING sequence constraints
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "sequence constraint test" << endl
                          << "========================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 50000
        };

        for(int numPushers=1; numPushers<=NUM_THREADS; numPushers++) {
        for(int numPoppers=1; numPoppers<=NUM_THREADS; numPoppers++) {

            zerotst::runtest(NUM_ITERATIONS, numPushers, numPoppers);
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
            NUM_THREADS = 4,
            NUM_ITERATIONS = 50000
        };

        for(int numPushers=1; numPushers<=NUM_THREADS; numPushers++) {
        for(int numPoppers=1; numPoppers<=NUM_THREADS; numPoppers++) {

            seqtst::runtest(NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;
      case 11: {
        // ---------------------------------------------------------
        // TESTING CONCERN: Verify the size calculations performed in
        // the constructor.
        // ---------------------------------------------------------

        {
            bcec_FixedQueue<CountedDelete> mX(100);
            V(mX.size());
            ASSERT(mX.size() == 100);
        }
        {
            bcec_FixedQueue<CountedDelete> mX(0x7FFF);
            V(mX.size());
            ASSERT(mX.size() == (0x7FFF));
        }
        {
            bcec_FixedQueue<CountedDelete> mX(2070);
            V(mX.size());
            ASSERT(mX.size() == 2070);
        }
      } break;

      case 10: {
        // ---------------------------------------------------------
        // TESTING CONCERN: Memory leak if pushing while disabled
        //
        // Plan: In a single thread, push an item while disabled and
        // assert that memory allocated does not increase.
        //
        // ---------------------------------------------------------

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);

        {
            CountedDelete cd;

            bcec_FixedQueue<CountedDelete> mX(100, &ta);
            ASSERT(0 == da.numBytesInUse());

            ASSERT(0 == mX.pushBack(cd));
            ASSERT(0 == da.numBytesInUse());

            cd = mX.popFront();
            ASSERT(0 == da.numBytesInUse());

            mX.disable();
            ASSERT(0 == da.numBytesInUse());

            int numBytes = ta.numBytesInUse();

            ASSERT(0 != mX.pushBack(cd));
            ASSERT(0 == da.numBytesInUse());
            V(CountedDelete::numDeletes());
            ASSERT(4 == CountedDelete::numDeletes() ||
                   3 == CountedDelete::numDeletes());
            ASSERT(0 == da.numBytesInUse());

            ASSERT(numBytes == ta.numBytesInUse());
        }

        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

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
            QUEUE_SIZE_SINGLETHREAD = 4,
            NUM_FULL_ITERATIONS = 1200,
            QUEUE_SIZE_MT = 30,
            NUM_PUSHERS = 12,
            QUEUE_SIZE_LARGE = 500000,
            NUM_PUSHERS_MORE = 20,
            NUM_PUSHERS_LESS = 2,
            EMPTY_VERIFY_MS = 1500
        };

        bcec_FixedQueue<int> queue(QUEUE_SIZE_SINGLETHREAD);
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

        bcec_FixedQueue<int> mtQueue(QUEUE_SIZE_MT);
        for (int i = 0; i < NUM_FULL_ITERATIONS; ++i) {
            if (veryVerbose && (0 == i % 100)) {
                cout << "     ...filling(" << i << ")...";
            }
            while (0 == mtQueue.tryPushBack(1)) ;
            if (veryVerbose && (0 == i % 100)) {
                cout << "filled." << endl;
            }

            bcemt_Barrier drainDoneBarrier(3);
            bcemt_Barrier reEnableBarrier(2);
            bcemt_ThreadGroup tg;
            tg.addThreads(bdef_BindUtil::bind(&case9disabler,
                                              &mtQueue,
                                              &drainDoneBarrier,
                                              &reEnableBarrier),
                          2);
            tg.addThread(bdef_BindUtil::bind(&case9drainer,
                                             &mtQueue,
                                             &drainDoneBarrier));
            tg.joinAll();
            LOOP_ASSERT(mtQueue.length(), 4 == mtQueue.length());
        }

        if (veryVerbose) cout << "...Disable-while-pushing test" << endl;
        {
            volatile bool done = false;
            bcemt_ThreadGroup pusherGroup;
            pusherGroup.addThreads(bdef_BindUtil::bind(&case9pusher,
                                                       &mtQueue,
                                                       &done),
                                   NUM_PUSHERS);
            mtQueue.disable();
            mtQueue.removeAll();
            bsls_Stopwatch timer;
            timer.start();
            while (timer.elapsedTime() * 1000 < EMPTY_VERIFY_MS) {
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
            bcec_FixedQueue<int> queue(QUEUE_SIZE_LARGE);

            bcemt_ThreadGroup pusherGroup;
            pusherGroup.addThreads(bdef_BindUtil::bind(&case9pusher,
                                                       &queue,
                                                       &done),
                                   NUM_PUSHERS_LESS);

            queue.disable();
            if (veryVerbose) {
                cout << "     (after disabling, queue has " << queue.length()
                     << " items)" << endl;
            }

            queue.removeAll();
            bsls_Stopwatch timer;
            timer.start();
            double elapsed;
            while ((elapsed = timer.elapsedTime() * 1000) < EMPTY_VERIFY_MS) {
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
            bcec_FixedQueue<int> queue(QUEUE_SIZE_LARGE);

            bcemt_ThreadGroup pusherGroup;
            pusherGroup.addThreads(bdef_BindUtil::bind(&case9pusher,
                                                       &queue,
                                                       &done),
                                   NUM_PUSHERS_MORE);

            // Sleep for up to 2 ms just to let some stuff get into the queue
            bcemt_ThreadUtil::microSleep(rand() % 2000);
            queue.disable();
            if (veryVerbose) {
                cout << "     (after disabling, queue has " << queue.length()
                     << " items)" << endl;
            }

            queue.removeAll();
            bsls_Stopwatch timer;
            timer.start();
            double elapsed;
            for (int i = 0; (elapsed = timer.elapsedTime() * 1000) <
                     EMPTY_VERIFY_MS; ++i) {
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
        // generation-count rollover logic is invoked.  The smallest number
        // of generation counts supported is 508; for a queue size of 3,
        // this implies that we need only run more than 3048 objects
        // through the queue successfully to trigger the rollover logic
        // and loop again.
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "Generation count logic test" << endl
                          << "==========================="   << endl;

        enum {
            NUM_PUSHER_THREADS = 6,
            QUEUE_SIZE = 3,
            NUM_VALUES = 3060 / NUM_PUSHER_THREADS
        };

        enum { NUM_ENTRIES = NUM_VALUES * NUM_PUSHER_THREADS };
        char reserved[NUM_ENTRIES];

        bcema_TestAllocator ta(veryVeryVerbose);

        bcemt_Barrier barrier(NUM_PUSHER_THREADS+1);

        {
            bcec_FixedQueue<char*> mX(3, &ta);
            bcemt_ThreadGroup tg;

            char* nextValue[NUM_PUSHER_THREADS];
            char* lastValue[NUM_PUSHER_THREADS];

            for (int j = 0; j < NUM_PUSHER_THREADS; ++j) {
                nextValue[j] = reserved + NUM_VALUES*j;
                lastValue[j] = reserved + NUM_VALUES*(j+1) - 1;
                tg.addThread(bdef_BindUtil::bind(&abaThread,
                                                 nextValue[j], lastValue[j],
                                                 &mX, &barrier, false));
            }
            tg.addThread(bdef_BindUtil::bind(&sleepAndWait, 100, &barrier));

            for (int numReceived = 0; numReceived < NUM_ENTRIES;
                 ++numReceived) {
                char* value = mX.popFront();
                int k;
                for (k = 0; k < NUM_PUSHER_THREADS; ++k) {
                    if (value == nextValue[k]) {
                        nextValue[k] += (value == lastValue[k] ? 0 : 1);
                        LOOP2_ASSERT((void*)nextValue[k],
                                     (void*)lastValue[k],
                                     nextValue[k] <= lastValue[k]);
                        break;
                    }
                }
                LOOP_ASSERT(numReceived, k < NUM_PUSHER_THREADS);
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
            NUM_PUSHER_THREADS = 40,
            NUM_VALUES = 6,
            NUM_ITERATIONS = 2000
        };

        enum { NUM_ENTRIES = NUM_VALUES * NUM_PUSHER_THREADS };
        char reserved[NUM_ENTRIES];

        bcema_TestAllocator ta(veryVeryVerbose);

        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            bcemt_Barrier barrier(NUM_PUSHER_THREADS+1);

            bcec_FixedQueue<char*> mX(NUM_ENTRIES+1, &ta);
            bcemt_ThreadGroup tg;

            char* nextValue[NUM_PUSHER_THREADS];
            char* lastValue[NUM_PUSHER_THREADS];

            for (int j = 0; j < NUM_PUSHER_THREADS; ++j) {
                nextValue[j] = reserved + NUM_VALUES*j;
                lastValue[j] = reserved + NUM_VALUES*(j+1) - 1;
                tg.addThread(bdef_BindUtil::bind(&abaThread,
                                                 nextValue[j], lastValue[j],
                                                 &mX, &barrier, false));
            }
            tg.addThread(bdef_BindUtil::bind(&sleepAndWait, 100, &barrier));

            for (int numReceived = 0; numReceived < NUM_ENTRIES;
                 ++numReceived) {
                char* value = mX.popFront();
                int k;
                for (k = 0; k < NUM_PUSHER_THREADS; ++k) {
                    if (value == nextValue[k]) {
                        nextValue[k] += (value == lastValue[k] ? 0 : 1);
                        LOOP2_ASSERT((void*)nextValue[k],
                                     (void*)lastValue[k],
                                     nextValue[k] <= lastValue[k]);
                        break;
                    }
                }
                LOOP_ASSERT(numReceived, k < NUM_PUSHER_THREADS);
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
        //  'bcec_FixedQueue<T>' and its specialization 'bcec_FixedQueue<T*>'
        //   in a single threaded environment.
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
            enum { NUM_ELEMENTS = 100 };
            bcec_FixedQueue<int> mX(NUM_ELEMENTS);

            for (int i = 0; i < NUM_ELEMENTS; ++i) {

                ASSERT(0 == mX.pushBack(0));
            }
            ASSERT(NUM_ELEMENTS == mX.length());
            mX.removeAll();
            ASSERT(0 == mX.length());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Single thread, queue of T*

        {
            enum { NUM_ELEMENTS = 100 };
            bcec_FixedQueue<int*> mX(NUM_ELEMENTS);

            int element = 0;

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                ASSERT(0 == mX.pushBack(&element));
            }

            ASSERT(NUM_ELEMENTS == mX.length());
            mX.removeAll();
            ASSERT(0 == mX.length());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Basic Multi-threaded test
        {
            const double PUSH_RATE = 100.0; // per sec
            const int    THRESHOLD = 200;
            const int    MAX_QUEUE_SIZE = THRESHOLD;

            bcec_FixedQueue<int> mX(MAX_QUEUE_SIZE);

            bool stop = false;

            bool            thresholdExceeded = false;
            bcemt_Condition thresholdExceededCondition;
            bcemt_Mutex     thresholdExceededMutex;

            bcemt_ThreadGroup tg;

            tg.addThread(bdef_BindUtil::bind(&test9PushBack,
                                             &mX,
                                             PUSH_RATE,
                                             THRESHOLD,
                                             &stop,
                                             &thresholdExceeded,
                                             &thresholdExceededCondition));

            while (!thresholdExceeded) {
                bcemt_LockGuard<bcemt_Mutex> guard(&thresholdExceededMutex);
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

          bcec_FixedQueue<StressNode> *queue =
                                            new bcec_FixedQueue<StressNode>(4);
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
        bcec_FixedQueue<StressNode> queue(queueSize);
        bool stopProducers = false;
        for(int i=0; i<numProducers; i++) {
            producerData[i].counts = new int[1];
            producerData[i].checksums = new int[1];
            producerData[i].counts[0] = 0;
            producerData[i].checksums[0] = 0;
            producerData[i].queue = &queue;
            producerData[i].maxCount = maxCount;
            producerData[i].thread = i;
            producerData[i].stopProd = &stopProducers;
            bcemt_ThreadUtil::create(&producerData[i].handle,
                    ((i%2) ? stressProducer2 : stressProducer1),
                    (void*)(producerData+i));
        }
        for(int i=0; i<numConsumers; i++) {
            consumerData[i].counts = new int[numProducers];
            consumerData[i].checksums = new int[numProducers];
            for(int j=0; j<numProducers; j++) {
                consumerData[i].counts[j] = 0;
                consumerData[i].checksums[j] = 0;
            }
            consumerData[i].queue = &queue;
            producerData[i].maxCount = maxCount;
            consumerData[i].thread = i;
            bcemt_ThreadUtil::create(&consumerData[i].handle,
                    ((i%2) ? stressConsumer2 : stressConsumer1),
                    (void*)(consumerData+i));
        }
        for(int i=0; i<seconds; i++) {
            bcemt_ThreadUtil::microSleep(1000000);
            if (verbose) cout << "." << flush;
        }
        if (verbose) cout << endl;
        // First, stop producers and join them.  Consumers are still running so
        // the producers must be looping continuously.
        if (seconds) stopProducers = true;
        for(int i=0; i<numProducers; i++) {
            bcemt_ThreadUtil::join(producerData[i].handle);
        }
        if (verbose) cout << "Producers stopped." << flush;
        // push messages to the queue to stop consumers
        for(int i=0; i<numConsumers; i++) {
            StressNode sn;
            sn.thread=-1;
            queue.pushBack(sn);
        }
        // then join consumers
        for(int i=0; i<numConsumers;i++) {
            bcemt_ThreadUtil::join(consumerData[i].handle);
        }
        if (verbose) cout << "Consumers stopped." << flush;
        // now, verify the results
        for(int i=0; i<numProducers;i++) {
            int count = 0;
            int checksum = 0;
            for(int j=0; j<numConsumers; j++) {
                count += consumerData[j].counts[i];
                checksum += consumerData[j].checksums[i];
            }
//            printf("%d:%08x:",count,checksum);
            ASSERT(producerData[i].counts[0] == count);
            ASSERT(producerData[i].checksums[0] == checksum);
        }
        for(int i=0; i<numProducers; i++) {
            delete[] producerData[i].counts;
            delete[] producerData[i].checksums;
        }
        for(int i=0; i<numConsumers; i++) {
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
          bcemt_Attribute attr;

          bcemt_ThreadUtil::Handle handles[NTHREADS*2];

          args.d_mutex.lock();
          args.d_iterations = NITERATIONS;

          for (int i=0; i<NTHREADS; ++i) {
              bcemt_ThreadUtil::create(&handles[i], attr,
                      popFrontTestThread, &args );
              while (args.d_count != (i+1) )
                  args.d_startCond.wait(&args.d_mutex);
          }

          if (veryVerbose) {
              cout << "All pop threads created..." << endl;
          }

          for (int i=0; i<NTHREADS; ++i) {
              bcemt_ThreadUtil::create(&handles[i+NTHREADS], attr,
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
              bcemt_ThreadUtil::join(handles[i]);
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
          bcemt_Attribute attr;

          bcemt_ThreadUtil::Handle handles[NTHREADS*2];

          args.d_mutex.lock();
          args.d_iterations = NITERATIONS;

          for (int i=0; i<NTHREADS; ++i) {
              bcemt_ThreadUtil::create(&handles[i], attr,
                      popFrontTestThread, &args );
              while (args.d_count != (i+1) )
                  args.d_startCond.wait(&args.d_mutex);
          }

          args.d_goSig++;
          args.d_goCond.broadcast();
          args.d_mutex.unlock();

          char reserved[NITERATIONS+1];

          for (int i=0; i<NITERATIONS; ++i) {
              //bcemt_LockGuard<bcemt_Mutex> lock(&args.d_mutex);
              //        while ( args.d_queue.length() < 1 ) {
              //        args.d_startCond.wait(&args.d_mutex);
              //        }
              Element *e = (Element*)(reserved+i);
              args.d_queue.pushBack(e);
          }
          for ( int i=0; i<NTHREADS; ++i)
              args.d_queue.pushBack((Element*)(0xffffffff));

          for (int i=0; i<NTHREADS; ++i) {
              bcemt_ThreadUtil::join(handles[i]);
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
        // imitates a producer-consumer system with a fixed size
        // queue using two semaphores
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
        bcec_FixedQueue<void*> queue(queueSize);
        //bsls_PlatformUtil::Int64 timeStart = bsls_TimeUtil::getTimer();
        for(int i=0; i<numConsumers; i++) {
            consumerData[i].queue = &queue;
            consumerData[i].count = 0;
            consumerData[i].stop = false;
            bcemt_ThreadUtil::create(&consumerData[i].handle,
                                     benchConsumer,
                                     (void*)(consumerData+i));
        }
        for(int i=0; i<numProducers; i++) {
            producerData[i].queue = &queue;
            producerData[i].stop = false;
            producerData[i].count = 0;
            bcemt_ThreadUtil::create(&producerData[i].handle,
                                     benchProducer,
                                     (void*)(producerData+i));
        }
        for(int j=0; j<samples; j++) {
            bsls_PlatformUtil::Int64 timeStart = bsls_TimeUtil::getTimer();
            bsls_PlatformUtil::Int64 timeStartCPU = ::clock();
            int* consumerCount = new int[numConsumers];
            for(int i=0; i<numConsumers; i++) {
                consumerCount[i] = consumerData[i].count;
            }
            bsls_PlatformUtil::Int64 throughput;
            bsls_PlatformUtil::Int64 throughputCPU;
            for(int i=0; i<seconds; i++) {
                bcemt_ThreadUtil::microSleep(1000000);
                bsls_PlatformUtil::Int64 totalMessages = 0;
                for(int i=0; i<numConsumers;i++) {
                    totalMessages += (consumerData[i].count-consumerCount[i]);
                }
                bsls_PlatformUtil::Int64 elapsed_us =
                                (bsls_TimeUtil::getTimer() - timeStart) / 1000;
                bsls_PlatformUtil::Int64 elapsed_usCPU =
                                                      ::clock() - timeStartCPU;
                throughput = (totalMessages*1000000/elapsed_us);
                throughputCPU = (totalMessages*1000000/elapsed_usCPU);
                cout << "testing: "
                     << elapsed_us/1000              << " ms, "
                     << elapsed_usCPU*100/elapsed_us << " CPU%, "
                     << totalMessages                << " msg, "
                     << fmt(throughput)              << " msg/s, "
                     << fmt(throughputCPU)           << " msg/CPUs"
                     << endl;
            }
            cout << "====== final:"
                 << fmt(throughput)    << " msg/s, "
                 << fmt(throughputCPU) << " msg/CPUs\n"
                 << endl;
        }
        cout << "stopping: " << flush;
        for(int i=0; i<numProducers; i++) {
            producerData[i].stop = true;
        }
        for(int i=0; i<numProducers; i++) {
            bcemt_ThreadUtil::join(producerData[i].handle);
            cout << 'p' << flush;
        }
        for(int i=0; i<numConsumers; i++) {
            consumerData[i].stop = true;
        }
        for(int i=0; i<numConsumers; i++) {
            queue.pushBack((void*)0xBAADF00D);
        }
        for(int i=0; i<numConsumers;i++) {
            bcemt_ThreadUtil::join(consumerData[i].handle);
            cout << 'c' << flush;
        }
        cout << endl;
        delete[] producerData;
        delete[] consumerData;
      } break;

      case -3: {
        enum {
            QUEUE_SIZE_LARGE = 500000,
            NUM_PUSHERS_MORE = 20,
            EMPTY_VERIFY_MS = 1500
        };

        cout << "...Non-full-queue test (more)" << endl;
        int numIterations = verbose ? atoi(argv[2]) : 1000;
        for (int a = 0; a < numIterations; ++a)
        {
            volatile bool done = false;
            bcec_FixedQueue<int> queue(QUEUE_SIZE_LARGE);

            bcemt_ThreadGroup pusherGroup;
            pusherGroup.addThreads(bdef_BindUtil::bind(&case9pusher,
                                                       &queue,
                                                       &done),
                                   NUM_PUSHERS_MORE);

            // Sleep for up to 2 ms just to let some stuff get into the queue
            bcemt_ThreadUtil::microSleep(rand() % 2000);
            queue.disable();
            if (veryVerbose) {
                cout << "     (after disabling, queue has " << queue.length()
                     << " items)" << endl;
            }

            int result;
            queue.removeAll();
            bsls_Stopwatch timer;
            timer.start();
            double elapsed;
            for (int i = 0; (elapsed = timer.elapsedTime() * 1000) <
                     EMPTY_VERIFY_MS; ++i) {
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
            NUM_THREADS = 6,
            NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : NUM_THREADS;

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
            NUM_THREADS = 6,
            NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : NUM_THREADS;

        if (verbose) cout << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "NUM PUSHERS: " << numPushers << endl
                          << "NUM POPPERS: " << numPoppers << endl;
        zerotst::runtest(numIterations, numPushers, numPoppers);
      } break;

      case -6: {
        // --------------------------------------------------------------------
        // STRESS TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "STRESS TEST -6" << endl
                          << "==============" << endl;
        enum {
            NUM_THREADS = 6,
            NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : NUM_THREADS;

        if (verbose) cout << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "NUM PUSHERS: " << numPushers << endl
                          << "NUM POPPERS: " << numPoppers << endl;

        indexqueue_zerotst::runtest(numIterations, numPushers, numPoppers);
      } break;
      case -7: {
        // --------------------------------------------------------------------
        // DISABLE TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "DISABLE TEST -7" << endl
                          << "===============" << endl;
        enum {
            NUM_THREADS = 6
        };

        int numPushers = argc > 2 ? atoi(argv[2]) : NUM_THREADS;

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
            NUM_THREADS = 6,
            NUM_ITERATIONS = 1000000,
            QUEUE_SIZE = 2047,
            PUSH_COUNT = 100
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : NUM_ITERATIONS;
        int numThreads = argc > 3 ? atoi(argv[3]) : NUM_THREADS;
        int queueSize = argc > 4 ? atoi(argv[4]) : QUEUE_SIZE;
        int pushCount = argc > 5 ? atoi(argv[5]) : PUSH_COUNT;

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
        // continuously.  They're interpreted modulo the queue size, but
        // the indexes themselves will abruptly roll over to 0 after
        // 2^32 operations on the queue.  For a period of time thereafter,
        // 'front' will be vastly larger than 'back' while the queue
        // size remains normal.
        //
        // Perform various multithreaded tests on a queue AFTER it has reached
        // this condition.  First, in a single thread, spin these indices up;
        // then, using multiple threads, stress-test operations that compare
        // front to back, such as pushing into a full queue or calculating
        // the length.
        //
        // ---------------------------------------------------------
        enum {
            QUEUE_SIZE = 30000 // fairly large to facilitate certain non-empty
                               // tests
        };

        // First, run our tests on a non-rolled-over queue
        bsl::cout << "Testing newly created queue..." << bsl::endl;
        bcemt_ThreadGroup workThreads;
        bces_AtomicInt doneFlag = 0;
        bcec_FixedQueue<int> youngQueue(QUEUE_SIZE);
        // Fill the queue up halfway.
        for (unsigned i = QUEUE_SIZE/2; i < QUEUE_SIZE; ++i) {
            youngQueue.pushBack(i);
        }

        workThreads.addThreads(bdef_BindUtil::bind(&rolloverLengthChecker,
                                                   &youngQueue,
                                                   &doneFlag),
                               2);

        bcemt_Turnstile pushTurnstile(6000.0);
        workThreads.addThread(bdef_BindUtil::bind(&rolloverPusher,
                                                  &youngQueue,
                                                  &doneFlag,
                                                  &pushTurnstile,
                                                  1));

        workThreads.addThread(bdef_BindUtil::bind(&rolloverPusher,
                                                  &youngQueue,
                                                  &doneFlag,
                                                  &pushTurnstile,
                                                  2));

        bcemt_Turnstile popTurnstile(3000.0);
        workThreads.addThreads(bdef_BindUtil::bind(&rolloverPopper,
                                                   &youngQueue,
                                                   &doneFlag,
                                                   &popTurnstile),
                               2);

        workThreads.joinAll();

        doneFlag = 0;

        bsl::cout << "\nIncrementing";

        bcec_FixedQueue<int> queue (QUEUE_SIZE);
        for (unsigned i = 0; i < 0xFFFFFFFF - QUEUE_SIZE; i += 5) {
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
        for (unsigned i = QUEUE_SIZE/2; i < QUEUE_SIZE; ++i) {
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
        workThreads.addThreads(bdef_BindUtil::bind(&rolloverLengthChecker,
                                                   &queue,
                                                   &doneFlag),
                               2);

        workThreads.addThread(bdef_BindUtil::bind(&rolloverPusher,
                                                  &queue,
                                                  &doneFlag,
                                                  &pushTurnstile,
                                                  1));

        workThreads.addThread(bdef_BindUtil::bind(&rolloverPusher,
                                                  &queue,
                                                  &doneFlag,
                                                  &pushTurnstile,
                                                  2));

        workThreads.addThreads(bdef_BindUtil::bind(&rolloverPopper,
                                                   &queue,
                                                   &doneFlag,
                                                   &popTurnstile),
                               2);

        workThreads.joinAll();
        bsl::cout << "Done.  testStatus = " << testStatus << bsl::endl;
      } break;

      case -10: {
        // ---------------------------------------------------------
        // tuning backoff
        // ---------------------------------------------------------
        cout << endl
             << "Tuning Backoff" << endl
             << "==============" << endl;
        enum {
            DEFAULT_PUSHPOP_THREADS=4,
            DEFAULT_ITERATIONS=20000,
            DEFAULT_WORKSPIN=75,
            DEFAULT_BACKOFF=2,
            QUEUE_SIZE = 2000
        };

        int numPushpopThreads = argc > 2 ? atoi(argv[2]) :
                                                DEFAULT_PUSHPOP_THREADS;
        int numIterations = argc > 3 ? atoi(argv[3]) :
                                                DEFAULT_ITERATIONS;
        int workSpin = argc > 4 ? atoi(argv[4]) :
                                                DEFAULT_WORKSPIN;
        int backoff = argc > 5 ? atoi(argv[5]) :
                                                DEFAULT_BACKOFF;

        bsl::cout << "Parameters:\n"
                  << "numPushPopThreads: " << numPushpopThreads << endl
                  << "numIterations: " << numIterations << endl
                  << "workSpin: " << workSpin << endl
                  << "backoff: " << backoff << endl;

        bcemt_Barrier barrier(numPushpopThreads + 1);

        bcemt_ThreadGroup tg;

        bcec_FixedQueue<int> queue(QUEUE_SIZE);
        queue.setBackoffLevel(backoff);

        tg.addThreads(bdef_BindUtil::bind(&Backoff::pushpopThread,
                                          &queue,
                                          &barrier,
                                          numIterations,
                                          workSpin),
                      numPushpopThreads);
        bsls_Stopwatch timer;

        barrier.wait();
        timer.start(true);
        tg.joinAll();
        timer.stop();

        double wall, user, system;
        timer.accumulatedTimes(&system, &user, &wall);

        cout << "Elapsed CPU time: " << user + system
             << " (" << user << "U, " << system << " S)"
             << endl;
        cout << "Elapsed wall time: " << wall << endl;
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
