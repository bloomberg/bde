// bcemt_barrier.t.cpp              -*-C++-*-
#include <bcemt_barrier.h>

#include <bces_atomictypes.h>  // for testing only
#include <bcemt_thread.h>      // for testing only

#include <bsls_platform.h>
#include <bsls_platformutil.h>
#include <bsls_timeutil.h>

#include <bdef_bind.h>
#include <bdetu_systemtime.h>

#include <bsl_functional.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>              // for usage example

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
// We begin by testing that a barrier can be constructed, destroyed, and hold a
// number of threads required to unblock it.  Once that is established, we test
// that the function 'numThreads()' properly returns the number passed to the
// constructor.  Next we test the core functionality of the barrier, that it
// blocks until the necessary number of calls to 'wait()' have been performed,
// that it then unblocks all the threads and resets the barrier to its initial
// state, and that this can be repeated as many times as desired.  Once
// 'wait()' is established to work, a similar test is performed for
// 'timedWait', with the additional test that the calls to 'timedWait' must
// time out if not enough threads call it within the time out period.  Next, we
// make sure that 'wait' and 'timedWait' interact properly so that a call to
// 'timedWait' can be unblocked by a call to 'wait' and vice versa.  Finally,
// we make sure the usage example compiles and runs as expected.
//-----------------------------------------------------------------------------
// [2] bcemt_Barrier(int numThreads);
// [2] ~bcemt_Barrier();
// [3] void wait();
// [4] void timedWait(const bdet_TimeInterval& timeOut);
// [2] int numThreads();
//-----------------------------------------------------------------------------
// [1] Breathing test
// [5] Testing interactions between 'wait' and 'timedWait'
// [6] USAGE Example
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static bces_SpinLock coutLock;

void aSsErT(int c, const char *s, int i, bool lock=true) {
    if (c) {
        if (lock) {
           coutLock.lock();
        }
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        coutLock.unlock();
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { \
       coutLock.lock(); \
       cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__, false);\
    }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { \
       coutLock.lock(); \
       cout << #I << ": " << I << "\t" << #J << ": " \
            << J << "\n"; aSsErT(1, #X, __LINE__, false); \
    }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { \
      coutLock.lock(); \
      cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
           << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__, false); \
   } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------

struct ThreadArgs {
    // This structure is used to pass arguments to the thread functions that
    // are used in testing.  The use of the individual data members depends on
    // the context.

    bcemt_Barrier  d_barrier;     // barrier used for testing

    bces_AtomicInt d_waitCount;   // count of threads waiting or about to
                                  // wait on the barrier being tested

    int            d_timeOut;     // time out to use when calling 'timedWait'
                                  // (in microseconds)

    bces_AtomicInt d_numTimedOut; // number of threads that time out on
                                  // a call to 'timedWait'

    ThreadArgs(int barrierCount, int timeOut=0)
        : d_barrier(barrierCount)
        , d_waitCount(0)
        , d_timeOut(timeOut)
        , d_numTimedOut(0)
    {
    }
};

//-----------------------------------------------------------------------------
//                           HELPER FUNCTIONS FOR TEST CASE 9
//-----------------------------------------------------------------------------
namespace case9 {

struct Control
{
    int           d_numIterations;
    int           d_numThreads;

    bcemt_Barrier d_b1;
    bcemt_Barrier d_b2;

    Control(int numIterations, int numThreads)
        : d_numIterations(numIterations)
        , d_numThreads(numThreads)
        , d_b1(numThreads)
        , d_b2(numThreads)
    {}
};

extern "C" void *a(void *arg)
{
    Control *c = (Control *)arg;

    for(int i=0; i<c->d_numIterations; i++) {
        c->d_b1.wait();
        c->d_b2.wait();
    }
    return 0;
}

extern "C" void *b(void *arg)
{
    char fillers[] = { -1, 0, 1};

    Control *c = (Control *)arg;

    void *mem = &c->d_b2;
    for(int i=0; i<c->d_numIterations; i++) {
        char filler = fillers[i%3];

        c->d_b1.wait();
        c->d_b2.wait();

        c->d_b2.~bcemt_Barrier();

        bsl::memset(mem, filler, sizeof(c->d_b2));

        bcemt_ThreadUtil::yield();
        new (mem) bcemt_Barrier(c->d_numThreads);
    }
    return 0;
}

void test(int numIterations, int numThreads)
{
    bsl::vector<bcemt_ThreadUtil::Handle>  hh;
    Control c(numIterations, numThreads);

    for (int i = 0; i<numThreads-1; ++i) {
        bcemt_ThreadUtil::Handle h;
        bcemt_ThreadUtil::create(&h, &a, (void *)&c);
        hh.push_back(h);
    }
    bcemt_ThreadUtil::Handle h;
    bcemt_ThreadUtil::create(&h, &b, (void *)&c);
    hh.push_back(h);

    for (int i = 0; i<numThreads; ++i) {
        bcemt_ThreadUtil::join(hh[i]);
    }
}

}

//-----------------------------------------------------------------------------
//                           HELPER FUNCTIONS FOR TEST CASE 5
//-----------------------------------------------------------------------------

extern "C" void * testThread5a(void *arg)
    // This function is used to test the 'wait' and 'timedWait' methods
    // together: it uses the 'd_waitCount' atomic integer provided
    // 'ThreadArgs' structure to make sure that the call to the 'wait' method
    // of the provided barrier occurs before the call to 'timedWait'.
{
    ThreadArgs *args = (ThreadArgs*)arg;

    if (++args->d_waitCount == 1) {
        args->d_barrier.wait();
    }
    else {
        bdet_TimeInterval timeOut(bdetu_SystemTime::now());
        timeOut.addMicroseconds(args->d_timeOut);
        int res = args->d_barrier.timedWait(timeOut);
        if (res) ++args->d_numTimedOut;
    }

    return arg;
}

extern "C" void * testThread5b(void *arg)
    // This function is used to test the 'wait' and 'timedWait' methods
    // together: it uses the 'd_waitCount' atomic integer of the
    // 'ThreadArgs' structure to make sure that the call to the 'timedWait'
    // method of the provided barrier occurs before the call to 'wait'.
{
    ThreadArgs *args = (ThreadArgs*)arg;

    if (++args->d_waitCount == 1) {
        bdet_TimeInterval timeOut(bdetu_SystemTime::now());
        timeOut.addMicroseconds(args->d_timeOut);
        int res = args->d_barrier.timedWait(timeOut);
        if (res) ++args->d_numTimedOut;
    }
    else {
        args->d_barrier.wait();
    }

    return arg;
}

//-----------------------------------------------------------------------------
//                           HELPER FUNCTIONS FOR TEST CASE 4
//-----------------------------------------------------------------------------

struct ThreadArgs4 {
    // This structure is used to pass arguments to the thread functions that
    // are used in testing.  The use of the individual data members depends on
    // the context.

    bcemt_Barrier  d_barrier;     // barrier used for testing

    int            d_timeOut;     // time out to use when calling 'timedWait'
                                  // (in microseconds)

    bces_AtomicInt d_stopCount;   // count indicating that thread has complete
                                  // its test

    bces_AtomicInt d_numTimedOut; // number of threads that time out on
                                  // a call to 'timedWait'

    int            d_nThreads;    // number of worker threads

    ThreadArgs4(int nThreads, int timeOut=0)
        : d_barrier(nThreads + 1)
        , d_timeOut(timeOut)
        , d_stopCount(0)
        , d_numTimedOut(0)
        , d_nThreads(nThreads)
    {
    }
};

extern "C" void * testThread4(void *arg)
    // This function is used to test the 'timedWait' method: it begins by
    // incrementing the 'd_startCount' member of the provided 'ThreadArgs4'
    // structure, then calls the 'wait' method of the provided barrier, and
    // finally increments 'd_stopCount' and returns.  This makes sure we can
    // synchronize with the main thread via 'd_startCount' and 'd_stopCount',
    // so as to measure the time between before and after the 'timedWait'
    // section.
{
    ThreadArgs4 *args = (ThreadArgs4*)arg;

    bdet_TimeInterval timeOut(bdetu_SystemTime::now());
    timeOut.addMicroseconds(args->d_timeOut);
    int res = args->d_barrier.timedWait(timeOut);
    bdet_TimeInterval duration(bdetu_SystemTime::now());
    duration.addMicroseconds(args->d_timeOut);
    duration -= timeOut;
    if (res) {
        // relax timing by 50us for Windows
#ifdef BSLS_PLATFORM__OS_WINDOWS
        const int relax = 50;
#else
        const int relax = 0;
#endif

        ++args->d_numTimedOut;
        LOOP2_ASSERT(args->d_timeOut, duration.totalMicroseconds(),
                      duration.totalMicroseconds() + relax >= args->d_timeOut);
    }

    // Wait until all the threads have stopped.

    ++args->d_stopCount;
    while (args->d_nThreads + 1 != args->d_stopCount) {
#ifdef BSLS_PLATFORM__OS_AIX
        // Avoid spinning as it can block on some overoptimized platforms.

        bcemt_ThreadUtil::yield();
#endif
    }

    return arg;
}

//-----------------------------------------------------------------------------
//                           HELPER FUNCTIONS FOR TEST CASE 3
//-----------------------------------------------------------------------------

extern "C" void * testThread3(void *arg)
    // This function is used to test the 'wait' method: it begins by
    // incrementing the 'd_waitCount' member of the provided 'ThreadArgs'
    // structure, then calls the 'wait' method of the provided barrier.
    // and finally decrements 'd_waitCount' and returns.
{
    ThreadArgs *args = (ThreadArgs*)arg;

    ++args->d_waitCount;
    args->d_barrier.wait();
    --args->d_waitCount;

    return arg;
}

//=============================================================================
//                           USAGE EXAMPLE FROM HEADER
//-----------------------------------------------------------------------------

enum {
    MAX_BASKET_TRADES = 10
};

struct Trade {
    // Trade stuff...
};

struct BasketTrade {
    bsl::vector<Trade> d_trades;  // array of trade that comprise the basket
};

int validateTrade(Trade &)
{
    int result = 0;
    // Do some checking here...

    return result;
}

int insertToDatabase(Trade &)
{
    int result = 0;
    // Insert the record here...

    return result;
}

int submitToExchange(Trade &)
{
    int result = 0;
    // Do submission here...

    return result;
}

int deleteFromDatabase(Trade &)
{
    int result = 0;
    // delete record here...

    return result;
}

int cancelAtExchange(Trade &)
{
    int result = 0;
    // cancel trade here...

    return result;
}

struct TradeThreadArgument {
    bsl::vector<Trade> *d_trades_p;
    bcemt_Barrier      *d_barrier_p;
    volatile bool      *d_errorFlag_p;
    int                 d_tradeNum;
};

TradeThreadArgument *processTrade(TradeThreadArgument *args)
{
    int retval;
    Trade &trade = (*args->d_trades_p)[args->d_tradeNum];

    retval = validateTrade(trade);
    if (retval) *args->d_errorFlag_p = true;
    args->d_barrier_p->wait();
    if (*args->d_errorFlag_p) return args;

    retval = insertToDatabase(trade);
    if (retval) *args->d_errorFlag_p = true;
    args->d_barrier_p->wait();
    if (*args->d_errorFlag_p) {
        if (!retval) deleteFromDatabase(trade);
        return args;
    }

    retval = submitToExchange(trade);
    if (retval) *args->d_errorFlag_p = true;
    args->d_barrier_p->wait();
    if (*args->d_errorFlag_p) {
        if (!retval) cancelAtExchange(trade);
        deleteFromDatabase(trade);
        return args;
    }
    return args;
}

extern "C" void *tradeProcessingThread(void *argsIn)
{
    return (void *) processTrade ((TradeThreadArgument *)argsIn);
}

bool processBasketTrade(BasketTrade &trade)
    // Return 'true' if the basket trade was processed successfully,
    // 'false' otherwise.  The trade is processed atomically, i.e.,
    // all the trades succeed, or none of the trades are executed.
{
    TradeThreadArgument arguments[MAX_BASKET_TRADES];
    bcemt_ThreadAttributes attributes;
    bcemt_ThreadUtil::Handle threadHandles[MAX_BASKET_TRADES];

    int numTrades = trade.d_trades.size();
    LOOP_ASSERT(numTrades, 0 < numTrades && MAX_BASKET_TRADES >= numTrades);

    bcemt_Barrier barrier(numTrades);
    bool errorFlag = false;

    for (int i = 0; i<numTrades; ++i) {
        arguments[i].d_trades_p    = &trade.d_trades;
        arguments[i].d_barrier_p   = &barrier;
        arguments[i].d_errorFlag_p = &errorFlag;
        arguments[i].d_tradeNum    = i;
        bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                 tradeProcessingThread,
                                 &arguments[i]);
    }

    for (int i = 0; i<numTrades;++i) {
        bcemt_ThreadUtil::join(threadHandles[i]);
    }

    return errorFlag == false;
}

class Case8_Driver
{
   bces_AtomicInt *d_state;
   bcemt_Barrier *d_barrier;
   int d_numCycles;

public:

   Case8_Driver(bces_AtomicInt *state,
                bcemt_Barrier *barrier,
                int numCycles) :
      d_state(state), d_barrier(barrier), d_numCycles(numCycles) {}

   void operator()() {
      for (int i = 0; i < d_numCycles; ++i) {
         d_barrier->wait();
         *d_state += 1;
         LOOP2_ASSERT(i, *d_state,
                      (*d_state > 2*i) && (*d_state <= (i+1) * 2));
      }
   }
};

class Case7_Waiter {

   typedef bcemt_Barrier Barrier;

   Barrier        *d_barrier;
   bces_AtomicInt *d_state;
   int             d_numWaits;
   int             d_numThreads;

public:
   Case7_Waiter(Barrier        *barrier=0,
                bces_AtomicInt *state=0,
                int             numWaits=0,
                int             numThreads=0) :
      d_barrier(barrier),
      d_state(state),
      d_numWaits(numWaits),
      d_numThreads(numThreads)
   {}

   void begin() {
      ASSERT(0 != d_state);
      bcemt_ThreadAttributes detached;
      bcemt_ThreadUtil::Handle dummy;
      detached.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);

      ASSERT(0 == bcemt_ThreadUtil::create(&dummy, detached,
                           bdef_BindUtil::bind(&Case7_Waiter::run,
                                               this)));
   }

   void run() {
      ASSERT(0 != d_state);
      for (int i = 0; i < d_numWaits; ++i) {
         d_barrier->wait();
         *d_state += 1;
         LOOP2_ASSERT(i, *d_state,
                      (*d_state > i * d_numThreads) &&
                      (*d_state <= (i+1) * d_numThreads));
      }
   }
};

void case7 (bcemt_Barrier* barrier,
            bool verbose, int numThreads, int numWaits) {

   bces_AtomicInt state = 0;
   vector<Case7_Waiter > waiters;
   waiters.insert(waiters.end(), numThreads-1,
                  Case7_Waiter(barrier, &state,
                               numWaits, numThreads));
   for_each(waiters.begin(), waiters.end(),
                mem_fun_ref(&Case7_Waiter::begin));
   for (int i = 0; i < numWaits; ++i) {
      ASSERT(state == i * numThreads);
      barrier->wait();
      state += 1;
      if (verbose) {
         coutLock.lock();
         cout << "Waiting: " << i+1 << "/" << numWaits+1 << endl;
         coutLock.unlock();
      }
      while (state < ((i+1) * numThreads)) bcemt_ThreadUtil::yield();
   }
   ASSERT(state == numWaits * numThreads);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) {
      case 9: {
        if (verbose) {
           cout << "Thread-safety of the destructor test" << endl;
        }
        // ----------------------------------------------------------------
        // Thread-safety of the destructor test
        //
        // Concern: whether the destructor is properly synchronized with
        // threads going through the wait.
        // ----------------------------------------------------------------
        enum {
           NUM_THREADS = 4,
           NUM_ITERATIONS =5000
        };

        case9::test(NUM_ITERATIONS, NUM_THREADS);

      } break;

      case 8: {
        if (verbose) {
           cout << "2-thread reuse test" << endl;
        }
        // ----------------------------------------------------------------
        // 2-thread reuse test
        //
        // Concern: That if the second thread in a two-thread group
        // enters a second wait cycle while the first thread is in the
        // middle of its first wait(), the second thread still waits
        // in the second wait cycle as expected.
        //
        // ----------------------------------------------------------------
        enum {
           NUM_WAIT_CYCLES = 50
        };

        bcemt_Barrier normalBarrier(2);

        bcemt_ThreadAttributes detached;
        bces_AtomicInt state = 0;
        bcemt_ThreadUtil::Handle dummy;
        detached.setDetachedState(
                                bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);

        if (veryVerbose) {
           cout << "Unencumbered test" << endl;
        }

        Case8_Driver driver(&state,
                            &normalBarrier,
                            NUM_WAIT_CYCLES);

        ASSERT(0 == bcemt_ThreadUtil::create(&dummy, detached, driver));

        for (int i = 0; i < NUM_WAIT_CYCLES; ++i) {
           if (veryVerbose) {
              cout << "Cycle " << i+1 << "/" << NUM_WAIT_CYCLES << endl;
           }
           normalBarrier.wait();
           state += 1;
           LOOP2_ASSERT(i, state,
                        (state > 2*i) && (state <= (i+1) * 2));
        }

      } break;

      case 7: {
        // ----------------------------------------------------------------
        // Reusable barrier test
        //
        // Concern: that bcemt_Barrier is reusable by the same thread group,
        // i.e., that N threads, after passing wait(), may pass a second
        // wait() correctly.
        //
        // Plan: set up N threads to wait on a shared barrier, with *one* of
        // the threads switching a state variable before calling wait().
        // There will be M wait() calls, and M after-wait states.  Verify
        // that the N-1 other threads only enter each after-wait state after
        // the N'th thread has switched the variable.
        // ----------------------------------------------------------------
       if (verbose) {
          cout << "\nReusable barrier test" << endl;
       }
       enum {
          NUM_THREADS = 4,
          NUM_SHORT_WAITS = 100,
          NUM_LONG_WAITS = 40
       };

       // These are test invariants rather than component invariants:
       ASSERT(NUM_THREADS * NUM_SHORT_WAITS < numeric_limits<int>::max());
       ASSERT(NUM_LONG_WAITS < NUM_SHORT_WAITS);
       bcemt_Barrier basicBarrier(NUM_THREADS);
       if (veryVerbose) {
          cout << "   ...Basic barrier..." << endl;
       }
       case7(&basicBarrier, veryVerbose, NUM_THREADS, NUM_SHORT_WAITS);

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //   The usage example from the header has been incorporated into this
        //   test driver.  All references to 'assert' have been replaced with
        //   'ASSERT'.  Call the test example function and assert that it works
        //   as expected.
        //
        // Plan:
        //   Construct a dummy 'BasketTrade' and invoke the
        //   'processBasketTrade' function.  Assert that the function returns
        //   the expected result.
        //
        // Testing:
        //   USAGE example
        // --------------------------------------------------------------------

          {
              BasketTrade basket;
              Trade t;
              enum {
                  NUMTRADES = 5
              };

              for (int i = 0; i < NUMTRADES; ++i) {
                  basket.d_trades.push_back(t);
              }
              ASSERT(processBasketTrade(basket));
          }
      } break;

      case 5: {
        // --------------------------------------------------------------------
        // TESTING interaction between 'wait' and 'timedWait'
        //
        // Concerns:
        //   That a call to 'wait' can be unblocked by a call to 'timedWait'
        //   and vice versa.
        //
        // Plan:
        //   Create a barrier object with two threads.  In one thread, call
        //   'wait', then in the other thread call 'timedWait' (make sure this
        //   happens after the call to 'wait'), and verify that no deadlock
        //   occurs.  Then reverse roles, making sure that first call to
        //   'timedWait' in first thread, then to 'wait' in second thread, also
        //   does not deadlock.
        //
        // Testing: interaction between 'wait' and 'timedWait'
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\tTesting interaction between 'wait' and 'timedWait'\n"
                 << "\t--------------------------------------------------"
                 << endl;

        {
            enum {
                NTHREADS   = 2,
                TIMEOUT    = 100000 // 0.1s, in microseconds
            };

            ThreadArgs args(NTHREADS, TIMEOUT);
            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadAttributes attributes;

            for (int i = 0; i < NTHREADS; ++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         testThread5a, &args);
            }

            for (int i = 0; i < NTHREADS; ++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
            }

            ASSERT(0 == args.d_numTimedOut);

            for (int i = 0; i < NTHREADS; ++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         testThread5b, &args);
            }

            for (int i = 0; i < NTHREADS; ++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
            }

            ASSERT(0 == args.d_numTimedOut);
        }
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'timedWait'
        //
        // Concerns:
        //   Verify that the barrier will time out properly when too few
        //   threads are waiting on the barrier.  Verify that the barrier will
        //   not unblock any waiting threads until the required number of
        //   threads have called the 'timedWait' method.  Also verify that once
        //   all threads have been unblocked, that the waiting count is
        //   properly reset, allowing the barrier to be reused.
        //
        // Plan:
        //   Create a barrier with an arbitrary 'numThreads' value.  Then
        //   create 'numThreads' - 1 threads that block on the barrier using
        //   the 'timedWait' method using an arbitrary 'timeOut' value.  Once
        //   the chosen number of threads have been started, wait until all
        //   the threads return.  Assert that all the threads time out.  Also
        //   assert that they do not time out before TIMEOUT.  This is done by
        //   measuring the time between the 'timedWait' initiation and return
        //   in each thread.
        //
        //   Next verify that the barrier resets properly after, and that the
        //   'timedWait' method returns properly when the required number of
        //   threads call 'timedWait' on the barrier.  Using a different
        //   barrier from the above test, create a new series of threads that
        //   block on the barrier using the 'timedWait' method.  Once the
        //   chosen number of threads have started and are blocked on the
        //   barrier, call the 'timedWait' function on the barrier from the
        //   main thread and verify that this method returns immediately
        //   without timing out.  This call should unblock all the threads that
        //   are waiting on the barrier.  Assert that there are no longer any
        //   threads waiting on the barrier and that none of the calls to
        //   'timedWait' timed out.  Finally, repeat the process again using
        //   the same barrier and a new set of threads to verify that the
        //   barrier resets properly.
        //
        // Testing:
        //   void timedWait(const bdet_TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing: 'timedWait'" << endl
                          << "====================" << endl;
        enum {
            NTHREADS   = 5,
            TIMEOUT    = 1000000 // 1s, in microseconds
        };

        bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
        bcemt_ThreadAttributes attributes;

        if (verbose)
            cout << "\tTesting 'timedWait' with timeouts" << endl
                 << "\t---------------------------------" << endl;

        {
            ThreadArgs4 args(NTHREADS, TIMEOUT);
            for (int i = 0; i < NTHREADS; ++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                                         testThread4, &args);
            }

            // Wait until all the threads have stopped.
            ++args.d_stopCount;
            while (NTHREADS + 1 != args.d_stopCount) {
#ifdef BSLS_PLATFORM__OS_AIX
                bcemt_ThreadUtil::yield();
#endif
            }

            LOOP_ASSERT(args.d_numTimedOut, NTHREADS == args.d_numTimedOut);
            for (int i = 0; i < NTHREADS; ++i) {
                bcemt_ThreadUtil::join(threadHandles[i]);
            }
        }

        if (verbose)
            cout << "\tTesting 'timedWait' without timeouts" << endl
                << "\t------------------------------------" << endl;

        ThreadArgs4 args(NTHREADS, 60 * TIMEOUT);  // 60s
        for (int n = 0; n < 2; ++n)
        {
            if (verbose) cout << "\titeration number " << n << endl;
            args.d_stopCount = 0;

            for (int i = 0; i < NTHREADS; ++i) {
                bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                        testThread4, &args);
            }

            // Note: previous testing used 'wait()', but this fails to make
            // sure that the 'timedWait' function also succeeds in unlocking
            // the other threads, and can lead to a TIMEOUT failure if the
            // worker threads time out before 'wait' is called.  We prefer to
            // use the 'timedWait' function instead and make sure it does not
            // time out.  If it does time out, then the test will not be able
            // to run properly, but this may be due to the failure of the test
            // driver to reach line 621: 'args.d_barrier.timedWait()' below
            // before the worker threads have already timed out (timer
            // failure), so we do not assert and issue a warning instead.

            bdet_TimeInterval timeOut = bdetu_SystemTime::now();
            timeOut.addMicroseconds(60 * TIMEOUT);  // 60s

            const int res = args.d_barrier.timedWait(timeOut);
            ASSERT(0 == res);

            if (0 == res ) {
                ++args.d_stopCount;
                while(NTHREADS + 1 != args.d_stopCount) {
#ifdef BSLS_PLATFORM__OS_AIX
                    bcemt_ThreadUtil::yield();
#endif
                }

                LOOP2_ASSERT(n, args.d_numTimedOut,
                        0 == args.d_numTimedOut);

                for (int i = 0; i < NTHREADS; ++i) {
                    bcemt_ThreadUtil::join(threadHandles[i]);
                }
            }
            else {
                // This can fail if 'timedWait' above gets executed after a
                // worker thread already timed out, but the extremely long
                // TIMEOUT makes this really unlikely!  Just to be complete,
                // we avoid locking this test driver indefinitely.
                // We still need to unlock the waiting threads.

                ++args.d_stopCount;
                for (int i = 0; i < NTHREADS; ++i) {
                    bcemt_ThreadUtil::join(threadHandles[i]);
                }
            }
        }
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'wait'
        //
        // Concerns:
        //   Verify that the barrier will not unblock any waiting threads until
        //   the required number of threads have called the wait method.  Also
        //   verify that once all threads have been unblocked, the waiting
        //   count is properly reset, allowing the barrier to be reused.
        //
        // Plan:
        //   Create a barrier with an arbitrary 'numThreads' value.  Then
        //   create 'numThreads' - 1 threads threads that block on the barrier
        //   using the 'wait' method.  Assert that these 'numThreads' - 1
        //   threads are blocked.  After the configured number of threads
        //   have been started and have called 'wait' on the barrier,
        //   delay for a short time and call 'wait' on the barrier from the
        //   main thread.  This should return immediately and unblock all
        //   threads that are waiting on the barrier.  Assert that there are
        //   no longer any threads blocking on the barrier.  Finally repeat the
        //   process again using the same barrier and a new set of threads
        //   to verify that the barrier resets properly.
        //
        // Testing:
        //   void wait();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing: 'wait'" << endl
                          << "===============" << endl;

        {
            const int NTHREADS = 4;
            ThreadArgs args(NTHREADS+1);
            bcemt_ThreadUtil::Handle threadHandles[NTHREADS];
            bcemt_ThreadAttributes attributes;

            for (int n = 0; n < 2; ++n)
            {
                if (verbose) cout << "\titeration number " << n << endl;
                for (int i = 0; i < NTHREADS; ++i) {
                    bcemt_ThreadUtil::create(&threadHandles[i], attributes,
                            testThread3, &args);
                }
                while (NTHREADS != args.d_waitCount) {
#ifdef BSLS_PLATFORM__OS_AIX
                    // avoid spinning as this can preempt on some platforms

                    bcemt_ThreadUtil::yield();
#endif
                }
                enum { NUM_CHECK = 100000 };
                for (int i = 0; i < NUM_CHECK; ++i) {
                    LOOP2_ASSERT(n, args.d_waitCount,
                                 NTHREADS == args.d_waitCount);
                }

                args.d_barrier.wait();

                while (0 != args.d_waitCount) {
#ifdef BSLS_PLATFORM__OS_AIX
                    bcemt_ThreadUtil::yield();
#endif
                }

                for (int i = 0; i < NUM_CHECK; ++i) {
                    LOOP2_ASSERT(n, args.d_waitCount, 0 == args.d_waitCount);
                }

                for (int i = 0; i < NTHREADS; ++i) {
                    bcemt_ThreadUtil::join(threadHandles[i]);
                }
            }
        }

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING DIRECT ACCESSORS:
        //
        // Concerns:
        //   Verify that the barrier correctly initializes with the specified
        //   'numThreads' value.
        //
        // Plan:
        //   For each of a sequence of independent values, construct a barrier
        //   and verify that it is initialized with the correct value.
        //
        // Testing:
        //   bcemt_Barrier(int numThreads);
        //   ~bcemt_Barrier();
        //   int numThreads();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing: direct accessors" << endl
                          << "=========================" << endl;

        {
            static const struct {
                int d_lineNum;
                int d_numThreads;
            } VALUES[] = {
                //line num threads
                //---- -----------
                { L_ ,           1 },
                { L_ ,           2 },
                { L_ ,          10 },
                { L_ ,          50 },
                { L_ ,         100 }
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE = VALUES[i].d_lineNum;
                const int NUM  = VALUES[i].d_numThreads;

                bcemt_Barrier x(NUM); const bcemt_Barrier &X = x;
                LOOP_ASSERT(LINE, NUM == X.numThreads());
            }
        }
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise basic functionality before beginning testing in earnest.
        //
        // Plan:
        //   Construct a barrier object for one thread.  Assert that
        //   the object initialized correctly using the 'numThreads' accessor
        //   function.  Then call 'wait' and 'timedWait' on the barrier.  The
        //   calls should return immediately.  Then destroy the barrier.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;

        {
            const bdet_TimeInterval TIMEOUT(1000);
                                            // 1000s, enough to ensure time out

            bcemt_Barrier b(1);
            ASSERT(1 == b.numThreads());
            b.wait();
            ASSERT(0 == b.timedWait(TIMEOUT));
        }
      } break;

      default: {
          testStatus = -1;
      }
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
