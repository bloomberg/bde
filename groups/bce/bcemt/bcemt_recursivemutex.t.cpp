// bcemt_recursivemutex.t.cpp                                         -*-C++-*-
#include <bcemt_recursivemutex.h>

#include <bcemt_threadattributes.h>
#include <bcemt_threadutil.h>
#include <bces_atomictypes.h>

#include <bsls_stopwatch.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//-----------------------------------------------------------------------------
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
#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";   \
                aSsErT(1, #X, __LINE__); } }
#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J <<         \
                        "\t" << #K << ": " << K << "\n";                      \
                aSsErT(1, #X, __LINE__); } }
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o line feed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bcemt_RecursiveMutex Obj;

extern "C" {
   typedef void *(*ThreadFunction)(void *);
}

void My_Sleep(int ms)
{
    bcemt_ThreadUtil::microSleep(1000 * ms);
}

void My_CreateDetachedThread(ThreadFunction function,
                             void* userData) {
    bcemt_ThreadAttributes attr;
    attr.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
    bcemt_ThreadUtil::Handle dum;
    bcemt_ThreadUtil::create(&dum, attr, function, userData);
}

struct Case1 {
    bcemt_RecursiveMutex *d_lock;
    bces_AtomicInt d_retval;
    bces_AtomicInt d_retvalSet;
};

extern "C" void* Case1Thread(void* arg_p) {
    Case1* arg = (Case1*)arg_p;

    arg->d_retval = arg->d_lock->tryLock();
    if (0 == arg->d_retval) {
        arg->d_lock->unlock();
    }
    arg->d_retvalSet = 1;
    return arg_p;
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
    case 1: {
        // ------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy a RecursiveMutex.  Lock; verify that tryLock
        // succeeds; from another thread, verify that tryLock fails;
        // unlock and verify that tryLock fails; unlock again and verify that
        // tryLock succeeds.
        // ------------------------------------------------------------------
        if (verbose) {
            cout << "BREATHING TEST" << endl;
            cout << "==============" << endl;
        }
        {
            enum {
                MAX_SLEEP_CYCLES = 1000
            };

            bcemt_RecursiveMutex lock;
            lock.lock();
            ASSERT(0 == lock.tryLock());

            Case1 args;
            args.d_lock = &lock;
            args.d_retval = 0;
            args.d_retvalSet = 0;
            ASSERT(0 == args.d_retvalSet);
            My_CreateDetachedThread(Case1Thread, &args);

            bsls_Stopwatch sleeptime;
            sleeptime.start();
            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                My_Sleep(50);
            }
            if (verbose) {
                cout << "Slept for " << sleeptime.elapsedTime() << endl;
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail
            if (verbose) {
                P(args.d_retval);
            }

            lock.unlock();
            args.d_retval = 0;
            args.d_retvalSet = 0;
            My_CreateDetachedThread(Case1Thread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                My_Sleep(50);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail again, still 1 lock
            if (verbose) {
                P(args.d_retval);
            }

            lock.unlock();
            args.d_retval = -1;
            args.d_retvalSet = 0;
            My_CreateDetachedThread(Case1Thread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                My_Sleep(50);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 == args.d_retval); // now should succeed
            if (verbose) {
                P(args.d_retval);
            }
        }
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
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
