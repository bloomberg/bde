// bslmt_fastpostsemaphoreimpl.t.cpp                                  -*-C++-*-

#include <bslmt_fastpostsemaphoreimpl.h>

#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_atomicoperations.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
// A 'bslmt::FastPostSemaphoreImpl' is the templated-for-testing implementation
// of a semaphore optimized for fast 'post'.  The methods of
// 'bslmt::FastPostSemaphoreImpl' are tested by directly exercising the
// functionality.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] FastPostSemaphoreImpl(clockType = e_REALTIME);
// [ 2] FastPostSemaphoreImpl(int count, clockType = e_REALTIME);
//
// MANIPULATORS
// [ 4] void enable();
// [ 4] void disable();
// [ 6] void post();
// [ 6] void post(int value);
// [ 8] int take(int maximumToTake);
// [ 8] int takeAll();
// [ 6] int timedWait(const bsls::TimeInterval& timeout);
// [ 3] int tryWait();
// [ 6] int wait();
//
// ACCESSORS
// [ 4] int getDisabledState() const;
// [ 7] int getValue() const;
// [ 4] bool isDisabled() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] CONCERN: MANIPULATORS SIGNAL AS EXPECTED
// [ 9] CONCERN: NO RACES RESULTING IN METHOD NON-COMPLETION

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                         GLOBAL OBJECTS FOR TESTING
// ----------------------------------------------------------------------------

// The "Exhaustive" 'struct' form a testing framework for verifying a set of
// functions will complete, regardless of the ordering of their operations.
// Specifically, 'ExhaustiveTest:::test' creates threads that use an
// 'ExhaustiveObj', which is defined to use 'ExhaustiveAtomicOps',
// 'ExhaustiveMutex', and 'ExhaustiveCondition'.  These implementations cause
// all but one executing thread to block and for the ordering of progress of
// each thread to be determined by the test aparatus.

#define EXHAUSTIVE_MAX_THREAD   5  // maximum number of threads
#define EXHAUSTIVE_MAX_DEPTH  100  // maximum depth of the schedule

                        // =========================
                        // struct ExhaustiveWaitable
                        // =========================

struct ExhaustiveWaitable {
    // This base class represents an object that can cause a thread to block,
    // and the number of threads that should be allowed to proceed.

    // PUBLIC DATA
    int d_signalCount;  // count of threads to be allowed to proceed
};

                          // =====================
                          // struct ExhaustiveData
                          // =====================

struct ExhaustiveData {
    // PUBLIC DATA
    bslmt::ThreadUtil::Handle  d_handle;  // used to join the thread
    bsls::Types::Uint64        d_id;      // thread id
    ExhaustiveWaitable        *d_wait;    // object waiting upon
    bool                       d_done;    // method under test has completed
};

                        // ==========================
                        // struct ExhaustiveAtomicOps
                        // ==========================

struct ExhaustiveAtomicOps {
    // Simulates 'bsls::AtomicOperations'.

    // PUBLIC TYPES
    struct AtomicTypes {
        typedef bsls::Types::Int64 Int64;  // simulated 'AtomicTypes::Int64'
    };

    // CLASS METHODS
    static void addInt64AcqRel(AtomicTypes::Int64 *pValue,
                               bsls::Types::Int64  value);
        // Simulate 'bsls::AtomicOperations::addInt64AcqRel' for the specified
        // 'pValue' and 'value'.  Invoke 'ExhaustiveTest::next()' to create a
        // scheduling point for the test aparatus.

    static bsls::Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *pValue,
                                               bsls::Types::Int64  value);
        // Simulate 'bsls::AtomicOperations::addInt64NvAcqRel' for the
        // specified 'pValue' and 'value'.  Invoke 'ExhaustiveTest::next()' to
        // create a scheduling point for the test aparatus.

    static bsls::Types::Int64 getInt64Acquire(AtomicTypes::Int64 *pValue);
        // Simulate 'bsls::AtomicOperations::getInt64Acquire' for the specified
        // 'pValue'.  Invoke 'ExhaustiveTest::next()' to create a scheduling
        // point for the test aparatus.

    static void initInt64(AtomicTypes::Int64 *pValue,
                          bsls::Types::Int64  value);
        // Simulate 'bsls::AtomicOperations::initInt64' for the specified
        // 'pValue' and 'value'.

    static bsls::Types::Int64 testAndSwapInt64AcqRel(
                                                 AtomicTypes::Int64 *pValue,
                                                 bsls::Types::Int64  oldValue,
                                                 bsls::Types::Int64  newValue);
        // Simulate 'bsls::AtomicOperations::testAndSwapInt64AcqRel' for the
        // specified 'pValue', 'oldValue', and 'newWalue'.  Invoke
        // 'ExhaustiveTest::next()' to create a scheduling point for the test
        // aparatus.
};

                          // ======================
                          // struct ExhaustiveMutex
                          // ======================

struct ExhaustiveMutex : public ExhaustiveWaitable {
    // Simulates 'bslmt::Mutex'.

    // CREATORS
    ExhaustiveMutex();
        // Create an 'ExhaustiveMutex' object.

    // MANIPULATORS
    void lock();
        // Simulate 'bslmt::Mutex::lock'.  Invoke 'ExhaustiveTest::next()' to
        // create a scheduling point for the test aparatus.

    void unlockRaw();
        // Simulate 'bslmt::Mutex::unlock'.

    void unlock();
        // Simulate 'bslmt::Mutex::unlock'.  Invoke 'ExhaustiveTest::next()' to
        // create a scheduling point for the test aparatus.

};

                        // ==========================
                        // struct ExhaustiveCondition
                        // ==========================

struct ExhaustiveCondition : public ExhaustiveWaitable {
    // Simulates 'bslmt::Condition'.

    // CREATORS
    ExhaustiveCondition(bsls::SystemClockType::Enum);
        // Create an 'ExhaustiveCondition' object.

    // MANIPULATORS
    void broadcast();
        // Simulate 'bslmt::Condition::broadcast'.  Invoke
        // 'ExhaustiveTest::next()' to create a scheduling point for the test
        // aparatus.

    void signal();
        // Simulate 'bslmt::Condition::signal'.  Invoke
        // 'ExhaustiveTest::next()' to create a scheduling point for the test
        // aparatus.

    int wait(ExhaustiveMutex *mutex);
        // Simulate 'bslmt::Condition::wait'.  Invoke 'ExhaustiveTest::next()'
        // to create a scheduling point for the test aparatus.
};

                          // =====================
                          // typedef ExhaustiveObj
                          // =====================

typedef bslmt::FastPostSemaphoreImpl<ExhaustiveAtomicOps,
                                     ExhaustiveMutex,
                                     ExhaustiveCondition> ExhaustiveObj;

                           // ====================
                           // class ExhaustiveTest
                           // ====================

class ExhaustiveTest {
    // Implements a mechanism for exhaustively ordering the progress of threads
    // using an 'ExhaustiveObj'.

    // PRIVATE CLASS METHODS
    static void testCreate(bslmt::ThreadUtil::ThreadFunction f, void *a);
        // Create a thread and invoke the specified function 'f' with the
        // specified argument 'a'.

    static void testHelper();
        // Execute the test.

  public:
    // PUBLIC CLASS DATA
    static bsls::Types::Uint64  s_doneId;      // id of main thread, to be
                                               // scheduled when a path of
                                               // testing is completed

    static bsls::AtomicUint64   s_scheduledId; // all threads block on this
                                               // value, no need for additional
                                               // synchronization

    static ExhaustiveData       s_data[EXHAUSTIVE_MAX_THREAD];
                                               // data for each thread

    static bsl::size_t          s_dataSize;    // number of threads

    static ExhaustiveObj       *s_obj_p;       // pointer to object used to
                                               // test the methods

    static bsl::size_t          s_schedule[EXHAUSTIVE_MAX_DEPTH];
                                               // indexes into 's_data' that
                                               // represents the currently
                                               // executing ordering of threads

    static bsl::size_t          s_scheduleSize;
                                               // size of 's_schedule'

    static bsl::size_t          s_scheduleIndex;
                                               // index into 's_schedule',
                                               // 's_schedule[s_scheduleIndex]'
                                               // indicats which thread is to
                                               // proceed

    static bsl::size_t          s_scheduleNextIndex;
                                               // index into 's_schedule'
                                               // indicating where to change
                                               // the schedule for the next
                                               // iteration

    static bsl::size_t          s_scheduleNextId;
                                               // value to place into
                                               // 's_schedule' at
                                               // 's_scheduleNextIndex'

    static bool                 s_doWait;      // whether 'wait' should block
                                               // threads; 'false' during
                                               // initialization of the
                                               // 'ExhaustiveObj'

    static bool                 s_verbose;     // verbosity of test

    // CLASS METHODS
    static void finish();
        // Mark the invoking thread as having completed.

    static void next();
        // Schedule the next thread to make progress using 's_schedule', and
        // wait to be scheduled.

    static void next(bsls::Types::Uint64 id);
        // Schedule the thread having the specified 'id' to make progress, and
        // wait to be scheduled.

    static void reset();
        // For the invoking thread, perform the inter-iteration reset (when the
        // tested method has completed).

    static void start();
        // For the invoking thread, prepare to begin an iteration.

    static void test(bslmt::ThreadUtil::ThreadFunction f1, void *a1);
        // Setup and execute the test for the specified function 'f1' taking
        // the specified argument 'a1'.

    static void test(bslmt::ThreadUtil::ThreadFunction  f1,
                     void                              *a1,
                     bslmt::ThreadUtil::ThreadFunction  f2,
                     void                              *a2);
        // Setup and execute the test for the specified functions 'f1' and 'f2'
        // taking the specified arguments 'a1' and 'a2', respectively.

    static void test(bslmt::ThreadUtil::ThreadFunction  f1,
                     void                              *a1,
                     bslmt::ThreadUtil::ThreadFunction  f2,
                     void                              *a2,
                     bslmt::ThreadUtil::ThreadFunction  f3,
                     void                              *a3);
        // Setup and execute the test for the specified functions 'f1', 'f2',
        // and 'f3' taking the specified arguments 'a1', 'a2', and 'a3',
        // respectively.

    static void unwaitAll(ExhaustiveWaitable *waitable);
        // Release all threads waiting for the specified 'waitable'.

    static void wait();
        // Block the invoking thread until the thread is scheduled.

    static void wait(ExhaustiveWaitable *waitable);
        // Mark the invoking thread as blocked on the specified 'waitable'.
};

                        // --------------------------
                        // struct ExhaustiveAtomicOps
                        // --------------------------

// CLASS METHODS
void ExhaustiveAtomicOps::addInt64AcqRel(AtomicTypes::Int64 *pValue,
                                         bsls::Types::Int64  value)
{
    ExhaustiveTest::next();
    *pValue += value;
}

bsls::Types::Int64 ExhaustiveAtomicOps::addInt64NvAcqRel(
                                                    AtomicTypes::Int64 *pValue,
                                                    bsls::Types::Int64  value)
{
    ExhaustiveTest::next();
    *pValue += value;

    return *pValue;
}

bsls::Types::Int64 ExhaustiveAtomicOps::getInt64Acquire(
                                                    AtomicTypes::Int64 *pValue)
{
    ExhaustiveTest::next();

    return *pValue;
}

void ExhaustiveAtomicOps::initInt64(AtomicTypes::Int64 *pValue,
                                    bsls::Types::Int64  value)
{
    *pValue = value;
}

bsls::Types::Int64 ExhaustiveAtomicOps::testAndSwapInt64AcqRel(
                                                  AtomicTypes::Int64 *pValue,
                                                  bsls::Types::Int64  oldValue,
                                                  bsls::Types::Int64  newValue)
{
    if (*pValue == oldValue) {
        *pValue = newValue;
        return oldValue;                                              // RETURN
    }
    return *pValue;
}

                          // ----------------------
                          // struct ExhaustiveMutex
                          // ----------------------

// CREATORS
ExhaustiveMutex::ExhaustiveMutex()
{
    d_signalCount = 1;
}

// MANIPULATORS
void ExhaustiveMutex::lock()
{
    ExhaustiveTest::wait(this);
}

void ExhaustiveMutex::unlockRaw()
{
    d_signalCount = 1;
}

void ExhaustiveMutex::unlock()
{
    unlockRaw();
    ExhaustiveTest::next();
}

                        // --------------------------
                        // struct ExhaustiveCondition
                        // --------------------------

// CREATORS
ExhaustiveCondition::ExhaustiveCondition(bsls::SystemClockType::Enum)
{
    d_signalCount = 0;
}

// MANIPULATORS
void ExhaustiveCondition::broadcast()
{
    ExhaustiveTest::unwaitAll(this);
    d_signalCount = 0;
    ExhaustiveTest::next();
}

void ExhaustiveCondition::signal()
{
    ++d_signalCount;
    ExhaustiveTest::next();
}

int ExhaustiveCondition::wait(ExhaustiveMutex *mutex)
{
    mutex->unlockRaw();
    ExhaustiveTest::wait(this);
    mutex->lock();

    return 0;
}

                           // --------------------
                           // class ExhaustiveTest
                           // --------------------

// PUBLIC CLASS DATA
bsls::Types::Uint64  ExhaustiveTest::s_doneId;
bsls::AtomicUint64   ExhaustiveTest::s_scheduledId;
ExhaustiveData       ExhaustiveTest::s_data[EXHAUSTIVE_MAX_THREAD];
bsl::size_t          ExhaustiveTest::s_dataSize;
ExhaustiveObj       *ExhaustiveTest::s_obj_p;
bsl::size_t          ExhaustiveTest::s_schedule[EXHAUSTIVE_MAX_DEPTH];
bsl::size_t          ExhaustiveTest::s_scheduleSize;
bsl::size_t          ExhaustiveTest::s_scheduleIndex;
bsl::size_t          ExhaustiveTest::s_scheduleNextIndex;
bsl::size_t          ExhaustiveTest::s_scheduleNextId;
bool                 ExhaustiveTest::s_doWait = true;
bool                 ExhaustiveTest::s_verbose = false;

// PRIVATE CLASS METHODS
void ExhaustiveTest::testCreate(bslmt::ThreadUtil::ThreadFunction f, void *a)
{
    bslmt::ThreadUtil::Handle h;
    ASSERT(0 == bslmt::ThreadUtil::create(&h, f, a));

    s_data[s_dataSize].d_handle = h;
    s_data[s_dataSize].d_id     = bslmt::ThreadUtil::idAsUint64(
                                             bslmt::ThreadUtil::handleToId(h));

    ++s_dataSize;
    ASSERT(EXHAUSTIVE_MAX_THREAD >= s_dataSize);
}

void ExhaustiveTest::testHelper()
{
    s_scheduleSize      = 0;
    s_scheduleIndex     = 0;
    s_scheduleNextIndex = 0;
    s_scheduleNextId    = 0;

    do {
        s_doWait = false;

        ExhaustiveObj mX;

        s_obj_p = &mX;

        s_schedule[s_scheduleNextIndex] = s_scheduleNextId;

        s_scheduleSize      = s_scheduleNextIndex + 1;
        s_scheduleIndex     = 0;
        s_scheduleNextIndex = 0;
        s_scheduleNextId    = 0;

        for (bsl::size_t i = 0; i < s_dataSize; ++i) {
            s_data[i].d_done = false;
            s_data[i].d_wait = 0;
        }

        s_doWait = true;

        next();
    } while (s_scheduleNextIndex || s_scheduleNextId);

    s_obj_p = 0;

    for (bsl::size_t i = 0; i < s_dataSize; ++i) {
        next(s_data[i].d_id);
    }

    for (bsl::size_t i = 0; i < s_dataSize; ++i) {
        bslmt::ThreadUtil::join(s_data[i].d_handle);
    }
}

// CLASS METHODS
void ExhaustiveTest::finish()
{
    s_scheduledId.storeRelease(s_doneId);
}

void ExhaustiveTest::next()
{
    bsls::Types::Uint64 id = s_doneId;

    if (s_scheduleIndex == s_scheduleSize) {
        BSLS_ASSERT_OPT(   s_scheduleSize < EXHAUSTIVE_MAX_DEPTH
                        && "exceeded maximum depth");
        bool done = true;
        for (bsl::size_t i = 0; i < s_dataSize; ++i) {
            done &= s_data[i].d_done;
        }
        if (!done) {
            bsl::size_t i    = 0;
            bool        acon = true;
            while (acon) {
                if (   !s_data[i].d_done
                    && (   0 == s_data[i].d_wait
                        || s_data[i].d_wait->d_signalCount)) {
                    s_schedule[s_scheduleIndex] = i;
                    ++s_scheduleSize;
                    acon = false;
                }
                else {
                    ++i;
                    if (i == s_dataSize) {
                        acon = false;
                    }
                }
            }
            BSLS_ASSERT_OPT(i < s_dataSize && "a thread did not complete");
        }
    }

    if (s_scheduleIndex < s_scheduleSize) {
        bsl::size_t i = s_schedule[s_scheduleIndex];

        BSLS_ASSERT_OPT(   !s_data[i].d_done
                        && (   0 == s_data[i].d_wait
                            || s_data[i].d_wait->d_signalCount)
                        && "invalid schedule");

        {
            // check for another
            ++i;
            bool acon = i < s_dataSize;
            while (acon) {
                if (   !s_data[i].d_done
                    && (   0 == s_data[i].d_wait
                        || s_data[i].d_wait->d_signalCount)) {
                    s_scheduleNextIndex = s_scheduleIndex;
                    s_scheduleNextId    = i;
                    acon = false;
                }
                else {
                    ++i;
                    if (i == s_dataSize) {
                        acon = false;
                    }
                }
            }
        }

        i = s_schedule[s_scheduleIndex];
        if (s_data[i].d_wait) {
            --s_data[i].d_wait->d_signalCount;
            s_data[i].d_wait = 0;
        }
        id = s_data[i].d_id;

        ++s_scheduleIndex;
    }

    next(id);
}

void ExhaustiveTest::next(bsls::Types::Uint64 id)
{
    // assign the next thread to execute

    s_scheduledId.storeRelease(id);

    // wait until told to execute

    wait();
}

void ExhaustiveTest::reset()
{
    bsls::Types::Uint64 id = bslmt::ThreadUtil::selfIdAsUint64();
    for (bsl::size_t i = 0; i < s_dataSize; ++i) {
        if (id == s_data[i].d_id) {
            s_data[i].d_done = true;
        }
    }
    next();
}

void ExhaustiveTest::start()
{
    wait();
}

void ExhaustiveTest::test(bslmt::ThreadUtil::ThreadFunction f1, void *a1)
{
    s_doneId      = bslmt::ThreadUtil::selfIdAsUint64();
    s_scheduledId = s_doneId;

    s_dataSize = 0;

    testCreate(f1, a1);

    testHelper();
}

void ExhaustiveTest::test(bslmt::ThreadUtil::ThreadFunction  f1,
                          void                              *a1,
                          bslmt::ThreadUtil::ThreadFunction  f2,
                          void                              *a2)
{
    s_doneId      = bslmt::ThreadUtil::selfIdAsUint64();
    s_scheduledId = s_doneId;

    s_dataSize = 0;

    testCreate(f1, a1);
    testCreate(f2, a2);

    testHelper();
}

void ExhaustiveTest::test(bslmt::ThreadUtil::ThreadFunction  f1,
                          void                              *a1,
                          bslmt::ThreadUtil::ThreadFunction  f2,
                          void                              *a2,
                          bslmt::ThreadUtil::ThreadFunction  f3,
                          void                              *a3)
{
    s_doneId      = bslmt::ThreadUtil::selfIdAsUint64();
    s_scheduledId = s_doneId;

    s_dataSize = 0;

    testCreate(f1, a1);
    testCreate(f2, a2);
    testCreate(f3, a3);

    testHelper();
}

void ExhaustiveTest::unwaitAll(ExhaustiveWaitable *waitable)
{
    for (bsl::size_t i = 0; i < s_dataSize; ++i) {
        if (waitable == s_data[i].d_wait) {
            s_data[i].d_wait = 0;
        }
    }
}

void ExhaustiveTest::wait()
{
    bsls::Types::Uint64 id = bslmt::ThreadUtil::selfIdAsUint64();
    if (s_doWait || id != s_doneId) {
        while (id != s_scheduledId.loadAcquire()) {
            bslmt::ThreadUtil::yield();
        }
    }
}

void ExhaustiveTest::wait(ExhaustiveWaitable *waitable)
{
    bsls::Types::Uint64 id = bslmt::ThreadUtil::selfIdAsUint64();
    bsl::size_t i = 0;
    while (id != s_data[i].d_id) {
        ++i;
    }

    s_data[i].d_wait = waitable;

    next();
}

            // methods used with the "Exhaustive" test framework

extern "C" void *Exhaustive_disable(void *arg)
{
    (void)arg;

    ExhaustiveTest::start();
    do {
        ExhaustiveTest::s_obj_p->disable();
        ExhaustiveTest::reset();
    } while (ExhaustiveTest::s_obj_p);
    ExhaustiveTest::finish();

    return 0;
}

extern "C" void *Exhaustive_post(void *arg)
{
    (void)arg;

    ExhaustiveTest::start();
    do {
        ExhaustiveTest::s_obj_p->post();
        ExhaustiveTest::reset();
    } while (ExhaustiveTest::s_obj_p);
    ExhaustiveTest::finish();

    return 0;
}

extern "C" void *Exhaustive_post2(void *arg)
{
    (void)arg;

    ExhaustiveTest::start();
    do {
        ExhaustiveTest::s_obj_p->post(2);
        ExhaustiveTest::reset();
    } while (ExhaustiveTest::s_obj_p);
    ExhaustiveTest::finish();

    return 0;
}

extern "C" void *Exhaustive_wait(void *arg)
{
    (void)arg;

    ExhaustiveTest::start();
    do {
        ExhaustiveTest::s_obj_p->wait();
        ExhaustiveTest::reset();
    } while (ExhaustiveTest::s_obj_p);
    ExhaustiveTest::finish();

    return 0;
}

                // other testing classes and methods

struct TestAtomicOperations {
    static bsl::deque<bsls::Types::Int64> s_override;

    struct AtomicTypes {
        typedef bsls::Types::Int64 Int64;
    };

    static void initInt64(AtomicTypes::Int64 *pValue, bsls::Types::Int64 value)
        // Set the value pointed to by the specified 'pValue' to the specified
        // 'value'.
    {
        *pValue = value;
    }

    static void addInt64AcqRel(AtomicTypes::Int64 *pValue,
                               bsls::Types::Int64  value)
        // Set the value pointed to by the specified 'pValue' to the specified
        // 'value' plus the original value of '*pValue'.
    {
        *pValue += value;
    }

    static bsls::Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *pValue,
                                               bsls::Types::Int64  value)
        // Set the value pointed to by the specified 'pValue' to the next
        // override value plus the specified 'value', and return this sum.
    {
        BSLS_ASSERT_OPT(   s_override.size()
                        && "insufficient number of override values");

        *pValue = s_override.front();
        s_override.pop_front();

        *pValue += value;
        return *pValue;
    }

    static void clearOverride()
        // Remove all override values.
    {
        s_override.clear();
    }

    static bsls::Types::Int64 getInt64Acquire(AtomicTypes::Int64 *pValue)
        // Return the value pointed to by the specified 'pValue'.
    {
        return *pValue;
    }

    static void pushOverride(int available, int disabled, int blocked)
        // Push an override value indicating the state of the specified
        // 'available' count, the specified 'disabled' generation, and the
        // specified number of 'blocked' threads.
    {
        typedef bslmt::FastPostSemaphoreImpl<bsls::AtomicOperations,
                                             bslmt::Mutex,
                                             bslmt::Condition> Obj;

        s_override.push_back(  Obj::k_AVAILABLE_INC    * available
                             + Obj::k_DISABLED_GEN_INC * disabled
                             + Obj::k_BLOCKED_INC      * blocked);
    }
};

bsl::deque<bsls::Types::Int64> TestAtomicOperations::s_override;

class TestCondition {
    static int s_signalCount;

  public:
    static int signalCount()
        // Return the number of signals sent.
    {
        return s_signalCount;
    }

    TestCondition(bsls::SystemClockType::Enum)
        // Create a 'TestCondition'.
    {
        s_signalCount = 0;
    }

    void signal()
        // Increment the number of signals sent.
    {
        ++s_signalCount;
    }

    int timedWait(bslmt::Mutex *, const bsls::TimeInterval&)
        // Return 0.
    {
        return 0;
    }

    int wait(bslmt::Mutex *)
        // Return 0.
    {
        return 0;
    }
};

int TestCondition::s_signalCount = 0;

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::FastPostSemaphoreImpl<bsls::AtomicOperations,
                                     bslmt::Mutex,
                                     bslmt::Condition> Obj;

typedef bslmt::FastPostSemaphoreImpl<TestAtomicOperations,
                                     bslmt::Mutex,
                                     TestCondition> TestObj;

const int k_DECISECOND = 100 * 1000;  // number of microseconds in 0.1 seconds

// ============================================================================
//                         GLOBAL METHODS FOR TESTING
// ----------------------------------------------------------------------------

static bsls::AtomicInt s_continue;

extern "C" void *watchdog(void *arg)
    // Return when '0 == s_continue', or if '0 == s_continue' does not occur
    // within 1 second, log the specified 'arg', which is a C-style string, and
    // 'abort'.
{
    const char *text = static_cast<const char *>(arg);

    const int MAX = 10;

    int count = 0;

    while (s_continue) {
        bslmt::ThreadUtil::microSleep(k_DECISECOND);
        ++count;

        ASSERTV(text, count < MAX);

        if (MAX == count && s_continue) abort();
    }

    return 0;
}

extern "C" void *timedWaitExpectDisabled(void *arg)
    // Invoke 'timedWait' with a one second timeout on the specified 'arg' and
    // verify the result value is 'e_DISABLED'.  The behavior is undefined
    // unless 'arg' is a point to a valid instance of 'Obj'.
{
    Obj& mX = *static_cast<Obj *>(arg);

    ASSERT(Obj::e_DISABLED == mX.timedWait(bsls::SystemTime::nowRealtimeClock()
                                                   + bsls::TimeInterval(1.0)));

    return 0;
}

extern "C" void *timedWaitExpectSuccess(void *arg)
    // Invoke 'timedWait' with a one second timeout on the specified 'arg' and
    // verify the result value is 'e_SUCCESS'.  The behavior is undefined
    // unless 'arg' is a point to a valid instance of 'Obj'.
{
    Obj& mX = *static_cast<Obj *>(arg);

    ASSERT(Obj::e_SUCCESS == mX.timedWait(bsls::SystemTime::nowRealtimeClock()
                                                   + bsls::TimeInterval(1.0)));

    return 0;
}

extern "C" void *waitExpectDisabled(void *arg)
    // Invoke 'wait' on the specified 'arg' and verify the result value is
    // 'e_DISABLED'.  The behavior is undefined unless 'arg' is a point to a
    // valid instance of 'Obj'.
{
    Obj& mX = *static_cast<Obj *>(arg);

    ASSERT(Obj::e_DISABLED == mX.wait());

    return 0;
}

extern "C" void *waitExpectSuccess(void *arg)
    // Invoke 'wait' on the specified 'arg' and verify the result value is
    // 'e_SUCCESS'.  The behavior is undefined unless 'arg' is a point to a
    // valid instance of 'Obj'.
{
    Obj& mX = *static_cast<Obj *>(arg);

    ASSERT(Obj::e_SUCCESS == mX.wait());

    return 0;
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void)veryVerbose;

    if (veryVeryVerbose) {
        ExhaustiveTest::s_verbose = true;
    }

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // CONCERN: NO RACES RESULTING IN METHOD NON-COMPLETION
        //   Ensure the manipulators do not cause races that prevent methods
        //   from completing.
        //
        // Concerns:
        //: 1 Where possible, ensure the concurrent execution of methods does
        //:   not result in a race that causes a method to not complete.
        //
        // Plan:
        //: 1 Using the "Exhaustive" testing framework, spot check the
        //:   concurrent execution of methods without requiring extensive
        //:   execution times.  (C-1)
        //
        // Testing:
        //   CONCERN: NO RACES RESULTING IN METHOD NON-COMPLETION
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "CONCERN: NO RACES RESULTING IN METHOD NON-COMPLETION"
                 << endl
                 << "===================================================="
                 << endl;
        }

        ExhaustiveTest::test(Exhaustive_post, 0,
                             Exhaustive_wait, 0);

        ExhaustiveTest::test(Exhaustive_wait,    0,
                             Exhaustive_disable, 0);

        ExhaustiveTest::test(Exhaustive_post2, 0,
                             Exhaustive_wait,  0,
                             Exhaustive_wait,  0);

        ExhaustiveTest::test(Exhaustive_post,    0,
                             Exhaustive_wait,    0,
                             Exhaustive_disable, 0);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'take' AND 'takeAll'
        //   Ensure the manipulators function as expected.
        //
        // Concerns:
        //: 1 The manipulators correctly reduce the semaphore's count and
        //:   return the correct value.
        //
        // Plan:
        //: 1 Directly verify the result of 'take' and 'takeAll' by using
        //:   'getValue' before and after the method invocations during a
        //:   sequence of operations on the semaphore, including using 'wait'
        //:   to block threads and drive the count of the semaphore to a
        //:   negative value.  (C-1)
        //
        // Testing:
        //   int take(int maximumToTake);
        //   int takeAll();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'take' AND 'takeAll'" << endl
                 << "============================" << endl;
        }

        {
            s_continue = 1;

            bslmt::ThreadUtil::Handle watchdogHandle;
            bslmt::ThreadUtil::create(
                                   &watchdogHandle,
                                   watchdog,
                                   const_cast<char *>("'take' and 'takeAll'"));

            {
                // verify 'take'

                Obj mX(3);  const Obj& X = mX;

                ASSERT(3 == X.getValue());

                ASSERT(1 == mX.take(1));

                ASSERT(2 == X.getValue());

                ASSERT(2 == mX.take(5));

                ASSERT(0 == X.getValue());

                ASSERT(0 == mX.take(5));

                ASSERT(0 == X.getValue());

                bslmt::ThreadUtil::Handle handle1;
                bslmt::ThreadUtil::create(&handle1, waitExpectSuccess, &mX);

                bslmt::ThreadUtil::Handle handle2;
                bslmt::ThreadUtil::create(&handle2, waitExpectSuccess, &mX);

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                ASSERT(0 == X.getValue());

                ASSERT(0 == mX.take(3));

                ASSERT(0 == X.getValue());

                mX.post();

                ASSERT(0 == X.getValue());

                ASSERT(0 == mX.take(3));

                ASSERT(0 == X.getValue());

                mX.post();

                ASSERT(0 == X.getValue());

                ASSERT(0 == mX.take(3));

                ASSERT(0 == X.getValue());

                mX.post();

                ASSERT(1 == X.getValue());

                ASSERT(1 == mX.take(3));

                ASSERT(0 == X.getValue());

                bslmt::ThreadUtil::join(handle1);
                bslmt::ThreadUtil::join(handle2);
            }
            {
                // verify 'takeAll'

                Obj mX(3);  const Obj& X = mX;

                ASSERT(3 == X.getValue());

                ASSERT(3 == mX.takeAll());

                ASSERT(0 == X.getValue());

                bslmt::ThreadUtil::Handle handle1;
                bslmt::ThreadUtil::create(&handle1, waitExpectSuccess, &mX);

                bslmt::ThreadUtil::Handle handle2;
                bslmt::ThreadUtil::create(&handle2, waitExpectSuccess, &mX);

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                ASSERT(0 == X.getValue());

                ASSERT(0 == mX.takeAll());

                ASSERT(0 == X.getValue());

                mX.post();

                ASSERT(0 == X.getValue());

                ASSERT(0 == mX.takeAll());

                ASSERT(0 == X.getValue());

                mX.post();

                ASSERT(0 == X.getValue());

                ASSERT(0 == mX.takeAll());

                ASSERT(0 == X.getValue());

                mX.post();

                ASSERT(1 == X.getValue());

                ASSERT(1 == mX.takeAll());

                ASSERT(0 == X.getValue());

                bslmt::ThreadUtil::join(handle1);
                bslmt::ThreadUtil::join(handle2);
            }

            s_continue = 0;

            bslmt::ThreadUtil::join(watchdogHandle);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'getValue'
        //   Ensure the accessor functions as expected.
        //
        // Concerns:
        //: 1 The accessor correctly reflects the value of the semaphore.
        //
        // Plan:
        //: 1 Directly verify the result of 'getValue' throughout a sequence of
        //:   operations on the semaphore, including using 'wait' to block
        //:   threads and drive the count of the semaphore to a negative value.
        //:   (C-1)
        //
        // Testing:
        //   int getValue() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'getValue'" << endl
                 << "==================" << endl;
        }

        {
            // verify 'getValue'

            s_continue = 1;

            bslmt::ThreadUtil::Handle watchdogHandle;
            bslmt::ThreadUtil::create(&watchdogHandle,
                                      watchdog,
                                      const_cast<char *>("'getValue'"));

            Obj mX(3);  const Obj& X = mX;

            ASSERT(3 == X.getValue());

            ASSERT(Obj::e_SUCCESS == mX.tryWait());

            ASSERT(2 == X.getValue());

            ASSERT(Obj::e_SUCCESS == mX.tryWait());

            ASSERT(1 == X.getValue());

            ASSERT(Obj::e_SUCCESS == mX.tryWait());

            ASSERT(0 == X.getValue());

            bslmt::ThreadUtil::Handle handle1;
            bslmt::ThreadUtil::create(&handle1, waitExpectSuccess, &mX);

            // sleep to allow the thread to block
            bslmt::ThreadUtil::microSleep(k_DECISECOND);

            ASSERT(0 == X.getValue());

            bslmt::ThreadUtil::Handle handle2;
            bslmt::ThreadUtil::create(&handle2, waitExpectSuccess, &mX);

            // sleep to allow the thread to block
            bslmt::ThreadUtil::microSleep(k_DECISECOND);

            ASSERT(0 == X.getValue());

            mX.post();

            ASSERT(0 == X.getValue());

            mX.post();

            ASSERT(0 == X.getValue());

            mX.post();

            ASSERT(1 == X.getValue());

            bslmt::ThreadUtil::join(handle1);
            bslmt::ThreadUtil::join(handle2);

            s_continue = 0;

            bslmt::ThreadUtil::join(watchdogHandle);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'post', 'timedWait', AND 'wait'
        //   Ensure the manipulators function as expected.
        //
        // Concerns:
        //: 1 The manipulators modify the semaphore's count correctly.
        //:
        //: 2 The 'post' manipulators will unblock waiting threads.
        //:
        //: 3 The 'timedWait' manipulator will timeout and, if the timeout
        //:   occurs, will not affect the semaphore's count.
        //:
        //: 4 Disablement of the queue releases blocked threads and results
        //:   in the correct semaphore count.
        //
        // Plan:
        //: 1 Create semaphores with varying initial count, invoke a
        //:   manipulator, and directly verify the count using 'tryWait'.  Use
        //:   a watchdog to detect blocked threads.  (C-1)
        //:
        //: 2 Create semaphores with zero initial count, use 'timedWait' (with
        //:   a very long timeout) and 'wait' to block created threads on the
        //:   semaphore,  execute a 'post' method, and then join the created
        //:   threads to verify they were released.  Use a watchdog to detect
        //:   unreleased threads.  (C-2)
        //:
        //: 3 Directly verify the timeout functionality of 'timedWait', the
        //:   return value of the method, and use 'tryWait' to verify the
        //:   semaphore count.
        //:
        //: 4 Create semaphores with zero initial count, use 'timedWait' (with
        //:   a very long timeout) and 'wait' to block created threads on the
        //:   semaphore, execute the 'disable' method, verify the return value
        //:   and then join the created threads to verify they were released.
        //:   Use a watchdog to detect unreleased threads.  (C-2)
        //
        // Testing:
        //   void post();
        //   void post(int value);
        //   int timedWait(const bsls::TimeInterval& timeout);
        //   int wait();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'post', 'timedWait', AND 'wait'" << endl
                 << "=======================================" << endl;
        }

        if (verbose) cout << "\nDirect test of count adjustments." << endl;
        {
            s_continue = 1;

            bslmt::ThreadUtil::Handle watchdogHandle;
            bslmt::ThreadUtil::create(&watchdogHandle,
                                      watchdog,
                                      const_cast<char *>("count"));

            for (int initialCount = -5; initialCount < 10; ++initialCount) {
                {
                    Obj mX(initialCount);

                    int count = initialCount;
                    while (count < 5) {
                        mX.post();
                        ++count;
                    }

                    while (count) {
                        ASSERT(Obj::e_SUCCESS == mX.wait());
                        --count;
                    }
                    ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
                }
                {
                    Obj mX(initialCount);

                    int count = initialCount;
                    while (count < 5) {
                        mX.post(1);
                        ++count;
                    }

                    while (count) {
                        ASSERT(Obj::e_SUCCESS ==
                                        mX.timedWait(bsls::TimeInterval(1.0)));
                        --count;
                    }
                    ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
                }
                {
                    Obj mX(initialCount);

                    int count = initialCount;
                    while (count < 5) {
                        mX.post(3);
                        count += 3;
                    }

                    while (count) {
                        ASSERT(Obj::e_SUCCESS == mX.wait());
                        --count;
                    }
                    ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
                }
            }

            s_continue = 0;

            bslmt::ThreadUtil::join(watchdogHandle);
        }

        if (verbose) {
            cout << "\nVerify 'post' releases blocked threads." << endl;
        }
        {
            s_continue = 1;

            bslmt::ThreadUtil::Handle watchdogHandle;
            bslmt::ThreadUtil::create(
                                &watchdogHandle,
                                watchdog,
                                const_cast<char *>("'post' releases blocked"));

            {
                const int k_NUM_THREAD = 6;

                Obj mX;

                bslmt::ThreadUtil::Handle handle[k_NUM_THREAD];
                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::create(&handle[i],
                                              waitExpectSuccess,
                                              &mX);
                }

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    mX.post();
                }

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::join(handle[i]);
                }
            }
            {
                const int k_NUM_THREAD = 6;

                Obj mX;

                bslmt::ThreadUtil::Handle handle[k_NUM_THREAD];
                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::create(&handle[i],
                                              timedWaitExpectSuccess,
                                              &mX);
                }

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    mX.post();
                }

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::join(handle[i]);
                }
            }
            {
                const int k_NUM_THREAD = 6;

                Obj mX;

                bslmt::ThreadUtil::Handle handle[k_NUM_THREAD];
                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::create(&handle[i],
                                              waitExpectSuccess,
                                              &mX);
                }

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                for (int i = 0; i < k_NUM_THREAD; i += 2) {
                    mX.post(2);
                }

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::join(handle[i]);
                }
            }
            {
                const int k_NUM_THREAD = 6;

                Obj mX;

                bslmt::ThreadUtil::Handle handle[k_NUM_THREAD];
                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::create(&handle[i],
                                              timedWaitExpectSuccess,
                                              &mX);
                }

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                for (int i = 0; i < k_NUM_THREAD; i += 3) {
                    mX.post(3);
                }

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::join(handle[i]);
                }
            }

            s_continue = 0;

            bslmt::ThreadUtil::join(watchdogHandle);
        }

        if (verbose) cout << "\nDirect test of 'timedWait' concerns." << endl;
        {
            Obj mX;

            bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
            bsls::TimeInterval until = now + bsls::TimeInterval(0.2);

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(until));

            ASSERT((bsls::SystemTime::nowRealtimeClock() - now)
                                                   >= bsls::TimeInterval(0.1));

            mX.post();

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
        }

        if (verbose) {
            cout << "\nVerify 'disable' releases blocked threads." << endl;
        }
        {
            s_continue = 1;

            bslmt::ThreadUtil::Handle watchdogHandle;
            bslmt::ThreadUtil::create(
                             &watchdogHandle,
                             watchdog,
                             const_cast<char *>("'disable' releases blocked"));

            {
                const int k_NUM_THREAD = 6;

                Obj mX;

                bslmt::ThreadUtil::Handle handle[k_NUM_THREAD];
                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::create(&handle[i],
                                              waitExpectDisabled,
                                              &mX);
                }

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                mX.disable();

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::join(handle[i]);
                }
            }
            {
                const int k_NUM_THREAD = 6;

                Obj mX;

                bslmt::ThreadUtil::Handle handle[k_NUM_THREAD];
                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::create(&handle[i],
                                              timedWaitExpectDisabled,
                                              &mX);
                }

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                mX.disable();

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::join(handle[i]);
                }
            }

            s_continue = 0;

            bslmt::ThreadUtil::join(watchdogHandle);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONCERN: MANIPULATORS SIGNAL AS EXPECTED
        //   Ensure the manipulators signal as expected.
        //
        // Concerns:
        //: 1 The 'post' manipulator signals only when the semaphore count goes
        //:   from zero to one, the semaphore it not disabled, and there is at
        //:   least one blocked thread.
        //:
        //: 2 The 'post(value)' manipulator signals only when the semaphore
        //:   count goes from 0 to 'value', the semaphore is not disabled, and
        //:   there is at least one blocked thread.
        //:
        //: 3 The 'wait' manipulator signals when the resource is available,
        //:   the semaphore is not disabled, and there is at least one blocked
        //:   thread.
        //:
        //: 4 The 'timedWait' manipulator signals when the resource is
        //:   available, the semaphore is not disabled, and there is at least
        //:   one blocked thread.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of states and
        //:   a flag indicating if the 'post' method is expected to signal.
        //:   Use 'TestObj', which specializes 'FastPostSemaphoreImpl' with
        //:   'TestAtomicOperations' and 'TestCondition' (as opposed to
        //:   'bsls::AtomicOperations' and 'bslmt::Condition'), to directly set
        //:   the state of the semaphore and to verify the signalling behavior
        //:   of 'post'.  (C-1)
        //:
        //: 2 Using the table-driven technique, specify a set of states, a
        //:   flag indicating if the 'post(value)' method is expected to
        //:   signal, and a value to be used in 'post(value)'.  Use 'TestObj',
        //:   which specializes 'FastPostSemaphoreImpl' with
        //:   'TestAtomicOperations' and 'TestCondition' (as opposed to
        //:   'bsls::AtomicOperations' and 'bslmt::Condition'), to directly set
        //:   the state of the semaphore and to verify the signalling behavior
        //:   of 'post(value)'.  (C-2)
        //:
        //: 3 Specify a set of states to cause a thread invoking 'wait' to
        //:   reach the 'addInt64NvAcqRel' invocation immediately proir to the
        //:   check for sending a signal.  Using the table-driven technique,
        //:   specify a set of states for *after* this 'addInt64NvAcqRel' and
        //:   a flag indicating if the 'wait' method is expected to signal.
        //:   Use 'TestObj', which specializes 'FastPostSemaphoreImpl' with
        //:   'TestAtomicOperations' and 'TestCondition' (as opposed to
        //:   'bsls::AtomicOperations' and 'bslmt::Condition'), to directly set
        //:   the state of the semaphore and to verify the signalling behavior
        //:   of 'wait'.  (C-3)
        //:
        //: 4 Specify a set of states to cause a thread invoking 'timedWait' to
        //:   reach the 'addInt64NvAcqRel' invocation immediately proir to the
        //:   check for sending a signal.  Using the table-driven technique,
        //:   specify a set of states for *after* this 'addInt64NvAcqRel' and
        //:   a flag indicating if the 'timedWait' method is expected to
        //:   signal.  Use 'TestObj', which specializes 'FastPostSemaphoreImpl'
        //:   with 'TestAtomicOperations' and 'TestCondition' (as opposed to
        //:   'bsls::AtomicOperations' and 'bslmt::Condition'), to directly set
        //:   the state of the semaphore and to verify the signalling behavior
        //:   of 'timedWait'.  (C-4)
        //
        // Testing:
        //   CONCERN: MANIPULATORS SIGNAL AS EXPECTED
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "CONCERN: MANIPULATORS SIGNAL AS EXPECTED" << endl
                 << "========================================" << endl;
        }

        if (verbose) cout << "\nTesting 'post()'." << endl;
        {
            static const struct {
                int d_line;       // source line number
                int d_available;  // available count attribute of state
                int d_disabled;   // disabled attribute of state
                int d_blocked;    // blocked attribute of state
                int d_expSignal;  // expected number of signals
            } DATA[] = {
                //LN  AVAILABLE  DISABLED  BLOCKED  SIGNAL
                //--  ---------  --------  -------  ------
                { L_,        -1,        0,       0,      0 },
                { L_,        -1,        0,       1,      0 },
                { L_,        -1,        0,       2,      0 },
                { L_,         0,        0,       0,      0 },
                { L_,         0,        0,       1,      1 },
                { L_,         0,        0,       2,      1 },
                { L_,         1,        0,       0,      0 },
                { L_,         1,        0,       1,      0 },
                { L_,         1,        0,       2,      0 },

                { L_,         0,        1,       1,      0 },
                { L_,         0,        1,       2,      0 },

                { L_,         0,        2,       1,      1 },
                { L_,         0,        2,       2,      1 },
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int AVAILABLE  = DATA[ti].d_available;
                const int DISABLED   = DATA[ti].d_disabled;
                const int BLOCKED    = DATA[ti].d_blocked;
                const int EXP_SIGNAL = DATA[ti].d_expSignal;

                TestObj mX;

                TestAtomicOperations::clearOverride();
                TestAtomicOperations::pushOverride(AVAILABLE,
                                                   DISABLED,
                                                   BLOCKED);

                mX.post();

                LOOP_ASSERT(LINE, EXP_SIGNAL == TestCondition::signalCount());
            }
        }

        if (verbose) cout << "\nTesting 'post(value)'." << endl;
        {
            static const struct {
                int d_line;       // source line number
                int d_available;  // available count attribute of state
                int d_disabled;   // disabled attribute of state
                int d_blocked;    // blocked attribute of state
                int d_value;      // value used with 'post(value)'
                int d_expSignal;  // expected number of signals
            } DATA[] = {
                //LN  AVAILABLE  DISABLED  BLOCKED  VALUE  SIGNAL
                //--  ---------  --------  -------  -----  ------
                { L_,        -1,        0,       0,     1,      0 },
                { L_,        -1,        0,       1,     1,      0 },
                { L_,        -1,        0,       2,     1,      0 },
                { L_,         0,        0,       0,     1,      0 },
                { L_,         0,        0,       1,     1,      1 },
                { L_,         0,        0,       2,     1,      1 },
                { L_,         1,        0,       0,     1,      0 },
                { L_,         1,        0,       1,     1,      0 },
                { L_,         1,        0,       2,     1,      0 },
                { L_,        -1,        0,       0,     2,      0 },
                { L_,        -1,        0,       1,     2,      0 },
                { L_,        -1,        0,       2,     2,      0 },
                { L_,         0,        0,       0,     2,      0 },
                { L_,         0,        0,       1,     2,      1 },
                { L_,         0,        0,       2,     2,      1 },
                { L_,         1,        0,       0,     2,      0 },
                { L_,         1,        0,       1,     2,      0 },
                { L_,         1,        0,       2,     2,      0 },

                { L_,         0,        1,       1,     1,      0 },
                { L_,         0,        1,       2,     1,      0 },
                { L_,         0,        1,       1,     2,      0 },
                { L_,         0,        1,       2,     2,      0 },

                { L_,         0,        2,       1,     1,      1 },
                { L_,         0,        2,       2,     1,      1 },
                { L_,         0,        2,       1,     2,      1 },
                { L_,         0,        2,       2,     2,      1 },
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int AVAILABLE  = DATA[ti].d_available;
                const int DISABLED   = DATA[ti].d_disabled;
                const int BLOCKED    = DATA[ti].d_blocked;
                const int VALUE      = DATA[ti].d_value;
                const int EXP_SIGNAL = DATA[ti].d_expSignal;

                TestObj mX;

                TestAtomicOperations::clearOverride();
                TestAtomicOperations::pushOverride(AVAILABLE,
                                                   DISABLED,
                                                   BLOCKED);

                mX.post(VALUE);

                LOOP_ASSERT(LINE, EXP_SIGNAL == TestCondition::signalCount());
            }
        }

        if (verbose) cout << "\nTesting 'wait' and 'timedWait'." << endl;
        {
            static const struct {
                int d_line;       // source line number
                int d_available;  // available count attribute of state
                int d_disabled;   // disabled attribute of state
                int d_blocked;    // blocked attribute of state
                int d_expSignal;  // expected number of signals
            } DATA[] = {
                //LN  AVAILABLE  DISABLED  BLOCKED  SIGNAL
                //--  ---------  --------  -------  ------
                { L_,        -1,        0,       0,      0 },
                { L_,        -1,        0,       1,      0 },
                { L_,        -1,        0,       2,      0 },
                { L_,         0,        0,       0,      0 },
                { L_,         0,        0,       1,      0 },
                { L_,         0,        0,       2,      0 },
                { L_,         1,        0,       0,      0 },
                { L_,         1,        0,       1,      1 },
                { L_,         1,        0,       2,      1 },
                { L_,         2,        0,       0,      0 },
                { L_,         2,        0,       1,      1 },
                { L_,         2,        0,       2,      1 },

                { L_,         1,        1,       1,      0 },
                { L_,         1,        1,       2,      0 },
                { L_,         2,        1,       1,      0 },
                { L_,         2,        1,       2,      0 },

                { L_,         1,        2,       1,      1 },
                { L_,         1,        2,       2,      1 },
                { L_,         2,        2,       1,      1 },
                { L_,         2,        2,       2,      1 },
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int AVAILABLE  = DATA[ti].d_available;
                const int DISABLED   = DATA[ti].d_disabled;
                const int BLOCKED    = DATA[ti].d_blocked;
                const int EXP_SIGNAL = DATA[ti].d_expSignal;

                {
                    TestObj mX;

                    TestAtomicOperations::clearOverride();
                    TestAtomicOperations::pushOverride(0, 0, 0);
                    TestAtomicOperations::pushOverride(0, 0, 0);

                    // the specified state is for *after* the
                    // 'addInt64NvAcqRel' so the table values are modified to
                    // account for the effect of the 'addInt64NvAcqRel'

                    TestAtomicOperations::pushOverride(AVAILABLE + 1,
                                                       DISABLED,
                                                       BLOCKED + 1);

                    mX.wait();

                    LOOP_ASSERT(LINE,
                                EXP_SIGNAL == TestCondition::signalCount());
                }
                {
                    TestObj mX;

                    TestAtomicOperations::clearOverride();
                    TestAtomicOperations::pushOverride(0, 0, 0);
                    TestAtomicOperations::pushOverride(0, 0, 0);

                    // the specified state is for *after* the
                    // 'addInt64NvAcqRel' so the table values are modified to
                    // account for the effect of the 'addInt64NvAcqRel'

                    TestAtomicOperations::pushOverride(AVAILABLE + 1,
                                                       DISABLED,
                                                       BLOCKED + 1);

                    mX.timedWait(bsls::SystemTime::nowRealtimeClock()
                                                    + bsls::TimeInterval(0.1));

                    LOOP_ASSERT(LINE,
                                EXP_SIGNAL == TestCondition::signalCount());
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'enable', 'disable', 'getDisabledState', AND 'isDisabled'
        //   Ensure the manipulators and accessor function as expected.
        //
        // Concerns:
        //: 1 The methods 'enable' and 'disable' toggle the disabled state as
        //:   expected.
        //:
        //: 2 The method 'getDisabledState' returns a value that indicates
        //:   disabled state as expected.  Furthermore, each change from
        //:   enabled-to-disabled and disabled-to-enabled changes the value
        //:   returned by the method.
        //:
        //: 3 The method 'isDisabled' returns the disabled state as expected.
        //:
        //: 4 A rapid sequence of 'disable' and 'enable' releases all blocked
        //:   threads.
        //:
        //: 5 A long sequence of 'disable' and 'enable' does not affect the
        //:   semaphore's count.  This is a whitebox concern.
        //:
        //: 6 Verify the methods 'getDisabledState' and 'isDisabled' are
        //:   'const' methods.
        //
        // Plan:
        //: 1 Directly verify the effects of 'enable' and 'disable' using
        //:   'tryWait'.  Using the known state of the semaphore, verify
        //:   'getDisabledState' and 'isDisabled'.  (C-1..3)
        //:
        //: 2 Create a semaphore, use the untested 'wait' method to block a
        //:   number of threads, invoke a sequence of 'disable' and 'enable'
        //:   methods, verify the return value of the 'wait' invocations is
        //:   'e_DISABLED', and join the created threads to ensure they were
        //:   unblocked.  Use a watchdog thread to verify the test completes
        //:   timely.  (C-4)
        //:
        //: 3 Create a semaphore, execute a long sequence of 'disable' and
        //:   'enable', use 'tryWait' to verify the count was not modified.
        //:   (C-5)
        //:
        //: 4 Invoke 'getDisabledState' and 'isDisabled' from 'const' objects.
        //:   (C-6)
        //
        // Testing:
        //   void enable();
        //   void disable();
        //   int getDisabledState() const;
        //   bool isDisabled() const;
        // --------------------------------------------------------------------

        if (verbose) {
//----------^
cout << endl
     << "TESTING 'enable', 'disable', 'getDisabledState', AND 'isDisabled'"
     << endl
     << "================================================================="
     << endl;
//----------v
        }

        {
            // basic functionality tests

            Obj mX;  const Obj& X = mX;

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
            ASSERT(!X.isDisabled());

            ASSERT(0 == X.getDisabledState());

            mX.disable();

            ASSERT(1 == X.getDisabledState());

            ASSERT(Obj::e_DISABLED == mX.tryWait());
            ASSERT( X.isDisabled());

            mX.enable();

            ASSERT(2 == X.getDisabledState());

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
            ASSERT(!X.isDisabled());

            mX.disable();
            mX.disable();

            ASSERT(3 == X.getDisabledState());

            ASSERT(Obj::e_DISABLED == mX.tryWait());
            ASSERT( X.isDisabled());

            mX.enable();
            mX.enable();

            ASSERT(4 == X.getDisabledState());

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
            ASSERT(!X.isDisabled());
        }
        {
            // verify 'disable'/'enable' sequences releases threads

            const int k_NUM_THREAD = 5;
            const int k_MAX_LENGTH = 5;

            s_continue = 1;

            bslmt::ThreadUtil::Handle watchdogHandle;
            bslmt::ThreadUtil::create(&watchdogHandle,
                                      watchdog,
                                      const_cast<char *>("enable/disable"));

            for (int length = 1; length <= k_MAX_LENGTH; ++length) {
                Obj mX;

                bslmt::ThreadUtil::Handle handle[k_NUM_THREAD];
                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::create(&handle[i],
                                              waitExpectDisabled,
                                              &mX);
                }

                // sleep to allow the threads to block
                bslmt::ThreadUtil::microSleep(k_DECISECOND);

                for (int i = 0; i < length; ++i) {
                    mX.disable();
                    mX.enable();
                }

                for (int i = 0; i < k_NUM_THREAD; ++i) {
                    bslmt::ThreadUtil::join(handle[i]);
                }
            }

            s_continue = 0;

            bslmt::ThreadUtil::join(watchdogHandle);
        }
        {
            // verify 'disable' and 'enable' do not affect semaphore count

            const int k_LENGTH = 1024;

            Obj mX;
            for (int i = 1; i <= k_LENGTH; ++i) {
                mX.disable();
                mX.enable();
                ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'tryWait'
        //   Ensure the manipulator functions as expected.
        //
        // Concerns:
        //: 1 The method 'tryWait' produces the expected value.
        //:
        //: 2 The method 'tryWait' modifies the semaphore count correctly.
        //:
        //: 3 When the semaphore is disabled, 'tryWait' return 'e_DISABLED'.
        //:
        //: 4 The method 'tryWait' does not block.
        //
        // Plan:
        //: 1 Create semaphores with varying initial count, invoke 'tryWait'
        //:   and directly verify the results.  (C-1,2)
        //:
        //: 2 Create a semaphore with varying initial count, use the untested
        //:   'disable' method to disable this semaphore, and verify the result
        //:   of 'tryWait'.  (C-3)
        //:
        //: 3 Use a watchdog thread to verify the test completes timely.  (C-4)
        //
        // Testing:
        //   int tryWait();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'tryWait'" << endl
                          << "=================" << endl;

        s_continue = 1;

        bslmt::ThreadUtil::Handle watchdogHandle;
        bslmt::ThreadUtil::create(&watchdogHandle,
                                  watchdog,
                                  const_cast<char *>("'tryWait'"));

        for (int initialCount = 0; initialCount < 10; ++initialCount) {
            Obj mX(initialCount);

            for (int i = 0; i < initialCount; ++i) {
                ASSERT(Obj::e_SUCCESS == mX.tryWait());
            }
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
        }

        for (int initialCount = 0; initialCount < 10; ++initialCount) {
            Obj mX(initialCount);

            mX.disable();

            ASSERT(Obj::e_DISABLED == mX.tryWait());
        }

        s_continue = 0;

        bslmt::ThreadUtil::join(watchdogHandle);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS
        //   The basic concern is that the constructors operate as expected.
        //
        // Concerns:
        //: 1 The semaphore count is correctly initialized.
        //:
        //: 2 The clock is correctly initialized.
        //
        // Plan:
        //: 1 Use the untested 'tryWait' and 'post' to verify the count.  (C-1)
        //:
        //: 2 Use the untested 'timedWait' to verify the clock.  (C-2)
        //
        // Testing:
        //   FastPostSemaphoreImpl(clockType = e_REALTIME);
        //   FastPostSemaphoreImpl(int count, clockType = e_REALTIME);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CREATORS" << endl
                          << "========" << endl;

        {
            // verify default count is 0 and default clock is realtime

            Obj mX;

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            mX.post();

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(
                                           bsls::SystemTime::nowRealtimeClock()
                                                   + bsls::TimeInterval(0.1)));

            bsls::TimeInterval duration = bsls::SystemTime::nowMonotonicClock()
                                        - start;

            ASSERT(bsls::TimeInterval(0.05) <= duration);
            ASSERT(bsls::TimeInterval(0.15) >= duration);
        }
        {
            // verify initial count can be set

            Obj mX(-1);

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            mX.post();

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            mX.post();

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(
                                           bsls::SystemTime::nowRealtimeClock()
                                                   + bsls::TimeInterval(0.1)));

            bsls::TimeInterval duration = bsls::SystemTime::nowMonotonicClock()
                                        - start;

            ASSERT(bsls::TimeInterval(0.05) <= duration);
            ASSERT(bsls::TimeInterval(0.15) >= duration);
        }
        {
            // verify clock can be set

            Obj mX(bsls::SystemClockType::e_MONOTONIC);

            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(
                                          bsls::SystemTime::nowMonotonicClock()
                                                   + bsls::TimeInterval(0.1)));

            bsls::TimeInterval duration = bsls::SystemTime::nowMonotonicClock()
                                        - start;

            ASSERT(bsls::TimeInterval(0.05) <= duration);
            ASSERT(bsls::TimeInterval(0.15) >= duration);
        }
        {
            // verify initial count and clock can be set

            Obj mX(2, bsls::SystemClockType::e_MONOTONIC);

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(
                                          bsls::SystemTime::nowMonotonicClock()
                                                   + bsls::TimeInterval(0.1)));

            bsls::TimeInterval duration = bsls::SystemTime::nowMonotonicClock()
                                        - start;

            ASSERT(bsls::TimeInterval(0.05) <= duration);
            ASSERT(bsls::TimeInterval(0.15) >= duration);
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
        // Plan:
        //: 1 Instantiate an object and verify basic functionality.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX;

        mX.post();
        mX.post(2);
        mX.wait();

        ASSERT(0 == mX.timedWait(bsls::SystemTime::nowRealtimeClock() +
                                 bsls::TimeInterval(60)));
        ASSERT(0 == mX.tryWait());
        ASSERT(0 != mX.tryWait());
        ASSERT(0 != mX.timedWait(bsls::SystemTime::nowRealtimeClock() +
                                 bsls::TimeInterval(0.1)));
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
// Copyright 2019 Bloomberg Finance L.P.
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
