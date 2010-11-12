// bdema_strpool.t.cpp  -*-C++-*-
#include <bdema_strpool.h>
#include <bdema_sequentialallocator.h>    // for testing only

#include <bslma_default.h>                // for testing only
#include <bslma_testallocator.h>          // for testing only
#include <bslma_testallocatorexception.h> // for testing only

#include <bsls_alignmentutil.h>           // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this 'bdema_StrPool' test suite are to verify that: 1) the
// 'allocate' method requests memory blocks of the expected sizes from its
// 'bdema_InfrequentDeleteBlockList' member object; 2) the 'allocate' method
// correctly adjusts the internal cursor position after memory is distributed;
// 3) the 'release' method releases all currently managed memory and the pool
// can be reused after 'release' is called; 4) the destructor releases all
// currently managed memory.
//
// To aid testing, the following functions are implemented:
//
// 1) int blockSize(int numBytes) - Return the adjusted block size based on the
// specified 'numBytes' using the same calculation performed by the
// 'bdema_InfrequentDeleteBlockList::allocate' method.
// 2) int growPool(bdema_StrPool& pool, int numBytes) - Invoke repeatedly the
// 'allocate' method of the specified 'pool' so that after this function
// returns, the next allocation request (size greater than 0) will cause the
// pool to allocate an internal memory block of size greater than or equal to
// the specified 'numBytes'.  Return this size.  Note that this function
// assumes that 'pool' has never been used previously.
//
// To achieve the above goals, the following respective testing procedures are
// executed (Note that in each procedure, one or more pools are created and
// initialized with a test allocator):
//
// 1) Request memory repeatedly to cause the pool to grow its internal memory
// block.  Verify that the expected memory block sizes are request from and
// recorded by the allocator.
// 2) Request memory of varying sizes that total to less than the current
// pool's size.  Verify that the difference between the returned memory
// addresses of two consecutive requests is equal to the size of the first
// request.  Also request memory of sizes not satisfiable by the current pool,
// and verify that the requests are recorded by the allocator, which indicates
// that the memory is allocated from the allocator and not from the pool's
// internal memory block.  Verify that a subsequent request satisfiable by the
// current pool returns a memory address within the pool's memory block.
// 3 and 4) Request memory of varying sizes, then invoke the pool's 'release'
// method.  Verify that the allocator indicates all memory is deallocated.
// Reuse the pool again to allocate memory and verify that 'allocate' still
// works as expected.  Allow the pool to leave scope and verify that all memory
// is deallocated.
//-----------------------------------------------------------------------------
// [2] bdema_StrPool(basicAllocator);
// [5] ~bdema_StrPool();
// [2] void *allocate(numBytes); // Verify memory block size growth.
// [4] void *allocate(numBytes); // Verify internal cursor calculation.
// [5] void release();
// [6] void reserveCapacity(numBytes);
// [7] void *allocate(numBytes); // Verify proper allocated sizes
//-----------------------------------------------------------------------------
// [8] USAGE EXAMPLE
// [3] int growPool(pool, numBytes);
// [1] int blockSize(numBytes);
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define A(X) cout << #X " = " << (void*) (X) << endl;
#define A_(X) cout << #X " = " << (void *) (X) << ", " << flush;
#define TAB cout << '\t';

//=============================================================================
//                     GLOBAL TYPE, CONSTANTS AND VARIABLES
//-----------------------------------------------------------------------------

typedef bdema_StrPool Obj;

// These type are copied from the 'bdema_InfrequentDeleteBlocklist.h' for
// testing purposes.

struct InfrequentDeleteBlock {
    InfrequentDeleteBlock              *d_next_p;
    bsls_AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

// This type is copied from 'bdema_strpool.cpp' to determine the internal
// limits of the 'bdema_StrPool' object.
enum {
    INITIAL_SIZE = 64,      // initial block size
    GROW_FACTOR  = 2,       // multiplicative factor to grow block size
    THRESHOLD    = 128,     // size beyond which a separate block is allocated
                            // if the current block cannot satisfy the request
    MAX_SIZE     = 8 * 1024 // maximum block size
};

int numLeftChildren   = 0;
int numMiddleChildren = 0;
int numRightChildren  = 0;
int numMostDerived    = 0;

struct LeftChild {
    char d_li;
    LeftChild()           { ++numLeftChildren; }
    virtual ~LeftChild()  { --numLeftChildren; }
};

struct MiddleChild {    // non-polymorphic middle child
    char d_mi;
    MiddleChild()         { ++numMiddleChildren; }
    ~MiddleChild()        { --numMiddleChildren; }
};

struct RightChild {
    char d_ri;
    RightChild()          { ++numRightChildren; }
    virtual ~RightChild() { --numRightChildren; }
};

struct MostDerived : LeftChild, MiddleChild, RightChild {
    char d_md;
    MostDerived()         { ++numMostDerived; }
    ~MostDerived()        { --numMostDerived; }
};

//=============================================================================
//                      FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int blockSize(int numBytes)
    // Return the adjusted block size based on the specified 'numBytes' using
    // the calculation performed by the
    // 'bdema_InfrequentDeleteBlockList::allocate' method.
{
    ASSERT(0 <= numBytes);

    if (numBytes) {
        numBytes += sizeof(InfrequentDeleteBlock) - 1;
        numBytes &= ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
    }

    return numBytes;
}

static int growPool(bdema_StrPool& pool, int numBytes)
    // Invoke repeatedly the 'allocate' method of the specified 'pool' so that
    // after this function returns, the next allocation request (size greater
    // than 0) will cause the pool to allocate an internal memory block of size
    // greater than or equal to the specified 'numBytes'.  Return this size.
    // Note that this function assumes that 'pool' has never been used
    // previously.
{
    if (numBytes > INITIAL_SIZE) {


        int leftOver = 0;
        int size;

        for (size = INITIAL_SIZE; size < numBytes; size *= GROW_FACTOR) {
            const int SZ = THRESHOLD > size ? size : THRESHOLD;
            const int N  = size / SZ;
            for (int i = 0; i < N; ++i) {
                pool.allocate(SZ);
            }
            leftOver = size % SZ;
            pool.allocate(leftOver);
        }
        return size;
    }
    return INITIAL_SIZE;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_cstrarray.h

class my_CstrArray {
    char            **d_array_p;     // dynamically allocated array
    int               d_size;        // physical capacity of this array
    int               d_length;      // logical length of this array
    bdema_StrPool     d_strPool;     // manages and supplies string memory
    bslma_Allocator  *d_allocator_p; // holds (but doesn't own) allocator

  private:  // not implemented.
    my_CstrArray(const my_CstrArray& original);

  private:
    void increaseSize();

  public:
    my_CstrArray(bslma_Allocator *basicAllocator = 0);
    ~my_CstrArray();

    my_CstrArray& operator=(const my_CstrArray& rhs);
    void append(const char *item);
    const char *operator[](int ind) const { return d_array_p[ind];}
    int length() const { return d_length; }
};

// FREE OPERATORS
ostream& operator<<(ostream& stream, const my_CstrArray& array);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_cstrarray.cpp

enum {
    MY_INITIAL_SIZE = 1, // initial physical capacity (number of elements)
    MY_GROW_FACTOR = 2   // multiplicative factor by which to grow 'd_size'
};

inline static
int nextSize(int size)
    // Return the specified 'size' multiplied by 'MY_GROW_FACTOR'.
{
    return size * MY_GROW_FACTOR;
}

inline static
void reallocate(char            ***array,
                int               *size,
                int                newSize,
                int                length,
                bslma_Allocator   *basicAllocator)
    // Reallocate memory in the specified 'array' using the specified
    // 'basicAllocator' and update the specified size to the specified
    // 'newSize'.  The specified 'length' number of leading elements are
    // preserved.  If 'new' should throw an exception, this function has no
    // effect.  The behavior is undefined unless 1 <= newSize, 0 <= length, and
    // newSize <= length.
{
    ASSERT(array);
    ASSERT(*array);
    ASSERT(size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(basicAllocator);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invariant

    char **tmp = *array;

    *array = (char **) basicAllocator->allocate(newSize * sizeof **array);
    // COMMIT
    memcpy(*array, tmp, length * sizeof **array);
    *size = newSize;
    basicAllocator->deallocate(tmp);
}

void my_CstrArray::increaseSize()
{
    reallocate(&d_array_p, &d_size, nextSize(d_size), d_length, d_allocator_p);
}

my_CstrArray::my_CstrArray(bslma_Allocator *basicAllocator)
: d_size(MY_INITIAL_SIZE)
, d_length(0)
, d_strPool(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    ASSERT(d_allocator_p)
    d_array_p = (char **) d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

my_CstrArray::~my_CstrArray()
{
    ASSERT(1 <= d_size);
    ASSERT(0 <= d_length);
    ASSERT(d_allocator_p);
    ASSERT(d_length <= d_size);
    d_allocator_p->deallocate(d_array_p);
}

my_CstrArray& my_CstrArray::operator=(const my_CstrArray& rhs)
{
    if (&rhs != this) {
        d_strPool.release();
        d_length = 0;
        if (rhs.d_size > d_size) {
            reallocate(&d_array_p, &d_size, rhs.d_size, d_length,
                       d_allocator_p);
        }
        for (d_length = 0; d_length < rhs.d_length; ++d_length) {
            int size = strlen(rhs.d_array_p[d_length]) + 1;
            char *elem = (char *) d_strPool.allocate(size);
            memcpy(elem, rhs.d_array_p[d_length], size);
            d_array_p[d_length] = elem;
        }
    }
    return *this;
}

void my_CstrArray::append(const char *item)
{
    if (d_length >= d_size) {
        this->increaseSize();
    }
    int sSize = strlen(item) + 1;
    char *elem = (char *) d_strPool.allocate(sSize);
    memcpy(elem, item, sSize * sizeof *item);
    d_array_p[d_length++] = elem;
}

ostream& operator<<(ostream& stream, const my_CstrArray& array)
{
    stream << "[ ";
    for (int i = 0; i < array.length(); ++i) {
        stream << '"' << array[i] << "\" ";
    }
    return stream << ']' << flush;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Create two 'my_CstrArray' objects.  Append different strings to
        //   both arrays and verify the contents of the arrays using
        //   'operator[]'.  Assign one array to another and verify the contents
        //   of the assigned array.  Allow both arrays to leave scope and
        //   verify that all memory is deallocated.
        //
        // Testing:
        //   Ensure usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "\nTesting 'my_CstrArray'." << endl;

        const char *DATA[] = { "A", "B", "C", "D", "E" };
        const int NUM_ELEM = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator ta(veryVeryVerbose);
        const bslma_TestAllocator& TA = ta;
        bslma_Allocator *const ALLOCATOR[] = { &ta, 0 };
        const int NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR;

        for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
            bslma_Allocator *a = ALLOCATOR[ai];
            my_CstrArray mX(a);    const my_CstrArray& X = mX;
            my_CstrArray mY(a);    const my_CstrArray& Y = mY;

            if (verbose) cout << "\t[Append data to 'X']" << endl;
            for (int xi = 0; xi < NUM_ELEM; ++xi) {
                mX.append(DATA[xi]);
                for (int j = 0; j < xi; ++j) {
                    LOOP3_ASSERT(ai, xi, j, 0 == strcmp(DATA[j], X[j]));
                }
            }
            if (veryVerbose) { TAB; P(X); TAB; P(Y); }

            if (verbose) cout << "\n\t[Append data to 'Y']" << endl;
            for (int yi = NUM_ELEM - 1; yi >= 3; --yi) {
                mY.append(DATA[yi]);
                for (int j = 0; j < NUM_ELEM - yi; ++j) {
            //--^
            LOOP3_ASSERT(ai, yi, j, 0 == strcmp(DATA[NUM_ELEM - j - 1], Y[j]));
            //--v
                }
            }
            if (veryVerbose) { TAB; P(X); TAB; P(Y); }

            if (verbose) cout << "\n\t[Assign 'X' to 'Y']" << endl;
            mY = X;
            if (veryVerbose) { TAB; P(X); TAB; P(Y); }

            for (int xj = 0; xj < NUM_ELEM; ++xj) {
                LOOP2_ASSERT(ai, xj, 0 == strcmp(DATA[xj], Y[xj]));
            }
        }
        ASSERT(0 == TA.numBytesInUse());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING deleteObject AND deleteObjectRaw
        //
        // Concerns:
        //   That 'deleteObject' and 'deleteObjectRaw' properly destroy and
        //   deallocate managed objects.
        //
        // Plan:
        //   Iterate where at the beginning of the loop, we create an object
        //   of type 'mostDerived' that multiply inherits from two types with
        //   virtual destructors.  Then in the middle of the loop we switch
        //   into several ways of destroying and deallocating the object with
        //   various forms of 'deleteObjectRaw' and 'deleteObject', after
        //   which we verify that the destructors have been run.  Each
        //   iteration we verify that the memory we got was the same as for
        //   the previous iteration, which shows that memory is being
        //   deallocated and recovered by the pool.
        //
        // Testing:
        //   deleteObjectRaw()
        //   deleteObject()
        // --------------------------------------------------------------------

        bool finished = false;
        Obj sp;
        for (int di = 0; !finished; ++di) {
            MostDerived *pMD = (MostDerived *) sp.allocate(sizeof(*pMD));
            const MostDerived *pMDC = pMD;
            new (pMD) MostDerived();

            ASSERT(1 == numLeftChildren);
            ASSERT(1 == numMiddleChildren);
            ASSERT(1 == numRightChildren);
            ASSERT(1 == numMostDerived);

            switch (di) {
              case 0: {
                sp.deleteObjectRaw(pMDC);
              } break;
              case 1: {
                const LeftChild *pLCC = pMDC;
                ASSERT((void*) pLCC == (void*) pMDC);
                sp.deleteObjectRaw(pLCC);
              } break;
              case 2: {
                sp.deleteObject(pMDC);
              } break;
              case 3: {
                const LeftChild *pLCC = pMDC;
                ASSERT((void*) pLCC == (void*) pMDC);
                sp.deleteObject(pLCC);
              } break;
              case 4: {
                const RightChild *pRCC = pMDC;
                ASSERT((void*) pRCC != (void*) pMDC);
                sp.deleteObject(pRCC);
              } break;
              case 5: {
                sp.deleteObjectRaw(pMDC);    // 2nd time we do this

                finished = true;
              } break;
              default: {
                ASSERT(0);
              }
            }

            LOOP_ASSERT(di, 0 == numLeftChildren);
            LOOP_ASSERT(di, 0 == numMiddleChildren);
            LOOP_ASSERT(di, 0 == numRightChildren);
            LOOP_ASSERT(di, 0 == numMostDerived);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST PROPER ALLOCATION SIZES
        //   Create a StrPool initialized with a sequential allocator.
        //   Allocate various number of strings.  After each allocation
        //   allocate one byte using the same sequential allocator and fill
        //   this byte with 'A'.  Fill the previously allocated strings with
        //   characters 'B'.  Verify the content of stand-alone allocated byte
        //   has not been changed.
        //
        // Testing:
        //   void alloactesreserveCapacity(numBytes);
        // --------------------------------------------------------------------
        bslma_TestAllocator ta(veryVeryVerbose);
        bdema_SequentialAllocator sa(&ta);
        bdema_StrPool pool(&sa);

        for (int i=1; i < 1024; ++i) {
            char *ptr = (char*) pool.allocate(i);
            char *nxt = (char*) sa.allocate(1);
            *nxt = 'A';
            bsl::memset(ptr, 'B', i);
            LOOP2_ASSERT(i, *nxt, *nxt == 'A');
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // RESERVECAPACITY TEST
        //   Create a pool initialized with a test allocator.  Verify
        //   'reserveCapacity' reserves sufficient memory when the requested
        //   'numBytes' both does and does not exceed the 'THRESHOLD'.
        //
        // Testing:
        //   void reserveCapacity(numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting 'reserveCapacity', size <= threshold."
                          << endl;
        {
            int poolSize = THRESHOLD;
            bslma_TestAllocator a(veryVeryVerbose); bslma_TestAllocator& A = a;
            bslma_TestAllocator& testAllocator = a;
            BEGIN_BSLMA_EXCEPTION_TEST {
                Obj mX(&a);
                mX.reserveCapacity(poolSize);

                // ensure reserveCapacity does not decrease capacity
                mX.reserveCapacity(1);

                int numAllocation = A.numAllocations();
                char *p0 = (char *) mX.allocate(1);
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p0); }
                // Ensure memory is allocated from the current block.
                ASSERT(A.numAllocations() == numAllocation);

                const int SIZE = THRESHOLD - 2;
                char *p1 = (char *) mX.allocate(SIZE);
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p1); }
                // Ensure memory is allocated from the current block.
                ASSERT(A.numAllocations() == numAllocation);
                ASSERT(1 == p1 - p0);

                ASSERT(poolSize - THRESHOLD - 2 < SIZE);
                char *p2 = (char *) mX.allocate(THRESHOLD + 1);
                ++numAllocation;
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p2); }
                // Ensure memory is allocated as an individual block.
                ASSERT(SIZE != p2 - p1);
                ASSERT(A.numAllocations() == numAllocation);

                char *p3 = (char *) mX.allocate(1);
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p3); }
                // Ensure memory is allocated from the current block.
                ASSERT(SIZE == p3 - p1);
                ASSERT(A.numAllocations() == numAllocation);
            } END_BSLMA_EXCEPTION_TEST
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'reserveCapacity', size > threshold."
                          << endl;
        {
            int poolSize = THRESHOLD + 3;
            bslma_TestAllocator a(veryVeryVerbose); bslma_TestAllocator& A = a;
            bslma_TestAllocator& testAllocator = a;
            BEGIN_BSLMA_EXCEPTION_TEST {
                Obj mX(&a);
                mX.reserveCapacity(poolSize);

                // ensure reserveCapacity does not decrease capacity
                mX.reserveCapacity(1);

                char *p0 = (char *) mX.allocate(1);
                int numAllocation = A.numAllocations();
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p0); }

                const int SIZE = THRESHOLD + 1;
                char *p1 = (char *) mX.allocate(SIZE);
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p1); }
                // Ensure memory is allocated from the current block.
                ASSERT(A.numAllocations() == numAllocation);
                ASSERT(1 == p1 - p0);

                ASSERT(poolSize - THRESHOLD - 2 < SIZE);
                char *p2 = (char *) mX.allocate(SIZE);
                ++numAllocation;
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p2); }
                // Ensure memory is allocated as an individual block.
                ASSERT(SIZE != p2 - p1);
                ASSERT(A.numAllocations() == numAllocation);
                ASSERT(A.lastAllocatedNumBytes() == blockSize(SIZE));

                char *p3 = (char *) mX.allocate(1);
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p3); }
                // Ensure memory is allocated from the current block.
                ASSERT(SIZE == p3 - p1);
                ASSERT(A.numAllocations() == numAllocation);
            } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // RELEASE TEST
        //   Create a pool initialized with a test allocator.  Request memory
        //   of varying sizes and then invoke the pool's 'release' method.
        //   Verify that the allocator indicates all memory has been
        //   deallocated.  Reuse the pool to allocate memory and verify that
        //   'allocate' works as expected.  Allow the pool to go out of scope
        //   and verify that all memory is released.
        //
        // Testing:
        //   ~bdema_StrPool();
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RELEASE TEST" << endl
                                  << "============" << endl;

        const int DATA[] = { 1, 5, 12, 24, 32, 1 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'release' and destructor." << endl;

        bslma_TestAllocator a(veryVeryVerbose);
        const bslma_TestAllocator& A = a;
        {
            Obj mX(&a);

            int poolSize = 128; // Enough to satisfy all requests in vector.
            poolSize = growPool(mX, poolSize);
            ASSERT(0 < A.numBytesInUse());

            if (verbose) cout << "\t[Release allocated memory]" << endl;

            mX.release();
            ASSERT(0 == A.numBytesInUse());

            if (verbose) cout << "\t[Reuse pool after 'release']" << endl;
            {
                int numAllocation = A.numAllocations();
                int totalSize = 0;
                char *p0 = 0;
                for (int di = 0; di < NUM_DATA; ++di) {
                    const int SIZE = DATA[di];
                    char *p = (char *) mX.allocate(SIZE);
                    if (0 == di) {
                        ++numAllocation;
                        int blkSize = blockSize(poolSize);
                        LOOP_ASSERT(di, A.lastAllocatedNumBytes() == blkSize);
                        LOOP_ASSERT(di, A.numAllocations() == numAllocation);
                    }
                    else {
                        int offset = p - p0;
                        const int EXP = DATA[di - 1];
                        if (veryVerbose) { TAB; P_(offset); TAB; P(EXP); }

                        // Ensure cursor is correctly offset.
                        LOOP_ASSERT(di , EXP == offset);
                    }
                    p0 = p;
                    totalSize += SIZE;
                }
                ASSERT(totalSize <= poolSize); // Ensure test data integrity.
                ASSERT(A.numAllocations() == numAllocation);

                // Request more than the remaining block and ensure a new block
                // is allocated.
                mX.allocate(poolSize - totalSize + 1);
                ++numAllocation;
                int blkSize = blockSize(poolSize * GROW_FACTOR);
                ASSERT(A.numAllocations() == numAllocation);
                ASSERT(A.lastAllocatedNumBytes() == blkSize);
            }
            ASSERT(0 < A.numBytesInUse());

            if (verbose) cout << "\t[Pool going out of scope (destructor)]"
                              << endl;
        }
        ASSERT(0 == A.numBytesInUse());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ALLOCATE TEST
        //   This test case verifies the following functionalities of the
        //   'allocate' method:
        //
        //   1) Ensure that 'allocate' correctly increments the internal cursor
        //   for varying request sizes - Create a pool initialized with a test
        //   allocator.  Grow the pool to a size that can accommodate all
        //   subsequent requests.  Request memory of varying sizes and verify
        //   that the difference between the returned memory addresses of two
        //   consecutive requests equals the size of the first request.
        //   Request memory of size less than or equal to 'THRESHOLD' but
        //   greater than that of the current pool's free memory and verify
        //   that a new memory block is requested from and recorded by the
        //   allocator.
        //
        //   2) Ensure that, for request sizes greater than 'THRESHOLD',
        //   'allocate' distributes memory from the current block if the
        //   block's capacity can satisfy the request, otherwise it requests
        //   individual memory blocks from its held allocator - Create a pool
        //   initialized with a test allocator.  Grow the pool to greater than
        //   or equal to 'THRESHOLD + 2' bytes.  Request 1 byte and then
        //   'THRESHOLD + 1' bytes.  Verify that 1) the memory addresses of the
        //   two requests are contiguous, and 2) no memory requests are
        //   recorded by the allocator.  Then deplete the current pool to less
        //   than or equal to 'THRESHOLD' bytes, and request 'THRESHOLD + 1'
        //   bytes and then 1 byte.  Verify that 1) the returned memory address
        //   for the 'THRESHOLD + 1'-byte request is *not* contiguous with
        //   previous memory addresses; 2) the size 'THRESHOLD + 1' is recorded
        //   by the allocator as the most recent memory request; and 3) the
        //   memory address of the 1-byte request *is* contiguous with requests
        //   prior to the 'THRESHOLD + 1'-byte request.
        //
        // Testing:
        //   void *allocate(numBytes);
        //   Ensure 'allocate' correctly modifies internal cursor.
        //   Ensure 'allocate' behaves correctly for sizes beyond 'THRESHOLD'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATE TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTesting 'allocate' w/ size > 0." << endl;
        {
            const int DATA[] = { 1, 5, 12, 24, 32, 1 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;
            bslma_TestAllocator& testAllocator = a;
            BEGIN_BSLMA_EXCEPTION_TEST {
                Obj mX(&a);

                int poolSize = 128; // Enough to satisfy all requests in vector
                poolSize = growPool(mX, poolSize);

                int numAllocation = A.numAllocations();
                int totalSize = 0;
                char *p0 = 0;
                for (int di = 0; di < NUM_DATA; ++di) {
                    const int SIZE = DATA[di];
                    char *p = (char *) mX.allocate(SIZE);
                    if (0 == di) {
                        ++numAllocation;
                        int blkSize = blockSize(poolSize);
                        LOOP_ASSERT(di, A.lastAllocatedNumBytes() == blkSize);
                        LOOP_ASSERT(di, A.numAllocations() == numAllocation);
                    }
                    else {
                        int offset = p - p0;
                        const int EXP = DATA[di - 1]; // last request size
                        if (veryVerbose) { TAB; P_(offset); TAB; P(EXP); }

                        // Ensure cursor is correctly offset.
                        LOOP_ASSERT(di , EXP == offset);
                    }
                    p0 = p;
                    totalSize += SIZE;
                }
                ASSERT(totalSize <= poolSize); // Ensure test data integrity.

                // Request more than the remaining block and ensure a new block
                // is allocated.
                mX.allocate(poolSize - totalSize + 1);
                ++numAllocation;
                int blkSize = blockSize(poolSize * GROW_FACTOR);
                ASSERT(A.numAllocations() == numAllocation);
                ASSERT(A.lastAllocatedNumBytes() == blkSize);
            } END_BSLMA_EXCEPTION_TEST
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'allocate' w/ size > threshold."
                          << endl;
        {
            int poolSize = THRESHOLD + 3;
            bslma_TestAllocator a(veryVeryVerbose); bslma_TestAllocator& A = a;
            bslma_TestAllocator& testAllocator = a;
            BEGIN_BSLMA_EXCEPTION_TEST {
                Obj mX(&a);
                poolSize = growPool(mX, poolSize);

                char *p0 = (char *) mX.allocate(1);
                int numAllocation = A.numAllocations();
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p0); }

                const int SIZE = THRESHOLD + 1;
                char *p1 = (char *) mX.allocate(SIZE);
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p1); }
                // Ensure memory is allocated from the current block.
                ASSERT(A.numAllocations() == numAllocation);
                ASSERT(1 == p1 - p0);

                ASSERT(poolSize - THRESHOLD - 2 < SIZE);
                char *p2 = (char *) mX.allocate(SIZE);
                ++numAllocation;
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p2); }
                // Ensure memory is allocated as an individual block.
                ASSERT(SIZE != p2 - p1);
                ASSERT(A.numAllocations() == numAllocation);
                ASSERT(A.lastAllocatedNumBytes() == blockSize(SIZE));

                char *p3 = (char *) mX.allocate(1);
                if (veryVerbose) { TAB; P_(numAllocation); TAB; A(p3); }
                // Ensure memory is allocated from the current block.
                ASSERT(SIZE == p3 - p1);
                ASSERT(A.numAllocations() == numAllocation);
            } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GROWPOOL TEST
        //   Iterate over a sequence of expected memory block sizes and perform
        //   independent tests.  For each size, create a pool initialized with
        //   a test allocator.  Invoke 'growPool' with the current pool and
        //   size.  Then request 1 byte from the resulting pool and verify that
        //   the expected memory block size is recorded.
        //
        // Testing:
        //   int growPool(pool, numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "GROWPOOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTesting 'growPool'." << endl;

        const int DATA[] = { 0, 1, INITIAL_SIZE, THRESHOLD, THRESHOLD + 1,
                             32, 64, 128, 500, 1000, 5000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;


        for (int di = 0; di < NUM_DATA; ++di) {
            bslma_TestAllocator a(veryVeryVerbose);
            const int SIZE = DATA[di];
            Obj mX(&a);
            int poolSize = growPool(mX, SIZE);
            if (veryVerbose) { TAB; P_(poolSize); TAB; P(SIZE); };
            LOOP_ASSERT(di, SIZE <= poolSize);

            int numBlocksBefore = a.numBlocksInUse ();
            mX.allocate(1);
            int numBlocksAfter = a.numBlocksInUse ();
            ASSERT (numBlocksAfter == numBlocksBefore + 1);

            const int EXP = a.lastAllocatedNumBytes();
            const int blkSize = blockSize(poolSize);
            if (veryVerbose) { TAB; P_(blkSize); TAB; P(EXP); }
            LOOP_ASSERT(di, EXP == blkSize);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC ALLOCATE TEST
        //   This test case verifies basic functionalities of the 'allocate'
        //   method so that they can be bootstrapped for further detailed
        //   testing of 'allocate':
        //
        //   1) Ensure that 'allocate(0)' returns 0 and does not alter the
        //   internal cursor - Create a pool initialized with a test allocator.
        //   Invoke 'allocate' with size 0 and verify that 0 is returned.  Then
        //   Invoke 'allocate' with sizes 1, 0, and 1, consecutively.  Verify
        //   that the difference between the returned memory addresses of the
        //   first and third allocation is 1.  This verification indicates that
        //   the pool's internal cursor was not changed by the second (size 0)
        //   allocation.  Note that the initial size of the memory block is
        //   assumed to be at least 2 bytes.
        //
        //   2) Ensure that 'allocate' correctly increases the memory block
        //   size and does not increase it beyond 'MAX_SIZE' - Create a pool
        //   initialized with a test allocator.  Request 1 byte to induce the
        //   allocation of a new memory block, and then allocate the remaining
        //   block to deplete the current pool.  Verify that the allocator
        //   records the expected memory block size request.  Repeat the above
        //   steps to verify allocations of memory blocks of increasing sizes.
        //   When the expected memory block size exceeds 'MAX_SIZE', verify
        //   that the size recorded by the allocator remains to be the most
        //   recent size that is less than or equal to 'MAX_SIZE'.
        //
        // Testing:
        //   void *allocate(numBytes);
        //   Ensure 'allocate' behaves as expected for requests of size 0.
        //   Ensure 'allocate' correctly increases internal memory block size.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC ALLOCATE TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "\nTesting default constructor and destructor."
                          << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;
            {
                Obj mX(&a);
            }
            ASSERT(0 == A.numAllocations());
            ASSERT(0 == A.numDeallocations());
        }

        if (verbose) cout << "\nTesting 'allocate' w/ size 0." << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;
            bslma_TestAllocator& testAllocator = a;
            BEGIN_BSLMA_EXCEPTION_TEST {
                Obj mX(&a);

                // Ensure that allocator's 'allocate' is never called.
                int numAllocation = A.numAllocations();
                ASSERT(0 == mX.allocate(0));
                ASSERT(A.numAllocations() == numAllocation);

                char *p1 = (char *) mX.allocate(1);
                ++numAllocation;
                ASSERT(A.numAllocations() == numAllocation);

                mX.allocate(0);
                ASSERT(A.numAllocations() == numAllocation);

                // Ensure the pool's cursor is not changed after 'allocate(0)'.
                char *p2 = (char *) mX.allocate(1);  ASSERT(1 == p2 - p1);

                // Pool releases all memory on destruction here.
            } END_BSLMA_EXCEPTION_TEST
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'allocate' to grow block size."
                          << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;
            bslma_TestAllocator& testAllocator = a;
            BEGIN_BSLMA_EXCEPTION_TEST {
                Obj mX(&a);

                int size = INITIAL_SIZE;
                while (size <= MAX_SIZE) {
                    mX.allocate(size);
                    const int EXP = blockSize(size);
                    const int numBytes = A.lastAllocatedNumBytes();
                    if (veryVerbose) {
                        TAB; P_(size); TAB; P_(numBytes); TAB; P(EXP);
                    }
                    LOOP_ASSERT(size, EXP == numBytes);

                    size *= GROW_FACTOR;       // grow pool
                }

                // Ensure block size does not grow after exceeding 'MAX_SIZE'.
                for (int i = 0; i < 3; ++i) {
                    mX.allocate(size);
                    const int EXP = blockSize(size);
                    const int numBytes = A.lastAllocatedNumBytes();
                    if (veryVerbose) {
                        TAB; P_(size); TAB; P_(numBytes); TAB; P(EXP);
                    }
                    LOOP_ASSERT(i, EXP == numBytes);
                }
            } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FILE-STATIC FUNCTION TEST
        //   Create a 'bdema_InfrequentDeleteBlockList' object initialized with
        //   a test allocator.  Invoke both the 'blockSize' function and the
        //   'bdema_InfrequentDeleteBlockList::allocate' method with varying
        //   memory sizes, and verify that the sizes returned by 'blockSize'
        //   are equal to the memory request sizes recorded by the allocator.
        //
        // Testing:
        //   int blockSize(numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "FILE-STATIC FUNCTION TEST" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\nTesting 'blockSize'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 64, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator a(veryVeryVerbose);
        bdema_InfrequentDeleteBlockList bl(&a);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            int blkSize = blockSize(SIZE);
            bl.allocate(SIZE);

            const int EXP = a.lastAllocatedNumBytes();
            if (veryVerbose) { TAB; P_(SIZE); TAB; P_(blkSize); TAB; P(EXP); }
            LOOP_ASSERT(i, EXP == blkSize);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
