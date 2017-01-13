// bslmt_readerwritermutex.t.cpp                                      -*-C++-*-

#include <bslmt_readerwritermutex.h>
#include <bslmt_rwmutex.h>

#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_atomic.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
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
// A 'bslmt::ReaderWriterMutex' uses an implementation class and hence testing
// the forwarding to the implementation is all that is required.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] ReaderWriterMutex();
// [ 2] ~ReaderWriterMutex();
//
// MANIPULATORS
// [ 2] void lockRead();
// [ 2] void lockWrite();
// [ 2] int tryLockRead();
// [ 2] int tryLockWrite();
// [ 2] void unlock();
// [ 2] void unlockRead();
// [ 2] void unlockWrite();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] WRITER BIAS
// [ 4] USAGE EXAMPLE

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

typedef bslmt::ReaderWriterMutex Obj;

// ============================================================================
//                   GLOBAL STRUCTS FOR TESTING
// ----------------------------------------------------------------------------

struct ThreadData {
    bslmt::ThreadUtil::Handle  d_handle;
    bslmt::Semaphore           d_step;
    bslmt::Semaphore           d_stepDone;
    Obj                       *d_mutex_p;
    bsls::Types::size_type     d_count;

    ThreadData() : d_mutex_p(0), d_count(0) {}

    ThreadData(Obj *pObj) : d_mutex_p(pObj), d_count(0) {}
};

// ============================================================================
//                   GLOBAL METHODS FOR TESTING
// ----------------------------------------------------------------------------

static bsls::AtomicInt s_continue;

const static int k_COMPLETION_COUNT = 100000;

extern "C" void *watchdog(void *arg)
{
    const char *text = static_cast<const char *>(arg);

    const int MAX = 10;

    int count = 0;

    while (s_continue) {
        bslmt::ThreadUtil::microSleep(100000);
        ++count;

        ASSERTV(text, count < MAX);

        if (MAX == count && s_continue) abort();
    }

    return 0;
}

extern "C" void *writeLock(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);

    while (s_continue == 2) {
        data->d_step.wait();
        data->d_mutex_p->lockWrite();
        data->d_stepDone.post();

        data->d_step.wait();
        data->d_mutex_p->unlock();
        data->d_stepDone.post();
    }

    return 0;
}

extern "C" void *readLock(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);

    while (s_continue == 2) {
        data->d_step.wait();
        data->d_mutex_p->lockRead();
        data->d_stepDone.post();

        data->d_step.wait();
        data->d_mutex_p->unlock();
        data->d_stepDone.post();
    }

    return 0;
}

extern "C" void *writeLockCount(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);

    while (s_continue == 2) {
        data->d_mutex_p->lockWrite();
        data->d_mutex_p->unlock();

        ++data->d_count;

        if (k_COMPLETION_COUNT == data->d_count) {
            s_continue = 0;
        }

        bslmt::ThreadUtil::yield();
    }

    return 0;
}

extern "C" void *starvationReadLockCount(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);

    while (s_continue == 2) {
        data->d_mutex_p->lockRead();
        bslmt::ThreadUtil::yield();
        data->d_mutex_p->unlock();

        ++data->d_count;

        if (k_COMPLETION_COUNT == data->d_count) {
            s_continue = 0;
        }

        bslmt::ThreadUtil::yield();
    }

    return 0;
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Maintaining an Account Balance
///- - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the use of
// 'bslmt::ReaderWriterMutex' to write a thread-safe class, 'my_Account'.  Note
// the typical use of 'mutable' for the lock:
//..
    class my_Account {
        // This 'class' represents a bank account with a single balance.

        // DATA
        bsls::Types::Uint64               d_pennies;  // amount of money in the
                                                      // account

        mutable bslmt::ReaderWriterMutex  d_lock;     // guard access to
                                                      // 'd_account_p'

      public:
        // CREATORS
        my_Account();
            // Create an account with zero balance.

        my_Account(const my_Account& original);
            // Create an account having the value of the specified 'original'
            // account.

        ~my_Account();
            // Destroy this account.

        // MANIPULATORS
        my_Account& operator=(const my_Account& rhs);
            // Atomically assign to this account the value of the specified
            // 'rhs' account, and return a reference to this modifiable
            // account.  Note that this operation is thread-safe; no 'lock' is
            // needed.

        void deposit(bsls::Types::Uint64 pennies);
            // Atomically deposit the specified 'pennies' into this account.
            // Note that this operation is thread-safe; no 'lock' is needed.

        int withdraw(bsls::Types::Uint64 pennies);
            // Attempt to atomically withdraw the specified 'pennies' from this
            // account.  Return 0 on success and update this account to reflect
            // the withdrawal.  Otherwise, return a non-zero value and do not
            // update the balance of this account.  Note that this operation is
            // thread-safe; no 'lock' is needed.

        // ACCESSORS
        bsls::Types::Uint64 balanceInPennies() const;
            // Atomically return the number of pennies that are available for
            // withdrawal from this account.
    };

    // CREATORS
    my_Account::my_Account()
    : d_pennies(0)
    {
    }

    my_Account::my_Account(const my_Account& original)
    : d_pennies(original.balanceInPennies())
    {
    }

    my_Account::~my_Account()
    {
    }

    // MANIPULATORS
    my_Account& my_Account::operator=(const my_Account& rhs)
    {
//..
// Where appropriate, clients should use a lock-guard to ensure that an
// acquired mutex is always properly released, even if an exception is thrown.
//..
        d_lock.lockWrite();
        d_pennies = rhs.balanceInPennies();
        d_lock.unlockWrite();
        return *this;
    }

    void my_Account::deposit(bsls::Types::Uint64 pennies)
    {
        d_lock.lockWrite();
        d_pennies += pennies;
        d_lock.unlockWrite();
    }

    int my_Account::withdraw(bsls::Types::Uint64 pennies)
    {
        int rv = 0;

        d_lock.lockWrite();

        if (pennies <= d_pennies) {
            d_pennies -= pennies;
        }
        else {
            rv = 1;
        }

        d_lock.unlockWrite();

        return rv;
    }

    // ACCESSORS
    bsls::Types::Uint64 my_Account::balanceInPennies() const
    {
        d_lock.lockRead();
        bsls::Types::Uint64 rv = d_pennies;
        d_lock.unlockRead();
        return rv;
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// The atomic 'my_Account' methods are used as expected:
//..
    my_Account account;

    account.deposit(10050);
    ASSERT(10050 == account.balanceInPennies());

    bsls::Types::Uint64 paycheckInPennies = 5025;

    account.deposit(paycheckInPennies);
    ASSERT(15075 == account.balanceInPennies());
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // WRITER BIAS
        //   This case verifies the lock is biased torwards writers.
        //
        // Concerns:
        //: 1 The lock is writer biased.
        //
        // Plan:
        //: 1 Create one writer and a number of reader threads that count the
        //:   number of times they are able to obtain the lock.  Evaluate the
        //:   resultant counts to ensure the lock exhibits a writer bias.
        //:   (C-1)
        //
        // Testing:
        //   WRITER BIAS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "WRITER BIAS" << endl
                          << "===========" << endl;

        const int numReaders = 10;

        ThreadData              writer;
        bsl::vector<ThreadData> reader(numReaders);

        Obj obj;

        s_continue = 2;

        for (int nr = 0; nr < numReaders; ++nr) {
            reader[nr].d_mutex_p = &obj;
            bslmt::ThreadUtil::create(&reader[nr].d_handle,
                                      starvationReadLockCount,
                                      &reader[nr]);
        }
        {
            writer.d_mutex_p = &obj;
            bslmt::ThreadUtil::create(&writer.d_handle,
                                      writeLockCount,
                                      &writer);
        }

        {
            bslmt::ThreadUtil::join(writer.d_handle);
        }

        for (int i = 0; i < numReaders; ++i) {
            bslmt::ThreadUtil::join(reader[i].d_handle);
        }

        // For a reader-preffering lock, the above test will result in,
        // typically, 'writer.d_count < k_COMPLETION_COUNT / 10000'.  To avoid
        // potential intermittant failures, the threshold will be set (only) a
        // hundred times higher than this measure.

        ASSERTV(writer.d_count, writer.d_count >= k_COMPLETION_COUNT / 100);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS AND MANIPULATORS
        //   This case verifies the forwarding to the implementation class.
        //
        // Concerns:
        //: 1 The methods function as expected.
        //
        // Plan:
        //: 1 Use multiple threads to distinguish the behavior of each method
        //:   and hence validate the forwarding.  (C-1)
        //
        // Testing:
        //   ReaderWriterMutex();
        //   ~ReaderWriterMutex();
        //   void lockRead();
        //   void lockWrite();
        //   int tryLockRead();
        //   int tryLockWrite();
        //   void unlock();
        //   void unlockRead();
        //   void unlockWrite();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CREATORS AND MANIPULATORS" << endl
                          << "=========================" << endl;

        {
            Obj                       obj;
            bslmt::ThreadUtil::Handle wd;
            ThreadData                t(&obj);

            s_continue = 2;

            bslmt::ThreadUtil::create(&wd,
                                      watchdog,
                                      const_cast<char *>("readLock"));
            bslmt::ThreadUtil::create(&t.d_handle, readLock, &t);

            t.d_step.post();
            t.d_stepDone.wait();

            obj.lockRead();
            obj.unlockRead();

            ASSERT(0 == obj.tryLockRead());
            obj.unlockRead();

            ASSERT(1 == obj.tryLockWrite());

            s_continue = 1;

            t.d_step.post();
            t.d_stepDone.wait();

            ASSERT(0 == obj.tryLockRead());
            obj.unlockRead();

            ASSERT(0 == obj.tryLockWrite());
            obj.unlockWrite();

            bslmt::ThreadUtil::join(t.d_handle);

            s_continue = 0;

            bslmt::ThreadUtil::join(wd);
        }

        {
            Obj                       obj;
            bslmt::ThreadUtil::Handle wd;
            ThreadData                t(&obj);

            s_continue = 2;

            bslmt::ThreadUtil::create(&wd,
                                      watchdog,
                                      const_cast<char *>("writeLock"));
            bslmt::ThreadUtil::create(&t.d_handle, writeLock, &t);

            t.d_step.post();
            t.d_stepDone.wait();

            ASSERT(1 == obj.tryLockRead());
            ASSERT(1 == obj.tryLockWrite());

            s_continue = 1;

            t.d_step.post();
            t.d_stepDone.wait();

            ASSERT(0 == obj.tryLockRead());
            obj.unlock();  // NOTE: not 'unlockRead'

            ASSERT(0 == obj.tryLockWrite());
            obj.unlockWrite();

            bslmt::ThreadUtil::join(t.d_handle);

            s_continue = 0;

            bslmt::ThreadUtil::join(wd);
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

        Obj obj;

        obj.lockRead();
        obj.unlock();

        obj.lockWrite();
        obj.unlock();
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
