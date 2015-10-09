// bdlma_buffermanager.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_buffermanager.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bslma_default.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// A 'bdlma::BufferManager' is a mechanism (i.e., having state but no value)
// that is used as a memory manager to manage an external buffer.  The primary
// concern is that 'bdlma::BufferManager' returns memory from an appropriate
// source (the external buffer supplied at construction), and respects the
// appropriate alignment strategy (also specified at construction).
//
// Even though 'bdlma::BufferManager' is a stateful object (where its state is
// determined by the internal cursor pointing to a particular part of the
// external buffer supplied at construction), there are no accessors that can
// precisely indicate the state of the buffer object.  The
// 'hasSufficientCapacity' method can only approximate the position of the
// internal cursor, but cannot pinpoint the position since the method takes
// into account the alignment strategy specified at construction.
//
// As a result, this test driver verifies the state of the buffer *indirectly*
// by making two consecutive allocations -- where the first allocation tests
// for the correctness of 'allocate', and the second verifies the internal
// state of the buffer object.
//
// Negative testing (for precondition violations) is also part of the test
// driver.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 2] bdlma::BufferManager(AlignmentStrategy s = NA);
// [ 2] bdlma::BufferManager(char *buf, int sz, AlignmentStrategy s = NA);
// [ 2] ~bdlma::BufferManager();
//
// // MANIPULATORS
// [ 3] void *allocate(size_type size);
// [ 3] void *allocateRaw(int size);
// [ 8] void deleteObjectRaw(const TYPE *object);
// [ 8] void deleteObject(const TYPE *object);
// [ 9] int expand(void *address, int size);
// [ 4] char *replaceBuffer(char *newBuffer, int newBufferSize);
// [ 5] void release();
// [ 6] void reset();
// [10] int truncate(void *address, int originalSize, int newSize);
//
// // ACCESSORS
// [ 2] char *buffer() const;
// [ 2] int bufferSize() const;
// [ 7] bool hasSufficientCapacity(int size) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

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

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlma::BufferManager      Obj;

typedef bsls::Alignment::Strategy Strat;

// On Windows, when an 8-byte aligned object is created on the stack, it
// actually gets aligned on a 4-byte boundary.  To work around this, create a
// static buffer instead.

enum { k_BUFFER_SIZE = 256 };
static bsls::AlignedBuffer<k_BUFFER_SIZE> bufferStorage;

enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

bool globalDestructorInvoked = false;

class my_Class {
    // This object indicates when its destructor is called by setting
    // 'globalDestructorInvoked' to 'true'.

  public:
    ~my_Class()
        // Destroy this object and set 'globalDestructorInvoked' to 'true'.
    {
        globalDestructorInvoked = true;
    }
};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// Suppose that we need to detect whether there are at least 'n' duplicates
// within an array of integers.  Furthermore, suppose that speed is a concern
// and we need the fastest possible implementation.  A natural solution will be
// to use a hash table.  To further optimize for speed, we can use a custom
// memory manager, such as 'bdlma::BufferManager', to speed up memory
// allocations.
//
// First, let's define the structure of a node inside our custom hash table
// structure:
//..
    struct my_Node {
        // This struct represents a node within a hash table.

        // DATA
        int      d_value;   // integer value this node holds
        int      d_count;   // number of occurrences of this integer value
        my_Node *d_next_p;  // pointer to the next node

        // CREATORS
        my_Node(int value, my_Node *next);
            // Create a node having the specified 'value' that refers to the
            // specified 'next' node.
    };

    // CREATORS
    my_Node::my_Node(int value, my_Node *next)
    : d_value(value)
    , d_count(1)
    , d_next_p(next)
    {
    }
//..
// Note that 'sizeof(my_Node) == 12' when compiled in 32-bit mode, and
// 'sizeof(my_Node) == 16' when compiled in 64-bit mode.  This difference
// affects the amount of memory used under different alignment strategies (see
// 'bsls_alignment' for more details on alignment strategies).
//
// We can then define the structure of our specialized hash table used for
// integer counting:
//..
    class my_IntegerCountingHashTable {
        // This class represents a hash table that is used to keep track of the
        // number of occurrences of various integers.  Note that this is a
        // highly specialized class that uses a 'bdlma::BufferManager' with
        // sufficient memory for memory allocations.

        // DATA
        my_Node              **d_nodeArray;  // array of 'my_Node' pointers

        int                    d_size;       // size of the node array

        bdlma::BufferManager  *d_buffer;     // buffer manager (held, not
                                             // owned)

      public:
        // CLASS METHODS
        static int calculateBufferSize(int tableLength, int numNodes);
            // Return the memory required by a 'my_IntegerCountingHashTable'
            // that has the specified 'tableLength' and 'numNodes'.

        // CREATORS
        my_IntegerCountingHashTable(int size, bdlma::BufferManager *buffer);
            // Create a hash table of the specified 'size', using the specified
            // 'buffer' to supply memory.  The behavior is undefined unless
            // '0 < size', 'buffer' is non-zero, and 'buffer' has sufficient
            // memory to support all memory allocations required.

        // ...

        // MANIPULATORS
        int insert(int value);
            // Insert the specified 'value' with a count of 1 into this hash
            // table if 'value' does not currently exist in the hash table, and
            // increment the count for 'value' otherwise.  Return the number of
            // occurrences of 'value' in this hash table.

        // ...
    };
//..
// The implementation of the rest of 'my_IntegerCountingHashTable' is elided as
// the class method 'calculateBufferSize', constructor, and the 'insert' method
// alone are sufficient to illustrate the use of 'bdlma::BufferManager':
//..
    // CLASS METHODS
    int my_IntegerCountingHashTable::calculateBufferSize(int tableLength,
                                                         int numNodes)
    {
        return static_cast<int>(tableLength * sizeof(my_Node *)
                              + numNodes * sizeof(my_Node)
                              + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
    }
//..
// Note that, in case the allocated buffer is not aligned, the size calculation
// includes a "fudge" factor equivalent to the maximum alignment requirement of
// the platform.
//..
    // CREATORS
    my_IntegerCountingHashTable::my_IntegerCountingHashTable(
                                                  int                   size,
                                                  bdlma::BufferManager *buffer)
    : d_size(size)
    , d_buffer(buffer)
    {
        // 'd_buffer' must have sufficient memory to satisfy the allocation
        // request (as specified by the constructor's contract).

        d_nodeArray = static_cast<my_Node **>(
                               d_buffer->allocate(d_size * sizeof(my_Node *)));

        bsl::memset(d_nodeArray, 0, d_size * sizeof(my_Node *));
    }

    // MANIPULATORS
    int my_IntegerCountingHashTable::insert(int value)
    {
        // Naive hash function using only mod.

        const int hashValue = value % d_size;
        my_Node **tmp       = &d_nodeArray[hashValue];

        while (*tmp) {
            if ((*tmp)->d_value != value) {
                tmp = &((*tmp)->d_next_p);
            }
            else {
                return ++((*tmp)->d_count);                           // RETURN
            }
        }

        // 'allocate' does not trigger dynamic memory allocation.  Therefore,
        // we don't have to worry about exceptions and can use placement 'new'
        // directly with 'allocate'.  'd_buffer' must have sufficient memory to
        // satisfy the allocation request (as specified by the constructor's
        // contract).

        *tmp = new(d_buffer->allocate(sizeof(my_Node))) my_Node(value, *tmp);

        return 1;
    }
//..
// Note that 'bdlma::BufferManager' is used to allocate memory blocks of
// heterogeneous sizes.  In the constructor, memory is allocated for the node
// array.  In 'insert', memory is allocated for the nodes.
//
// Finally, in the following 'detectNOccurrences' function, we can use the hash
// table class to detect whether any integer value occurs at least 'n' times
// within a specified array:
//..
    bool detectNOccurrences(int n, const int *array, int length)
        // Return 'true' if any integer value in the specified 'array' having
        // the specified 'length' appears at least the specified 'n' times, and
        // 'false' otherwise.
    {
        const int MAX_SIZE = my_IntegerCountingHashTable::
                                           calculateBufferSize(length, length);
//..
// We then allocate an external buffer to be used by 'bdlma::BufferManager'.
// Normally, this buffer will be created on the program stack if we know the
// length in advance (for example, if we specify in the contract of this
// function that we only handle arrays having a length of up to 10,000
// integers).  However, to make this function more general, we decide to
// allocate the memory dynamically.  This approach is still much more efficient
// than using the default allocator, say, to allocate memory for individual
// nodes within 'insert', since we need only a single dynamic allocation,
// versus separate dynamic allocations for every single node:
//..
        bslma::Allocator *allocator = bslma::Default::defaultAllocator();
        char *buffer = static_cast<char *>(allocator->allocate(MAX_SIZE));
//..
// We use a 'bslma::DeallocatorGuard' to automatically deallocate the buffer
// when the function ends:
//..
        bslma::DeallocatorGuard<bslma::Allocator> guard(buffer, allocator);

        bdlma::BufferManager bufferManager(buffer, MAX_SIZE);
        my_IntegerCountingHashTable table(length, &bufferManager);

        while (--length >= 0) {
            if (n == table.insert(array[length])) {
                return true;                                          // RETURN
            }
        }

        return false;
    }
//..
// Note that the calculation of 'MAX_SIZE' assumes natural alignment.  If
// maximum alignment is used instead, a larger buffer is needed since each node
// object will then be maximally aligned, which takes up 16 bytes each instead
// of 12 bytes on a 32-bit architecture.  On a 64-bit architecture, there will
// be no savings using natural alignment since the size of a node will be 16
// bytes regardless.

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
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        const int array[5] = { 1, 2, 4, 2, 3 };

        bool result = detectNOccurrences(2, array, 5);
        ASSERT(true  == result);

        result = detectNOccurrences(3, array, 5);
        ASSERT(false == result);

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TRUNCATE TEST
        //
        // Concerns:
        //   1. That 'truncate' reduces the amount of memory allocated to the
        //      specified 'newSize', and returns 'newSize' on success.
        //
        //   2. That when 'truncate' fails, 'originalSize' is returned.
        //
        //   3. QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concern 1, using the table-driven technique, create test
        //   vectors having an alignment strategy, initial allocation size,
        //   new size, and expected offset.  First allocate memory of the
        //   initial allocation size, then truncate to the new size, then
        //   allocate memory (1 byte) again.  Verify that the second allocation
        //   matches the expected offset.  Also verify the return value after
        //   truncating.
        //
        //   For concern 2, attempt to truncate the memory returned by the
        //   initial allocation, and verify that the return value is
        //   'originalSize'.
        //
        //   For concern 3, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   int truncate(void *address, int originalSize, int newSize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TRUNCATE TEST" << endl
                                  << "=============" << endl;

        char *buffer = bufferStorage.buffer();

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nTesting 'truncate'." << endl;

        static const struct {
            int   d_line;         // line number
            Strat d_strategy;     // alignment strategy
            int   d_initialSize;  // size of initial allocation request
            int   d_newSize;      // truncate size
            int   d_expOffset;    // expected memory offset
        } DATA[] = {
            // LINE     STRAT       INITIALSIZE   NEWSIZE   EXPOFFSET
            // ----     -----       -----------   -------   ---------

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,           0,            0 },
            {  L_,      NAT,           1,           1,            1 },
            {  L_,      NAT,           2,           0,            0 },
            {  L_,      NAT,           2,           1,            1 },
            {  L_,      NAT,           2,           2,            2 },
            {  L_,      NAT,           3,           0,            0 },
            {  L_,      NAT,           3,           1,            1 },
            {  L_,      NAT,           3,           2,            2 },
            {  L_,      NAT,           3,           3,            3 },
            {  L_,      NAT,           8,           4,            4 },
            {  L_,      NAT,          15,          15,           15 },
            {  L_,      NAT,          16,           0,            0 },
            {  L_,      NAT,          16,          15,           15 },
            {  L_,      NAT,          16,          16,           16 },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,           0,            0 },
            {  L_,      MAX,           1,           1,  k_MAX_ALIGN },
            {  L_,      MAX,           2,           0,            0 },
            {  L_,      MAX,           2,           1,  k_MAX_ALIGN },
            {  L_,      MAX,           2,           2,  k_MAX_ALIGN },
            {  L_,      MAX,           3,           0,            0 },
            {  L_,      MAX,           3,           1,  k_MAX_ALIGN },
            {  L_,      MAX,           3,           2,  k_MAX_ALIGN },
            {  L_,      MAX,           3,           3,  k_MAX_ALIGN },
            {  L_,      MAX,           8,           4,  k_MAX_ALIGN },
            {  L_,      MAX,          15,          15,           16 },
            {  L_,      MAX,          16,           0,            0 },
            {  L_,      MAX,          16,           1,  k_MAX_ALIGN },
            {  L_,      MAX,          16,          15,           16 },
            {  L_,      MAX,          16,          16,           16 },

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,           1,           0,            0 },
            {  L_,      BYT,           1,           1,            1 },
            {  L_,      BYT,           2,           0,            0 },
            {  L_,      BYT,           2,           1,            1 },
            {  L_,      BYT,           2,           2,            2 },
            {  L_,      BYT,           3,           0,            0 },
            {  L_,      BYT,           3,           1,            1 },
            {  L_,      BYT,           3,           2,            2 },
            {  L_,      BYT,           3,           3,            3 },
            {  L_,      BYT,           8,           4,            4 },
            {  L_,      BYT,          15,          15,           15 },
            {  L_,      BYT,          16,           0,            0 },
            {  L_,      BYT,          16,           1,            1 },
            {  L_,      BYT,          16,          15,           15 },
            {  L_,      BYT,          16,          16,           16 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;         // line number
            const Strat STRAT       = DATA[ti].d_strategy;     // align. strat.
            const int   INITIALSIZE = DATA[ti].d_initialSize;  // initial size
            const int   NEWSIZE     = DATA[ti].d_newSize;      // new size
            const int   EXPOFFSET   = DATA[ti].d_expOffset;    // exp. offset

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYT
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(INITIALSIZE) P_(NEWSIZE) P(EXPOFFSET)
            }

            Obj mX(buffer, k_BUFFER_SIZE, STRAT);

            void *addr1 = mX.allocate(INITIALSIZE);
            ASSERT(0 != addr1);

            int ret = mX.truncate(addr1, INITIALSIZE, NEWSIZE);
            LOOP2_ASSERT(NEWSIZE, ret, NEWSIZE == ret);

            void *addr2 = mX.allocate(1);
            LOOP3_ASSERT(LINE, EXPOFFSET, addr2,
                         &buffer[0] + EXPOFFSET == addr2);

            // Truncating previously allocated address should fail.
            if (EXPOFFSET != 0
             && ((char *)addr1 + INITIALSIZE) != ((char *)addr2 + 1)) {
                ret = mX.truncate(addr1, INITIALSIZE, NEWSIZE);
                LOOP2_ASSERT(INITIALSIZE, ret, INITIALSIZE == ret);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 != address'" << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE);

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS(mX.truncate(addr, 2, 1));

                ASSERT_SAFE_FAIL(mX.truncate(   0, 1, 0));
            }

            if (veryVerbose) cout << "\t'0 <= newSize'" << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE);

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS(mX.truncate(addr, 2,  1));
                ASSERT_SAFE_PASS(mX.truncate(addr, 1,  0));

                ASSERT_SAFE_FAIL(mX.truncate(addr, 0, -1));
            }

            if (veryVerbose) cout << "\t'newSize <= originalSize'" << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE);

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS(mX.truncate(addr, 2, 2));

                ASSERT_SAFE_FAIL(mX.truncate(addr, 2, 3));
            }
        }

#undef NAT
#undef MAX
#undef BYT

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // EXPAND TEST
        //
        // Concerns:
        //   1. That 'expand' uses up all of the available memory within the
        //      buffer.
        //
        //   2. That 'expand' returns the updated size of the memory used on
        //      success.
        //
        //   3. That 'expand' returns the original 'size' on failure.
        //
        //   4. QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concerns 1 and 2, using the table-driven technique, create
        //   test vectors having an alignment strategy, initial allocation
        //   size, and expected memory used.  First make the initial allocation
        //   of the specified size.  Then allocate 1 byte and call 'expand'.
        //   Verify that the return value of 'expand' is the same as the
        //   expected memory used.  Finally, invoke 'allocate' again and verify
        //   that it returns 0 (i.e., all memory is used).
        //
        //   For concern 3, after invoking 'expand' when testing concerns 1 and
        //   2, invoke 'expand' again on the initial memory allocated.  Verify
        //   that the value returned is the original 'size'.
        //
        //   For concern 4, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   int expand(void *address, int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "EXPAND TEST" << endl
                                  << "===========" << endl;

        char *buffer = bufferStorage.buffer();

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nTesting 'expand'." << endl;

        static const struct {
            int   d_line;         // line number
            Strat d_strategy;     // alignment strategy
            int   d_initialSize;  // size of initial allocation request
            int   d_expused;      // expected memory used after 'expand'
        } DATA[] = {
            // LINE     STRAT       INITIALSIZE       EXPUSED
            // ----     -----       -----------       -------

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,               255 },
            {  L_,      NAT,           2,               254 },
            {  L_,      NAT,           3,               253 },
            {  L_,      NAT,           4,               252 },
            {  L_,      NAT,           7,               249 },
            {  L_,      NAT,           8,               248 },
            {  L_,      NAT,          15,               241 },
            {  L_,      NAT,          16,               240 },
            {  L_,      NAT,         100,               156 },
            {  L_,      NAT,         254,                 2 },
            {  L_,      NAT,         255,                 1 },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1, 256 - k_MAX_ALIGN },
            {  L_,      MAX,           2, 256 - k_MAX_ALIGN },
            {  L_,      MAX,           3, 256 - k_MAX_ALIGN },
            {  L_,      MAX,           4, 256 - k_MAX_ALIGN },
            {  L_,      MAX,           7, 256 - k_MAX_ALIGN },
            {  L_,      MAX,           8, 256 - k_MAX_ALIGN },
            {  L_,      MAX,          15,               240 },
            {  L_,      MAX,          16,               240 },
            {  L_,      MAX,         108,               144 },

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,           1,               255 },
            {  L_,      BYT,           2,               254 },
            {  L_,      BYT,           3,               253 },
            {  L_,      BYT,           4,               252 },
            {  L_,      BYT,           7,               249 },
            {  L_,      BYT,           8,               248 },
            {  L_,      BYT,          15,               241 },
            {  L_,      BYT,          16,               240 },
            {  L_,      BYT,         108,               148 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;
            const Strat STRAT       = DATA[ti].d_strategy;
            const int   INITIALSIZE = DATA[ti].d_initialSize;
            const int   EXPUSED     = DATA[ti].d_expused;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYT
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(INITIALSIZE) P(EXPUSED)
            }

            Obj mX(buffer, k_BUFFER_SIZE, STRAT);

            mX.allocate(INITIALSIZE);
            void *addr = mX.allocate(1);

            ASSERT(0 != addr);

            int newSize = mX.expand(addr, 1);
            LOOP3_ASSERT(LINE, EXPUSED, newSize, EXPUSED == newSize);

            void *addr2 = mX.allocate(1);
            ASSERT(0 == addr2);

            int ret = mX.expand(addr, newSize);
            LOOP2_ASSERT(LINE, ret, newSize == ret);
        }

        // No initial allocation, just allocate and expand directly.
        Obj mX(buffer, k_BUFFER_SIZE);

        void *addr = mX.allocate(1);
        ASSERT(&buffer[0] == addr);

        int newSize = mX.expand(addr, 1);
        ASSERT(k_BUFFER_SIZE == newSize);

        addr = mX.allocate(1);
        ASSERT(0 == addr);

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 != address'" << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE);

                void *addr = mX.allocate(1);

                ASSERT_SAFE_PASS(mX.expand(addr, 1));

                ASSERT_SAFE_FAIL(mX.expand(   0, 4));
            }

            if (veryVerbose) cout << "\t'0 < size'" << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE);

                void *addr = mX.allocate(1);

                ASSERT_SAFE_PASS(mX.expand(addr,  1));

                ASSERT_SAFE_FAIL(mX.expand(addr,  0));
                ASSERT_SAFE_FAIL(mX.expand(addr, -1));
            }
        }

#undef NAT
#undef MAX
#undef BYT

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // DELETEOBJECT TEST
        //
        // Concerns:
        //   1. That both the 'deleteObject' and 'deleteObjectRaw' methods
        //      invoke the destructor of the object passed in.
        //
        // Plan:
        //   Since 'deleteObject' and 'deleteObjectRaw' do not deallocate
        //   memory, we just need to ensure that the destructor of the object
        //   passed in is invoked.
        //
        // Testing:
        //   void deleteObjectRaw(const TYPE *object);
        //   void deleteObject(const TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DELETEOBJECT TEST" << endl
                                  << "=================" << endl;

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting 'deleteObject'." << endl;
        {
            Obj mX(buffer, k_BUFFER_SIZE);

            void *addr = mX.allocate(sizeof(my_Class));
            my_Class *obj = new(addr) my_Class();

            globalDestructorInvoked = false;
            mX.deleteObject(obj);

            ASSERT(true == globalDestructorInvoked);
        }

        if (verbose) cout << "\nTesting 'deleteObjectRaw'." << endl;
        {
            Obj mX(buffer, k_BUFFER_SIZE);

            void *addr = mX.allocate(sizeof(my_Class));
            my_Class *obj = new(addr) my_Class();

            globalDestructorInvoked = false;
            mX.deleteObjectRaw(obj);

            ASSERT(true == globalDestructorInvoked);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'hasSufficientCapacity' TEST
        //
        // Concerns:
        //   1. That 'hasSufficientCapacity' returns 'true' when an allocation
        //      of 'size' bytes can be satisfied, and 'false' otherwise.
        //
        //   2. QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   Since 'allocate' is thoroughly tested for the overflow case, we
        //   can use depth-first enumeration to test this by enumerating buffer
        //   sizes from 1 to 8 and test all two-allocation combinations.
        //   Whenever 'hasSufficientCapacity' returns 'false', the next
        //   allocation request should return 0 (and vice versa).
        //
        //   For concern 2, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   bool hasSufficientCapacity(int size) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'hasSufficientCapacity' TEST" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting 'hasSufficientCapacity'." << endl;

        char *buffer = bufferStorage.buffer();

        for (int al = 0; al <= 2; ++al) {                   // align. strategy
            Strat STRAT = (Strat)al;

            for (int i = 1; i <= 8; ++i) {                  // buffer size
                const int BUFFER_SIZE = i;

                if (veryVerbose) { T_ P(BUFFER_SIZE) }

                for (int j = 1; j <= i; ++j) {              // 1st allocation
                    const int ALLOCSIZE1 = j;

                    if (veryVerbose) { T_ T_ P(ALLOCSIZE1) }

                    for (int k = 1; k <= i - j + 1; ++k) {  // 2nd allocation
                        const int ALLOCSIZE2 = k;

                        if (veryVerbose) { T_ T_ T_ P(ALLOCSIZE2) }

                        Obj mX(buffer, BUFFER_SIZE, STRAT);  const Obj& X = mX;

                        // Initial allocation should succeed.

                        bool  ret  =  X.hasSufficientCapacity(ALLOCSIZE1);
                        void *addr = mX.allocate(ALLOCSIZE1);
                        LOOP_ASSERT(addr, 0 != addr);
                        ASSERT(true == ret);

                        // Subsequent allocation might or might not succeed.
                        // However, it should match the result of
                        // 'hasSufficientCapacity'.

                        ret  =  X.hasSufficientCapacity(ALLOCSIZE2);
                        addr = mX.allocate(ALLOCSIZE2);

                        LOOP2_ASSERT(addr, ret, (0 != addr) == ret);

                    }
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 < size'" << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE);

                ASSERT_SAFE_PASS(mX.hasSufficientCapacity( 1));

                ASSERT_SAFE_FAIL(mX.hasSufficientCapacity( 0));
                ASSERT_SAFE_FAIL(mX.hasSufficientCapacity(-1));
            }

            if (veryVerbose) cout << "\t'0 != buffer()'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.hasSufficientCapacity(1));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'reset' TEST
        //
        // Concerns:
        //   1. That 'reset' removes from the management of the object the
        //      external buffer currently being managed (if any).
        //
        //   2. That 'reset' has no effect on the previously managed buffer.
        //
        //   3. That 'reset' can be called on an object that is not managing
        //      a buffer.
        //
        // Plan:
        //   For concern 1, after the call to 'reset' verify that a buffer is
        //   no longer being managed using the 'buffer' accessor method.
        //
        //   For concern 2, set all bytes in the buffer to '0xA' prior to
        //   calling 'reset'.  Verify that the bytes in the buffer remain '0xA'
        //   after 'reset' is called.
        //
        //   For concern 3, perform a separate test that 'reset' can be called
        //   on a default-constructed object with no (apparent) ill effects.
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'reset' TEST" << endl
                                  << "============" << endl;

        char *buffer = bufferStorage.buffer();

        {
            char bufferRef[k_BUFFER_SIZE];

            memset(buffer,    0xA, k_BUFFER_SIZE);
            memset(bufferRef, 0xA, k_BUFFER_SIZE);

            Obj mX(buffer, k_BUFFER_SIZE);  const Obj& X = mX;

            // Allocate some memory.
            mX.allocate(1);
            mX.allocate(16);

            // Reset the object.
            mX.reset();

            ASSERT(0 == X.buffer());
            ASSERT(0 == X.bufferSize());

            if (verbose)
                cout << "\nTesting that there is no effect on the buffer."
                     << endl;

            ASSERT(0 == memcmp(buffer, bufferRef, k_BUFFER_SIZE));
        }

        {
            Obj mX;  const Obj& X = mX;

            mX.reset();

            ASSERT(0 == X.buffer());
            ASSERT(0 == X.bufferSize());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   1. That 'release' sets the internal cursor of the memory manager
        //      to 0, but retains the external buffer being managed (if any).
        //
        //   2. That 'release' has no effect on the managed buffer.
        //
        //   3. That 'release' can be called on an object that is not managing
        //      a buffer.
        //
        // Plan:
        //   For concern 1, after the call to 'release': (1) verify that the
        //   currently managed buffer is the same as before using the 'buffer'
        //   accessor method, and (2) verify that the entire buffer is
        //   available for reuse by requesting the entire buffer in a single
        //   allocation.
        //
        //   For concern 2, set all bytes in the buffer to '0xA' prior to
        //   calling 'release'.  Verify that the bytes in the buffer remain
        //   '0xA' after 'release' is called.
        //
        //   For concern 3, perform a separate test that 'release' can be
        //   called on a default-constructed object with no (apparent) ill
        //   effects.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting address managed after 'release'."
                          << endl;
        {
            char bufferRef[k_BUFFER_SIZE];

            memset(buffer,    0xA, k_BUFFER_SIZE);
            memset(bufferRef, 0xA, k_BUFFER_SIZE);

            Obj mX(buffer, k_BUFFER_SIZE);  const Obj& X = mX;

            // Allocate some memory.
            mX.allocate(1);
            mX.allocate(16);

            // Release all memory.
            mX.release();

            LOOP2_ASSERT(buffer, X.buffer(), buffer == X.buffer());
            LOOP2_ASSERT(k_BUFFER_SIZE, X.bufferSize(),
                         k_BUFFER_SIZE == X.bufferSize());

            if (verbose)
                cout << "\nTesting that there is no effect on the buffer."
                     << endl;

            ASSERT(0 == memcmp(buffer, bufferRef, k_BUFFER_SIZE));

            void *addr = mX.allocate(k_BUFFER_SIZE);  ASSERT(0 != addr);
        }

        {
            Obj mX;  const Obj& X = mX;

            mX.release();

            ASSERT(0 == X.buffer());
            ASSERT(0 == X.bufferSize());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'replaceBuffer' TEST
        //
        // Concerns:
        //   1. That 'replaceBuffer' replaces the buffer managed by the
        //      'bdlma::BufferManager' object, and that the amount of memory
        //      available for allocation is 'newBufferSize'.
        //
        //   2. That the address returned is the address of the previously
        //      managed buffer (if any).
        //
        //   3. That the previously managed buffer is not changed in any way
        //      by the 'replaceBuffer' call.
        //
        //   4. QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concerns 1 and 2, after the call to 'replaceBuffer', use the
        //   'buffer' accessor method to verify that the address returned is
        //   that of the previously managed buffer.  In addition, attempt to
        //   allocate the entire memory available in the buffer and verify that
        //   the allocation succeeded.
        //
        //   For concern 3, set all bytes in the initial buffer to '0xA' prior
        //   to calling 'replaceBuffer'.  Verify that the bytes in the buffer
        //   remain '0xA' after 'replaceBuffer' is called.
        //
        //   For concern 4, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   char *replaceBuffer(char *newBuffer, int newBufferSize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'replaceBuffer' TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting replace and address returned." << endl;
        {
            enum { k_NEW_BUFFER_SIZE = 512 };

            bsls::AlignedBuffer<k_NEW_BUFFER_SIZE> bufferStorage2;

            char *buffer  = bufferStorage.buffer();   // initial buffer
            char *buffer2 = bufferStorage2.buffer();  // buffer for replacing

            char bufferRef[k_BUFFER_SIZE];  // reference for validating

            memset(buffer,    0xA, k_BUFFER_SIZE);
            memset(bufferRef, 0xA, k_BUFFER_SIZE);

            Obj mX(buffer, k_BUFFER_SIZE);  const Obj& X = mX;
            void *addr1 = mX.allocate(k_BUFFER_SIZE);      ASSERT(0 != addr1);

            char *ret = mX.replaceBuffer(buffer2, k_NEW_BUFFER_SIZE);
            LOOP2_ASSERT((void *)&buffer[0], ret, &buffer[0] == ret);

            LOOP2_ASSERT((void *)&buffer2[0], X.buffer(),
                         &buffer2[0] == X.buffer());
            LOOP2_ASSERT(k_NEW_BUFFER_SIZE, X.bufferSize(),
                         k_NEW_BUFFER_SIZE == X.bufferSize());

            void *addr2 = mX.allocate(k_NEW_BUFFER_SIZE);  ASSERT(0 != addr2);

            if (verbose) cout << "\nTesting that there is no effect on the"
                                 " initial buffer." << endl;

            ASSERT(0 == memcmp(ret, bufferRef, k_BUFFER_SIZE));

            Obj mY;  const Obj& Y = mY;

            ret = mY.replaceBuffer(buffer, k_BUFFER_SIZE);
            ASSERT(0 == ret);

            LOOP2_ASSERT((void *)&buffer[0], Y.buffer(),
                         &buffer[0] == Y.buffer());
            LOOP2_ASSERT(k_BUFFER_SIZE, Y.bufferSize(),
                         k_BUFFER_SIZE == Y.bufferSize());

            void *addr3 = mY.allocate(k_BUFFER_SIZE);  ASSERT(0 != addr3);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            char *buffer = bufferStorage.buffer();

            if (veryVerbose) cout << "\t'0 != newBuffer'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_PASS(mX.replaceBuffer(buffer, k_BUFFER_SIZE));

                ASSERT_SAFE_FAIL(mX.replaceBuffer(     0, k_BUFFER_SIZE));
            }

            if (veryVerbose) cout << "\t'0 < newBufferSize'" << endl;
            {
                Obj mX;

                ASSERT_SAFE_PASS(mX.replaceBuffer(buffer,  1));

                ASSERT_SAFE_FAIL(mX.replaceBuffer(buffer,  0));
                ASSERT_SAFE_FAIL(mX.replaceBuffer(buffer, -1));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ALLOCATE TEST
        //
        // Concerns:
        //   1. That the allocated memory address is correctly aligned
        //      according to the alignment strategy indicated at construction
        //      of the 'bdlma::BufferManager' and comes from the external
        //      buffer also supplied at construction.
        //
        //   2. That when 'allocate' is used and the allocation request causes
        //      the supplied buffer to overflow, 0 is returned.
        //
        //   3. That we can allocate the expected amount of memory from the
        //      buffer, after taking into account alignment effects.
        //
        //   4. QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concern 1, using the table-driven technique, create a set of
        //   test vectors having different alignment strategies, allocation
        //   sizes, and expected addresses of a subsequent allocation (please
        //   see the TEST PLAN for an explanation of why this is done).  Next,
        //   construct a 'bdlma::BufferManager' using a maximally-aligned
        //   buffer and the alignment strategy from the test vector.  Invoke
        //   'allocate' and 'allocateRaw' with an allocation request of 1 byte.
        //   Then allocate a second time with the allocation size specified in
        //   the test vector and verify the results of the 'allocate' and
        //   'allocateRaw' methods with the expected values.
        //
        //   For concern 2, create buffers of different sizes such that, after
        //   an initial allocation, the next allocation request will result
        //   in a buffer overflow.  Verify that 0 is returned.
        //
        //   For concern 3, using the table-driven technique, create a set of
        //   test vectors specifying the buffer size, allocation size,
        //   alignment strategy, and expected number of allocations.  Then keep
        //   allocating until 'allocate' returns 0.
        //
        //   For concern 4, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   void *allocate(size_type size);
        //   void *allocateRaw(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATE TEST" << endl
                                  << "=============" << endl;

        char *buffer = bufferStorage.buffer();

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nTesting 'allocate' and 'allocateRaw'." << endl;
        {

        static const struct {
            int   d_line;       // line number
            Strat d_strategy;   // alignment strategy
            int   d_allocSize;  // size of allocation request
            int   d_expOffset;  // expected address of second allocation
        } DATA[] = {
            // LINE     STRAT       ALLOCSIZE       EXPOFFSET
            // ----     -----       ---------       ---------

            // NATURAL ALIGNMENT
            {  L_,      NAT,         1,                     1 },
            {  L_,      NAT,         2,                     2 },
            {  L_,      NAT,         3,                     1 },
            {  L_,      NAT,         4,                     4 },
            {  L_,      NAT,         7,                     1 },
            {  L_,      NAT,         8,                     8 },
            {  L_,      NAT,        15,                     1 },
            {  L_,      NAT,        16,           k_MAX_ALIGN },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,         1,           k_MAX_ALIGN },
            {  L_,      MAX,         2,           k_MAX_ALIGN },
            {  L_,      MAX,         3,           k_MAX_ALIGN },
            {  L_,      MAX,         4,           k_MAX_ALIGN },
            {  L_,      MAX,         7,           k_MAX_ALIGN },
            {  L_,      MAX,         8,           k_MAX_ALIGN },
            {  L_,      MAX,        15,           k_MAX_ALIGN },
            {  L_,      MAX,        16,           k_MAX_ALIGN },

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,         1,                     1 },
            {  L_,      BYT,         2,                     1 },
            {  L_,      BYT,         3,                     1 },
            {  L_,      BYT,         4,                     1 },
            {  L_,      BYT,         7,                     1 },
            {  L_,      BYT,         8,                     1 },
            {  L_,      BYT,        15,                     1 },
            {  L_,      BYT,        16,                     1 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE      = DATA[ti].d_line;
            const Strat STRAT     = DATA[ti].d_strategy;
            const int   ALLOCSIZE = DATA[ti].d_allocSize;
            const int   EXPOFFSET = DATA[ti].d_expOffset;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYT
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(ALLOCSIZE) P(EXPOFFSET)
            }

            Obj mX(buffer, k_BUFFER_SIZE, STRAT);

            void *addr = mX.allocate(1);
            LOOP3_ASSERT(LINE, (void *)&buffer[0], addr, &buffer[0] == addr);

            addr = mX.allocate(ALLOCSIZE);
            LOOP3_ASSERT(LINE, (void *)&buffer[EXPOFFSET], addr,
                         &buffer[EXPOFFSET] == addr);
        }

        }

        if (verbose) cout << "\nTesting overflowed allocation." << endl;
        {

        static const struct {
            int   d_line;        // line number
            Strat d_strategy;    // alignment strategy
            int   d_bufferSize;  // size of the buffer
            int   d_allocSize1;  // size of allocation request 1
            int   d_allocSize2;  // size of allocation request 2
        } DATA[] = {
            // LINE     STRAT    BUFFERSIZE     ALLOCSIZE1   ALLOCSIZE2
            // ----     -----    ----------     ----------   ----------

            // Natural Alignment
            {  L_,      NAT,     1,             1,            1  },
            {  L_,      NAT,     2,             1,            2  },
            {  L_,      NAT,     2,             2,            1  },
            {  L_,      NAT,     3,             1,            2  },
            {  L_,      NAT,     3,             2,            2  },
            {  L_,      NAT,     3,             3,            1  },
            {  L_,      NAT,     4,             1,            4  },
            {  L_,      NAT,     4,             2,            4  },
            {  L_,      NAT,     4,             3,            2  },
            {  L_,      NAT,     4,             4,            1  },
            {  L_,      NAT,     5,             1,            4  },
            {  L_,      NAT,     5,             2,            4  },
            {  L_,      NAT,     5,             3,            2  },
            {  L_,      NAT,     5,             4,            2  },
            {  L_,      NAT,     5,             4,            4  },
            {  L_,      NAT,     5,             5,            1  },
            {  L_,      NAT,     8,             1,            8  },
            {  L_,      NAT,     8,             2,            8  },
            {  L_,      NAT,     8,             3,            8  },
            {  L_,      NAT,     8,             4,            8  },
            {  L_,      NAT,     8,             5,            4  },
            {  L_,      NAT,     8,             6,            4  },
            {  L_,      NAT,     8,             7,            2  },
            {  L_,      NAT,     8,             8,            1  },

            // Maximum Alignment
            {  L_,      MAX,     1,             1,            1  },
            {  L_,      MAX,     2,             1,            1  },
            {  L_,      MAX,     2,             2,            1  },
            {  L_,      MAX,     3,             1,            1  },
            {  L_,      MAX,     3,             2,            1  },
            {  L_,      MAX,     3,             3,            1  },
            {  L_,      MAX,     4,             1,            1  },
            {  L_,      MAX,     4,             2,            1  },
            {  L_,      MAX,     4,             3,            1  },
            {  L_,      MAX,     4,             4,            1  },
            {  L_,      MAX,     5,             1,            1  },
            {  L_,      MAX,     5,             2,            1  },
            {  L_,      MAX,     5,             3,            1  },
            {  L_,      MAX,     5,             4,            1  },
            {  L_,      MAX,     5,             4,            1  },
            {  L_,      MAX,     5,             5,            1  },
            {  L_,      MAX,     8,             1,            1  },
            {  L_,      MAX,     8,             2,            1  },
            {  L_,      MAX,     8,             3,            1  },
            {  L_,      MAX,     8,             4,            1  },
            {  L_,      MAX,     8,             5,            1  },
            {  L_,      MAX,     8,             6,            1  },
            {  L_,      MAX,     8,             7,            1  },
            {  L_,      MAX,     8,             8,            1  },

            // 1-byte Alignment
            {  L_,      BYT,     1,             1,            1  },
            {  L_,      BYT,     2,             1,            2  },
            {  L_,      BYT,     2,             2,            1  },
            {  L_,      BYT,     3,             1,            3  },
            {  L_,      BYT,     3,             2,            2  },
            {  L_,      BYT,     3,             3,            1  },
            {  L_,      BYT,     4,             4,            1  },
            {  L_,      BYT,     5,             2,            4  },
            {  L_,      BYT,     5,             3,            3  },
            {  L_,      BYT,     5,             5,            1  },
            {  L_,      BYT,     8,             8,            1  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE       = DATA[ti].d_line;
            const Strat STRAT      = DATA[ti].d_strategy;
            const int   BUFFERSIZE = DATA[ti].d_bufferSize;
            const int   ALLOCSIZE1 = DATA[ti].d_allocSize1;
            const int   ALLOCSIZE2 = DATA[ti].d_allocSize2;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYT
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(BUFFERSIZE) P_(ALLOCSIZE1) P(ALLOCSIZE2)
            }

            Obj mX(buffer, BUFFERSIZE, STRAT);
            void *addr = mX.allocate(ALLOCSIZE1);

            LOOP2_ASSERT(LINE, addr, 0 != addr);

            addr = mX.allocate(ALLOCSIZE2);

            LOOP2_ASSERT(LINE, addr, 0 == addr);
        }

        }

        if (verbose) cout << "\nTesting continuous allocation with 'allocate'"
                             " and 'allocateRaw'."
                          << endl;
        {

        static const struct {
            int   d_line;        // line number
            Strat d_strategy;    // alignment strategy
            int   d_bufferSize;  // size of the buffer
            int   d_allocSize;   // size of allocation request
            int   d_expAlloc;    // expected number of allocations
        } DATA[] = {
            // LINE     STRAT    BUFFERSIZE     ALLOCSIZE     EXPALLOC
            // ----     -----    ----------     ---------     --------

            // Natural Alignment
            {  L_,      NAT,     1,             1,                  1 },
            {  L_,      NAT,     1,             2,                  0 },
            {  L_,      NAT,     2,             1,                  2 },
            {  L_,      NAT,     2,             2,                  1 },
            {  L_,      NAT,     4,             1,                  4 },
            {  L_,      NAT,     4,             2,                  2 },
            {  L_,      NAT,     4,             3,                  1 },
            {  L_,      NAT,     4,             4,                  1 },
            {  L_,      NAT,     4,             8,                  0 },
            {  L_,      NAT,     8,             1,                  8 },
            {  L_,      NAT,     8,             2,                  4 },
            {  L_,      NAT,     8,             4,                  2 },
            {  L_,      NAT,     8,             8,                  1 },
            {  L_,      NAT,     8,            16,                  0 },
            {  L_,      NAT,    16,             1,                 16 },
            {  L_,      NAT,    16,             2,                  8 },
            {  L_,      NAT,    16,             4,                  4 },
            {  L_,      NAT,    16,             8,                  2 },
            {  L_,      NAT,    16,            16,                  1 },
            {  L_,      NAT,    16,            32,                  0 },

            // Maximum Alignment
            {  L_,      MAX,     1,             1,                  1 },
            {  L_,      MAX,     1,             2,                  0 },
            {  L_,      MAX,     2,             1,                  1 },
            {  L_,      MAX,     2,             2,                  1 },
            {  L_,      MAX,     4,             1,                  1 },
            {  L_,      MAX,     4,             2,                  1 },
            {  L_,      MAX,     4,             3,                  1 },
            {  L_,      MAX,     4,             4,                  1 },
            {  L_,      MAX,     8,             1,                  1 },
            {  L_,      MAX,     8,             2,                  1 },
            {  L_,      MAX,     8,             4,                  1 },
            {  L_,      MAX,     8,             8,                  1 },
            {  L_,      MAX,    16,             1,   16 / k_MAX_ALIGN },
            {  L_,      MAX,    16,             2,   16 / k_MAX_ALIGN },
            {  L_,      MAX,    16,             4,   16 / k_MAX_ALIGN },
            {  L_,      MAX,    16,             8,   16 / k_MAX_ALIGN },
            {  L_,      MAX,    16,            16,                  1 },
            {  L_,      MAX,    16,            32,                  0 },

            // 1-byte Alignment
            {  L_,      BYT,     1,             1,                  1 },
            {  L_,      BYT,     1,             2,                  0 },
            {  L_,      BYT,     2,             1,                  2 },
            {  L_,      BYT,     2,             2,                  1 },
            {  L_,      BYT,     3,             1,                  3 },
            {  L_,      BYT,     3,             2,                  1 },
            {  L_,      BYT,     3,             3,                  1 },
            {  L_,      BYT,     4,             1,                  4 },
            {  L_,      BYT,     4,             2,                  2 },
            {  L_,      BYT,     4,             3,                  1 },
            {  L_,      BYT,     4,             4,                  1 },
            {  L_,      BYT,     4,             8,                  0 },
            {  L_,      BYT,     5,             1,                  5 },
            {  L_,      BYT,     5,             2,                  2 },
            {  L_,      BYT,     5,             3,                  1 },
            {  L_,      BYT,     5,             4,                  1 },
            {  L_,      BYT,     5,             5,                  1 },
            {  L_,      BYT,     5,             6,                  0 },
            {  L_,      BYT,     8,             1,                  8 },
            {  L_,      BYT,     8,             2,                  4 },
            {  L_,      BYT,     8,             4,                  2 },
            {  L_,      BYT,     8,             8,                  1 },
            {  L_,      BYT,     8,            16,                  0 },
            {  L_,      BYT,    16,             1,                 16 },
            {  L_,      BYT,    16,             2,                  8 },
            {  L_,      BYT,    16,             4,                  4 },
            {  L_,      BYT,    16,             8,                  2 },
            {  L_,      BYT,    16,            16,                  1 },
            {  L_,      BYT,    16,            32,                  0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE       = DATA[ti].d_line;
            const Strat STRAT      = DATA[ti].d_strategy;
            const int   BUFFERSIZE = DATA[ti].d_bufferSize;
            const int   ALLOCSIZE  = DATA[ti].d_allocSize;
            const int   EXPALLOC   = DATA[ti].d_expAlloc;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYT
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(BUFFERSIZE) P_(ALLOCSIZE) P(EXPALLOC)
            }

            // Get the appropriate buffer to use.
            Obj mX(buffer, BUFFERSIZE, STRAT);
            Obj mY(buffer, BUFFERSIZE, STRAT);

            int   count = 0;
            void *addr  = 0;

            while ((addr = mX.allocate(ALLOCSIZE))) {
                void *addr2 = mY.allocateRaw(ALLOCSIZE);

                if (STRAT == NAT) {
                    LOOP2_ASSERT((void *)&buffer[ALLOCSIZE * count], addr,
                                         &buffer[ALLOCSIZE * count] == addr);
                    LOOP2_ASSERT(addr, addr2, addr == addr2);
                }
                else if (STRAT == MAX) {
                    const int INCR = ALLOCSIZE < k_MAX_ALIGN
                                   ? k_MAX_ALIGN
                                   : ALLOCSIZE;

                    LOOP2_ASSERT((void *)&buffer[INCR * count], addr,
                                         &buffer[INCR * count] == addr);
                    LOOP2_ASSERT(addr, addr2, addr == addr2);
                }
                else {  // STRAT == BYT
                    LOOP2_ASSERT((void *)&buffer[ALLOCSIZE * count], addr,
                                         &buffer[ALLOCSIZE * count] == addr);
                    LOOP2_ASSERT(addr, addr2, addr == addr2);
                }
                ++count;
            }

            LOOP3_ASSERT(LINE, EXPALLOC, count, EXPALLOC == count);
        }

        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 < size'" << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE);

                ASSERT_SAFE_PASS(mX.allocate(    1));
                ASSERT_SAFE_PASS(mX.allocateRaw( 1));

                ASSERT_SAFE_FAIL(mX.allocate(    0));
                ASSERT_SAFE_FAIL(mX.allocateRaw( 0));
                ASSERT_SAFE_FAIL(mX.allocateRaw(-1));
            }
        }

#undef NAT
#undef MAX
#undef BYT

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTORS / ACCESSORS TEST
        //
        // Concerns:
        //   1. That a 'bdlma::BufferManager' can be constructed using an
        //      external buffer and the correct buffer size.
        //
        //   2. That the alignment strategy, when not specified at
        //      construction, defaults to natural alignment.
        //
        //   3. That the accessors return the correct address and size of the
        //      currently managed buffer (and 0 when no buffer is being
        //      managed).
        //
        //   4. QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   First, for concerns 1 and 3, construct two buffer objects, one
        //   with an initial buffer and one without.  Then, verify that the
        //   'buffer' and 'bufferSize' methods return the correct address and
        //   size for each buffer object.
        //
        //   For concern 2, create four buffer objects, one without specifying
        //   the alignment strategy and the other three with specifying the
        //   various alignment strategies.  Invoke 'allocate' twice on each
        //   object, and verify, using the result of the second allocation,
        //   that the alignment strategy of the buffer object is correctly
        //   applied.
        //
        //   For concern 4, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   bdlma::BufferManager(AlignmentStrategy s = NA);
        //   bdlma::BufferManager(char *buf, int sz, AlignmentStrategy s = NA);
        //   ~bdlma::BufferManager();
        //   char *buffer() const;
        //   int bufferSize() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTORS / ACCESSORS TEST" << endl
                                  << "======================" << endl;

        enum { k_ALLOC_SIZE1 = 1, k_ALLOC_SIZE2 = 4 };

        char buffer[k_BUFFER_SIZE];

        if (verbose) cout << "\nTesting ctors and accessors." << endl;
        {
            Obj mX(buffer, k_BUFFER_SIZE);  const Obj& X = mX;
            Obj mY;                         const Obj& Y = mY;

            LOOP2_ASSERT(&buffer[0], X.buffer(),
                         &buffer[0] == X.buffer());
            LOOP_ASSERT(Y.buffer(), 0 == Y.buffer());

            LOOP2_ASSERT(k_BUFFER_SIZE, X.bufferSize(),
                         k_BUFFER_SIZE == X.bufferSize());
            LOOP_ASSERT(Y.bufferSize(), 0 == Y.bufferSize());
        }

        if (verbose) cout << "\nTesting default ctor and alignment strategy."
                          << endl;
        {
            Obj mV;
            Obj mW(bsls::Alignment::BSLS_NATURAL);
            Obj mX(bsls::Alignment::BSLS_MAXIMUM);
            Obj mY(bsls::Alignment::BSLS_BYTEALIGNED);

            mV.replaceBuffer(buffer, k_BUFFER_SIZE);
            mW.replaceBuffer(buffer, k_BUFFER_SIZE);
            mX.replaceBuffer(buffer, k_BUFFER_SIZE);
            mY.replaceBuffer(buffer, k_BUFFER_SIZE);

            const int NAT_OFFSET =
                bsls::AlignmentUtil::calculateAlignmentOffset(
                                      &buffer[k_ALLOC_SIZE1],
                                      k_ALLOC_SIZE2);

            const int MAX_OFFSET1 =
                bsls::AlignmentUtil::calculateAlignmentOffset(
                                      &buffer[0],
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

            const int MAX_OFFSET2 =
                bsls::AlignmentUtil::calculateAlignmentOffset(
                                      &buffer[k_ALLOC_SIZE1 + MAX_OFFSET1],
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

            void *addr;

            addr = mV.allocate(k_ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[0], addr, &buffer[0] == addr);

            addr = mV.allocate(k_ALLOC_SIZE2);
            LOOP2_ASSERT(&buffer[k_ALLOC_SIZE1 + NAT_OFFSET], addr,
                         &buffer[k_ALLOC_SIZE1 + NAT_OFFSET] == addr);

            addr = mW.allocate(k_ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[0], addr, &buffer[0] == addr);

            addr = mW.allocate(k_ALLOC_SIZE2);
            LOOP2_ASSERT(&buffer[k_ALLOC_SIZE1 + NAT_OFFSET], addr,
                         &buffer[k_ALLOC_SIZE1 + NAT_OFFSET] == addr);

            addr = mX.allocate(k_ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[MAX_OFFSET1], addr,
                         &buffer[MAX_OFFSET1] == addr);

            addr = mX.allocate(k_ALLOC_SIZE2);
            LOOP2_ASSERT(
                     &buffer[k_ALLOC_SIZE1 + MAX_OFFSET1 + MAX_OFFSET2],
                     addr,
                     &buffer[k_ALLOC_SIZE1 + MAX_OFFSET1 + MAX_OFFSET2] ==
                                                                         addr);

            addr = mY.allocate(k_ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[0], addr, &buffer[0] == addr);

            addr = mY.allocate(k_ALLOC_SIZE2);
            LOOP2_ASSERT(&buffer[k_ALLOC_SIZE1], addr,
                         &buffer[k_ALLOC_SIZE1] == addr);
        }

        if (verbose) cout << "\nTesting 3 arg. ctor and alignment strategy."
                          << endl;
        {
            Obj mV(buffer, k_BUFFER_SIZE);
            Obj mW(buffer, k_BUFFER_SIZE, bsls::Alignment::BSLS_NATURAL);
            Obj mX(buffer, k_BUFFER_SIZE, bsls::Alignment::BSLS_MAXIMUM);
            Obj mY(buffer, k_BUFFER_SIZE, bsls::Alignment::BSLS_BYTEALIGNED);

            const int NAT_OFFSET =
                bsls::AlignmentUtil::calculateAlignmentOffset(
                                      &buffer[k_ALLOC_SIZE1],
                                      k_ALLOC_SIZE2);

            const int MAX_OFFSET1 =
                bsls::AlignmentUtil::calculateAlignmentOffset(
                                      &buffer[0],
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

            const int MAX_OFFSET2 =
                bsls::AlignmentUtil::calculateAlignmentOffset(
                                      &buffer[k_ALLOC_SIZE1 + MAX_OFFSET1],
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

            void *addr;

            addr = mV.allocate(k_ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[0], addr, &buffer[0] == addr);

            addr = mV.allocate(k_ALLOC_SIZE2);
            LOOP2_ASSERT(&buffer[k_ALLOC_SIZE1 + NAT_OFFSET], addr,
                         &buffer[k_ALLOC_SIZE1 + NAT_OFFSET] == addr);

            addr = mW.allocate(k_ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[0], addr, &buffer[0] == addr);

            addr = mW.allocate(k_ALLOC_SIZE2);
            LOOP2_ASSERT(&buffer[k_ALLOC_SIZE1 + NAT_OFFSET], addr,
                         &buffer[k_ALLOC_SIZE1 + NAT_OFFSET] == addr);

            addr = mX.allocate(k_ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[MAX_OFFSET1], addr,
                         &buffer[MAX_OFFSET1] == addr);

            addr = mX.allocate(k_ALLOC_SIZE2);
            LOOP2_ASSERT(
                     &buffer[k_ALLOC_SIZE1 + MAX_OFFSET1 + MAX_OFFSET2],
                     addr,
                     &buffer[k_ALLOC_SIZE1 + MAX_OFFSET1 + MAX_OFFSET2] ==
                                                                         addr);

            addr = mY.allocate(k_ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[0], addr, &buffer[0] == addr);

            addr = mY.allocate(k_ALLOC_SIZE2);
            LOOP2_ASSERT(&buffer[k_ALLOC_SIZE1], addr,
                         &buffer[k_ALLOC_SIZE1] == addr);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 != buffer'" << endl;
            {
                ASSERT_SAFE_PASS(Obj(buffer, k_BUFFER_SIZE));

                ASSERT_SAFE_FAIL(Obj(     0, k_BUFFER_SIZE));
            }

            if (veryVerbose) cout << "\t'0 < bufferSize'" << endl;
            {
                ASSERT_SAFE_PASS(Obj(buffer,  1));

                ASSERT_SAFE_FAIL(Obj(buffer,  0));
                ASSERT_SAFE_FAIL(Obj(buffer, -1));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. That a 'bdlma::BufferManager' can be created and destroyed.
        //
        //   2. That 'allocate' returns a block of memory having the specified
        //      size and alignment.
        //
        //   3. That 'allocate' returns a block of memory from the external
        //      buffer supplied at construction.
        //
        // Plan:
        //   First, initialize a 'bdlma::BufferManager' with a 'static' buffer.
        //   Next, allocate a block of memory from the buffer object and verify
        //   that it comes from the external buffer.  Then, allocate another
        //   block of memory from the buffer object, and verify that the first
        //   allocation returned a block of memory of sufficient size by
        //   checking that 'addr2 >= addr1 + k_ALLOC_SIZE1'.  Also verify that
        //   the alignment strategy indicated at construction is followed by
        //   checking the address of the second allocation.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        enum { k_ALLOC_SIZE1 = 4, k_ALLOC_SIZE2 = 8 };

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting constructor." << endl;

        Obj mX(buffer, k_BUFFER_SIZE);  const Obj& X = mX;
        ASSERT(&buffer[0]  == X.buffer());
        ASSERT(k_BUFFER_SIZE == X.bufferSize());

        if (verbose) cout << "\nTesting allocate." << endl;

        void *addr1 = mX.allocate(k_ALLOC_SIZE1);
        ASSERT(&buffer[0] == addr1);
        LOOP2_ASSERT((void *)&buffer[k_BUFFER_SIZE - 1], addr1,
                             &buffer[k_BUFFER_SIZE - 1] >= addr1);

        void *addr2 = mX.allocate(k_ALLOC_SIZE2);
        ASSERT(&buffer[0] < addr2);
        ASSERT(&buffer[k_BUFFER_SIZE - 1] >= addr2);
        ASSERT((char *)addr2 >= (char *)addr1 + k_ALLOC_SIZE1);

        LOOP2_ASSERT((void *)&buffer[8], addr2, &buffer[8] == addr2);

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
