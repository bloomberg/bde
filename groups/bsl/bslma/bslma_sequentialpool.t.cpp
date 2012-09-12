// bslma_sequentialpool.t.cpp                                         -*-C++-*-

#ifndef BDE_OSS_TEST

#include <bslma_sequentialpool.h>

#include <bslma_allocator.h>                    // for testing only
#include <bslma_bufferallocator.h>              // for testing only
#include <bslma_infrequentdeleteblocklist.h>    // for testing only
#include <bslma_testallocator.h>                // for testing only

#include <bsls_alignedbuffer.h>     // for testing only
#include <bsls_alignmentutil.h>     // for testing only
#include <bsls_types.h>      // for testing only

#include <climits>
#include <cstdlib>                  // atoi()
#include <cstring>                  // memset(), memcpy()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this 'bslma::SequentialPool' test suite are the following:
// 1) Verify that 'allocate' returns memory of the correct size and alignment
// as specified by the 'strategy' parameter at construction.
//
// 2) Verify that the internal buffer capacity grows as specified by the
// 'initialSize' parameter at construction.
//
// 3) Verify that 'release' and the destructor frees all memory allocated
// through the pool.  And resets the pool to its initial state.
//
// 4) Verity that the optional buffer is used before the internal block list
// when the optional buffer is supplied.
//
// 5) Verity that the initial size parameter is being used to allocate the
// correct amount of memory.
//
// The corresponding testing procedures are implemented as follows:
// 1) Initialize a sequential pool with varying alignment strategies, and then
// invoke 'allocate' with varying sizes.  Also invoke buffer allocator's
// 'allocateFromBuffer' class method with the same allocation sizes.  Verify
// that the offset of the pool's returned memory address respective to the
// beginning of the pool's internal buffer is the same as that for the memory
// address returned by 'allocateFromBuffer'.  Note that this test relies on the
// implementation detail of the 'allocate' method, which uses
// 'allocateFromBuffer' to allocate memory from the pool's internal buffer.
//
// 2) Initialize a sequential pool with varying buffer sizes and a test
// allocator.  Invoke 'allocate' repeatedly to deplete the internal buffer and
// cause replenishment.  Verify that the sequential pool allocates the expected
// buffer size from the test allocator.
//
// 3) Initialize two sequential pools, each supplied with its own test
// allocator.  Invoke 'allocate' repeatedly on both sequential pools.  Invoke
// 'release' on one sequential pool, and allow the other to go out of scope.
// Verify that both test allocators indicate no memory is in use.
//
// 4) Initialize a number of sequential pools with an optional buffer.  Invoke
// 'allocate' repeatedly, ensuring that after each allocate, the correct
// amount of memory is returned from the optional buffer before allocation
// memory from the internal block list.
//
// 5) Initialize a number of sequential pools with different initial capacity.
// Invoke 'allocate' on all sequential pools, verifying that each call to
// allocate returns a valid usable amount of memory.
//
//-----------------------------------------------------------------------------
// [ 4] bslma::SequentialPool(allocator);
// [ 4] bslma::SequentialPool(strategy, allocator);
// [ 5] bslma::SequentialPool(initialSize, allocator);
// [ 5] bslma::SequentialPool(initialSize, strategy, allocator);
// [ 4] bslma::SequentialPool(buffer, bufSize, allocator);
// [ 4] bslma::SequentialPool(buffer, bufSize, strategy, allocator);
// [ 5] bslma::SequentialPool(initialSize, maxSize, allocator);
// [ 5] bslma::SequentialPool(initialSize, maxSize, strategy, alloc);
// [ 4] bslma::SequentialPool(buffer, bufSize, maxSize, allocator);
// [ 4] bslma::SequentialPool(buffer, bufSize, maxSize, strategy, ta);
// [ 4] ~bslma::SequentialPool();
// [ ] void *allocate(size);
// [ ] void release();
//-----------------------------------------------------------------------------
// [6] USAGE TEST
// [5] Initial Size Test
// [4] Optional Buffer Test
// [2] Ensure internal buffer grows as specified (goal 2)
// [1] int blockSize(numBytes);
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

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << '\t' << #L << ": " << L << "\n"; \
              aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
                                              // Print identifier and pointer.
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define A(X) cout << #X " = " << (void *) (X) << endl; // Print address
#define A_(X) cout << #X " = " << (void *) (X) << ", " << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslma::SequentialPool              Obj;
typedef bslma::TestAllocator               TestAllocator;
typedef bslma::BufferAllocator             BufferAllocator;
typedef BufferAllocator::AlignmentStrategy AlignStrategy;

// This type is copied from the 'bslma_infrequentdeleteblocklist.h' for testing
// purposes.

struct InfrequentDeleteBlock {
    InfrequentDeleteBlock               *d_next_p;
    bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

// This is copied from 'bslma_sequentialpool.cpp' to help determine the
// internal limits of the pool.

enum {
    INITIAL_SIZE = 256,   // default initial buffer size
    GROW_FACTOR  = 2      // multiplicative factor by which to grow 'd_bufSize'
};

//=============================================================================
//                        FILE-STATIC HELPER FUNCTIONS
//-----------------------------------------------------------------------------
static int blockSize(int numBytes)
    // Return the adjusted block size based on the specified 'numBytes' using
    // the calculation performed by the
    // 'bslma::InfrequentDeleteBlockList::allocate' method.
{
    ASSERT(0 <= numBytes);

    if (numBytes) {
        numBytes += sizeof(InfrequentDeleteBlock) - 1;
        numBytes &= ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
    }

    return numBytes;
}

static int calculateNextSize(int currSize, int size, int maxSize = INT_MAX)
{
    ASSERT(0 <= currSize);
    ASSERT(0 <= size);

    if (0 == currSize) {
        return INITIAL_SIZE;
    }

    do {
        currSize *= 2;
    } while (currSize < size);

    return currSize < maxSize ? currSize : maxSize;
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_list.h

class my_List {
    char                   *d_typeArray_p;
    void                  **d_list_p;
    int                     d_length;
    int                     d_size;
    bslma::SequentialPool   d_pool;
    bslma::Allocator       *d_allocator_p;

  private: // not implemented
    my_List(const my_List& original);

  private:
    void increaseSize();

  public :
    enum Type { INT, DOUBLE, INT64 };

    my_List(bslma::Allocator *basicAllocator);
    my_List(char * buffer, int bufferSize, bslma::Allocator *basicAllocator);

    ~my_List();
    my_List& operator=(const my_List& rhs);
    void append(int value);
    void append(double value);
    void append(bsls::Types::Int64 value);
    void removeAll();

    const int *theInt(int index) const;
    const double *theDouble(int index) const;
    const bsls::Types::Int64 *theInt64(int index) const;
    Type type(int index) const;
    int length() const;
};

inline
void my_List::removeAll()
{
    d_pool.release();
    d_length = 0;
}

inline
const int *my_List::theInt(int index) const
{
    return (int *) d_list_p[index];
}

inline
const double *my_List::theDouble(int index) const
{
    return (double *) d_list_p[index];
}

inline
const bsls::Types::Int64 *my_List::theInt64(int index) const
{
    return (bsls::Types::Int64 *) d_list_p[index];
}

inline
my_List::Type my_List::type(int index) const
{
    return (Type) d_typeArray_p[index];
}

inline
int my_List::length() const
{
    return d_length;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_list.cpp

enum { MY_INITIAL_SIZE = 1, MY_GROW_FACTOR = 2 };

static
void copyElement(void **list, my_List::Type type, int index, void *srcElement,
                 bslma::SequentialPool *pool)
    // Copy the value of the specified 'srcElement' of the specified 'type' to
    // the specified 'index' position in the specified 'list'.  Use the
    // specified 'pool' to supply memory.
{
    ASSERT(list);
    ASSERT(0 <= index);
    ASSERT(srcElement);
    ASSERT(pool);

    typedef bsls::Types::Int64 Int64;

    switch (type) {
      case my_List::INT:
        list[index] = new(pool->allocate(sizeof(int)))
                      int(*((int *) srcElement));
        break;
      case my_List::DOUBLE:
        list[index] = new(pool->allocate(sizeof(double)))
                      double(*((double *) srcElement));
        break;
      case my_List::INT64:
        list[index] = new(pool->allocate(sizeof(Int64)))
                      Int64(*((Int64 *) srcElement));
        break;
      default:
        ASSERT(0 && "ERROR (my_List): Invalid element type.");
    }
}

static
void reallocate(void ***list, char **typeArray, int *size,
                int newSize, int length, bslma::Allocator *basicAllocator)
    // Reallocate memory in the specified 'list' and 'typeArray' using the
    // specified 'basicAllocator' and update the specified size to the
    // specified 'newSize'.  The specified 'length' number of leading
    // elements are preserved in 'list' and 'typeArray'.  If 'allocate'
    // should throw an exception, this function has no effect.  The
    // behavior is undefined unless 1 <= newSize, 0 <= length, and
    // newSize <= length.
{
    ASSERT(list);
    ASSERT(*list);
    ASSERT(typeArray);
    ASSERT(*typeArray);
    ASSERT(size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invariant
    ASSERT(basicAllocator);

    void **newList =
        (void **) basicAllocator->allocate(newSize * sizeof *newList);
    char *newTypeArray =
        (char *) basicAllocator->allocate(newSize * sizeof *newTypeArray);
    memcpy(newList, *list, length * sizeof **list);
    memcpy(newTypeArray, *typeArray, length * sizeof **typeArray);
    basicAllocator->deallocate(*list);
    basicAllocator->deallocate(*typeArray);
    *list = newList;
    *typeArray = newTypeArray;
    *size = newSize;
}

void my_List::increaseSize()
{
     int newSize = d_size * MY_GROW_FACTOR;
     reallocate(&d_list_p, &d_typeArray_p, &d_size, newSize,
                d_length, d_allocator_p);
}

my_List::my_List(char *buffer,int bufferSize, bslma::Allocator *basicAllocator)
: d_length()
, d_size(MY_INITIAL_SIZE)
, d_pool(buffer, bufferSize, basicAllocator)
, d_allocator_p(basicAllocator)
{
    ASSERT(d_allocator_p);

    d_typeArray_p =
        (char *) d_allocator_p->allocate(d_size * sizeof *d_typeArray_p);
    d_list_p =
        (void **) d_allocator_p->allocate(d_size * sizeof *d_list_p);
}

my_List::my_List(bslma::Allocator *basicAllocator)
: d_length(0)
, d_size(MY_INITIAL_SIZE)
, d_pool(basicAllocator)
, d_allocator_p(basicAllocator)
{
    ASSERT(d_allocator_p);

    d_typeArray_p =
        (char *) d_allocator_p->allocate(d_size * sizeof *d_typeArray_p);
    d_list_p =
        (void **) d_allocator_p->allocate(d_size * sizeof *d_list_p);
}

my_List::~my_List()
{
    ASSERT(d_typeArray_p);
    ASSERT(d_list_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length);  ASSERT(d_length <= d_size);
    ASSERT(d_allocator_p);

    d_allocator_p->deallocate(d_typeArray_p);
    d_allocator_p->deallocate(d_list_p);
}

my_List& my_List::operator=(const my_List& rhs)
{
    if (&rhs != this) {
        // not aliased

        d_pool.release();
        d_length = 0;

        int newLength = rhs.d_length;
        if (newLength > d_size) {
            reallocate(&d_list_p, &d_typeArray_p, &d_size,
                       newLength, d_length, d_allocator_p);
        }
        for (int i = 0; i < newLength; ++i) {
            d_typeArray_p[i] = rhs.d_typeArray_p[i];
            copyElement(d_list_p, (Type) d_typeArray_p[i], i,
                        rhs.d_list_p[i], &d_pool);
        }
        d_length = newLength;
    }
    return *this;
}

void my_List::append(int value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    int *item = (int *) d_pool.allocate(sizeof *item);
    *item = value;
    d_typeArray_p[d_length] = (char) my_List::INT;
    d_list_p[d_length++] = item;
}

void my_List::append(double value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    double *item = (double *) d_pool.allocate(sizeof *item);
    *item = value;
    d_typeArray_p[d_length] = (char) my_List::DOUBLE;
    d_list_p[d_length++] = item;
}

void my_List::append(bsls::Types::Int64 value)
{
    typedef bsls::Types::Int64 Int64;

    if (d_length >= d_size) {
        increaseSize();
    }
    Int64 *item = (Int64 *) d_pool.allocate(sizeof *item);
    *item = value;
    d_typeArray_p[d_length] = (char) my_List::INT64;
    d_list_p[d_length++] = item;
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
      case 6: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            char buf[64];
            char *buf_p = buf;
            my_List mX(buf_p, 64, &ta);    const my_List &X = mX;
            ASSERT(0 == X.length());

            const bsls::Types::Int64 K = 1;
            mX.append(K);
            if (veryVerbose) { T_ P(*X.theInt64(0)); }
            ASSERT(1 ==  X.length());
            ASSERT(K == *X.theInt64(0));  ASSERT(my_List::INT64 == X.type(0));

            const int I = 5;
            mX.append(I);
            if (veryVerbose) { T_ P(*X.theInt(1)); }
            ASSERT(2 ==  X.length());
            ASSERT(K == *X.theInt64(0));  ASSERT(my_List::INT64 == X.type(0));
            ASSERT(I == *X.theInt(1));    ASSERT(my_List::INT   == X.type(1));

            const double D = 2.5;
            mX.append(D);
            if (veryVerbose) { T_ P(*X.theDouble(2)); }
            ASSERT(3 ==  X.length());
            ASSERT(K == *X.theInt64(0));  ASSERT(my_List::INT64  == X.type(0));
            ASSERT(I == *X.theInt(1));    ASSERT(my_List::INT    == X.type(1));
            ASSERT(D == *X.theDouble(2)); ASSERT(my_List::DOUBLE == X.type(2));

            my_List mY(&ta);    const my_List& Y = mY;
            mY.append(D);
            mY.append(K);

            mY = X;
            ASSERT(Y.length() == X.length());
            ASSERT(*Y.theInt64(0) == *X.theInt64(0));
            ASSERT(*Y.theInt(1) == *X.theInt(1));
            ASSERT(*Y.theDouble(2) == *X.theDouble(2));
            if (veryVerbose) {
                T_ P(*Y.theInt64(0));
                T_ P(*Y.theInt(1));
                T_ P(*Y.theDouble(2));
            };

            mX.removeAll();
            ASSERT(0 == X.length());
            ASSERT(3 == Y.length());

            mY.removeAll();
            ASSERT(0 == Y.length());
        }
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // INITIAL SIZE TEST
        //
        // Concerns:
        //   a) Invalid initial size test.
        //      The parameter initial size determines the buffer allocation
        //      strategy.  Positive means replenish with the same size,
        //      negative means replenish with a geometrically larger buffer.
        //      Note that this is not implemented presently, but non-positive
        //      values should certainly not cause a hang or crash.
        //
        // Plan:
        //   a) Initialize sequentialPool with a variety of values as the
        //      initial value, including positive, negative, and 0.  Confirm
        //      that allocate returns a usable value.
        //
        // Testing:
        //   explicit bslma::SequentialPool(
        //      int                                        initialSize,
        //      bslma::Allocator                          *basicAllocator = 0);
        //   bslma::SequentialPool(
        //      int                                        initialSize,
        //      bslma::BufferAllocator::AlignmentStrategy  strategy,
        //      bslma::Allocator                          *basicAllocator = 0);
        //   bslma::SequentialPool(
        //      int                                        initialSize,
        //      int                                        maxBufferSize,
        //      bslma::Allocator                          *basicAllocator = 0);
        //   bslma::SequentialPool(
        //      int                                        initialSize,
        //      int                                        maxBufferSize,
        //      bslma::BufferAllocator::AlignmentStrategy  strategy,
        //      bslma::Allocator                          *basicAllocator = 0);
        //   void *allocate(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "INITIAL SIZE TEST" << endl
                                  << "=================" << endl;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            // Test constructors that used initial size.

            if (veryVerbose) {
               cout << "a.  Initial Size Test." << endl;
            }
            struct Block {
                Block                               *d_next_p;
                bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force
                                                                // alignment
            };

            // Block copied from bslma_infrequentdeleteblocklist.h.  This is
            // the amount of additional memory used per allocation.

            bslma::BufferAllocator::AlignmentStrategy strategy =
                                     bslma::BufferAllocator::NATURAL_ALIGNMENT;
            char *cBuffer;

            const int NA = 32, NB = 1, NC = 0, ND = -1, NE = -32, NF = -128;
            const int MA = 64, MB = 8;
            //   a) Invalid initial size test.

            int sizeA(NA);
            bslma::SequentialPool mX(sizeA, strategy, &ta);
            cBuffer = (char *)mX.allocate(MA);
            ASSERT(blockSize(NA) + blockSize(MA) == ta.numBytesInUse());
            mX.release();

            bslma::SequentialPool mY(sizeA, sizeA * 2, strategy, &ta);
            cBuffer = (char *)mY.allocate(MA);
            ASSERT(blockSize(NA) + blockSize(MA) == ta.numBytesInUse());
            mY.release();

            for (int i = 1;
                 i < bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT + 1;
                 ++i) {
                bslma::TestAllocator ta2(veryVeryVerbose);
                int sizeB(NB);
                bslma::SequentialPool mA(sizeB, strategy, &ta2);
                cBuffer = (char *)mA.allocate(i);
                ASSERT(ta2.numBytesInUse() == 1 == i
                                           ?  blockSize(i)
                                           :  2 * blockSize(i));
                mA.release();

                bslma::SequentialPool mB(sizeB, sizeB * 2, strategy, &ta2);
                cBuffer = (char *)mB.allocate(i);
                ASSERT(ta2.numBytesInUse() == 1 == i
                                           ?  blockSize(i)
                                           :  2 * blockSize(i));
                mA.release();
            }

            int sizeC(NC);
            bslma::SequentialPool mA(sizeC, strategy, &ta);
            cBuffer = (char *)mA.allocate(MB);
            ASSERT(blockSize(INITIAL_SIZE) == ta.numBytesInUse());
            mA.release();

            bslma::SequentialPool mB(sizeC, 1, strategy, &ta);
            cBuffer = (char *)mB.allocate(MB);
            ASSERT(blockSize(INITIAL_SIZE) == ta.numBytesInUse());
            mB.release();

            int sizeD(ND);
            bslma::SequentialPool mC(sizeD, strategy, &ta);
            cBuffer = (char *)mC.allocate(MB);
            ASSERT(blockSize(-ND) + blockSize(MB) == ta.numBytesInUse());
            mC.release();

            bslma::SequentialPool mD(sizeD, 2, strategy, &ta);
            cBuffer = (char *)mD.allocate(MB);
            ASSERT(blockSize(std::abs(ND)) + blockSize(MB)
                                                        == ta.numBytesInUse());
            mD.release();

            int sizeE(NE);
            bslma::SequentialPool mE(sizeE, strategy, &ta);
            cBuffer = (char *)mE.allocate(MA);
            ASSERT(blockSize(-NE) + blockSize(MA) == ta.numBytesInUse());
            mE.release();

            bslma::SequentialPool mF(sizeE, std::abs(NE) * 4, strategy, &ta);
            cBuffer = (char *)mF.allocate(MA);
            ASSERT(blockSize(-NE) + blockSize(MA) == ta.numBytesInUse());
            mF.release();

            int sizeF(NF);
            bslma::SequentialPool mG(sizeF, strategy, &ta);
            cBuffer = (char *)mG.allocate(MA);

            ASSERT(blockSize(-NF) == ta.numBytesInUse());
            // Is greater than threshold in current implementation, and without
            // buffer, so allocates a separate memory block.

            mG.release();
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // OPTIONAL BUFFER TEST
        //
        // Concerns:
        //   a) Static buffer limits test.
        //      A bslma_sequentialallocator object allows an optional buffer to
        //      be supplied on construction.  This buffer is used for
        //      allocations until it runs out of memory.  Further allocations
        //      use the internal block list.
        //
        //   b) Optional buffer constructor test.
        //      The optional buffer is supplied on construction.  Behavior
        //      regarding the optional buffer is the same for all constructors.
        //      The buffer should be used before the internal block list.  When
        //      the buffer is unable to supply the requested amount of memory,
        //      then internal block list should provide the memory.
        //
        //   c) Static buffer test with alignment considerations.
        //      Due to the need to return aligned memory, a user supplied
        //      optional buffer may have additional memory (enough to fulfill
        //      the request) but unable to do so.  In these cases, the rest of
        //      the memory in the static buffer should be discarded and new
        //      memory supplied from the internal block list.
        //
        //   d) Large allocation with static buffer test.
        //      Even if the user supplies a static buffer, if the requested
        //      memory exceeds the amount of free memory in the static buffer,
        //      memory should be supplied from the internal block list.
        //      Additional free memory in the buffer is no longer used.
        //
        //   e) Empty static buffer test.
        //      If the user supplies an empty buffer, all memory should be
        //      supplied from the internal block list.
        //
        //   f) Don't deallocate static memory test.
        //      The static buffer is held by the class, not owned.  When the
        //      bslma_sequentialpool is destroyed, the static buffer should
        //      not be destroyed.
        //
        // Plan:
        //   a) Static buffer limits test.
        //      Supply a static buffer with a constant amount of memory (64
        //      bytes).  Any memory requests that do not exceed 64 bytes should
        //      be allocated from the static buffer.  All values near to 64 are
        //      tested.  Memory allocations less than or equal to 64 bytes
        //      will not cause the internal block list to supply memory.  All
        //      values above 64 will.
        //
        //   b) Optional buffer constructor test.
        //      All constructors involving the optional buffer are used to
        //      create a bslma_sequentialpool.  64 bytes are allocated, and
        //      will be supplied from the static buffer.  An additional byte is
        //      requested, and should be supplied from the internal block list.
        //
        //   c) Static buffer test with alignment considerations.
        //      A static buffer is supplied.  Memory is requested such that
        //      there is still free memory on the list, but when memory is
        //      aligned correctly there is not enough memory to supply the
        //      requested amount of memory.  Thus, this allocation should cause
        //      memory to be supplied from the internal block list.
        //
        //   d) Large allocation with static buffer test.
        //      A allocation is requested that is larger than the static buffer
        //      size.  This allocation should request memory from the internal
        //      block list.
        //
        //   e) Empty static buffer test.
        //      A valid buffer is supplied, but the buffer size is set to 0.
        //      Any requests for memory should be have memory supplied from the
        //      internal block list.
        //
        //   f) Don't deallocate static memory test.
        //      The static buffer supplied is actually dynamically allocated
        //      memory from the same allocator that supplies memory to the
        //      internal block list.  Destroy the sequentialpool, and verify
        //      that the memory for the static buffer still exists.
        //
        // Testing:
        // explicit bslma::SequentialPool(
        //      bslma::Allocator                          *basicAllocator = 0);
        // explicit bslma::SequentialPool(
        //      bslma::BufferAllocator::AlignmentStrategy  strategy,
        //      bslma::Allocator                          *basicAlloc = 0);
        // bslma::SequentialPool(
        //      char                                      *buffer,
        //      int                                        bufferSize,
        //      bslma::Allocator                          *basicAllocator = 0);
        // bslma::SequentialPool(
        //      char                                      *buffer,
        //      int                                        bufferSize,
        //      bslma::BufferAllocator::AlignmentStrategy  strategy,
        //      bslma::Allocator                          *basicAllocator = 0);
        // bslma::SequentialPool(
        //      char                                      *buffer,
        //      int                                        bufferSize,
        //      int                                        maxBufferSize,
        //      bslma::Allocator                          *basicAllocator = 0);
        // bslma::SequentialPool(
        //      char                                      *buffer,
        //      int                                        bufferSize,
        //      int                                        maxBufferSize,
        //      bslma::BufferAllocator::AlignmentStrategy  strategy,
        //      bslma::Allocator                          *basicAllocator = 0);
        // ~bslma::SequentialPool();
        // void *allocate(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "OPTIONAL BUFFER TEST" << endl
                                  << "====================" << endl;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                MAJOR_BUFFER_SIZE      = 1024,
                STATIC_BUFFER_SIZE     = 64
            };

            // Copied from bslma_infrequentdeleteblocklist.h/cpp

            struct Block {
                Block                               *d_next_p;
                bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force
                                                                // alignment
            };
            const int EXTRA_MEM_USED =
                       sizeof(Block) - bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
            // Block copied from bslma_infrequentdeleteblocklist.h.  This is
            // the amount of additional memory used per allocation.

            static union {
                char majorBuffer[MAJOR_BUFFER_SIZE];
                bsls::AlignmentUtil::MaxAlignedType  dummy;
            };
            // Align 'buffer' to MaxAlignedType.

            char *                             buffer;
            int                                bufferSize = STATIC_BUFFER_SIZE;
            char *                             cBuffer;
            int                                initialSize(32);
            bslma::BufferAllocator::AlignmentStrategy strategy =
                                     bslma::BufferAllocator::NATURAL_ALIGNMENT;

            buffer = majorBuffer + 128;
            // Move the buffer to the middle.

            {
                // Concern a) Static buffer limits test.
                // Test the limits of the static buffer.

                if (veryVerbose) {
                   cout << "a.  Static buffer limits test." << endl;
                }
                {
                    // Allocating 63 bytes, 1 byte under the buffer size.  This
                    // allocation should happen on the static buffer.

                    bslma::SequentialPool mX(buffer, bufferSize,
                                             strategy, &ta);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer =  (char *)mX.allocate(63);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());
                    mX.release();

                    bslma::SequentialPool mY(buffer, bufferSize, 32,
                                             strategy, &ta);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer =  (char *)mY.allocate(63);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());
                    mY.release();
                }
                {
                    // Allocating 64 bytes, exactly the limit.  This allocation
                    // should happen on the static buffer because the buffer is
                    // aligned to 64 bytes exactly.

                    bslma::SequentialPool mX(buffer, bufferSize,
                                             strategy, &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer =  (char *)mX.allocate(64);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());
                    mX.release();

                    bslma::SequentialPool mY(buffer, bufferSize, 32,
                                             strategy, &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer =  (char *)mY.allocate(64);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());
                    mY.release();
                }
                {
                    // Allocating 65 bytes, 1 byte over the limit.  This
                    // allocation should happen through the allocator.

                    bslma::SequentialPool mX(buffer, bufferSize,
                                             strategy, &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    int numBytes = 65;
                    cBuffer = (char *)mX.allocate(numBytes);
                    ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                    ASSERT(blockSize(numBytes)  == ta.numBytesInUse());
                    mX.release();

                    bslma::SequentialPool mY(buffer, bufferSize, 32,
                                             strategy, &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(numBytes);
                    ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                    ASSERT(blockSize(numBytes)  == ta.numBytesInUse());
                    mY.release();
                }
                {
                    // Allocating 66 bytes, 2 bytes over the limit.  This
                    // allocation should happen through the allocator.

                    bslma::SequentialPool mX(buffer, bufferSize,
                                             strategy, &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    int numBytes = 66;
                    cBuffer = (char *)mX.allocate(numBytes);
                    ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                    ASSERT(blockSize(numBytes)  == ta.numBytesInUse());
                    mX.release();

                    bslma::SequentialPool mY(buffer, bufferSize,
                                             strategy, &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(numBytes);
                    ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                    ASSERT(blockSize(numBytes)  == ta.numBytesInUse());
                    mY.release();
                }
                {
                    // Allocating 63 bytes, then 1 byte, another 1 bytes, and
                    // another 1 byte.  First two allocations should happen on
                    // the static buffer, the next allocation should use the
                    // allocator, and the last allocation should use the buffer
                    // allocated in the previous allocation.  Thus the last
                    // allocation should not increase the number of bytes used
                    // by the allocator.

                    bslma::SequentialPool mX(buffer, bufferSize,
                                             strategy, &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)mX.allocate(63);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer = (char *)mX.allocate(1);
                    ASSERT(cBuffer == buffer + 63);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer = (char *)mX.allocate(1);
                    ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                    ASSERT(blockSize(bufferSize) == ta.numBytesInUse());

                    int   bytesUsed  = ta.numBytesInUse();
                    char *tBuffer    = cBuffer;

                    cBuffer        = (char *)mX.allocate(1);
                    ASSERT((cBuffer   <= buffer) || (cBuffer  >= buffer + 64));
                    ASSERT(cBuffer    == tBuffer + 1);
                    ASSERT(bytesUsed  == ta.numBytesInUse());
                    mX.release();

                    bslma::SequentialPool mY(buffer, bufferSize,
                                             strategy, &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(63);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(1);
                    ASSERT(cBuffer == buffer + 63);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(1);
                    ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                    ASSERT(blockSize(bufferSize) == ta.numBytesInUse());

                    bytesUsed  = ta.numBytesInUse();
                    tBuffer    = cBuffer;

                    cBuffer        = (char *)mY.allocate(1);
                    ASSERT((cBuffer   <= buffer) || (cBuffer  >= buffer + 64));
                    ASSERT(cBuffer    == tBuffer + 1);
                    ASSERT(bytesUsed  == ta.numBytesInUse());
                    mY.release();
                }

                ASSERT(0 == ta.numBytesInUse());
            }
            {
                // Concern b) Optional buffer constructor test.
                // Test all constructors that take a optional static buffer.
                // These tests all allocate the limit on a static buffer, then
                // allocate one more byte, which should use the provided
                // allocator.  The static buffer is 64 bytes.

                if (veryVerbose) {
                   cout << "b.  Optional buffer constructor test." << endl;
                }
                {
                    bslma::SequentialPool mX(buffer,
                                             bufferSize,
                                             &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)mX.allocate(64);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer = (char *)mX.allocate(1);
                    ASSERT(blockSize(bufferSize) == ta.numBytesInUse());
                    mX.release();

                    bslma::SequentialPool mY(buffer,
                                             -bufferSize,
                                             16,
                                             &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(64);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(100);
                    ASSERT(blockSize(100) == ta.numBytesInUse());
                    mY.release();
                }
                {
                    bslma::SequentialPool mX(buffer, bufferSize, strategy,
                                             &ta);

                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)mX.allocate(64);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer = (char *)mX.allocate(1);
                    ASSERT(blockSize(bufferSize) == ta.numBytesInUse());
                    mX.release();

                    bslma::SequentialPool mY(buffer,
                                             -bufferSize,
                                             16,
                                             strategy,
                                             &ta);
                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(64);
                    ASSERT(cBuffer == buffer);
                    ASSERT(0       == ta.numBytesInUse());

                    cBuffer = (char *)mY.allocate(100);
                    ASSERT(blockSize(100) == ta.numBytesInUse());
                    mY.release();
                }
            }
            {
                // Concern c) Static buffer test with alignment considerations.
                // Test that static buffer is used before the internal block
                // list is asked to provide memory with consideration to
                // alignment.

                if (veryVerbose) {
                   cout << "c.  Static buffer test with alignment "
                        << "considerations."
                        << endl;
                }
                {
                    // Allocate until static buffer is not completely full,
                    // additional usable free memory in the buffer is not
                    // enough to fulfill the request.  There is enough
                    // free memory to fulfill the request, but most of it
                    // is scattered and fragmented and unusable.

                    int blockSize = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

                    bslma::SequentialPool seqPool(
                               buffer, 8*blockSize,
                               bslma::BufferAllocator::MAXIMUM_ALIGNMENT, &ta);

                    int offset   = 0; // alignment offset

                    cBuffer = (char *)seqPool.allocate(1);
                    ASSERT(cBuffer == buffer);
                    offset += blockSize;

                    cBuffer = (char *)seqPool.allocate(blockSize);
                    // BlockSize - 1 bytes wasted.

                    ASSERT(cBuffer == buffer + offset);
                    offset += blockSize;

                    cBuffer = (char *)seqPool.allocate(1);
                    ASSERT(cBuffer == buffer + offset);
                    offset += blockSize;

                    cBuffer = (char *)seqPool.allocate(blockSize);
                    // BlockSize - 1 bytes wasted.

                    ASSERT(cBuffer == buffer + offset);
                    offset += blockSize;

                    cBuffer = (char *)seqPool.allocate(1);
                    ASSERT(cBuffer == buffer + offset);
                    offset += blockSize;

                    cBuffer = (char *)seqPool.allocate(blockSize);
                    // BlockSize - 1 bytes wasted.

                    ASSERT(cBuffer == buffer + offset);
                    offset += blockSize;

                    cBuffer = (char *)seqPool.allocate(1);
                    ASSERT(cBuffer == buffer + offset);
                    offset += blockSize;

                    ASSERT(0 == ta.numBytesInUse());
                    cBuffer = (char *)seqPool.allocate(blockSize + 1);
                    // BlockSize - 1 bytes wasted.

                    seqPool.release();
                    ASSERT(0 == ta.numBytesInUse());
                }
                {
                    // Allocate until static buffer is not completely full, but
                    // is unable to allocate another block of the desired size
                    // because of alignment considerations.

                    int blockSize = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

                    buffer += blockSize/2;
                    bslma::SequentialPool seqPool(buffer, 8*blockSize,
                                                  strategy, &ta);

                    cBuffer = (char *)seqPool.allocate(
                                                6*blockSize + blockSize/2 + 1);
                    ASSERT(cBuffer == buffer);

                    cBuffer = (char *)seqPool.allocate(blockSize);
                    // Block + blockSize/2 - 1  bytes wasted.

                    buffer -= 4;
                }
                {
                    buffer += 63;
                    // Mis align the buffer such that the buffer does not
                    // start aligned.  Thus, a 64 byte allocation
                    // cannot be allocated off the static buffer, even though
                    // there is enough space.

                    bslma::SequentialPool seqPool(buffer, bufferSize,
                                                  strategy, &ta);

                    ASSERT(0 == ta.numBytesInUse());

                    cBuffer = (char *)seqPool.allocate(64);
                    ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                    ASSERT(64 + EXTRA_MEM_USED == ta.numBytesInUse());

                    buffer -= 63;
                    // Reset buffer to correct alignment.

                }
            }

            {
                // Concern d) Large allocation with static buffer test.
                // Test that even with a unused static buffer, if the
                // allocation is too large, then allocate with the internal
                // pool.

                if (veryVerbose) {
                   cout << "d.  Large allocation with static buffer test."
                        << endl;
                }

                bslma::SequentialPool seqPool(buffer, bufferSize,
                                              strategy, &ta);

                ASSERT(0 == ta.numBytesInUse());

                int numBytes = 65;
                cBuffer = (char *)seqPool.allocate(numBytes);
                ASSERT(blockSize(numBytes) == ta.numBytesInUse());
            }
            {
                // Concern e) Empty static buffer test.
                // Supply a empty buffer.  Any allocations should be allocated
                // though the internal block list.

                if (veryVerbose) {
                   cout << "e.  Empty static buffer test." << endl;
                }

                bslma::SequentialPool seqPool(buffer, 0, strategy, &ta);
                ASSERT(0 == ta.numBytesInUse());

                cBuffer = (char *)seqPool.allocate(1);
                ASSERT(ta.numBytesInUse() > 0);

                seqPool.release();
                ASSERT(0 == ta.numBytesInUse());
            }
            {
                // Concern f) Don't deallocate static memory test.
                // The provided static buffer is held, not owned.  When seqPool
                // is destroyed, the static buffer should not be deallocated.

                if (veryVerbose) {
                   cout << "f.  Don't deallocate static memory test." << endl;
                }

                const int NA = 128;
                ASSERT(0 == ta.numBytesInUse());
                cBuffer = (char *)ta.allocate(NA);
                ASSERT(NA == ta.numBytesInUse());

                {
                    const int NB = 64, NC = 1024;
                    bslma::SequentialPool seqPool(cBuffer, bufferSize,
                                                  strategy, &ta);
                    char * nBuffer = (char *)seqPool.allocate(NB);
                    nBuffer = (char *)seqPool.allocate(NC);
                    ASSERT(NA + blockSize(NC) == ta.numBytesInUse());
                }
                ASSERT(NA == ta.numBytesInUse());

                ta.deallocate(cBuffer);
                ASSERT(0 == ta.numBytesInUse());
            }
        }
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // RELEASE TEST:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RELEASE TEST" << endl
                                  << "============" << endl;

        typedef bslma::BufferAllocator BA;

        const int TH = 64;
        struct {
            int                   d_line;
            int                   d_bufSize;
            int                   d_requestSize;
            int                   d_numRequests;
        } DATA[] = {
            { L_, 1, 1, 2 },
            { L_, 1, 5, 2 },
            { L_, 1, TH - 1, 2 },
            { L_, 1, TH, 2 },
            { L_, 1, TH + 1, 2 },

            { L_, TH - 1, 1, TH },
            { L_, TH - 1, 5, 1 + (TH - 1) / 5 },
            { L_, TH - 1, TH - 2, 2 },
            { L_, TH - 1, TH - 1, 2 },
            { L_, TH - 1, TH, 2 },

            { L_, TH, 1, TH + 1 },
            { L_, TH, 5, 1 + TH / 5 },
            { L_, TH, TH - 1, 2 },
            { L_, TH, TH, 2 },
            { L_, TH, TH + 1, 2 },

            { L_, TH + 1, 1, TH + 2 },
            { L_, TH + 1, 5, 1 + (TH + 1) / 5 },
            { L_, TH + 1, TH, 2 },
            { L_, TH + 1, TH + 1, 2 },
            { L_, TH + 1, TH + 2, 2 },

            { L_, -1, 1, 2 },
            { L_, -1, 5, 2 },
            { L_, -1, TH - 1, 2 },
            { L_, -1, TH, 2 },
            { L_, -1, TH + 1, 2 },
            /* TBD            { L_, -1, -MS, 2 },
            { L_, -1, -MS + 1, 2 },*/
        };

        const int DATA_SIZE = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < DATA_SIZE; ++i) {

            const int LINE    = DATA[i].d_line;
            // const int BUFSIZE = DATA[i].d_bufSize; // unused

            const int REQSIZE = DATA[i].d_requestSize;
            const int NUMREQ  = DATA[i].d_numRequests;

            const int MAXNUMREQ  = TH + 2;
            LOOP2_ASSERT(MAXNUMREQ, NUMREQ, MAXNUMREQ >= NUMREQ);
            if (MAXNUMREQ < NUMREQ) continue;

            // Try each test using both maximum and natural alignment

            typedef bslma::BufferAllocator::AlignmentStrategy
                                                             AlignmentStrategy;
            AlignmentStrategy strategy;
            for (strategy = bslma::BufferAllocator::MAXIMUM_ALIGNMENT;
                 strategy <= bslma::BufferAllocator::NATURAL_ALIGNMENT;
                 strategy = (AlignmentStrategy) (strategy + 1)) {

                // Keep track of the sequence of allocations from the test
                // allocator.

                int numBlocksInUse[MAXNUMREQ];
                int numBytesInUse[MAXNUMREQ];
                int offsetInBlock[MAXNUMREQ];

                bslma::TestAllocator ta(veryVeryVerbose);
                // Since there is no way to set the initial capacity except at
                // construction, we must let it use the default initial
                // capacity.
//             int capacity(BUFSIZE);
//             bslma::SequentialPool pool(capacity, &ta);

                bslma::SequentialPool pool(strategy, &ta);
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(0 == ta.numBytesInUse());

                // Make NUMREQ requests for memory, recording how the allocator
                // was used after each request.

                for (int reqNum = 0; reqNum < NUMREQ; ++reqNum) {
                    void *returnAddr = pool.allocate(REQSIZE);
                    LOOP2_ASSERT(LINE, reqNum, returnAddr);
                    LOOP2_ASSERT(LINE, reqNum, ta.numBlocksInUse());

                    numBlocksInUse[reqNum] = ta.numBlocksInUse();
                    numBytesInUse[reqNum] = ta.numBytesInUse();
                    int offset = ((char *) returnAddr -
                                  (char *) ta.lastAllocatedAddress());
                    offsetInBlock[reqNum] = offset;

                    if (veryVerbose) {
                        P_(reqNum) P(returnAddr);
                        P(numBlocksInUse[reqNum]);
                        P(numBytesInUse[reqNum]);
                        P(offsetInBlock[reqNum]);
                    }
                } // end for (each request)

                // Now reset and verify that all memory is returned.

                pool.release();
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(0 == ta.numBytesInUse());

                // Now repeat the allocations and verify that the exact same
                // pattern is observed.

                for (int reqNum = 0; reqNum < NUMREQ; ++reqNum) {
                    void *returnAddr = pool.allocate(REQSIZE);
                    ASSERT(returnAddr);
                    ASSERT(ta.numBlocksInUse());

                    LOOP4_ASSERT(LINE, reqNum,
                                 numBlocksInUse[reqNum], ta.numBlocksInUse(),
                                 numBlocksInUse[reqNum]==ta.numBlocksInUse());
                    LOOP4_ASSERT(LINE, reqNum,
                                 numBytesInUse[reqNum], ta.numBytesInUse(),
                                 numBytesInUse[reqNum] == ta.numBytesInUse());
                    int offset = ((char *) returnAddr -
                                  (char *) ta.lastAllocatedAddress());
                    LOOP4_ASSERT(LINE, reqNum,
                                 offsetInBlock[reqNum], offset,
                                 offsetInBlock[reqNum] == offset);
                } // end for (each request)
            } // end for (each alignment strategy)
        } // end for (each test vector)

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'allocate' FUNCTION:
        //
        // Concerns:
        //   1. If no buffer is specified then the pool always allocates
        //      requests from the blockList.  The first allocation is of
        //      INITIAL_SIZE, and subsequent buffer resizes are of exponential
        //      size.
        //   2. If 'initialSize' is specified then all requests up to
        //      'initialSize' require no additional allocations.  Subsequent
        //      buffer resizes are constant or exponential based on whether
        //      'initialSize' is positive or negative.
        //   3. If 'buffer' and 'bufSize' is specified then all requests up to
        //      'bufSize' are satisfied from 'buffer' with no memory
        //      allocations, and subsequent requests cause the internal buffer
        //      to be resized either by constant or exponential growth based on
        //      whether 'bufSize' is positive or negative.
        //   4. All requests over a specified THRESHOLD are satisfied directly
        //      from the blockList if the they cannot be satisfied by the
        //      pool's internal buffer
        //   5. The buffer returned in aligned according to the specified
        //      strategy.
        //
        // Plan:
        //   For 1 - 4 construct objects mX, mY and mZ with default, maximum
        //   and natural alignment allocation strategy using the three
        //   different constructors, namely the default constructor, the
        //   constructor taking an 'initialSize' and the constructor taking a
        //   buffer.  Additionally pass a test allocator to the constructor to
        //   monitor the memory allocations by the pool.  Confirm the bytes
        //   allocated by the constructor are as expected.  Ensure that the
        //   test case contains both positive, negative and zero value of
        //   'initialSize' and 'bufferSize'.
        //
        //   Call the allocate function with various size values, including
        //   positive, negative and zero values.  Ensure that the bytes
        //   allocated by the passed in allocator are as expected.
        //
        // Testing:
        //   void *allocate(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATE TEST" << endl
                                  << "=============" << endl;

        const int DATA[]   = { 0, 1, 5, 7, 8, 15, 16, 24, 31, 32, 33, 48,
                               63, 64, 65, 66, 127, 128, 129, 255, 256,
                               511, 512, 1023, 1024, 1025 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        AlignStrategy MAXIMUM_ALIGNMENT = BufferAllocator::MAXIMUM_ALIGNMENT;
        AlignStrategy NATURAL_ALIGNMENT = BufferAllocator::NATURAL_ALIGNMENT;

        if (verbose) {
            cout << "\nTesting default constructor" << endl;
        }
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                TestAllocator ta(veryVeryVerbose),
                              tb(veryVeryVerbose), tc(veryVeryVerbose);
                Obj mX(&ta);
                Obj mY(MAXIMUM_ALIGNMENT, &tb);
                Obj mZ(NATURAL_ALIGNMENT, &tc);

                mX.allocate(SIZE);
                mY.allocate(SIZE);
                mZ.allocate(SIZE);

                if (0 == SIZE) {
                    continue;
                }

                if (SIZE <= INITIAL_SIZE * 2) {
                    LOOP2_ASSERT(
                            i, SIZE,
                            blockSize(INITIAL_SIZE * 2) == ta.numBytesInUse());
                    LOOP2_ASSERT(
                            i, SIZE,
                            blockSize(INITIAL_SIZE * 2) == tb.numBytesInUse());
                    LOOP2_ASSERT(
                            i, SIZE,
                            blockSize(INITIAL_SIZE * 2) == tc.numBytesInUse());
                }
                else {
                    int nextSize = calculateNextSize(std::abs(INITIAL_SIZE),
                                                     SIZE);
                    LOOP4_ASSERT(i, SIZE, blockSize(nextSize),
                                 ta.numBytesInUse(),
                                 blockSize(nextSize) == ta.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                                 blockSize(nextSize) == tb.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                                 blockSize(nextSize) == tc.numBytesInUse());
                }
            }
        }

        if (verbose) {
            cout << "\nTesting 'initialSize' constructor" << endl;
        }
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                const int INITIAL_SIZES[] = { SIZE - 1, SIZE, SIZE + 1,
                                              -SIZE - 1, -SIZE, -SIZE + 1 };
                const int NUM_INITIAL_SIZES = sizeof  INITIAL_SIZES
                                            / sizeof *INITIAL_SIZES;

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int INITIAL_SIZE = INITIAL_SIZES[j];

                    TestAllocator ta(veryVeryVerbose),
                                  tb(veryVeryVerbose), tc(veryVeryVerbose);

                    if (veryVerbose) {
                        P(INITIAL_SIZE) P(SIZE)
                    }

                    Obj mX(INITIAL_SIZE, &ta);
                    Obj mY(INITIAL_SIZE, MAXIMUM_ALIGNMENT, &tb);
                    Obj mZ(INITIAL_SIZE, NATURAL_ALIGNMENT, &tc);

                    const int NA = ta.numBytesInUse();
                    const int NB = tb.numBytesInUse();
                    const int NC = tc.numBytesInUse();

                    mX.allocate(SIZE);
                    mY.allocate(SIZE);
                    mZ.allocate(SIZE);

                    if (SIZE <= std::abs(INITIAL_SIZE)) {
                        LOOP_ASSERT(i, NA == ta.numBytesInUse());
                        LOOP_ASSERT(i, NB == tb.numBytesInUse());
                        LOOP_ASSERT(i, NC == tc.numBytesInUse());
                    }
                    else if (INITIAL_SIZE < 0) {
                        int nextSize = calculateNextSize(
                                                 std::abs(INITIAL_SIZE), SIZE);
                        LOOP3_ASSERT(i, NA + blockSize(nextSize),
                                     ta.numBytesInUse(),
                                     NA + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                        LOOP_ASSERT(i, NB + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                        LOOP_ASSERT(i, NC + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                    }
                    else {
                        if (0 == INITIAL_SIZE && SIZE < ::INITIAL_SIZE) {
                            LOOP_ASSERT(i, NA + blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB + blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NC + blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                        }
                        else {
                            LOOP_ASSERT(i, NA + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NC + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                        }
                    }
                }
            }
        }

        if (verbose) {
            cout << "\nTesting 'initialSize' and 'maxBufferSize' constructor"
                 << endl;
        }
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                const int INITIAL_SIZES[] = { SIZE - 1, SIZE, SIZE + 1,
                                              -SIZE - 1, -SIZE, -SIZE + 1 };
                const int NUM_INITIAL_SIZES = sizeof  INITIAL_SIZES
                                            / sizeof *INITIAL_SIZES;

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int INITIAL_SIZE = INITIAL_SIZES[j];

                    TestAllocator ta(veryVeryVerbose),
                                  tb(veryVeryVerbose), tc(veryVeryVerbose);

                    if (veryVerbose) {
                        P(INITIAL_SIZE) P(SIZE)
                    }

                    const int NUM_MAX_SIZES = 3;
                    int MAX_SIZES[NUM_MAX_SIZES];
                    if (INITIAL_SIZE < 0) {
                        MAX_SIZES[0] = std::abs(INITIAL_SIZE) * 2;
                        MAX_SIZES[1] = std::abs(INITIAL_SIZE) * 4;
                        MAX_SIZES[2] = std::abs(INITIAL_SIZE) * 8;
                    }
                    else {
                        MAX_SIZES[0] = INT_MAX;
                        MAX_SIZES[1] = INT_MAX;
                        MAX_SIZES[2] = INT_MAX;
                    }

                    for (int k = 0; k < NUM_MAX_SIZES; ++k) {
                        const int MAX_SIZE = MAX_SIZES[k];

                        Obj mX(INITIAL_SIZE, MAX_SIZE, &ta);
                        Obj mY(INITIAL_SIZE, MAX_SIZE,
                               MAXIMUM_ALIGNMENT, &tb);
                        Obj mZ(INITIAL_SIZE, MAX_SIZE,
                               NATURAL_ALIGNMENT, &tc);

                        const int NA = ta.numBytesInUse();
                        const int NB = tb.numBytesInUse();
                        const int NC = tc.numBytesInUse();

                        mX.allocate(SIZE);
                        mY.allocate(SIZE);
                        mZ.allocate(SIZE);

                        if (SIZE <= std::abs(INITIAL_SIZE)) {
                            LOOP_ASSERT(i, NA == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB == tb.numBytesInUse());
                            LOOP_ASSERT(i, NC == tc.numBytesInUse());
                        }
                        else if (INITIAL_SIZE < 0) {
                            if (SIZE < MAX_SIZE) {
                                int nextSize = calculateNextSize(
                                       std::abs(INITIAL_SIZE), SIZE, MAX_SIZE);
                                LOOP_ASSERT(i, NA + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NB + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NC + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                            }
                            else {
                                LOOP_ASSERT(i, NA + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NB + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NC + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            }
                        }
                        else {
                            if (0 == INITIAL_SIZE && SIZE < ::INITIAL_SIZE) {
                                LOOP_ASSERT(i, NA + blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NB + blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NC + blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                            }
                            else {
                                LOOP_ASSERT(i, NA + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NB + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NC + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            }
                        }
                    }
                }
            }
        }

        if (verbose) {
            cout << "\nTesting constructor supplying a buffer" << endl;
        }
        {
            const int BUFFER_SIZE = 2048;

            // Maximally aligned buffer used by 'allocateFromBuffer'.
            // Note that 'bufferStorage' *must* be declared 'static' to force
            // maximal alignment under Windows and MSVC.

            static bsls::AlignedBuffer<BUFFER_SIZE> bufferStorage;
            char *buffer = bufferStorage.buffer();

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                const int INITIAL_SIZES[] = {
                    SIZE - 1, SIZE, SIZE + 1,
                    -SIZE - 1, -SIZE, -SIZE + 1
                };
                const int NUM_INITIAL_SIZES = sizeof  INITIAL_SIZES
                                            / sizeof *INITIAL_SIZES;

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int INITIAL_SIZE = INITIAL_SIZES[j];

                    TestAllocator ta(veryVeryVerbose),
                                  tb(veryVeryVerbose), tc(veryVeryVerbose);

                    if (veryVerbose) {
                        P(INITIAL_SIZE) P(SIZE)
                    }

                    Obj mX(buffer, INITIAL_SIZE, &ta);
                    Obj mY(buffer, INITIAL_SIZE, MAXIMUM_ALIGNMENT, &tb);
                    Obj mZ(buffer, INITIAL_SIZE, NATURAL_ALIGNMENT, &tc);

                    LOOP_ASSERT(i, 0 == ta.numBytesInUse());
                    LOOP_ASSERT(i, 0 == tb.numBytesInUse());
                    LOOP_ASSERT(i, 0 == tc.numBytesInUse());

                    mX.allocate(SIZE);
                    mY.allocate(SIZE);
                    mZ.allocate(SIZE);

                    if (SIZE <= std::abs(INITIAL_SIZE)) {
                        LOOP_ASSERT(i, 0 == ta.numBytesInUse());
                        LOOP_ASSERT(i, 0 == tb.numBytesInUse());
                        LOOP_ASSERT(i, 0 == tc.numBytesInUse());
                    }
                    else if (INITIAL_SIZE < 0) {
                        int nextSize = calculateNextSize(
                                                 std::abs(INITIAL_SIZE), SIZE);
                        LOOP_ASSERT(i, blockSize(nextSize)
                                                        == ta.numBytesInUse());
                        LOOP_ASSERT(i, blockSize(nextSize)
                                                        == ta.numBytesInUse());
                        LOOP_ASSERT(i, blockSize(nextSize)
                                                        == ta.numBytesInUse());
                    }
                    else {
                        if (0 == INITIAL_SIZE && SIZE < ::INITIAL_SIZE) {
                            LOOP_ASSERT(i, blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                        }
                        else {
                            LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                        }
                    }
                }
            }
        }

        if (verbose) {
            cout << "\nTesting constructor supplying a buffer"
                 << " and maxBufferSize" << endl;
        }
        {
            const int BUFFER_SIZE = 2048;

            // Maximally aligned buffer used by 'allocateFromBuffer'.
            // Note that 'bufferStorage' *must* be declared 'static' to force
            // maximal alignment under Windows and MSVC.

            static bsls::AlignedBuffer<BUFFER_SIZE> bufferStorage;
            char *buffer = bufferStorage.buffer();

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                const int INITIAL_SIZES[] = {
                    SIZE - 1, SIZE, SIZE + 1,
                    -SIZE - 1, -SIZE, -SIZE + 1
                };
                const int NUM_INITIAL_SIZES = sizeof  INITIAL_SIZES
                                            / sizeof *INITIAL_SIZES;

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int INITIAL_SIZE = INITIAL_SIZES[j];

                    TestAllocator ta(veryVeryVerbose),
                                  tb(veryVeryVerbose), tc(veryVeryVerbose);

                    if (veryVerbose) {
                        P(INITIAL_SIZE) P(SIZE)
                    }

                    const int NUM_MAX_SIZES = 3;
                    int MAX_SIZES[NUM_MAX_SIZES];
                    if (INITIAL_SIZE < 0) {
                        MAX_SIZES[0] = std::abs(INITIAL_SIZE) * 2;
                        MAX_SIZES[1] = std::abs(INITIAL_SIZE) * 4;
                        MAX_SIZES[2] = std::abs(INITIAL_SIZE) * 8;
                    }
                    else {
                        MAX_SIZES[0] = INT_MAX;
                        MAX_SIZES[1] = INT_MAX;
                        MAX_SIZES[2] = INT_MAX;
                    }

                    for (int k = 0; k < NUM_MAX_SIZES; ++k) {
                        const int MAX_SIZE = MAX_SIZES[k];

                        Obj mX(buffer, INITIAL_SIZE, MAX_SIZE, &ta);
                        Obj mY(buffer, INITIAL_SIZE, MAX_SIZE,
                               MAXIMUM_ALIGNMENT, &tb);
                        Obj mZ(buffer, INITIAL_SIZE, MAX_SIZE,
                               NATURAL_ALIGNMENT, &tc);

                        LOOP_ASSERT(i, 0 == ta.numBytesInUse());
                        LOOP_ASSERT(i, 0 == tb.numBytesInUse());
                        LOOP_ASSERT(i, 0 == tc.numBytesInUse());

                        mX.allocate(SIZE);
                        mY.allocate(SIZE);
                        mZ.allocate(SIZE);

                        if (SIZE <= std::abs(INITIAL_SIZE)) {
                            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
                            LOOP_ASSERT(i, 0 == tb.numBytesInUse());
                            LOOP_ASSERT(i, 0 == tc.numBytesInUse());
                        }
                        else if (INITIAL_SIZE < 0) {
                            if (SIZE < MAX_SIZE) {
                                int nextSize = calculateNextSize(
                                       std::abs(INITIAL_SIZE), SIZE, MAX_SIZE);
                                LOOP_ASSERT(i, blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, blockSize(nextSize)
                                                        == ta.numBytesInUse());
                            }
                            else {
                                LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            }
                        }
                        else {
                            if (0 == INITIAL_SIZE && SIZE < ::INITIAL_SIZE) {
                                LOOP_ASSERT(i, blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, blockSize(::INITIAL_SIZE)
                                                        == ta.numBytesInUse());
                            }
                            else {
                                LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            }
                        }
                    }
                }
            }
        }

        if (verbose) {
            cout << "\nTesting the returned buffer" << endl;
        }
        {
            typedef bslma::BufferAllocator BA;
            typedef bsls::AlignmentUtil    AL;

            const int BUFFER_SIZE = 8192;

            // Maximally aligned buffer used by 'allocateFromBuffer'.
            // Note that 'bufferStorage' *must* be declared 'static' to force
            // maximal alignment under Windows and MSVC.

            static bsls::AlignedBuffer<BUFFER_SIZE> bufferStorage;
            char *buffer = bufferStorage.buffer();

            for (int ai = MAXIMUM_ALIGNMENT; ai <= NATURAL_ALIGNMENT; ++ai) {
                if (verbose)
                    cout << "\nTesting 'allocate' w/ "
                         << (MAXIMUM_ALIGNMENT == ai ? "maximum" : "natural")
                         << " alignment." << endl;

                const BA::AlignmentStrategy STRATEGY = MAXIMUM_ALIGNMENT == ai
                    ? BA::MAXIMUM_ALIGNMENT
                    : BA::NATURAL_ALIGNMENT;

                bslma::TestAllocator ta(veryVeryVerbose);
                const bslma::TestAllocator& TA = ta;
                Obj mX(BUFFER_SIZE, STRATEGY, &ta);
                int cursor = 0;

                void *pX = mX.allocate(1); // Cause allocation of buffer
                BA::allocateFromBuffer(&cursor,
                                       buffer,
                                       BUFFER_SIZE,
                                       1,
                                       STRATEGY);

                // Starting address of the pool's internal buffer.

                const char *const HEAD = (char *) TA.lastAllocatedAddress()
                    + sizeof(InfrequentDeleteBlock)
                    - bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
                if (veryVerbose) { T_ A(buffer); T_ A(HEAD); }
                LOOP_ASSERT(ai, HEAD == (char *)pX);

                const int DATA[]   = { 0, 1, 5, 7, 8, 15, 16, 24, 31, 32, 33,
                                       48, 63, 64, 65, 66, 127, 128, 129, 255,
                                       256, 511, 512, 1023, 1024, 1025 };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;
                for (int di = 0; di < NUM_DATA; ++di) {
                    const int SIZE = DATA[di];
                    char *pB = (char *) BA::allocateFromBuffer(
                                 &cursor, buffer, BUFFER_SIZE, SIZE, STRATEGY);
                    char *pX = (char *) mX.allocate(SIZE);

                    if (!pB) {
                        LOOP2_ASSERT(ai, di, !pX);
                        continue;
                    }

                    const int EXP = pB - buffer;
                    int offset = pX - HEAD;
                    if (veryVerbose) { T_ P_(offset); T_ P(EXP); T_ P(SIZE); }

                    // Ensure memory offset from the pool's 'allocate' is
                    // equal to that from 'allocateFromBuffer'.

                    LOOP2_ASSERT(ai, di, EXP == offset);

                    // Ensure request does not exceed buffer size.

                    LOOP2_ASSERT(ai, di, BUFFER_SIZE >= offset);
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FILE-STATIC FUNCTION TEST
        //   Create a 'bslma::BlockList' object initialized with a test
        //   allocator.  Invoke both the 'blockSize' function and the
        //   'bslma::BlockList::allocate' method with varying memory sizes, and
        //   verify that the sizes returned by 'blockSize' are equal to the
        //   memory request sizes recorded by the allocator.
        //
        // Testing:
        //   int blockSize(numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "FILE-STATIC FUNCTION TEST" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\nTesting 'blockSize'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 64, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator a(veryVeryVerbose);
        bslma::InfrequentDeleteBlockList bl(&a);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            int blkSize = blockSize(SIZE);
            bl.allocate(SIZE);

            // if the first 'SIZE' is 0, the allocator's 'allocate' is never
            // called, thus, 'lastAllocatedSize' will return -1 instead of 0.

            const int EXP = i || SIZE ? a.lastAllocatedNumBytes() : 0;

            if (veryVerbose) { T_ P_(SIZE); T_ P_(blkSize); T_ P(EXP); }
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

#else

int main(int argc, char *argv[])
{
    return 0;
}

#endif  // #ifndef BDE_OSS_TEST


// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
