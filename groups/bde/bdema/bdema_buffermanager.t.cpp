// bdema_buffermanager.t.cpp                                          -*-C++-*-
#include <bdema_buffermanager.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdema_BufferManager' is a mechanism (i.e., having state but no value)
// that is used as a memory manager to manage an external buffer.  The primary
// concern is that 'bdema_BufferManager' returns memory from an appropriate
// source (the external buffer supplied at construction), and respect the
// appropriate alignment strategy (also specified at construction).
//
// Even though 'bdema_BufferManager' is a stateful object (where its state is
// determined by the internal cursor pointing to a particular part of the
// external buffer supplied at construction), there are no accessors that can
// exactly indicate the state of the buffer object.  The
// 'hasSufficientCapacity' method can only approximate the position of the
// internal cursor, but cannot pinpoint the position since the method takes
// into account of the alignment strategy specified at construction.
//
// As a result, this test driver verifies the state of the buffer *indirectly*
// by making two consecutive allocations - where the first allocation tests
// for the correctness of 'allocate', and the second verifies the internal
// state of the buffer object.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 2] bdema_BufferManager(AlignmentStrategy s = NA)
// [ 2] bdema_BufferManager(char * b, int i, AlignmentStrategy s = NA)
// [  ] ~bdema_BufferManager()
//
// // MANIPULATORS
// [ 3] void *allocate(int size)
// [ 3] void *allocateRaw(int size)
// [ 7] void deleteObjectRaw(const TYPE *object)
// [ 7] void deleteObject(const TYPE *object)
// [ 8] int expand(void *address, int size)
// [ 4] char *replaceBuffer(char *newBuffer, int newSize)
// [ 5] void release()
// [ 9] int truncate(void *address, int originalSize, int newSize)
//
// // ACCESSORS
// [ 2] char *buffer() const
// [ 2] int bufferSize() const
// [ 6] bool hasSufficientCapacity(int size) const
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------
typedef bdema_BufferManager Obj;

bool globalDestructorInvoked = false;

// On windows, when a 8-byte aligned object is created on the stack, it
// actually get aligned on 4-byte boundary.  To fix this, create a static
// buffer instead.
enum { BUFFER_SIZE = 256 };
static bsls_AlignedBuffer<BUFFER_SIZE> bufferStorage;

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

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
// Suppose we need to detect whether there are 'n' duplicates within an array
// of integers.  Furthermore, suppose speed is a concern and we need the
// fastest possible implementation.  A natural solution will be to use a hash
// table.  To further optimize the speed, we can use a custom memory manager,
// such as 'bdema_BufferManager', to speed up memory allocations.
//
// First, let's define the structure of a node inside the custom hash table
// structure:
//..
    struct my_Node {
        // This struct represents a node within a hash table.

        // DATA
        int      d_value;   // integer value this node holds
        int      d_count;   // number of occurences of this integer value
        my_Node *d_next_p;  // pointer to the next node

        // CREATORS
        my_Node(int value);
            // Create a node with the specified 'value'.
    };

    // CREATORS
    my_Node::my_Node(int value)
    : d_value(value)
    , d_count(1)
    , d_next_p(0)
    {
    }
//..
// Note that the above 'my_Node' structure is 12 bytes when compiled under
// 32-bit mode, and 16 bytes when compiled under 64-bit mode.  This difference
// affects the amount of memory saved under different alignment strategies (see
// 'bsls_alignment' for more details on alignment strategies).
//
// We can then define the structure of our speciailized hash table used for
// integer counting:
//..
    class my_IntegerCountingHashTable {
        // This class represents a hash table that is used to keep track of the
        // number of occurences of various integers.  Note that this is a
        // highly specialized class that uses a 'bdema_BufferManager' with
        // sufficient memory for memory allocations.

        // DATA
        my_Node      **d_nodeArray;  // an array of 'my_Node' pointers
        int            d_size;       // size of the node array
        bdema_BufferManager  *d_buffer;     // memory manager (held, not owned)

      public:
        // CLASS METHODS
        static int calculateBufferSize(int tableLength, int numNodes);
            // Return the memory required by a 'my_IntegerCountingHashTable'
            // that has the specified 'tableLength' and 'numNodes'.

        // CREATORS
        my_IntegerCountingHashTable(int size, bdema_BufferManager *buffer);
            // Create a hash table of the specified 'size', using the specified
            // 'buffer' to supply memory.  The behavior is undefined unless
            // '0 < size', 'buffer' is non-zero, and 'buffer' has sufficient
            // memory to support all memory allocations required.

        // ...

        // MANIPULATORS
        int insert(int value);
            // Insert the specified 'value' with a count of 1 into this hash
            // table if 'value' does not currently exist in the hash table, or
            // increment the count for 'value' if it already exists.  Return
            // the number of occurences of 'value'.

        // ...
    };
//..
// The implementation of the rest of 'my_IntegerCountingHashTable' is elided as
// the class method 'calculateBufferSize', constructor and the 'insert' method
// alone are sufficient to illustrate the use of 'bdema_BufferManager':
//..
    // CLASS METHODS
    int my_IntegerCountingHashTable::calculateBufferSize(int tableSize,
                                                         int numNodes)
    {
        return tableSize * sizeof(my_Node *) + numNodes * sizeof(my_Node)
                                      + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
    }
//..
// Note that, in case the allocated buffer is not aligned, the size calculation
// includes a "fudge" factor equivalent to the maximum alignment requirement of
// the platform.
//..
    // CREATORS
    my_IntegerCountingHashTable::my_IntegerCountingHashTable(
                                                   int           size,
                                                   bdema_BufferManager *buffer)
    : d_size(size)
    , d_buffer(buffer)
    {
        // 'd_buffer' must have sufficient memory to satisfy the allocation
        // request (specified by the constructor's contract).

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
                return ++((*tmp)->d_count);
            }
        }

        // 'allocate' does not trigger dynamic memory allocation, therefore
        // we don't have to worry about exceptions and can use placement 'new'
        // directly with 'allocate'.  'd_buffer' must have sufficient memory to
        // satisfy the allocation request (specified by the constructor's
        // contract).

        *tmp = new(d_buffer->allocate(sizeof(my_Node))) my_Node(value);

        return 1;
    }
//..
// Note that 'bdema_BufferManager' is used to allocate memory blocks of
// heterogenous sizes.  In the constructor, memory is allocated for the node
// array.  In 'insert', memory is allocated for the nodes.
//
// Finally, in the following 'detectNOccurrences' function, we can use the hash
// table class to detect whether any integer values at least occured 'n' times
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
//
// We then allocate an external buffer to be used by 'bdema_BufferManager'.
// Normally, this buffer will be created on the program stack if we know the
// length in advance (for example, if we specify in the contract of this
// function we only handle arrays having a length of up to 10,000 integers).
// To make this function more general, we decide to allocate the memory
// dynamically.  This approach is still much more efficient than using the
// default allocator, as we only need a single dynamic allocation, versus
// dynamic allocations for every single node:
//..
        bslma_Allocator *alloc = bslma_Default::defaultAllocator();
        char *buffer = static_cast<char *>(alloc->allocate(MAX_SIZE));
//..
// We can use a 'bslma_DeallocatorGuard' to automatically deallocate the buffer
// when the function ends:
//..
        bslma_DeallocatorGuard<bslma_Allocator> guard(buffer, alloc);

        bdema_BufferManager bufferManager(buffer, MAX_SIZE);
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
// object will then be aligned maximally, which then takes up 16 bytes each
// instead of 12 bytes on a 32-bit architecture.  On a 64-bit architecture,
// there will be no savings using natural alignment since the size of a node
// will be 16 bytes regardless.

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Copy usage example from the header file, uncomment the code and
        //   change all 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        int array[5] = { 1, 2, 4, 2, 3 };

        bool result = detectNOccurrences(2, array, 5);
        ASSERT(true  == result);

        result = detectNOccurrences(3, array, 5);
        ASSERT(false == result);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TRUNCATE TEST
        //
        // Concerns:
        //   1. That 'truncate' reduces the amount of memory allocated to the
        //      specified 'newSize', and returns 'newSize' on success.
        //
        //   2. That when 'truncate' fails, 'originalSize' is returned.
        //
        // Plan:
        //   For concern 1, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial allocation size,
        //   the new size, and expected offset.  First allocate memory of
        //   initial allocation size, then truncate to the new size, and
        //   allocate memory (1 byte) again.  Verify that the latest allocation
        //   matches the expected offset.  Also verify the return value after
        //   truncating.
        //
        //   For concern 2, truncate the memory returned by the initial
        //   allocation, and verify that the return value is 'originalSize'.
        //
        // Testing:
        //   int truncate(void *address, int originalSize, int newSize)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TRUNCATE TEST" << endl
                                  << "==============" << endl;

        typedef bsls_Alignment::Strategy St;

        enum { MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

        char *buffer = bufferStorage.buffer();

#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting 'truncate'." << endl;

        static const struct {
            int d_line;         // line number
            St  d_strategy;     // alignment strategy
            int d_initialSize;  // size of initial allocation request
            int d_newSize;      // truncate size
            int d_expOffset;    // expected memory offset
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
            {  L_,      MAX,           1,           1,    MAX_ALIGN },
            {  L_,      MAX,           2,           0,            0 },
            {  L_,      MAX,           2,           1,    MAX_ALIGN },
            {  L_,      MAX,           2,           2,    MAX_ALIGN },
            {  L_,      MAX,           3,           0,            0 },
            {  L_,      MAX,           3,           1,    MAX_ALIGN },
            {  L_,      MAX,           3,           2,    MAX_ALIGN },
            {  L_,      MAX,           3,           3,    MAX_ALIGN },
            {  L_,      MAX,           8,           4,    MAX_ALIGN },
            {  L_,      MAX,          15,          15,           16 },
            {  L_,      MAX,          16,           0,            0 },
            {  L_,      MAX,          16,           1,    MAX_ALIGN },
            {  L_,      MAX,          16,          15,           16 },
            {  L_,      MAX,          16,          16,           16 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;         // line number
            const St  STRAT       = DATA[ti].d_strategy;     // alignment strat
            const int INITIALSIZE = DATA[ti].d_initialSize;  // initial size
            const int NEWSIZE     = DATA[ti].d_newSize;      // new size
            const int EXPOFFSET   = DATA[ti].d_expOffset;    // expected offset

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(INITIALSIZE) P_(NEWSIZE) P(EXPOFFSET)
            }

            Obj bufferObject(buffer, BUFFER_SIZE, STRAT);

            void *addr1 = bufferObject.allocate(INITIALSIZE);
            ASSERT(0 != addr1);

            int ret = bufferObject.truncate(addr1, INITIALSIZE, NEWSIZE);
            LOOP2_ASSERT(NEWSIZE, ret, NEWSIZE == ret);

            void *addr2 = bufferObject.allocate(1);
            LOOP3_ASSERT(LINE, EXPOFFSET, addr2,
                                              &buffer[0] + EXPOFFSET == addr2);

            // Truncating previously allocated address should fail.
            if (EXPOFFSET != 0 &&
                        ((char *)addr1 + INITIALSIZE) != ((char *)addr2 + 1)) {
                ret = bufferObject.truncate(addr1, INITIALSIZE, NEWSIZE);
                LOOP2_ASSERT(INITIALSIZE, ret, INITIALSIZE == ret);
            }
        }

#undef MAX
#undef NAT

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // EXPAND TEST
        //
        // Concerns:
        //   1) That 'expand' uses up all the available memory within the
        //      buffer.
        //
        //   2) That 'expand' returns the updated size of memory used on
        //      success.
        //
        //   3) That 'expand' returns the original 'size' on failure.
        //
        // Plan:
        //   For concerns 1 and 2, using the table-driven technique, create
        //   test vectors having the alignment strategy, initial memory offset
        //   and expected memory used.  First allocate memory necessary for the
        //   initial memory offset.  Then allocate 1 byte and call 'expand'.
        //   Verify the return value of 'expand' is the same as the expected
        //   memory used.  Finally, invoke 'allocate' again and verify it
        //   returns 0 (i.e., all memory is used).
        //
        //   For concern 3, after invoking 'expand' when testing concerns 1 and
        //   2, invoke 'expand' again on the initial memory allocated.  Verify
        //   that the adddress returned is the original 'size'.
        //
        // Testing:
        //   int expand(void *address, int size)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "EXPAND TEST" << endl
                                  << "===========" << endl;

        typedef bsls_Alignment::Strategy St;

        enum { MAX_ALIGN   = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

        char *buffer = bufferStorage.buffer();

#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting 'expand'." << endl;

        static const struct {
            int d_line;         // line number
            St  d_strategy;     // alignment strategy
            int d_initialSize;  // size of initial allocation request
            int d_expused;      // expected memory used after 'expand'
        } DATA[] = {
            // LINE     STRAT       INITIALSIZE       EXPUSED
            // ----     -----       -----------       -------

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,             255 },
            {  L_,      NAT,           2,             254 },
            {  L_,      NAT,           3,             253 },
            {  L_,      NAT,           4,             252 },
            {  L_,      NAT,           7,             249 },
            {  L_,      NAT,           8,             248 },
            {  L_,      NAT,          15,             241 },
            {  L_,      NAT,          16,             240 },
            {  L_,      NAT,         100,             156 },
            {  L_,      NAT,         254,               2 },
            {  L_,      NAT,         255,               1 },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1, 256 - MAX_ALIGN },
            {  L_,      MAX,           2, 256 - MAX_ALIGN },
            {  L_,      MAX,           3, 256 - MAX_ALIGN },
            {  L_,      MAX,           4, 256 - MAX_ALIGN },
            {  L_,      MAX,           7, 256 - MAX_ALIGN },
            {  L_,      MAX,           8, 256 - MAX_ALIGN },
            {  L_,      MAX,          15,             240 },
            {  L_,      MAX,          16,             240 },
            {  L_,      MAX,         108,             144 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const St  STRAT       = DATA[ti].d_strategy;
            const int INITIALSIZE = DATA[ti].d_initialSize;
            const int EXPUSED     = DATA[ti].d_expused;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(INITIALSIZE) P(EXPUSED)
            }

            Obj bufferObject(buffer, BUFFER_SIZE, STRAT);

            bufferObject.allocate(INITIALSIZE);
            void *addr = bufferObject.allocate(1);

            ASSERT(0 != addr);

            int newSize = bufferObject.expand(addr, 1);
            LOOP3_ASSERT(LINE, EXPUSED, newSize, EXPUSED == newSize);

            void *addr2 = bufferObject.allocate(1);
            ASSERT(0 == addr2);

            int ret = bufferObject.expand(addr, 1);
            LOOP2_ASSERT(LINE, ret, 1 == ret);
        }

        // No initial allocation, just allocate and expand directly.
        Obj bufferObject(buffer, BUFFER_SIZE);

        void *addr = bufferObject.allocate(1);
        ASSERT(&buffer[0] == addr);

        int newSize = bufferObject.expand(addr, 1);
        ASSERT(BUFFER_SIZE == newSize);

        addr = bufferObject.allocate(1);
        ASSERT(0 == addr);

#undef MAX
#undef NAT

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // DELETE TEST
        //
        // Concerns:
        //   1) Both 'deleteObject' and 'deleteObjectRaw' methods invokes the
        //      destructor of the object passed in.
        //
        // Plan:
        //   Since 'deleteObject' and 'deleteObjectRaw' does not deallocate
        //   memory, we just need to ensure that the destructor of the object
        //   passed in is invoked.
        //
        // Testing:
        //   void deleteObjectRaw(const TYPE *object)
        //   void deleteObject(const TYPE *object)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DELETE TEST" << endl
                                  << "===========" << endl;

        if (verbose) cout << "\nTesting 'deleteObject'." << endl;
        {
            char buf[256];
            Obj bufferObject(buf, 256);

            void *addr = bufferObject.allocate(sizeof(my_Class));
            my_Class *obj = new(addr)my_Class();

            globalDestructorInvoked = false;
            bufferObject.deleteObject(obj);

            ASSERT(true == globalDestructorInvoked);
        }

        if (verbose) cout << "\nTesting 'deleteObjectRaw'." << endl;
        {
            char buf[256];
            Obj bufferObject(buf, 256);

            void *addr = bufferObject.allocate(sizeof(my_Class));
            my_Class *obj = new(addr)my_Class();

            globalDestructorInvoked = false;
            bufferObject.deleteObjectRaw(obj);

            ASSERT(true == globalDestructorInvoked);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'hasSufficientCapacity' TEST
        //
        // Concerns:
        //   1) That 'hasSufficientCapacity' returns 'true' when the next
        //      allocation can be satisfied, and 'false' otherwise.
        //
        // Plan:
        //   Since 'allocate' is thoroughly tested for the overflow case, we
        //   can use depth-first enumeration to test this by enumerating buffer
        //   sizes from 1 to 8 and test all two-allocation combinations.
        //   Whenever 'hasSufficientCapacity' returns 'false', the next
        //   allocation request should return 0 (and vice versa).
        //
        // Testing:
        //   bool hasSufficientCapacity(int size) const
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'hasSufficientCapacity' TEST" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting 'hasSufficientCapacity'." << endl;

        char *buffer = bufferStorage.buffer();

        for (int i = 1; i <= 8; ++i) {  // buffer size
            const int BUFFER_SIZE = i;

            if (veryVerbose) { T_ P(BUFFER_SIZE) }

            for (int j = 1; j <= i; ++j) {  // first allocation
                const int ALLOCSIZE1 = j;

                if (veryVerbose) { T_ T_ P(ALLOCSIZE1) }

                for (int k = 1; k <= i - j + 1; ++k) {  // second allocation
                    const int ALLOCSIZE2 = k;

                    if (veryVerbose) { T_ T_ T_ P(ALLOCSIZE2) }

                    Obj bufferObject(buffer, BUFFER_SIZE);

                    // Initial allocation should succeed.

                    void *addr = bufferObject.allocate(ALLOCSIZE1);
                    LOOP_ASSERT(addr, 0 != addr);

                    // Subsequent allocation might or might not succeed.
                    // However, it should match the result of
                    // 'hasSufficientCapacity'.

                    bool ret = bufferObject.hasSufficientCapacity(k);
                    addr = bufferObject.allocate(ALLOCSIZE2);

                    LOOP2_ASSERT(addr, ret, (0 != addr) == ret);

                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   1) That 'release' sets the internal cursor of this memory manager
        //      to 0, but retains the external buffer managed.
        //
        // Plan:
        //   For concern 1, after the 'release' call, verify the currently
        //   managed buffer is the same as before using the 'buffer' accessor
        //   method.
        //
        //   For concern 2, set all bytes in the buffer to '0xA', then invoke
        //   'release'.  Verify that the bytes in the buffer remains '0xA'.
        //
        // Testing:
        //   void release()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << "\nTesting address managed after 'release'."
                          << endl;

        enum { BUFFER_SIZE = 256 };

        char buffer[BUFFER_SIZE];
        char bufferRef[BUFFER_SIZE];

        memset(buffer,    0xA, BUFFER_SIZE);
        memset(bufferRef, 0xA, BUFFER_SIZE);

        Obj bufferObject(buffer, BUFFER_SIZE);

        // Allocate some memory.
        bufferObject.allocate(1);
        bufferObject.allocate(16);

        // Release all memory.
        bufferObject.release();

        LOOP2_ASSERT(buffer, bufferObject.buffer(),
                                              buffer == bufferObject.buffer());
        LOOP2_ASSERT(BUFFER_SIZE, bufferObject.bufferSize(),
                                     BUFFER_SIZE == bufferObject.bufferSize());

        if (verbose) cout << "\nTesting that there is no side effect"
                             " to initial buffer." << endl;

        ASSERT(0 == memcmp(buffer, bufferRef, BUFFER_SIZE));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'replaceBuffer' TEST
        //
        // Concerns:
        //   1) That 'replaceBuffer' swaps the buffer managed by
        //      'bdema_BufferManager'.
        //
        //   2) That the address returned is the address of the previously
        //      managed buffer.
        //
        //   3) That the previously managed buffer is not changed in any way
        //      after the 'replaceBuffer' call.
        //
        // Plan:
        //   For concern 1 and 2, after the 'replaceBuffer' call, verify the
        //   currently managed buffer using the 'buffer' accessor method and
        //   verify the address returned to be the same as the previously
        //   managed buffer.
        //
        //   For concern 3, set all bytes in the initial buffer to '0xA', then
        //   replace the buffer with a second buffer.  Verify that the bytes in
        //   the first buffer remains '0xA'.
        //
        // Testing:
        //   char *replaceBuffer(char *newBuffer, int newSize)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'replaceBuffer' TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting swap and address returned." << endl;

        enum { BUFFER_SIZE = 256, NEW_BUFFER_SIZE = 512 };

        char buffer[BUFFER_SIZE];       // initial buffer
        char buffer2[NEW_BUFFER_SIZE];  // buffer for swapping
        char bufferRef[BUFFER_SIZE];    // reference for validating

        memset(buffer,    0xA, BUFFER_SIZE);
        memset(bufferRef, 0xA, BUFFER_SIZE);

        Obj bufferObject(buffer, BUFFER_SIZE);

        char *ret = bufferObject.replaceBuffer(buffer2, NEW_BUFFER_SIZE);

        LOOP2_ASSERT((void *)&buffer[0], ret, &buffer[0] == ret);
        LOOP2_ASSERT((void *)&buffer2[0], bufferObject.buffer(),
                                         &buffer2[0] == bufferObject.buffer());
        LOOP2_ASSERT(NEW_BUFFER_SIZE, bufferObject.bufferSize(),
                                 NEW_BUFFER_SIZE == bufferObject.bufferSize());

        if (verbose) cout << "\nTesting that there is no side effect"
                             " to initial buffer." << endl;

        ASSERT(0 == memcmp(ret, bufferRef, BUFFER_SIZE));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ALLOCATE TEST
        //
        // Concerns:
        //   1) That the allocated memory address is correctly aligned
        //      according to the alignment strategy specified at construction
        //      of the 'bdema_BufferManager' and comes from the external buffer
        //      also supplied at construction.
        //
        //   2) That when 'allocate' is used and the allocation request causes
        //      the supplied buffer to overflow, 0 is returned.
        //
        //   3) That we can allocate the expected amount of memory from the
        //      buffer, after taking into account of alignment effects.
        //
        // Plan:
        //   For concern 1, using the table-driven technique, create a set of
        //   test vectors having different alignment strategy, allocation
        //   sizes, the expected address of a subsequent allocation (please see
        //   test driver overview for an explanation of why this is done).
        //   Next, create a buffer on the stack that is maximally aligned.
        //   Then, construct a 'bdema_BufferManager' using the buffer and the
        //   alignment strategy from the test vector.  Invoke 'allocate' and
        //   'allocateRaw' with an allocation request of 1 byte.  Then vector.
        //   Then allocate a second time with the allocation size specified in
        //   the test vector and verify the result of the 'allocate' and
        //   'allocateRaw' method with the expected values.
        //
        //   For concern 2, create buffers of different size such that, after
        //   an initial allocation, the next allocation request will result
        //   in a buffer overflow.  Verify that 0 is returned.
        //
        //   For concern 3, using the table-driven technique, create a set of
        //   test vectors specifying the buffer size, allocation size,
        //   alignment strategy and expected number of allocations, keep
        //   allocating until 'allocate' returns 0.
        //
        // Testing:
        //   void *allocate(int size)
        //   void *allocateRaw(int size)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATE TEST" << endl
                                  << "=============" << endl;

        typedef bsls_Alignment::Strategy St;

        enum { MAX_ALIGN   = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

        char *buffer = bufferStorage.buffer();

#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting 'allocate' and 'allocateRaw'." << endl;
        {

        static const struct {
            int d_line;       // line number
            St  d_strategy;   // alignment strategy
            int d_allocSize;  // size of allocation request
            int d_expOffset;  // expected address of second allocation
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
            {  L_,      NAT,        16,             MAX_ALIGN },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,         1,             MAX_ALIGN },
            {  L_,      MAX,         2,             MAX_ALIGN },
            {  L_,      MAX,         3,             MAX_ALIGN },
            {  L_,      MAX,         4,             MAX_ALIGN },
            {  L_,      MAX,         7,             MAX_ALIGN },
            {  L_,      MAX,         8,             MAX_ALIGN },
            {  L_,      MAX,        15,             MAX_ALIGN },
            {  L_,      MAX,        16,             MAX_ALIGN },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE      = DATA[ti].d_line;
            const St  STRAT     = DATA[ti].d_strategy;
            const int ALLOCSIZE = DATA[ti].d_allocSize;
            const int EXPOFFSET = DATA[ti].d_expOffset;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(ALLOCSIZE) P(EXPOFFSET)
            }

            Obj bufferObject(buffer, BUFFER_SIZE, STRAT);

            void *addr = bufferObject.allocate(1);
            LOOP3_ASSERT(LINE, (void *)&buffer[0], addr, &buffer[0] == addr);

            addr = bufferObject.allocate(ALLOCSIZE);
            LOOP3_ASSERT(LINE, (void *)&buffer[EXPOFFSET], addr,
                                               &buffer[EXPOFFSET] == addr);
        }

        }

        if (verbose) cout << "\nTesting overflowed allocation." << endl;
        {

        static const struct {
            int  d_line;        // line number
            St   d_strategy;    // alignment strategy
            int  d_bufferSize;  // size of the buffer
            int  d_allocSize1;  // size of allocation request 1
            int  d_allocSize2;  // size of allocation request 2
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
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE       = DATA[ti].d_line;
            const St  STRAT      = DATA[ti].d_strategy;
            const int BUFFERSIZE = DATA[ti].d_bufferSize;
            const int ALLOCSIZE1 = DATA[ti].d_allocSize1;
            const int ALLOCSIZE2 = DATA[ti].d_allocSize2;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(BUFFERSIZE) P_(ALLOCSIZE1) P(ALLOCSIZE2)
            }

            Obj bufferObject(buffer, BUFFERSIZE, STRAT);
            void *addr = bufferObject.allocate(ALLOCSIZE1);

            LOOP2_ASSERT(LINE, addr, 0 != addr);

            addr = bufferObject.allocate(ALLOCSIZE2);

            LOOP2_ASSERT(LINE, addr, 0 == addr);
        }

        }

        if (verbose) cout << "\nTesting continuous allocation with 'allocate'"
                             " and 'allocateRaw'."
                          << endl;
        {

        static const struct {
            int d_line;        // line number
            St  d_strategy;    // alignment strategy
            int d_bufferSize;  // size of the buffer
            int d_allocSize;   // size of allocation request
            int d_expAlloc;    // expected number of allocations
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
            {  L_,      MAX,    16,             1,     16 / MAX_ALIGN },
            {  L_,      MAX,    16,             2,     16 / MAX_ALIGN },
            {  L_,      MAX,    16,             4,     16 / MAX_ALIGN },
            {  L_,      MAX,    16,             8,     16 / MAX_ALIGN },
            {  L_,      MAX,    16,            16,                  1 },
            {  L_,      MAX,    16,            32,                  0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE       = DATA[ti].d_line;
            const St  STRAT      = DATA[ti].d_strategy;
            const int BUFFERSIZE = DATA[ti].d_bufferSize;
            const int ALLOCSIZE  = DATA[ti].d_allocSize;
            const int EXPALLOC   = DATA[ti].d_expAlloc;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(BUFFERSIZE) P_(ALLOCSIZE) P(EXPALLOC)
            }

            // Get the appropriate buffer to use
            Obj bufferObject(buffer, BUFFERSIZE, STRAT);
            Obj bufferObjectRaw(buffer, BUFFERSIZE, STRAT);

            int   count = 0;
            void *addr  = 0;

            while(addr = bufferObject.allocate(ALLOCSIZE)) {
                void *addr2 = bufferObjectRaw.allocateRaw(ALLOCSIZE);

                if (STRAT == NAT) {
                    LOOP2_ASSERT((void *)&buffer[ALLOCSIZE * count], addr,
                                           &buffer[ALLOCSIZE * count] == addr);
                    LOOP2_ASSERT(addr, addr2, addr == addr2);
                }
                else {
                    const int INCR = ALLOCSIZE < MAX_ALIGN
                                   ? MAX_ALIGN
                                   : ALLOCSIZE;

                    LOOP2_ASSERT((void *)&buffer[INCR * count], addr,
                                                &buffer[INCR * count] == addr);
                    LOOP2_ASSERT(addr, addr2, addr == addr2);
                }
                ++count;
            }

            LOOP3_ASSERT(LINE, EXPALLOC, count, EXPALLOC == count);
        }

        }

#undef NAT
#undef MAX

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR / ACCESSORS TEST
        //
        // Concerns:
        //   1) That a 'bdema_BufferManager' can be constructed using an
        //      external buffer and the correct buffer size.
        //
        //   2) That the alignment strategy, when not specified at
        //      construction, is defaulted to natural alignment.
        //
        //   3) That the accessors return the correct address and size of the
        //      currently managed buffer (and 0 when none is being managed).
        //
        // Plan:
        //   First, for concern 1 and 3, construct two buffer objects, one with
        //   an initial buffer and one without.  Then, verify that 'buffer' and
        //   'bufferSize' methods returns the correct pointer and size to
        //   either buffer objects.
        //
        //   For concern 2, create two buffer objects, one without specifying
        //   the alignment strategy and the other one specifies the strategy
        //   to be maximum alignment.  Invoke 'allocate' twice, and verify,
        //   using the result of the second allocation, the alignment strategy
        //   of the buffer object.
        //
        // Testing:
        //   bdema_BufferManager(AlignmentStrategy s = NA)
        //   bdema_BufferManager(char * b, int i, AlignmentStrategy s = NA)
        //   char *buffer() const
        //   int bufferSize() const
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR / ACCESSORS TEST" << endl
                                  << "=====================" << endl;

        enum { BUFFER_SIZE = 256, ALLOC_SIZE1 = 1, ALLOC_SIZE2 = 4 };
        enum { MAX_ALIGN   = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };
        char buffer[BUFFER_SIZE];

        if (verbose) cout << "\nTesting ctor and accessors." << endl;
        {
            Obj bufferObject1(buffer, BUFFER_SIZE);
            Obj bufferObject2;

            LOOP2_ASSERT(&buffer[0], bufferObject1.buffer(),
                                         &buffer[0] == bufferObject1.buffer());
            LOOP_ASSERT(bufferObject2.buffer(), 0 == bufferObject2.buffer());

            LOOP2_ASSERT(BUFFER_SIZE, bufferObject1.bufferSize(),
                                    BUFFER_SIZE == bufferObject1.bufferSize());
            LOOP_ASSERT(bufferObject2.bufferSize(),
                                              0 == bufferObject2.bufferSize());
        }

        if (verbose) cout << "\nTesting ctor and alignment strategy." << endl;
        {
            Obj bufferObject1(buffer, BUFFER_SIZE);
            Obj bufferObject2(buffer, BUFFER_SIZE,
                              bsls_Alignment::BSLS_MAXIMUM);

            const int NAT_OFFSET =
                bsls_AlignmentUtil::calculateAlignmentOffset(
                                            &buffer[ALLOC_SIZE1], ALLOC_SIZE2);

            const int MAX_OFFSET1 =
                bsls_AlignmentUtil::calculateAlignmentOffset(
                           &buffer[0], bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);

            const int MAX_OFFSET2 =
                bsls_AlignmentUtil::calculateAlignmentOffset(
                                       &buffer[ALLOC_SIZE1 + MAX_OFFSET1],
                                       bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);

            void *addr = bufferObject1.allocate(ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[0], addr, &buffer[0] == addr);

            addr       = bufferObject1.allocate(ALLOC_SIZE2);
            LOOP2_ASSERT(&buffer[ALLOC_SIZE1 + NAT_OFFSET], addr,
                                                 &buffer[ALLOC_SIZE2] == addr);

            addr       = bufferObject2.allocate(ALLOC_SIZE1);
            LOOP2_ASSERT(&buffer[MAX_OFFSET1], addr,
                                                 &buffer[MAX_OFFSET1] == addr);

            addr       = bufferObject2.allocate(ALLOC_SIZE2);
            LOOP2_ASSERT(
                     &buffer[ALLOC_SIZE1 + MAX_OFFSET1 + MAX_OFFSET2],
                     addr,
                     &buffer[ALLOC_SIZE1 + MAX_OFFSET1 + MAX_OFFSET2] == addr);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) That a 'bdema_BufferManager' can be created and destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and alignment strategy.
        //
        //   3) That 'allocate' returns a block of memory from the external
        //      buffer supplied at construction.
        //
        // Plan:
        //   First, create a buffer on the stack, then initialize a
        //   'bdema_BufferManager' with the buffer.  Next, allocate a block of
        //   memory from the buffer object and verify that it comes from the
        //   external buffer.  Then, allocate another block of memory from the
        //   buffer object, and verify that the first allocation returned a
        //   block of memory of sufficient size by checking that
        //   'addr2 > addr1 + allocsize1'.  Also verify that the alignment
        //   strategy specified at construction is followed by checking the
        //   address of the second allocation.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATING TEST" << endl
                                  << "=============" << endl;

        enum { ALLOC_SIZE1 = 4, ALLOC_SIZE2 = 8 };

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting constructor." << endl;
        Obj bufferObject(buffer, BUFFER_SIZE);
        ASSERT(&buffer[0]  == bufferObject.buffer());
        ASSERT(BUFFER_SIZE == bufferObject.bufferSize());

        if (verbose) cout << "\nTesting allocate." << endl;
        void *addr1 = bufferObject.allocate(ALLOC_SIZE1);
        ASSERT(&buffer[0] == addr1);
        LOOP2_ASSERT((void *)&buffer[BUFFER_SIZE - 1], addr1,
                                            &buffer[BUFFER_SIZE - 1] >= addr1);

        void *addr2 = bufferObject.allocate(ALLOC_SIZE2);
        ASSERT(&buffer[0] < addr2);
        ASSERT(&buffer[BUFFER_SIZE - 1] >= addr2);

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



// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
