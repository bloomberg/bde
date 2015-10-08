// bslmt_mutex.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_mutex.h>

#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_map.h>

#include <bsls_atomic.h>

using namespace BloombergLP;
using namespace bsl;

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

///Usage
///-----
// The following snippets of code illustrate the use of 'bslmt::Mutex' to write
// a thread-safe class, 'my_SafeAccount', given a thread-unsafe class,
// 'my_Account'.  The simple 'my_Account' class is defined as follows:
//..
    class my_Account {
        // This 'class' represents a bank account with a single balance.  It
        // is not thread-safe.

        // DATA
        double d_money;  // amount of money in the account

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
            // Assign to this account the value of the specified 'rhs' account,
            // and return a reference to this modifiable account.

        void deposit(double amount);
            // Deposit the specified 'amount' of money into this account.

        void withdraw(double amount);
            // Withdraw the specified 'amount' of money from this account.

        // ACCESSORS
        double balance() const;
            // Return the amount of money that is available for withdrawal
            // from this account.
    };

    // CREATORS
    my_Account::my_Account()
    : d_money(0.0)
    {
    }

    my_Account::my_Account(const my_Account& original)
    : d_money(original.d_money)
    {
    }

    my_Account::~my_Account()
    {
    }

    // MANIPULATORS
    my_Account& my_Account::operator=(const my_Account& rhs)
    {
        d_money = rhs.d_money;
        return *this;
    }

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
//..
// Next, we use a 'bslmt::Mutex' object to render atomic the function calls of
// a new thread-safe class that uses the thread-unsafe class in its
// implementation.  Note the typical use of 'mutable' for the lock:
//..
    class my_SafeAccountHandle {
        // This 'class' provides a thread-safe handle to an account (held, not
        // owned) passed at construction.

        // DATA
        my_Account          *d_account_p;  // held, not owned
        mutable bslmt::Mutex  d_lock;       // guard access to 'd_account_p'

        // NOT IMPLEMENTED
        my_SafeAccountHandle(const my_SafeAccountHandle&);
        my_SafeAccountHandle& operator=(const my_SafeAccountHandle&);

      public:
        // CREATORS
        my_SafeAccountHandle(my_Account *account);
            // Create a thread-safe handle to the specified 'account'.

        ~my_SafeAccountHandle();
            // Destroy this handle.  Note that the held account is unaffected
            // by this operation.

        // MANIPULATORS
        void deposit(double amount);
            // Atomically deposit the specified 'amount' of money into the
            // account held by this handle.  Note that this operation is
            // thread-safe; no 'lock' is needed.

        void lock();
            // Provide exclusive access to the underlying account held by this
            // object.

        void unlock();
            // Release exclusivity of the access to the underlying account held
            // by this object.

        void withdraw(double amount);
            // Atomically withdraw the specified 'amount' of money from the
            // account held by this handle.  Note that this operation is
            // thread-safe; no 'lock' is needed.

        // ACCESSORS
        my_Account *account() const;
            // Return the address of the modifiable account held by this
            // handle.

        double balance() const;
            // Atomically return the amount of money that is available for
            // withdrawal from the account held by this handle.
    };
//..
// The implementation show-casing the use of 'bslmt::Mutex' follows:
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
    void my_SafeAccountHandle::deposit(double amount)
    {
//..
// Where appropriate, clients should use a lock-guard to ensure that an
// acquired mutex is always properly released, even if an exception is thrown.
// See 'bslmt_lockguard' for more information:
//..
        d_lock.lock();  // consider using 'bslmt::LockGuard'
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

    void my_SafeAccountHandle::withdraw(double amount)
    {
        d_lock.lock();  // consider using 'bslmt::LockGuard'
        d_account_p->withdraw(amount);
        d_lock.unlock();
    }

    // ACCESSORS
    my_Account *my_SafeAccountHandle::account() const
    {
        return d_account_p;
    }

    double my_SafeAccountHandle::balance() const
    {
        d_lock.lock();  // consider using 'bslmt::LockGuard'
        const double res = d_account_p->balance();
        d_lock.unlock();
        return res;
    }
//..

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bslmt::Mutex Obj;

class ZeroInt {
    int d_i;

  public:
    ZeroInt() : d_i(0) {}
    ZeroInt& operator=(int i) { d_i = i; return *this; }
    ZeroInt& operator++() { ++d_i; return *this; }
    operator int() const { return d_i; }
};

struct ZeroIntMap : bsl::map<int, ZeroInt> {
    int sum() const
    {
        const const_iterator b = begin();
        const const_iterator e = end();

        int ret = 0;
        for (const_iterator it = b; e != it; ++it) {
            ret += it->second;
        }

        return ret;
    }
};

bsl::ostream& operator<<(bsl::ostream& stream, const ZeroIntMap& thismap)
{
    const ZeroIntMap::const_iterator b = thismap.begin();
    const ZeroIntMap::const_iterator e = thismap.end();

    stream << "Sum:" << thismap.sum();

    bool first_time = true;
    for (ZeroIntMap::const_iterator it = b; e != it; ++it) {
        if (first_time) {
            first_time = false;
        }
        else {
            stream << ',';
        }
        stream << ' ';
        stream << it->first << ':' << (int) it->second;
    }

    return stream;
}

                                // ------
                                // case 1
                                // ------
struct ThreadInfo {
    Obj *d_lock;
    bsls::AtomicInt d_retval;
    bsls::AtomicInt d_retvalSet;
};

extern "C" void* MyThread(void* arg_p)
{
    ThreadInfo* arg = (ThreadInfo*)arg_p;

    arg->d_retval = arg->d_lock->tryLock();
    if (0 == arg->d_retval) {
        arg->d_lock->unlock();
    }
    arg->d_retvalSet = 1;
    return arg_p;
}

                                 // -------
                                 // case -1
                                 // -------

namespace BSLMT_MUTEX_CASE_MINUS_1 {

    enum { k_NUM_NOT_URGENT_THREADS = 128,
           k_NUM_THREADS            = k_NUM_NOT_URGENT_THREADS + 1 };

int translatePriority(bslmt::ThreadAttributes::SchedulingPolicy policy,
                      bool                                      low)
{
    if (low) {
        return bslmt::ThreadUtil::getMinSchedulingPriority(policy);   // RETURN
    }
    else {
        return bslmt::ThreadUtil::getMaxSchedulingPriority(policy);   // RETURN
    }
}

struct F {
    bool                  d_urgent;
    static int            s_urgentPlace;
    static bool           s_firstThread;
    static bsls::AtomicInt s_lockCount;
    static bsls::AtomicInt s_finished;
    static bslmt::Mutex    s_mutex;

    // CREATORS
    F() : d_urgent(false) {}

    // ACCESSORS
    void operator()();
};
int            F::s_urgentPlace;
bool           F::s_firstThread = 1;
bsls::AtomicInt F::s_finished(0);
bsls::AtomicInt F::s_lockCount(0);
bslmt::Mutex    F::s_mutex;

void F::operator()()
{
    enum { k_LIMIT = 10 * 1024 };

    for (int i = 0; i < k_LIMIT; ++i) {
        ++s_lockCount;
        s_mutex.lock();
        if (s_firstThread) {
            s_firstThread = false;

            // Careful!  This could take 2 seconds to wake up!

            bslmt::ThreadUtil::microSleep(200 * 1000);
            ASSERT(k_NUM_THREADS == s_lockCount);
        }
        s_mutex.unlock();
        --s_lockCount;
    }

    if (d_urgent) {
        s_urgentPlace = s_finished;
    }
    ++s_finished;
}

}  // close namespace BSLMT_MUTEX_CASE_MINUS_1

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
      case 2: {
// The handle's atomic methods are used just as the corresponding methods in
// 'my_Account':
//..
    my_Account account;
    account.deposit(100.50);
    double paycheck = 50.25;
    my_SafeAccountHandle handle(&account);

                               ASSERT(100.50 == handle.balance());
    handle.deposit(paycheck);  ASSERT(150.75 == handle.balance());
//..
// We can also use the handle's 'lock' and 'unlock' methods to implement
// non-primitive atomic transactions on the account:
//..
    double check[5] = { 25.0, 100.0, 99.95, 75.0, 50.0 };

    handle.lock();  // consider using 'bslmt::LockGuard'

    double originalBalance = handle.account()->balance();
    for (int i = 0; i < 5; ++i) {
        handle.account()->deposit(check[i]);
    }
    ASSERT(originalBalance + 349.95 == handle.account()->balance());
    handle.unlock();
//..
      } break;
      case 1: {
        // ------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy a mutex.  Lock and verify that tryLock fails;
        // unlock and verify that tryLock succeeds.
        // ------------------------------------------------------------------
        if (verbose) {
            cout << "BREATHING TEST" << endl;
            cout << "==============" << endl;
        }
        enum {
            k_MAX_SLEEP_CYCLES = 1000,
            k_SLEEP_MS = 100
        };

        {
            Obj lock;
            lock.lock();

            ThreadInfo args;
            args.d_lock = &lock;

            args.d_retval = 0;
            args.d_retvalSet = 0;
            bslmt::ThreadAttributes attr;
            attr.setDetachedState(
                               bslmt::ThreadAttributes::e_CREATE_DETACHED);
            bslmt::ThreadUtil::Handle dum;
            bslmt::ThreadUtil::create(&dum, attr, &MyThread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < k_MAX_SLEEP_CYCLES;
                 ++i) {
                bslmt::ThreadUtil::microSleep(1000 * k_SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail
            if (verbose) {
                P(args.d_retval);
            }

            lock.unlock();

            args.d_retval = 0;
            args.d_retvalSet = 0;
            bslmt::ThreadUtil::create(&dum, attr, &MyThread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < k_MAX_SLEEP_CYCLES;
                 ++i) {
                bslmt::ThreadUtil::microSleep(1000 * k_SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 == args.d_retval); // should succeed
            if (verbose) {
                P(args.d_retval);
            }

        }
      } break;
      case -1: {
        // ------------------------------------------------------------------
        // Testing priorities on mutex heavy tasks.
        // ------------------------------------------------------------------

        namespace TC = BSLMT_MUTEX_CASE_MINUS_1;

        typedef bslmt::ThreadAttributes::SchedulingPolicy Policy;
        const Policy DF = bslmt::ThreadAttributes::e_SCHED_DEFAULT;
        const Policy SO = bslmt::ThreadAttributes::e_SCHED_OTHER;
        const Policy SF = bslmt::ThreadAttributes::e_SCHED_FIFO;
        const Policy SR = bslmt::ThreadAttributes::e_SCHED_RR;

        ZeroIntMap urgentPlaces[2];

        struct {
            int    d_line;
            Policy d_policy;
            bool   d_urgentLow;
            bool   d_normalizedPriorities;
        } DATA[] = {
            { L_, DF, 0, 0 },
            { L_, DF, 0, 1 },
            { L_, DF, 1, 0 },
            { L_, DF, 1, 1 },
            { L_, SO, 0, 0 },
            { L_, SO, 0, 1 },
            { L_, SO, 1, 0 },
            { L_, SO, 1, 1 },
            { L_, SF, 0, 0 },
            { L_, SF, 0, 1 },
            { L_, SF, 1, 0 },
            { L_, SF, 1, 1 },
            { L_, SR, 0, 0 },
            { L_, SR, 0, 1 },
            { L_, SR, 1, 0 },
            { L_, SR, 1, 1 },
        };

        const int DATA_LEN = static_cast<int>(sizeof(DATA) / sizeof(*DATA));

        for (int i = 0; i < DATA_LEN; ++i) {
            const int    LINE       = DATA[i].d_line;
            const Policy POLICY     = DATA[i].d_policy;
            const int    URGENT_LOW = DATA[i].d_urgentLow;
            const int    NORM_PRI   = DATA[i].d_normalizedPriorities;

            const int    URGENT_PRIORITY =     TC::translatePriority(
                                                                  POLICY,
                                                                  URGENT_LOW);
            const int    NOT_URGENT_PRIORITY = TC::translatePriority(
                                                                  POLICY,
                                                                  !URGENT_LOW);

            const double NORM_URGENT_PRI     = URGENT_LOW ? 0.0 : 1.0;
            const double NORM_NOT_URGENT_PRI = URGENT_LOW ? 1.0 : 0.0;

            if (veryVerbose) {
                if (NORM_PRI) {
                    P_(LINE) P_(POLICY) P(NORM_URGENT_PRI);
                }
                else {
                    P_(LINE) P_(POLICY)
                    P_(URGENT_PRIORITY) P(NOT_URGENT_PRIORITY);
                }
            }

            ASSERT(URGENT_PRIORITY != NOT_URGENT_PRIORITY);

            TC::F::s_urgentPlace = -1;
            TC::F::s_finished = 0;
            TC::F::s_firstThread = true;

            TC::F fs[TC::k_NUM_THREADS];
            bslmt::ThreadUtil::Handle handles[TC::k_NUM_THREADS];

            bslmt::ThreadAttributes notUrgentAttr;
            notUrgentAttr.setStackSize(1024 * 1024);
            notUrgentAttr.setInheritSchedule(0);
            notUrgentAttr.setSchedulingPolicy(POLICY);

            bslmt::ThreadAttributes urgentAttr(notUrgentAttr);

            if (NORM_PRI) {
                notUrgentAttr.setSchedulingPriority(
                                bslmt::ThreadUtil::convertToSchedulingPriority(
                                                 POLICY, NORM_NOT_URGENT_PRI));
                urgentAttr.   setSchedulingPriority(
                                bslmt::ThreadUtil::convertToSchedulingPriority(
                                                 POLICY, NORM_URGENT_PRI));
            }
            else {
                notUrgentAttr.setSchedulingPriority(NOT_URGENT_PRIORITY);
                urgentAttr.   setSchedulingPriority(    URGENT_PRIORITY);
            }

            fs[TC::k_NUM_THREADS - 1].d_urgent = true;

            int rc;
            int numThreads = 0;
            for ( ; numThreads < TC::k_NUM_THREADS; ++numThreads) {
                bslmt::ThreadAttributes *attr
                                    = numThreads < TC::k_NUM_NOT_URGENT_THREADS
                                    ? &notUrgentAttr
                                    : &urgentAttr;
                rc = bslmt::ThreadUtil::create(&handles[numThreads],
                                              *attr,
                                              fs[numThreads]);
                LOOP3_ASSERT(LINE, rc, numThreads, 0 == rc);
                if (rc) {
                    break;
                }
            }

            for (int j = 0; j < numThreads; ++j) {
                rc = bslmt::ThreadUtil::join(handles[j]);
                LOOP3_ASSERT(LINE, rc, j, 0 == rc);
                if (rc) {
                    break;
                }
            }

            ASSERT(TC::F::s_urgentPlace >= 0);
            ASSERT(TC::F::s_urgentPlace < TC::k_NUM_THREADS);
            ASSERT(!TC::F::s_firstThread);
            ASSERT(TC::k_NUM_THREADS == TC::F::s_finished);

            urgentPlaces[URGENT_LOW][LINE] = TC::F::s_urgentPlace;
        }

        cout << "Urgent low:  " << urgentPlaces[true ] << endl;
        cout << "Urgent high: " << urgentPlaces[false] << endl;
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
