// bdlcc_skiplist.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlcc_skiplist.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslmt_lockguard.h>
#include <bslmt_barrier.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bslmt_threadgroup.h>

#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bdlf_bind.h>
#include <bslma_defaultallocatorguard.h>
#include <bdlt_currenttime.h>
#include <bdlt_intervalconversionutil.h>
#include <bdlt_datetime.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>

#include <bsls_assert.h>

#include <bsl_cstdlib.h>
#include <bsl_c_stdlib.h>  // 'rand_r'
#include <bsl_cmath.h>     // 'floor', 'ceil'
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

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

#define V(X) { if (verbose) P(X) }            // Print in verbose mode

bslmt::Mutex coutMutex;

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

// ============================================================================
//                            SOME HELPFUL EXTRAS
// ----------------------------------------------------------------------------
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

void case18(bdlcc::SkipList<int, int>* list,
            int                        numIterations,
            int                        level,
            bool                       unique)
{
    int id = static_cast<int>(bslmt::ThreadUtil::selfIdAsInt()) + 2;

    if (0 <= level) {
        for (int i = 0; i < numIterations; ++i) {
            bdlcc::SkipList<int, int>::Pair *h;
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
            bdlcc::SkipList<int, int>::Pair *h;
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
    static bsls::AtomicInt deleteCount;
    bool isTemp;

public:
    CountedDelete() : isTemp(true) {}
    CountedDelete(const CountedDelete&) : isTemp(false) {}

    ~CountedDelete() {
        if (!isTemp) {
            ++deleteCount;
        }
    }

    static int getDeleteCount() {
        return deleteCount;
    }

    CountedDelete& operator= (const CountedDelete&) {
        isTemp = false;
        return *this;
    }
};

bsls::AtomicInt CountedDelete::deleteCount(0);

void case20(bdlcc::SkipList<int, CountedDelete>* list, int maxRefCount)
{
    unsigned seed = (unsigned)bslmt::ThreadUtil::selfIdAsInt();

    // Increment the ref count up to maxRefCount, then decrease.  While
    // incrementing, 10% of the time throw in a decrement.

    bdlcc::SkipList<int, CountedDelete>::Pair *front;
    ASSERT(0 == list->frontRaw(&front));

    for (int numReferencesAdded = 1; numReferencesAdded < maxRefCount; )
    {
        list->addPairReferenceRaw(front);
#if defined BSLS_PLATFORM_OS_WINDOWS
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

void case19(bdlcc::SkipList<int, int>* list,
            int                        numIterations,
            int                        level,
            bool                       unique,
            bool                       getHandles)
{
    int id = static_cast<int>(bslmt::ThreadUtil::selfIdAsInt()) + 2;

    bdlcc::SkipListPairHandle<int,int> uniqueH;

    if (0 <= level) {
        for (int i = 0; i < numIterations; ++i) {
            if (getHandles) {
                bdlcc::SkipList<int, int>::Pair *h;
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
                bdlcc::SkipList<int, int>::PairHandle h;
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
    typedef bdlcc::SkipList<bdlt::Datetime, bsl::function<void()> > List;

    List                                                    d_list;
    bslmt::ThreadUtil::Handle                                d_dispatcher;
    bslmt::Condition                                         d_notEmptyCond;
    bslmt::Barrier                                           d_startBarrier;
    bslmt::Mutex                                             d_condMutex;
    volatile int                                            d_doneFlag;

    // PRIVATE METHODS
    void dispatcherThread()
    {
        d_startBarrier.wait();

        while (!d_doneFlag) {
            List::PairHandle firstItem;
            if (0 == d_list.front(&firstItem)) {
                // The list is not empty.

                bsls::TimeInterval when =
                    bdlt::IntervalConversionUtil::convertToTimeInterval(
                                   firstItem.key() - bdlt::CurrentTime::utc());
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
                                              bdlt::CurrentTime::now() + when);
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

  private:
    // Not implemented:
    SimpleScheduler(const SimpleScheduler&);

  public:
    //CREATORS
    SimpleScheduler(bslma::Allocator *basicAllocator = 0)
    : d_list(basicAllocator)
    , d_startBarrier(2)
    , d_doneFlag(false)
    {
        int rc = bslmt::ThreadUtil::create(
                    &d_dispatcher,
                    bdlf::BindUtil::bind(&SimpleScheduler::dispatcherThread,
                                        this));
        BSLS_ASSERT(0 == rc);
        d_startBarrier.wait();
    }

    ~SimpleScheduler()
    {
        stop();
    }

    void stop() {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
        if (bslmt::ThreadUtil::invalidHandle() != d_dispatcher) {
            bslmt::ThreadUtil::Handle dispatcher = d_dispatcher;
            d_doneFlag = true;
            d_notEmptyCond.signal();
            {
                bslmt::LockGuardUnlock<bslmt::Mutex> g(&d_condMutex);
                bslmt::ThreadUtil::join(dispatcher);
            }
            d_dispatcher = bslmt::ThreadUtil::invalidHandle();
        }
    }

    void scheduleEvent(const bsl::function<void()>& event,
                       const bdlt::Datetime&        when)
    {
        // Use 'addR' since this event will probably be placed near the end of
        // the list.

        bool newFrontFlag;
        d_list.addR(when, event, &newFrontFlag);
        if (newFrontFlag) {
            // This event is scheduled before all other events.  Wake up the
            // dispatcher thread.

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

void case16Produce (bdlcc::SkipList<int, int> *list, bsls::AtomicInt *done)
{
    int count = 0;
    while (!(*done)) {
        if (0 > ++count) {
            count = 0;
        }
        bdlcc::SkipList<int, int>::Pair *h;
        list->addRaw(&h, count, count);
        bslmt::ThreadUtil::yield();
        list->releaseReferenceRaw(h);
    }
}

void case16Consume(bdlcc::SkipList<int, int> *list, bsls::AtomicInt *done)
{
    while (!(*done)) {
        bdlcc::SkipList<int, int>::Pair *h1;
        ASSERT(0 == list->frontRaw(&h1));
        bdlcc::SkipList<int, int>::Pair *h2;
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
    // scan forward using 'raw' and 'skip' methods; and also using 'front' and
    // 'next' (non-raw) methods, in parallel.
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
    // scan backward using 'skip' and also 'previous', in parallel

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

// ============================================================================
//                         CASE 101 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace SKIPLIST_TEST_CASE_101 {

typedef bdlcc::SkipList<int,int> List;

enum {
    k_NUM_THREADS    = 12,
    k_NUM_ITERATIONS = 100
};

void threadFunc(List *list, int numIterations, int threadNum)
{
    for (int j=0; j<numIterations; j++) {
        for (int i=0; i<numIterations; i++) {
            list->add(1000*i + threadNum, -1000*i - threadNum);
        }

        for (int i=0; i<numIterations; i++) {
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

    bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::create(&threads[i],
                                 bdlf::BindUtil::bind(&threadFunc,
                                                     &list,
                                                     (int)k_NUM_ITERATIONS,
                                                     i+1));
    }

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    VERIFY_LIST_EX(list, VALUES2);
}

}  // close namespace SKIPLIST_TEST_CASE_101

// ============================================================================
//                        CASE -100 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace SKIPLIST_TEST_CASE_MINUS_100 {

static bsls::AtomicInt currentTime(0);
typedef bdlcc::SkipList<bsls::TimeInterval,int> TimeQ;

enum {
    k_NUM_THREADS    = 4,
    k_NUM_ITERATIONS = 100,
    k_SEND_COUNT = 1000,
    k_RCV_COUNT = 900,
    k_DELAY = 500
};

void threadFunc(TimeQ *timeQueue,
                int    numIterations,
                int    sendCount,
                int    receiveCount,
                int    delay)
{
    bsl::vector<TimeQ::Pair*> timers;
    timers.resize(sendCount);

    bsls::Stopwatch sw;

    for (int i=0; i<numIterations; i++) {
        if ( verbose ) {
            sw.start();
        }

        // "send" messages
        for (int snd=0; snd<sendCount; snd++) {
            currentTime++;
            bsls::TimeInterval t(currentTime + delay, 0);
            timeQueue->addRaw(&timers[snd], t, delay);
        }

        // "receive" replies
        for (int rcv=0; rcv<receiveCount; rcv++) {
            timeQueue->remove(timers[rcv]);
            timeQueue->releaseReferenceRaw(timers[rcv]);
        }

        // "resend" replies
        bsls::TimeInterval now(currentTime, 0);
        while (1) {
            TimeQ::Pair *resubmit;
            if (0 != timeQueue->frontRaw(&resubmit)) {
                break;
            }

            const bsls::TimeInterval& k = resubmit->key();
            if (k <= now) {
                timeQueue->remove(resubmit);
                int newdelay = resubmit->data() * 2;
                bsls::TimeInterval t(currentTime + newdelay, 0);
                timeQueue->add(t, newdelay);
                timeQueue->releaseReferenceRaw(resubmit);
            }
            else {
                timeQueue->releaseReferenceRaw(resubmit);
                break;
            }
        }

        // clean up remaining handles
        for (int cln=receiveCount; cln<sendCount; cln++) {
            timeQueue->releaseReferenceRaw(timers[cln]);
        }

        if ( verbose ) {
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

    bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::create(&threads[i],
                                 bdlf::BindUtil::bind(&threadFunc,
                                                     &timeQueue,
                                                     (int)k_NUM_ITERATIONS,
                                                     (int)k_SEND_COUNT,
                                                     (int)k_RCV_COUNT,
                                                     (int)k_DELAY));
    }

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

}

}  // close namespace SKIPLIST_TEST_CASE_MINUS_100

namespace {

void pushBackWrapper(bsl::vector<int> *vector, int item)
{
    vector->push_back(item);
}
}  // close unnamed namespace

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 23: {
        // --------------------------------------------------------------------
        // DISTRIBUTION TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nDISTRIBUTION TEST"
                               << "\n=================" << bsl::endl;

        enum { k_NUM_ITERATIONS = 60000 };

        typedef bdlcc::SkipList<int,int> Obj;
        typedef Obj::PairHandle H;

        Obj list;

        int n[32] = {0};

        for (int i=0; i<k_NUM_ITERATIONS; i++) {
            H h;
            list.add(&h, 0, 0);
            int l = list.level(h);
            ASSERT(l>=0);
            ASSERT(l<32);
            n[l]++;
            list.remove(h);
        }

        double exp = (k_NUM_ITERATIONS / 4) * 3;
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
            e_COPY_A_TO_B,
            e_COPY_B_TO_A,
            e_ADD_TO_A,
            e_ADD_TO_B,
            e_CLEAR_A,
            e_CLEAR_B
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
            e_COPY_A_TO_B,
            L_
        },
        {
            "", "",
            "[]", "[]", true,
            e_COPY_B_TO_A,
            L_
        },

           //  *** Tests with one list empty:
        {
            "alpha", "the",
            "[[ (level = 2) alpha => the ]]", "[]",
            false,
            e_ADD_TO_A,
            L_
        }
        };

        const int numParameters = sizeof(parameters) / sizeof(Parameters);

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        {
            bdlcc::SkipList<bsl::string, bsl::string> mA(&ta), mB(&ta);

            // set the list level to 2
            mA.addAtLevelUniqueRaw(0, 2, "", "");
            mB.addAtLevelUniqueRaw(0, 2, "", "");
            mA.removeAll();
            mB.removeAll();

            for (int i = 0; i < numParameters; ++i) {
                const Parameters& p = parameters[i];

                bdlcc::SkipList<bsl::string, bsl::string> mA2(mA, &ta);
                bdlcc::SkipList<bsl::string, bsl::string> mB2(mB, &ta);

                switch (p.d_operation) {
                case e_COPY_A_TO_B:
                    mB = mA;
                    mB2 = mA;
                break;
                case e_COPY_B_TO_A:
                    mA = mB;
                    mA2 = mB;
                    break;
                case e_ADD_TO_A:
                    LOOP_ASSERT(p.d_line,
                                0 == mA.addAtLevelUniqueRaw
                                (0, 2, p.d_key, p.d_data));
                    LOOP_ASSERT(p.d_line,
                                0 == mA2.addAtLevelUniqueRaw
                                (0, 2, p.d_key, p.d_data));
                    break;
                case e_ADD_TO_B:
                    LOOP_ASSERT(p.d_line,
                                0 == mB.addAtLevelUniqueRaw
                                (0, 2, p.d_key, p.d_data));
                    LOOP_ASSERT(p.d_line,
                                0 == mB2.addAtLevelUniqueRaw
                                (0, 2, p.d_key, p.d_data));
                    break;
                case e_CLEAR_A:
                    LOOP_ASSERT(p.d_line, mA.removeAll() ==
                                mA2.removeAll());
                    break;
                 case e_CLEAR_B:
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
        // Test appropriate behavior of 'skipBackward' and 'skipForward' and
        // their "raw" variants in a single-threaded environment.
        //
        // Concerns:
        //   * skipping at the front/end resets the handle (as appropriate)
        //     and returns 0
        //   * skipping a removed item returns e_NOT_FOUND
        //   * reference counting is correct
        // ----------------------------------------------------
        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bdlcc::SkipList<int, bsl::string> SkipList;
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
            ASSERT(SkipList::e_NOT_FOUND == obj.skipBackward(&h3));
            ASSERT(h3.isValid());
            ASSERT(SkipList::e_NOT_FOUND == obj.skipForward(&h3));
            ASSERT(h3.isValid());

            obj.remove(p4);
            ASSERT(SkipList::e_NOT_FOUND == obj.skipBackwardRaw(&p4));
            ASSERT(0 != p4);
            ASSERT(SkipList::e_NOT_FOUND == obj.skipForwardRaw(&p4));
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

        typedef bdlcc::SkipList<int, CountedDelete> SkipList;

        enum {
            k_NUM_THREADS = 5
        };

        SkipList mX;
        SkipList::Pair *handle;
        bslmt::ThreadGroup tg;

        mX.addRaw(&handle, 1, CountedDelete());

        int numThreads = k_NUM_THREADS;
        if (verbose) {
            numThreads = atoi(argv[2]);
            if (numThreads == 0) {
                numThreads = k_NUM_THREADS;
            }
            else {
                cout << "Running with " << numThreads << " threads."
                     << endl;
            }
        }
        int maxRefCount = 4000 / numThreads;

        tg.addThreads(bdlf::BindUtil::bind(&case20, &mX, maxRefCount),
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

        typedef bdlcc::SkipList<int, int> SkipList;

        enum {
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 1000
        };

        int numThreads = k_NUM_THREADS;
        if (verbose) {
            numThreads = atoi(argv[2]);
            if (numThreads == 0) {
                numThreads = k_NUM_THREADS;
            }
            else {
                cout << "Running with " << numThreads << " threads."
                     << endl;
            }
        }

        int numIterations = k_NUM_ITERATIONS;
        if (veryVerbose) {
            numIterations = atoi(argv[3]);
            if (numIterations == 0) {
                numIterations = k_NUM_ITERATIONS;
            }
            else {
                cout << "Running with " << numIterations << " iterations."
                     << endl;
            }
        }

        bslma::TestAllocator ta;
        {
            SkipList mX(&ta);
            bslmt::ThreadGroup tg;

            if (verbose) {
                cout << "Phase 1: no handles" << endl;
            }

            tg.addThreads(bdlf::BindUtil::bind(&case19, &mX,
                                              numIterations, -1, false,
                                              false),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 2: unique values" << endl;
            }

            tg.addThreads(bdlf::BindUtil::bind(&case19, &mX,
                                              numIterations, -1, true, true),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 3: level 0" << endl;
            }

            tg.addThreads(bdlf::BindUtil::bind(&case19, &mX,
                                              numIterations, 0, false, true),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 4: random level" << endl;
            }

            tg.addThreads(bdlf::BindUtil::bind(&case19, &mX,
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

        typedef bdlcc::SkipList<int, int> SkipList;

        enum {
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 1000
        };

        int numThreads = k_NUM_THREADS;
        if (verbose) {
            numThreads = atoi(argv[2]);
            if (numThreads == 0) {
                numThreads = k_NUM_THREADS;
            }
            else {
                cout << "Running with " << numThreads << " threads."
                     << endl;
            }
        }

        int numIterations = k_NUM_ITERATIONS;
        if (veryVerbose) {
            numIterations = atoi(argv[3]);
            if (numIterations == 0) {
                numIterations = k_NUM_ITERATIONS;
            }
            else {
                cout << "Running with " << numIterations << " iterations."
                     << endl;
            }
        }

        bslma::TestAllocator ta;
        {
            SkipList mX(&ta);

            if (verbose) {
                cout << "Phase 1: level 0" << endl;
            }

            bslmt::ThreadGroup tg;
            tg.addThreads(bdlf::BindUtil::bind(&case18, &mX,
                                              numIterations, 0, false),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 2: random level" << endl;
            }

            tg.addThreads(bdlf::BindUtil::bind(&case18, &mX,
                                              numIterations, -1, false),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());

            if (verbose) {
                cout << "Phase 3: unique values" << endl;
            }

            tg.addThreads(bdlf::BindUtil::bind(&case18, &mX,
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

        typedef bdlcc::SkipList<int, CountedDelete> SkipList;

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

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            typedef bdlcc::SkipList<int, int> SkipList;
            SkipList list(&ta);

            bsls::AtomicInt done(0);

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
            // line, key, data, level
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

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bdlcc::SkipList<int, bsl::string> SkipList;
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
            bslma::TestAllocator ta(veryVeryVerbose);
            bslma::TestAllocator da(veryVeryVerbose);
            bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
            typedef bdlcc::SkipList<int, bsl::string> SkipList;
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
                ASSERT(ret==SkipList::e_DUPLICATE);
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
            typedef bdlcc::SkipList<int, const char *> SkipList;

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
            typedef bdlcc::SkipList<int,bsl::string> SkipList;

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
            ASSERT(ret==SkipList::e_NOT_FOUND);
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
            typedef bdlcc::SkipList<int, const char *> SkipList;

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

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bdlcc::SkipList<int, bsl::string> SkipList;
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

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bdlcc::SkipList<int, bsl::string> SkipList;
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

        typedef bdlcc::SkipList<bsls::TimeInterval, bsl::function<void()> >
                                                               EventTimeQueue;
        typedef EventTimeQueue::PairHandle                     EventHandle;

        bsls::TimeInterval    timer;
        bsl::function<void()> callback;

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

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bdlcc::SkipList<int, bsl::string> SkipList;
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
            { L_ , 1, "1", 0 },
            { L_ , 3, "3", 0 },
            { L_ , 0, "0", 0 },
            { L_ , 2, "2", 0 },
            { L_ , 4, "4", 0 },
        };

        DATA VALUES2[] = {
            { L_ , 0, "0", 0},
            { L_ , 1, "1", 0},
            { L_ , 2, "2", 0},
            { L_ , 3, "3", 0},
            { L_ , 4, "4", 0},
        };

        if (verbose) cout << endl
                          << "iteration test" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bdlcc::SkipList<int, bsl::string> SkipList;
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
        //   (1) Update on an item that's been removed returns e_NOT_FOUND.
        //   (2) Update to an existing position returns e_DUPLICATE if
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
            bslma::TestAllocator ta(veryVeryVerbose);
            bslma::TestAllocator da(veryVeryVerbose);
            bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
            typedef bdlcc::SkipList<int,int> Obj;
            typedef Obj::PairHandle H;
            typedef int (Obj::*Updater)(const Obj::Pair*,
                                        const int &,
                                        bool*,
                                        bool);
            {
                Obj obj(&ta);
                H h;

                enum {k_NUM_ITEMS = 4};

                for (int i = 0; i < k_NUM_ITEMS; ++i) {
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
                            ASSERT(rc == Obj::e_DUPLICATE);
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

                    H items[k_NUM_ITEMS];
                    for (int j = 0; j < k_NUM_ITEMS; ++j) {
                        obj2.add(items + j, j, j);
                    }

                    ASSERT(obj.length() == obj2.length());
                    ASSERT(k_NUM_ITEMS == obj2.length());

                    //Now check concerns (1) and (5)
                    ASSERT(0 == obj2.remove(items[0]));
                    ASSERT(Obj::e_NOT_FOUND == (obj2.*updater)(items[0],
                                                                 100,
                                                                 0, false));
                    ASSERT(Obj::e_NOT_FOUND == (obj2.*updater)(items[0],
                                                                 100,
                                                                 0, true));
                    ASSERT(0 == obj2.remove(items[3]));
                    ASSERT(Obj::e_NOT_FOUND == (obj2.*updater)(items[3],
                                                                 100,
                                                                 0, false));
                    ASSERT(Obj::e_NOT_FOUND == (obj2.*updater)(items[3],
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
            typedef bdlcc::SkipList<int, const char *> SkipList;

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
            bslma::TestAllocator ta(veryVeryVerbose);
            bslma::TestAllocator da(veryVeryVerbose);
            bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

            typedef bdlcc::SkipList<int,bsl::string> SkipList;
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
                ASSERT(ret==SkipList::e_NOT_FOUND);
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

            bdlt::Datetime now = bdlt::CurrentTime::utc(),
                scheduleTime;

            // Add events out of sequence and ensure they are executed in the
            // proper order

            if (veryVerbose) {
                cout << "Starting scheduling: " << now << endl;
            }
            scheduleTime = now;
            scheduleTime.addMilliseconds(1500);
            if (veryVerbose) {
                cout << "scheduling: " << scheduleTime << endl;
            }
            scheduler.scheduleEvent(bdlf::BindUtil::bind(
                               &pushBackWrapper, &values, 1),
                                    scheduleTime);

            scheduleTime = now;
            scheduleTime.addMilliseconds(750);
            if (veryVerbose) {
                cout << "scheduling: " << scheduleTime << endl;
            }
            scheduler.scheduleEvent(bdlf::BindUtil::bind(
                               &pushBackWrapper, &values, 0),
                                    scheduleTime);

            scheduleTime = now;
            scheduleTime.addMilliseconds(2250);
            if (veryVerbose) {
                cout << "scheduling: " << scheduleTime << endl;
            }
            scheduler.scheduleEvent(bdlf::BindUtil::bind(
                               &pushBackWrapper, &values, 2),
                                    scheduleTime);
            ASSERT(values.empty());

            scheduleTime.addMilliseconds(250);
            while (bdlt::CurrentTime::utc() < scheduleTime) {
                bslmt::ThreadUtil::microSleep(10000);
            }
            bsls::Stopwatch waitTimer;
            waitTimer.start();

            // sleep for up to 2.5 seconds until everything gets executed (in
            // nightly build environments, which are heavily overloaded, it's
            // hard to set precise time requirements)
            while (3 != values.size()  && 2.5 > waitTimer.elapsedTime()) {
                bslmt::ThreadUtil::microSleep(10000);
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
            typedef bdlcc::SkipList<int, const char *> SkipList;

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

            bdlcc::SkipList<int, bsl::string> Obj2;
            ASSERT(Obj2.isEmpty());
            Obj2.add(3, "3");
            Obj2.add(2, "2");
            Obj2.add(0, "0");
            Obj2.add(1, "1");
            ASSERT(4 == Obj2.length());
            ASSERT(!Obj2.isEmpty());

            bdlcc::SkipList<int, bsl::string>::Pair *h2;
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

            bdlcc::SkipList<int, bsl::string>::PairHandle h2h;
            ret = Obj2.popFront(&h2h);
            ASSERT(ret==0);
            V(h2h.data());
            V(h2h.key());

            ASSERT("3" == h2h.data());

            ASSERT(0 == Obj2.length());
            ASSERT(Obj2.isEmpty());

            ret = Obj2.popFront(&h2h);
            ASSERT(ret==SkipList::e_NOT_FOUND);
        }
      } break;
      case -101: {
        // --------------------------------------------------------------------
        // The thread-safety test
        // --------------------------------------------------------------------
        SKIPLIST_TEST_CASE_101::run();
      } break;
      case -100: {
        // --------------------------------------------------------------------
        // The router simulation (kind of) test
        // --------------------------------------------------------------------
        SKIPLIST_TEST_CASE_MINUS_100::run();
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
