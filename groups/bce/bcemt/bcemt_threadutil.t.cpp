// bcemt_threadutil.t.cpp        -*-C++-*-
#include <bcemt_threadutil.h>

#include <bcemt_threadattributes.h>
#include <bces_atomictypes.h>

#include <bdef_bind.h>
#include <bdetu_systemtime.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

namespace {
    // unnamed namespace for local resources
                        // ===================
                        // class ThreadChecker
                        // ===================

class ThreadChecker {

   bces_AtomicInt d_count;

public:
    // CREATORS
    ThreadChecker() : d_count(0) {}

    // MANIPULATORS
    void eval() {
       ++d_count;
    }

    bdef_Function<void(*)()> getFunctor()
    {
       return bdef_BindUtil::bind(&ThreadChecker::eval, this);
    }

    // ACCESSORS
    int count() const {
       return d_count;
    }
};

extern "C" void *myThreadFunction(void *)
    // Print to standard output "Another second has passed"
    // every second for five seconds.
{
    for(int i = 0; i < 5; ++i) {
        bcemt_ThreadUtil::microSleep(1000);  // 1 msec used for efficiency
        if (verbose) bsl::cout << "Another millisecond has passed\n";
    }
    return 0;
}

extern "C" void *mySmallStackThreadFunction(void *threadArg)
    // Initialize a small object on the stack and do some work.
{
    char *initValue = (char *)threadArg;
    char Small[8];
    memset(&Small[0], *initValue, 8);
    // do some work ...
    return 0;
}

void createSmallStackSizeThread()
    // Create a detached thread with the small stack size
    // and perform some work
{
    enum { STACK_SIZE = 16384 };
    bcemt_ThreadAttributes attributes;
    attributes.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
    attributes.setStackSize(STACK_SIZE);

    char initValue = 1;
    bcemt_ThreadUtil::Handle handle;
    bcemt_ThreadUtil::create(&handle,
                             attributes,
                             mySmallStackThreadFunction,
                             &initValue);
}

}  // close unnamed namespace

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
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MICROSLEEP
        //
        // Concerns:
        //   That 'microSleep' always sleeps at least the given amount of
        //   time, and does not sleep an unreasonable amount of extra time.
        //
        // Plan:
        //   Several times, sleep for 0.3 seconds, then check how much time
        //   has passed and verify that it is within acceptable boundaries.
        // --------------------------------------------------------------------

        enum { SLEEP_MICROSECONDS = 300 * 1000 };
        const double SLEEP_SECONDS = SLEEP_MICROSECONDS * 1e-6;
        const double OVERSHOOT_MIN = -1e-5;
#if defined(BSLS_PLATFORM__OS_SOLARIS) || defined(BSLS_PLATFORM__OS_LINUX)
            const double TOLERANCE = 2;   // microSleep is obscenely
                                          // imprecise on Sun and to a
                                          // lesser extent Linux
#else
            const double TOLERANCE = 0.05;
#endif

        for (int i = 0; i < 8; ++i) {
            double start   = bdetu_SystemTime::now().totalSecondsAsDouble();
            bcemt_ThreadUtil::microSleep(SLEEP_MICROSECONDS);
            double elapsed = bdetu_SystemTime::now().totalSecondsAsDouble() -
                                                                         start;

            double overshoot = elapsed - SLEEP_SECONDS;

            if (veryVerbose) P(overshoot);

            LOOP_ASSERT( overshoot, overshoot >= OVERSHOOT_MIN);
            LOOP2_ASSERT(overshoot, TOLERANCE, overshoot < TOLERANCE);
        }
    } break;
    case 3: {
        // --------------------------------------------------------------------
        // TESTING usage examples
        //
        // Concern: that the usage examples (including those that were
        // previously in the 'bcemt_thread' component) compile and work
        // properly.
        // --------------------------------------------------------------------

        // BASIC EXAMPLE
        if (verbose) cout << "\nBasic thread utilities example" << endl;

        bcemt_ThreadAttributes attributes;
        bcemt_ThreadUtil::Handle handle;
        bcemt_ThreadUtil::create(&handle, attributes, myThreadFunction, 0);
        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::join(handle);
        if (verbose) bsl::cout << "A five second interval has elapsed\n";

        if (verbose) cout << "\nSmall stack example" << endl;

        createSmallStackSizeThread();
    } break;
    case 2: {
        // --------------------------------------------------------------------
        // TESTING 'isEqual'
        //
        // Concerns: A valid thread handle is equal to itself.  Two valid
        // thread handles are equal.  An invalid thread handle is unequal to
        // any other thread handle, including itself.
        //
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "\n isEqual test" << endl;
        }

        bcemt_ThreadUtil::Handle validH1, validH2;
        bcemt_ThreadUtil::create(&validH1, myThreadFunction, 0);
        bcemt_ThreadUtil::create(&validH2, myThreadFunction, 0);
        bcemt_ThreadUtil::Handle validH1copy = validH1;

        ASSERT(1 == bcemt_ThreadUtil::isEqual(validH1, validH1));
        ASSERT(1 == bcemt_ThreadUtil::isEqual(validH1, validH1copy));
        ASSERT(0 == bcemt_ThreadUtil::isEqual(validH1, validH2));
        ASSERT(0 == bcemt_ThreadUtil::isEqual(
                                             bcemt_ThreadUtil::invalidHandle(),
                                             validH2));
        ASSERT(0 == bcemt_ThreadUtil::isEqual(
                                             bcemt_ThreadUtil::invalidHandle(),
                                             validH1copy));
        ASSERT(0 == bcemt_ThreadUtil::isEqual(validH1,
                                           bcemt_ThreadUtil::invalidHandle()));
        ASSERT(1 == bcemt_ThreadUtil::isEqual(
                                           bcemt_ThreadUtil::invalidHandle(),
                                           bcemt_ThreadUtil::invalidHandle()));

        bcemt_ThreadUtil::join(validH1);
        bcemt_ThreadUtil::join(validH2);
    } break;
    case 1: {
        // --------------------------------------------------------------------
        // Invokable functor test
        // --------------------------------------------------------------------
       if (verbose) {
          cout << "\nInvokable functor test" << endl;
       }

       enum { THREAD_COUNT = 10 }; // Actually twice this many

       bcemt_ThreadAttributes detached;
       detached.setDetachedState(
                                bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);

       ThreadChecker joinableChecker;
       ThreadChecker detachedChecker;
       bcemt_ThreadUtil::Handle handles[THREAD_COUNT], dummy;
       for (int i = 0; i < THREAD_COUNT; ++i) {
          ASSERT(0 == bcemt_ThreadUtil::create(&handles[i],
                                               joinableChecker.getFunctor()));
          ASSERT(0 == bcemt_ThreadUtil::create(&dummy,
                                               detached,
                                               detachedChecker.getFunctor()));
       }

       // Join the joinable threads
       for (int i = 0; i < THREAD_COUNT; ++i) {
          ASSERT(0 == bcemt_ThreadUtil::join(handles[i]));
       }

       int iterations = 100;
       while ((THREAD_COUNT != joinableChecker.count() ||
               THREAD_COUNT != detachedChecker.count()) &&
              0 < --iterations)
       {
           bcemt_ThreadUtil::microSleep(100 * 1000);  // 100 msec
           bcemt_ThreadUtil::yield();
       }

       ASSERT(THREAD_COUNT == joinableChecker.count());
       ASSERT(THREAD_COUNT == detachedChecker.count());
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
