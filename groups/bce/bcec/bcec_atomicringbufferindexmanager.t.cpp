// bcec_atomicringbufferindexmanager.t.cpp       -*-C++-*-

#include <bcec_atomicringbufferindexmanager.h>

#include <bdes_bitutil.h>
#include <bcema_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// ----------------------------------------------------------------------------
// PUBLIC CONSTANTS
// [ 2] e_MAX_CAPACITY 
// CREATORS
// [ 2] bcec_AtomicRingBufferIndexManager(unsigned int, bslma::Allocator *);
// [ 2] ~bcec_AtomicRingBufferIndexManager();
// MANIPULATORS
// [ 3] int acquirePushIndex(unsigned int *, unsigned int *);
// [ 3] void releasePushIndex(unsigned int , unsigned int );
// [ 3] int acquirePopIndex(unsigned int *, unsigned int *);
// [ 3] void releasePopIndex(unsigned int , unsigned int );
// [  ] void incrementPopIndexFrom(unsigned int );
// [ 4] void disable();
// [ 4] void enable();
// ACCESSORS
// [ 4] bool isEnabled() const;        
// [ 3] unsigned int length() const;
// [ 2] unsigned int capacity() const; 
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close anonymous namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t"    \
                         << #J << ": " << J << "\t"    \
                         << #K << ": " << K << "\t"    \
                         << #L << ": " << L << "\n";   \
               aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

typedef bcec_AtomicRingBufferIndexManager Obj;

//=============================================================================
//                  GLOBAL HELP FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.

      case 5: {

            unsigned int generation, index;
            bslma::TestAllocator oa;
            Obj x(10, &oa); const Obj &X = x;

            ASSERTV(X.isEnabled());

            ASSERT(0 == x.acquirePushIndex(&generation, &index));
            ASSERT(0 <  x.acquirePopIndex(&generation, &index));

      }
      case 4: {
        // --------------------------------------------------------------------
        // TESTING: 'enable', 'disable', 'isEnabled'
        //
        // Concerns:
        //  1 Objects are created in an enabled state.
        //
        //  2 Calling 'disable' on an enabled queue prevents additional
        //    elements from being pushed, but not from being popped.
        //
        //  3 Calling 'enabled' on a disabled queue allows additional elements
        //    to be pushed.
        // 
        //  4 Calling 'enable' on an enabled queue has no effect.
        //  
        //  5 Calling 'disable' on a disabled queue has no effect.
        //
        //  6 Calling 'isEnabled' returns 'true' if either (1) neither
        //    'enable' or 'disable' has been called, or (2) 'enable' has been
        //    called more recently than 'disable'.
        //
        // Plan:
        //
        //  1 Create a new test-object, verify the 'isEnabled' status, and
        //    verify an element can be pushed.  (C-1)
        //
        //  2 Create a test-object and using a non-loop based test, enabled
        //    and disable the buffer, while testing its enabled status and
        //    verifying elements can be pushed and popped. (C-2,3,4,5).
        //
        // Testing:
        //   void disable();
        //   void enable();
        //   bool isEnabled() const;        
        // --------------------------------------------------------------------

        if (verbose) cout << endl 
                          << "PRIMARY MANIPULATORS AND ACCESSORS" << endl
                          << "==================================" << endl;



        if (verbose) cout << "\nTest that a newly constructed obj is enabled"
                          << endl;
        {
            unsigned int generation, index;
            bslma::TestAllocator oa;
            Obj x(10, &oa); const Obj &X = x;

            ASSERTV(X.isEnabled());

            ASSERT(0 == x.acquirePushIndex(&generation, &index));
            x.releasePushIndex(generation, index);
        }

        if (verbose) cout << "\nTest that a disabled obj cannot push new "
                          << "values but can pop existing ones"
                          << endl;
        {
            unsigned int generation, index;
            bslma::TestAllocator oa;
            Obj x(3, &oa); const Obj &X = x;

            ASSERTV(X.isEnabled());

            // Insert 2 elements.
            ASSERT(0 == x.acquirePushIndex(&generation, &index));
            x.releasePushIndex(generation, index);
            ASSERTV(1 == X.length());

            ASSERT(0 == x.acquirePushIndex(&generation, &index));
            x.releasePushIndex(generation, index);
            ASSERTV(2 == X.length());

            // Disable the queue.
            x.disable();
            ASSERTV(!X.isEnabled());

            // Test that attempting to push fails.
            ASSERT(0 > x.acquirePushIndex(&generation, &index));
            ASSERTV(2 == X.length());

            // Test that attempting to pop succeeds.
            ASSERT(0 == x.acquirePopIndex(&generation, &index));
            x.releasePopIndex(generation, index);
            ASSERTV(1 == X.length());

            // Test that attempting to push still fails.
            ASSERT(0 > x.acquirePushIndex(&generation, &index));
            ASSERTV(1 == X.length());

            // Disable the queue a second time, and verify that has no effect.
            x.disable();
            ASSERTV(!X.isEnabled());

            // Test that attempting to push still fails.
            ASSERT(0 > x.acquirePushIndex(&generation, &index));
            ASSERTV(1 == X.length());

            // Enable the queue.
            x.enable();
            ASSERTV(X.isEnabled());

            // Test that attempting to push succeeds.
            ASSERT(0 == x.acquirePushIndex(&generation, &index));
            x.releasePushIndex(generation, index);
            ASSERTV(2 == X.length());

            // Test that attempting to pop succeeds.
            ASSERT(0 == x.acquirePopIndex(&generation, &index));
            x.releasePopIndex(generation, index);
            ASSERTV(1 == X.length());

            // Enable the queue a second time, and verify that has no effect.
            x.enable();
            ASSERTV(X.isEnabled());

            // Test that attempting to push succeeds.
            ASSERT(0 == x.acquirePushIndex(&generation, &index));
            x.releasePushIndex(generation, index);
            ASSERTV(2 == X.length());

        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND ACCESORS
        //
        // Concerns:
        //  1 'length' on a newly constructed queue returns 0.
        //
        //  2 'acqurePushIndex' increments the 'length' of the queue.
        // 
        //  3 Calling 'acquirePushIndex' multiple times will not return
        //    the same index.
        //  
        //  4 Calling 'acquirePushIndex' when the queue is full returns the
        //    correct error.
        //
        //  5 Calling 'acquirePopIndex' when the queue is empty returns the
        //    correct error.
        //
        //  6 Calling 'acquirePopIndex' when the queue is not empty, and there
        //    are released push indices, returns the oldest released push
        //    index and the generation from where it was pushed, and
        //    decrements the length.
        //
        //  7 Calling 'releasePushIndex' permits the index to later be
        //    acquired for popping.
        //
        //  8 Calling 'releasePopIndex' permits the index to later be
        //    acquired for pushing
        //
        //  9 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //
        //  1 For each capcity value in a series of capacity values, call
        //    'acquirePushIndex' to that maxmimum capacity.  At each point,
        //    verify the expected index, generation count, and return
        //    status. (C-1,2,3) 
        //
        //  2 For each capcity value in a series of capacity values:
        //
        //    1 Call 'acquirePopIndex' and verify it fails.
        //
        //    2 Push and pop an index, and verify the expected index, length,
        //      and generation counts.  (C-5)
        //
        //  3 For each capcity value in a series of capacity values:
        //
        //    1 Fill the queue
        //
        //    2 Call 'acquirePushIndex' and verify it fails.
        //
        //    2 Pop and then push an index, and verify the expected index,
        //      length, and generation counts.  (C-4)
        //
        //  3 For each capcity value in a series of capacity values, iterate
        //    through every possible sequence of push and pop operations and
        //    verify the behavior against the expected behavior. (C1-8)
        //
        //  4 Use the assertion test facility to test function preconditions.
        //
        // Testing:
        //   int acquirePushIndex(unsigned int *, unsigned int *);
        //   void releasePushIndex(unsigned int , unsigned int );
        //   int acquirePopIndex(unsigned int *, unsigned int *);
        //   void releasePopIndex(unsigned int , unsigned int );
        //   unsigned int length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl 
                          << "PRIMARY MANIPULATORS AND ACCESSORS" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\nVerify acquiring push indices into a queue"
                          << endl;
        {
            for (int i = 1; i < 100; ++i) {
                const unsigned int CAPACITY = i;

                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;

                ASSERTV(X.length(), 0 == X.length());

                unsigned int index;
                unsigned int generation;
                for (unsigned int j = 0; j < CAPACITY; ++j) {

                    int rc = x.acquirePushIndex(&generation, &index);
                    ASSERTV(rc,            0 == rc);
                    ASSERTV(index, j,      j == index);
                    ASSERTV(generation,    0 == generation);
                    ASSERTV(X.length(),    j == X.length() - 1);
                }

                int rc = x.acquirePushIndex(&generation, &index);
                ASSERTV(rc,         0 < rc);
                ASSERTV(X.length(), CAPACITY == X.length());
                
            }
        }

        if (verbose) cout << "\nVerify 'acquiringPopIndex' an empty queue"
                          << endl;
        {
            for (int i = 1; i < 100; ++i) {
                const unsigned int CAPACITY = i;

                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;
                ASSERTV(X.length(), 0 == X.length());

                unsigned int index;
                unsigned int generation;

                for (unsigned int gen = 0; gen < 3; ++gen) {
                    for (unsigned int idx = 0; idx < CAPACITY; ++idx) {
                        int rc = x.acquirePopIndex(&generation, &index);
                        ASSERTV(rc,         0 < rc);
                                                
                        rc = x.acquirePushIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), 1 == X.length());

                        x.releasePushIndex(generation, index);
                        ASSERTV(X.length(), 1 == X.length());

                        rc = x.acquirePopIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), 0 == X.length());

                        x.releasePopIndex(generation, index);
                        ASSERTV(X.length(), 0 == X.length());
                    }
                }                        
            }
        }

        if (verbose) cout << "\nVerify 'acquiringPushIndex' on a full queue"
                          << endl;
        {
            for (int i = 1; i < 100; ++i) {
                const unsigned int CAPACITY = i;

                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;
                ASSERTV(X.length(), 0 == X.length());

                unsigned int index;
                unsigned int generation;

                // Fill the queue
                for (unsigned int idx = 0; idx < CAPACITY; ++idx) {
                    x.acquirePushIndex(&generation, &index);
                    x.releasePushIndex(generation, index);
                }
                ASSERTV(X.length(), X.length() == CAPACITY);

                for (unsigned int gen = 0; gen < 3; ++gen) {
                    for (unsigned int idx = 0; idx < CAPACITY; ++idx) {
                        int rc = x.acquirePushIndex(&generation, &index);
                        ASSERTV(rc,         0 < rc);
                                                
                        rc = x.acquirePopIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), X.length() == CAPACITY - 1);

                        x.releasePopIndex(generation, index);
                        ASSERTV(X.length(), X.length() == CAPACITY - 1);

                        rc = x.acquirePushIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen + 1);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), X.length() == CAPACITY);

                        x.releasePushIndex(generation, index);
                        ASSERTV(X.length(), X.length() == CAPACITY);
                    }
                }                        
            }
        }

        if (verbose) cout << "\nVerify 'acquiringPushIndex' on a full queue"
                          << endl;
        {
            for (int i = 1; i < 100; ++i) {
                const unsigned int CAPACITY = i;

                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;
                ASSERTV(X.length(), 0 == X.length());

                unsigned int index;
                unsigned int generation;

                // Fill the queue
                for (unsigned int idx = 0; idx < CAPACITY; ++idx) {
                    x.acquirePushIndex(&generation, &index);
                    x.releasePushIndex(generation, index);
                }
                ASSERTV(X.length(), X.length() == CAPACITY);

                for (unsigned int gen = 0; gen < 3; ++gen) {
                    for (unsigned int idx = 0; idx < CAPACITY; ++idx) {
                        int rc = x.acquirePushIndex(&generation, &index);
                        ASSERTV(rc,         0 < rc);
                                                
                        rc = x.acquirePopIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), X.length() == CAPACITY - 1);

                        x.releasePopIndex(generation, index);
                        ASSERTV(X.length(), X.length() == CAPACITY - 1);

                        rc = x.acquirePushIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen + 1);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), X.length() == CAPACITY);

                        x.releasePushIndex(generation, index);
                        ASSERTV(X.length(), X.length() == CAPACITY);
                    }
                }                        
            }
        }
        
        if (verbose) cout << "\nIterate over all possible sequences of 12"
                          << "push and pop operations"
                          << endl;
        // The following test iterates through all 12 bit integers using the
        // values of the 12 bits to encode whether the operation is a push or
        // pop, then performs a push or pop and verify results against an
        // oracle. 
        {
            int maxOp = bdes_BitUtil::base2Log(4096);
            ASSERT(12 == maxOp);

            for (int capacity = 1; capacity < 6; ++capacity) {
                const unsigned int CAPACITY = capacity;
                for (int opSequence = 0; opSequence < 4096; ++opSequence) {
                    unsigned int PUSH_INDEX = -1;
                    unsigned int PUSH_GEN   =  0;
                    unsigned int POP_INDEX  = -1;
                    unsigned int POP_GEN    =  0;
                    unsigned int LENGTH     =  0;
                    
                    bslma::TestAllocator oa;
                    Obj x(CAPACITY, &oa); const Obj &X = x;
                    
                    for (int op = 0; op < maxOp; ++op) {
                        bool isPush = bdes_BitUtil::isSetOne(opSequence, op);
                        unsigned int index, gen;
                        bool SUCCESS = isPush ? LENGTH < CAPACITY : LENGTH > 0;
                        if (isPush) {
                            if (SUCCESS) {
                                ++LENGTH;
                                ++PUSH_INDEX;
                                if (PUSH_INDEX == CAPACITY) {
                                    PUSH_INDEX = 0;
                                    ++PUSH_GEN;
                                }
                            }
                            int rc = x.acquirePushIndex(&gen, &index);
                            ASSERTV(rc, SUCCESS,        SUCCESS == (0 == rc));
                            ASSERTV(LENGTH, X.length(), LENGTH  == X.length());
                            if (0 != rc) {
                                continue;
                            }
                            x.releasePushIndex(gen, index);
                            ASSERTV(PUSH_INDEX, index, PUSH_INDEX == index);
                            ASSERTV(PUSH_GEN, gen,     PUSH_GEN == gen);
                        }
                        else { 
                            // Pop.
                            if (SUCCESS) {
                                --LENGTH;
                                ++POP_INDEX;
                                if (POP_INDEX == CAPACITY) {
                                    POP_INDEX = 0;
                                    ++POP_GEN;
                                }
                            }
                            int rc = x.acquirePopIndex(&gen, &index);
                            ASSERTV(rc, SUCCESS,        SUCCESS == (0 == rc));
                            ASSERTV(LENGTH, X.length(), LENGTH  == X.length());
                            if (0 != rc) {
                                continue;
                            }
                            x.releasePopIndex(gen, index);
                            ASSERTV(POP_INDEX, index, POP_INDEX == index);
                            ASSERTV(POP_GEN, gen,     POP_GEN == gen);
                        }
                   }
                }
            }        
        }
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator oa;

            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            unsigned int index, generation;
            
            Obj obj(10, &oa);
            ASSERT_FAIL(obj.acquirePushIndex(&generation, 0));
            ASSERT_FAIL(obj.acquirePushIndex(0, &index));
            ASSERT_PASS(obj.acquirePushIndex(&generation, &index));
            
            ASSERT_FAIL(obj.releasePushIndex(generation, 
                                             Obj::e_MAX_CAPACITY));
            ASSERT_FAIL(obj.releasePushIndex(UINT_MAX, index));
            ASSERT_PASS(obj.releasePushIndex(generation, index));


            ASSERT_FAIL(obj.acquirePopIndex(&generation, 0));
            ASSERT_FAIL(obj.acquirePopIndex(0, &index));
            ASSERT_PASS(obj.acquirePopIndex(&generation, &index));
            
            ASSERT_FAIL(obj.releasePopIndex(generation, 
                                             Obj::e_MAX_CAPACITY));
            ASSERT_FAIL(obj.releasePopIndex(UINT_MAX, index));
            ASSERT_PASS(obj.releasePopIndex(generation, index));
        }

        
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR
        //
        // Concerns:
        //
        // 1 The 'capacity' accessor returns the capacity supplied at
        //   construction 
        //
        // 2 The ARB uses the supplied allocator to allocate memory
        //
        // 3 If no allocator is explicitly supplied at construction, the
        //   default allocator is used
        //
        // 4 All allocated memory is released on destruction.
        //
        // 5 e_MAX_CAPACITY is the maximum capacity the allows for at least 2
        //   generations and a disabled status to be represented in a 32bit
        //   integer. 
        //
        // 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //
        // 1 Construct a series of test objects with varying capacity and test
        //   the capacity of the resulting object. (C-1)
        //
        // 2 Construct a test object and supply it a test allocator, verify that
        //   memory is allocated only from the supplied allocator. Verify
        //   allocated memory is released on destruction. (C-2,4)
        //
        // 3 Construct a test object and do not explicitly supply it an
        //   allocator, verify memory is allocated from the default
        //   allocator. Verify allocated memory is released on destruction.
        //   (C-3,4)
        // 
        // 4 Verify that representing 'e_MAX_CAPACITY' in an unsigned int
        //   leaves 2 bits to represent the disabled state and 2 complete
        //   generations. (C-5).
        //
        // 5 Use the assertion test facility to test function preconditions.
        //
        // Testing:
        //   bcec_AtomicRingBufferIndexManager(unsigned int, Allocator *);
        //   ~bcec_AtomicRingBufferIndexManager();
        //   unsigned int capacity() const; 
        // --------------------------------------------------------------------

        if (verbose) cout << endl 
                          << "DEFAULT CTOR" << endl
                          << "============" << endl;

        if (verbose) cout << "\nVerify the capacity." << endl;
        {
            bslma::TestAllocator oa;

            const unsigned int VALUES[] = { 
                1, 
                2, 
                8, 
                100, 
                10000,

                // Note that the following test cases are not enabled as they
                // exhaust memory on many platforms.
                // Obj::e_MAX_CAPACITY - 1,
                // Obj::e_MAX_CAPACITY 
            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj x(VALUES[i], &oa); const Obj& X = x;
                ASSERTV(X.capacity(), VALUES[i] == X.capacity());
            }
        }

        if (verbose) cout << "\nTest explicit the obj allocator." << endl;
        {
            bslma::TestAllocator oa, da;
            bslma::DefaultAllocatorGuard dag(&da);
            
            {
                Obj x(5, &oa);
                ASSERTV(da.numBytesInUse(), 0 == da.numBytesInUse());
                ASSERTV(oa.numBytesInUse(), 0  < oa.numBytesInUse());
            }
            ASSERTV(da.numBytesInUse(), 0 == da.numBytesInUse());
            ASSERTV(oa.numBytesInUse(), 0 == oa.numBytesInUse());
        }

        if (verbose) cout << "\nTest with the default allocator." << endl;
        {
            bslma::TestAllocator da;
            bslma::DefaultAllocatorGuard dag(&da);
            
            {
                Obj x(5); 
                ASSERTV(da.numBytesInUse(), 0 < da.numBytesInUse());
            }
            ASSERTV(da.numBytesInUse(), 0 == da.numBytesInUse());
        }

        if (verbose) cout << "\nTest 'e_MAX_CAPACITY' properties." << endl;
        {
            bsls::Types::Uint64 maxCapacity = Obj::e_MAX_CAPACITY;
            ASSERTV(UINT_MAX == maxCapacity * 4 - 1);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator oa, da;

            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_OPT_PASS(Obj obj(1, &oa));
            ASSERT_OPT_FAIL_RAW(Obj obj(Obj::e_MAX_CAPACITY + 1, &oa));

            // The following test is disabled because it causes a divide by 0
            // failure prior to reaching the assertion.
            // ASSERT_OPT_FAIL(Obj obj(0, &oa));
        }
      } break;
      case 1: {
       // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            Obj mX(1, &ta);
            ASSERT(mX.isEnabled());
            ASSERT(0 == mX.length());
            ASSERT(1 == mX.capacity());

            bsl::size_t generation, index;
            ASSERT(0 == mX.acquirePushIndex(&generation, &index));
            mX.releasePushIndex(generation, index);
            ASSERT(1 == mX.length());
            ASSERT(0 == generation);
            ASSERT(0 == index);
            ASSERT(0 != mX.acquirePushIndex(&generation, &index));
            ASSERT(1 == mX.length());
            
            generation = index = 1;
            ASSERT(0 == mX.acquirePopIndex(&generation, &index));            
            mX.releasePopIndex(generation, index);
            ASSERT(0 == mX.length());
            ASSERT(0 == generation);
            ASSERT(0 == index);
            ASSERT(0 != mX.acquirePopIndex(&generation, &index));            
            ASSERT(0 == mX.length());
            ASSERT(1 == generation);
            ASSERT(0 == index);

            mX.disable();
            ASSERT(!mX.isEnabled());
            ASSERT(0 != mX.acquirePushIndex(&generation, &index));
            ASSERT(0 == mX.length());

            generation = index = 1;
            mX.enable();
            ASSERT(mX.isEnabled());
            ASSERT(0 == mX.acquirePushIndex(&generation, &index));
            mX.releasePushIndex(generation, index);
            ASSERT(1 == mX.length());
            ASSERT(1 == generation);
            ASSERT(0 == index);            
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

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
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

