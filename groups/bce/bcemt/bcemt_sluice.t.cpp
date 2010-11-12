// bcemt_sluice.t.cpp                                                 -*-C++-*-
#include <bcemt_sluice.h>

#include <bcemt_lockguard.h>
#include <bcemt_mutex.h>
#include <bcemt_semaphore.h>    // for testing only
#include <bcemt_threadgroup.h>  // for testing only
#include <bcemt_threadutil.h>   // for testing only

#include <bces_atomictypes.h>

#include <bdef_bind.h>
#include <bdetu_systemtime.h>

#include <bslma_testallocator.h>
#include <bsls_stopwatch.h>
#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

#include <bsl_c_time.h>
#include <bsl_c_stdio.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
//-----------------------------------------------------------------------------
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
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

static bcemt_Mutex coutMutex;

#define MTCOUT   { coutMutex.lock(); cout << bcemt_ThreadUtil::selfIdAsInt() \
                                          << ": "
#define MTENDL   endl;  coutMutex.unlock(); }
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcemt_Sluice Obj;

class My_TestAllocator : public bslma_Allocator {

    // DATA
    bslma_TestAllocator d_allocator;
    bcemt_Mutex         d_lock;

  public:
    explicit
    My_TestAllocator(bool verbose)
        : d_allocator(verbose)
    {}

    virtual ~My_TestAllocator()
    {}

    virtual void *allocate(size_type size)
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
        return d_allocator.allocate(size);
    }

    virtual void deallocate(void *address)
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
        d_allocator.deallocate(address);
    }

    int numAllocations() {
        return d_allocator.numAllocations();
    }

    int numBytesInUse() {
        return d_allocator.numBytesInUse();
    }
};

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------

void enterAndWaitUntilDone(Obj            *sluice,
                           int            *done,
                           bcemt_Mutex    *lock,
                           bces_AtomicInt *iterations)
{
    while (1) {
        lock->lock();
        if (*done) {
            lock->unlock();
            break;
        }
        const void *token = sluice->enter();
        lock->unlock();
        sluice->wait(token);
        ++(*iterations);
    }
}

void enterPostSleepAndWait(Obj *sluice, bcemt_Semaphore *sem)
{
    const void *token = sluice->enter();

    sem->post();

    bcemt_ThreadUtil::sleep(bdet_TimeInterval(2));
    sluice->wait(token);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    My_TestAllocator ta(veryVeryVerbose);

    switch (test) {
      case 3: {
        // --------------------------------------------------------------------
        // STRESS AND ALLOCATOR TEST
        //
        // From many threads, enter and wait.  From the main thread,
        // continuously signal.  After several seconds, set a 'done' flag,
        // signal once, and join all threads.
        //
        // Execute this test with a TestAllocator to look for leaks.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Stress / Allocator Test" << endl
                          << "=======================" << endl;

        enum {
            NUM_WAITING_THREADS = 20,
            NUM_TEST_SECONDS = 3
        };

        Obj mX(&ta);
        int done = 0;
        bcemt_Mutex lock;
        bces_AtomicInt iterations = 0;

        bcemt_ThreadGroup tg;

        ASSERT(NUM_WAITING_THREADS ==
               tg.addThreads(bdef_BindUtil::bind(&enterAndWaitUntilDone,
                                                 &mX,
                                                 &done,
                                                 &lock,
                                                 &iterations),
                             NUM_WAITING_THREADS));
        bsls_Stopwatch timer;
        timer.start();
        while (timer.elapsedTime() < NUM_TEST_SECONDS) {
            mX.signalOne();
            mX.signalOne();
            mX.signalOne();
            mX.signalOne();
            mX.signalOne();
        }
        lock.lock();
        done = 1;
        lock.unlock();

        mX.signalAll();
        tg.joinAll();

        if (verbose) {
            P(iterations);
        }
        // we should ALWAYS be able to make WAY MORE THAN 100 iterations per
        // thread per second
        LOOP_ASSERT(iterations,
                    100 * NUM_WAITING_THREADS * NUM_TEST_SECONDS < iterations);
      }
      if (verbose) {
          P(ta.numAllocations());
      }
      ASSERT(0 < ta.numAllocations());
      ASSERT(0 == ta.numBytesInUse());

      break;
      case 2: {
        // --------------------------------------------------------------------
        // DELAYED WAIT TEST
        //
        // Concerns:
        //   * When a thread enters the gate, and another thread signals
        //     the condition, and THEN the first thread invokes wait(), it
        //     does not block.
        //   * As above, but with three waiting threads and signalAll() -
        //     using the same sluice as before.
        //   * Repeat the entire test using the same sluice.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Delayed Wait Test" << endl
                          << "=================" << endl;

        enum {
            NUM_SIGNALED_THREADS = 3,
            NUM_ITERATIONS = 2
        };

        Obj mX;

        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            bcemt_Semaphore readySem;

            bcemt_ThreadUtil::Handle h;
            int rc = bcemt_ThreadUtil::create(&h,
                                   bdef_BindUtil::bind(&enterPostSleepAndWait,
                                                       &mX, &readySem));
            BSLS_ASSERT(0 == rc); // test invariant

            readySem.wait();
            mX.signalOne();
            bcemt_ThreadUtil::join(h);

            bcemt_ThreadGroup tg;
            rc = tg.addThreads(bdef_BindUtil::bind(&enterPostSleepAndWait,
                                                   &mX, &readySem),
                               NUM_SIGNALED_THREADS);
            BSLS_ASSERT(NUM_SIGNALED_THREADS == rc); // test invariant

            for (int i = 0; i < NUM_SIGNALED_THREADS; ++i) {
                readySem.wait();
            }
            mX.signalAll();
            tg.joinAll();
        }
      } break; // success if we can reach the end of the test
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Exercises basic functionality.
        // --------------------------------------------------------------------

        bdet_TimeInterval future = bdetu_SystemTime::now();
        future.addSeconds(100);
        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;
        {
            Obj x;
            const void *t1 = x.enter();
            x.signalOne();
            x.wait(t1);
        }
        {
            Obj x;
            const void *t1 = x.enter();
            const void *t2 = x.enter();
            x.signalOne();
            x.signalOne();
            x.wait(t1);
            ASSERT(0 == x.timedWait(t2, future));
        }
        {
            Obj x;
            x.signalOne();
        }
        {
            Obj x;
            const void *t1 = x.enter();
            const void *t2 = x.enter();
            x.signalAll();
            x.wait(t1);
            ASSERT(0 == x.timedWait(t2, future));
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
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
