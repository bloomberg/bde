// bslmt_readerwritermutexassert.t.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_readerwritermutexassert.h>

#include <bslmt_readerwritermutex.h>
#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_atomic.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_vector.h>

#include <bsls_atomic.h>

#include <algorithm>  // 'BSL::for_each'
#include <numeric>    // 'BSL::accumulate'
#include <utility>    // 'BSL::make_pair'

#include <cstring>    // 'BSL::strcpy' 'BSL::strcat'
#include <float.h>    // 'DBL_MIN'
#include <stdio.h>    // DEBUG 'printf'

using namespace BloombergLP;
using namespace bsl;
namespace BSL = native_std;  // for Usage Examples

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Two main testing strategies are employed here:
//: 1 Assert on a locked mutex and observe nothing happens.
//: 2 Assert on an unlocked mutex with an assert handler installed that will
//:   throw an exception, catch the exception, and verify that an exception
//:   was thrown.
//-----------------------------------------------------------------------------
// MACROS
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE
//: o BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// [ 2] CONCERN: Testing macros on mutexes locked by the current thread
// [ 2] CONCERN: Testing macros on unlocked mutexes
// [ 3] CONCERN: Testing macros on mutexes locked by a different thread
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

int     verbose;
int veryVerbose;

                              // -------------
                              // Usage Example
                              // -------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Checking Consistency Within a Private Method
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes multithreaded code is written such that the author of a function
// requires that a caller has already acquired a mutex.  The
// 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED*' family of assertions allows the programmers
// to verify, using defensive programming techniques, that the mutex in
// question is indeed locked.
//
// Suppose we have a fully thread-safe queue that contains 'int' values, and is
// guarded by an internal mutex.  We can use 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED'
// to ensure (in appropriate build modes) that proper internal locking of the
// mutex is taking place.
//
// First, we define the container:
//..
    class MyThreadSafeQueue {
        // This 'class' provides a fully *thread-safe* unidirectional queue of
        // 'int' values.  See {'bsls_glossary'|Fully Thread-Safe}.  All public
        // manipulators operate as single, atomic actions.

        // DATA
        bsl::deque<int>      d_deque;    // underlying non-*thread-safe*
                                         // standard container

        mutable bslmt::ReaderWriterMutex
                             d_rwMutex; // readerwritemutex for thread safety

        // PRIVATE MANIPULATOR
        int popImp(int *result);
            // Assign the value at the front of the queue to the specified
            // '*result', and remove the value at the front of the queue;
            // return 0 if the queue was not initially empty, and a non-zero
            // value (with no effect) otherwise.  The behavior is undefined
            // unless 'd_rwWutex' is locked for writing.

        void addValueToEach(int value);
            // Add the specified 'value' to each element of this queue.  The
            // behavior is undefined unless the caller has acquired a write
            // lock on this queue.

        // PRIVATE ACCESSOR
        bsl::pair<int, double> getStats() const;
            // Return a 'bsl::pair<int, int>' containing the number of elements
            // and the mean value of the elements of this queue.  The mean
            // values is set to 'DBL_MIN' if the number of elements is 0.  The behavior
            // is undefined unless the call has locked this queue (either a
            // read lock or write lock).

      public:
        // ...

        // MANIPULATORS
        void normalize();
            // Add the average value (rounded down to the nearest integer) of
            // the elements of this queue to each element in the queue.

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

        void push(int value);
            // ...

        template <class INPUT_ITER>
        void pushRange(const INPUT_ITER& first, const INPUT_ITER& last);
            // ...

        // ACCESSORS
        double mean() const; 
            // Return the mean value of the elements of this queue.
    };
//..
// Notice that our public manipulators have two forms: push/pop a single
// element, and push/pop a collection of elements.  Popping even a single
// element is non-trivial, so we factor this operation into a non-*thread-safe*
// private manipulator that performs the pop, and is used in both public 'pop'
// methods.  This private manipulator requires that the mutex be locked, but
// cannot lock the mutex itself, since the correctness of 'popAll' demands that
// all of the pops be collectively performed using a single mutex lock/unlock.
//
// Then, we define the private manipulator:
//..
    // PRIVATE MANIPULATOR
    int MyThreadSafeQueue::popImp(int *result)
    {
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(&d_rwMutex);

        if (d_deque.empty()) {
            return -1;                                                // RETURN
        }
        else {
            *result = d_deque.front();
            d_deque.pop_front();
            return 0;                                                 // RETURN
        }
    }
//..
// Notice that, on the very first line, the private manipulator verifies, as a
// precondition check, that the mutex has been acquired, using one of the
// 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE*' macros.  We use the '...IS_LOCKED_SAFE...'
// version of the macro so that the check, which on some platforms is as
// expensive as locking the mutex, is performed in only the safe build mode.
//
// Next, we define the public manipulators; each of which must acquire a lock
// on the mutex (note that there is a bug in 'popAll'):
//..
    // MANIPULATORS
    int MyThreadSafeQueue::pop(int *result)
    {
        BSLS_ASSERT(result);

        d_rwMutex.lockWrite();
        int rc = popImp(result);
        d_rwMutex.unlockWrite();
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
        d_rwMutex.lockWrite();
        d_deque.push_back(value);
        d_rwMutex.unlockWrite();
    }

    template <class INPUT_ITER>
    void MyThreadSafeQueue::pushRange(const INPUT_ITER& first,
                                      const INPUT_ITER& last)
    {
        d_rwMutex.lockWrite();
        d_deque.insert(d_deque.begin(), first, last);
        d_rwMutex.unlockWrite();
    }
//..
// Notice that, in 'popAll', we forgot to lock/unlock the mutex!
//
// Then, in our function 'example2Function', we make use of our class to create
// and exercise a 'MyThreadSafeQueue' object:
//..
    void testThreadSafeQueue(bsl::ostream& stream)
    {
        MyThreadSafeQueue queue;
//..
// Next, we populate the queue using 'pushRange':
//..
        const int rawData[] = { 17, 3, 21, -19, 4, 87, 29, 3, 101, 31, 36 };
        enum { k_RAW_DATA_LENGTH = sizeof rawData / sizeof *rawData };

        queue.pushRange(rawData + 0, rawData + k_RAW_DATA_LENGTH);
//..
// Then, we pop a few items off the front of the queue and verify their values:
//..
        int value = -1;

        ASSERT(0 == queue.pop(&value));    ASSERT(17 == value);
        ASSERT(0 == queue.pop(&value));    ASSERT( 3 == value);
        ASSERT(0 == queue.pop(&value));    ASSERT(21 == value);
//..
// Next, we attempt to empty the queue with 'popAll', which, if built in safe
// mode, would fail because it neglects to lock the mutex:
//..
        bsl::vector<int> v;
        queue.popAll(&v);

        stream << "Remaining raw numbers: ";
        for (bsl::size_t ti = 0; ti < v.size(); ++ti) {
            stream << (ti ? ", " : "") << v[ti];
        }
        stream << bsl::endl;
    }
//..
// Then, we build in non-safe mode and run:
//..
//  Remaining raw numbers: -19, 4, 87, 29, 3, 101, 31, 36
//..
// Notice that, since the test case is being run in a single thread and our
// check is disabled, the bug where the mutex was not acquired does not
// manifest itself in a visible error, and we observe the seemingly correct
// output.
//
// Now, we build in safe mode (which enables our check), run the program (which
// calls 'example2Function'), and observe that, when we call 'popAll', the
// 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&d_mutex)' macro issues an error message
// and aborts:
//..
//  Assertion failed: BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&d_mutex),
//  file bslmt_mutexassertislocked.t.cpp, line 137 Aborted (core dumped)
//..
// Finally, note that the message printed above and the subsequent aborting of
// the program were the result of a call to 'bsls::Assert::invokeHandler',
// which in this case was configured (by default) to call
// 'bsls::Assert::failAbort'.  Other handlers may be installed that produce
// different results, but in all cases should prevent the program from
// proceeding normally.
//
//..
    // PRIVATE ACCESSORS
    bsl::pair<int, double> MyThreadSafeQueue::getStats() const
    {
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(&d_rwMutex);

        int numElements = d_deque.size();

        if (0 == numElements) {
            return BSL::make_pair(numElements, DBL_MIN);              // RETURN
        }

        int    sum  = BSL::accumulate(d_deque.cbegin(), d_deque.cend(), 0);
        double mean = static_cast<double>(sum)
                    / static_cast<double>(numElements);

        return BSL::make_pair(numElements, mean);
    }

    // PRIVATE ACCESSORS
    void MyThreadSafeQueue::addValueToEach(int value)
    {
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(&d_rwMutex);

        for (bsl::deque<int>::iterator itr  = d_deque.begin(),
                                       end  = d_deque.end();
                                       end != itr; ++itr) {
            *itr += value;
        }
    }

    // ACCESSORS
    double MyThreadSafeQueue::mean() const
    {
        d_rwMutex.lockRead();
        bsl::pair<int, double> result = getStats();
        d_rwMutex.unlockRead();
        return result.second;
    }

    // MANIPULATORS
    void MyThreadSafeQueue::normalize()
    {
        d_rwMutex.lockWrite();
        int adjustment = static_cast<int>(mean()); 
        addValueToEach(-adjustment);
        d_rwMutex.unlockWrite();
    }


                                  // ------
                                  // case 3
                                  // ------

struct TestCase3SubThread {
    bslmt::ReaderWriterMutex *d_mutexToAssertOn;
    bslmt::ReaderWriterMutex *d_mutexThatMainThreadWillUnlock;
    bsls::AtomicInt          *d_subthreadWillIncrementValue;

    void operator()()
    {
        d_mutexToAssertOn->lockWrite();
        ++*d_subthreadWillIncrementValue;
        d_mutexThatMainThreadWillUnlock->lockWrite();
    }
};

                                  // ------
                                  // case 2
                                  // ------

namespace TestCase2 {

enum AssertMode {
    e_NO_THROW
  , e_SAFE_MODE
  , e_NORMAL_MODE
  , e_OPT_MODE
} mode;

int        expectedLine;
AssertMode expectedThrow = e_NO_THROW;
char       cfg;

void myHandler(const char *text, const char *file, int line)
{
    if (veryVerbose ) {
        P(mode)
        P_(expectedLine) P_(expectedThrow) P(cfg)
        P_(line)         P_(file)          P(text)
    }
    const char *base   = "BSLMT_READERWRITERMUTEXASSERT_IS";

    const char *oper   = 'a' == cfg ? "_LOCKED"       :
                         'b' == cfg ? "_LOCKED_READ"  :
                         'c' == cfg ? "_LOCKED_WRITE" :
                         /* else */   "_UNKNOWN_OPER" ;

    const char *level  = e_SAFE_MODE   == mode ? "_SAFE" :
                         e_NORMAL_MODE == mode ? ""      :
                         e_OPT_MODE    == mode ? "_OPT"  :
                         /* else */       "UNKNOWN_MODE" ;

    const char *suffix = "(&rwMutex)";

    char expectedText[128];

    BSL::strcpy(expectedText, base);
    BSL::strcat(expectedText, oper);
    BSL::strcat(expectedText, level);
    BSL::strcat(expectedText, suffix);

#if 0
    switch (mode) {
      case e_SAFE_MODE: {
        ASSERT(0 == bsl::strcmp(
                      "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex)",
                      text));
      } break;
      case e_NORMAL_MODE: {
        ASSERT(0 == bsl::strcmp(
                           "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(&rwMutex)",
                           text));
      } break;
      case e_OPT_MODE: {
        ASSERT(0 == bsl::strcmp(
                       "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT(&rwMutex)",
                       text));
      } break;
      default: {
        ASSERTV(mode, 0);
      }
    }
#endif

    ASSERTV(expectedText,
            text,
            0 == bsl::strcmp(expectedText, text));

    ASSERTV(expectedLine,
            line,
            expectedLine == line);

    ASSERT(0 ==bsl::strcmp(__FILE__, file));

#ifdef BDE_BUILD_TARGET_EXC
    throw mode;
#else
    // We cannot return to 'bsls::Assert::invokeHandler'.  Make sure this test
    // fails.

    ASSERT(0 &&
           "BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED* "
           "failed with exceptions disabled");
    abort();
#endif // BDE_BUILD_TARGET_EXC
}

}  // close namespace TestCase2

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 That the usage example compiles and functions as expected.
        //
        // Plan:
        //: o Call 'testThreadSafeQueue', which implements and runs the usage
        //:   example, but don't call it in safe assert mode unless
        //:   'veryVerbose' is selected, since it will abort in that mode.
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
        if (!veryVerbose) {
            cout << "Usage example not run in safe mode unless 'veryVerbose'"
                    " is set since it will abort\n";
            break;
        }
#endif

        testThreadSafeQueue(cout);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ON LOCK HELD BY ANOTHER THREAD
        //
        // Concerns:
        //: 1 That 'BSLMT__ASSERTREADERWRITERMUTEX_IS_LOCKED*' is never calling
        //:   'bsls::Assert::invokeHandler' if the mutex is locked by another
        //:   thread.
        //
        // Plan:
        //: o Spawn a subthread that will lock a mutex, then, once it has,
        //:   call the macros to assert that it is locked and observe that
        //:   no failures occur.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING LOCK HELD BY OTHER THREAD\n"
                             "=================================\n";

        bslmt::ReaderWriterMutex mutexToAssertOn;
        bslmt::ReaderWriterMutex mutexThatMainThreadWillUnlock;
        bsls::AtomicInt          subthreadWillIncrementValue;

        subthreadWillIncrementValue = 0;
        mutexThatMainThreadWillUnlock.lockWrite();

        TestCase3SubThread functor;
        functor.d_mutexToAssertOn = &mutexToAssertOn;
        functor.d_mutexThatMainThreadWillUnlock =
                                        &mutexThatMainThreadWillUnlock;
        functor.d_subthreadWillIncrementValue = &subthreadWillIncrementValue;

        bslmt::ThreadUtil::Handle handle;
        int sts = bslmt::ThreadUtil::create(&handle, functor);
        ASSERT(0 == sts);

        bslmt::ThreadUtil::microSleep(10 * 1000);

        while (0 == subthreadWillIncrementValue) {
            ; // do nothing
        }

        // The subthread has locked the mutex.  Now observe that none of these
        // macros blow up.

        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&mutexToAssertOn);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(     &mutexToAssertOn);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &mutexToAssertOn);

        // The subthread is blocked waiting for us to unlock
        // 'mutexThatMainThreadWillUnlock'.  Unlock it so the subthread can
        // finish and join the sub thread.

        mutexThatMainThreadWillUnlock.unlockWrite();
        sts = bslmt::ThreadUtil::join(handle);
        ASSERT(0 == sts);

        // Both mutexes are locked, unlock them so they won't assert when
        // destroyed.

        mutexToAssertOn.              unlockWrite();
        mutexThatMainThreadWillUnlock.unlockWrite();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED*'
        //
        // Concerns:
        //: 1 That 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED*' is never calling
        //:   'bsls::Assert::invokeHandler' if the mutex is locked.
        //: 2 That, in appropriate build modes, 'invokeHandler' is in fact
        //:   called.  This test is only run when exceptions are enabled.
        //
        //: Plan:
        //: 1 With the mutex locked and the assert handler set to
        //:   'bsls::failAbort' (the default), call all three '*_IS_LOCKED'
        //:   asserts and verify that they don't fail (C-1).
        //: 2 Only if exceptions are enabled, unlock the mutex and set the
        //:   assert handler to 'TestCase2::myHandler' then call all 3
        //:   macros in try-catch blocks.  Expect throws depending on the
        //:   build mode.
        // --------------------------------------------------------------------

        if (verbose) cout
                      < "TESTING 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED*'\n"
                        "==================================================\n";

#ifdef BDE_BUILD_TARGET_EXC

// Save 'BSLS_ASSERT_*_IS_ACTIVE' states.

#if defined(      BSLS_ASSERT_SAFE_IS_ACTIVE)
#   define  SAVED_BSLS_ASSERT_SAFE_IS_ACTIVE
#endif
#if defined(      BSLS_ASSERT_IS_ACTIVE)
#   define  SAVED_BSLS_ASSERT_IS_ACTIVE
#endif
#if defined(      BSLS_ASSERT_OPT_IS_ACTIVE)
#   define  SAVED_BSLS_ASSERT_OPT_IS_ACTIVE
#endif


        if (veryVerbose) cout << "testing with mutex locked\n";
        {
            bslmt::ReaderWriterMutex rwMutex;
            rwMutex.lockRead();

#define BSLS_ASSERT_SAFE_IS_ACTIVE
#undef  BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT( &rwMutex);

#undef  BSLS_ASSERT_SAFE_IS_ACTIVE
#define BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT( &rwMutex);

#define BSLS_ASSERT_SAFE_IS_ACTIVE
#undef  BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT( &rwMutex);

            rwMutex.unlockRead();
            rwMutex.lockWrite();

#define BSLS_ASSERT_SAFE_IS_ACTIVE
#undef  BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT( &rwMutex);

#undef  BSLS_ASSERT_SAFE_IS_ACTIVE
#define BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT( &rwMutex);

#define BSLS_ASSERT_SAFE_IS_ACTIVE
#undef  BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT( &rwMutex);

            rwMutex.unlockWrite();
        }
#if 0
// Temporary
#define BSLS_ASSERT_SAFE_IS_ACTIVE
#define BSLS_ASSERT_IS_ACTIVE
#define BSLS_ASSERT_OPT_IS_ACTIVE
#endif
        if (veryVerbose) cout << "Plan 2: testing with mutex unlocked\n";
        {
            bslmt::ReaderWriterMutex rwMutex;
            bool expectThrow;


            bsls::Assert::setFailureHandler(&TestCase2::myHandler);
// ----------------------------------------------------------------------------
#if 1
#define BSLS_ASSERT_SAFE_IS_ACTIVE
#undef  BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE

    TestCase2::expectedThrow = TestCase2::e_SAFE_MODE;
    TestCase2::mode          = TestCase2::e_SAFE_MODE;

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
    for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
        TestCase2::cfg = cfg;
        try {
            switch (cfg) {
              case 'a': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
              case 'b': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
              case 'c': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
            }
        } catch (TestCase2::AssertMode thrown) {
            ASSERTV(cfg,
                    thrown,
                    TestCase2::expectedThrow,
                    TestCase2::expectedThrow == thrown);
        }
    }
#endif // BSLS_ASSERT_SAFE_IS_ACTIVE

#ifdef BSLS_ASSERT_IS_ACTIVE
    for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
        TestCase2::cfg = cfg;
        try {
            switch (cfg) {
              case 'a': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
              case 'b': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
              case 'c': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
            }
        } catch (TestCase2::AssertMode thrown) {
            ASSERTV(cfg,
                    thrown,
                    TestCase2::expectedThrow,
                    TestCase2::expectedThrow == thrown);
        }
    }
#endif // BSLS_ASSERT_IS_ACTIVE

#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
    for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
        TestCase2::cfg = cfg;
        try {
            switch (cfg) {
              case 'a': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
              case 'b': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
              case 'c': {
                TestCase2::expectedLine = __LINE__ + 1;
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT(&rwMutex);
                ASSERTV(cfg, !"Reachable")
                break;
              }
            }
        } catch (TestCase2::AssertMode thrown) {
            ASSERTV(cfg,
                    thrown,
                    TestCase2::expectedThrow,
                    TestCase2::expectedThrow == thrown);
        }
    }
#endif // BSLS_ASSERT_OPT_IS_ACTIVE

#if 0
#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
            printf("BSLS_ASSERT_SAFE_IS_ACTIVE: OK\n");
#else
            printf("BSLS_ASSERT_SAFE_IS_ACTIVE: NG\n");
#endif
#ifdef BSLS_ASSERT_IS_ACTIVE
            printf("BSLS_ASSERT_IS_ACTIVE: OK\n");
#else
            printf("BSLS_ASSERT_IS_ACTIVE: NG\n");
#endif
#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
            printf("BSLS_ASSERT_OPT_IS_ACTIVE: OK\n");
#else
            printf("BSLS_ASSERT_OPT_IS_ACTIVE: NG\n");
#endif
#endif

#if 0
    // This should all be disabled.
    try {
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(      &rwMutex);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ( &rwMutex);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(&rwMutex);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT(      &rwMutex);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT( &rwMutex);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT(&rwMutex);
    } catch (TestCase2::AssertMode thrown) {
        ASSERT(!"Reachable")
    }
#endif

#if 0
#undef  BSLS_ASSERT_SAFE_IS_ACTIVE
#define BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE

#define BSLS_ASSERT_SAFE_IS_ACTIVE
#undef  BSLS_ASSERT_IS_ACTIVE
#undef  BSLS_ASSERT_OPT_IS_ACTIVE
#endif

// ----------------------------------------------------------------------------


#if 0
#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
            expectThrow = true;
#else
            expectThrow = false;
#endif
#endif
#if 0
            try {
                SETUP( __LINE__ + 1);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
                ASSERT(!expectThrow);

                if (veryVerbose) cout << "Didn't throw SAFE\n";
            } catch (TestCase2::AssertMode thrown) {
                ASSERT(5 == thrown);
                ASSERT(expectThrow);

                if (veryVerbose) cout << "Threw SAFE\n";
            }

#if 0
#ifdef BSLS_ASSERT_IS_ACTIVE
            expectThrow = true;
#else
            expectThrow = false;
#endif
#endif
            try {
                SETUP( __LINE__ + 1);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(&rwMutex);
                ASSERT(!expectThrow);

                if (veryVerbose) cout << "Didn't throw\n";
            } catch (int thrown) {
                ASSERT(5 == thrown);
                ASSERT(expectThrow);

                if (veryVerbose) cout << "Threw\n";
            }
#if 0
#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
            expectThrow = true;
#else
            expectThrow = false;
#endif
#endif

            try {
                SETUP( __LINE__ + 1);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT(&rwMutex);
                ASSERT(!expectThrow);

                if (veryVerbose) cout << "Didn't throw OPT\n";
            } catch (int thrown) {
                ASSERT(5 == thrown);
                ASSERT(expectThrow);

                if (veryVerbose) cout << "Threw OPT\n";
            }
#endif  // 1 
#endif

            bsls::Assert::setFailureHandler(&bsls::Assert::failAbort);

        }

// Restore 'BSLS_ASSERT_*_IS_ACTIVE' states.

#if 0
#if defined(SAVED_BSLS_ASSERT_SAFE_IS_ACTIVE)
#   define        BSLS_ASSERT_SAFE_IS_ACTIVE
#endif
#if defined(SAVED_BSLS_ASSERT_IS_ACTIVE)
#   define        BSLS_ASSERT_IS_ACTIVE
#endif
#if defined(SAVED_BSLS_ASSERT_OPT_IS_ACTIVE)
#   define        BSLS_ASSERT_OPT_IS_ACTIVE
#endif

#undef SAVED_BSLS_ASSERT_SAFE_IS_ACTIVE
#undef SAVED_BSLS_ASSERT_IS_ACTIVE
#undef SAVED_BSLS_ASSERT_OPT_IS_ACTIVE
#endif

#endif // BDE_BUILD_TARGET_EXC

      } break;
      case 1: {
        // ------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy a mutex.  Lock and verify that tryLock fails;
        // unlock and verify that tryLock succeeds.
        // ------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        bslmt::ReaderWriterMutex mutex;
        mutex.lockWrite();

        // All of these asserts should pass.

        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&mutex);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(     &mutex);
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &mutex);

        mutex.unlockWrite();
      } break;
      case -1: {
        // ------------------------------------------------------------------
        // TESTING 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT'
        // ------------------------------------------------------------------

        if (verbose) cout << "WATCH ASSERT BLOW UP\n"
                             "====================\n";

        bslmt::ReaderWriterMutex mutex;

        cout << "Expect opt assert fail now, line number is: " <<
                                                          __LINE__ + 2 << endl;

        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT(&mutex);

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
