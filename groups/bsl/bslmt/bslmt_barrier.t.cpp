// bslmt_barrier.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_barrier.h>

#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_atomic.h>  // for testing only

#include <bsls_platform.h>
#include <bsls_spinlock.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>
#include <bsls_timeutil.h>

/* TBD -- bind
#include <bdlf_bind.h>
*/

#include <bsl_algorithm.h>
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
// [2] bslmt::Barrier(int numThreads);
// [2] ~bslmt::Barrier();
// [3] void wait();
// [4] void timedWait(const bsls::TimeInterval& timeOut);
// [2] int numThreads();
//-----------------------------------------------------------------------------
// [1] Breathing test
// [5] Testing interactions between 'wait' and 'timedWait'
// [6] USAGE Example

static bsls::SpinLock coutLock = BSLS_SPINLOCK_UNLOCKED;

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

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

struct ThreadArgs {
    // This structure is used to pass arguments to the thread functions that
    // are used in testing.  The use of the individual data members depends on
    // the context.

    bslmt::Barrier  d_barrier;     // barrier used for testing

    bsls::AtomicInt d_waitCount;   // count of threads waiting or about to
                                  // wait on the barrier being tested

    int            d_timeOut;     // time out to use when calling 'timedWait'
                                  // (in microseconds)

    bsls::AtomicInt d_numTimedOut; // number of threads that time out on
                                  // a call to 'timedWait'

    bsls::SystemClockType::Enum d_clockType;
                                  // clock type to use for 'timedWait' method

    ThreadArgs(int                         barrierCount,
               int                         timeOut = 0,
               bsls::SystemClockType::Enum clock
                                           = bsls::SystemClockType::e_REALTIME)
        : d_barrier(barrierCount, clock)
        , d_waitCount(0)
        , d_timeOut(timeOut)
        , d_numTimedOut(0)
        , d_clockType(clock)
    {
    }
};

// ----------------------------------------------------------------------------
//                      HELPER FUNCTIONS FOR TEST CASE 9
// ----------------------------------------------------------------------------

namespace case9 {

struct Control
{
    int           d_numIterations;
    int           d_numThreads;

    bslmt::Barrier d_b1;
    bslmt::Barrier d_b2;

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

        c->d_b2.~Barrier();

        bsl::memset(mem, filler, sizeof(c->d_b2));

        bslmt::ThreadUtil::yield();
        new (mem) bslmt::Barrier(c->d_numThreads);
    }
    return 0;
}

void test(int numIterations, int numThreads)
{
    bsl::vector<bslmt::ThreadUtil::Handle>  hh;
    Control c(numIterations, numThreads);

    for (int i = 0; i<numThreads-1; ++i) {
        bslmt::ThreadUtil::Handle h;
        bslmt::ThreadUtil::create(&h, &a, (void *)&c);
        hh.push_back(h);
    }
    bslmt::ThreadUtil::Handle h;
    bslmt::ThreadUtil::create(&h, &b, (void *)&c);
    hh.push_back(h);

    for (int i = 0; i<numThreads; ++i) {
        bslmt::ThreadUtil::join(hh[i]);
    }
}

}  // close namespace case9

// ----------------------------------------------------------------------------
//                      HELPER FUNCTIONS FOR TEST CASE 5
// ----------------------------------------------------------------------------

extern "C" void * testThread5a(void *arg)
    // This function is used to test the 'wait' and 'timedWait' methods
    // together: it uses the 'd_waitCount' atomic integer provided 'ThreadArgs'
    // structure to make sure that the call to the 'wait' method of the
    // provided barrier occurs before the call to 'timedWait'.
{
    ThreadArgs *args = (ThreadArgs*)arg;

    if (++args->d_waitCount == 1) {
        args->d_barrier.wait();
    }
    else {
        bsls::TimeInterval timeOut(bsls::SystemTime::now(args->d_clockType));
        timeOut.addMicroseconds(args->d_timeOut);
        int res = args->d_barrier.timedWait(timeOut);
        if (res) ++args->d_numTimedOut;
    }

    return arg;
}

extern "C" void * testThread5b(void *arg)
    // This function is used to test the 'wait' and 'timedWait' methods
    // together: it uses the 'd_waitCount' atomic integer of the 'ThreadArgs'
    // structure to make sure that the call to the 'timedWait' method of the
    // provided barrier occurs before the call to 'wait'.
{
    ThreadArgs *args = (ThreadArgs*)arg;

    if (++args->d_waitCount == 1) {
        bsls::TimeInterval timeOut(bsls::SystemTime::now(args->d_clockType));
        timeOut.addMicroseconds(args->d_timeOut);
        int res = args->d_barrier.timedWait(timeOut);
        if (res) ++args->d_numTimedOut;
    }
    else {
        args->d_barrier.wait();
    }

    return arg;
}

// ----------------------------------------------------------------------------
//                      HELPER FUNCTIONS FOR TEST CASE 4
// ----------------------------------------------------------------------------

struct ThreadArgs4 {
    // This structure is used to pass arguments to the thread functions that
    // are used in testing.  The use of the individual data members depends on
    // the context.

    bslmt::Barrier  d_barrier;     // barrier used for testing

    int            d_timeOut;     // time out to use when calling 'timedWait'
                                  // (in microseconds)

    bsls::AtomicInt d_stopCount;   // count indicating that thread has complete
                                  // its test

    bsls::AtomicInt d_numTimedOut; // number of threads that time out on
                                  // a call to 'timedWait'

    int            d_nThreads;    // number of worker threads

    bsls::SystemClockType::Enum d_clockType;
                                  // clock type used for 'timedWait' method

    ThreadArgs4(int nThreads, int timeOut, bsls::SystemClockType::Enum clock)
    : d_barrier(nThreads + 1, clock)
    , d_timeOut(timeOut)
    , d_stopCount(0)
    , d_numTimedOut(0)
    , d_nThreads(nThreads)
    , d_clockType(clock)
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

    bsls::TimeInterval start(bsls::SystemTime::now(args->d_clockType));
    bsls::TimeInterval timeOut(start);
    timeOut.addMicroseconds(args->d_timeOut);

    int res = args->d_barrier.timedWait(timeOut);

    bsls::TimeInterval end(bsls::SystemTime::now(args->d_clockType));
    bsls::TimeInterval duration = end - start;

    if (res) {
        // relax timing by 50us for Windows
#ifdef BSLS_PLATFORM_OS_WINDOWS
        const int relax = 2000;
#else
        const int relax = 0;
#endif

        ++args->d_numTimedOut;

        LOOP3_ASSERT(args->d_clockType,
                     args->d_timeOut,
                     duration.totalMicroseconds(),
                     duration.totalMicroseconds() + relax >= args->d_timeOut);
    }

    // Wait until all the threads have stopped.

    ++args->d_stopCount;
    while (args->d_nThreads + 1 != args->d_stopCount) {
#ifdef BSLS_PLATFORM_OS_AIX
        // Avoid spinning as it can block on some overoptimized platforms.

        bslmt::ThreadUtil::yield();
#endif
    }

    return arg;
}

// ----------------------------------------------------------------------------
//                      HELPER FUNCTIONS FOR TEST CASE 3
// ----------------------------------------------------------------------------

extern "C" void * testThread3(void *arg)
    // This function is used to test the 'wait' method: it begins by
    // incrementing the 'd_waitCount' member of the provided 'ThreadArgs'
    // structure, then calls the 'wait' method of the provided barrier, and
    // finally decrements 'd_waitCount' and returns.
{
    ThreadArgs *args = (ThreadArgs*)arg;

    ++args->d_waitCount;
    args->d_barrier.wait();
    --args->d_waitCount;

    return arg;
}

// ============================================================================
//                         USAGE EXAMPLE FROM HEADER
// ----------------------------------------------------------------------------

///Usage
///-----
// The following example demonstrates the use of a 'bslmt::Barrier' to create
// "checkpoints" in a threaded "basket trade" processing logic.  In this
// example, a "basket" is a series of trades submitted as one logical trade.
// If any given trade fails to process for any reason, then all the trades must
// be cancelled.
//
// The example is driven through function 'processBasketTrade', which takes as
// its argument a reference to a 'BasketTrade' structure.  The 'BasketTrade'
// structure contains a collection of 'Trade' objects; the 'processBasketTrade'
// function creates a separate thread to manage each 'Trade' object.
//
// The 'Trade' threads proceed independently, except that they synchronize with
// one another at various stages of the trade processing: each thread waits for
// all trades to complete a given step before any individual trade thread
// proceeds to the next step.
//
// The 'bslmt::Barrier' is used repeatedly at each processing stage to wait for
// all trades to complete the given stage before continuing to the next stage.
//
// To begin, we define the fundamental structures 'Trade' and 'BasketTrade'.
//..
    enum {
        k_MAX_BASKET_TRADES = 10
    };

    struct Trade {
        // Trade stuff...
    };

    struct BasketTrade {
        bsl::vector<Trade> d_trades;  // array of trade that comprise the
                                      // basket
    };
//..
// Functions 'validateTrade', 'insertToDatabase', and 'submitToExchange' define
// functionality for the three stages of trade processing.  Again, the
// 'bslmt::Barrier' will be used so that no individual trade moves forward to
// the next stage before all trades have completed the given stage.  So, for
// instance, no individual trade can call the 'insertToDatabase' function until
// all trades have successfully completed the 'validateTrade' function.
//
// Functions 'deleteFromDatabase' and 'cancelAtExchange' are used for rolling
// back all trades in the event that any one trade fails to move forward.
//
// The implementation of these functions is left incomplete for our example.
//..
    int validateTrade(Trade &trade)
    {
        (void)trade;
        int result = 0;
        // Do some checking here...

        return result;
    }

    int insertToDatabase(Trade &trade)
    {
        (void)trade;
        int result = 0;
        // Insert the record here...

        return result;
    }

    int submitToExchange(Trade &trade)
    {
        (void)trade;
        int result = 0;
        // Do submission here...

        return result;
    }

    int deleteFromDatabase(Trade &trade)
    {
        (void)trade;
        int result = 0;
        // Delete record here...

        return result;
    }

    int cancelAtExchange(Trade &trade)
    {
        (void)trade;
        int result = 0;
        // Cancel trade here...

        return result;
    }
//..
// The 'processTrade' function handles a single trade within a Trade Basket.
// Because this function is called within a 'bslmt::Thread' callback (see the
// 'tradeProcessingThread' function, below), its arguments are passed in a
// single structure.  The 'processTrade' function validates a trade, stores the
// trade into a database, and registers that trade with an exchange.  At each
// step, the 'processTrade' function synchronizes with other trades in the
// Trade Basket.
//..
    struct TradeThreadArgument {
        bsl::vector<Trade> *d_trades_p;
        bslmt::Barrier     *d_barrier_p;
        volatile bool      *d_errorFlag_p;
        int                 d_tradeNum;
    };

    TradeThreadArgument *processTrade(TradeThreadArgument *arguments)
    {
        int retval;
        Trade &trade = (*arguments->d_trades_p)[arguments->d_tradeNum];

        retval = validateTrade(trade);
//..
// If this trade failed validation, then indicate that an error has occurred.
// Note that, even when an error occurs, we must still block on the barrier
// object; otherwise, other threads which did not fail would remain blocked
// indefinitely.
//..
        if (retval) *arguments->d_errorFlag_p = true;
        arguments->d_barrier_p->wait();
//..
// Once all threads have completed the validation phase, check to see if any
// errors occurred; if so, exit.  Otherwise continue to the next step.
//..
        if (*arguments->d_errorFlag_p) return arguments;              // RETURN

        retval = insertToDatabase(trade);
        if (retval) *arguments->d_errorFlag_p = true;
        arguments->d_barrier_p->wait();
//..
// As before, if an error occurs on this thread, we must still block on the
// barrier object.  This time, if an error has occurred, we need to check to
// see whether this trade had an error.  If not, then the trade has been
// inserted into the database, so we need to remove it before we exit.
//..
        if (*arguments->d_errorFlag_p) {
            if (!retval) deleteFromDatabase(trade);
            return arguments;                                         // RETURN
        }
//..
// The final synchronization point is at the exchange.  As before, if there is
// an error in the basket, we may need to cancel the individual trade.
//..
        retval = submitToExchange(trade);
        if (retval) *arguments->d_errorFlag_p = true;
        arguments->d_barrier_p->wait();
        if (*arguments->d_errorFlag_p) {
            if (!retval) cancelAtExchange(trade);
            deleteFromDatabase(trade);
            return arguments;                                         // RETURN
        }
//..
// All synchronized steps have completed for all trades in this basket.  The
// basket trade is placed.
//..
        return arguments;
    }
//..
// Function 'tradeProcessingThread' is a callback for 'bslmt::ThreadUtil',
// which requires 'void' pointers for argument and return type and 'extern "C"'
// linkage.  'bslmt::ThreadUtil::create()' expects a pointer to this function,
// and provides that function pointer to the newly created thread.  The new
// thread then executes this function.
//
// The 'tradeProcessingThread' function receives the 'void' pointer, casts it
// to our required type ('TradeThreadArgument *'), and then calls the
// type-specific function, 'processTrade'.  On return, the specific type is
// cast back to 'void*'.
//..
    extern "C" void *tradeProcessingThread(void *argumentsIn)
    {
        return (void *) processTrade ((TradeThreadArgument *)argumentsIn);
    }
//..
// Function 'processBasketTrade' drives the example.  Given a 'BasketTrade',
// the function spawns a separate thread for each individual trade in the
// basket, supplying the function 'tradeProcessingThread' to be executed on
// each thread.
//..
    bool processBasketTrade(BasketTrade& trade)
        // Return 'true' if the specified basket 'trade' was processed
        // successfully, and 'false' otherwise.  The 'trade' is processed
        // atomically, i.e., all the trades succeed, or none of the trades are
        // executed.
    {
        TradeThreadArgument arguments[k_MAX_BASKET_TRADES];
        bslmt::ThreadAttributes attributes;
        bslmt::ThreadUtil::Handle threadHandles[k_MAX_BASKET_TRADES];

        int numTrades = trade.d_trades.size();
        ASSERT(0 < numTrades && k_MAX_BASKET_TRADES >= numTrades);
//..
// Construct the barrier that will be used by the processing threads.  Since a
// thread will be created for each trade in the basket, use the number of
// trades as the barrier count.  When 'bslmt::Barrier::wait()' is called, the
// barrier will require 'numTrades' objects to wait before all are released.
//..
        bslmt::Barrier barrier(numTrades);
        bool errorFlag = false;
//..
// Create a thread to process each trade.
//..
        for (int i = 0; i < numTrades; ++i) {
            arguments[i].d_trades_p    = &trade.d_trades;
            arguments[i].d_barrier_p   = &barrier;
            arguments[i].d_errorFlag_p = &errorFlag;
            arguments[i].d_tradeNum    = i;
            bslmt::ThreadUtil::create(&threadHandles[i],
                                      attributes,
                                      tradeProcessingThread,
                                      &arguments[i]);
        }
//..
// Wait for all threads to complete.
//..
        for (int i = 0; i < numTrades; ++i) {
            bslmt::ThreadUtil::join(threadHandles[i]);
        }
//..
// Check if any error occurred.
//..
        return false == errorFlag;
    }
//..

class Case8_Driver
{
   bsls::AtomicInt *d_state;
   bslmt::Barrier *d_barrier;
   int d_numCycles;

public:

   Case8_Driver(bsls::AtomicInt *state, bslmt::Barrier *barrier, int numCycles)
   : d_state(state), d_barrier(barrier), d_numCycles(numCycles) {}

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

   typedef bslmt::Barrier Barrier;

   Barrier        *d_barrier;
   bsls::AtomicInt *d_state;
   int             d_numWaits;
   int             d_numThreads;

public:
   Case7_Waiter(Barrier         *barrier=0,
                bsls::AtomicInt *state=0,
                int              numWaits=0,
                int              numThreads=0)
   : d_barrier(barrier),
     d_state(state),
     d_numWaits(numWaits),
     d_numThreads(numThreads)
   {}

   void begin() {
      ASSERT(0 != d_state);
      bslmt::ThreadAttributes detached;
      bslmt::ThreadUtil::Handle dummy;
      detached.setDetachedState(
                               bslmt::ThreadAttributes::e_CREATE_DETACHED);

      /* TBD -- bind
      ASSERT(0 == bslmt::ThreadUtil::create(&dummy, detached,
                           bdlf::BindUtil::bind(&Case7_Waiter::run,
                                               this)));
      */
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

void case7(bslmt::Barrier *barrier, bool verbose, int numThreads, int numWaits)
{
   bsls::AtomicInt state(0);
   vector<Case7_Waiter > waiters;
   waiters.insert(waiters.end(), numThreads-1,
                  Case7_Waiter(barrier, &state,
                               numWaits, numThreads));
   for_each(waiters.begin(), waiters.end(), mem_fun_ref(&Case7_Waiter::begin));
   for (int i = 0; i < numWaits; ++i) {
      ASSERT(state == i * numThreads);
      barrier->wait();
      state += 1;
      if (verbose) {
         coutLock.lock();
         cout << "Waiting: " << i+1 << "/" << numWaits+1 << endl;
         coutLock.unlock();
      }
      while (state < ((i+1) * numThreads)) bslmt::ThreadUtil::yield();
   }
   ASSERT(state == numWaits * numThreads);
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
           k_NUM_THREADS = 4,
           k_NUM_ITERATIONS =5000
        };

        case9::test(k_NUM_ITERATIONS, k_NUM_THREADS);

      } break;

      case 8: {
        if (verbose) {
           cout << "2-thread reuse test" << endl;
        }
        // ----------------------------------------------------------------
        // 2-thread reuse test
        //
        // Concern: That if the second thread in a two-thread group enters a
        // second wait cycle while the first thread is in the middle of its
        // first wait(), the second thread still waits in the second wait cycle
        // as expected.
        //
        // ----------------------------------------------------------------
        enum {
           k_NUM_WAIT_CYCLES = 50
        };

        bslmt::Barrier normalBarrier(2);

        bslmt::ThreadAttributes detached;
        bsls::AtomicInt state(0);
        bslmt::ThreadUtil::Handle dummy;
        detached.setDetachedState(
                               bslmt::ThreadAttributes::e_CREATE_DETACHED);

        if (veryVerbose) {
           cout << "Unencumbered test" << endl;
        }

        Case8_Driver driver(&state,
                            &normalBarrier,
                            k_NUM_WAIT_CYCLES);

        ASSERT(0 == bslmt::ThreadUtil::create(&dummy, detached, driver));

        for (int i = 0; i < k_NUM_WAIT_CYCLES; ++i) {
           if (veryVerbose) {
              cout << "Cycle " << i+1 << "/" << k_NUM_WAIT_CYCLES << endl;
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
        // Concern: that bslmt::Barrier is reusable by the same thread group,
        // i.e., that N threads, after passing wait(), may pass a second wait()
        // correctly.
        //
        // Plan: set up N threads to wait on a shared barrier, with *one* of
        // the threads switching a state variable before calling wait().  There
        // will be M wait() calls, and M after-wait states.  Verify that the
        // N-1 other threads only enter each after-wait state after the N'th
        // thread has switched the variable.
        // ----------------------------------------------------------------
       if (verbose) {
          cout << "\nReusable barrier test" << endl;
       }
       enum {
          k_NUM_THREADS = 4,
          k_NUM_SHORT_WAITS = 100,
          k_NUM_LONG_WAITS = 40
       };

       // These are test invariants rather than component invariants:
       ASSERT(k_NUM_THREADS * k_NUM_SHORT_WAITS < numeric_limits<int>::max());
       ASSERT(k_NUM_LONG_WAITS < k_NUM_SHORT_WAITS);
       bslmt::Barrier basicBarrier(k_NUM_THREADS);
       if (veryVerbose) {
          cout << "   ...Basic barrier..." << endl;
       }
       /* TBD -- bind
       case7(&basicBarrier, veryVerbose, k_NUM_THREADS, k_NUM_SHORT_WAITS);
       */

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
                  k_NUM_TRADES = 5
              };

              for (int i = 0; i < k_NUM_TRADES; ++i) {
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
                k_NTHREADS = 2,
                k_TIMEOUT  = 100000 // 0.1s, in microseconds
            };

            bslmt::ThreadAttributes attributes;

            {
                if (veryVerbose) cout << "\t\tUsing REALTIME clock" << endl;

                ThreadArgs args(k_NTHREADS,
                                k_TIMEOUT,
                                bsls::SystemClockType::e_REALTIME);
                bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                             testThread5a, &args);
                }

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::join(threadHandles[i]);
                }

                ASSERT(0 == args.d_numTimedOut);

                ThreadArgs args1(k_NTHREADS,
                                 k_TIMEOUT,
                                 bsls::SystemClockType::e_REALTIME);

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                             testThread5b, &args1);
                }

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::join(threadHandles[i]);
                }

                ASSERT(0 == args1.d_numTimedOut);
            }

            {
                if (veryVerbose) cout << "\t\tUsing MONOTONIC clock" << endl;

                ThreadArgs args(k_NTHREADS,
                                k_TIMEOUT,
                                bsls::SystemClockType::e_MONOTONIC);
                bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                             testThread5a, &args);
                }

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::join(threadHandles[i]);
                }

                ASSERT(0 == args.d_numTimedOut);

                ThreadArgs args1(k_NTHREADS,
                                 k_TIMEOUT,
                                 bsls::SystemClockType::e_MONOTONIC);

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                             testThread5b, &args1);
                }

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::join(threadHandles[i]);
                }

                ASSERT(0 == args1.d_numTimedOut);
            }
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
        //   assert that they do not time out before k_TIMEOUT.  This is done
        //   by measuring the time between the 'timedWait' initiation and
        //   return in each thread.
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
        //   void timedWait(const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing: 'timedWait'" << endl
                          << "====================" << endl;
        enum {
            k_NTHREADS   = 5,
            k_TIMEOUT    = 1000000 // 1s, in microseconds
        };

        if (verbose)
            cout << "\tTesting 'timedWait' with timeouts" << endl
                 << "\t---------------------------------" << endl;

        bslmt::ThreadAttributes attributes;

        {
            if (veryVerbose) cout << "\t\tUsing REALTIME clock" << endl;

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];

            {
                ThreadArgs4 args(k_NTHREADS,
                                 k_TIMEOUT,
                                 bsls::SystemClockType::e_REALTIME);
                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                             testThread4, &args);
                }

                // Wait until all the threads have stopped.
                ++args.d_stopCount;
                while (k_NTHREADS + 1 != args.d_stopCount) {
#ifdef BSLS_PLATFORM_OS_AIX
                    bslmt::ThreadUtil::yield();
#endif
                }

                LOOP_ASSERT(args.d_numTimedOut,
                            k_NTHREADS == args.d_numTimedOut);
                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::join(threadHandles[i]);
                }
            }
        }

        {
            if (veryVerbose) cout << "\t\tUsing MONOTONIC clock" << endl;

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];

            {
                ThreadArgs4 args(k_NTHREADS,
                                 k_TIMEOUT,
                                 bsls::SystemClockType::e_MONOTONIC);
                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                             testThread4, &args);
                }

                // Wait until all the threads have stopped.
                ++args.d_stopCount;
                while (k_NTHREADS + 1 != args.d_stopCount) {
#ifdef BSLS_PLATFORM_OS_AIX
                    bslmt::ThreadUtil::yield();
#endif
                }

                LOOP_ASSERT(args.d_numTimedOut,
                            k_NTHREADS == args.d_numTimedOut);
                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::join(threadHandles[i]);
                }
            }
        }

        if (verbose)
            cout << "\tTesting 'timedWait' without timeouts" << endl
                 << "\t------------------------------------" << endl;

        {
            if (veryVerbose) cout << "\t\tUsing REALTIME clock" << endl;

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];

            ThreadArgs4 args(k_NTHREADS,
                             60 * k_TIMEOUT,
                             bsls::SystemClockType::e_REALTIME);
            for (int n = 0; n < 2; ++n)
            {
                if (verbose) cout << "\titeration number " << n << endl;
                args.d_stopCount = 0;

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                             testThread4, &args);
                }

                // Note: previous testing used 'wait()', but this fails to make
                // sure that the 'timedWait' function also succeeds in
                // unlocking the other threads, and can lead to a k_TIMEOUT
                // failure if the worker threads time out before 'wait' is
                // called.  We prefer to use the 'timedWait' function instead
                // and make sure it does not time out.  If it does time out,
                // then the test will not be able to run properly, but this may
                // be due to the failure of the test driver to reach line 621:
                // 'args.d_barrier.timedWait()' below before the worker threads
                // have already timed out (timer failure), so we do not assert
                // and issue a warning instead.

                bsls::TimeInterval timeOut =
                                          bsls::SystemTime::nowRealtimeClock();
                timeOut.addMicroseconds(60 * k_TIMEOUT);  // 60s

                const int res = args.d_barrier.timedWait(timeOut);
                ASSERT(0 == res);

                if (0 == res ) {
                    ++args.d_stopCount;
                    while(k_NTHREADS + 1 != args.d_stopCount) {
#ifdef BSLS_PLATFORM_OS_AIX
                        bslmt::ThreadUtil::yield();
#endif
                    }

                    LOOP2_ASSERT(n, args.d_numTimedOut,
                                 0 == args.d_numTimedOut);

                    for (int i = 0; i < k_NTHREADS; ++i) {
                        bslmt::ThreadUtil::join(threadHandles[i]);
                    }
                }
                else {
                    // This can fail if 'timedWait' above gets executed after a
                    // worker thread already timed out, but the extremely long
                    // k_TIMEOUT makes this really unlikely! Just to be
                    // complete, we avoid locking this test driver
                    // indefinitely.  We still need to unlock the waiting
                    // threads.

                    ++args.d_stopCount;
                    for (int i = 0; i < k_NTHREADS; ++i) {
                        bslmt::ThreadUtil::join(threadHandles[i]);
                    }
                }
            }
        }

        {
            if (veryVerbose) cout << "\t\tUsing MONOTONIC clock" << endl;

            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];

            ThreadArgs4 args(k_NTHREADS,
                             60 * k_TIMEOUT,
                             bsls::SystemClockType::e_MONOTONIC);
            for (int n = 0; n < 2; ++n)
            {
                if (verbose) cout << "\titeration number " << n << endl;
                args.d_stopCount = 0;

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                                             testThread4, &args);
                }

                // Note: previous testing used 'wait()', but this fails to make
                // sure that the 'timedWait' function also succeeds in
                // unlocking the other threads, and can lead to a k_TIMEOUT
                // failure if the worker threads time out before 'wait' is
                // called.  We prefer to use the 'timedWait' function instead
                // and make sure it does not time out.  If it does time out,
                // then the test will not be able to run properly, but this may
                // be due to the failure of the test driver to reach line 621:
                // 'args.d_barrier.timedWait()' below before the worker threads
                // have already timed out (timer failure), so we do not assert
                // and issue a warning instead.

                bsls::TimeInterval timeOut =
                                         bsls::SystemTime::nowMonotonicClock();
                timeOut.addMicroseconds(60 * k_TIMEOUT);  // 60s

                const int res = args.d_barrier.timedWait(timeOut);
                ASSERT(0 == res);

                if (0 == res ) {
                    ++args.d_stopCount;
                    while(k_NTHREADS + 1 != args.d_stopCount) {
#ifdef BSLS_PLATFORM_OS_AIX
                        bslmt::ThreadUtil::yield();
#endif
                    }

                    LOOP2_ASSERT(n, args.d_numTimedOut,
                                 0 == args.d_numTimedOut);

                    for (int i = 0; i < k_NTHREADS; ++i) {
                        bslmt::ThreadUtil::join(threadHandles[i]);
                    }
                }
                else {
                    // This can fail if 'timedWait' above gets executed after a
                    // worker thread already timed out, but the extremely long
                    // k_TIMEOUT makes this really unlikely! Just to be
                    // complete, we avoid locking this test driver
                    // indefinitely.  We still need to unlock the waiting
                    // threads.

                    ++args.d_stopCount;
                    for (int i = 0; i < k_NTHREADS; ++i) {
                        bslmt::ThreadUtil::join(threadHandles[i]);
                    }
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
            const int k_NTHREADS = 4;
            ThreadArgs args(k_NTHREADS+1);
            bslmt::ThreadUtil::Handle threadHandles[k_NTHREADS];
            bslmt::ThreadAttributes attributes;

            for (int n = 0; n < 2; ++n)
            {
                if (verbose) cout << "\titeration number " << n << endl;
                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::create(&threadHandles[i], attributes,
                            testThread3, &args);
                }
                while (k_NTHREADS != args.d_waitCount) {
#ifdef BSLS_PLATFORM_OS_AIX
                    // avoid spinning as this can preempt on some platforms

                    bslmt::ThreadUtil::yield();
#endif
                }
                enum { k_NUM_CHECK = 100000 };
                for (int i = 0; i < k_NUM_CHECK; ++i) {
                    LOOP2_ASSERT(n, args.d_waitCount,
                                 k_NTHREADS == args.d_waitCount);
                }

                args.d_barrier.wait();

                while (0 != args.d_waitCount) {
#ifdef BSLS_PLATFORM_OS_AIX
                    bslmt::ThreadUtil::yield();
#endif
                }

                for (int i = 0; i < k_NUM_CHECK; ++i) {
                    LOOP2_ASSERT(n, args.d_waitCount, 0 == args.d_waitCount);
                }

                for (int i = 0; i < k_NTHREADS; ++i) {
                    bslmt::ThreadUtil::join(threadHandles[i]);
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
        //   bslmt::Barrier(int numThreads);
        //   ~bslmt::Barrier();
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
                //line # threads
                //---- ---------
                { L_ ,         1 },
                { L_ ,         2 },
                { L_ ,        10 },
                { L_ ,        50 },
                { L_ ,       100 }
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE = VALUES[i].d_lineNum;
                const int NUM  = VALUES[i].d_numThreads;

                {
                    bslmt::Barrier x(NUM); const bslmt::Barrier &X = x;
                    LOOP_ASSERT(LINE, NUM == X.numThreads());
                }

                {
                    bslmt::Barrier x(NUM, bsls::SystemClockType::e_REALTIME);
                    const bslmt::Barrier &X = x;
                    LOOP_ASSERT(LINE, NUM == X.numThreads());
                }

                {
                    bslmt::Barrier x(NUM, bsls::SystemClockType::e_MONOTONIC);
                    const bslmt::Barrier &X = x;
                    LOOP_ASSERT(LINE, NUM == X.numThreads());
                }
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
            const bsls::TimeInterval k_TIMEOUT(1000);
                                            // 1000s, enough to ensure time out

            bslmt::Barrier b(1);
            ASSERT(1 == b.numThreads());
            b.wait();
            ASSERT(0 == b.timedWait(k_TIMEOUT));
        }
      } break;

      default: {
          testStatus = -1;
      }
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
