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
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED'
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE'
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT'
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ'
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE'
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT'
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE'
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE'
//: o 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT'
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// [ 2] CONCERN: Testing macros on mutexes locked by the current thread
// [ 2] CONCERN: Testing macros on unlocked mutexes
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
// 'bslmt::ReaderWriterMutex' that allows multiple concurrent readers.
//
// Sometimes multithreaded code is written such that the author of a function
// requires that a caller has already acquired a readerwritermutex.  The
// 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED*' family of assertions allows the
// programmers to verify, using defensive programming techniques, that the
// readerwritermutex in question is indeed locked.
//
// Suppose we have a fully thread-safe queue that contains 'int' values, and is
// guarded by an internal readerwritermutex.  We can use
// 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED' to ensure (in appropriate build
// modes) that proper internal locking of the mutex is taking place.
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

#if 0
        void addValueToEach(int value);
            // Add the specified 'value' to each element of this queue.  The
            // behavior is undefined unless the caller has acquired a write
            // lock on this queue.
#endif

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
            // ...

#if 1
        template <class INPUT_ITER>
        void pushRange(const INPUT_ITER& first, const INPUT_ITER& last);
            // ...
#endif

        // ACCESSORS
        BSL::size_t numElements() const;
            // Return the number of elements in this queue.

        double mean() const; 
            // Return the mean value of the elements of this queue.
    };
//..
// Notice that our public manipulators have two forms: push/pop a single
// element, and push/pop a collection of elements.  Popping even a single
// element is non-trivial, so we factor this operation into a non-*thread-safe*
// private manipulator that performs the pop, and is used in both public 'pop'
// methods.  This private manipulator requires exclusive access to the queue,
// but cannot lock the mutex itself, since the correctness of 'popAll' demands
// that all of the pops be collectively performed using a single lock/unlock.
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
// 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE*' macros.  We use the
// "normal" flavor of the lock (rather than the the '*_SAFE' version) because
// this test is not particularly expensive.
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

#if 1
    template <class INPUT_ITER>
    void MyThreadSafeQueue::pushRange(const INPUT_ITER& first,
                                      const INPUT_ITER& last)
    {
        d_rwMutex.lockWrite();
        d_deque.insert(d_deque.begin(), first, last);
        d_rwMutex.unlockWrite();
    }
#endif
// Notice that each of these methods require exclusive access to the queue --
// and having learned the lession of {'bslmt_mutexassert'|Example 1} -- we were
// careful to acquire a write lock for the duration of each operation.
#if 0
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
#endif
//
// This version of 'MyThreadSafeQueue' also provides two accessor methods. 
// One is 'numElements', the other is 'mean()' (of the values in the
// queue).
// As as theses are 'const'-qualified method (i.e., guaranteed not to modify
// the queue) these methods can be safely called concurrently by multiple
// threads.  Thus, our implementations each acquires a read lock.
//..
    // ACCESSORS
    BSL::size_t MyThreadSafeQueue::numElements() const
    {
        d_rwMutex.lockRead();
        BSL::size_t numElements = d_deque.size();
        d_rwMutex.unlockRead();
        return numElements;
    }
    double MyThreadSafeQueue::mean() const
    {
        d_rwMutex.lockRead();
        bsl::pair<int, double> result = getStats();
        d_rwMutex.unlockRead();
        return result.second;
    }
//..
// Notice that the bulk of the work of 'mean' is done by the private method 'getStats'.
// The calculation is factored out because we must also also calculate
// the mean value in the added 'purgeAll' method.  Our normal inclination may 
// be use the 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ' check in
// 'getStats', but since 'purgeAll' (correctly) guards its operation using
// a write lock we use the 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED' check
// (for either a read lock or a write lock).
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

#if 0
    // PRIVATE MANIPULATORS
    void MyThreadSafeQueue::addValueToEach(int value)
    {
        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(&d_rwMutex);

        for (bsl::deque<int>::iterator itr  = d_deque.begin(),
                                       end  = d_deque.end();
                                       end != itr; ++itr) {
            *itr += value;
        }
    }
#endif


    // MANIPULATORS
#if 0
    void MyThreadSafeQueue::normalize()
    {
        d_rwMutex.lockWrite();
        int adjustment = static_cast<int>(mean()); 
        addValueToEach(-adjustment);
        d_rwMutex.unlockWrite();
    }
#endif
//    
//..
// Finally, we implement the manipulator that must calculate the mean.

    void MyThreadSafeQueue::purgeAll(double limit)
    {
        d_rwMutex.lockWrite();
        bsl::pair<int, double> results = getStats();  // requires some lock
        if (0 < results.first && limit < results.second) {
#if 0
            int dummy;
            int rc;
            while (-1 != (rc = popImp(&dummy))) {  // requies a write lock
                ;
            }
#endif
            for (int i = 0; i < results.first; ++i) {
                int dummy;
                int rc = popImp(&dummy);  // requires a write lock
                ASSERT(0 == rc);
            }
        }
        d_rwMutex.unlockWrite();
    }

// Finally, we confirm that our accessors work as expected:
//..
    void testEnhancedThreadSafeQueue()
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

#ifdef BDE_BUILD_TARGET_EXC

enum AssertMode {
    e_NO_THROW
  , e_SAFE_MODE
  , e_NORMAL_MODE
  , e_OPT_MODE
}          mode;

int        expectedLine;
char       cfg;

void myHandler(const char *text, const char *file, int line)
{
    if (veryVerbose) {
        P(mode)
        P_(expectedLine)  P(cfg)
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

    ASSERTV(expectedText,
            text,
            0 == bsl::strcmp(expectedText, text));

    ASSERTV(expectedLine,
            line,
            expectedLine == line);

    ASSERT(0 ==bsl::strcmp(__FILE__, file));

    throw mode;
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
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 That the usage example compiles and functions as expected.
        //
        // Plan:
        //: o Call 'testEnhancedThreadSafeQueue', which implements and runs the usage
        //:   example
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

#if 0
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
        if (!veryVerbose) {
            cout << "Usage example not run in safe mode unless 'veryVerbose'"
                    " is set since it will abort\n";
            break;
        }
#endif
#endif

        testEnhancedThreadSafeQueue();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED*'
        //
        // Concerns:
        //: 1 The 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED*' macros do not call
        //:   'bsls::Assert::invokeHander' if the readerwritermutex is locked
        //:   -- read locked, write locked, or either, depending on the macro.
        //:
        //: 2 In the expected build mode, if any of these macros are called
        //:   when the readerwritermutex does *not* have the expected lock, the
        //:   currenlty installed 'bsls::Assert::invokeHander' is called with
        //:   the expected values.
        //:
        //: 3 Each of these macros become no-ops except in the build mode for
        //:   which each is intended to be active.
        //
        //: Plan:
        //: 1 With a readerwritemutex locked for read, and the assert handler
        //:   set to 'bsls::failAbort' (the default), call all the read-related
        //:   macros and confirm that they do not fire.  Repeat with a write
        //:   lock and the write-related macros.
        //:
        //: 2 For each build level: '_SAFE', "normal", and '_OPT' if build
        //:   modes where the macros are expected to be enabled, test each of
        //:   the three on an unlocked readerwritermutex and confirm that the
        //:   custom 'TestCase2::myHandler" function is called.  That function
        //:   will confirm that it was called with the expected arguments.
        //
        //: 3 For each build level: '_SAFE', "normal", and '_OPT' if build
        //:   modes where the macros are expected to be *disabled*, test each
        //:   of the three on an unlocked readerwritermutex and confirm that
        //:   the custom 'TestCase2::myHandler" function is *not* called.
        // --------------------------------------------------------------------

        if (verbose) cout
                      < "TESTING 'BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED*'\n"
                        "==================================================\n";


        if (veryVerbose) cout << "testing with readerwritemutex locked\n";
        {
            bslmt::ReaderWriterMutex rwMutex;
            rwMutex.lockRead();

            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT( &rwMutex);

            rwMutex.unlockRead();
            rwMutex.lockWrite();

            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT( &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(&rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(     &rwMutex);
            BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT( &rwMutex);

            rwMutex.unlockWrite();
        }

#ifdef BDE_BUILD_TARGET_EXC

        if (veryVerbose) cout << "testing with readerwritermutex unlocked\n";
        {
            bslmt::ReaderWriterMutex rwMutex;

            bsls::Assert::setFailureHandler(&TestCase2::myHandler);

            if (veryVeryVerbose) cout << "testing '*_SAFE' macros" << endl;
#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
            TestCase2::mode = TestCase2::e_SAFE_MODE;

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
                        TestCase2::expectedLine = __LINE__ + 2;
                        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE(
                                                                     &rwMutex);
                        ASSERTV(cfg, !"Reachable")
                        break;
                      }
                      case 'c': {
                        TestCase2::expectedLine = __LINE__ + 2;
                        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(
                                                                    &rwMutex);
                        ASSERTV(cfg, !"Reachable")
                        break;
                      }
                    }
                } catch (TestCase2::AssertMode thrown) {
                    ASSERTV(cfg,
                            TestCase2::mode,
                            thrown,
                            TestCase2::mode == thrown);
                }
            }

#else  // BSLS_ASSERT_SAFE_IS_ACTIVE
            try {
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_SAFE(      &rwMutex);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_SAFE( &rwMutex);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_SAFE(&rwMutex);
            } catch (TestCase2::AssertMode thrown) {
                ASSERT(!"Reachable")
            }

#endif // BSLS_ASSERT_SAFE_IS_ACTIVE

            if (veryVeryVerbose) cout << "testing 'normal' macros" << endl;
#ifdef BSLS_ASSERT_IS_ACTIVE
            TestCase2::mode = TestCase2::e_NORMAL_MODE;

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
                        TestCase2::expectedLine = __LINE__ + 2;
                        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(
                                                                     &rwMutex);
                        ASSERTV(cfg, !"Reachable")
                        break;
                      }
                    }
                } catch (TestCase2::AssertMode thrown) {
                    ASSERTV(cfg,
                            TestCase2::mode,
                            thrown,
                            TestCase2::mode == thrown);
                }
            }
#else  // BSLS_ASSERT_IS_ACTIVE
            try {
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED(      &rwMutex);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ( &rwMutex);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE(&rwMutex);
            } catch (TestCase2::AssertMode thrown) {
                ASSERT(!"Reachable")
            }
#endif // BSLS_ASSERT_IS_ACTIVE

            if (veryVeryVerbose) cout << "testing '*_OPT' macros" << endl;
#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
            TestCase2::mode = TestCase2::e_OPT_MODE;

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
                        TestCase2::expectedLine = __LINE__ + 2;
                        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT(
                                                                     &rwMutex);
                        ASSERTV(cfg, !"Reachable")
                        break;
                      }
                      case 'c': {
                        TestCase2::expectedLine = __LINE__ + 2;
                        BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT(
                                                                     &rwMutex);
                        ASSERTV(cfg, !"Reachable")
                        break;
                      }
                    }
                } catch (TestCase2::AssertMode thrown) {
                    ASSERTV(cfg,
                            TestCase2::mode,
                            thrown,
                            TestCase2::mode == thrown);
                }
            }
#else   // BSLS_ASSERT_OPT_IS_ACTIVE
            try {
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_OPT(      &rwMutex);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_READ_OPT( &rwMutex);
                BSLMT_READERWRITERMUTEXASSERT_IS_LOCKED_WRITE_OPT(&rwMutex);
            } catch (TestCase2::AssertMode thrown) {
                ASSERT(!"Reachable")
            }
#endif // BSLS_ASSERT_OPT_IS_ACTIVE

            bsls::Assert::setFailureHandler(&bsls::Assert::failAbort);
        }
#else  // BDE_BUILD_TARGET_EXC

        if (verbose) cout << "Some tests skipped in non-exception build"
                          << endl;

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
