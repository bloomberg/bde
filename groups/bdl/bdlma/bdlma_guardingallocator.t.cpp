// bdlma_guardingallocator.t.cpp                                      -*-C++-*-
#include <bdlma_guardingallocator.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignmentutil.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

#include <setjmp.h>
#include <signal.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
  #include <crtdbg.h>   // '_CrtSetReportMode', to suppress popups
  #include <windows.h>  // 'GetSystemInfo'
#else
  #include <pthread.h>
  #include <unistd.h>   // 'sysconf'
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// 'bdlma::GuardingAllocator' is a special-purpose allocator mechanism that
// adjoins a read/write protected guard page to each memory block that is
// returned by the 'allocate' method.  The primary concern is that the memory
// allocated from the system facility on behalf of the 'allocate' method (in
// particular, guard pages) is managed correctly (see the 'Guard Pages' section
// of the component-level documentation).  'setjmp' and 'longjmp' are used in
// conjunction with a signal handler to test that the guard pages are write-
// protected.  (Care is taken to ensure that 'longjmp' does not by-pass the
// destruction of any stack objects that are of user-defined type.)  Note that
// since the 'bdlma::GuardingAllocator' constructor does not accept an optional
// allocator argument, there is scant opportunity to use 'bslma::TestAllocator'
// in this test driver.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] GuardingAllocator(GuardPageLocation l = e_AFTER_USER_BLOCK);
// [ 2] ~GuardingAllocator();
//
// MANIPULATORS
// [ 3] void *allocate(size_type size);
// [ 3] void deallocate(void *address);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 4] CONCERN: The 'allocate' and 'deallocate' methods are thread-safe.

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
//                       STANDARD BDE TEST DRIVER MACROS
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

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL VARIABLES / TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlma::GuardingAllocator Obj;
typedef Obj::GuardPageLocation   Enum;

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef jmp_buf    JumpBuffer;
#else
typedef sigjmp_buf JumpBuffer;
#endif

static JumpBuffer g_jumpBuffer;
static bool       g_withinTestFlag = false;  // see 'signalHandler' (below)

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

typedef void *(*ThreadFunction)(void *arg);

// ============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

extern "C" {

void signalHandler(int signal)
    // Handle the specified 'signal'.  Note that this signal handler is
    // intended for 'SIGSEGV' and 'SIGBUS' only.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    ASSERT(SIGSEGV == signal);
#else
    ASSERT(SIGSEGV == signal || SIGBUS == signal);
#endif

    if (g_withinTestFlag) {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        longjmp   (g_jumpBuffer, 1);
#else
        siglongjmp(g_jumpBuffer, 1);
#endif
    }
    else {
        ASSERT("Unexpected invocation of 'signalHandler'."  && 0);
    }
}

}  // close extern "C"

static
bool causesMemoryFault(void *address, int offset, char value)
    // Return 'true' if assigning the specified 'value' to the byte at the
    // specified 'offset' from the specified 'address' causes a memory fault,
    // and 'false' otherwise.
{
    bool faultFlag = false;

    // Register a signal handler for the duration of this function.

    signal(SIGSEGV, signalHandler);

#ifndef BSLS_PLATFORM_OS_WINDOWS
    signal(SIGBUS,  signalHandler);
#endif

    // Enable the signal handler's "long jump" capabilities.

    g_withinTestFlag = true;

    // Set the jump position.

#ifdef BSLS_PLATFORM_OS_WINDOWS
    const int rc = setjmp(g_jumpBuffer);
#else
    const int rc = sigsetjmp(g_jumpBuffer, 1);
#endif

    if (0 == rc) {
        // Write 'value' to the desired location.  If there is a memory fault,
        // then 'signalHandler' should be invoked.

        *(static_cast<char *>(address) + offset) = value;
    }
    else if (1 == rc) {
        // There was a long jump from 'signalHandler'.

        faultFlag = true;
    }
    else {
        ASSERT("Unexpected return value from 'setjmp' or 'sigsetjmp'."  && 0);
    }

    // Restore the default behavior for the signals.

    signal(SIGSEGV, SIG_DFL);

#ifndef BSLS_PLATFORM_OS_WINDOWS
    signal(SIGBUS,  SIG_DFL);
#endif

    // Disable the signal handler's "long jump" capabilities.

    g_withinTestFlag = false;

    return faultFlag;
}

// 'OFFSET' definition copied from the component '.cpp' file.

const bslma::Allocator::size_type OFFSET =
                                       bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

struct AfterUserBlockDeallocationData
    // Helper struct storing the addresses we need for deallocation when the
    // guard page location is 'e_AFTER_USER_BLOCK'.
{
    void *d_firstPage; // address we need to deallocate
    void *d_guardPage; // address of the page we need to un-protect
};

AfterUserBlockDeallocationData *getDataBlockAddress(void *address)
    // Utility function to compute the 'AfterUserBlockDeallocationData*'
    // corresponding to the specified 'address'.
{
    return static_cast<AfterUserBlockDeallocationData*>(
            static_cast<void*>(
                static_cast<char *>(address) - OFFSET * 2));
}

static
void *guardPageAddress(void *address, Enum location, int pageSize)
    // Return the address of the guard page, having the specified 'pageSize'
    // (in bytes), associated with the specified 'address' that was allocated
    // from a guarding allocator configured for guard pages at the specified
    // 'location' (with respect to 'address').
{
    if (Obj::e_AFTER_USER_BLOCK == location) {
        AfterUserBlockDeallocationData *deallocData =
            getDataBlockAddress(address);

        return deallocData->d_guardPage;                              // RETURN
    }
    else {
        ASSERT(Obj::e_BEFORE_USER_BLOCK == location);

        return static_cast<char *>(address) - pageSize;               // RETURN
    }
}

static
void assertGuardPageIsProtected(void *address, Enum location, int pageSize)
    // Assert that the guard page, having the specified 'pageSize' (in bytes),
    // associated with the specified 'address' that was allocated from a
    // guarding allocator configured for guard pages at the specified
    // 'location' (with respect to 'address'), is write-protected.
{
    // For efficiency, test the first and last bytes of the guard page, and
    // every 64th byte in between.

    void *g = guardPageAddress(address, location, pageSize);

    for (int i = 0; i < pageSize; i += 64) {
        LOOP_ASSERT(i, causesMemoryFault(g, i, 'x'));
    }

    const int lastByte = pageSize - 1;

    LOOP_ASSERT(lastByte, causesMemoryFault(g, lastByte, 'x'));
}

static
void overwritePadding(void *address, int size, Enum location, int pageSize)
    // Overwrite the (unprotected) padding associated with the specified
    // 'address' that was allocated, using the specified request 'size' (in
    // bytes), from a guarding allocator configured for guard pages of the
    // specified 'pageSize' (in bytes) at the specified 'location' (with
    // respect to 'address').  Note that this method is for white-box testing.
    // Also note that this method is based on the implementation of 'allocate'.
{
    const int roundedUpSize =
        static_cast<int>(bsls::AlignmentUtil::roundUpToMaximalAlignment(size));

    // Adjust for additional memory needed to stash reference addresses when
    // 'e_AFTER_USER_BLOCK' is in use.

    const int adjustedSize = Obj::e_AFTER_USER_BLOCK == location
                           ? roundedUpSize + static_cast<int>(OFFSET) * 2
                           : roundedUpSize;

    // Calculate the number of pages required for 'adjustedSize'.

    const int numPages = (adjustedSize + pageSize - 1) / pageSize;

    const int numPaddingBytes = numPages * pageSize - adjustedSize;

    void *p = 0;

    if (location == Obj::e_AFTER_USER_BLOCK) {
        AfterUserBlockDeallocationData *deallocData =
            getDataBlockAddress(address);

        p = deallocData->d_firstPage;
    }
    else {
        p = static_cast<char *>(address) + roundedUpSize;
    }

    ASSERT(p);

    bsl::memset(p, 0xff, numPaddingBytes);
}

static
ThreadId createThread(ThreadFunction function, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)function, arg, 0, 0);
#else
    ThreadId id;
    pthread_create(&id, 0, function, arg);
    return id;
#endif
}

static
void joinThread(ThreadId id)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
#else
    pthread_join(id, 0);
#endif
}

namespace TestCase4 {

struct ThreadInfo {
    int  d_numIterations;
    Obj *d_objX_p;
    Obj *d_objY_p;
};

extern "C" void *threadFunction1(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_objX_p;
    Obj& mY = *info->d_objY_p;

    int n = 2;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p = mX.allocate(n);  bsl::memset(p, 0xff, n);

        void *q = mY.allocate(n);  bsl::memset(q, 0xff, n);

        mX.deallocate(p);

        mY.deallocate(q);

        if (n > 10000) {
            n = 2;
        }
        else {
            n *= 2;
        }
    }

    return arg;
}

extern "C" void *threadFunction2(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_objX_p;
    Obj& mY = *info->d_objY_p;

    int n = 3;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p1 = mX.allocate(n);      bsl::memset(p1, 0xff, n);
        void *q1 = mY.allocate(n);      bsl::memset(q1, 0xff, n);

        void *p2 = mX.allocate(n * 2);  bsl::memset(p2, 0xff, n * 2);
        void *q2 = mY.allocate(n * 2);  bsl::memset(q2, 0xff, n * 2);

        mY.deallocate(q2);
        mY.deallocate(q1);

        mX.deallocate(p1);
        mX.deallocate(p2);

        if (n > 10000) {
            n = 3;
        }
        else {
            n *= 3;
        }
    }

    return arg;
}

extern "C" void *threadFunction3(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_objX_p;
    Obj& mY = *info->d_objY_p;

    int n = 5;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *q1 = mY.allocate(n);      bsl::memset(q1, 0xff, n);
        void *q2 = mY.allocate(n * 3);  bsl::memset(q2, 0xff, n * 3);
        void *q3 = mY.allocate(n * 7);  bsl::memset(q3, 0xff, n * 7);

        void *p1 = mX.allocate(n);      bsl::memset(p1, 0xff, n);
        void *p2 = mX.allocate(n * 3);  bsl::memset(p2, 0xff, n * 3);
        void *p3 = mX.allocate(n * 7);  bsl::memset(p3, 0xff, n * 7);

        mX.deallocate(p3);
        mX.deallocate(p2);
        mX.deallocate(p1);

        mY.deallocate(q1);
        mY.deallocate(q2);
        mY.deallocate(q3);

        if (n > 10000) {
            n = 5;
        }
        else {
            n *= 5;
        }
    }

    return arg;
}

}  // close namespace TestCase4

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Diagnosing Buffer Overflow
///- - - - - - - - - - - - - - - - - - -
// Use of a 'bdlma::GuardingAllocator' is indicated, for example, if some code
// under development is suspected of having a buffer overrun (or underrun) bug,
// and more sophisticated tools that detect such conditions are either not
// available, or are inconvenient to apply to the situation at hand.
//
// This usage example illustrates a guarding allocator being brought to bear on
// a buffer overrun bug.  The bug in question arises in the context of an
// artificial data handling class, 'my_DataHandler'.  This class makes use of a
// (similarly artificial) data translation utility that translates chunks of
// data among various data styles.  In our idealized example, we assume that
// the length of the output resulting from some data translation is precisely
// determinable from the length of the input data and the respective styles of
// the input and the (desired) output.  For simplicity, we also assume that
// input data comes from a trusted source.
//
// First, we define an enumeration of data styles:
//..
    enum my_DataStyle {
        e_STYLE_NONE = 0
      , e_STYLE_A    = 1  // default style
      , e_STYLE_AA   = 2  // style exactly twice as verbose as 'e_STYLE_A'
      // etc.
    };
//..
// Next, we define the (elided) interface of our data translation utility:
//..
    struct my_DataTranslationUtil {
        // This 'struct' provides a namespace for data translation utilities.

        // CLASS METHODS
        static int outputSize(my_DataStyle outputStyle,
                              my_DataStyle inputStyle,
                              int          inputLength);
            // Return the buffer size (in bytes) required to store the result
            // of converting input data of the specified 'inputLength' (in
            // bytes), in the specified 'inputStyle', into the specified
            // 'outputStyle'.  The behavior is undefined unless
            // '0 <= inputLength'.

        static int translate(char         *output,
                             my_DataStyle  outputStyle,
                             const char   *input,
                             my_DataStyle  inputStyle);
            // Load into the specified 'output' buffer the result of converting
            // the specified 'input' data, in the specified 'inputStyle', into
            // the specified 'outputStyle'.  Return 0 on success, and a
            // non-zero value otherwise.  The behavior is undefined unless
            // 'output' has sufficient capacity to hold the translated result.
            // Note that this method assumes that 'input' originated from a
            // trusted source.
    };
//..
// Next, we define 'my_DataHandler', a simple class that makes use of
// 'my_DataTranslationUtil':
//..
    class my_DataHandler {
        // This 'class' provides a basic data handler.

        // DATA
        my_DataStyle      d_inStyle;     // style of 'd_inBuffer' contents
        char             *d_inBuffer;    // input supplied at construction
        int               d_inCapacity;  // capacity (in bytes) of 'd_inBuffer'
        my_DataStyle      d_altStyle;    // alternative style (if requested)
        char             *d_altBuffer;   // buffer for alternative style
        bslma::Allocator *d_allocator_p; // memory allocator (held, not owned)

      private:
        // Not implemented:
        my_DataHandler(const my_DataHandler&);

      public:
        // CREATORS
        my_DataHandler(const char       *input,
                       int               inputLength,
                       my_DataStyle      inputStyle,
                       bslma::Allocator *basicAllocator = 0);
            // Create a data handler for the specified 'input' data, in the
            // specified 'inputStyle', having the specified 'inputLength' (in
            // bytes).  Optionally specify a 'basicAllocator' used to supply
            // memory.  If 'basicAllocator' is 0, the currently installed
            // default allocator is used.  The behavior is undefined unless
            // '0 <= inputLength'.

        ~my_DataHandler();
            // Destroy this data handler.

        // ...

        // MANIPULATORS
        int generateAlternate(my_DataStyle alternateStyle);
            // Generate data for this data handler in the specified
            // 'alternateStyle'.  Return 0 on success, and a non-zero value
            // otherwise.  If 'alternateStyle' is the same as the style of data
            // supplied at construction, this method returns 0 with no effect.

        // ...
    };
//..
// Next, we show the definition of the 'my_DataHandler' constructor:
//..
    my_DataHandler::my_DataHandler(const char       *input,
                                   int               inputLength,
                                   my_DataStyle      inputStyle,
                                   bslma::Allocator *basicAllocator)
    : d_inStyle(inputStyle)
    , d_inBuffer(0)
    , d_inCapacity(inputLength)
    , d_altStyle(e_STYLE_NONE)
    , d_altBuffer(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        BSLS_ASSERT(0 <= inputLength);

        void *tmp = d_allocator_p->allocate(inputLength);
        bsl::memcpy(tmp, input, inputLength);
        d_inBuffer = static_cast<char *>(tmp);
    }
//..
// Next, we show the definition of the 'generateAlternate' manipulator.  Note
// that we have deliberately introduced a bug in 'generateAlternate' to cause
// buffer overrun:
//..
    int my_DataHandler::generateAlternate(my_DataStyle alternateStyle)
    {
        if (alternateStyle == d_inStyle) {
            return 0;                                                 // RETURN
        }

        int altLength = my_DataTranslationUtil::outputSize(alternateStyle,
                                                           d_inStyle,
                                                           d_inCapacity);
        (void)altLength;

        // Oops!  Should have used 'altLength'.
        char *tmpAltBuffer = (char *)d_allocator_p->allocate(d_inCapacity);
        int rc = my_DataTranslationUtil::translate(tmpAltBuffer,
                                                   alternateStyle,
                                                   d_inBuffer,
                                                   d_inStyle);

        if (rc) {
            d_allocator_p->deallocate(tmpAltBuffer);
            return rc;                                                // RETURN
        }

        d_altStyle  = alternateStyle;
        d_altBuffer = tmpAltBuffer;

        return 0;
    }
//..

// ============================================================================
//          Additional Functionality Needed to Complete Usage Test Case
// ----------------------------------------------------------------------------

static JumpBuffer usageJumpBuffer;
static bool       invokedUsageSignalHandlerFlag = false;

void usageSignalHandler(int /* signal */)
    // Handle the specified 'signal'.  Note that this signal handler is for use
    // within the USAGE EXAMPLE only.
{
    invokedUsageSignalHandlerFlag = true;

#ifdef BSLS_PLATFORM_OS_WINDOWS

    longjmp(usageJumpBuffer, 1);

#else

    siglongjmp(usageJumpBuffer, 1);

#endif
}

int my_DataTranslationUtil::outputSize(my_DataStyle outputStyle,
                                       my_DataStyle inputStyle,
                                       int          inputLength)
{
    (void)outputStyle;
    (void)inputStyle;

    return 2 * inputLength;
}

int my_DataTranslationUtil::translate(char         *output,
                                      my_DataStyle  outputStyle,
                                      const char   *input,
                                      my_DataStyle  inputStyle)
{
    (void)outputStyle;
    (void)inputStyle;

    // Write twice as much content to 'output' as is contained in 'input'.

    while (*input != '@') {
        *output++ = *input++;
        *output++ = 'X';
    }

    *output = '@';

    return 0;
}

my_DataHandler::~my_DataHandler()
{
    d_allocator_p->deallocate(d_inBuffer);
    d_allocator_p->deallocate(d_altBuffer);
}

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

#ifdef BSLS_PLATFORM_OS_WINDOWS
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    const int pageSize = static_cast<int>(info.dwPageSize);
#else
    const int pageSize = static_cast<int>(sysconf(_SC_PAGESIZE));
#endif

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Start Usage example augmentation.
    ASSERT(false == invokedUsageSignalHandlerFlag);

#ifdef BSLS_PLATFORM_OS_WINDOWS

    signal(SIGSEGV, usageSignalHandler);

#else

    struct sigaction sa;
    sa.sa_handler = usageSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGBUS,  &sa, 0);

#endif
// End Usage example augmentation.

// Next, we define some data (in 'e_STYLE_A'):
//..
    const char *input = "AAAAAAAAAAAAAAA@";  // data always terminated with '@'
//..
// Then, we define a 'my_DataHandler' object, 'handler', to process that data:
//..
//  my_DataHandler handler(input, 16, e_STYLE_A);
//..
// Note that our 'handler' object uses the default allocator.
//
// Next, we request that an alternate data style, 'e_STYLE_AA', be generated by
// 'handler'.  Unfortunately, data in style 'e_STYLE_AA' is twice as large as
// that in style 'e_STYLE_A' making it a virtual certainty that the program
// will crash due to the insufficiently sized buffer that is allocated in the
// 'generateAlternate' method to accommodate the 'e_STYLE_AA' data:
//..
//  int rc = handler.generateAlternate(e_STYLE_AA);
//  if (!rc) {
//      // use data in alternate style
//  }
//..
// Suppose that after performing a brief post-mortem on the resulting core
// file, we strongly suspect that a buffer overrun is the root cause, but the
// program crashed in a context far removed from that of the source of the
// problem (which is often the case with buffer overrun issues).
//
// Consequently, we modify the code to supply a guarding allocator to the
// 'handler' object, then rebuild and rerun the program.  We have configured
// the guarding allocator (below) to place guard pages *after* user blocks.
// Note that 'e_AFTER_USER_BLOCK' is the default, so it need not be specified
// at construction as we have (pedantically) done here:
//..
    typedef bdlma::GuardingAllocator GA;
    GA guard(GA::e_AFTER_USER_BLOCK);

    my_DataHandler handler(input, 16, e_STYLE_A, &guard);

// Start Usage example augmentation.
#ifdef BSLS_PLATFORM_OS_WINDOWS

    if (0 == setjmp(usageJumpBuffer)) {

#else

    if (0 == sigsetjmp(usageJumpBuffer, 1)) {

#endif

    int rc = handler.generateAlternate(e_STYLE_AA);
    if (!rc) {
        // use data in alternate style
    }

    }
    else {
        ASSERT(true == invokedUsageSignalHandlerFlag);
        if (veryVerbose)
            cout << "Signal handler invoked from USAGE as expected." << endl;
    }
// End Usage example augmentation.

//..
// With a guarding allocator now in place, a memory fault is triggered when a
// guard page is overwritten as a result of the buffer overrun bug.  Hence, the
// program will dump core in a context that is more proximate to the buggy
// code, resulting in a core file that will be more amenable to revealing the
// issue when analyzed in a debugger.

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCURRENCY
        //   Ensure that 'allocate' and 'deallocate' are thread-safe.
        //
        // Concerns:
        //: 1 That 'allocate' and 'deallocate' are thread-safe.
        //
        // Plan:
        //: 1 Create two 'bdlma::GuardingAllocator' objects, 'mX' and 'mY',
        //:   configured using 'e_AFTER_USER_BLOCK' and 'e_BEFORE_USER_BLOCK',
        //:   respectively.
        //:
        //: 2 Within a loop, create three threads that iterate a specified
        //:   number of times and that perform a different sequence of
        //:   allocation and deallocation operations on the two allocators from
        //:   P-1.  (C-1)
        //
        // Testing:
        //   CONCERN: The 'allocate' and 'deallocate' methods are thread-safe.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCURRENCY" << endl
                          << "===========" << endl;

        using namespace TestCase4;

        Obj mX(Obj::e_AFTER_USER_BLOCK);
        Obj mY(Obj::e_BEFORE_USER_BLOCK);

        const int NUM_TEST_ITERATIONS   =  10;
        const int NUM_THREAD_ITERATIONS = 100;

        ThreadInfo info = { NUM_THREAD_ITERATIONS, &mX, &mY };

        for (int ti = 0; ti < NUM_TEST_ITERATIONS; ++ti) {
            ThreadId id1 = createThread(&threadFunction1, &info);
            ThreadId id2 = createThread(&threadFunction2, &info);
            ThreadId id3 = createThread(&threadFunction3, &info);

            joinThread(id1);
            joinThread(id2);
            joinThread(id3);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ALLOCATE AND DEALLOCATE
        //   Ensure that 'allocate' and 'deallocate' work as expected,
        //   especially with respect to proper management of guard pages.
        //
        // Concerns:
        //: 1 Memory blocks returned by 'allocate' are obtained from a system
        //:   facility that allocates blocks of memory in multiples of the
        //:   system page size.
        //:
        //: 2 Memory blocks returned by 'allocate' are of at least the
        //:   requested size (in bytes).
        //:
        //: 3 Memory blocks returned by 'allocate' are maximally aligned.
        //:
        //: 4 Memory blocks returned by 'allocate' have an associated guard
        //:   page that is write-protected and properly oriented with respect
        //:   to the user block.
        //:
        //: 5 Calling 'allocate' with 0 returns 0 and has no effect.
        //:
        //: 6 'deallocate' returns memory back to the system facility.
        //:
        //: 7 'deallocate' un-protects the guard page associated with the
        //:   memory block.
        //:
        //: 8 Calling 'deallocate' with 0 has no effect.
        //:
        //: 9 There is no allocation from either the default or global
        //:   allocators.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' object and install it as the
        //:   current default allocator.
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   1 Specify a set of allocation request sizes.
        //:
        //: 3 For each value, 'LOC', in the 'GuardPageLocation' enumeration:
        //:   (C-1..4, 6..7, 9)
        //:
        //:   1 For each row 'R' (representing an allocation request size)
        //:     in the table described in P-2:
        //:
        //:     1 Use the default constructor and 'LOC' to create a modifiable
        //:       'Obj' 'mX'.
        //:
        //:     2 Perform an allocation request of the size specified by 'R'
        //:       (from P-3.1).
        //:
        //:     3 Verify that the returned memory address is non-null and
        //:       maximally aligned, and that the entire extent of the user
        //:       block can be overwritten.  (C-2..3)
        //:
        //:     4 Verify that a write-protected guard page is positioned
        //:       appropriately with respect to the user block.  (C-4)
        //:
        //:     5 As a white-box test, verify that the unused (unprotected)
        //:       padding can be overwritten.  (C-1)
        //:
        //:     6 Verify that no temporary memory is allocated from any
        //:       allocator.  (C-9)
        //:
        //:     7 Delallocate the memory block from P-3.  (C-6..7)
        //:
        //: 4 Repeat P-3, except this time make all of the allocation requests
        //:   from P-2 without any intervening deallocations; verify that the
        //:   guard pages of earlier allocations are not affected.
        //:
        //: 5 Perform a separate brute-force test to verify that an allocation
        //:   request for 0 bytes returns 0 and has no effect on any allocator.
        //:   (C-5)
        //:
        //: 6 Perform a separate brute-force test to verify that deallocation
        //:   of 0 has no effect.  (C-8)
        //
        // Testing:
        //   void *allocate(size_type size);
        //   void deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ALLOCATE AND DEALLOCATE" << endl
                          << "=======================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        const int DATA[] = {
            1, 2, 3, 4, 7, 8, 15, 16, 31, 32, 63, 64, 127, 128,
            pageSize - 16, pageSize - 15, pageSize - 8, pageSize - 7,
            pageSize - 1, pageSize, pageSize + 1,
            pageSize + 7, pageSize + 8, pageSize + 15, pageSize + 16,
            3 * pageSize + 97
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the ctor argument
            Enum location;

            if ('a' == CONFIG) {
                location = Obj::e_AFTER_USER_BLOCK;
            }
            else {
                location = Obj::e_BEFORE_USER_BLOCK;
            }

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int SIZE = DATA[ti];

                if (veryVerbose) { P_(CONFIG); P(SIZE); }

                Obj mX(location);

                void *p = mX.allocate(SIZE);
                LOOP2_ASSERT(CONFIG, p, p);
                LOOP2_ASSERT(CONFIG, da.numBlocksTotal(),
                             0 == da.numBlocksTotal());

                // Verify maximal alignment.

                typedef bsls::AlignmentUtil U;
                const int offset = U::calculateAlignmentOffset(
                                                        p,
                                                        U::BSLS_MAX_ALIGNMENT);
                LOOP2_ASSERT(CONFIG, ti, 0 == offset);

                // Overwrite user block.

                bsl::memset(p, 0xff, SIZE);

                // Verify guard page is protected.

                assertGuardPageIsProtected(p, location, pageSize);

                // White-box testing: overwrite (unprotected) padding.

                overwritePadding(p, SIZE, location, pageSize);

                mX.deallocate(p);

                LOOP2_ASSERT(CONFIG, da.numBlocksTotal(),
                             0 == da.numBlocksTotal());
            }

            {
                Obj mX(location);

                void *p[NUM_DATA];

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int SIZE = DATA[ti];

                    if (veryVerbose) { P_(CONFIG); P(SIZE); }

                    p[ti] = mX.allocate(SIZE);
                    LOOP2_ASSERT(CONFIG, p[ti], p[ti]);
                    LOOP2_ASSERT(CONFIG, da.numBlocksTotal(),
                                 0 == da.numBlocksTotal());

                    // Verify maximal alignment.

                    typedef bsls::AlignmentUtil U;
                    const int offset = U::calculateAlignmentOffset(
                                                        p[ti],
                                                        U::BSLS_MAX_ALIGNMENT);
                    LOOP2_ASSERT(CONFIG, ti, 0 == offset);

                    // Overwrite user block.

                    bsl::memset(p[ti], 0xff, SIZE);

                    // Verify guard page is protected.

                    assertGuardPageIsProtected(p[ti], location, pageSize);

                    // White-box testing: overwrite (unprotected) padding.

                    overwritePadding(p[ti], SIZE, location, pageSize);
                }

                // Verify guard pages are still protected.

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    LOOP2_ASSERT(CONFIG, ti, p[ti]);

                    assertGuardPageIsProtected(p[ti], location, pageSize);
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    mX.deallocate(p[ti]);
                }
            }
        }

        if (verbose) cout << "\nTesting 'allocate(0)'." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            {
                Obj mX(Obj::e_AFTER_USER_BLOCK);

                void *p = mX.allocate(0);
                ASSERT(0 == p);
            }

            {
                Obj mX(Obj::e_BEFORE_USER_BLOCK);

                void *p = mX.allocate(0);
                ASSERT(0 == p);
            }

            ASSERT(0 == da.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting 'deallocate(0)'." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            {
                Obj mX(Obj::e_AFTER_USER_BLOCK);

                mX.deallocate(0);

                void *p = mX.allocate(5);

                mX.deallocate(0);

                mX.deallocate(p);
            }

            {
                Obj mX(Obj::e_BEFORE_USER_BLOCK);

                mX.deallocate(0);

                void *p = mX.allocate(5);

                mX.deallocate(0);

                mX.deallocate(p);
            }

            ASSERT(0 == da.numBlocksTotal());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR & DTOR
        //   Ensure that we can use the constructor to create an object, that
        //   we can 'allocate' a memory block from the object having a guard
        //   page at the expected position with respect to the block, and that
        //   we can safely destroy the object.
        //
        // Concerns:
        //: 1 Objects created using the constructor are correctly configured
        //:   according to the argument (optionally) supplied at construction.
        //:
        //: 2 The default constructor allocates no memory.
        //:
        //: 3 There is no allocation from either the default or global
        //:   allocators.
        //:
        //: 4 The destructor has no effect on any outstanding allocated memory.
        //
        // Plan:
        //: 1 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   a 'GuardPageLocation' value, (b) passing 'e_AFTER_USER_BLOCK'
        //:   explicitly, and (c) passing 'e_BEFORE_USER_BLOCK' explicitly.
        //:   For each of these three iterations:  (C-1..3)
        //:
        //:   1 Create two 'bslma::TestAllocator' objects and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'mX', with its guard page location configured appropriately
        //:     (see P-1); use a distinct test allocator for the object's
        //:     footprint.
        //:
        //:   3 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-2)
        //:
        //:   4 Allocate a block from 'mX' and verify that the guard page is
        //:     correctly positioned with respect to the user block.  (C-1)
        //:
        //:   5 Verify that no temporary memory is allocated from any
        //:     allocator.  (C-3)
        //:
        //: 2 Perform a separate test to verify that the destructor has no
        //:   effect on any outstanding allocated memory.  (C-4)
        //
        // Testing:
        //   GuardingAllocator(GuardPageLocation l = e_AFTER_USER_BLOCK);
        //   ~GuardingAllocator();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR & DTOR" << endl
                                  << "===========" << endl;

        const int SIZE = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

        if (verbose) cout << "\nTesting constructor." << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the ctor argument

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj *objPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objPtr = new (fa) Obj(Obj::e_AFTER_USER_BLOCK);
              } break;
              case 'c': {
                objPtr = new (fa) Obj(Obj::e_BEFORE_USER_BLOCK);
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad guard config.");
              } break;
            }
            LOOP_ASSERT(CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr;

            // Verify no allocation from the default allocator.

            LOOP2_ASSERT(CONFIG, da.numBlocksTotal(),
                         0 == da.numBlocksTotal());

            // Verify we can allocate from the object and write to the returned
            // memory block.

            void *p = mX.allocate(SIZE);
            LOOP2_ASSERT(CONFIG, p, p);
            LOOP2_ASSERT(CONFIG, da.numBlocksTotal(),
                         0 == da.numBlocksTotal());

            // Overwrite user block.

            bsl::memset(p, 0xff, SIZE);

            Enum location;

            if ('a' == CONFIG || 'b' == CONFIG) {
                location = Obj::e_AFTER_USER_BLOCK;
            }
            else {
                location = Obj::e_BEFORE_USER_BLOCK;
            }

            // Verify guard page is protected.

            assertGuardPageIsProtected(p, location, pageSize);

            // White-box testing: overwrite (unprotected) padding.

            overwritePadding(p, SIZE, location, pageSize);

            // Deallocate to avoid memory leak.

            mX.deallocate(p);
            LOOP2_ASSERT(CONFIG, da.numBlocksTotal(),
                         0 == da.numBlocksTotal());

            // Destroy object and reclaim dynamically allocated footprint.

            fa.deleteObject(objPtr);

            // Verify all memory has been released.

            LOOP2_ASSERT(CONFIG, fa.numBlocksInUse(),
                         0 ==  fa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG, da.numBlocksTotal(),
                         0 == da.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            void *p = 0;

            {
                Obj mX(Obj::e_AFTER_USER_BLOCK);

                p = mX.allocate(SIZE);

                ASSERT(p);

                bsl::memset(p, 'x', SIZE);
            }

            ASSERT(p);

            for (int i = 0; i < SIZE; ++i) {
                ASSERT('x' == *(static_cast<const char *>(p) + i));
            }

            // Verify guard page is still protected.

            assertGuardPageIsProtected(p, Obj::e_AFTER_USER_BLOCK, pageSize);

            void *q = 0;

            {
                Obj mX(Obj::e_BEFORE_USER_BLOCK);

                q = mX.allocate(SIZE);

                ASSERT(q);

                bsl::memset(q, 'x', SIZE);
            }

            ASSERT(q);

            for (int i = 0; i < SIZE; ++i) {
                ASSERT('x' == *(static_cast<const char *>(q) + i));
            }

            // Verify guard page is still protected.

            assertGuardPageIsProtected(q, Obj::e_BEFORE_USER_BLOCK, pageSize);

            ASSERT(0 == da.numBlocksTotal());

            // Note: will produce memory leak in Purify.
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:    testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'mX' using 'e_AFTER_USER_BLOCK'.
        //: 2 Allocate a block 'bx' from 'mX' and overwrite the block.
        //: 3 Write to the guard page following 'bx'.
        //: 4 Deallocate block 'bx'.
        //: 5 Allow 'mX' to go out of scope.
        //: 6 Create an object 'mY' using 'e_BEFORE_USER_BLOCK'.
        //: 7 Allocate a block 'by' from 'mY' and overwrite the block.
        //: 8 Write to the guard page preceding 'by'.
        //: 9 Deallocate block 'by'.
        //:10 Allow 'mY' to go out of scope.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const int SIZE = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

        if (verbose) cout << "\nTesting 'e_AFTER_USER_BLOCK'." << endl;
        {
            if (veryVerbose) cout <<
                "\tP-1: Create an object 'mX' using 'e_AFTER_USER_BLOCK'."
                << endl;

            Obj mX(Obj::e_AFTER_USER_BLOCK);

            if (veryVerbose) cout <<
              "\tP-2: Allocate a block 'bx' from 'mX' and overwrite the block."
                 << endl;

            void *p = mX.allocate(SIZE);  ASSERT(p);
            bsl::memset(p, 0xff, SIZE);

            if (veryVerbose) cout <<
                "\tP-3: Write to the guard page following 'bx'." << endl;

            ASSERT(causesMemoryFault(p, pageSize / 2, 'x'));

            if (veryVerbose) cout << "\tP-4: Deallocate block 'bx'." << endl;

            mX.deallocate(p);

            if (veryVerbose) cout << "\tP-5: Allow 'mX' to go out of scope."
                                  << endl;
        }

        if (verbose) cout << "\nTesting 'e_BEFORE_USER_BLOCK'." << endl;
        {
            if (veryVerbose) cout <<
                "\tP-6: Create an object 'mY' using 'e_BEFORE_USER_BLOCK'."
                << endl;

            Obj mY(Obj::e_BEFORE_USER_BLOCK);

            if (veryVerbose) cout <<
              "\tP-7: Allocate a block 'by' from 'mY' and overwrite the block."
                << endl;

            void *p = mY.allocate(SIZE);  ASSERT(p);
            bsl::memset(p, 0xff, SIZE);

            if (veryVerbose) cout <<
                "\tP-8: Write to the guard page preceding 'by'." << endl;

            ASSERT(causesMemoryFault(p, -(pageSize / 2), 'x'));

            if (veryVerbose) cout << "\tP-9: Deallocate block 'by'." << endl;

            mY.deallocate(p);

            if (veryVerbose) cout << "\tP-10: Allow 'mY' to go out of scope."
                                  << endl;
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
