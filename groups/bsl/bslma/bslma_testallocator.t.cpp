// bslma_testallocator.t.cpp                                          -*-C++-*-

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_alignmentutil.h>
#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <stdio.h>              // 'printf'
#include <stdlib.h>             // 'atoi'
#include <string.h>             // 'memset', 'strlen'
#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>             // 'pipe', 'close', 'dup'
#endif
#if defined(BSLS_PLATFORM_OS_SOLARIS)
#include <sys/resource.h>       // 'setrlimit', etc.
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <crtdbg.h>  // '_CrtSetReportMode', to suppress popups
#else
#include <pthread.h>
#endif

#include <new>     // 'std::bad_alloc'

using namespace BloombergLP;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a concrete implementation of a protocol.  This "test"
// implementation calls 'malloc' and 'free' directly.  We must verify that
// objects of this concrete class do not call global operators 'new' and
// 'delete'.  We can do that by redefining these global operators and
// instrumenting them to be sure that these operators are in fact not called.
//
// We also need to verify that when bad addresses are supplied that we can
// detect them and report the problem to 'stdout'.  Since this behavior is
// not an error during the testing of this component, we will first set the
// quiet flag to suppress the output part, but will still verify the status
// to ensure that the problem was in fact detected.
//
// We must also verify that when exceptions are enabled, the test allocator
// throws an exception after the number of requests exceeds the allocator's
// (non-negative) allocation limit.  We achieve this by repeatedly allocating
// memory from the test allocator until the number of requests exceeds the
// allocation limit, then verify that exceptions thrown by the allocator are
// caught and that they contain the expected contents.
//-----------------------------------------------------------------------------
// [ 1] bslma::TestAllocator(bool verboseFlag = 0);
// [ 6] bslma::TestAllocator(const char *name, bool verboseFlag = 0);
// [ 2] ~bslma::TestAllocator();
// [ 3] void *allocate(size_type size);
// [ 3] void deallocate(void *address);
// [ 2] void setAllocationLimit(Int64 limit);
// [ 2] void setNoAbort(bool flagValue);
// [ 2] void setQuiet(bool flagValue);
// [ 2] void setVerbose(bool flagValue);
// [ 2] Int64 allocationLimit() const;
// [ 2] bool isNoAbort() const;
// [ 2] bool isQuiet() const;
// [ 2] bool isVerbose() const;
// [ 1] void *lastAllocatedAddress() const;
// [ 1] size_type lastAllocatedNumBytes() const;
// [ 1] void *lastDeallocatedAddress() const;
// [ 1] size_type lastDeallocatedNumBytes() const;
// [ 6] const char *name() const;
// [ 1] Int64 numAllocations() const;
// [ 1] Int64 numBlocksInUse() const;
// [ 1] Int64 numBlocksMax() const;
// [ 1] Int64 numBlocksTotal() const;
// [  ] Int64 numBoundsErrors() const;
// [ 1] Int64 numBytesInUse() const;
// [ 1] Int64 numBytesMax() const;
// [ 1] Int64 numBytesTotal() const;
// [ 1] Int64 numDeallocations() const;
// [ 1] Int64 numMismatches() const;
// [12] void print() const;
// [ 2] int status() const;
//-----------------------------------------------------------------------------
// [14] USAGE TEST
// [ 5] Ensure that exception is thrown after allocation limit is exceeded.
// [ 1] Make sure that all counts are initialized to zero (placement new).
// [ 1] Make sure that global operators new and delete are *not* called.
// [ 3] Ensure that the allocator is incompatible with new/delete.
// [ 3] Ensure that the allocator is incompatible with malloc/free.
// [ 3] Ensure that mismatched deallocations are detected/reported.
// [ 3] Ensure that repeated deallocations are detected/reported.
// [ 3] Ensure that an invalid cached length is reported.
// [ 3] Ensure that deallocated memory is scribbled.
// [ 3] Ensure that memory leaks (byte/block) are detected/reported.
// [ 7] Ensure that memory allocation list is kept track of properly.
// [ 8] Ensure that cross allocation/deallocation is detected immediately.
// [ 9] Ensure that 'std::bad_alloc' is thrown if 'malloc' fails.
// [10] Test 'numBlocksInUse', 'numBlocksTotal'
// [11] Ensure that over and underruns are properly caught.
// [13] Ensure that 'allocate' and 'deallocate' are thread-safe.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslma::TestAllocator Obj;

// This is copied from 'bslma_testallocator.cpp' to compare with scribbled
// deallocated memory.
const unsigned char SCRIBBLED_MEMORY = 0xA5;   // byte used to scribble
                                               // deallocated memory

enum { PADDING_SIZE = sizeof(bsls::AlignmentUtil::MaxAlignedType) };
                                                    // size of the padding
                                                    // before and after the
                                                    // user segment

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

typedef void *(*ThreadFunction)(void *arg);

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static
ThreadId createThread(ThreadFunction func, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, 0);
#else
    ThreadId id;
    pthread_create(&id, 0, func, arg);
    return id;
#endif
}

static
void joinThread(ThreadId id)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
#else
    pthread_join(id, 0);
#endif
}

namespace TestCase13 {

struct ThreadInfo {
    int  d_numIterations;
    Obj *d_obj_p;
};

extern "C" void *threadFunction1(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;

    int n = 2;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p = mX.allocate(n);  memset(p, 0xff, n);
        mX.deallocate(p);

        if (n > 10000) {
            n = 2;
        }
        else {
            n *= 2;
        }
    }

    return arg;
}

extern "C" void *threadFunction2(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;

    int n = 3;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p1 = mX.allocate(n);      memset(p1, 0xff, n);
        void *p2 = mX.allocate(n * 2);  memset(p2, 0xff, n * 2);
        mX.deallocate(p1);
        mX.deallocate(p2);

        if (n > 10000) {
            n = 3;
        }
        else {
            n *= 3;
        }
    }

    return arg;
}

extern "C" void *threadFunction3(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_obj_p;

    int n = 5;

    for (int i = 0; i < info->d_numIterations; ++i) {
        void *p1 = mX.allocate(n);      memset(p1, 0xff, n);
        void *p2 = mX.allocate(n * 3);  memset(p2, 0xff, n * 3);
        void *p3 = mX.allocate(n * 7);  memset(p3, 0xff, n * 7);
        mX.deallocate(p3);
        mX.deallocate(p2);
        mX.deallocate(p1);

        if (n > 10000) {
            n = 5;
        }
        else {
            n *= 5;
        }
    }

    return arg;
}

}  // close namespace TestCase13

//-----------------------------------------------------------------------------
//                      REDEFINED GLOBAL OPERATOR NEW
//-----------------------------------------------------------------------------

static int globalNewCalledCount = 0;
static int globalNewCalledCountIsEnabled = 0;

static int globalDeleteCalledCount = 0;
static int globalDeleteCalledCountIsEnabled = 0;

#ifdef BDE_BUILD_TARGET_EXC
void *operator new(size_t size) throw(std::bad_alloc)
#else
void *operator new(size_t size)
#endif
    // Trace use of global operator new.  Note that we must use printf
    // to avoid recursion.
{
    void *addr = malloc(size);

    if (globalNewCalledCountIsEnabled) {
        ++globalNewCalledCount;
        printf ("global new called, count = %d: %p\n",
                globalNewCalledCount, addr);
    }

    return addr;
}

#ifdef BDE_BUILD_TARGET_EXC
void operator delete(void *address) throw()
#else
void operator delete(void *address)
#endif
    // Trace use of global operator delete.
{
    if (globalDeleteCalledCountIsEnabled) {
        ++globalDeleteCalledCount;
        printf("global delete freeing: %p\n", address);
    }

    free(address);
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_shortarray.h

class my_ShortArray {
    short *d_array_p; // dynamically-allocated array of short integers
    int d_size;       // physical size of the 'd_array_p' array (elements)
    int d_length;     // logical length of the 'd_array_p' array (elements)
    bslma::Allocator *d_allocator_p; // holds (but does not own) allocator

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_ShortArray(bslma::Allocator *basicAllocator = 0);
        // Create an empty array.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, global
        // operators 'new' and 'delete' are used.
     // ...

    ~my_ShortArray();
    void append(short value);
    const short& operator[](int index) const { return d_array_p[index]; }
    int length() const { return d_length; }
    operator const short *() const { return d_array_p; }
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.cpp

my_ShortArray::my_ShortArray(bslma::Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator)
{
    size_t sz = d_size * sizeof *d_array_p;
    if (basicAllocator) {
        d_array_p = (short *) d_allocator_p->allocate(sz);
    }
    else {
        d_array_p = (short *) operator new(sz);
    }
}

my_ShortArray::~my_ShortArray()
{
    // CLASS INVARIANTS
    ASSERT(d_array_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length); ASSERT(d_length <= d_size);

    if (d_allocator_p) {
        d_allocator_p->deallocate(d_array_p);
    }
    else {
        operator delete(d_array_p);
    }
}

inline void my_ShortArray::append(short value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_array_p[d_length++] = value;
}

inline static
void reallocate(short            **array,
                int                newSize,
                int                length,
                bslma::Allocator  *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified 'newSize'
    // using the specified 'basicAllocator', or, if 'basicAllocator' is 0,
    // global operators 'new' and 'delete'.  The specified 'length' number of
    // leading elements are preserved.  Since the class invariant requires
    // that the physical capacity of the container may grow but never shrink;
    // the behavior is undefined unless length <= newSize.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= newSize);          // enforce class invariant

    short *tmp = *array;                // support exception neutrality
    size_t sz = newSize * sizeof **array;
    if (basicAllocator) {
        *array = (short *) basicAllocator->allocate(sz);
    }
    else {
        *array = (short *) operator new(sz);
    }

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);

    if (basicAllocator) {
        basicAllocator->deallocate(tmp);
    }
    else {
        operator delete(tmp);
    }
}

void my_ShortArray::increaseSize()
{
     int proposedNewSize = d_size * GROW_FACTOR;      // reallocate can throw
     ASSERT(proposedNewSize > d_length);
     reallocate(&d_array_p, proposedNewSize, d_length, d_allocator_p);
     d_size = proposedNewSize;                        // we're committed
}

void debugprint(const my_ShortArray& array)
{
    printf("[");
    const int len = array.length();
    for (int i = 0; i < len; ++i) {
        printf(" %d", array[i]);
    }
    printf(" ]");
    fflush(stdout);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.t.cpp

///Usage
///-----
// The 'bslma::TestAllocator' defined in this component can be used in
// conjunction with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
// 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros to test the memory usage
// patterns of an object that uses the 'bslma::Allocator' protocol in its
// interface.  In this example, we illustrate how we might test that an object
// under test is exception-neutral.  For illustration purposes, we will assume
// the existence of a 'my_shortarray' component implementing an
// 'std::vector'-like array type, 'myShortArray':
//..
//  // my_shortarray.t.cpp
//  #include <my_shortarray.h>
//
//  #include <bslma_testallocator.h>
//  #include <bslma_testallocatorexception.h>
//
//  // ...
//
//..
// Below we provide a 'static' function, 'areEqual', that will allow us to
// compare two short arrays:
//..
    static
    bool areEqual(const short *array1, const short *array2, int numElements)
        // Return 'true' if the specified initial 'numElements' in the
        // specified 'array1' and 'array2' have the same values, and 'false'
        // otherwise.
    {
        for (int i = 0; i < numElements; ++i) {
            if (array1[i] != array2[i]) {
                return false;                                         // RETURN
            }
        }
        return true;
    }
//
//  // ...
//
//..

static int verifyPrint(const bslma::TestAllocator& ta,
                       const char* const          FMT,
                       int                        verboseFlag)
    // Return 0 if the specified 'ta' prints the same message as 'FMT'.  Note
    // that this function uses 'pipe' and 'fork', therefore it only works with
    // unix.
#ifdef BSLS_PLATFORM_OS_UNIX
{
    bool verbose = verboseFlag > 2;
    bool veryVerbose = verboseFlag > 3;

    if (verbose) printf("\tCompare with expected result.\n");

    const int SIZE = 2000;         // Must be big enough to hold output string.
    const char XX  = (char) 0xFF;  // Value used to represent an unset char.

    char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
    const char *CTRL_BUF = mCtrlBuf; // Used to check for extra characters.

    char buf[SIZE];
    memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' char values.

    if (veryVerbose) printf("\nEXPECTED FORMAT:\n%s\n", FMT);

    // Because bslma is a low-level utility, bslma::TestAllocator does not
    // have a function to print to ostream, and thus cannot print to a
    // stringstream.  The print() member function always prints to stdout.
    // The code below forks a process and captures stdout to a memory
    // buffer.

    // We must now flush any io buffers to ensure that the stream read from the
    // forked process has exactly the state that we expect.

    fflush(stdout);

    int pipes[2];
    ssize_t sz;
    pipe(pipes);
    if (fork()) {
        // Parent process.  Read pipe[0] into memory
        sz = read(pipes[0], buf, SIZE);
        if (sz >= 0) { buf[sz] = '\0'; }
    }
    else {
        // Child process, print to pipes[1].
        close(1);
        dup(pipes[1]);

        // This call print() function sends its output to the pipe,
        // which is in turn read into 'buf' by the parent process.
        ta.print();

        exit(0);
    }

    if (veryVerbose) printf("\nACTUAL FORMAT:\n%s\n", buf);

    ASSERT(sz > 0);     // Check that something was printed (and read).
    ASSERT(sz < SIZE);  // Check buffer is large enough.
    ASSERT(XX == buf[SIZE - 1]);  // Check for overrun.

    return memcmp(buf, FMT, sz);
}
#else
{
    return 0;
}
#endif // defined BSLS_PLATFORM_OS_UNIX

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // Additional code for usage test:
    bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

    switch (test) { case 0:
      case 14: {
        // --------------------------------------------------------------------
        // TEST USAGE
        //   Verify that the usage example for testing exception neutrality is
        //   free of syntax errors and works as advertised.
        //
        // Testing:
        //   USAGE TEST - Make sure usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST USAGE"
                            "\n==========\n");

        typedef short Element;
        const Element VALUES[] = { 1, 2, 3, 4, -5 };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        const Element &V0 = VALUES[0],
                      &V1 = VALUES[1],
                      &V2 = VALUES[2],
                      &V3 = VALUES[3],
                      &V4 = VALUES[4];

// The following is an abbreviated standard test driver.  Note that the number
// of arguments specify the verbosity level that the test driver uses for
// printing messages:
//..
//  int main(int argc, char *argv[])
//  {
//      int                 test = argc > 1 ? atoi(argv[1]) : 0;
//      bool             verbose = argc > 2;
//      bool         veryVerbose = argc > 3;
//      bool     veryVeryVerbose = argc > 4;
//      bool veryVeryVeryVerbose = argc > 5;
//
//..
// We now define a 'bslma::TestAllocator', 'sa', named "supplied" to indicate
// that it is the allocator to be supplied to our object under test, as well as
// to the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' macro (below).  Note that
// if 'veryVeryVeryVerbose' is 'true', then 'sa' prints all allocation and
// deallocation requests to 'stdout' and also prints the accumulated statistics
// on destruction:
//..
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

//      switch (test) { case 0:
//
//        // ...
//
//        case 6: {
//
//          // ...
//
            struct {
                int   d_line;
                int   d_numElem;
                short d_exp[NUM_VALUES];
            } DATA[] = {
                { L_, 0, { } },
                { L_, 1, { V0 } },
                { L_, 5, { V0, V1, V2, V3, V4 } }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE     = DATA[ti].d_line;
                const int    NUM_ELEM = DATA[ti].d_numElem;
                const short *EXP      = DATA[ti].d_exp;

                if (veryVerbose) { T_ P_(ti) P_(NUM_ELEM) }

                // ...

//..
// All code that we want to test for exception-safety must be enclosed within
// the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
// 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros, which internally implement
// a 'do'-'while' loop.  Code provided by the
// 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' macro sets the allocation limit
// of the supplied allocator to 0 causing it to throw an exception on the first
// allocation.  This exception is caught by code provided by the
// 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macro, which increments the
// allocation limit by 1 and re-runs the same code again.  Using this scheme we
// can check that our code does not leak memory for any memory allocation
// request.  Note that the curly braces surrounding these macros, although
// visually appealing, are not technically required:
//..
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                  my_ShortArray mA(&sa);
                  const my_ShortArray& A = mA;
                  for (int ei = 0; ei < NUM_ELEM; ++ei) {
                      mA.append(VALUES[ei]);
                  }
                  if (veryVerbose) { T_ T_  P_(NUM_ELEM) P(A) }
                  LOOP_ASSERT(LINE, areEqual(EXP, A, NUM_ELEM));
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }

//..
// After the exception-safety test we can ensure that all the memory allocated
// from 'sa' was successfully deallocated:
//..
            if (veryVerbose) sa.print();
//
//        } break;
//
//        // ...
//
//      }
//
//      // ...
//  }
//..
// Note that the 'BDE_BUILD_TARGET_EXC' macro is defined at compile-time to
// indicate whether or not exceptions are enabled.

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CONCURRENCY
        //   Ensure that 'allocate' and 'deallocate' are thread-safe.
        //
        // Concerns:
        //: 1 That 'allocate' and 'deallocate' are thread-safe.  (Note that
        //:   although all methods of 'bslma::TestAllocator' are thread-safe,
        //:   the thread safety of 'allocate' and 'deallocate' are of paramount
        //:   concern.)
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator'.
        //:
        //: 2 Within a loop, create three threads that iterate a specified
        //:   number of times and that perform a different sequence of
        //:   allocation and deallocation operations on the test allocator from
        //:   P-1.
        //:
        //: 3 After each iteration, use the accessors to verify the expected
        //:   state of the test allocator.  (C-1)
        //
        // Testing:
        //   CONCERN: 'allocate' and 'deallocate' are thread-safe.
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONCURRENCY"
                            "\n===========\n");

        using namespace TestCase13;

        Obj mX("concurrent allocator", veryVeryVeryVerbose);
        const Obj& X = mX;

        const int NUM_TEST_ITERATIONS   =  10;
        const int NUM_THREAD_ITERATIONS = 500;

        ThreadInfo info = { NUM_THREAD_ITERATIONS, &mX };

        for (int ti = 0; ti < NUM_TEST_ITERATIONS; ++ti) {
            ThreadId id1 = createThread(&threadFunction1, &info);
            ThreadId id2 = createThread(&threadFunction2, &info);
            ThreadId id3 = createThread(&threadFunction3, &info);

            joinThread(id1);
            joinThread(id2);
            joinThread(id3);

            ASSERT(0 == X.status());

            ASSERT(X.lastAllocatedAddress());
            ASSERT(X.lastDeallocatedAddress());

            ASSERT(X.numAllocations() == X.numDeallocations());

            ASSERT(0  < X.lastAllocatedNumBytes());
            ASSERT(0  < X.lastDeallocatedNumBytes());

            ASSERT(0 == X.numBlocksInUse());
            ASSERT(0  < X.numBlocksMax());
            ASSERT(0  < X.numBlocksTotal());


            ASSERT(0 == X.numBytesInUse());
            ASSERT(0  < X.numBytesMax());
            ASSERT(0  < X.numBytesTotal());

            ASSERT(0 == X.numBoundsErrors());
            ASSERT(0 == X.numMismatches());
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TEST 'print' METHOD
        //
        // Concerns:
        //: 1 The method correctly prints the allocator state including the
        //:   in use, maximum, and total number of blocks and bytes.
        //:
        //: 2 Any outstanding allocations are mentioned.
        //:
        //: 3 The allocator name is provided is specified.
        //:
        //: 4 The method is declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //: 4 For each row (representing a distinct attribute value, 'V') in
        //:   the table of P-3, verify that the class method returns the
        //:   expected value.  (C-1)
        //:
        //: 1 Create a 'bslma_TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   void print() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST 'print' METHOD"
                            "\n===================\n");

        static const char *TEMPLATE_WITH_NAME =
                        "\n"
                        "==================================================\n"
                        "                TEST ALLOCATOR %s STATE\n"
                        "--------------------------------------------------\n"
                        "        Category\tBlocks\tBytes\n"
                        "        --------\t------\t-----\n"
                        "          IN USE\t%lld\t%lld\n"
                        "             MAX\t%lld\t%lld\n"
                        "           TOTAL\t%lld\t%lld\n"
                        "      MISMATCHES\t%lld\n"
                        "   BOUNDS ERRORS\t%lld\n"
                        "--------------------------------------------------\n";

        static const char *TEMPLATE_WITHOUT_NAME =
                        "\n"
                        "==================================================\n"
                        "                TEST ALLOCATOR STATE\n"
                        "--------------------------------------------------\n"
                        "        Category\tBlocks\tBytes\n"
                        "        --------\t------\t-----\n"
                        "          IN USE\t%lld\t%lld\n"
                        "             MAX\t%lld\t%lld\n"
                        "           TOTAL\t%lld\t%lld\n"
                        "      MISMATCHES\t%lld\n"
                        "   BOUNDS ERRORS\t%lld\n"
                        "--------------------------------------------------\n";

        const int MAX_ENTRIES = 10;
        static const
        struct DefaultDataRow {
            int         d_line;                  // source line number
            const char *d_name_p;                // allocator name
            int         d_numAllocs;             // number of allocations
            int         d_allocs[MAX_ENTRIES];   // num allocation bytes
            int         d_numDeallocs;           // number of deallocations
            int         d_deallocs[MAX_ENTRIES]; // num deallocation indices
        } DATA [] = {
            {
                L_,
                0,
                0,
                { 0 },
                0,
                { 0 },
            },
            {
                L_,
                "",
                0,
                { 0 },
                0,
                { 0 },
            },
            {
                L_,
                "FIRST_ALLOCATOR",
                0,
                { 0 },
                0,
                { 0 },
            },
            {
                L_,
                0,
                2,
                { 10, 20 },
                1,
                { 1 },
            },
            {
                L_,
                "",
                2,
                { 10, 20 },
                1,
                { 1 },
            },
            {
                L_,
                "SecondAllocator",
                2,
                { 10, 20 },
                1,
                { 1 },
            },
            {
                L_,
                0,
                5,
                { 10, 20, 30, 40, 50 },
                2,
                { 1, 3 },
            },
            {
                L_,
                "",
                5,
                { 10, 20, 30, 40, 50 },
                2,
                { 1, 3 },
            },
            {
                L_,
                "Third Allocator",
                5,
                { 10, 20, 30, 40, 50 },
                2,
                { 1, 3 },
            },
            {
                L_,
                0,
                2,
                { 10, 20 },
                2,
                { 0, 1 },
            },
            {
                L_,
                "",
                2,
                { 10, 20 },
                2,
                { 0, 1 },
            },
            {
                L_,
                "Fourth_Allocator",
                2,
                { 10, 20 },
                2,
                { 0, 1 },
            },
            {
                L_,
                0,
                10,
                { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
                5,
                { 0, 2, 4, 6, 8 },
            },
            {
                L_,
                "",
                10,
                { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
                5,
                { 0, 2, 4, 6, 8 },
            },
            {
                L_,
                "finalallocator",
                10,
                { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
                5,
                { 0, 2, 4, 6, 8 },
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE           = DATA[ti].d_line;
            const char       *NAME           = DATA[ti].d_name_p;
            const int         NUM_ALLOCS     = DATA[ti].d_numAllocs;
            const int        *ALLOCS         = DATA[ti].d_allocs;
            const int         NUM_DEALLOCS   = DATA[ti].d_numDeallocs;
            const int        *DEALLOCS       = DATA[ti].d_deallocs;
                  void       *addresses[MAX_ENTRIES];
                  int         numRemAllocs   = 0;
                  int         remAllocs[MAX_ENTRIES] = { 0 };

            (void) LINE;
            (void) ALLOCS;
            (void) DEALLOCS;


            Obj ta;
            Obj *mXPtr = NAME ? new (ta) Obj(NAME) : new (ta) Obj();

            Obj& mX = *mXPtr;  const Obj& X = mX;

            for (int di = 0; di < NUM_ALLOCS; ++di) {
                const int BYTES = DATA[ti].d_allocs[di];

                addresses[di] = mX.allocate(BYTES);
            }

            for (int di = 0; di < NUM_DEALLOCS; ++di) {
                const int INDEX = DATA[ti].d_deallocs[di];

                ASSERT(0 <= INDEX && INDEX < NUM_ALLOCS);

                mX.deallocate(addresses[INDEX]);
            }

            const int   EXP_BUF_SIZE = 1024;
            char        expBuffer[EXP_BUF_SIZE];

            if (NAME) {
                sprintf(expBuffer,
                        TEMPLATE_WITH_NAME,
                        NAME,
                        X.numBlocksInUse(), X.numBytesInUse(),
                        X.numBlocksMax(), X.numBytesMax(),
                        X.numBlocksTotal(), X.numBytesTotal(),
                        X.numMismatches(), X.numBoundsErrors());
            }
            else {
                sprintf(expBuffer,
                        TEMPLATE_WITHOUT_NAME,
                        X.numBlocksInUse(), X.numBytesInUse(),
                        X.numBlocksMax(), X.numBytesMax(),
                        X.numBlocksTotal(), X.numBytesTotal(),
                        X.numMismatches(), X.numBoundsErrors());
            }

            size_t offset = strlen(expBuffer);

            if (NUM_ALLOCS != NUM_DEALLOCS) {
                for (int i = 0; i < NUM_ALLOCS; ++i) {
                    bool wasDeallocated = false;
                    for (int j = 0; j < NUM_DEALLOCS; ++j) {
                        const int INDEX = DATA[ti].d_deallocs[j];

                        if (INDEX == i) {
                            wasDeallocated = true;
                            break;
                        }
                    }

                    if (!wasDeallocated) {
                        remAllocs[numRemAllocs] = i;
                        ++numRemAllocs;
                    }
                }

                strcpy(expBuffer + offset,
                       " Indices of Outstanding Memory Allocations:\n ");
                offset = strlen(expBuffer);

                for (int i = 0; i < numRemAllocs; ++i) {
                    sprintf(expBuffer + offset, "%d\t", remAllocs[i]);
                    offset = strlen(expBuffer);
                }

                if (numRemAllocs > 0) {
                    sprintf(expBuffer + offset, "\n ");
                }
            }
            ASSERT(0 == verifyPrint(X, expBuffer, argc));

            for (int i = 0; i < numRemAllocs; ++i) {
                mX.deallocate(addresses[remAllocs[i]]);
            }

            ta.deleteObject(mXPtr);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING BUFFER OVERRUN DETECTION
        //
        // Concerns:
        //   That the test allocator properly detects buffer over and underrun.
        //
        // Plan:
        //   Set the allocator in quiet mode, then deliberately cause buffer
        //   over and underruns to happen, verifying that they have happened
        //   by observer 'numMismatches'.  First, verify that legitimate
        //   writing over the user segment does not trigger false errors.  At
        //   the end, verify 'status' is what's expected and that
        //  'numMismatches' is still 0.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting buffer over/underrun detection"
                            "\n======================================\n");

        Obj alloc(veryVeryVerbose);
        char *seg;

        alloc.setQuiet(true);

        // verify we can write all over the user segment without triggering
        // over/underrun fault
        for (int segLen = 1; segLen < 1000; ++segLen) {
            seg = (char *) alloc.allocate(segLen);
            memset(seg, 0x07, segLen);
            alloc.deallocate(seg);
            ASSERT(0 == alloc.numMismatches());
            ASSERT(0 == alloc.numBoundsErrors());

            seg = (char *) alloc.allocate(segLen);
            memset(seg, 0, segLen);
            alloc.deallocate(seg);
            ASSERT(0 == alloc.numMismatches());
            ASSERT(0 == alloc.numBoundsErrors());
        }

        if (veryVerbose) {
            printf("Done verifying that writing over the segment doesn't\n"
                   "    trigger over/under run errors, numMismatches = %lld\n",
                   alloc.numMismatches());
        }

        bsls::Types::Int64 expectedBoundsErrors = 0;
        bool success;

        // Verify overrun is detected -- write on the trailing pad, deallocate,
        // and verify that numMismatches() has incremented to reflect the error
        // was caught.
        for (int segLen = 1; segLen < 1000; ++segLen) {
            for (int badByte = 0; badByte < PADDING_SIZE; ++badByte) {
                seg = (char *) alloc.allocate(segLen);
                seg[segLen + badByte] = 0x07;
                alloc.deallocate(seg);
                ASSERT(success =
                          (++expectedBoundsErrors == alloc.numBoundsErrors()));

                if (veryVerbose) {
                    printf("%successfully tested overrun at %d bytes"
                           " past the end of %d length segment\n",
                           (success ? "S" : "Uns"), badByte, segLen);
                }
            }
        }

        // Verify underrun is detected -- write on the trailing pad,
        // deallocate, and verify that numMismatches() has incremented to
        // reflect the error was caught.
        for (int segLen = 1; segLen < 1000; ++segLen) {
            for (int badByte = 0; badByte < PADDING_SIZE; ++badByte) {
                seg = (char *) alloc.allocate(segLen);
                seg[-(badByte + 1)] = 0x07;
                alloc.deallocate(seg);
                ASSERT(success =
                          (++expectedBoundsErrors == alloc.numBoundsErrors()));

                if (veryVerbose) {
                    printf("%successfully tested underrun at %d bytes"
                           " before the start of %d length segment\n",
                           (success ? "S" : "Uns"), badByte, segLen);
                }
            }
        }

        ASSERT(!alloc.numMismatches());

        // detect how far before the segment we have to corrupt to cause a
        // mismatch error.
        int mismatchByte = PADDING_SIZE - 1;
        do {
            ++mismatchByte;
            seg = (char *) alloc.allocate(100);
            seg[-mismatchByte] = 0x07;
            alloc.deallocate(seg);
        } while (!alloc.numMismatches());

        expectedBoundsErrors = alloc.numBoundsErrors();
        int mismatchErrors = 1;
        ASSERT(alloc.numMismatches() == mismatchErrors);

        // Verify underrun is not detected when a mismatch occurs
        for (int segLen = 1; segLen < 1000; ++segLen) {
            for (int badByte = 0; badByte < PADDING_SIZE; ++badByte) {
                seg = (char *) alloc.allocate(segLen);
                seg[-(badByte + 1)] = 0x07;
                seg[-mismatchByte] = 0x07;
                alloc.deallocate(seg);
                ASSERT(success =
                         (expectedBoundsErrors == alloc.numBoundsErrors() &&
                          ++mismatchErrors     == alloc.numMismatches()));

                if (veryVerbose) {
                    printf("%successfully tested underrun with mismatch at %d "
                           "bytes before the start of %d length segment\n",
                           (success ? "S" : "Uns"), badByte, segLen);
                }
            }
        }

        ASSERT(alloc.status() == expectedBoundsErrors + mismatchErrors);
        ASSERT(expectedBoundsErrors == alloc.numBoundsErrors());
        ASSERT(mismatchErrors       == alloc.numMismatches());
      } break;
      case 10: {
        if (verbose) printf("\nExpose bug in 'bslma::TestAllocator'\n");
        {
            Obj testAllocator(veryVeryVerbose);
            bslma::Allocator *ta = &testAllocator;

            ASSERT(0 == testAllocator.numBlocksTotal());
            ASSERT(0 == testAllocator.numBlocksInUse());

            void *p = ta->allocate(123);
            ASSERT(1 == testAllocator.numBlocksTotal());
            ASSERT(1 == testAllocator.numBlocksInUse());

            void *q = ta->allocate(456);

            ASSERT(2 == testAllocator.numBlocksTotal());

            ta->deallocate(q);
            ta->deallocate(p);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TEST ALLOCATOR THROWS std::bad_alloc IF 'malloc' FAILS
        //
        // Testing:
        //   That the test allocator throws std::bad_alloc if it is unable
        //   to get memory.
        //
        // Concerns:
        //   That the test allocator will throw std::bad_alloc if 'malloc'
        //   returns 0.
        //
        // Plan:
        //   Set up a request for memory that fails, and verify that the
        //   exception occurs.
        // --------------------------------------------------------------------

        // A number of ways of getting malloc to fail were considered, all
        // had problems.  Asking for huge amounts of memory often took a lot
        // of time and wasted resources before 'malloc' gave up.  The
        // best approach turned out to be to set a low limit on the amount
        // of memory that could be obtained, and exceed that.  It was,
        // however, not portable, so we decided to implement the test only
        // on Solaris.
        // TBD: Implement this test on more platforms

        if (verbose) printf("\nTEST THROWING STD::BAD_ALLOC"
                            "\n============================\n");

#ifdef BDE_BUILD_TARGET_EXC
// TBD This test is failing under gcc 4.3.2 with an uncaught exception.
// It does *not* appear to be an issue with EH support, but an issue with the
// test case proper.  In the debugger, it appeared that the runtime had
// insufficient resources to handle the exception, so 'abort' was invoked.
#if defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CMP_GNU)
        if (verbose) printf("\nTest throwing std::bad_alloc\n");

        rlimit rl = { 1 << 20, 1 << 20 };
        int sts = setrlimit(RLIMIT_AS, &rl);    // limit us to 1 MByte
        ASSERT(0 == sts);

        sts = getrlimit(RLIMIT_AS, &rl);
        ASSERT(0 == sts);
        if (verbose) printf(sizeof(int) == sizeof(rl.rlim_cur)
                            ? "Limits: %d, %d\n" : "Limits: %lld, %lld\n",
                            rl.rlim_cur,
                            rl.rlim_max);
        ASSERT(1 << 20 == rl.rlim_cur);

        Obj ta;

        bool caught = false;
        void *p = (void *) 0x12345678;
        try {
            p = ta.allocate(1 << 21);
            printf("Error: allocate() returned\n");
            ASSERT(0);
        } catch (std::bad_alloc) {
            caught = true;
            if (verbose) printf("Exception caught, p = %p\n", p);
        }

        ASSERT(caught);
#else
        if (verbose) printf(
                           "No testing.  Testing skipped on this platform.\n");
#endif
#else
        if (verbose) printf("No testing.  Exceptions are not enabled.\n");
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TEST CROSS MEMORY ALLOCATION/DEALLOCATION DETECTION
        //   Testing the detection of cross allocation/deallocation.  The
        //   implementation uses the memory allocation list managed within
        //   the 'bslma::TestAllocator'.  Makes sure that the number of
        //   mismatches are counted when cross allocation/deallocation occurs.
        //   Test using two allocators allocating the same amount of memory
        //   in the same sequence, and deallocating each other's memory block.
        //
        // Testing:
        //   'd_list' inside 'bslma::TestAllocator'
        // --------------------------------------------------------------------

        if (verbose) printf(
                    "\nTEST CROSS MEMORY ALLOCATION/DEALLOCATION DETECTION"
                    "\n===================================================\n");

        {
        if (verbose) printf("\tTest cross memory allocation list\n");

        Obj allocator1(veryVeryVerbose);
        allocator1.setNoAbort(verbose);
        allocator1.setQuiet(!veryVerbose);

        Obj allocator2(veryVeryVerbose);

        void *a1 = allocator1.allocate(40);
        void *a2 = allocator1.allocate(30);
        void *a3 = allocator1.allocate(20);

        void *b1 = allocator2.allocate(40);
        void *b2 = allocator2.allocate(30);
        void *b3 = allocator2.allocate(20);

        if (verbose) printf("Cross deallocating first block\n");
        allocator1.deallocate(b1);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(1  == allocator1.numMismatches());
        ASSERT(3  == allocator2.numBlocksInUse());
        ASSERT(90 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Cross deallocating second block\n");
        allocator1.deallocate(b2);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(2  == allocator1.numMismatches());
        ASSERT(3  == allocator2.numBlocksInUse());
        ASSERT(90 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Cross deallocating third block\n");
        allocator1.deallocate(b3);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(3  == allocator2.numBlocksInUse());
        ASSERT(90 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "second test allocator.  Try first block\n");
        allocator2.deallocate(b1);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(2  == allocator2.numBlocksInUse());
        ASSERT(50 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "second test allocator.  Try second block\n");
        allocator2.deallocate(b2);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(1  == allocator2.numBlocksInUse());
        ASSERT(20 == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "second test allocator.  Try third block\n");
        allocator2.deallocate(b3);
        ASSERT(3  == allocator1.numBlocksInUse());
        ASSERT(90 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(0  == allocator2.numBlocksInUse());
        ASSERT(0  == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "first test allocator.  Try first block\n");
        allocator1.deallocate(a1);
        ASSERT(2  == allocator1.numBlocksInUse());
        ASSERT(50 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(0  == allocator2.numBlocksInUse());
        ASSERT(0  == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "first test allocator.  Try second block\n");
        allocator1.deallocate(a2);
        ASSERT(1  == allocator1.numBlocksInUse());
        ASSERT(20 == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(0  == allocator2.numBlocksInUse());
        ASSERT(0  == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());

        if (verbose) printf("Make sure we can still deallocate using the "
                            "first test allocator.  Try third block\n");
        allocator1.deallocate(a3);
        ASSERT(0  == allocator1.numBlocksInUse());
        ASSERT(0  == allocator1.numBytesInUse());
        ASSERT(3  == allocator1.numMismatches());
        ASSERT(0  == allocator2.numBlocksInUse());
        ASSERT(0  == allocator2.numBytesInUse());
        ASSERT(0  == allocator2.numMismatches());
        }
     } break;
     case 7: {
        // --------------------------------------------------------------------
        // TEST MEMORY ALLOCATION LIST
        //   Testing the memory allocation list managed internally within the
        //   'bslma::TestAllocator'.  Ensures that the list of outstanding
        //   memory blocks are kept track of.  Test the case where there's no
        //   allocated memory block outstanding, all allocated memory
        //   blocks outstanding and some allocated memory blocks outstanding.
        //
        // Testing:
        //   'd_list' inside 'bslma::TestAllocator'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST MEMORY ALLOCATION LIST"
                            "\n===========================\n");

        {
        if (verbose) printf("\tTest empty memory allocation list\n");

        Obj a;

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t0\t0\n"
            "             MAX\t0\t0\n"
            "           TOTAL\t0\t0\n"
            "      MISMATCHES\t0\n"
            "   BOUNDS ERRORS\t0\n"
            "--------------------------------------------------\n"
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));
        }

        {
        if (verbose) printf("\tTest full memory allocation list\n");

        Obj a;

        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);
        void *p3 = a.allocate(20);
        void *p4 = a.allocate(10);
        void *p5 = a.allocate( 1);
        void *p6 = a.allocate( 1);
        void *p7 = a.allocate( 1);
        void *p8 = a.allocate( 1);
        void *p9 = a.allocate( 1);

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t9\t105\n"
            "             MAX\t9\t105\n"
            "           TOTAL\t9\t105\n"
            "      MISMATCHES\t0\n"
            "   BOUNDS ERRORS\t0\n"
            "--------------------------------------------------\n"
            " Indices of Outstanding Memory Allocations:\n"
            " 0\t1\t2\t3\t4\t5\t6\t7\t\n"
            " 8\t\n "
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));

        a.deallocate(p1);
        a.deallocate(p2);
        a.deallocate(p3);
        a.deallocate(p4);
        a.deallocate(p5);
        a.deallocate(p6);
        a.deallocate(p7);
        a.deallocate(p8);
        a.deallocate(p9);
        }

        {
        if (verbose) printf("\tTest partial memory allocation list\n");

        Obj a;

        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);
        void *p3 = a.allocate(20);
        void *p4 = a.allocate(10);
        void *p5 = a.allocate( 1);

        a.deallocate(p3);
        a.deallocate(p5);

        void *p6 = a.allocate(10);

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t4\t90\n"
            "             MAX\t5\t101\n"
            "           TOTAL\t6\t111\n"
            "      MISMATCHES\t0\n"
            "   BOUNDS ERRORS\t0\n"
            "--------------------------------------------------\n"
            " Indices of Outstanding Memory Allocations:\n"
            " 0\t1\t3\t5\t\n "
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));

        a.deallocate(p1);
        a.deallocate(p2);
        a.deallocate(p4);
        a.deallocate(p6);
        }

        {
        if (verbose) printf("\tTest empty memory allocation list\n");

        Obj a;

        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);

        a.deallocate(p1);
        a.deallocate(p2);

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t0\t0\n"
            "             MAX\t2\t70\n"
            "           TOTAL\t2\t70\n"
            "      MISMATCHES\t0\n"
            "   BOUNDS ERRORS\t0\n"
            "--------------------------------------------------\n"
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST NAMED CONSTRUCTOR AND NAME ACCESS
        //   Testing the 'bslma::TestAllocator' constructed with a name.
        //   Ensures that the name is accessible through the 'name' function.
        //
        // Testing:
        //   bslma::TestAllocator(const char *name, bool verboseFlag = 0);
        //   const char *name() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST NAMED CONSTRUCTOR AND NAME ACCESS"
                            "\n======================================\n");

        const char   *NAME   = "Test Allocator";
        const size_t  length = strlen(NAME);
        Obj a(NAME, veryVeryVerbose);

        if (verbose) printf("Make sure all internal states initialized.\n");
        ASSERT(0 == a.numBlocksInUse());
        ASSERT(0 == a.numBytesInUse());
        ASSERT(0 == a.numBlocksMax());
        ASSERT(0 == a.numBytesMax());
        ASSERT(0 == a.numBlocksTotal());
        ASSERT(0 == a.numBytesTotal());
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(0 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) printf("Make sure name is correct.\n");

        ASSERT(0 == memcmp(NAME, a.name(), length));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST ALLOCATION LIMIT
        //   Create a test allocator and set its allocation limit to varying
        //   values.  Verify that exception is thrown with the correct content
        //   for every allocation request that occurs after the number of
        //   requests exceeds the current allocation limit.  Also verify that
        //   exception is never thrown for negative allocation limits.
        //
        // Testing:
        //   Ensure that exception is thrown after allocation limit is exceeded
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST ALLOCATION LIMIT"
                            "\n=====================\n");

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nTesting for exception thrown after exceeding "
                              "allocation limit.\n");

        const int NUM_ALLOC = 5;
        const int LIMIT[] = { 0, 1, 4, 5, -1, -100 };
        const int NUM_TEST = sizeof LIMIT / sizeof *LIMIT;

        Obj mX(veryVeryVerbose);

        for (int ti = 0; ti < NUM_TEST; ++ti) {
            mX.setAllocationLimit(LIMIT[ti]);

            for (int ai = 0; ai < NUM_ALLOC; ++ai) {
                const bslma::Allocator::size_type SIZE = ai + 1;
                                             // alloc size varies for each test
                if (veryVerbose) { P_(ti); P_(ai); P_(SIZE); P(LIMIT[ti]); }
                try {
                    void *p = mX.allocate(SIZE);
                    mX.deallocate(p);
                }
                catch (bslma::TestAllocatorException& e) {
                    bslma::Allocator::size_type numBytes = e.numBytes();
                    if (veryVerbose) { printf("Caught: "); P(numBytes); }
                    LOOP2_ASSERT(ti, ai, LIMIT[ti] == ai);
                    LOOP2_ASSERT(ti, ai, SIZE == numBytes);
                    continue;
                }
                LOOP2_ASSERT(ti, ai, LIMIT[ti] != ai);
            }
        }
#else
        if (verbose) printf("\nNo testing.  Exceptions are not enabled.\n");
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRINT
        //   Lightly verify that the 'print' method works.
        //
        // Testing:
        //   void print() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRINT"
                            "\n=============\n");

        if (verbose) printf("\nTest a single case with unique fields.\n");

        Obj a;

        if (verbose) printf("\tSet up unique fields.\n");
        void *p1 = a.allocate(40);
        void *p2 = a.allocate(30);
        void *p3 = a.allocate(20);
        void *p4 = a.allocate( 1);

        a.deallocate(p4);
        a.deallocate(p1);

        p1 = a.allocate(10);

        ASSERT(3 == a.numBlocksInUse());  ASSERT( 60 == a.numBytesInUse());
        ASSERT(4 == a.numBlocksMax());    ASSERT( 91 == a.numBytesMax());
        ASSERT(5 == a.numBlocksTotal());  ASSERT(101 == a.numBytesTotal());
        ASSERT(0 == a.numMismatches());

        const char* const FMT =
            "\n"
            "==================================================\n"
            "                TEST ALLOCATOR STATE\n"
            "--------------------------------------------------\n"
            "        Category\tBlocks\tBytes\n"
            "        --------\t------\t-----\n"
            "          IN USE\t3\t60\n"
            "             MAX\t4\t91\n"
            "           TOTAL\t5\t101\n"
            "      MISMATCHES\t0\n"
            "   BOUNDS ERRORS\t0\n"
            "--------------------------------------------------\n"
            " Indices of Outstanding Memory Allocations:\n"
            " 1\t2\t4\t\n "
            ;

        ASSERT(0 == verifyPrint(a, FMT, argc));

        a.deallocate(p1);
        a.deallocate(p2);
        a.deallocate(p3);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST ERROR COUNTS
        //   Disable the abort mode and ensure that attempting failure modes
        //   is correctly detected and logged in the numMismatches counter.
        //   Also verify that status is correct.  Note that this test case
        //   will cause purify errors.
        //
        // Testing:
        //   ~bslma::TestAllocator();
        //   void *allocate(size_type size);
        //   void deallocate(void *address);
        //
        //   Ensure that the allocator is incompatible with new/delete.
        //   Ensure that the allocator is incompatible with malloc/free.
        //   Ensure that mismatched deallocations are detected/reported.
        //   Ensure that repeated deallocations are detected/reported.
        //   Ensure that an invalid cached length is reported.
        //   Ensure that deallocated memory is scribbled.
        //   Ensure that memory leaks (byte/block) are detected/reported.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST ERROR COUNTS"
                            "\n=================\n");

        if (verbose) printf(
                     "\nNote:\n"
                     "  Error messages can be viewed in veryVerbose mode\n"
                     "  and Memory trace messages in veryVeryVerbose mode.\n");

        const char *const LINE =
        "-------------------------------------------------------------------";

        {
            int i;
            if (verbose) printf("\nEnsure incompatibility with new/delete.\n");

            Obj a(veryVeryVerbose);
            a.setNoAbort(verbose); a.setQuiet(!veryVerbose);

            if (verbose) printf("\t[deallocate unallocated pointer]\n");
            if (veryVerbose) puts(LINE);
            void *p = operator new(100);
            for (i = 0; i < 100; ++i) { ((char *)p)[i] = (char) i; }
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(0 == a.status());
                                ASSERT(0 == a.numMismatches());

            a.deallocate(p);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(1 == a.status());
                                ASSERT(1 == a.numMismatches());
            operator delete(p);
            p = a.allocate(7);  ((char *)p)[0] = (char) 0xA7;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(7 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(1 == a.numMismatches());

            a.deallocate(p);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(1 == a.status());
                                ASSERT(1 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                   "\nEnsure repeated deallocations are detected/reported.\n");

            if (verbose) printf("\t[deallocate deallocated pointer]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate(p);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(2 == a.status());
                                ASSERT(2 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                               "\nEnsure incompatibility with malloc/free.\n");
            p = malloc(200);
            for (i = 0; i < 200; ++i) { ((char *)p)[i] = (char) i; }
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(2 == a.status());
                                ASSERT(2 == a.numMismatches());
            if (verbose) printf("\t[deallocate unallocated pointer]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate(p);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                ASSERT(3 == a.status());
                                ASSERT(3 == a.numMismatches());
            free(p);
            p = a.allocate(5);  ((char *)p)[0] = (char) 0xA5;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(5 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(3 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
       "\nEnsure immediately repeated deallocations are detected/reported.\n");
            if (verbose) printf("\t[deallocate deallocated pointer]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate(p);
            a.deallocate(p);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                LOOP_ASSERT(a.status(), 4 == a.status());
                                ASSERT(4 == a.numMismatches());

            p = a.allocate(3);  ((char *)p)[0] = (char) 0xA3;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(1 == a.numBlocksMax());
                                ASSERT(7 == a.numBytesMax());
                                ASSERT(3 == a.numBlocksTotal());
                                ASSERT(15 == a.numBytesTotal());
                                LOOP_ASSERT(a.status(), 4 == a.status());
                                ASSERT(4 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                 "\nEnsure mismatched deallocations are detected/reported.\n");

            if (verbose) printf(
                               "\t[deallocate pointer + sizeof(size_type)]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate((bslma::Allocator::size_type *)p + 1);
            if (veryVerbose) puts(LINE);
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(5 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                               "\t[deallocate pointer - sizeof(size_type)]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate((bslma::Allocator::size_type *)p - 1);
            if (veryVerbose) puts(LINE);
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(3 == a.numBytesInUse());
                                ASSERT(0  < a.status());
                                ASSERT(6 == a.numMismatches());
            a.deallocate(p);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                LOOP_ASSERT(a.status(), 6 == a.status());
                                ASSERT(6 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                           "\nEnsure an invalid cached length is reported.\n");

            struct B {
                int d_m;
                int d_s;
            } b[2] = { { (int) 0xdeadbeef, -1 }, { 0x11, 0x22 } };

            if (verbose) printf("\t[deallocate memory with bad length]\n");
            if (veryVerbose) puts(LINE);
            a.deallocate(b + 1);
            if (veryVerbose) puts(LINE);
                                ASSERT(0 == a.numBlocksInUse());
                                ASSERT(0 == a.numBytesInUse());
                                LOOP_ASSERT(a.status(), 7 == a.status());
                                ASSERT(7 == a.numMismatches());

#if 0
    // Accessing deallocated memory can result in errors on some platforms.
    // For this reason, this part of the test has been removed for now.
            if (verbose) printf("\nEnsure deallocated memory is scribbled.\n");
            unsigned char *q = (unsigned char *) a.allocate(9);
            memset(q, 0, 9);
            a.deallocate(q);
            if (verbose) printf("\t[deallocate memory scribbled]\n");
            for (int mi = 0; mi < 9; ++mi) {
                LOOP_ASSERT(mi, SCRIBBLED_MEMORY == q[mi]);
            }
            if (veryVerbose) puts(LINE);
#endif

            p = a.allocate(9);  ((char *)p)[0] = (char) 0xA9;
                                ASSERT(1 == a.numBlocksInUse());
                                ASSERT(9 == a.numBytesInUse());
                                ASSERT(0 <  a.status());
                                ASSERT(7 == a.numMismatches());
            if (veryVerbose) printf("status = %d\n", a.status());

            if (verbose) printf(
                "\nEnsure memory leaks (byte/block) are detected/reported.\n");

            if (verbose) printf("\t[memory leak]\n");
            if (veryVerbose) puts(LINE);
        }
        if (veryVerbose) puts(LINE);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // GET/SET FLAGS
        //   Verify that each of the get/set flags is working properly.
        //   Also verify that status correctly returns 0.
        //
        // Testing:
        //   void setAllocationLimit(Int64 limit);
        //   void setNoAbort(bool flagValue);
        //   void setQuiet(bool flagValue);
        //   void setVerbose(bool flagValue);
        //   Int64 allocationLimit() const;
        //   bool isNoAbort() const;
        //   bool isQuiet() const;
        //   bool isVerbose() const;
        //   int status() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nGET/SET FLAGS"
                            "\n=============\n");

        if (verbose) printf("\nTest get/set flags\n");
        {
            Obj a(veryVeryVerbose);

            ASSERT(0 == a.isQuiet());
            ASSERT(0 == a.isNoAbort());
            ASSERT(veryVeryVerbose == a.isVerbose());
            ASSERT(0 >  a.allocationLimit());

            if (verbose) printf("\tVerbose\n");

            a.setVerbose(10);   ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(1 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setVerbose(0);    ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) printf("\tQuiet\n");

            a.setQuiet(10);     ASSERT(1 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setQuiet(0);      ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) printf("\tNoAbort\n");

            a.setNoAbort(10);   ASSERT(0 == a.isQuiet());
                                ASSERT(1 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            a.setNoAbort(0);    ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 >  a.allocationLimit());

            if (verbose) printf("\tAllocationLimit\n");

            a.setAllocationLimit(5);
                                ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(5 ==  a.allocationLimit());

            a.setAllocationLimit(0);
                                ASSERT(0 == a.isQuiet());
                                ASSERT(0 == a.isNoAbort());
                                ASSERT(0 == a.isVerbose());
                                ASSERT(0 == a.allocationLimit());

            a.setAllocationLimit(-1);
                                ASSERT( 0 == a.isQuiet());
                                ASSERT( 0 == a.isNoAbort());
                                ASSERT( 0 == a.isVerbose());
                                ASSERT(-1 == a.allocationLimit());

            if (verbose) printf("\tStatus\n");

                                ASSERT(0 == a.status());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST:
        //   Create a test allocator in as buffer to make sure each field
        //   is properly initialized (with no verbose flag).  Then create a
        //   test allocator on the program stack and verify that all of the
        //   non-error counts are working properly.  Ensure that neither
        //   'new' or 'delete' are called.
        //
        // Testing:
        //   Make sure that all counts are initialized to zero (placement new).
        //
        //   bslma::TestAllocator(bool verboseFlag = 0);
        //   size_type lastAllocatedNumBytes() const;
        //   void *lastAllocatedAddress() const;
        //   size_type lastDeallocatedNumBytes() const;
        //   void *lastDeallocatedAddress() const;
        //   Int64 numAllocations() const;
        //   Int64 numBlocksInUse() const;
        //   Int64 numBlocksMax() const;
        //   Int64 numBlocksTotal() const;
        //   Int64 numBytesInUse() const;
        //   Int64 numBytesMax() const;
        //   Int64 numBytesTotal() const;
        //   Int64 numDeallocations() const;
        //   Int64 numMismatches() const;
        //
        //   Make sure that global operators new and delete are *not* called.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC TEST"
                            "\n==========\n");

        if (verbose) printf("\nCreate an allocator in a buffer\n");

        bsls::ObjectBuffer<Obj> arena;

        memset(&arena, 0xA5, sizeof arena);
        Obj *p = new(&arena) Obj;

        if (verbose) printf(
            "\nMake sure all counts/and flags are initialized\n");

        if (veryVerbose) p->print();

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == p->numBlocksInUse());
        ASSERT(0 == p->numBytesInUse());
        ASSERT(0 == p->numBlocksMax());
        ASSERT(0 == p->numBytesMax());
        ASSERT(0 == p->numBlocksTotal());
        ASSERT(0 == p->numBytesTotal());
        ASSERT(0 == p->isQuiet());
        ASSERT(0 == p->isNoAbort());
        ASSERT(0 == p->isVerbose());
        ASSERT(0 == p->status());
        ASSERT(0 == p->lastAllocatedNumBytes());
        ASSERT(0 == p->lastAllocatedAddress());
        ASSERT(0 == p->lastDeallocatedNumBytes());
        ASSERT(0 == p->lastDeallocatedAddress());
        ASSERT(0 == p->numAllocations());
        ASSERT(0 == p->numDeallocations());

        if (verbose) printf("\nCreate an allocator\n");

        Obj a(veryVeryVerbose);

        if (verbose) printf("\nMake sure counts work properly\n");

        if (veryVerbose) a.print();

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == a.numBlocksInUse());
        ASSERT(0 == a.numBytesInUse());
        ASSERT(0 == a.numBlocksMax());
        ASSERT(0 == a.numBytesMax());
        ASSERT(0 == a.numBlocksTotal());
        ASSERT(0 == a.numBytesTotal());
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(0 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) printf("\tallocate 1\n");
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = a.allocate(1);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(1 == a.numBlocksInUse());
        ASSERT(1 == a.numBytesInUse());
        ASSERT(1 == a.numBlocksMax());
        ASSERT(1 == a.numBytesMax());
        ASSERT(1 == a.numBlocksTotal());
        ASSERT(1 == a.numBytesTotal());
        ASSERT(1 == a.lastAllocatedNumBytes());
        ASSERT(addr1 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(1 == a.numAllocations());
        ASSERT(0 == a.numDeallocations());

        if (verbose) printf("\tallocate 20\n");
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = a.allocate(20);
        globalNewCalledCountIsEnabled = 0;
        ASSERT( 2 == a.numBlocksInUse());
        ASSERT(21 == a.numBytesInUse());
        ASSERT( 2 == a.numBlocksMax());
        ASSERT(21 == a.numBytesMax());
        ASSERT( 2 == a.numBlocksTotal());
        ASSERT(21 == a.numBytesTotal());
        ASSERT(20 == a.lastAllocatedNumBytes());
        ASSERT(addr2 == a.lastAllocatedAddress());
        ASSERT( 0 == a.lastDeallocatedNumBytes());
        ASSERT( 0 == a.lastDeallocatedAddress());
        ASSERT( 2 == a.numAllocations());
        ASSERT( 0 == a.numDeallocations());

        if (verbose) printf("\tdeallocate 20\n");
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT( 1 == a.numBlocksInUse());
        ASSERT( 1 == a.numBytesInUse());
        ASSERT( 2 == a.numBlocksMax());
        ASSERT(21 == a.numBytesMax());
        ASSERT( 2 == a.numBlocksTotal());
        ASSERT(21 == a.numBytesTotal());
        ASSERT(20 == a.lastAllocatedNumBytes());
        ASSERT(addr2 == a.lastAllocatedAddress());
        ASSERT(20 == a.lastDeallocatedNumBytes());
        ASSERT(addr2 == a.lastDeallocatedAddress());
        ASSERT( 2 == a.numAllocations());
        ASSERT( 1 == a.numDeallocations());

        if (verbose) printf("\tallocate 300\n");
        globalNewCalledCountIsEnabled = 1;
        void *addr3 = a.allocate(300);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(  2 == a.numBlocksInUse());
        ASSERT(301 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(20 == a.lastDeallocatedNumBytes());
        ASSERT(addr2 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  1 == a.numDeallocations());

        if (verbose) printf("\tdeallocate 300\n");
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr3);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(  1 == a.numBlocksInUse());
        ASSERT(  1 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(300 == a.lastDeallocatedNumBytes());
        ASSERT(addr3 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  2 == a.numDeallocations());

        if (verbose) printf("\tdeallocate 1\n");
        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(  0 == a.numBlocksInUse());
        ASSERT(  0 == a.numBytesInUse());
        ASSERT(  2 == a.numBlocksMax());
        ASSERT(301 == a.numBytesMax());
        ASSERT(  3 == a.numBlocksTotal());
        ASSERT(321 == a.numBytesTotal());
        ASSERT(300 == a.lastAllocatedNumBytes());
        ASSERT(addr3 == a.lastAllocatedAddress());
        ASSERT(1 == a.lastDeallocatedNumBytes());
        ASSERT(addr1 == a.lastDeallocatedAddress());
        ASSERT(  3 == a.numAllocations());
        ASSERT(  3 == a.numDeallocations());

        if (verbose) printf("\nMake sure allocate/deallocate invalid "
                            "size/address is recorded\n.");

        a.setNoAbort(1);
        a.setQuiet(1);

        if (verbose) printf("\tallocate 0\n");
        a.allocate(0);
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(1 == a.lastDeallocatedNumBytes());
        ASSERT(addr1 == a.lastDeallocatedAddress());
        ASSERT(4 == a.numAllocations());
        ASSERT(3 == a.numDeallocations());

        if (verbose) printf("\tdeallocate 0\n");
        a.deallocate(0);
        ASSERT(0 == a.lastAllocatedNumBytes());
        ASSERT(0 == a.lastAllocatedAddress());
        ASSERT(0 == a.lastDeallocatedNumBytes());
        ASSERT(0 == a.lastDeallocatedAddress());
        ASSERT(4 == a.numAllocations());
        ASSERT(4 == a.numDeallocations());

        if (verbose) printf("\nEnsure new and delete are not called.\n");
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING BUFFER UNDERRUN ABORT
        //
        // Concerns:
        //   That when we abort do to a buffer underrun in non-quiet mode, the
        //   message comes out right.
        //
        // Plan:
        //   Deliberately do a buffer underrun and abort.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting buffer underrun abort"
                            "\n=============================\n");

        Obj alloc(veryVeryVerbose);
        char *seg;

        // make non-quiet underrun happen (and abort) so we can observe the
        // message
        seg = (char *) alloc.allocate(100);
        seg[-3] = 0x07;
        alloc.deallocate(seg);
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // TESTING BUFFER OVERRUN ABORT
        //
        // Concerns:
        //   That when we abort do to a buffer overrun in non-quiet mode, the
        //   message comes out right.
        //
        // Plan:
        //   Deliberately do a buffer overrun and abort.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting buffer overrun abort"
                            "\n============================\n");

        Obj alloc(veryVeryVerbose);
        char *seg;

        // make non-quiet overrun happen (and abort) so we can observe the
        // message
        seg = (char *) alloc.allocate(100);
        seg[103] = 0x07;
        alloc.deallocate(seg);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
