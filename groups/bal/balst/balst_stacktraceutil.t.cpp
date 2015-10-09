// balst_stacktraceutil.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceutil.h>

#include <balst_objectfileformat.h>
#include <balst_stackaddressutil.h>
#include <balst_stacktrace.h>

#include <bdlb_string.h>

#include <bdlma_sequentialallocator.h>

#include <bdlf_function.h>
#include <bdlt_currenttime.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmt_threadutil.h>

#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef BSLS_PLATFORM_OS_CYGWIN

#ifdef BSLS_PLATFORM_OS_WINDOWS
// for 'EnumWindows'

# pragma comment(lib, "user32.lib")
# include <windows.h>

// 'getStackAddresses' will not be able to trace through our stack frames if
// we're optimized on Windows

# pragma optimize("", off)

#else

#include <unistd.h>    // sbrk

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
// [10] multithreaded
// [11] hexStackTrace
// [12] printHexStackTrace
// [13] heap memory leak test
// [14] usage 1
// [15] usage 2
// [16] usage 3
//-----------------------------------------------------------------------------

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
//                    GLOBAL HELPER #DEFINES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//          GLOBAL HELPER TYPES, CLASSES, and CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef balst::StackAddressUtil         Address;
typedef balst::StackTrace               ST;
typedef balst::StackTraceFrame          Frame;
typedef balst::StackTraceUtil           Util;

#if   defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
    enum { FORMAT_ELF = 1, FORMAT_WINDOWS = 0, FORMAT_XCOFF = 0,
           FORMAT_DLADDR = 0 };

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
    enum { FORMAT_ELF = 0, FORMAT_WINDOWS = 0, FORMAT_XCOFF = 0,
           FORMAT_DLADDR = 1 };
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=0,
           PLAT_DARWIN = 1 };
#elif defined(BALST_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
    enum { FORMAT_ELF = 0, FORMAT_WINDOWS = 1, FORMAT_XCOFF = 0,
           FORMAT_DLADDR = 0 };
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=0, PLAT_WIN=1,
           PLAT_DARWIN = 0 };
#elif defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)
    enum { FORMAT_ELF = 0, FORMAT_WINDOWS = 0, FORMAT_XCOFF = 1,
           FORMAT_DLADDR = 0 };
    enum { PLAT_SUN=0, PLAT_LINUX=0, PLAT_HP=0, PLAT_AIX=1, PLAT_WIN=0,
           PLAT_DARWIN = 0 };
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

static bslma::TestAllocator ota;
static bdlma::SequentialAllocator ta(&ota);
static bslma::TestAllocator defaultAllocator;

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
void testStackTrace(const balst::StackTrace& st)
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
            const unsigned int maxOffset = PLAT_HP ? 2048 : 1024;
            LOOP2_ASSERT(i, offset, offset > 0);
            LOOP2_ASSERT(i, offset, reachedMain || offset < maxOffset);
        }

        if (!FORMAT_ELF && !FORMAT_DLADDR && DEBUG_ON && !reachedMain) {
            ASSERT(frame.isSourceFileNameKnown());
            ASSERT(frame.lineNumber() > 0);
        }

//#endif
    }
}

                                // -------
                                // case 11
                                // -------

namespace BALST_STACKTRACEUTIL_TEST_CASE_11 {

bool straightTrace = true;

void stackTop();    // forward dclaration

void recurseABunchOfTimes(int *depth, int, void *, int, void *)
{
    if (--*depth <= 0) {
        stackTop();
    }
    else {
        recurseABunchOfTimes(depth, 0, depth, 0, depth);
    }

    ++*depth;
}

void stackTop()
{
    bsl::stringstream ss;

    if (straightTrace) {
        ss << Util::hexStackTrace;
    }
    else {
        bsl::ostream *ps = &Util::printHexStackTrace(ss, '\n');
        ASSERT(&ss == ps);
    }

    int numDelims = 0;
    char delim = straightTrace ? ' ' : '\n';
    const bsl::string& str = ss.str();
    for (unsigned int u = 0; u < str.length(); ++u) {
        numDelims += delim == str[u];
    }
    LOOP_ASSERT(numDelims, numDelims >= 6);

    if (veryVerbose) {
        *out_p << ss.str() << bsl::endl;
    }

    bsl::vector<void *> v;

    for (const char *pc = str.c_str(); *pc; ) {
        if ('0' != pc[0] || 'x' != pc[1]) {
            ASSERT(0 && "garbled output");
            break;
        }

        UintPtr u;

        int rc = sscanf(pc + 2, SIZE_T_CONTROL_STRING, &u);
        ASSERT(1 == rc);

        v.push_back((void *) u);

        const char *next = strchr(pc, delim);
        pc = next ? next + 1 : "";
    }

    LOOP_ASSERT(v.size(), v.size() >= 7);

    if (PLAT_WIN && !DEBUG_ON) {
        return;                                                       // RETURN
    }

    if (!v.empty()) {
        ST st;

        int rc = Util::loadStackTraceFromAddressArray(&st, &v[0], v.size());
        ASSERT(0 == rc);

        if (veryVerbose) {
            *out_p << st;
        }

        int numStackTop                          = 0;
        int numRecurseABunchOfTimes              = 0;
        int numBALST_STACKTRACEUTIL_TEST_CASE_11 = 0;
        int numMain                              = 0;

        for (int i = 0; i < st.length(); ++i) {
            const char *sym = st[i].symbolName().c_str();

            numStackTop += !!bsl::strstr(sym, "stackTop");
            numRecurseABunchOfTimes +=
                                    !!bsl::strstr(sym, "recurseABunchOfTimes");
            numBALST_STACKTRACEUTIL_TEST_CASE_11 +=
                       !!bsl::strstr(sym, "BALST_STACKTRACEUTIL_TEST_CASE_11");
            numMain += !!bsl::strstr(sym, "main");
        }

        ASSERT(1 == numStackTop);
        ASSERT(5 == numRecurseABunchOfTimes)
        ASSERT(6 == numBALST_STACKTRACEUTIL_TEST_CASE_11);
        ASSERT(1 <= numMain);

        if (veryVerbose) {
            P_(numStackTop);    P_(numRecurseABunchOfTimes);
            P_(numBALST_STACKTRACEUTIL_TEST_CASE_11);    P(numMain);
        }
    }
}

}  // close namespace BALST_STACKTRACEUTIL_TEST_CASE_11

                                // -------
                                // case 10
                                // -------

// We want long, ccmplicated to demangle names

namespace BALST_STACKTRACEUTIL_TEST_CASE_10 {
namespace NS_10_2 {
namespace NS_10_3 {
namespace NS_10_4 {

#undef  BALST_STACKTRACEUTIL_TEST_10_SYMBOLS
#if defined(BDE_BUILD_TARGET_DBG) || !defined(BSLS_PLATFORM_OS_WINDOWS)
#define BALST_STACKTRACEUTIL_TEST_10_SYMBOLS
#endif

void topOfTheStack(void *, void *, void *, void *)
{
    ST st;

    int rc = Util::loadStackTraceFromStack(&st, 2000, true);
    LOOP_ASSERT(rc, 0 == rc);

#if defined(BALST_STACKTRACEUTIL_TEST_10_SYMBOLS)
    const int len = st.length();

    bool tots = false;
    bool rabo = false;
    bool lffs = false;
    bool tc10 = false;
    bool ns2  = false;
    bool ns3  = false;
    bool ns4  = false;

    for (int i = 0; i < len; ++i) {
        const bsl::string& s = st[i].symbolName();
        const bsl::size_t npos = bsl::string::npos;

        if (!tots && npos != s.find("topOfTheStack")) {
            tots = true;
        }
        if (!rabo && npos != s.find("recurseABunchOfTimes")) {
            rabo = true;
        }
        if (!lffs && npos != s.find("loopForFourSeconds")) {
            lffs = true;
        }
        if (!tc10 && npos != s.find("BALST_STACKTRACEUTIL_TEST_CASE_10")) {
            tc10 = true;
        }
        if (!ns2  && npos != s.find("NS_10_2")) {
            ns2  = true;
        }
        if (!ns3  && npos != s.find("NS_10_3")) {
            ns3  = true;
        }
        if (!ns4  && npos != s.find("NS_10_4")) {
            ns4  = true;
        }
    }

    ASSERT(tots && rabo && lffs && tc10 && ns2 && ns3 && ns4);
#endif
}

void recurseABunchOfTimes(int *depth, int, void *, int, void *)
{
    if (--*depth <= 0) {
        topOfTheStack(depth, depth, depth, depth);
    }
    else {
        recurseABunchOfTimes(depth, 0, depth, 0, depth);
    }

    ++*depth;
}

void loopForFourSeconds()
{
    bsls::TimeInterval start = bdlt::CurrentTime::now();

    do {
        int depth = 20;
        for (int i = 0; i < 100; ++i) {
            recurseABunchOfTimes(&depth, 0, &i, 0, &i);
            ASSERT(20 == depth);
        }
    } while ((bdlt::CurrentTime::now() - start).totalSecondsAsDouble() < 4);
}

}  // close namespace NS_10_4
}  // close namespace NS_10_3
}  // close namespace NS_10_2
}  // close namespace BALST_STACKTRACEUTIL_TEST_CASE_10

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
                const balst::StackTraceFrame& frame = st[i];
                const char *lnBegin = frame.libraryFileName().c_str();
                const char *ln = lnBegin + bsl::strlen(lnBegin);
                while (ln > lnBegin && '/' != ln[-1] && '\\' != ln[-1]) {
                    --ln;
                }
                if (bsl::strstr(lnBegin, "lib")) {
                    ln = lnBegin;
                }
                const char *snBegin = frame.sourceFileName().c_str();
                const char *sn = snBegin + bsl::strlen(snBegin);
                while (sn > snBegin && '/' != sn[-1] && '\\' != sn[-1]) {
                    --sn;
                }

                if (!PLAT_WIN || verbose) {
                    fprintf(fp,
                            "(%d) %s+0x" SIZE_T_CONTROL_STRING " at 0x"
                                     SIZE_T_CONTROL_STRING " in %s:%d in %s\n",
                            i,
                            frame.isSymbolNameKnown()
                                                   ? frame.symbolName().c_str()
                                                   : "-- unknown",
                            (UintPtr) frame.offsetFromSymbol(),
                            (UintPtr) frame.address(),
                            sn,
                            frame.lineNumber(),
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

#if defined(BSLS_PLATFORM_OS_UNIX) &&                                         \
    !defined(BSLS_PLATFORM_OS_DARWIN) &&                                      \
    !(defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CPU_SPARC))
// The goal here is to create an identifier > 32,000 bytes
// and < '((1 << 15) - 64)' bytes long.

// I think (but am not positive) that the C++ std guarantees support for
// individual id's up to 1K long, and namespaces nested up to 255 deep.  Here
// we take it's 158 long, nested in namespace 200 deep.

#define SYM07_40   fortycharsybolabcdefghijklmnopqrstuvwxyz
#define SYM07_36   thirtysevencharsymbolabcdefghijklm__

#define SYM07_158(A, B)                                                       \
        SYM07_CAT6(SYM07_40, SYM07_40, SYM07_40, SYM07_36, A, B)

#define SYM07_CAT6(     A, B, C, D, E, F)     SYM07_CAT6_IMPL(A, B, C, D, E, F)
#define SYM07_CAT6_IMPL(A, B, C, D, E, F)                                     \
        A ## B ## C ## D ## E ## F

#define NS07_158(A, B)                                                        \
        namespace SYM07_158(A, B) {

#define SYM07_1598(A)                                                         \
        SYM07_158(A, a)  ::  SYM07_158(A, b)  ::  SYM07_158(A, c)  ::         \
        SYM07_158(A, d)  ::  SYM07_158(A, e)  ::  SYM07_158(A, f)  ::         \
        SYM07_158(A, g)  ::  SYM07_158(A, h)  ::  SYM07_158(A, i)  ::         \
        SYM07_158(A, j)

#define NS07_1598(A)                                                          \
        NS07_158(A, a)       NS07_158(A, b)       NS07_158(A, c)              \
        NS07_158(A, d)       NS07_158(A, e)       NS07_158(A, f)              \
        NS07_158(A, g)       NS07_158(A, h)       NS07_158(A, i)              \
        NS07_158(A, j)

#define ENDNS07_1598                                                          \
        }}}}  }}}  }}}

#define SYM07_31998                                                           \
        SYM07_1598(a)   ::   SYM07_1598(b)   ::   SYM07_1598(c)   ::          \
        SYM07_1598(d)   ::   SYM07_1598(e)   ::   SYM07_1598(f)   ::          \
        SYM07_1598(e)   ::   SYM07_1598(h)   ::   SYM07_1598(i)   ::          \
        SYM07_1598(j)   ::   SYM07_1598(k)   ::   SYM07_1598(l)   ::          \
        SYM07_1598(m)   ::   SYM07_1598(n)   ::   SYM07_1598(o)   ::          \
        SYM07_1598(p)   ::   SYM07_1598(q)   ::   SYM07_1598(r)   ::          \
        SYM07_1598(s)   ::   SYM07_1598(t)

#define NS07_31998                                                            \
        NS07_1598(a)         NS07_1598(b)         NS07_1598(c)                \
        NS07_1598(d)         NS07_1598(e)         NS07_1598(f)                \
        NS07_1598(e)         NS07_1598(h)         NS07_1598(i)                \
        NS07_1598(j)         NS07_1598(k)         NS07_1598(l)                \
        NS07_1598(m)         NS07_1598(n)         NS07_1598(o)                \
        NS07_1598(p)         NS07_1598(q)         NS07_1598(r)                \
        NS07_1598(s)         NS07_1598(t)

#define ENDNS07_31998                                                         \
        ENDNS07_1598         ENDNS07_1598         ENDNS07_1598                \
        ENDNS07_1598         ENDNS07_1598         ENDNS07_1598                \
        ENDNS07_1598         ENDNS07_1598         ENDNS07_1598                \
        ENDNS07_1598         ENDNS07_1598         ENDNS07_1598                \
        ENDNS07_1598         ENDNS07_1598         ENDNS07_1598                \
        ENDNS07_1598         ENDNS07_1598         ENDNS07_1598                \
        ENDNS07_1598         ENDNS07_1598

# define SYM07    SYM07_31998
# define NS07     NS07_31998
# define ENDNS07  ENDNS07_31998
static const size_t case07MinLen = 32000;

#else
// WINDOWS || DARWIN

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
        const balst::StackTraceFrame& frame = st[i];

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

void case_5_top(bool demangle, bool useTestAllocator)
{
    bslma::TestAllocator ta;
    ST stTest(&ta);
    ST stHeapBypass;

    ST& st = useTestAllocator ? stTest : stHeapBypass;

    bsls::Stopwatch sw;
    sw.start(true);
    int rc = Util::loadStackTraceFromStack(&st, 2000, demangle);
    LOOP_ASSERT(rc, 0 == rc);
    sw.stop();
    if (0 == rc) {
        testStackTrace(st);

        Util::printFormatted(*out_p, st);
        *out_p << cc("User time: ") << sw.accumulatedUserTime() <<
                cc(", wall time: ") << sw.accumulatedWallTime() << endl;

#if defined(BSLS_PLATFORM_OS_SOLARIS)                                         \
 && !(defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
        demangle = false;    // demangling never happens with Sun CC
#endif

        if (DEBUG_ON || !PLAT_WIN) {
            // check that the names are right

            bool dot = '.' == *st[0].symbolName().c_str();

            const char *match = ".case_5_top";
            match += !dot;
            int len = (int) bsl::strlen(match);
            const char *sn = st[0].symbolName().c_str();
            LOOP3_ASSERT(sn, match, len,
                                   !demangle || !bsl::strncmp(sn, match, len));
            LOOP2_ASSERT(sn, match,              bsl::strstr( sn, match));

            if (!FORMAT_ELF && !FORMAT_DLADDR && DEBUG_ON) {
                // 'case_5_top' is global -- elf can't find source file names
                // for globals

                const char *sfnMatch = "balst_stacktraceutil.t.cpp";
                const char *sfn = st[0].sourceFileName().c_str();
                sfn = nullGuard(sfn);

                int sfnMatchLen = (int) bsl::strlen(sfnMatch);
                int sfnLen = (int) bsl::strlen(sfn);
                sfn += bsl::max(0, sfnLen - sfnMatchLen);

                LOOP2_ASSERT(sfn, sfnMatch, !bsl::strcmp(sfn, sfnMatch));
            }

            demangle &= !PLAT_LINUX;    // The LInux demangler has a bug where
                                        // it fails on file-scope static
                                        // functions.

            match = ".case_5_bottom";
            match += !dot;
            len = (int) bsl::strlen(match);

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

                if (bsl::strstr(sn, "bsl") && bsl::strstr(sn, "function")) {
                    continue;
                }

                LOOP3_ASSERT(i, sn, match, bsl::strstr(sn, match));
                if (demangle && !FORMAT_DLADDR) {
                    LOOP4_ASSERT(i, sn, match, len,
                                                !bsl::strncmp(sn, match, len));
                }

                ++recursersFound;

                if (!FORMAT_DLADDR && DEBUG_ON) {
                    // 'case_5_bottom' is static, so the source file name will
                    // be known on elf, thus it will be known for all
                    // platforms other than Mach-O.

                    const char *sfnMatch = "balst_stacktraceutil.t.cpp";
                    const char *sfn = st[i].sourceFileName().c_str();

                    int sfnMatchLen = (int) bsl::strlen(sfnMatch);
                    int sfnLen = (int) bsl::strlen(sfn);
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
void case_5_bottom(bool demangle, bool useTestAllocator, int *depth)
{
    if (--*depth <= 0) {
        bsl::function<void(bool, bool)> func = &case_5_top;
        func(demangle, useTestAllocator);
    }
    else {
        case_5_bottom(demangle, useTestAllocator, depth);
    }

    ++*depth;
}


                                // ------
                                // case 4
                                // ------

bool case_4_top_called_demangle = false;
bool case_4_top_called_mangle   = false;

static
void case_4_top(bool demangle)
{
    if (demangle) {
        ASSERT(!case_4_top_called_demangle);
        case_4_top_called_demangle = true;
    }
    else {
        ASSERT(!case_4_top_called_mangle);
        case_4_top_called_mangle = true;
    }

    ST st;
    int rc = Util::loadStackTraceFromStack(&st, 100, demangle);
    LOOP_ASSERT(rc, 0 == rc);
    if (0 == rc) {
        testStackTrace(st);

        bslma::TestAllocator ta;
        bsl::vector<const char *> matches(&ta);
        matches.push_back("case_4_top");
        matches.push_back("middle");
        matches.push_back("bottom");
        matches.push_back("main");

        bsl::stringstream os(&ta);
        Util::printFormatted(os, st);
        bsl::string str(&ta);
        {
            bslma::DefaultAllocatorGuard guard(&ta);
            str = os.str();
        }
        checkOutput(str, matches);
        problem();    // set 'out_p' if problem

        Util::printFormatted(*out_p, st);
    }
}

namespace CASE_4 {

void middle(bool demangle)
{
    for (int i = 0; i < 1024; ++i) {
        if (i & 0xabc4) {
            (*foilOptimizer(case_4_top))(demangle);
            i += 1024 << 4;
        }
        else if (i > 4) {
            ASSERT(0);
        }
    }
}

void bottom(bool demangle, double x)
{
    for (int i = 0; i < (1 << 15); ++i) {
        x *= x;
        if (i & 0x1234) {
            (*foilOptimizer(middle))(demangle);
            i += 1 << 15;
        }
        else if (i > 4) {
            ASSERT(0);
        }
    }
}

}  // close namespace CASE_4

                                // ------
                                // case 3
                                // ------

static bool calledCase3TopDemangle = false;
static bool calledCase3TopMangle   = false;

static
void case_3_Top(bool demangle)
{
    if (demangle) {
        ASSERT(!calledCase3TopDemangle);
        calledCase3TopDemangle = true;
    }
    else {
        ASSERT(!calledCase3TopMangle);
        calledCase3TopMangle = true;
    }

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

        bslma::TestAllocator ta;
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
            bslma::DefaultAllocatorGuard guard(&ta);
            str = os.str();
        }
        checkOutput(str, matches);
        if (verbose || problem()) {
            *out_p << str;
        }
    }
}

namespace CASE_3 {

void upperMiddle(bool demangle)
{
    for (int j = 0; j < 100; ++j) {
        if (j & 16) {
            j *= j * j;
            (*foilOptimizer(case_3_Top))(demangle);
        }
        else if (j > 16) {
            ASSERT(0);
        }
    }
}

int lowerMiddle(bool demangle)
{
    for (int j = 0; j < 100; ++j) {
        if (j & 16) {
            j *= j;
            (*foilOptimizer(upperMiddle))(demangle);
        } else if (j > 16) {
            ASSERT(0);
        }
    }

    return 7;
}

double bottom(bool demangle)
{
    double x = 0;

    for (int i = 0; i < 100; ++i) {
        if (i & 4) {
            i *= 50;
            x = 3.7 * (*foilOptimizer(lowerMiddle))(demangle);
        } else if (i > 4) {
            ASSERT(0);
        }
    }

    return x;
}

}  // close namespace CASE_3

                                // ------
                                // case 2
                                // ------

namespace CASE_2 {

bool topCalled = false;

void top(bslma::Allocator *alloc)
    // Note that we don't get
{
    ASSERT(!topCalled);
    topCalled = true;

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

            bslma::DefaultAllocatorGuard guard(alloc);
            strA = ssA.str();
            checkOutput(strA, matches);
        }

        {
            bsl::stringstream ssB(alloc);
            bsl::ostream& ssBRef = st.print(ssB, 0, -1);
            ASSERT(&ssB == &ssBRef);

            bslma::DefaultAllocatorGuard guard(alloc);
            strB = ssB.str();
            checkOutput(strB, matches);
        }
        ASSERT(strA == strB);
    }

    {
        bsl::stringstream ssC(alloc);
        bsl::ostream& ssCRef = Util::printFormatted(ssC, st);

        bslma::DefaultAllocatorGuard guard(alloc);

        ASSERT(&ssC == &ssCRef);
        checkOutput(ssC.str(), matches);

        if (veryVerbose) cout << ssC.str();
    }
}

void bottom(bslma::Allocator *alloc)
{
    for (int i = 0; i < 0x20; ++i) {
        if ((i & 2) && (i & 4)) {
            (*foilOptimizer(top))(alloc);
            i += 0x1f;
        }
        else if (7 == i) {
            ASSERT(0);
        }
    }
}

}  // close namespace CASE_2

                                // ------
                                // case 1
                                // ------

namespace CASE_1 {

bool topCalled = false;

void top(bslma::Allocator *alloc)
{
    ASSERT(!topCalled);
    topCalled = true;

    bsl::vector<const char *> matches(alloc);
    matches.push_back("top");
    matches.push_back("bottom");
    matches.push_back("main");

    {
        enum { IGNORE_FRAMES = balst::StackAddressUtil::k_IGNORE_FRAMES };

        void *addresses[3 + IGNORE_FRAMES];
        bsl::memset(addresses, 0, sizeof(addresses));
        int na = balst::StackAddressUtil::getStackAddresses(addresses,
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

void bottom(bslma::Allocator *alloc)
{
    for (int i = 0; i < 20; ++i) {
        if ((i & 1) && (i & 2)) {
            (*foilOptimizer(&top))(alloc);
            i += 100;
        }
        else if (i & 8) {
            ASSERT(0);
            (*foilOptimizer(&top))(alloc);
        }
    }
}

}  // close namespace CASE_1

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

                                    // -------
                                    // Usage 3
                                    // -------

// Example 3: Outputting a hex stack trace.

// In this example, we demonstrate how to output return addresses from the
// stack to a stream in hex.  Note that in this case the stack trace is never
// stored to a data object -- when the 'operator<<' is passed a pointer to the
// 'hexStackTrace' function, it calls the 'hexStackTrace' function, which
// gathers the stack addresses and immediately streams them out.  After the
// 'operator<<' is finished, the stack addresses are no longer stored anywhere.

// First, we define a routine 'recurseExample3' which will recurse the
// specified 'depth' times, then call 'traceExample3'.

void traceExample3();    // forward declaration

static void recurseExample3(int *depth)
    // Recurse the specified 'depth' number of times, then call
    // 'traceExample3', which will print a stack-trace.
{
    if (--*depth > 0) {
        recurseExample3(depth);
    }
    else {
        traceExample3();
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a
                // loop.
}

void traceExample3()
{
    // Now, within 'traceExample3', we output the stack addresses in
    // hex by streaming the function pointer 'hexStackTrace' to the ostream:

    *out_p << balst::StackTraceUtil::hexStackTrace << endl;
}

                                    // -------
                                    // Usage 2
                                    // -------

// Example 2: Loading a Stack-Trace From an Array of Stack Addresses.

// In this example, we demonstrate obtaining return addresses from the stack
// using 'balst::StackAddressUtil', and later using them to load a
// 'balst::StackTrace' object with a description of the stack.  This approach
// may be desirable if one wants to quickly save the addresses that are the
// basis for a stack-trace, postponing the more time-consuming translation of
// those addresses to more human-readable debug information until later.  To do
// this, we create an array of pointers to hold the return addresses from the
// stack, which may not be desirable if we are in a situation where there isn't
// much room on the stack.

// First, we define a routine 'recurseExample2' which will recurse the
// specified 'depth' times, then call 'traceExample2'.

void traceExample2();    // forward declaration

static void recurseExample2(int *depth)
    // Recurse the specified 'depth' number of times, then call
    // 'traceExample2', which will print a stack-trace.
{
    if (--*depth > 0) {
        recurseExample2(depth);
    }
    else {
        traceExample2();
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a
                // loop.
}

void traceExample2()
{
    // Then, within 'traceExample2', we create a stack-trace object and an
    // array 'addresses' to hold some addresses.

    balst::StackTrace stackTrace;
    enum { ARRAY_LENGTH = 50 };
    void *addresses[ARRAY_LENGTH];

    // Next, we call 'balst::StackAddressUtil::getStackAddresses' to get the
    // stored return addresses from the stack and load them into the array
    // 'addresses'.  The call returns the number of addresses saved into the
    // array, which will be less than or equal to 'ARRAY_LENGTH'.

    int numAddresses = balst::StackAddressUtil::getStackAddresses(
                                                         addresses,
                                                         ARRAY_LENGTH);

    // Then, we call 'loadStackTraceFromAddressArray' to initialize the
    // information in the stack-trace object, such as function names, source
    // file names, and line numbers, if they are available.  The optional
    // argument, 'demanglingPreferredFlag', defaults to 'true'.

    int rc = balst::StackTraceUtil::loadStackTraceFromAddressArray(
                                                             &stackTrace,
                                                             addresses,
                                                             numAddresses);
    ASSERT(0 == rc);

    // Finally, we can print out the stack-trace object using 'printFormatted',
    // or iterate through the stack-trace frames, printing them out one by one.
    // In this example, we want instead to output only function names, and not
    // line numbers, source file names, or library names, so we iterate through
    // the stack-trace frames and print out only the properties we want.  Note
    // that if a string is unknown, it is represented as "", here we print it
    // out as "--unknown--" to let the user see that the name was unresolved.

    for (int i = 0; i < stackTrace.length(); ++i) {
        const balst::StackTraceFrame& frame = stackTrace[i];

        const char *symbol = frame.isSymbolNameKnown()
                           ? frame.symbolName().c_str()
                           : "--unknown__";
        *out_p << '(' << i << "): " << symbol << endl;
    }
}

                                 // -------
                                 // Usage 1
                                 // -------

// Example 1: loading stack-trace directly from the stack.

// We start by defining a routine, 'recurseExample1', that will recurse the
// specified 'depth' times, then call 'traceExample1':

void traceExample1();    // forward declaration

void recurseExample1(int *depth)
    // Recurse the specified 'depth' number of times, then call
    // 'traceExample1'.
{
    if (--*depth > 0) {
        recurseExample1(depth);
    }
    else {
        traceExample1();
    }

    ++*depth;   // Prevent compiler from optimizing tail recursion as a loop.
}

// Then, we define the function 'traceExample1', that will print a stack-trace:

void traceExample1()
{
// Now, we create a 'balst::StackTrace' object and call
// 'loadStackTraceFrameStack' to load the information from the stack of the
// current thread into the stack-trace object.
//
// In this call to 'loadStackTraceFromStack', we use the default value of
// 'maxFrames', which is at least 1024 and the default value for
// 'demanglingPreferredFlag', which is 'true', meaning that the operation will
// attempt to demangle function names.  Note that the object 'stackTrace' takes
// very little room on the stack, and by default allocates most of its memory
// directly from virtual memory without going through the heap, minimizing
// potential complications due to stack-size limits and possible heap
// corruption.

    balst::StackTrace stackTrace;
    int rc = balst::StackTraceUtil::loadStackTraceFromStack(&stackTrace);
    ASSERT(0 == rc);

// Finally, we use 'printFormatted' to stream out the stack-trace, one frame
// per line, in a concise, human-readable format.

    balst::StackTraceUtil::printFormatted(*out_p, stackTrace);
}

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

    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

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
      case 16: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE THREE
        //
        // Concerns:
        //  That the usage example that uses 'hexStackTrace' works.
        //
        // Plan: call the routines in the usage example to observe that the
        //  stack trace works.
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST OF USAGE EXAMPLE 2\n"
                             "=======================\n";

        // Call 'recurseExample3' with will recurse 'depth' times, then print
        // a hex stack trace.

        int depth = 5;
        recurseExample3(&depth);
        ASSERT(5 == depth);
      } break;
      case 15: {
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
      case 14: {
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
      case 13: {
        // --------------------------------------------------------------------
        // TESTING: Potential Memory Leak (see DRQS 42134199)
        //
        // Concerns:
        //: 1 That heap memory allocated when resolving symbols is reclaimed.
        //
        // Plan:
        //: 1 Resolve symbols repeatedly and observe, using 'sbrk', that the
        //:   stack top remains constant (note that the stack top will grow
        //:   for the first several iterations due to memory fragmentation,
        //:   but should eventually settle down into 100% of memory being
        //:   reclaimed.
        // --------------------------------------------------------------------

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
        if (verbose) cout << "Memory Leak Test is Performed on Unix Only\n"
                             "==========================================\n";
#else
        if (verbose) cout << "Memory Leak Test\n"
                             "================\n";

        bslma::TestAllocator ta;

        balst::StackTrace st(&ta);
        UintPtr heapTop = 0;
        int iterations = verbose ? 500 : 50;

        for (int ti = 0; ti < iterations;  ++ti) {
            for (int tj = 0; tj < 10; ++tj) {
                Util::loadStackTraceFromStack(&st);
                ASSERT(st.length() >= 1);
                st.resize(0);
            }

            if (4 == ti) {
                heapTop = (UintPtr) sbrk(0);
            }
            else if (ti > 4) {
                LOOP2_ASSERT((UintPtr) sbrk(0), heapTop,
                                                 (UintPtr) sbrk(0) == heapTop);
            }

            if (verbose) P((UintPtr) sbrk(0));
        }
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING PRINT HEX TRACE
        //
        // Concerns:
        //   That 'Util::printHexStackTrace;' outputs the correct function
        //   pointers.
        //
        // Plan:
        //   Do the output to a stringstream, then parse the hex pointers, put
        //   them into a stack trace and resolve them and verify that reslts
        //   in the expected symbols.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PRINT HEX STACK TRACE\n"
                             "=============================\n";

        using namespace BALST_STACKTRACEUTIL_TEST_CASE_11;

        bslma::TestAllocator da2;
        bslma::DefaultAllocatorGuard guard(&da2);

        straightTrace = false;

        int depth = 5;
        recurseABunchOfTimes(&depth, depth, &depth, depth, &depth);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING << HEX TRACE
        //
        // Concerns:
        //   That 'stream << Util::hexStackTrace;' outputs the correct
        //   function pointers.
        //
        // Plan:
        //   Do the output to a stringstream, then parse the hex pointers, put
        //   them into a stack trace and resolve them and verify that reslts
        //   in the expected symbols.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING << HEX STACK TRACE\n"
                             "==========================\n";

        using namespace BALST_STACKTRACEUTIL_TEST_CASE_11;

        bslma::TestAllocator da2;
        bslma::DefaultAllocatorGuard guard(&da2);

        int depth = 5;
        recurseABunchOfTimes(&depth, depth, &depth, depth, &depth);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING MULTITHREADEDNESS
        //
        // Concern:
        //   Some parts of getting a stack trace, particularly pieces like
        //   demangling that aren't BDE code, might not be thread-safe.
        //
        // Plan:
        //   Repeatedly do stack traces in 2 threads simultanously.  Don't
        //   bother streaming them -- the streaming code is entirely ours and
        //   we know it's safe.
        // --------------------------------------------------------------------

        if (verbose) cout << "Multithreaded Test\n"
                             "==================\n";

#ifndef BALST_STACKTRACEUTIL_TEST_10_SYMBOLS
        cout << "Not built with symbols -- no symbols checked\n";
#endif

        namespace TC1 = BALST_STACKTRACEUTIL_TEST_CASE_10;
        namespace TC = TC1::NS_10_2::NS_10_3::NS_10_4;

        bsl::function<void()> func = &TC::loopForFourSeconds;
        bslmt::ThreadUtil::Handle handles[2];
        for (int i = 0; i < 2; ++i) {
            int rc = bslmt::ThreadUtil::create(&handles[i], func);
            ASSERT(0 == rc);
        }
        for (int i = 0; i < 2; ++i) {
            int rc = bslmt::ThreadUtil::join(handles[i]);
            ASSERT(0 == rc);
        }
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

        // Note that 'traces' (stack TRACES), 'ln' (Line NumberS), and 'GET_ST"
        // (GET Stack Trace) must have very short names since it is imperative
        // that we be able do several things on a single 79 column line.

        ST  traces[DATA_POINTS];
        int lns[   DATA_POINTS];

#define GET_ST(st)                                                            \
        rc = Util::loadStackTraceFromStack(st, 100, false);                   \
        LOOP_ASSERT(rc, 0 == rc);

        int rc;

        // We ensure here that the line number returned by the stack trace is
        // the line number of the actual call, and not the line after it, or
        // the line of the first executable statement after it.

        // We make the assignment conditional on the state of the stack trace
        // just obtained, to prevent any clever compilers from anticipating the
        // only assignment to lns[*] and assigning them at variable creation
        // and putting *NO* executable statement where we're assigning
        // '__LINE__'.

        GET_ST(&traces[0]); lns[0] = traces[0].length() > 0 ? __LINE__ : -1;

        GET_ST(&traces[1]);
        lns[1] = traces[1].length() > 0 ? __LINE__ - 1 : -1;

        GET_ST(&traces[2]);

        lns[2] = traces[2].length() > 0 ? __LINE__ - 2 : -1;

        UintPtr lastAddress    = 0;
        IntPtr  lastOffset     = 0;
        int     lastLineNumber = 0;

        for (int i = 0; i < DATA_POINTS; ++i) {
            balst::StackTrace& stackTrace = traces[i];
            const balst::StackTraceFrame& frame = stackTrace[0];

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

            if (DEBUG_ON && !FORMAT_ELF && !FORMAT_DLADDR) {
                int lineNumber = frame.lineNumber();
                LOOP3_ASSERT(i, lineNumber, lns[i], lineNumber == lns[i]);
                LOOP2_ASSERT(lastLineNumber, lineNumber,
                                                  lastLineNumber < lineNumber);
                lastLineNumber = lineNumber;

                if (veryVerbose) { P_(lineNumber); P(lns[i]) }
            }
        }

        if (verbose || problem()) {
            for (int i = 0; i < DATA_POINTS; ++i) {
                *out_p << '(' << i << ")(0): " << traces[i][0] << endl;
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
        case_5_bottom(false, false, &depth);    // no demangle
        ASSERT(startDepth  == depth);

        depth *= 2;
        startDepth *= 2;
        case_5_bottom(true,  false, &depth);    // demangle
        ASSERT(startDepth  == depth);

        (*foilOptimizer(CASE_4::bottom))(false, 3.7);    // no demangling
        ASSERT(case_4_top_called_mangle);

        (*foilOptimizer(CASE_4::bottom))(true,  3.7);    // demangling
        ASSERT(case_4_top_called_demangle);
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

        // no demangle, hbpa
        (*foilOptimizer(case_5_bottom))(false, false, &depth);
        ASSERT(startDepth == depth);

        case_5_bottom(true,  false, &depth);    // demangle, hbpa
        ASSERT(startDepth == depth);

        case_5_bottom(false, true,  &depth);    // no demangle, test alloc
        ASSERT(startDepth == depth);

        case_5_bottom(true,  true,  &depth);    // demangle, test alloc
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

        (*foilOptimizer(TC::bottom))(false, 3.7);    // no demangling
        ASSERT(case_4_top_called_mangle);

        (*foilOptimizer(TC::bottom))(true,  3.7);    // demangling
        ASSERT(case_4_top_called_demangle);
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

        (void) (*foilOptimizer(TC::bottom))(false);    // no demangling

        ASSERT(calledCase3TopMangle);

        (void) (*foilOptimizer(TC::bottom))(true);     // demangling

        ASSERT(calledCase3TopDemangle);
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

        (*foilOptimizer(TC::bottom))(&ta);
        ASSERT(TC::topCalled);

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
        //:     'balst::StackAddressUtil::getStackAddresses' and then
        //:     initialize a stack trace using
        //:     'loadStackTraceFromAddressArray'.
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

        (*foilOptimizer(&TC::bottom))(&ta);
        ASSERT(TC::topCalled);

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
