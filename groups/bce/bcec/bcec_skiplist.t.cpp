// bcec_skiplist.t.cpp             -*-C++-*-

#include <bcec_skiplist.h>

#include <bcema_testallocator.h>
#include <bcemt_lockguard.h>
#include <bcemt_barrier.h>
#include <bcemt_thread.h>
#include <bcemt_threadgroup.h>
#include <bdet_timeinterval.h>

#include <bdef_bind.h>
#include <bdef_function.h>
#include <bslma_defaultallocatorguard.h>
#include <bdetu_systemtime.h>
#include <bdetu_timeinterval.h>
#include <bdet_datetime.h>
#include <bdetu_time.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>

#include <bsls_assert.h>

#include <bsl_cstdlib.h>
#include <bsl_c_stdlib.h>  // cstdlib does not include rand_r
#include <bsl_cmath.h>     // floor/ceil
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
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
#define NL() cout << endl;                    // End of line
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define Q_(X) cout << "<| " #X " |>, ";       // Q(X) without '\n'
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define L_ __LINE__                           // current Line number
#define V(X) { if (verbose) P(X) }            // Print in verbose mode

bcemt_Mutex coutMutex;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

//=============================================================================
//                  SOME HELPFUL EXTRAS
//-----------------------------------------------------------------------------
#define ASSERT_L(X, L) { \
   if (!(X)) { aSsErT(1, #X, L); }}

#define ASSERT_LL(X, Y, L1, L2) { \
   if ((X)!=(Y)) { \
            aSsErT(1, #X "==" #Y, L2); \
       cout << "(" << L1 << ", " \
            << #X << ": " << X << ", " \
            << #Y << ": " << Y << ") " << endl; \
   }}

#define ASSERTT(X) { aSsErTT(!(X), #X, __LINE__); }

void aSsErTT(int c, const char *s, int i)
{
    if (c) {
        coutMutex.lock();
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
        coutMutex.unlock();
    }
}

void case18(bcec_SkipList<int, int>* list, int numIterations, int level,
            bool unique)
{
    int id = bcemt_ThreadUtil::selfIdAsInt() + 2;

    if (0 <= level) {
        for (int i = 0; i < numIterations; ++i) {
            bcec_SkipList<int, int>::Pair *h;
            int key = i;
            if (unique) {
                do {
                    key += id * numIterations;
                } while (list->exists(key));
                ASSERTT(0 == list->addAtLevelUniqueRawR(&h, level, key, id));
            }
            else {
                list->addAtLevelRawR(&h, level, key, id);
            }
            list->releaseReferenceRaw(h);
            list->removeAll();
        }
    }
    else {
        for (int i = 0; i < numIterations; ++i) {
            bcec_SkipList<int, int>::Pair *h;
            int key = i;
            if (unique) {
                do {
                    key += id * numIterations;
                } while (list->exists(key));
                ASSERTT(0 == list->addUniqueRawR(&h, key, id));
            }
            else {
                list->addRawR(&h, key, id);
            }
            list->releaseReferenceRaw(h);
            list->removeAll();
        }
    }
}

class CountedDelete
{
    static bces_AtomicInt deleteCount;
    bool isTemp;

public:
    CountedDelete() : isTemp(true) {}
    CountedDelete(const CountedDelete& rhs) : isTemp(false) {}

    ~CountedDelete() {
        if (!isTemp) {
            ++deleteCount;
        }
    }

    static int getDeleteCount() {
        return deleteCount;
    }

    CountedDelete& operator= (const CountedDelete& rhs) {
        isTemp = false;
        return *this;
    }
};

bces_AtomicInt CountedDelete::deleteCount = 0;

void case20(bcec_SkipList<int, CountedDelete>* list, int maxRefCount)
{
    unsigned seed = (unsigned)bcemt_ThreadUtil::selfIdAsInt();

    // Increment the ref count up to maxRefCount, then decrease.  While
    // incrementing, 10% of the time throw in a decrement.

    bcec_SkipList<int, CountedDelete>::Pair *front;
    ASSERT(0 == list->frontRaw(&front));

    for (int numReferencesAdded = 1; numReferencesAdded < maxRefCount; )
    {
        list->addPairReferenceRaw(front);
#if defined BSLS_PLATFORM__OS_WINDOWS
        double random = rand() / (double)RAND_MAX;
#else
        double random = rand_r(&seed) / (double)RAND_MAX;
#endif

        if (random < 0.1) {
            list->releaseReferenceRaw(front);
        }
        else {
            numReferencesAdded++;
        }
    }
    for (int i = 0; i < maxRefCount; ++i) {
        list->releaseReferenceRaw(front);
    }
}

void case19(bcec_SkipList<int, int>* list, int numIterations, int level,
            bool unique, bool getHandles)
{
    int id = bcemt_ThreadUtil::selfIdAsInt() + 2;

    bcec_SkipListPairHandle<int,int> uniqueH;

    if (0 <= level) {
        for (int i = 0; i < numIterations; ++i) {
            if (getHandles) {
                bcec_SkipList<int, int>::Pair *h;
                int key = i;
                if (unique) {
                    do {
                        key += id * numIterations;
                    } while (0 == list->find(&uniqueH, key));
                    ASSERTT(0 ==
                            list->addAtLevelUniqueRaw(&h, level, key, id));
                }
                else {
                    list->addAtLevelRaw(&h, level, key, id);
                }
                list->releaseReferenceRaw(h);
            }
            else {
                int key = i;
                if (unique) {
                    do {
                        key += id * numIterations;
                    } while (0 == list->find(&uniqueH, key));
                    ASSERTT(0 == list->addAtLevelUniqueRaw(0, level, key, id));
                }
                else {
                    list->addAtLevelRaw(0, level, key, id);
                }
            }
            list->removeAll();
        }
    }
    else {
        for (int i = 0; i < numIterations; ++i) {
            if (getHandles) {
                bcec_SkipList<int, int>::PairHandle h;
                int key = i;
                if (unique) {
                    do {
                        key += id * numIterations;
                    } while (0 == list->find(&uniqueH, key));
                    ASSERTT(0 == list->addUnique(&h, key, id));
                }
                else {
                    list->add(&h, key, id);
                }
            }
            else {
                int key = i;
                if (unique) {
                    do {
                        key += id * numIterations;
                    } while (0 == list->find(&uniqueH, key));
                    ASSERTT(0 == list->addUnique(key, id));
                }
                else {
                    list->add(key, id);
                }
            }
            list->removeAll();
        }
    }
}

class SimpleScheduler
{
    // DATA
    typedef bcec_SkipList<bdet_Datetime, bdef_Function<void(*)()> > List;

    List                                                    d_list;
    bcemt_ThreadUtil::Handle                                d_dispatcher;
    bcemt_Condition                                         d_notEmptyCond;
    bcemt_Barrier                                           d_startBarrier;
    bcemt_Mutex                                             d_condMutex;
    volatile int                                            d_doneFlag;

    // PRIVATE METHODS
    void dispatcherThread()
    {
        d_startBarrier.wait();

        while (!d_doneFlag) {
            List::PairHandle firstItem;
            if (0 == d_list.front(&firstItem)) {
                // The list is not empty.

                bdet_TimeInterval when;
                bdetu_TimeInterval::convertToTimeInterval(&when,
                               firstItem.key() -
                               bdetu_SystemTime::nowAsDatetimeUtc());
                if (when.totalSecondsAsDouble() <= 0) {
                    // Execute now and remove from schedule, then iterate.

                    d_list.remove(firstItem);
                    firstItem.data()();
                }
                else {
                    // Wait until the first scheduled item is due.

                    d_condMutex.lock();
                    List::PairHandle newFirst;
                    if (!d_doneFlag && (0 != d_list.front(&newFirst) ||
                                        newFirst.key() == firstItem.key())) {
                        d_notEmptyCond.timedWait(&d_condMutex,
                                               bdetu_SystemTime::now() + when);
                    }
                    d_condMutex.unlock();
                }
            }
            else {
                // The list is empty; wait on the condition variable.

                d_condMutex.lock();
                if (d_list.isEmpty() && !d_doneFlag) {
                    d_notEmptyCond.wait(&d_condMutex);
                }
                d_condMutex.unlock();
            }
        }
    }

public:

    //CREATORS
    SimpleScheduler(bslma_Allocator *basicAllocator = 0)
    : d_list(basicAllocator)
    , d_startBarrier(2)
    , d_doneFlag(false)
    {
        int rc = bcemt_ThreadUtil::create(
                    &d_dispatcher,
                    bdef_BindUtil::bind(&SimpleScheduler::dispatcherThread,
                                        this));
        BSLS_ASSERT(0 == rc);
        d_startBarrier.wait();
    }

    ~SimpleScheduler()
    {
        stop();
    }

    void stop() {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_condMutex);
        if (bcemt_ThreadUtil::invalidHandle() != d_dispatcher) {
            bcemt_ThreadUtil::Handle dispatcher = d_dispatcher;
            d_doneFlag = true;
            d_notEmptyCond.signal();
            {
                bcemt_LockGuardUnlock<bcemt_Mutex> g(&d_condMutex);
                bcemt_ThreadUtil::join(dispatcher);
            }
            d_dispatcher = bcemt_ThreadUtil::invalidHandle();
        }
    }

    void scheduleEvent(const bdef_Function<void(*)()>& event,
                       const bdet_Datetime& when)
    {
        // Use 'addR' since this event will probably be placed near the end
        // of the list.

        bool newFrontFlag;
        d_list.addR(when, event, &newFrontFlag);
        if (newFrontFlag) {
            // This event is scheduled before all other events.  Wake up
            // the dispatcher thread.

            d_condMutex.lock();
            d_notEmptyCond.signal();
            d_condMutex.unlock();
        }
    }
};

struct DATA {
    int         l;
    int         key;
    const char *data;
    int         level;
};

struct IDATA {
    int l;
    int key;
    int data;
    int level;
};

void case16Produce (bcec_SkipList<int, int> *list,
                    bces_AtomicInt          *done)
{
    int count = 0;
    while (!(*done)) {
        if (0 > ++count) {
            count = 0;
        }
        bcec_SkipList<int, int>::Pair *h;
        list->addRaw(&h, count, count);
        bcemt_ThreadUtil::yield();
        list->releaseReferenceRaw(h);
    }
}

void case16Consume(bcec_SkipList<int, int> *list,
                   bces_AtomicInt          *done)
{
    while (!(*done)) {
        bcec_SkipList<int, int>::Pair *h1;
        ASSERT(0 == list->frontRaw(&h1));
        bcec_SkipList<int, int>::Pair *h2;
        ASSERT(0 == list->findRRaw(&h2, h1->key()));
        ASSERT(0 != h2);
        ASSERT(h2->key() == h2->key());
        list->releaseReferenceRaw(h1);
        list->releaseReferenceRaw(h2);
        list->remove(h1);
    }
}

template<class SKIPLIST, class ARRAY>
void populate(SKIPLIST *list, const ARRAY& array, int length)
{
    for (int i=0; i<length; i++) {
        list->add(array[i].key, array[i].data);
    }
}

template<class SKIPLIST, class ARRAY>
void populateEx(SKIPLIST *list, const ARRAY& array, int length)
{
    for (int i=0; i<length; i++) {
        list->addAtLevelRaw(0, array[i].level, array[i].key, array[i].data);
    }
}

template<class SKIPLIST, class ARRAY>
void verify(SKIPLIST *list, const ARRAY& array, int length, int line)
{
    // scan forward using 'raw' and 'skip' methods; and also using 'front'
    // and 'next' (non-raw) methods, in parallel.
    typename SKIPLIST::Pair *p;
    typename SKIPLIST::PairHandle h;
    list->frontRaw(&p);
    list->front(&h);

    int i;

    for (i=0; i<length && p; i++) {
        ASSERT_LL(p->key(), array[i].key, array[i].l, line);
        ASSERT_LL(p->data(), array[i].data, array[i].l, line);

        ASSERT_LL(h.key(), array[i].key, array[i].l, line);
        ASSERT_LL(h.data(), array[i].data, array[i].l, line);

        int ret = list->skipForwardRaw(&p);
        ASSERT(ret==0);

        ret = list->next(&h, h);
        ASSERT(ret == 0 || i == length-1);
    }

    if (p) {
        list->releaseReferenceRaw(p);
    }

    ASSERT_L(i==length, line);
    ASSERT_L(p==0, line);
}

template<class SKIPLIST, class ARRAY>
void verifyReverse(const SKIPLIST& list,
                   const ARRAY&    array,
                   int             length,
                   int             line)
{
    // scan backward  using 'skip' and also 'previous', in parallel

    typename SKIPLIST::PairHandle p, p2;
    list.back(&p);
    list.back(&p2);

    int i;

    for (i=length-1; i>=0 && p; i--) {
        ASSERT_LL(p.key(), array[i].key, array[i].l, line);
        ASSERT_LL(p.data(), array[i].data, array[i].l, line);

        ASSERT_LL(p2.key(), array[i].key, array[i].l, line);
        ASSERT_LL(p2.data(), array[i].data, array[i].l, line);

        int ret = list.skipBackward(&p);
        ASSERT(ret==0);

        ret = list.previous(&p2, p2);
        ASSERT(ret == 0 || i == 0);
    }

    ASSERT_L(i==-1, line);
    ASSERT_L(p==0, line);
}

template<class SKIPLIST, class ARRAY>
void verifyEx(SKIPLIST* list, const ARRAY& array, int length, int line)
{
    typename SKIPLIST::Pair *p;
    list->frontRaw(&p);

    int i;

    for (i=0; i<length && p; i++) {
        ASSERT_LL(p->key(), array[i].key, array[i].l, line);
        ASSERT_LL(p->data(), array[i].data, array[i].l, line);
        ASSERT_LL(list->level(p), array[i].level, array[i].l, line);

        int ret = list->skipForwardRaw(&p);
        ASSERT(ret==0);
    }

    if (p) {
        list->releaseReferenceRaw(p);
    }

    ASSERT_L(i==length, line);
    ASSERT_L(p==0, line);
}

#define POPULATE_LIST(LP, A)       { populate(LP, A, sizeof(A)/sizeof(A[0])); }
#define POPULATE_LIST_EX(LP, A)  { populateEx(LP, A, sizeof(A)/sizeof(A[0])); }

#define VERIFY_LIST(L, A) { verify(&(L), A, sizeof(A)/sizeof(A[0]), __LINE__);}

#define VERIFY_LIST_REVERSE(L, A) { \
                       verifyReverse(L, A, sizeof(A)/sizeof(A[0]), __LINE__); }

#define VERIFY_LIST_EX(L, A) { \
                         verifyEx(&(L), A, sizeof(A)/sizeof(A[0]), __LINE__); }

//=============================================================================
//                      CASE 101 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_SKIPLIST_TEST_CASE_101 {

typedef bcec_SkipList<int,int> List;

enum {
    NUM_THREADS    = 12,
    NUM_ITERATIONS = 100
};

void threadFunc(List                *list,
                int                  numIterations,
                int                  threadNum)
{
    for (int j=0; j<numIterations; j++) {
        for(int i=0; i<numIterations; i++) {
            list->add(1000*i + threadNum, -1000*i - threadNum);
        }

        for(int i=0; i<numIterations; i++) {
            int k = 1000*i + threadNum;
            int d = -k;
            List::Pair *h;
            ASSERT(0 == list->findRaw(&h, k));
            ASSERT(h);
            ASSERT(h->key()==k);
            ASSERT(h->data()==d);
            list->remove(h);
            list->releaseReferenceRaw(h);
        }
    }
}

void run()
{
    if (verbose) cout << endl
                      << "The thread-safety test" << endl
                      << "======================" << endl;

    IDATA VALUES1[] = {
        { L_ , 1, 1, 0},
        { L_ , 3, 3, 1},
        { L_ , 0, 0, 2},
        { L_ , 2, 2, 3},
        { L_ , 4, 4, 4},
    };

    IDATA VALUES2[] = {
        { L_ , 0, 0, 2},
        { L_ , 1, 1, 0},
        { L_ , 2, 2, 3},
        { L_ , 3, 3, 1},
        { L_ , 4, 4, 4},
    };

    List list;
    POPULATE_LIST_EX(&list, VALUES1);

    bcemt_ThreadUtil::Handle threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        bcemt_ThreadUtil::create(&threads[i],
                                 bdef_BindUtil::bind(&threadFunc,
                                                     &list,
                                                     (int)NUM_ITERATIONS,
                                                     i+1));
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        bcemt_ThreadUtil::join(threads[i]);
    }

    VERIFY_LIST_EX(list, VALUES2);
}

} // close namespace BCEC_SKIPLIST_TEST_CASE_101

//=============================================================================
//                      CASE -100 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_SKIPLIST_TEST_CASE_MINUS_100 {

static bces_AtomicInt currentTime = 0;
typedef bcec_SkipList<bdet_TimeInterval,int> TimeQ;

enum {
    NUM_THREADS    = 4,
    NUM_ITERATIONS = 100,
    SEND_COUNT = 1000,
    RCV_COUNT = 900,
    DELAY = 500
};

void threadFunc(TimeQ               *timeQueue,
                int                  numIterations,
                int                  sendCount,
                int                  rcvCount,
                int                  delay)
{
    bsl::vector<TimeQ::Pair*> timers;
    timers.resize(sendCount);

    bsls_Stopwatch sw;

    for(int i=0; i<numIterations; i++) {
        if( verbose ) {
            sw.start();
        }

        // "send" messages
        for(int snd=0; snd<sendCount; snd++) {
            currentTime++;
            bdet_TimeInterval t(currentTime + delay, 0);
            timeQueue->addRaw(&timers[snd], t, delay);
        }

        // "receive" replies
        for(int rcv=0; rcv<rcvCount; rcv++) {
            timeQueue->remove(timers[rcv]);
            timeQueue->releaseReferenceRaw(timers[rcv]);
        }

        // "resend" replies
        bdet_TimeInterval now(currentTime, 0);
        while(1) {
            TimeQ::Pair *resubmit;
            if (0 != timeQueue->frontRaw(&resubmit)) {
                break;
            }

            const bdet_TimeInterval& k = resubmit->key();
            if (k <= now) {
                timeQueue->remove(resubmit);
                int newdelay = resubmit->data() * 2;
                bdet_TimeInterval t(currentTime + newdelay, 0);
                timeQueue->add(t, newdelay);
                timeQueue->releaseReferenceRaw(resubmit);
            }
            else {
                timeQueue->releaseReferenceRaw(resubmit);
                break;
            }
        }

        // clean up remaining handles
        for(int cln=rcvCount; cln<sendCount; cln++) {
            timeQueue->releaseReferenceRaw(timers[cln]);
        }

        if( verbose ) {
            sw.stop();

            int iteration = i;
            double elapsed = sw.elapsedTime();
            P(iteration);
            P(elapsed);
        }
    }
}

void run()
{
    if (verbose) cout << endl
                      << "The router simulation (kind of) test" << endl
                      << "====================================" << endl;

    TimeQ timeQueue;

    bcemt_ThreadUtil::Handle threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        bcemt_ThreadUtil::create(&threads[i],
                                 bdef_BindUtil::bind(&threadFunc,
                                                     &timeQueue,
                                                     (int)NUM_ITERATIONS,
                                                     (int)SEND_COUNT,
                                                     (int)RCV_COUNT,
                                                     (int)DELAY));
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        bcemt_ThreadUtil::join(threads[i]);
    }

}

} // close namespace BCEC_SKIPLIST_TEST_CASE_MINUS_100

namespace {
void pushBackWrapper(bsl::vector<int> *vector,
                     int               item)
{
    vector->push_back(item);
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

    switch (test) { case 0:  // Zero is always the leading case.
      case 23: {
        // --------------------------------------------------------------------
        // DISTRIBUTION TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nDISTRIBUTION TEST"
                               << "\n=================" << bsl::endl;

        enum { NUM_ITERATIONS = 60000 };

        typedef bcec_SkipList<int,int> Obj;
        typedef Obj::PairHandle H;

        Obj list;

        int n[32] = {0};

        for (int i=0; i<NUM_ITERATIONS; i++) {
            H h;
            list.add(&h, 0, 0);
            int l = list.level(h);
            ASSERT(l>=0);
            ASSERT(l<32);
            n[l]++;
            list.remove(h);
        }

        double exp = (NUM_ITERATIONS / 4) * 3;
        double div = 0.1;

        for (int i=0; i<32; i++) {

            V(i);
            V(exp);
            V(div);
            V(n[i]);
            V("**********");

            ASSERT(n[i] <= bsl::ceil(exp * (1.0 + div)));
            ASSERT(n[i] >= bsl::floor(exp * (1.0 - div)));
            exp /= 4;
            div *= 2;
        }

      }  break;
      case 22: {
        // ----------------------------------------------------
        // Value-semantic tests

        enum {
            COPY_A_TO_B,
            COPY_B_TO_A,
            ADD_TO_A,
            ADD_TO_B,
            CLEAR_A,
            CLEAR_B
        };

        struct Parameters {
            const char* d_key;
            const char* d_data;
            const char* d_expectedValue1;
            const char* d_expectedValue2;
            bool        d_expectedEqual;
            int         d_operation;
            int         d_line;
        } parameters[] = {
            // *** Tests with both lists empty:
        {
            "", "",
            "[]", "[]", true,
            COPY_A_TO_B,
            L_
        },
        {
            "", "",
            "[]", "[]", true,
            COPY_B_TO_A,
            L_
        },

           //  *** Tests with one list empty:
        {
            "alpha", "the",
            "[[ (level = 2) alpha => the ]]", "[]",
            false,
            ADD_TO_A,
            L_
        }
        };

        const int numParameters = sizeof(parameters) / sizeof(Parameters);

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
        {
            bcec_SkipList<bsl::string, bsl::string> mA(&ta), mB(&ta);

            // set the list level to 2
            mA.addAtLevelUniqueRaw(0, 2, "", "");
            mB.addAtLevelUniqueRaw(0, 2, "", "");
            mA.removeAll();
            mB.removeAll();

            for (int i = 0; i < numParameters; ++i) {
                const Parameters& p = parameters[i];

                bcec_SkipList<bsl::string, bsl::string> mA2(mA, &ta);
                bcec_SkipList<bsl::string, bsl::string> mB2(mB, &ta);

                switch (p.d_operation) {
                case COPY_A_TO_B:
                    mB = mA;
                    mB2 = mA;
                break;
                case COPY_B_TO_A:
                    mA = mB;
                    mA2 = mB;
                    break;
                case ADD_TO_A:
                    LOOP_ASSERT(p.d_line,
                                0 == mA.addAtLevelUniqueRaw
                                (0, 2, p.d_key, p.d_data));
                    LOOP_ASSERT(p.d_line,
                                0 == mA2.addAtLevelUniqueRaw
                                (0, 2, p.d_key, p.d_data));
                    break;
                case ADD_TO_B:
                    LOOP_ASSERT(p.d_line,
                                0 == mB.addAtLevelUniqueRaw
                                (0, 2, p.d_key, p.d_data));
                    LOOP_ASSERT(p.d_line,
                                0 == mB2.addAtLevelUniqueRaw
                                (0, 2, p.d_key, p.d_data));
                    break;
                case CLEAR_A:
                    LOOP_ASSERT(p.d_line, mA.removeAll() ==
                                mA2.removeAll());
                    break;
                 case CLEAR_B:
                    LOOP_ASSERT(p.d_line, mB.removeAll() ==
                                mB2.removeAll());
                    break;
                }

                LOOP_ASSERT(p.d_line, mA == mA2);
                LOOP_ASSERT(p.d_line, mA.length() == mA2.length());
                LOOP_ASSERT(p.d_line, mB == mB2);
                LOOP_ASSERT(p.d_line, mB.length() == mB2.length());
                LOOP_ASSERT(p.d_line, !(mA != mA2));
                LOOP_ASSERT(p.d_line, !(mB != mB2));
                LOOP_ASSERT(p.d_line, (mA == mB) == p.d_expectedEqual);
                LOOP_ASSERT(p.d_line, (mA.length() == mB.length()) ==
                                      p.d_expectedEqual);
                LOOP_ASSERT(p.d_line, (mA != mB) == !p.d_expectedEqual);

                bsl::ostringstream aValue, bValue;
                aValue << mA;
                bsl::string expectedA = p.d_expectedValue1;
                bsl::string expectedB = p.d_expectedValue2;
                LOOP3_ASSERT(p.d_line, expectedA, aValue.str(),
                             expectedA == aValue.str());

                bValue << mB;
                LOOP3_ASSERT(p.d_line, expectedB, bValue.str(),
                             expectedB == bValue.str());
            }
        }
        ASSERT(0 == da.numBytesInUse());
        V(ta.numAllocations());
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 21: {
        // ----------------------------------------------------
        // Testing 'skipBackward' and 'skipForward'
        //
        // Test appropriate behavior of 'skipBackward' and 'skipForward'
        // and their "raw" variants in a single-threaded environment.
        //
        // Concerns:
        //   * skipping at the front/end resets the handle (as appropriate)
        //     and returns 0
        //   * skipping a removed item returns BCEC_NOT_FOUND
        //   * reference counting is correct
        // ----------------------------------------------------
        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bcec_SkipList<int, bsl::string> SkipList;
        {
            SkipList obj(&ta);

            SkipList::PairHandle h1, h2, h3;
            SkipList::Pair *p4, *p5, *p6;

            obj.add(&h1, 1, "the");
            obj.add(&h2, 2, "quick");
            obj.add(&h3, 3, "brown");
            obj.addRaw(&p4, 4, "fox");
            obj.addRaw(&p5, 5, "jumped");
            obj.addRaw(&p6, 6, "over");

            ASSERT(0 == obj.skipForward(&h2));
            ASSERT(h2.key() == 3);
            ASSERT(h2.data() == "brown");

            ASSERT(0 == obj.skipBackward(&h2));
            ASSERT(h2.key() == 2);
            ASSERT(h2.data() == "quick");

            ASSERT(0 == obj.skipForwardRaw(&p5));
            ASSERT(p5->key() == 6);
            ASSERT(p5->data() == "over");

            ASSERT(0 == obj.skipBackwardRaw(&p5));
            ASSERT(p5->key() == 5);
            ASSERT(p5->data() == "jumped");

            ASSERT(0 == obj.skipBackward(&h1));
            ASSERT(!h1.isValid());

            ASSERT(0 == obj.skipForwardRaw(&p6));
            ASSERT(0 == p6);

            SkipList::Pair *f;
            ASSERT(0 == obj.frontRaw(&f));
            ASSERT(0 == obj.skipBackwardRaw(&f));
            ASSERT(0 == f);

            SkipList::PairHandle b;
            ASSERT(0 == obj.back(&b));
            ASSERT(0 == obj.skipForward(&b));
            ASSERT(!b.isValid());

            obj.remove(h3);
            ASSERT(SkipList::BCEC_NOT_FOUND == obj.skipBackward(&h3));
            ASSERT(h3.isValid());
            ASSERT(SkipList::BCEC_NOT_FOUND == obj.skipForward(&h3));
            ASSERT(h3.isValid());

            obj.remove(p4);
            ASSERT(SkipList::BCEC_NOT_FOUND == obj.skipBackwardRaw(&p4));
            ASSERT(0 != p4);
            ASSERT(SkipList::BCEC_NOT_FOUND == obj.skipForwardRaw(&p4));
            ASSERT(0 != p4);

            obj.releaseReferenceRaw(p4);
            obj.releaseReferenceRaw(p5);
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 20: {
        // ----------------------------------------------------
        // 'addPairReferenceRaw' thread safety test
        // ----------------------------------------------------
        if (verbose) cout << endl
                          << "addPairReferenceRaw Safety test" << endl
                          << "===============================" << endl;

        typedef bcec_SkipList<int, CountedDelete> SkipList;

        enum {
            NUM_THREADS = 5
        };

        SkipList mX;
        SkipList::Pair *handle;
        bcemt_ThreadGroup tg;

        mX.addRaw(&handle, 1, CountedDelete());

        int numThreads = NUM_THREADS;
        if (verbose) {
            numThreads = atoi(argv[2]);
            if (numThreads == 0) {
                numThreads = NUM_THREADS;
            }
            else {
                cout << "Running with " << numThreads << " threads."
                     << endl;
            }
        }
        int maxRefCount = 4000 / numThreads;

        tg.addThreads(bdef_BindUtil::bind(&case20, &mX, maxRefCount),
                      numThreads);
        tg.joinAll();

        mX.remove(handle);
        mX.releaseReferenceRaw(handle);
        ASSERT(1 == CountedDelete::getDeleteCount());

      } break;
      case 19: {
        // ----------------------------------------------------
        // 'removeAll' thread safety test (add instead of addR)
        // ----------------------------------------------------
        if (verbose) cout << endl
                          << "RemoveAll Safety test (forward)" << endl
                          << "===============================" << endl;

        typedef bcec_SkipList<int, int> SkipList;

        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 1000
        };

        int numThreads = NUM_THREADS;
        if (verbose) {
            numThreads = atoi(argv[2]);
            if (numThreads == 0) {
                numThreads = NUM_THREADS;
            }
            else {
                cout << "Running with " << numThreads << " threads."
                     << endl;
            }
        }

        int numIterations = NUM_ITERATIONS;
        if (veryVerbose) {
            numIterations = atoi(argv[3]);
            if (numIterations == 0) {
                numIterations = NUM_ITERATIONS;
            }
            else {
                cout << "Running with " << numIterations << " iterations."
                     << endl;
            }
        }

        bcema_TestAllocator ta;
        {
            SkipList mX(&ta);
            bcemt_ThreadGroup tg;

            if (verbose) {
                cout << "Phase 1: no handles" << endl;
            }

            tg.addThreads(bdef_BindUtil::bind(&case19, &mX,
                                              numIterations, -1, false,
                                              false),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 2: unique values" << endl;
            }

            tg.addThreads(bdef_BindUtil::bind(&case19, &mX,
                                              numIterations, -1, true, true),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 3: level 0" << endl;
            }

            tg.addThreads(bdef_BindUtil::bind(&case19, &mX,
                                              numIterations, 0, false, true),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 4: random level" << endl;
            }

            tg.addThreads(bdef_BindUtil::bind(&case19, &mX,
                                              numIterations, -1, false, true),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 18: {
        // ------------------------------------
        // 'removeAll' thread safety test
        // ------------------------------------
        if (verbose) cout << endl
                          << "RemoveAll Safety test" << endl
                          << "=====================" << endl;

        typedef bcec_SkipList<int, int> SkipList;

        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 1000
        };

        int numThreads = NUM_THREADS;
        if (verbose) {
            numThreads = atoi(argv[2]);
            if (numThreads == 0) {
                numThreads = NUM_THREADS;
            }
            else {
                cout << "Running with " << numThreads << " threads."
                     << endl;
            }
        }

        int numIterations = NUM_ITERATIONS;
        if (veryVerbose) {
            numIterations = atoi(argv[3]);
            if (numIterations == 0) {
                numIterations = NUM_ITERATIONS;
            }
            else {
                cout << "Running with " << numIterations << " iterations."
                     << endl;
            }
        }

        bcema_TestAllocator ta;
        {
            SkipList mX(&ta);

            if (verbose) {
                cout << "Phase 1: level 0" << endl;
            }

            bcemt_ThreadGroup tg;
            tg.addThreads(bdef_BindUtil::bind(&case18, &mX,
                                              numIterations, 0, false),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 2: random level" << endl;
            }

            tg.addThreads(bdef_BindUtil::bind(&case18, &mX,
                                              numIterations, -1, false),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 3: unique values" << endl;
            }

            tg.addThreads(bdef_BindUtil::bind(&case18, &mX,
                                              numIterations, -1, true),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 17: {
        // ------------------------------------
        // 'remove' test
        // ------------------------------------
        if (verbose) cout << endl
                          << "Remove test" << endl
                          << "===========" << endl;

        typedef bcec_SkipList<int, CountedDelete> SkipList;

        SkipList list;
        {
            list.add(1000, CountedDelete());
        }
        SkipList::PairHandle firstItem;
        ASSERT(0 == list.front(&firstItem));

        ASSERT(!list.isEmpty());

        list.remove(firstItem);
        ASSERT(list.isEmpty());
        ASSERT(0 == CountedDelete::getDeleteCount());
        firstItem.release();
        ASSERT(1 == CountedDelete::getDeleteCount());

        ASSERT(list.isEmpty());

        {
            list.add(900, CountedDelete());
        }
        ASSERT(0 == list.front(&firstItem));
        ASSERT(1 == list.length());

        firstItem.release();
        ASSERT(1 == CountedDelete::getDeleteCount());
        {
            list.add(1000, CountedDelete());
        }
        ASSERT(0 == list.front(&firstItem));
        list.remove(firstItem);
        ASSERT(1 == CountedDelete::getDeleteCount());

        ASSERT(0 == list.front(&firstItem));
        ASSERT(2 == CountedDelete::getDeleteCount());

        list.remove(firstItem);

        ASSERT(list.isEmpty());
        ASSERT(2 == CountedDelete::getDeleteCount());
        firstItem.release();
        ASSERT(3 == CountedDelete::getDeleteCount());

      } break;

      case 16: {
        // ------------------------------------
        // Node deallocation thread safety test
        // ------------------------------------
        if (verbose) cout << endl
                          << "Node deallocation thread safety test" << endl
                          << "====================================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            typedef bcec_SkipList<int, int> SkipList;
            SkipList list(&ta);

            bces_AtomicInt done = 0;

            // TBD
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;

      case 15: {
        // --------------------------------------------------------------------
        // findR test
        // --------------------------------------------------------------------
        DATA VALUES1[] = {
            // line,  key,  data,  level
            { L_ ,       1,   "1",    0},
            { L_ ,       3,   "3",    1},
            { L_ ,       0,   "0",    2},
            { L_ ,       2,   "2",    3},
            { L_ ,       4,   "4",    4},
        };

        DATA VALUES2[] = {
            { L_ , 0, "0", 2},
            { L_ , 1, "1", 0},
            { L_ , 3, "3", 1},
            { L_ , 4, "4", 4},
        };

        if (verbose) cout << endl
                          << "findR test" << endl
                          << "==============" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bcec_SkipList<int, bsl::string> SkipList;
        {
            SkipList Obj(&ta);
            POPULATE_LIST_EX(&Obj, VALUES1);

            SkipList::PairHandle h;
            ASSERT(0 != Obj.find(&h, 10));

            ASSERT(0 == Obj.findR(&h, 2));
            Obj.remove(h);

            VERIFY_LIST_EX(Obj, VALUES2);

            if (veryVerbose) {
                Obj.print(cout);
            } else if (verbose) Obj.print(cout, 0, -1) << endl;
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 14: {
        // --------------------------------------------------------------------
        // updateR
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "updateR test" << endl
                          << "=============" << endl;

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            bcema_TestAllocator da(veryVeryVerbose);
            bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
            typedef bcec_SkipList<int, bsl::string> SkipList;
            {
                SkipList Obj(&ta);
                SkipList::PairHandle h;

                SkipList::PairHandle h0;
                SkipList::PairHandle h1;
                SkipList::PairHandle h2;
                SkipList::PairHandle h3;

                int ret;

                Obj.addR(&h0, 0, "0");
                Obj.addR(&h1, 1, "1");
                Obj.addR(&h2, 2, "2");
                Obj.addR(&h3, 3, "3");
                if (verbose) Obj.print(cout, 0, -1) << endl;

                ret = Obj.updateR(h3, 3, 0, false);
                ASSERT(ret==0);
                if (verbose) Obj.print(cout, 0, -1) << endl;

                ret = Obj.updateR(h3, 0, 0, false);
                ASSERT(ret==SkipList::BCEC_DUPLICATE);
                if (verbose) Obj.print(cout, 0, -1) << endl;

                Obj.updateR(h3, 10);
                if (verbose) Obj.print(cout, 0, -1) << endl;

                Obj.updateR(h0, 13);
                if (verbose) Obj.print(cout, 0, -1) << endl;

                Obj.updateR(h1, 12);
                if (verbose) Obj.print(cout, 0, -1) << endl;

                Obj.updateR(h2, 11);
                if (verbose) Obj.print(cout, 0, -1) << endl;

                Obj.popFront(&h);
                ASSERT("3" == h.data());
                V(h.data());

                Obj.popFront(&h);
                ASSERT("2" == h.data());
                V(h.data());

                Obj.popFront(&h);
                ASSERT("1" == h.data());
                V(h.data());

                Obj.popFront(&h);
                ASSERT("0" == h.data());
                V(h.data());
            }
            V(da.numBytesInUse());
            ASSERT(0 == da.numBytesInUse());
            V(ta.numBytesInUse());
            ASSERT(0 == ta.numBytesInUse());
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // isNewTop with addR
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "isNewTop with addR" << endl
                          << "==================" << endl;

        {
            typedef bcec_SkipList<int, const char *> SkipList;

            bool isNewTop;

            SkipList Obj;
            SkipList::PairHandle h;

            Obj.addR(2, "2");

            Obj.addR(3, "3", &isNewTop);
            ASSERT(!isNewTop);

            Obj.addR(1, "1", &isNewTop);
            ASSERT(isNewTop);

            Obj.addR(&h, 0, "0", &isNewTop);
            ASSERT(isNewTop);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // more addR
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "More addR" << endl
                          << "=========" << endl;

        {
            typedef bcec_SkipList<int,bsl::string> SkipList;

            SkipList Obj;
            ASSERT(Obj.isEmpty());
            Obj.addR(3, "3");
            Obj.addR(2, "2");
            Obj.addR(0, "0");
            Obj.addR(1, "1");
            ASSERT(!Obj.isEmpty());
            ASSERT(4 == Obj.length());
            SkipList::PairHandle h;
            int ret;
            ret = Obj.popFront(&h);
            ASSERT(ret==0);
            V(h.data());
            V(h.key());
            ASSERT("0" == h.data());

            ret = Obj.popFront(&h);
            ASSERT(ret==0);
            V(h.data());
            V(h.key());

            ASSERT("1" == h.data());

            ret = Obj.popFront(&h);
            ASSERT(1 == Obj.length());
            ASSERT(ret==0);

            V(h.data());
            V(h.key());
            ASSERT("2" == h.data());

            ret = Obj.popFront(&h);
            ASSERT(ret==0);

            V(h.data());
            V(h.key());

            ASSERT("3" == h.data());

            ASSERT(Obj.isEmpty());

            ret = Obj.popFront(&h);
            ASSERT(ret==SkipList::BCEC_NOT_FOUND);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // addR
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "addR test" << endl
                          << "=========" << endl;

        {
            typedef bcec_SkipList<int, const char *> SkipList;

            SkipList Obj;
            ASSERT(Obj.isEmpty());
            Obj.addR(0, "Hi");
            ASSERT(!Obj.isEmpty());
            ASSERT(1 == Obj.length());
            SkipList::PairHandle h;
            int ret = Obj.popFront(&h);
            ASSERT(0 == Obj.length());
            ASSERT(ret==0);
            ASSERT(Obj.isEmpty());
            bsl::string d = h.data();
            V(d);
            ASSERT(d=="Hi");
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // removeAll
        // --------------------------------------------------------------------
        DATA VALUES1[] = {
            { L_ , 1, "1", 0},
            { L_ , 3, "3", 1},
            { L_ , 0, "0", 2},
            { L_ , 2, "2", 3},
            { L_ , 4, "4", 4},
        };

        DATA VALUES2[] = {
            { L_ , 0, "0", 2},
            { L_ , 1, "1", 0},
            { L_ , 2, "2", 3},
            { L_ , 3, "3", 1},
            { L_ , 4, "4", 4},
        };

        DATA VALUES3[] = {
            { L_ , 0, "0", 0},
            { L_ , 1, "1", 0},
            { L_ , 2, "2", 0},
            { L_ , 3, "3", 0},
            { L_ , 4, "4", 0},
        };

        if (verbose) cout << endl
                          << "removeAll test" << endl
                          << "==============" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bcec_SkipList<int, bsl::string> SkipList;
        {
            SkipList Obj(&ta);
            POPULATE_LIST_EX(&Obj, VALUES1);
            VERIFY_LIST_EX(Obj, VALUES2);

            ASSERT(5 == Obj.length());
            int numRemoved = Obj.removeAll();
            ASSERT(Obj.isEmpty());
            ASSERT(0 == Obj.length());
            V(numRemoved);
            ASSERT(numRemoved==5);

            POPULATE_LIST_EX(&Obj, VALUES1);
            VERIFY_LIST_EX(Obj, VALUES2);

            bsl::vector<SkipList::PairHandle> removed;
            numRemoved = Obj.removeAll(&removed);
            ASSERT(Obj.isEmpty());
            V(numRemoved);
            LOOP_ASSERT(numRemoved, numRemoved==5);
            LOOP_ASSERT(removed.size(), removed.size()==5);

            if (veryVerbose) {
                Obj.print(cout);
            } else if (verbose) Obj.print(cout, 0, -1) << endl;
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());

        {
            SkipList Obj(&ta);
            POPULATE_LIST_EX(&Obj, VALUES3);

            int numRemoved = Obj.removeAll();
            ASSERT(Obj.isEmpty());
            V(numRemoved);
            ASSERT(numRemoved==5);
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 9: {
        // --------------------------------------------------------------------
        // find test
        // --------------------------------------------------------------------
        DATA VALUES1[] = {
            { L_ , 1, "1", 0},
            { L_ , 3, "3", 1},
            { L_ , 0, "0", 2},
            { L_ , 2, "2", 3},
            { L_ , 4, "4", 4},
        };

        DATA VALUES2[] = {
            { L_ , 0, "0", 2},
            { L_ , 1, "1", 0},
            { L_ , 3, "3", 1},
            { L_ , 4, "4", 4},
        };

        if (verbose) cout << endl
                          << "find test" << endl
                          << "==============" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bcec_SkipList<int, bsl::string> SkipList;
        {
            SkipList Obj(&ta);
            POPULATE_LIST_EX(&Obj, VALUES1);

            SkipList::PairHandle h;
            ASSERT(0 != Obj.find(&h, 10));

            ASSERT(0 == Obj.find(&h, 2));

            Obj.remove(h);
            h.release();

            VERIFY_LIST_EX(Obj, VALUES2);

            if (veryVerbose) {
                Obj.print(cout);
            } else if (verbose) Obj.print(cout, 0, -1) << endl;
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // timer test
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "timer test" << endl
                          << "==========" << endl;

        typedef bcec_SkipList<bdet_TimeInterval, bdef_Function<void(*)()> >
                                                               EventTimeQueue;
        typedef EventTimeQueue::PairHandle                     EventHandle;

        bdet_TimeInterval         timer;
        bdef_Function<void(*)()> callback;

        EventTimeQueue eventTimeQueue;
        EventHandle h;
        eventTimeQueue.add(&h, timer, callback);
      } break;

      case 7: {
        // --------------------------------------------------------------------
        // iteration test
        // --------------------------------------------------------------------
        DATA VALUES1[] = {
            { L_ , 1, "1", 0},
            { L_ , 3, "3", 1},
            { L_ , 0, "0", 2},
            { L_ , 2, "2", 3},
            { L_ , 4, "4", 4},
        };

        DATA VALUES2[] = {
            { L_ , 0, "0", 2},
            { L_ , 1, "1", 0},
            { L_ , 2, "2", 3},
            { L_ , 3, "3", 1},
            { L_ , 4, "4", 4},
        };

        if (verbose) cout << endl
                          << "iteration test" << endl
                          << "==============" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bcec_SkipList<int, bsl::string> SkipList;
        {
            SkipList Obj(&ta);
            POPULATE_LIST_EX(&Obj, VALUES1);
            VERIFY_LIST(Obj, VALUES2);
            VERIFY_LIST_EX(Obj, VALUES2);

            if (veryVerbose) {
                Obj.print(cout);
            } else if (verbose) Obj.print(cout, 0, -1) << endl;
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // iteration test
        // --------------------------------------------------------------------
        DATA VALUES1[] = {
            { L_ , 1, "1"},
            { L_ , 3, "3"},
            { L_ , 0, "0"},
            { L_ , 2, "2"},
            { L_ , 4, "4"},
        };

        DATA VALUES2[] = {
            { L_ , 0, "0"},
            { L_ , 1, "1"},
            { L_ , 2, "2"},
            { L_ , 3, "3"},
            { L_ , 4, "4"},
        };

        if (verbose) cout << endl
                          << "iteration test" << endl
                          << "==============" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bcec_SkipList<int, bsl::string> SkipList;
        {
            SkipList Obj(&ta);
            POPULATE_LIST(&Obj, VALUES1);
            VERIFY_LIST(Obj, VALUES2);
            VERIFY_LIST_REVERSE(Obj, VALUES2);

            if (veryVerbose) {
                Obj.print(cout);
            } else if (verbose) Obj.print(cout, 0, -1) << endl;
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // 'update' test
        //
        // Concerns:
        //   (1) Update on an item that's been removed returns BCEC_NOT_FOUND.
        //   (2) Update to an existing position returns BCEC_DUPLICATE if
        //       allowDuplicates is false
        //   (3) Update updates the key value stored on the node.
        //   (4) After an update, the data can be looked up by its new value
        //       but not by its old value.
        //   (5) Update to an existing position succeeds if allowDuplicates
        //       is true
        //
        // Plan: each test step will update an item to a new location and
        // verify 2 through 4 (we'll check (1) and (5) separately afterwards).
        // We want to ensure that we move both *to* and *from* the beginning,
        // middle, and end of the test.
        //
        // Note that an important feature of the "parameters" set is that
        // executing it returns the list to its original state.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "update test" << endl
                          << "=============" << endl;

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            bcema_TestAllocator da(veryVeryVerbose);
            bslma_DefaultAllocatorGuard defaultAllocGuard(&da);
            typedef bcec_SkipList<int,int> Obj;
            typedef Obj::PairHandle H;
            typedef int (Obj::*Updater)(const Obj::Pair*,
                                        const int &,
                                        bool*, bool);
            {
                Obj obj(&ta);
                H h;

                enum {NUM_ITEMS = 4};

                for (int i = 0; i < NUM_ITEMS; ++i) {
                    obj.add(i*3, i);
                }

                Updater updaters[] = { &Obj::update, &Obj::updateR };

                const struct Parameters {
                    int d_line;
                    int d_from;
                    int d_to;
                    int d_value;
                    bool d_isDuplicate;
                } parameters[] = {
                    {L_,  0,  4,  0,  0},  //front-to-middle
                    {L_,  3,  6,  1,  1},  //front dup
                    {L_,  3, 10,  1,  0},  //front-to-back
                    {L_, 10,  4,  1,  1},  //back dup
                    {L_,  6, 12,  2,  0},  //middle-to-back
                    {L_,  9,  3,  3,  0},  //middle-to-front
                    {L_,  4, 10,  0,  1},  //middle dup
                    {L_, 12,  2,  2,  0},  //back-to-front
                    {L_,  4,  0,  0,  0},  //middle-to-back, again
                    {L_, 10,  1,  1,  0},  //back-to-middle
                    {L_,  3,  9,  3,  0},  //no move
                    {L_,  2,  6,  2,  0},  //no move
                    {L_,  1,  3,  1,  0},  //no move
                };

                for (int i = 0; i < 2; ++i) {
                    Updater updater = updaters[i];
                    for (int j = 0; j < (int) (sizeof(parameters) /
                             sizeof (Parameters)); ++j) {
                        const Parameters& p = parameters[j];

                        H fromH;
                        ASSERT(0 == obj.find(&fromH, p.d_from));
                        ASSERT(p.d_value == fromH.data());

                        int rc = (obj.*updater)(fromH, p.d_to, 0, false);
                        if (p.d_isDuplicate) {
                            ASSERT(rc == Obj::BCEC_DUPLICATE);
                        }
                        else {
                            LOOP3_ASSERT(i, p.d_line, rc, 0 == rc);
                            ASSERT(p.d_to == fromH.key());

                            H toH;
                            ASSERT(0 == obj.find(&toH, p.d_to));
                            ASSERT(p.d_to == toH.key());
                            ASSERT(p.d_value == toH.data());
                            ASSERT(0 != obj.find(&toH, p.d_from));
                        }
                    }

                    Obj obj2(&ta);

                    H items[NUM_ITEMS];
                    for (int j = 0; j < NUM_ITEMS; ++j) {
                        obj2.add(items + j, j, j);
                    }

                    ASSERT(obj.length() == obj2.length());
                    ASSERT(NUM_ITEMS == obj2.length());

                    //Now check concerns (1) and (5)
                    ASSERT(0 == obj2.remove(items[0]));
                    ASSERT(Obj::BCEC_NOT_FOUND == (obj2.*updater)(items[0],
                                                                 100,
                                                                 0, false));
                    ASSERT(Obj::BCEC_NOT_FOUND == (obj2.*updater)(items[0],
                                                                 100,
                                                                 0, true));
                    ASSERT(0 == obj2.remove(items[3]));
                    ASSERT(Obj::BCEC_NOT_FOUND == (obj2.*updater)(items[3],
                                                                 100,
                                                                 0, false));
                    ASSERT(Obj::BCEC_NOT_FOUND == (obj2.*updater)(items[3],
                                                                 100,
                                                                 0, true));

                    ASSERT(0 == (obj2.*updater)(items[1], 2, 0, true));
                    ASSERT(2 == items[1].key());
                    ASSERT(1 == items[1].data());
                    ASSERT(2 == items[2].key());
                }

                obj.removeAll();
                ASSERT(0 == obj.length());
            }
            V(da.numBytesInUse());
            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 == da.numAllocations());
            V(ta.numBytesInUse());
            ASSERT(0 == ta.numBytesInUse());
            ASSERT(0 < ta.numAllocations());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // isNewTop
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "isNewTop Test" << endl
                          << "=============" << endl;

        {
            typedef bcec_SkipList<int, const char *> SkipList;

            bool isNewTop;

            SkipList Obj;
            SkipList::PairHandle h;

            Obj.add(2, "2");
            ASSERT(1 == Obj.length());

            Obj.add(3, "3", &isNewTop);
            ASSERT(!isNewTop);
            ASSERT(2 == Obj.length());

            Obj.add(1, "1", &isNewTop);
            ASSERT(isNewTop);

            Obj.add(&h, 0, "0", &isNewTop);
            ASSERT(isNewTop);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Allocation Test
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Allocation Test" << endl
                          << "===============" << endl;

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            bcema_TestAllocator da(veryVeryVerbose);
            bslma_DefaultAllocatorGuard defaultAllocGuard(&da);

            typedef bcec_SkipList<int,bsl::string> SkipList;
            {
                SkipList Obj(&ta);
                ASSERT(Obj.isEmpty());
                V(da.numBytesInUse());
                ASSERT(0 == da.numBytesInUse());
                { Obj.add(3, "3"); }
                { Obj.add(2, "2"); }
                { Obj.add(0, "0"); }
                { Obj.add(1, "1"); }
                ASSERT(!Obj.isEmpty());
                V(da.numBytesInUse());
                ASSERT(0 == da.numBytesInUse());
                SkipList::PairHandle h;
                int ret;
                ret = Obj.popFront(&h);
                ASSERT(ret==0);
                V(h.data());
                V(h.key());

                ASSERT("0" == h.data());

                V(da.numBytesInUse());
                ASSERT(0 == da.numBytesInUse());

                ret = Obj.popFront(&h);
                ASSERT(ret==0);
                V(h.data());
                V(h.key());

                ASSERT("1" == h.data());
                V(da.numBytesInUse());
                ASSERT(0 == da.numBytesInUse());

                ret = Obj.popFront(&h);
                ASSERT(ret==0);
                V(h.data());
                V(h.key());

                ASSERT("2" == h.data());
                V(da.numBytesInUse());
                ASSERT(0 == da.numBytesInUse());

                ret = Obj.popFront(&h);
                ASSERT(ret==0);
                V(h.data());
                V(h.key());

                ASSERT("3" == h.data());
                V(h.data());
                V(h.key());

                ASSERT(Obj.isEmpty());
                V(da.numBytesInUse());
                ASSERT(0 == da.numBytesInUse());

                ret = Obj.popFront(&h);
                ASSERT(ret==SkipList::BCEC_NOT_FOUND);
                V(da.numBytesInUse());
                ASSERT(0 == da.numBytesInUse());
            }
            V(da.numBytesInUse());
            ASSERT(0 == da.numBytesInUse());
            V(ta.numBytesInUse());
            ASSERT(0 < ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Usage Example
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Usage Example" << endl
                          << "=============" << endl;

        {
            bsl::vector<int> values;

            SimpleScheduler scheduler;

            bdet_Datetime now = bdetu_SystemTime::nowAsDatetimeUtc(),
                scheduleTime;

            // Add events out of sequence and ensure they are executed
            // in the proper order

            if (veryVerbose) {
                cout << "Starting scheduling: " << now << endl;
            }
            scheduleTime = now;
            scheduleTime.addMilliseconds(1500);
            if (veryVerbose) {
                cout << "scheduling: " << scheduleTime << endl;
            }
            scheduler.scheduleEvent(bdef_BindUtil::bind(
                               &pushBackWrapper, &values, 1),
                                    scheduleTime);

            scheduleTime = now;
            scheduleTime.addMilliseconds(750);
            if (veryVerbose) {
                cout << "scheduling: " << scheduleTime << endl;
            }
            scheduler.scheduleEvent(bdef_BindUtil::bind(
                               &pushBackWrapper, &values, 0),
                                    scheduleTime);

            scheduleTime = now;
            scheduleTime.addMilliseconds(2250);
            if (veryVerbose) {
                cout << "scheduling: " << scheduleTime << endl;
            }
            scheduler.scheduleEvent(bdef_BindUtil::bind(
                               &pushBackWrapper, &values, 2),
                                    scheduleTime);
            ASSERT(values.empty());

            scheduleTime.addMilliseconds(250);
            while (bdetu_SystemTime::nowAsDatetimeUtc() < scheduleTime) {
                bcemt_ThreadUtil::microSleep(10000);
            }
            bsls_Stopwatch waitTimer;
            waitTimer.start();

            // sleep for up to 2.5 seconds until everything gets executed
            // (in nightly build environments, which are heavily overloaded,
            // it's hard to set precise time requirements)
            while (3 != values.size()  && 2.5 > waitTimer.elapsedTime()) {
                bcemt_ThreadUtil::microSleep(10000);
            }
            LOOP_ASSERT(waitTimer.elapsedTime(),
                        2.5 > waitTimer.elapsedTime());

            scheduler.stop();
            LOOP_ASSERT(values.size(), 3 == values.size());
            if (3 == values.size()) {
                ASSERT(0 == values[0]);
                ASSERT(1 == values[1]);
                ASSERT(2 == values[2]);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            typedef bcec_SkipList<int, const char *> SkipList;

            SkipList Obj;
            ASSERT(Obj.isEmpty());
            ASSERT(0 == Obj.length());
            Obj.add(0, "Hi");
            ASSERT(!Obj.isEmpty());
            ASSERT(1 == Obj.length());
            SkipList::Pair *h;
            int ret = Obj.popFrontRaw(&h);
            ASSERT(ret==0);
            ASSERT(Obj.isEmpty());
            LOOP_ASSERT(Obj.length(), 0 == Obj.length());
            bsl::string d = h->data();
            V(d);
            Obj.releaseReferenceRaw(h);
            ASSERT(d=="Hi");

            bcec_SkipList<int, bsl::string> Obj2;
            ASSERT(Obj2.isEmpty());
            Obj2.add(3, "3");
            Obj2.add(2, "2");
            Obj2.add(0, "0");
            Obj2.add(1, "1");
            ASSERT(4 == Obj2.length());
            ASSERT(!Obj2.isEmpty());

            bcec_SkipList<int, bsl::string>::Pair *h2;
            ret = Obj2.popFrontRaw(&h2);
            LOOP_ASSERT(Obj2.length(), 3 == Obj2.length());
            ASSERT(ret==0);

            V(h2->data());
            V(h2->key());

            ASSERT("0" == h2->data());
            Obj2.releaseReferenceRaw(h2);

            ret = Obj2.popFrontRaw(&h2);
            ASSERT(ret==0);
            V(h2->data());
            V(h2->key());
            ASSERT("1" == h2->data());
            Obj2.releaseReferenceRaw(h2);

            ret = Obj2.popFrontRaw(&h2);
            ASSERT(ret==0);
            ASSERT(1 == Obj2.length());
            V(h2->data());
            V(h2->key());
            ASSERT("2" == h2->data());
            Obj2.releaseReferenceRaw(h2);

            bcec_SkipList<int, bsl::string>::PairHandle h2h;
            ret = Obj2.popFront(&h2h);
            ASSERT(ret==0);
            V(h2h.data());
            V(h2h.key());

            ASSERT("3" == h2h.data());

            ASSERT(0 == Obj2.length());
            ASSERT(Obj2.isEmpty());

            ret = Obj2.popFront(&h2h);
            ASSERT(ret==SkipList::BCEC_NOT_FOUND);
        }
      } break;
      case -101: {
        // --------------------------------------------------------------------
        // The thread-safety test
        // --------------------------------------------------------------------
        BCEC_SKIPLIST_TEST_CASE_101::run();
      } break;
      case -100: {
        // --------------------------------------------------------------------
        // The router simulation (kind of) test
        // --------------------------------------------------------------------
        BCEC_SKIPLIST_TEST_CASE_MINUS_100::run();
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
