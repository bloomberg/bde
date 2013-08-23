// bsls_bsllock.t.cpp                                                 -*-C++-*-
#include <bsls_bsllock.h>

#include <bsls_asserttest.h>     // for testing only
#include <bsls_atomic.h>         // for testing only
#include <bsls_bsltestutil.h>    // for testing only

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a mutually exclusive locking primitive ("mutex") that wraps
// a suitable platform-specific mechanism, and a guard for that mutex type.
// The operations on the mutex type, 'bsls::BslLock', are 'lock' and 'unlock',
// and the lone operation on the guard is 'release'.  In each test we use two
// concurrent threads, verifying that the 'lock', 'unlock', and 'release'
// methods invoked by the respective threads occur in the expected order.
// ----------------------------------------------------------------------------
// 'BslLock' class:
// [ 1] BslLock::BslLock();
// [ 1] BslLock::~BslLock();
// [ 1] void BslLock::lock();
// [ 1] void BslLock::unlock();
//
// 'BslLockGuard' class:
// [ 2] BslLockGuard::BslLockGuard(BslLock *lock);
// [ 2] BslLockGuard::~BslLockGuard();
// [ 2] void BslLockGuard::release();
// ----------------------------------------------------------------------------
// [ 1] BASIC TEST
// [ 3] USAGE EXAMPLE
// [ 2] CONCERN: Precondition violations are detected when enabled.

// ============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::BslLock      Obj;
typedef bsls::BslLockGuard Guard;

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

typedef void *(*ThreadFunction)(void *arg);

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static
ThreadId createThread(ThreadFunction func, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, 0);
#else
    ThreadId id;
    pthread_create(&id, 0, func, arg);
    return id;
#endif
}

static
void joinThread(ThreadId id)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
#else
    pthread_join(id, 0);
#endif
}

static
void sleepSeconds(int seconds)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

enum { MAX_SLEEP_CYCLES = 2 };

static
void pause(bsls::AtomicInt *value)
    // Pause the current thread until the specified '*value' is non-zero, or
    // a sufficient number of sleep cycles have elapsed.
{
    for (int i = 0; 0 == *value && i < MAX_SLEEP_CYCLES; ++i) {
        sleepSeconds(1);
    }
}

                                // -----------
                                // cases 1 & 2
                                // -----------

enum { MAIN_THREAD = 1, CHILD_THREAD = 2 };

struct ThreadInfo {
    Obj             *d_lock;
    bsls::AtomicInt  d_firstIn;
    bsls::AtomicInt  d_threadDone;
};

extern "C" void *threadFunction(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    info->d_lock->lock();                                             // LOCK
    if (0 == info->d_firstIn) {
        info->d_firstIn = CHILD_THREAD;
    }
    info->d_lock->unlock();                                           // UNLOCK
    info->d_threadDone = 1;

    return arg;
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate the use of 'bsls::BslLock' and
// 'bsls::BslLockGuard' to write a thread-safe class, 'my_SafeAccount', given
// a thread-unsafe class, 'my_Account'.  The simple 'my_Account' class is
// defined as follows:
//..
    class my_Account {
        // This 'class' represents a bank account with a single balance.  It
        // is not thread-safe.

        // DATA
        double d_money;  // amount of money in the account

      public:
        // CREATORS
        my_Account();
            // Create an account having a zero balance.

        my_Account(const my_Account& original);
            // Create an account having the value of the specified 'original'
            // account.

        ~my_Account();
            // Destroy this account.

        // MANIPULATORS
        my_Account& operator=(const my_Account& rhs);
            // Assign to this account the value of the specified 'rhs' account,
            // and return a reference providing modifiable access to this
            // account.

        void deposit(double amount);
            // Deposit the specified 'amount' of money into this account.
            // The behavior is undefined unless 'amount >= 0.0'.

        void withdraw(double amount);
            // Withdraw the specified 'amount' of money from this account.
            // The behavior is undefined unless 'amount >= 0.0'.

        // ACCESSORS
        double balance() const;
            // Return the amount of money that is available for withdrawal
            // from this account.
    };
//..
// The implementation of 'my_Account' is straightforward and omitted here for
// brevity.
//
// Next, we use a 'bsls::BslLock' data member to render atomic the function
// calls of a new thread-safe class that uses the thread-unsafe class in its
// implementation.  Note the typical use of 'mutable' for declaring the lock:
//..
    class my_SafeAccountHandle {
        // This 'class' provides a thread-safe handle to the thread-unsafe
        // account that is supplied at construction.

        // DATA
        my_Account            *d_account_p;  // held, not owned
        mutable bsls::BslLock  d_lock;       // guard access to 'd_account_p'

      private:
        // NOT IMPLEMENTED
        my_SafeAccountHandle(const my_SafeAccountHandle&);
        my_SafeAccountHandle& operator=(const my_SafeAccountHandle&);

      public:
        // CREATORS
        my_SafeAccountHandle(my_Account *account);
            // Create a thread-safe handle to the specified modifiable
            // 'account'.

        ~my_SafeAccountHandle();
            // Destroy this handle.  Note that the held account is unaffected
            // by this operation.

        // MANIPULATORS
        my_Account *account();
            // Return an address providing modifiable access to the account
            // held by this handle.

        void deposit(double amount);
            // Atomically deposit the specified 'amount' of money into the
            // account held by this handle.  The behavior is undefined unless
            // the calling thread does not hold the lock on this handle and
            // 'amount >= 0.0'.  Note that this operation is thread-safe.

        void lock();
            // Acquire the lock that provides exclusive access to the
            // underlying account held by this object.  The behavior is
            // undefined unless the calling thread does not already hold the
            // lock on this handle.

        void unlock();
            // Release the lock that provides exclusive access to the
            // underlying account held by this object.  The behavior is
            // undefined unless the calling thread holds the lock on this
            // handle.

        void withdraw(double amount);
            // Atomically withdraw the specified 'amount' of money from the
            // account held by this handle.  The behavior is undefined unless
            // the calling thread does not hold the lock on this handle and
            // 'amount >= 0.0'.  Note that this operation is thread-safe.

        // ACCESSORS
        const my_Account *account() const;
            // Return an address providing non-modifiable access to the account
            // held by this handle.

        double balance() const;
            // Atomically return the amount of money that is available for
            // withdrawal from the account held by this handle.  The behavior
            // is undefined unless the calling thread does not hold the lock
            // on this handle.
    };
//..
// The implementation of 'my_SafeAccountHandle' show-casing the use of
// 'bsls::BslLock', 'bsls::BslLockGuard', and 'my_Account' follows:
//..
    // CREATORS
    my_SafeAccountHandle::my_SafeAccountHandle(my_Account *account)
    : d_account_p(account)
    {
    }

    my_SafeAccountHandle::~my_SafeAccountHandle()
    {
    }

    // MANIPULATORS
    my_Account *my_SafeAccountHandle::account()
    {
        return d_account_p;
    }

    void my_SafeAccountHandle::deposit(double amount)
    {
//..
// In this method we make direct use of the interface of 'bsls::BslLock'.
// However, wherever appropriate, clients should use a 'bsls::BslLockGuard'
// object to ensure that an acquired mutex is always properly released, even if
// an exception is thrown:
//..
        d_lock.lock();  // consider using 'bsls::BslLockGuard'
        d_account_p->deposit(amount);
        d_lock.unlock();
    }

    void my_SafeAccountHandle::lock()
    {
        d_lock.lock();
    }

    void my_SafeAccountHandle::unlock()
    {
        d_lock.unlock();
    }
//..
// In the implementation of 'withdraw' we make use of the 'lock' and 'unlock'
// methods provided by 'my_SafeAccountHandle':
//..
    void my_SafeAccountHandle::withdraw(double amount)
    {
        lock();         // consider using 'bsls::BslLockGuard'
        d_account_p->withdraw(amount);
        unlock();
    }

    // ACCESSORS
    const my_Account *my_SafeAccountHandle::account() const
    {
        return d_account_p;
    }
//..
// The implementation of 'balance' uses a 'bsls::BslLockGuard' to automatically
// acquire and release the lock.  The lock is acquired as a side-effect of the
// construction of 'guard', and released when the guard object is destroyed
// upon returning from the function:
//..
    double my_SafeAccountHandle::balance() const
    {
        bsls::BslLockGuard guard(&d_lock);  // a very good practice

        return d_account_p->balance();
    }
//..

// ============================================================================
//         Additional Functionality Needed to Complete Usage Test Case
// ----------------------------------------------------------------------------

// CREATORS
my_Account::my_Account()
: d_money(0)
{
}

my_Account::~my_Account()
{
}

// MANIPULATORS
void my_Account::deposit(double amount)
{
    d_money += amount;
}

void my_Account::withdraw(double amount)
{
    d_money -= amount;
}

// ACCESSORS
double my_Account::balance() const
{
    return d_money;
}

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// The handle's atomic methods are used just as the corresponding methods in
// 'my_Account':
//..
    my_Account account;
    account.deposit(100.50);
    const double paycheck = 50.25;
    my_SafeAccountHandle handle(&account);

                               ASSERT(100.50 == handle.balance());
    handle.deposit(paycheck);  ASSERT(150.75 == handle.balance());
//..
// Client code can also directly use the handle's 'lock' and 'unlock' methods
// to effect non-primitive atomic transactions on the account:
//..
    const double check[5] = { 25.0, 100.0, 99.95, 75.0, 50.0 };

    handle.lock();

    const double originalBalance = handle.account()->balance();
    for (int i = 0; i < 5; ++i) {
        handle.account()->deposit(check[i]);
    }
    ASSERT(originalBalance + 349.95 == handle.account()->balance());

    handle.unlock();
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // GUARD TEST
        //   Ensure that 'BslLockGuard' works as expected.
        //
        // Concerns:
        //: 1 Following construction, the associated lock is in the locked
        //:   state.
        //:
        //: 2 Following destruction with no prior call to 'release', the
        //:   associated lock is in the unlocked state.
        //:
        //: 3 'release' releases the associated lock from management and
        //:   leaves the lock locked.
        //:
        //: 4 Destruction following a call to 'release' has no effect on the
        //:   state of the associated lock.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a lock, 'mX', in the main thread.  Then, within a nested
        //:   block, create a guard for 'mX', 'mG', and create a child thread
        //:   that has access to 'mX'.  The child thread, upon acquiring the
        //:   lock on 'mX', atomically updates a shared "first in" variable if
        //:   and only if it still has its initial value (0).  Allow the child
        //:   thread to run for a brief time before letting 'mG' go out of
        //:   scope.  Verify, at appropriate points, that the "first in"
        //:   variable has the expected result.  (C-1..2)
        //:
        //: 2 Repeat P-1, except call 'release' on 'mG' before allowing it to
        //:   go out of scope.  Verify that the child thread cannot update the
        //:   "first in" variable until 'unlock' is called on 'mX' in the main
        //:   thread.  (C-3..4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered.  (C-5)
        //
        // Testing:
        //   BslLockGuard::BslLockGuard(BslLock *lock);
        //   BslLockGuard::~BslLockGuard();
        //   void BslLockGuard::release();
        // --------------------------------------------------------------------

        if (verbose) printf("\nGUARD TEST"
                            "\n==========\n");

        // C-1..2
        {
            Obj mX;

            ThreadInfo info;
            info.d_lock       = &mX;
            info.d_firstIn    = 0;
            info.d_threadDone = 0;

            ThreadId id;

            {
                Guard mG(&mX);                                        // LOCK

                id = createThread(&threadFunction, &info);

                pause(&info.d_threadDone);
                ASSERT(0 == info.d_firstIn);
            }                                                         // UNLOCK

            joinThread(id);

            ASSERT(CHILD_THREAD == info.d_firstIn);
        }

        // C-3..4
        {
            Obj mX;

            ThreadInfo info;
            info.d_lock       = &mX;
            info.d_firstIn    = 0;
            info.d_threadDone = 0;

            ThreadId id;

            {
                Guard mG(&mX);                                        // LOCK

                id = createThread(&threadFunction, &info);

                pause(&info.d_threadDone);
                ASSERT(0 == info.d_firstIn);

                mG.release();
            }

            pause(&info.d_threadDone);
            ASSERT(0 == info.d_firstIn);

            mX.unlock();                                              // UNLOCK

            joinThread(id);

            ASSERT(CHILD_THREAD == info.d_firstIn);
        }

        // C-5
        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            {
                Obj mX;

                ASSERT_SAFE_PASS((Guard(&mX)));
                ASSERT_SAFE_FAIL( Guard(  0));
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Ensure that 'BslLock' works as expected.
        //
        // Concerns:
        //: 1 Following construction, the lock is in the unlocked state.
        //:
        //: 2 'lock' acquires the lock for exclusive access by the calling
        //:    thread.
        //:
        //: 3 'unlock' releases the lock making it available to other threads.
        //:
        // Plan:
        //: 1 Create a lock, 'mX', in the main thread, then create a child
        //:   thread that has access to 'mX'.  Each of the threads, upon
        //:   acquiring the lock on 'mX', atomically update a shared "first in"
        //:   variable if and only if it still has its initial value (0); both
        //:   threads use the 'lock' method.  Allow the child thread to run for
        //:   a brief time before locking in the main thread.  After joining
        //:   with the child thread, verify the "first in" variable has the
        //:   expected result.  Repeat, except this time lock 'mX' in the main
        //:   thread *before* creating the child thread.  (C-1..3)
        //
        // Testing:
        //   BslLock::BslLock();
        //   BslLock::~BslLock();
        //   void BslLock::lock();
        //   void BslLock::unlock();
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC TEST"
                            "\n==========\n");

        // C-1..3
        {
            Obj mX;

            ThreadInfo info;
            info.d_lock       = &mX;
            info.d_firstIn    = 0;
            info.d_threadDone = 0;

            ThreadId id = createThread(&threadFunction, &info);

            pause(&info.d_threadDone);

            mX.lock();                                                // LOCK
            if (0 == info.d_firstIn) {
                info.d_firstIn = MAIN_THREAD;
            }
            mX.unlock();                                              // UNLOCK

            joinThread(id);

            ASSERT(CHILD_THREAD == info.d_firstIn);
        }

        {
            Obj mX;
            mX.lock();                                                // LOCK

            ThreadInfo info;
            info.d_lock       = &mX;
            info.d_firstIn    = 0;
            info.d_threadDone = 0;

            ThreadId id = createThread(&threadFunction, &info);

            pause(&info.d_threadDone);

            if (0 == info.d_firstIn) {
                info.d_firstIn = MAIN_THREAD;
            }
            mX.unlock();                                              // UNLOCK

            joinThread(id);

            ASSERT(MAIN_THREAD == info.d_firstIn);
        }

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
