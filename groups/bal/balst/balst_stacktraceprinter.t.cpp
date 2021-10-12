// balst_stacktraceprinter.t.cpp                                      -*-C++-*-

#include <balst_stacktraceprinter.h>

#include <balst_stacktraceprintutil.h>

//#include <ball_log.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)

#pragma optimize("", off)

#endif

#if !defined(BSLS_PLATFORM_OS_CYGWIN)

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                  TEST PLAN
//
// A major concern in this test driver is that we need to prevent optimizers
// from inlining or chaining calls, which will distort the stack trace
// obtained, so we employ 'bslim::TestUtil::makeFunctionCallNonInline' to do an
// identity transform on function pointers that the compiler cannot understand,
// and then do calls through those pointers, which prevents inlining.  To
// prevent chaining, we make sure that functions do things after calls.
//-----------------------------------------------------------------------------
// CLASS METHOD
// [ 2] Obj(int, bool, int);
// [ 2] ostream& operator<<(ostream&, const Obj&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                    GLOBAL HELPER #DEFINES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//          GLOBAL HELPER TYPES, CLASSES, and CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balst::StackTracePrinter   Obj;
typedef balst::StackTracePrintUtil Util;
typedef bsls::Types::UintPtr       UintPtr;
typedef bsls::Types::IntPtr        IntPtr;

#if defined(BSLS_PLATFORM_OS_WINDOWS) && !defined(BDE_BUILD_TARGET_DBG)
enum { e_NO_SYMBOLS = 1 };
#else
enum { e_NO_SYMBOLS = 0 };
#endif

// ============================================================================
//                    GLOBAL HELPER VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;
static int veryVeryVeryVerbose;

const bsl::size_t npos = bsl::string::npos;

template <class TYPE>
TYPE foilOptimizer(const TYPE funcPtr)
    // The function just returns 'funcPtr', but only after putting it through a
    // transform that the optimizer can't possibly understand that leaves it
    // with its original value.  'TYPE' is expected to be a function pointer
    // type.
    //
    // Note that it's still necessary to put a lot of the routines through
    // contortions to avoid the optimizer optimizing tail calls as jumps.
{
    TYPE ret, ret2 = funcPtr;

    UintPtr u = (UintPtr) funcPtr;

    const int loopGuard  = 0x8edf1000;    // garbage with a lot of trailing
                                          // 0's.
    const int toggleMask = 0xa72c3dca;    // pure garbage

    UintPtr u2 = u;
    for (int i = 0; !(i & loopGuard); ++i) {
        u ^= (i & toggleMask);
    }

    ret = (TYPE) u;

    // That previous loop toggled all the bits in 'u' that it touched an even
    // number of times, so 'ret == ret2', but I'm pretty sure the optimizer
    // can't figure that out.

    ASSERT(  u2 ==   u);
    ASSERT(ret2 == ret);

    return ret;
}
                                // ------
                                // case 2
                                // ------

namespace CASE_2 {

struct ConfigurationOptions {
    // DATA
    int  d_numDefaults;
    int  d_maxFrames;
    bool d_demanglingPreferredFlag;
    int  d_additionalIgnoreFrames;

    // CREATORS
    explicit
    ConfigurationOptions(int  numDefaults = 0,
                         int  maxFrames = -1,
                         bool demanglingPreferredFlag = true,
                         int  additionalIgnoreFrames = 0)
    : d_numDefaults(numDefaults)
    , d_maxFrames(maxFrames)
    , d_demanglingPreferredFlag(demanglingPreferredFlag)
    , d_additionalIgnoreFrames(additionalIgnoreFrames)
    {
        if (numDefaults >= 1) {
            ASSERT(d_additionalIgnoreFrames == 0);

            if (numDefaults >= 2) {
                ASSERT(d_demanglingPreferredFlag == true);

                if (numDefaults >= 3) {
                    ASSERT(d_maxFrames == -1);
                }
            }
        }
    }
};

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const ConfigurationOptions& options)
    // output all the variables in the specified 'options' to the specified
    // 'stream'.  Return 'stream'.
{
    stream << "num defaults: "         << options.d_numDefaults <<
              "   max frames: "        << options.d_maxFrames <<
              "   demangle: "          << options.d_demanglingPreferredFlag <<
              "   add ignore frames: " << options.d_additionalIgnoreFrames;

    return stream;
}

bool hasDemangling(const bsl::string& st)
    // Return 'true' if stack trace 'st' has demangled symbols and 'false'
    // otherwise.
{
    const bsl::size_t posColons = st.find("::");
    const bool ret = npos != posColons;
    const bsl::size_t posPlus   = st.find('+');
    if (ret) {
        ASSERTV(st, npos != posPlus);
        ASSERT(posColons < posPlus);
    }

    return ret;
}

int recurseAndStackTrace(bsl::ostream&               printerStream,
                         bsl::ostream&               utilStream,
                         const ConfigurationOptions& options,
                         int                         callDepth)
    // Recurse until the specified 'callDepth' reaches 5, then perform a stack
    // trace to the specified 'stream'.  After recursing, use 'Obj' to do the
    // stack trace to 'printerStream, and 'Util::printStackTrace' to do a stack
    // trace to 'utilStream'.  Allow the specified 'numDefaults' arguments to
    // default.  Pass the specified 'maxFrames', 'demangle', and 'addIgnore' to
    // the corresponding arguments of the 'Obj' c'tor or
    // 'Util::printStackTrace'.  The behavior is undefined unless
    // 'options.d_numDefaults' is in the range '[ 0 .. 3 ]'.
{
    const int  numDefaults = options.d_numDefaults;
    const int  maxFrames   = options.d_maxFrames;
    const bool demangle    = options.d_demanglingPreferredFlag;
    const int  addIgnore   = options.d_additionalIgnoreFrames;

    BSLS_ASSERT(0 <= numDefaults);
    BSLS_ASSERT(numDefaults <= 3);

    int ret = 0;

    if (0 < --callDepth) {
        ret = (*bslim::TestUtil::makeFunctionCallNonInline(
                                                       &recurseAndStackTrace))(
                                                                 printerStream,
                                                                 utilStream,
                                                                 options,
                                                                 callDepth);
        ASSERT(0 == ret);
    }
    else {
        bsl::ostream& osA =
                         0 == numDefaults
                       ? (printerStream << Obj(maxFrames, demangle, addIgnore))
                       : 1 == numDefaults
                       ? (printerStream << Obj(maxFrames, demangle))
                       : 2 == numDefaults
                       ? (printerStream << Obj(maxFrames))
                       : (printerStream << Obj());
        ret |= &osA != &printerStream;

        utilStream << endl;
        bsl::ostream& osB = 0 == numDefaults
                          ? Util::printStackTrace(utilStream,
                                                  maxFrames,
                                                  demangle,
                                                  addIgnore)
                          : 1 == numDefaults
                          ? Util::printStackTrace(utilStream,
                                                  maxFrames,
                                                  demangle)
                          : 2 == numDefaults
                          ? Util::printStackTrace(utilStream,
                                                  maxFrames)
                          : Util::printStackTrace(utilStream);
        ret |= &osB != &utilStream;
    }

    // Do some random things to prevent chaining, but if no asserts have
    // failed, be sure to return 0.

    if (ret) {
        // Only happens on failure.

        BSLS_ASSERT(0 == ret && "stream returned didn't match passed");

        printerStream << endl;
    }

    callDepth += 2;

    BSLS_ASSERT(0 < callDepth);

    return ret || testStatus || 10 < callDepth ? testStatus + callDepth : 0;
}

bsl::string skipFirstFrame(const bsl::string& str)
    // Return the specified stack trace 'str' without the first 2 lines of it.
    // The behavior is undefined unless 'str' contains at least 2 '\n's.
{
    bsl::size_t pos = str.find('\n');
    if (npos == pos) {
        return bsl::string();                                         // RETURN
    }
    ++pos;

    pos = str.find('\n', pos);
    if (npos == pos) {
        return bsl::string();                                         // RETURN
    }
    ++pos;

    return str.substr(pos);
}

void testOptions(int LINE, const ConfigurationOptions& options)
{
    const int  MAX_FRAMES = -1 == options.d_maxFrames
                          ? 1024
                          : options.d_maxFrames;
    const bool DEMANGLE   = options.d_demanglingPreferredFlag;
    const int  ADD_IGNORE = options.d_additionalIgnoreFrames;

    // At most one arg has a non-default value at a time.

    ASSERTV(LINE, options, 1 >=
              ((-1 != options.d_maxFrames) + (!DEMANGLE) + (ADD_IGNORE != 0)));

    bsl::ostringstream ossPrinter, ossUtil;
    (*bslim::TestUtil::makeFunctionCallNonInline(
                                           &recurseAndStackTrace))(ossPrinter,
                                                                   ossUtil,
                                                                   options,
                                                                   5);
    const bsl::string& printerResult = ossPrinter.str();
    const bsl::string& utilResult    = ossUtil.   str();

    // Everything after the first line should match.

    const bsl::string& printerSkipFirst = skipFirstFrame(printerResult);
    const bsl::string& utilSkipFirst    = skipFirstFrame(utilResult);

    ASSERTV(LINE, options, printerSkipFirst, utilSkipFirst,
                                            printerSkipFirst == utilSkipFirst);

    // Max frames arg was obeyed
    // - - - - - - - - - - - - -

    const IntPtr newlineCount = bsl::count(printerResult.begin(),
                                           printerResult.end(),
                                           '\n');
    ASSERTV(LINE, newlineCount <= MAX_FRAMES + 1);
    if (MAX_FRAMES <= 7) {
        ASSERTV(LINE, newlineCount == MAX_FRAMES + 1);
    }

    if (e_NO_SYMBOLS) {
        return;                                                       // RETURN
    }

    if (0 == ADD_IGNORE) {
        ASSERTV(LINE, printerResult, (MAX_FRAMES < 6) ==
                                  (npos == printerResult.find("testOptions")));
    }

    // Demanging flag was obeyed
    // - - - - - - - - - - - - -

    if (0 < MAX_FRAMES && ADD_IGNORE < 6) {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        // Always demangles

        ASSERTV(LINE, DEMANGLE, printerResult, hasDemangling(printerResult));
#else
        ASSERTV(LINE, DEMANGLE, printerResult,
                                     DEMANGLE == hasDemangling(printerResult));
#endif
    }

    // Add ignore arg was obeyed
    // - - - - - - - - - - - - -

    // If '1 <= ADD_INGORE', the first frame, which will not match, is not
    // printed.  Everything else is expected to match.

    if (1 <= ADD_IGNORE) {
        ASSERTV(LINE, utilResult, printerResult, utilResult == printerResult);
    }

    // If the first frame is visible, it will not match (different offsets).

    if (0 == ADD_IGNORE && 0 < MAX_FRAMES) {
        ASSERTV(LINE, utilResult, printerResult, utilResult != printerResult);
    }

    // Add ignore hides functions as expected

    if (6 <= MAX_FRAMES) {
        ASSERTV(LINE, ADD_IGNORE, printerResult, (5 <= ADD_IGNORE) ==
                         (npos == printerResult.find("recurseAndStackTrace")));
        ASSERTV(LINE, ADD_IGNORE, printerResult, (6 <= ADD_IGNORE) ==
                                  (npos == printerResult.find("testOptions")));
    }
}

}  // close namespace CASE_2

                                // ------
                                // case 1
                                // ------

namespace CASE_1 {

bool called = false;

void top(bsl::ostream& stream)
    // Perform a stack trace to the specified 'stream'.
{
    called = true;

    stream << Obj();
}

int under(bsl::ostream& stream)
    // Call the function 'top' in such a way that it can't be inlined.
{
    // still attempting to thwart optimizer -- all this does is call 'top' a
    // bunch of times.

    called = false;

    int i = 0;
    for (; i < 100; ++i) {
        if ((i & 2) && (i & 8)) {
            (*bslim::TestUtil::makeFunctionCallNonInline(&top))(stream);

            break;
        }
    }

    ASSERT(called);

    return i;
}

}  // close namespace CASE_1

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

                                    // -------
                                    // Usage 1
                                    // -------

//#define U_BALL_LOG_FATAL if (verbose) BALL_LOG_FATAL
#  define U_BALL_LOG_FATAL if (verbose) cout

///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Streaming to BALL
/// - - - - - - - - - - - - - -
// First, we define a recursive function 'recurseAndPrintStack' that recurses 4
// times, then calls '<< StackTracePrinter()' to obtain a stack trace and print
// it to 'BALL_LOG_FATAL':
//..
//  BALL_LOG_SET_NAMESPACE_CATEGORY("MY.CATEGORY");

    void recurseAndStreamStackDefault()
        // Recurse 4 times and print a stack trace to 'BALL_LOG_FATAL'.
    {
        static int recurseCount = 0;

        if (recurseCount++ < 4) {
            recurseAndStreamStackDefault();
        }
        else {
            U_BALL_LOG_FATAL << balst::StackTracePrinter();
        }
    }
//..
// which, on Linux, produces the output:
//..
//  02SEP2021_21:55:58.619290 21325 140602555295616 FATAL /bb/.../balst_stacktr
//  aceprinter.t.cpp 733 UNINITIALIZED_LOGGER_MANAGER
//  (0): recurseAndStreamStackDefault()+0xcf at 0x408acc source:balst_stacktrac
//  eprinter.t.cpp:733 in balst_stacktraceprinter.t
//  (1): recurseAndStreamStackDefault()+0x2a at 0x408a27 source:balst_stacktrac
//  eprinter.t.cpp:735 in balst_stacktraceprinter.t
//  (2): recurseAndStreamStackDefault()+0x2a at 0x408a27 source:balst_stacktrac
//  eprinter.t.cpp:735 in balst_stacktraceprinter.t
//  (3): recurseAndStreamStackDefault()+0x2a at 0x408a27 source:balst_stacktrac
//  eprinter.t.cpp:735 in balst_stacktraceprinter.t
//  (4): recurseAndStreamStackDefault()+0x2a at 0x408a27 source:balst_stacktrac
//  eprinter.t.cpp:735 in balst_stacktraceprinter.t
//  (5): main+0x1a7 at 0x408ff8 source:balst_stacktraceprinter.t.cpp:901 in bal
//  st_stacktraceprinter.t
//  (6): __libc_start_main+0xf5 at 0x7fe0943de495 in /lib64/libc.so.6
//  (7): --unknown-- at 0x4074c5 in balst_stacktraceprinter.t
//..
// Note that long lines of output here have been hand-wrapped to fit into
// comments in this 79-column source file.  Also note that if the full path of
// the executable or library is too long, only the basename will be displayed
// by the facility, while if it is short, then the full path will be displayed.
//
// Then we define a similar recursive function, except that when we construct
// the 'StackTracePrinter' object, we pass 2 to the 'maxFrames' argument,
// indicating, for some reason, that we want to see only the top two stack
// frames:
//..
    void recurseAndStreamStackMaxFrames2()
        // Recurse 4 times and print a stack trace to 'BALL_LOG_FATAL'.
    {
        static int recurseCount = 0;

        if (recurseCount++ < 4) {
            recurseAndStreamStackMaxFrames2();
        }
        else {
            U_BALL_LOG_FATAL << balst::StackTracePrinter(2);
        }
    }
//..
// which produces the output:
//..
//  02SEP2021_21:55:58.624623 21325 140602555295616 FATAL /bb/.../balst_stacktr
//  aceprinter.t.cpp 773 UNINITIALIZED_LOGGER_MANAGER
//  (0): recurseAndStreamStackMaxFrames2()+0xcf at 0x408be1 source:balst_stackt
//  raceprinter.t.cpp:773 in balst_stacktraceprinter.t
//  (1): recurseAndStreamStackMaxFrames2()+0x2a at 0x408b3c source:balst_stackt
//  raceprinter.t.cpp:775 in balst_stacktraceprinter.t
//..
// Now, we define another similar recursive function, except that when we
// construct the 'StackTracePrinter' object, we default 'maxFrames' to a large
// value by passing it -1, and turn off demangling by passing 'false' to the
// 'damanglingPreferredFlag' argument:
//..
    void recurseAndStreamStackNoDemangle()
        // Recurse 4 times and print a stack trace to 'BALL_LOG_FATAL'.
    {
        static int recurseCount = 0;

        if (recurseCount++ < 4) {
            recurseAndStreamStackNoDemangle();
        }
        else {
            U_BALL_LOG_FATAL << balst::StackTracePrinter(-1, false);
        }
    }
//..
// which produces the output:
//..
//  02SEP2021_21:55:58.636414 21325 140602555295616 FATAL /bb/.../balst_stacktr
//  aceprinter.t.cpp 798 UNINITIALIZED_LOGGER_MANAGER
//  (0): _Z31recurseAndStreamStackNoDemanglev+0xcf at 0x408cf6 source:balst_sta
//  cktraceprinter.t.cpp:798 in balst_stacktraceprinter.t
//  (1): _Z31recurseAndStreamStackNoDemanglev+0x2a at 0x408c51 source:balst_sta
//  cktraceprinter.t.cpp:800 in balst_stacktraceprinter.t
//  (2): _Z31recurseAndStreamStackNoDemanglev+0x2a at 0x408c51 source:balst_sta
//  cktraceprinter.t.cpp:800 in balst_stacktraceprinter.t
//  (3): _Z31recurseAndStreamStackNoDemanglev+0x2a at 0x408c51 source:balst_sta
//  cktraceprinter.t.cpp:800 in balst_stacktraceprinter.t
//  (4): _Z31recurseAndStreamStackNoDemanglev+0x2a at 0x408c51 source:balst_sta
//  cktraceprinter.t.cpp:800 in balst_stacktraceprinter.t
//  (5): main+0x1b1 at 0x409002 source:balst_stacktraceprinter.t.cpp:903 in bal
//  st_stacktraceprinter.t
//  (6): __libc_start_main+0xf5 at 0x7fe0943de495 in /lib64/libc.so.6
//  (7): --unknown-- at 0x4074c5 in balst_stacktraceprinter.t
//..
// Finally, we define another similar recursive function, except that we pass
// default values to the first 2 arguments of the 'StackTracePrinter' and pass
// 5 to the third 'additionalIgnoreFrames' argument.  This indicates a number
// of frames from the top of the stack to be ignored, which may be desired if
// the caller wants to do the streaming from within their own stack trace
// facility, in which case the top couple of frames would be within that stack
// trace facility, and unwanted and distracting for clients of that facility:
//..
    void recurseAndStreamStackAddIgnore5()
        // Recurse 4 times and print a stack trace to 'BALL_LOG_FATAL'.
    {
        static int recurseCount = 0;

        if (recurseCount++ < 4) {
            recurseAndStreamStackAddIgnore5();
        }
        else {
            U_BALL_LOG_FATAL << balst::StackTracePrinter(-1, true, 5);
        }
    }
//..
// which produces the output:
//..
//  02SEP2021_21:55:58.647501 21325 140602555295616 FATAL /bb/.../balst_stacktr
//  aceprinter.t.cpp 836 UNINITIALIZED_LOGGER_MANAGER
//  (0): main+0x1b6 at 0x409007 source:balst_stacktraceprinter.t.cpp:904 in bal
//  st_stacktraceprinter.t
//  (1): __libc_start_main+0xf5 at 0x7fe0943de495 in /lib64/libc.so.6
//  (2): --unknown-- at 0x4074c5 in balst_stacktraceprinter.t
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

        recurseAndStreamStackDefault();
        recurseAndStreamStackMaxFrames2();
        recurseAndStreamStackNoDemangle();
        recurseAndStreamStackAddIgnore5();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // COMPARISON WITH STACKTRACEPRINTUTIL
        //
        // Concerns:
        //: 1 That output matches that from 'StackTracePrintUtil', except
        //:   starting with an initial '\n'.
        //:
        //: 2 That each argument controls the output accordingly.
        //
        // Plan:
        //: 1 Define a 'struct', 'ConfigurationOptions', which contains the
        //:   values for each of the arguments to be passed to the stack trace
        //:   printer, plus 'd_numDefaults', which is the number of arguments
        //:   to be defaulted by not being specified.
        //:
        //: 2 Define a function, 'recurseAndStackTrace' which will recurse a
        //:   specified number of times, and then stream two stack traces to
        //:   two streams passed to it, one with 'Obj()', and the other with
        //:   'StackTracePrintUtil::printStackTrace', to be checked and
        //:   compared.
        //:
        //: 3 Define a function, 'testOptions', which is passed a
        //:   'ConfigurationOptions' object and will call
        //:   'recurseAndStackTrace' with those options, and compare the two
        //:   stack traces and observe that the properties of the stack traces
        //:   match the arguments of 'ConfigurationOptions'.
        //:
        //: 4 Note that the two stack traces will not normally exactly match,
        //:   because the calls to the stack trace utilities will come from
        //:   different offsets and different line numbers in the top frame
        //:   recorded.  Write a function 'skipFirstFrame' which will remove
        //:   the first frame of the trace, and then compare the two stack
        //:   traces after that.  They should match.
        //:
        //: 5 Check that the 'maxFrames' argument was obeyed by counting the
        //:   number of '\n's in the output and whether the function
        //:   'testOptions' is displayed.
        //:
        //: 6 Check whether the demangling flag is obeyed by observing whether
        //:   the substring "::" occurs in the output (it is only to be found
        //:   in demangled symbols.  Note that demangling cannot be disabled
        //:   on Windows.
        //:
        //: 7 Check whether the 'additionalIgnoreFrames' argument is obeyed
        //:   by observing whether "recurseAndStackTrace" and "testOptions"
        //:   are seen in the trace.
        //
        // Testing:
        //   Obj(int, bool, int);
        //   ostream& operator<<(ostream&, const Obj&);
        // --------------------------------------------------------------------

        if (verbose) cout << "COMPARISON WITH STACKTRACEPRINTUTIL\n"
                             "===================================\n";

        namespace TC = CASE_2;

        typedef TC::ConfigurationOptions Options;

        if (verbose) cout << "Everything defaulting:\n";

        // numDefaults = 0: 'Obj(<default>, <default>, <default>)'

        TC::testOptions(L_, Options());    // Everything passed default.

        // numDefaults = 3: 'Obj()'

        TC::testOptions(L_, Options(3));   // All args defaulting

        if (verbose) cout << "Various values of 'maxFrames':\n";

        // numDefaults = 0: 'Obj(x, <default>, <default>)'

        TC::testOptions(L_, Options(0, -1));
        TC::testOptions(L_, Options(0, 0));
        TC::testOptions(L_, Options(0, 5));
        TC::testOptions(L_, Options(0, 6));
        TC::testOptions(L_, Options(0, 7));
        TC::testOptions(L_, Options(0, 20));

        // numDefaults = 2: 'Obj(x)'

        TC::testOptions(L_, Options(2, -1));
        TC::testOptions(L_, Options(2, 0));
        TC::testOptions(L_, Options(2, 5));
        TC::testOptions(L_, Options(2, 6));
        TC::testOptions(L_, Options(2, 7));
        TC::testOptions(L_, Options(2, 20));

        if (verbose) cout << "Disabling demangling\n";

        // numDefaults = 0: 'Obj(x, y, <default>)'

        TC::testOptions(L_, Options(0, -1, false));
        TC::testOptions(L_, Options(0, -1, true));

        // numDefaults = 1: 'Obj(x, y)'

        TC::testOptions(L_, Options(1, -1, false));
        TC::testOptions(L_, Options(1, -1, true));

        if (verbose) cout << "Additional Ignore frames\n";

        // numDefaults = 0: 'Obj(x, y, z)'

        TC::testOptions(L_, Options(0, -1, true, 0));
        TC::testOptions(L_, Options(0, -1, true, 1));
        TC::testOptions(L_, Options(0, -1, true, 5));
        TC::testOptions(L_, Options(0, -1, true, 6));
        TC::testOptions(L_, Options(0, -1, true, 7));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //:
        //: 2 That certain forms of undefined behavior are caught via asserts.
        //
        // Plan:
        //: 1 Several routines deep, do a stack trace to a string stream.
        //:
        //: 2 Check that the right sequence of routine names is present in
        //:   the string stream.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        namespace TC = CASE_1;

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        bsl::stringstream oss;

        int result = (*bslim::TestUtil::makeFunctionCallNonInline(&TC::under))(
                                                                          oss);
        ASSERT(result >= 6);
        const bsl::string& str = oss.str();

        bsl::vector<const char *> matches;
        matches.push_back("top");
        matches.push_back("under");
        matches.push_back("main");

        if (veryVerbose) {
            cout << str;
        }

        if (e_NO_SYMBOLS && npos == str.find("top")) {
            break;
        }

        bsl::size_t pos = 0, newPos;
        for (unsigned uu = 0; uu < matches.size(); ++uu, pos = newPos) {
            newPos = str.find(matches[uu], pos);
            ASSERT(npos != newPos);
            ASSERT(pos < newPos);
        }


        if (verbose) cout << "Negative Testing\n";
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Obj());
            ASSERT_PASS(Obj(-1, true, 0));
            ASSERT_PASS(Obj(1000, false, 10));
            ASSERT_FAIL(Obj(-2));
            ASSERT_FAIL(Obj(-1, true, -1));
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }

    return testStatus;
}

#else

int main()
{
    return -1;
}

#endif

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
