// bslmt_mutexassert.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_mutexassert.h>

#include <bslmt_mutex.h>
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

using namespace BloombergLP;
using namespace bsl;

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
//: o BSLMT_MUTEXASSERT_IS_LOCKED
//: o BSLMT_MUTEXASSERT_IS_LOCKED_SAFE
//: o BSLMT_MUTEXASSERT_IS_LOCKED_OPT
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

int verbose;
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
// 'BSLMT_MUTEXASSERT_IS_LOCKED*' family of assertions allows the programmers
// to verify, using defensive programming techniques, that the mutex in
// question is indeed locked.
//
// Suppose we have a fully thread-safe queue that contains 'int' values, and is
// guarded by an internal mutex.  We can use 'BSLMT_MUTEXASSERT_IS_LOCKED_SAFE'
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

        mutable bslmt::Mutex  d_mutex;    // mutex to provide thread safety

        // PRIVATE MANIPULATOR
        int popImp(int *result);
            // Assign the value at the front of the queue to the specified
            // '*result', and remove the value at the front of the queue;
            // return 0 if the queue was not initially empty, and a non-zero
            // value (with no effect) otherwise.  The behavior is undefined
            // unless 'd_mutex' is locked.

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

        void push(int value);
            // ...

        template <class INPUT_ITER>
        void pushRange(const INPUT_ITER& first, const INPUT_ITER& last);
            // ...
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
        BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&d_mutex);

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
// 'BSLMT_MUTEXASSERT_IS_LOCKED*' macros.  We use the '...IS_LOCKED_SAFE...'
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

        d_mutex.lock();
        int rc = popImp(result);
        d_mutex.unlock();
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
        d_mutex.lock();
        d_deque.push_back(value);
        d_mutex.unlock();
    }

    template <class INPUT_ITER>
    void MyThreadSafeQueue::pushRange(const INPUT_ITER& first,
                                      const INPUT_ITER& last)
    {
        d_mutex.lock();
        d_deque.insert(d_deque.begin(), first, last);
        d_mutex.unlock();
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
// 'BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&d_mutex)' macro issues an error message
// and aborts:
//..
//  Assertion failed: BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&d_mutex),
//  file bslmt_mutexassertislocked.t.cpp, line 137 Aborted (core dumped)
//..
// Finally, note that the message printed above and the subsequent aborting of
// the program were the result of a call to 'bsls::Assert::invokeHandler',
// which in this case was configured (by default) to call
// 'bsls::Assert::failAbort'.  Other handlers may be installed that produce
// different results, but in all cases should prevent the program from
// proceeding normally.

                                  // ------
                                  // case 3
                                  // ------

struct TestCase3SubThread {
    bslmt::Mutex    *d_mutexToAssertOn;
    bslmt::Mutex    *d_mutexThatMainThreadWillUnlock;
    bsls::AtomicInt *d_subthreadWillIncrementValue;

    void operator()()
    {
        d_mutexToAssertOn->lock();
        ++*d_subthreadWillIncrementValue;
        d_mutexThatMainThreadWillUnlock->lock();

        // Both mutexes are locked, unlock them so they won't assert when
        // destroyed.
        d_mutexThatMainThreadWillUnlock->unlock();
        d_mutexToAssertOn->unlock();
    }
};

                                  // ------
                                  // case 2
                                  // ------

namespace TEST_CASE_2 {

enum AssortMode {
    e_SAFE_MODE,
    e_NORMAL_MODE,
    e_OPT_MODE
} mode;

int expectedLine;

void myHandler(const char *text, const char *file, int line)
{
    switch (mode) {
      case e_SAFE_MODE: {
        ASSERT(!bsl::strcmp("BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&mutex)", text));
      } break;
      case e_NORMAL_MODE: {
        ASSERT(!bsl::strcmp("BSLMT_MUTEXASSERT_IS_LOCKED(&mutex)",      text));
      } break;
      case e_OPT_MODE: {
        ASSERT(!bsl::strcmp("BSLMT_MUTEXASSERT_IS_LOCKED_OPT(&mutex)",  text));
      } break;
      default: {
        ASSERT(0);
      }
    }

    LOOP2_ASSERT(line, expectedLine, expectedLine == line);

    ASSERT(!bsl::strcmp(__FILE__, file));

#ifdef BDE_BUILD_TARGET_EXC
    throw 5;
#else
    // We can't return to 'bsls::Assert::invokeHandler'.  Make sure this test
    // fails.

    ASSERT(0 &&
              "BSLMT_MUTEXASSERT_IS_LOCKED* failed wtih exceptions disabled");
    abort();
#endif
}

}  // close namespace TEST_CASE_2

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
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
        //: 1 That 'BSLMT_MUTEXASSERT_IS_LOCKED*' is never calling
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

        bslmt::Mutex    mutexToAssertOn;
        bslmt::Mutex    mutexThatMainThreadWillUnlock;
        bsls::AtomicInt subthreadWillIncrementValue;

        subthreadWillIncrementValue = 0;
        mutexThatMainThreadWillUnlock.lock();

        TestCase3SubThread functor;
        functor.d_mutexToAssertOn               = &mutexToAssertOn;
        functor.d_mutexThatMainThreadWillUnlock =
                                                &mutexThatMainThreadWillUnlock;
        functor.d_subthreadWillIncrementValue   = &subthreadWillIncrementValue;

        bslmt::ThreadUtil::Handle handle;
        int sts = bslmt::ThreadUtil::create(&handle, functor);
        ASSERT(0 == sts);

        bslmt::ThreadUtil::microSleep(10 * 1000);

        while (0 == subthreadWillIncrementValue) {
            ; // do nothing
        }

        // The subthread has locked the mutex.  Now observe that none of these
        // macros blow up.

        BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&mutexToAssertOn);
        BSLMT_MUTEXASSERT_IS_LOCKED(     &mutexToAssertOn);
        BSLMT_MUTEXASSERT_IS_LOCKED_OPT( &mutexToAssertOn);

        // The subthread is blocked waiting for us to unlock
        // 'mutexThatMainThreadWillUnlock'.  Unlock it so the subthread can
        // finish and join the sub thread.

        mutexThatMainThreadWillUnlock.unlock();
        sts = bslmt::ThreadUtil::join(handle);
        ASSERT(0 == sts);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'BSLMT_MUTEXASSERT_IS_LOCKED*'
        //
        // Concerns:
        //: 1 That 'BSLMT_MUTEXASSERT_IS_LOCKED*' is never calling
        //:   'bsls::Assert::invokeHandler' if the mutex is locked.
        //: 2 That, in appropriate build modes, 'invokeHandler' is in fact
        //:   called.  This test is only run when exceptions are enabled.
        //
        //: Plan:
        //: 1 With the mutex locked and the assert handler set to
        //:   'bsls::failAbort' (the default), call all three '*_IS_LOCKED'
        //:   asserts and verify that they don't fail (C-1).
        //: 2 Only if exceptions are enabled, unlock the mutex and set the
        //:   assert handler to 'TEST_CASE_2::myHandler' then call all 3
        //:   macros in try-catch blocks.  Expect throws depending on the
        //:   build mode.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'BSLMT_MUTEXASSERT_IS_LOCKED*'\n"
                             "=====================================\n";

        bslmt::Mutex mutex;
        mutex.lock();

        if (veryVerbose) cout << "Plan 1: testing with mutex locked\n";
        {

            BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&mutex);
            BSLMT_MUTEXASSERT_IS_LOCKED(     &mutex);
            BSLMT_MUTEXASSERT_IS_LOCKED_OPT( &mutex);
        }

        if (veryVerbose) cout << "Plan 2: testing with mutex unlocked\n";
        {
            mutex.unlock();

#ifdef BDE_BUILD_TARGET_EXC

            bsls::Assert::setFailureHandler(&TEST_CASE_2::myHandler);

            bool expectThrow;

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
            expectThrow = true;
#else
            expectThrow = false;
#endif

            try {
                TEST_CASE_2::mode = TEST_CASE_2::e_SAFE_MODE;
                TEST_CASE_2::expectedLine = __LINE__ + 1;
                BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&mutex);
                ASSERT(!expectThrow);

                if (veryVerbose) cout << "Didn't throw SAFE\n";
            } catch (int thrown) {
                ASSERT(5 == thrown);
                ASSERT(expectThrow);

                if (veryVerbose) cout << "Threw SAFE\n";
            }

#ifdef BSLS_ASSERT_IS_ACTIVE
            expectThrow = true;
#else
            expectThrow = false;
#endif

            try {
                TEST_CASE_2::mode = TEST_CASE_2::e_NORMAL_MODE;
                TEST_CASE_2::expectedLine = __LINE__ + 1;
                BSLMT_MUTEXASSERT_IS_LOCKED(&mutex);
                ASSERT(!expectThrow);

                if (veryVerbose) cout << "Didn't throw\n";
            } catch (int thrown) {
                ASSERT(5 == thrown);
                ASSERT(expectThrow);

                if (veryVerbose) cout << "Threw\n";
            }

#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
            expectThrow = true;
#else
            expectThrow = false;
#endif

            try {
                TEST_CASE_2::mode = TEST_CASE_2::e_OPT_MODE;
                TEST_CASE_2::expectedLine = __LINE__ + 1;
                BSLMT_MUTEXASSERT_IS_LOCKED_OPT(&mutex);
                ASSERT(!expectThrow);

                if (veryVerbose) cout << "Didn't throw OPT\n";
            } catch (int thrown) {
                ASSERT(5 == thrown);
                ASSERT(expectThrow);

                if (veryVerbose) cout << "Threw OPT\n";
            }

            bsls::Assert::setFailureHandler(&bsls::Assert::failAbort);
#endif
        }
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

        bslmt::Mutex mutex;
        mutex.lock();

        // All of these asserts should pass.

        BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&mutex);
        BSLMT_MUTEXASSERT_IS_LOCKED(     &mutex);
        BSLMT_MUTEXASSERT_IS_LOCKED_OPT( &mutex);

        mutex.unlock();
      } break;
      case -1: {
        // ------------------------------------------------------------------
        // TESTING 'BSLMT_MUTEXASSERT_IS_LOCKED_OPT'
        // ------------------------------------------------------------------

        if (verbose) cout << "WATCH ASSERT BLOW UP\n"
                             "====================\n";

        bslmt::Mutex mutex;

        cout << "Expect opt assert fail now, line number is: " <<
                                                          __LINE__ + 2 << endl;

        BSLMT_MUTEXASSERT_IS_LOCKED_OPT(&mutex);

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
