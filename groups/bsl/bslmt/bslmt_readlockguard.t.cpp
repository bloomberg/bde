// bslmt_readlockguard.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_readlockguard.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>
#include <bsl_cstring.h>  // 'strcmp'
#include <bsl_cstdlib.h>  // 'atoi'

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
// This program tests the functionality of the 'bslmt::ReadLockGuard' class.
// It verifies that the class properly locks the synchronization object for
// read at construction time, and that it properly unlocks the object at
// destruction time.  A helper class, 'my_RWLock', is created to facilitate the
// test process.  'my_RWLock' implements the required lock and unlock
// interfaces and provides a means to determine when the functions are called.
//
//-----------------------------------------------------------------------------
// bslmt::ReadLockGuard
// ============================================================================
// [2] bslmt::ReadLockGuard();
// [2] ~bslmt::ReadLockGuard();
// [2] bslmt::ReadLockGuard::release();
// [2] bslmt::ReadLockGuard::ptr();
//
// bslmt::ReadLockGuardUnlock
// ============================================================================
// [3] bslmt::ReadLockGuardUnlock();
// [3] ~bslmt::ReadLockGuardUnlock();
// [3] bslmt::ReadLockGuardUnlock::release();
// [3] bslmt::ReadLockGuardUnlock::ptr();
//
// bslmt::ReadLockGuardTryLock
// ============================================================================
// [4] bslmt::ReadLockGuardTryLock();
// [4] ~bslmt::ReadLockGuardTryLock();
// [4] bslmt::ReadLockGuardTryLock::release();
// [4] bslmt::ReadLockGuardTryLock::ptr();
//-----------------------------------------------------------------------------
// [1] Ensure helper class 'my_RWLock' works as expected
// [5] INTERACTION BET. 'bslmt::ReadLockGuard' AND 'bslmt::ReadLockGuardUnlock'
// [6] DEPRECATED 'bslmt::LockReadGuard'
// [7] USAGE EXAMPLES

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

int someCondition = 0;
int someUpgradeCondition = 0;
int someOtherCondition = 0;

struct my_RWLock {
    // This class provides a simulated mutual exclusion mechanism which
    // conforms to the interface required by the 'bslmt::ReadLockGuard' class.
    // It operates using a counter to track the symbolic "locked" state.  Each
    // call to the lock and unlock functions increment or decrement the lock
    // count respectively.  The current state of the lock count is accessible
    // through the lockCount method.  The 'tryLock' is designed to fail the
    // first time, then succeed every other time.

    int d_count;
    int d_attempt;

    my_RWLock() : d_count(0), d_attempt(1)  {}
    ~my_RWLock() {};
    int lockCount() const { return d_count; }
    int tryLockRead() {
        if ((++d_attempt)%2) {
            lockRead();
        } else {
            return 1;                                                 // RETURN
        }
        return 0;
    }
    void upgradeToWriteLock() {}
    void lockRead() { ++d_count; }
    void lockReadReserveWrite() { ++d_count; }
    void unlock() { --d_count; }
};

struct my_Object {
    void someMethod() const {};
    void someOtherMethod() const {};
    void someUpgradeMethod() {};
    void defaultMethod() const {};
};

///Usage
///-----
// Use this component to ensure that in the event of an exception or exit from
// any point in a given scope, the synchronization object will be properly
// unlocked.  The following function, 'errorProneFunc', is overly complex, not
// exception safe, and contains a bug.
//..
    static void errorProneFunc(const my_Object *obj, my_RWLock *rwlock)
    {
        rwlock->lockRead();
        if (someCondition) {
            obj->someMethod();
            rwlock->unlock();
            return;                                                   // RETURN
        } else if (someOtherCondition) {
            obj->someOtherMethod();
            // MISTAKE! forgot to unlock rwlock
            return;                                                   // RETURN
        }
        obj->defaultMethod();
        rwlock->unlock();
        return;
    }
//..
// The function can be rewritten with a cleaner and safer implementation using
// a guard object.  The 'safeFunc' function is simpler than 'errorProneFunc',
// is exception safe, and avoids the multiple calls to unlock that can be a
// source of errors.
//..
    static void safeFunc(const my_Object *obj, my_RWLock *rwlock)
    {
        bslmt::ReadLockGuard<my_RWLock> guard(rwlock);
        if (someCondition) {
            obj->someMethod();
            return;                                                   // RETURN
        } else if (someOtherCondition) {
            obj->someOtherMethod();
            // OK, rwlock is automatically unlocked
            return;                                                   // RETURN
        }
        obj->defaultMethod();
        return;
    }
//..
// When blocking while acquiring the lock is not desirable, one may instead use
// a 'bslmt::ReadLockGuardTryLock' in the typical following fashion:
//..
    static int safeButNonBlockingFunc(const my_Object *obj, my_RWLock *rwlock)
        // Perform task and return positive value if locking succeeds.  Return
        // 0 if locking fails.
    {
        const int RETRIES = 1; // use higher values for higher success rate
        bslmt::ReadLockGuardTryLock<my_RWLock> guard(rwlock, RETRIES);
        if (guard.ptr()) { // rwlock is locked
            if (someCondition) {
                obj->someMethod();
                return 2;                                             // RETURN
            } else if (someOtherCondition) {
                obj->someOtherMethod();
                return 3;                                             // RETURN
            }
            obj->defaultMethod();
            return 1;                                                 // RETURN
        }
        return 0;
    }
//..
// If the underlying lock object provides an upgrade to a lock for write (as
// does 'bslmt::ReaderWriterLock' with the 'upgradeToWriteLock' function, for
// example), this can be safely used in conjunction with
// 'bslmt::ReadLockGuard', as long as the same 'unlock' method is used to
// release both kinds of locks.  The following method illustrates this usage:
//..
    static void safeUpdateFunc(my_Object *obj, my_RWLock *rwlock)
    {
        const my_Object *constObj = obj;
        bslmt::ReadLockGuard<my_RWLock> guard(rwlock);
        if (someUpgradeCondition) {
            rwlock->upgradeToWriteLock();
            obj->someUpgradeMethod();
            return;                                                   // RETURN
        } else if (someOtherCondition) {
            constObj->someOtherMethod();
            // OK, rwlock is automatically unlocked
            return;                                                   // RETURN
        }
        constObj->defaultMethod();
        return;
    }
//..
// In the above code, the call to 'upgradeToWriteLock' is not necessarily
// atomic, as the upgrade may release the lock for read and be interrupted
// before getting a lock for write.  It is possible to guarantee atomicity (as
// does 'bslmt::ReaderWriterLock' if the 'lockReadReserveWrite' function is
// used instead of 'lockRead', for example), but the standard constructor
// should not be used.  Instead, the 'lockReadReserveWrite' lock function
// should be used explicitly, and the guard constructed with an object which is
// already locked.  The following method illustrates this usage:
//..
    static void safeAtomicUpdateFunc(my_Object *obj, my_RWLock *rwlock)
    {
        const my_Object *constObj = obj;
        rwlock->lockReadReserveWrite();
        const int PRELOCKED = 1;
        bslmt::ReadLockGuard<my_RWLock> guard(rwlock, PRELOCKED);
        if (someUpgradeCondition) {
            rwlock->upgradeToWriteLock();
            obj->someUpgradeMethod();
            return;                                                   // RETURN
        } else if (someOtherCondition) {
            constObj->someOtherMethod();
            return;                                                   // RETURN
        }
        constObj->defaultMethod();
        return;
    }
//..
// Note that in the code above, the function 'rwlock->lockRead()' is never
// called, but is nevertheless required for the code to compile.
//
// Instantiations of 'bslmt::ReadLockGuardUnlock' can be interleaved with
// instantiations of 'bslmt::ReadLockGuard' to create both critical sections
// and regions where the lock is released.
//..
    void f(my_RWLock *rwlock)
    {
        bslmt::ReadLockGuard<my_RWLock> guard(rwlock);

        // critical section here

        {
            bslmt::ReadLockGuardUnlock<my_RWLock> guard(rwlock);

            // rwlock is unlocked here

        } // rwlock is locked again here

        // critical section here

    } // rwlock is unlocked here
//..
// Care must be taken so as not to interleave guard objects in such a way as to
// cause an illegal sequence of calls on a lock (two sequential lock calls or
// two sequential unlock calls on a non-recursive read/write lock).

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concern:
        //   That the example in the component-level documentation compiles
        //   and works as expected.
        //
        // Plan:
        //   Run the example for all combinations of external conditions, make
        //   sure the bug is detected if the buggy branch is taken, and that
        //   the lock is released in all other cases.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tUsage Example" << endl
                                  << "\t=============" << endl;

        my_Object obj;
        my_RWLock rwlock;

        for (someCondition = 0; someCondition < 2; ++someCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_RWLock rwlock;
                errorProneFunc(&obj, &rwlock);
                if (0 == someCondition && 1 == someOtherCondition) {
                    ASSERT( 0 < rwlock.lockCount() );
                } else  {
                    ASSERT( 0 == rwlock.lockCount() );
                }
            }
        }

        for (someCondition = 0; someCondition < 2; ++someCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_RWLock rwlock;
                safeFunc(&obj, &rwlock);
                ASSERT( 0 == rwlock.lockCount() );
            }
        }

        for (someCondition = 0; someCondition < 2; ++someCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_RWLock rwlock;
                // rwlock.tryLockRead() will fail the first time
                ASSERT( 0 == safeButNonBlockingFunc(&obj, &rwlock) );
                ASSERT( 0 == rwlock.lockCount() );
                // but succeed the second time
                ASSERT( 0 < safeButNonBlockingFunc(&obj, &rwlock) );
                ASSERT( 0 == rwlock.lockCount() );
            }
        }

        for (someUpgradeCondition = 0; someUpgradeCondition < 2;
                ++someUpgradeCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_RWLock rwlock;
                safeUpdateFunc(&obj, &rwlock);
                ASSERT( 0 == rwlock.lockCount() );
            }
        }

        for (someCondition = 0; someCondition < 2; ++someCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_RWLock rwlock;
                f(&rwlock);
                ASSERT( 0 == rwlock.lockCount() );
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // DEPRECATED CLASSES
        //
        // Concern:
        //   Backwards compatibility: ensure that the 'bslmt::LockReadGuard'
        //   class exists and still works.
        //
        // Plan:
        //   Copy case 2, but use the old deprecated name.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting deprecated classes" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t lock guard constructor & destructor" <<endl;
            {
                bslmt::LockReadGuard<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bslmt::LockReadGuard<my_RWLock> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                        const int PRE_LOCKED = 1; mX.lockRead();
                        bslmt::LockReadGuard<my_RWLock> l3(&mX, PRE_LOCKED);
                        if(verbose)  P(X.lockCount());
                        ASSERT(3 == X.lockCount());
                    }
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                }
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
            }
            ASSERT(0 == X.lockCount());

            if (verbose)
                cout << "\t\t lock guard release and ptr access" << endl;
            {
                bslmt::LockReadGuard<my_RWLock> l1(&mX);
                ASSERT(&mX == l1.ptr());

                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                my_RWLock *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
            }
            ASSERT(1 == mX.lockCount());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // INTERACTION BETWEEN 'bslmt::ReadLockGuard' AND
        // 'bslmt::ReadLockGuardUnlock'
        //
        // Concern:
        //   That 'bslmt::ReadLockGuard' and 'bslmt::ReadLockGuardUnlock'
        //   interact together as expected.  That two different lock guards on
        //   two different 'my_RWLock' objects do not interfere with each
        //   other.
        //
        // Plan:
        //   We verify that using two independent 'my_RWLock' objects with two
        //   distinct 'bslmt::ReadLockGuard' *and* 'bslmt::ReadLockGuardUnlock'
        //   objects in the same scope have no effect on each other.
        //
        // Testing:
        //   Interaction between lock and unlock guards, as well as
        //   between two lock guards on two different 'my_RWLock' objects.
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\tTesting interaction between "
                    "'bslmt::ReadLockGuard' and bslmt::ReadLockGuardUnlock'"
                 << endl
                 << "=============================="
                    "========================================"
                 << endl;

        {
            my_RWLock mX1;    const my_RWLock &X1 = mX1;
            my_RWLock mX2;    const my_RWLock &X2 = mX2;

            mX1.lockRead();
            {
                bslmt::ReadLockGuard<my_RWLock> l1(&mX1);
                bslmt::ReadLockGuard<my_RWLock> l2(&mX2);
                ASSERT(2 == X1.lockCount());
                ASSERT(1 == X2.lockCount());

                {
                    bslmt::ReadLockGuardUnlock<my_RWLock> u1(&mX1);
                    bslmt::ReadLockGuardUnlock<my_RWLock> u2(&mX2);
                    ASSERT(1 == X1.lockCount());
                    ASSERT(0 == X2.lockCount());
                }
                ASSERT(2 == X1.lockCount());
                ASSERT(1 == X2.lockCount());
            }
            ASSERT(1 == X1.lockCount());
            ASSERT(0 == X2.lockCount());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bslmt::ReadLockGuardTryLock'
        //
        // Concern:
        //   That the basic functionality of the 'bslmt::ReadLockGuardTryLock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested
        //   'bslmt::ReadLockGuardTryLock' objects using a common 'my_RWLock'
        //   object.  With each new object we verify that the lock function is
        //   called only if the constructor succeeds.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bslmt::ReadLockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied
        //   only if the constructor succeeded in acquiring the lock.  We then
        //   verify that 'release' makes no attempt to unlock the supplied
        //   object and that when the 'bslmt::ReadLockGuardTryLock' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bslmt::ReadLockGuardTryLock' can be
        //   created with a null lock, and that 'release' may be called on the
        //   guard.
        //
        // Testing:
        //   bslmt::ReadLockGuardTryLock();
        //   ~bslmt::ReadLockGuardTryLock();
        //   bslmt::ReadLockGuardTryLock::release();
        //   bslmt::ReadLockGuardTryLock::ptr();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "\tTesting: \n"
                 << "\t  bslmt::ReadLockGuardTryLock(); \n"
                 << "\t  ~bslmt::ReadLockGuardTryLock(); \n"
                 << "\t  bslmt::ReadLockGuardTryLock::release(); \n"
                 << "\t  bslmt::ReadLockGuardTryLock::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t trylock guard constructors & destructor" <<endl;
            {
                bslmt::ReadLockGuardTryLock<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
                ASSERT(0 == l1.ptr());
                {
                    bslmt::ReadLockGuardTryLock<my_RWLock> l2(&mX);
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                    {
                        bslmt::ReadLockGuardTryLock<my_RWLock> l3(&mX, 10);
                        if(verbose)  P(X.lockCount());
                        ASSERT(2 == X.lockCount());
                    }
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                }
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
            }
            ASSERT(0 == X.lockCount());

            if (verbose)
                cout << "\t\t trylock guard release and ptr access" << endl;
            {
                bslmt::ReadLockGuardTryLock<my_RWLock> l1(&mX, 2);

                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                my_RWLock *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
            }
            ASSERT(1 == mX.lockCount());
        }

        {
            bslmt::ReadLockGuardTryLock<my_RWLock> mX(0, 6);
        }
        {
            bslmt::ReadLockGuardTryLock<my_RWLock> mX(0, 6);
            mX.release();
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bslmt::ReadLockGuardUnlock'
        //
        // Concern:
        //   That the basic functionality of the 'bslmt::ReadLockGuardUnlock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested
        //   'bslmt::ReadLockGuardUnlock' objects using a common 'my_RWLock'
        //   object.  With each new object we verify that the unlock function
        //   is called.  As each object is destroyed, we verify that the lock
        //   function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bslmt::ReadLockGuardUnlock' and calling
        //   'release'.  We verify that the returned pointer matches the value
        //   we supplied.  We then verify that 'release' makes no attempt to
        //   unlock the supplied object and that when the
        //   'bslmt::ReadLockGuardUnlock' object is destroyed, it does not
        //   unlock the object.
        //
        //   Finally we test that a 'bslmt::ReadLockGuardUnlock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bslmt::ReadLockGuardUnlock();
        //   ~bslmt::ReadLockGuardUnlock();
        //   bslmt::ReadLockGuardUnlock::release();
        //   bslmt::ReadLockGuardUnlock::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bslmt::ReadLockGuardUnlock(); \n"
                          << "\t  ~bslmt::ReadLockGuardUnlock(); \n"
                          << "\t  bslmt::ReadLockGuardUnlock::release(); \n"
                          << "\t  bslmt::ReadLockGuardUnlock::ptr(); \n"
                          << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t unlock guard constructor & destructor" <<endl;
            {
                bslmt::ReadLockGuardUnlock<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                {
                    const int PRE_UNLOCKED = 0;
                    bslmt::ReadLockGuardUnlock<my_RWLock> l2(
                                                            &mX, PRE_UNLOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                    {
                        const int PRE_UNLOCKED = 1; mX.unlock();
                        bslmt::ReadLockGuardUnlock<my_RWLock> l3(&mX,
                                                                PRE_UNLOCKED);
                        if(verbose)  P(X.lockCount());
                        ASSERT(-3 == X.lockCount());
                    }
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                }
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
            }
            ASSERT(0 == X.lockCount());

            if (verbose)
                cout << "\t\t unlock guard release and ptr access" << endl;
            {
                bslmt::ReadLockGuardUnlock<my_RWLock> l1(&mX);
                ASSERT(&mX == l1.ptr());

                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                my_RWLock *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
            }
            ASSERT(-1 == mX.lockCount());
        }

        {
            bslmt::ReadLockGuardUnlock<my_RWLock> mX(0);
        }
        {
            bslmt::ReadLockGuardUnlock<my_RWLock> mX(0);
            mX.release();
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bslmt::ReadLockGuard'
        //
        // Concern:
        //   That the basic functionality of the 'bslmt::ReadLockGuard' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bslmt::ReadLockGuard'
        //   objects using a common 'my_RWLock' object.  With each new object
        //   we verify the the lock function was called.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bslmt::ReadLockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied.
        //   We then verify that 'release' makes no attempt to unlock the
        //   supplied object and that when the 'bslmt::ReadLockGuard' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bslmt::ReadLockGuard' can be created with
        //   a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bslmt::ReadLockGuard();
        //   ~bslmt::ReadLockGuard();
        //   bslmt::ReadLockGuard::release();
        //   bslmt::ReadLockGuard::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bslmt::ReadLockGuard(); \n"
                          << "\t  ~bslmt::ReadLockGuard(); \n"
                          << "\t  bslmt::ReadLockGuard::release(); \n"
                          << "\t  bslmt::ReadLockGuard::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t lock guard constructor & destructor" <<endl;
            {
                bslmt::ReadLockGuard<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bslmt::ReadLockGuard<my_RWLock> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                    const int PRE_LOCKED = 1; mX.lockRead();
                        bslmt::ReadLockGuard<my_RWLock> l3(&mX, PRE_LOCKED);
                        if(verbose)  P(X.lockCount());
                        ASSERT(3 == X.lockCount());
                    }
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                }
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
            }
            ASSERT(0 == X.lockCount());

            if (verbose)
                cout << "\t\t lock guard release and ptr access" << endl;
            {
                bslmt::ReadLockGuard<my_RWLock> l1(&mX);
                ASSERT(&mX == l1.ptr());

                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                my_RWLock *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
            }
            ASSERT(1 == mX.lockCount());
        }

        {
            bslmt::ReadLockGuard<my_RWLock> mX(0);
        }
        {
            bslmt::ReadLockGuard<my_RWLock> mX(0);
            mX.release();
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //   The 'my_RWLock' class is a simple type that supports the lock and
        //   unlock methods used by the guard class.  These methods simply
        //   increment and decrement a count integer data member.
        //
        // Testing:
        //   my_RWLock();
        //   ~my_RWLock();
        //   my_RWLock::lockRead();
        //   my_RWLock::tryLockRead();
        //   my_RWLock::unlock();
        //   my_RWLock::lockCount() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASSES TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "Testing my_RWLock." << endl;

        my_RWLock mX;    const my_RWLock &X = mX;
        ASSERT(0 == X.lockCount());

        mX.lockRead();
        ASSERT(1 == X.lockCount());

        mX.unlock();
        ASSERT(0 == X.lockCount());

        mX.lockRead();
        mX.lockRead();
        ASSERT(2 == X.lockCount());
        mX.unlock();
        ASSERT(1 == X.lockCount());
        mX.lockRead();
        mX.lockRead();
        ASSERT(3 == X.lockCount());
        mX.unlock();
        mX.unlock();
        mX.unlock();
        ASSERT(0 == X.lockCount());

        mX.tryLockRead();
        ASSERT(0 == X.lockCount());
        mX.tryLockRead();
        ASSERT(1 == X.lockCount());
        mX.unlock();
        ASSERT(0 == X.lockCount());
        mX.tryLockRead();
        mX.tryLockRead();
        mX.tryLockRead();
        mX.tryLockRead();
        ASSERT(2 == X.lockCount());
        mX.unlock();
        mX.unlock();
        ASSERT(0 == X.lockCount());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    return( testStatus );
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
