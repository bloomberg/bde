// bslmt_lockguard.t.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_lockguard.h>

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
// This program tests the functionality of the 'bslmt::LockGuard' class.  It
// verifies that the class properly locks the synchronization object at
// construction time, and that it properly unlocks the object at destruction
// time.  A helper class, 'my_Mutex', is created to facilitate the test
// process.  'my_Mutex' implements the required lock and unlock interfaces and
// provides a means to determine when the functions are called.
//
//-----------------------------------------------------------------------------
// bslmt::LockGuard
// ============================================================================
// [2] bslmt::LockGuard();
// [2] ~bslmt::LockGuard();
// [2] release();
//
// bslmt::LockGuardUnlock
// ============================================================================
// [3] bslmt::LockGuardUnlock();
// [3] ~bslmt::LockGuardUnlock();
// [3] release();

// bslmt::LockGuardTryLock
// ============================================================================
// [4] bslmt::LockGuardTryLock();
// [4] ~bslmt::LockGuardTryLock();
// [4] release();
//-----------------------------------------------------------------------------
// [1] Ensure helper class 'my_Mutex' works as expected
// [5] INTERACTION BETWEEN 'bslmt::LockGuard' AND 'bslmt::LockGuardUnlock'
// [6] DEPRECATED 'bslmt::TryLockGuard' and 'bslmt::UnLockGuard'
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
int someOtherCondition = 0;

struct my_Mutex {
    // This class provides a simulated mutual exclusion mechanism which
    // conforms to the interface required by the bslmt::LockGuard class.  It
    // operates using a counter to track the symbolic "locked" state.  Each
    // call to the lock and unlock functions increment or decrement the lock
    // count respectively.  The current state of the lock count is accessible
    // through the lockCount method.  The 'tryLock' is designed to fail the
    // first time, then succeed every other time.

    int d_count;
    int d_attempt;

    my_Mutex() : d_count(0), d_attempt(1) {}
    ~my_Mutex() {};
    int lockCount() const { return d_count; }

    int tryLock() {
        if ((++d_attempt) % 2) {
            lock();
        }
        else {
            return 1;                                                 // RETURN
        }
        return 0;
    }

    void lock() { ++ d_count; }
    void unlock() { --d_count; }
};

struct my_Object {
    void someMethod() {};
    void someOtherMethod() {};
    void defaultMethod() {};
};

///Usage
///-----
// Use this component to ensure that in the event of an exception or exit from
// any point in a given scope, the synchronization object will be properly
// unlocked.  The following function, 'errorProneFunc', is overly complex, not
// exception safe, and contains a bug.
//..
    static void errorProneFunc(my_Object *obj, my_Mutex *mutex)
    {
        mutex->lock();
        if (someCondition) {
            obj->someMethod();
            mutex->unlock();
            return;                                                   // RETURN
        } else if (someOtherCondition) {
            obj->someOtherMethod();
            // MISTAKE! forgot to unlock mutex
            return;                                                   // RETURN
        }
        obj->defaultMethod();
        mutex->unlock();
        return;
    }
//..
// The function can be rewritten with a cleaner and safer implementation using
// a guard object.  The 'safeFunc' function is simpler than 'errorProneFunc',
// is exception safe, and avoids the multiple calls to unlock that can be a
// source of errors.
//..
    static void safeFunc(my_Object *obj, my_Mutex *mutex)
    {
        bslmt::LockGuard<my_Mutex> guard(mutex);
        if (someCondition) {
            obj->someMethod();
            return;                                                   // RETURN
        } else if (someOtherCondition) {
            obj->someOtherMethod();
            // OK, mutex is automatically unlocked
            return;                                                   // RETURN
        }
        obj->defaultMethod();
        return;
    }
//..
// When blocking while acquiring the lock is not desirable, one may instead use
// a 'bslmt::LockGuardTryLock' in the typical following fashion:
//..
    static int safeButNonBlockingFunc(my_Object *obj, my_Mutex *mutex)
        // Perform task and return positive value if locking succeeds.  Return
        // 0 if locking fails.
    {
        const int RETRIES = 1; // use higher values for higher success rate
        bslmt::LockGuardTryLock<my_Mutex> guard(mutex, RETRIES);
        if (guard.ptr()) { // mutex is locked
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
// Instantiations of 'bslmt::LockGuardUnlock' can be interleaved with
// instantiations of 'bslmt::LockGuard' to create both critical sections and
// regions where the lock is released.
//..
    void f(my_Mutex *mutex)
    {
        bslmt::LockGuard<my_Mutex> guard(mutex);

        // critical section here

        {
            bslmt::LockGuardUnlock<my_Mutex> guard(mutex);

            // mutex is unlocked here

        } // mutex is locked again here

        // critical section here

    } // mutex is unlocked here
//..
// Care must be taken so as not to interleave guard objects in such a way as to
// cause an illegal sequence of calls on a lock (two sequential lock calls or
// two sequential unlock calls on a non-recursive mutex).

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
        my_Mutex mutex;

        for (someCondition = 0; someCondition < 2; ++someCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_Mutex mutex;
                errorProneFunc(&obj, &mutex);
                if (0 == someCondition && 1 == someOtherCondition) {
                    ASSERT( 0 < mutex.lockCount() );
                } else  {
                    ASSERT( 0 == mutex.lockCount() );
                }
            }
        }

        for (someCondition = 0; someCondition < 2; ++someCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_Mutex mutex;
                safeFunc(&obj, &mutex);
                ASSERT( 0 == mutex.lockCount() );
            }
        }

        for (someCondition = 0; someCondition < 2; ++someCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_Mutex mutex;
                // mutex.tryLock() fails the first time
                ASSERT( 0 == safeButNonBlockingFunc(&obj, &mutex) );
                ASSERT( 0 == mutex.lockCount() );
                // but succeeds the second time
                ASSERT( 0 < safeButNonBlockingFunc(&obj, &mutex) );
                ASSERT( 0 == mutex.lockCount() );
            }
        }

        for (someCondition = 0; someCondition < 2; ++someCondition) {
            for (someOtherCondition = 0; someOtherCondition < 2;
                    ++someOtherCondition) {
                my_Mutex mutex;
                f(&mutex);
                ASSERT( 0 == mutex.lockCount() );
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING backwards-compatibility
        //
        // Concern:
        //   That 'bslmt::TryLockGuard' and 'bslmt::UnLockGuard' are equivalent
        //   to their new, properly-named counterparts.
        //
        // Plan:
        //   Copy cases 3 and 4 below and re-execute them with the deprecated
        //   classes.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: backwards-compatibility"
                          << endl;
        {
            my_Mutex mX;    const my_Mutex &X = mX;

            if (verbose)
                cout << "\t\t trylock guard constructors & destructor" <<endl;
            {   // next will fail
                bslmt::TryLockGuard<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
                ASSERT(0 == l1.ptr());
                {   // next will succeed
                    bslmt::TryLockGuard<my_Mutex> l2(&mX);
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                    {   // next will succeed at second attempt
                        bslmt::TryLockGuard<my_Mutex> l3(&mX,10);
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
                cout << "\t\t trylock guard release" << endl;
            {
                bslmt::TryLockGuard<my_Mutex> l1(&mX, 2);

                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                my_Mutex *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
            }
            ASSERT(1 == mX.lockCount());
        }
        {
            my_Mutex mX;    const my_Mutex &X = mX;

            if (verbose)
                cout << "\t\t unlock guard constructor & destructor" <<endl;
            {
                bslmt::UnLockGuard<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                {
                    const int PRE_UNLOCKED = 0;
                    bslmt::UnLockGuard<my_Mutex> l2(&mX, PRE_UNLOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                    {
                        const int PRE_UNLOCKED = 1; mX.unlock();
                        bslmt::UnLockGuard<my_Mutex> l3(&mX, PRE_UNLOCKED);
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
                cout << "\t\t unlock guard release" << endl;
            {
                bslmt::UnLockGuard<my_Mutex> l1(&mX);
                ASSERT(&mX == l1.ptr());

                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                my_Mutex *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
            }
            ASSERT(-1 == mX.lockCount());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // INTERACTION BETWEEN 'bslmt::LockGuard' AND 'bslmt::LockGuardUnlock'
        //
        // Concern:
        //   That bslmt::LockGuard and bslmt::LockGuardUnlock interact together
        //   as expected.  That two different lock guards on two different
        //   'my_Mutex' objects do not interfere with each other.
        //
        // Plan:
        //   We verify that using two independent 'my_Mutex' objects with two
        //   distinct 'bslmt::LockGuard' *and* 'bslmt::LockGuardUnlock' objects
        //   in the same scope have no effect on each other.
        //
        // Testing:
        //   Interaction between lock and unlock guards, as well as
        //   between two lock guards on two different 'my_Mutex' objects.
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\tTesting interaction between "
                    "'bslmt::LockGuard' and bslmt::LockGuardUnlock'" << endl
                 << "=============================="
                    "========================================" << endl;

        {
            my_Mutex mX1;    const my_Mutex &X1 = mX1;
            my_Mutex mX2;    const my_Mutex &X2 = mX2;

            mX1.lock();
            {
                bslmt::LockGuard<my_Mutex> l1(&mX1);
                bslmt::LockGuard<my_Mutex> l2(&mX2);
                ASSERT(2 == X1.lockCount());
                ASSERT(1 == X2.lockCount());

                {
                    bslmt::LockGuardUnlock<my_Mutex> u1(&mX1);
                    bslmt::LockGuardUnlock<my_Mutex> u2(&mX2);
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
        // TESTING CLASS 'bslmt::LockGuardTryLock'
        //
        // Concern:
        //   That the basic functionality of the 'bslmt::LockGuardTryLock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bslmt::LockGuardTryLock'
        //   objects using a common 'my_Mutex' object.  With each new object we
        //   verify that the lock function is called only if the constructor
        //   succeeds.  As each object is destroyed, we verify that the unlock
        //   function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bslmt::LockGuard' and calling 'release'.  We
        //   verify that the returned pointer matches the value we supplied
        //   only if the constructor succeeded in acquiring the lock.  We then
        //   verify that 'release' makes no attempt to unlock the supplied
        //   object and that when the 'bslmt::LockGuardTryLock' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bslmt::LockGuardTryLock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bslmt::LockGuardTryLock();
        //   ~bslmt::LockGuardTryLock();
        //   bslmt::LockGuardTryLock::release();
        //   bslmt::LockGuardTryLock::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bslmt::LockGuardTryLock(); \n"
                          << "\t  ~bslmt::LockGuardTryLock(); \n"
                          << "\t  bslmt::LockGuardTryLock::release(); \n"
                          << endl;

        {
            my_Mutex mX;    const my_Mutex &X = mX;

            if (verbose)
                cout << "\t\t trylock guard constructors & destructor" <<endl;
            {   // next will fail
                bslmt::LockGuardTryLock<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
                ASSERT(0 == l1.ptr());
                {   // next will succeed
                    bslmt::LockGuardTryLock<my_Mutex> l2(&mX);
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                    {   // next will succeed at second attempt
                        bslmt::LockGuardTryLock<my_Mutex> l3(&mX,10);
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
                cout << "\t\t trylock guard release" << endl;
            {
                bslmt::LockGuardTryLock<my_Mutex> l1(&mX, 2);

                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                my_Mutex *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
            }
            ASSERT(1 == mX.lockCount());
        }

        {
            bslmt::LockGuardTryLock<my_Mutex> mX(0, 6);
        }
        {
            bslmt::LockGuardTryLock<my_Mutex> mX(0, 6);
            mX.release();
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bslmt::LockGuardUnlock'
        //
        // Concern:
        //   That the basic functionality of the 'bslmt::LockUnGuard' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bslmt::LockGuardUnlock'
        //   objects using a common 'my_Mutex' object.  With each new object
        //   we verify that the unlock function is called.  As each object is
        //   destroyed, we verify that the lock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bslmt::LockGuardUnlock' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied.
        //   We then verify that 'release' makes no attempt to unlock
        //   the supplied object and that when the 'bslmt::LockGuardUnlock'
        //   object is destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bslmt::LockGuardUnlock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bslmt::LockGuardUnlock();
        //   ~bslmt::LockGuardUnlock();
        //   bslmt::LockGuardUnlock::release();
        //   bslmt::LockGuardUnlock::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bslmt::LockGuardUnlock(); \n"
                          << "\t  ~bslmt::LockGuardUnlock(); \n"
                          << "\t  bslmt::LockGuardUnlock::release(); \n"
                          << endl;

        {
            my_Mutex mX;    const my_Mutex &X = mX;

            if (verbose)
                cout << "\t\t unlock guard constructor & destructor" <<endl;
            {
                bslmt::LockGuardUnlock<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                {
                    const int PRE_UNLOCKED = 0;
                    bslmt::LockGuardUnlock<my_Mutex> l2(&mX, PRE_UNLOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                    {
                        const int PRE_UNLOCKED = 1; mX.unlock();
                        bslmt::LockGuardUnlock<my_Mutex> l3(&mX, PRE_UNLOCKED);
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
                cout << "\t\t unlock guard release" << endl;
            {
                bslmt::LockGuardUnlock<my_Mutex> l1(&mX);
                ASSERT(&mX == l1.ptr());

                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                my_Mutex *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
            }
            ASSERT(-1 == mX.lockCount());
        }

        {
            bslmt::LockGuardUnlock<my_Mutex> mX(0);
        }
        {
            bslmt::LockGuardUnlock<my_Mutex> mX(0);
            mX.release();
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bslmt::LockGuard'
        //
        // Concern:
        //   That the basic functionality of the 'bslmt::LockGuard' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bslmt::LockGuard'
        //   objects using a common 'my_Mutex' object.  With each new object
        //   we verify the the lock function was called.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bslmt::LockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied.
        //   We then verify that 'release' makes no attempt to unlock
        //   the supplied object and that when the 'bslmt::LockGuard' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bslmt::LockGuard' can be created with a
        //   null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bslmt::LockGuard();
        //   ~bslmt::LockGuard();
        //   bslmt::LockGuard::release();
        //   bslmt::LockGuard::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bslmt::LockGuard(); \n"
                          << "\t  ~bslmt::LockGuard(); \n"
                          << "\t  bslmt::LockGuard::release(); \n" << endl;

        {
            my_Mutex mX;    const my_Mutex &X = mX;

            if (verbose)
                cout << "\t\t lock guard constructor & destructor" <<endl;
            {
                bslmt::LockGuard<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bslmt::LockGuard<my_Mutex> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                        const int PRE_LOCKED = 1; mX.lock();
                        bslmt::LockGuard<my_Mutex> l3(&mX, PRE_LOCKED);
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
                cout << "\t\t lock guard release" << endl;
            {
                bslmt::LockGuard<my_Mutex> l1(&mX);
                ASSERT(&mX == l1.ptr());

                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                my_Mutex *pm = l1.release();
                ASSERT(0 == l1.ptr());

                ASSERT(&mX == pm);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
            }
            ASSERT(1 == mX.lockCount());
        }

        {
            bslmt::LockGuard<my_Mutex> mX(0);
        }
        {
            bslmt::LockGuard<my_Mutex> mX(0);
            mX.release();
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //   The 'my_Mutex' class is a simple type that supports the lock and
        //   unlock methods used by the guard class.  These methods simply
        //   increment and decrement a count integer data member.
        //
        // Testing:
        //   my_Mutex();
        //   ~my_Mutex();
        //   lock();
        //   tryLock();
        //   unlock();
        //   lockCount() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASSES TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "Testing my_Mutex." << endl;

        my_Mutex mX;    const my_Mutex &X = mX;
        ASSERT(0 == X.lockCount());

        mX.lock();
        ASSERT(1 == X.lockCount());

        mX.unlock();
        ASSERT(0 == X.lockCount());

        mX.lock();
        mX.lock();
        ASSERT(2 == X.lockCount());
        mX.unlock();
        ASSERT(1 == X.lockCount());
        mX.lock();
        mX.lock();
        ASSERT(3 == X.lockCount());
        mX.unlock();
        mX.unlock();
        mX.unlock();
        ASSERT(0 == X.lockCount());

        mX.tryLock();
        ASSERT(0 == X.lockCount());
        mX.tryLock();
        ASSERT(1 == X.lockCount());
        mX.unlock();
        ASSERT(0 == X.lockCount());
        mX.tryLock();
        mX.tryLock();
        mX.tryLock();
        mX.tryLock();
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
