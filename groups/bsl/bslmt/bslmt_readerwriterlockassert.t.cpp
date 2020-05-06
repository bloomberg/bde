// bslmt_readerwriterlockassert.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_readerwriterlockassert.h>

#include <bslmt_readerwriterlock.h>
#include <bslmt_readerwritermutex.h>
#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttestexception.h>
#include <bsls_atomic.h>
#include <bsls_nameof.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h> // 'BSL::strcpy' 'BSL::strcat'
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_vector.h>

#include <algorithm>  // 'BSL::for_each'
#include <numeric>    // 'BSL::accumulate'
#include <utility>    // 'BSL::make_pair'

#include <float.h>    // 'DBL_MIN'

using namespace BloombergLP;
using namespace bsl;
namespace BSL = native_std;  // for Usage Examples

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Two main testing strategies are employed here:
//: 1 Assert on a locked lock object and observe nothing happens.
//: 2 Assert on an unlocked lock object with an assert handler installed that
//:   will throw an exception, catch the exception, and verify that an
//:   exception was thrown.
//-----------------------------------------------------------------------------
// MACROS
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED'
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE'
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT'
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ'
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE'
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_OPT'
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE'
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE'
//: o 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_OPT'
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// [ 2] CONCERN: Testing macros on locks held by the current thread.
// [ 2] CONCERN: Testing macros on unlocked lock objects.
//-----------------------------------------------------------------------------

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int         verbose;
int     veryVerbose;
int veryVeryVerbose;

                              // -------------
                              // Usage Example
                              // -------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Checking Consistency Within Private Methods
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example is an generalization of {'bslmt_mutexassert'|Example 1:
// Checking Consistency Within a Private Method}.  In that example, a mutex was
// used to control access.  Here, the (simple) mutex is replaced with a
// 'bslmt::ReaderWriterLock' that is allows multiple concurrent access to the
// queue when possible.
//
// Sometimes multithreaded code is written such that the author of a function
// requires that a caller has already acquired a lock.  The
// 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED*' family of assertions allows the
// programmers to detect, using defensive programming techniques, if the
// required lock has *not* been acquired.
//
// Suppose we have a fully thread-safe queue that contains 'int' values, and is
// guarded by an internal 'bslmt::ReaderWriterLock' object.
//
// First, we define the container class:
//..
    class MyThreadSafeQueue {
        // This 'class' provides a fully *thread-safe* unidirectional queue of
        // 'int' values.  See {'bsls_glossary'|Fully Thread-Safe}.  All public
        // methods operate as single, atomic actions.

        // DATA
        bsl::deque<int>      d_deque;    // underlying non-*thread-safe*
                                         // standard container

        mutable bslmt::ReaderWriterLock
                             d_rwLock;   // coordinate thread access

        // PRIVATE MANIPULATOR
        int popImp(int *result);
            // Assign the value at the front of the queue to the specified
            // '*result', and remove the value at the front of the queue;
            // return 0 if the queue was not initially empty, and a non-zero
            // value (with no effect) otherwise.  The behavior is undefined
            // unless 'd_rwLock' is locked for writing.

        // PRIVATE ACCESSOR
        bsl::pair<int, double> getStats() const;
            // Return a 'bsl::pair<int, int>' containing the number of elements
            // and the mean value of the elements of this queue.  The mean
            // values is set to 'DBL_MIN' if the number of elements is 0.  The
            // behavior is undefined unless the call has locked this queue
            // (either a read lock or write lock).

      public:
        // ...

        // MANIPULATORS

        int pop(int *result);
            // Assign the value at the front of the queue to the specified
            // '*result', and remove the value at the front of the queue;
            // return 0 if the queue was not initially empty, and a non-zero
            // value (with no effect) otherwise.

        void popAll(bsl::vector<int> *result);
            // Assign the values of all the elements from this queue, in order,
            // to the specified '*result', and remove them from this queue.
            // Any previous contents of '*result' are discarded.  Note that, as
            // with the other public manipulators, this entire operation occurs
            // as a single, atomic action.

        void purgeAll(double limit);
            // Remove all elements from this queue if their mean exceeds the
            // specified 'limit'.

        void push(int value);
            // Push the specified 'value' onto this queue.

        template <class INPUT_ITER>
        void pushRange(const INPUT_ITER& first, const INPUT_ITER& last);
            // Push the values from the specified 'first' (inclusive) to the
            // specified 'last' (exclusive) onto this queue.

        // ACCESSORS
        double mean() const;
            // Return the mean value of the elements of this queue.

        BSL::size_t numElements() const;
            // Return the number of elements in this queue.
    };
//..
// Notice that this version of the 'MyThreadSafeQueue' class has two public
// accessors, 'numElements' and 'mean', and an additional manipulator,
// 'purgeAll'.
//
// Then, we implement most of the manipulators:
//..
    // PRIVATE MANIPULATOR
    int MyThreadSafeQueue::popImp(int *result)
    {
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE(&d_rwLock);

        if (d_deque.empty()) {
            return -1;                                                // RETURN
        }
        else {
            *result = d_deque.front();
            d_deque.pop_front();
            return 0;                                                 // RETURN
        }
    }

    // MANIPULATORS
    int MyThreadSafeQueue::pop(int *result)
    {
        BSLS_ASSERT(result);

        d_rwLock.lockWrite();
        int rc = popImp(result);
        d_rwLock.unlockWrite();
        return rc;
    }

    void MyThreadSafeQueue::popAll(bsl::vector<int> *result)
    {
        BSLS_ASSERT(result);

        const int size = static_cast<int>(d_deque.size());
        result->resize(size);
        int *begin = result->begin();
        for (int index = 0; index < size; ++index) {
            int rc = popImp(&begin[index]);
            BSLS_ASSERT(0 == rc);
        }
    }

    void MyThreadSafeQueue::push(int value)
    {
        d_rwLock.lockWrite();
        d_deque.push_back(value);
        d_rwLock.unlockWrite();
    }

    template <class INPUT_ITER>
    void MyThreadSafeQueue::pushRange(const INPUT_ITER& first,
                                      const INPUT_ITER& last)
    {
        d_rwLock.lockWrite();
        d_deque.insert(d_deque.begin(), first, last);
        d_rwLock.unlockWrite();
    }
//..
// Notice that these implementations are identical to those shown in
// {'bslmt_mutexassert'|Example 1} except that the 'lock' calls to the
// 'bslmt::Mutex' there have been changed here to 'lockWrite' calls on a
// 'bslmt::ReaderWriterLock'.  Both operations provide exclusive access to the
// container.
//
// Also notice that, having learned the lesson of {'bslmt_mutexassert'|Example
// 1}, we were careful to acquire a write lock for the duration of each of
// these operation and to check the precondition of the private 'popImp'
// method by using the 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE' macro.
//
// Finally notice that we use the "normal" flavor of the macro (rather than the
// '*_SAFE' version) because this test is not particularly expensive.
//
// Next, we implement the accessor methods of the container:
//..
    // ACCESSORS
    double MyThreadSafeQueue::mean() const
    {
        d_rwLock.lockRead();
        bsl::pair<int, double> result = getStats();
        d_rwLock.unlockRead();
        return result.second;
    }

    BSL::size_t MyThreadSafeQueue::numElements() const
    {
        d_rwLock.lockRead();
        BSL::size_t numElements = d_deque.size();
        d_rwLock.unlockRead();
        return numElements;
    }
//..
// Notice that each of these methods acquire a read lock for the duration of
// the operation.  These locks allow shared access provided that the container
// is not changed, a reasonable assumption for these 'const'-qualified methods.
//
// Also notice that the bulk of the work of 'mean' is done by the private
// method 'getStats'.  One's might except the private method to confirm that a
// lock was acquired by using the
// 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ' macro; however, the reason
// for creating that private method is so that it can be reused by the
// 'purgeAll' method, a non-'const' method that requires a write lock.  Thus,
// 'getStats' is an occassion to use the
// 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED' check (for either a read lock *or*
// a write lock).
//..
    // PRIVATE ACCESSORS
    bsl::pair<int, double> MyThreadSafeQueue::getStats() const
    {
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(&d_rwLock);

        int numElements = d_deque.size();

        if (0 == numElements) {
            return BSL::make_pair(numElements, DBL_MIN);              // RETURN
        }

        int    sum  = BSL::accumulate(d_deque.cbegin(), d_deque.cend(), 0);
        double mean = static_cast<double>(sum)
                    / static_cast<double>(numElements);

        return BSL::make_pair(numElements, mean);
    }
//..
// Next, we implement the 'purgeAll' method:
//..
    void MyThreadSafeQueue::purgeAll(double limit)
    {
        d_rwLock.lockWrite();
        bsl::pair<int, double> results = getStats();      // requires some lock
        if (0 < results.first && limit < results.second) {
            for (int i = 0; i < results.first; ++i) {
                int dummy;
                int rc = popImp(&dummy);               // requires a write lock
                ASSERT(0 == rc);
            }
        }
        d_rwLock.unlockWrite();
    }
//..
// Finally, we confirm that our accessors work as expected:
//..
    void testEnhancedThreadSafeQueue()
        // Exercise the added methods of the 'MyThreadSafeQueue' class.
    {
        MyThreadSafeQueue queue;

        const int rawData[] = { 17, 3, -20, 7, 28 };
        enum { k_RAW_DATA_LENGTH = sizeof rawData / sizeof *rawData };

        queue.pushRange(rawData + 0, rawData + k_RAW_DATA_LENGTH);

        ASSERT(5 == queue.numElements());
        ASSERT(7 == queue.mean());

        queue.push(100000);
        queue.purgeAll(10);

        ASSERTV(queue.numElements(), 0       == queue.numElements());
        ASSERTV(queue.mean()       , DBL_MIN == queue.mean());
    }
//..

                                  // ------
                                  // case 3
                                  // ------

struct TestCase3SubThread {
    bslmt::ReaderWriterLock *d_lockToAssertOn_p;
    bslmt::ReaderWriterLock *d_lockThatMainThreadWillUnlock_p;
    bsls::AtomicInt         *d_subthreadWillIncrementValue_p;

    void operator()()
        // Interact with the main thread.
    {
        d_lockToAssertOn_p->lockWrite();
        ++*d_subthreadWillIncrementValue_p;
        d_lockThatMainThreadWillUnlock_p->lockWrite();
    }
};

                                  // ------
                                  // case 2
                                  // ------

namespace TestCase2 {

#ifdef BDE_BUILD_TARGET_EXC

enum AssertMode {
    e_NO_THROW
  , e_SAFE_MODE
  , e_NORMAL_MODE
  , e_OPT_MODE
};

AssertMode  mode;
int         expectedLine;
char        cfg;
const char *expectedLevel;

void myViolationHandler(const bsls::AssertViolation& violation)
    // Confirm that the specified 'violation' has attributes consistent with
    // those set in the global variables 'mode', 'expectedLine', 'cfg', and
    // 'level'.
{
    if (veryVerbose) {
        P_(mode) P_(expectedLine) P(cfg)
        P_(violation.comment())
        P_(violation.fileName())
        P_(violation.lineNumber())
         P(violation.assertLevel())
    }
    const char *base   = "BSLMT_READERWRITERLOCKASSERT_IS";

    const char *oper   = 'a' == cfg ? "_LOCKED"       :
                         'b' == cfg ? "_LOCKED_READ"  :
                         'c' == cfg ? "_LOCKED_WRITE" :
                         /* else */   "_UNKNOWN_OPER" ;

    const char *level  = e_SAFE_MODE   == mode ? "_SAFE" :
                         e_NORMAL_MODE == mode ? ""      :
                         e_OPT_MODE    == mode ? "_OPT"  :
                         /* else */       "UNKNOWN_MODE" ;

    const char *suffix = "(&rwLock)";

    char expectedText[128];

    BSL::strcpy(expectedText, base);
    BSL::strcat(expectedText, oper);
    BSL::strcat(expectedText, level);
    BSL::strcat(expectedText, suffix);

    ASSERTV(expectedLine, violation.lineNumber(),
            expectedLine == violation.lineNumber());
    ASSERTV(expectedText, violation.comment(),
            0 == bsl::strcmp(expectedText, violation.comment()));
    ASSERTV(__FILE__, violation.fileName(),
            0 == bsl::strcmp(__FILE__, violation.fileName()));
    ASSERTV(expectedLevel, violation.assertLevel(),
            0 == bsl::strcmp(expectedLevel, violation.assertLevel()));

    throw bsls::AssertTestException(violation.comment(),
                                    violation.fileName(),
                                    violation.lineNumber(),
                                    violation.assertLevel());
}

template <class RW_LOCK>
void test()
{

    if (verbose) {
        P(bsls::NameOf<RW_LOCK>())
    }

    if (veryVerbose) cout << "testing with reader-writer lock object locked\n";
    {
        RW_LOCK rwLock;
        rwLock.lockRead();

        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE(&rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ(&rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT( &rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE(&rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ(     &rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_OPT( &rwLock);

        rwLock.unlockRead();
        rwLock.lockWrite();

        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE(&rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(     &rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT( &rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE(&rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE(     &rwLock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_OPT( &rwLock);

        rwLock.unlockWrite();
    }

#ifdef BDE_BUILD_TARGET_EXC

    if (veryVerbose) cout << "testing with reader-writer lock unlocked\n";
    {
        RW_LOCK rwLock;

        bsls::Assert::ViolationHandler priorViolationHandler =
                                              bsls::Assert::violationHandler();
        bsls::Assert::setViolationHandler(&TestCase2::myViolationHandler);

        if (veryVeryVerbose) cout << "testing '*_SAFE' macros" << endl;
#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        TestCase2::mode          = TestCase2::e_SAFE_MODE;
        TestCase2::expectedLevel = bsls::Assert::k_LEVEL_SAFE;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            TestCase2::cfg = cfg;

            try {
                switch (cfg) {
                  case 'a': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                  case 'b': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                  case 'c': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                }
            } catch (bsls::AssertTestException thrown) {
                ASSERT(0 == BSL::strcmp(thrown.level(),
                                        bsls::Assert::k_LEVEL_SAFE));
            }
        }

#else  // BSLS_ASSERT_SAFE_IS_ACTIVE
        try {
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE(      &rwLock);
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_SAFE( &rwLock);
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_SAFE(&rwLock);
        } catch (bsls::AssertTestException thrown) {
            ASSERT(!"Reachable")
        }

#endif // BSLS_ASSERT_SAFE_IS_ACTIVE

        if (veryVeryVerbose) cout << "testing 'normal' macros" << endl;
#ifdef BSLS_ASSERT_IS_ACTIVE
        TestCase2::mode          = TestCase2::e_NORMAL_MODE;
        TestCase2::expectedLevel = bsls::Assert::k_LEVEL_ASSERT;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            TestCase2::cfg = cfg;
            try {
                switch (cfg) {
                  case 'a': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                  case 'b': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                  case 'c': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                }
            } catch (bsls::AssertTestException thrown) {
                ASSERT(0 == BSL::strcmp(thrown.level(),
                                        bsls::Assert::k_LEVEL_ASSERT));
            }
        }
#else  // BSLS_ASSERT_IS_ACTIVE
        try {
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(      &rwLock);
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ( &rwLock);
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE(&rwLock);
        } catch (bsls::AssertTestException thrown) {
            ASSERT(!"Reachable")
        }
#endif // BSLS_ASSERT_IS_ACTIVE

        if (veryVeryVerbose) cout << "testing '*_OPT' macros" << endl;
#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
        TestCase2::mode          = TestCase2::e_OPT_MODE;
        TestCase2::expectedLevel = bsls::Assert::k_LEVEL_OPT;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            TestCase2::cfg = cfg;
            try {
                switch (cfg) {
                  case 'a': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                  case 'b': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_OPT(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                  case 'c': {
                    TestCase2::expectedLine = __LINE__ + 1;
                    BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_OPT(&rwLock);
                    ASSERTV(cfg, !"Reachable")
                    break;
                  }
                }
            } catch (bsls::AssertTestException thrown) {
                ASSERTV(thrown.level(),
                                0 == BSL::strcmp(thrown.level(),
                                        bsls::Assert::k_LEVEL_OPT));
            }
        }
#else   // BSLS_ASSERT_OPT_IS_ACTIVE
        try {
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT(      &rwLock);
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_READ_OPT( &rwLock);
            BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_WRITE_OPT(&rwLock);
        } catch (bsls::AssertTestException thrown) {
            ASSERT(!"Reachable")
        }
#endif // BSLS_ASSERT_OPT_IS_ACTIVE

        bsls::Assert::setViolationHandler(priorViolationHandler);
    }
#else  // BDE_BUILD_TARGET_EXC

    if (verbose) cout << "Some tests skipped in non-exception build"
                      << endl;

#endif // BDE_BUILD_TARGET_EXC
}

#endif // BDE_BUILD_TARGET_EXC

}  // close namespace TestCase2

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 That the usage example compiles and functions as expected.
        //
        // Plan:
        //: 1 Call 'testEnhancedThreadSafeQueue', which implements and runs the
        //:   usage example
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

        testEnhancedThreadSafeQueue();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED*'
        //
        // Concerns:
        //: 1 The 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED*' macros do not call
        //:   'bsls::Assert::invokeHander' if the reader-writer lock is locked
        //:   -- read locked, write locked, or either, depending on the macro.
        //:
        //: 2 In the expected build mode, if any of these macros are called
        //:   when the reader-writer lock does *not* have the expected lock,
        //:   the currently installed 'bsls::Assert::invokeHander' is called
        //:   with the expected values.
        //:
        //: 3 Each of these macros become no-ops except in the build mode for
        //:   which each is intended to be active.
        //:
        //: 4 The macros perform identically irrespective of the type of lock
        //:   used -- provide the lock provides 'isLocked', 'isLockedRead', and
        //:   'isLockWrite' methods.
        //
        // Plan:
        //: 1 With a reader-writer lock object locked for read, and the assert
        //:   handler set to 'bsls::failAbort' (the default), call all the
        //:   read-related macros and confirm that they do not fire.  Repeat
        //:   with a write lock and the write-related macros.
        //:
        //: 2 For each build level: '_SAFE', "normal", and '_OPT' if build
        //:   modes where the macros are expected to be enabled, test each of
        //:   the three on an unlocked reader-writer lock and confirm that the
        //:   custom 'TestCase2::myHandler" function is called.  That function
        //:   will confirm that it was called with the expected arguments.
        //
        //: 3 For each build level: '_SAFE', "normal", and '_OPT' if build
        //:   modes where the macros are expected to be *disabled*, test each
        //:   of the three on an unlocked reader-writer lock and confirm that
        //:   the custom 'TestCase2::myHandler" function is *not* called.
        //:
        //: 4 Write the test as a function templated on the lock type and
        //:   instantiate the test for two lock types that meet the
        //:   requirements:
        //:   o 'bslmt::ReaderWriterLock'
        //:   o 'bslmt::ReaderWriterMutex'
        //
        // Testing:
        //   CONCERN: Testing macros on locks held by the current thread.
        //   CONCERN: Testing macros on unlocked lock objectss.
        // --------------------------------------------------------------------

        if (verbose) cout
                     << "TESTING 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED*'\n"
                        "=================================================\n";

        TestCase2::test<bslmt::ReaderWriterLock>();
        TestCase2::test<bslmt::ReaderWriterMutex>();

      } break;
      case 1: {
        // ------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The component is sufficiently same to justify comprehensive
        //:   testing.
        //:
        //: 2 Create and destroy a reader-writer lock, write lock it, and
        //:   confirm that the three "IS_LOCKED*" macros pass.
        //
        // Plan
        //: 1 Exercise the component lightly.
        //
        // Testing:
        //   BREATHING TEST
        // ------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        bslmt::ReaderWriterLock lock;
        lock.lockWrite();

        // All of these asserts should pass.

        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_SAFE(&lock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED(     &lock);
        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT( &lock);

        lock.unlockWrite();

      } break;
      case -1: {
        // ------------------------------------------------------------------
        // TESTING 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT'
        //
        // Concerns:
        //: 1 The macros can actually abort the process.
        //
        // Plan:
        //: 1 Execute several failure cases and observe the crash.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout
                  << "TESTING 'BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT'\n"
                     "=====================================================\n";

        if (veryVerbose) cout << "WATCH ASSERT BLOW UP\n"
                                 "====================\n";

        bslmt::ReaderWriterLock lock;

        cout << "Expect opt assert fail now, line number is: " <<
                                                          __LINE__ + 2 << endl;

        BSLMT_READERWRITERLOCKASSERT_IS_LOCKED_OPT(&lock);

        BSLS_ASSERT_OPT(0);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
