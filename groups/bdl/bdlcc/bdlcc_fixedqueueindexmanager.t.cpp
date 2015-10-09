// bdlcc_fixedqueueindexmanager.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlcc_fixedqueueindexmanager.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslmt_threadutil.h>
#include <bslmt_barrier.h>

#include <bdlb_random.h>
#include <bdlf_bind.h>
#include <bdlb_bitutil.h>

#include <bslma_defaultallocatorguard.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cstdint.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <bsl_sstream.h>
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

using namespace BloombergLP;
using namespace bsl;
// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The 'bdlcc::FixedQueueIndexManager' is a lock-free fully thread-safe
// mechanism for managing array indices in an externally maintained queue of
// objects.  There are a few elements that make this component particularly
// difficult to test thoroughly:
//: 1 It is fully thread-safe
//:
//: 2 The public interface doesn't expose state information helpful in
//:   verifying correctness of multi-threaded code.
//:
//: 3 The generation count used to avoid ABA problems has boundary cases that
//:   are impractical to test (a task would need to be running for hours or
//:   days).
// This test-driver applies standard BDE testing methodology to most of the
// public interface of 'bdlcc::FixedQueueIndexManager' for single threaded
// tests.  To handle the practical concerns above though, this test driver also
// defines testing mechanisms that access and manipulate private data members
// of the object under test.  This is allows for setting the generation count
// to values near boundary cases that would not otherwise be reachable in a
// reasonable period of time, and verifying the internal state of an object
// under heavy contention (i.e., a more complete thread-safety test).
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 8] unsigned int numRepresentableGenerations(unsigned int );
// [ 9] unsigned int numRepresentableGenerations(unsigned int );
// PUBLIC CONSTANTS
// [ 2] k_MAX_CAPACITY
// CREATORS
// [ 2] bdlcc::FixedQueueIndexManager(unsigned int, bslma::Allocator *);
// [ 2] ~bdlcc::FixedQueueIndexManager();
// MANIPULATORS
// [ 3] int reservePushIndex(unsigned int *, unsigned int *);
// [ 3] void commitPushIndex(unsigned int , unsigned int );
// [ 3] int reservePopIndex(unsigned int *, unsigned int *);
// [ 3] void commitPopIndex(unsigned int , unsigned int );
// [ 6] int reservePopIndexForClear(unsigned *,unsigned *,unsigned,unsigned);
// [ 7] void abortPushIndexReservation(unsigned int, unsigned int);
// [ 5] void disable();
// [ 5] void enable();
// [ 7] void abortPushIndexReservation(unsigned int, unsigned int);
// ACCESSORS
// [ 5] bool isEnabled() const;
// [ 3] unsigned int length() const;
// [ 2] unsigned int capacity() const;
// [10] bsl::ostream& print(bsl::ostream& ) const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE
// [ 4] CONCERN: 'gg' generator and 'dirtyGG' generator
// [11] CONCERN: Thread-Safety (concurrent access does not corrupt state)
// [12] CONCERN: maxCombinedIndex

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

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

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

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

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

typedef bdlcc::FixedQueueIndexManager Obj;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const bdlcc::FixedQueueIndexManager& indexManager)
{
    indexManager.print(stream);
    return stream;
}

//=============================================================================
//                        GENERATOR LANGUAGE FOR gg
//=============================================================================
//
// The gg function initializes the state of a 'bdlcc::FixedQueueIndexManager'
// using a supplied 'pushCombinedIndex' and 'popCombinedIndex'.  Te following
// table shows examples for various states:
//..
//  Legend: { generation | state }  State = { E => EMPTY,
//                                            F => FULL }
//
// pushComIdx popCombIdx       Resulting Index Manager State (capacity 3)
// ---------- ---------        -----------------------------
//                                   v-pushIndex
//     0          0              [ {0|E} {0|E} {0|E) ]
//                                   ^-popIndex
//
//                                          v-pushIndex
//     1          0              [ {0|F} {0|E} {0|E) ]
//                                   ^-popIndex
//
//                                               v-pushIndex
//     2          0              [ {0|F} {0|F} {0|E) ]
//                                   ^-popIndex
//
//                                               v-pushIndex
//     2          2              [ {0|E} {0|E} {0|E) ]
//                                               ^-popIndex
//
//                                   v-pushIndex
//     3          0              [ {0|F} {0|F} {0|F) ]
//                                   ^-popIndex
//
//     4          0              ! INVALID !
//
//                                         v-pushIndex
//     4          1              [ {1|F} {0|E} {0|F) ]
//                                              ^-popIndex
//
//                                         v-pushIndex
//     7          5              [ {2|F} {1|E} {1|F) ]
//                                               ^-popIndex
//..

void gg(Obj          *result,
        unsigned int  pushCombinedIndex,
        unsigned int  popCombinedIndex)
    // Initialize the specified 'result' to have the specified
    // 'pushCombinedIndex' and the specified 'popCombinedIndex'.  The behavior
    // is undefined unless 'result' is empty, the push and pop index refer to
    // the first cell in the circular buffer,
    // 'pushCombinedIndex >= popCombinedIndex' and
    // 'pushCombinedIndex - popCombinedIndex <= result->capacity()'.  Note that
    // a combined index is the combination of generation count and index, as
    // 'combined index = generation * capacity + index'.
{
    BSLS_ASSERT(pushCombinedIndex >= popCombinedIndex);
    BSLS_ASSERT(pushCombinedIndex - popCombinedIndex <= result->capacity());

    for (unsigned int i = 0; i < popCombinedIndex; ++i) {
        unsigned int generation, index;

        int rc = result->reservePushIndex(&generation, &index);
        (void)rc;   // suppress warning
        result->commitPushIndex(generation, index);

        rc = result->reservePopIndex(&generation, &index);
        BSLS_ASSERT(0     == rc);
        BSLS_ASSERT(index == i % result->capacity());
        result->commitPopIndex(generation, index);
    }

    for (unsigned int i = popCombinedIndex; i < pushCombinedIndex; ++i) {
        unsigned int generation, index;

        int rc = result->reservePushIndex(&generation, &index);
        (void)rc;   // suppress warning
        BSLS_ASSERT(0     == rc);
        BSLS_ASSERT(index == i % result->capacity());
        result->commitPushIndex(generation, index);
    }
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Thread-Safe Queue of Integers
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a simple thread-safe queue of integers
// using a 'bdlcc::FixedQueueIndexManager' to synchronize the queue operations.
//
// We start by declaring the data members of an 'IntegerQueue', a vector of
// integers, to hold the values in the queue, and an index manager to ensure
// thread-safe access to the indices of the vector:
//..
    class IntegerQueue {
        // This class provides a fully thread-safe queue of integers with a
        // fixed maximum capacity.

        // DATA
        bdlcc::FixedQueueIndexManager d_indexManager;  // manages 'd_values'
                                                       // indices

        bsl::vector<int>              d_values;        // maintains values

      private:
        // Not implemented:
        IntegerQueue(const IntegerQueue&);

      public:
//..
// Then, we declare the methods of an integer queue:
//..
        // CREATORS
        explicit IntegerQueue(bsl::size_t       capacity,
                              bslma::Allocator *basicAllocator = 0);
            // Create a queue capable of holding up to the specified
            // 'capacity' number of integer values.

        ~IntegerQueue();
            // Destroy this queue.

        // MANIPULATORS
        int tryPushBack(int value);
            // Attempt to push the specified 'value' onto the back of this
            // queue.  Return 0 on success, and a non-zero value if this queue
            // is full.

        int tryPopFront(int *result);
            // Attempt to remove an element from the front of this queue and
            // load the removed value into the specified 'result'.  Return 0
            // on success, and a non-zero value otherwise.

        // ACCESSORS
        bsl::size_t length() const;
            // Return a snapshot of the number of elements currently in this
            // queue.

        bsl::size_t capacity() const;
            // Return the maximum number of elements that this queue can hold.
    };
//..
//  Next, we define the constructor, which initializes both the index manager
//  and vector with the supplied capacity:
//..
    // CREATORS
    IntegerQueue::IntegerQueue(bsl::size_t       capacity,
                               bslma::Allocator *basicAllocator)
    : d_indexManager(capacity, basicAllocator)
    , d_values(capacity, 0, basicAllocator)
    {
    }

    IntegerQueue::~IntegerQueue()
    {
    }
//..
// Now, we define 'tryPushBack' and 'tryPopFront', which use the index manager
// to reserve an index in the vector, operate on that index, and then commit
// that index back to the index manager:
//..
    // MANIPULATORS
    int IntegerQueue::tryPushBack(int value)
    {
        unsigned int generation, index;
        if (0 == d_indexManager.reservePushIndex(&generation, &index)) {
            d_values[index] = value;
            d_indexManager.commitPushIndex(generation, index);
            return 0;                                                 // RETURN
        }
        return -1;
    }

    int IntegerQueue::tryPopFront(int *result)
    {
        unsigned int generation, index;
        if (0 == d_indexManager.reservePopIndex(&generation, &index)) {
            *result = d_values[index];
            d_indexManager.commitPopIndex(generation, index);
            return 0;                                                 // RETURN
        }
        return -1;
    }
//..
// Notice that because none of these operations allocate memory, we do not need
// to add code to ensure exception safety.
//
// Then, we define the accessors to the integer queue:
//..
    // ACCESSORS
    bsl::size_t IntegerQueue::length() const
    {
        return d_indexManager.length();
    }

    bsl::size_t IntegerQueue::capacity() const
    {
        return d_indexManager.capacity();
    }
//..

// ============================================================================
//                      *IMPLEMENTATION SPECIFIC* TOOLS
// ----------------------------------------------------------------------------

// The following tools flagrantly make use of implementation details and
// platform specific behavior (i.e., 'reinterpret_cast').  Extraordinary
// measures are taken in this test-driver to allow testing that would otherwise
// not be possible.  Specifically, this enables: (1) testing generation counts
// beyond the range that could be tested in a reasonable amount of time (2)
// verify the internal state of an index manager is not corrupted under heavy
// thread contention.

struct FixedQueueIndexManagerDataMembers {
    // This struct maintains the exact same data members as
    // 'bdlcc::FixedQueueIndexManager' and is used for debugging and extreme
    // white box testing, where verifying the internal state of the
    // 'bdlcc::FixedQueueIndexManager' is necessary.

    // PRIVATE CONSTANTS
    enum {
        e_PADDING =
                   bslmt::Platform::e_CACHE_LINE_SIZE - sizeof(bsls::AtomicInt)
    };

    // DATA
    bsls::AtomicInt     d_pushIndex;
    const char          d_pushIndexPad[e_PADDING];
    bsls::AtomicInt     d_popIndex;
    const char          d_popIndexPad[e_PADDING];
    const bsl::size_t   d_capacity;
    const unsigned int  d_maxGeneration;
    const unsigned int  d_maxCombinedIndex;
    bsls::AtomicInt    *d_states;
    bslma::Allocator   *d_allocator_p;
};

enum ElementState {
    e_EMPTY    = 0,   // element is empty and available for writing
    e_WRITING  = 1,   // element is reserved for writing
    e_FULL     = 2,   // element has a value in it
    e_READING  = 3    // element is reserved for reading
};

static const unsigned int k_ELEMENT_STATE_MASK     = 0x3;
static const unsigned int k_GENERATION_COUNT_SHIFT = 0x2;
static const unsigned int k_DISABLED_STATE_MASK = 1 << ((sizeof(int) * 8) - 1);

BSLMF_ASSERT(sizeof(FixedQueueIndexManagerDataMembers) ==
             sizeof(bdlcc::FixedQueueIndexManager));

class FixedQueueState {
    // This class provides access to the internal state of a
    // 'bdlcc::FixedQueueIndexManager', and is used for debugging and extreme
    // white box testing, where verifying the internal state of the
    // 'bdlcc::FixedQueueIndexManager' is necessary.

    // DATA
    const FixedQueueIndexManagerDataMembers *d_data;

  public:

    // CREATORS
    explicit FixedQueueState(
                            const bdlcc::FixedQueueIndexManager *indexManager);
        // Create an an 'FixedQueueState' object to access the state of the
        // specified 'indexManager'.

    // ACCESSORS
    unsigned int pushIndex() const;
        // Return the current push index for the index manager supplied at
        // construction.

    unsigned int pushGeneration() const;
        // Return the current push generation for the index manager supplied at
        // construction.

    unsigned int popIndex() const;
        // Return the current pop index for the index manager supplied at
        // construction.

    unsigned int popGeneration() const;
        // Return the current pop generation for the index manager supplied at
        // construction.

    unsigned int elementGeneration(unsigned int index) const;
        // Return the generation of the element at the specified 'index'.

    ElementState elementState(unsigned int index) const;
        // Return the state of the element at the specified 'index'.

    unsigned int capacity() const;
        // Return the maximum generation value.

    unsigned int maxGeneration() const;
        // Return the maximum generation value.

    unsigned int maxCombinedIndex() const;
        // Return the maximum combined index value.

    bsl::ostream& print(bsl::ostream &stream) const;
        // Write the state of the index manager supplied at construction to the
        // specified 'stream'.
};

// CREATORS
FixedQueueState::FixedQueueState(
                             const bdlcc::FixedQueueIndexManager *indexManager)
: d_data(reinterpret_cast<const FixedQueueIndexManagerDataMembers *>(
                                                                 indexManager))
{
}

// ACCESSORS
unsigned int FixedQueueState::pushIndex() const
{
    return d_data->d_pushIndex % capacity();
}

unsigned int FixedQueueState::pushGeneration() const
{
    return d_data->d_pushIndex / capacity();
}

unsigned int FixedQueueState::popIndex() const
{
    return d_data->d_popIndex % capacity();
}

unsigned int FixedQueueState::popGeneration() const
{
    return d_data->d_popIndex / capacity();
}

unsigned int FixedQueueState::elementGeneration(unsigned int index) const
{
    return d_data->d_states[index] >> k_GENERATION_COUNT_SHIFT;
}

ElementState FixedQueueState::elementState(unsigned int index) const
{
    return static_cast<ElementState>(
                              d_data->d_states[index] & k_ELEMENT_STATE_MASK);
}

unsigned int FixedQueueState::maxGeneration() const
{
    return d_data->d_maxGeneration;
}

unsigned int FixedQueueState::maxCombinedIndex() const
{
    return d_data->d_maxCombinedIndex;
}

unsigned int FixedQueueState::capacity() const
{
    return static_cast<unsigned int>(d_data->d_capacity);
}

void dirtyAdjustGeneration(Obj *result, unsigned int generation)
    // Load into the specified 'result' the state of an empty queue at the
    // beginning of the specified 'generation' (i.e., the push and pop index
    // both refer to the first cell.  Note that this operation is equivalent to
    // 'gg' but flagrantly abuses implementation information and compiler
    // dependent behavior in order to allow the assignment of combined indices
    // with vary large values.
{
   FixedQueueIndexManagerDataMembers *data =
                 reinterpret_cast<FixedQueueIndexManagerDataMembers *>(result);

   bsl::size_t capacity = result->capacity();
   for (bsl::size_t i = 0; i < capacity; ++i) {
       data->d_states[i] = generation << k_GENERATION_COUNT_SHIFT;
   }

   data->d_pushIndex = static_cast<int>(generation * capacity);
   data->d_popIndex  = static_cast<int>(generation * capacity);
}

void dirtyGG(Obj          *result,
             unsigned int  pushCombinedIndex,
             unsigned int  popCombinedIndex)
    // Initialize the specified 'result' to have the specified
    // 'pushCombinedIndex' and the specified 'popCombinedIndex'.
{
    BSLS_ASSERT(pushCombinedIndex >= popCombinedIndex);
    BSLS_ASSERT(pushCombinedIndex - popCombinedIndex <= result->capacity());

    unsigned int startGeneration = static_cast<int>(popCombinedIndex
                                                  / result->capacity());

    dirtyAdjustGeneration(result, startGeneration);
    gg(result,
       pushCombinedIndex - startGeneration
                              * static_cast<unsigned int>(result->capacity()),
       popCombinedIndex  - startGeneration
                              * static_cast<unsigned int>(result->capacity()));
}

// ============================================================================
//               TESTING: THREAD SAFETY (NO STATE CORRUPTION)
// ----------------------------------------------------------------------------

class TestThreadStateBarrier {
     bslmt::Barrier   d_barrier;
     bsls::AtomicInt d_state;

  public:
     enum State {
         e_WAIT     = 0,
         e_CONTINUE = 1,
         e_EXIT     = 2
     };

    // CREATORS
    explicit TestThreadStateBarrier(int numTestThreads);
        // Create a state barrier for the specified 'numTestThreads'.  Note
        // that the test threads are meant to be controlled by an additional
        // thread.

    ~TestThreadStateBarrier();
        // Destroy this state barrier

    // MANIPULATORS
                         // Test Thread State Functions

    State state();
        // Return the state of the current test.  If the resulting state is
        // 'e_WAIT' the calling thread must call 'blockUntilStateChange' before
        // continuing, and if the state is 'e_EXIT' the calling thread must
        // terminate.

    void blockUntilStateChange();
        // Block the calling thread until the test administrating thread has
        // indicated the test should continue.

                         // Controlled Thread State Functions

    void suspendTest();
        // Set the state of this state barrier to 'e_WAIT', and block the
        // calling thread until the number of threads supplied at construction
        // have called 'state' and received the 'e_WAIT' state.

    void continueTest();
        // Set the state of this state barrier to 'e_CONTINUE' and wait until
        // the number of threads supplied at construction have called
        // 'blockUntilStateChange'.

    void exitTest();
        // Set the state of this state barrier to 'e_EXIT', and block the
        // calling thread until the number of threads supplied at construction
        // have called 'state' and received the 'e_EXIT' state.
};

// CREATORS
TestThreadStateBarrier::TestThreadStateBarrier(int numTestThreads)
: d_barrier(numTestThreads + 1)
, d_state(e_WAIT)
{
}

TestThreadStateBarrier::~TestThreadStateBarrier()
{
}

// MANIPULATORS
inline
TestThreadStateBarrier::State TestThreadStateBarrier::state()
{
    State state = static_cast<State>(d_state.loadRelaxed());
    if (state == e_CONTINUE) {
        return state;                                                 // RETURN
    }
    d_barrier.wait();
    return state;
}

void TestThreadStateBarrier::blockUntilStateChange()
{
    d_barrier.wait();
}

void TestThreadStateBarrier::suspendTest()
{
    d_state = e_WAIT;
    d_barrier.wait();
}

void TestThreadStateBarrier::continueTest()
{
    d_state = e_CONTINUE;
    d_barrier.wait();
}

void TestThreadStateBarrier::exitTest()
{
    d_state = e_EXIT;
    d_barrier.wait();
}

inline
void performDelay(int period)
    // If the number of times this function has been called is an even multiple
    // of the specified 'period', then put the current thread to sleep before
    // returning, otherwise this function has no effect.
{
    static bsls::AtomicInt delayPeriod;

    if (period) {
        int delay = delayPeriod.addRelaxed(1);
        if (0 == delay % period) {
            bslmt::ThreadUtil::microSleep(10, 0);
        }
    }
}

void writerThread(Obj *x, TestThreadStateBarrier *testState, int delayPeriod)
    // Simulate a client pushing elements from the specified 'x' test object,
    // using the specified 'testState' to determine the current state of the
    // test (running, paused, exiting), and periodically inserting delays using
    // the specified 'delayPeriod'.
{
    const bsl::size_t CAPACITY = x->capacity();

    testState->blockUntilStateChange();

    for (;;) {
        TestThreadStateBarrier::State state = testState->state();
        if (state == TestThreadStateBarrier::e_EXIT) {
            return;                                                   // RETURN
        }
        else if (state == TestThreadStateBarrier::e_WAIT) {
            testState->blockUntilStateChange();
            continue;
        }
        bsl::size_t length = x->length();
        ASSERTV(length, length <= CAPACITY);

        for (int i = 0; i < 5; ++i) {
            unsigned int generation, index;
            int rc = x->reservePushIndex(&generation, &index);
            performDelay(delayPeriod);
            if (0 == rc) {
                x->commitPushIndex(generation, index);
            }
        }
    }

}

void readerThread(Obj *x, TestThreadStateBarrier *testState, int delayPeriod)
    // Simulate a client popping elements from the specified 'x' test object,
    // using the specified 'testState' to determine the current state of the
    // test (running, paused, exiting), and periodically inserting delays using
    // the specified 'delayPeriod'.
{
    const bsl::size_t CAPACITY = x->capacity();

    testState->blockUntilStateChange();

    for (;;) {
        TestThreadStateBarrier::State state = testState->state();
        if (state == TestThreadStateBarrier::e_EXIT) {
            return;                                                   // RETURN
        }
        else if (state == TestThreadStateBarrier::e_WAIT) {
            testState->blockUntilStateChange();
            continue;
        }
        bsl::size_t length = x->length();
        ASSERTV(length, length <= CAPACITY);

        for (int i = 0; i < 5; ++i) {
            unsigned int generation, index;
            int rc = x->reservePopIndex(&generation, &index);
            performDelay(delayPeriod);
            if (0 == rc) {
                x->commitPopIndex(generation, index);
            }
        }
    }
}

void exceptionThread(Obj                    *x,
                     TestThreadStateBarrier *testState,
                     int                     delayPeriod)
    // Simulate a series of exceptions being handled by a client pushing
    // elements into the specified 'x' test object, using the specified
    // 'testState' to determine the current state of the test (running, paused,
    // exiting), and periodically inserting delays using the specified
    // 'delayPeriod'.
{

    const bsl::size_t CAPACITY = x->capacity();

    testState->blockUntilStateChange();

    for (;;) {
        TestThreadStateBarrier::State state = testState->state();
        if (state == TestThreadStateBarrier::e_EXIT) {
            return;                                                   // RETURN
        }
        else if (state == TestThreadStateBarrier::e_WAIT) {
            testState->blockUntilStateChange();
            continue;
        }

        bsl::size_t length = x->length();
        ASSERTV(length, length <= CAPACITY);

        unsigned int endGeneration, endIndex;
        int rc = x->reservePushIndex(&endGeneration, &endIndex);
        performDelay(delayPeriod);
        if (0 == rc) {
            unsigned int generation, index;
            while (0 == x->reservePopIndexForClear(&generation,
                                         &index,
                                         endGeneration,
                                         endIndex)) {
                performDelay(delayPeriod);
                x->commitPopIndex(generation, index);
                performDelay(delayPeriod);
            }
            performDelay(delayPeriod);
            x->abortPushIndexReservation(endGeneration, endIndex);
        }
    }
}

void assertValidState(Obj *x)
    // Use 'ASSERT' to verify the properties of the specified 'x' test object.
{
    const bsl::size_t CAPACITY = x->capacity();

    ASSERT(CAPACITY >= x->length());

    FixedQueueState state(x);

    unsigned int pushIndex = state.pushIndex();
    unsigned int popIndex  = state.popIndex();
    unsigned int pushGeneration = state.pushGeneration();
    unsigned int popGeneration  = state.popGeneration();

    unsigned int combinedPushIndex = pushGeneration
                                   * static_cast<unsigned int>(CAPACITY)
                                   + pushIndex;
    unsigned int combinedPopIndex  = popGeneration
                                   * static_cast<unsigned int>(CAPACITY)
                                   + popIndex;

    ASSERT(combinedPushIndex >= combinedPopIndex &&
           combinedPushIndex <= combinedPopIndex + CAPACITY);

    bool EMPTY = (combinedPopIndex == combinedPushIndex);
    // The queue can be diagrammed as:
    //..
    //  ,--------------------------------.
    //  |  Zone 1  |  Zone 2  |  Zone 3  |
    //  `--------------------------------'
    //             ^          ^
    //             |          |
    //          Push/Pop    Push/Pop
    //..

    unsigned int zone1EndIndex   = bsl::min(popIndex, pushIndex);
    unsigned int zone1Generation = popGeneration + 1;
    ElementState zone1State      = (popIndex < pushIndex || EMPTY)
                                   ? e_EMPTY
                                   : e_FULL;

    unsigned int zone2EndIndex   = bsl::max(popIndex, pushIndex);
    unsigned int zone2Generation = popIndex < pushIndex
                                   ? popGeneration
                                   : popGeneration + 1;
    ElementState zone2State      = popIndex <  pushIndex ? e_FULL  : e_EMPTY;

    unsigned int zone3Generation = popGeneration;
    ElementState zone3State      = (popIndex < pushIndex || EMPTY)
                                   ? e_EMPTY
                                   : e_FULL;

    for (unsigned int i = 0; i < zone1EndIndex; ++i) {
        ASSERTV(*x, EMPTY, zone1State      == state.elementState(i));
        ASSERTV(*x, zone1Generation == state.elementGeneration(i));
    }
    for (unsigned int i = zone1EndIndex; i < zone2EndIndex; ++i) {
        ASSERTV(*x, zone2State      == state.elementState(i));
        ASSERTV(*x, zone2Generation == state.elementGeneration(i));
    }
    for (unsigned int i = zone2EndIndex; i < CAPACITY; ++i) {
        ASSERTV(*x, zone3State      == state.elementState(i));
        ASSERTV(*x, zone3Generation == state.elementGeneration(i));
    }
}

// ============================================================================
//                             PERFORMANCE TEST
// ----------------------------------------------------------------------------

namespace PERFORMANCE_TEST {

void writerThread(Obj                    *x,
                  bsls::AtomicInt64      *writeCount,
                  TestThreadStateBarrier *testState)
    // Simulate a client pushing elements from the specified 'x' test object,
    // using the specified 'testState' to determine the current state of the
    // test (running, paused, exiting).
{
    bsls::Types::Int64 count = 0;
    const bsl::size_t CAPACITY = x->capacity();

    testState->blockUntilStateChange();

    for (;;) {
        TestThreadStateBarrier::State state = testState->state();
        if (state == TestThreadStateBarrier::e_EXIT) {
            *writeCount += count;
            return;                                                   // RETURN
        }
        else if (state == TestThreadStateBarrier::e_WAIT) {
            testState->blockUntilStateChange();
            continue;
        }
        bsl::size_t length = x->length();
        ASSERTV(length, length <= CAPACITY);

        for (int i = 0; i < 5; ++i) {
            unsigned int generation, index;
            int rc = x->reservePushIndex(&generation, &index);
            if (0 == rc) {
                ++count;
                x->commitPushIndex(generation, index);
            }
        }
    }

}

void readerThread(Obj                    *x,
                  bsls::AtomicInt64      *readCount,
                  TestThreadStateBarrier *testState)
    // Simulate a client popping elements from the specified 'x' test object,
    // using the specified 'testState' to determine the current state of the
    // test (running, paused, exiting), and periodically inserting delays using
    // the specified 'delayPeriod'.
{
    bsls::Types::Int64 count = 0;
    const bsl::size_t CAPACITY = x->capacity();

    testState->blockUntilStateChange();

    for (;;) {
        TestThreadStateBarrier::State state = testState->state();
        if (state == TestThreadStateBarrier::e_EXIT) {
            *readCount += count;
            return;                                                   // RETURN
        }
        else if (state == TestThreadStateBarrier::e_WAIT) {
            testState->blockUntilStateChange();
            continue;
        }
        bsl::size_t length = x->length();
        ASSERTV(length, length <= CAPACITY);
        for (int i = 0; i < 5; ++i) {
            unsigned int generation, index;
            int rc = x->reservePopIndex(&generation, &index);
            if (0 == rc) {
                ++count;
                x->commitPopIndex(generation, index);
            }
        }
    }
}

}  // close namespace PERFORMANCE_TEST

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING USAGE EXAMPLE" << endl
                                  << "=====================" << endl;

// Finally, we create an 'IntegerQueue', and push and pop a couple of elements
// into the queue:
//..
    IntegerQueue intQueue(2);
    int rc = intQueue.tryPushBack(1);
    ASSERT(0 == rc);

    rc = intQueue.tryPushBack(2);
    ASSERT(0 == rc);

    rc = intQueue.tryPushBack(3);
    ASSERT(0 != rc);

    ASSERT(2 == intQueue.length());

    int result;

    rc = intQueue.tryPopFront(&result);
    ASSERT(0 == rc);
    ASSERT(1 == result);
//..
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CONCERN: maxCombinedIndex
        //
        // Concerns:
        //  1 That pushing an element at the maximum combined index value,
        //    means that the next element pushed will be at combined index of
        //    0.
        //
        //  2 That poping an element at the maximum combined index value,
        //    means that the next element popped will be at combined index of
        //    0.
        //
        //  3 That length returns the correct length when the combined push
        //    index has gone past maxCombinedIndex.
        //
        //  4 That 'reservePopIndexForClear' and 'abortPushIndexReservation'
        //    clear the correct element and increment the push and pop ind
        //
        //  5 Manipulating the index manager near the maximum combined
        //    index on multiple threads simultaneously does not corrupted the
        //    index manager state.
        //
        // Plan:
        //  1 Call 'reservePushIndex', 'commitPushIndex',
        //    'reservePopIndex', 'commitPopIndex', on a queue of capacity one
        //    and verify the expected results.
        //
        //  2 For a series of valid combined push index and pop index values,
        //    manipulate an index buffer to the maximum generation value, and
        //    push and pop elements according to the push index and pop index
        //    for the test.  At each step validate the results of the
        //    operation.
        //
        //  3 For a series of valid combined push index and pop index values,
        //    manipulate an index buffer to the maximum generation value, use
        //    'gg' to populate the buffer, and then call
        //    'reservePopIndexForClear' and 'abortPushIndexGeneration' to
        //    empty the queue.  At each step validate the results of the
        //    operation.
        //
        //  4 For a table driven number of reading threads, writing threads,
        //    "exception" threads (threads calling 'reservePopIndexForClear'
        //    and 'abortPushIndexResevation'), and injected delay value:
        //
        //    1 Create the described set of threads and supply the periodic
        //      delay value.
        //
        //    2 Repeatedly initialize a index manager to near the maximum
        //      combined index, allow the threads to run into they reset the
        //      combined index back to 0, validate the state of the index
        //      buffer.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCERN: maxCombinedIndex" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTest push & pop with a buffer of capacity 1"
                          << endl;
        {
            bslma::TestAllocator oa;
            Obj x(1, &oa); const Obj &X = x;

            FixedQueueState state(&x);

            const unsigned int MAX_COMBINED_INDEX = UINT_MAX >> 2;
            const unsigned int MAX_GENERATION     = UINT_MAX >> 2;

            const unsigned int maxGeneration =
                                 FixedQueueState(&x).maxGeneration();
            const unsigned int maxCombinedIndex =
                              FixedQueueState(&x).maxCombinedIndex();

            ASSERTV(MAX_GENERATION, maxGeneration,
                    MAX_GENERATION == maxGeneration);

            ASSERTV(MAX_COMBINED_INDEX, maxCombinedIndex,
                    MAX_COMBINED_INDEX == maxCombinedIndex);

            dirtyGG(&x, MAX_COMBINED_INDEX, MAX_COMBINED_INDEX);

            if (veryVerbose) {
                P(X);
            }

            unsigned int generation, index;
            ASSERT(0 == X.length());
            ASSERT(0 == x.reservePushIndex(&generation, &index));
            ASSERT(MAX_GENERATION == generation);
            ASSERT(0              == index);
            x.commitPushIndex(generation, index);

            ASSERT(0 == x.reservePopIndex(&generation, &index));
            ASSERT(MAX_GENERATION == generation);
            ASSERT(0              == index);
            x.commitPopIndex(generation, index);
            ASSERT(0 == X.length());

            ASSERT(0 == x.reservePushIndex(&generation, &index));
            ASSERT(0 == generation);
            ASSERT(0 == index);
            x.commitPushIndex(generation, index);
            ASSERT(1 == X.length());

            ASSERT(0 == x.reservePopIndex(&generation, &index));
            ASSERT(0 == generation);
            ASSERT(0 == index);
            x.commitPopIndex(generation, index);
            ASSERT(0 == X.length());
        }

        if (verbose) cout << "\nTest push and pop with various size buffers"
                          << endl;
        {
            struct {
               int          d_line;
               unsigned int d_capacity;
               unsigned int d_pushIndex;
               unsigned int d_popIndex;
            } DATA [] = {
                // Capacity 2 queues for a couple generations
                { L_, 2, 1, 0 },
                { L_, 2, 2, 0 },
                { L_, 2, 2, 1 },
                { L_, 2, 2, 2 },
                { L_, 2, 3, 1 },
                { L_, 2, 3, 2 },
                { L_, 2, 3, 3 },
                { L_, 2, 4, 2 },
                { L_, 2, 4, 3 },
                { L_, 2, 4, 4 },

                // Capacity 3 queues for a couple generations
                { L_, 3, 2, 0 },
                { L_, 3, 3, 0 },
                { L_, 3, 3, 1 },
                { L_, 3, 3, 2 },
                { L_, 3, 3, 3 },
                { L_, 3, 4, 1 },
                { L_, 3, 4, 2 },
                { L_, 3, 4, 3 },
                { L_, 3, 4, 4 },
                { L_, 3, 5, 2 },
                { L_, 3, 5, 3 },
                { L_, 3, 5, 4 },
                { L_, 3, 5, 5 },

                // Capacity 7 queue
                { L_, 7,  6,  0 },
                { L_, 7,  7,  0 },
                { L_, 7,  7,  6 },
                { L_, 7, 13,  7 },
                { L_, 7, 14,  7 },
            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);
            for (int i = 0; i < NUM_DATA; ++i) {
                const unsigned int LINE       = DATA[i].d_line;
                const unsigned int CAPACITY   = DATA[i].d_capacity;
                const unsigned int PUSH_INDEX = DATA[i].d_pushIndex;
                const unsigned int POP_INDEX  = DATA[i].d_popIndex;

                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;

                const unsigned NUM_GENERATIONS =
                                    Obj::numRepresentableGenerations(CAPACITY);
                const unsigned MAX_GENERATION = NUM_GENERATIONS - 1;

                dirtyAdjustGeneration(&x, MAX_GENERATION);

                if (veryVerbose) { P(LINE); P(X); }

                // Push and pop elements up until the pop-index.

                unsigned int gen, idx;
                for (unsigned int i = 0; i < POP_INDEX; ++i) {
                    unsigned int INDEX      = i % CAPACITY;
                    unsigned int GENERATION = (MAX_GENERATION + i/CAPACITY) %
                                              NUM_GENERATIONS;
                    ASSERT(0 == x.reservePushIndex(&gen, &idx));
                    ASSERT(INDEX      == idx);
                    ASSERT(GENERATION == gen);
                    x.commitPushIndex(gen, idx);
                    ASSERT(1 == X.length());

                    ASSERT(0 == x.reservePopIndex(&gen, &idx));

                    ASSERT(INDEX      == idx);
                    ASSERT(GENERATION == gen);
                    x.commitPopIndex(gen, idx);
                    ASSERT(0 == X.length());

                }

                // Push elements up to the push index

                for (unsigned int i = POP_INDEX; i < PUSH_INDEX; ++i) {
                    unsigned int INDEX      = i % CAPACITY;
                    unsigned int GENERATION = (MAX_GENERATION + i/CAPACITY) %
                                              NUM_GENERATIONS;

                    ASSERT(0 == x.reservePushIndex(&gen, &idx));

                    ASSERT(INDEX      == idx);
                    ASSERT(GENERATION == gen);
                    x.commitPushIndex(gen, idx);
                    ASSERT(i - POP_INDEX + 1 == X.length());
                }

                // Pop elements until the queue is empty.

                for (unsigned int i = POP_INDEX; i < PUSH_INDEX; ++i) {
                    unsigned int INDEX      = i % CAPACITY;
                    unsigned int GENERATION = (MAX_GENERATION + i/CAPACITY) %
                                              NUM_GENERATIONS;

                    ASSERT(0 == x.reservePopIndex(&gen, &idx));

                    ASSERT(INDEX      == idx);
                    ASSERT(GENERATION == gen);
                    x.commitPopIndex(gen, idx);
                    ASSERTV(X, X.length(), PUSH_INDEX - i - 1 == X.length());
                }
            }
        }

        if (verbose) cout << "\nTest reservePopIndexForClear and "
                          << "abortPusIndexReservation"
                          << endl;
        {
            struct {
               int          d_line;
               unsigned int d_capacity;
               unsigned int d_pushIndex;
               unsigned int d_popIndex;
            } DATA [] = {
                // Capacity 2 queues for a couple generations
                { L_, 2, 1, 0 },
                { L_, 2, 2, 1 },
                { L_, 2, 2, 2 },
                { L_, 2, 3, 2 },
                { L_, 2, 3, 3 },
                { L_, 2, 4, 3 },
                { L_, 2, 4, 4 },

                // Capacity 3 queues for a couple generations
                { L_, 3, 2, 0 },
                { L_, 3, 3, 1 },
                { L_, 3, 3, 2 },
                { L_, 3, 3, 3 },
                { L_, 3, 4, 2 },
                { L_, 3, 4, 3 },
                { L_, 3, 4, 4 },
                { L_, 3, 5, 3 },
                { L_, 3, 5, 4 },
                { L_, 3, 5, 5 },

                // Capacity 7 queue
                { L_, 7,  6,  0 },
                { L_, 7,  7,  6 },
                { L_, 7, 13,  7 },
            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);
            for (int i = 0; i < NUM_DATA; ++i) {
                const unsigned int LINE       = DATA[i].d_line;
                const unsigned int CAPACITY   = DATA[i].d_capacity;
                const unsigned int PUSH_INDEX = DATA[i].d_pushIndex;
                const unsigned int POP_INDEX  = DATA[i].d_popIndex;

                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;

                const unsigned NUM_GENERATIONS =
                                    Obj::numRepresentableGenerations(CAPACITY);
                const unsigned MAX_GENERATION = NUM_GENERATIONS - 1;

                dirtyAdjustGeneration(&x, MAX_GENERATION);

                if (veryVerbose) { P(LINE); P(X); }

                gg(&x, PUSH_INDEX, POP_INDEX);

                // Pop elements until the queue is empty.

                unsigned int endGen, endIdx, gen, idx;
                ASSERT(0 == x.reservePushIndex(&endGen, &endIdx));

                for (unsigned int i = POP_INDEX; i < PUSH_INDEX; ++i) {
                    unsigned int INDEX      = i % CAPACITY;
                    unsigned int GENERATION = (MAX_GENERATION + i/CAPACITY) %
                                              NUM_GENERATIONS;

                    ASSERT(0 == x.reservePopIndexForClear(
                                   &gen, &idx, endGen, endIdx));
                    ASSERT(INDEX      == idx);
                    ASSERT(GENERATION == gen);
                    x.commitPopIndex(gen, idx);

                }
                ASSERT(0 != x.reservePopIndexForClear(
                                   &gen, &idx, endGen, endIdx));
                x.abortPushIndexReservation(endGen, endIdx);
                ASSERT(0 == X.length());
            }
        }
        if (verbose) cout << "\nTest thread-safety iterating past the"
                          << "max combined index"
                          << endl;

        {
            bool COMPLETE = argc > 2 && bsl::string("--complete") == argv[2];

            // Configure this test to periodically delay threads.
            const int TOTAL_TIME_S = COMPLETE ? 60 : 3;
                // Total elapsed time for this test-case.  This value is not
                // meant to be accurate, but provides rough control over the
                // time the test takes.

            const unsigned int CAPACITY = 20;

            struct {
                int d_line;
                int d_delayPeriod;        // period with which to insert delays
                int d_numReaders;         // number of reader threads
                int d_numWriters;         // number of writer threads
                int d_numExceptions;      // number of exception threads
            } DATA[] = {

//               Line Delay Rdrs  Wrtrs  Excps
//              ======================================
                { L_,  0,    1,     1,    0 },
                { L_,  0,    5,     5,    1 },
                { L_,  0,    5,     5,    5 },
                { L_,  5,    5,     5,    1 },
                { L_,  5,    5,     5,    5 },

            };
            const int NUM_DATA = COMPLETE
                               ? static_cast<int>(sizeof(DATA) / sizeof(*DATA))
                               : 5;

            const double ELAPSED_TIME_PER_TEST = (double)TOTAL_TIME_S /
                                                         NUM_DATA;

            for (int i = 0; i < NUM_DATA; ++i) {

                int DELAY          = DATA[i].d_delayPeriod;
                int NUM_READERS    = DATA[i].d_numReaders;
                int NUM_WRITERS    = DATA[i].d_numWriters;
                int NUM_EXCEPTIONS = DATA[i].d_numExceptions;
                int NUM_THREADS    = NUM_READERS+NUM_WRITERS+NUM_EXCEPTIONS;

                TestThreadStateBarrier state(NUM_THREADS);
                Obj x(CAPACITY);  const Obj& X = x;

                unsigned NUM_GENERATIONS =
                                    Obj::numRepresentableGenerations(CAPACITY);
                unsigned MAX_GENERATION = NUM_GENERATIONS - 1;

                bsl::vector<bslmt::ThreadUtil::Handle> handles;
                handles.resize(NUM_THREADS);
                int thread = 0;

                for (int i = 0; i < NUM_WRITERS; ++i) {
                    int rc = bslmt::ThreadUtil::create(
                        &handles[thread],
                        bdlf::BindUtil::bind(&writerThread,
                                             &x,
                                             &state,
                                             DELAY));
                    BSLS_ASSERT_OPT(0 == rc); // test invariant
                    ++thread;
                }
                for (int i = 0; i < NUM_READERS; ++i) {
                    int rc = bslmt::ThreadUtil::create(
                     &handles[thread],
                     bdlf::BindUtil::bind(&readerThread, &x, &state, DELAY));
                    BSLS_ASSERT_OPT(0 == rc); // test invariant
                    ++thread;
                }
                for (int i = 0; i < NUM_EXCEPTIONS; ++i) {
                    int rc = bslmt::ThreadUtil::create(
                        &handles[thread],
                        bdlf::BindUtil::bind(&exceptionThread,
                                             &x,
                                             &state,
                                             DELAY));
                    BSLS_ASSERT_OPT(0 == rc); // test invariant
                    ++thread;
                }

                bsls::Stopwatch s;
                s.start();
                do {
                    dirtyAdjustGeneration(&x, MAX_GENERATION - 10);

                    if (veryVerbose) { P(X); }

                    state.continueTest();
                    while (1) {
                        bslmt::ThreadUtil::yield();
                        unsigned int popGeneration =
                                     FixedQueueState(&x).popGeneration();
                        if (popGeneration > 10 &&
                            popGeneration <= MAX_GENERATION / 2)
                            break;
                    }
                    state.suspendTest();
                    assertValidState(&x);
                    if (veryVerbose) { P(X); }
                } while (s.elapsedTime() <= ELAPSED_TIME_PER_TEST);
                state.continueTest();
                state.exitTest();
                for (int i = 0; i < NUM_THREADS; ++i) {
                    bslmt::ThreadUtil::join(handles[i]);
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // CONCERN: Thread-Safety (concurrent access does not corrupt state)
        //
        // Concerns:
        //  1 That concurrent use of the manipulators will not corrupt the
        //    state of the buffer.
        //
        // Plan:
        //  1 For a table driven number of reading threads, writing threads,
        //    "exception" threads (threads calling 'reservePopIndexForClear'
        //    and 'abortPushIndexResevation'), and injected delay value:
        //
        //    1 Create the described set of threads and supply the periodic
        //      delay value.
        //
        //    2 Execute those threads for a period of time, interupting them
        //      periodically to validate their state with 'assertValidState'.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCERN: Thread-Safety (concurrent modification)"
                          << endl
                          << "================================================"
                          << endl;

        bool COMPLETE = argc > 2 &&
                        bsl::string("--complete") == argv[2];

        // Configure this test to periodically delay threads.
        const int TOTAL_TIME_S = COMPLETE ? 60 : 3;
                // Total elapsed time for this test-case.  This value is not
                // meant to be accurate, but provides rough control over the
                // time the test takes.

        const int NUM_PROBES = 10;
                // Number of probes to perform for each test

        struct {
                int d_line;
                int d_capacity;           // queue capacity
                int d_startGeneration;    // start generation for the queue
                int d_delayPeriod;        // period with which to insert delays
                int d_numReaders;         // number of reader threads
                int d_numWriters;         // number of writer threads
                int d_numExceptions;      // number of exception threads
        } DATA[] = {

//           Line Cap Gen  Delay Rdrs  Wrtrs  Excps
//           ======================================
            { L_,  15,  0,     0,  5,     5,    0 },
            { L_,  15,  0,     0,  5,     5,    1 },
            { L_,  15,  0,     0,  5,     5,    5 },
            { L_,  15,  0,     5,  5,     5,    1 },
            { L_,  15,  0,     5,  5,     5,    5 },

//                       COMPREHENSIVE TESTS
//
//           Line Cap Gen  Delay Rdrs  Wrtrs  Excps
//           ======================================
            { L_,   1,  0,     0,  5,     5,    0 },
            { L_,   1,  0,     0,  5,     5,    1 },
            { L_,   1,  0,     0,  5,     5,    5 },
            { L_,   1,  0,     5,  5,     5,    1 },
            { L_,   1,  0,     5,  5,     5,    5 },

            { L_,  15,  0,     2,  5,     5,    0 },
            { L_,  15,  0,     2,  5,     5,    1 },
            { L_,  15,  0,     2,  5,     5,    5 },

        };

        const int NUM_DATA = COMPLETE
                           ? static_cast<int>(sizeof(DATA) / sizeof(*DATA))
                           : 5;

        const double ELAPSED_TIME_PER_TEST =  (double)TOTAL_TIME_S / NUM_DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            int CAPACITY       = DATA[i].d_capacity;
            int DELAY          = DATA[i].d_delayPeriod;
            int NUM_READERS    = DATA[i].d_numReaders;
            int NUM_WRITERS    = DATA[i].d_numWriters;
            int NUM_EXCEPTIONS = DATA[i].d_numExceptions;
            int NUM_THREADS    = NUM_READERS + NUM_WRITERS + NUM_EXCEPTIONS;

            TestThreadStateBarrier state(NUM_THREADS);
            Obj x(CAPACITY);  const Obj& X = x;

            bsl::vector<bslmt::ThreadUtil::Handle> handles;
            handles.resize(NUM_THREADS);
            int thread = 0;

            for (int i = 0; i < NUM_WRITERS; ++i) {
                int rc = bslmt::ThreadUtil::create(
                     &handles[thread],
                     bdlf::BindUtil::bind(&writerThread, &x, &state, DELAY));
                BSLS_ASSERT_OPT(0 == rc); // test invariant
                ++thread;
            }
            for (int i = 0; i < NUM_READERS; ++i) {
                int rc = bslmt::ThreadUtil::create(
                     &handles[thread],
                     bdlf::BindUtil::bind(&readerThread, &x, &state, DELAY));
                BSLS_ASSERT_OPT(0 == rc); // test invariant
                ++thread;
            }
            for (int i = 0; i < NUM_EXCEPTIONS; ++i) {
                int rc = bslmt::ThreadUtil::create(
                     &handles[thread],
                     bdlf::BindUtil::bind(&exceptionThread,
                                          &x,
                                          &state,
                                          DELAY));
                BSLS_ASSERT_OPT(0 == rc); // test invariant
                ++thread;
            }

            state.continueTest();

            double delayPerIter = ELAPSED_TIME_PER_TEST / NUM_PROBES;
            for (int i = 0; i < NUM_PROBES; ++i) {
                bslmt::ThreadUtil::sleep(bsls::TimeInterval(delayPerIter));
                state.suspendTest();
                assertValidState(&x);
                if (veryVeryVerbose) {
                    P(X);
                }
                state.continueTest();
            }
            state.exitTest();

            for (int i = 0; i < NUM_THREADS; ++i) {
                bslmt::ThreadUtil::join(handles[i]);
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING: 'print'
        //
        // Concerns:
        //  1 'print' renders the state of the object to the stream
        //
        // Plan:
        //  1 'print' the state of a stream and test the output against an
        //    expected result
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: print" << endl
                          << "==============" << endl;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wconversion"
#endif
        {
            bslma::TestAllocator oa;
            Obj x(4, &oa); const Obj &X = x;

            gg(&x, 7, 4);

            unsigned int generation, index;
            x.reservePushIndex(&generation, &index);
            x.reservePopIndex(&generation, &index);
            x.commitPopIndex(generation, index);
            x.reservePopIndex(&generation, &index);
            const char *EXPECTED = "\n"
                "        capacity: 4\n"
                "         enabled: 1\n"
                "   maxGeneration: 536870911\n"
                "maxCombinedIndex: 2147483647\n"
                "  pushGeneration: 2\n"
                "       pushIndex: 0\n"
                "   popGeneration: 1\n"
                "        popIndex: 2\n"
                "       0: { 2   | EMPTY   } <-- push\n"
                "       1: { 1   | READING }\n"
                "       2: { 1   | FULL    } <-- pop\n"
                "       3: { 1   | WRITING }\n";

            if (veryVerbose) {
                P(X);
            }
            bsl::ostringstream stream;
            X.print(stream);
            ASSERTV(stream.str(), EXPECTED, stream.str() == EXPECTED);
        }

        {
            bslma::TestAllocator oa;
            Obj x(11, &oa); const Obj &X = x;

            gg(&x, 1234, 1234);

            const char *EXPECTED = "\n"
                "        capacity: 11\n"
                "         enabled: 1\n"
                "   maxGeneration: 195225785\n"
                "maxCombinedIndex: 2147483645\n"
                "  pushGeneration: 112\n"
                "       pushIndex: 2\n"
                "   popGeneration: 112\n"
                "        popIndex: 2\n"
                "       0: { 113 | EMPTY   }\n"
                "       1: { 113 | EMPTY   }\n"
                "       2: { 112 | EMPTY   } <-- push & pop\n"
                "       3: { 112 | EMPTY   }\n"
                "       4: { 112 | EMPTY   }\n"
                "       5: { 112 | EMPTY   }\n"
                "       6: { 112 | EMPTY   }\n"
                "       7: { 112 | EMPTY   }\n"
                "       8: { 112 | EMPTY   }\n"
                "       9: { 112 | EMPTY   }\n"
                "      10: { 112 | EMPTY   }\n";

            if (veryVerbose) {
                P(X);
            }
            bsl::ostringstream stream;
            X.print(stream);
            ASSERTV(stream.str(), EXPECTED, stream.str() == EXPECTED);
        }
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: 'circularDifference'
        //
        // Concerns:
        //  1 Verify that the returned is the absolute value of the minimum
        //    distance between two points in a circular number space of the
        //    supplied size.
        //
        //  2 Verify the sign of the returned value correctly indicates
        //    whether one must increment or decrement that distance from the
        //    'substrahend' to arrive at the minuend.
        //
        // Plan:
        //  1 For set of interesting capacity values, call
        //    'numRepresentableGenerations' and verify the expected
        //    mathematical properties of the returned value.
        //
        // Testing:
        //   unsigned int numRepresentableGenerations(unsigned int );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING: numRepresentableGenerations"
                          << endl;

        if (verbose) cout << "\nTesting against pre-computed values" << endl;
        {

            const unsigned int INT_MAX_PLUS_1 =
                                            static_cast<unsigned>(INT_MAX) + 1;
            const int           INT_MAX_DIV_2 = INT_MAX_PLUS_1 / 2;

            struct {
                int          d_line;
                unsigned int d_minuend;
                unsigned int d_subtrahend;
                unsigned int d_maxSize;
                int          d_expectedValue;
            } DATA[] = {
                // capacity 1
                { L_, 0,  0,  1,  0 },

                // capacity 2
                { L_, 1,  1,  2,  0 },
                { L_, 1,  0,  2,  1 },
                { L_, 0,  1,  2, -1 },

                // capacity 3
                { L_, 2,  0,  3, -1 },
                { L_, 2,  1,  3,  1 },
                { L_, 2,  2,  3,  0 },
                { L_, 1,  0,  3,  1 },
                { L_, 1,  1,  3,  0 },
                { L_, 1,  2,  3, -1 },
                { L_, 0,  0,  3,  0 },
                { L_, 0,  1,  3, -1 },
                { L_, 0,  2,  3,  1 },

                // capacity 4
                { L_, 3,  0,  4, -1 },
                { L_, 3,  1,  4,  2 },
                { L_, 3,  2,  4,  1 },
                { L_, 3,  3,  4,  0 },
                { L_, 0,  3,  4,  1 },
                { L_, 1,  3,  4, -2 },
                { L_, 2,  3,  4, -1 },
                { L_, 3,  3,  4,  0 },

                // capacity INT_MAX
          { L_,       INT_MAX,             0, INT_MAX_PLUS_1,             -1 },
          { L_,             0,       INT_MAX, INT_MAX_PLUS_1,              1 },
          { L_, INT_MAX_DIV_2,             0, INT_MAX_PLUS_1,  INT_MAX_DIV_2 },
          { L_,             0, INT_MAX_DIV_2, INT_MAX_PLUS_1, -INT_MAX_DIV_2 },

                // Examples circularDifference( 0, 359, 360) == 1
                // circularDifference( 359, 0, 360) == -1 circularDifference(
                // 180, 0, 360) == 180 circularDifference( 0, 180, 360) == -180

                { L_,   0, 359,  360,    1 },
                { L_, 359,   0,  360,   -1 },
                { L_, 180,   0,  360,  180 },
                { L_,   0, 180,  360, -180 },

            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE       = DATA[i].d_line;
                const unsigned int MINUEND    = DATA[i].d_minuend;
                const unsigned int SUBTRAHEND = DATA[i].d_subtrahend;
                const unsigned int SIZE       = DATA[i].d_maxSize;
                const int          EXPECTED   = DATA[i].d_expectedValue;

                ASSERTV(LINE,
                        Obj::circularDifference(MINUEND, SUBTRAHEND, SIZE),
                        EXPECTED ==
                        Obj::circularDifference(MINUEND, SUBTRAHEND, SIZE));
            }

        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING: 'numRepresentableGenerations'
        //
        // Concerns:
        //  1 Verify the 'numRepresentableGenerations' value is the maximum
        //    number of complete generations that can fit into a 32bit integer
        //    where 2 bits have been reserved OR when multiplied by the
        //    capacity is the maximum number of combined indices that can be
        //    represented in a 32bit integer with 1 byte removed.
        //
        //
        // Plan:
        //  1 For set of interesting capacity values, call
        //    'numRepresentableGenerations' and verify the expected
        //    mathematical properties of the returned value.
        //
        // Testing:
        //   unsigned int numRepresentableGenerations(unsigned int );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING: numRepresentableGenerations"
                          << endl;
        {

            unsigned int DATA[] = {
                1,
                2,
                3,
                4,
                15,
                16,
                17,
                Obj::k_MAX_CAPACITY - 1,
                Obj::k_MAX_CAPACITY
            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const unsigned int CAPACITY = DATA[i];
                const unsigned int numRepGen =
                               Obj::numRepresentableGenerations(CAPACITY);

                // The maximum element state generation is the maximum value
                // that can be represented in a 32bit integer with 2 bits used
                // for state.  The maximum combined index is the maximum value
                // that can be represented in a 32bit integer with 1 bit used
                // for a disabled flag.

                const unsigned int MAX_ELEM_STATE_GEN = (UINT_MAX >> 2);
                const unsigned int MAX_COMB_INDEX     = (UINT_MAX >> 1);

                // Assert that either the 'numRepresentableGenerations' value
                // is the maximum that can be put into a element state value
                // (32 bit integer with 2 bits used for state), or is the
                // maximum value that can fit into a push index (32 bit integer
                // with 1 bit used for disabled).

                ASSERTV(numRepGen, numRepGen >= 2);
                ASSERTV(CAPACITY, numRepGen,
                        MAX_ELEM_STATE_GEN == numRepGen - 1 ||
                        ((numRepGen * CAPACITY - 1      <= MAX_COMB_INDEX) &&
                        ((numRepGen + 1) * CAPACITY - 1 >  MAX_COMB_INDEX)));

            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING: 'abortPushIndexReservation'
        //
        // Concerns:
        //  1 'abortPushIndexReservation' marks the cell reserved for writing
        //     as empty.
        //
        //  2 'abortPushIndexReservation' increments the push index
        //
        //  3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //  1 For a queue of capacity 1, call 'abortPushIndexReseevation' and
        //    verify that the push index is incremented and the reserved cell
        //    is marked empty. (C-1,2).
        //
        //  2 For a table driven set of possible queue states, initialize a
        //    queue to that state, call 'reservePushIndex' and then
        //    'reservePopIndexForClear' until 'reservePopIndexForClear'
        //    returns failure.  Call 'abortPushIndexReservation', then call
        //    'reservePushIndex' repeatedly to verify that the push index was
        //    correctly implemented and all the cells were marked empty.
        //    (C-1,2).
        //
        // Testing:
        //   void abortPushIndexReservation(unsigned int, unsigned int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: abortPushIndexReservation" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\nTest with a buffer of capacity 1"
                          << endl;

        {

            // Note that its not possible to clear a pop index with a queue of
            // capacity 1.

            unsigned int genA, idxA, genB, idxB;
            bslma::TestAllocator oa;
            Obj x(1, &oa); const Obj &X = x;

            ASSERT(0 == x.reservePushIndex(&genA, &idxA));
            ASSERT(0 != x.reservePushIndex(&genA, &idxA));
            x.abortPushIndexReservation(genA, idxA);
            ASSERT(0 == X.length());
            ASSERT(0 == x.reservePushIndex(&genB, &idxB));

            ASSERT(genA + 1 == genB);
            ASSERT(idxA     == idxB);
        }

        if (verbose) cout << "\nTest various queues of elements"
                          << endl;
        {
            struct {
               int          d_line;
               unsigned int d_capacity;
               unsigned int d_pushIndex;
               unsigned int d_popIndex;
               unsigned int d_expectedClears;
            } DATA [] = {
                { L_, 1, 0, 0, 0 },

                // Capacity 2 queues for a couple generations
                { L_, 2, 0, 0, 0 },
                { L_, 2, 1, 0, 1 },
                { L_, 2, 1, 1, 0 },
                { L_, 2, 2, 1, 1 },
                { L_, 2, 2, 2, 0 },
                { L_, 2, 3, 2, 1 },
                { L_, 2, 3, 3, 0 },

                // Capacity 3 queues for a couple generations
                { L_, 3, 0, 0, 0 },
                { L_, 3, 1, 0, 1 },
                { L_, 3, 1, 1, 0 },
                { L_, 3, 2, 0, 2 },
                { L_, 3, 2, 1, 1 },
                { L_, 3, 2, 2, 0 },
                { L_, 3, 3, 1, 2 },
                { L_, 3, 3, 2, 1 },
                { L_, 3, 3, 3, 0 },
                { L_, 3, 4, 2, 2 },
                { L_, 3, 4, 3, 1 },
                { L_, 3, 4, 4, 0 },

                // Capacity 7 queue
                { L_, 7, 14, 14, 0 },
                { L_, 7, 15, 14, 1 },
                { L_, 7, 20, 14, 6 },
                { L_, 7, 18, 18, 0 },
                { L_, 7, 19, 18, 1 },
                { L_, 7, 24, 18, 6 },

            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const unsigned int CAPACITY   = DATA[i].d_capacity;
                const unsigned int PUSH_INDEX = DATA[i].d_pushIndex;
                const unsigned int POP_INDEX  = DATA[i].d_popIndex;
                const unsigned int LINE       = DATA[i].d_line;
                const unsigned int NUM_CLEARS = DATA[i].d_expectedClears;

                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;

                gg(&x, PUSH_INDEX, POP_INDEX);

                if (veryVerbose) { P(LINE); P(X); }

                const unsigned int endGen = PUSH_INDEX / CAPACITY;
                const unsigned int endIdx = PUSH_INDEX % CAPACITY;

                unsigned int gen, idx;
                ASSERT(0 == x.reservePushIndex(&gen, &idx));
                ASSERT(endGen == gen);
                ASSERT(endIdx == idx);

                for (unsigned int j = 0; j < NUM_CLEARS; ++j) {
                    ASSERT(0 == x.reservePopIndexForClear(
                                      &gen, &idx, endGen, endIdx));
                    ASSERT((POP_INDEX + j) / CAPACITY == gen);
                    ASSERT((POP_INDEX + j) % CAPACITY == idx);
                    x.commitPopIndex(gen, idx);
                }
                ASSERT(0 != x.reservePopIndexForClear(
                                  &gen, &idx, endGen, endIdx));

                // Call abort push index.
                x.abortPushIndexReservation(endGen, endIdx);

                // Verify the queue is now empty and the current push index has
                // been incremented from the cell previously reserved for
                // writing.

                ASSERT(0 == X.length());
                for (unsigned int j = 0; j < CAPACITY; ++j) {
                    ASSERT(0 == x.reservePushIndex(&gen, &idx));
                    ASSERT(j + 1 == X.length());
                    ASSERT(endGen * CAPACITY + endIdx + j + 1 ==
                           gen    * CAPACITY + idx);

                }
                ASSERT(0 != x.reservePushIndex(&gen, &idx));
                ASSERT(CAPACITY == X.length());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator oa;

            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            unsigned int resultGen, resultIdx, index, generation;

            Obj obj(10, &oa);
            obj.reservePushIndex(&generation, &index);

            ASSERT_FAIL(obj.abortPushIndexReservation(generation, UINT_MAX));
            ASSERT_FAIL(obj.abortPushIndexReservation(UINT_MAX, index));
            ASSERT_FAIL(obj.abortPushIndexReservation(generation, index + 1));
            ASSERT_PASS(obj.abortPushIndexReservation(generation, index));

            obj.reservePushIndex(&generation, &index);
            obj.commitPushIndex(generation, index);
            obj.reservePushIndex(&generation, &index);
            obj.commitPushIndex(generation, index);
            obj.reservePushIndex(&generation, &index);

            ASSERT_FAIL(obj.abortPushIndexReservation(generation, index));

            obj.reservePopIndexForClear(&resultGen,
                                        &resultIdx,
                                        generation,
                                        index);
            obj.reservePopIndexForClear(&resultGen,
                                        &resultIdx,
                                        generation,
                                        index);

            ASSERT_PASS(obj.abortPushIndexReservation(generation, index));
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING: 'reservePopIndexForClear'
        //
        // Concerns:
        //  1 An error status is returned when the head of the queue refers to
        //    the generation and index supplied to 'endGeneration' and
        //    'endIndex'
        //
        //  2 'reservePopIndexForClear' returns success, clears the head of
        //     the queue if the current combined pop index is not
        //     'endGeneration' and 'endIndex'.
        //
        //  3 'reservePopIndexForClear' will not clear an index acquired for
        //    popping.
        //
        //  4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //
        //  1 For a queue of capacity 1, attempt to call
        //    'reservePopIndexForClear' and verify that it fails. (C-1,3)
        //
        //  2 For a queue of capacity 2, perform several iterations, pushing
        //    an item onto the queue, and calling 'reservePopIndexForClear'.
        //    (C-1,2,3)
        //
        //  3 For a table driven set of possible initial queue states: call
        //    'reservePopIndexForClear' repeatedly unto it returns failure, and
        //    verify that it succeeds the expected number of times, returns the
        //    correct cleared indices, and leaves those indices empty.
        //    (C-1,2,3)
        //
        //  4 For an arbitrary full queue, iterate through the possible
        //    capacity values calling 'reservePopIndex' that number of times,
        //    prior to calling 'reservePopIndexForClear'.  Verify
        //    'reservePopIndexForClear' does not attempt to clear a index
        //    reserved for popping.  (C-1,2,3)
        //
        //  5 Use the assertion test facility to test function
        //    preconditions. (C-4)
        //
        // Testing:
        //   int reservePopIndexForClear
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: reservePopIndexForClear" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nTest with a buffer of capacity 1"
                          << endl;

        {
            // Note that its not possible to clear a pop index with a queue of
            // capacity 1.

            unsigned int gen, idx;
            unsigned int resultGen = 13;
            unsigned int resultIdx = 13;
            bslma::TestAllocator oa;
            Obj x(1, &oa); const Obj &X = x;

            ASSERT(0 == x.reservePushIndex(&gen, &idx));
            ASSERT(0 != x.reservePopIndexForClear(&resultGen,
                                                  &resultIdx,
                                                  0,
                                                  0));
            ASSERT(13 == resultGen);
            ASSERT(13 == resultIdx);
            ASSERT(1 == X.length());
        }

        if (verbose) cout << "\nTest with a buffer of capacity 2"
                          << endl;

        {
            unsigned int gen, idx;
            unsigned int resultGen = 13;
            unsigned int resultIdx = 13;
            bslma::TestAllocator oa;
            Obj x(2, &oa); const Obj &X = x;

            ASSERT(0 == x.reservePushIndex(&gen, &idx));

            if (veryVerbose) { P(L_); P(X); }
            ASSERT(0 != x.reservePopIndexForClear(
                                &resultGen, &resultIdx, gen, idx));
            ASSERT(13 == resultGen);
            ASSERT(13 == resultIdx);
            x.commitPushIndex(gen, idx);

            ASSERT(0 == x.reservePushIndex(&gen, &idx));
            if (veryVerbose) { P(L_); P(X); }
            ASSERT(0 == x.reservePopIndexForClear(
                                &resultGen, &resultIdx, gen, idx));
            ASSERT(0 == resultGen);
            ASSERT(0 == resultIdx);
            x.commitPopIndex(resultGen, resultIdx);

            x.commitPushIndex(gen, idx);
            ASSERT(0 == x.reservePushIndex(&gen, &idx));
            if (veryVerbose) { P(L_); P(X); }
            ASSERT(0 == x.reservePopIndexForClear(
                                &resultGen, &resultIdx, gen, idx));
            ASSERT(0 == resultGen);
            ASSERT(1 == resultIdx);
            x.commitPopIndex(resultGen, resultIdx);
            x.commitPushIndex(gen, idx);

            ASSERT(0 == x.reservePushIndex(&gen, &idx));
            if (veryVerbose) { P(L_); P(X); }
            ASSERT(0 == x.reservePopIndexForClear(
                                 &resultGen, &resultIdx, gen, idx));
            ASSERT(1 == resultGen);
            ASSERT(0 == resultIdx);
            x.commitPopIndex(resultGen, resultIdx);
            x.commitPushIndex(gen, idx);

            ASSERT(0 == x.reservePushIndex(&gen, &idx));
            if (veryVerbose) { P(L_); P(X); }
            ASSERT(0 == x.reservePopIndexForClear(
                                 &resultGen, &resultIdx, gen, idx));
            ASSERT(1 == resultGen);
            ASSERT(1 == resultIdx);
            x.commitPopIndex(resultGen, resultIdx);
            x.commitPushIndex(gen, idx);

            ASSERT(0 == x.reservePushIndex(&gen, &idx));
            if (veryVerbose) { P(L_); P(X); }
            ASSERT(0 == x.reservePopIndexForClear(
                                 &resultGen, &resultIdx, gen, idx));
            ASSERT(2 == resultGen);
            ASSERT(0 == resultIdx);
            x.commitPopIndex(resultGen, resultIdx);
            x.commitPushIndex(gen, idx);
        }

        if (verbose) cout << "\nTest various queues clear the expected number"
                          << " of elements"
                          << endl;
        {
            struct {
               int          d_line;
               unsigned int d_capacity;
               unsigned int d_pushIndex;
               unsigned int d_popIndex;
               unsigned int d_expectedClears;
            } DATA [] = {
                { L_, 1, 0, 0, 0 },

                // Capacity 2 queues for a couple generations
                { L_, 2, 0, 0, 0 },
                { L_, 2, 1, 0, 1 },
                { L_, 2, 1, 1, 0 },
                { L_, 2, 2, 1, 1 },
                { L_, 2, 2, 2, 0 },
                { L_, 2, 3, 2, 1 },
                { L_, 2, 3, 3, 0 },

                // Capacity 3 queues for a couple generations
                { L_, 3, 0, 0, 0 },
                { L_, 3, 1, 0, 1 },
                { L_, 3, 1, 1, 0 },
                { L_, 3, 2, 0, 2 },
                { L_, 3, 2, 1, 1 },
                { L_, 3, 2, 2, 0 },
                { L_, 3, 3, 1, 2 },
                { L_, 3, 3, 2, 1 },
                { L_, 3, 3, 3, 0 },
                { L_, 3, 4, 2, 2 },
                { L_, 3, 4, 3, 1 },
                { L_, 3, 4, 4, 0 },

                // Capacity 7 queue
                { L_, 7, 14, 14, 0 },
                { L_, 7, 15, 14, 1 },
                { L_, 7, 20, 14, 6 },
                { L_, 7, 18, 18, 0 },
                { L_, 7, 19, 18, 1 },
                { L_, 7, 24, 18, 6 },

            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const unsigned int CAPACITY   = DATA[i].d_capacity;
                const unsigned int PUSH_INDEX = DATA[i].d_pushIndex;
                const unsigned int POP_INDEX  = DATA[i].d_popIndex;
                const unsigned int LINE       = DATA[i].d_line;
                const unsigned int NUM_CLEARS = DATA[i].d_expectedClears;

                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;

                gg(&x, PUSH_INDEX, POP_INDEX);

                if (veryVerbose) { P(LINE); P(X); }

                const unsigned int endGen = PUSH_INDEX / CAPACITY;
                const unsigned int endIdx = PUSH_INDEX % CAPACITY;

                unsigned int gen, idx;
                ASSERT(0 == x.reservePushIndex(&gen, &idx));
                ASSERT(endGen == gen);
                ASSERT(endIdx == idx);

                for (unsigned int j = 0; j < NUM_CLEARS; ++j) {
                    ASSERT(0 == x.reservePopIndexForClear(
                                       &gen, &idx, endGen, endIdx));
                    ASSERT((POP_INDEX + j) / CAPACITY == gen);
                    ASSERT((POP_INDEX + j) % CAPACITY == idx);
                    x.commitPopIndex(gen, idx);
                }
                ASSERT(0 != x.reservePopIndexForClear(
                                       &gen, &idx, endGen, endIdx));
                x.commitPushIndex(endGen, endIdx);
                ASSERT(1 == X.length());
            }
        }

        if (verbose) cout << "\nTest that acquire-pop indices reduces the "
                          << "number of expected clears"
                          << bsl::endl;
        {

            const unsigned int CAPACITY   = 10;
            const unsigned int PUSH_INDEX = 4123;  // arbitrary
            const unsigned int POP_INDEX  = static_cast<unsigned int>(
                                            PUSH_INDEX - (CAPACITY - 1));

            for (unsigned int numPoppers = 0;
                 numPoppers < CAPACITY - 1;
                 ++numPoppers) {
                bslma::TestAllocator oa;
                Obj x(CAPACITY, &oa); const Obj &X = x;

                gg(&x, PUSH_INDEX, POP_INDEX);

                if (veryVerbose) { P(L_); P(X); }

                const unsigned int endGen = PUSH_INDEX / CAPACITY;
                const unsigned int endIdx = PUSH_INDEX % CAPACITY;

                unsigned int gen, idx;
                ASSERT(0 == x.reservePushIndex(&gen, &idx));
                ASSERT(endGen == gen);
                ASSERT(endIdx == idx);

                for (unsigned int i = 0; i < numPoppers; ++i) {
                    ASSERT(0 == x.reservePopIndex(&gen, &idx));
                    ASSERT((POP_INDEX + i) / CAPACITY == gen);
                    ASSERT((POP_INDEX + i) % CAPACITY == idx);
                }

                for (unsigned int i = 0; i < CAPACITY-numPoppers-1; ++i) {
                    ASSERT(0 == x.reservePopIndexForClear(
                                            &gen, &idx, endGen, endIdx));
                    ASSERT((POP_INDEX + numPoppers + i) / CAPACITY == gen);
                    ASSERT((POP_INDEX + numPoppers + i) % CAPACITY == idx);
                    x.commitPopIndex(gen, idx);
                }
                ASSERT(0 != x.reservePopIndexForClear(
                                            &gen, &idx, endGen, endIdx));
                ASSERT(1 == X.length());
            }
        }
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator oa;

            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            unsigned int resultGen, resultIdx, index, generation;

            Obj obj(10, &oa);
            obj.reservePushIndex(&generation, &index);

            ASSERT_FAIL(obj.reservePopIndexForClear(
                            &resultGen, &resultIdx, 0, 11));
            ASSERT_FAIL(obj.reservePopIndexForClear(
                            &resultGen, &resultIdx, UINT_MAX,0));
            ASSERT_FAIL(obj.reservePopIndexForClear(
                            &resultGen, 0, 0, 0));
            ASSERT_FAIL(obj.reservePopIndexForClear(
                            0, &resultIdx, 0, 0));
            ASSERT_PASS(obj.reservePopIndexForClear(
                            &resultGen, &resultIdx, 0, 0));
        }

      } break;
      case 5: {
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
                          << "TESTING: enabled & disabled" << endl
                          << "===========================" << endl;

        if (verbose) cout << "\nTest that a newly constructed obj is enabled"
                          << endl;
        {
            unsigned int generation, index;
            bslma::TestAllocator oa;
            Obj x(10, &oa); const Obj &X = x;

            ASSERTV(X.isEnabled());

            ASSERT(0 == x.reservePushIndex(&generation, &index));
            x.commitPushIndex(generation, index);
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
            ASSERT(0 == x.reservePushIndex(&generation, &index));
            x.commitPushIndex(generation, index);
            ASSERTV(1 == X.length());

            ASSERT(0 == x.reservePushIndex(&generation, &index));
            x.commitPushIndex(generation, index);
            ASSERTV(2 == X.length());

            // Disable the queue.
            x.disable();
            ASSERTV(!X.isEnabled());

            // Test that attempting to push fails.
            ASSERT(0 > x.reservePushIndex(&generation, &index));
            ASSERTV(2 == X.length());

            // Test that attempting to pop succeeds.
            ASSERT(0 == x.reservePopIndex(&generation, &index));
            x.commitPopIndex(generation, index);
            ASSERTV(1 == X.length());

            // Test that attempting to push still fails.
            ASSERT(0 > x.reservePushIndex(&generation, &index));
            ASSERTV(1 == X.length());

            // Disable the queue a second time, and verify that has no effect.
            x.disable();
            ASSERTV(!X.isEnabled());

            // Test that attempting to push still fails.
            ASSERT(0 > x.reservePushIndex(&generation, &index));
            ASSERTV(1 == X.length());

            // Enable the queue.
            x.enable();
            ASSERTV(X.isEnabled());

            // Test that attempting to push succeeds.
            ASSERT(0 == x.reservePushIndex(&generation, &index));
            x.commitPushIndex(generation, index);
            ASSERTV(2 == X.length());

            // Test that attempting to pop succeeds.
            ASSERT(0 == x.reservePopIndex(&generation, &index));
            x.commitPopIndex(generation, index);
            ASSERTV(1 == X.length());

            // Enable the queue a second time, and verify that has no effect.
            x.enable();
            ASSERTV(X.isEnabled());

            // Test that attempting to push succeeds.
            ASSERT(0 == x.reservePushIndex(&generation, &index));
            x.commitPushIndex(generation, index);
            ASSERTV(2 == X.length());

        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'gg'
        //   Ensure that the generator function is able to create an object in
        //   any state.
        //
        // Concerns:
        //   The 'gg' generator is implemented as a finite state machine (FSM).
        //   All symbols must be recognized successfully, and all possible
        //   state transitions must be verified.
        //
        // Plan:
        //   Evaluate a series of test strings of increasing complexity to
        //   validate every major state and transition in the FSM underlying
        //   gg.  Add extra tests to validate less likely transitions until
        //   they are all covered.
        //
        // Testing:
        //   Obj& gg(Obj *object, unsigned, unsigned);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Generator Functions" << endl
                          << "===================" << endl;

        struct {
            int          d_line;
            unsigned int d_capacity;
            unsigned int d_pushIndex;
            unsigned int d_popIndex;
            bool         d_nextPush;
            bool         d_nextPop;
        } VALUES[] = {
            { L_,  1,  0,  0,    true, false },
            { L_,  1,  1,  0,   false,  true },
            { L_,  1,  1,  1,    true, false },
            { L_,  1,  2,  1,   false,  true },

            { L_,  2,  0,  0,    true, false },
            { L_,  2,  1,  0,    true,  true },
            { L_,  2,  1,  1,    true, false },
            { L_,  2,  2,  0,   false,  true },
            { L_,  2,  2,  1,    true,  true },
            { L_,  2,  2,  2,    true, false },
            { L_,  2,  3,  1,   false,  true },
            { L_,  2,  3,  2,    true,  true },
            { L_,  2,  3,  3,    true, false },
            { L_,  2,  4,  2,   false,  true },
            { L_,  2,  4,  3,    true,  true },
            { L_,  2,  4,  4,    true, false },

            { L_,  7, 14, 14,    true, false },
            { L_,  7, 15, 14,    true,  true },
            { L_,  7, 16, 15,    true,  true },
            { L_,  7, 16, 16,    true, false },
            { L_,  7, 23, 16,   false,  true }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        if (verbose) cout << "\nTest 'gg'" << endl;
        {
            const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const unsigned int CAPACITY   = VALUES[i].d_capacity;
                const unsigned int PUSH_INDEX = VALUES[i].d_pushIndex;
                const unsigned int POP_INDEX  = VALUES[i].d_popIndex;
                const unsigned int LINE       = VALUES[i].d_line;

                if (veryVerbose) { P(LINE); }

                // Test push index
                {
                    const bool         SUCCESS    = VALUES[i].d_nextPush;
                    const unsigned int GENERATION = PUSH_INDEX / CAPACITY;
                    const unsigned int INDEX      = PUSH_INDEX % CAPACITY;

                    bslma::TestAllocator oa;
                    Obj x(CAPACITY, &oa); const Obj& X = x;

                    gg(&x, PUSH_INDEX, POP_INDEX);

                    if (veryVerbose) { P(X); }

                    unsigned int generation, index;
                    ASSERT(X.length() ==  PUSH_INDEX - POP_INDEX);

                    int rc = x.reservePushIndex(&generation, &index);

                    ASSERT(SUCCESS == (0 == rc));
                    if (0 == rc) {
                        ASSERT(GENERATION == generation);
                        ASSERT(INDEX      == index);
                    }
                }
                // Test pop index
                {
                    const bool         SUCCESS    = VALUES[i].d_nextPop;
                    const unsigned int GENERATION = POP_INDEX / CAPACITY;
                    const unsigned int INDEX      = POP_INDEX % CAPACITY;

                    bslma::TestAllocator oa;
                    Obj x(CAPACITY, &oa); const Obj& X = x;

                    gg(&x, PUSH_INDEX, POP_INDEX);

                    if (veryVerbose) { P(X); }

                    unsigned int generation, index;
                    ASSERT(X.length() ==  PUSH_INDEX - POP_INDEX);

                    int rc = x.reservePopIndex(&generation, &index);

                    ASSERT(SUCCESS == (0 == rc));
                    if (0 == rc) {
                        ASSERT(GENERATION == generation);
                        ASSERT(INDEX      == index);
                    }
                }

            }
        }

        if (verbose) cout << "\nTest 'dirtyGG'" << endl;

        {

            for (int i = 0; i < NUM_VALUES; ++i) {
                const unsigned int CAPACITY   = VALUES[i].d_capacity;
                const unsigned int PUSH_INDEX = VALUES[i].d_pushIndex;
                const unsigned int POP_INDEX  = VALUES[i].d_popIndex;
                const unsigned int LINE       = VALUES[i].d_line;

                if (veryVerbose) { P(LINE); }

                // Test push index
                {
                    const bool         SUCCESS    = VALUES[i].d_nextPush;
                    const unsigned int GENERATION = PUSH_INDEX / CAPACITY;
                    const unsigned int INDEX      = PUSH_INDEX % CAPACITY;

                    bslma::TestAllocator oa;
                    Obj x(CAPACITY, &oa); const Obj& X = x;

                    dirtyGG(&x, PUSH_INDEX, POP_INDEX);

                    if (veryVerbose) { P(X); }

                    unsigned int generation, index;
                    ASSERT(X.length() ==  PUSH_INDEX - POP_INDEX);

                    int rc = x.reservePushIndex(&generation, &index);

                    ASSERT(SUCCESS == (0 == rc));
                    if (0 == rc) {
                        ASSERT(GENERATION == generation);
                        ASSERT(INDEX      == index);
                    }
                }
                // Test pop index
                {
                    const bool         SUCCESS    = VALUES[i].d_nextPop;
                    const unsigned int GENERATION = POP_INDEX / CAPACITY;
                    const unsigned int INDEX      = POP_INDEX % CAPACITY;

                    bslma::TestAllocator oa;
                    Obj x(CAPACITY, &oa); const Obj& X = x;

                    dirtyGG(&x, PUSH_INDEX, POP_INDEX);

                    if (veryVerbose) { P(X); }

                    unsigned int generation, index;
                    ASSERT(X.length() ==  PUSH_INDEX - POP_INDEX);

                    int rc = x.reservePopIndex(&generation, &index);

                    ASSERT(SUCCESS == (0 == rc));
                    if (0 == rc) {
                        ASSERT(GENERATION == generation);
                        ASSERT(INDEX      == index);
                    }
                }
            }
        }

        if (verbose) cout << "\nTest 'assertValidState'" << endl;
        {
            for (unsigned int capacity = 0; capacity < 5; ++capacity) {
                for (unsigned int startIdx = 0;
                                  startIdx < capacity * 3;
                                  ++startIdx) {
                    for (unsigned int length = 0;
                                      length <= capacity;
                                      ++length) {
                        bslma::TestAllocator oa;
                        Obj x(capacity, &oa); const Obj& X = x;

                        gg(&x, startIdx + length, startIdx);
                        assertValidState(&x);
                        if (veryVeryVerbose) {
                            P(X);
                        }
                    }

                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND ACCESSORS
        //
        // Concerns:
        //  1  'length' on a newly constructed queue returns 0.
        //
        //  2  'acqurePushIndex' increments the 'length' of the queue.
        //
        //  3  Calling 'reservePushIndex' multiple times will not return
        //     the same index.
        //
        //  4  Calling 'reservePushIndex' when the queue is full returns the
        //     correct error.
        //
        //  5  Calling 'reservePopIndex' when the queue is empty returns the
        //     correct error.
        //
        //  6  Calling 'reservePopIndex' when the queue is not empty, and there
        //     are released push indices, returns the oldest released push
        //     index and the generation from where it was pushed, and
        //     decrements the length.
        //
        //  7  Calling 'commitPushIndex' permits the index to later be
        //     acquired for popping.
        //
        //  8  Calling 'commitPopIndex' permits the index to later be
        //     acquired for pushing
        //
        //  9  If 'commitPopIndex' or 'commitPushIndex' fails than the return
        //     arguments are unmodified.
        //
        //  10 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //
        //  1 For each capcity value in a series of capacity values, call
        //    'reservePushIndex' to that maximum capacity.  At each point,
        //    verify the expected index, generation count, and return
        //    status. (C-1,2,3)
        //
        //  2 For each capcity value in a series of capacity values:
        //
        //    1 Call 'reservePopIndex' and verify it fails.
        //
        //    2 Push and pop an index, and verify the expected index, length,
        //      and generation counts.  (C-5)
        //
        //  3 For each capcity value in a series of capacity values:
        //
        //    1 Fill the queue
        //
        //    2 Call 'reservePushIndex' and verify it fails.
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
        //   int reservePushIndex(unsigned int *, unsigned int *);
        //   void commitPushIndex(unsigned int , unsigned int );
        //   int reservePopIndex(unsigned int *, unsigned int *);
        //   void commitPopIndex(unsigned int , unsigned int );
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

                    int rc = x.reservePushIndex(&generation, &index);
                    ASSERTV(rc,            0 == rc);
                    ASSERTV(index, j,      j == index);
                    ASSERTV(generation,    0 == generation);
                    ASSERTV(X.length(),    j == X.length() - 1);
                    x.commitPushIndex(generation, index);
                }

                int rc = x.reservePushIndex(&generation, &index);
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
                        int rc = x.reservePopIndex(&generation, &index);
                        ASSERTV(rc,         0 < rc);

                        rc = x.reservePushIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), 1 == X.length());

                        x.commitPushIndex(generation, index);
                        ASSERTV(X.length(), 1 == X.length());

                        rc = x.reservePopIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), 0 == X.length());

                        x.commitPopIndex(generation, index);
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
                    x.reservePushIndex(&generation, &index);
                    x.commitPushIndex(generation, index);
                }
                ASSERTV(X.length(), X.length() == CAPACITY);

                for (unsigned int gen = 0; gen < 3; ++gen) {
                    for (unsigned int idx = 0; idx < CAPACITY; ++idx) {
                        int rc = x.reservePushIndex(&generation, &index);
                        ASSERTV(rc,         0 < rc);

                        rc = x.reservePopIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), X.length() == CAPACITY - 1);

                        x.commitPopIndex(generation, index);
                        ASSERTV(X.length(), X.length() == CAPACITY - 1);

                        rc = x.reservePushIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen + 1);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), X.length() == CAPACITY);

                        x.commitPushIndex(generation, index);
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
                    x.reservePushIndex(&generation, &index);
                    x.commitPushIndex(generation, index);
                }
                ASSERTV(X.length(), X.length() == CAPACITY);

                for (unsigned int gen = 0; gen < 3; ++gen) {
                    for (unsigned int idx = 0; idx < CAPACITY; ++idx) {
                        int rc = x.reservePushIndex(&generation, &index);
                        ASSERTV(rc,         0 < rc);

                        rc = x.reservePopIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), X.length() == CAPACITY - 1);

                        x.commitPopIndex(generation, index);
                        ASSERTV(X.length(), X.length() == CAPACITY - 1);

                        rc = x.reservePushIndex(&generation, &index);
                        ASSERTV(rc,         0 == rc);
                        ASSERTV(generation, generation == gen + 1);
                        ASSERTV(index,      index      == idx);
                        ASSERTV(X.length(), X.length() == CAPACITY);

                        x.commitPushIndex(generation, index);
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
            int maxOp = 12;
            ASSERT(4096 == 1 << maxOp);

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
                        bool isPush = bdlb::BitUtil::isBitSet(
                                                     (uint32_t)opSequence, op);
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
                            int rc = x.reservePushIndex(&gen, &index);
                            ASSERTV(rc, SUCCESS,        SUCCESS == (0 == rc));
                            ASSERTV(LENGTH, X.length(), LENGTH  == X.length());
                            if (0 != rc) {
                                continue;
                            }
                            x.commitPushIndex(gen, index);
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
                            int rc = x.reservePopIndex(&gen, &index);
                            ASSERTV(rc, SUCCESS,        SUCCESS == (0 == rc));
                            ASSERTV(LENGTH, X.length(), LENGTH  == X.length());
                            if (0 != rc) {
                                continue;
                            }
                            x.commitPopIndex(gen, index);
                            ASSERTV(POP_INDEX, index, POP_INDEX == index);
                            ASSERTV(POP_GEN, gen,     POP_GEN == gen);
                        }
                   }
                }
            }
        }

        if (verbose) cout << "\nTest that function args are unmodified on "
                          << "failure"
                          << endl;
        {
            bslma::TestAllocator oa;
            Obj x(1, &oa); const Obj &X = x;

            unsigned int gen = 54321, idx = 12345;

            ASSERT(0 != x.reservePopIndex(&gen, &idx));
            ASSERT(54321 == gen);
            ASSERT(12345 == idx);

            ASSERT(0 == x.reservePushIndex(&gen, &idx));
            x.commitPushIndex(gen, idx);

            ASSERT(0 == gen);
            ASSERT(0 == idx);

            gen = 54321;
            idx = 12345;

            ASSERT(0 != x.reservePushIndex(&gen, &idx));
            ASSERT(54321 == gen);
            ASSERT(12345 == idx);

            ASSERT(1 == X.length());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator oa;

            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            unsigned int index, generation;

            {
                // test reserve push index.

                Obj obj(10, &oa);
                ASSERT_FAIL(obj.reservePushIndex(&generation, 0));
                ASSERT_FAIL(obj.reservePushIndex(0, &index));
                ASSERT_PASS(obj.reservePushIndex(&generation, &index));
            }
            {
                // test commit push index.

                Obj obj(10, &oa);
                obj.reservePushIndex(&generation, &index);
                ASSERT_FAIL(obj.commitPushIndex(generation,
                                                Obj::k_MAX_CAPACITY));
                ASSERT_FAIL(obj.commitPushIndex(UINT_MAX, index));
                ASSERT_PASS(obj.commitPushIndex(generation, index));
                ASSERT_FAIL(obj.commitPushIndex(generation, index));

                obj.reservePopIndex(&generation, &index);
                ASSERT_FAIL(obj.commitPushIndex(generation, index));
            }

            {
                // test reserve pop index.
                Obj obj(10, &oa);

                ASSERT_FAIL(obj.reservePopIndex(&generation, 0));
                ASSERT_FAIL(obj.reservePopIndex(0, &index));
                ASSERT_PASS(obj.reservePopIndex(&generation, &index));

            }

            {
                // test commit pop index.

                Obj obj(10, &oa);
                obj.reservePushIndex(&generation, &index);
                obj.commitPushIndex(generation, index);

                obj.reservePopIndex(&generation, &index);
                ASSERT_FAIL(obj.commitPopIndex(generation,
                                               Obj::k_MAX_CAPACITY));
                ASSERT_FAIL(obj.commitPopIndex(UINT_MAX, index));
                ASSERT_PASS(obj.commitPopIndex(generation, index));
                ASSERT_FAIL(obj.commitPopIndex(generation, index));

                obj.reservePushIndex(&generation, &index);
                obj.commitPushIndex(generation, index);

                ASSERT_FAIL(obj.commitPopIndex(generation, index));
            }
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
        // 5 k_MAX_CAPACITY is the maximum capacity the allows for at least 2
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
        // 2 Construct a test object and supply it a test allocator, verify
        //   that memory is allocated only from the supplied allocator. Verify
        //   allocated memory is released on destruction. (C-2,4)
        //
        // 3 Construct a test object and do not explicitly supply it an
        //   allocator, verify memory is allocated from the default
        //   allocator. Verify allocated memory is released on destruction.
        //   (C-3,4)
        //
        // 4 Verify that representing 'k_MAX_CAPACITY' in an unsigned int
        //   leaves 2 bits to represent the disabled state and 2 complete
        //   generations. (C-5).
        //
        // 5 Use the assertion test facility to test function preconditions.
        //
        // Testing:
        //   bdlcc::FixedQueueIndexManager(unsigned int, Allocator *);
        //   ~bdlcc::FixedQueueIndexManager();
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
                // exhaust memory on many platforms.  Obj::k_MAX_CAPACITY - 1,
                // Obj::k_MAX_CAPACITY
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

        if (verbose) cout << "\nTest 'k_MAX_CAPACITY' properties." << endl;
        {
            bsls::Types::Uint64 maxCapacity = Obj::k_MAX_CAPACITY;
            ASSERTV(UINT_MAX == maxCapacity * 4 - 1);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator oa, da;

            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_OPT_PASS(Obj obj(1, &oa));
            ASSERT_OPT_FAIL_RAW(Obj obj(Obj::k_MAX_CAPACITY + 1, &oa));

            // The following test is disabled because it causes a divide by 0
            // failure prior to reaching the assertion.  ASSERT_OPT_FAIL(Obj
            // obj(0, &oa));
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

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            Obj mX(1, &ta);
            ASSERT(mX.isEnabled());
            ASSERT(0 == mX.length());
            ASSERT(1 == mX.capacity());

            unsigned int generation, index;
            ASSERT(0 == mX.reservePushIndex(&generation, &index));
            mX.commitPushIndex(generation, index);
            ASSERT(1 == mX.length());
            ASSERT(0 == generation);
            ASSERT(0 == index);
            ASSERT(0 != mX.reservePushIndex(&generation, &index));
            ASSERT(1 == mX.length());

            generation = index = 1;
            ASSERT(0 == mX.reservePopIndex(&generation, &index));
            mX.commitPopIndex(generation, index);
            ASSERT(0 == mX.length());
            ASSERT(0 == generation);
            ASSERT(0 == index);
            ASSERT(0 != mX.reservePopIndex(&generation, &index));
            ASSERT(0 == mX.length());

            mX.disable();
            ASSERT(!mX.isEnabled());
            ASSERT(0 != mX.reservePushIndex(&generation, &index));
            ASSERT(0 == mX.length());

            generation = index = 1;
            mX.enable();
            ASSERT(mX.isEnabled());
            ASSERT(0 == mX.reservePushIndex(&generation, &index));
            mX.commitPushIndex(generation, index);
            ASSERT(1 == mX.length());
            ASSERT(1 == generation);
            ASSERT(0 == index);
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case -1: {
       // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //   This negative case performs a performance test on a
        //   'bdlcc::FixedQueueIndexManager'.
        //
        // Concerns:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;

        using PERFORMANCE_TEST::readerThread;
        using PERFORMANCE_TEST::writerThread;

        const int TOTAL_TIME_S = argc > 2 ? atoi(argv[2]) : 30;

        struct {
                int d_line;
                int d_capacity;           // queue capacity
                int d_numReaders;         // number of reader threads
                int d_numWriters;         // number of writer threads
        } DATA[] = {

//           Line Cap     Rdrs  Wrtrs
//           =========================
            { L_,  1000,     1,    1  },
            { L_,  1000,     5,    5  },
            { L_,  1000,    15,    15 },
            { L_,  1000,     5,    1  },
            { L_,  1000,     1,    5  },

            { L_,     5,     1,    1  },
            { L_,     5,     5,    5  },
            { L_,     5,    15,    15 },
            { L_,     5,     5,    1  },
            { L_,     5,     1,    5  },

        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        const double ELAPSED_TIME_PER_TEST =  (double)TOTAL_TIME_S / NUM_DATA;

        // Note that this output format is designed to be easily imported into
        // excel.

        bsl::cout <<
            "Capacity,  Num Readers, Num Writers,   Reads/Sec,   Writes/Sec\n"
            "--------,-------------,------------,------------,-------------\n";

        for (int i = 0; i < NUM_DATA; ++i) {
            int CAPACITY       = DATA[i].d_capacity;
            int NUM_READERS    = DATA[i].d_numReaders;
            int NUM_WRITERS    = DATA[i].d_numWriters;
            int NUM_THREADS    = NUM_READERS + NUM_WRITERS;

            TestThreadStateBarrier state(NUM_THREADS);
            Obj x(CAPACITY);

            bsl::vector<bslmt::ThreadUtil::Handle> handles;
            handles.resize(NUM_THREADS);
            int thread = 0;

            bsls::AtomicInt64 writeCount, readCount;
            for (int i = 0; i < NUM_WRITERS; ++i) {
                int rc = bslmt::ThreadUtil::create(
                  &handles[thread],
                  bdlf::BindUtil::bind(&writerThread,
                                       &x,
                                       &writeCount,
                                       &state));
                BSLS_ASSERT_OPT(0 == rc); // test invariant
                ++thread;
            }
            for (int i = 0; i < NUM_READERS; ++i) {
                int rc = bslmt::ThreadUtil::create(
                  &handles[thread],
                  bdlf::BindUtil::bind(&readerThread, &x, &readCount, &state));
                BSLS_ASSERT_OPT(0 == rc); // test invariant
                ++thread;
            }

            state.continueTest();
            bsls::Stopwatch s;
            s.start();

            bslmt::ThreadUtil::sleep(
                                    bsls::TimeInterval(ELAPSED_TIME_PER_TEST));

            s.stop();
            state.exitTest();

            for (int i = 0; i < NUM_THREADS; ++i) {
                bslmt::ThreadUtil::join(handles[i]);
            }

            double elapsed = s.elapsedTime();

            bsl::cout
                << bsl::setw(8) << CAPACITY << ", "
                << bsl::setw(10) << NUM_READERS << ", "
                << bsl::setw(11) << NUM_WRITERS << ", "
                << bsl::setw(12) << (double)(readCount)/elapsed << ", "
                << bsl::setw(12) << (double)(writeCount)/elapsed << bsl::endl;
        }
      } break;
      case -2: {

        if (verbose) cout << endl
                          << "PRINT INTERESTING STATES" << endl
                          << "========================" << endl;

        // The following negative test case is used to help visualize various
        // states of an index manager.

        struct {
            int          d_line;
            unsigned int d_capacity;
            unsigned int d_pushIndex;
            unsigned int d_popIndex;
        } VALUES[] = {
            { L_,  3,  0,  0 },
            { L_,  3,  1,  0 },
            { L_,  3,  2,  0 },
            { L_,  3,  3,  0 },
            { L_,  3,  3,  1 },
            { L_,  3,  4,  2 },
            { L_,  3,  4,  3 },
            { L_,  3,  4,  4 },
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const unsigned int CAPACITY   = VALUES[i].d_capacity;
            const unsigned int PUSH_INDEX = VALUES[i].d_pushIndex;
            const unsigned int POP_INDEX  = VALUES[i].d_popIndex;
            const unsigned int LINE       = VALUES[i].d_line;

            if (veryVerbose) { P(LINE); }

            Obj x(CAPACITY); const Obj& X = x;

            gg(&x, PUSH_INDEX, POP_INDEX);

            X.print(bsl::cout);
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
