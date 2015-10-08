// bslmt_once.t.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_once.h>

#include <bslmt_barrier.h>

#include <bslim_testutil.h>

#include <bsls_timeutil.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

#include <bslma_default.h>

#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bsls_timeinterval.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Testing the linear mechanics of 'bslmt::Once' and 'bslmt::OnceGuard' is
// straight-forward.  However, because this component is intended to produce
// thread-safe behavior in the presence of thread contention, testing must
// focus on deliberately creating contention and verifying the integrity of the
// mechanics.  The key concept is that when multiple threads try to enter a
// one-time code region simultaneously, only one thread succeeds and the others
// block until the first thread exits the region.  Other threads are then
// unblocked and skip the one-time region.  We detect that only one thread has
// entered the region by making sure that each thread does modifies a different
// data structure and verifying that only one of the data structures was
// modified.  We verify that all threads wait until the first one exits the
// region by showing that the side-effect of executing the one-time code is
// seen by all threads.
//
// In each test we create contention by starting multiple threads and having
// them wait at a barrier so that they all arrive at the once-code as close to
// simultaneously as possible.  Using this technique, we can test the basic
// 'enter', 'leave', and 'cancel' functionality of 'bslmt::Once', the
// 'bslmt::OnceGuard' class, and the 'BSLMT_ONCE_DO' macro.  We round out the
// tests with a breathing test and usage example.
//-----------------------------------------------------------------------------
// [ 3] void bslmt::Once::cancel(OnceLock *onceLock);
// [ 2] bool bslmt::Once::enter(OnceLock *onceLock);
// [ 2] void bslmt::Once::leave(OnceLock *onceLock);
// [ 4] bslmt::OnceGuard::bslmt::OnceGuard(bslmt::Once *once);
// [ 4] bslmt::OnceGuard::~bslmt::OnceGuard(bslmt::Once *once);
// [ 5] void bslmt::OnceGuard::cancel();
// [ 4] bool bslmt::OnceGuard::enter();
// [ 4] bool bslmt::OnceGuard::isInProgress() const;
// [ 6] void bslmt::OnceGuard::leave();
// [ 7] void bslmt::OnceGuard::setOnce(bslmt::Once *once);
// [ 9] BSLMT_ONCE_CANCEL
// [ 8] BSLMT_ONCE_DO
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::Types::Int64     Int64;

typedef bslmt::ThreadUtil       TU;
typedef bslmt::ThreadAttributes TA;
typedef bslmt::Barrier          BAR;

struct my_BreathingTestArg {
    // This structure defines a general set or arguments used to communicate
    // with test functions.
    BAR           *d_barrier_p;

    my_BreathingTestArg(BAR *barrier)
    : d_barrier_p(barrier)
    {
    }

    ~my_BreathingTestArg()
    {
    }
};

static int  onceValue = 0;
static bslmt::Once onceControl =  BSLMT_ONCE_INITIALIZER;

void breathingOnceFunc()
{
    ++onceValue;
}

extern "C"
void *breathingThreadFunc(void *ptr)
    // Thread function for breathing test
{
    my_BreathingTestArg *arg = (my_BreathingTestArg*)ptr;
    ASSERT(arg != 0);
    ASSERT(arg->d_barrier_p != 0);

    ASSERT(onceValue == 0);

    arg->d_barrier_p->wait();

    for (int i=0; i < 100; ++i) {
        onceControl.callOnce(breathingOnceFunc);
        ASSERT(onceValue == 1);
    }

    return ptr;
}

struct my_TestRecord {
    BAR   *d_barrier_p;

    int            d_id;          // Record ID
    bslmt::Once    *d_once_p;      // Shared once object
    volatile int  *d_winner_p;    // Pointer to winner
    int            d_sleepTime;   // Time to sleep in once-code
    bool           d_threadRan;   // Gets set to true when this thread finishes
    bool           d_isWinner;    // Set true if this thread wins the race
    int            d_foundWinner; // Winner at time function completes
    Int64          d_endTime;     // Time at which test function returns
};

extern "C"
void *enterLeaveTest(void *ptr)
    // Thread function for testing 'bslmt::Once::enter' and
    // 'bslmt::Once::leave'
{
    my_TestRecord *testRec = static_cast<my_TestRecord*>(ptr);
    ASSERT(testRec != 0);
    ASSERT(testRec->d_barrier_p != 0);
    ASSERT(testRec->d_winner_p != 0);

    ASSERT(! testRec->d_isWinner);

    testRec->d_barrier_p->wait();

    bslmt::Once::OnceLock context;
    if (testRec->d_once_p->enter(&context)) {
        ASSERT(! testRec->d_isWinner);

        testRec->d_isWinner = true;
        *testRec->d_winner_p = testRec->d_id;

        if (testRec->d_sleepTime) {
            // Sleep 'testRec->d_sleepTime' milliseconds.
            bsls::TimeInterval sleepInterval(0);
            sleepInterval.addMilliseconds(testRec->d_sleepTime);
            TU::sleep(sleepInterval);
        }

        testRec->d_endTime = bsls::TimeUtil::getTimer();
        testRec->d_once_p->leave(&context);
    }
    else {
        testRec->d_endTime = bsls::TimeUtil::getTimer();
    }

    testRec->d_foundWinner = *testRec->d_winner_p;  // Record the winner
    testRec->d_threadRan = true;

    return ptr;
}

extern "C"
void *enterCancelTest(void *ptr)
    // Thread function for testing 'bslmt::Once::cancel'
{
    my_TestRecord *testRec = static_cast<my_TestRecord*>(ptr);
    ASSERT(testRec != 0);
    ASSERT(testRec->d_barrier_p != 0);
    ASSERT(testRec->d_winner_p != 0);

    ASSERT(! testRec->d_isWinner);

    testRec->d_barrier_p->wait();

    bslmt::Once::OnceLock context;
    if (testRec->d_once_p->enter(&context)) {
        ASSERT(! testRec->d_isWinner);

        if (testRec->d_sleepTime) {
            // Sleep 'testRec->d_sleepTime' milliseconds.
            bsls::TimeInterval sleepInterval(0);
            sleepInterval.addMilliseconds(testRec->d_sleepTime);
            TU::sleep(sleepInterval);
        }

        testRec->d_endTime = bsls::TimeUtil::getTimer();
        if (testRec->d_winner_p[0] == -1) {
            // First winner cancels
            testRec->d_winner_p[0] = testRec->d_id;
            testRec->d_foundWinner = testRec->d_winner_p[1];
            testRec->d_once_p->cancel(&context);
            testRec->d_endTime = bsls::TimeUtil::getTimer();
            testRec->d_threadRan = true;
            return ptr;                                               // RETURN
        }
        else {
            // Second winner completes the operation
            testRec->d_isWinner = true;
            testRec->d_winner_p[1] = testRec->d_id;
            testRec->d_once_p->leave(&context);
        }
    }
    else {
        testRec->d_endTime = bsls::TimeUtil::getTimer();
    }

    testRec->d_foundWinner = testRec->d_winner_p[1];  // Record the winner
    testRec->d_threadRan = true;

    return ptr;
}

extern "C"
void *guardTest(void *ptr)
    // Thread function for testing 'bslmt::OnceGuard'
{
    my_TestRecord *testRec = static_cast<my_TestRecord*>(ptr);
    ASSERT(testRec != 0);
    ASSERT(testRec->d_barrier_p != 0);
    ASSERT(testRec->d_winner_p != 0);

    ASSERT(! testRec->d_isWinner);

    testRec->d_barrier_p->wait();

    {
        bslmt::OnceGuard onceGuard(testRec->d_once_p);
        ASSERT(! onceGuard.isInProgress());
        if (onceGuard.enter()) {
            ASSERT(onceGuard.isInProgress());
            ASSERT(! testRec->d_isWinner);

            testRec->d_isWinner = true;
            *testRec->d_winner_p = testRec->d_id;

            if (testRec->d_sleepTime) {
                // Sleep 'testRec->d_sleepTime' milliseconds.
                bsls::TimeInterval sleepInterval(0);
                sleepInterval.addMilliseconds(testRec->d_sleepTime);
                TU::sleep(sleepInterval);
            }

            testRec->d_endTime = bsls::TimeUtil::getTimer();
        }
        else {
            ASSERT(! onceGuard.isInProgress());
            testRec->d_endTime = bsls::TimeUtil::getTimer();
        }
    }

    testRec->d_foundWinner = *testRec->d_winner_p;  // Record the winner
    testRec->d_threadRan = true;

    return ptr;
}

extern "C"
void *guardCancelTest(void *ptr)
    // Thread function for testing 'bslmt::OnceGuard::cancel'
{
    my_TestRecord *testRec = static_cast<my_TestRecord*>(ptr);
    ASSERT(testRec != 0);
    ASSERT(testRec->d_barrier_p != 0);
    ASSERT(testRec->d_winner_p != 0);

    ASSERT(! testRec->d_isWinner);

    testRec->d_barrier_p->wait();

    {
        bslmt::OnceGuard onceGuard(testRec->d_once_p);
        ASSERT(! onceGuard.isInProgress());
        if (onceGuard.enter()) {
            ASSERT(onceGuard.isInProgress());
            ASSERT(! testRec->d_isWinner);

            if (testRec->d_sleepTime) {
                // Sleep 'testRec->d_sleepTime' milliseconds.
                bsls::TimeInterval sleepInterval(0);
                sleepInterval.addMilliseconds(testRec->d_sleepTime);
                TU::sleep(sleepInterval);
            }

            testRec->d_endTime = bsls::TimeUtil::getTimer();
            if (testRec->d_winner_p[0] == -1) {
                // First winner cancels
                testRec->d_winner_p[0] = testRec->d_id;
                testRec->d_foundWinner = testRec->d_winner_p[1];
                onceGuard.cancel();
                ASSERT(! onceGuard.isInProgress());
                testRec->d_endTime = bsls::TimeUtil::getTimer();
                testRec->d_threadRan = true;
                return ptr;                                           // RETURN
            }
            else {
                // Second winner completes the operation
                testRec->d_isWinner = true;
                testRec->d_winner_p[1] = testRec->d_id;
            }
        }
        else {
            ASSERT(! onceGuard.isInProgress());
            testRec->d_endTime = bsls::TimeUtil::getTimer();
        }
    }

    testRec->d_foundWinner = testRec->d_winner_p[1];  // Record the winner
    testRec->d_threadRan = true;

    return ptr;
}

extern "C"
void *macroTest(void *ptr)
    // Thread function for testing the 'BSLMT_ONCE_DO' macro
{
    my_TestRecord *testRec = static_cast<my_TestRecord*>(ptr);
    ASSERT(testRec != 0);
    ASSERT(testRec->d_barrier_p != 0);
    ASSERT(testRec->d_winner_p != 0);

    ASSERT(! testRec->d_isWinner);

    testRec->d_barrier_p->wait();

    BSLMT_ONCE_DO {
        ASSERT(! testRec->d_isWinner);

        testRec->d_isWinner = true;
        *testRec->d_winner_p = testRec->d_id;

        if (testRec->d_sleepTime) {
            // Sleep 'testRec->d_sleepTime' milliseconds.
            bsls::TimeInterval sleepInterval(0);
            sleepInterval.addMilliseconds(testRec->d_sleepTime);
            TU::sleep(sleepInterval);
        }
    }

    testRec->d_endTime = bsls::TimeUtil::getTimer();

    testRec->d_foundWinner = *testRec->d_winner_p;  // Record the winner
    testRec->d_threadRan = true;

    return ptr;
}

extern "C"
void *macroCancelTest(void *ptr)
    // Thread function for testing the 'BSLMT_ONCE_CANCEL' macro
{
    my_TestRecord *testRec = static_cast<my_TestRecord*>(ptr);
    ASSERT(testRec != 0);
    ASSERT(testRec->d_barrier_p != 0);
    ASSERT(testRec->d_winner_p != 0);

    ASSERT(! testRec->d_isWinner);

    testRec->d_barrier_p->wait();

    BSLMT_ONCE_DO {
        ASSERT(! testRec->d_isWinner);

        if (testRec->d_sleepTime) {
            // Sleep 'testRec->d_sleepTime' milliseconds.
            bsls::TimeInterval sleepInterval(0);
            sleepInterval.addMilliseconds(testRec->d_sleepTime);
            TU::sleep(sleepInterval);
        }

        if (testRec->d_winner_p[0] == -1) {
            // First winner cancels
            testRec->d_winner_p[0] = testRec->d_id;
            testRec->d_foundWinner = testRec->d_winner_p[1];
            BSLMT_ONCE_CANCEL();
            testRec->d_endTime = bsls::TimeUtil::getTimer();
            testRec->d_threadRan = true;
            return ptr;                                               // RETURN
        }
        else {
            // Second winner completes the operation
            testRec->d_isWinner = true;
            testRec->d_winner_p[1] = testRec->d_id;
        }
    }

    testRec->d_foundWinner = testRec->d_winner_p[1];  // Record the winner
    testRec->d_endTime = bsls::TimeUtil::getTimer();

    testRec->d_threadRan = true;

    return ptr;
}

// ============================================================================
//                    CLASSES FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// Typically, the facilities in this component are used to implement a
// thread-safe singleton.  Below, we implement the a singleton four ways,
// illustrating the two ways to directly use 'bslmt::Once', the use of
// 'bslmt::OnceGuard', and the use of 'BSLMT_ONCE_DO'.  In each example, the
// singleton functions take a C-string ('const char*') argument and return a
// reference to a 'bsl::string' object constructed from the input string.  Only
// the first call to each singleton function affect the contents of the
// singleton string.  (The argument is ignored on subsequent calls.)
//
///First Implementation
/// - - - - - - - - - -
// Our first implementation uses the 'BSLMT_ONCE_DO' construct, the
// recommended way to use this component.  The function is a variation of the
// singleton pattern described by Scott Meyers, except that the 'BSLMT_ONCE_DO'
// macro is used to handle multiple entries to the function in a thread-safe
// manner:
//..
    const bsl::string& singleton0(const char *s)
    {
        static bsl::string *theSingletonPtr = 0;
        BSLMT_ONCE_DO {
            static bsl::string theSingleton(s,
                                            bslma::Default::globalAllocator());
            theSingletonPtr = &theSingleton;
        }
        return *theSingletonPtr;
    }
//..
// The 'BSLMT_ONCE_DO' mechanism suffices for most situations; however, if more
// flexibility is required, review the remaining examples in this series for
// more design choices.  The next example will use the lowest-level facilities
// of 'bslmt::Once'.  The two following examples use progressively higher-level
// facilities to produce simpler singleton implementations (though none as
// simple as the 'BSLMT_ONCE_DO' example above).
//
///Second Implementation
///- - - - - - - - - - -
// The next singleton function implementation directly uses the 'doOnce' method
// of 'bslmt::Once'.  We begin by declaring a simple function that does most of
// the work of the singleton, i.e., constructing the string and setting a
// (static) pointer to the string:
//..
    static bsl::string *theSingletonPtr = 0;

    void singletonImp(const char *s)
    {
        static bsl::string theSingleton(s);
        theSingletonPtr = &theSingleton;
    }
//..
// The above function is *not* thread-safe.  Firstly, many threads might
// attempt to simultaneously construct the 'theSingleton' object.  Secondly,
// once 'theSingletonPtr' is set by one thread, other threads still might not
// see the change (and try to initialize the singleton again).
//
// The 'singleton1' function, below, calls 'singletonImp' via the 'callOnce'
// method of 'bslmt::Once' to ensure that 'singletonImp' is called by only one
// thread and that the result is visible to all threads.  We start by creating
// and initializing a static object of type 'bslmt::Once':
//..
/* TBD -- bind
    #include <bdlf_bind.h>
*/
    const bsl::string& singleton1(const char *s)
    {
        static bslmt::Once once = BSLMT_ONCE_INITIALIZER;
//..
// Since the 'callOnce' method takes only a no-argument functor (or function),
// to call 'callOnce', we must bind our argument 's' to our function,
// 'singletonImp' using a binder method and then pass that functor to
// 'callOnce'.  The first thread (and only the first thread) entering this
// section of code we will set 'theSingleton'.
//..
        /* TBD -- bind
        once.callOnce(bdlf::BindUtil::bind(singletonImp, s));
        */
        return *theSingletonPtr;
    }
//..
// Once we return from 'callOnce', the appropriate memory barrier has been
// executed so that the change to 'theSingletonPtr' is visible to all threads.
// A thread calling 'callOnce' after the initialization has completed would
// immediately return from the call.  A thread calling 'callOnce' while
// initialization is still in progress would block until initialization
// completes and then return.
//
// *Implementation* *Note*: As an optimization, developers sometimes pre-check
// the value to be set, 'theSingletonPtr' in this case, to avoid (heavy) memory
// barrier operations; however, that practice is not recommended here.  First,
// the value of the string may be cached by a different CPU, even though the
// pointer has already been updated on the common memory bus.  Second, The
// implementation of the 'callOnce' method is fast enough that a pre-check
// would not provide any performance benefit.
//
// The one advantage of this implementation over the previous one is that an
// exception thrown from within 'singletonImp' will cause the 'bslmt::Once'
// object to be restored to its original state, so that the next entry into the
// singleton will retry the operation.
//
///Third Implementation
/// - - - - - - - - - -
// Our next implementation, 'singleton2', eliminates the need for the
// 'singletonImp' function and thereby does away with the use of the
// 'bdlf::BindUtil' method; however, it does require use of
// 'bslmt::Once::OnceLock', created on each thread's stack and passed to the
// methods of 'bslmt::Once'.  First, we declare a static 'bslmt::Once' object
// as before, and also declare a static pointer to 'bsl::string':
//..
    const bsl::string& singleton2(const char *s)
    {
        static bslmt::Once once = BSLMT_ONCE_INITIALIZER;
        static bsl::string *theSingletonPtr = 0;
//..
// Next, we define a local 'bslmt::Once::OnceLock' object and pass it to the
// 'enter' method:
//..
        bslmt::Once::OnceLock onceLock;
        if (once.enter(&onceLock)) {
//..
// If the 'enter' method returns true, we proceed with the initialization of
// the singleton, as before.
//..
            static bsl::string theSingleton(s);
            theSingletonPtr = &theSingleton;
//..
// When initialization is complete, the 'leave' method is called for the same
// context cookie previously used in the call to 'enter':
//..
            once.leave(&onceLock);
        }
//..
// When any thread reaches this point, initialization has been complete and
// initialized string is returned:
//..
        return *theSingletonPtr;
    }
//..
//
///Fourth Implementation
///- - - - - - - - - - -
// Our final implementation, 'singleton3', uses 'bslmt::OnceGuard' to simplify
// the previous implementation by using 'bslmt::OnceGuard' to hide (automate)
// the use of 'bslmt::Once::OnceLock'.  We begin as before, defining a static
// 'bslmt::Once' object and a static 'bsl::string' pointer:
//..
    const bsl::string& singleton3(const char *s)
    {
        static bslmt::Once  once            = BSLMT_ONCE_INITIALIZER;
        static bsl::string *theSingletonPtr = 0;
//..
// We then declare a local 'bslmt::OnceGuard' object and associate it with the
// 'bslmt::Once' object before entering the one-time initialization region:
//..
        bslmt::OnceGuard onceGuard(&once);
        if (onceGuard.enter()) {
            static bsl::string theSingleton(s);
            theSingletonPtr = &theSingleton;
        }
        return *theSingletonPtr;
    }
//..
// Note that it is unnecessary to call 'onceGuard.leave()' because that is
// called automatically before the function returns.  This machinery makes the
// code more robust in the presence of, e.g., return statements in the
// initialization code.
//
// If there is significant code after the end of the one-time initialization,
// the guard and the initialization code should be enclosed in an extra block
// so that the guard is destroyed as soon as validly possible and allow other
// threads waiting on the initialization to continue.  Alternatively, one can
// call 'onceGuard.leave()' explicitly at the end of the initialization.
//
///Using the Semaphore Implementations
///- - - - - - - - - - - - - - - - - -
// The following pair of functions, 'thread1func' and 'thread2func' which will
// be run by different threads:
//..
    void *thread1func(void *)
    {
        const bsl::string& s0 = singleton0("0 hello");
        const bsl::string& s1 = singleton1("1 hello");
        const bsl::string& s2 = singleton2("2 hello");
        const bsl::string& s3 = singleton3("3 hello");

        ASSERT('0' == s0[0]);
        ASSERT('1' == s1[0]);
        ASSERT('2' == s2[0]);
        ASSERT('3' == s3[0]);

        // ... lots more code goes here
        return 0;
    }

    void *thread2func(void *)
    {
        const bsl::string& s0 = singleton0("0 goodbye");
        const bsl::string& s1 = singleton1("1 goodbye");
        const bsl::string& s2 = singleton2("2 goodbye");
        const bsl::string& s3 = singleton3("3 goodbye");

        ASSERT('0' == s0[0]);
        ASSERT('1' == s1[0]);
        ASSERT('2' == s2[0]);
        ASSERT('3' == s3[0]);

        // ... lots more code goes here
        return 0;
    }
//..
// Both threads attempt to initialize the four singletons.  In our example,
// each thread passes a distinct argument to the singleton, allowing us to
// identify the thread that initializes the singleton.  (In practice, the
// arguments passed to a specific singleton are almost always fixed and most
// singletons don't take arguments at all.)
//
// Assuming that the first thread function wins all of the races to initialize
// the singletons, the first singleton is set to "0 hello", the second
// singleton to "1 hello", etc.
//..
    int usageExample1()
    {
        void startThread1();
        void startThread2();

        startThread1();
        startThread2();

        ASSERT(singleton0("0") == "0 hello");
        ASSERT(singleton1("1") == "1 hello");
        ASSERT(singleton2("2") == "2 hello");
        ASSERT(singleton3("3") == "3 hello");

        return 0;
    }
//..

void startThread1() { thread1func(0); }
void startThread2() { thread2func(0); }

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // Specify "0.xyz" as second argument (argv[2]) to set repeat count to
    // "xyz".

    int REPEAT_COUNT = 20;  // Repeat test 100 times
    if (verbose) {
        const char *pt = bsl::strchr(argv[2], '.');
        if (pt) REPEAT_COUNT = bsl::atoi(pt + 1);
        if (REPEAT_COUNT < 1) REPEAT_COUNT = 1;
        bsl::cout << "Repeat count == " << REPEAT_COUNT << bsl::endl;
    }

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns: That the usage examples documented in the header file
        //    compile and run as described.
        //
        // Plan: Copy the usage example verbatim, changing 'assert' to 'ASSERT'
        //
        // Testing:
        //      USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        /* TBD -- bind
        usageExample1();
        */

      } break;

      case 9: {
        // --------------------------------------------------------------------
        // TESTING BSLMT_ONCE_CANCEL
        //
        // Concerns:
        //   - Calling 'BSLMT_ONCE_CANCEL' within a 'BSLMT_ONCE_DO' construct
        //     will leave the one-time code region in a restartable state.
        //   - Exactly one thread that is waiting on the 'BSLMT_ONCE_DO' will
        //     be admitted into the one-time region.
        //   - When any thread completes the one-time code, all other threads
        //     will be unblocked and will not execute the one-time code.
        //
        // Plan:
        //   - Create an array of data records with one element per thread.
        //   - Create a function that, using 'BSLMT_ONCE_DO',
        //     sets an entry in a shared array of "winner" flags then, if this
        //     is the first thread to enter the one-time region, calls
        //     'BSLMT_ONCE_CANCEL', but if this is the second thread to enter,
        //     sets its own winner flag lets the guard go out of scope.
        //   - For a small number, 'n' (minimum 4), start 'n' threads, all of
        //     which will synchronize on a barrier and attempt to run the
        //     "once" function simultaneously.
        //   - Verify that *two* entries were filled in the array of winner
        //     flags.
        //   - Verify that the first entry in the winner array is in a
        //     canceled state.
        //   - Verify that the second entry in the winner array is in a
        //     completed state.
        //   - Verify that the remaining threads considered the second winner
        //     to be *the* winner.
        //   - Test cannot be repeated in the same execution because the the
        //     'BSLMT_ONCE_DO' macro uses a hidden static 'bslmt::Once' object
        //     that cannot be reset.
        //
        // Testing:
        //      BSLMT_ONCE_CANCEL
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BSLMT_ONCE_CANCEL"
                               << "\n========================="
                               << bsl::endl;

        static const int MAX_THREADS = 5;
        static const int SLEEP_TIME  = 0; // milliseconds

        int numThreads = MAX_THREADS;

        BAR barrier(numThreads);
        TA attr;
        TU::Handle threads[MAX_THREADS];
        my_TestRecord testRecords[MAX_THREADS];
        int winners[3] = { -1, -1, -1 };

        // Initialize the test records
        for (int i = 0; i < numThreads; ++i) {
            testRecords[i].d_barrier_p   = &barrier;
            testRecords[i].d_id          = i;
            testRecords[i].d_once_p      = 0;
            testRecords[i].d_winner_p    = winners;
            testRecords[i].d_sleepTime   = SLEEP_TIME;
            testRecords[i].d_threadRan   = false;
            testRecords[i].d_isWinner    = false;
            testRecords[i].d_foundWinner = -1;
            testRecords[i].d_endTime     = 0;
        }

        // Start all threads
        Int64 startTime;
        for (int i = 0; i < numThreads; ++i) {
            if (i == numThreads - 1) {
                startTime = bsls::TimeUtil::getTimer();
            }
            LOOP2_ASSERT(i, numThreads,
                         0 == TU::create(&threads[i], attr,
                                         &macroCancelTest,
                                         &testRecords[i]));
        }

        // Wait for all threads
        for (int i = 0; i < numThreads; ++i) {
            LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
        }

        // Verify that there were exactly two winners
        ASSERT(winners[0] != -1);
        ASSERT(winners[1] != -1);
        ASSERT(winners[2] == -1);

        // Verify that the first winner canceled and the 2nd did not
        ASSERT(! testRecords[winners[0]].d_isWinner);
        ASSERT(-1 == testRecords[winners[0]].d_foundWinner);
        ASSERT(testRecords[winners[1]].d_isWinner);
        ASSERT(winners[1] == testRecords[winners[1]].d_foundWinner);

        // Verify expected results of all threads.  At this point
        // 'winnerElapsed' refers to the 2nd winner
        for (int i = 0; i < numThreads; ++i) {

            LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

            if (winners[0] == i) continue; // Skip canceled winner

            if (testRecords[i].d_isWinner) {
                LOOP3_ASSERT(i, winners[1], numThreads, i == winners[1]);
            }

            LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                         testRecords[i].d_foundWinner == winners[1]);
        }

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // TESTING BSLMT_ONCE_DO
        //
        // Concerns:
        //   - 'BSLMT_ONCE_DO' can fully automate use of 'bslmt::Once'.
        //   - 'BSLMT_ONCE_DO' can be used twice in the same block.
        //
        // Plan:
        //   - Create an array of data records with one element per thread.
        //   - Create a function that uses 'BSLMT_ONCE_DO' to enter the
        //     one-time code, set a "winner" flag within a specified record
        //     and a shared "winner" flag, then destroys the guard by exiting
        //     the scope.
        //   - In the thread function, verify that 'isInProgress' reflects the
        //     current state of the 'bslmt::OnceGuard' object.
        //   - Start a few threads, all of which will
        //     synchronize on a barrier and attempt to run the "once" function
        //     simultaneously.
        //   - Verify that, after the once-time code, the "done" flag is set to
        //     true in all threads (i.e., no thread proceeded until the winner
        //     set the "done" flag).
        //   - Verify that only one thread set its "winner" flag.
        //   - Rerun the threads and verify that none of the threads are
        //     winners the second time.  (Test cannot be reset and repeated in
        //     the same execution because the the 'BSLMT_ONCE_DO' macro uses a
        //     hidden static 'bslmt::Once' object that cannot be reset.)
        //   - Create two, non-nested BSLMT_ONCE_DO constructs within a
        //     for-loop block, each incrementing a different counter.  Verify
        //     that compilation is successful (i.e., they do not conflict with
        //     each other) and that they each counter is incremented exactly
        //     once.
        //   - Repeat the previous test but with nested BSLMT_ONCE_DO
        //     constructs.
        //
        // Testing:
        //      BSLMT_ONCE_DO
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BSLMT_ONCE_DO"
                               << "\n=====================" << bsl::endl;

        static const int MAX_THREADS = 5;
        static const int SLEEP_TIME  = 0; // milliseconds

        int numThreads = MAX_THREADS;

        BAR barrier(numThreads);
        TA attr;
        TU::Handle threads[MAX_THREADS];
        my_TestRecord testRecords[MAX_THREADS];
        int winner = -1;

        // Initialize the test records
        for (int i = 0; i < numThreads; ++i) {
            testRecords[i].d_barrier_p   = &barrier;
            testRecords[i].d_id          = i;
            testRecords[i].d_once_p      = 0;
            testRecords[i].d_winner_p    = &winner;
            testRecords[i].d_sleepTime   = SLEEP_TIME;
            testRecords[i].d_threadRan   = false;
            testRecords[i].d_isWinner    = false;
            testRecords[i].d_foundWinner = -1;
            testRecords[i].d_endTime     = 0;
        }

        // Start all threads
        Int64 startTime;
        for (int i = 0; i < numThreads; ++i) {
            if (i == numThreads - 1) {
                startTime = bsls::TimeUtil::getTimer();
            }
            LOOP2_ASSERT(i, numThreads,
                         0 == TU::create(&threads[i], attr,
                                         &macroTest,
                                         &testRecords[i]));
        }

        // Wait for all threads
        for (int i = 0; i < numThreads; ++i) {
            LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
        }

        // Verify that there was a winner
        LOOP2_ASSERT(numThreads, winner, winner != -1);
        LOOP2_ASSERT(numThreads, winner, testRecords[winner].d_isWinner);

        // Verify expected results of all threads.
        for (int i = 0; i < numThreads; ++i) {

            LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

            if (testRecords[i].d_isWinner) {
                LOOP3_ASSERT(i, winner, numThreads, i == winner);
            }

            LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                         testRecords[i].d_foundWinner == winner);
        }

        // Re-initialize the test records
        winner = -1;
        for (int i = 0; i < numThreads; ++i) {
            testRecords[i].d_threadRan   = false;
            testRecords[i].d_isWinner    = false;
            testRecords[i].d_foundWinner = -1;
            testRecords[i].d_endTime     = 0;
        }

        // Start all threads again
        for (int i = 0; i < numThreads; ++i) {
            if (i == numThreads - 1) {
                startTime = bsls::TimeUtil::getTimer();
            }
            LOOP2_ASSERT(i, numThreads,
                         0 == TU::create(&threads[i], attr,
                                         &macroTest,
                                         &testRecords[i]));
        }

        // Wait for all threads
        for (int i = 0; i < numThreads; ++i) {
            LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
        }

        // Verify no winner and small elapsed times.
        LOOP2_ASSERT(numThreads, winner, -1 == winner);
        for (int i = 0; i < numThreads; ++i) {
            LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);
            ASSERT(! testRecords[i].d_isWinner);
            ASSERT(-1 == testRecords[i].d_foundWinner);
        }

        // Check that two constructs can be used in the same block:
        {
            static int counter1 = 0, counter2 = 0;
            for (int i = 0; i < 5; ++i) {
                BSLMT_ONCE_DO {
                    ASSERT(0 == counter1);
                    ASSERT(0 == counter2);
                    ++counter1;
                }
                ASSERT(1 == counter1);

                BSLMT_ONCE_DO {
                    ASSERT(1 == counter1);
                    ASSERT(0 == counter2);
                    ++counter2;
                }
                ASSERT(1 == counter1);
                ASSERT(1 == counter2);
            }
            ASSERT(1 == counter1);
            ASSERT(1 == counter2);
        }

        // Check that two constructs can be nested
        {
            static int counter1 = 0, counter2 = 0;
            for (int i = 0; i < 5; ++i) {
                BSLMT_ONCE_DO {
                    ASSERT(0 == counter1);
                    ASSERT(0 == counter2);
                    ++counter1;
                    BSLMT_ONCE_DO {
                        ASSERT(1 == counter1);
                        ASSERT(0 == counter2);
                        ++counter2;
                    }
                }

                ASSERT(1 == counter1);
                ASSERT(1 == counter2);
            }
            ASSERT(1 == counter1);
            ASSERT(1 == counter2);
        }

      } break;

      case 7: {
        // --------------------------------------------------------------------
        // TESTING BSLMT_ONCEGUARD::SETONCE
        //
        // Concerns:
        //    - After calling 'leave' on a 'bslmt::OnceGuard', it can be
        //      associated with a new 'bslmt::Once' object and re-used.
        //
        //
        // Plan:
        //    - Because a 'bslmt::OnceGuard' object is not itself shared
        //      among threads and because the underlying thread-safe access to
        //      the 'bslmt::Once' object has already been verified by other
        //      test cases, there is no need even to spawn threads in this
        //      test case.
        //    - Create a 'bslmt::Once' object.
        //    - Construct a 'bslmt::OnceGuard' using the 'bslmt::Once' object.
        //    - Call 'enter' on the guard and verify that it returns true.
        //    - Verify that the guard 'isInProgress' returns true.
        //    - Call 'leave' on the guard.
        //    - Verify that the guard 'isInProgress' now returns false.
        //    - Create a second 'bslmt::Once' object.
        //    - Call 'setOnce' on the guard to associated it with the second
        //      'bslmt::Once' object.
        //    - Call 'enter' on the guard and verify that it returns true.
        //    - Verify that the guard 'isInProgress' returns true.
        //    - Call 'leave' on the guard.
        //    - Verify that the guard 'isInProgress' now returns false.
        //    - Create a third 'bslmt::Once' object.
        //    - Call 'enter' and 'leave' on the third 'bslmt::Once' object to
        //      put it into a "done" state.
        //    - Call 'setOnce' on the guard to associate it with the third
        //      'bslmt::Once' object.
        //    - Verify that 'enter' still returns false (because the
        //      'bslmt::Once' object has already been used).
        //
        // Testing:
        //      void bslmt::OnceGuard::setOnce(bslmt::Once *once);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BSLMT_ONCEGUARD::SETONCE"
                               << "\n================================"
                               << bsl::endl;

        bslmt::Once once1 = BSLMT_ONCE_INITIALIZER;
        bslmt::OnceGuard guard(&once1);
        ASSERT(! guard.isInProgress());
        ASSERT(  guard.enter());
        ASSERT(  guard.isInProgress());
        guard.leave();
        ASSERT(! guard.isInProgress());

        bslmt::Once once2 = BSLMT_ONCE_INITIALIZER;
        guard.setOnce(&once2);
        ASSERT(! guard.isInProgress());
        ASSERT(  guard.enter());
        ASSERT(  guard.isInProgress());
        guard.leave();
        ASSERT(! guard.isInProgress());

        bslmt::Once once3 = BSLMT_ONCE_INITIALIZER;
        bslmt::Once::OnceLock onceLock;
        ASSERT(once3.enter(&onceLock));
        once3.leave(&onceLock);
        guard.setOnce(&once3);
        ASSERT(! guard.isInProgress());
        ASSERT(! guard.enter());

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING BSLMT_ONCEGUARD::LEAVE
        //
        // Concerns:
        //    - Calling 'leave' on a 'bslmt::OnceGuard' will put the
        //      associated 'bslmt::Once' object into a "done" state.
        //    - The 'isInProgress' method returns 'false' after 'leave' is
        //      called.
        //
        // Plan:
        //    - Because a 'bslmt::OnceGuard' object is not itself shared
        //      among threads and because the underlying thread-safe access to
        //      the 'bslmt::Once' object has already been verified by other
        //      test cases, there is no need even to spawn threads in this
        //      test case.
        //    - Create a 'bslmt::Once' object.
        //    - Construct a 'bslmt::OnceGuard' using the 'bslmt::Once' object.
        //    - Call 'enter' on the guard and verify that it returns true.
        //    - Verify that the guard 'isInProgress' returns true.
        //    - Call 'leave' on the guard.
        //    - Verify that the guard 'isInProgress' now returns false.
        //    - Again, call 'enter' on the guard and verify that it now
        //      returns false.
        //    - Verify that the guard 'isInProgress' returns false.
        //    - Call 'leave' on the original 'bslmt::Once' object and verify
        //      that it returns false.
        //
        // Testing:
        //      void bslmt::OnceGuard::leave();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BSLMT_ONCEGUARD::LEAVE"
                               << "\n=============================="
                               << bsl::endl;

        bslmt::Once once = BSLMT_ONCE_INITIALIZER;
        bslmt::OnceGuard guard(&once);
        ASSERT(! guard.isInProgress());
        ASSERT(  guard.enter());
        ASSERT(  guard.isInProgress());
        guard.leave();
        ASSERT(! guard.isInProgress());
        ASSERT(! guard.enter());
        bslmt::Once::OnceLock onceLock;
        ASSERT(! once.enter(&onceLock));

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // TESTING BSLMT_ONCEGUARD::CANCEL
        //
        // Concerns:
        //   - Calling 'cancel' on a 'bslmt::OnceGuard' object will leave the
        //     one-time code region in a restartable state.
        //   - Exactly one thread that is waiting on the 'bslmt::Once' will
        //     be admitted into the one-time region.
        //   - When any thread completes the one-time code, all other threads
        //     will be unblocked and will not execute the one-time code.
        //
        // Plan:
        //   - Create an array of data records with one element per thread.
        //   - Create a function that, using a 'bslmt::OnceGuard' on a shared
        //     'bslmt::Once' object, sets an entry in a shared array of
        //     "winner" flags then, if this is the first thread to enter the
        //     one-time region, calls 'cancel' on the guard but if this is the
        //     second thread to enter, sets its own winner flag lets the guard
        //     go out of scope.
        //   - For a small number, 'n' (minimum 4), start 'n' threads, all of
        //     which will synchronize on a barrier and attempt to run the
        //     "once" function simultaneously.
        //   - Verify that *two* entries were filled in the array of winner
        //     flags.
        //   - Verify that the first entry in the winner array is in a
        //     canceled state.
        //   - Verify that the second entry in the winner array is in a
        //     completed state.
        //   - Verify that the remaining threads considered the second winner
        //     to be *the* winner.
        //   - Repeat this test several times, to uncover potential
        //     timing-related problems.
        //
        // Testing:
        //      void bslmt::OnceGuard::cancel();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BSLMT_ONCEGUARD::CANCEL"
                               << "\n==============================="
                               << bsl::endl;

        static const int MAX_THREADS = 5;
        static const int SLEEP_TIME  = 0; // milliseconds

        int numThreads = MAX_THREADS;

        for (int repetition = 0; repetition < REPEAT_COUNT; ++repetition) {

            BAR barrier(numThreads);
            TA attr;
            TU::Handle threads[MAX_THREADS];
            my_TestRecord testRecords[MAX_THREADS];
            bslmt::Once once = BSLMT_ONCE_INITIALIZER;
            int winners[3] = { -1, -1, -1 };

            // Initialize the test records
            for (int i = 0; i < numThreads; ++i) {
                testRecords[i].d_barrier_p   = &barrier;
                testRecords[i].d_id          = i;
                testRecords[i].d_once_p      = &once;
                testRecords[i].d_winner_p    = winners;
                testRecords[i].d_sleepTime   = SLEEP_TIME;
                testRecords[i].d_threadRan   = false;
                testRecords[i].d_isWinner    = false;
                testRecords[i].d_foundWinner = -1;
                testRecords[i].d_endTime     = 0;
            }

            // Start all threads
            Int64 startTime;
            for (int i = 0; i < numThreads; ++i) {
                if (i == numThreads - 1) {
                    startTime = bsls::TimeUtil::getTimer();
                }
                LOOP2_ASSERT(i, numThreads,
                             0 == TU::create(&threads[i], attr,
                                             &guardCancelTest,
                                             &testRecords[i]));
            }

            // Wait for all threads
            for (int i = 0; i < numThreads; ++i) {
                LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
            }

            // Verify that there were exactly two winners
            LOOP_ASSERT(repetition, winners[0] != -1);
            LOOP_ASSERT(repetition, winners[1] != -1);
            LOOP_ASSERT(repetition, winners[2] == -1);

            // Verify that the first winner canceled and the 2nd did not
            LOOP_ASSERT(repetition, ! testRecords[winners[0]].d_isWinner);
            LOOP_ASSERT(repetition,
                        -1 == testRecords[winners[0]].d_foundWinner);
            LOOP_ASSERT(repetition,   testRecords[winners[1]].d_isWinner);
            LOOP_ASSERT(repetition,
                        winners[1] == testRecords[winners[1]].d_foundWinner);

            // Verify expected results of all threads.  At this point
            // 'winnerElapsed' refers to the 2nd winner
            for (int i = 0; i < numThreads; ++i) {

                LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

                if (winners[0] == i) continue; // Skip canceled winner

                if (testRecords[i].d_isWinner) {
                    LOOP3_ASSERT(i, winners[1], numThreads, i == winners[1]);
                }

                LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                             testRecords[i].d_foundWinner == winners[1]);
            }

        } // end for (repetition)

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING BSLMT_ONCEGUARD
        //
        // Concerns: That the 'bslmt::OnceGuard' can automate the 'leave'
        //   some of the functionality of 'bslmt::Once'.  In particular,
        //   'leave' is called automatically on destruction.
        //
        // Plan:
        //   - Create an array of data records with one element per thread.
        //   - Create a function that uses a 'bslmt::Once' guard to enter the
        //     one-time code, set a "winner" flag within a specified record
        //     and a shared "winner" flag, then destroys the guard by exiting
        //     the scope.
        //   - In the thread function, verify that 'isInProgress' reflects the
        //     current state of the 'bslmt::OnceGuard' object.
        //   - Start a few threads, all of which will
        //     synchronize on a barrier and attempt to run the "once" function
        //     simultaneously.
        //   - Verify that, after the once-time code, the "done" flag is set to
        //     true in all threads (i.e., no thread proceeded until the winner
        //     set the "done" flag).
        //   - Verify that only one thread set its "winner" flag.
        //   - Repeat the test without resetting the bslmt::Once structure.
        //   - Verify that none of the threads are winners the second time.
        //   - Reset the 'bslmt::Once' object and repeat this test several
        //     times to maximize the chance of exposing unhandled race
        //     conditions.
        //   - Repeat the entire test with two separate 'bslmt::Once' objects
        //     such that half the threads access one object and the other half
        //     access the other object.
        //
        // Testing:
        //      bslmt::OnceGuard::bslmt::OnceGuard(bslmt::Once *once);
        //      bslmt::OnceGuard::~bslmt::OnceGuard(bslmt::Once *once);
        //      bool bslmt::OnceGuard::enter();
        //      bool bslmt::OnceGuard::isInProgress() const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING BSLMT_ONCEGUARD"
                               << "\n=======================" << bsl::endl;

        static const int MAX_THREADS = 5;
        static const int SLEEP_TIME  = 0; // milliseconds

        if (verbose) bsl::cout << "Testing with one 'bslmt::Once' object"
                               << bsl::endl;

        int numThreads = MAX_THREADS;

        for (int repetition = 0; repetition < REPEAT_COUNT; ++repetition) {

            BAR barrier(numThreads);
            TA attr;
            TU::Handle threads[MAX_THREADS];
            my_TestRecord testRecords[MAX_THREADS];
            bslmt::Once once = BSLMT_ONCE_INITIALIZER;
            int winner = -1;

            // Initialize the test records
            for (int i = 0; i < numThreads; ++i) {
                testRecords[i].d_barrier_p   = &barrier;
                testRecords[i].d_id          = i;
                testRecords[i].d_once_p      = &once;
                testRecords[i].d_winner_p    = &winner;
                testRecords[i].d_sleepTime   = SLEEP_TIME;
                testRecords[i].d_threadRan   = false;
                testRecords[i].d_isWinner    = false;
                testRecords[i].d_foundWinner = -1;
                testRecords[i].d_endTime     = 0;
            }

            // Start all threads
            Int64 startTime;
            for (int i = 0; i < numThreads; ++i) {
                if (i == numThreads - 1) {
                    startTime = bsls::TimeUtil::getTimer();
                }
                LOOP2_ASSERT(i, numThreads,
                             0 == TU::create(&threads[i], attr,
                                             &guardTest,
                                             &testRecords[i]));
            }

            // Wait for all threads
            for (int i = 0; i < numThreads; ++i) {
                LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
            }

            // Verify that there was a winner
            LOOP3_ASSERT(repetition, numThreads, winner, winner != -1);
            LOOP3_ASSERT(repetition, numThreads, winner,
                         testRecords[winner].d_isWinner);

            // Verify expected results of all threads.
            for (int i = 0; i < numThreads; ++i) {

                LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

                if (testRecords[i].d_isWinner) {
                    LOOP3_ASSERT(i, winner, numThreads, i == winner);
                }

                LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                             testRecords[i].d_foundWinner == winner);
            }

            // Re-initialize the test records
            winner = -1;
            for (int i = 0; i < numThreads; ++i) {
                testRecords[i].d_threadRan   = false;
                testRecords[i].d_isWinner    = false;
                testRecords[i].d_foundWinner = -1;
                testRecords[i].d_endTime     = 0;
            }

            // Start all threads again
            for (int i = 0; i < numThreads; ++i) {
                if (i == numThreads - 1) {
                    startTime = bsls::TimeUtil::getTimer();
                }
                LOOP2_ASSERT(i, numThreads,
                             0 == TU::create(&threads[i], attr,
                                             &guardTest,
                                             &testRecords[i]));
            }

            // Wait for all threads
            for (int i = 0; i < numThreads; ++i) {
                LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
            }

            // Verify no winner and small elapsed times.
            LOOP3_ASSERT(numThreads, repetition, winner, -1 == winner);
            for (int i = 0; i < numThreads; ++i) {
                LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);
                ASSERT(! testRecords[i].d_isWinner);
                ASSERT(-1 == testRecords[i].d_foundWinner);
            }

        } // end for (repetition)

        if (verbose) bsl::cout << "Testing with two 'bslmt::Once' objects"
                               << bsl::endl;

        for (int repetition = 0; repetition < REPEAT_COUNT; ++repetition) {

            BAR barrier(numThreads);
            TA attr;
            TU::Handle threads[MAX_THREADS];
            my_TestRecord testRecords[MAX_THREADS];
            int winner1 = -1;
            int winner2 = -1;
            bslmt::Once once1 = BSLMT_ONCE_INITIALIZER;
            bslmt::Once once2 = BSLMT_ONCE_INITIALIZER;

            // Initialize the test records
            for (int i = 0; i < numThreads; ++i) {
                // 'x' is true for half the threads
                bool x = i < numThreads / 2;
                testRecords[i].d_barrier_p   = &barrier;
                testRecords[i].d_id          = i;
                testRecords[i].d_once_p      = x ? &once1 : &once2;
                testRecords[i].d_winner_p    = x ? &winner1 : &winner2;
                testRecords[i].d_sleepTime   = SLEEP_TIME;
                testRecords[i].d_threadRan   = false;
                testRecords[i].d_isWinner    = false;
                testRecords[i].d_foundWinner = -1;
                testRecords[i].d_endTime     = 0;
            }

            // Start all threads
            Int64 startTime;
            for (int i = 0; i < numThreads; ++i) {
                if (i == numThreads - 1) {
                    startTime = bsls::TimeUtil::getTimer();
                }
                LOOP2_ASSERT(i, numThreads,
                             0 == TU::create(&threads[i], attr,
                                             guardTest,
                                             &testRecords[i]));
            }

            // Wait for all threads
            for (int i = 0; i < numThreads; ++i) {
                LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
            }

            // Verify that there was a winner for 1st half of threads
            LOOP3_ASSERT(repetition, numThreads, winner1, winner1 != -1);
            LOOP3_ASSERT(repetition, numThreads, winner1,
                         winner1 < numThreads / 2);
            LOOP3_ASSERT(repetition, numThreads, winner1,
                         testRecords[winner1].d_isWinner);

            // Verify expected results of all threads.
            for (int i = 0; i < numThreads / 2; ++i) {

                LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

                if (testRecords[i].d_isWinner) {
                    LOOP3_ASSERT(i, winner1, numThreads, i == winner1);
                }

                LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                             testRecords[i].d_foundWinner == winner1);
            }

            // Verify that there was a winner for 2nd half of threads
            LOOP3_ASSERT(repetition, numThreads, winner2, winner2 != -1);
            LOOP3_ASSERT(repetition, numThreads, winner2,
                         winner2 >= numThreads / 2);
            LOOP3_ASSERT(repetition, numThreads, winner2,
                         testRecords[winner2].d_isWinner);

            // Verify expected results of all threads.
            for (int i = numThreads / 2 + 1; i < numThreads; ++i) {

                LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

                if (testRecords[i].d_isWinner) {
                    LOOP3_ASSERT(i, winner2, numThreads, i == winner2);
                }

                LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                             testRecords[i].d_foundWinner == winner2);
            }

            // Re-initialize the test records
            winner1 = -1;
            winner2 = -1;
            for (int i = 0; i < numThreads; ++i) {
                testRecords[i].d_threadRan   = false;
                testRecords[i].d_isWinner    = false;
                testRecords[i].d_foundWinner = -1;
                testRecords[i].d_endTime     = 0;
            }

            // Start all threads again
            for (int i = 0; i < numThreads; ++i) {
                if (i == numThreads - 1) {
                    startTime = bsls::TimeUtil::getTimer();
                }
                LOOP2_ASSERT(i, numThreads,
                             0 == TU::create(&threads[i], attr,
                                             &guardTest,
                                             &testRecords[i]));
            }

            // Wait for all threads
            for (int i = 0; i < numThreads; ++i) {
                LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
            }

            // Verify no winner and small elapsed times.
            LOOP2_ASSERT(repetition, winner1, -1 == winner1);
            LOOP2_ASSERT(repetition, winner2, -1 == winner2);
            for (int i = 0; i < numThreads; ++i) {
                LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);
                ASSERT(! testRecords[i].d_isWinner);
                ASSERT(-1 == testRecords[i].d_foundWinner);
            }

        } // end for (repetition)

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING CANCEL
        //
        // Concerns:
        //   - Calling 'cancel' on a 'bslmt::Once' object will leave the
        //     one-time code region in a restartable state.
        //   - Exactly one thread that is waiting on a 'bslmt::Once' will
        //     be admitted into the one-time region.
        //   - When any thread completes the one-time code, all other threads
        //     will be unblocked and will not execute the one-time code.
        //
        // Plan:
        //   - Create an array of data records with one element per thread.
        //   - Create a function that, using 'enter' on a shared 'bslmt::Once'
        //     object, sets an entry in a shared array of "winner" flags,
        //     sleeps for 200 milliseconds, then, if this is the first thread
        //     to enter the one-time region, calls 'cancel' but if this is the
        //     second thread to enter, sets its own winner flag and calls
        //     'leave'.
        //   - For a small number, 'n' (minimum 4), start 'n' threads, all of
        //     which will synchronize on a barrier and attempt to run the
        //     "once" function simultaneously.
        //   - Verify that *two* entries were filled in the array of winner
        //     flags.
        //   - Verify that the first entry in the winner array is in a
        //     canceled state.
        //   - Verify that the second entry in the winner array is in a
        //     completed state.
        //   - Verify that the remaining threads considered the second winner
        //     to be *the* winner.
        //   - Verify that the elapsed time for the first winner is just over
        //     the 200ms sleep time.
        //   - Verify that the elapsed time for the second winner is just over
        //     twice the 200ms sleep time.
        //   - Verify that the elapsed time each of the remaining threads is
        //     just over the elapsed time of the second winner.
        //   - Repeat this test several times, to uncover potential
        //     timing-related problems.
        //
        // Testing:
        //      void bslmt::Once::cancel(OnceLock *onceLock);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING CANCEL"
                               << "\n==============" << bsl::endl;

        static const int MAX_THREADS = 5;
        static const int SLEEP_TIME  = 200; // milliseconds

        // Minimum and maximum elapsed times for a thread that sleeps are 15ms
        // less than nominal sleep time to 75ms more than nominal sleep time.
        static const int MIN_ELAPSED = SLEEP_TIME - 15;
        static const int MAX_ELAPSED = SLEEP_TIME + 75;

        // Losers may each finish up to 50ms behind winner
        static const int MAX_NOOP_DELAY = 50;

        static const int THREAD_COUNT1[] = { 1, 2, 3, 4, 8, 12, 15 };
        static const int NUM_THREAD_COUNTS1 =
            sizeof THREAD_COUNT1 / sizeof THREAD_COUNT1[0];

        int numThreads = MAX_THREADS;

        for (int repetition = 0; repetition < REPEAT_COUNT; ++repetition) {

            BAR barrier(numThreads);
            TA attr;
            TU::Handle threads[MAX_THREADS];
            my_TestRecord testRecords[MAX_THREADS];
            bslmt::Once once = BSLMT_ONCE_INITIALIZER;
            int winners[3] = { -1, -1, -1 };

            // Initialize the test records
            for (int i = 0; i < numThreads; ++i) {
                testRecords[i].d_barrier_p   = &barrier;
                testRecords[i].d_id          = i;
                testRecords[i].d_once_p      = &once;
                testRecords[i].d_winner_p    = winners;
                testRecords[i].d_sleepTime   = SLEEP_TIME;
                testRecords[i].d_threadRan   = false;
                testRecords[i].d_isWinner    = false;
                testRecords[i].d_foundWinner = -1;
                testRecords[i].d_endTime     = 0;
            }

            // Start all threads
            Int64 startTime;
            for (int i = 0; i < numThreads; ++i) {
                if (i == numThreads - 1) {
                    startTime = bsls::TimeUtil::getTimer();
                }
                LOOP2_ASSERT(i, numThreads,
                             0 == TU::create(&threads[i], attr,
                                             &enterCancelTest,
                                             &testRecords[i]));
            }

            // Wait for all threads
            for (int i = 0; i < numThreads; ++i) {
                LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
            }

            // Verify that there were exactly two winners
            LOOP_ASSERT(repetition, winners[0] != -1);
            LOOP_ASSERT(repetition, winners[1] != -1);
            LOOP_ASSERT(repetition, winners[2] == -1);

            // Verify that the first winner canceled and the 2nd did not
            LOOP_ASSERT(repetition, ! testRecords[winners[0]].d_isWinner);
            LOOP_ASSERT(repetition,
                        -1 == testRecords[winners[0]].d_foundWinner);
            LOOP_ASSERT(repetition,   testRecords[winners[1]].d_isWinner);
            LOOP_ASSERT(repetition,
                        winners[1] == testRecords[winners[1]].d_foundWinner);

            // Get winner's elapsed times in milliseconds and verify that it is
            // within the expected range.
            int winnerElapsed = (testRecords[winners[0]].d_endTime -
                                 startTime) / 1000000;
            if (verbose) {
                LOOP2_ASSERT(winners[0], winnerElapsed,
                             MIN_ELAPSED <= winnerElapsed &&
                             MAX_ELAPSED >= winnerElapsed);
            }

            winnerElapsed = (testRecords[winners[1]].d_endTime -
                             startTime) / 1000000;
            if (verbose) {
                LOOP2_ASSERT(winners[1], winnerElapsed,
                             2*MIN_ELAPSED <= winnerElapsed &&
                             2*MAX_ELAPSED >= winnerElapsed);
            }

            // Verify expected results of all threads.  At this point
            // 'winnerElapsed' refers to the 2nd winner
            for (int i = 0; i < numThreads; ++i) {

                LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

                if (winners[0] == i) continue; // Skip canceled winner

                if (testRecords[i].d_isWinner) {
                    LOOP3_ASSERT(i, winners[1], numThreads, i == winners[1]);
                }

                LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                             testRecords[i].d_foundWinner == winners[1]);

                int elapsedTime = (testRecords[i].d_endTime -
                                   startTime) / 1000000;
                if (verbose) {
                    // Verify that measured elapsed time is no less than winner
                    // elapsed time and no more than specified delay above
                    // winner elapsed time.
                    LOOP4_ASSERT(i, numThreads, elapsedTime, winnerElapsed,
                                 elapsedTime >= winnerElapsed);
                    LOOP4_ASSERT(i, numThreads, elapsedTime, winnerElapsed,
                                 elapsedTime <= winnerElapsed+MAX_NOOP_DELAY);
                }
            }

        } // end for (repetition)

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING ENTER AND LEAVE
        //
        // Concerns: Given 'n' threads attempting to simultaneously execute
        //   the same one-time execution code, which is protected by a
        //   'bslmt::Once' object using the 'enter' and 'leave' methods, the
        //   following occurs:
        //   - Only one thread (the "winner") actually executes the code.
        //   - The remaining threads (the "losers") wait for the winner to
        //     finish before proceeding.
        //   - The losers all proceed more-or-less at once, waiting
        //     for the winner, but not for the other losers.
        //   - Subsequent attempts to enter the same one-time code do nothing
        //     and return immediately.
        //   - The above concerns are met for as few as 1 and as many as 15
        //     threads.  (It is assumed that more than 15 threads will not
        //     change the behavior.)
        //   - If there are two ore more 'bslmt::Once' objects, then they are
        //     independent of one-another and access to one does not block or
        //     significantly slow down access to the other.
        //
        // Plan:
        //   - Create an array of data records with one element per thread.
        //   - Create a function that, using 'enter' on a shared 'bslmt::Once'
        //     object, sets a "winner" flag within a specified record and a
        //     shared "winner" flag, sleeps for 200 milliseconds, then calls
        //     'leave'.
        //   - For 'n' = 1 to 15, start 'n' threads, all of which will
        //     synchronize on a barrier and attempt to run the "once" function
        //     simultaneously.
        //   - Verify that, after the once-time code, the "done" flag is set to
        //     true in all threads (i.e., no thread proceeded until the winner
        //     set the "done" flag).
        //   - Verify that only one thread set its "winner" flag.
        //   - Verify that no thread takes much more than 200
        //     milliseconds to complete its operation, including the time
        //     waiting for the winner to finish.  This is to show that the
        //     losers only wait for the winner, not for each other.
        //   - Repeat the test without resetting the bslmt::Once structure.
        //   - Verify that none of the threads are winners the second time.
        //   - Verify that it takes almost no time for each thread to complete.
        //   - Reset the 'bslmt::Once' object and repeat this test many times
        //     to maximize the chance of exposing unhandled race conditions.
        //   - Repeat the entire test with two separate 'bslmt::Once' objects
        //     such that half the threads access one object and the other half
        //     access the other object.
        //
        // Testing:
        //      bool bslmt::Once::enter(OnceLock *onceLock);
        //      void bslmt::Once::leave(OnceLock *onceLock);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING ENTER AND LEAVE"
                               << "\n=======================" << bsl::endl;

        static const int MAX_THREADS = 25;
        static const int SLEEP_TIME  = 200; // milliseconds

        // Minimum and maximum elapsed times for a thread that sleeps are 15ms
        // less than nominal sleep time to 75ms more than nominal sleep time.
        static const int MIN_ELAPSED = SLEEP_TIME - 15;
        static const int MAX_ELAPSED = SLEEP_TIME + 75;

        // Losers may each finish up to 75ms behind winner
        static const int MAX_NOOP_DELAY = 75;

        if (verbose) bsl::cout << "Testing with one 'bslmt::Once' object"
                               << bsl::endl;

        static const int THREAD_COUNT1[] = { 1, 2, 3, 4, 8, 12, 15 };
        static const int NUM_THREAD_COUNTS1 =
            sizeof THREAD_COUNT1 / sizeof THREAD_COUNT1[0];

        for (int t = 1; t < NUM_THREAD_COUNTS1; ++t) {

            int numThreads = THREAD_COUNT1[t];
            ASSERT(numThreads <= MAX_THREADS);

            if (verbose) bsl::cout << "Testing " << numThreads << " thread(s)"
                                   << bsl::endl;

            for (int repetition = 0; repetition < REPEAT_COUNT; ++repetition) {

                BAR barrier(numThreads);
                TA attr;
                TU::Handle threads[MAX_THREADS];
                my_TestRecord testRecords[MAX_THREADS];
                bslmt::Once once = BSLMT_ONCE_INITIALIZER;
                int winner = -1;

                // Initialize the test records
                for (int i = 0; i < numThreads; ++i) {
                    testRecords[i].d_barrier_p   = &barrier;
                    testRecords[i].d_id          = i;
                    testRecords[i].d_once_p      = &once;
                    testRecords[i].d_winner_p    = &winner;
                    testRecords[i].d_sleepTime   = SLEEP_TIME;
                    testRecords[i].d_threadRan   = false;
                    testRecords[i].d_isWinner    = false;
                    testRecords[i].d_foundWinner = -1;
                    testRecords[i].d_endTime     = 0;
                }

                // Start all threads
                Int64 startTime;
                for (int i = 0; i < numThreads; ++i) {
                    if (i == numThreads - 1) {
                        startTime = bsls::TimeUtil::getTimer();
                    }
                    LOOP2_ASSERT(i, numThreads,
                                 0 == TU::create(&threads[i], attr,
                                                 &enterLeaveTest,
                                                 &testRecords[i]));
                }

                // Wait for all threads
                for (int i = 0; i < numThreads; ++i) {
                    LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
                }

                // Verify that there was a winner
                LOOP3_ASSERT(repetition, numThreads, winner, winner != -1);
                LOOP3_ASSERT(repetition, numThreads, winner,
                             testRecords[winner].d_isWinner);

                // Get winner's elapsed time in milliseconds and verify that it
                // is within the expected range.
                int winnerElapsed = (testRecords[winner].d_endTime -
                                     startTime) / 1000000;
                if (verbose) {
                    LOOP3_ASSERT(winner, numThreads, winnerElapsed,
                                 MIN_ELAPSED <= winnerElapsed &&
                                 MAX_ELAPSED >= winnerElapsed);
                }

                // Verify expected results of all threads.
                for (int i = 0; i < numThreads; ++i) {

                    LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

                    if (testRecords[i].d_isWinner) {
                        LOOP3_ASSERT(i, winner, numThreads, i == winner);
                    }

                    LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                                 testRecords[i].d_foundWinner == winner);

                    if (verbose) {
                        int elapsedTime = (testRecords[i].d_endTime -
                                           startTime) / 1000000;
                        // Verify that measured elapsed time is no less than
                        // winner elapsed time and no more than specified delay
                        // above winner elapsed time.
                        LOOP4_ASSERT(i, numThreads, elapsedTime, winnerElapsed,
                                     elapsedTime >= winnerElapsed);
                        LOOP4_ASSERT(i, numThreads, elapsedTime, winnerElapsed,
                                     elapsedTime <= (winnerElapsed +
                                                     MAX_NOOP_DELAY));
                    }
                }

                // Re-initialize the test records
                winner = -1;
                for (int i = 0; i < numThreads; ++i) {
                    testRecords[i].d_threadRan   = false;
                    testRecords[i].d_isWinner    = false;
                    testRecords[i].d_foundWinner = -1;
                    testRecords[i].d_endTime     = 0;
                }

                // Start all threads again
                for (int i = 0; i < numThreads; ++i) {
                    if (i == numThreads - 1) {
                        startTime = bsls::TimeUtil::getTimer();
                    }
                    LOOP2_ASSERT(i, numThreads,
                                 0 == TU::create(&threads[i], attr,
                                                 &enterLeaveTest,
                                                 &testRecords[i]));
                }

                // Wait for all threads
                for (int i = 0; i < numThreads; ++i) {
                    LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
                }

                // Verify no winner and small elapsed times.
                LOOP3_ASSERT(numThreads, repetition, winner, -1 == winner);
                for (int i = 0; i < numThreads; ++i) {
                    LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);
                    ASSERT(! testRecords[i].d_isWinner);
                    ASSERT(-1 == testRecords[i].d_foundWinner);
                    // Verify that elapsed time is no more than specified delay
                    if (verbose) {
                        int elapsedTime = (testRecords[i].d_endTime -
                                           startTime) / 1000000;
                        LOOP3_ASSERT(i, numThreads, elapsedTime,
                                     elapsedTime < MAX_NOOP_DELAY);
                    }
                }

            } // end for (repetition)
        } // end for (numThreads)

        if (verbose) bsl::cout << "Testing with two 'bslmt::Once' objects"
                               << bsl::endl;

        static const int THREAD_COUNT2[] = { 2, 4, 8, 12, 20 };
        static const int NUM_THREAD_COUNTS2 =
            sizeof THREAD_COUNT2 / sizeof THREAD_COUNT2[0];

        for (int t = 1; t < NUM_THREAD_COUNTS2; ++t) {

            int numThreads = THREAD_COUNT2[t];
            ASSERT(numThreads <= MAX_THREADS);

            if (verbose) bsl::cout << "Testing " << numThreads << " thread(s)"
                                   << bsl::endl;

            for (int repetition = 0; repetition < REPEAT_COUNT; ++repetition) {

                BAR barrier(numThreads);
                TA attr;
                TU::Handle threads[MAX_THREADS];
                my_TestRecord testRecords[MAX_THREADS];
                int winner1 = -1;
                int winner2 = -1;
                bslmt::Once once1 = BSLMT_ONCE_INITIALIZER;
                bslmt::Once once2 = BSLMT_ONCE_INITIALIZER;

                // Initialize the test records
                for (int i = 0; i < numThreads; ++i) {
                    // 'x' is true for half the threads
                    bool x = i < numThreads / 2;
                    testRecords[i].d_barrier_p   = &barrier;
                    testRecords[i].d_id          = i;
                    testRecords[i].d_once_p      = x ? &once1 : &once2;
                    testRecords[i].d_winner_p    = x ? &winner1 : &winner2;
                    testRecords[i].d_sleepTime   = SLEEP_TIME;
                    testRecords[i].d_threadRan   = false;
                    testRecords[i].d_isWinner    = false;
                    testRecords[i].d_foundWinner = -1;
                    testRecords[i].d_endTime     = 0;
                }

                // Start all threads
                Int64 startTime;
                for (int i = 0; i < numThreads; ++i) {
                    if (i == numThreads - 1) {
                        startTime = bsls::TimeUtil::getTimer();
                    }
                    LOOP2_ASSERT(i, numThreads,
                                 0 == TU::create(&threads[i], attr,
                                                 enterLeaveTest,
                                                 &testRecords[i]));
                }

                // Wait for all threads
                for (int i = 0; i < numThreads; ++i) {
                    LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
                }

                // Verify that there was a winner for 1st half of threads
                LOOP3_ASSERT(repetition, numThreads, winner1, winner1 != -1);
                LOOP3_ASSERT(repetition, numThreads, winner1,
                             winner1 < numThreads / 2);
                LOOP3_ASSERT(repetition, numThreads, winner1,
                             testRecords[winner1].d_isWinner);

                // Get winner's elapsed time in milliseconds and verify that it
                // is within the expected range.
                int winnerElapsed = (testRecords[winner1].d_endTime -
                                     startTime) / 1000000;
                if (verbose) {
                    LOOP3_ASSERT(winner1, numThreads, winnerElapsed,
                                 MIN_ELAPSED <= winnerElapsed &&
                                 MAX_ELAPSED >= winnerElapsed);
                }

                // Verify expected results of all threads.
                for (int i = 0; i < numThreads / 2; ++i) {

                    LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

                    if (testRecords[i].d_isWinner) {
                        LOOP3_ASSERT(i, winner1, numThreads, i == winner1);
                    }

                    LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                                 testRecords[i].d_foundWinner == winner1);

                    if (verbose) {
                        int elapsedTime = (testRecords[i].d_endTime -
                                           startTime) / 1000000;
                        // Verify that measured elapsed time is no less than
                        // winner elapsed time and no more than specified delay
                        // above winner elapsed time.
                        LOOP4_ASSERT(i, numThreads, elapsedTime, winnerElapsed,
                                     elapsedTime >= winnerElapsed);
                        LOOP4_ASSERT(i, numThreads, elapsedTime, winnerElapsed,
                                     elapsedTime <= (winnerElapsed +
                                                     MAX_NOOP_DELAY));
                    }
                }

                // Verify that there was a winner for 2nd half of threads
                LOOP3_ASSERT(repetition, numThreads, winner2, winner2 != -1);
                LOOP3_ASSERT(repetition, numThreads, winner2,
                             winner2 >= numThreads / 2);
                LOOP3_ASSERT(repetition, numThreads, winner2,
                             testRecords[winner2].d_isWinner);

                // Get winner's elapsed time in milliseconds and verify that it
                // is within the expected range.
                winnerElapsed = (testRecords[winner2].d_endTime -
                                 startTime) / 1000000;
                if (verbose) {
                    LOOP3_ASSERT(winner2, numThreads, winnerElapsed,
                                 MIN_ELAPSED <= winnerElapsed &&
                                 MAX_ELAPSED >= winnerElapsed);
                }

                // Verify expected results of all threads.
                for (int i = numThreads / 2 + 1; i < numThreads; ++i) {

                    LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);

                    if (testRecords[i].d_isWinner) {
                        LOOP3_ASSERT(i, winner2, numThreads, i == winner2);
                    }

                    LOOP3_ASSERT(i, numThreads, testRecords[i].d_foundWinner,
                                 testRecords[i].d_foundWinner == winner2);

                    if (verbose) {
                        int elapsedTime = (testRecords[i].d_endTime -
                                           startTime) / 1000000;

                        // Verify that measured elapsed time is no less than
                        // winner elapsed time and no more than specified delay
                        // above winner elapsed time.
                        LOOP4_ASSERT(i, numThreads, elapsedTime, winnerElapsed,
                                     elapsedTime >= winnerElapsed);
                        LOOP4_ASSERT(i, numThreads, elapsedTime, winnerElapsed,
                                     elapsedTime <= (winnerElapsed +
                                                     MAX_NOOP_DELAY));
                    }
                }

                // Re-initialize the test records
                winner1 = -1;
                winner2 = -1;
                for (int i = 0; i < numThreads; ++i) {
                    testRecords[i].d_threadRan   = false;
                    testRecords[i].d_isWinner    = false;
                    testRecords[i].d_foundWinner = -1;
                    testRecords[i].d_endTime     = 0;
                }

                // Start all threads again
                for (int i = 0; i < numThreads; ++i) {
                    if (i == numThreads - 1) {
                        startTime = bsls::TimeUtil::getTimer();
                    }
                    LOOP2_ASSERT(i, numThreads,
                                 0 == TU::create(&threads[i], attr,
                                                 &enterLeaveTest,
                                                 &testRecords[i]));
                }

                // Wait for all threads
                for (int i = 0; i < numThreads; ++i) {
                    LOOP2_ASSERT(i, numThreads, 0 == TU::join(threads[i]));
                }

                // Verify no winner and small elapsed times.
                LOOP2_ASSERT(repetition, winner1, -1 == winner1);
                LOOP2_ASSERT(repetition, winner2, -1 == winner2);
                for (int i = 0; i < numThreads; ++i) {
                    LOOP2_ASSERT(i, numThreads, testRecords[i].d_threadRan);
                    ASSERT(! testRecords[i].d_isWinner);
                    ASSERT(-1 == testRecords[i].d_foundWinner);
                    if (verbose) {
                    // Verify that elapsed time is no more than specified delay
                        int elapsedTime = (testRecords[i].d_endTime -
                                           startTime) / 1000000;
                        LOOP3_ASSERT(i, numThreads, elapsedTime,
                                     elapsedTime < MAX_NOOP_DELAY);
                    }
                }

            } // end for (repetition)
        } // end for (numThreads)

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Verify that specified function is executed only once
        //
        // Plan:
        //   Create several threads.  Each of them calls several times
        //   the same function through the same Once object.  Join all
        //   threads and ensure that function was invoked only once by
        //   checking special static variable, modified by function
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        const int NUM_THREADS = 25;
        BAR barrier(NUM_THREADS);
        TA attr;
        TU::Handle h[NUM_THREADS];

        my_BreathingTestArg arg(&barrier);

        ASSERT(0 == onceValue);

        for (int i=0; i < NUM_THREADS; ++ i) {
            ASSERT(0 == TU::create(&h[i], attr, breathingThreadFunc, &arg));
        }

        for (int i=0; i < NUM_THREADS; ++ i) {
            ASSERT(0 == TU::join(h[i]));
            ASSERT(1 == onceValue);
        }

        for (int i=0; i < 100; ++i) {
            onceControl.callOnce(breathingOnceFunc);
            ASSERT(onceValue == 1);
        }
      } break;

      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
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
