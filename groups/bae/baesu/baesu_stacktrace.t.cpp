// baesu_stacktrace.t.cpp                                             -*-C++-*-
#include <baesu_stacktrace.h>

#include <baesu_objectfileformat.h>
#include <baesu_stackaddressutil.h>

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

#endif

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
// TEST PLAN
//
// [ 1] default c'tor, d'tor
// [ 2] manipulator and accessor
// [ 2] initializeFromAddressArray
// [ 2] initializeFromStack
// [ 2] numFrames
// [ 2] stackFrame
// [ 2] printStackTrace
// [ 3] operator<<, printTerse
// [ 4] printStackTrace
// [ 5] initializeFromAddressArray
// [ 6] initializeFromStack
// [ 7] deep stack trace
// [ 9] function within shared lib
// [10] multiple traces in one pass
// [11] large symbols
// [12] output of trace with fprintf rather than stream
// [13] function in .h file on call stack
// [14] line #'s and offsets
// [15] usage 1
// [16] usage 2
// [17] usage 3
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

typedef baesu_StackAddressUtil         Address;
typedef baesu_StackTrace               Obj;
typedef baesu_StackTraceFrame          Frame;
typedef baesu_StackTraceUtil           Util;

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

bool stringstreamUsed;  // stringstreams allocate temporaries with the default
                        // allocator, so if we use any stringstreams we should
                        // not consider it an error if the default allocator
                        // has been used

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

static
const char *cc(const char *string)
{
    return string;
}

static
const char *nullGuard(const char *string)
{
    return string ? string : "(null)";
}

                                // --------------
                                // testStackTrace
                                // --------------

static
void testStackTrace(const baesu_StackTrace& st)
    // Verify that the specified StackTrace 'st' is sane.
{
    LOOP_ASSERT(st.numFrames(), st.numFrames() > 0);

    bool reachedMain = false;   // all the routines above 'main' are pretty
                                // small
    for (int i = 0; i < st.numFrames(); ++i) {
        const Frame& frame = st.stackFrame(i);

        ASSERT(frame.address());
        ASSERT((PLAT_WIN && !DEBUG_ON) || frame.libraryFileName().c_str());

        // The very last symbol might be weird on some platforms, so tolerate
        // that.

        if (i == st.numFrames() - 1) {
            continue;
        }

        ASSERT((PLAT_WIN && !DEBUG_ON) || frame.isSymbolNameValid());
        if (npos != frame.symbolName().find("main")) {
            reachedMain = true;
        }

        if (!PLAT_WIN || DEBUG_ON) {
            LOOP_ASSERT(i, frame.isOffsetFromSymbolValid());

            UintPtr offset = frame.offsetFromSymbol();
            const unsigned maxOffset = PLAT_HP ? 0x600 : 512;
            LOOP2_ASSERT(i, offset, offset > 0);
            LOOP2_ASSERT(i, offset, reachedMain || offset < maxOffset);
        }

        if (!FORMAT_ELF && DEBUG_ON) {
            ASSERT(frame.isSourceFileNameValid());
            ASSERT(frame.lineNumber() > 0);
        }

//#endif
    }
}

                                // -------
                                // case 13
                                // -------

namespace CASE_13 {

// Pointer to be set to inline '&Obj::forTestingOnlyDump'.

void top()
{
    typedef void (*TestDumpPtrType)(bsl::string *);
    union {
        TestDumpPtrType d_funcPtr;
        UintPtr         d_uintPtr;
    } testDumpUnion;

    testDumpUnion.d_funcPtr = &Obj::forTestingOnlyDump;
    testDumpUnion.d_uintPtr = foilOptimizer(testDumpUnion.d_uintPtr);

    bsl::string dump;
    (*testDumpUnion.d_funcPtr)(&dump);

    if (!FORMAT_ELF && !FORMAT_WINDOWS && DEBUG_ON) {
        // Elf totally doesn't provide souce file names of global routines,
        // Windows doesn't provide the source file name for an inline routine.

        bsl::vector<const char *> matches;
        matches.push_back("forTestingOnlyDump");
        matches.push_back(" source:baesu_stacktrace.h");
        matches.push_back(" in baesu_stacktrace.t");
        matches.push_back("top");
        matches.push_back(" source:baesu_stacktrace.t.cpp");
        matches.push_back(" in baesu_stacktrace.t");
        matches.push_back("main");
        matches.push_back(" source:baesu_stacktrace.t.cpp");
        matches.push_back(" in baesu_stacktrace.t");
        checkOutput(dump, matches);
    }
    else {
        bsl::vector<const char *> matches;
        matches.push_back("forTestingOnlyDump");
        matches.push_back("top");
        matches.push_back("main");
        checkOutput(dump, matches);
    }

    if (!FORMAT_WINDOWS || DEBUG_ON) {
        int lines = 0;
        for (bsl::size_t pos = 0; pos < dump.length(); ++lines, ++pos) {
            pos = dump.find('\n', pos);
            if (bsl::string::npos == pos) {
                break;
            }
            ASSERT('\n' == dump[pos]);
        }
        LOOP_ASSERT(lines, lines >= 3);
    }

    if (verbose || problem()) {
        *out_p << dump;
    }
}

}  // close namespace CASE_13

                                // -------
                                // case 12
                                // -------

static
void case_12_top()
{
    FILE *fp = stderr;
    if (!PLAT_WIN && !verbose) {
        fp = fopen("/dev/null", "w");
        ASSERT(fp);
    }

    for (bool demangle = false; true; demangle = !demangle) {
        Obj st;
        int rc = st.initializeFromStack(2000, demangle);
        LOOP_ASSERT(rc, 0 == rc);
        if (0 == rc) {
            testStackTrace(st);

            for (int i = 0; i < st.numFrames(); ++i) {
                const baesu_StackTraceFrame frame = st.stackFrame(i);
                const char *lnBegin = frame.libraryFileName().c_str();
                const char *ln = lnBegin + bsl::strlen(lnBegin);
                while (ln > lnBegin && '/' != ln[-1] && '\\' != ln[-1]) {
                    --ln;
                }
                if (bsl::strstr(lnBegin, "lib")) {
                    ln = lnBegin;
                }

                if (!PLAT_WIN || verbose) {
                    fprintf(fp,
                            "(%d) %s+0x" SIZE_T_CONTROL_STRING " at 0x"
                                              SIZE_T_CONTROL_STRING " in %s\n",
                            i,
                            frame.isSymbolNameValid()
                                                   ? frame.symbolName().c_str()
                                                   : "-- unknown",
                            (UintPtr) frame.offsetFromSymbol(),
                            (UintPtr) frame.address(),
                            ln);
                }
            }
        }

        if (demangle) {
            break;
        }
    }

    if (!PLAT_WIN && !verbose) {
        fclose(fp);
    }
}

static
void case_12_recurse(int *depth)
{
    if (--*depth <= 0) {
        case_12_top();
    }
    else {
        case_12_recurse(depth);
    }

    ++*depth;
}

                                // -------
                                // case 11
                                // -------

#ifdef BSLS_PLATFORM__OS_UNIX
// The goal here is to create an identifier > 32,000 bytes
// and < '((1 << 15) - 64)' bytes long.

// SYM11_50 followed by '::' should expand to about 50 chars of identifier
#define SYM11_50(A, B, C)                                                     \
    n23456789012345678901234567890123456789012345##A##B##C

#define NS11_50(A, B, C)                                                      \
    namespace SYM11_50(A, B, C) {

#define SYM11_1000(D, E)                                                      \
    SYM11_50(D,E,a):: SYM11_50(D,E,f):: SYM11_50(D,E,k):: SYM11_50(D,E,p)::   \
    SYM11_50(D,E,b):: SYM11_50(D,E,g):: SYM11_50(D,E,l):: SYM11_50(D,E,q)::   \
    SYM11_50(D,E,c):: SYM11_50(D,E,h):: SYM11_50(D,E,m):: SYM11_50(D,E,r)::   \
    SYM11_50(D,E,d):: SYM11_50(D,E,i):: SYM11_50(D,E,n):: SYM11_50(D,E,s)::   \
    SYM11_50(D,E,e):: SYM11_50(D,E,j):: SYM11_50(D,E,o):: SYM11_50(D,E,t)

#define NS11_1000(D, E)                                                       \
    NS11_50(D, E, a)  NS11_50(D, E, f)  NS11_50(D, E, k)  NS11_50(D, E, p)    \
    NS11_50(D, E, b)  NS11_50(D, E, g)  NS11_50(D, E, l)  NS11_50(D, E, q)    \
    NS11_50(D, E, c)  NS11_50(D, E, h)  NS11_50(D, E, m)  NS11_50(D, E, r)    \
    NS11_50(D, E, d)  NS11_50(D, E, i)  NS11_50(D, E, n)  NS11_50(D, E, s)    \
    NS11_50(D, E, e)  NS11_50(D, E, j)  NS11_50(D, E, o)  NS11_50(D, E, t)

#define ENDNS11_1000   } } } } }   } } } } }   } } } } }   } } } } }

#define SYM11_10000(X)                                                        \
    SYM11_1000(X, a)::SYM11_1000(X, d)::SYM11_1000(X, g)::SYM11_1000(X, j)::  \
    SYM11_1000(X, b)::SYM11_1000(X, e)::SYM11_1000(X, h)::SYM11_1000(X, k)::  \
    SYM11_1000(X, c)::SYM11_1000(X, f)

#define NS11_10000(X)                                                         \
    NS11_1000(X, a)   NS11_1000(X, d)   NS11_1000(X, g)   NS11_1000(X, j)     \
    NS11_1000(X, b)   NS11_1000(X, e)   NS11_1000(X, h)   NS11_1000(X, k)     \
    NS11_1000(X, c)   NS11_1000(X, f)

#define ENDNS11_10000                                                         \
    ENDNS11_1000      ENDNS11_1000      ENDNS11_1000      ENDNS11_1000        \
    ENDNS11_1000      ENDNS11_1000      ENDNS11_1000      ENDNS11_1000        \
    ENDNS11_1000      ENDNS11_1000

#define SYM11_32000                                                           \
    SYM11_10000(a) :: SYM11_10000(b) :: SYM11_10000(c) ::                     \
    SYM11_1000(d,a):: SYM11_1000(d,b)

#define NS11_32000                                                            \
    NS11_10000(a)     NS11_10000(b)     NS11_10000(c)                         \
    NS11_1000(d,a)    NS11_1000(d,b)

#define ENDNS11_32000                                                         \
    ENDNS11_1000     ENDNS11_1000                                             \
    ENDNS11_10000    ENDNS11_10000    ENDNS11_10000

# define SYM11    SYM11_32000
# define NS11     NS11_32000
# define ENDNS11  ENDNS11_32000
static const size_t case11MinLen = 32000;

#else
// WINDOWS

# define SYM11_50(A, B, C)                                                    \
    n23456789012345678901234567890123456789012345##A##B##C

# define NS11_50(A, B, C)                                                     \
    namespace SYM11_50(A, B, C) {

# define SYM11_900(D, E)                                                      \
    SYM11_50(D,E,a):: SYM11_50(D,E,f):: SYM11_50(D,E,k):: SYM11_50(D,E,p)::   \
    SYM11_50(D,E,b):: SYM11_50(D,E,g):: SYM11_50(D,E,l):: SYM11_50(D,E,q)::   \
    SYM11_50(D,E,c):: SYM11_50(D,E,h):: SYM11_50(D,E,m):: SYM11_50(D,E,r)::   \
    SYM11_50(D,E,d):: SYM11_50(D,E,i):: SYM11_50(D,E,n):: SYM11_50(D,E,s)::   \
    SYM11_50(D,E,e):: SYM11_50(D,E,j)

# define NS11_900(D, E)                                                       \
    NS11_50(D, E, a)  NS11_50(D, E, f)  NS11_50(D, E, k)  NS11_50(D, E, p)    \
    NS11_50(D, E, b)  NS11_50(D, E, g)  NS11_50(D, E, l)  NS11_50(D, E, q)    \
    NS11_50(D, E, c)  NS11_50(D, E, h)  NS11_50(D, E, m)  NS11_50(D, E, r)    \
    NS11_50(D, E, d)  NS11_50(D, E, i)  NS11_50(D, E, n)  NS11_50(D, E, s)    \
    NS11_50(D, E, e)  NS11_50(D, E, j)

# define ENDNS11_900   } } } } }   } } } } }   } } } } }   } } }

# define SYM11    SYM11_900(a, b)::SYM11_900(a, c)::                          \
                  SYM11_50(1,2,3)::SYM11_50(1,2,4)::SYM11_50(1,2,5)::         \
                  n23456789112345678921234567893123456

# define NS11     NS11_900(a, b) NS11_900(a, c)                               \
                  NS11_50(1,2,3)   NS11_50(1,2,4)   NS11_50(1,2,5)            \
                  namespace n23456789112345678921234567893123456 {

# define ENDNS11  ENDNS11_900 ENDNS11_900 } } } }
static const size_t case11MinLen = 1999;

#endif
# define SYM11Q(s)  #s

NS11    // open namespace

static inline
const char *nullGuard(const char *pc)
{
    return pc ? pc : "";
}

void case_11_top()
{
    size_t maxMangled   = 0;
    size_t maxDemangled = 0;

    Obj st;
    int rc = st.initializeFromStack(2000, true);             // always demangle
    LOOP_ASSERT(rc, 0 == rc);

    const char *maxMangledStr = "";
    for (int i = 0; i < st.numFrames(); ++i) {
        const baesu_StackTraceFrame& frame = st.stackFrame(i);

        size_t mangled = frame.mangledSymbolName().length();
        if (mangled > maxMangled) {
            maxMangled = mangled;
            maxMangledStr = frame.mangledSymbolName().c_str();
        }
        maxDemangled = bsl::max(maxDemangled, frame.symbolName().length());
    }

    if (verbose) {
        P(maxMangled);    P(maxDemangled);
    }

    if (maxMangled < case11MinLen) {
        *out_p << "SYM11 = ";
        *out_p << nullGuard(SYM11Q(SYM11));
        *out_p << bsl::endl;
        *out_p << "mangledSymbolName = '";
        *out_p << nullGuard(maxMangledStr);
        *out_p << "'\n";
    }
    if (!PLAT_WIN || DEBUG_ON) {
        LOOP_ASSERT(maxMangled,   maxMangled   >= case11MinLen);
        LOOP_ASSERT(maxDemangled, maxDemangled >= case11MinLen);
    }

    if (veryVerbose || problem()) {
        *out_p << st;
    }
}

ENDNS11  // close namespace

                                // ------
                                // case 8
                                // ------

extern "C" {

#ifdef BSLS_PLATFORM__OS_WINDOWS

BOOL CALLBACK phonyEnumWindowsProc(HWND, LPARAM)
{
    if (!DEBUG_ON) {
        return FALSE;                                                 // RETURN
    }

    bsl::stringstream ss;               stringstreamUsed = true;
    Util::printStackTrace(ss);
    const bsl::string& dump = ss.str();
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
    bsl::stringstream ss;                   stringstreamUsed = true;
    Util::printStackTrace(ss);
    const bsl::string& dump = ss.str();
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
                                // case 7
                                // ------

void case_7_top(bool demangle)
{
    Obj st;
    bsls_Stopwatch sw;
    sw.start(true);
    int rc = st.initializeFromStack(2000, demangle);
    LOOP_ASSERT(rc, 0 == rc);
    sw.stop();
    if (0 == rc) {
        testStackTrace(st);

        *out_p << st;
        *out_p << cc("User time: ") << sw.accumulatedUserTime() <<
                cc(", wall time: ") << sw.accumulatedWallTime() << endl;

#if defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(bsls_PLATFORM__CMP_GNU)
        demangle = false;    // demangling never happens with Sun CC
#endif

        if (DEBUG_ON || !PLAT_WIN) {
            // check that the names are right

            bool dot = '.' == *st.stackFrame(0).symbolName().c_str();

            const char *match = ".case_7_top";
            match += !dot;
            int len = bsl::strlen(match);
            const char *sn = st.stackFrame(0).symbolName().c_str();
            LOOP3_ASSERT(sn, match, len,
                                   !demangle || !bsl::strncmp(sn, match, len));
            LOOP2_ASSERT(sn, match,              bsl::strstr( sn, match));

            if (!FORMAT_ELF && DEBUG_ON) {
                // 'case_7_top' is global -- elf can't find source file names
                // for globals

                const char *sfnMatch = "baesu_stacktrace.t.cpp";
                const char *sfn = st.stackFrame(0).sourceFileName().c_str();
                sfn = nullGuard(sfn);

                int sfnMatchLen = bsl::strlen(sfnMatch);
                int sfnLen = bsl::strlen(sfn);
                sfn += bsl::max(0, sfnLen - sfnMatchLen);

                LOOP2_ASSERT(sfn, sfnMatch, !bsl::strcmp(sfn, sfnMatch));
            }

            match = ".case_7_bottom";
            match += !dot;
            len = bsl::strlen(match);

            bool finished = false;
            int recursersFound = 0;
            for (int i = 1; i < st.numFrames(); ++i) {
                sn = st.stackFrame(i).symbolName().c_str();
                if (!sn || !*sn) {
                    ASSERT(sn && *sn);
                    break;
                }

                if (bsl::strstr(sn, "main")) {
                    finished = true;
                    break;
                }

                if (bsl::strstr(sn, "bdef_Function")) {
                    continue;
                }

                LOOP3_ASSERT(i, sn, match, bsl::strstr(sn, match));
                if (demangle) {
                    LOOP4_ASSERT(i, sn, match, len,
                                                !bsl::strncmp(sn, match, len));
                }

                ++recursersFound;

                if (DEBUG_ON) {
                    // 'case_7_bottom' is static, so the source file name will
                    // be known on elf, thus it will be known for all
                    // platforms.

                    const char *sfnMatch = "baesu_stacktrace.t.cpp";
                    const char *sfn =
                                     st.stackFrame(i).sourceFileName().c_str();

                    int sfnMatchLen = bsl::strlen(sfnMatch);
                    int sfnLen = bsl::strlen(sfn);
                    sfn += bsl::max(0, sfnLen - sfnMatchLen);

                    LOOP2_ASSERT(sfn, sfnMatch, !bsl::strcmp(sfn, sfnMatch));
                }
            }

            ASSERT(recursersFound > 0);
            ASSERT(finished);

            if (!verbose && problem()) {
                *out_p << st;
            }
        }
    }
}

static
void case_7_bottom(bool demangle, int *depth)
{
    if (--*depth <= 0) {
        bdef_Function<void(*)(bool)> func = &case_7_top;
        func(demangle);
    }
    else {
        case_7_bottom(demangle, depth);
    }

    ++*depth;
}


                                // ------
                                // case 6
                                // ------

bool case_6_top_called = false;

static
void case_6_top(bool demangle)
{
    if (case_6_top_called) return;
    case_6_top_called = true;

    Obj st;
    int rc = st.initializeFromStack(100, demangle);
    LOOP_ASSERT(rc, 0 == rc);
    if (0 == rc) {
        testStackTrace(st);

        bslma_TestAllocator ta;
        bsl::vector<const char *> matches(&ta);
        matches.push_back("case_6_top");
        matches.push_back("middle");
        matches.push_back("bottom");
        matches.push_back("main");

        bsl::stringstream os;                   stringstreamUsed = true;
        os << st;
        const bsl::string& str = os.str();
        checkOutput(str, matches);
        problem();    // set 'out_p' if problem

        *out_p << str;
    }
}

namespace CASE_6 {

bool middleCalled = false;

void middle(bool demangle)
{
    if (middleCalled) return;
    middleCalled = true;

    case_6_top_called = false;

    for (int i = 0; i < 1024; ++i) {
        if (i & 0xabcd) {
            case_6_top(demangle);
            i += 1024 << 4;
        }
    }

    ASSERT(case_6_top_called);
}

bool bottomCalled = false;

void bottom(bool demangle, double x)
{
    if (bottomCalled) return;
    bottomCalled = true;

    middleCalled = false;

    for (int i = 0; i < (1 << 15); ++i) {
        x *= x;
        if (i & 0x1234) {
            middle(demangle);
            i += 1 << 12;
        }
    }

    ASSERT(middleCalled);
}

}  // close namespace CASE_6

                                // ------
                                // case 5
                                // ------

static bool calledCase5Top = false;

static
void case_5_Top(bool demangle)
{
    if (calledCase5Top) return;
    calledCase5Top = true;

    enum { MAX_FRAMES = 100 };
    void *addresses[MAX_FRAMES];
    Obj st;

    int numAddresses = Address::getStackAddresses(addresses, MAX_FRAMES);

    int rc = st.initializeFromAddressArray(addresses, numAddresses, demangle);
    LOOP_ASSERT(rc, 0 == rc);
    if (0 == rc) {
        testStackTrace(st);

        bslma_TestAllocator ta;
        bsl::vector<const char *> matches(&ta);
        matches.push_back("case_5_Top");
        matches.push_back("upperMiddle");
        matches.push_back("lowerMiddle");
        matches.push_back("bottom");
        matches.push_back("main");

        bsl::stringstream os;                   stringstreamUsed = true;
        os << st;
        const bsl::string& str = os.str();
        checkOutput(str, matches);
        if (verbose || problem()) {
            *out_p << str;
        }
    }
}

namespace CASE_5 {

bool calledUpperMiddle = false;

void upperMiddle(bool demangle, int i)
{
    if (calledUpperMiddle) return;
    calledUpperMiddle = true;

    calledCase5Top = false;

    for (int j = 0; j < 100; ++j) {
        if (j & 16) {
            i *= i;
            case_5_Top(demangle);
        }
    }

    ASSERT(calledCase5Top);
}

bool calledLowerMiddle = false;

int lowerMiddle(bool demangle)
{
    if (calledLowerMiddle) return 97;
    calledLowerMiddle = true;

    calledUpperMiddle = false;

    for (int j = 0; j < 100; ++j) {
        if (j & 16) {
            upperMiddle(demangle, 10);
        }
    }

    ASSERT(calledUpperMiddle);

    return 7;
}

bool calledBottom = false;

double bottom(bool demangle)
{
    if (calledBottom) return 0.89;
    calledBottom = true;

    calledLowerMiddle = false;

    double x = 0;

    for (int i = 0; i < 100; ++i) {
        if (i & 4) {
            i *= 50;
            x = 3.7 * lowerMiddle(demangle);
        }
    }

    ASSERT(calledLowerMiddle);

    return x;
}

}  // close namespace CASE_5

                                    // ------
                                    // case 4
                                    // ------

namespace CASE_4 {

bool calledTop = false;

static
void top()
{
    if (calledTop) return;
    calledTop = true;

    bslma_TestAllocator ta;
    bsl::vector<const char *> matches(&ta);
    matches.push_back("top");
    matches.push_back("highMiddle");
    matches.push_back("lowMiddle");
    matches.push_back("bottom");
    matches.push_back("main");

    bsl::stringstream os;                       stringstreamUsed = true;
    baesu_StackTraceUtil::printStackTrace(os);
    checkOutput(os.str(), matches);

    if (verbose) {
        *out_p << os.str();
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

}  // close namespace CASE_4

                                // ------
                                // case 3
                                // ------

namespace CASE_3 {

bool called = false;

void top(bslma_Allocator *alloc)
    // Note that we don't get
{
    if (called) return;
    called = true;

    bsl::vector<const char *> matches(alloc);
    matches.push_back("top");
    matches.push_back("bottom");
    matches.push_back("main");

    Obj st;
    st.initializeFromStack();

    bsl::string strA(alloc);
    {
        bsl::stringstream ssA(alloc);           stringstreamUsed = true;
        bsl::ostream& ssARef = ssA << st;
        ASSERT(&ssA == &ssARef);
        {
            bslma_DefaultAllocatorGuard guard(alloc);

            strA = ssA.str();
        }
        checkOutput(strA, matches);
    }

    bsl::string strB(alloc);
    {
        bsl::stringstream ssB(alloc);
        bsl::ostream& ssBRef = st.printTerse(ssB);
        ASSERT(&ssB == &ssBRef);
        {
            bslma_DefaultAllocatorGuard guard(alloc);

            strB = ssB.str();
        }
        checkOutput(strB, matches);
    }

    LOOP2_ASSERT(strA, strB, strA == strB);
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
    }

    ASSERT(called);
}

}  // close namespace CASE_3

                                // ------
                                // case 2
                                // ------

namespace CASE_2 {

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
        bsl::stringstream myStream(alloc);           stringstreamUsed = true;
        baesu_StackTraceUtil::printStackTrace(myStream);
        bsl::string str(alloc);

        {
            bslma_DefaultAllocatorGuard guard(alloc);

            // 'bsl::string::str' may create temporaries if the string is
            // large

            str = myStream.str();
        }
        checkOutput(str, matches);

        if (problem()) {
            baesu_StackTraceUtil::printStackTrace(*out_p);
        }
    }

    {
        enum { IGNORE_FRAMES = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES };

        void *addresses[3 + IGNORE_FRAMES];
        bsl::memset(addresses, 0, sizeof(addresses));
        int na = baesu_StackAddressUtil::getStackAddresses(addresses,
                                                           3 + IGNORE_FRAMES);
        na -= IGNORE_FRAMES;

        if (!PLAT_WIN || DEBUG_ON) {
            LOOP_ASSERT(na, 3 == na);

            Obj st;
            int rc = st.initializeFromAddressArray(addresses + IGNORE_FRAMES,
                                                   na);
            ASSERT(!rc);
            LOOP_ASSERT(st.numFrames(), st.numFrames() == na);

            const char *sn;
            sn = st.stackFrame(0).symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "top"));
            sn = st.stackFrame(1).symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "bottom"));
            sn = st.stackFrame(2).symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "main"));

            LOOP_ASSERT(st, !problem());
        }
    }

    {
        Obj st;
        int rc = st.initializeFromStack(3);
        ASSERT(!rc);

        if (!PLAT_WIN || DEBUG_ON) {
            LOOP_ASSERT(st.numFrames(), 3 == st.numFrames());

            const char *sn;
            sn = st.stackFrame(0).symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "top"));
            sn = st.stackFrame(1).symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "bottom"));
            sn = st.stackFrame(2).symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "main"));
        }

        if (problem()) {
            *out_p << st;
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

}  // close namespace CASE_2

                            // ------------------
                            // case -1: benchmark
                            // ------------------

namespace CASE_MINUS_ONE {

typedef int (*GetStackPointersFunc)(void **buffer,
                                    int    maxFrames);
GetStackPointersFunc funcPtr;

enum {
    RECURSION_DEPTH = 40,
    MAX_FRAMES = 100
};

void recurser(int  iterations,
              int *depth)
{
    if (--*depth <= 0) {
        void *addresses[MAX_FRAMES];
        for (int i = iterations; i > 0; --i) {
            int frames = (*funcPtr)(addresses, MAX_FRAMES);
            LOOP2_ASSERT(RECURSION_DEPTH, frames, RECURSION_DEPTH < frames);
            LOOP2_ASSERT(RECURSION_DEPTH + 10, frames,
                                                RECURSION_DEPTH + 10 > frames);
        }
    }
    else {
        recurser(iterations, depth);
    }

    ++*depth;         // prevent tail recursion optimization
}

}  // close namespace CASE_MINUS_ONE

//=============================================================================
//                                USAGE EXAMPLES
//-----------------------------------------------------------------------------

                                    // -------
                                    // Usage 3
                                    // -------

static
void recurseAndPrintExample3(int *depth)
{
    if (--*depth > 0) {
        // Recurse until '0 == *depth' before generating a stack trace.

        recurseAndPrintExample3(depth);
    }
    else {
        enum { ARRAY_LENGTH = 50 };
        void *addresses[ARRAY_LENGTH];
        baesu_StackTrace st;

        // First, we call 'getStackAddresses' to get the stored return
        // addresses from the stack and load them into the array 'addresses'.
        // The call returns the number of addresses saved into the array, which
        // will be less than or equal to 'ARRAY_LENGTH'.

        int numAddresses = baesu_StackAddressUtil::getStackAddresses(
                                                             addresses,
                                                             ARRAY_LENGTH);

        // Next we call 'initializeFromAddressArray' to initialize the
        // information in the stack trace object, such as function names,
        // source file names, and line numbers, if they are available.  The
        // third argument, demangle, is 'true'.

        int rc = st.initializeFromAddressArray(addresses, numAddresses);

        // There will be no frames and 'initializeFromAddressArray' will
        // fail if optimized on Windows with the optimizer.

#if !defined(BSLS_PLATFORM__OS_WINDOWS) || !defined(BDE_BUILD_TARGET_OPT)
        ASSERT(0 == rc);
        ASSERT(st.numFrames() >= 6);                    // main + 5 recurse
#endif

        // Finally, wne can now print out the stack trace object using
        // 'cout << st;', or iterate through the stack trace frames,
        // printing them out one by one.  In this example, we want only
        // function names, and not line numbers, source file names, or
        // library names, so we iterate through the stack trace frames and
        // print out only those properties.

        for (int i = 0; i < st.numFrames(); ++i) {
            const baesu_StackTraceFrame& frame = st.stackFrame(i);

            const char *sn = frame.symbolName().c_str();
            sn = sn ? sn : "--unknown--";
            *out_p << '(' << i << "): " << sn << endl;
        }
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a
                // loop.
}

                                    // -------
                                    // Usage 2
                                    // -------

static
void recurseAndPrintExample2(int *depth)
    // This function does the stack trace.
{
    if (--*depth > 0) {
        // Recurse until '0 == *depth' before generating a stack trace.

        recurseAndPrintExample2(depth);
    }
    else {
        // Here we create a 'StackTrace' object and call
        // 'initializeFromStack' to load the information from the stack of
        // the current thread into the stack trace object.  We then use the
        // '<<' operator to print out the stack trace.  In this case,
        // 'maxFrames' is defaulting to 1000 and demangling is defaulting
        // to 'on'.  Note that the object 'st' takes very little room on
        // the stack, allocating most of its memory directly from virtual
        // memory without going through the heap, minimizing potential
        // complications due to stack size limits and possible heap
        // corruption.

        baesu_StackTrace st;
        int rc = st.initializeFromStack();

        // 'initializeFromStack' will fail and there will be no frames on
        // Windows compiled with the optimizer.

#if !defined(BSLS_PLATFORM__OS_WINDOWS) || !defined(BDE_BUILD_TARGET_OPT)
        ASSERT(0 == rc);
        ASSERT(st.numFrames() >= 6);                    // main + 5 recurse
#endif

        *out_p << st;    // print it out
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a
                // loop.
}

                                    // -------
                                    // Usage 1
                                    // -------

static
void recurseAndPrintExample1(int *depth)
{
    if (--*depth > 0) {
        // Recurse until '0 == *depth' before generating a stack trace.

        recurseAndPrintExample1(depth);
    }
    else {
        // Calling 'printStackTrace' (static method) is the easiest way to
        // print out a stack trace.  In this case, the 'maxFrames' argument
        // is unspecified, defaulting to 1000 which is more than we need
        // and the 'demangle' argument is unspecified, defaulting to 'on'.

        baesu_StackTraceUtil::printStackTrace(*out_p);
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
    if (verbose) {
        out_p = &cout;
        stringstreamUsed = false;
    }
    else {
        out_p = &dummyOstream;
        stringstreamUsed = true;
    }

    switch (test) { case 0:
      case 17: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE THREE
        //
        // Concerns:
        //  That the usage example that uses 'getStackAddresses' and
        //  'initializeFromAddressArray' works.
        //
        // Plan: call the routines in the usage example to observe that the
        //  stack trace works.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF USAGE EXAMPLE 3\n"
                             "=======================\n";

        // Call 'recurseAndPrintExample3' with will recurse 'depth' times, then
        // print a stack trace.

        int depth = 5;
        recurseAndPrintExample3(&depth);
        ASSERT(5 == depth);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TWO
        //
        // Concerns:
        //  That the usage example that uses 'initializeFromStack' works.
        //
        // Plan: call the routines in the usage example to observe that the
        //  stack trace works.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF USAGE EXAMPLE 2\n"
                             "=======================\n";

        // Call 'recurseAndPrintExample2' with will recurse 'depth' times, then
        // print a stack trace.

        int depth = 5;
        recurseAndPrintExample2(&depth);
        ASSERT(5 == depth);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE ONE
        //
        // Concerns:
        //  That the usage example works.
        //
        // Plan: call the routines in the usage example to observe that the
        //  stack trace works.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF USAGE EXAMPLE 1\n"
                             "=======================\n";

        // Call 'recurseAndPrintExample1' with will recurse 'depth' times, then
        // print a stack trace.

        int depth = 5;
        recurseAndPrintExample1(&depth);
        ASSERT(5 == depth);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING LINE #'S AND OFFSETS
        //
        // Concern:
        //: 1 That the line numbers are accurate
        //: 2 The offsets are consistent with the addresses.
        //
        // Plan:
        //: 1 Within one routine, take stack traces 3 times in a row to 3 stack
        //:   trace objects.
        //: 2 On the line after each stack trace is taken, store the line
        //:   number by assigning to ints from '__LINE__'.
        //: 3 Verify that the symbol name for the top frame of the 3 stack
        //:   trace objects is correct.
        //: 4 Verify that the line numbers from the top frames of the 3 stack
        //:   trace objects match the line numbers we saved to ints.
        //: 5 Verify that the offsets from the top frames of the 3 stack trace
        //:   objects are in the right order.
        //: 6 Verify that the addresses from the top frames of the 3 stack
        //:   trace objects are in the right order.
        //: 7 Verify that the differences between the offsets are the same as
        //:   the differences between the addresses from those frames.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING LINE #'S AND OFFSETS\n"
                             "============================\n";

        enum { DATA_POINTS = 3 };

        Obj stackTraces[DATA_POINTS];
        int lineNumbers[DATA_POINTS];

        // Note that the '__LINE__' assignments *MUST* occur on the lines after
        // the 'initializeFromStack' calls.

        int rc;
        rc = stackTraces[0].initializeFromStack(100, false);
        LOOP_ASSERT(rc, 0 == rc);                  lineNumbers[0] = __LINE__;

        rc = stackTraces[1].initializeFromStack(100, false);
        LOOP_ASSERT(rc, 0 == rc);                  lineNumbers[1] = __LINE__;

        rc = stackTraces[2].initializeFromStack(100, false);
        LOOP_ASSERT(rc, 0 == rc);                  lineNumbers[2] = __LINE__;

        UintPtr lastAddress = 0;
        IntPtr lastOffset   = 0;
        int lastLineNumber  = 0;

        if (PLAT_WIN) {
            --lineNumbers[0];
            --lineNumbers[1];
            --lineNumbers[2];
        }

        for (int i = 0; i < DATA_POINTS; ++i) {
            baesu_StackTrace& stackTrace = stackTraces[i];
            const baesu_StackTraceFrame& frame = stackTrace.stackFrame(0);

            UintPtr thisAddress = (UintPtr) frame.address();
            IntPtr offset = frame.offsetFromSymbol();

            if (!PLAT_WIN || DEBUG_ON) {
                LOOP2_ASSERT(i, frame.symbolName(),
                                      npos != frame.symbolName().find("main"));

                ASSERT(thisAddress > lastAddress);

                LOOP2_ASSERT(lastOffset, offset, lastOffset < offset);

                if (0 != i) {
                    IntPtr offsetInc  = offset - lastOffset;
                    IntPtr addressInc = thisAddress - lastAddress;
                    LOOP2_ASSERT(offsetInc, addressInc,
                                                      offsetInc == addressInc);
                }
            }

            lastAddress = thisAddress;
            lastOffset = offset;

            if (DEBUG_ON && !FORMAT_ELF) {
                int lineNumber = frame.lineNumber();
                LOOP3_ASSERT(i, lineNumber, lineNumbers[i],
                                                 lineNumber == lineNumbers[i]);
                LOOP2_ASSERT(lastLineNumber, lineNumber,
                                                  lastLineNumber < lineNumber);
                lastLineNumber = lineNumber;
            }
        }

        if (verbose || problem()) {
            for (int i = 0; i < DATA_POINTS; ++i) {
                *out_p << '(' << i << ")(0): " <<
                                          stackTraces[i].stackFrame(0) << endl;
            }
        }
      }  break;
      case 13: {
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

        namespace TC = CASE_13;

        TC::top();
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING WITH FPRINTF
        //
        // Concern:
        //: 1 On some platforms, streaming the stacktrace out calls 'malloc'.
        //:   Provide a test that does a stack trace without using any C++
        //:   streaming to enable debugging with minimal calls to 'malloc', at
        //:   least on those platforms where 'printf' doesn't call 'malloc'.
        //
        // Plan:
        //: 1 Do a test case that won't stream out unless an assert fails, that
        //:   otherwise does output with 'printf' only.
        //: 2 On Unix, write the output to "/dev/null" to detect whether a
        //:   segfault will occur duing output.
        // --------------------------------------------------------------------

        // setbuf(stdout, 0);

        if (verbose) printf("FPRINTF TEST\n"
                            "============\n");

        int depth = 5;
        case_12_recurse(&depth);
        ASSERT(5 == depth);
      }  break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING LARGE SYMBOLS
        //
        // Concerns:
        //: 1 That the package can resolve large symbols.
        //
        // Plan:
        //   The expected max symbol size is a 1999 bytes on Windows,
        //   and a little less than 32K on Unix.  Verify that we can go near
        //   these limits.
        //
        //: 1 Create a routine with a very long name, which does a stack
        //:   trace.  Call that routine.
        //: 2 Within that routine, do the stack trace and examine the lengths
        //:   of the symbols encountered on the stack trace, and ensure that
        //:   on of them is indeed as long as we intended.
        // --------------------------------------------------------------------

        if (verbose) cout << "LARGE SYMBOL TESTING\n"
                             "====================\n";

        SYM11::case_11_top();
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // DOING STACK TRACE TWICE
        //
        // Concern:
        // 1 That the stack trace works properly when called multiple times
        //   within a run.
        //
        // Plan:
        // 1 Run stack trace tests from previous test cases multiple times,
        //   and run different tests, to ensure that an appropriate result
        //   unaffected by previous results is delivered every time.
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTIPLE TESTS\n"
                             "==============\n";

        if (verbose) cout << "Indicate depth by passing an int for 1st v\n";

        int startDepth = 10;
        if (verbose && isdigit(*argv[2])) {
            startDepth = bsl::atoi(argv[2]);
        }
        int depth = startDepth;
        case_7_bottom(false, &depth);    // no demangle
        ASSERT(startDepth  == depth);

        depth *= 2;
        startDepth *= 2;
        case_7_bottom(true,  &depth);    // demangle
        ASSERT(startDepth  == depth);

        CASE_6::bottomCalled = false;
        CASE_6::bottom(false, 3.7);    // no demangling
        ASSERT(CASE_6::bottomCalled);

        CASE_6::bottomCalled = false;
        CASE_6::bottom(true,  3.7);    // demangling
        ASSERT(CASE_6::bottomCalled);
      } break;
      case 9: {
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
      case 8: {
        // --------------------------------------------------------------------
        // CASE 8
        //
        // This test case has been retired.
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST OF RECURSIVE DEEP TRACE
        //
        // Concerns:
        //: 1 That the stack trace utility will work properly on a deep
        //:   stack trace.
        //
        // Plan:
        //: 1 Deeply recurse within a routine, then call the routine
        //:   'case_7_top', which will create a stack trace object and populate
        //:   it using 'initializeFromStack'.
        //: 2 Verify that the source file names and symbol names in the stack
        //:   trace object are as expected.
        //: 3 Repeat the test twice, once with demangling and once without.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF RECURSIVE DEEP TRACE WITH FUNCTOR\n"
                             "=========================================\n";

        if (verbose) cout << "Indicate depth by passing an int for 1st v\n";

        int startDepth = verbose ? 10 : 200;
        if (verbose && isdigit(*argv[2])) {
            startDepth = bsl::atoi(argv[2]);
        }
        int depth = startDepth;

        case_7_bottom(false, &depth);    // no demangle
        ASSERT(startDepth == depth);

        case_7_bottom(true,  &depth);    // demangle
        ASSERT(startDepth == depth);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST OF initializeFromStack
        //
        // Concern:
        //: 1 That 'initializeFromStack' produces a proper stack trace.
        //
        // Plan:
        //: 1 Create a stack trace object, then populate it using
        //:   'initializeFromStack'.
        //: 2 Do sanity checks on the stack trace object.
        //: 3 Output the stack trace object to a stringstream using '<<'.
        //: 3 Confirm that the expected routine names are in the string
        //:   prodced by the stringstream, in the right order.
        //: 4 Repeat the test twice, with and without demangling enabled.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF DIRECT CALL TO initializeFromStack\n"
                             "==========================================\n";

        namespace TC = CASE_6;

        TC::bottomCalled = false;
        TC::bottom(false, 3.7);    // no demangling
        ASSERT(TC::bottomCalled);

        TC::bottomCalled = false;
        TC::bottom(true,  3.7);    // demangling
        ASSERT(TC::bottomCalled);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST OF STACK TRACE USING 'getStackAddresses' /
        //                                         'initializeFromAddressArray'
        //
        // Concerns:
        //: 1 That the combination of 'getStackAddresses' and
        //:   'initializeFromAddressArray' produces a proper stack trace.
        //
        // Plan:
        //: 1 Call 'getStackAddresses' followed by
        //:   'initializeFromAddressArray' to populate a stack trace object.
        //: 2 Run sanity checks on the stack trace.
        //: 3 Output the stack trace to a stringstream using '<<'.
        //: 4 Verify that the expected routine names were output to the
        //:   stringstream, in the right order.
        //: 5 Repeat the whole test twice, once with & without demangling
        //:   enabled.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF DIRECT CALL TO getStackAddresses\n"
                             "========================================\n";

        namespace TC = CASE_5;

        TC::calledBottom = false;
        (void) TC::bottom(false);    // no demangling
        ASSERT(TC::calledBottom);

        TC::calledBottom = false;
        (void) TC::bottom(true);     // demangling
        ASSERT(TC::calledBottom);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'printStackTrace' TEST
        //
        // Concerns:
        //: 1 That 'printStackTrace' works properly.
        //
        // Plan:
        //: 1 Call 'printStackTrace()' to print a stack trace to a
        //:   stringstream.
        //: 2 Verify that the string created by the stringstream has the
        //:   expected routine names.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'printStackTrace'\n"
                             "=========================\n";

        namespace TC = CASE_4;

        (void) TC::bottom();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRINTTERSE AND STREAM TEST
        //
        // Concerns:
        //: 1 That both '<<' and 'printTerse' write to the specified 'ostream'.
        //: 2 That both '<<' and 'printTerse' output exactly the same string.
        //: 3 That the strings that are output are non-zero.
        //: 4 That the streams returned are the same streams that are passed
        //:   in.
        //: 5 That neither function allocates any memory from the default
        //:   allocator.
        //
        // Plan:
        //: 1 Create a stack trace object.
        //: 2 Output the stack trace object to a stringstream using '<<'.
        //:   1 Verify that the string produced contains the expected routine
        //:     names, in the right order.
        //:   2 Verify that the reference returned by the '<<' operation refers
        //:     to the stream passed in.
        //: 3 Output the stack trace object to a stringstream using
        //:   'printTerse'.
        //:   1 Verify that the string produced contains the expected routine
        //:     names, in the right order.
        //:   2 Verify that the reference returned by the 'printTerse' refers
        //:     to the stream passed in.
        //: 4 Verify that the default allocator was not used by either
        //:   operator.
        // --------------------------------------------------------------------

        namespace TC = CASE_3;

        if (verbose) cout << "Print and Streamout Test\n"
                             "========================\n";

        TC::bottom(&ta);

        ASSERT(0 == defaultAllocator.numAllocations());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND ACCESSOR TEST
        //
        // Concerns:
        //: 1 That 'numFrames' of a default constructed object is 0.
        //: 2 That 'numFrames' accurately reflects the # of frames.
        //: 3 That the basic manipulators work properly.
        //:   1 That 'Obj::initializeFromAddressArray' works properly
        //:   2 That 'Obj::initializeFromStack' works properly
        //: 4 That the basic accessors work properly
        //:   1 That 'Util::printStackTrace' works properly
        //:   2 That 'Obj::stackFrame' works properly
        //: 5 That no memory is allocated using the default allocator by either
        //:   of the two methods for initializing a stack trace object, or
        //:   'Util::printStackTrace'.
        //
        // Plan:
        //: 1 Create and destroy an object.
        //:   1 Verify the allocator is not the default allocator.
        //: 2 Within a routine a couple of levels deep on the stack
        //:   1 Obtain a string containing the trace using
        //:     'Util::printStackTrace' and verify that it contains the
        //:     routines we expect to find on the stack.
        //:   2 Create a stack trace object and initialize it using
        //:     'Obj::initializeFromAddressArray'.  Verify using
        //:     'Obj::stackFrame' that the trace contains the routines
        //:     expected.
        //:   3 Create a stack trace object and initialize it using
        //:     'Obj::initializeFromStack'.  Verify using
        //:     'Obj::stackFrame' that the trace contains the routines
        //:     expected.
        //: 3 Call 'numAllocations' on the default allocator to verify that no
        //    memory allocation using that allocator has occurred.
        // --------------------------------------------------------------------

        namespace TC = CASE_2;

        if (verbose) cout << "Manipulator & Accessor Test\n"
                             "===========================\n";

        {
            Obj st;
            ASSERT(0 == st.numFrames());

            ASSERT(st.allocator());
            ASSERT(st.allocator() != &ota);
            ASSERT(st.allocator() != &ta);
            ASSERT(st.allocator() != &defaultAllocator);
        }

        TC::bottom(&ta);

        ASSERT(0 == defaultAllocator.numAllocations());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CREATOR TEST
        //
        // Concerns:
        //: 1 That neither the default constructor nor the destructor segfault
        //:   when called.  There are no other creators.
        //
        // Plan:
        //: 1 Create and destroy an object.
        // --------------------------------------------------------------------

        if (verbose) cout << "Creator Test\n"
                             "============\n";

        Obj st;
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // BENCHMARK OF getStackAddresses
        // --------------------------------------------------------------------

        namespace TD = CASE_MINUS_ONE;

        baesu_StackTrace st;

        int depth = TD::RECURSION_DEPTH;

        int iterations = PLAT_SUN || PLAT_WIN ? 100 : 100 * 1000;

        bsls_Stopwatch sw;
        TD::funcPtr = &Address::getStackAddresses;

        sw.start(true);
        TD::recurser(iterations, &depth);
        sw.stop();

        ASSERT(TD::RECURSION_DEPTH == depth);

        cout << "getStackAddresses: user: " <<
                                 sw.accumulatedUserTime() / iterations <<
                   ", wall: " << sw.accumulatedWallTime() / iterations << endl;
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (!stringstreamUsed) {
        ASSERT(0 == defaultAllocator.numAllocations());
    }

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
