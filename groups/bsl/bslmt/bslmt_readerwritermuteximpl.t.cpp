// bslmt_readerwritermuteximpl.t.cpp                                  -*-C++-*-

#include <bslmt_readerwritermuteximpl.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomicoperations.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// A 'bslmt::ReaderWriterMutexImpl' is the templated-for-testing implementation
// of a reader-writer lock.  The templatization allows for the creation of a
// script-based testing object, 'TestImpl', that enables simplified testing of
// the concerns for each method.  The methods of 'bslmt::ReaderWriterMutexImpl'
// are tested by directly exercising the functionality or by using the
// depth-limited enumeration technique.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] ReaderWriterMutexImpl();
// [ 2] ~ReaderWriterMutexImpl();
//
// MANIPULATORS
// [ 3] void lockRead();
// [ 4] void lockWrite();
// [ 8] int tryLockRead();
// [ 9] int tryLockWrite();
// [ 7] void unlock();
// [ 5] void unlockRead();
// [ 6] void unlockWrite();
//
// ACCESSORS
// [10] bool isLocked() const;
// [10] bool isLockedRead() const;
// [10] bool isLockedWrite() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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

int test        = 0;
int verbose     = 0;
int veryVerbose = 0;

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

// ============================================================================
//                   GLOBAL STRUCTS FOR TESTING
// ----------------------------------------------------------------------------
// The struct 'TestImpl' serves as the concrete type for all three template
// arguments of 'bslmt::ReaderWriterMutexImpl'; 'TestImpl' provides static
// methods for atomic operations, mutex 'lock' and 'unlock', and semaphore
// 'post' and 'wait'.  By using a
// 'ReaderWriterMutexImpl<TestImpl, TestImpl, TestImpl>', a script can be
// defined to test the execution of a 'ReaderWriterMutexImpl' method.
// Specifically, the internal state of the 'ReaderWriterMutexImpl' can be
// verified and set, and the methods called from the implementation can be
// tracked.
//
///SCRIPT SPECIFICATION
///--------------------
// The script is a 'bsl::vector<int>'.  Every negative value in the script
// represents a method call.  The implementation of 'TestImpl' uses 'ASSERT' to
// verify the expected method has been called.  Before every negative script
// entry, there may be zero, one, or two non-negative numbers.  If present, the
// first non-negative number is used to verify ('ASSERT') the value of the
// internal state of the'ReaderWriterMutexImpl'.  If present, the second
// non-negative number is used to replace the state value.
//
// The non-negative script entries are encoded as three digits, with each digit
// representing a different count.  The first digit is the number of writers
// (limited to 0 or 1 due to the 'ReaderWriterMutexImpl' implementation).  The
// second digit is the count of pending writers.  The third digit is the number
// of readers in the lock.

struct TestImpl {
    static const bsls::Types::Int64 k_READER         = 0x0000000000000001LL;
    static const bsls::Types::Int64 k_PENDING_WRITER = 0x0000000100000000LL;
    static const bsls::Types::Int64 k_WRITER         = 0x1000000000000000LL;

    static bsls::AtomicOperations::AtomicTypes::Int64 *s_state_p;

    static bsl::vector<int>                            s_script;
    static bsls::Types::size_type                      s_scriptAt;

    enum {
        k_INIT            =  -1,
        k_GET             =  -2,
        k_ADD             =  -3,
        k_CAS             =  -4,
        k_LOCK            =  -5,
        k_TRYLOCK_SUCCEED =  -6,
        k_TRYLOCK_FAIL    =  -7,
        k_UNLOCK          =  -8,
        k_WAIT            =  -9,
        k_POST            = -10
    };

    static void printScript(int exp)
        // Display an error message providing the script and an indication of
        // the specified 'exp' value at the current script location.
    {
        cout << "   ";
        for (bsls::Types::size_type i = 0; i < s_script.size(); ++i) {
            if (s_scriptAt == i) {
                cout << " (" << exp << " != " << s_script[i] << ')';
            }
            else {
                cout << ' ' << s_script[i];
            }
        }
        cout << endl;
    }

    static void processState()
        // Perform the validate and assignment of the internal state.
    {
        ASSERT(0 != s_state_p);

        if (s_state_p) {
            bsls::Types::Int64 EXP = -1;
            bsls::Types::Int64 set = -1;
            if (   s_scriptAt < s_script.size()
                && s_script[s_scriptAt] >= 0) {
                int  script           = s_script[s_scriptAt];

                int  numReader        =  script        % 10;
                int  numPendingWriter = (script /  10) % 10;
                int  numWriter        = (script / 100) % 10;

                EXP = k_READER         * numReader
                    + k_PENDING_WRITER * numPendingWriter
                    + k_WRITER         * numWriter;

                bsls::Types::Int64 state =
                                   bsls::AtomicOperations::getInt64(s_state_p);

                ASSERT(EXP == state);

                if (veryVerbose && EXP != state) {
                    printScript(static_cast<int>(
                                      state % k_PENDING_WRITER
                                   + (state % k_WRITER) / k_PENDING_WRITER * 10
                                   +  state / k_WRITER * 100));
                }

                ++s_scriptAt;
            }
            if (   s_scriptAt < s_script.size()
                && s_script[s_scriptAt] >= 0) {
                int  script           = s_script[s_scriptAt];

                int  numReader        =  script        % 10;
                int  numPendingWriter = (script /  10) % 10;
                int  numWriter        = (script / 100) % 10;

                set = k_READER * numReader
                    + k_PENDING_WRITER * numPendingWriter
                    + k_WRITER * numWriter;

                bsls::AtomicOperations::setInt64(s_state_p, set);

                ++s_scriptAt;
            }
        }
    }

    static void processFunction(const int expectedFunctionId)
        // Process the script up through the next method call and verify the
        // expected method is called by examining the specified
        // 'expectedFunctionId'.
    {
        processState();

        ASSERT(s_scriptAt < s_script.size());

        if (veryVerbose && s_scriptAt >= s_script.size()) {
            printScript(-999);
        }

        ASSERTV(s_scriptAt,
                expectedFunctionId,
                s_script[s_scriptAt],
                expectedFunctionId == s_script[s_scriptAt]);

        if (veryVerbose &&  expectedFunctionId != s_script[s_scriptAt]) {
            printScript(expectedFunctionId);
        }

        ++s_scriptAt;
    }

    static void assignScript(const bsl::vector<int>& script)
        // Assign the specified 'script' for verification.
    {
        s_script   = script;
        s_scriptAt = 0;
    }

    static void assertScriptComplete()
        // Verify the script has been completely consumed.
    {
        processState();

        ASSERT(s_scriptAt == s_script.size());

        if (veryVerbose && s_scriptAt != s_script.size()) {
            printScript(-999);
        }
    }

    // ATOMIC_OP implementations
    static void initInt64(bsls::AtomicOperations::AtomicTypes::Int64 *pState) {
        s_state_p = pState;

        processFunction(k_INIT);

        bsls::AtomicOperations::initInt64(s_state_p);
    }

    static bsls::Types::Int64 getInt64(
                    const bsls::AtomicOperations::AtomicTypes::Int64 *pState) {
        ASSERT(pState == s_state_p);

        processFunction(k_GET);
        return bsls::AtomicOperations::getInt64(s_state_p);
    }

    static bsls::Types::Int64 getInt64Acquire(
                   const bsls::AtomicOperations::AtomicTypes::Int64 *pState) {
        ASSERT(pState == s_state_p);

        processFunction(k_GET);
        return bsls::AtomicOperations::getInt64(s_state_p);
    }

    static void addInt64AcqRel(
                           bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                           bsls::Types::Int64                          value) {
        ASSERT(pState == s_state_p);

        processFunction(k_ADD);
        bsls::AtomicOperations::addInt64AcqRel(s_state_p, value);
    }

    static bsls::Types::Int64 addInt64Nv(
                           bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                           bsls::Types::Int64                          value) {
        ASSERT(pState == s_state_p);

        processFunction(k_ADD);
        return bsls::AtomicOperations::addInt64Nv(s_state_p, value);
    }

    static bsls::Types::Int64 addInt64NvAcqRel(
                           bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                           bsls::Types::Int64                          value) {
        ASSERT(pState == s_state_p);

        processFunction(k_ADD);
        return bsls::AtomicOperations::addInt64NvAcqRel(s_state_p, value);
    }

    static bsls::Types::Int64 testAndSwapInt64(
                        bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                        bsls::Types::Int64                          value,
                        bsls::Types::Int64                          newValue) {
        ASSERT(pState == s_state_p);

        processFunction(k_CAS);
        return bsls::AtomicOperations::testAndSwapInt64(s_state_p,
                                                        value,
                                                        newValue);
    }

    static bsls::Types::Int64 testAndSwapInt64AcqRel(
                        bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                        bsls::Types::Int64                          value,
                        bsls::Types::Int64                          newValue) {
        ASSERT(pState == s_state_p);

        processFunction(k_CAS);
        return bsls::AtomicOperations::testAndSwapInt64AcqRel(s_state_p,
                                                              value,
                                                              newValue);
    }

    // MUTEX implementations
    static void lock() {
        processFunction(k_LOCK);
    }

    static int tryLock() {
        processState();

        ASSERT(s_scriptAt < s_script.size());

        if (veryVerbose && s_scriptAt >= s_script.size()) {
            printScript(-999);
        }

        ASSERTV(s_scriptAt,
                s_script[s_scriptAt],
                   k_TRYLOCK_SUCCEED == s_script[s_scriptAt]
                || k_TRYLOCK_FAIL    == s_script[s_scriptAt]);

        if (   veryVerbose
            && k_TRYLOCK_SUCCEED != s_script[s_scriptAt]
            && k_TRYLOCK_FAIL    != s_script[s_scriptAt]) {
            printScript(-998);
        }

        int rv = 0;
        if (   s_scriptAt < s_script.size()
            && k_TRYLOCK_FAIL == s_script[s_scriptAt]) {
            rv = 1;
        }

        ++s_scriptAt;

        return rv;
    }

    static void unlock() {
        processFunction(k_UNLOCK);
    }

    // SEMAPHORE implementations
    static void wait() {
        processFunction(k_WAIT);
    }

    static void post() {
        processFunction(k_POST);
    }
};

bsls::AtomicOperations::AtomicTypes::Int64 *TestImpl::s_state_p   = 0;
bsl::vector<int>                            TestImpl::s_script(
                                            bslma::Default::globalAllocator());
bsls::Types::size_type                      TestImpl::s_scriptAt = 0;

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::ReaderWriterMutexImpl<TestImpl, TestImpl, TestImpl>  Obj;

typedef bslmt::ReaderWriterMutexImpl<bsls::AtomicOperations,
                                     bslmt::Mutex,
                                     bslmt::Semaphore>              RealObj;


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
    test        = argc > 1 ? atoi(argv[1]) : 0;
    verbose     = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // ACCESSORS
        //
        // Concerns:
        //: 1 Each accessor correctly returns the known state of a lock object.
        //:
        //: 2 Each accessor is 'const' qualified.
        //
        // Plan:
        //: 1 An ad-hoc sequence of (previously tested) lock and unlock
        //:   operations is used to put a test object into different state.
        //:   The accessors are used to corroborate those states.  (C-1)
        //:
        //: 2 Each accessor invocation is done via a 'const'-reference to the
        //:   object under test.  (C-2)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING ACCESSORS" << endl
                 << "=================" << endl;
        }

        RealObj mX; const RealObj& X = mX;
        ASSERT(false == X.isLocked());
        ASSERT(false == X.isLockedRead());
        ASSERT(false == X.isLockedWrite());

        mX.lockRead();
        ASSERT(true  == X.isLocked());
        ASSERT(true  == X.isLockedRead());
        ASSERT(false == X.isLockedWrite());

        mX.unlockRead();
        ASSERT(false == X.isLocked());
        ASSERT(false == X.isLockedRead());
        ASSERT(false == X.isLockedWrite());

        mX.lockWrite();
        ASSERT(true  == X.isLocked());
        ASSERT(false == X.isLockedRead());
        ASSERT(true  == X.isLockedWrite());

        mX.unlockWrite();
        ASSERT(false == X.isLocked());
        ASSERT(false == X.isLockedRead());
        ASSERT(false == X.isLockedWrite());

        int rcR = mX.tryLockRead();
        ASSERT(0 == rcR);
        ASSERT(true  == X.isLocked());
        ASSERT(true  == X.isLockedRead());
        ASSERT(false == X.isLockedWrite());

        mX.unlockRead();
        ASSERT(false == X.isLocked());
        ASSERT(false == X.isLockedRead());
        ASSERT(false == X.isLockedWrite());

        int rcW = mX.tryLockWrite();
        ASSERT(0 == rcW);
        ASSERT(true  == X.isLocked());
        ASSERT(false == X.isLockedRead());
        ASSERT(true  == X.isLockedWrite());

        mX.unlockWrite();
        ASSERT(false == X.isLocked());
        ASSERT(false == X.isLockedRead());
        ASSERT(false == X.isLockedWrite());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'tryLockWrite'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'tryLockWrite' operates correctly in the presence of other
        //:   manipulations upon the lock.
        //
        // Plan:
        //: 1 Directly test the execution paths.  (C-1)
        //
        // Testing:
        //   void tryLockWrite();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'tryLockWrite'" << endl
                 << "======================" << endl;
        }

        // The values in 'DATA_*' represent the number of writers (0 or 1),
        // number of pending writers, and the number of readers written as
        // digits of the value (e.g., 123 represents 1 writer, 2 pending
        // writers, and 3 readers).

        const int DATA_NO_READER[] = {   0,  10,  20 };

        const int DATA_READER[]    = {   1,   2,   3,
                                        11,  12,  13,
                                        21,  22,  23 };

        const bsls::Types::size_type NUM_NO_READER =
                                sizeof DATA_NO_READER / sizeof *DATA_NO_READER;
        const bsls::Types::size_type NUM_READER =
                                      sizeof DATA_READER / sizeof *DATA_READER;

        if (verbose) cout << "\nSuccess." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_NO_READER; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_NO_READER[i]);
                script.push_back(TestImpl::k_TRYLOCK_SUCCEED);
                script.push_back(TestImpl::k_GET);
                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_NO_READER[i] + 100);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                int rv = obj.tryLockWrite();
                ASSERT(0 == rv);

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nFailure on 'tryLock'." << endl;

        {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(TestImpl::k_TRYLOCK_FAIL);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                int rv = obj.tryLockWrite();
                ASSERT(1 == rv);

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nFailure due to readers." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_READER; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_READER[i]);
                script.push_back(TestImpl::k_TRYLOCK_SUCCEED);
                script.push_back(TestImpl::k_GET);
                script.push_back(TestImpl::k_UNLOCK);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                int rv = obj.tryLockWrite();
                ASSERT(1 == rv);

                TestImpl::assertScriptComplete();
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'tryLockRead'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'tryLockRead' operates correctly in the presence of other
        //:   manipulations upon the lock.
        //
        // Plan:
        //: 1 Directly test the execution paths.  (C-1)
        //
        // Testing:
        //   void tryLockRead();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'tryLockRead'" << endl
                 << "=====================" << endl;
        }

        // The values in 'DATA_*' represent the number of writers (0 or 1),
        // number of pending writers, and the number of readers written as
        // digits of the value (e.g., 123 represents 1 writer, 2 pending
        // writers, and 3 readers).

        const int DATA_CAS[]    = {   0,  1,  2 };

        const int DATA_NO_CAS[] = {  10,  11,  12,
                                     20,  21,  22,
                                    100, 101, 102,
                                    110, 111, 112,
                                    120, 121, 122 };

        const bsls::Types::size_type NUM_CAS =
                                            sizeof DATA_CAS / sizeof *DATA_CAS;

        const bsls::Types::size_type NUM_NO_CAS =
                                      sizeof DATA_NO_CAS / sizeof *DATA_NO_CAS;

        if (verbose) cout << "\nAttempt CAS, CAS succeeds." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_CAS; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_CAS[i]);
                script.push_back(TestImpl::k_GET);
                script.push_back(TestImpl::k_CAS);
                script.push_back(DATA_CAS[i] + 1);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                int rv = obj.tryLockRead();
                ASSERT(0 == rv);

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nAttempt CAS, CAS fails, mutex succeeds."
                          << endl;

        for (bsls::Types::size_type i = 0; i < NUM_CAS; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_CAS[i]);
                script.push_back(TestImpl::k_GET);
                script.push_back(DATA_CAS[i]);
                script.push_back(DATA_CAS[i] + 1);
                script.push_back(TestImpl::k_CAS);
                script.push_back(TestImpl::k_TRYLOCK_SUCCEED);
                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_CAS[i] + 2);
                script.push_back(TestImpl::k_UNLOCK);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                int rv = obj.tryLockRead();
                ASSERT(0 == rv);

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nAttempt CAS, CAS fails, mutex fails."
                          << endl;

        for (bsls::Types::size_type i = 0; i < NUM_CAS; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_CAS[i]);
                script.push_back(TestImpl::k_GET);
                script.push_back(DATA_CAS[i]);
                script.push_back(DATA_CAS[i] + 1);
                script.push_back(TestImpl::k_CAS);
                script.push_back(TestImpl::k_TRYLOCK_FAIL);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                int rv = obj.tryLockRead();
                ASSERT(1 == rv);

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nDo not attempt CAS, mutex succeeds." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_NO_CAS; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_NO_CAS[i]);
                script.push_back(TestImpl::k_GET);
                script.push_back(TestImpl::k_TRYLOCK_SUCCEED);
                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_NO_CAS[i] + 1);
                script.push_back(TestImpl::k_UNLOCK);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                int rv = obj.tryLockRead();
                ASSERT(0 == rv);

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nDo not attempt CAS, mutex fails." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_NO_CAS; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_NO_CAS[i]);
                script.push_back(TestImpl::k_GET);
                script.push_back(TestImpl::k_TRYLOCK_FAIL);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                int rv = obj.tryLockRead();
                ASSERT(1 == rv);

                TestImpl::assertScriptComplete();
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'unlock'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'unlock' operates correctly in the presence of other
        //:   manipulations upon the lock.
        //
        // Plan:
        //: 1 Directly test the execution paths.  (C-1)
        //
        // Testing:
        //   void unlock();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'unlock'" << endl
                 << "================" << endl;
        }

        // The values in 'DATA_*' represent the number of writers (0 or 1),
        // number of pending writers, and the number of readers written as
        // digits of the value (e.g., 123 represents 1 writer, 2 pending
        // writers, and 3 readers).

        const int DATA_NO_POST[] = {  11,  12,  13,
                                      21,  22,  23,
                                      31,  32,  33,
                                     102, 103, 104,
                                     112, 113, 114,
                                     122, 123, 124 };

        const int DATA_POST[]    = { 101, 111, 121 };

        const int DATA_WRITE[] = { 100, 110, 120 };

        const bsls::Types::size_type NUM_NO_POST =
                                    sizeof DATA_NO_POST / sizeof *DATA_NO_POST;
        const bsls::Types::size_type NUM_POST =
                                          sizeof DATA_POST / sizeof *DATA_POST;
        const bsls::Types::size_type NUM_WRITE =
                                        sizeof DATA_WRITE / sizeof *DATA_WRITE;

        if (verbose) {
            cout << "\nUnlock read, no semaphore manipulation." << endl;
        }

        for (bsls::Types::size_type i = 0; i < NUM_NO_POST; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_NO_POST[i]);
                script.push_back(TestImpl::k_GET);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                script.push_back(TestImpl::k_GET);
#endif

                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_NO_POST[i] - 1);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                obj.unlock();

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nUnlock read, semaphore manipulation." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_POST; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_POST[i]);
                script.push_back(TestImpl::k_GET);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                script.push_back(TestImpl::k_GET);
#endif

                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_POST[i] - 1);
                script.push_back(TestImpl::k_POST);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                obj.unlock();

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nUnlock write." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_WRITE; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_WRITE[i]);
                script.push_back(TestImpl::k_GET);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                script.push_back(TestImpl::k_GET);
#endif

                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_WRITE[i] - 100);
                script.push_back(TestImpl::k_UNLOCK);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                obj.unlock();

                TestImpl::assertScriptComplete();
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'unlockWrite'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'unlockWrite' operates correctly in the presence of other
        //:   manipulations upon the lock.
        //
        // Plan:
        //: 1 Directly test the execution paths, with and without semaphore
        //:   manipulation.  (C-1)
        //
        // Testing:
        //   void unlockWrite();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'unlockWrite'" << endl
                 << "=====================" << endl;
        }

        // The values in 'DATA_*' represent the number of writers (0 or 1),
        // number of pending writers, and the number of readers written as
        // digits of the value (e.g., 123 represents 1 writer, 2 pending
        // writers, and 3 readers).

        const int DATA_INITIAL[] = { 100, 101, 102,
                                     110, 111, 112,
                                     120, 121, 122 };

        const bsls::Types::size_type NUM_INITIAL =
                                    sizeof DATA_INITIAL / sizeof *DATA_INITIAL;

        for (bsls::Types::size_type i = 0; i < NUM_INITIAL; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_INITIAL[i]);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                script.push_back(TestImpl::k_GET);
#endif

                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_INITIAL[i] - 100);
                script.push_back(TestImpl::k_UNLOCK);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                obj.unlockWrite();

                TestImpl::assertScriptComplete();
            }
        }

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            {
                bsl::vector<int> script;
                {
                    // Produce the script for the method attempt.

                    script.push_back(TestImpl::k_INIT);
                    script.push_back(0);
                    script.push_back(100);
                    script.push_back(TestImpl::k_GET);
                    script.push_back(TestImpl::k_ADD);
                    script.push_back(0);
                    script.push_back(TestImpl::k_UNLOCK);
                }

                TestImpl::assignScript(script);

                {
                    Obj obj;
                    ASSERT_SAFE_PASS(obj.unlockWrite());

                    TestImpl::assertScriptComplete();
                }
            }
            {
                bsl::vector<int> script;
                {
                    // Produce the script for the method attempt.

                    script.push_back(TestImpl::k_INIT);
                    script.push_back(TestImpl::k_GET);
                }

                TestImpl::assignScript(script);

                {
                    Obj obj;
                    ASSERT_SAFE_FAIL(obj.unlockWrite());
                }
            }
        }
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'unlockRead'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'unlockRead' operates correctly in the presence of other
        //:   manipulations upon the lock.
        //
        // Plan:
        //: 1 Directly test the execution paths, with and without semaphore
        //:   manipulation.  (C-1)
        //
        // Testing:
        //   void unlockRead();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'unlockRead'" << endl
                 << "====================" << endl;
        }

        // The values in 'DATA_*' represent the number of writers (0 or 1),
        // number of pending writers, and the number of readers written as
        // digits of the value (e.g., 123 represents 1 writer, 2 pending
        // writers, and 3 readers).

        const int DATA_NO_POST[] = {  11,  12,  13,
                                      21,  22,  23,
                                      31,  32,  33,
                                     102, 103, 104,
                                     112, 113, 114,
                                     122, 123, 124 };

        const int DATA_POST[]    = { 101, 111, 121 };

        const bsls::Types::size_type NUM_NO_POST =
                                    sizeof DATA_NO_POST / sizeof *DATA_NO_POST;
        const bsls::Types::size_type NUM_POST =
                                          sizeof DATA_POST / sizeof *DATA_POST;

        if (verbose) cout << "\nSemaphore is not manipulated." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_NO_POST; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_NO_POST[i]);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                script.push_back(TestImpl::k_GET);
#endif

                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_NO_POST[i] - 1);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                obj.unlockRead();

                TestImpl::assertScriptComplete();
            }
        }

        if (verbose) cout << "\nSemaphore is manipulated." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_POST; ++i) {
            bsl::vector<int> script;
            {
                // Produce the script for the method attempt.

                script.push_back(TestImpl::k_INIT);
                script.push_back(0);
                script.push_back(DATA_POST[i]);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                script.push_back(TestImpl::k_GET);
#endif

                script.push_back(TestImpl::k_ADD);
                script.push_back(DATA_POST[i] - 1);
                script.push_back(TestImpl::k_POST);
            }

            TestImpl::assignScript(script);

            {
                Obj obj;
                obj.unlockRead();

                TestImpl::assertScriptComplete();
            }
        }

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            {
                bsl::vector<int> script;
                {
                    // Produce the script for the method attempt.
                    script.push_back(TestImpl::k_INIT);
                    script.push_back(0);
                    script.push_back(1);
                    script.push_back(TestImpl::k_GET);
                    script.push_back(TestImpl::k_ADD);
                }

                TestImpl::assignScript(script);

                {
                    Obj obj;
                    ASSERT_SAFE_PASS(obj.unlockRead());

                    TestImpl::assertScriptComplete();
                }
            }
            {
                bsl::vector<int> script;
                {
                    // Produce the script for the method attempt.

                    script.push_back(TestImpl::k_INIT);
                    script.push_back(TestImpl::k_GET);
                }

                TestImpl::assignScript(script);

                {
                    Obj obj;
                    ASSERT_SAFE_FAIL(obj.unlockRead());
                }
            }
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'lockWrite'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'lockWrite' operates correctly in the presence of other
        //:   manipulations upon the lock.
        //
        // Plan:
        //: 1 Directly test the execution paths, with and without semaphore
        //:   acquisition.  (C-1)
        //
        // Testing:
        //   void lockWrite();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'lockWrite'" << endl
                 << "===================" << endl;
        }

        // The values in 'DATA_*' represent the number of writers (0 or 1),
        // number of pending writers, and the number of readers written as
        // digits of the value (e.g., 123 represents 1 writer, 2 pending
        // writers, and 3 readers).

        const int DATA_INITIAL[]   = {   0,   1,   2,
                                        10,  11,  12,
                                        20,  21,  22,
                                       100, 101, 102,
                                       110, 111, 112,
                                       120, 121, 122 };

        const int DATA_READER[]    = {  11,  12,  13,
                                        21,  22,  23,
                                        31,  32,  33,
                                       111, 112, 113,
                                       121, 122, 123,
                                       131, 132, 133 };

        const int DATA_NO_READER[] = {  10,  20,  30,
                                       110, 120, 130 };

        const bsls::Types::size_type NUM_INITIAL =
                                    sizeof DATA_INITIAL / sizeof *DATA_INITIAL;
        const bsls::Types::size_type NUM_READER =
                                      sizeof DATA_READER / sizeof *DATA_READER;
        const bsls::Types::size_type NUM_NO_READER =
                                sizeof DATA_NO_READER / sizeof *DATA_NO_READER;

        if (verbose) cout << "\nSemaphore is acquired." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_INITIAL; ++i) {
            for (bsls::Types::size_type j = 0; j < NUM_READER; ++j) {
                bsl::vector<int> script;
                {
                    // Produce the script for the method attempt.

                    script.push_back(TestImpl::k_INIT);
                    script.push_back(0);
                    script.push_back(DATA_INITIAL[i]);
                    script.push_back(TestImpl::k_ADD);
                    script.push_back(TestImpl::k_LOCK);
                    script.push_back(DATA_INITIAL[i] + 10);
                    script.push_back(DATA_READER[j]);
                    script.push_back(TestImpl::k_ADD);
                    script.push_back(DATA_READER[j] + 100 - 10);
                    script.push_back(TestImpl::k_WAIT);
                }

                TestImpl::assignScript(script);

                {
                    Obj obj;
                    obj.lockWrite();

                    TestImpl::assertScriptComplete();
                }
            }
        }

        if (verbose) cout << "\nSemaphore is not acquired." << endl;

        for (bsls::Types::size_type i = 0; i < NUM_INITIAL; ++i) {
            for (bsls::Types::size_type j = 0; j < NUM_NO_READER; ++j) {
                bsl::vector<int> script;
                {
                    // Produce the script for the method attempt.

                    script.push_back(TestImpl::k_INIT);
                    script.push_back(0);
                    script.push_back(DATA_INITIAL[i]);
                    script.push_back(TestImpl::k_ADD);
                    script.push_back(TestImpl::k_LOCK);
                    script.push_back(DATA_INITIAL[i] + 10);
                    script.push_back(DATA_NO_READER[j]);
                    script.push_back(TestImpl::k_ADD);
                    script.push_back(DATA_NO_READER[j] + 100 - 10);
                }

                TestImpl::assignScript(script);

                {
                    Obj obj;
                    obj.lockWrite();

                    TestImpl::assertScriptComplete();
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'lockRead'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'lockRead' operates correctly in the presence of other
        //:   manipulations upon the lock.
        //
        // Plan:
        //: 1 Using the enumeration technique to a depth of 3, where depth
        //:   implies the number of failed compare-and-swap attempts, verify
        //:   the 'lockRead' method can successfully complete with and without
        //:   mutex acquisition.  (C-1)
        //
        // Testing:
        //   void lockRead();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'lockRead'" << endl
                 << "==================" << endl;
        }

        // The values in 'DATA_*' represent the number of writers (0 or 1),
        // number of pending writers, and the number of readers written as
        // digits of the value (e.g., 123 represents 1 writer, 2 pending
        // writers, and 3 readers).

        const int DATA_WRITER[]    = {  10,  11,  12,
                                        20,  21,  22,
                                       100, 101, 102,
                                       110, 111, 112,
                                       120, 121, 122 };
        const int DATA_NO_WRITER[] = {   0,   1,   2 };

        const bsls::Types::size_type NUM_WRITER =
                                      sizeof DATA_WRITER / sizeof *DATA_WRITER;
        const bsls::Types::size_type NUM_NO_WRITER =
                                sizeof DATA_NO_WRITER / sizeof *DATA_NO_WRITER;

        if (verbose) cout << "\nMutex is acquired." << endl;

        for (int depth = 0; depth <= 3; ++depth) {
            if (verbose) cout << "\tDepth = " << depth << '.' << endl;

            // Compute the number of iterations for the 'depth'.

            bsls::Types::size_type maxIndex = NUM_WRITER;
            for (int i = 0; i < depth; ++i) {
                maxIndex *= NUM_NO_WRITER;
            }

            // Test the method for every possible sequence of the 'depth' and
            // 'DATA_*' values.

            for (bsls::Types::size_type index = 0; index < maxIndex; ++index) {
                bsls::Types::size_type i;
                bsls::Types::size_type d1;
                bsls::Types::size_type d2;
                bsls::Types::size_type d3;
                {
                    bsls::Types::size_type v = index;

                    i  =  v % NUM_WRITER;
                    v  /= NUM_WRITER;
                    d1 =  v % NUM_NO_WRITER;
                    v  /= NUM_NO_WRITER;
                    d2 =  v % NUM_NO_WRITER;
                    v  /= NUM_NO_WRITER;
                    d3 =  v % NUM_NO_WRITER;
                }

                if (   (depth > 1 && d1 == d2)
                    || (depth > 2 && d2 == d3)) {
                    continue;
                }

                // Produce the script for the method attempt.  The 'depth'
                // corresponds to the number of 'CAS' attempts before the mutex
                // is locked.

                bsl::vector<int> script;
                {
                    script.push_back(TestImpl::k_INIT);
                    script.push_back(0);
                    if (0 == depth) {
                        script.push_back(DATA_WRITER[i]);
                        script.push_back(TestImpl::k_GET);
                    }
                    else {
                        script.push_back(DATA_NO_WRITER[d1]);
                        script.push_back(TestImpl::k_GET);
                        script.push_back(DATA_NO_WRITER[d1]);
                        if (1 < depth) {
                            script.push_back(DATA_NO_WRITER[d2]);
                            script.push_back(TestImpl::k_CAS);
                            script.push_back(DATA_NO_WRITER[d2]);
                            if (2 < depth) {
                                script.push_back(DATA_NO_WRITER[d3]);
                                script.push_back(TestImpl::k_CAS);
                                script.push_back(DATA_NO_WRITER[d3]);
                            }
                        }
                        script.push_back(DATA_WRITER[i]);
                        script.push_back(TestImpl::k_CAS);
                    }
                    script.push_back(TestImpl::k_LOCK);
                    script.push_back(TestImpl::k_ADD);
                    script.push_back(DATA_WRITER[i] + 1);
                    script.push_back(TestImpl::k_UNLOCK);
                }

                TestImpl::assignScript(script);

                {
                    Obj obj;
                    obj.lockRead();

                    TestImpl::assertScriptComplete();
                }
            }
        }

        if (verbose) cout << "\nMutex is not acquired." << endl;

        for (int depth = 0; depth <= 3; ++depth) {
            if (verbose) cout << "\tDepth = " << depth << '.' << endl;

            // Compute the number of iterations for the 'depth'.

            bsls::Types::size_type maxIndex = NUM_NO_WRITER;
            for (int i = 0; i < depth; ++i) {
                maxIndex *= NUM_NO_WRITER;
            }

            // Test the method for every possible sequence of the 'depth' and
            // 'DATA_*' values.

            for (bsls::Types::size_type index = 0; index < maxIndex; ++index) {
                bsls::Types::size_type i;
                bsls::Types::size_type d1;
                bsls::Types::size_type d2;
                bsls::Types::size_type d3;
                {
                    bsls::Types::size_type v = index;

                    i  =  v % NUM_NO_WRITER;
                    v  /= NUM_NO_WRITER;
                    d1 =  v % NUM_NO_WRITER;
                    v  /= NUM_NO_WRITER;
                    d2 =  v % NUM_NO_WRITER;
                    v  /= NUM_NO_WRITER;
                    d3 =  v % NUM_NO_WRITER;
                }

                if (   (1 == depth && i == d1)
                    || (2 == depth && (i == d1 || d1 == d2 || d2 == i))
                    || (3 == depth && (   i == d1
                                       || d1 == d2
                                       || d2 == d3
                                       || d3 == i))) {
                    continue;
                }

                // Produce the script for the method attempt.  The 'depth'
                // corresponds to the number of 'CAS' attempts before the
                // compare-and-swap succeeds.

                bsl::vector<int> script;
                {
                    script.push_back(TestImpl::k_INIT);
                    script.push_back(0);
                    if (0 == depth) {
                        script.push_back(DATA_NO_WRITER[i]);
                        script.push_back(TestImpl::k_GET);
                    }
                    else {
                        script.push_back(DATA_NO_WRITER[d1]);
                        script.push_back(TestImpl::k_GET);
                        script.push_back(DATA_NO_WRITER[d1]);
                        if (1 < depth) {
                            script.push_back(DATA_NO_WRITER[d2]);
                            script.push_back(TestImpl::k_CAS);
                            script.push_back(DATA_NO_WRITER[d2]);
                            if (2 < depth) {
                                script.push_back(DATA_NO_WRITER[d3]);
                                script.push_back(TestImpl::k_CAS);
                                script.push_back(DATA_NO_WRITER[d3]);
                            }
                        }
                        script.push_back(DATA_NO_WRITER[i]);
                        script.push_back(TestImpl::k_CAS);
                    }
                    script.push_back(DATA_NO_WRITER[i]);
                    script.push_back(TestImpl::k_CAS);
                    script.push_back(DATA_NO_WRITER[i] + 1);
                }

                TestImpl::assignScript(script);

                {
                    Obj obj;
                    obj.lockRead();

                    TestImpl::assertScriptComplete();
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS TEST
        //   The basic concern is that the default constructor and the
        //   destructor operate as expected.
        //
        // Concerns:
        //: 1 The default constructor correctly initializes the internal state.
        //:
        //: 2 The destructor does nothing.
        //
        // Plan:
        //: 1 Using the 'TestImpl', directly verify the operatios of the
        //:   creators.  (C-1,2)
        //
        // Testing:
        //   ReaderWriterMutexImpl();
        //   ~ReaderWriterMutexImpl();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CREATORS TEST" << endl
                          << "==============" << endl;

        bsl::vector<int> script;
        script.push_back(TestImpl::k_INIT);
        script.push_back(  0);
        TestImpl::assignScript(script);

        {
            Obj obj;

            TestImpl::assertScriptComplete();
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
        //: 1 Create objects.
        //:
        //: 2 Exercise these objects using primary manipulators.
        //:
        //: 3 Verify expected values throughout.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

          {
              bsl::vector<int> script;
              {
                  script.push_back(TestImpl::k_INIT);
              }

              TestImpl::assignScript(script);

              Obj obj;

              TestImpl::assertScriptComplete();
          }
          {
              bsl::vector<int> script;
              {
                  script.push_back(TestImpl::k_INIT);
                  script.push_back(0);
              }

              TestImpl::assignScript(script);

              Obj obj;

              TestImpl::assertScriptComplete();
          }
          {
              bsl::vector<int> script;
              {
                  script.push_back(TestImpl::k_INIT);
                  script.push_back(  0);
                  script.push_back( 10);
                  script.push_back(TestImpl::k_GET);
                  script.push_back(TestImpl::k_LOCK);
                  script.push_back(TestImpl::k_ADD);
                  script.push_back( 11);
                  script.push_back(TestImpl::k_UNLOCK);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                  script.push_back(TestImpl::k_GET);
#endif

                  script.push_back(TestImpl::k_ADD);
                  script.push_back( 10);
              }

              TestImpl::assignScript(script);

              Obj obj;
              obj.lockRead();
              obj.unlockRead();

              TestImpl::assertScriptComplete();
          }
          {
              bsl::vector<int> script;
              {
                  script.push_back(TestImpl::k_INIT);
                  script.push_back(TestImpl::k_GET);
                  script.push_back(TestImpl::k_CAS);
                  script.push_back(  1);
                  script.push_back(101);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                  script.push_back(TestImpl::k_GET);
#endif

                  script.push_back(TestImpl::k_ADD);
                  script.push_back(100);
                  script.push_back(TestImpl::k_POST);
              }

              TestImpl::assignScript(script);

              Obj obj;
              obj.lockRead();
              obj.unlockRead();

              TestImpl::assertScriptComplete();
          }
          {
              bsl::vector<int> script;
              {
                  script.push_back(TestImpl::k_INIT);
                  script.push_back(TestImpl::k_ADD);
                  script.push_back( 10);
                  script.push_back(TestImpl::k_LOCK);
                  script.push_back(TestImpl::k_ADD);
                  script.push_back(100);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
                  script.push_back(TestImpl::k_GET);
#endif

                  script.push_back(TestImpl::k_ADD);
                  script.push_back(  0);
                  script.push_back(TestImpl::k_UNLOCK);
              }

              TestImpl::assignScript(script);

              Obj obj;
              obj.lockWrite();
              obj.unlockWrite();

              TestImpl::assertScriptComplete();
          }
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
// Copyright 2016 Bloomberg Finance L.P.
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
