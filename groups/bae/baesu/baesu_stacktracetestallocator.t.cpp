// baesu_stacktracetestallocator.t.cpp                                -*-C++-*-
#include <baesu_stacktracetestallocator.h>

#include <baesu_stacktrace.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_threadutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_newdeleteallocator.h>
#include <bsls_atomic.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#ifdef BSLS_PLATFORM__OS_WINDOWS

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
#define QV(X) if (verbose) Q(X);
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

typedef bsls::Types::UintPtr UintPtr;

#if defined(BSLS_PLATFORM__OS_WINDOWS)
    enum { PLAT_WIN = 1 };
#else
    enum { PLAT_WIN = 0 };
#endif

#ifdef BDE_BUILD_TARGET_DBG
    enum { DEBUG_ON = 1 };
#else
    enum { DEBUG_ON = 0 };
#endif

}  // close unnamed namespace

//=============================================================================
// GLOBAL HELPER VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;

static const bsl::size_t npos = bsl::string::npos;

                                // -----
                                // Usage
                                // -----

struct S {
    // This 'struct' allows us to keep a linked list of 'double's.

    static S                *s_head;
    S                       *d_next;
    double                   d_x;
};
S *S::s_head = 0;


void usagePushVal(double x, bslma::Allocator *alloc)
    // Add a node to our linked list containing the specified 'x'.
{
    S *p = new(*alloc) S;

    p->d_next = S::s_head;
    p->d_x = x;
    S::s_head = p;
}

void usagePushGeometricSequence(double base, bslma::Allocator *alloc)
    // Given a specified 'base', dd a sequence of nodes to the linked list in a
    // geometric sequence from 'base' through 'base ** 4', in geometric
    // increments of 'base'.  If 'base <= 1', do nothing.
{
    if (base <= 1) {
        return;
    }

    double x = 1.0;
    for (int i = 0; i < 4; ++i) {
        usagePushVal((x *= base), alloc);
    }
}

void usageMiddle(bslma::Allocator *alloc)
    // Iterate, doing 2 calls, each of which eventually calls 'pushVal' and
    // thus allocate, but with 2 different call stacks.
{
    for (double x = 0; x < 100; x += 10) {
        usagePushVal(x, alloc);
        usagePushGeometricSequence(x, alloc);
    }
}

void usageBottom()
{
    // First, create a stringstream to which output will go if we're not in
    // verbose mode.  Create a pointer to an 'ostream' which will point to
    // 'cout' if we're in verbose mode, the stringstream otherwise.

    bsl::stringstream ss(&bslma::NewDeleteAllocator::singleton());
    bsl::ostream *pOut = (verbose ? &cout : &ss);

    {
        // Next, create 'ta', a stack trace test allocator, and associate it
        // with the stream 'pOut'.

        baesu_StackTraceTestAllocator ta("Usage Test Allocator", pOut);

        // Then, turn off abort mode.  If abort mode were left enabled, the
        // destructor would abort if any memory were leaked.

        ta.setNoAbort(true);

        // Next, call 'usageMiddle', which will iterate, calling other routines
        // which will allocate many segments of memory.

        usageMiddle(&ta);

        // Now, verify that we have unfreed memory outstanding.

        ASSERT(ta.numBlocksInUse() > 0);
        if (verbose) {
            cout << ta.numBlocksInUse() << " segments leaked\n";
        }

        // Finally, destroy 'ta', leaking allocated memory.  The destructor of
        // 'ta' will write a report about the leaked segments to '*pOut', and
        // automatically clean up, freeing all the unfreed segments.  Note that
        // although leaked memory was only allocated from one routine, it was
        // allocated from 2 call chains, so 2 call chains will be reported.
        // Also note that had no memory been leaked, no report would be
        // written.
    }
}

// Output of usage example on AIX:
//  46 segments leaked
//  ============================================================================
//  Error: memory leaked:
//  46 segment(s) in allocator 'Usage Test Allocator' in use.
//  Segment(s) allocated in 2 place(s).
//  ----------------------------------------------------------------------------
//  Allocation place 1, 10 segment(s) in use.
//  Stack trace at allocation time:
//  (0): .usagePushVal(double,BloombergLP::bslma::Allocator*)+0x5c at 0x1001bd54
//  source:baesu_stacktracetestallocator.t.cpp:143 in
//  baesu_stacktracetestallocator.t.
//  (1): .usageMiddle(BloombergLP::bslma::Allocator*)+0x3c at 0x1001bc94 source:
//  baesu_stacktracetestallocator.t.cpp:170 in baesu_stacktracetestallocator.t.
//  (2): .usageBottom()+0x158 at 0x1001b4a8
//  source:baesu_stacktracetestallocator.t.cpp:198 in
//  baesu_stacktracetestallocator.t.
//  (3): .main+0x198 at 0x100008f0
//   source:baesu_stacktracetestallocator.t.cpp:471 in
//  baesu_stacktracetestallocator.t.
//  (4): .__start+0x6c at 0x1000020c source:crt0main.s in
//  baesu_stacktracetestallocator.t.
//  ----------------------------------------------------------------------------
//  Allocation place 2, 36 segment(s) in use.
//  Stack trace at allocation time:
//  (0): .usagePushVal(double,BloombergLP::bslma::Allocator*)+0x5c at
//  0x1001bd54 source:baesu_stacktracetestallocator.t.cpp:143 in
//  baesu_stacktracetestallocator.t.
//  (1): .usagePushGeometricSequence(double,BloombergLP::bslma::Allocator*)+0x6c
//  at 0x1001be1c source:baesu_stacktracetestallocator.t.cpp:161 in
//  baesu_stacktracetestallocator.t.
//  (2): .usageMiddle(BloombergLP::bslma::Allocator*)+0x4c at 0x1001bca4
//  source:baesu_stacktracetestallocator.t.cpp:171 in
//  baesu_stacktracetestallocator.t.
//  (3): .usageBottom()+0x158 at 0x1001b4a8
//  source:baesu_stacktracetestallocator.t.cpp:198 in
//  baesu_stacktracetestallocator.t.
//  (4): .main+0x198 at 0x100008f0
//  source:baesu_stacktracetestallocator.t.cpp:471 in
//  baesu_stacktracetestallocator.t.
//  (5): .__start+0x6c at 0x1000020c source:crt0main.s in
//  baesu_stacktracetestallocator.t.
//..

                                // ------
                                // case 3
                                // ------

namespace MultiThreadedTest {

struct NotEqual {
    int d_value;

    // CREATORS
    NotEqual(int value) : d_value(value) {}

    bool operator()(int test)
    {
        return test != d_value;
    }
};

struct Functor {
    enum { NUM_THREADS = 10 };

    // DATA
    static bsls::AtomicInt         s_threadRand;
    static bsls::AtomicInt         s_numUnfreedSegments;
    static bcemt_Barrier           s_finishBarrier;

    bsl::vector<int *>             d_alloced;
    bsl::size_t                    d_randNum;
    int                            d_64;
    baesu_StackTraceTestAllocator *d_allocator;

    // CREATORS
    Functor(bslma::Allocator              *vecAllocator,
            baesu_StackTraceTestAllocator *traceAllocator)
    : d_alloced(vecAllocator)
    , d_randNum(0x55aa55aa + ++s_threadRand)
    , d_allocator(traceAllocator)
    {}

    Functor(const Functor& original)
    : d_alloced(original.d_alloced,
                original.d_alloced.get_allocator().mechanism())
    , d_randNum(original.d_randNum + ++s_threadRand * 7)
    , d_allocator(original.d_allocator)
    {}

    // MANIPULATORS
    unsigned rand()
    {
        return d_randNum = d_randNum * 1103515245 + 12345;
    }

    void freeOne(int index)
    {
        ASSERT((unsigned) index < d_alloced.size());
        int *segment = d_alloced[index];
        int *end = segment + *segment + 1;
        int fillWord = (int) (UintPtr) segment;
        int *f = bsl::find_if(segment + 1, end, NotEqual(fillWord));
        LOOP4_ASSERT(fillWord, *f, *segment, f - segment - 1, end == f);
        bsl::fill(segment + 1, end, ~fillWord);
        *segment = -5;

        d_allocator->deallocate(segment);
        d_alloced[index] = d_alloced.back();
        d_alloced.pop_back();
    }

    void freeSome()
    {
        int mod = bsl::min((int) d_alloced.size() / 4,
                           (int) d_alloced.size() - 4);
        mod = bsl::max(mod, 0);
        for (int numToFree = mod > 1 ? rand() % mod : 0; numToFree;
                                                                 --numToFree) {
            int index = rand() % d_alloced.size();
            freeOne(index);
        }
    }

    void top();

    void nest4();

    void nest3();

    void nest2();

    void nest1();

    void operator()();
};
bsls::AtomicInt Functor::s_threadRand(0);
bsls::AtomicInt Functor::s_numUnfreedSegments(0);
bcemt_Barrier   Functor::s_finishBarrier(NUM_THREADS + 1);

void Functor::top()
{
    int allocLength = 5 + rand() % 20;
    int *segment = (int *) d_allocator->allocate(
                                              (allocLength + 1) * sizeof(int));
    d_alloced.push_back(segment);
    *segment = allocLength;
    int fillWord = (int) (UintPtr) segment;
    bsl::fill(segment + 1, segment + allocLength + 1, fillWord);
    ASSERT(fillWord == segment[1]);
}

void Functor::nest4()
{
    for (int i = 1; i < d_64; i += 32) {
        top();
    }
}

void Functor::nest3()
{
    int end = d_64 >> 3;
    for (int j = 4; j < end; ++j) {
        nest4();
    }
}

void Functor::nest2()
{
    for (int i = 0; i < d_64 / 16; ++i) {
        nest3();
    }
}

void Functor::nest1()
{
    for (int i = 0; i < 3; ++i) {
        nest2();
    }
}

void Functor::operator()()
{
    ASSERT(0 == d_alloced.size());

    d_64 = 75;
    for (int i = 0; 0 == (i & 0x23474000); ++i) {
        d_64 ^= i;
    }
    d_64 -= 11;

    ASSERT(64 == d_64);

    for (int i = 0; i < 10; ++i) {
        nest1();
        freeSome();
    }

    s_numUnfreedSegments += d_alloced.size();

    s_finishBarrier.wait();

    // Main thread will now gather a report on unfreed segments

    s_finishBarrier.wait();
    for (int i = d_alloced.size() - 1; i >= 0; --i) {
        freeOne(i);
    }
}

}  // close namespace MultiThreadedTest

                                // ------
                                // case 1
                                // ------

static
int caseOneDelta = 64;

void caseOne_c(bslma::Allocator *alloc)
{
    (void) alloc->allocate(100);    // leak some more memory
}

static
void caseOne_b(bslma::Allocator *alloc)
{
    for (int i = 1; i < 1024 && (i & 1); i += (caseOneDelta * 2)) {
        if (i > 64) {
            caseOne_c(alloc);
            ++i;
        }
    }
}

void caseOne_a(bslma::Allocator *alloc)
{
    for (int i = 1; i < 1024 && (i & 1); i += (caseOneDelta * 2)) {
        if (i > 64) {
            caseOne_b(alloc);
            ++i;
        }
    }
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

    bslma::TestAllocator da;
    bslma::DefaultAllocatorGuard guard(&da);

    int expectedDefaultAllocations = 0;

    switch (test) { case 0:
      case 5: {
        //---------------------------------------------------------------------
        // USAGE EXAMPLE
        //---------------------------------------------------------------------

        if (verbose) cout << "Usage Example\n"
                             "=============\n";

        usageBottom();
      }  break;
      case 4: {
        //---------------------------------------------------------------------
        // RELEASE TEST
        //
        // Concern:
        //   That the 'release' function properly frees all outstanding
        //   segments.
        //
        // Plan:
        //   Allocate some segments, call release, and verify that the ocunt
        //   of allocated segments goes to zero.  Use a 'bslma_TestAllocator'
        //   as the underlying allocator, since it will detect any leaks.
        //---------------------------------------------------------------------

        if (verbose) cout << "Release Test\n"
                             "============\n";

        bslma_TestAllocator sta("sta");
        bsl::stringstream ss(&sta);
        baesu_StackTraceTestAllocator *pta =
                                  new(sta) baesu_StackTraceTestAllocator("ta",
                                                                         &ss,
                                                                         8,
                                                                         true,
                                                                         &sta);
        for (int i = 0; i < 1000; ++i) {
            (void) pta->allocate(100);
        }

        ASSERT(1000 ==  pta->numBlocksInUse());

        if (! veryVerbose) {
            pta->release();

            ASSERT(0 == pta->numBlocksInUse());
        }
        else {
            pta->setNoAbort(true);
        }

        sta.deleteObject(pta);

        if (veryVerbose) {
            const bsl::string& ostr = ss.str();
            ++expectedDefaultAllocations;             // otherSs.str() uses da

            ASSERT(!ostr.empty());
            cout << ostr;
        }
        else {
            ASSERT(ss.str().empty());
        }
      }  break;
      case 3: {
        //---------------------------------------------------------------------
        // MULTITHREADED TEST
        //
        // Concern:
        //   That the allocator might hang or malfunction if used in a
        //   multithreaded context.
        //
        // Plan:
        //   Submit many threads that thrash doing allocation and deallocation
        //   in a somewhat random way, see if any hanging or irregularity
        //   occurs.
        //---------------------------------------------------------------------

        if (verbose) cout << "Multithreaded Test\n"
                             "==================\n";

        namespace TC = MultiThreadedTest;

        typedef bcemt_ThreadUtil Util;

        bcema_TestAllocator vta("vta", false);              // vector allocator
        bslma_TestAllocator sta("sta");

        bsl::stringstream ss(&sta);

        baesu_StackTraceTestAllocator *pta =
                                  new(sta) baesu_StackTraceTestAllocator("ta",
                                                                         &ss,
                                                                         8,
                                                                         true,
                                                                         &sta);

        Util::Handle handles[TC::Functor::NUM_THREADS];
        int rc = 0;
        for (int i = 0; 0 == rc && i < TC::Functor::NUM_THREADS; ++i) {
            rc = Util::create(&handles[i], TC::Functor(&vta, pta));
            ASSERT(0 == rc);

            static
            bool isInplace = bdef_FunctionUtil::IsInplace<TC::Functor>::VALUE;
            expectedDefaultAllocations += 0 == isInplace;
            
            static bool firstTime = true;
            if (verbose && firstTime) {
                firstTime = false;
                P(isInplace);
            }
        }

        TC::Functor::s_finishBarrier.wait();

        if (verbose) {
            Q(Finished Thrashing);
            P(TC::Functor::s_numUnfreedSegments);
        }
        LOOP_ASSERT(TC::Functor::s_numUnfreedSegments,
                                        TC::Functor::s_numUnfreedSegments >=
                                                 4 * TC::Functor::NUM_THREADS);

        bsl::stringstream otherSs(&sta);
        pta->reportBlocksInUse(&otherSs);
        const bsl::string& otherStr = otherSs.str();
        ++expectedDefaultAllocations;                  // otherSs.str() uses da

        ASSERT(TC::Functor::s_numUnfreedSegments ==
                                                  (int) pta->numBlocksInUse());

        bsl::size_t pos;
        {
            bsl::stringstream matchSs(&sta);
            matchSs << TC::Functor::s_numUnfreedSegments << 
                                        " segment(s) in allocator 'ta' in use";
            ASSERT(npos != (pos = otherStr.find(matchSs.str())));
            ++expectedDefaultAllocations;              // otherSs.str() uses da
        }

        const char *expectedStrings[] = { " in 1 place",
                                          "MultiThreadedTest",
                                          "Functor",
                                          "top",
                                          "MultiThreadedTest",
                                          "Functor",
                                          "nest4",
                                          "MultiThreadedTest",
                                          "Functor",
                                          "nest3",
                                          "MultiThreadedTest",
                                          "Functor",
                                          "nest2",
                                          "MultiThreadedTest",
                                          "Functor",
                                          "nest1",
                                          "MultiThreadedTest",
                                          "Functor" };

        enum { NUM_EXPECTED_STRINGS = sizeof expectedStrings /
                                                     sizeof *expectedStrings };
        for (int i = 0; i < NUM_EXPECTED_STRINGS; ++i) {
            bsl::size_t nextPos = otherStr.find(expectedStrings[i], pos);
            LOOP3_ASSERT(otherStr, i, expectedStrings[i], npos != nextPos);
            pos = npos != nextPos ? nextPos : pos;
        }
        if (veryVerbose) {
            cout << "Report of blocks in use\n" << otherStr;
        }
                                   
        TC::Functor::s_finishBarrier.wait();

        for (int i = 0; i < TC::Functor::NUM_THREADS; ++i) {
            Util::join(handles[i]);
        }

        LOOP_ASSERT(pta->numBlocksInUse(), 0 == pta->numBlocksInUse());

        sta.deleteObject(pta);

        ASSERT(ss.str().empty());     // nothing written to 'ss' by d'tor of ta
      }  break;
      case 2: {
        //---------------------------------------------------------------------
        // SUCCESSFUL FREEING TEST
        //
        // Concern: If segments are allocated and deallocated, that the test
        //   allocator issues no complaints and leaks no memory of its own.
        //
        // Plan: Create a baesu_StackTraceTestAllocator based on a
        //   bslma_TestAllocator, allocate a bunch of segments, storing
        //   pointers to them in an array, then free the segments.  Observe
        //---------------------------------------------------------------------

        if (verbose) cout << "SUCCESSFUL FREEING TEST\n"
                             "=======================\n";

        bslma_TestAllocator sta;

        bsl::stringstream ss(&sta);

        QV(Named Allocator);
        {
            baesu_StackTraceTestAllocator ta("ta",
                                             &ss,
                                             8,
                                             true,
                                             &sta);

            enum { NUM_SEGMENTS = 100 };

            void *segment[NUM_SEGMENTS];

            for (int i = 0; i < NUM_SEGMENTS; ++i) {
                segment[i] = ta.allocate(1 + i * 3);
            }

            ASSERT(NUM_SEGMENTS == ta.numBlocksInUse());

            bsl::stringstream otherSs(&sta);
            ta.reportBlocksInUse(&otherSs);

            const bsl::string& otherStr = otherSs.str();
            ++expectedDefaultAllocations;              // otherSs.str() uses da

            LOOP_ASSERT(otherStr, npos != otherStr.find(
                                   "100 segment(s) in allocator 'ta' in use"));
            LOOP_ASSERT(otherStr, npos != otherStr.find(" in 1 place"));

            if (verbose) {
                cout << otherStr;
            }

            for (int i = 0; i < NUM_SEGMENTS; ++i) {
                ta.deallocate(segment[i]);
            }

            ASSERT(0 == ta.numBlocksInUse());
        }

        QV(Unnamed Allocator);
        {
            baesu_StackTraceTestAllocator ta(&ss,
                                             8,
                                             true,
                                             &sta);

            enum { NUM_SEGMENTS = 100 };

            void *segment[NUM_SEGMENTS];

            for (int i = 0; i < NUM_SEGMENTS; ++i) {
                segment[i] = ta.allocate(1 + i * 3);
            }

            ASSERT(NUM_SEGMENTS == ta.numBlocksInUse());

            bsl::stringstream otherSs(&sta);
            ta.reportBlocksInUse(&otherSs);

            const bsl::string& otherStr = otherSs.str();
            ++expectedDefaultAllocations;              // otherSs.str() uses da

            LOOP_ASSERT(otherStr, npos != otherStr.find(
                                                    " 100 segment(s) in use"));
            LOOP_ASSERT(otherStr, npos != otherStr.find(" in 1 place"));

            if (verbose) {
                cout << otherStr;
            }

            for (int i = 0; i < NUM_SEGMENTS; ++i) {
                ta.deallocate(segment[i]);
            }

            ASSERT(0 == ta.numBlocksInUse());
        }

        ASSERT(ss.str().empty());     // nothing written to 'ss' by d'tor of ta

        // Upon destruction, 'sta' will verify that 'ta' didn't leak anything.
      }  break;
      case 1: {
        //---------------------------------------------------------------------
        // BREATHING TEST
        //
        // Set 'veryVerbose' to make this test abort when the test allocator
        // is destructed.
        //---------------------------------------------------------------------

        if (verbose) cout << "BREATING TEST\n"
                             "=============\n";

        bslma::NewDeleteAllocator otherTa;

        int maxDepths[] = { 100, 10, 4, 3 };
        enum { NUM_MAX_DEPTHS = sizeof maxDepths / sizeof *maxDepths };

        for (int d = 0; d < NUM_MAX_DEPTHS; ++d) {
            if (verbose) {
                cout << endl << endl;
                P(maxDepths[d]);
            }

            bsl::stringstream out(&otherTa);

            {
                baesu_StackTraceTestAllocator ta("TestAlloc1",
                                                 &out,
                                                 maxDepths[d]);

                (void) ta.allocate(100);    // leak some memory

                for (int i = 0; i < 40; ++i) {
                    caseOne_a(&ta);
                }

                if (!veryVerbose) {
                    ta.setNoAbort(true);
                }
            }

            const bsl::string& outStr = out.str();
            ++expectedDefaultAllocations;                // out.str() uses da

            if (!PLAT_WIN || DEBUG_ON) {
                bsl::size_t pos = 0;
                ASSERT(npos != (pos = outStr.find("caseOne_c", pos)));
                ASSERT(npos != (pos = outStr.find("caseOne_b", pos)));
                ASSERT(npos != (pos = outStr.find("caseOne_a", pos)));
                pos = 0;
                ASSERT(npos != (pos = outStr.find("main",      pos)));
                if (maxDepths[d] >= 4) {
                    ASSERT(npos != (pos = outStr.find("main",  pos + 4)));
                }
            }

            if (verbose) {
                cout << outStr;
            }
        }
      }  break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    LOOP2_ASSERT(expectedDefaultAllocations, da.numAllocations(),
                            expectedDefaultAllocations == da.numAllocations());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
