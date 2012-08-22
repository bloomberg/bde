// baesu_stacktracetestallocator.t.cpp                                -*-C++-*-
#include <baesu_stacktracetestallocator.h>

#include <baesu_stacktrace.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_threadutil.h>

#include <bdesu_fileutil.h>
#include <bdeu_string.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_newdeleteallocator.h>
#include <bsls_atomic.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_fstream.h>
#include <bsl_set.h>
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
//                                 Overview
//                                 --------
// The component under test is a thread-safe mechanism non value type.
//
// Manipulators:
//: o allocate
//: o deallocate
//: o release
//: o setNoAbort
//
// Accessors
//: o numBlocksInUse
//: o reportBlocksInUse
//
// Things to test:
//
// Already tested:
//: o That a stack trace can be done which and prints out the names of the
//:   expected routines (note that we observe that mangled names on all
//:   platforms include relevant namespace names and actual function names
//:   as substrings, though the '::' are not represented as such).
//: o That the expected names come out in the expected order.
//: o That the stack traces go to at least the depth specified to the
//:   allocator c'tor
//: o That if no memory is leaked, the destructor will finish without writing
//:   anything to the 'ostream' associated with it at construction.
//: o That all combinations of args / optional args passed to c'tors work
//: o That if a stream is specified to 'reportBlocksInUse', its output goes
//:   to that stream.
//: o That the value of 'isNoAbort' is false at construction, and after that
//:   reflects the latest value passed to 'setNoAbort'.
//: o That if 'noAbort' is set, destruction of the alloctor will not cause
//:   an abort.
//: o That the allocator will cope with multiple outstanding segments being
//:   allocated / deallocated in random order, rather than just having the
//:   most recently allocated segment freed, or having segments freed in the
//:   order in which they were allocated.
//: o That 'numBlocksInUse' accurately reflects the number of outstanding
//:   segments.
//: o That reports of segments in use properly reflect the number of segments.
//: o That the allocator is thread-safe.
//: o That 'release', if called, releases all allocated segments.
//: o That multiple frees of the same segment are correctly reported, and don't
//:   result in an abort if 'noAbort' is set.
//: o That freeing a segment allocated by one stack trace test allocator by
//:   another stack trace test allocator is correctly reported, and will not
//:   result in an abort if 'noAbort' is set.
//: o That the default allocator is not used by this class (it defaults to
//:   using the malloc free allocator singleton if no allocator is specified).
//-----------------------------------------------------------------------------
// Already Semi Tested:
//: o That if no stream is specified to 'reportBlocksInUse', its output goes to
//:   the stream specified to the allocator's c'tor (already indirectly tested
//:   if you know the d'tor calls 'reportBlocksInUse this way).
//: o At least configured stack trace depth is respected (already tested in
//:   breathing test, test again somewhere else).
//-----------------------------------------------------------------------------
// Testing not done yet.
//: o Test check for 'numRecordedFrames >= 2'.
//: o That multiple frees of the same segment result in an abort if 'noAbort'
//:   is not set.
//: o That freeing a segment allocated by one stack trace test allocator by
//:   another stack trace test allocator will result in an abort if 'noAbort'
//:   is not set.
//: o That attempting to free a segment allocated with 'bslma::TestAllocator'
//:   will be detected, reported, and result in aborts appropriately depending
//:   on 'isNoAbort()'.
//: o ???? Should we do a test where we attempt to free segments created by
//:   'malloc' and 'NewDeleteAllocator' with the stack trace test allocator?
//:   Such an action would offend purify, and we can't guarantee the result,
//:   though the odds against failure are astronomical.
//: o That a slight buffer (by up to the size of 4 pointers) underrun is
//:   correctly detected, and will result in an abort (but only if 'isNoAbort'
//:   is 'false').  Test should be run for all lengths of overwrite from 1
//:   byte to the size of 4 ptrs.
//: o That 'demanglingPreferredFlag' is respected.  Have to #ifdef, demangling
//:   always happens on Windows and never on Solaris.
//: o That allocated segments are properly aligned.
//: o That attempting to deallocate a pointer that doesn't meet the minimum
//:   alignment requirements of this allocator (and therefore known not to have
//:   been allocated by this allocator) is properly reported and results in
//:   abort depending on 'isNoAbort'.
//: o That segments returned really are of at least the requested size.
//: o Is exception-safe if the underlying allocator throws exceptions.
//-----------------------------------------------------------------------------
// [25] * usage example
//      * name the allocator in the usage example
// [24] * segment header test -- use 'my_SegmentHeader' to test
//      * magic number
//      * pointer to allocator
//      * validity of next segment (can't check prev segment)
// [23] * test out of memory condition (aborts or throws) -- maybe not do this?
// [22] * test all meaningful combos of c'tor args, ie 'name == 0'
// [21] * testing that # of recorded frames EXACTLY equals 'numRecordedFrames'
// [20] * testing for undefined behavior with setjmp/longjmp
//        'numRecordedFrames >= 2'
//        'name == 0'
// [19] * stack depth testing
//      * verify that the stack depth reported is exactly as great as what
//        was specified at construction.
//      * verify that the stack depth reported is not greater than
//        (MAX_ALIGNMENT - 1) / sizeof(void *) more frames than were requested
//        by the constructor.
//      * do this test by having a recursive call stack, and then counting
//        the number of occurrences of the name of the recursing routine in
//        the report.
// [18] * exception safety
//      * Pass the allocator a bslma::TestAllocator rigged to throw exceptions
//        and verify that the allocator handles this properly.
// [17] * alignment & min size test
//      * attempt to free pointers that don't meet the minimum alignment
//        required for any pointer allocated by this type.
//      * allocate segments from 0 byte to 100 bytes long, several segments for
//        each size.  Calculate alignment
//        via 'bsls::AlignmentUtil::calculateAlignmentFromSize', and verify
//        that the segment return always satisfies the alignment requirement.
//        Write over the full length of the segment. Use 'bslma::TestAllocator'
//        as the underlying allocator as it will detect overruns if the any of
//        the segments were smaller than requested.
//      * fill segments with byte calculated from xor of all bits in ptr,
//        verify before freeing no bytes have changed.
//      * store segments in struct { void *d_p; int d_len; };
//      * have array of segments, use a random number generator to determine
//        which is allocated or freed next.  Maintain count of existing
//        segments so never exceed available slots and only free when there is
//        something to be freed.  Also monitor 'numBlocksInUse' during this
//        and make sure it is accurate.
// [16] * demanglingPreferredFlag
//      * #ifdef appropriate for platform, do reports with & without
//        'demanglingPreferredFlag' by searching for
//        'BloombergLP::baesu_StackTraceTestAllocator::' in the report -- it
//        will be present if demangling is done, it won't if not
// [15] * Underrun detection
//      * Verify that writing before the beginning of the buffer from 1 byte
//        to the size of 4 pointers (a) is always detected, and (b) does not
//        result in segfaults, and (c) results in failure handling appropriate
//        for 'failureHandler'
// [14] Deallocation errors test
//      * repeat all tests with and without 'noAbort'
//      * attempt to deallocate segments allocated with 'malloc' and
//        global 'new' (actually, since we override 'new' to call 'malloc'
//        for the sake of other tests, we can only check 'malloc').
//      * verify that all errors encountered in this test will result in
//        failure of assert unless 'noAbort' is set -- repeat all tests with &
//        without 'noAbort'
//      * Attempt to free segments created by 'bslma::TestAllocator' and
//        'bcema_TestAllocator'
//      * Pass a pointer that is not pointer-aligned, triggering alignment
//        error detection
// [13] Calling 'release' successfully frees all allocated segments
//      * add test to verify that calling it when no segments are outstanding
//        does no harm
//      * call it with 1 through 10 segments outstanding
//      * verify that d'tor also successfully frees all allocated segments
//      * if d'tor aborts, it should be before segments are released (to
//        provide a more meaningful core dump)
// [12] Thread-safety test
//      * Overhaul so that threads do less work and more allocating.
//      * Create allocator that 'has-a' test allocator, and specifically keeps
//        a count of how many times allocate or deallocate is currently
//        happening, and asserts they're never happening at once.
//      * Have test set up a barrier before allocating, allocates, a bunch of
//        times, does same thing with frees.
// [11] Successful freeing of all allocated segments results in no report
//      being written at destruction
// [10] Bslx streaming (N/A)
// [ 9] Assignment (N/A)
// [ 8] Swap (N/A)
// [ 7] Copy c'tor (N/A)
// [ 6] Equality comparisons (N/A)
// [ 5] Print and output operator (N/A)
// [ 4] All accessors
//      * verify that if 'reportBlocksInUse' is called with no args, it writes
//        its report to the stream specified at construction of the allocator.
//      * verify that if 'reportBlocksInUse' is called with no memory
//        outstanding, it produces no output.
// [ 3] Value c'tor (N/A)
// [ 2] All constructors, destructor, all manipulators
//      * Verify that underlying testallocator passed is providing memory, and
//        freeing memory when 'deallocate' is called.
//      * Instrument new/delete like mallocfreeallocator.t.cpp to verify
//        default allocator configuration not coming from new/delete.
//      * assert there is no output about leaked segments before d'tor is
//        called
//      * repeat all tests with and without abort flag (no abort is expected
//        in this case
//      * call 'ASSERT(oss.str().empty());' before d'tor is called
//      * call one instance of object, destroy it with no memory outstanding,
//        verify no report written by d'tor
//      * use 'leakTwiceA', verify order of 'leadTwice{C,B,A}' in trace.
//      * verify allocator name in report
// [ 1] Breathing test
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
// GLOBAL HELPER VARIABLES AND TYPES FOR TESTING
//-----------------------------------------------------------------------------

typedef baesu_StackTraceTestAllocator Obj;

static int verbose;
static int veryVerbose;

static const bsl::size_t npos = bsl::string::npos;

static const double myPi      = 3.14159265358979323846;

                                // -----
                                // Usage
                                // -----

struct ShipsCrew {
    // This struct will, at construction, read and parse a file describing
    // the names of the crew of a ship.

  private:
    // PRIVATE TYPES
    struct Less {
        // Functor to compare two 'const char *'s

        bool operator()(const char *a, const char *b) const
        {
            return bsl::strcmp(a, b) < 0;
        }
    };

    typedef bsl::set<const char *, Less> NameSet;

  public:
    // PUBLIC DATA
    const char       *d_captain;
    const char       *d_firstMate;
    const char       *d_cook;
    NameSet           d_sailors;

  private:
    // PRIVATE MANIPULATORS
    void addSailor(const bsl::string& name);
        // Add the specified 'name' to the set of sailor's names.  Check for
        // redundancy.

    const char *copy(const bsl::string& str);
        // Allocate memory for a copy of the specified 'str' as a char array,
        // copy the contents of 'str' into it, and return a pointer to the
        // array.

    void setCaptain(const bsl::string& name);
        // Set the name of the ship's captain to the specified 'name', but only
        // if the captain's name was not already set.

    void setCook(const bsl::string& name);
        // Set the name of the ship's cook to the specified 'name', but only if
        // the captain's name was not already set.

    void setFirstMate(const bsl::string& name);
        // Set the name of the ship's first mate to the specified 'name', but
        // only if the captain's name was not already set.

  public:
    // CREATORS
    explicit
    ShipsCrew(const char *crewFileName);
        // Read the names of the ship's crew in from the file with the
        // specified name 'crewFileName'.

    ~ShipsCrew();
        // Destroy this object and free memory.
};

// PRIVATE MANIPULATORS
void ShipsCrew::addSailor(const bsl::string& name)
{
    BSLS_ASSERT(! d_sailors.count(name.c_str()));
    d_sailors.insert(copy(name));
}

const char *ShipsCrew::copy(const bsl::string& str)
{
    return BloombergLP::bdeu_String::copy(str,
                                          bslma::Default::defaultAllocator());
}

void ShipsCrew::setCaptain(const bsl::string& name)
{
    BSLS_ASSERT(! d_captain);
    d_captain = copy(name);
}

void ShipsCrew::setCook(const bsl::string& name)
{
    BSLS_ASSERT(! d_cook);
    d_cook = copy(name);   // This was line 231 when this test case was written
}

void ShipsCrew::setFirstMate(const bsl::string& name)
{
    BSLS_ASSERT(! d_firstMate);
    d_firstMate = copy(name);
}

// CREATORS
ShipsCrew::ShipsCrew(const char *crewFileName)
: d_captain(0)
, d_firstMate(0)
, d_cook(0)
, d_sailors()
{
    typedef BloombergLP::bdeu_String String;

    bsl::ifstream input(crewFileName);
    BSLS_ASSERT(!input.eof() && !input.bad());

    while (!input.bad() && !input.eof()) {
        bsl::string line;
        bsl::getline(input, line);

        bsl::size_t colon = line.find(':');
        if (bsl::string::npos != colon) {
            bsl::string field = line.substr(0, colon);
            bsl::string name  = line.substr(colon + 1);

            if (0 == String::lowerCaseCmp(field, "captain")) {
                setCaptain(name);
            }
            else if (0 == String::lowerCaseCmp(field, "first mate")) {
                setFirstMate(name);
            }
            else if (0 == String::lowerCaseCmp(field, "cook")) {
                setCook(name);
            }
            else if (0 == String::lowerCaseCmp(field, "sailor")) {
                addSailor(name);
            }
            else {
                cerr << "Unrecognized field '" << field << "'\n";
            }
        }
        else if (!line.empty()) {
            cerr << "Garbled line '" << line << "'\n";
        }
    }
}

ShipsCrew::~ShipsCrew()
{
    bslma::Allocator *da = bslma::Default::defaultAllocator();

    da->deallocate(const_cast<char *>(d_captain));
    da->deallocate(const_cast<char *>(d_firstMate));

    // Note that deallocating the strings will invalidate 'd_sailors' -- any
    // manipulation of 'd_sailors' other than destruction after this would lead
    // to undefined behavior.

    const NameSet::iterator end = d_sailors.end();
    for (NameSet::iterator it = d_sailors.begin(); end != it; ++it) {
        da->deallocate(const_cast<char *>(*it));
    }
}

bsl::string getCaptain(const char *fileName)
{
    ShipsCrew crew(fileName);

    return crew.d_captain ? crew.d_captain : "";
}

                                // -----
                                // Usage
                                // -----

struct S {
    // This 'struct' allows us to keep a linked list of 'double's.

    // DATA
    static S                *s_head;
    S                       *d_next;
    double                   d_x;
};
S *S::s_head = 0;


void usagePushVal(double x, bslma::Allocator *alloc)
    // Add a node to our linked list containing the specified 'x'.
{
    S *p = new(*alloc) S;

    p->d_x = x;
    p->d_next = S::s_head;
    S::s_head = p;
}

void usagePushGeometricSequence(double base, bslma::Allocator *alloc)
    // Given a specified 'base', dd a sequence of nodes to the linked list in a
    // geometric sequence from 'base' through 'base ** 4', in geometric
    // increments of 'base'.
{
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

        ta.setFailureHandler(&Obj::failureHandlerNoop);

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
        // automatically clean up, freeing all the leaked segments.  Note that
        // although leaked memory was only allocated from one routine, it was
        // allocated from 2 call chains, so 2 call chains will be reported.
        // Also note that had no memory been leaked, no report would be
        // written.
    }
}

// Output of usage example on AIX.  Note that the 2 call stacks shown are
// different -- only one of them contains the routine
// 'usagePushGeometricSequence':
//
//  46 segments leaked
//  ===========================================================================
//  Error: memory leaked:
//  1001 segment(s) in allocator 'myTestAllocator' in use.
//  Segment(s) allocated in 2 place(s).
//  ---------------------------------------------------------------------------
//  Allocation place 1, 1 segment(s) in use.
//  Stack trace at allocation time:
//  (0): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x84 at 0x1001b80c
//        source:baesu_stacktracetestallocator.t.cpp:155 in
//        baesu_stacktracetestallocator.t.
//  (1): .main+0x1b4 at 0x1000090c
//        source:baesu_stacktracetestallocator.t.cpp:398 in
//        baesu_stacktracetestallocator.t.
//  (2): .__start+0x6c at 0x1000020c source:crt0main.s in
//        baesu_stacktracetestallocator.t.
//  ---------------------------------------------------------------------------
//  Allocation place 2, 1000 segment(s) in use.
//  Stack trace at allocation time:
//  (0): .usageTop(int*,BloombergLP::bslma::Allocator*)+0x70 at 0x1001b8e0
//        source:baesu_stacktracetestallocator.t.cpp:146 in
//        baesu_stacktracetestallocator.t.
//  (1): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0xa8 at 0x1001b830
//        source:baesu_stacktracetestallocator.t.cpp:161 in
//        baesu_stacktracetestallocator.t.
//  (2): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001b81c
//        source:baesu_stacktracetestallocator.t.cpp:158 in
//        baesu_stacktracetestallocator.t.
//  (3): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001b81c
//        source:baesu_stacktracetestallocator.t.cpp:158 in
//        baesu_stacktracetestallocator.t.
//  (4): .usageRecurser(int*,BloombergLP::bslma::Allocator*)+0x94 at 0x1001b81c
//        source:baesu_stacktracetestallocator.t.cpp:158 in
//        baesu_stacktracetestallocator.t.
//  (5): .main+0x1b4 at 0x1000090c
//        source:baesu_stacktracetestallocator.t.cpp:398 in
//        baesu_stacktracetestallocator.t.
//  (6): .__start+0x6c at 0x1000020c source:crt0main.s in
//        baesu_stacktracetestallocator.t.
//  IOT/Abort trap (core dumped)
//..

                                // ------
                                // case 3
                                // ------

namespace MultiThreadedTest {

struct NotEqual {
    int d_value;

    // CREATORS
    explicit
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
    unsigned                       d_randNum;
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
            int index = rand() % (unsigned) d_alloced.size();
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

    s_numUnfreedSegments += (int) d_alloced.size();

    s_finishBarrier.wait();

    // Main thread will now gather a report on unfreed segments

    s_finishBarrier.wait();
    for (int i = (int) d_alloced.size() - 1; i >= 0; --i) {
        freeOne(i);
    }
}

}  // close namespace MultiThreadedTest

                                // ------
                                // case 1
                                // ------

// I have to put some meaningless gobbledegook into these methods or the
// optimizer will inline them.

static
int caseOneDelta = 64;

void caseOne_c(bslma::Allocator *alloc, int *depth)
{
    int depthIn = *depth + (int) (bsl::log(1.0) + 0.5);

    for (int i = 0; 0 == (i & 0x473600); ++i) {
        // This transform will toggle every bit in '*depth' that it touches
        // an even number of times, leaving it unchanged, in a way that the
        // optimizer can't figure out.

        *depth ^= i;
    }
    ASSERT(*depth == depthIn + (int) (bsl::sin(myPi) + 0.5));

    (void) alloc->allocate(100);    // leak some more memory

    *depth += (int) (bsl::cos(myPi / 2) + 0.5);
}

void caseOne_b(bslma::Allocator *alloc, int *depth)
{
    int incCount = (int) bsl::log(1.01);
    for (int i = 1; i < 1024 && (i & 1); i += (caseOneDelta * 2)) {
        ++*depth;
        ++incCount;

        if (i > 64) {
            caseOne_c(alloc, depth);
            ++i;
        }
    }

    *depth -= incCount;
}

void caseOne_a(bslma::Allocator *alloc)
{
    int depth = 0;

    for (int i = 1; i < 1024 && (i & 1); i += (caseOneDelta * 2)) {
        if (i > 64) {
            caseOne_b(alloc, &depth);
            ++i;
        }
    }

    ASSERT(depth == 0);
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
      case 16: {
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        // Pre usage example: prepare the file 'shipscrew.txt' in the local
        // directory for use by the usage example.

        {
            bsl::ofstream outFile("shipscrew.txt");

            outFile << "captain:Steve Miller\n"
                    << "first mate:Sally Chandler\n"
                    << "cook:Bob Jones\n"
                    << "sailor:Mitch Sandler\n"
                    << "sailor:Ben Lampert\n"
                    << "sailor:Daniel Smith\n"
                    << "sailor:Joe Owens\n";
        }

        {
            baesu_StackTraceTestAllocator ta;
            ta.setFailureHandler(&Obj::failureHandlerNoop);
            bslma::DefaultAllocatorGuard guard(&ta);

            bsl::string captain = getCaptain("shipscrew.txt");

            cout << "The captain is: " << captain << bsl::endl;
        }

        // When 'ta' is destroyed, the following message is written to stdout.
        //
        // ====================================================================
        // Error: memory leaked:
        // 1 segment(s) in allocator '<unnamed>' in use.
        // Segment(s) allocated from 1 trace(s).
        // --------------------------------------------------------------------
        // Allocation trace 1, 1 segment(s) in use.
        // Stack trace at allocation time:
        // (0): BloombergLP::baesu_StackTraceTestAllocator::.allocate(long)+
        //      0x2fc at 0x100007b64 source:baesu_stacktracetestallocator.cpp:
        //      335 in baesu_stacktracetestallocator.t.d
        // (1): BloombergLP::bdeu_String::.copy(const char*,int,BloombergLP::
        //      bslma::Allocator*)+0xbc at 0x10001893c source:bdeu_string.cpp:
        //      96 in baesu_stacktracetestallocator.t.d
        // (2): ShipsCrew::.copy(const bsl::basic_string<char,std::char_traits<
        //      char>,bsl::allocator<char> >&)+0x8c at 0x1000232c4 source:
        //      baesu_stacktracetestallocator.t.cpp:610 in
        //      baesu_stacktracetestallocator.t.d
        // (3): ShipsCrew::.setCook(const bsl::basic_string<char,std::
        //      char_traits<char>,bsl::allocator<char> >&)+0x54 at 0x1000233e4
        //      source:baesu_stacktracetestallocator.t.cpp:232 in
        //      baesu_stacktracetestallocator.t.d
        // (4): ShipsCrew::.__ct(const char*)+0x430 at 0x1000214c8
        //      source:baesu_stacktracetestallocator.t.cpp:272 in
        //      baesu_stacktracetestallocator.t.d
        // (5): .getCaptain(const char*)+0x44 at 0x100020704 source:
        //      baesu_stacktracetestallocator.t.cpp:306 in
        //      baesu_stacktracetestallocator.t.d
        // (6): .main+0x278 at 0x100000ab0 source:
        //      baesu_stacktracetestallocator.t.cpp:727 in
        //      baesu_stacktracetestallocator.t.d
        // (7): .__start+0x74 at 0x1000002fc source:crt0_64.s in
        //      baesu_stacktracetestallocator.t.d

        // Finally, we see that the leaked memory was in the 'setCook' method
        // line 232 (line numbers are generally the line after the subroutine
        // call in question -- the call to 'copy' was on line 231).  The
        // destructor neglected to deallocate the cook's name.

        // Note the following:
        //: o If we hadn't called 'setFailureHandler(&Obj::failureHandlerNoop)', the above report would
        //:   have been followed by a core dump. Since 'isNoAbort' was set,
        //:   'ta's destructor instead frees all leaked memory after giving
        //:   the report and returns.
        //: o Output will vary by platform.  Not all platforms support line
        //:   number information and demangling.  This report was generated on
        //:   AIX, and a couple of AIX quirks are visible -- identifiers have a
        //:   '.' prepended, and the constructor name got converted to '__ct'.

        // Post usage example: clean up 'shipscrew.txt'

        bdesu_FileUtil::remove("shipscrew.txt");
      }  break;
      case 15: {
        //---------------------------------------------------------------------
        // USAGE EXAMPLE
        //---------------------------------------------------------------------

        if (verbose) cout << "Usage Example\n"
                             "=============\n";

        usageBottom();
      }  break;
      case 14: {
        //---------------------------------------------------------------------
        // ERROR TEST
        //
        // Concern:
        //   That errors are properly reported.
        //
        // Plan:
        //   Provoke certain error messages and observe them being output.
        //---------------------------------------------------------------------


        expectedDefaultAllocations = -1;    // turn off default alloc checking

        bsl::stringstream oss;
        Obj ta("alpha",
               &oss);
        ta.setFailureHandler(&Obj::failureHandlerNoop);

        void *ptr = ta.allocate(6);
        ta.deallocate(ptr);
        ASSERT(oss.str().empty());
        ta.deallocate(ptr);
        LOOP_ASSERT(oss.str(), npos != oss.str().find(
                                    "freed second time by allocator 'alpha'"));
        oss.str("");

        bsl::stringstream oss2;
        Obj ta2(&oss2);
        ta2.setFailureHandler(&Obj::failureHandlerNoop);

        ptr = ta2.allocate(100);
        ta.deallocate(ptr);
        ASSERT(oss2.str().empty());
        LOOP_ASSERT(oss.str(), npos != oss.str().find("Error: attempt to free"
            " segment by wrong allocator.\n    Segment belongs to allocator"
                  " '<unnamed>'\n    Attempted to free by allocator 'alpha'"));
        oss.str("");

        ta2.deallocate(ptr);
        ASSERT(oss2.str().empty());
      }  break;
      case 13: {
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
            pta->setFailureHandler(&Obj::failureHandlerNoop);
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
      case 12: {
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

        const char *expectedStrings[] = { " from 1 trace",
                                          "StackTraceTestAllocator",
                                          "allocate",
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
      case 11: {
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
            LOOP_ASSERT(otherStr, npos != otherStr.find(" from 1 trace"));

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
            LOOP_ASSERT(otherStr, npos != otherStr.find(" from 1 trace"));

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
      case 10: {
        //---------------------------------------------------------------------
        // BSLS STREAMING
        //
        // This component does not support bslx streaming.
        //---------------------------------------------------------------------
      }  break;
      case 9: {
        //---------------------------------------------------------------------
        // ASSINGMENT OPERATOR
        //
        // This component has no assignment operator.
        //---------------------------------------------------------------------
      }  break;
      case 8: {
        //---------------------------------------------------------------------
        // SWAP FUNCTION
        //
        // This component has no swap function.
        //---------------------------------------------------------------------
      }  break;
      case 7: {
        //---------------------------------------------------------------------
        // COPY C'TOR
        //
        // This component has no copy c'tor
        //---------------------------------------------------------------------
      }  break;
      case 6: {
        //---------------------------------------------------------------------
        // EQUALITY COMPARATOR
        //
        // This component has no equality comparator
        //---------------------------------------------------------------------
      }  break;
      case 5: {
        //---------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //
        // This component has no print or output operator
        //---------------------------------------------------------------------
      }  break;
      case 4: {
        //---------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concern:
        //   That 'failureHandler', 'numBlocksInUse', and 'reportBlocksInUse'
        //   function properly.
        //
        // Plan:
        // Manipulate the failure handler and observe it with the
        // 'failureHandler' accessor.  Allocate and free some segments and
        // observe that 'numBlocksInUse' tracks the number of allocations
        // correctly.  Call 'reportBlocksInUse' and observe that it gives a
        // report appropriate to the number of blocks that are in use.
        //---------------------------------------------------------------------

        expectedDefaultAllocations = -1;    // turn off default allocator
                                            // monitoring
        Obj ta("my_allocator");

        enum {
            MAX_ALLOC_LENGTH     = 100,

            // The following 3 numbers should all be relatively prime

            SEGMENT_ARRAY_LENGTH = 10,
            ALLOC_INC            = 3,
            FREE_INC             = 7
        };

        ASSERT(ta.failureHandler() == &Obj::failureHandlerAbort);
        ta.setFailureHandler(&Obj::failureHandlerNoop);
        ASSERT(ta.failureHandler() == &Obj::failureHandlerNoop);
        ta.setFailureHandler(Obj::failureHandlerAbort);
        ASSERT(ta.failureHandler() == &Obj::failureHandlerAbort);

        bsl::ostringstream oss;
        ta.reportBlocksInUse(&oss);
        ASSERT(oss.str().empty());    // no blocks in use

        void *segments[SEGMENT_ARRAY_LENGTH] = { 0 };

        unsigned numSegments = 0;

        // do a lot of allocating and freeing, not just freeing the segment
        // most recently allocated, but rather choosing the segment to free in
        // a somewhat random fashion.

        for (int i = 0; i < 100; ++i) {
            unsigned allocIdx = (unsigned) rand() % SEGMENT_ARRAY_LENGTH;
            while (segments[allocIdx]) {
                allocIdx = (allocIdx + ALLOC_INC) % SEGMENT_ARRAY_LENGTH;
            }
            segments[allocIdx] = ta.allocate(
                       bsl::max<int>(1, (unsigned) rand() % MAX_ALLOC_LENGTH));
            ++numSegments;
            LOOP3_ASSERT(i, ta.numBlocksInUse(), numSegments,
                                           ta.numBlocksInUse() == numSegments);

            if (numSegments >= 4) {
                unsigned freeIdx = (unsigned) rand() % SEGMENT_ARRAY_LENGTH;
                while (! segments[freeIdx]) {
                    freeIdx = (freeIdx+ FREE_INC) % SEGMENT_ARRAY_LENGTH;
                }
                ta.deallocate(segments[freeIdx]);
                segments[freeIdx] = 0;
                --numSegments;
                ASSERT(ta.numBlocksInUse() == numSegments);
            }
        }

        ASSERT(3 == numSegments);
        ASSERT(3 == ta.numBlocksInUse());

        ASSERT(oss.str().empty());
        ta.reportBlocksInUse(&oss);
        const bsl::string& report = oss.str();

        ASSERT(!report.empty());
        LOOP_ASSERT(report,
                npos != report.find("3 segment(s) in allocator 'my_allocator'"
                                    " in use"));
        ASSERT(npos != report.find("main"));

        for (int i = 0; i < SEGMENT_ARRAY_LENGTH; ++i) {
            if (segments[i]) {
                ta.deallocate(segments[i]);
                --numSegments;
            }
            ASSERT(ta.numBlocksInUse() == numSegments);
        }

        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == numSegments);
      }  break;
      case 3: {
        //---------------------------------------------------------------------
        // VALUE C'TOR
        //
        // This component has no value c'tor
        //---------------------------------------------------------------------
      }  break;
      case 2: {
        //---------------------------------------------------------------------
        // DEFAULT C'TOR, PRIMARY MANIPULATORS, D'TOR
        //
        // Concern:
        //   Need to test creators and all manipulators
        //
        // Plan:
        //   Loop, using a different creator / combination of optional args
        //   every time.  Allocate segments, then free them in one of three
        //   ways:
        //:   o by calling 'deallocate'
        //:   o by calling 'release'
        //:   o by calling the d'tor
        //   use a 'bslma::TestAllocator' when possible so that if the test
        //   fails to free some memory, the bslma test allocator will detect
        //   the leak and the test will fail.
        //   When a memory leak is expected to be caught at destruction and
        //   reported to a stringstream, verify the report is reasonable.
        //---------------------------------------------------------------------

        if (verbose) cout << "DEFAULT C'TOR, PRIMARY MANIPULATORS, D'TOR\n"
                             "==========================================\n";

        bslma::TestAllocator ota;

        for (int i = 0; i < 2; ++i) {
            const bool CLEAN_DESTROY = i;

            for (char c = 'a'; c <= 'j' ; ++c) {
                Obj *pta = 0;

                bsl::stringstream oss(&ota);

                // Note that 'Obj's that are constructed without an allocator
                // arg use the gmalloc allocator singleton, not 'da'

                switch (c) {
                  case 'a': {
                    pta = new (ota) Obj();
                  } break;
                  case 'b': {
                    pta = new (ota) Obj(&oss);
                  } break;
                  case 'c': {
                    pta = new (ota) Obj(&oss,
                                        10);
                  } break;
                  case 'd': {
                    pta = new (ota) Obj(&oss,
                                        10,
                                        false);
                  } break;
                  case 'e': {
                    pta = new (ota) Obj(&oss,
                                        10,
                                        false,
                                        &ota);
                  } break;
                  case 'f': {
                    pta = new (ota) Obj("my_allocator");
                  } break;
                  case 'g': {
                    pta = new (ota) Obj("my_allocator",
                                        &oss);
                  } break;
                  case 'h': {
                    pta = new (ota) Obj("my_allocator",
                                        &oss,
                                        10);
                  } break;
                  case 'i': {
                    pta = new (ota) Obj("my_allocator",
                                        &oss,
                                        10,
                                        false);
                  } break;
                  case 'j': {
                    pta = new (ota) Obj("my_allocator",
                                        &oss,
                                        10,
                                        false,
                                        &ota);
                  } break;
                }

                ASSERT(0 != pta);
                Obj& ta = *pta;

                ASSERT(ta.failureHandler() == &Obj::failureHandlerAbort);
                ta.setFailureHandler(&Obj::failureHandlerNoop);
                ASSERT(ta.failureHandler() == &Obj::failureHandlerNoop);
                ta.setFailureHandler(Obj::failureHandlerAbort);
                ASSERT(ta.failureHandler() == &Obj::failureHandlerAbort);
                ta.setFailureHandler(&Obj::failureHandlerNoop);
                ASSERT(ta.failureHandler() == &Obj::failureHandlerNoop);

                void *segment = ta.allocate(100);
                ASSERT(1 == ta.numBlocksInUse());
                ta.deallocate(segment);
                ASSERT(0 == ta.numBlocksInUse());

                segment = ta.allocate(50);
                ASSERT(1 == ta.numBlocksInUse());
                ta.release();
                ASSERT(0 == ta.numBlocksInUse());

                segment = ta.allocate(200);
                if (CLEAN_DESTROY) {
                    ta.deallocate(segment);
                }

                // If 'CLEAN_DESTROY' there will be no blocks in use and the
                // d'tor will not write a report.  Otherwise, 'segment' will
                // still be unfreed and a report will be written.

                ASSERT(! CLEAN_DESTROY == ta.numBlocksInUse());
                ota.deleteObject(pta);

                const bool OSS_REPORT = 'a' != c && 'f' != c && !CLEAN_DESTROY;

                const bsl::string& report = oss.str();
                expectedDefaultAllocations += OSS_REPORT;

                ASSERT(report.empty() == !OSS_REPORT);
                if (OSS_REPORT) {
                    ASSERT(npos != report.find("main"));
                    ASSERT(npos != report.find("1 segment(s) in use"));
                    ASSERT(npos != report.find("Error: memory leaked"));
                    ASSERT(npos != report.find("allocate"));
                    ASSERT((c >= 'f') ==
                                        (npos != report.find("my_allocator")));
                }
            }
        }
      }  break;
      case 1: {
        //---------------------------------------------------------------------
        // BREATHING TEST
        //
        // Set 'veryVerbose' to make this test abort when the test allocator
        // is destructed.
        //---------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        bslma::NewDeleteAllocator otherTa;

        int maxDepths[] = { 100, 10, 5, 4, 3 };
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
                    ta.setFailureHandler(&Obj::failureHandlerNoop);
                }
            }

            const bsl::string& outStr = out.str();
            ++expectedDefaultAllocations;                // out.str() uses da

            if (!PLAT_WIN || DEBUG_ON) {
                bsl::size_t pos = 0;
                ASSERT(npos != (pos = outStr.find("StackTraceTestAllocator",
                                                               pos)));
                ASSERT(npos != (pos = outStr.find("allocate",  pos)));
                ASSERT(npos != (pos = outStr.find("caseOne_c", pos)));
                ASSERT(npos != (pos = outStr.find("caseOne_b", pos)));
                if (maxDepths[d] >= 4) {
                    ASSERT(npos != (pos = outStr.find("caseOne_a", pos)));
                }
                pos = 0;
                LOOP_ASSERT(outStr,
                                npos != (pos = outStr.find("main",      pos)));
                if (maxDepths[d] > 4) {
                    LOOP2_ASSERT(outStr, maxDepths[d],
                                npos != (pos = outStr.find("main",  pos + 4)));
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
                        expectedDefaultAllocations < 0 ||
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
