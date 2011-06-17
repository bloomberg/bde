// baesu_stacktraceutil.t.cpp                                         -*-C++-*-
#include <baesu_stacktraceutil.h>

#include <baesu_objectfileformat.h>
#include <baesu_stackaddressutil.h>
#include <baesu_stacktrace.h>

#include <bdef_function.h>
#include <bdema_sequentialallocator.h>
#include <bdesu_fileutil.h>
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

// 'getStackAddresses' will not be able to trace through our stack frames if
// we're optimized on Windows

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
// [ 1] loadStackTraceFromAddressArray
// [ 1] loadStackTraceFrameStack
// [ 2] printFormatted
// [ 3] loadStackTraceFromAddressArray twice
// [ 4] loadStackTraceFrameStack twice
// [ 5] recursive deep twice
// [ 6] multiple times
// [ 7] large symbols
// [ 8] output of trace with fprintf rather than stream
// [ 9] line #'s and offsets
// [10] usage 1
// [11] usage 2
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
typedef baesu_StackTrace               ST;
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

static bslma_TestAllocator ota;
static bdema_SequentialAllocator ta(&ota);
static bslma_TestAllocator defaultAllocator;

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
        P(str.length());  P(str);
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
    LOOP_ASSERT(st.length(), st.length() > 0);

    bool reachedMain = false;   // all the routines above 'main' are pretty
                                // small
    for (int i = 0; i < st.length(); ++i) {
        const Frame& frame = st[i];

        ASSERT(frame.address());
        ASSERT((PLAT_WIN && !DEBUG_ON) || frame.libraryFileName().c_str());

        // The very last symbol might be weird on some platforms, so tolerate
        // that.

        if (i == st.length() - 1) {
            continue;
        }

        ASSERT((PLAT_WIN && !DEBUG_ON) || frame.isSymbolNameKnown());
        if (npos != frame.symbolName().find("main")) {
            reachedMain = true;
        }

        if (!PLAT_WIN || DEBUG_ON) {
            LOOP_ASSERT(i, frame.isOffsetFromSymbolKnown());

            UintPtr offset = frame.offsetFromSymbol();
            const unsigned maxOffset = PLAT_HP ? 2048 : 1024;
            LOOP2_ASSERT(i, offset, offset > 0);
            LOOP2_ASSERT(i, offset, reachedMain || offset < maxOffset);
        }

        if (!FORMAT_ELF && DEBUG_ON) {
            ASSERT(frame.isSourceFileNameKnown());
            ASSERT(frame.lineNumber() > 0);
        }

//#endif
    }
}

                                // ------
                                // case 8
                                // ------

static
void case_8_top()
{
    FILE *fp = stdout;
    if (!PLAT_WIN && !verbose) {
        fp = fopen("/dev/null", "w");
        ASSERT(fp);
    }

    for (bool demangle = false; true; demangle = !demangle) {
        ST st;
        int rc = Util::loadStackTraceFromStack(&st, 2000, demangle);
        LOOP_ASSERT(rc, 0 == rc);
        if (0 == rc) {
            testStackTrace(st);

            for (int i = 0; i < st.length(); ++i) {
                const baesu_StackTraceFrame& frame = st[i];
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
                            frame.isSymbolNameKnown()
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
void case_8_recurse(int *depth)
{
    if (--*depth <= 0) {
        case_8_top();
    }
    else {
        case_8_recurse(depth);
    }

    ++*depth;
}

                                // ------
                                // case 7
                                // ------

#ifdef BSLS_PLATFORM__OS_UNIX
// The goal here is to create an identifier > 32,000 bytes
// and < '((1 << 15) - 64)' bytes long.

// SYM07_50 followed by '::' should expand to about 50 chars of identifier
#define SYM07_50(A, B, C)                                                     \
    n23456789012345678901234567890123456789012345##A##B##C

#define NS07_50(A, B, C)                                                      \
    namespace SYM07_50(A, B, C) {

#define SYM07_1000(D, E)                                                      \
    SYM07_50(D,E,a):: SYM07_50(D,E,f):: SYM07_50(D,E,k):: SYM07_50(D,E,p)::   \
    SYM07_50(D,E,b):: SYM07_50(D,E,g):: SYM07_50(D,E,l):: SYM07_50(D,E,q)::   \
    SYM07_50(D,E,c):: SYM07_50(D,E,h):: SYM07_50(D,E,m):: SYM07_50(D,E,r)::   \
    SYM07_50(D,E,d):: SYM07_50(D,E,i):: SYM07_50(D,E,n):: SYM07_50(D,E,s)::   \
    SYM07_50(D,E,e):: SYM07_50(D,E,j):: SYM07_50(D,E,o):: SYM07_50(D,E,t)

#define NS07_1000(D, E)                                                       \
    NS07_50(D, E, a)  NS07_50(D, E, f)  NS07_50(D, E, k)  NS07_50(D, E, p)    \
    NS07_50(D, E, b)  NS07_50(D, E, g)  NS07_50(D, E, l)  NS07_50(D, E, q)    \
    NS07_50(D, E, c)  NS07_50(D, E, h)  NS07_50(D, E, m)  NS07_50(D, E, r)    \
    NS07_50(D, E, d)  NS07_50(D, E, i)  NS07_50(D, E, n)  NS07_50(D, E, s)    \
    NS07_50(D, E, e)  NS07_50(D, E, j)  NS07_50(D, E, o)  NS07_50(D, E, t)

#define ENDNS07_1000   } } } } }   } } } } }   } } } } }   } } } } }

#define SYM07_10000(X)                                                        \
    SYM07_1000(X, a)::SYM07_1000(X, d)::SYM07_1000(X, g)::SYM07_1000(X, j)::  \
    SYM07_1000(X, b)::SYM07_1000(X, e)::SYM07_1000(X, h)::SYM07_1000(X, k)::  \
    SYM07_1000(X, c)::SYM07_1000(X, f)

#define NS07_10000(X)                                                         \
    NS07_1000(X, a)   NS07_1000(X, d)   NS07_1000(X, g)   NS07_1000(X, j)     \
    NS07_1000(X, b)   NS07_1000(X, e)   NS07_1000(X, h)   NS07_1000(X, k)     \
    NS07_1000(X, c)   NS07_1000(X, f)

#define ENDNS07_10000                                                         \
    ENDNS07_1000      ENDNS07_1000      ENDNS07_1000      ENDNS07_1000        \
    ENDNS07_1000      ENDNS07_1000      ENDNS07_1000      ENDNS07_1000        \
    ENDNS07_1000      ENDNS07_1000

#define SYM07_32000                                                           \
    SYM07_10000(a) :: SYM07_10000(b) :: SYM07_10000(c) ::                     \
    SYM07_1000(d,a):: SYM07_1000(d,b)

#define NS07_32000                                                            \
    NS07_10000(a)     NS07_10000(b)     NS07_10000(c)                         \
    NS07_1000(d,a)    NS07_1000(d,b)

#define ENDNS07_32000                                                         \
    ENDNS07_1000     ENDNS07_1000                                             \
    ENDNS07_10000    ENDNS07_10000    ENDNS07_10000

# define SYM07    SYM07_32000
# define NS07     NS07_32000
# define ENDNS07  ENDNS07_32000
static const size_t case07MinLen = 32000;

#else
// WINDOWS

# define SYM07_50(A, B, C)                                                    \
    n23456789012345678901234567890123456789012345##A##B##C

# define NS07_50(A, B, C)                                                     \
    namespace SYM07_50(A, B, C) {

# define SYM07_900(D, E)                                                      \
    SYM07_50(D,E,a):: SYM07_50(D,E,f):: SYM07_50(D,E,k):: SYM07_50(D,E,p)::   \
    SYM07_50(D,E,b):: SYM07_50(D,E,g):: SYM07_50(D,E,l):: SYM07_50(D,E,q)::   \
    SYM07_50(D,E,c):: SYM07_50(D,E,h):: SYM07_50(D,E,m):: SYM07_50(D,E,r)::   \
    SYM07_50(D,E,d):: SYM07_50(D,E,i):: SYM07_50(D,E,n):: SYM07_50(D,E,s)::   \
    SYM07_50(D,E,e):: SYM07_50(D,E,j)

# define NS07_900(D, E)                                                       \
    NS07_50(D, E, a)  NS07_50(D, E, f)  NS07_50(D, E, k)  NS07_50(D, E, p)    \
    NS07_50(D, E, b)  NS07_50(D, E, g)  NS07_50(D, E, l)  NS07_50(D, E, q)    \
    NS07_50(D, E, c)  NS07_50(D, E, h)  NS07_50(D, E, m)  NS07_50(D, E, r)    \
    NS07_50(D, E, d)  NS07_50(D, E, i)  NS07_50(D, E, n)  NS07_50(D, E, s)    \
    NS07_50(D, E, e)  NS07_50(D, E, j)

# define ENDNS07_900   } } } } }   } } } } }   } } } } }   } } }

# define SYM07    SYM07_900(a, b)::SYM07_900(a, c)::                          \
                  SYM07_50(1,2,3)::SYM07_50(1,2,4)::SYM07_50(1,2,5)::         \
                  n23456789112345678921234567893123456

# define NS07     NS07_900(a, b) NS07_900(a, c)                               \
                  NS07_50(1,2,3)   NS07_50(1,2,4)   NS07_50(1,2,5)            \
                  namespace n23456789112345678921234567893123456 {

# define ENDNS07  ENDNS07_900 ENDNS07_900 } } } }
static const size_t case07MinLen = 1999;

#endif
# define SYM07Q(s)  #s

NS07    // open namespace

static inline
const char *nullGuard(const char *pc)
{
    return pc ? pc : "";
}

void case_07_top()
{
    size_t maxMangled   = 0;
    size_t maxDemangled = 0;

    ST st;
    int rc = Util::loadStackTraceFromStack(&st, 2000);       // always demangle
    LOOP_ASSERT(rc, 0 == rc);

    const char *maxMangledStr = "";
    for (int i = 0; i < st.length(); ++i) {
        const baesu_StackTraceFrame& frame = st[i];

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

    if (maxMangled < case07MinLen) {
        *out_p << "SYM07 = ";
        *out_p << nullGuard(SYM07Q(SYM07));
        *out_p << bsl::endl;
        *out_p << "mangledSymbolName = '";
        *out_p << nullGuard(maxMangledStr);
        *out_p << "'\n";
    }
    if (!PLAT_WIN || DEBUG_ON) {
        LOOP_ASSERT(maxMangled,   maxMangled   >= case07MinLen);
        LOOP_ASSERT(maxDemangled, maxDemangled >= case07MinLen);
    }

    if (veryVerbose || problem()) {
        Util::printFormatted(*out_p, st);
    }
}

ENDNS07  // close namespace

                                // ------
                                // case 5
                                // ------

void case_5_top(bool demangle)
{
    ST st;
    bsls_Stopwatch sw;
    sw.start(true);
    int rc = Util::loadStackTraceFromStack(&st, 2000, demangle);
    LOOP_ASSERT(rc, 0 == rc);
    sw.stop();
    if (0 == rc) {
        testStackTrace(st);

        Util::printFormatted(*out_p, st);
        *out_p << cc("User time: ") << sw.accumulatedUserTime() <<
                cc(", wall time: ") << sw.accumulatedWallTime() << endl;

#if defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(bsls_PLATFORM__CMP_GNU)
        demangle = false;    // demangling never happens with Sun CC
#endif

        if (DEBUG_ON || !PLAT_WIN) {
            // check that the names are right

            bool dot = '.' == *st[0].symbolName().c_str();

            const char *match = ".case_5_top";
            match += !dot;
            int len = bsl::strlen(match);
            const char *sn = st[0].symbolName().c_str();
            LOOP3_ASSERT(sn, match, len,
                                   !demangle || !bsl::strncmp(sn, match, len));
            LOOP2_ASSERT(sn, match,              bsl::strstr( sn, match));

            if (!FORMAT_ELF && DEBUG_ON) {
                // 'case_5_top' is global -- elf can't find source file names
                // for globals

                const char *sfnMatch = "baesu_stacktraceutil.t.cpp";
                const char *sfn = st[0].sourceFileName().c_str();
                sfn = nullGuard(sfn);

                int sfnMatchLen = bsl::strlen(sfnMatch);
                int sfnLen = bsl::strlen(sfn);
                sfn += bsl::max(0, sfnLen - sfnMatchLen);

                LOOP2_ASSERT(sfn, sfnMatch, !bsl::strcmp(sfn, sfnMatch));
            }

            match = ".case_5_bottom";
            match += !dot;
            len = bsl::strlen(match);

            bool finished = false;
            int recursersFound = 0;
            for (int i = 1; i < st.length(); ++i) {
                sn = st[i].symbolName().c_str();
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
                    // 'case_5_bottom' is static, so the source file name will
                    // be known on elf, thus it will be known for all
                    // platforms.

                    const char *sfnMatch = "baesu_stacktraceutil.t.cpp";
                    const char *sfn = st[i].sourceFileName().c_str();

                    int sfnMatchLen = bsl::strlen(sfnMatch);
                    int sfnLen = bsl::strlen(sfn);
                    sfn += bsl::max(0, sfnLen - sfnMatchLen);

                    LOOP2_ASSERT(sfn, sfnMatch, !bsl::strcmp(sfn, sfnMatch));
                }
            }

            ASSERT(recursersFound > 0);
            ASSERT(finished);

            if (!verbose && problem()) {
                Util::printFormatted(*out_p, st);
            }
        }
    }
}

static
void case_5_bottom(bool demangle, int *depth)
{
    if (--*depth <= 0) {
        bdef_Function<void(*)(bool)> func = &case_5_top;
        func(demangle);
    }
    else {
        case_5_bottom(demangle, depth);
    }

    ++*depth;
}


                                // ------
                                // case 4
                                // ------

bool case_4_top_called = false;

static
void case_4_top(bool demangle)
{
    if (case_4_top_called) return;
    case_4_top_called = true;

    ST st;
    int rc = Util::loadStackTraceFromStack(&st, 100, demangle);
    LOOP_ASSERT(rc, 0 == rc);
    if (0 == rc) {
        testStackTrace(st);

        bslma_TestAllocator ta;
        bsl::vector<const char *> matches(&ta);
        matches.push_back("case_4_top");
        matches.push_back("middle");
        matches.push_back("bottom");
        matches.push_back("main");

        bsl::stringstream os(&ta);
        Util::printFormatted(os, st);
        bsl::string str(&ta);
        {
            bslma_DefaultAllocatorGuard guard(&ta);
            str = os.str();
        }
        checkOutput(str, matches);
        problem();    // set 'out_p' if problem

        Util::printFormatted(*out_p, st);
    }
}

namespace CASE_4 {

bool middleCalled = false;

void middle(bool demangle)
{
    if (middleCalled) return;
    middleCalled = true;

    case_4_top_called = false;

    for (int i = 0; i < 1024; ++i) {
        if (i & 0xabcd) {
            case_4_top(demangle);
            i += 1024 << 4;
        }
    }

    ASSERT(case_4_top_called);
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

}  // close namespace CASE_4

                                // ------
                                // case 3
                                // ------

static bool calledCase5Top = false;

static
void case_3_Top(bool demangle)
{
    if (calledCase5Top) return;
    calledCase5Top = true;

    enum { MAX_FRAMES = 100 };
    void *addresses[MAX_FRAMES];
    ST st;

    int numAddresses = Address::getStackAddresses(addresses, MAX_FRAMES);

    int rc = Util::loadStackTraceFromAddressArray(&st,
                                                  addresses,
                                                  numAddresses,
                                                  demangle);
    LOOP_ASSERT(rc, 0 == rc);
    if (0 == rc) {
        testStackTrace(st);

        bslma_TestAllocator ta;
        bsl::vector<const char *> matches(&ta);
        matches.push_back("case_3_Top");
        matches.push_back("upperMiddle");
        matches.push_back("lowerMiddle");
        matches.push_back("bottom");
        matches.push_back("main");

        bsl::stringstream os(&ta);
        Util::printFormatted(os, st);
        bsl::string str(&ta);
        {
            bslma_DefaultAllocatorGuard guard(&ta);
            str = os.str();
        }
        checkOutput(str, matches);
        if (verbose || problem()) {
            *out_p << str;
        }
    }
}

namespace CASE_3 {

bool calledUpperMiddle = false;

void upperMiddle(bool demangle, int i)
{
    if (calledUpperMiddle) return;
    calledUpperMiddle = true;

    calledCase5Top = false;

    for (int j = 0; j < 100; ++j) {
        if (j & 16) {
            i *= i;
            case_3_Top(demangle);
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

}  // close namespace CASE_3

                                // ------
                                // case 2
                                // ------

namespace CASE_2 {

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

    ST st;
    Util::loadStackTraceFromStack(&st);

    {
        bsl::string strA(alloc);
        bsl::string strB(alloc);
        {
            bsl::stringstream ssA(alloc);
            bsl::ostream& ssARef = ssA << st;
            ASSERT(&ssA == &ssARef);

            bslma_DefaultAllocatorGuard guard(alloc);
            strA = ssA.str();
            checkOutput(strA, matches);
        }

        {
            bsl::stringstream ssB(alloc);
            bsl::ostream& ssBRef = st.print(ssB, 0, -1);
            ASSERT(&ssB == &ssBRef);

            bslma_DefaultAllocatorGuard guard(alloc);
            strB = ssB.str();
            checkOutput(strB, matches);
        }
        ASSERT(strA == strB);
    }

    {
        bsl::stringstream ssC(alloc);
        bsl::ostream& ssCRef = Util::printFormatted(ssC, st);

        bslma_DefaultAllocatorGuard guard(alloc);

        ASSERT(&ssC == &ssCRef);
        checkOutput(ssC.str(), matches);
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
    }

    ASSERT(called);
}

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
        enum { IGNORE_FRAMES = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES };

        void *addresses[3 + IGNORE_FRAMES];
        bsl::memset(addresses, 0, sizeof(addresses));
        int na = baesu_StackAddressUtil::getStackAddresses(addresses,
                                                           3 + IGNORE_FRAMES);
        na -= IGNORE_FRAMES;

        if (!PLAT_WIN || DEBUG_ON) {
            LOOP_ASSERT(na, 3 == na);

            ST st;
            int rc = Util::loadStackTraceFromAddressArray(
                                                     &st,
                                                     addresses + IGNORE_FRAMES,
                                                     na);
            ASSERT(!rc);
            LOOP_ASSERT(st.length(), st.length() == na);

            const char *sn;
            sn = st[0].symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "top"));
            sn = st[1].symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "bottom"));
            sn = st[2].symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "main"));

            LOOP_ASSERT(st, !problem());
        }
    }

    {
        ST st;
        int rc = Util::loadStackTraceFromStack(&st, 3);
        ASSERT(!rc);

        if (!PLAT_WIN || DEBUG_ON) {
            LOOP_ASSERT(st.length(), 3 == st.length());

            const char *sn;
            sn = st[0].symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "top"));
            sn = st[1].symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "bottom"));
            sn = st[2].symbolName().c_str();
            LOOP_ASSERT(sn, bsl::strstr(sn, "main"));
        }

        if (problem()) {
            Util::printFormatted(*out_p, st);
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
                                    // Usage 2
                                    // -------

// Example 2: loading the stack trace from an array of stack addresses.

// In this example, we demonstrate obtaining return addresses from the stack
// using 'baesu_StackAddressUtil', and later using them to load a
// 'baesu_StackTrace' object with a description of the stack.  This approach
// may be desirable if one wants to quickly save the addresses that are the
// basis for a stack trace, postponing the more time-consuming translation of
// those addresses to more human-readable debug information until later.  To do
// this, we create an array of pointers to hold the return addresses from the
// stack, which may not be desirable if we are in a situation where there isn't
// much room on the stack.

// First, we define a routine 'recurseExample2' which will recurse the
// specified 'depth' times, then call 'traceExample2'.

static void recurseExample2(int *depth)
    // Recurse the specified 'depth' number of times, then call
    // 'traceExample2', which will print a stack trace.
{
    if (--*depth > 0) {
        recurseExample2(depth);
    }
    else {
        void traceExample2();

        traceExample2();
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a
                // loop.
}

void traceExample2()
{
    // Then, within 'traceExample2', we create a stack trace object and an
    // array 'addresses' to hold some addresses.

    baesu_StackTrace stackTrace;
    enum { ARRAY_LENGTH = 50 };
    void *addresses[ARRAY_LENGTH];

    // Next, we call 'getStackAddresses' to get the stored return addresses
    // from the stack and load them into the array 'addresses'.  The call
    // returns the number of addresses saved into the array, which will be less
    // than or equal to 'ARRAY_LENGTH'.

    int numAddresses = baesu_StackAddressUtil::getStackAddresses(
                                                         addresses,
                                                         ARRAY_LENGTH);

    // Then, we call 'loadStackTraceFromAddressArray' to initialize the
    // information in the stack trace object, such as function names, source
    // file names, and line numbers, if they are available.  The optional
    // argument, 'demanglingPreferredFlag', defaults to 'true'.

    int rc = baesu_StackTraceUtil::loadStackTraceFromAddressArray(
                                                             &stackTrace,
                                                             addresses,
                                                             numAddresses);
    ASSERT(0 == rc);

    // Finally, we can print out the stack trace object using 'printFormatted',
    // or iterate through the stack trace frames, printing them out one by one.
    // In this example, we want instead to output only function names, and not
    // line numbers, source file names, or library names, so we iterate through
    // the stack trace frames and print out only the properties we want.

    for (int i = 0; i < stackTrace.length(); ++i) {
        const baesu_StackTraceFrame& frame = stackTrace[i];

        const char *symbol = frame.isSymbolNameKnown()
                           ? frame.symbolName().c_str()
                           : "--unknown__";
        *out_p << '(' << i << "): " << symbol << endl;
    }
}

                                 // -------
                                 // Usage 1
                                 // -------

// Example 1: loading stack trace with 'loadStackTraceFromStack':

// First, we define a routine 'recurseExample1' which will recurse the
// specified 'depth' times, then call 'traceExample1'.

void recurseExample1(int *depth)
    // Recurse the specified 'depth' number of times, then call
    // 'traceExample1'.
{
    if (--*depth > 0) {
        recurseExample1(depth);
    }
    else {
        void traceExample1();

        traceExample1();
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a loop.
}

// Then, we define 'traceExample1', which will print a stack trace:

void traceExample1()
{
// Next, we create a 'baesu_StackTrace' object and call
// 'loadStackTraceFrameStack' to load the information from the stack of the
// current thread into the stack trace object.
//
// In this call to 'loadStackTraceFromStack', 'maxFrames' defaults to 1024 and
// 'demanglingPreferredFlag' defaults to 'true', meaning that the function will
// attempt to demangle function names.  Note that the object 'stackTrace' takes
// very little room on the stack, allocating most of its memory directly from
// virtual memory without going through the heap, minimizing potential
// complications due to stack-size limits and possible heap corruption.

    baesu_StackTrace stackTrace;
    int rc = baesu_StackTraceUtil::loadStackTraceFromStack(&stackTrace);
    ASSERT(0 == rc);

// Finally, we use 'printFormatted' to stream out the stack trace, one frame
// per line, in a concise, human-friendly format.

    baesu_StackTraceUtil::printFormatted(*out_p, stackTrace);
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

    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    // 'dummyOstream' is a way of achieving the equivalent of opening /dev/null
    // that works on Windoze.

    bsl::stringstream dummyOstream(&ta);
    if (verbose) {
        out_p = &cout;
    }
    else {
        out_p = &dummyOstream;
    }

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TWO
        //
        // Concerns:
        //  That the usage example that uses 'getStackAddresses' and
        //  'loadStackTraceFrameAddressArray' works.
        //
        // Plan: call the routines in the usage example to observe that the
        //  stack trace works.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF USAGE EXAMPLE 2\n"
                             "=======================\n";

        // Call 'example2' with will recurse 'depth' times, then print a stack
        // trace.

        int depth = 5;
        recurseExample2(&depth);
        ASSERT(5 == depth);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE ONE
        //
        // Concerns:
        //  That the usage example that uses 'loadStackTraceFrameStack' works.
        //
        // Plan: call the routines in the usage example to observe that the
        //  stack trace works.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF USAGE EXAMPLE 1\n"
                             "=======================\n";

        // Call 'example1' with will recurse 'depth' times, then print a stack
        // trace.

        int depth = 5;
        recurseExample1(&depth);
        ASSERT(5 == depth);
      } break;
      case 9: {
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

        ST  stackTraces[DATA_POINTS];
        int lineNumbers[DATA_POINTS];

        // Note that the '__LINE__' assignments *MUST* occur on the lines after
        // the 'loadStackTraceFrameStack' calls.

        int rc;
        rc = Util::loadStackTraceFromStack(&stackTraces[0], 100, false);
        LOOP_ASSERT(rc, 0 == rc);                  lineNumbers[0] = __LINE__;

        rc = Util::loadStackTraceFromStack(&stackTraces[1], 100, false);
        LOOP_ASSERT(rc, 0 == rc);                  lineNumbers[1] = __LINE__;

        rc = Util::loadStackTraceFromStack(&stackTraces[2], 100, false);
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
            const baesu_StackTraceFrame& frame = stackTrace[0];

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
                                          stackTraces[i][0] << endl;
            }
        }
      }  break;
      case 8: {
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
        case_8_recurse(&depth);
        ASSERT(5 == depth);
      }  break;
      case 7: {
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

        SYM07::case_07_top();
      }  break;
      case 6: {
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
        case_5_bottom(false, &depth);    // no demangle
        ASSERT(startDepth  == depth);

        depth *= 2;
        startDepth *= 2;
        case_5_bottom(true,  &depth);    // demangle
        ASSERT(startDepth  == depth);

        CASE_4::bottomCalled = false;
        CASE_4::bottom(false, 3.7);    // no demangling
        ASSERT(CASE_4::bottomCalled);

        CASE_4::bottomCalled = false;
        CASE_4::bottom(true,  3.7);    // demangling
        ASSERT(CASE_4::bottomCalled);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST OF RECURSIVE DEEP TRACE
        //
        // Concerns:
        //: 1 That the stack trace utility will work properly on a deep
        //:   stack trace.
        //
        // Plan:
        //: 1 Deeply recurse within a routine, then call the routine
        //:   'case_5_top', which will create a stack trace object and populate
        //:   it using 'loadStackTraceFrameStack'.
        //: 2 Verify that the source file names and symbol names in the stack
        //:   trace object are as expected.
        //: 3 Repeat the test twice, once with demangling and once without.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF RECURSIVE DEEP TRACE WITH FUNCTOR\n"
                             "=========================================\n";

        if (verbose) cout << "Indicate depth by passing an int for 1st v\n";

        int startDepth = 10;
        if (verbose && isdigit(*argv[2])) {
            startDepth = bsl::atoi(argv[2]);
        }
        int depth = startDepth;

        case_5_bottom(false, &depth);    // no demangle
        ASSERT(startDepth == depth);

        case_5_bottom(true,  &depth);    // demangle
        ASSERT(startDepth == depth);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST OF loadStackTraceFrameStack
        //
        // Concern:
        //: 1 That 'loadStackTraceFrameStack' produces a proper stack trace.
        //
        // Plan:
        //: 1 Create a stack trace object, then populate it using
        //:   'loadStackTraceFrameStack'.
        //: 2 Do sanity checks on the stack trace object.
        //: 3 Output the stack trace object to a stringstream using
        //:   'printFormatted'.
        //: 4 Confirm that the expected routine names are in the string prodced
        //:   by the stringstream, in the right order.
        //: 5 Repeat the test twice, with and without demangling enabled.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                        "TEST OF DIRECT CALL TO loadStackTraceFrameStack\n"
                        "===============================================\n";

        namespace TC = CASE_4;

        TC::bottomCalled = false;
        TC::bottom(false, 3.7);    // no demangling
        ASSERT(TC::bottomCalled);

        TC::bottomCalled = false;
        TC::bottom(true,  3.7);    // demangling
        ASSERT(TC::bottomCalled);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST OF STACK TRACE USING 'getStackAddresses' /
        //                                    'loadStackTraceFrameAddressArray'
        //
        // Concerns:
        //: 1 That the combination of 'getStackAddresses' and
        //:   'loadStackTraceFrameAddressArray' produces a proper stack trace.
        //
        // Plan:
        //: 1 Call 'getStackAddresses' followed by
        //:   'loadStackTraceFrameAddressArray' to populate a stack trace
        //:   object.
        //: 2 Run sanity checks on the stack trace.
        //: 3 Output the stack trace to a stringstream using '<<'.
        //: 4 Verify that the expected routine names were output to the
        //:   stringstream, in the right order.
        //: 5 Repeat the whole test twice, once with & without demangling
        //:   enabled.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF DIRECT CALL TO getStackAddresses\n"
                             "========================================\n";

        namespace TC = CASE_3;

        TC::calledBottom = false;
        (void) TC::bottom(false);    // no demangling
        ASSERT(TC::calledBottom);

        TC::calledBottom = false;
        (void) TC::bottom(true);     // demangling
        ASSERT(TC::calledBottom);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRINT, OPERATOR<<, PRINTFORMATTED AND STREAM TEST
        //
        // Concerns:
        //: 1 That 'printFormatted' writes to the specified 'ostream'.
        //: 2 That the strings that are output are non-zero.
        //: 3 That the streams returned are the same streams that are passed
        //:   in.
        //: 4 That the output contains the expected routine
        //:   names.
        //: 5 That neither function allocates any memory from the default
        //:   allocator.
        //
        // Plan:
        //: 1 Create a stack trace object.
        //: 2 Output the stack trace object to a stringstream using
        //:   'print', 'operator<<', and 'printFormatted'.
        //:   1 Verify that the string produced contains the expected routine
        //:     names, in the right order.
        //:   2 Verify that the reference returned by the output function
        //:     operation refers to the stream passed in.
        //: 3 Verify that the default allocator was not used by any of these
        //:   functions.
        // --------------------------------------------------------------------

        namespace TC = CASE_2;

        if (verbose) cout << "Print and Streamout Test\n"
                             "========================\n";

        TC::bottom(&ta);

        ASSERT(0 == defaultAllocator.numAllocations());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // LOADSTACKTRACEFROMADDRESSARRAY & LOADSTACKTRACEFROMSTACK TEST
        //
        // Concerns:
        //: 1 That 'loadStackTraceFromAddressArray' correctly loads and
        //:   resolves addresses passed in.
        //: 2 That 'loadStackTraceFromStack' correctly loads a stack trace
        //:   directly from the stack.
        // Plan:
        //: 1 Get several subroutines deep on the stack.
        //:   1 load a stack trace using
        //:     'baesu_StackAddressUtil::getStackAddresses' and then initialize
        //:     a stack trace using 'loadStackTraceFromAddressArray'.
        //:   2 verify the subroutine names on the stack trace.
        //:   3 load another stack trace using 'loadStackTraceFromStack'.
        //:   4 verify the subroutine names on the stack trace.
        // --------------------------------------------------------------------

        namespace TC = CASE_1;

        if (verbose) cout << "Manipulator & Accessor Test\n"
                             "===========================\n";

        {
            ST st;
            ASSERT(0 == st.length());

            ASSERT(st.allocator());
            ASSERT(st.allocator() != &ota);
            ASSERT(st.allocator() != &ta);
            ASSERT(st.allocator() != &defaultAllocator);
        }

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
