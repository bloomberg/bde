// bcemt_threadutil.t.cpp        -*-C++-*-
#include <bcemt_threadutil.h>

#include <bcemt_configuration.h>
#include <bcemt_threadattributes.h>
#include <bces_atomictypes.h>
#include <bces_platform.h>

#include <bdef_bind.h>
#include <bdetu_systemtime.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_map.h>

#ifdef BCES_PLATFORM_POSIX_THREADS
#include <pthread.h>
#endif

#ifndef BSLS_PLATFORM_OS_WINDOWS
#include <alloca.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
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

typedef bcemt_ThreadUtil    Obj;

int verbose;
int veryVerbose;
int veryVeryVerbose;

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr)      BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr)      BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_FAIL_RAW(expr)  BSLS_ASSERTTEST_ASSERT_FAIL_RAW(expr)
#define ASSERT_PASS_RAW(expr)  BSLS_ASSERTTEST_ASSERT_PASS_RAW(expr)

//=============================================================================
//                  GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

bsls_Types::IntPtr intPtrAbs(bsls_Types::IntPtr a)
{
    return a >= 0 ? a : -a;
}

//=============================================================================

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
    void eval()
    {
       ++d_count;
    }

    bdef_Function<void(*)()> getFunctor()
    {
       return bdef_BindUtil::bind(&ThreadChecker::eval, this);
    }

    // ACCESSORS
    int count() const
    {
       return d_count;
    }
};

extern "C" void *myThreadFunction(void *)
    // Print to standard output "Another second has passed"
    // every second for five seconds.
{
    for(int i = 0; i < 3; ++i) {
        bcemt_ThreadUtil::microSleep(0, 1);
        if (verbose) bsl::cout << "Another second has passed\n";
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

//-----------------------------------------------------------------------------
//                       Multipriority Usage Test Case
//-----------------------------------------------------------------------------

namespace MULTIPRIORITY_USAGE_TEST_CASE {

// Note that in practice, thread priorities only seem to make a difference
// when multiple stopped threads are simultaneously ready to run, so we won't
// see any effect of the different priorities in this case.

struct MostUrgentThreadFunctor {
    void operator()() const
    {
        if (verbose) {
            bsl::printf("Most urgent\n");
        }
    }
};

struct FairlyUrgentThreadFunctor {
    void operator()() const
    {
        if (verbose) {
            bsl::printf("Fairly urgent\n");
        }
    }
};

struct LeastUrgentThreadFunctor {
    void operator()() const
    {
        if (verbose) {
            bsl::printf("Least urgent\n");
        }
    }
};

}  // close namespace MULTIPRIORITY_USAGE_TEST_CASE

//-----------------------------------------------------------------------------
//                          CONFIGURATION TEST CASE
//-----------------------------------------------------------------------------

namespace BCEMT_CONFIGURATION_TEST_NAMESPACE {

struct Func {
    int         d_stackToUse;
    char       *d_lastBuf;
    static bool s_success;

    void recurser(char *base);
        // Recurse to create depth on stack

    void operator()();
        // Initialize, then call recurser, then set 'd_success'
};
bool Func::s_success;

void Func::recurser(char *base)
    // Consume greater than 'd_stackToUse' of stack depth
{
    char buf[5 * 1000];

    ASSERT(buf != d_lastBuf);    // make sure optimizer didn't remove recursion
    d_lastBuf = buf;

    char garbage = 0xa3;
    for (char *pc = buf; pc < buf + sizeof(buf); ++pc) {
        *pc = (garbage += 9);
    }

    if   (intPtrAbs(buf - base) < d_stackToUse
       && intPtrAbs(buf + sizeof(buf) - base) < d_stackToUse) {
        recurser(base);
    }

    garbage = 0xa3;
    for (char *pc = buf; pc < buf + sizeof(buf); ++pc) {
        ASSERT(*pc == (garbage += 9));
    }
}

void Func::operator()()
{
    if (verbose) P(d_stackToUse);

    d_lastBuf = 0;
    char base;
    recurser(&base);

    s_success = true;
}

}  // close namespace BCEMT_CONFIGURATION_TEST_NAMESPACE


extern "C"
void *configurationTestFunction(void *stackToUse)
{
    BCEMT_CONFIGURATION_TEST_NAMESPACE::Func func;

    func.d_stackToUse = (int) (bsls_Types::IntPtr) stackToUse;
    func.s_success   = false;

    func();

    ASSERT(func.d_stackToUse == (int) (bsls_Types::IntPtr) stackToUse);
    ASSERT(func.s_success);

    return 0;
}

//-----------------------------------------------------------------------------
//                             STACKSIZE TEST CASE
//-----------------------------------------------------------------------------

namespace STACKSIZE_TEST_CASE_NAMESPACE {

template <int BUFFER_SIZE>
struct Func {
    void operator()()
    {
        char buffer[BUFFER_SIZE == 0 ? 1 : BUFFER_SIZE];
        static char *pc;

        char garbage = 0xa3;
        for (pc = buffer; pc < buffer + BUFFER_SIZE; ++pc, garbage += 9) {
            *pc = garbage;
        }
    }

    static
    void staticFunc()
    {
        Func func;
        func();
    }
};

typedef void (*CppFuncPtr)();

extern "C"
void *callCppFunction(void *function)
{
    CppFuncPtr funcPtr = (CppFuncPtr) function;

    (*funcPtr)();

    return 0;
}

template <int BUFFER_SIZE>
void testStackSize()
{
#ifdef BSLS_PLATFORM_OS_UNIX
    // In test cases -2 and -4, Linux was crashing about 4K away from the stack
    // in 32 & 64 bit.  All other unix platforms were running past the end of
    // the stack without crashing.

    enum { FUDGE_FACTOR = 8192 };
#else
    // In test case -4, the crash on 32 bit was further than 12K away from
    // the end of the stack stack, on 64 bit it was further than 16k away.

    enum { FUDGE_FACTOR = 8192 + 2048 * sizeof(void *) };
#endif

    bcemt_ThreadAttributes attr;
    attr.setStackSize(BUFFER_SIZE + FUDGE_FACTOR);
    attr.setGuardSize(0);

    Obj::Handle handle;

    int cRc = Obj::create(&handle, attr, Func<BUFFER_SIZE>());
    LOOP_ASSERT(BUFFER_SIZE, 0 == cRc);
    int jRc = 0;
    if (0 == cRc) {
        jRc = Obj::join(handle);
        ASSERT(0 == jRc);
    }

    if (verbose) {
        cout << "testStackSize<" << BUFFER_SIZE << ">() " <<
                                    ((cRc | jRc) ? "failed\n" : "succeeded\n");
    }

    cRc = Obj::create(&handle,
                      attr,
                      &callCppFunction,
                      (void *) &Func<BUFFER_SIZE>::staticFunc);
    LOOP_ASSERT(BUFFER_SIZE, 0 == cRc);
    jRc = 0;
    if (0 == cRc) {
        jRc = Obj::join(handle);
        ASSERT(0 == jRc);
    }

    if (verbose) {
        cout << "testStackSize<" << BUFFER_SIZE << ">::staticFunc() " <<
                                    ((cRc | jRc) ? "failed\n" : "succeeded\n");
    }
}

}  // close namespace STACKSIZE_TEST_CASE_NAMESPACE

//-----------------------------------------------------------------------------
//                                    TEST CASE 6
//-----------------------------------------------------------------------------

extern "C" {

static
void TlsDestructor6_1(void *);
static
void TlsDestructor6_2(void *);

}  // extern "C"

namespace BCEMT_THREADUTIL_TLSKEY_TEST6 {

Obj::Key parentKey1;
Obj::Key parentKey2;
bces_AtomicInt terminations1;
bces_AtomicInt terminations2;

struct TlsKeyTestFunctor {
    int d_seed;

    // CREATOR
    explicit
    TlsKeyTestFunctor(int seed) : d_seed(seed) {}

    // ACCESSORS
    void operator()() const;
};

void TlsKeyTestFunctor::operator()() const
{
    namespace TC = BCEMT_THREADUTIL_TLSKEY_TEST6;

    int rc;

    ASSERT(0 == Obj::getSpecific(TC::parentKey1));
    ASSERT(0 == Obj::getSpecific(TC::parentKey2));

    rc = Obj::setSpecific(TC::parentKey1, (void *)  (5 * d_seed));
    ASSERT(0 == rc);
    rc = Obj::setSpecific(TC::parentKey2, (void *) (12 * d_seed));
    ASSERT(0 == rc);

    ASSERT((void *)  (5 * d_seed) == Obj::getSpecific(TC::parentKey1));
    ASSERT((void *) (12 * d_seed) == Obj::getSpecific(TC::parentKey2));

    rc = Obj::setSpecific(TC::parentKey1, (void *) 0);
    ASSERT(0 == rc);

    return;
}

}  // close namespace BCEMT_THREADUTIL_TLSKEY_TEST6

extern "C" {

static
void TlsDestructor6_1(void *)
{
    namespace TC = BCEMT_THREADUTIL_TLSKEY_TEST6;

    ++ TC::terminations1;
}

static
void TlsDestructor6_2(void *)
{
    namespace TC = BCEMT_THREADUTIL_TLSKEY_TEST6;

    ++ TC::terminations2;
}

}  // extern "C"

//-----------------------------------------------------------------------------
//                                    TEST CASE 5
//-----------------------------------------------------------------------------

extern "C" {

static
void createKeyTestDestructor5(void *);

}  // extern "C"

namespace BCEMT_THREADUTIL_CREATEKEY_TEST5 {

Obj::Id childId;
Obj::Key parentKey;
Obj::Key childKey1;
Obj::Key childKey2;
int terminated = 0;

struct CreateKeyTestFunctor {
    bool d_doDestructor;    // do the destructor if and only if this is 'true'

    // CREATORS
    explicit
    CreateKeyTestFunctor(bool doDestructor) : d_doDestructor(doDestructor) {}

    // ACCESSORS
    void operator()() const;
};

void CreateKeyTestFunctor::operator()() const
{
    namespace TC = BCEMT_THREADUTIL_CREATEKEY_TEST5;

    TC::childId = Obj::selfId();

    int rc = Obj::createKey(&TC::childKey1,
                            d_doDestructor ? &createKeyTestDestructor5 : 0);
    ASSERT(0 == rc);
    ASSERT(TC::parentKey != TC::childKey1);
    ASSERT(0 == Obj::getSpecific(TC::childKey1));

    rc = Obj::setSpecific(TC::childKey1, (void *) 1);
    ASSERT(0 == rc);

    ASSERT((void *) 1 == Obj::getSpecific(TC::childKey1));

    rc = Obj::createKey(&TC::childKey2, &createKeyTestDestructor5);
    ASSERT(0 == rc);
    ASSERT(TC::childKey2 != TC::childKey1);
    ASSERT(TC::childKey2 != TC::parentKey);
    ASSERT(0 == Obj::getSpecific(TC::childKey2));

    rc = Obj::setSpecific(TC::childKey2, (void *) (d_doDestructor ? 1 : 0));
    ASSERT(0 == rc);

    ASSERT((void *) (d_doDestructor ? 1 : 0) ==
                                              Obj::getSpecific(TC::childKey2));

    return;
}

}  // close namespace BCEMT_THREADUTIL_CREATEKEY_TEST5

extern "C" {

static
void createKeyTestDestructor5(void *data)
{
    namespace TC = BCEMT_THREADUTIL_CREATEKEY_TEST5;

    ASSERT(Obj::selfId() == TC::childId);

    if (TC::terminated < 2)  {
        ASSERT((void *) 1 == data);
        int zeroCount = (0 == Obj::getSpecific(TC::childKey1)) +
                        (0 == Obj::getSpecific(TC::childKey2));
#ifdef BCES_PLATFORM_POSIX_THREADS
        ASSERT(zeroCount == TC::terminated + 1);
#else
        ASSERT(0 == zeroCount);
#endif
    }
    else {
#ifdef BCES_PLATFORM_POSIX_THREADS
        ASSERT((void *) 5 == data);
#else
        ASSERT(0);
#endif
    }

    if (1 == TC::terminated) {
        int rc = Obj::setSpecific(TC::childKey2, (void *) 5);
        ASSERT(0 == rc);
    }

    ++ TC::terminated;
}

}  // extern "C"

inline
long mymax(long a, long b)
{
    return a >= b ? a : b;
}

inline
long myAbs(long a)
{
    return a >= 0 ? a : -a;
}

void testCaseMinus1Recurser(const char *start)
{
    enum { BUF_LEN = 1024 };
    char buffer[BUF_LEN];
    static double lastDistance = 1;

    double distance = (double) mymax(myAbs(&buffer[0]           - start),
                                     myAbs(&buffer[BUF_LEN - 1] - start));
    if (distance / lastDistance > 1.02) {
        cout << (int) distance << endl << flush;
        lastDistance = distance;
    }

    testCaseMinus1Recurser(start);
}

extern "C" void *testCaseMinus1ThreadMain(void *)
{
    char c = 0;
    testCaseMinus1Recurser(&c);

    return 0;
}

//-----------------------------------------------------------------------------
//                                  TEST CASE -2
//-----------------------------------------------------------------------------

#ifndef BSLS_PLATFORM_OS_WINDOWS

enum { CLEARANCE_TEST_START  = 0,
       CLEARANCE_TEST_DONE   = 1234,
       CLEARANCE_BUFFER_SIZE = 64 * 1024 - 600 };

static int clearanceTestState;
static int clearanceTestAllocaSize;

extern "C" void *clearanceTest(void *)
{
    if (clearanceTestAllocaSize) {
        char *pc = (char *) alloca(clearanceTestAllocaSize);
        ASSERT(pc);

        pc[0] = 0;
        ++pc[0];
        pc[clearanceTestAllocaSize - 1] = 0;
        ++pc[clearanceTestAllocaSize - 1];
    }

    clearanceTestState = CLEARANCE_TEST_DONE;

    return 0;
}

#endif

//-----------------------------------------------------------------------------
//                                  TEST CASE -3
//-----------------------------------------------------------------------------

bool stackGrowthIsNegative(char *pc)
{
    char c;

    return &c < pc;
}

//-----------------------------------------------------------------------------
//                                  TEST CASE -4
//-----------------------------------------------------------------------------

extern "C" void *secondClearanceTest(void *vStackSize)
{
    static int growth;
    char c;
    growth = stackGrowthIsNegative(&c) ? -10 : 10;

    static int stackSize;
    stackSize = (int) (bsls_Types::IntPtr) vStackSize;

    static char *pc;
    pc = &c;

    static int diff;

    for (;; pc += growth) {
        *pc = 0;
        diff =  stackSize - (int) myAbs(pc - &c);

        printf("%d\n", diff);
    }

    return 0;
}

//=============================================================================
//                                  MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // TESTING: sleepUntil
        //
        // Note that this is a system-call wrapper, and this test is intended
        // to ensure the system call is correctly called by the 
        // 'bcem_threadutil'.  This test specifically does *not* test the
        // accuracy of the underlying system call.  Also note that
        // due to the nature of the system call, testing values at the upper
        // bound of the valid range is not reasonable.  Test case -5, has been
        // created and run by hand to verify (slightly) longer time periods.
        //
        // Concerns:
        //: 1 'sleepUntil' suspends the current thread until the indicated
        //:    time in the future (within some reasonable limit).
        //: 
        //: 2 'sleepUntil' does not suspend the current thread (or suspends it
        //:    very briefly), for the current time, or times in the past.
        //:
        //: 3  QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Call 'sleepUntil' for a series of values less than a second in
        //:   the future, and verify that system time after sleeping is within
        //:   a reasonable range of the expected target time. (C-1)
        //:
        //: 2 Call 'sleepUntil' for a value in the past, and verify that
        //:   the function returns to the caller in a reasonably small amount
        //:   of time. (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time-interval values. (using the
        //:   'BSLS_ASSERTTEST_*' macros (C-3)
        //
        // Testing:
        //   void sleepUntil(const bdet_TimeInterval& );        
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl 
                 << "CLASS METHOD 'sleepUntil'" << endl
                 << "=========================" << endl;

        if (veryVerbose) {
            cout << "sleepUntil for times in the future" << endl;
        }

        for (int i = 0; i < 5; ++i) {
            if (veryVeryVerbose) {
                cout << "sleepUntil for " << i * 100 << "ms" << endl;
            }

            bdet_TimeInterval expectedTime = bdetu_SystemTime::now();
            
            expectedTime.addMilliseconds(i * 100);
            
            Obj::sleepUntil(expectedTime);
            
            bdet_TimeInterval actualTime = bdetu_SystemTime::now();
            
            ASSERT(actualTime >= expectedTime);
            LOOP_ASSERT((actualTime - expectedTime).totalMilliseconds(),
                        (actualTime - expectedTime).totalMilliseconds() < 50);
        }

        if (veryVerbose) {
            cout << "sleepUntil for times in the past" << endl;
        }
        {
            bdet_TimeInterval expectedTime = bdetu_SystemTime::now();
                      
            Obj::sleepUntil(expectedTime - bdet_TimeInterval(1));
            
            bdet_TimeInterval actualTime = bdetu_SystemTime::now();
            
            ASSERT(actualTime >= expectedTime);
            LOOP_ASSERT((actualTime - expectedTime).totalMilliseconds(),
                        (actualTime - expectedTime).totalMilliseconds() < 50);
            
        }
        if (verbose) { 
            cout << "Negative Testing." << endl;
        }
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            // Note that we must use 'RAW' handlers as the assertions are
            // generated by implementation components.

            ASSERT_PASS(Obj::sleepUntil(bdet_TimeInterval(0)));
            ASSERT_FAIL_RAW(Obj::sleepUntil(bdet_TimeInterval(-1, 0)));
            ASSERT_FAIL_RAW(Obj::sleepUntil(bdet_TimeInterval(0, -1)));
            ASSERT_FAIL_RAW(Obj::sleepUntil(bdet_TimeInterval(253402300800LL,
                                                              0)));
        }
    }  break;
      case 11: {
        // --------------------------------------------------------------------
        // Usgae Example 3: MULTIPLE PRIORITY THREADS
        //
        // Concern:
        //   Need to demonstrate setting priorities for threads.  Note that we
        //   don't demonstrate the different priorities actually having an
        //   effect, we just demonstrate how to set them.
        //
        // Plan:
        //   Spawn 3 threads with different priorities.  Note that this test
        //   works on Windows even though priorities are ignored there, and on
        //   Linux even though the max priority equals the min priority.  This
        //   is because we don't do any testing of which thread runs first, in
        //   fact, since there are only 3 threads and they are all EXTREMELY
        //   short jobs, there probably won't be any contention for processors
        //   at all.  This is just to demonstrate the interface.
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_HPUX
        // Spawning threads fails on HPUX if 'inheritSchedule != true'.

        if (1) break;
#endif

        using namespace MULTIPRIORITY_USAGE_TEST_CASE;

        enum { NUM_THREADS = 3 };

        bcemt_ThreadUtil::Handle handles[NUM_THREADS];
        bcemt_ThreadUtil::Invokable functions[NUM_THREADS] = {
                                                  MostUrgentThreadFunctor(),
                                                  FairlyUrgentThreadFunctor(),
                                                  LeastUrgentThreadFunctor() };
        double priorities[NUM_THREADS] = { 1.0, 0.5, 0.0 };

        bcemt_ThreadAttributes attributes;
        attributes.setInheritSchedule(false);
        const bcemt_ThreadAttributes::SchedulingPolicy policy =
                                     bcemt_ThreadAttributes::BCEMT_SCHED_OTHER;
        attributes.setSchedulingPolicy(policy);

        for (int i = 0; i < NUM_THREADS; ++i) {
            attributes.setSchedulingPriority(
                 bcemt_ThreadUtil::convertToSchedulingPriority(policy,
                                                               priorities[i]));
            int rc = bcemt_ThreadUtil::create(&handles[i],
                                              attributes,
                                              functions[i]);
            ASSERT(0 == rc);
        }

        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc = bcemt_ThreadUtil::join(handles[i]);
            ASSERT(0 == rc);
        }
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // BCEMT_CONFIGURATION TEST
        //
        // Concern:
        //   That bcemt_Configuration can really affect stack size.
        //
        // Plan:
        //   Configure a stack size several times the native default, then
        //   verify the stack is at least about that size.  If the stack is
        //   only about the native size, we should get a stack overflow.
        // --------------------------------------------------------------------

        const int stackSize =
                       5 * bcemt_Configuration::nativeDefaultThreadStackSize();
        const int stackToUse =
                       4 * bcemt_Configuration::nativeDefaultThreadStackSize();
        bcemt_Configuration::setDefaultThreadStackSize(stackSize);

        bcemt_ThreadUtil::Handle handle;

        if (verbose) Q(Test functor with no attributes);
        {
            BCEMT_CONFIGURATION_TEST_NAMESPACE::Func func;

            func.d_stackToUse = stackToUse;
            func.s_success    = false;

            ASSERT(func.d_stackToUse >
                      3 * bcemt_Configuration::nativeDefaultThreadStackSize());

            int rc = bcemt_ThreadUtil::create(&handle, func);
            ASSERT(0 == rc);

            rc = bcemt_ThreadUtil::join(handle);
            ASSERT(0 == rc);

            ASSERT(func.s_success);
            ASSERT(func.d_stackToUse == stackToUse);
        }

        if (verbose) Q(Test functor with default attributes);
        {
            BCEMT_CONFIGURATION_TEST_NAMESPACE::Func func;

            func.d_stackToUse = stackToUse;
            func.s_success    = false;

            bcemt_ThreadAttributes attr;
            int rc = bcemt_ThreadUtil::create(&handle, attr, func);
            ASSERT(0 == rc);

            rc = bcemt_ThreadUtil::join(handle);
            ASSERT(0 == rc);

            ASSERT(func.s_success);
            ASSERT(func.d_stackToUse == stackToUse);
        }

        if (verbose) Q(Test C function with no attributes);
        {
            int rc = bcemt_ThreadUtil::create(
                                     &handle,
                                     &configurationTestFunction,
                                     (void *) (bsls_Types::IntPtr) stackToUse);
            ASSERT(0 == rc);

            rc = bcemt_ThreadUtil::join(handle);
            ASSERT(0 == rc);
        }

        if (verbose) Q(Test C function with default attributes object);
        {
            bcemt_ThreadAttributes attr;
            int rc = bcemt_ThreadUtil::create(
                                     &handle,
                                     attr,
                                     &configurationTestFunction,
                                     (void *) (bsls_Types::IntPtr) stackToUse);
            ASSERT(0 == rc);

            rc = bcemt_ThreadUtil::join(handle);
            ASSERT(0 == rc);
        }
      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // CONVERTTOSCHEDULINGPRIORITY
        //
        // Concern:
        //   That 'convertToSchedulingPriority' works as specced.
        //
        // Plan:
        //   Call 'get{Min,Max}SchedPriority' and compare the results they
        //   return to results reterned by 'convertToSchedulingPriority'.
        // --------------------------------------------------------------------

        typedef bcemt_ThreadAttributes Attr;

        Attr::SchedulingPolicy policies[] = { Attr::BCEMT_SCHED_OTHER,
                                              Attr::BCEMT_SCHED_FIFO,
                                              Attr::BCEMT_SCHED_RR,
                                              Attr::BCEMT_SCHED_DEFAULT };
        enum { NUM_POLICIES = sizeof policies / sizeof *policies };

        for (int i = 0; i < NUM_POLICIES; ++i) {
            const Attr::SchedulingPolicy POLICY = policies[i];

            const int minPri = Obj::getMinSchedulingPriority(POLICY);
            const int maxPri = Obj::getMaxSchedulingPriority(POLICY);

            ASSERT(Attr::getMinSchedPriority(POLICY) == minPri);
            ASSERT(Attr::getMaxSchedPriority(POLICY) == maxPri);

            const int loPri =  Obj::convertToSchedulingPriority(POLICY, 0.0);
            LOOP2_ASSERT(loPri, minPri, loPri == minPri);

            const int midPri = Obj::convertToSchedulingPriority(POLICY, 0.5);
            LOOP2_ASSERT(midPri, minPri, midPri >= minPri);
            LOOP2_ASSERT(midPri, maxPri, midPri <= maxPri);

            const int hiPri =  Obj::convertToSchedulingPriority(POLICY, 1.0);
            LOOP2_ASSERT(hiPri, maxPri, hiPri == maxPri);

            if (hiPri != loPri) {
                LOOP2_ASSERT(hiPri, loPri, hiPri >= loPri + 2);

                LOOP2_ASSERT(midPri, hiPri, midPri < hiPri);
                LOOP2_ASSERT(midPri, loPri, midPri > loPri);
            }
            else {
#if !defined(BSLS_PLATFORM_OS_LINUX) && !defined(BSLS_PLATFORM_OS_WINDOWS)
                // This should only happen on Linux and Windows
                ASSERT(0);
#endif

                LOOP2_ASSERT(midPri, hiPri, midPri == hiPri);
                LOOP2_ASSERT(midPri, loPri, midPri == loPri);
            }
        }
      }  break;
      case 8: {
        // --------------------------------------------------------------------
        // STACK SIZE
        //
        // Concern:
        //   Does setting 'stackSize' allow one to create a buffer of nearly
        //   that size in the thread?
        //
        // Plan:
        //   For various stack sizes, create threads with that stack size and
        //   create a buffer of nearly that size in the thread (minus a
        //   platform-dependent fudge factor), and see if we crash.  Note that
        //   this test is not guaranteed to fail if there is a problem -- it is
        //   possible that if there is a bug and invalid memory access occurs,
        //   it will just abuse the heap but not result in a crash.
        // --------------------------------------------------------------------

        namespace TC = STACKSIZE_TEST_CASE_NAMESPACE;

        enum { K = 1024 };

        if (verbose) {
#ifdef PTHREAD_STACK_MIN
            P(PTHREAD_STACK_MIN);
#else
            cout << "'PTHREAD_STACK_MIN' undefined\n";
#endif
        }

        TC::testStackSize<    0    >();
        TC::testStackSize<    1 * K>();
        TC::testStackSize<    2 * K>();
        TC::testStackSize<    3 * K>();
        TC::testStackSize<    4 * K>();
        TC::testStackSize<    7 * K>();
        TC::testStackSize<    8 * K>();
        TC::testStackSize<    9 * K>();
        TC::testStackSize<   10 * K>();
        TC::testStackSize<   12 * K>();
        TC::testStackSize<   14 * K>();
        TC::testStackSize<   15 * K>();
        TC::testStackSize<   16 * K>();
        TC::testStackSize<   17 * K>();
        TC::testStackSize<   18 * K>();
        TC::testStackSize<   20 * K>();
        TC::testStackSize<   24 * K>();
        TC::testStackSize<   28 * K>();
        TC::testStackSize<   31 * K>();
        TC::testStackSize<   32 * K>();
        TC::testStackSize<   33 * K>();
        TC::testStackSize<   36 * K>();
        TC::testStackSize<   40 * K>();
        TC::testStackSize<   44 * K>();
        TC::testStackSize<   48 * K>();
        TC::testStackSize<   52 * K>();
        TC::testStackSize<   56 * K>();
        TC::testStackSize<   58 * K>();
        TC::testStackSize<   60 * K>();
        TC::testStackSize<   62 * K>();
        TC::testStackSize<   63 * K>();
        TC::testStackSize<   64 * K>();
        TC::testStackSize<   65 * K>();
        TC::testStackSize<   68 * K>();
        TC::testStackSize<   72 * K>();
        TC::testStackSize<   76 * K>();
        TC::testStackSize<   80 * K>();
        TC::testStackSize<   84 * K>();
        TC::testStackSize<   88 * K>();
        TC::testStackSize<   92 * K>();
        TC::testStackSize<   96 * K>();
        TC::testStackSize<  100 * K>();
        TC::testStackSize<  104 * K>();
        TC::testStackSize<  108 * K>();
        TC::testStackSize<  112 * K>();
        TC::testStackSize<  116 * K>();
        TC::testStackSize<  120 * K>();
        TC::testStackSize<  124 * K>();
        TC::testStackSize<  127 * K>();
        TC::testStackSize<  128 * K>();
        TC::testStackSize<  129 * K>();
        TC::testStackSize<  255 * K>();
        TC::testStackSize<  256 * K>();
        TC::testStackSize<  257 * K>();
        TC::testStackSize<  511 * K>();
        TC::testStackSize<  512 * K>();
        TC::testStackSize<  513 * K>();
        TC::testStackSize< 1023 * K>();
        TC::testStackSize< 1024 * K>();
        TC::testStackSize< 1025 * K>();
        TC::testStackSize< 2047 * K>();
        TC::testStackSize< 2048 * K>();
        TC::testStackSize< 2049 * K>();
        TC::testStackSize< 4095 * K>();
        TC::testStackSize< 4096 * K>();
        TC::testStackSize< 4097 * K>();
        TC::testStackSize< 8191 * K>();
        TC::testStackSize< 8192 * K>();
        TC::testStackSize< 8193 * K>();
        TC::testStackSize<16383 * K>();
        TC::testStackSize<16384 * K>();
        TC::testStackSize<16385 * K>();
      }  break;
      case 7: {
        // --------------------------------------------------------------------
        // DELETEKEY, THREAD-SPECIFICITY OF DATA TEST ON TLS
        //
        // Concerns:
        //   That 'deleteKey' deletes a TLS key for ALL threads, that data
        //   associated with a key by 'setSpecific' is thread-specific.
        //
        // Plan: Create 2 keys in the parent thread.  Verify both
        // exist in the child thread -- delete 1 in the child thread,
        // verify the data set in one thread is not visible from
        // another thread and does not affect the data set in another
        // thread.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                       "'deleteKey', THREAD SPECIFICITY OF DATA IN TLS TEST\n"
                       "===================================================\n";

        namespace TC = BCEMT_THREADUTIL_TLSKEY_TEST6;

        Obj::createKey(&TC::parentKey1, &TlsDestructor6_1);
        Obj::createKey(&TC::parentKey2, &TlsDestructor6_2);

        int rc;
        rc = Obj::setSpecific(TC::parentKey1, (void *) 1);
        ASSERT(0 == rc);
        rc = Obj::setSpecific(TC::parentKey2, (void *) 2);
        ASSERT(0 == rc);

        bcemt_ThreadUtil::Handle handle;
        bcemt_ThreadUtil::create(&handle, TC::TlsKeyTestFunctor(10));
        bcemt_ThreadUtil::join(handle);

        bcemt_ThreadUtil::create(&handle, TC::TlsKeyTestFunctor(20));
        bcemt_ThreadUtil::join(handle);

        bcemt_ThreadUtil::create(&handle, TC::TlsKeyTestFunctor(30));
        bcemt_ThreadUtil::join(handle);

        rc = Obj::deleteKey(TC::parentKey1);
        ASSERT(0 == rc);
        rc = Obj::deleteKey(TC::parentKey1);
        ASSERT(0 != rc);

        ASSERT(0 == TC::terminations1);
        ASSERT(3 == TC::terminations2);

#if 0
        // parentKey1 is deleted -- this is undefined behavior

        ASSERT((void *) 1 == Obj::getSpecific(TC::parentKey1));
        ASSERT(0 == Obj::setSpecific(TC::parentKey1, (void *) 7));
#endif

        // check 'parentKey2; is unaffected
        ASSERT((void *) 2 == Obj::getSpecific(TC::parentKey2));
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING CREATEKEY, SETSPECIFIC, AND GETSPECIFIC
        //
        // Concerns: That the destructor function passed to 'createKey' is
        //   executed by the thread that is terminating, that 'createKey',
        //   'setSpecific', // and 'getSpecific' all work in the thread as well
        //   as in the destructor function.
        //
        // Plan:
        //   Spawn a thread, and within that thread create 2 keys and associate
        //   non-zero values with them, and both keys associated with the same
        //   destructor.  Verify that the destructor is called twice.  Repeat
        //   the process passing 0 in the 'destructor' fields of the keys, and
        //   verify that the destructor is not called.
        //       The second time the destructor is terminated, associate a
        //   non-zero value with one of the keys and verify this results in its
        //   being called again.
        // --------------------------------------------------------------------

        namespace TC = BCEMT_THREADUTIL_CREATEKEY_TEST5;

        Obj::Key parentKey;
        int rc = Obj::createKey(&parentKey, 0);
        ASSERT(0 == rc);

        void *data = Obj::getSpecific(parentKey);
        ASSERT(0 == data);

        rc = Obj::setSpecific(parentKey, (void *) 2);
        ASSERT(0 == rc);

        ASSERT((void *) 2 == Obj::getSpecific(parentKey));

        bcemt_ThreadUtil::Handle handle;
        bcemt_ThreadUtil::create(&handle, TC::CreateKeyTestFunctor(true));
        bcemt_ThreadUtil::join(handle);

#ifdef BCES_PLATFORM_POSIX_THREADS
        ASSERT(3 == TC::terminated);
#else
        ASSERT(2 == TC::terminated);
#endif
        ASSERT(parentKey != TC::childKey1);
        ASSERT(Obj::selfId() != TC::childId);

        ASSERT((void *) 2 == Obj::getSpecific(parentKey));
        ASSERT(0 == Obj::getSpecific(TC::childKey1));

        TC::terminated = 0;
        TC::childKey1 = parentKey;
        TC::childId = Obj::selfId();

        bcemt_ThreadUtil::create(&handle, TC::CreateKeyTestFunctor(false));
        bcemt_ThreadUtil::join(handle);

        ASSERT(0 == TC::terminated);
        ASSERT(parentKey != TC::childKey1);
        ASSERT(Obj::selfId() != TC::childId);

        ASSERT((void *) 2 == Obj::getSpecific(parentKey));
        ASSERT(0 == Obj::getSpecific(TC::childKey1));
        ASSERT(0 == Obj::getSpecific(TC::childKey2));
      }  break;
      case 5: {
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
#if defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_LINUX)
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
    }  break;
    case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE Example 2
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSmall stack usage example" << endl;

        createSmallStackSizeThread();    // usage example 2
    }  break;
    case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE Example 1
        //
        // Concern: that the usage examples (including those that were
        // previously in the 'bcemt_thread' component) compile and work
        // properly.
        // --------------------------------------------------------------------

        // BASIC EXAMPLE
        if (verbose) cout << "\nBasic thread utilities example" << endl;

        bcemt_Configuration::setDefaultThreadStackSize(
                     bcemt_Configuration::recommendedDefaultThreadStackSize());

        bcemt_ThreadAttributes attr;
        attr.setStackSize(1024 * 1024);

        bcemt_ThreadUtil::Handle handle;
        int rc = bcemt_ThreadUtil::create(&handle, attr, myThreadFunction, 0);
        ASSERT(0 == rc);
        bcemt_ThreadUtil::yield();
        rc = bcemt_ThreadUtil::join(handle);
        ASSERT(0 == rc);

        if (verbose) bsl::cout << "A three second interval has elapsed\n";
    }  break;
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
    }  break;
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
      }  break;
      case -1: {
        // --------------------------------------------------------------------
        // STACK OVERFLOW TEST
        //
        // Concern:
        //   How big is the stack, really?  The main concern here is that the
        //   stack size on HPUX is being properly adjusted so that the
        //   specified stack size really is close to the effective stack size.
        //
        // Plan:
        //   Recurse, printing out how deep we are, until we overflow.
        //
        // Observations:
        //   So: Solaris
        //   AI: AIX
        //   HP: HPUX
        //   Li: Linux
        //   Wi: Windows
        //
        //   Results accurate to 2% or worse
        //
        //   Note that on AIX, the stack overflow causes the thread to die
        //   without any warning messages, and the thread attempting to join
        //   it just hangs.  On other Unix platforms, the stack overflow causes
        //   a segfault.
        //
        //   Native Limit:
        //   -------------
        //
        //   So 32:  1035871
        //   So 64:  2082903
        //
        //   AI 32:   112128
        //   AI 64:   213760
        //
        //   HP 32:   110240
        //   HP 64:   110240
        //
        //   Li 32: 66691239
        //   Li 64: 66359287
        //
        //   Wi 32:  1022207
        //
        //   export CASE_MINUS_1_STACK_SIZE=1000000:
        //   ---------------------------------------
        //
        //   So 32:   994111
        //   So 64:  1005879
        //
        //   AI 32:  1020832
        //   AI 64:  1118720
        //
        //   HP 32:   966160
        //   HP 64:   966160
        //
        //   Li 32:   976567
        //   Li 64:   991159
        //
        //   Wi 32:  1022207
        //
        //   export CASE_MINUS_1_DEFAULT_SIZE=800000:
        //   ----------------------------------------
        //
        //   So 32:   789119
        //   So 64:   787479
        //
        //   AI 32:   884176
        //   AI 64:   806400
        //
        //   HP 32:   771680
        //   HP 64:   771680
        //
        //   Li 32:   780391
        //   Li 64:   775735
        //
        //   Wi 32:   834239
        // --------------------------------------------------------------------

        bcemt_ThreadAttributes attr;

        const char *stackSizeString = bsl::getenv("CASE_MINUS_1_STACK_SIZE");
        if (stackSizeString) {
            attr.setStackSize(atoi(stackSizeString));
            P(attr.stackSize());
        }

        const char *defaultSizeString =
                                      bsl::getenv("CASE_MINUS_1_DEFAULT_SIZE");
        if (defaultSizeString) {
            BSLS_ASSERT_OPT(!stackSizeString);
            bcemt_Configuration::setDefaultThreadStackSize(
                                                      atoi(defaultSizeString));
            P(bcemt_Configuration::defaultThreadStackSize());
        }

        bcemt_ThreadUtil::Handle handle;
        if (stackSizeString) {
            bcemt_ThreadUtil::create(&handle, attr, &testCaseMinus1ThreadMain,
                                     0);
        }
        else {
            bcemt_ThreadUtil::create(&handle, &testCaseMinus1ThreadMain, 0);
        }

        bcemt_ThreadUtil::join(handle);
      }  break;
#ifndef BSLS_PLATFORM_OS_WINDOWS
      case -2: {
        // --------------------------------------------------------------------
        // CLEARANCE TEST
        //
        // Concern:
        //   What is the minimum amount of stack needed for a thread to
        //   function?
        //
        // Plan:
        //   Specify a stack size at run time and verify that threads can be
        //   spawned with that stack size.  Since this test uses 'alloca.h',
        //   which doesn't exist on Windows, the test is disabled there.
        // --------------------------------------------------------------------

        bcemt_ThreadAttributes attr;
        ASSERT(verbose);
        int clearanceTestStackSize = bsl::atoi(argv[2]);
        P(clearanceTestStackSize);
        attr.setStackSize(clearanceTestStackSize);
        attr.setGuardSize(0);

        clearanceTestAllocaSize = 0;
        clearanceTestState = CLEARANCE_TEST_START;
        bcemt_ThreadUtil::Handle handle;
        int rc = bcemt_ThreadUtil::create(&handle, attr, &clearanceTest, 0);
        ASSERT(0 == rc);
        rc = bcemt_ThreadUtil::join(handle);
        ASSERT(0 == rc);
        ASSERT(CLEARANCE_TEST_DONE == clearanceTestState);

        Q(Test 0 Completed);

        for (clearanceTestAllocaSize = clearanceTestStackSize / 2;;
                                              clearanceTestAllocaSize += 100) {
            int diff = clearanceTestStackSize - clearanceTestAllocaSize;
            P(diff);

            clearanceTestState = CLEARANCE_TEST_START;
            rc = bcemt_ThreadUtil::create(&handle, attr, &clearanceTest, 0);
            ASSERT(0 == rc);
            rc = bcemt_ThreadUtil::join(handle);
            ASSERT(0 == rc);

            ASSERT(CLEARANCE_TEST_DONE == clearanceTestState);
        }

        Q(Alloca Test Completed);
      }  break;
#endif
      case -3: {
        // --------------------------------------------------------------------
        // STACK GROWTH DIRECTION TEST
        //
        // Concern:
        //   Determine whether the stack grows in a positive or negative
        //   direction.
        //
        // Plan:
        //   Declare two automatic variables in two different stack frames,
        //   subtract pointers between the two to deterimine direction of stack
        //   growth.
        // --------------------------------------------------------------------

        char c;

        cout << (stackGrowthIsNegative(&c) ? "negative" : "positive") << endl;
      }  break;
      case -4: {
        // --------------------------------------------------------------------
        // SECOND STACK CLEARANCE TEST
        //
        // Concern:
        //   Determine stack size by, rather than recursing, just accessing
        //   memory further and further up the stack.
        // --------------------------------------------------------------------

        setbuf(stdout, 0);

        bcemt_ThreadAttributes attr;
        ASSERT(verbose);

#ifdef PTHREAD_STACK_MIN
        int stackSize = (int) PTHREAD_STACK_MIN;
#else
        int stackSize = 1 << 17;
#endif
        if (verbose) {
            stackSize += atoi(argv[2]);
        }
        printf("stackSize = %d\n", stackSize);

        attr.setStackSize(stackSize);
        attr.setGuardSize(0);

        bcemt_ThreadUtil::Handle handle;
        int rc = bcemt_ThreadUtil::create(&handle,
                                          attr,
                                          &secondClearanceTest,
                                          (void *) stackSize);
        ASSERT(0 == rc);
        rc = bcemt_ThreadUtil::join(handle);
      }  break;
      case -5: {
        // --------------------------------------------------------------------
        // TESTING: sleepUntil (Longer duration)
        //
        // Note that this test case is intended to be run manually, and is an 
        // extension to test case 2 that tests for longer durations than should
        // be run in a typical build cycle.
        //
        // Concerns:
        //: 1 'sleepUntil' suspends the current thread until the indicated
        //:    time in the future (within some reasonable limit).  For
        //:    times > 1s in the future.
        //
        // Plan:
        //: 1 Call 'sleepUntil' for a series of values less than a second in
        //:   the future, and verify that system time after sleeping is within
        //:   a reasonable range of the expected target time. (C-1)
        //
        // Testing:
        //   void sleepUntil(const bdet_TimeInterval& );        
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl 
                 << "CLASS METHOD 'sleepUntil'" << endl
                 << "=========================" << endl;

        if (veryVerbose) {
            cout << "sleepUntil for times in the future" << endl;
        }

        for (int i = 0; i < 5; ++i) {
            if (veryVeryVerbose) {
                cout << "sleepUntil for " << i * 30 << "s" << endl;
            }

            bdet_TimeInterval expectedTime = bdetu_SystemTime::now();
            
            expectedTime.addSeconds(i * 30);
            
            Obj::sleepUntil(expectedTime);
            
            bdet_TimeInterval actualTime = bdetu_SystemTime::now();
            
            ASSERT(actualTime >= expectedTime);
            LOOP_ASSERT((actualTime - expectedTime).totalMilliseconds(),
                        (actualTime - expectedTime).totalMilliseconds() < 50);
        }

        
    }  break;
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
