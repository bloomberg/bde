// balst_stacktraceprintutil.t.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceprintutil.h>

#include <balst_objectfileformat.h>
#include <balst_stackaddressutil.h>    // 'getStackAddresses(0,0)'

#include <bdlma_sequentialallocator.h>
#include <bdlb_string.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>

#ifndef BSLS_PLATFORM_OS_CYGWIN

#ifdef BSLS_PLATFORM_OS_WINDOWS
// for 'EnumWindows'

# pragma comment(lib, "user32.lib")
# include <windows.h>

# pragma optimize("", off)

#endif

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
// CLASS METHOD
// [ 2] bsl::ostream& printStackTrace(ostream& s, int max, bool demangle);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ 3] CONCERN: 'printStackTrace' works for routine in static library
// [ 4] CONCERN: 'printStackTrace' works for inline routine on stack

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I, J, K, M, X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" << #K << ": " << K << "\t" \
                    << #M << ": " << M << "\n"; aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

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

namespace {

typedef balst::StackTracePrintUtil           PrintUtil;
typedef balst::StackTracePrintUtil_Test      PrintUtilTest;

#if   defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
    enum { FORMAT_ELF = 1, FORMAT_WINDOWS = 0, FORMAT_DLADDR = 0 };

# if   defined(BSLS_PLATFORM_OS_SOLARIS)
    enum { PLAT_SUN=1, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=0 };
# elif defined(BSLS_PLATFORM_OS_LINUX)
    enum { PLAT_SUN=0, PLAT_LINUX=1, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=0 };
# elif defined(BSLS_PLATFORM_OS_HPUX)
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=1, PLAT_AIX=0, PLAT_WIN=0 };
# else
#   error unknown platform
# endif

#elif defined(BALST_OBJECTFILEFORMAT_RESOLVER_DLADDR)
    enum { FORMAT_ELF = 0, FORMAT_WINDOWS = 0, FORMAT_DLADDR = 1 };
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=0 };
#elif defined(BALST_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
    enum { FORMAT_ELF = 0, FORMAT_WINDOWS = 1, FORMAT_DLADDR = 0 };
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=1 };
#elif defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)
    enum { FORMAT_ELF = 0, FORMAT_WINDOWS = 0, FORMAT_DLADDR = 0 };
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=1, PLAT_WIN=0 };
#else
# error unknown object file format
#endif

#ifdef BDE_BUILD_TARGET_DBG
    enum { DEBUG_ON = 1 };
#else
    enum { DEBUG_ON = 0 };
#endif


#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BSLS_PLATFORM_CPU_64_BIT)
// On Windows, longs aren't big enough to hold pointers or 'size_t's

#define SIZE_T_CONTROL_STRING "%llx"
typedef long long unsigned int UintPtr;

#else

#define SIZE_T_CONTROL_STRING "%lx"
typedef long      unsigned int UintPtr;

#endif

typedef bsls::Types::IntPtr    IntPtr;

}  // close unnamed namespace

// ============================================================================
//                    GLOBAL HELPER VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;

bsl::ostream *out_p;    // pointer to either 'cout' or a dummy stringstream
                        // that is never output, depending on the value of
                        // 'verbose'.

static const bsl::size_t npos = bsl::string::npos;

static inline
bool problem()
{
    if (testStatus) {
        out_p = &cout;
        verbose = true;

        return true;                                                  // RETURN
    }

    return false;
}

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING

UintPtr foilOptimizer(const UintPtr u)
    // The function just returns 'u', but only after putting it through a
    // transform that the optimizer can't possibly understand that leaves it
    // with its original value.
{
    const int loopGuard  = 0x8edf0000;    // garbage with a lot of trailing
                                          // 0's.
    const int toggleMask = 0xa72c3dca;    // pure garbage

    UintPtr u2 = u;
    for (int i = 0; !(i & loopGuard); ++i) {
        u2 ^= (i & toggleMask);
    }

    // That previous loop toggled all the bits in 'u2' that it touched an even
    // number of times, so 'u2 == u', but I'm pretty sure the optimizer can't
    // figure that out.

    ASSERT(u == u2);

    return u2;
}

//-----------------------------------------------------------------------------

void checkOutput(const bsl::string&               str,
                 const bsl::vector<const char *>& matches)
    // check that the specified 'str' contains all the strings specified in the
    // vector 'matches' in order.  Note that 'matches' may be modified.
{
    bslma::TestAllocator localAllocator;
    bdlma::SequentialAllocator sa(&localAllocator);

    if (PLAT_WIN && !DEBUG_ON) {
        return;                                                       // RETURN
    }

    const size_t NPOS = bsl::string::npos;
    for (bsl::size_t vecI = 0, posN = 0; vecI < matches.size(); ++vecI) {
        bsl::size_t newPos = str.find(matches[vecI], posN);
        LOOP3_ASSERT(vecI, matches[vecI], str.substr(posN), NPOS != newPos);
        posN = NPOS != newPos ? newPos : posN;
    }

    if (problem()) {
        *out_p << str;
    }
}

                                // -------
                                // case 4
                                // -------

namespace CASE_4 {

// Pointer to be set to inline '&PrintUtil::forTestingOnlyDump'.

void top()
{
    typedef void (*PrintStackTraceToStringPtrType)(bsl::string *string);
    union {
        PrintStackTraceToStringPtrType d_funcPtr;
        UintPtr                        d_uintPtr;
    } testDumpUnion;

    testDumpUnion.d_funcPtr = &PrintUtilTest::printStackTraceToString;
    testDumpUnion.d_uintPtr = foilOptimizer(testDumpUnion.d_uintPtr);

    bslma::TestAllocator ta;
    bsl::string dump(&ta);
    (*testDumpUnion.d_funcPtr)(&dump);

    if (!FORMAT_ELF && !FORMAT_DLADDR && !FORMAT_WINDOWS && DEBUG_ON) {
        // Elf doesn't provide souce file names of global routines,
        // Dladdr never provides source file names for anything,
        // Windows doesn't provide the source file name for an inline routine.

        bsl::vector<const char *> matches(&ta);
        matches.push_back("balst");
        matches.push_back("StackTracePrintUtil_Test");
        matches.push_back("printStackTraceToString");
        matches.push_back(" source:balst_stacktraceprintutil.h");
        matches.push_back(" in balst_stacktraceprintutil.t");
        matches.push_back("\n");
        matches.push_back("top");
        matches.push_back(" source:balst_stacktraceprintutil.t.cpp");
        matches.push_back(" in balst_stacktraceprintutil.t");
        matches.push_back("\n");
        matches.push_back("main");
        matches.push_back(" source:balst_stacktraceprintutil.t.cpp");
        matches.push_back(" in balst_stacktraceprintutil.t");
        matches.push_back("\n");
        checkOutput(dump, matches);
    }
    else {
        bsl::vector<const char *> matches(&ta);
        matches.push_back("balst");
        matches.push_back("StackTracePrintUtil_Test");
        matches.push_back("printStackTraceToString");
        matches.push_back("\n");
        matches.push_back("CASE_4");
        matches.push_back("top");
        matches.push_back("\n");
        matches.push_back("main");
        matches.push_back("\n");
        checkOutput(dump, matches);
    }

    int lines = 0;
    for (bsl::size_t pos = 0; pos < dump.length(); ++lines, ++pos) {
        pos = dump.find('\n', pos);
        if (bsl::string::npos == pos) {
            break;
        }
        ASSERT('\n' == dump[pos]);
    }
    LOOP_ASSERT(lines, lines >= 3);

    if (verbose || problem()) {
        *out_p << dump;
    }
}

}  // close namespace CASE_4

                                // ------
                                // case 3
                                // ------

extern "C" {

#ifdef BSLS_PLATFORM_OS_WINDOWS

BOOL CALLBACK phonyEnumWindowsProc(HWND, LPARAM)
{
    if (!DEBUG_ON) {
        return FALSE;                                                 // RETURN
    }

    bslma::TestAllocator ta;
    bsl::stringstream ss(&ta);
    PrintUtil::printStackTrace(ss);
    bsl::string dump(&ta);
    {
        bslma::DefaultAllocatorGuard guard(&ta);
        dump = ss.str();
    }
    const bsl::size_t NPOS = bsl::string::npos;

    struct {
        int         d_line;
        bool        d_noNewlines;
        const char *d_match;
    } STRINGS[] = {
        { L_, false, "phonyEnumWindowsProc" },
        { L_, false, "EnumWindows" },
        { L_, true,  " in " },
        { L_, true,  ".DLL" },
        { L_, false, "main" } };
    enum { NUM_STRINGS = sizeof STRINGS / sizeof *STRINGS };

    const char *pc = dump.c_str();
    for (int i = 0; i < NUM_STRINGS; ++i) {
        int LINE          = STRINGS[i].d_line;
        bool NO_NEWLINES  = STRINGS[i].d_noNewlines;
        const char *MATCH = STRINGS[i].d_match;

        const char *matchPc = bdlb::String::strstrCaseless(pc,
                                                          bsl::strlen(pc),
                                                          MATCH,
                                                          bsl::strlen(MATCH));
        LOOP2_ASSERT(LINE, MATCH, matchPc);

        const char *newlinePc = bsl::strstr(pc, "\n");
        LOOP_ASSERT(LINE, newlinePc);
        if (NO_NEWLINES) {
            LOOP2_ASSERT(LINE, MATCH, matchPc < newlinePc);
        }

        pc = !matchPc ? "" : matchPc + bsl::strlen(MATCH);
    }

    if (verbose || problem()) {
        *out_p << dump;
    }

    return FALSE;
}

#else

static int phonyCompare(const void *, const void *)
{
    bslma::TestAllocator ta;
    bsl::stringstream ss(&ta);
    PrintUtil::printStackTrace(ss);
    bsl::string dump(&ta);
    {
        bslma::DefaultAllocatorGuard guard(&ta);
        dump = ss.str();
    }
    const bsl::size_t NPOS = bsl::string::npos;

    struct {
        int         d_line;
        bool        d_noNewlines;
        const char *d_match;
    } STRINGS[] = {
        { L_, false, "phonyCompare" },
        { L_, false, "qsort" },
        { L_, true,  " in " },
        { L_, true,  FORMAT_DLADDR ? "/libsystem_c" : "/libc." },
        { L_, false, "main" } };
    enum { NUM_STRINGS = sizeof STRINGS / sizeof *STRINGS };

    bsl::size_t pos = 0;
    for (int i = 0; i < NUM_STRINGS; ++i) {
        int LINE          = STRINGS[i].d_line;
        bool NO_NEWLINES  = STRINGS[i].d_noNewlines;
        const char *MATCH = STRINGS[i].d_match;

        bsl::size_t matchPos = dump.find(MATCH, pos);
        LOOP2_ASSERT(LINE, MATCH, NPOS != matchPos);

        if (NO_NEWLINES) {
            bsl::size_t newlinePos = dump.find("\n", pos);
            LOOP_ASSERT( LINE, NPOS != newlinePos);
            LOOP2_ASSERT(LINE, MATCH, matchPos < newlinePos);
        }

        pos = matchPos + bsl::strlen(MATCH);
    }

    if (verbose || problem()) {
        *out_p << dump;
    }

    return 0;
}

#endif

}


                                    // ------
                                    // case 2
                                    // ------

namespace CASE_2 {

bool calledTop = false;

static
int top()
{
    if (calledTop) return 9;                                          // RETURN
    calledTop = true;

    bslma::TestAllocator ta;
    bsl::vector<const char *> matches(&ta);
    matches.push_back("top");
    matches.push_back("\n");
    matches.push_back("highMiddle");
    matches.push_back("\n");
    matches.push_back("lowMiddle");
    matches.push_back("\n");
    matches.push_back("bottom");
    matches.push_back("\n");
    matches.push_back("main");
    matches.push_back("\n");

    bsl::stringstream os(&ta);
    balst::StackTracePrintUtil::printStackTrace(os);

    bsl::string str(&ta);
    {
        bslma::DefaultAllocatorGuard guard(&ta);

        // 'bsl::stringstream::str' may create temporaries if the string is
        // large

        str = os.str();
    }

    checkOutput(str, matches);

    if (verbose) {
        *out_p << str;
    }

    return 7 + !!verbose;
}

bool calledHighMiddle = false;

static
int highMiddle(int i)
{
    if (calledHighMiddle) return 40;                                  // RETURN
    calledHighMiddle = true;

    calledTop = false;

    i ^= ~i;            // effectively 'i = 0', but 'uses' value of 'i'
    for (; i < 40; ++i) {
        if (i & 16) {
            i += 5;
            ASSERT(top() >= 6);
        }
        else if (i & 8) {
            i += 7;
            ASSERT(top() >= 7);
        }
    }

    ASSERT(calledTop);

    return i;
}

bool calledLowMiddle = false;

int lowMiddle()
{
    if (calledLowMiddle) return 30;                                   // RETURN
    calledLowMiddle = true;

    calledHighMiddle = false;

    int i = 0;
    for (; i < 30; ++i) {
        if (i & 4) {
            i += 12;
            ASSERT(highMiddle(10) >= 40);
        }
        else if ((i & 2) && (i & 16)) {
            i += 5;
            ASSERT(highMiddle(10) >= 39);
        }
    }

    ASSERT(calledHighMiddle);

    return i;
}

static
int bottom()
{
    calledLowMiddle = false;

    int i = 0;
    for (; i < 20; ++i) {
        if (i & 8) {
            i += 7;
            ASSERT(lowMiddle() >= 30);
        }
        if ((i & 2) && (i & 4)) {
            i += 4;
            ASSERT(lowMiddle() >= 28);
        }
    }

    ASSERT(calledLowMiddle);

    return i;
}

// Make global ptrs to the static routines to prevent optimizer inlinng them.

int (*highMiddlePtr)(int) = &highMiddle;
int (*bottomPtr)()        = &bottom;

}  // close namespace CASE_2

                                // ------
                                // case 1
                                // ------

namespace CASE_1 {

bool called = false;

void top(bslma::Allocator *alloc)
{
    if (called) return;                                               // RETURN
    called = true;

    bsl::vector<const char *> matches(alloc);
    matches.push_back("top");
    matches.push_back("bottom");
    matches.push_back("main");

    {
        bsl::stringstream myStream(alloc);
        balst::StackTracePrintUtil::printStackTrace(myStream);
        bsl::string str(alloc);

        {
            bslma::DefaultAllocatorGuard guard(alloc);

            // 'bsl::stringstream::str' may create temporaries if the string is
            // large

            str = myStream.str();
        }
        checkOutput(str, matches);

        if (problem()) {
            *out_p << str;
        }
    }
}

int bottom(bslma::Allocator *alloc)
{
    // still attempting to thwart optimizer -- all this does is call 'top'
    // a bunch of times.

    called = false;

    int i = 0;
    for (; i < 0x20; ++i) {
        if ((i & 2) && (i & 4)) {
            i += 7;
            top(alloc);
        }
        else if ((i & 1) && (i & 8)) {
            i += 5;
            top(alloc);
        }
    }

    ASSERT(called);
    ASSERT(i >= 0x20);

    return i;
}

}  // close namespace CASE_1

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

                                    // -------
                                    // Usage 1
                                    // -------

// First, we define a recursive function 'recurseAndPrintStack' that recurses
// to the specified 'depth', then calls
// 'balst::StackTracePrintUtil::printStackTrace' to obtain a stack trace and
// print it to 'cout'.  When we call 'printStackTrace', neither of the optional
// arguments corresponding to 'maxFrames' or 'demanglingPreferredFlag' are
// supplied; 'maxFrames' defaults to at least 1024 (which is more than we
// need), and 'demanglingPreferredFlag' defaults to 'true'.

static
void recurseAndPrintStack(int *depth)
    // Recurse to the specified 'depth', then print out the stack trace to
    // 'cout'.
{
    if (--*depth > 0) {
        recurseAndPrintStack(depth);
    }
    else {
        balst::StackTracePrintUtil::printStackTrace(*out_p);
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a
                // loop.
}

// Then, we call 'recurseAndPrintStack' from the main program.

// Now, invoking the main program on AIX produces the following output:

// Finally, we observe the following about the above output to 'cout'.  Notice
// that as the actual output would write each stack trace frame all on a single
// line, and all the lines here were longer than 80 characters, it has been
// manually edited to wrap and fit neatly within 79 character lines.  Also note
// the program name is truncated to 32 characters in length.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test    = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose     = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // make sure the shared lib containing 'malloc' is loaded

    (void) bsl::malloc(100);

    // see if we can avoid calling 'malloc' from here on out

    bslma::TestAllocator ota;
    bdlma::SequentialAllocator ta(&ota);

    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    // 'dummyOstream' is a way of achieving the equivalent of opening /dev/null
    // that works on Windoze.

    bsl::stringstream dummyOstream(&ta);
    out_p = verbose ? &cout : &dummyOstream;

    switch (test) { case 0:
      case 5: {
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

        if (verbose) cout << "TEST OF USAGE EXAMPLE\n"
                             "=====================\n";

        // Call 'recurseAndPrintExample1' with will recurse 'depth' times, then
        // print a stack trace.

        int depth = 5;
        recurseAndPrintStack(&depth);
        ASSERT(5 == depth);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING WITH A FUNCTION IN A .h FILE IN THE CALL STACK
        //
        // Concern:
        //: 1 That the stacktrace performs correctly if one of the functions
        //:   on the call stack is defined in a .h file.
        //
        // Plan:
        //: 1 A static inline function, 'forTestingOnlyDump', is defined within
        //:   this class.  It does a stack trace using 'operator<<' and stores
        //:   the result to a string.
        //:
        //: 2 'forTestingOnlyDump' must be called out of line to get it to
        //:   leave a stack frame.  Since it is declared inline, the compiler
        //:   will go to great lengths to inline it, especially in optimized
        //:   mode.  Take a pointer to the function and call it through that
        //:   pointer.
        //:
        //: 3 If we just store the address of the function to a pointer and
        //:   call through that pointer, the optimizer will sometimes *STILL*
        //:   figure out what we are doing and inline the call.  Call
        //:   'foilOptimizer', which does a transform on the function pointer
        //:   that results in it being unchanged, that is too complex for the
        //:   optimizer to understand.  Thus, the compiler has no choice but to
        //:   call the routine out of line.
        //:
        //: 4 On platforms / build modes that support source file names,
        //:   verify that the source file name of the inline function is
        //:   'balst_stacktrace.h'.
        //
        // Testing:
        //   CONCERN: 'printStackTrace' works for inline routine on stack
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST WITH INLINE FUNCTION\n"
                             "=========================\n";

        namespace TC = CASE_4;

        TC::top();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING STACK TRACE WITH FUNCTION IN A SHARED LIBRARY
        //
        // Concerns:
        //: 1 That correct routine names and library names are found if a
        //:   routine from a shared library is on the stack.
        //
        // Plan:
        //: 1 Create a routine that will do a stack trace.  Pass a pointer to
        //:   it to a shared library routine that we know will call our
        //:   routine.
        //:
        //: 2 Within our routine, do a stack trace, and examine it to verify
        //:   that the shared library routine name and the shared library name
        //:   appear appropriately.
        //
        //  On unix, we call 'qsort' with an array of length 2, which will
        //  result in the comparison function we provide being called once
        //  within libc.  On windows, use 'EnumWindows' function from
        //  'user32.dll' for the same purpose.
        //
        // Testing:
        //   CONCERN: 'printStackTrace' works for routine in static library
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_WINDOWS

        if (verbose) cout << "TEST USING ENUMWINDOWS\n"
                             "======================\n";

        EnumWindows(&phonyEnumWindowsProc, 0);

#else

        if (verbose) cout << "TEST USING QSORT\n"
                             "================\n";

        int ints[] = { 0, 1 };

        bsl::qsort(&ints, 2, sizeof(ints[0]), &phonyCompare);

#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'printStackTrace'
        //
        // Concerns:
        //: 1 The method outputs a correct sequence of routine names.
        //:
        //: 2 The number of frames printed does not exceed 'maxFrame'
        //:
        //: 3 Names are attempted to be demangled if 'demanglingPreferredFlag'
        //:   is 'true', and are not demangled otherwise.
        //:
        //: 4 'demanglingPreferredFlag' defaults to 'true'.
        //:
        //: 5 The method correctly returns the given 'stream'.
        //:
        //: 6 Any allocated memory is released at the end of the method.
        //:
        //: 7 An error message is correctly printed when an error occurs.
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Call 'printStackTrace()' to print a stack trace to a
        //:   stringstream.
        //:
        //: 2 Verify that the string created by the stringstream has the
        //:   expected routine names.
        //:
        //: 3 Use the 'BSLS_ASSERTTEST_*' macros to verify that, in appropriate
        //:   build modes, defensive checks are triggered for invalid
        //:   arguments.
        //
        // Testing:
        //   bsl::ostream& printStackTrace(ostream& s, int max, bool demangle);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'printStackTrace'\n"
                             "=========================\n";

        {
            namespace TC = CASE_2;

            (void) TC::bottom();

            ASSERT(0 == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\tprintStackTrace" << endl;
            {
                bsl::stringstream ss(&ta);
                ASSERT_PASS(PrintUtil::printStackTrace(ss, -1));
                ASSERT_FAIL(PrintUtil::printStackTrace(ss, -2));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
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

        if (verbose) cout << "Manipulator & Accessor Test\n"
                             "===========================\n";

        int result = TC::bottom(&ta);
        ASSERT(result >= 0x20);

        ASSERT(0 == defaultAllocator.numAllocations());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == defaultAllocator.numAllocations());

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
