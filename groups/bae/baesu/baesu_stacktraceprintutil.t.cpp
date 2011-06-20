// baesu_stacktraceprintutil.t.cpp                                    -*-C++-*-
#include <baesu_stacktraceprintutil.h>

#include <baesu_objectfileformat.h>
#include <baesu_stackaddressutil.h>    // 'getStackAddresses(0,0)'

#include <bdef_function.h>
#include <bdema_sequentialallocator.h>
#include <bdeu_string.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>
#include <bsls_platformutil.h>
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


#ifdef BSLS_PLATFORM__OS_WINDOWS
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
// TEST PLAN
//-----------------------------------------------------------------------------
// [ 1] printStackTrace
// [ 2] printStackTrace
// [ 3] routine in static library
// [ 4] inline routine (called out of line) on stack
// [ 5] usage
//-----------------------------------------------------------------------------

//=============================================================================
// STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

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

//=============================================================================
// STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

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

//=============================================================================
// SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
// GLOBAL HELPER #DEFINES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// GLOBAL HELPER TYPES, CLASSES, and CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

typedef baesu_StackTracePrintUtil           PrintUtil;
typedef baesu_StackTracePrintUtil_Test      PrintUtilTest;

#if   defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)
    enum { FORMAT_ELF = 1, FORMAT_WINDOWS = 0, FORMAT_XCOFF = 0 };

# if   defined(BSLS_PLATFORM__OS_SOLARIS)
    enum { PLAT_SUN=1, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=0 };
# elif defined(BSLS_PLATFORM__OS_LINUX)
    enum { PLAT_SUN=0, PLAT_LINUX=1, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=0 };
# elif defined(BSLS_PLATFORM__OS_HPUX)
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=1, PLAT_AIX=0, PLAT_WIN=0 };
# else
#   error unknown platform
# endif

#elif defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
    enum { FORMAT_ELF = 0, FORMAT_WINDOWS = 1, FORMAT_XCOFF = 0 };
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=1 };
#elif defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
    enum { FORMAT_ELF = 0, FORMAT_WINDOWS = 0, FORMAT_XCOFF = 1 };
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=1, PLAT_WIN=0 };
#else
# error unknown object file format
#endif

#ifdef BDE_BUILD_TARGET_DBG
    enum { DEBUG_ON = 1 };
#else
    enum { DEBUG_ON = 0 };
#endif


#if defined(BSLS_PLATFORM__OS_WINDOWS) && defined(BSLS_PLATFORM__CPU_64_BIT)
// On Windows, longs aren't big enough to hold pointers or 'size_t's

#define SIZE_T_CONTROL_STRING "%llx"
typedef long long unsigned int UintPtr;

#else

#define SIZE_T_CONTROL_STRING "%lx"
typedef long      unsigned int UintPtr;

#endif

typedef bsls_Types::IntPtr     IntPtr;

}  // close unnamed namespace

//=============================================================================
// GLOBAL HELPER VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

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
    bslma_TestAllocator localAllocator;
    bdema_SequentialAllocator sa(&localAllocator);

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

    bslma_TestAllocator ta;
    bsl::string dump(&ta);
    (*testDumpUnion.d_funcPtr)(&dump);

    if (!FORMAT_ELF && !FORMAT_WINDOWS && DEBUG_ON) {
        // Elf totally doesn't provide souce file names of global routines,
        // Windows doesn't provide the source file name for an inline routine.

        bsl::vector<const char *> matches(&ta);
        matches.push_back("baesu_StackTracePrintUtil_Test");
        matches.push_back("printStackTraceToString");
        matches.push_back(" source:baesu_stacktraceprintutil.h");
        matches.push_back(" in baesu_stacktraceprintutil.t");
        matches.push_back("\n");
        matches.push_back("top");
        matches.push_back(" source:baesu_stacktraceprintutil.t.cpp");
        matches.push_back(" in baesu_stacktraceprintutil.t");
        matches.push_back("\n");
        matches.push_back("main");
        matches.push_back(" source:baesu_stacktraceprintutil.t.cpp");
        matches.push_back(" in baesu_stacktraceprintutil.t");
        matches.push_back("\n");
        checkOutput(dump, matches);
    }
    else {
        bsl::vector<const char *> matches(&ta);
        matches.push_back("baesu_StackTracePrintUtil_Test");
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

#ifdef BSLS_PLATFORM__OS_WINDOWS

BOOL CALLBACK phonyEnumWindowsProc(HWND, LPARAM)
{
    if (!DEBUG_ON) {
        return FALSE;                                                 // RETURN
    }

    bslma_TestAllocator ta;
    bsl::stringstream ss(&ta);
    PrintUtil::printStackTrace(ss);
    bsl::string dump(&ta);
    {
        bslma_DefaultAllocatorGuard guard(&ta);
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

        const char *matchPc = bdeu_String::strstrCaseless(pc,
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
    bslma_TestAllocator ta;
    bsl::stringstream ss(&ta);
    PrintUtil::printStackTrace(ss);
    bsl::string dump(&ta);
    {
        bslma_DefaultAllocatorGuard guard(&ta);
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
        { L_, true,  "/libc." },
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
void top()
{
    if (calledTop) return;
    calledTop = true;

    bslma_TestAllocator ta;
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
    baesu_StackTracePrintUtil::printStackTrace(os);

    bsl::string str(&ta);
    {
        bslma_DefaultAllocatorGuard guard(&ta);

        // 'bsl::stringstream::str' may create temporaries if the string is
        // large

        str = os.str();
    }

    checkOutput(str, matches);

    if (verbose) {
        *out_p << str;
    }
}

bool calledHighMiddle = false;

static
void highMiddle(int i)
{
    if (calledHighMiddle) return;
    calledHighMiddle = true;

    calledTop = false;

    i ^= ~i;            // effectively 'i = 0', but 'uses' value of 'i'
    for (; i < 40; ++i) {
        if (i & 16) {
            top();
        }
        else if (i & 8) {
            top();
        }
    }

    ASSERT(calledTop);
}

bool calledLowMiddle = false;

int lowMiddle()
{
    if (calledLowMiddle) return 7;
    calledLowMiddle = true;

    calledHighMiddle = false;

    for (int i = 0; i < 30; ++i) {
        if (i & 4) {
            highMiddle(10);
        }
    }

    ASSERT(calledHighMiddle);

    return 5;
}

static
int bottom()
{
    calledLowMiddle = false;

    int i = 0;
    for (; i < 20; ++i) {
        if (i & 8) {
            i *= lowMiddle();
        }
    }

    ASSERT(calledLowMiddle);

    return i;
}

// Make global ptrs to the static routines to prevent optimizer inlinng them.

void (*highMiddlePtr)(int) = &highMiddle;
int  (*bottomPtr)()        = &bottom;

}  // close namespace CASE_2

                                // ------
                                // case 1
                                // ------

namespace CASE_1 {

bool called = false;

void top(bslma_Allocator *alloc)
{
    if (called) return;
    called = true;

    bsl::vector<const char *> matches(alloc);
    matches.push_back("top");
    matches.push_back("bottom");
    matches.push_back("main");

    {
        bsl::stringstream myStream(alloc);
        baesu_StackTracePrintUtil::printStackTrace(myStream);
        bsl::string str(alloc);

        {
            bslma_DefaultAllocatorGuard guard(alloc);

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

void bottom(bslma_Allocator *alloc)
{
    // still attempting to thwart optimizer -- all this does is call 'top'
    // a bunch of times.

    called = false;

    for (int i = 0; i < 0x20; ++i) {
        if ((i & 2) && (i & 4)) {
            top(alloc);
        }
        else if ((i & 1) && (i & 8)) {
            top(alloc);
        }
    }

    ASSERT(called);
}

}  // close namespace CASE_1

//=============================================================================
//                                USAGE EXAMPLES
//-----------------------------------------------------------------------------

                                    // -------
                                    // Usage 1
                                    // -------

static
void recurseAndPrintExample1(int *depth)
    // Recurse the specified 'depth' number of times, then do a stack trace.
{
    if (--*depth > 0) {
        // Recurse until '0 == *depth' before generating a stack trace.

        recurseAndPrintExample1(depth);
    }
    else {
        // Call 'printStackTrace' to print out a stack trace.  In this case,
        // the 'maxFrames' argument is unspecified, defaulting to 1000 which is
        // more than we need and the 'demangle' argument is unspecified,
        // defaulting to 'on'.

        baesu_StackTracePrintUtil::printStackTrace(*out_p);
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a
                // loop.
}

//=============================================================================
// MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test    = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose     = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST CASE " << test << endl;

    // make sure the shared lib containing 'malloc' is loaded

    (void) bsl::malloc(100);

    // see if we can avoid calling 'malloc' from here on out

    bslma_TestAllocator ota;
    bdema_SequentialAllocator ta(&ota);

    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

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
        recurseAndPrintExample1(&depth);
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
        //: 2 'forTestingOnlyDump' must be called out of line to get it to
        //:   leave a stack frame.  Since it is declared inline, the compiler
        //:   will go to great lengths to inline it, especially in optimized
        //:   mode.  Take a pointer to the function and call it through that
        //:   pointer.
        //: 3 If we just store the address of the function to a pointer and
        //:   call through that pointer, the optimizer will sometimes *STILL*
        //:   figure out what we are doing and inline the call.  Call
        //:   'foilOptimizer', which does a transform on the function pointer
        //:   that results in it being unchanged, that is too complex for the
        //:   optimizer to understand.  Thus, the compiler has no choice but to
        //:   call the routine out of line.
        //: 4 On platforms / build modes that support source file names,
        //:   verify that the source file name of the inline function is
        //:   'baesu_stacktrace.h'.
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
        //: 2 Within our routine, do a stack trace, and examine it to verify
        //:   that the shared library routine name and the shared library name
        //:   appear appropriately.
        //
        //  On unix, we call 'qsort' with an array of length 2, which will
        //  result in the comparison function we provide being called once
        //  within libc.  On windows, use 'EnumWindows' function from
        //  'user32.dll' for the same purpose.
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_WINDOWS

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
        //: 2 Verify that the string created by the stringstream has the
        //:   expected routine names.
        //
        // Testing:
        //   bsl::ostream& printStackTrace(ostream& s, int max, bool demangle);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'printStackTrace'\n"
                             "=========================\n";

        namespace TC = CASE_2;

        (void) TC::bottom();
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
        //: 2 Check that the right sequence of routine names is present in
        //:   the string stream.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        namespace TC = CASE_1;

        if (verbose) cout << "Manipulator & Accessor Test\n"
                             "===========================\n";

        TC::bottom(&ta);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
