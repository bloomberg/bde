// bslma_sequentialallocator.t.cpp                                    -*-C++-*-

#include <bslma_sequentialallocator.h>

#if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED

#include <bslma_allocator.h>          // for testing only
#include <bslma_bufferallocator.h>    // for testing only
#include <bslma_default.h>            // for testing only
#include <bslma_sequentialpool.h>     // for testing only
#include <bslma_testallocator.h>      // for testing only

#include <climits>
#include <cstdlib>     // atoi()
#include <cstring>     // memcpy()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this test suite are to verify 1) that
// 'bslma::SequentialAllocator' correctly proxies memory requests (except
// deallocation) to its 'bslma::SequentialPool' member; and 2) that the
// 'deallocate' method does *not* deallocate any memory.
//
// To achieve goal 1, create a string allocator and a string pool, and supply
// each with its own instance of test allocator.  Request memory of varying
// sizes from both the string allocator and the string pool.  Verify that the
// test allocators contains the same number of bytes in use and the same total
// number of bytes requested.
//
// To achieve goal 2, create a string allocator supplied with a test allocator.
// Request memory of varying sizes and then deallocate each memory block.
// Verify that the number of bytes in use indicated by the test allocator does
// not decrease after each 'deallocate' method invocation.
//-----------------------------------------------------------------------------
// [ 2] bslma::SequentialAllocator(allocator);
// [ 2] bslma::SequentialAllocator(strategy, allocator);
// [ 2] bslma::SequentialAllocator(initialSize, allocator);
// [ 2] bslma::SequentialAllocator(initialSize, strategy, allocator);
// [ 2] bslma::SequentialAllocator(buffer, bufSize, allocator);
// [ 2] bslma::SequentialAllocator(buffer, bufSize, strategy, allocator);
// [ 2] bslma::SequentialAllocator(initialSize, maxSize, allocator);
// [ 2] bslma::SequentialAllocator(initialSize, maxSize, strategy, alloc);
// [ 2] bslma::SequentialAllocator(buffer, bufSize, maxSize, allocator);
// [ 2] bslma::SequentialAllocator(buf, bufSize, maxSize, strategy, ta);
// [ 2] ~bslma::SequentialAllocator();
// [ 1] void *allocate(numBytes);
// [ 3] void deallocate(address);
// [ 4] void release();
// [ 4] void reserveCapacity(numBytes);
// [ 5] void truncate(void *address, int originalNumBytes, int newNumBytes);
// [ 6] int expand(void *address, int originalNumBytes);
// [ 6] int expand(void *address, int originalNumBytes, int maxNumBytes);
// [ 7] void *allocateAndExpand(int *size);
// [ 7] void *allocateAndExpand(int *size, int maxNumBytes);
//-----------------------------------------------------------------------------
// [8] USAGE EXAMPLE

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
#define TAB cout << '\t';

typedef bslma::SequentialAllocator         Obj;
typedef bslma::SequentialPool              Pool;
typedef bslma::TestAllocator               TestAllocator;
typedef bslma::BufferAllocator             BufferAllocator;
typedef BufferAllocator::AlignmentStrategy AlignStrategy;

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
class my_DoubleStack {
    double           *d_stack_p;      // dynamically allocated array (d_size
                                      // elements)

    int               d_size;         // physical capacity of this stack (in
                                      // elements)

    int               d_length;       // logical index of next available stack
                                      // element

    bslma::Allocator *d_allocator_p;  // holds (but does not own) object

    friend class my_DoubleStackIter;

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_DoubleStack(bslma::Allocator *basicAllocator = 0);
    my_DoubleStack(const my_DoubleStack&  other,
                   bslma::Allocator      *basicAllocator = 0);
    ~my_DoubleStack();

    // MANIPULATORS
    my_DoubleStack& operator=(const my_DoubleStack& rhs);
    void push(double value);
    void pop();

    // ACCESSORS
    const double& top() const;
    bool isEmpty() const;
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

// ...

my_DoubleStack::my_DoubleStack(bslma::Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator)
{
    ASSERT(d_allocator_p);
    d_stack_p = (double *) d_allocator_p->allocate(d_size * sizeof *d_stack_p);
}

my_DoubleStack::~my_DoubleStack()
{
    // CLASS INVARIANTS
    ASSERT(d_allocator_p);
    ASSERT(d_stack_p);
    ASSERT(0 <= d_length);
    ASSERT(0 <= d_size);
    ASSERT(d_length <= d_size);

    d_allocator_p->deallocate(d_stack_p);
}

inline void my_DoubleStack::push(double value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_stack_p[d_length++] = value;
}

inline static
void reallocate(double **array, int newSize, int length,
                bslma::Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified
    // 'newSize' using the specified 'basicAllocator'.  The specified
    // 'length' number of leading elements are preserved.  Since the
    //  class invariant requires that the physical capacity of the
    // container may grow but never shrink; the behavior is undefined
    // unless length <= newSize.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(basicAllocator);
    ASSERT(length <= newSize);        // enforce class invariant

    double *tmp = *array;             // support exception neutrality
    *array = (double *) basicAllocator->allocate(newSize * sizeof **array);

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);
    basicAllocator->deallocate(tmp);
}

void my_DoubleStack::increaseSize()
{
     int proposedNewSize = d_size * GROW_FACTOR;      // reallocate can throw
     ASSERT(proposedNewSize > d_length);
     reallocate(&d_stack_p, proposedNewSize, d_length, d_allocator_p);
     d_size = proposedNewSize;                        // we're committed
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Need to Complete Usage Test Case

class my_DoubleStackIter {
    const double *d_stack_p;
    int d_index;
  private:
    my_DoubleStackIter(const my_DoubleStackIter&);
    my_DoubleStackIter& operator=(const my_DoubleStackIter&);
  public:
    my_DoubleStackIter(const my_DoubleStack& stack)
    : d_stack_p(stack.d_stack_p), d_index(stack.d_length - 1) { }
    void operator++() { --d_index; }
    operator const void *() const { return d_index >= 0 ? this : 0; }
    const double& operator()() const { return d_stack_p[d_index]; }
};

ostream& operator<<(ostream& stream, const my_DoubleStack& stack)
{
    stream << "(top) [";
    for (my_DoubleStackIter it(stack); it; ++it) {
        stream << ' ' << it();
    }
    return stream << " ] (bottom)" << flush;
}

enum { INITIAL_BUF_SIZE = 256 };

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Ensure usage example compiles and works.
        //
        // Testing:
        //   Ensure usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;
        {
            bslma::SequentialAllocator myA;
            bslma::Allocator& a = myA;
            my_DoubleStack s(&a);
            s.push(1.25);
            s.push(1.5);
            s.push(1.75);

            if (verbose) {
                cout << "\t\t" << s << endl;
            }
        }
      } break;
      case 7: {
        // TBD doc and move
        // quick test for allocateAndExpand
        {
            bslma::TestAllocator ta(veryVeryVerbose);
            bslma::SequentialAllocator sa(&ta);

            char *addr0 = (char *)sa.allocate(64);
            sa.truncate(addr0, 64, 0);

            const int BUF_SIZE = INITIAL_BUF_SIZE * 2;
            int size;

            size = 8;
            char *addr1 = (char *)sa.allocateAndExpand(&size);
            ASSERT(addr0 == addr1);
            ASSERT(BUF_SIZE == size);

            size = 8;
            sa.truncate(addr1, BUF_SIZE, 0);
            addr1 = (char *)sa.allocateAndExpand(&size, 16);
            ASSERT(addr0 == addr1);
            ASSERT(16 == size);

            size = 8;
            sa.truncate(addr1, 16, 0);
            addr1 = (char *)sa.allocateAndExpand(&size, 1024);
            ASSERT(addr0 == addr1);
            ASSERT(BUF_SIZE == size);
        }
      } break;
      case 6: {
        // TBD doc and move
        // quick test for expand
        {
            bslma::TestAllocator ta(veryVeryVerbose);
            bslma::SequentialAllocator sa(&ta);

            const int SIZE = INITIAL_BUF_SIZE * 2;
            char *addr0 = (char *)sa.allocate(SIZE);
            sa.truncate(addr0, SIZE, 0);
            char *addr1 = (char *)sa.allocate(8);
            ASSERT(addr0 == addr1);

            ASSERT(SIZE == sa.expand(addr1, 8));
            ASSERT(addr0 == addr1);

            ASSERT(SIZE == sa.expand(addr1, SIZE));

            sa.truncate(addr1, SIZE, 8);
            ASSERT(16 == sa.expand(addr1, 8, 16));
            ASSERT(24 == sa.expand(addr1, 16, 24));
            ASSERT(32 == sa.expand(addr1, 24, 32));
            ASSERT(SIZE == sa.expand(addr1, 32, SIZE));

            sa.truncate(addr1, SIZE, 8);
            sa.expand(addr1, 8, 16);
            char *addr2 = (char *)sa.allocate(8);
            ASSERT(addr1 + 16 == addr2);
            ASSERT(16 == sa.expand(addr1, 16));
            ASSERT(16 == sa.expand(addr1, 16, 20));
            ASSERT(SIZE - 16 == sa.expand(addr2, 8));
        }
      } break;
      case 5: {
        // TBD doc and move
        // quick test for truncate
        {
            bslma::TestAllocator ta(veryVeryVerbose);
            bslma::SequentialAllocator sa(&ta);

            char *addr0 = (char *)sa.allocate(32);
            sa.truncate(addr0, 32, 0);
            char *addr1 = (char *)sa.allocate(8);
            ASSERT(addr0 == addr1);

            char *addr2 = (char *)sa.allocate(8);
            sa.truncate(addr1, 8, 4);
            sa.truncate(addr2, 8, 4);
            char *addr3 = (char *)sa.allocate(4);
            ASSERT(addr2 + 4 == addr3);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // RESERVECAPACITY TEST
        //   Create a string allocator and a string pool, and initialize each
        //   with its own instance of test allocator.  Reserve capacity and
        //   request memory of varying sizes from both the string allocator and
        //   the string pool.  Verify that both test allocators contain the
        //   same number of bytes in use and the same total number of bytes
        //   requested.
        //
        // Testing:
        //   void reserveCapacity(numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "Testing 'reserveCapacity'." << endl;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int RES_DATA[] = { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_RES_DATA = sizeof RES_DATA / sizeof *RES_DATA;

        bslma::TestAllocator sequentialAllocatorTA(veryVeryVerbose);
        bslma::TestAllocator sequentialPoolTA(veryVeryVerbose);

        for (int j = 0; j < NUM_RES_DATA; ++j) {
            bslma::SequentialAllocator sa(&sequentialAllocatorTA);
            bslma::SequentialPool sp(&sequentialPoolTA);

            sa.reserveCapacity(RES_DATA[j]);
            sp.reserveCapacity(RES_DATA[j]);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];
                void *p = sa.allocate(SIZE);
                void *q = sp.allocate(SIZE);
                LOOP_ASSERT(i, sequentialAllocatorTA.numBytesInUse()
                                          == sequentialPoolTA.numBytesInUse());
            }

            sa.release();
            sp.release();
            ASSERT(0 == sequentialAllocatorTA.numBytesInUse());
            ASSERT(0 == sequentialPoolTA.numBytesInUse());
            ASSERT(sequentialAllocatorTA.numBytesTotal()
                                          == sequentialPoolTA.numBytesTotal());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEALLOCATE TEST
        //   Create a sequentialing allocator initialized with a test
        //   allocator.  Request memory of varying sizes and then deallocate
        //   each memory.  Verify that the number of bytes in use indicated by
        //   the test allocator does not decrease after each 'deallocate'
        //   method invocation.
        //
        // Testing:
        //   void deallocate(address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEALLOCATE TEST" << endl
                                  << "===============" << endl;

        if (verbose) cout << "Testing 'deallocate'." << endl;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::SequentialAllocator sa(&ta);

        int lastNumBytesInUse = ta.numBytesInUse();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            void *p = sa.allocate(SIZE);
            const int numBytesInUse = ta.numBytesInUse();
            sa.deallocate(p);
            LOOP_ASSERT(i, numBytesInUse == ta.numBytesInUse());
            LOOP_ASSERT(i, lastNumBytesInUse <= ta.numBytesInUse());
            lastNumBytesInUse = ta.numBytesInUse();
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR TEST
        //   Create a string allocator and a string pool, and initialize each
        //   with its own instance of test allocator, and its own optional
        //   buffer of the proper size.  Check that the allocator is not called
        //   if requested to allocate what fits in the buffer.
        //
        // Testing:
        //   bslma::SequentialAllocator(allocator);
        //   bslma::SequentialAllocator(strategy, allocator);
        //   bslma::SequentialAllocator(initialSize, allocator);
        //   bslma::SequentialAllocator(initialSize, strategy, allocator);
        //   bslma::SequentialAllocator(buffer, bufSize, allocator);
        //   bslma::SequentialAllocator(buffer, bufSize, strategy, allocator);
        //   bslma::SequentialAllocator(initialSize, maxSize, allocator);
        //   bslma::SequentialAllocator(initialSize, maxSize, strategy, alloc);
        //   bslma::SequentialAllocator(buffer, bufSize, maxSize, allocator);
        //   bslma::SequentialAllocator(buf, bufSize, maxSize, strategy, ta);
        //   ~bslma::SequentialAllocator();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONSTRUCTOR TEST" << endl
                                  << "================" << endl;

        if (verbose) cout << "Testing 'default constructor'." << endl;

        AlignStrategy MAX = BufferAllocator::MAXIMUM_ALIGNMENT;
        AlignStrategy NAT = BufferAllocator::NATURAL_ALIGNMENT;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        {
            TestAllocator ta(veryVeryVerbose);
            TestAllocator tb(veryVeryVerbose);

            Obj mX(&ta); Obj mY(MAX, &ta); Obj mZ(NAT, &ta);
            ASSERT(0 == ta.numBytesInUse());

            Pool mA(&tb); Pool mB(MAX, &tb); Pool mC(NAT, &tb);
            ASSERT(0 == tb.numBytesInUse());

            mX.allocate(128);
            mA.allocate(128);
            ASSERT(ta.numBytesInUse() == tb.numBytesInUse());

            mY.allocate(INITIAL_SIZE * 2);
            mB.allocate(INITIAL_SIZE * 2);
            ASSERT(ta.numBytesInUse() == tb.numBytesInUse());

            mZ.allocate(INITIAL_SIZE * 8);
            mC.allocate(INITIAL_SIZE * 8);
            ASSERT(ta.numBytesInUse() == tb.numBytesInUse());
        }

        for (int i = 0; i < NUM_DATA; ++i)
        {
            const int SIZE = DATA[i];

            bslma::TestAllocator sequentialPoolTA(veryVeryVerbose);
            bslma::SequentialPool sp(&sequentialPoolTA);
            char *buffer = (char*)sp.allocate(SIZE);

            bslma::TestAllocator sequentialAllocatorTA(veryVeryVerbose);
            bslma::SequentialAllocator sa(buffer, 
                                          SIZE, 
                                          &sequentialAllocatorTA);

            void *p = sa.allocate(SIZE);
            LOOP_ASSERT(i, 0 == sequentialAllocatorTA.numBytesInUse());
            void *q = sa.allocate(SIZE);
            if (SIZE) {
                LOOP_ASSERT(i, 0 < sequentialAllocatorTA.numBytesInUse());
            }
        }

        {
            const int MAX_BUFFER_SIZE = 8192;
            char BUFFER[MAX_BUFFER_SIZE];

            const struct {
                int           d_line;
                AlignStrategy d_strategy;
                int           d_initialSize;
                int           d_maxSize;
                int           d_allocSize;
            } DATA[] = {
           // Line      Strategy   InitialSize    MaxSize   AllocSize
           // ----      --------   -----------    -------   ---------
        {      L_,          NAT,            0,         -1,        0    },
        {      L_,          MAX,            0,         -1,        0    },
        {      L_,          NAT,            0,         -1,        1    },
        {      L_,          MAX,            0,         -1,        1    },
        {      L_,          NAT,            0,         -1,        8    },
        {      L_,          MAX,            0,         -1,        8    },
        {      L_,          NAT,            0,         -1,      512    },
        {      L_,          MAX,            0,         -1,      512    },
        {      L_,          NAT,            0,         -1,     1024    },
        {      L_,          MAX,            0,         -1,     1024    },

        {      L_,          NAT,           64,         -1,        0    },
        {      L_,          MAX,           64,         -1,        0    },
        {      L_,          NAT,           64,         -1,        1    },
        {      L_,          MAX,           64,         -1,        1    },
        {      L_,          NAT,           64,         -1,        8    },
        {      L_,          MAX,           64,         -1,        8    },
        {      L_,          NAT,           64,         -1,      512    },
        {      L_,          MAX,           64,         -1,      512    },
        {      L_,          NAT,           64,         -1,     1024    },
        {      L_,          MAX,           64,         -1,     1024    },

        {      L_,          NAT,          -64,        256,        0    },
        {      L_,          MAX,          -64,        256,        0    },
        {      L_,          NAT,          -64,        256,        1    },
        {      L_,          MAX,          -64,        256,        1    },
        {      L_,          NAT,          -64,        256,        8    },
        {      L_,          MAX,          -64,        256,        8    },
        {      L_,          NAT,          -64,        256,      255    },
        {      L_,          MAX,          -64,        256,      255    },
        {      L_,          NAT,          -64,        256,      256    },
        {      L_,          MAX,          -64,        256,      256    },
        {      L_,          NAT,          -64,        256,      257    },
        {      L_,          MAX,          -64,        256,      257    },
        {      L_,          NAT,          -64,        256,      512    },
        {      L_,          MAX,          -64,        256,      512    },
        {      L_,          NAT,          -64,        256,     1024    },
        {      L_,          MAX,          -64,        256,     1024    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i)
            {
                const int           LINE         = DATA[i].d_line;
                const AlignStrategy STRATEGY     = DATA[i].d_strategy;
                const int           INITIAL_SIZE = DATA[i].d_initialSize;
                const int           MAX_SIZE     = -1 == DATA[i].d_maxSize
                                                 ? INT_MAX
                                                 : DATA[i].d_maxSize;
                const int           ALLOC_SIZE   = DATA[i].d_allocSize;

                {
                    TestAllocator ta(veryVeryVerbose), tb(veryVeryVerbose);
                    Obj mX(INITIAL_SIZE, &ta); Pool mP(INITIAL_SIZE, &tb);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                    mX.allocate(ALLOC_SIZE);
                    mP.allocate(ALLOC_SIZE);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                }
                {
                    TestAllocator ta(veryVeryVerbose), tb(veryVeryVerbose);
                    Obj mX(INITIAL_SIZE, STRATEGY, &ta);
                    Pool mP(INITIAL_SIZE, STRATEGY, &tb);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                    mX.allocate(ALLOC_SIZE);
                    mP.allocate(ALLOC_SIZE);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                }
                {
                    TestAllocator ta(veryVeryVerbose), tb(veryVeryVerbose);
                    Obj mX(INITIAL_SIZE, MAX_SIZE, &ta);
                    Pool mP(INITIAL_SIZE, MAX_SIZE, &tb);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                    mX.allocate(ALLOC_SIZE);
                    mP.allocate(ALLOC_SIZE);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                }
                {
                    TestAllocator ta(veryVeryVerbose), tb(veryVeryVerbose);
                    Obj mX(INITIAL_SIZE, MAX_SIZE, STRATEGY, &ta);
                    Pool mP(INITIAL_SIZE, MAX_SIZE, STRATEGY, &tb);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                    mX.allocate(ALLOC_SIZE);
                    mP.allocate(ALLOC_SIZE);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                }
                {
                    TestAllocator ta(veryVeryVerbose), tb(veryVeryVerbose);
                    Obj mX(BUFFER, INITIAL_SIZE, &ta);
                    Pool mP(BUFFER, INITIAL_SIZE, &tb);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                    mX.allocate(ALLOC_SIZE);
                    mP.allocate(ALLOC_SIZE);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                }
                {
                    TestAllocator ta(veryVeryVerbose), tb(veryVeryVerbose);
                    Obj mX(BUFFER, INITIAL_SIZE, STRATEGY, &ta);
                    Pool mP(BUFFER, INITIAL_SIZE, STRATEGY, &tb);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                    mX.allocate(ALLOC_SIZE);
                    mP.allocate(ALLOC_SIZE);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                }
                {
                    TestAllocator ta(veryVeryVerbose), tb(veryVeryVerbose);
                    Obj mX(BUFFER, INITIAL_SIZE, MAX_SIZE, &ta);
                    Pool mP(BUFFER, INITIAL_SIZE, MAX_SIZE, &tb);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                    mX.allocate(ALLOC_SIZE);
                    mP.allocate(ALLOC_SIZE);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                }
                {
                    TestAllocator ta(veryVeryVerbose), tb(veryVeryVerbose);
                    Obj mX(BUFFER, INITIAL_SIZE, MAX_SIZE, STRATEGY, &ta);
                    Pool mP(BUFFER, INITIAL_SIZE, MAX_SIZE, STRATEGY, &tb);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                    mX.allocate(ALLOC_SIZE);
                    mP.allocate(ALLOC_SIZE);
                    LOOP_ASSERT(LINE,
                                ta.numBytesInUse() == tb.numBytesInUse());
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Create a sequentialing allocator and a sequentialing pool, and
        //   initialize each with its own instance of test allocator.  Request
        //   memory of varying sizes from both the sequentialing allocator and
        //   the sequentialing pool.  Verify that both test allocators contain
        //   the same number of bytes in use and the same total number of bytes
        //   requested.
        //
        // Testing:
        //   bslma::SequentialAllocator(basicAllocator);
        //   ~bslma::SequentialAllocator();
        //   void *allocate(numBytes);
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing 'allocate', 'deallocate' and 'release'."
                          << endl;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator sequentialAllocatorTA(veryVeryVerbose);
        bslma::TestAllocator sequentialPoolTA(veryVeryVerbose);

        bslma::SequentialAllocator sa(&sequentialAllocatorTA);
        bslma::SequentialPool sp(&sequentialPoolTA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            sa.allocate(SIZE);
            sp.allocate(SIZE);
            LOOP_ASSERT(i, sequentialAllocatorTA.numBytesInUse()
                           == sequentialPoolTA.numBytesInUse());
        }

        sa.release();
        sp.release();
        ASSERT(0 == sequentialAllocatorTA.numBytesInUse());
        ASSERT(0 == sequentialPoolTA.numBytesInUse());
        ASSERT(sequentialAllocatorTA.numBytesTotal() ==
                                             sequentialPoolTA.numBytesTotal());

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
