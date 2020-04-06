// balst_stacktraceprintutil.t.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceprintutil.h>

#include <balst_objectfileformat.h>

#include <bdlma_sequentialallocator.h>
#include <bdlb_string.h>

#include <bslim_testutil.h>

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
//                      STANDARD BDE TEST DRIVER MACROS
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

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

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
    enum { e_FORMAT_ELF = 1, e_FORMAT_WINDOWS = 0, e_FORMAT_DLADDR = 0 };

# if   defined(BSLS_PLATFORM_OS_SOLARIS)
    enum { e_PLAT_SUN=1, e_PLAT_LINUX=0, e_PLAT_HP=0, e_PLAT_AIX=0,
                                                                e_PLAT_WIN=0 };
# elif defined(BSLS_PLATFORM_OS_LINUX)
    enum { e_PLAT_SUN=0, e_PLAT_LINUX=1, e_PLAT_HP=0, e_PLAT_AIX=0,
                                                                e_PLAT_WIN=0 };
# elif defined(BSLS_PLATFORM_OS_HPUX)
    enum { e_PLAT_SUN=0, e_PLAT_LINUX=0, e_PLAT_HP=1, e_PLAT_AIX=0,
                                                                e_PLAT_WIN=0 };
# else
#   error unknown platform
# endif

#elif defined(BALST_OBJECTFILEFORMAT_RESOLVER_DLADDR)
    enum { e_FORMAT_ELF = 0, e_FORMAT_WINDOWS = 0, e_FORMAT_DLADDR = 1 };
    enum { e_PLAT_SUN=0, e_PLAT_LINUX=0, e_PLAT_HP=0, e_PLAT_AIX=0,
                                                                e_PLAT_WIN=0 };
#elif defined(BALST_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
    enum { e_FORMAT_ELF = 0, e_FORMAT_WINDOWS = 1, e_FORMAT_DLADDR = 0 };
    enum { e_PLAT_SUN=0, e_PLAT_LINUX=0, e_PLAT_HP=0, e_PLAT_AIX=0,
                                                                e_PLAT_WIN=1 };
#elif defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)
    enum { e_FORMAT_ELF = 0, e_FORMAT_WINDOWS = 0, e_FORMAT_DLADDR = 0 };
    enum { e_PLAT_SUN=0, e_PLAT_LINUX=0, e_PLAT_HP=0, e_PLAT_AIX=1,
                                                                e_PLAT_WIN=0 };
#else
# error unknown object file format
#endif

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_DWARF
    enum { e_FORMAT_DWARF = 1 };
#else
    enum { e_FORMAT_DWARF = 0 };
#endif

#ifdef BDE_BUILD_TARGET_DBG
    enum { e_DEBUG_ON = 1 };
#else
    enum { e_DEBUG_ON = 0 };
#endif

#if defined(BDE_BUILD_TARGET_OPT)
    enum { e_OPT_ON = 1 };
#else
    enum { e_OPT_ON = 0 };
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    // Windows does not demangle parens or function args.

    enum { e_DEMANGLE_PARENS = 0,

# if  (BSLS_PLATFORM_CMP_VERSION >= 1700 && BSLS_PLATFORM_CMP_VERSION < 2000)
    // MSVC cl-18.00, cl-19.00, and cl-19.10 don't demangle the '::' part
    // either.

           e_DEMANGLE_COLONS  = 0 };
# else
           e_DEMANGLE_COLONS  = 1 };
# endif

#else
    enum { e_DEMANGLE_PARENS = 1,
           e_DEMANGLE_COLONS  = 1 };
#endif

// Linux clang can't demangle statics, and statics are invisible to Windows
// cl-17 - cl-19.

#if defined(BSLS_PLATFORM_OS_LINUX) && defined(BSLS_PLATFORM_CMP_CLANG)
# define u_STATIC
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
# if BSLS_PLATFORM_CMP_VERSION >= 1700 || BSLS_PLATFORM_CMP_VERSION < 2000
#   define u_STATIC
# else
#   define u_STATIC static
# endif
#else
# define u_STATIC static
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

const char *execName;

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

template <class FUNC_PTR>
inline
FUNC_PTR funcFoilOptimizer(const FUNC_PTR funcPtr)
{
    UintPtr ret = reinterpret_cast<UintPtr>(funcPtr);

    return reinterpret_cast<FUNC_PTR>(foilOptimizer(ret));
}

//-----------------------------------------------------------------------------

bool checkOutput(const bsl::string&               str,
                 const bsl::vector<const char *>& matches)
    // The specified 'matches' vector contains a sequence of strings.  This
    // function is to assert that all of those strings are present, as
    // substrings, in order, in the specified 'str'.
{
    const int ts = testStatus;

    bslma::TestAllocator localAllocator;
    bdlma::SequentialAllocator sa(&localAllocator);

    if (e_PLAT_WIN && !e_DEBUG_ON) {
        return true;                                                  // RETURN
    }

    const size_t NPOS = bsl::string::npos;
    for (bsl::size_t vecI = 0, posN = 0; vecI < matches.size(); ++vecI) {
        bsl::size_t newPos = str.find(matches[vecI], posN);
        ASSERTV(vecI, matches[vecI], str, str.substr(posN), NPOS != newPos);


        if (!e_PLAT_WIN) {
            posN = NPOS != newPos ? newPos : posN;
        }

#if defined(BSLS_PLATFORM_CMP_MSVC)
# if BSLS_PLATFORM_CMP_VERSION >= 1700 && BSLS_PLATFORM_CMP_VERSION < 2000
        // On the cl-17 - cl-19 versions of the MSVC windows compiler,
        // demangling is very confused and demangles namespaces, classes, and
        // methods in switched-around order.

        posN = 0;
# endif
#endif
    }

    if (problem()) {
        *out_p << str;
    }

    return ts == testStatus;
}

                                // -------
                                // case 4
                                // -------

namespace CASE_4 {

// Pointer to be set to inline '&PrintUtil::forTestingOnlyDump'.

bool isColonPair(const char *pc)
{
    return ':' == *pc && ':' == pc[1] && '\0' == pc[2];
}

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

    bsl::string inExec(execName, &ta);
    const char slash = e_FORMAT_WINDOWS ? '\\' : '/';
    bsl::size_t pos = inExec.rfind(slash);
    pos = bsl::string::npos == pos ? 0 : pos + 1;
    inExec.erase(0, pos);

    // The call to 'checkOutput' will verify that all the strings present in
    // the vector 'matches' occur, in order, in the string 'dump' which
    // contains the output of 'printStackTraceToString'.

    bsl::vector<const char *> matches(&ta);

    if (!(e_FORMAT_ELF && !e_FORMAT_DWARF) && !e_FORMAT_DLADDR &&
                                !e_FORMAT_WINDOWS && e_DEBUG_ON && !e_OPT_ON) {
        // Elf doesn't provide souce file names of global routines,
        // Dladdr never provides source file names for anything,
        // Windows often doesn't provide the source file name.

        matches.push_back("BloombergLP");
        matches.push_back("::");
        matches.push_back("balst");
        matches.push_back("::");
        matches.push_back("StackTracePrintUtil_Test");
        matches.push_back("::");
        matches.push_back(e_DEMANGLE_PARENS
                             ? "printStackTraceToString(bsl::basic_string<char"
                             : "printStackTraceToString");
        matches.push_back(" source:balst_stacktraceprintutil.h");
        matches.push_back(inExec.c_str());
        matches.push_back("\n");
        matches.push_back("CASE_4");
        matches.push_back(e_DEMANGLE_PARENS ? "top()" : "top");
        matches.push_back(" source:balst_stacktraceprintutil.t.cpp");
        matches.push_back(inExec.c_str());
        matches.push_back("\n");
        matches.push_back("main");
        matches.push_back(" source:balst_stacktraceprintutil.t.cpp");
        matches.push_back(inExec.c_str());
        matches.push_back("\n");
        if (!e_DEMANGLE_COLONS) {
            matches.erase(bsl::remove_if(matches.begin(),
                                         matches.end(),
                                         isColonPair),
                          matches.end());
        }
    }
    else {
        matches.push_back("BloombergLP");
        matches.push_back("::");
        matches.push_back("balst");
        matches.push_back("::");
        matches.push_back("StackTracePrintUtil_Test");
        matches.push_back("::");
        matches.push_back(e_DEMANGLE_PARENS
                             ? "printStackTraceToString(bsl::basic_string<char"
                             : "printStackTraceToString");
        matches.push_back("\n");
        matches.push_back("CASE_4");
        matches.push_back(e_DEMANGLE_PARENS ? "top()" : "top");
        matches.push_back("\n");
        matches.push_back("main");
        matches.push_back("\n");
        if (!e_DEMANGLE_COLONS) {
            matches.erase(bsl::remove_if(matches.begin(),
                                         matches.end(),
                                         isColonPair),
                          matches.end());
        }
    }

    checkOutput(dump, matches);

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
    if (!e_DEBUG_ON) {
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

extern "C"
int phonyCompare(const void *, const void *)
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

    struct Data {
        int         d_line;
        bool        d_noNewlines;
        const char *d_match;
    };

    Data strippedStrings[] = {
        { L_, false, "phonyCompare" },
        { L_, false, "main" } };
    enum { NUM_STRIPPED_STRINGS = sizeof strippedStrings /
                                                     sizeof *strippedStrings };

    Data debugStrings[] = {
        { L_, false, "phonyCompare" },
        { L_, false, "qsort" },
        { L_, true,  " in " },
        { L_, true,  e_FORMAT_DLADDR ? "/libsystem_c" : "/libc." },
        { L_, false, "main" } };
    enum { NUM_DEBUG_STRINGS = sizeof debugStrings / sizeof *debugStrings };

    const bool stripped = NPOS != dump.find("--unknown--");

    if (stripped) cout << "System libs stripped, unable to find 'qsort'\n";

    struct Data *STRINGS    = stripped ? strippedStrings : debugStrings;
    const int    numStrings = stripped ? NUM_STRIPPED_STRINGS
                                       : NUM_DEBUG_STRINGS;

    bsl::size_t pos = 0;
    for (int i = 0; i < numStrings; ++i) {
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

u_STATIC
int top()
{
    if (calledTop) return 9;                                          // RETURN
    calledTop = true;

    bslma::TestAllocator ta;
    bsl::vector<const char *> matches(&ta);
    matches.push_back(e_DEMANGLE_PARENS ? "top()" : "top");
    matches.push_back("\n");
    matches.push_back(e_DEMANGLE_PARENS ? "highMiddle(int" : "highMiddle");
    matches.push_back("\n");
    matches.push_back(e_DEMANGLE_PARENS ? "lowMiddle()" : "lowMiddle");
    matches.push_back("\n");
    matches.push_back(e_DEMANGLE_PARENS ? "bottom()" : "bottom");
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

u_STATIC
int highMiddle(int i)
{
    if (calledHighMiddle) return 40;                                  // RETURN
    calledHighMiddle = true;

    calledTop = false;

    i ^= ~i;            // effectively 'i = 0', but 'uses' value of 'i'
    for (; i < 40; ++i) {
        if (i & 16) {
            i += 5;
            ASSERT((*funcFoilOptimizer(&top))() >= 6);
        }
        else if (i & 8) {
            i += 7;
            ASSERT((*funcFoilOptimizer(&top))() >= 7);
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
            ASSERT((*funcFoilOptimizer(&highMiddle))(10) >= 40);
        }
        else if ((i & 2) && (i & 16)) {
            i += 5;
            ASSERT((*funcFoilOptimizer(&highMiddle))(10) >= 39);
        }
    }

    ASSERT(calledHighMiddle);

    return i;
}

u_STATIC
int bottom()
{
    calledLowMiddle = false;

    int i = 0;
    for (; i < 20; ++i) {
        if (i & 8) {
            i += 7;
            ASSERT((*funcFoilOptimizer(&lowMiddle))() >= 30);
        }
        if ((i & 2) && (i & 4)) {
            i += 4;
            ASSERT((*funcFoilOptimizer(&lowMiddle))() >= 28);
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

bool top(bslma::Allocator *alloc)
{
    const int ts = testStatus;

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

    return ts == testStatus;
}

int bottom(bslma::Allocator *alloc)
{
    // still attempting to thwart optimizer -- all this does is call 'top'
    // a bunch of times.

    called = false;

    int i = 0;
    for (; i < 0x20; ++i) {
        if ((i & 2) && (i & 4)) {
            if ((*funcFoilOptimizer(&top))(alloc)) {
                break;
            }
        }
        else if ((i & 1) && (i & 8)) {
            i += 5;
            top(alloc);
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

    execName = argv[0];

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

            (void) (*funcFoilOptimizer(&TC::bottom))();

            ASSERT(0 == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

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

        int result = (*funcFoilOptimizer(&TC::bottom))(&ta);
        ASSERT(result >= 6);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(testStatus || 0 == defaultAllocator.numAllocations());

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
