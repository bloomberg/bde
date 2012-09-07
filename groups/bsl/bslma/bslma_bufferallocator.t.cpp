// bslma_bufferallocator.t.cpp                                        -*-C++-*-

#include <bslma_bufferallocator.h>

#if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED

#include <bslma_allocator.h>    // for testing only

#include <bsls_types.h>  // for testing only
#include <bsls_alignmentutil.h>
#include <bsls_exceptionutil.h>

#include <stdexcept>
#include <cstdlib>              // atoi()
#include <cstdio>
#include <cstring>              // memset(), memcpy(), strlen()
#include <iostream>
#include <stdio.h>              // snprintf()
#ifdef BSLS_PLATFORM__OS_UNIX
#include <unistd.h>             // pipe(), close() and dup().
#endif

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test, which is a concrete implementation of a protocol,
// allocates memory from a fixed-size buffer.  We must ensure that the
// allocated memory is properly aligned according to the size of allocation,
// current location of the cursor, and alignment strategy.  We verify these
// properties by sampling interesting values in each of the following four
// categories: 1) buffer size, 2) alignment, 3) cursor position, and 4)
// allocation size.  We create a table of test vectors containing the cross
// product of sets of values in the four categories and the expected values
// after allocation.  We then loop through the table of vectors, confirming
// that each specified allocation produces the expected results.  We must also
// ensure that, when a buffer allocator is installed with a callback function
// and the allocator runs out of memory, the callback function is invoked with
// the correct argument.
//-----------------------------------------------------------------------------
// [2] static void *allocateFromBuffer(cs, buf, bsz, sz, strategy);
// [2] static void *allocateFromBuffer(cs, buf, bsz, sz, alignment);
// [3] bslma::BufferAllocator(buf, bsz, callback);
// [3] bslma::BufferAllocator(buf, bsz, strategy, callback);
// [3] ~bslma::BufferAllocator();
// [3] void *allocate(sz);
// [3] void deallocate(addr);
// [4] void print() const;
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [6] USAGE TEST
// [5] Ensure that callback function is invoked when out-of-memory
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define PA(X) cout << #X " = " << ((void *) X) << endl;
#define PA_(X) cout << #X " = " << ((void *) X) << ", " << flush;
                                              // Print identifier and pointer.
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslma::BufferAllocator Obj;

//=============================================================================
//                        FILE-STATIC HELPER FUNCTIONS
//-----------------------------------------------------------------------------

static inline
int calcAlign(int size)
{
    return bsls::AlignmentUtil::calculateAlignmentFromSize(size);
}

static inline
int calcOffset(void *address, int alignment)
{
    return bsls::AlignmentUtil::calculateAlignmentOffset(address, alignment);
}

static int globalLastCallbackArg;
static int globalCallbackCnt;

// Idiom for calling an allocator and converting 'std::bad_alloc' to
// 'bsls::StdExceptionTranslator::bad_alloc'
struct AllocArgs
{
    bslma::Allocator            *d_alloc;
    bslma::Allocator::size_type  d_size;
    void                        *d_retValue;
};

static void *allocCallback(int size)
{
    globalLastCallbackArg = size;
    ++globalCallbackCnt;
    return (void *) 0;
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

class my_ShortArray {
    short *d_array_p; // dynamically-allocated array of short integers
    int d_size;       // physical size of the 'd_array_p' array (elements)
    int d_length;     // logical length of the 'd_array_p' array (elements)
    bslma::Allocator *d_allocator_p; // holds (but does not own) allocator

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_ShortArray(bslma::Allocator *basicAllocator);
        // Create an empty array using the specified 'basicAllocator' to
        // supply memory.
    // ...

    ~my_ShortArray();
    void append(int value);
    const short& operator[](int index) const { return d_array_p[index]; }
    int length() const { return d_length; }
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
// ...

my_ShortArray::my_ShortArray(bslma::Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator)
{
    ASSERT(d_allocator_p);
    d_array_p =
        (short *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

my_ShortArray::~my_ShortArray()
{
    // CLASS INVARIANTS
    ASSERT(d_array_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length); ASSERT(d_length <= d_size);
    ASSERT(d_allocator_p);

    d_allocator_p->deallocate(d_array_p);
}

inline void my_ShortArray::append(int value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_array_p[d_length++] = value;
}

inline static
void reallocate(short **array, int newSize, int length,
                bslma::Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified
    // 'newSize' using the specified 'basicAllocator' or global new
    // operator.  The specified 'length' number of leading elements are
    // preserved.  Since the class invariant requires that the physical
    // capacity of the container may grow but never shrink; the behavior
    // is undefined unless length <= newSize.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= newSize);        // enforce class invariant
    ASSERT(basicAllocator);

    short *tmp = *array;              // support exception neutrality
    *array = (short *) basicAllocator->allocate(newSize * sizeof **array);

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);
    basicAllocator->deallocate(tmp);
}

void my_ShortArray::increaseSize()
{
     int proposedNewSize = d_size * GROW_FACTOR;    // reallocate can throw
     ASSERT(proposedNewSize > d_length);
     reallocate(&d_array_p, proposedNewSize, d_length, d_allocator_p);
     d_size = proposedNewSize;                      // we're committed
}
// ...

void *callbackFunc(int size)
{
    cerr << "Buffer allocator out of memory." << endl;
    exit(-1);
    return (void *) 0;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        {
            static char memory[1024];
            bslma::BufferAllocator allocator(memory, sizeof memory,
                                             callbackFunc);
            my_ShortArray mA(&allocator);
            const my_ShortArray& A = mA;
            mA.append(123);
            ASSERT(A.length() == 1);
            ASSERT(123 == A[0]);

            mA.append(456);
            ASSERT(A.length() == 2);
            ASSERT(123 == A[0]);
            ASSERT(456 == A[1]);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CALLBACK TEST
        //   Create buffer allocators installed with a global callback
        //   function.  Allocate varying sizes of memory that exceed the
        //   allocators' capacities, and verify that the callback function is
        //   called when out-of-memory by checking the global variable that is
        //   assigned with the value of the last argument passed to the
        //   callback function.  Also verify the number of times that the
        //   callback is invoked by comparing the global callback counter
        //   (incremented inside the callback function) to the local counter.
        //   Using white-box knowledge of the implementation, a single
        //   cumulative test covering all objects under test is sufficient to
        //   confirm the invocation of the callback.
        //
        // Testing:
        //   Ensure that callback function is invoked when out-of-memory.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CALLBACK TEST" << endl
                                  << "=============" << endl;

        const int BUF_SZ = 64;
        char buf[BUF_SZ];
        int localCallbackCnt = 0;
        const int SIZE[] = { 0, 65, 1, 64, 5, 63, 6, 62, 12, 60, 24 };
        Obj mX(buf, BUF_SZ, allocCallback);
        Obj mY(buf, BUF_SZ, Obj::MAXIMUM_ALIGNMENT, allocCallback);
        Obj mZ(buf, BUF_SZ, Obj::NATURAL_ALIGNMENT, allocCallback);

        for (int i = 0; i < (int)(sizeof SIZE / sizeof *SIZE); ++i) {
            const int sz = SIZE[i];
            void *aX = mX.allocate(sz);
            if (0 == aX && 0 != sz) {
                if (veryVerbose) { P_(i); P(globalLastCallbackArg); }
                LOOP_ASSERT(i, sz == globalLastCallbackArg);
                globalLastCallbackArg = 0; // reset value
                ++localCallbackCnt;
            }
            void *aY = mY.allocate(sz);
            if (0 == aY && 0 != sz) {
                if (veryVerbose) { P_(i); P(globalLastCallbackArg); }
                LOOP_ASSERT(i, sz == globalLastCallbackArg);
                globalLastCallbackArg = 0;
                ++localCallbackCnt;
            }
            void *aZ = mZ.allocate(sz);
            if (0 == aZ && 0 != sz) {
                if (veryVerbose) { P_(i); P(globalLastCallbackArg); }
                LOOP_ASSERT(i, sz == globalLastCallbackArg);
                globalLastCallbackArg = 0;
                ++localCallbackCnt;
            }
        }
        if (veryVerbose) { P(globalCallbackCnt); }
        ASSERT(localCallbackCnt == globalCallbackCnt);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // OUTPUT OPERATOR TEST (<<)
        //   Create and configure a buffer allocator with some chosen values.
        //   Output the allocator to a buffer in memory and verify that the
        //   buffer contains the expected format.  Note that testing 'print'
        //   and 'operator<<(st, allocator)' implicitly tests
        //   'operator<<(st, strategy)' since the output of the first two
        //   methods contains the output of 'operator<<(st, strategy)'.
        //
        // Testing:
        //   void print() const;
        // --------------------------------------------------------------------

// The following code uses pipe() and fork(), so only works on Unix.
#ifdef BSLS_PLATFORM__OS_UNIX

        if (verbose) cout << endl << "PRINT TEST" << endl
                                  << "==========" << endl;

        const int MEM_SZ = 64;
        char memory[MEM_SZ];

        // 0 - MAXIMUM_ALIGNMENT, 1 - NATURAL_ALIGNMENT
        for (int i = 0; i < 2; ++i) {
            // configure allocator
            Obj::AlignmentStrategy strategy =
                i ? Obj::NATURAL_ALIGNMENT : Obj::MAXIMUM_ALIGNMENT;
            bslma::BufferAllocator a(memory, MEM_SZ, strategy);
            const int allocSz = 8;
            a.allocate(allocSz);
            const int offset = calcOffset(memory,
                                          i ? calcAlign(allocSz)
                                    : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

            // configure expected value buffer
            const int BUF_SZ =  1000;     // Big enough to hold output.
            const char XX = (char) 0xFF;  // Value for an unset char.
            char mExp[BUF_SZ];    const char *EXP = mExp;
            memset(mExp, XX, BUF_SZ);

            void *nullPtr = 0;            // Prevent output of integer 0.
            snprintf(mExp, BUF_SZ,
                     "buffer address      = %p\n"
                     "buffer size         = %d\n"
                     "cursor position     = %d\n"
                     "allocation function = %p\n"
                     "alignment strategy  = %s\n",
                     (void *) memory,
                     MEM_SZ,
                     allocSz + offset,
                     nullPtr,
                     (i ? "NATURAL_ALIGNMENT" : "MAXIMUM_ALIGNMENT"));

            // test and verify output
            char buf[BUF_SZ];  memset(buf, XX, BUF_SZ); // unset

            // Because bslma is a low-level utility, bslma::TestAllocator does
            // not have a function to print to ostream, and thus cannot print
            // to a strstream.  The print() member function always prints to
            // 'stdout'.  The code below forks a process and captures stdout
            // to a memory buffer.
            int pipes[2];
            int sz;
            pipe(pipes);
            if (fork()) {
                // Parent process.  Read pipe[0] into memory
                sz = read(pipes[0], buf, BUF_SZ);
                if (sz >= 0) { buf[sz] = '\0'; }
            }
            else {
                // Child process, print to pipes[1].
                close(1);
                dup(pipes[1]);

                // This call print() function sends its output to the pipe,
                // which is in turn read into 'buf' by the parent process.
                a.print();

                exit(0);
            }

            if (veryVerbose) {
                cout << "\nEXPECTED FORMAT:"       << endl << EXP  << endl
                     << "\nACTUAL FORMAT (print):" << endl << buf << endl;
            }

            const int EXP_SZ = strlen(EXP);
            ASSERT(EXP_SZ < BUF_SZ);           // Check buffer is large enough.
            ASSERT(sz < BUF_SZ);               // Check buffer is large enough.
            ASSERT(0 < sz);                    // Check something was printed
            ASSERT(XX == buf[BUF_SZ - 1]);     // Check for overrun.
            ASSERT(0 == memcmp(buf, EXP, BUF_SZ));
        }
#endif // defined BSLS_PLATFORM__OS_UNIX
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ALLOCATE TEST:
        //   Iterate over a set of loop-generated vectors of varying buffer
        //   size and perform independent tests.  For each test, create a
        //   default buffer allocator and buffer allocators using alignment
        //   strategies 'MAXIMUM_ALIGNMENT' and 'NATURAL_ALIGNMENT'.  Call the
        //   'allocate' method on each of the allocators for varying allocation
        //   sizes, and verify the results against those produced by calling
        //   'allocateFromBuffer' class method.  Note that the callback
        //   function mechanism is not tested in this case.
        //
        // Testing:
        //   bslma::BufferAllocator(buf, bsz, callback);
        //   bslma::BufferAllocator(buf, bsz, strategy, callback);
        //   ~bslma::BufferAllocator();
        //   void *allocate(sz);
        //   void deallocate(addr);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATE TEST" << endl
                                  << "=============" << endl;

        const int BUF_SZ[] = { 64, 63, 62, 60 };
        const int ALLOC_SZ[] = { 0, 1, 5, 6, 12, 24, 100 };

        for (int i = 0; i < (int)(sizeof BUF_SZ / sizeof *BUF_SZ); ++i) {
            const int SIZE = BUF_SZ[i];
            char *buf = new char[SIZE];
            int cursorM = 0, cursorN = 0;
            Obj mX(buf, SIZE);
            Obj mY(buf, SIZE, Obj::MAXIMUM_ALIGNMENT);
            Obj mZ(buf, SIZE, Obj::NATURAL_ALIGNMENT);

            for (int j = 0; j < (int)(sizeof ALLOC_SZ / sizeof *ALLOC_SZ);
                                                                         ++j) {
                const int sz = ALLOC_SZ[j];

                void *aM = Obj::allocateFromBuffer(&cursorM, buf, SIZE, sz,
                                                   Obj::MAXIMUM_ALIGNMENT);
                void *aN = Obj::allocateFromBuffer(&cursorN, buf, SIZE, sz,
                                                   Obj::NATURAL_ALIGNMENT);

                void *aX = 0;
                void *aY = 0;
                void *aZ = 0;

#ifdef BDE_BUILD_TARGET_EXC
                const int szM = sz;
                const int szN = sz;
#else
                // If exceptions are disabled, avoid causing allocate failure
                const int szM = (aM ? sz : 0);
                const int szN = (aN ? sz : 0);
#endif

                BSLS_TRY {
                    aX = mX.allocate(szM);
                    aY = mY.allocate(szM);
                }
                BSLS_CATCH (std::bad_alloc) {
                    // Allocation was supposed to fail
                    LOOP2_ASSERT(i, j, 0 != sz && 0 == aM);
                }

                BSLS_TRY {
                    aZ = mZ.allocate(szN);
                }
                BSLS_CATCH (std::bad_alloc) {
                    // Allocation was supposed to fail
                    LOOP2_ASSERT(i, j, 0 != sz && 0 == aN);
                }

                if (veryVerbose){
                    P_(sz); PA_(aX); PA_(aY); PA_(aZ); PA_(aM); PA(aN);
                }

                LOOP2_ASSERT(i, j, aX == aY);
                LOOP2_ASSERT(i, j, aX == aM);
                LOOP2_ASSERT(i, j, aZ == aN);

                mX.deallocate(aX);
                mY.deallocate(aY);
                mZ.deallocate(aZ);
            }
            delete [] buf;
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHODS TEST:
        //   Iterate over a set of tabulated test vectors and perform
        //   independent tests.  The test vectors consist of the cross product
        //   of sets of values for the buffer size, alignment, cursor position
        //   and allocation size.  If 'N4' is detected in a test vector's
        //   alignment field and the platform is 4-byte maximally aligned, or
        //   'N8' is detected and the platform is 8-byte maximally aligned,
        //   'allocateFromBuffer(cs, buf, bsz, sz, strategy)' is tested with
        //   'NATURAL_ALIGNMENT' as its alignment strategy.  Otherwise,
        //   'allocateFromBuffer(cs, buf, bsz, sz, alignment)' is tested.  If a
        //   test vector's alignment field value is equal to 'MAX_ALIGNMENT',
        //   'allocateFromBuffer(cs, buf, bsz, sz, strategy)' is also tested
        //   automatically with 'MAXIMUM_ALIGNMENT' as its alignment strategy.
        //   Verify that the resulting cursor is equal to the expected cursor
        //   position, and the allocated memory address is correctly offset
        //   according to the specified alignment.  Note that the above test is
        //   performed for buffers aligned on 4-byte and 8-byte boundaries.
        //
        // Testing:
        //   static void *allocateFromBuffer(cs, buf, bsz, sz, strategy);
        //   static void *allocateFromBuffer(cs, buf, bsz, sz, alignment);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CLASS METHODS TEST" << endl
                                  << "==================" << endl;

        enum NaturalAlignmentType {
            N4  = -1, // Natural alignment w/ 4-byte max alignment.
            N8  = -2  // Natural alignment w/ 8-byte max alignment.
        };
        enum {
            NA = -1   // Indicate 'null' is returned for a test vector.
        };

        // Note that for 'd_expOffset' and d_expCursor', the first element in
        // the array represents the expected value when the buffer is aligned
        // on a 4-byte boundary, and the second element for when the buffer is
        // is aligned on an 8-byte boundary.
        struct {
            int d_line;          // line number
            int d_bufSize;       // buffer size
            int d_align;         // alignment (1, 2, 4, 8, N4, N8)
            int d_cursorPos;     // initial cursor position
            int d_allocSize;     // allocation request size
            int d_expOffset[2];  // expected memory offset
            int d_expCursor[2];  // expected cursor position after request
        } DATA[] = {
   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     64,       1,        3,        3,       {0, 0},    {6, 6}     },
   {  L_,     64,       1,        3,        6,       {0, 0},    {9, 9}     },
   {  L_,     64,       1,        3,        12,      {0, 0},    {15, 15}   },
   {  L_,     64,       1,        3,        24,      {0, 0},    {27, 27}   },
   {  L_,     64,       1,        6,        3,       {0, 0},    {9, 9}     },
   {  L_,     64,       1,        6,        6,       {0, 0},    {12, 12}   },
   {  L_,     64,       1,        6,        12,      {0, 0},    {18, 18}   },
   {  L_,     64,       1,        6,        24,      {0, 0},    {30, 30}   },
   {  L_,     64,       1,        12,       3,       {0, 0},    {15, 15}   },
   {  L_,     64,       1,        12,       6,       {0, 0},    {18, 18}   },
   {  L_,     64,       1,        12,       12,      {0, 0},    {24, 24}   },
   {  L_,     64,       1,        12,       24,      {0, 0},    {36, 36}   },
   {  L_,     64,       1,        24,       3,       {0, 0},    {27, 27}   },
   {  L_,     64,       1,        24,       6,       {0, 0},    {30, 30}   },
   {  L_,     64,       1,        24,       12,      {0, 0},    {36, 36}   },
   {  L_,     64,       1,        24,       24,      {0, 0},    {48, 48}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     64,       2,        3,        3,       {1, 1},    {7, 7}     },
   {  L_,     64,       2,        3,        6,       {1, 1},    {10, 10}   },
   {  L_,     64,       2,        3,        12,      {1, 1},    {16, 16}   },
   {  L_,     64,       2,        3,        24,      {1, 1},    {28, 28}   },
   {  L_,     64,       2,        6,        3,       {0, 0},    {9, 9}     },
   {  L_,     64,       2,        6,        6,       {0, 0},    {12, 12}   },
   {  L_,     64,       2,        6,        12,      {0, 0},    {18, 18}   },
   {  L_,     64,       2,        6,        24,      {0, 0},    {30, 30}   },
   {  L_,     64,       2,        12,       3,       {0, 0},    {15, 15}   },
   {  L_,     64,       2,        12,       6,       {0, 0},    {18, 18}   },
   {  L_,     64,       2,        12,       12,      {0, 0},    {24, 24}   },
   {  L_,     64,       2,        12,       24,      {0, 0},    {36, 36}   },
   {  L_,     64,       2,        24,       3,       {0, 0},    {27, 27}   },
   {  L_,     64,       2,        24,       6,       {0, 0},    {30, 30}   },
   {  L_,     64,       2,        24,       12,      {0, 0},    {36, 36}   },
   {  L_,     64,       2,        24,       24,      {0, 0},    {48, 48}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     64,       4,        3,        3,       {1, 1},    {7, 7}     },
   {  L_,     64,       4,        3,        6,       {1, 1},    {10, 10}   },
   {  L_,     64,       4,        3,        12,      {1, 1},    {16, 16}   },
   {  L_,     64,       4,        3,        24,      {1, 1},    {28, 28}   },
   {  L_,     64,       4,        6,        3,       {2, 2},    {11, 11}   },
   {  L_,     64,       4,        6,        6,       {2, 2},    {14, 14}   },
   {  L_,     64,       4,        6,        12,      {2, 2},    {20, 20}   },
   {  L_,     64,       4,        6,        24,      {2, 2},    {32, 32}   },
   {  L_,     64,       4,        12,       3,       {0, 0},    {15, 15}   },
   {  L_,     64,       4,        12,       6,       {0, 0},    {18, 18}   },
   {  L_,     64,       4,        12,       12,      {0, 0},    {24, 24}   },
   {  L_,     64,       4,        12,       24,      {0, 0},    {36, 36}   },
   {  L_,     64,       4,        24,       3,       {0, 0},    {27, 27}   },
   {  L_,     64,       4,        24,       6,       {0, 0},    {30, 30}   },
   {  L_,     64,       4,        24,       12,      {0, 0},    {36, 36}   },
   {  L_,     64,       4,        24,       24,      {0, 0},    {48, 48}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     64,       8,        3,        3,       {1, 5},    { 7, 11}   },
   {  L_,     64,       8,        3,        6,       {1, 5},    {10, 14}   },
   {  L_,     64,       8,        3,        12,      {1, 5},    {16, 20}   },
   {  L_,     64,       8,        3,        24,      {1, 5},    {28, 32}   },
   {  L_,     64,       8,        6,        3,       {6, 2},    {15, 11}   },
   {  L_,     64,       8,        6,        6,       {6, 2},    {18, 14}   },
   {  L_,     64,       8,        6,        12,      {6, 2},    {24, 20}   },
   {  L_,     64,       8,        6,        24,      {6, 2},    {36, 32}   },
   {  L_,     64,       8,        12,       3,       {0, 4},    {15, 19}   },
   {  L_,     64,       8,        12,       6,       {0, 4},    {18, 22}   },
   {  L_,     64,       8,        12,       12,      {0, 4},    {24, 28}   },
   {  L_,     64,       8,        12,       24,      {0, 4},    {36, 40}   },
   {  L_,     64,       8,        24,       3,       {4, 0},    {31, 27}   },
   {  L_,     64,       8,        24,       6,       {4, 0},    {34, 30}   },
   {  L_,     64,       8,        24,       12,      {4, 0},    {40, 36}   },
   {  L_,     64,       8,        24,       24,      {4, 0},    {52, 48}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     64,       N4,       3,        3,       {0, 0},    {6, 6}     },
   {  L_,     64,       N4,       3,        6,       {1, 1},    {10, 10}   },
   {  L_,     64,       N4,       3,        12,      {1, 1},    {16, 16}   },
   {  L_,     64,       N4,       3,        24,      {1, 1},    {28, 28}   },
   {  L_,     64,       N4,       6,        3,       {0, 0},    {9, 9}     },
   {  L_,     64,       N4,       6,        6,       {0, 0},    {12, 12}   },
   {  L_,     64,       N4,       6,        12,      {2, 2},    {20, 20}   },
   {  L_,     64,       N4,       6,        24,      {2, 2},    {32, 32}   },
   {  L_,     64,       N4,       12,       3,       {0, 0},    {15, 15}   },
   {  L_,     64,       N4,       12,       6,       {0, 0},    {18, 18}   },
   {  L_,     64,       N4,       12,       12,      {0, 0},    {24, 24}   },
   {  L_,     64,       N4,       12,       24,      {0, 0},    {36, 36}   },
   {  L_,     64,       N4,       24,       3,       {0, 0},    {27, 27}   },
   {  L_,     64,       N4,       24,       6,       {0, 0},    {30, 30}   },
   {  L_,     64,       N4,       24,       12,      {0, 0},    {36, 36}   },
   {  L_,     64,       N4,       24,       24,      {0, 0},    {48, 48}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     64,       N8,       3,        3,       {0, 0},    {6, 6}     },
   {  L_,     64,       N8,       3,        6,       {1, 1},    {10, 10}   },
   {  L_,     64,       N8,       3,        12,      {1, 1},    {16, 16}   },
   {  L_,     64,       N8,       3,        24,      {1, 5},    {28, 32}   },
   {  L_,     64,       N8,       6,        3,       {0, 0},    {9, 9}     },
   {  L_,     64,       N8,       6,        6,       {0, 0},    {12, 12}   },
   {  L_,     64,       N8,       6,        12,      {2, 2},    {20, 20}   },
   {  L_,     64,       N8,       6,        24,      {6, 2},    {36, 32}   },
   {  L_,     64,       N8,       12,       3,       {0, 0},    {15, 15}   },
   {  L_,     64,       N8,       12,       6,       {0, 0},    {18, 18}   },
   {  L_,     64,       N8,       12,       12,      {0, 0},    {24, 24}   },
   {  L_,     64,       N8,       12,       24,      {0, 4},    {36, 40}   },
   {  L_,     64,       N8,       24,       3,       {0, 0},    {27, 27}   },
   {  L_,     64,       N8,       24,       6,       {0, 0},    {30, 30}   },
   {  L_,     64,       N8,       24,       12,      {0, 0},    {36, 36}   },
   {  L_,     64,       N8,       24,       24,      {4, 0},    {52, 48}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     60,       1,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     60,       1,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     60,       1,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     60,       1,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     60,       2,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     60,       2,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     60,       2,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     60,       2,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     60,       4,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     60,       4,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     60,       4,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     60,       4,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     60,       8,        56,       1,       {NA, 0},   {56, 57}   },
   {  L_,     60,       8,        56,       2,       {NA, 0},   {56, 58}   },
   {  L_,     60,       8,        56,       4,       {NA, 0},   {56, 60}   },
   {  L_,     60,       8,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     60,       N4,       56,       1,       {0, 0},    {57, 57}   },
   {  L_,     60,       N4,       56,       2,       {0, 0},    {58, 58}   },
   {  L_,     60,       N4,       56,       4,       {0, 0},    {60, 60}   },
   {  L_,     60,       N4,       56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     60,       N8,       56,       1,       {0, 0},    {57, 57}   },
   {  L_,     60,       N8,       56,       2,       {0, 0},    {58, 58}   },
   {  L_,     60,       N8,       56,       4,       {0, 0},    {60, 60}   },
   {  L_,     60,       N8,       56,       8,       {NA, NA},  {56, 56}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     62,       1,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     62,       1,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     62,       1,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     62,       1,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     62,       2,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     62,       2,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     62,       2,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     62,       2,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     62,       4,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     62,       4,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     62,       4,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     62,       4,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     62,       8,        56,       1,       {4, 0},    {61, 57}   },
   {  L_,     62,       8,        56,       2,       {4, 0},    {62, 58}   },
   {  L_,     62,       8,        56,       4,       {NA, 0},   {56, 60}   },
   {  L_,     62,       8,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     62,       N4,       56,       1,       {0, 0},    {57, 57}   },
   {  L_,     62,       N4,       56,       2,       {0, 0},    {58, 58}   },
   {  L_,     62,       N4,       56,       4,       {0, 0},    {60, 60}   },
   {  L_,     62,       N4,       56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     62,       N8,       56,       1,       {0, 0},    {57, 57}   },
   {  L_,     62,       N8,       56,       2,       {0, 0},    {58, 58}   },
   {  L_,     62,       N8,       56,       4,       {0, 0},    {60, 60}   },
   {  L_,     62,       N8,       56,       8,       {NA, NA},  {56, 56}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     63,       1,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     63,       1,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     63,       1,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     63,       1,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     63,       2,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     63,       2,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     63,       2,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     63,       2,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     63,       4,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     63,       4,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     63,       4,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     63,       4,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     63,       8,        56,       1,       {4, 0},    {61, 57}   },
   {  L_,     63,       8,        56,       2,       {4, 0},    {62, 58}   },
   {  L_,     63,       8,        56,       4,       {NA, 0},   {56, 60}   },
   {  L_,     63,       8,        56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     63,       N4,       56,       1,       {0, 0},    {57, 57}   },
   {  L_,     63,       N4,       56,       2,       {0, 0},    {58, 58}   },
   {  L_,     63,       N4,       56,       4,       {0, 0},    {60, 60}   },
   {  L_,     63,       N4,       56,       8,       {NA, NA},  {56, 56}   },
   {  L_,     63,       N8,       56,       1,       {0, 0},    {57, 57}   },
   {  L_,     63,       N8,       56,       2,       {0, 0},    {58, 58}   },
   {  L_,     63,       N8,       56,       4,       {0, 0},    {60, 60}   },
   {  L_,     63,       N8,       56,       8,       {NA, NA},  {56, 56}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     64,       1,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     64,       1,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     64,       1,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     64,       1,        56,       8,       {0, 0},    {64, 64}   },
   {  L_,     64,       2,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     64,       2,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     64,       2,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     64,       2,        56,       8,       {0, 0},    {64, 64}   },
   {  L_,     64,       4,        56,       1,       {0, 0},    {57, 57}   },
   {  L_,     64,       4,        56,       2,       {0, 0},    {58, 58}   },
   {  L_,     64,       4,        56,       4,       {0, 0},    {60, 60}   },
   {  L_,     64,       4,        56,       8,       {0, 0},    {64, 64}   },
   {  L_,     64,       8,        56,       1,       {4, 0},    {61, 57}   },
   {  L_,     64,       8,        56,       2,       {4, 0},    {62, 58}   },
   {  L_,     64,       8,        56,       4,       {4, 0},    {64, 60}   },
   {  L_,     64,       8,        56,       8,       {NA, 0},   {56, 64}   },
   {  L_,     64,       N4,       56,       1,       {0, 0},    {57, 57}   },
   {  L_,     64,       N4,       56,       2,       {0, 0},    {58, 58}   },
   {  L_,     64,       N4,       56,       4,       {0, 0},    {60, 60}   },
   {  L_,     64,       N4,       56,       8,       {0, 0},    {64, 64}   },
   {  L_,     64,       N8,       56,       1,       {0, 0},    {57, 57}   },
   {  L_,     64,       N8,       56,       2,       {0, 0},    {58, 58}   },
   {  L_,     64,       N8,       56,       4,       {0, 0},    {60, 60}   },
   {  L_,     64,       N8,       56,       8,       {NA, 0},   {56, 64}   },

   // Boundary conditions

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     64,       1,        0,        64,      {0, 0},    {64, 64}   },
   {  L_,     64,       1,        0,        65,      {NA, NA},  {0, 0}     },
   {  L_,     64,       1,        1,        63,      {0, 0},    {64, 64}   },
   {  L_,     64,       1,        1,        64,      {NA, NA},  {1, 1}     },
   {  L_,     64,       1,        63,       1,       {0, 0},    {64, 64}   },
   {  L_,     64,       1,        63,       2,       {NA, NA},  {63, 63}   },
   {  L_,     64,       1,        64,       0,       {NA, NA},  {64, 64}   },
   {  L_,     64,       1,        64,       1,       {NA, NA},  {64, 64}   },
   {  L_,     64,       2,        0,        64,      {0, 0},    {64, 64}   },
   {  L_,     64,       2,        0,        65,      {NA, NA},  {0, 0}     },
   {  L_,     64,       2,        1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     64,       2,        1,        64,      {NA, NA},  {1, 1}     },
   {  L_,     64,       2,        1,        62,      {1, 1},    {64, 64}   },
   {  L_,     64,       2,        63,       1,       {NA, NA},  {63, 63}   },
   {  L_,     64,       2,        63,       2,       {NA, NA},  {63, 63}   },
   {  L_,     64,       2,        64,       0,       {NA, NA},  {64, 64}   },
   {  L_,     64,       2,        64,       1,       {NA, NA},  {64, 64}   },
   {  L_,     64,       4,        0,        64,      {0, 0},    {64, 64}   },
   {  L_,     64,       4,        0,        65,      {NA, NA},  {0, 0}     },
   {  L_,     64,       4,        1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     64,       4,        1,        64,      {NA, NA},  {1, 1}     },
   {  L_,     64,       4,        1,        60,      {3, 3},    {64, 64}   },
   {  L_,     64,       4,        63,       1,       {NA, NA},  {63, 63}   },
   {  L_,     64,       4,        63,       2,       {NA, NA},  {63, 63}   },
   {  L_,     64,       4,        64,       0,       {NA, NA},  {64, 64}   },
   {  L_,     64,       4,        64,       1,       {NA, NA},  {64, 64}   },
   {  L_,     64,       8,        0,        64,      {NA, 0},   {0, 64}    },
   {  L_,     64,       8,        0,        65,      {NA, NA},  {0, 0}     },
   {  L_,     64,       8,        1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     64,       8,        1,        64,      {NA, NA},  {1, 1}     },
   {  L_,     64,       8,        1,        56,      {3, 7},    {60, 64}   },
   {  L_,     64,       8,        63,       1,       {NA, NA},  {63, 63}   },
   {  L_,     64,       8,        63,       2,       {NA, NA},  {63, 63}   },
   {  L_,     64,       8,        64,       0,       {NA, NA},  {64, 64}   },
   {  L_,     64,       8,        64,       1,       {NA, NA},  {64, 64}   },
   {  L_,     64,       N4,       0,        64,      {0, 0},    {64, 64}   },
   {  L_,     64,       N4,       0,        65,      {NA, NA},  {0, 0}     },
   {  L_,     64,       N4,       1,        63,      {0, 0},    {64, 64}   },
   {  L_,     64,       N4,       1,        64,      {NA, NA},  {1, 1}     },
   {  L_,     64,       N4,       63,       1,       {0, 0},    {64, 64}   },
   {  L_,     64,       N4,       63,       2,       {NA, NA},  {63, 63}   },
   {  L_,     64,       N4,       64,       0,       {NA, NA},  {64, 64}   },
   {  L_,     64,       N4,       64,       1,       {NA, NA},  {64, 64}   },
   {  L_,     64,       N8,       0,        64,      {NA, 0},   {0, 64}    },
   {  L_,     64,       N8,       0,        65,      {NA, NA},  {0, 0}     },
   {  L_,     64,       N8,       1,        63,      {0, 0},    {64, 64}   },
   {  L_,     64,       N8,       1,        64,      {NA, NA},  {1, 1}     },
   {  L_,     64,       N8,       63,       1,       {0, 0},    {64, 64}   },
   {  L_,     64,       N8,       63,       2,       {NA, NA},  {63, 63}   },
   {  L_,     64,       N8,       64,       0,       {NA, NA},  {64, 64}   },
   {  L_,     64,       N8,       64,       1,       {NA, NA},  {64, 64}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     63,       1,        0,        63,       {0, 0},   {63, 63}   },
   {  L_,     63,       1,        0,        64,      {NA, NA},  {0, 0}     },
   {  L_,     63,       1,        1,        62,      {0, 0},    {63, 63}   },
   {  L_,     63,       1,        1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     63,       1,        62,       1,       {0, 0},    {63, 63}   },
   {  L_,     63,       1,        62,       2,       {NA, NA},  {62, 62}   },
   {  L_,     63,       1,        63,       0,       {NA, NA},  {63, 63}   },
   {  L_,     63,       1,        63,       1,       {NA, NA},  {63, 63}   },
   {  L_,     63,       2,        0,        63,      {0, 0},    {63, 63}   },
   {  L_,     63,       2,        0,        64,      {NA, NA},  {0, 0}     },
   {  L_,     63,       2,        1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     63,       2,        1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     63,       2,        1,        61,      {1, 1},    {63, 63}   },
   {  L_,     63,       2,        62,       1,       {0, 0},    {63, 63}   },
   {  L_,     63,       2,        62,       2,       {NA, NA},  {62, 62}   },
   {  L_,     63,       2,        63,       0,       {NA, NA},  {63, 63}   },
   {  L_,     63,       2,        63,       1,       {NA, NA},  {63, 63}   },
   {  L_,     63,       4,        0,        63,      {0, 0},    {63, 63}   },
   {  L_,     63,       4,        0,        64,      {NA, NA},  {0, 0}     },
   {  L_,     63,       4,        1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     63,       4,        1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     63,       4,        1,        59,      {3, 3},    {63, 63}   },
   {  L_,     63,       4,        62,       1,       {NA, NA},  {62, 62}   },
   {  L_,     63,       4,        62,       2,       {NA, NA},  {62, 62}   },
   {  L_,     63,       4,        63,       0,       {NA, NA},  {63, 63}   },
   {  L_,     63,       4,        63,       1,       {NA, NA},  {63, 63}   },
   {  L_,     63,       8,        0,        63,      {NA, 0},   {0, 63}    },
   {  L_,     63,       8,        0,        64,      {NA, NA},  {0, 0}     },
   {  L_,     63,       8,        1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     63,       8,        1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     63,       8,        1,        55,      {3, 7},    {59, 63}   },
   {  L_,     63,       8,        62,       1,       {NA, NA},  {62, 62}   },
   {  L_,     63,       8,        62,       2,       {NA, NA},  {62, 62}   },
   {  L_,     63,       8,        63,       0,       {NA, NA},  {63, 63}   },
   {  L_,     63,       8,        63,       1,       {NA, NA},  {63, 63}   },
   {  L_,     63,       N4,       0,        63,      {0, 0},    {63, 63}   },
   {  L_,     63,       N4,       0,        64,      {NA, NA},  {0, 0}     },
   {  L_,     63,       N4,       1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     63,       N4,       1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     63,       N4,       62,       1,       {0, 0},    {63, 63}   },
   {  L_,     63,       N4,       62,       2,       {NA, NA},  {62, 62}   },
   {  L_,     63,       N4,       63,       0,       {NA, NA},  {63, 63}   },
   {  L_,     63,       N4,       63,       1,       {NA, NA},  {63, 63}   },
   {  L_,     63,       N8,       0,        63,      {0, 0},    {63, 63}   },
   {  L_,     63,       N8,       0,        64,      {NA, NA},  {0, 0}     },
   {  L_,     63,       N8,       1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     63,       N8,       1,        63,      {NA, NA},  {1, 1}     },
   {  L_,     63,       N8,       62,       1,       {0, 0},    {63, 63}   },
   {  L_,     63,       N8,       62,       2,       {NA, NA},  {62, 62}   },
   {  L_,     63,       N8,       63,       0,       {NA, NA},  {63, 63}   },
   {  L_,     63,       N8,       63,       1,       {NA, NA},  {63, 63}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     62,       1,        0,        62,      {0, 0},    {62, 62}   },
   {  L_,     62,       1,        0,        63,      {NA, NA},  {0, 0}     },
   {  L_,     62,       1,        1,        61,      {0, 0},    {62, 62}   },
   {  L_,     62,       1,        1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     62,       1,        61,       1,       {0, 0},    {62, 62}   },
   {  L_,     62,       1,        61,       2,       {NA, NA},  {61, 61}   },
   {  L_,     62,       1,        62,       0,       {NA, NA},  {62, 62}   },
   {  L_,     62,       1,        62,       1,       {NA, NA},  {62, 62}   },
   {  L_,     62,       2,        0,        62,      {0, 0},    {62, 62}   },
   {  L_,     62,       2,        0,        63,      {NA, NA},  {0, 0}     },
   {  L_,     62,       2,        1,        61,      {NA, NA},  {1, 1}     },
   {  L_,     62,       2,        1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     62,       2,        1,        60,      {1, 1},    {62, 62}   },
   {  L_,     62,       2,        61,       1,       {NA, NA},  {61, 61}   },
   {  L_,     62,       2,        61,       2,       {NA, NA},  {61, 61}   },
   {  L_,     62,       2,        62,       0,       {NA, NA},  {62, 62}   },
   {  L_,     62,       2,        62,       1,       {NA, NA},  {62, 62}   },
   {  L_,     62,       4,        0,        62,      {0, 0},    {62, 62}   },
   {  L_,     62,       4,        0,        63,      {NA, NA},  {0, 0}     },
   {  L_,     62,       4,        1,        61,      {NA, NA},  {1, 1}     },
   {  L_,     62,       4,        1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     62,       4,        1,        58,      {3, 3},    {62, 62}   },
   {  L_,     62,       4,        61,       1,       {NA, NA},  {61, 61}   },
   {  L_,     62,       4,        61,       2,       {NA, NA},  {61, 61}   },
   {  L_,     62,       4,        62,       0,       {NA, NA},  {62, 62}   },
   {  L_,     62,       4,        62,       1,       {NA, NA},  {62, 62}   },
   {  L_,     62,       8,        0,        62,      {NA, 0},   {0, 62}    },
   {  L_,     62,       8,        0,        63,      {NA, NA},  {0, 0}     },
   {  L_,     62,       8,        1,        61,      {NA, NA},  {1, 1}     },
   {  L_,     62,       8,        1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     62,       8,        1,        54,      {3, 7},    {58, 62}   },
   {  L_,     62,       8,        61,       1,       {NA, NA},  {61, 61}   },
   {  L_,     62,       8,        61,       2,       {NA, NA},  {61, 61}   },
   {  L_,     62,       8,        62,       0,       {NA, NA},  {62, 62}   },
   {  L_,     62,       8,        62,       1,       {NA, NA},  {62, 62}   },
   {  L_,     62,       N4,       0,        62,      {0, 0},    {62, 62}   },
   {  L_,     62,       N4,       0,        63,      {NA, NA},  {0, 0}     },
   {  L_,     62,       N4,       1,        61,      {0, 0},    {62, 62}   },
   {  L_,     62,       N4,       1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     62,       N4,       61,       1,       {0, 0},    {62, 62}   },
   {  L_,     62,       N4,       61,       2,       {NA, NA},  {61, 61}   },
   {  L_,     62,       N4,       62,       0,       {NA, NA},  {62, 62}   },
   {  L_,     62,       N4,       62,       1,       {NA, NA},  {62, 62}   },
   {  L_,     62,       N8,       0,        62,      {0, 0},    {62, 62}   },
   {  L_,     62,       N8,       0,        63,      {NA, NA},  {0, 0}     },
   {  L_,     62,       N8,       1,        61,      {0, 0},    {62, 62}   },
   {  L_,     62,       N8,       1,        62,      {NA, NA},  {1, 1}     },
   {  L_,     62,       N8,       61,       1,       {0, 0},    {62, 62}   },
   {  L_,     62,       N8,       61,       2,       {NA, NA},  {61, 61}   },
   {  L_,     62,       N8,       62,       0,       {NA, NA},  {62, 62}   },
   {  L_,     62,       N8,       62,       1,       {NA, NA},  {62, 62}   },

   // line    buffer    align-    cursor    alloc    expected   expected
   // no.     size      ment      pos       size     offset     cursor
   // ----    ------    ------    ------    -----    --------   --------
   {  L_,     60,       1,        0,        60,      {0, 0},    {60, 60}   },
   {  L_,     60,       1,        0,        61,      {NA, NA},  {0, 0}     },
   {  L_,     60,       1,        1,        59,      {0, 0},    {60, 60}   },
   {  L_,     60,       1,        1,        60,      {NA, NA},  {1, 1}     },
   {  L_,     60,       1,        59,       1,       {0, 0},    {60, 60}   },
   {  L_,     60,       1,        59,       2,       {NA, NA},  {59, 59}   },
   {  L_,     60,       1,        60,       0,       {NA, NA},  {60, 60}   },
   {  L_,     60,       1,        60,       1,       {NA, NA},  {60, 60}   },
   {  L_,     60,       2,        0,        60,      {0, 0},    {60, 60}   },
   {  L_,     60,       2,        0,        61,      {NA, NA},  {0, 0}     },
   {  L_,     60,       2,        1,        59,      {NA, NA},  {1, 1}     },
   {  L_,     60,       2,        1,        60,      {NA, NA},  {1, 1}     },
   {  L_,     60,       2,        1,        58,      {1, 1},    {60, 60}   },
   {  L_,     60,       2,        59,       1,       {NA, NA},  {59, 59}   },
   {  L_,     60,       2,        59,       2,       {NA, NA},  {59, 59}   },
   {  L_,     60,       2,        60,       0,       {NA, NA},  {60, 60}   },
   {  L_,     60,       2,        60,       1,       {NA, NA},  {60, 60}   },
   {  L_,     60,       4,        0,        60,      {0, 0},    {60, 60}   },
   {  L_,     60,       4,        0,        61,      {NA, NA},  {0, 0}     },
   {  L_,     60,       4,        1,        59,      {NA, NA},  {1, 1}     },
   {  L_,     60,       4,        1,        60,      {NA, NA},  {1, 1}     },
   {  L_,     60,       4,        1,        56,      {3, 3},    {60, 60}   },
   {  L_,     60,       4,        59,       1,       {NA, NA},  {59, 59}   },
   {  L_,     60,       4,        59,       2,       {NA, NA},  {59, 59}   },
   {  L_,     60,       4,        60,       0,       {NA, NA},  {60, 60}   },
   {  L_,     60,       4,        60,       1,       {NA, NA},  {60, 60}   },
   {  L_,     60,       8,        0,        60,      {NA, 0},   {0, 60}    },
   {  L_,     60,       8,        0,        61,      {NA, NA},  {0, 0}     },
   {  L_,     60,       8,        1,        59,      {NA, NA},  {1, 1}     },
   {  L_,     60,       8,        1,        60,      {NA, NA},  {1, 1}     },
   {  L_,     60,       8,        1,        52,      {3, 7},    {56, 60}   },
   {  L_,     60,       8,        59,       1,       {NA, NA},  {59, 59}   },
   {  L_,     60,       8,        59,       2,       {NA, NA},  {59, 59}   },
   {  L_,     60,       8,        60,       0,       {NA, NA},  {60, 60}   },
   {  L_,     60,       8,        60,       1,       {NA, NA},  {60, 60}   },
   {  L_,     60,       N4,       0,        60,      {0, 0},    {60, 60}   },
   {  L_,     60,       N4,       0,        61,      {NA, NA},  {0, 0}     },
   {  L_,     60,       N4,       1,        59,      {0, 0},    {60, 60}   },
   {  L_,     60,       N4,       1,        60,      {NA, NA},  {1, 1}     },
   {  L_,     60,       N4,       59,       1,       {0, 0},    {60, 60}   },
   {  L_,     60,       N4,       59,       2,       {NA, NA},  {59, 59}   },
   {  L_,     60,       N4,       60,       0,       {NA, NA},  {60, 60}   },
   {  L_,     60,       N4,       60,       1,       {NA, NA},  {60, 60}   },
   {  L_,     60,       N8,       0,        60,      {0, 0},    {60, 60}   },
   {  L_,     60,       N8,       0,        61,      {NA, NA},  {0, 0}     },
   {  L_,     60,       N8,       1,        59,      {0, 0},    {60, 60}   },
   {  L_,     60,       N8,       1,        60,      {NA, NA},  {1, 1}     },
   {  L_,     60,       N8,       59,       1,       {0, 0},    {60, 60}   },
   {  L_,     60,       N8,       59,       2,       {NA, NA},  {59, 59}   },
   {  L_,     60,       N8,       60,       0,       {NA, NA},  {60, 60}   },
   {  L_,     60,       N8,       60,       1,       {NA, NA},  {60, 60}   }
        };

        const int DATA_SZ = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "Testing 'allocateFromBuffer'." << endl;
        {
            typedef bsls::AlignmentUtil T;
            enum {
                FOUR_BYTE_ALIGN = 0,
                EIGHT_BYTE_ALIGN = 1,
                NUM_ALIGN = 2
            };

            char *memory = (char *) malloc(1000);
            char *buf;

            for (int ti = FOUR_BYTE_ALIGN; ti < NUM_ALIGN; ++ti) {
                switch (ti) {
                  case FOUR_BYTE_ALIGN: {
                      buf = (bsls::Types::UintPtr) memory % 8 == 4
                            ? memory : memory + 4;
                      if (verbose)
                          cout << "\tUsing 4-byte aligned buffer." << endl;
                  } break;
                  case EIGHT_BYTE_ALIGN: {
                      buf = (bsls::Types::UintPtr) memory % 8 == 4
                            ? memory + 4 : memory;
                      if (verbose)
                          cout << "\tUsing 8-byte aligned buffer." << endl;
                  } break;
                  default:
                    LOOP_ASSERT(ti, 0 && "*** Invalid alignment mode ***");
                }

                char *const BUF = buf; // ensure pointer not modified
                for (int i = 0; i < DATA_SZ; ++i) {
                    const int LINE = DATA[i].d_line;
                    const int SIZE = DATA[i].d_bufSize;
                    const int al   = DATA[i].d_align;
                    int       cp   = DATA[i].d_cursorPos;
                    const int iCp  = cp;        // initial cursor value.
                    const int as   = DATA[i].d_allocSize;
                    const int eo   = DATA[i].d_expOffset[ti];
                    const int ec   = DATA[i].d_expCursor[ti];

                    if ((N4 == al && 4 != T::BSLS_MAX_ALIGNMENT)
                        || (N8 == al && 8 != T::BSLS_MAX_ALIGNMENT)
                        || T::BSLS_MAX_ALIGNMENT < al) {
                        continue;
                    }

                    void *ret;
                    if (N4 == al || N8 == al) {
                        Obj::AlignmentStrategy st = Obj::NATURAL_ALIGNMENT;
                        ret = Obj::allocateFromBuffer(&cp, BUF, SIZE, as, st);
                    }
                    else {
                        ret = Obj::allocateFromBuffer(&cp, BUF, SIZE, as, al);
                    }

                    if (eo == NA) {
                        if (veryVerbose) {
                            cout << '\t'; P_(i); PA_(BUF); P_(cp); PA(ret);
                        }
                        LOOP3_ASSERT(LINE, ti, i, 0 == ret);
                    }
                    else {
                        int off = (char *) ret - (BUF + iCp);
                        if (veryVerbose) {
                            cout << '\t'; P_(i); PA_(BUF); P_(cp);
                            PA_(ret); P(off);
                        }
                        LOOP3_ASSERT(LINE, ti, i, off == eo);
                    }
                    LOOP3_ASSERT(LINE, ti, i, cp == ec);

                    if (al == T::BSLS_MAX_ALIGNMENT) {
                        // Test MAXIMUM_ALIGNMENT strategy.
                        int maxCp = DATA[i].d_cursorPos;
                        Obj::AlignmentStrategy st = Obj::MAXIMUM_ALIGNMENT;
                        void *maxRet = Obj::allocateFromBuffer(&maxCp, BUF,
                                                               SIZE, as, st);

                        if (veryVerbose) {cout << '\t'; P_(maxCp); PA(maxRet);}
                        LOOP3_ASSERT(LINE, ti, i, maxCp == cp);
                        LOOP3_ASSERT(LINE, ti, i, maxRet == ret);
                    }
                }
            }
            free(memory);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create buffer allocators using the default constructor.  Allocate
        //   memory of different sizes and verify that memory is properly
        //   aligned.  Also allocate memory using 'allocateFromBuffer' with
        //   different alignment and verify memory is aligned according to the
        //   specified alignment.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        typedef bsls::AlignmentUtil T;

        const int BUF_SZ = 64;
        char buffer[BUF_SZ];

        const int SIZE[] = { 65, 8, 1, 16, 1, 2, 4, 16, 32, 8, 1 };
        const int SIZE_LEN = sizeof SIZE / sizeof *SIZE;

        if (verbose) cout << "Trying bufferAllocator w/ default ctor." << endl;
        {
            bslma::BufferAllocator a(buffer, BUF_SZ);
            char *freePtr = buffer;

            for (int i = 0; i < SIZE_LEN; ++i) {
                int offset = calcOffset(freePtr, T::BSLS_MAX_ALIGNMENT);
                bool willFail = freePtr + offset + SIZE[i] > buffer + BUF_SZ;

#ifndef BDE_BUILD_TARGET_EXC
                if (willFail) {
                    // Skip this entry if it's going to fail and exceptions
                    // are turned off.
                    continue;
                }
#endif

                char *ret = 0;
                BSLS_TRY {
                    ret = (char*) a.allocate(SIZE[i]);

                    LOOP_ASSERT(i, ! willFail);
                    LOOP_ASSERT(i, freePtr <= ret);
                    LOOP_ASSERT(i, ret + SIZE[i] <= buffer + BUF_SZ);
                    LOOP_ASSERT(i, 0 == bsls::Types::UintPtr(ret)
                                                      % T::BSLS_MAX_ALIGNMENT);
                    freePtr = ret + SIZE[i];
                    a.deallocate(ret);
                }
                BSLS_CATCH (std::bad_alloc) {
                    LOOP_ASSERT(i, willFail);
                }
            }
        }

        if (verbose)
           cout << "Trying 'allocateFromBuffer' w/ natural alignment." << endl;
        {
            int cursor = 0;

            for (int i = 0; i < SIZE_LEN; ++i) {
                const int sz = SIZE[i];
                char *ret = (char *) Obj::allocateFromBuffer(
                                           &cursor, buffer,
                                           BUF_SZ, sz, Obj::NATURAL_ALIGNMENT);
                if (0 == ret) {
                    int offset =
                        calcOffset(buffer + cursor, calcAlign(sz));
                    LOOP_ASSERT(i, cursor + offset + sz > BUF_SZ);
                }
                else {
                    LOOP_ASSERT(i, buffer + cursor == ret + sz);
                    LOOP_ASSERT(i, ret + sz <= buffer + BUF_SZ);
                    LOOP_ASSERT(
                               i,
                               0 == bsls::Types::UintPtr(ret) % calcAlign(sz));
                }
            }
        }

        if (verbose)
            cout << "Trying 'allocateFromBuffer' w/ alignment of 2." << endl;
        {
            const int ALIGN = 2;
            int cursor = 0;

            for (int i = 0; i < SIZE_LEN; ++i) {
                const int sz = SIZE[i];
                char *ret = (char *) Obj::allocateFromBuffer(
                                                            &cursor, buffer,
                                                            BUF_SZ, sz, ALIGN);
                if (0 == ret) {
                    int offset =
                        calcOffset(buffer + cursor, ALIGN);
                    LOOP_ASSERT(i, cursor + offset + sz > BUF_SZ);
                }
                else {
                    LOOP_ASSERT(i, buffer + cursor == ret + sz);
                    LOOP_ASSERT(i, ret + sz <= buffer + BUF_SZ);
                    LOOP_ASSERT(i,
                                0 == bsls::Types::UintPtr(ret) % ALIGN);
                }
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

#else

int main(int argc, char *argv[])
{
    return 0;
}

#endif  // #if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED


// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
