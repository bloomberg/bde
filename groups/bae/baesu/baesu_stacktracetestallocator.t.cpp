// baesu_stacktracetestallocator.t.cpp                                -*-C++-*-
#include <baesu_stacktracetestallocator.h>

#include <baesu_stacktrace.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_threadutil.h>

#include <bdesu_fileutil.h>
#include <bdeu_random.h>
#include <bdeu_string.h>

#include <bslma_bufferallocator.h>
#include <bslma_deallocatorguard.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocator.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocatorexception.h>  // for testing only
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_fstream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <malloc.h>
#include <setjmp.h>

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
//-----------------------------------------------------------------------------
// [23] o usage example
// [22] o segment header test -- use 'my_SegmentHeader' to test
//      o magic number
//      o pointer to allocator
//      o validity of next segment
//      o validity of pointer from prev segment
// [20] o testing for undefined behavior with setjmp/longjmp
//      o 'numRecordedFrames >= 2'
//      o 'name == 0' is handled by substituting "<unnamed>" form 'name'.
// [19] o stack depth testing
//      o verify that the stack depth reported is exactly what
//        was specified at construction.
//      o do this test by having a recursive call stack, and then counting
//        the number of occurrences of the name of the recursing routine in
//        the report.
// [18] o exception safety
//      o Pass the allocator a bslma::TestAllocator rigged to throw exceptions
//        and verify that the allocator handles this properly.
// [17] o alignment & min size test
//      o allocate segments from 0 byte to 100 bytes long, several segments for
//        each size.  Calculate alignment
//        via 'bsls::AlignmentUtil::calculateAlignmentFromSize', and verify
//        that the segment return always satisfies the alignment requirement.
//        Write over the full length of the segment. Use 'bslma::TestAllocator'
//        as the underlying allocator as it will detect overruns if the any of
//        the segments were smaller than requested.
//      o fill segments with random byte, verify they still contain this byte
//        when freed
//      o store segments in struct { void *d_p; int d_len; char d_fill;};
//      o have array of segments, use a random number generator to determine
//        which is allocated or freed next.  Maintain count of existing
//        segments so never exceed available slots and only free when there is
//        something to be freed.  Also monitor 'numBlocksInUse' during this
//        and make sure it is accurate.
// [16] o demanglingPreferredFlag
//      o #ifdef appropriate for platform, do reports with & without
//        'demanglingPreferredFlag' by searching for
//        'BloombergLP::baesu_StackTraceTestAllocator::' in the report -- it
//        will be present if demangling is done, it won't if not
// [15] o Underrun detection
//      o Verify that writing before the beginning of the buffer from 1 byte
//        to the size of 4 pointers (a) is always detected, and (b) does not
//        result in segfaults, and (c) results in failure handling appropriate
//        for 'failureHandler'
//      o Verify the same thing for single-byte writes up to the size of 1 ptr
//        before the start -- such writes will always hit the magic number.
// [14] Deallocation errors test
//      o repeat all tests with and without 'noAbort'
//      o attempt to deallocate segments allocated with 'malloc' and
//        global 'new' (actually, since we override 'new' to call 'malloc'
//        for the sake of other tests, we can only check 'malloc').
//      o Attempt to free segments created by 'bslma::TestAllocator' and
//        'bcema_TestAllocator'
//      o Pass a pointer that is not pointer-aligned, triggering alignment
//        error detection
// [13] Calling 'release' successfully frees all allocated segments
//      o add test to verify that calling it when no segments are outstanding
//        does no harm
//      o call it with 0 through 100 segments outstanding
//      o verify that d'tor also successfully frees all allocated segments
//      o if d'tor aborts, it should be before segments are released (to
//        provide a more meaningful core dump)
// [12] Thread-safety test
//      o Overhaul so that threads do less work and more allocating.
//      o Create allocator that 'has-a' test allocator, and specifically keeps
//        a count of how many times allocate or deallocate is currently
//        happening, and asserts they're never happening at once.
//      o Have test set up a barrier before allocating, allocates, a bunch of
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
//      o verify that if 'reportBlocksInUse' is called with no args, it writes
//        its report to the stream specified at construction of the allocator.
//      o verify that if 'reportBlocksInUse' is called with no memory
//        outstanding, it produces no output.
// [ 3] Value c'tor (N/A)
// [ 2] All constructors, destructor, all manipulators
//      o Verify that underlying testallocator passed is providing memory, and
//        freeing memory when 'deallocate' is called.
//      o Verify that the default allocator is not used when no allocator is
//        passed to the STTA at construction.
//      X Instrument new/delete like mallocfreeallocator.t.cpp to verify
//        default allocator configuration not coming from new/delete.  (Effort
//        was abandoned -- stringstream uses global 'new' & 'delete', even when
//        an allocator is passed.o -- huge number of calls to 'new' & 'delete'
//        beyond our control)
//      o assert there is no output about leaked segments before d'tor is
//        called
//      o repeat all tests with and without abort flag (no abort is expected
//        in this case
//      o call 'ASSERT(oss.str().empty());' before d'tor is called
//      o call one instance of object, destroy it with no memory outstanding,
//        verify no report written by d'tor
//      o verify allocator name in report
//      o verify operation of release with & without memory allocated.
// [ 1] Breathing test
//      o use 'leakTwiceA', verify order of 'leakTwice{C,B,A}' in trace.
//-----------------------------------------------------------------------------
//
//                        ----------------------------
//                        Note on 'setjmp' / 'longjmp'
//                        ----------------------------
//
// It was hoped that having failure handlers and assert handlers that do
// 'longjmp's would provide a flexible, portable testing mechanism that would
// work even when exceptions were disabled.  This turned out to work very well
// on Unix, but on Windows 'longjmp' turned out to be very flaky and caused
// unpredictable crashes, so we had to go through the test driver and
// disable the many places 'longjmp' was called on Windows.  Fortunately,
// 'setjmp' by itself turned out to be reasonably benign and we did not have
// to circumvent the 'setjmp' calls on Windows.
//
// It is inadvisable to use 'setjmp' / 'longjmp' in future test drivers.
//
//                      ---------------------------------
//                      Note on Foiling compiler inlining
//                      ---------------------------------
//
// Optimizing compilers have a tendency to inline function calls, which will
// cause these test cases to fail when expected function names are missing from
// stack traces.  To foil this, function pointers are stored in arrays of
// function pointers and looked up at run time.
//
// There is also a problem where if a function call is the last thing in a
// function, the compiler may replace a call to a function to a chaining jump
// to the beginning of a a function.  To foil this we put a little activity
// after the function calls in functions that we want to appear in stack
// traces.

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

#if !defined(BSLS_PLATFORM__OS_WINDOWS) || defined(BDE_BUILD_TARGET_DBG)
enum { CAN_FIND_SYMBOLS = 1 };
#else
enum { CAN_FIND_SYMBOLS = 0 };
#endif

}  // close unnamed namespace

//=============================================================================
// GLOBAL HELPER VARIABLES AND TYPES FOR TESTING
//-----------------------------------------------------------------------------

typedef baesu_StackTraceTestAllocator Obj;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

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

// ============================================================================
//                           RETOOL 'NEW' & 'DELETE'
// ============================================================================

#if 0
// This experiment was abandoned -- it turned out the stringstreams were
// calling global 'new' & 'delete' quite a bit, even when passed an allocator,
// and there's not anything this component can do about it.

static int numGlobalNewCalls    = 0;
static int numGlobalDeleteCalls = 0;

#ifdef BDE_BUILD_TARGET_EXC
void *operator new(size_t size) throw(std::bad_alloc)
#else
void *operator new(size_t size)
#endif
    // Trace use of global operator new.  Note that we must use printf
    // to avoid recursion.
{
    ++numGlobalNewCalls;
    return malloc(size);
}

#ifdef BDE_BUILD_TARGET_EXC
void operator delete(void *address) throw()
#else
void operator delete(void *address)
#endif
    // Trace use of global operator delete.
{
    ++numGlobalDeleteCalls;
    free(address);
}

#endif

// ============================================================================
//                               my_jmpAbort
// ============================================================================

jmp_buf my_setJmpBuf;    // Note 'jmp_buf' is an array type

void my_assertHandlerLongJmp(const char *,  // text
                             const char *,  // fail
                             int         )  // lineo
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    // setjmp / longjmp is flaky on Windows

    ASSERT(0);
#endif

    longjmp(my_setJmpBuf, true);
}

void my_failureHandlerLongJmp()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    // setjmp / longjmp is flaky on Windows

    ASSERT(0);
#endif

    longjmp(my_setJmpBuf, true);
}

bool my_failureHandlerFlag = false;
void my_failureHandlerSetFlag()
{
    my_failureHandlerFlag = true;
}

#ifdef BSLS_PLATFORM__OS_WINDOWS
enum { ABORT_LIMIT = 1 };
#else
enum { ABORT_LIMIT = 2 };
#endif

// ============================================================================
// There is a problem with some of the optimizers being *VERY* clever about
// inlining calls.  Even if you take a pointer to a function and then call
// through the pointer,
// ============================================================================

typedef void (*VoidFuncPtr)();

// Note bsl::vector has a bug with storing function pointers that is being
// fixed on a separate branch as this is being written.

VoidFuncPtr voidFuncs[10];
unsigned voidFuncsSize = 0;

unsigned idxVoidFuncRecurser;
unsigned idxVoidFuncLeakTwiceA;
unsigned idxVoidFuncLeakTwiceB;
unsigned idxVoidFuncLeakTwiceC;

#ifdef BSLS_PLATFORM__CPU_64_BIT
static
const UintPtr my_HIGH_ONES = (UintPtr) 1111111111 * 10 * 1000 * 1000 * 1000;
#else
static
const UintPtr my_HIGH_ONES = 0;
#endif

static const UintPtr my_UNFREED_SEGMENT_MAGIC = 1222222221 + my_HIGH_ONES;
static const UintPtr my_FREED_SEGMENT_MAGIC   = 1999999991 + my_HIGH_ONES;

struct my_SegmentHeader {
    // It was felt that we should go totally white box and test the internal
    // data structures of this component, even though that is not generally
    // done in bde.
    //
    // This is an exact copy of the private 'SegmentHeader' 'struct' in the
    // imp file.

    // DATA
    my_SegmentHeader              *d_next_p;      // next object in the
                                                  // doubly-linked list

    my_SegmentHeader             **d_prevNext_p;  // pointer to the 'd_next_p'
                                                  // field of the previous
                                                  // object, or the head ptr of
                                                  // the linked list if there
                                                  // is no previous object.

    baesu_StackTraceTestAllocator *d_allocator_p; // creator of segment

    UintPtr                        d_magic;       // Magic number -- has
                                                  // different values for
                                                  // an unfreed segment, a
                                                  // freed segment, or the
                                                  // head node.
};

// ============================================================================
//                            CASE-SPECIFIC CODE
// ============================================================================

                                    // -------
                                    // case 19
                                    // -------

bslma::Allocator *recurserAllocator;
int recurseDepth;

void recurser()
{
    if (--recurseDepth >= 0) {
        (*voidFuncs[idxVoidFuncRecurser])();
    }
    else {
        (void) recurserAllocator->allocate(100);
    }

    ++recurseDepth;
}

                                    // -------
                                    // case 17
                                    // -------

namespace AlignAndFillTest {

struct NotEqual {
    // Boolean operator for search arrays of ints to find if any element does
    // not equal the value this 'struct' is initialized with.

    char d_value;

    // CREATORS
    explicit
    NotEqual(char value) : d_value(value) {}

    bool operator()(char test)
    {
        return test != d_value;
    }
};

}  // close namespace AlignAndFillTest

                                    // -------
                                    // case 12
                                    // -------

namespace MultiThreadedTest {

class TouchyAllocator : public bslma::Allocator {
    // This is a thread-unsafe allocator that will detect any attempt to do
    // thread-unsafe allocation or deallocation.

    volatile bool             d_inUse;
    mutable bslma::Allocator *d_allocator;

  private:
    // NOT IMPLEMENTED
    TouchyAllocator& operator=(const TouchyAllocator&);
    TouchyAllocator(const TouchyAllocator&);

  public:
    // CREATOR
    explicit
    TouchyAllocator(bslma::Allocator *allocator)
    : d_inUse(false)
    , d_allocator(allocator) {}

    // MANIPULATORS
    virtual void *allocate(bslma::Allocator::size_type size)
    {
        ASSERT(!d_inUse);
        d_inUse = true;
        void *ret = d_allocator->allocate(size);
        ASSERT(d_inUse);
        d_inUse = false;
        return ret;
    }

    virtual void deallocate(void *address)
    {
        ASSERT(!d_inUse);
        d_inUse = true;
        d_allocator->deallocate(address);
        ASSERT(d_inUse);
        d_inUse = false;
    }
};

struct Functor {
    // Multithreaded opeartor.

    enum { NUM_THREADS = 10 };

    // TYPES
    typedef void (Functor::*FuncPtr)();

    // DATA
    static FuncPtr          s_funcPtrs[10];
    static const unsigned   s_nest1Idx    = 1;
    static const unsigned   s_nest2Idx    = 2;
    static const unsigned   s_nest3Idx    = 3;
    static const unsigned   s_nest4Idx    = 4;
    static const unsigned   s_allocOneIdx = 5;
    static const unsigned   s_freeSomeIdx = 6;

    static bsls::AtomicInt  s_threadRand;
    static bcemt_Barrier    s_startBarrier;
    static bcemt_Barrier    s_underwayBarrier;

    bsl::vector<int *>      d_alloced;
    int                     d_randNum;
    int                     d_nestDepth;
    int                     d_numAllocations;
    bslma::Allocator       *d_allocator_p;

    // CLASS METHODS
    static
    void initFuncPtrs()
    {
        // We must fully specify names of methods when taking pointers to
        // them -- major nuisance.

        s_funcPtrs[s_nest1Idx]    = &MultiThreadedTest::Functor::nest1;
        s_funcPtrs[s_nest2Idx]    = &MultiThreadedTest::Functor::nest2;
        s_funcPtrs[s_nest3Idx]    = &MultiThreadedTest::Functor::nest3;
        s_funcPtrs[s_nest4Idx]    = &MultiThreadedTest::Functor::nest4;
        s_funcPtrs[s_allocOneIdx] = &MultiThreadedTest::Functor::allocOne;
        s_funcPtrs[s_freeSomeIdx] = &MultiThreadedTest::Functor::freeSome;
    }

    // CREATORS
    explicit
    Functor(bslma::Allocator *traceAllocator)
    : d_alloced(traceAllocator)
    , d_numAllocations(0)
    , d_allocator_p(traceAllocator)
    {
        bdeu_Random::generate15(&d_randNum, ++s_threadRand * 987654321);
    }

    Functor(const Functor& original)
    : d_alloced(original.d_alloced,
                original.d_allocator_p)
    , d_numAllocations(0)
    , d_allocator_p(original.d_allocator_p)
    {
        bdeu_Random::generate15(&d_randNum,
                                original.d_randNum + ++s_threadRand * 9876543);
    }

  private:
    // MANIPULATORS
    void freeOne(int index)
    {
        ASSERT((unsigned) index < d_alloced.size());
        int *segment = d_alloced[index];
        d_allocator_p->deallocate(segment);
        d_alloced[index] = d_alloced.back();
        d_alloced.pop_back();
    }

    void freeSome()
        // Free a random number of segments in random order, but never free if
        // there are less than 5 segments -- so if there are >= 4 segments
        // when we start, there will be >= 4 segments when we finish.
    {
        int sz = (int) d_alloced.size();
        int maxNumToFree = bsl::max(0, bsl::min(sz / 2, sz - 4));
        int r = bdeu_Random::generate15(&d_randNum);    // always positive
        int numToFree = r % (maxNumToFree + 1);
        for (int i = 0; i < numToFree; ++i) {
            r = ((r + 29) & 0xffff);                    // always positive

            sz = (int) d_alloced.size();
            BSLS_ASSERT(sz > 0);
            freeOne(r % sz);
        }

        ASSERT(d_alloced.size() >= 4 || !numToFree);
    }

    void allocOne();

    void nest4();

    void nest3();

    void nest2();

    void nest1();

  public:
    void operator()();
};
Functor::FuncPtr Functor::s_funcPtrs[10];
bsls::AtomicInt  Functor::s_threadRand(0);
bcemt_Barrier    Functor::s_startBarrier(NUM_THREADS);
bcemt_Barrier    Functor::s_underwayBarrier(NUM_THREADS + 1);

void Functor::allocOne()
{
    int allocLength = 1 + (bdeu_Random::generate15(&d_randNum) & 0x7f);

    if (++d_numAllocations <= 10) {
        s_startBarrier.wait();    // Get everybody lined up so they all try to
                                  // allocate at the same time the first few
                                  // iterations to stress thread safety
    }

    int *segment = (int *) d_allocator_p->allocate(allocLength);
    d_alloced.push_back(segment);
}

void Functor::nest4()
{
    if (++d_nestDepth > 10) {
        return;
    }

    FuncPtr allocOnePtr = s_funcPtrs[s_allocOneIdx];
    FuncPtr freeSomePtr = s_funcPtrs[s_freeSomeIdx];
    for (int i = 0; i < 3000; ++i) {
        for (int j = 0; j < 2; ++j) {
            (this->*allocOnePtr)();
        }
        (this->*freeSomePtr)();

        if (75 == i) {
            s_underwayBarrier.wait();

            // Main thread will now gather a report on unfreed segments while
            // the other threads continue to thrash.
        }
    }

    --d_nestDepth;                    // Guarantee routine calls, not chaining
}

void Functor::nest3()
{
    if (++d_nestDepth > 10) {
        return;
    }

    (this->*s_funcPtrs[s_nest4Idx])();

    --d_nestDepth;                    // Guarantee routine calls, not chaining
}

void Functor::nest2()
{
    if (++d_nestDepth > 10) {
        return;
    }

    (this->*s_funcPtrs[s_nest3Idx])();

    --d_nestDepth;                    // Guarantee routine calls, not chaining
}

void Functor::nest1()
{
    if (++d_nestDepth > 10) {
        return;
    }

    (this->*s_funcPtrs[s_nest2Idx])();

    --d_nestDepth;                    // Guarantee routine calls, not chaining
}

void Functor::operator()()
{
    ASSERT(0 == d_alloced.size());

    d_nestDepth = 0;

    (this->*s_funcPtrs[s_nest1Idx])();

    ASSERT(!d_nestDepth);
}

}  // close namespace MultiThreadedTest

                                // ------
                                // case 1
                                // ------

static
bslma_Allocator *leakTwiceAllocator = 0;
int leakTwiceCount;

static
void leakTwiceC()
{
    for (int i = 0; i < 100; ++i) {
        leakTwiceAllocator->allocate(84);    // leak in second place
    }
}

// not static

static
void leakTwiceB()
{
    if (++leakTwiceCount > 10) {
        return;
    }

    (*voidFuncs[idxVoidFuncLeakTwiceC])();

    --leakTwiceCount;                 // force routine call instead of chaining
}

static
void leakTwiceA()
{
    if (++leakTwiceCount > 10) {
        return;
    }

    ASSERT(leakTwiceAllocator);

    leakTwiceAllocator->allocate(25);        // leak in first place

    (*voidFuncs[idxVoidFuncLeakTwiceB])();

    --leakTwiceCount;                 // force routine call instead of chaining
}

//=============================================================================
// MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test        = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose         = argc > 2;
    veryVerbose     = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST CASE " << test << endl;

    bslma::TestAllocator da;
    bslma::DefaultAllocatorGuard guard(&da);

    memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

    int expectedDefaultAllocations = 0;

    idxVoidFuncRecurser   = 0;
    idxVoidFuncLeakTwiceA = 1;
    idxVoidFuncLeakTwiceB = 2;
    idxVoidFuncLeakTwiceC = 3;
    voidFuncs[idxVoidFuncRecurser]   = &recurser;
    voidFuncs[idxVoidFuncLeakTwiceA] = &leakTwiceA;
    voidFuncs[idxVoidFuncLeakTwiceB] = &leakTwiceB;
    voidFuncs[idxVoidFuncLeakTwiceC] = &leakTwiceC;

    ASSERT(voidFuncsSize <= sizeof voidFuncs / sizeof *voidFuncs);

    switch (test) { case 0:
      case 22: {
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
            baesu_StackTraceTestAllocator ta("Test Allocator");
            ta.setFailureHandler(&Obj::failureHandlerNoop);
            bslma::DefaultAllocatorGuard guard(&ta);

            bsl::string captain = getCaptain("shipscrew.txt");

            cout << "The captain is: " << captain << bsl::endl;
        }

        // When 'ta' is destroyed, the following message is written to stdout.
        //
        // ====================================================================
        // Error: memory leaked:
        // 1 segment(s) in allocator 'Test Allocator' in use.
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
        //
        // Finally, we see that the leaked memory was in the 'setCook' method
        // line 232 (line numbers are generally the line after the subroutine
        // call in question -- the call to 'copy' was on line 231).  The
        // destructor neglected to deallocate the cook's name.
        //
        // Note the following:
        //: o If we hadn't called
        //:   'setFailureHandler(&Obj::failureHandlerNoop)', the above report
        //:   would have been followed by a core dump.  Since 'isNoAbort' was
        //:   set, 'ta's destructor instead frees all leaked memory after
        //:   giving the report and returns.
        //: o Output will vary by platform.  Not all platforms support line
        //:   number information and demangling.  This report was generated on
        //:   AIX, and a couple of AIX quirks are visible -- identifiers have a
        //:   '.' prepended, and the constructor name got converted to '__ct'.

        bdesu_FileUtil::remove("shipscrew.txt");
      }  break;
      case 21: {
        //---------------------------------------------------------------------
        // WHITE-BOX EXAMINATION OF SEGMENT HEADER
        //
        // Concern:
        //   That the segment header is as it should be.  This test is
        //   redundant with asserts already done every time a segment is
        //   deallocated.
        //
        // Plan:
        //: 1 Allocate two segments, and use Use the 'my_SegmentHeader'
        //:   redefinition of the segment header to examine the segment header
        //:   on an allocated segments.
        //---------------------------------------------------------------------

        if (verbose) cout << "WHITE-BOX EXAMINATION OF SEGMENT HEADER\n"
                             "=======================================\n";

        Obj ta;

        my_SegmentHeader *seg1 = (my_SegmentHeader *) ta.allocate(10);
        --seg1;
        my_SegmentHeader *seg2 = (my_SegmentHeader *) ta.allocate(10);
        --seg2;

        ASSERT(seg2->d_next_p == seg1);
        ASSERT(&seg2->d_next_p == seg1->d_prevNext_p);
        ASSERT(seg1 == *seg1->d_prevNext_p);
        ASSERT(seg2 == *seg2->d_prevNext_p);
        ASSERT(&ta == seg1->d_allocator_p);
        ASSERT(&ta == seg2->d_allocator_p);
        ASSERT(my_UNFREED_SEGMENT_MAGIC == seg1->d_magic);
        ASSERT(my_UNFREED_SEGMENT_MAGIC == seg2->d_magic);

        ta.release();
      }  break;
      case 20: {
        //---------------------------------------------------------------------
        // TESTING CORRECTNESS OF ARGUMENTS OF CONSTRUCTOR
        //
        // Concern:
        //   That the constructor properly checks arguments passed in.  The
        //   only things you could get wrong are passing 0 to 'name', and
        //   specifying 'maxNumRecordedFrames < 2'.  In the case of passing '0'
        //   to 'name', it's not an error per se, 'name' just defaults to
        //   '<unnamed>', the same as if 'name' were unspecified.
        //
        // Plan:
        //: 1 Create an STTA, specifying 0 to the 'name' c'tor argument, leak
        //:   some memory and do a report, and observe that the allocator name
        //:   reported is '<unnamed>'.
        //: 2 Set the assert handler for 'BSLS_ASSERT' to do a 'longjmp'.
        //: 3 Attempt to create an 'STTA' with no name specified, passing '1'
        //:   'maxNumRecordedFrames'.  Observe that the c'tor doesn't return
        //:   and 'longjmp' is invoked.
        //: 4 Attempt to create an 'STTA' with a name specified, passing '1'
        //:   'maxNumRecordedFrames'.  Observe that the c'tor doesn't return
        //:   and 'longjmp' is invoked.
        //---------------------------------------------------------------------

        if (verbose) cout << "TESTING INVATIANTS OF CONSTRUCTOR\n"
                             "=================================\n";

        expectedDefaultAllocations = -1;    // turn off default alloc checking

        bsl::stringstream ss;

        {
            Obj ta((const char *) 0, &ss);
            (void) ta.allocate(100);
            ta.reportBlocksInUse();
            ASSERT(npos != ss.str().find(
                               "segment(s) in allocator '<unnamed>' in use."));
            ta.release();
        }
        ss.str("");

#if defined(BSLS_ASSERT_IS_ACTIVE) && !defined(BSLS_PLATFORM__OS_WINDOWS)
        bsls::Assert::setFailureHandler(my_assertHandlerLongJmp);

        bool caught = false;
        if (setjmp(my_setJmpBuf)) {
            caught = true;
        }
        else {
            Obj ta(&ss, 1);

            ASSERT(0 && "Didn't catch too few num recorded frames");
        }
        ASSERT(caught);

        caught = false;
        if (setjmp(my_setJmpBuf)) {
            caught = true;
        }
        else {
            Obj ta("alpha", &ss, 1);

            ASSERT(0 && "Didn't catch too few num recorded frames");
        }
        ASSERT(caught);
#endif

        ASSERT(ss.str().empty());
      }  break;
      case 19: {
        //---------------------------------------------------------------------
        // CONFIGURED STACK DEPTH TEST.
        //
        // Concern:
        //   That the number of stack frames reported is exactly what's
        //   configured at construction.
        //
        // Plan:
        //: 1 Loop over varying values of 'depth'.  For each loop:
        //:   o Set a variable 'RECORDED_FRAMES' substantially less than
        //:     'depth'.
        //:   o Create an STTA configured with 'RECORDED_FRAMES' specified to
        //:     the 'maxRecordedFrames' argument.
        //:   o Initialize the static variable 'recurserAllocator' to point to
        //:     the object under test, and the static variable 'recurseDepth'
        //:     to the value of 'depth'.  These two variables will be used by
        //:     the 'recurser' function.
        //:   o Call the 'recurser' function.  It will recurse at least
        //:     'recurseDepth' times, then leak some memory allocated by the
        //:     object under test.
        //:   o Call 'reportBlocksInUse' to get the report about the leaked
        //:     segment.
        //:   o Count the number of times the function name 'recurser' occurs
        //:     in the report created by 'reportBlocksInUse', and verify that
        //:     this number is one less than 'RECORDED_FRAMES'.
        //---------------------------------------------------------------------

        if (verbose) cout << "CONFIGURED STACK DEPTH TEST\n"
                             "===========================\n";

        expectedDefaultAllocations = -1;    // turn off default alloc checking

        if (!CAN_FIND_SYMBOLS) break;    // This test won't work unless we can
                                         // access routine names in the stack
                                         // trace.

        bsl::stringstream ss;

        for (int depth = 6; depth < 20; ++depth) {
            const int RECORDED_FRAMES = depth - 4;

            Obj ta(&ss, RECORDED_FRAMES);

            recurserAllocator = &ta;
            recurseDepth = depth;
            recurser();

            ASSERT(depth == recurseDepth);

            ASSERT(ss.str().empty());

            ta.reportBlocksInUse();
            const bsl::string& report = ss.str();
            ta.release();

            int numRecurserInTrace = 0;
            bsl::size_t pos = report.find("allocate");
            while (true) {
                pos = report.find("recurser", pos);
                if (npos == pos) {
                    break;
                }
                ++numRecurserInTrace;
                ++pos;
            }

            ASSERT(numRecurserInTrace + 1 == RECORDED_FRAMES);
            ss.str("");
        }
      }  break;
      case 18: {
        //---------------------------------------------------------------------
        // EXCEPTION-SAFETY TEST
        //
        // Concern:
        //   That the allocator is exception-safe.
        //
        // Plan:
        //   Note that care must be taken with stringstreams as it is not
        //   possible, once they've allocated any memory, to get them to
        //   release it unless they're destroyed.
        //
        //: 1 Use the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_{BEGIN,END}' macros
        //:   to test an STTA object for exception safety.
        //:   o Pass the bslma test allocator to the object at construction,
        //:     with the bslma test allocator configured to throw exceptions.
        //:   o Also have the default allocator be the configured bslma test
        //:     allocator.  If any memory is leaked due to exceptions being
        //:     unsafe, we'll find out about it when we destroy the bslma test
        //:     allocator.
        //:   o Allocate a segment and call 'reportBlocksInUse' with a
        //:     stringstream that uses 'sta'.  We create a new stringstream for
        //:     this purpose so all it's memory will be released when
        //:     subsequent exceptions are thrown.
        //---------------------------------------------------------------------

        if (verbose) cout << "TEST ALLOCATOR LIMIT\n"
                             "====================\n";

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "Testing for exception thrown after exceeding "
                          << "allocation limit." << endl;

        expectedDefaultAllocations = -1;    // turn off default alloc checking

        bslma::TestAllocator sta;
        bslma::DefaultAllocatorGuard guard(&sta);
        bslma::TestAllocator ssTa;
        bsl::stringstream ss(&ssTa);
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sta) {
            Obj ta("alpha", &ss, 8, true, &sta);;

            void *p = ta.allocate(100);

            bslma_DeallocatorGuard<bslma::Allocator> guard(p, &ta);

            bsl::stringstream ssR(&sta);
            ta.reportBlocksInUse(&ssR);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#else
        if (verbose) cout << "No testing.  Exceptions are not enabled."
                          << endl;
#endif
      }  break;
      case 17: {
        //---------------------------------------------------------------------
        // ALIGNMENT & RANDOM ALLOCATE / FREE TEST
        //
        // Concerns:
        //: o That allocated segments are properly aligned
        //: o That the allocator functions properly when a large number of
        //:   segments of varying sizes are allocated and freed in random
        //:   order
        //
        // Plan:
        //: 1 Maintain a large array of objects of 'struct' 'Segment', where
        //:   'Segment' defines a record that will store a length, an expected
        //:   alignment, a randomly-generated fill char, a pointer to an
        //:   allocated segment, and a state flag to indicate whether the
        //:   segment has been allocated or not.
        //: 2 Define the 'length' field ('d_len') to have values which will
        //:   provoke all possible alignments, and the 'alignment' field
        //:   ('d_align') to have the alignment requirement appropriate for
        //:   'd_len'.
        //: 3 Iterate 128K times, randomly chosing whether to allocate or free
        //:   each iteration.
        //:   o Upon allocating, fill each segment with a random byte, and save
        //:     the byte.  Check that the segment meets the alignment
        //:     requirement appropriate for its length.
        //:   o When freeing, verify the segment is still filled with the byte
        //:     it was filled with when allocated.
        //:   o Check the 'numBlocksInUse' accessor to verify that it properly
        //:     tracks its expected value.
        //---------------------------------------------------------------------

        if (verbose) cout << "ALIGNMENT & RANDOM ALLOCATE / FREE TEST\n"
                             "=======================================\n";

        namespace TC = AlignAndFillTest;

        enum { NUM_SIZES        = 128,
               MAX_NUM_SEGMENTS = NUM_SIZES * 2,
               QUARTER_SEGMENTS = MAX_NUM_SEGMENTS / 4,
               ITERATIONS       = 128 << 10 };    // 128K

        struct Segment {
            char *d_ptr;
            int   d_len;
            int   d_align;
            char  d_fill;
            bool  d_alloced;
        } segments[MAX_NUM_SEGMENTS];

        // Initialize the 'd_len', 'd_align', and 'd_fill' fields.

        const int ptrAlign = bsls::AlignmentUtil::calculateAlignmentFromSize(
                                                               sizeof(void *));

        memset(segments, 0, sizeof(segments));
        for (int i = 0; i < MAX_NUM_SEGMENTS; ++i) {
            Segment& s = segments[i];
            s.d_len = i / 2;

            // 'AlignmentUtil' fails if passed an argument of 0

            s.d_align = !s.d_len
                      ? 8
                      : bsls::AlignmentUtil::calculateAlignmentFromSize(
                                                                      s.d_len);
            s.d_align = bsl::max<int>(s.d_align, ptrAlign);

            // verify one bit of alignment set

            ASSERT(0 == (s.d_align & (s.d_align - 1)));
        }

        Obj ta;
        int randNum;
        bdeu_Random::generate15(&randNum, 987654321);

        // allocate 100% of the segments, in random order

        for (int i = 0; i < MAX_NUM_SEGMENTS; ++i) {
            int index = bdeu_Random::generate15(&randNum) % MAX_NUM_SEGMENTS;
            while (segments[index].d_alloced) {
                index = (index + 1) % MAX_NUM_SEGMENTS;
            }

            Segment& s = segments[index];

            s.d_ptr     = (char *) ta.allocate(s.d_len);
            s.d_alloced = true;
            s.d_fill    = (char) bdeu_Random::generate15(&randNum);
            memset(s.d_ptr, s.d_fill, s.d_len);

            ASSERT(!s.d_ptr == !s.d_len);

            // verify segment aligned

            LOOP3_ASSERT((void *) s.d_ptr, s.d_len, s.d_align,
                                   0 == ((UintPtr) s.d_ptr & (s.d_align - 1)));
        }
        int numSegments = MAX_NUM_SEGMENTS;
        int numBlocksInUse = numSegments - 2;
        ASSERT((int) ta.numBlocksInUse() == numBlocksInUse);

        // Now go around chosing segments at random to be allocated or freed
        // in random order.

        for (int i = 0; i < ITERATIONS; ++i) {
            const bool alloc = numSegments > 3 * QUARTER_SEGMENTS
                             ? false
                             : numSegments <     QUARTER_SEGMENTS
                             ? true
                             : (bdeu_Random::generate15(&randNum) & 1);

            // Steer the # of segments allocated to be at around half the
            // slots, give or take a quarter.

            int index = bdeu_Random::generate15(&randNum) % MAX_NUM_SEGMENTS;
            while (alloc == segments[index].d_alloced) {
                index = (index + 1) % MAX_NUM_SEGMENTS;
            }
            Segment& s = segments[index];

            if (alloc) {
                // allocate

                ASSERT(!s.d_alloced);
                ASSERT(!s.d_ptr);

                s.d_ptr     = (char *) ta.allocate(s.d_len);
                s.d_alloced = true;
                ++numSegments;
                if (s.d_ptr) {
                    ++numBlocksInUse;
                }

                ASSERT(!s.d_ptr == !s.d_len);
                s.d_fill = (char) bdeu_Random::generate15(&randNum);
                memset(s.d_ptr, s.d_fill, s.d_len);

                // verify segment aligned

                LOOP3_ASSERT((void *) s.d_ptr, s.d_len, s.d_align,
                                   0 == ((UintPtr) s.d_ptr & (s.d_align - 1)));
            }
            else {
                // free

                ASSERT(s.d_alloced);
                ASSERT(!s.d_ptr == !s.d_len);

                char *end = s.d_ptr + s.d_len;
                char *f = bsl::find_if(s.d_ptr, end, TC::NotEqual(s.d_fill));
                ASSERT(f == end);
                memset(s.d_ptr, ~s.d_fill, s.d_len);

                ta.deallocate(s.d_ptr);
                s.d_ptr = 0;
                s.d_alloced = false;
                --numSegments;
                if (s.d_len) {
                    --numBlocksInUse;
                }
            }

            ASSERT((int) ta.numBlocksInUse() == numBlocksInUse);
        }

        ta.release();
      }  break;
      case 16: {
        //---------------------------------------------------------------------
        // DEMANGLING
        //
        // Concerns:
        //   That demangling is appropriately controlled by the flag (this
        //   depends on the platform).
        //
        // Plan:
        // The character ':' does not occur in mangled names, but it will occur
        // in demangled names.  Since the method
        // 'BloombergLP::baesu_StackTraceTestAllocator::allocator' will always
        // be in a stack trace, we can verify that demangling was successful by
        // searching the stack trace for
        // 'BloombergLP::baesu_StackTraceTestAllocator::'.  (Note that the
        // routine name 'allocator' gets distorted to '.allocator' on AIX).
        //
        //: 1 Iterate through both values of the boolean variable
        //:   'demangleExpected'.  However, since demangling ALWAYS happens on
        //:   Windows and NEVER on Solaris CC, the variable must be overridden
        //:   on those platforms.
        //: 2 Create an STTA, specifying 'demangleExpected' to the c'tor and
        //:   with a stringstream specified to the 'ostream' argument.
        //: 3 Allocate a segment
        //: 4 Call 'reportBlocksInUse'
        //: 5 Release memory and destroy the STTA.
        //: 6 Observe that whether the substring
        //:   'BloombergLP::baesu_StackTraceTestAllocator' is found is
        //:   consistent with 'demangleExpected'.
        //---------------------------------------------------------------------

        if (verbose) cout << "Demangling Test\n"
                             "===============\n";

        if (!CAN_FIND_SYMBOLS) {
            break;          // We can't do this test unless we can find symbols
        }

        expectedDefaultAllocations = -1;    // turn off default alloc checking

        for (int d = 0; d < 2; ++d) {
            const bool DEMANGLE_CONFIG = d;
            bool demangleExpected = DEMANGLE_CONFIG;

#if defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(BSLS_PLATFORM__CMP_GNU)
            // never on Solaris CC

            demangleExpected = false;
#endif
#if defined(BSLS_PLATFORM__OS_WINDOWS)
            // always on Windows

            demangleExpected = true;
#endif

            bsl::stringstream ss;

            {
                Obj ta("alpha", &ss, 8, DEMANGLE_CONFIG);

                (void) ta.allocate(100);

                ta.reportBlocksInUse();

                ta.release();
            }

            const bool FOUND = npos != ss.str().find(
                               "BloombergLP::baesu_StackTraceTestAllocator::");
            LOOP3_ASSERT(ss.str(), demangleExpected, FOUND,
                                                    FOUND == demangleExpected);

            if (veryVerbose) {
                P_(DEMANGLE_CONFIG);    P_(demangleExpected);    P(ss.str());
            }
        }
      }  break;
      case 15: {
        //---------------------------------------------------------------------
        // UNDERRUN DETECTION
        //
        // Concern:
        //: That the allocator will detect certain types of underruns.  Two
        //: specific types are tested:
        //: 1 Continuous underruns, writing continuously from the first byte
        //:   before the segment start to up to 4 pointer sizes before the
        //:   first byte start.
        //: 2 Single-byte wild writes, at any byte up to one pointer size
        //:   before the start of the segment.
        //
        // Plan:
        //   Underruns are detected via a magic number that is flush against
        //   the client's area of the segment.  All of the underruns detected
        //   are detected by this magic number being perterbed.  The data
        //   before this magic number consists of pointers, so we cannot
        //   guarantee that corruption of these pointers will be detectable.
        //  
        //   The most likely garbage bytes to occur, therefore the most likely
        //   bytes to be accidentally written in an underrun, are '0', '1', and
        //   '0xff'.  Therefore all continuous underruns and wild writes will
        //   be repeatedly tested for those 3 values, verifying that they do
        //   not naturally occur anywhere in the magic numbers.
        //
        //: 1 Allocate a segment
        //: 2 Record the number of blocks in use from the STTA with the
        //:   'numBlocksInUse' method.
        //: 3 Write either a contiguous area before the start of the segment,
        //:   or a wild write to a single byte, after first saving the value of
        //:   the area that will be over written.
        //: 4 Repeat each test with the two possible boolean values of the
        //:   ABORT variable, in one case with 'deallocate' calling a failure
        //:   handler that does a longjmp, in the other the failure handler is
        //:   'Noop' and the deallocate returns without freeing anything.
        //: 5 After the 'deallocate' call, verify that either a 'longjmp'
        //:   occurred or the 'deallocate' returned, as expected, depending on
        //:   the value of the 'ABORT' variable.
        //: 6 Verify that a report was written mentioning the corruption.
        //: 7 Verify that no memory was freed by the 'deallocate' call using
        //:   the 'numBlocksInUse' accessor.
        //: 8 Restore the state of the memory that was corrupted to its
        //:   pre-corrupted state.
        //: 9 Deallocate the segment, which should be successful.
        //---------------------------------------------------------------------

        if (verbose) cout << "UNDERRUN DETECTION\n"
                             "==================\n";

        expectedDefaultAllocations = -1;    // turn off default alloc checking

        bslma::TestAllocator sta;
        bsl::stringstream ss(&sta);

        // The most likely garbage chars are 0, 1, and 0xff, so it is important
        // our magic numbers are chosen so as to detect stray occurrances of
        // those chars.

        const unsigned char fillChars[] = { 0, 1, 0xff };
        const unsigned char *end = fillChars + sizeof(fillChars);
        baesu_StackTraceTestAllocator ta("alpha", &ss);

        if (verbose) Q(Coninuous Underruns);
        {
            for (int a = 0; a < ABORT_LIMIT; ++a) {
                const bool ABORT = a;

                for (unsigned u = 1; u <= 4 * sizeof(void *); ++u) {
                    for (const unsigned char *pu = fillChars; pu < end; ++pu) {
                        if (veryVerbose) {
                            cout << "Continuous:   ";
                            P_(ABORT);   P_(u);    P((int) *pu);
                        }

                        ASSERT(ss.str().empty());

                        unsigned char *ptr =(unsigned char *) ta.allocate(100);

                        const unsigned numBlocks = (unsigned)
                                                           ta.numBlocksInUse();

                        char saveBuffer[4 * sizeof(void *)];
                        memcpy(saveBuffer, ptr - u, u);
                        memset(ptr - u, *pu, u);

                        if (setjmp(my_setJmpBuf)) {
                            ASSERT(ABORT);
                        }
                        else {
                            ta.setFailureHandler(ABORT
                                                 ? &my_failureHandlerLongJmp
                                                 : &my_failureHandlerSetFlag);
                            my_failureHandlerFlag = false;

                            ta.deallocate(ptr);

                            ASSERT(!ABORT);
                            ASSERT(my_failureHandlerFlag);
                        }

                        ta.setFailureHandler(&Obj::failureHandlerAbort);

                        LOOP_ASSERT(ss.str(), npos != ss.str().find(
                                              "Error: corrupted segment at "));
                        LOOP_ASSERT(ss.str(), npos != ss.str().find(
                               " attempted to be freed by allocator 'alpha'"));

                        ASSERT(ta.numBlocksInUse() == numBlocks);

                        ss.str("");

                        memcpy(ptr - u, saveBuffer, u);
                        ta.deallocate(ptr);

                        ASSERT(ss.str().empty());

                        ASSERT(0 == ta.numBlocksInUse());
                    }
                }
            }
        }

        if (verbose) Q(Single-byte Underruns);
        {
            for (int a = 0; a < ABORT_LIMIT; ++a) {
                const bool ABORT = a;

                for (int i = 1; i <= (int) sizeof(void *); ++i) {
                    for (const unsigned char *pu = fillChars; pu < end; ++pu) {
                        if (veryVerbose) {
                            cout << "Single-byte:   ";
                            P_(ABORT);   P_(i);    P((int) *pu);
                        }

                        ASSERT(ss.str().empty());

                        unsigned char *ptr =(unsigned char *) ta.allocate(100);

                        const int numBlocks = (int) ta.numBlocksInUse();

                        unsigned char saveChar = ptr[-i];
                        ptr[-i] = *pu;

                        if (setjmp(my_setJmpBuf)) {
                            ASSERT(ABORT);
                        }
                        else {
                            ta.setFailureHandler(ABORT
                                                 ? &my_failureHandlerLongJmp
                                                 : &my_failureHandlerSetFlag);

                            my_failureHandlerFlag = false;

                            ta.deallocate(ptr);

                            ASSERT(!ABORT);
                            ASSERT(my_failureHandlerFlag);
                        }

                        ta.setFailureHandler(&Obj::failureHandlerAbort);

                        LOOP_ASSERT(ss.str(), npos != ss.str().find(
                                              "Error: corrupted segment at "));
                        LOOP_ASSERT(ss.str(), npos != ss.str().find(
                               " attempted to be freed by allocator 'alpha'"));

                        ASSERT((int) ta.numBlocksInUse() == numBlocks);

                        ss.str("");

                        ptr[-i] = saveChar;
                        ta.deallocate(ptr);

                        ASSERT(ss.str().empty());

                        ASSERT(0 == ta.numBlocksInUse());
                    }
                }
            }
        }
      }  break;
      case 14: {
        //---------------------------------------------------------------------
        // DEALLOCATION ERROR TEST
        //
        // Concern:
        //: That the stack trace test allocator properly detects certain
        //: classes of errors at deallocation:
        //: 1 Deallocating same segment twice.
        //: 2 Freeing a STTA allocated segment by another STTA
        //: 3 Freeing a 'malloc' allocated segment by an STTA
        //: 4 Freeing a 'new[]' allocated segment by an STTA
        //: 5 Freeing a 'new' allocated segment by an STTA
        //: 6 Freeing a 'bslma::TestAllocator' allocated segment by an STTA
        //: 7 Freeing a 'bcema_TestAllocator' allocated segment by an STTA
        //: 8 Freeing a misaligned semgent
        //
        // Plan:
        //: 1 Iterate through both values of the boolean ABORT
        //:   o For each of the categories 1-7 in 'Concerns':
        //:     1 Install a 'setjmp' handler
        //:     2 Install a failure handler in the STTA
        //:       o If 'ABORT' is set, install a 'Longjmp' handler
        //:       o If 'ABORT' is not set, install a "Noop' handler
        //:     3 Allocate a segment of memory, the source depending on which
        //:       of test cases 1-7 we're in.
        //:     4 Note the number of blocks allocated by the STTA
        //:     5 Attempt to 'deallocate' the segment with the STTA
        //:     6 Verify that either a longjmp occurred or the deallocate
        //:       returned, depending on the value of 'ABORT'
        //:     7 Verify that no memory was freed using 'numBlocksInUse'
        //:     8 Verify that the expected report of the problem (1-7) under
        //:       'Concerns' above was written to the stream passed to the
        //:       STTA at its construction.
        //:     9 Wipe the string stream clean and properly deallocate the
        //:       segment allocated in step 1.
        //:   o Category 8 in 'Concerns" -- misaligned segments.
        //:     1 Allocate a segment of memory from the STTA, store it in a
        //:       pointer 'cPtr' of type 'char *'
        //:     2 Iterate the variable 'offset' from
        //:       '[ 1 .. sizeof(void *) - 1 ]'
        //:       o Install a setjmp handling block
        //:       o Install a failure handler in the STTA
        //:         1 If 'ABORT' is set, install a 'Longjmp' handler
        //:         2 If 'ABORT' is not set, install a "Noop' handler
        //:       o Note the number of blocks allocated by the STTA
        //:       o Attempt to deallocate 'cPtr + offset', which will be
        //:         misaligned, with the STTA
        //:       o Verify that either a longjmp occurred or the deallocate
        //:         returned, depending on the value of 'ABORT'
        //:       o Verify that no memory was freed, using 'numBlocksInUse'
        //:       o Verify that the expected report of the problem (1-7) under
        //:         'Concerns' above was written to the stream passed to the
        //:         STTA at its construction.
        //:       o Wipe the string stream clean
        //:     3 Properly deallocate the segment allocated in step 1.
        //---------------------------------------------------------------------

        expectedDefaultAllocations = -1;    // turn off default alloc checking

        bsl::stringstream oss;
        Obj ta("alpha", &oss);

        bsl::stringstream oss2;
        Obj ta2(&oss2);

        for (int i = 0; i < ABORT_LIMIT; ++i) {
            const bool ABORT = i;

            ASSERT(oss.str().empty());

            if (verbose) Q(Check deallocating same segment twice);
            {
                // We use a special underlying allocator in this place, because
                // if the underlying OS writes over freed memory, we cannot
                // detect redundant frees as such.  We use a
                // 'bslma::BufferAllocator' because it won't actually write
                // over a freed segment, nor will it turn it over to the
                // underlying OS which may do uncontrollable things with it.

                char buffer[4 * 1000];
                bslma::BufferAllocator ba(buffer, sizeof(buffer));
                Obj tba("beta", &oss, 8, true, &ba);

                unsigned tbaBlocks;
                if (setjmp(my_setJmpBuf)) {
                    if (veryVerbose) Q(Abort: deallocating same segment twice);

                    ASSERT(ABORT);
                }
                else {
                    tba.setFailureHandler(ABORT ? &my_failureHandlerLongJmp
                                                : &my_failureHandlerSetFlag);

                    void *ptr = tba.allocate(6);

                    ASSERT(oss.str().empty());

                    tba.deallocate(ptr);

                    tbaBlocks = (unsigned) tba.numBlocksInUse();
                    my_failureHandlerFlag = false;

                    tba.deallocate(ptr);

                    if (veryVerbose) Q(NoAbort: dealloc same segment twice);

                    ASSERT(!ABORT);
                    ASSERT(my_failureHandlerFlag);
                }

                tba.setFailureHandler(Obj::failureHandlerAbort);
                memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                // Make sure nothing was freed before the failure handler
                // was called.

                ASSERT(tbaBlocks  == tba.numBlocksInUse());

                // Make sure a report was written.

                ASSERT(!oss.str().empty());

                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                                     "freed second time by allocator 'beta'"));
                oss.str("");
            }

            if (verbose) Q(Check freeing by wrong allocator of right type)
            {
                void *ptr;
                unsigned taBlocks;
                if (setjmp(my_setJmpBuf)) {
                    if (veryVerbose) Q(Abort: deallocating by wrong alloc);

                    ASSERT(ABORT);
                }
                else {
                    ta.setFailureHandler(ABORT ? &my_failureHandlerLongJmp
                                               : &my_failureHandlerSetFlag);

                    ASSERT(oss. str().empty());
                    ASSERT(oss2.str().empty());

                    ptr = ta2.allocate(100);

                    taBlocks = (unsigned) ta.numBlocksInUse();

                    my_failureHandlerFlag = false;

                    ta.deallocate(ptr);

                    if (veryVerbose) Q(NoAbort: deallocating by wrong alloc);

                    ASSERT(!ABORT);
                    ASSERT(my_failureHandlerFlag);
                }

                ta.setFailureHandler(Obj::failureHandlerAbort);
                memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                // Make sure nothing was freed before the failure handler
                // was called.

                ASSERT(taBlocks  == ta.numBlocksInUse());

                // Make sure a report was written.

                ASSERT(!oss.str().empty());

                ASSERT(oss2.str().empty());
                LOOP_ASSERT(oss.str(),
                                npos != oss.str().find("Error: attempt to free"
                        " segment by wrong allocator.\n    Segment belongs to"
                        " allocator '<unnamed>'\n    Attempted to free by"
                        " allocator 'alpha'"));
                oss.str("");
                ta2.deallocate(ptr);
            }

            if (verbose) Q(Check freeing of segment allocated with malloc);
            {
                void *ptr;

                unsigned taBlocks;
                if (setjmp(my_setJmpBuf)) {
                    if (veryVerbose) Q(Abort: freeing malloced);

                    ASSERT(ABORT);
                }
                else {
                    ta.setFailureHandler(ABORT ? &my_failureHandlerLongJmp
                                               : &my_failureHandlerSetFlag);

                    ASSERT(oss.str().empty());

                    ptr = malloc(100);

                    taBlocks = (unsigned) ta.numBlocksInUse();
                    my_failureHandlerFlag = false;

                    ta.deallocate(ptr);

                    if (veryVerbose) Q(NoAbort: freeing malloced);

                    ASSERT(!ABORT);
                    ASSERT(my_failureHandlerFlag);
                }

                ta.setFailureHandler(Obj::failureHandlerAbort);
                memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                ASSERT(taBlocks  == ta.numBlocksInUse());

                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                                              "Error: corrupted segment at "));
                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                               " attempted to be freed by allocator 'alpha'"));

                oss.str("");
                free(ptr);
            }

            if (verbose) Q(Check freeing of array alloced with new[]);
            {
                char *ptr;

                unsigned taBlocks;
                if (setjmp(my_setJmpBuf)) {
                    if (veryVerbose) Q(Abort: freeing newed);

                    ASSERT(ABORT);
                }
                else {
                    ta.setFailureHandler(ABORT ? &my_failureHandlerLongJmp
                                               : &my_failureHandlerSetFlag);

                    ASSERT(oss.str().empty());

                    ptr = new char[100];

                    taBlocks = (unsigned) ta.numBlocksInUse();
                    my_failureHandlerFlag = false;

                    ta.deallocate(ptr);

                    if (veryVerbose) Q(NoAbort: freeing newed);

                    ASSERT(!ABORT);
                    ASSERT(my_failureHandlerFlag);
                }

                ta.setFailureHandler(Obj::failureHandlerAbort);
                memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                                              "Error: corrupted segment at "));
                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                               " attempted to be freed by allocator 'alpha'"));

                ASSERT(taBlocks  == ta.numBlocksInUse());

                oss.str("");
                delete[] ptr;
            }

            if (verbose) Q(Check freeing of array alloced with new);
            {
                int *ptr;

                unsigned taBlocks;
                if (setjmp(my_setJmpBuf)) {
                    if (veryVerbose) Q(Abort: freeing newed);

                    ASSERT(ABORT);
                }
                else {
                    ta.setFailureHandler(ABORT ? &my_failureHandlerLongJmp
                                               : &my_failureHandlerSetFlag);

                    ASSERT(oss.str().empty());

                    ptr = new int;

                    taBlocks = (unsigned) ta.numBlocksInUse();
                    my_failureHandlerFlag = false;

                    ta.deallocate(ptr);

                    if (veryVerbose) Q(NoAbort: freeing newed);

                    ASSERT(!ABORT);
                    ASSERT(my_failureHandlerFlag);
                }

                ta.setFailureHandler(Obj::failureHandlerAbort);
                memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                                              "Error: corrupted segment at "));
                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                               " attempted to be freed by allocator 'alpha'"));

                oss.str("");
                delete ptr;
            }

            if (verbose) Q(Check freeing of seg alloced with bslma::TA);
            {
                bslma::TestAllocator taBsl;
                void *ptr;
                unsigned numBlocks;

                if (setjmp(my_setJmpBuf)) {
                    if (veryVerbose) Q(Abort: freeing bslma TAed);

                    ASSERT(ABORT);
                }
                else {
                    ta.setFailureHandler(ABORT ? &my_failureHandlerLongJmp
                                               : &my_failureHandlerSetFlag);

                    ASSERT(oss.str().empty());

                    ptr = taBsl.allocate(100);

                    numBlocks = (unsigned) ta.numBlocksInUse();
                    my_failureHandlerFlag = false;

                    ta.deallocate(ptr);

                    if (veryVerbose) Q(NoAbort: freeing bslma TAed);

                    ASSERT(!ABORT);
                    ASSERT(my_failureHandlerFlag);
                }

                ta.setFailureHandler(Obj::failureHandlerAbort);
                memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                ASSERT(ta.numBlocksInUse() == numBlocks);

                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                                              "Error: corrupted segment at "));
                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                               " attempted to be freed by allocator 'alpha'"));

                oss.str("");
                taBsl.deallocate(ptr);
            }

            if (verbose) Q(Check freeing of seg alloced with bcema_TA);
            {
                bcema_TestAllocator taBce;
                void *ptr;
                unsigned numBlocks;

                if (setjmp(my_setJmpBuf)) {
                    if (veryVerbose) Q(Abort: freeing bcema TAed);

                    ASSERT(ABORT);
                }
                else {
                    ta.setFailureHandler(ABORT ? &my_failureHandlerLongJmp
                                               : &my_failureHandlerSetFlag);

                    ASSERT(oss.str().empty());

                    ptr = taBce.allocate(100);

                    numBlocks = (unsigned) ta.numBlocksInUse();
                    my_failureHandlerFlag = false;

                    ta.deallocate(ptr);

                    if (veryVerbose) Q(NoAbort: freeing bcema TAed);

                    ASSERT(!ABORT);
                    ASSERT(my_failureHandlerFlag);
                }

                ta.setFailureHandler(Obj::failureHandlerAbort);
                memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                ASSERT(ta.numBlocksInUse() == numBlocks);

                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                                              "Error: corrupted segment at "));
                LOOP_ASSERT(oss.str(), npos != oss.str().find(
                               " attempted to be freed by allocator 'alpha'"));

                oss.str("");
                taBce.deallocate(ptr);
            }

            if (verbose) Q(Check freeing of misaligned segment);
            {
                char *cPtr = (char *) ta.allocate(100);
                unsigned numBlocks;

                for (unsigned offset = 1; offset < sizeof(void *); ++offset) {
                    ASSERT(oss.str().empty());

                    if (setjmp(my_setJmpBuf)) {
                        if (veryVerbose) Q(Abort: freeing misaligned TAed);

                        ASSERT(ABORT);
                    }
                    else {
                        ta.setFailureHandler(ABORT ?&my_failureHandlerLongJmp
                                                   :&my_failureHandlerSetFlag);

                        ASSERT(oss.str().empty());

                        numBlocks = (unsigned) ta.numBlocksInUse();
                        my_failureHandlerFlag = false;

                        ta.deallocate(cPtr + offset);

                        if (veryVerbose) Q(NoAbort: freeing misaligned TAed);

                        ASSERT(!ABORT);
                        ASSERT(my_failureHandlerFlag);
                    }

                    ta.setFailureHandler(Obj::failureHandlerAbort);
                    memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                    ASSERT(ta.numBlocksInUse() == numBlocks);

                    LOOP_ASSERT(oss.str(), npos != oss.str().find(
                            "Badly aligned segment passed to allocator"
                            " 'alpha' must have been allocated by another"
                            " type of allocator\n"));

                    oss.str("");
                }

                ta.deallocate(cPtr);
            }
        }
      }  break;
      case 13: {
        //---------------------------------------------------------------------
        // RELEASE AND DESTRUCTOR TEST
        //
        // Concern:
        //: o That the 'release' function properly frees all outstanding
        //:   segments.
        //: o If the destructor is called with memory outstanding, and the
        //:   failure handler returns without aborting or throwing, the d'tor
        //:   reports and frees all outstanding memory.
        //: o That the destructor reports, but does not free memory, before
        //:   calling the failure handler.
        //
        // Plan:
        //: 1 Release test.  Loop for 'numAllocs' from 0 to 100:
        //:   o Create a 'bslma::TestAllocator', and a stack trace test
        //:     allocator based on that 'bslma::TestAllocator'.
        //:   o Allocate 'numAllocs' segments.
        //:   o Verify the number of outstanding memory segments is
        //:     'numAllocs'.
        //:   o Call 'release'.
        //:   o Verify, with the 'numBlocksInUse' allocator, that the memory
        //:     has been freed.
        //:   o Destroy the stack trace test allocator and verify that no
        //:     report is written.
        //:   o Destroy the 'bslma::TestAllocator' to further verify that all
        //:     memory was freed.
        //: 2 D'tor with 'Noop' failure handler: Loop for 'numAllocs' from 0 to
        //:   100:
        //:   o Create a 'bslma::TestAllocator', and a stack trace test
        //:     allocator based on that 'bslma::TestAllocator'.
        //:   o Allocate 'numAllocs' segments.
        //:   o Verify the number of outstanding memory segments is
        //:     'numAllocs'.
        //:   o Install the 'Noop' failure assert handler.
        //:   o Destroy the stack trace test allocator.
        //:   o Verify that a report was written if 'numAllocs > 0'.
        //:   o Verify with the 'numBlocksInUse' accessor of the
        //:     'bslma::Allocator' that all memory has been freed.
        //: 3 D'tor with 'longjmp' failure handler: Loop for 'numAllocs' from
        //:   one to 100:
        //:   o Create a 'bslma::TestAllocator', and a stack trace test
        //:     allocator based on that 'bslma::TestAllocator'.
        //:   o Allocate 'numAllocs' segments.
        //:   o Verify the number of outstanding memory segments is
        //:     'numAllocs'.
        //:   o Install the 'longjmp' failure assert handler.
        //:   o Attempt to destroy the stack trace test allocator.
        //:   o Verify that the d'tor did a longjmp.
        //:   o Verify that a report was written
        //:   o Verify with the 'numBlocksInUse' accessor of both allocators
        //:     that *NO* memory has been freed.
        //:   o 'release' the remaining memory and destroy the stack trace
        //:     test allocator.
        //---------------------------------------------------------------------

        if (verbose) cout << "Release Test\n"
                             "============\n";

        expectedDefaultAllocations = -1;

        if (verbose) Q(Verify release works on varying number of segments);
        {
            bsl::stringstream ss;

            for (int numAllocs = 0; numAllocs <= 100; ++numAllocs) {
                bslma::TestAllocator sta("sta");
                baesu_StackTraceTestAllocator *pta =
                                 new (sta) baesu_StackTraceTestAllocator("ta",
                                                                         &ss,
                                                                         8,
                                                                         true,
                                                                         &sta);

                for (int i = 0; i < numAllocs; ++i) {
                    (void) pta->allocate(100);
                }
                ASSERT(numAllocs == (int) pta->numBlocksInUse());
                pta->release();
                ASSERT(0 == pta->numBlocksInUse());

                sta.deleteObject(pta);

                ASSERT(ss.str().empty());

                // Destruction of 'sta' confirms that all memory was freed.
            }

            ASSERT(ss.str().empty());
        }


        if (verbose) Q(Destroy with varying number of segments allocated);
        {
            bsl::stringstream ss;

            for (int numAllocs = 0; numAllocs <= 100; ++numAllocs) {
                bslma_TestAllocator sta("sta");
                baesu_StackTraceTestAllocator *pta =
                                 new (sta) baesu_StackTraceTestAllocator("ta",
                                                                         &ss,
                                                                         8,
                                                                         true,
                                                                         &sta);

                for (int i = 0; i < numAllocs; ++i) {
                    (void) pta->allocate(100);
                }
                ASSERT(numAllocs == (int) pta->numBlocksInUse());

                ASSERT(ss.str().empty());
                pta->setFailureHandler(&Obj::failureHandlerNoop);
                sta.deleteObject(pta);

                // Verify report was written, if expected

                if (0 != numAllocs) {
                    ASSERT(!ss.str().empty());
                    if (CAN_FIND_SYMBOLS) {
                        ASSERT(npos != ss.str().find("main"));
                    }
                }
                else {
                    ASSERT(ss.str().empty());
                }

                // Verify memory was freed.

                LOOP2_ASSERT(numAllocs, sta.numBlocksInUse(),
                                                    0 == sta.numBlocksInUse());

                ss.str("");

                // Successful destruction of 'sta' proves all memory was freed.
            }
        }

#ifndef BSLS_PLATFORM__OS_WINDOWS
        if (verbose) Q(Longjmp on destruction with segments outstanding);
        {
            bsl::stringstream ss;

            for (int numAllocs = 1; numAllocs <= 100; ++numAllocs) {
                bslma_TestAllocator sta("sta");
                baesu_StackTraceTestAllocator *pta =
                                 new (sta) baesu_StackTraceTestAllocator("ta",
                                                                         &ss,
                                                                         8,
                                                                         true,
                                                                         &sta);

                for (int i = 0; i < numAllocs; ++i) {
                    (void) pta->allocate(100);
                }
                ASSERT(numAllocs == (int) pta->numBlocksInUse());

                unsigned staBlocks = (unsigned) sta.numBlocksInUse();

                ASSERT(ss.str().empty());
                if (setjmp(my_setJmpBuf)) {
                    pta->setFailureHandler(Obj::failureHandlerAbort);
                    memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                    // Make sure a report was written.

                    ASSERT(!ss.str().empty());
                    if (CAN_FIND_SYMBOLS) {
                        ASSERT(npos != ss.str().find("main"));
                    }

                    // Make sure nothing was freed before the failure handler
                    // was called.

                    ASSERT(staBlocks == sta.numBlocksInUse());
                    ASSERT(pta->numBlocksInUse() == (unsigned) numAllocs);
                }
                else {
                    pta->setFailureHandler(&my_failureHandlerLongJmp);

                    sta.deleteObject(pta);    // Will long jmp

                    ASSERT(0);
                }

                ss.str("");
                pta->release();
                sta.deleteObject(pta);

                ASSERT(ss.str().empty());
            }
        }
#endif
      }  break;
      case 12: {
        //---------------------------------------------------------------------
        // MULTITHREADED TEST
        //
        // Concern:
        //   That the allocator doesn't hang or malfunction if used in a
        //   multithreaded context, and that it is safe to use a thread-unsafe
        //   allocator as the underlying allocator, even if the STTA itself
        //   is used in a multithreaded context.
        //
        // Plan:
        //: 1 Create a type, 'TouchyAllocator', derived from
        //:   'bslma::Allocator', which will hold a pointer to a
        //:   bslma::Allocator and provide 'allocate' and 'deallocate' methods
        //:   which will propagate to the 'allocate' and 'deallocate' methods
        //:   of the held allocator, only before and after the called methods
        //:   they will assert to make sure that there is no simultaneous
        //:   calling of 'allocate' or 'deallocate' by multiple threads.
        //: 2 Create a stack trace test allocator with a 'TouchyAllocator'
        //:   passed as the underlying allocator.
        //: 3 Create a functor object.  The function 'Functor::operator()' will
        //:   nest several routine calls deep, then frequently allocate and
        //:   deallocate segments in random order from an allocator passed at
        //:   construction.
        //: 4 Start many threads, all calling 'Functor::operator()', all
        //:   sharing a stack trace test allocator whose underlying allocator
        //:   is a 'TouchyAllocator'.
        //: 5 Set up barriers before the first few allocations by the threads
        //:   to encourage the threads to all try to allocate at the same time,
        //:   maximizing contention.
        //: 6 The subthreads will all reach a barrier after they are done
        //:   allocating and freeing.
        //: 7 After the threads have done a lot of allocating and freeing, have
        //:   them pause with some memory outstanding.  While they are paused,
        //:   generate a report from the main thread of the stack trace from
        //:   the outstanding segments (which should all be allocated from the
        //:   same point in the code) and verify that the report is as
        //:   expected.
        //: 8 Allow the paused threads to continue and join them.
        //: 9 Verify that the expected amount of memory is still outstanding,
        //:   'release' it, destroy the stack trace test allocator, and verify
        //:   that no report was written upon destruction.
        //---------------------------------------------------------------------

        if (verbose) cout << "Multithreaded Test\n"
                             "==================\n";

        namespace TC = MultiThreadedTest;

        typedef bcemt_ThreadUtil Util;

        bslma_TestAllocator bslta("bsl_ta");
        TC::TouchyAllocator touchy(&bslta);

        TC::Functor::initFuncPtrs();

        bsl::stringstream ss(&touchy);

        baesu_StackTraceTestAllocator *pta =
                           new (touchy) baesu_StackTraceTestAllocator("ta",
                                                                      &ss,
                                                                      8,
                                                                      true,
                                                                      &touchy);

        Util::Handle handles[TC::Functor::NUM_THREADS];
        int rc = 0;
        for (int i = 0; 0 == rc && i < TC::Functor::NUM_THREADS; ++i) {
            rc = Util::create(&handles[i], TC::Functor(pta));
            ASSERT(0 == rc);

            static
            bool isInplace = bdef_FunctionUtil::IsInplace<TC::Functor>::VALUE;
            expectedDefaultAllocations += 0 == isInplace;

            static bool firstTime = true;
            if (verbose && firstTime) {
                P(isInplace);
            }
            firstTime = false;
        }

        TC::Functor::s_underwayBarrier.wait();    // wait for everybody to get
                                                  // started

        bsl::stringstream otherSs(&touchy);
        pta->reportBlocksInUse(&otherSs);
        const bsl::string& otherStr = otherSs.str();
        ++expectedDefaultAllocations;                  // otherSs.str() uses da

        const char *expectedStrings[] = {
                                    " segment(s) in allocator 'ta' in use",
                                    " trace(s)",
                                    "StackTraceTestAllocator",
                                    "allocate",
                                    "MultiThreadedTest",
                                    "Functor",
                                    "allocOne",
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
        bsl::size_t pos = 0;
        int expectedToFind = CAN_FIND_SYMBOLS ? NUM_EXPECTED_STRINGS : 2;
        for (int i = 0; i < expectedToFind; ++i) {
            bsl::size_t nextPos = otherStr.find(expectedStrings[i], pos);
            LOOP3_ASSERT(otherStr, i, expectedStrings[i], npos != nextPos);
            pos = npos != nextPos ? nextPos : pos;
        }
        if (veryVerbose) {
            cout << "Report of blocks in use\n" << otherStr;
        }

        for (int i = 0; i < TC::Functor::NUM_THREADS; ++i) {
            Util::join(handles[i]);
        }

        // Threads should each have at least 4 segments allocated left over
        // after they finish.

        LOOP_ASSERT(pta->numBlocksInUse(),
                        pta->numBlocksInUse() >= TC::Functor::NUM_THREADS * 4);

        pta->release();

        touchy.deleteObject(pta);

        ASSERT(ss.str().empty());     // nothing written to 'ss' by d'tor of ta
      }  break;
      case 11: {
        //---------------------------------------------------------------------
        // SUCCESSFUL FREEING TEST
        //
        // Concern: If segments are allocated and deallocated, that the test
        //   allocator issues no complaints and leaks no memory of its own.
        //
        // Plan:
        //: In 2 blocks, for named and unnamed allocators:
        //: 1 Create a 'stringstream' 'ss'.
        //: 2 Create an allocator 'ta', named or unnamed, depending on which
        //:   block we're in, with output redirected to 'ss'.
        //: 3 Allocate 100 segments.
        //: 4 Create a 'stringstream' 'otherSs' and write a report to it with
        //:   'reportBlocksInUse', verify it reports the outstanding memory.
        //: 5 Free the 100 segments.
        //: 6 Destroy 'ta'.
        //: 7 Verify that nothing was written to 'ss' since no memory was
        //:   outstanding when 'ta' was destroyed.
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
        //: 1 Create a 'stringstream' 'taOss'.
        //: 2 Create a stack trace test allocator 'ta' with its output
        //:   redirected to 'taOss'.
        //: 3 Manipulate 'ta's failure handler with 'setFailureHandler' and
        //:   observe its value with the 'failurehandler' accessor.
        //: 4 Declare a second 'stringstream' 'oss'.
        //: 5 Call the 'reportBlocksInUse' accessor with no memory allocated.
        //:   o The first time, call it with output redirected to 'oss'.
        //:     Nothing should be written to 'oss'.  Verify this.
        //:   o The second time, call the 'reportBlocksInUse' accessor with
        //:     output defaulting to 'taOss'.  Nothing should be written to
        //:     'taOss'.  Verify this.
        //: 6 Loop 100 times
        //:   o Allocate segments of random lengths, keeping pointers to up to
        //:     4 segments.
        //:   o Freeing one of the 4 segments in random order
        //:   o After every allocate or free, verify that the 'numBlocksInUse'
        //:     accessor accurately tracks the number of unfreed segments in
        //:     existance.
        //: 7 After the loop, 3 segments should remain allocated.  Verify this.
        //: 8 Call 'reportBlocksInUse' twice.
        //:   o The first time, call it with output redirected to 'oss'.
        //:   o The second time, call it with output defaulting to 'taOss.
        //:   o Examine 'oss.str()' to see it reports the 3 outstanding
        //:     segments properly, and the name of the allocator.
        //:   o Verify 'oss.str() == taOss.str()'.
        //: 9 Free the remaining segments.
        //:10 Call 'reportBlocksInUse' twice again, with output going to 'oss'
        //:   and 'taOss', and verify that this time no output is written.
        //:11 Destroy 'ta' and verify that nothing is written to 'taOss'.
        //---------------------------------------------------------------------

        expectedDefaultAllocations = -1;    // turn off default allocator
                                            // monitoring
        bsl::ostringstream taOss;
        {
            Obj ta("my_allocator", &taOss);

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
            ASSERT(oss.str().empty());      // no blocks in use
            ta.reportBlocksInUse();
            ASSERT(taOss.str().empty());    // no blocks in use

            void *segments[SEGMENT_ARRAY_LENGTH] = { 0 };

            int randNum;
            bdeu_Random::generate15(&randNum, 987654321);
            unsigned numSegments = 0;

            // do a lot of allocating and freeing, not just freeing the segment
            // most recently allocated, but rather choosing the segment to free
            // in a somewhat random fashion.

            for (int i = 0; i < 100; ++i) {
                unsigned allocIdx = bdeu_Random::generate15(&randNum) %
                                                          SEGMENT_ARRAY_LENGTH;
                while (segments[allocIdx]) {
                    allocIdx = (allocIdx + ALLOC_INC) % SEGMENT_ARRAY_LENGTH;
                }
                segments[allocIdx] = ta.allocate(
                       bsl::max<int>(1, bdeu_Random::generate15(&randNum) %
                                                            MAX_ALLOC_LENGTH));
                ++numSegments;
                LOOP3_ASSERT(i, ta.numBlocksInUse(), numSegments,
                                           ta.numBlocksInUse() == numSegments);

                if (numSegments >= 4) {
                    unsigned freeIdx = bdeu_Random::generate15(&randNum) %
                                                          SEGMENT_ARRAY_LENGTH;
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
            ASSERT(!CAN_FIND_SYMBOLS || npos != report.find("main"));

            ASSERT(taOss.str().empty());
            ta.reportBlocksInUse();
            const bsl::string& taReport = taOss.str();

            ASSERT(!taReport.empty());
            ASSERT(taReport == report);

            for (int i = 0; i < SEGMENT_ARRAY_LENGTH; ++i) {
                if (segments[i]) {
                    ta.deallocate(segments[i]);
                    --numSegments;
                }
                ASSERT(ta.numBlocksInUse() == numSegments);
            }

            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(0 == numSegments);

            oss.str("");
            taOss.str("");

            ta.reportBlocksInUse(&oss);
            ASSERT(oss.str().empty());
            ta.reportBlocksInUse();
            ASSERT(taOss.str().empty());
        }

        ASSERT(taOss.str().empty());    // nothing happened at destruction
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
        //: 1 Monitor use of the default alloctor throughout this example,
        //:   updating 'expectedDefaultAllocations' every time we deliberately
        //:   use it (only for getting the string value of a 'stringstream'
        //:   object), to verify that the object under test never uses the
        //:   default allocator.
        //: 2 Create a 'bslma::TestAllocator' object 'ota' for use by the
        //:   allocator and the stringstream we will sometimes route its output
        //:   to.
        //: 3 Using a loop-based approach, loop through all possible
        //:   combinations of two booleans:
        //:   o CLEAN_DESTROY: if true, no memory will be outstanding when the
        //:     stack trace test allocator object is destroyed, and no report
        //:     is expected.
        //:   o FAILURE_LONGJMP: if true, the stack trace test allocator
        //:     object's failure handler is set to a function that will call
        //:     'longjmp'.  If false, the handler is set to the 'Noop' handler,
        //:     which will just return without doing anything.
        //:   o Using a loop-based approach, create a stack trace test
        //:     allocator object 'ta' using each of of 10 possible combinations
        //:     of args to the stack trace test allocator constructor.
        //:     1 Use the 'setFailureHandler' manipulator to manipulate the
        //:       failure handler, and verify the value of the changed value
        //:       with the 'failureHandler' accessor.
        //:     2 Call the 'release' manipulator.  Since no memory has been
        //:       allocator using 'ta', it should make no difference to the
        //:       number of blocks allocated by 'ta' or 'ota', observe this
        //:       with the 'numBlocksInUse' accessors of 'ta' and 'ota'.
        //:     3 Allocate a segment of memory.  Observe the difference using
        //:       the 'numBlocksInUse' accessors of 'ta' and 'ota'.  Deallocate
        //:       the segment again and observe via the 'numBlocksInUse'
        //:       accessors that the memory usage by 'ta' and 'ota' returns to
        //:       what it previously was.
        //:     4 Allocate another segment, then call the 'release' manipulator
        //:       of 'ta' and observe that the memory usage of 'ta' and 'ota'
        //:       is restored to what it was before the memory was allocated.
        //:     5 Allocate 100 segments, then call the 'release' manipulator of
        //:       'ta' and observe that the memory usage of 'ta' and 'ota' is
        //:       restored to what it was before the memory was allocated.
        //:     6 Allocate one segment, and if the 'CLEAN_DESTROY' flag is set,
        //:       deallocate it again.
        //:     7 Call 'setjmp' to install a block as the handler if a
        //:       'longjmp' occurs.  The block is described beginning in
        //:       section 11.
        //:     8 If 'FAILURE_LONGJMP' set the failure handler to a handler
        //:       that will call 'longjmp', otherwise set it to a 'Noop'
        //:       handler that will do nothing.
        //:     9 Attempt to delete 'pta'.  If
        //:       '!CLEAN_DESTROY && FAILURE_LONGJMP', the destructor should
        //:       call 'longjmp' before any memory is freed.
        //:    10 If 'deleteObject' returns, assert
        //:       'CLEAN_DESTROY || !FAILURE_LONGJMP'.
        //:    11 In the longjmp handling block, first assert '!CLEAN_DESTROY'
        //:       and assert 'FAILURE_LONGJMP'.
        //:    12 Set the failure handler to 'abort'.
        //:    13 Verify that the memory allocated by 'ta' was not freed by the
        //:       destructor before the failure handler was called.
        //:    14 Depending on which constructor was used to create 'ta', and
        //:       whether any memory was outstanding, compute boolean
        //:       'OSS_REPORT' which is true if the destructor should have
        //:       written a report to stringstream 'oss' and 'false' otherwise.
        //:    15 Examine 'oss.str()' and verify that it meets our expectations
        //:       of the report generated by the destructor.
        //:    16 If the destructor called 'longjmp', release memory allocated
        //:       by the object and successfully destroy it.
        //: 4 Start a second block which will verify the capture of default
        //:   output to 'cout'
        //:   1 Define an assert macro 'ASSERT_STDERR' that will do its output
        //:     to 'cerr' since we will be capturing 'cout's output to a
        //:     stringstream.
        //:   2 Define a stringstream 'oss' and redirect 'cout's output to
        //:     'oss'.
        //:   3 In both branches of an if statement
        //:     o Create an object 'ta' with one of the two c'tors of 'Obj'
        //:       that don't take an argument for 'ostream'.
        //:     o Allocate a segment.
        //:     o Set the failure handler to 'Noop'.
        //:     o Destroy 'ta'.
        //:   4 Examine oss.str() to verify that it meets expectations, using
        //:     'ASSERT_STDERR'.
        //---------------------------------------------------------------------

        if (verbose) cout << "DEFAULT C'TOR, PRIMARY MANIPULATORS, D'TOR\n"
                             "==========================================\n";

        bslma::TestAllocator ota;
        bslma::TestAllocator ssTa;

        if (verbose) Q("Loop to exercise all c'tors");
        for (int i = 0; i < 2 * ABORT_LIMIT; ++i) {
            const bool CLEAN_DESTROY   = i %  2;
            const bool FAILURE_LONGJMP = i >= 2;

            for (char c = 'a'; c <= 'j' ; ++c) {
                if (veryVerbose) {
                    P_(CLEAN_DESTROY);    P_(FAILURE_LONGJMP);    P(c);
                }

                Obj *pta = 0;

                bsl::stringstream oss(&ssTa);

                // Note that 'Obj's that are constructed without an allocator
                // arg use the gmalloc allocator singleton, not 'da'

                bool otaPassed = false;
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
                    otaPassed = true;
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
                    otaPassed = true;
                  } break;
                }

                ASSERT(0 != pta);
                Obj& ta = *pta;

                ASSERT(ta.failureHandler() == &Obj::failureHandlerAbort);
                ta.setFailureHandler(&Obj::failureHandlerNoop);
                ASSERT(ta.failureHandler() == &Obj::failureHandlerNoop);
                ta.setFailureHandler(Obj::failureHandlerAbort);
                ASSERT(ta.failureHandler() == &Obj::failureHandlerAbort);

                // No blocks are allocated.  Verify 'release' has no effect.

                const int otaNumBlocks = (int) ota.numBlocksInUse();
                ASSERT(0 == ta.numBlocksInUse());
                ta.release();
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(otaNumBlocks == ota.numBlocksInUse());

                void *segment = ta.allocate(100);
                ASSERT(1 == ta.numBlocksInUse());
                ASSERT(!otaPassed || otaNumBlocks + 1 == ota.numBlocksInUse());
                ta.deallocate(segment);
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(otaNumBlocks == ota.numBlocksInUse());

                // Verify release works for 1 block allocated.

                segment = ta.allocate(50);
                ASSERT(1 == ta.numBlocksInUse());
                ta.release();
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(otaNumBlocks == ota.numBlocksInUse());

                // Verify release works for many blocks allocated

                for (int j = 0; j < 100; ++j) {
                    (void) ta.allocate(4 + j);
                }
                ASSERT(100 == ta.numBlocksInUse());
                LOOP2_ASSERT(otaNumBlocks, ota.numBlocksInUse(),
                     !otaPassed || otaNumBlocks + 100 <= ota.numBlocksInUse());
                ta.release();
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(otaNumBlocks == ota.numBlocksInUse());

                segment = ta.allocate(200);
                if (CLEAN_DESTROY) {
                    ta.deallocate(segment);
                }

                // If 'CLEAN_DESTROY' there will be no blocks in use and the
                // d'tor will not write a report.  Otherwise, 'segment' will
                // still be unfreed and a report will be written.

                ASSERT(! CLEAN_DESTROY == ta.numBlocksInUse());
                ASSERT(oss.str().empty());    // verify no output has been
                                              // written before d'tor called

                if (setjmp(my_setJmpBuf)) {
                    LOOP_ASSERT(c, !CLEAN_DESTROY && FAILURE_LONGJMP);
                    ta.setFailureHandler(Obj::failureHandlerAbort);

                    ASSERT(1 == ta.numBlocksInUse());
                }
                else {
                    if (FAILURE_LONGJMP) {
                        ta.setFailureHandler(&my_failureHandlerLongJmp);
                    }
                    else {
                        ta.setFailureHandler(&my_failureHandlerSetFlag);
                    }

                    my_failureHandlerFlag = false;
                    ota.deleteObject(pta);

                    LOOP2_ASSERT(i, c, CLEAN_DESTROY || !FAILURE_LONGJMP);
                    ASSERT(my_failureHandlerFlag == (!CLEAN_DESTROY &&
                                                            !FAILURE_LONGJMP));
                    LOOP_ASSERT(ota.numBlocksInUse(),
                                                    0 == ota.numBlocksInUse());
                }
                memset(my_setJmpBuf, 0, sizeof(my_setJmpBuf));

                const bool OSS_REPORT = 'a' != c && 'f' != c && !CLEAN_DESTROY;

                const bsl::string& report = oss.str();
                expectedDefaultAllocations += OSS_REPORT;

                ASSERT(report.empty() == !OSS_REPORT);
                if (OSS_REPORT) {
                    ASSERT(npos != report.find("1 segment(s) in use"));
                    ASSERT(npos != report.find("Error: memory leaked"));
                    ASSERT((c >= 'f') ==
                                        (npos != report.find("my_allocator")));
                    if (CAN_FIND_SYMBOLS) {
                        ASSERT(npos != report.find("main"));
                        ASSERT(npos != report.find("allocate"));
                    }
                }

                if (!CLEAN_DESTROY && FAILURE_LONGJMP) {
                    // Make sure we haven't released memory before calling
                    // the failure handler:

                    ta.release();
                    ota.deleteObject(pta);
                }
            }
        }

        if (verbose) Q("2: Prove if no ostream is given, output goes to cout");
        {
#           define ASSERT_STDERR(exp)                                         \
                if (!(exp)) {                                                 \
                    cerr << "Error " << __FILE__ << "(" << __LINE__ << "): "  \
                                       << #exp << "    (failed)" << endl;     \
                    if (0 <= testStatus && testStatus <= 100) ++testStatus;   \
                }

            bsl::stringstream oss(&ota);
            cout.rdbuf(oss.rdbuf());

            for (int i = 0; i < 2; ++i) {
                const bool NAME = i;

                oss.str("");
                ASSERT_STDERR(oss.str().empty());

                if (NAME) {
                    Obj ta("my_allocator");

                    ta.setFailureHandler(&Obj::failureHandlerNoop);
                    ta.allocate(100);
                }
                else {
                    Obj ta;

                    ta.setFailureHandler(&Obj::failureHandlerNoop);
                    ta.allocate(100);
                }

                const bsl::string& report = oss.str();
                ++expectedDefaultAllocations;

                ASSERT_STDERR(!report.empty());
                ASSERT_STDERR(npos != report.find("Error: memory leaked"));
                ASSERT_STDERR(npos != report.find("1 segment(s) in use"));
                ASSERT_STDERR((npos != report.find("my_allocator")) == NAME);
                if (CAN_FIND_SYMBOLS) {
                    ASSERT_STDERR(npos != report.find("BloombergLP"));
                    ASSERT_STDERR(npos != report.find(
                                             "baesu_StackTraceTestAllocator"));
                    ASSERT_STDERR(npos != report.find("allocate"));
                    ASSERT_STDERR(npos != report.find("main"));
                }
            }

#           undef ASSERT_STDERR
        }
      }  break;
      case 1: {
        //---------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concern:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 For varying values of 'maxDepths[d]':
        //:   o Declare a stack trace test allocator object, directing output
        //:     to a stringstream.  Call 'leakTwiceA', which will leak memory
        //:     in two places.
        //:   o If not 'veryVerbose', set the failure handler to '...Noop'.
        //:   o Destroy the stack trace allocator object.  If 'veryVerbose',
        //:     this will cause an abort due to leaked memory.
        //:   o Examine the trace written to the stringstream and verity that
        //:     it is as expected.
        //
        // Additional Instructions:
        //: o Set 'veryVerbose' to make this test abort when the test allocator
        //:   is destructed.
        //---------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        bslma::NewDeleteAllocator otherTa;

        int maxDepths[] = { 100, 10, 5, 4 };
        enum { NUM_MAX_DEPTHS = sizeof maxDepths / sizeof *maxDepths };

        for (int d = 0; d < NUM_MAX_DEPTHS; ++d) {
            if (verbose) {
                cout << endl << endl;
                P(maxDepths[d]);
            }

            bsl::stringstream out(&otherTa);

            {
                baesu_StackTraceTestAllocator ta("TestAlloc1",
                                                 (veryVerbose ? &cout : &out),
                                                 maxDepths[d]);
                leakTwiceAllocator = &ta;
                leakTwiceCount = 0;

                (*voidFuncs[idxVoidFuncLeakTwiceA])();                // RETURN

                ASSERT(!leakTwiceCount);

                if (!veryVerbose) {
                    ta.setFailureHandler(&Obj::failureHandlerNoop);
                }
            }

            const bsl::string& outStr = out.str();
            ++expectedDefaultAllocations;                // out.str() uses da

            bsl::size_t pos = 0;
            ASSERT(npos != (pos = outStr.find("TestAlloc1", pos)));
            if (CAN_FIND_SYMBOLS) {
                ASSERT(npos != (pos = outStr.find("BloombergLP",pos)));
                ASSERT(npos != (pos = outStr.find("StackTraceTestAllocator",
                                                                pos)));
                ASSERT(npos != (pos = outStr.find("allocate",   pos)));
                ASSERT(npos != (pos = outStr.find("leakTwiceC", pos)));
                ASSERT(npos != (pos = outStr.find("leakTwiceB", pos)));
                pos = 0;
                ASSERT(npos != (pos = outStr.find("leakTwiceA", pos)));
                LOOP_ASSERT(maxDepths[d], npos !=
                               (pos = outStr.find("leakTwiceA", pos + 1)));
                ASSERT(npos != (pos = outStr.find("main",       pos)));
            }
            if (testStatus > 0) P(outStr);

            if (verbose) {
                P_(maxDepths[d]);    P(outStr);
            }
        }
      }  break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

#if 0
    // Tracking calls to global 'new' & 'delete' was abandoned -- see comment
    // above.

    LOOP_ASSERT(numGlobalNewCalls,    0 == numGlobalNewCalls);
    LOOP_ASSERT(numGlobalDeleteCalls, 0 == numGlobalDeleteCalls);
#endif

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
