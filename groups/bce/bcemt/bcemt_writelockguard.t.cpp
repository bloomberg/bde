// bcemt_writelockguard.t.cpp                                         -*-C++-*-
#include <bcemt_writelockguard.h>

#include <bsl_iostream.h>
#include <bsl_cstring.h>  // strcmp()
#include <bsl_cstdlib.h>  // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
// This program tests the functionality of the 'bcemt_WriteLockGuard' class.
// It verifies that the class properly locks the synchronization object for
// writing at construction time, and that it properly unlocks the object at
// destruction time.  A helper class, 'my_RWLock', is created to facilitate the
// test process.  'my_RWLock' implements the required lock and unlock
// interfaces and provides a means to determine when the functions are called.
//
//-----------------------------------------------------------------------------
// bcemt_WriteLockGuard
// ===================
// [2] bcemt_WriteLockGuard();
// [2] ~bcemt_WriteLockGuard();
// [2] bcemt_WriteLockGuard::release();
// [2] bcemt_WriteLockGuard::ptr();
//
// bcemt_WriteLockGuardUnlock
// =====================
// [3] bcemt_WriteLockGuardUnlock();
// [3] ~bcemt_WriteLockGuardUnlock();
// [3] bcemt_WriteLockGuardUnlock::release();
// [3] bcemt_WriteLockGuardUnlock::ptr();
//
// bcemt_WriteLockGuardTryLock
// =====================
// [4] bcemt_WriteLockGuardTryLock();
// [4] ~bcemt_WriteLockGuardTryLock();
// [4] bcemt_WriteLockGuardTryLock::release();
// [4] bcemt_WriteLockGuardTryLock::ptr();
//-----------------------------------------------------------------------------
// [1] Ensure helper class 'my_RWLock' works as expected
// [5] INTERACTION BETW 'bcemt_WriteLockGuard' AND 'bcemt_WriteLockGuardUnlock'
// [6] DEPRECATED 'bcemt_LockWriteGuard'
// [7] USAGE EXAMPLES
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define PA(X, L) cout << #X " = "; printArray(X, L); cout << endl;
                                              // Print array 'X' of length 'L'
#define PA_(X, L) cout << #X " = "; printArray(X, L); cout << ", " << flush;
                                              // PA(X, L) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int someUpgradeCondition = 0;
int someOtherUpgradeCondition = 0;

struct my_RWLock {
    // This class provides a simulated mutual exclusion mechanism which
    // conforms to the interface required by the 'bcemt_WriteLockGuard' class.
    // It operates using a counter to track the symbolic "locked" state.
    // Each call to the lock and unlock functions increment or decrement
    // the lock count respectively.  The current state of the lock count
    // is accessible through the lockCount method.  The 'tryLock' is designed
    // to fail the first time, then succeed every other time.

    int d_count;
    int d_attempt;

    my_RWLock() : d_count(0), d_attempt(1)  {}
    ~my_RWLock() {};
    int lockCount() const { return d_count; }
    int tryLockWrite() {
        if ((++d_attempt)%2) {
            lockWrite();
        } else {
            return 1;
        }
        return 0;
    }
    void lockWrite() { ++d_count; }
    void unlock() { --d_count; }
};

struct my_Object {
    void someUpgradeMethod() {};
    void someOtherUpgradeMethod() {};
    void defaultUpgradeMethod() {};
};

// USAGE EXAMPLE

static void errorProneFunc(my_Object *obj, my_RWLock *rwlock)
{
    rwlock->lockWrite();
    if (someUpgradeCondition) {
        obj->someUpgradeMethod();
        rwlock->unlock();
        return;
    } else if (someOtherUpgradeCondition) {
        obj->someOtherUpgradeMethod();
        return;                             // MISTAKE! forgot to unlock rwlock
    }
    obj->defaultUpgradeMethod();
    rwlock->unlock();
    return;
}

static void safeFunc(my_Object *obj, my_RWLock *rwlock)
{
    bcemt_WriteLockGuard<my_RWLock> guard(rwlock);
    if (someUpgradeCondition) {
        obj->someUpgradeMethod();
        return;
    } else if (someOtherUpgradeCondition) {
        obj->someOtherUpgradeMethod();
        return;                         // OK, rwlock is automatically unlocked
    }
    obj->defaultUpgradeMethod();
    return;
}

static int safeButNonBlockingFunc(my_Object *obj, my_RWLock *rwlock)
    // Perform task and return positive value if locking succeeds.
    // Return 0 if locking fails.
{
    const int RETRIES = 1; // use higher values for higher success rate
    bcemt_WriteLockGuardTryLock<my_RWLock> guard(rwlock, RETRIES);
    if (guard.ptr()) { // rwlock is locked
        if (someUpgradeCondition) {
            obj->someUpgradeMethod();
            return 2;
        } else if (someOtherUpgradeCondition) {
            obj->someOtherUpgradeMethod();
            return 3;
        }
        obj->defaultUpgradeMethod();
        return 1;
    }
    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

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

        for (someUpgradeCondition = 0; someUpgradeCondition < 2;
                ++someUpgradeCondition) {
            for (someOtherUpgradeCondition = 0; someOtherUpgradeCondition < 2;
                    ++someOtherUpgradeCondition) {
                my_RWLock rwlock;
                errorProneFunc(&obj, &rwlock);
                if (0 == someUpgradeCondition
                 && 1 == someOtherUpgradeCondition) {
                    ASSERT( 0 < rwlock.lockCount() );
                } else  {
                    ASSERT( 0 == rwlock.lockCount() );
                }
            }
        }

        for (someUpgradeCondition = 0; someUpgradeCondition < 2;
                ++someUpgradeCondition) {
            for (someOtherUpgradeCondition = 0; someOtherUpgradeCondition < 2;
                    ++someOtherUpgradeCondition) {
                my_RWLock rwlock;
                safeFunc(&obj, &rwlock);
                ASSERT( 0 == rwlock.lockCount() );
            }
        }

        for (someUpgradeCondition = 0; someUpgradeCondition < 2;
                ++someUpgradeCondition) {
            for (someOtherUpgradeCondition = 0; someOtherUpgradeCondition < 2;
                    ++someOtherUpgradeCondition) {
                my_RWLock rwlock;
                // rwlock.tryLockWrite() will fail the first time
                ASSERT( 0 == safeButNonBlockingFunc(&obj, &rwlock) );
                ASSERT( 0 == rwlock.lockCount() );
                // but succeed the second time
                ASSERT( 0 < safeButNonBlockingFunc(&obj, &rwlock) );
                ASSERT( 0 == rwlock.lockCount() );
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // DEPRECATED CLASSES
        //
        // Concern:
        //   Backwards compatibility: ensure that the 'bcemt_LockWriteGuard'
        //   class exists and still works.
        //
        // Plan:
        //   Copy case 2, but use the old deprecated name.
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "\tTesting deprecated classes" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t lock guard constructor & destructor" <<endl;
            {
                bcemt_LockWriteGuard<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bcemt_LockWriteGuard<my_RWLock> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                    const int PRE_LOCKED = 1; mX.lockWrite();
                        bcemt_LockWriteGuard<my_RWLock> l3(&mX, PRE_LOCKED);
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
                bcemt_LockWriteGuard<my_RWLock> l1(&mX);
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
        // INTERACTION BETWEEN 'bcemt_WriteLockGuard' AND
        // 'bcemt_WriteLockGuardUnlock'
        //
        // Concern:
        //   That 'bcemt_WriteLockGuard' and 'bcemt_WriteLockGuardUnlock'
        //   interact together as expected.  That two different lock guards on
        //   two different 'my_RWLock' objects do not interfere with each
        //   other.
        //
        // Plan:
        //   We verify that using two independent 'my_RWLock' objects with two
        //   distinct 'bcemt_WriteLockGuard' *and* 'bcemt_WriteLockGuardUnlock'
        //   objects in the same scope have no effect on each other.
        //
        // Testing:
        //   Interaction between lock and unlock guards, as well as
        //   between two lock guards on two different 'my_RWLock' objects.
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\tTesting interaction between "
                    "'bcemt_WriteLockGuard' and bcemt_WriteLockGuardUnlock'"
                 << endl
                 << "=============================="
                    "========================================"
                 << endl;

        {
            my_RWLock mX1;    const my_RWLock &X1 = mX1;
            my_RWLock mX2;    const my_RWLock &X2 = mX2;

            mX1.lockWrite();
            {
                bcemt_WriteLockGuard<my_RWLock> l1(&mX1);
                bcemt_WriteLockGuard<my_RWLock> l2(&mX2);
                ASSERT(2 == X1.lockCount());
                ASSERT(1 == X2.lockCount());

                {
                    bcemt_WriteLockGuardUnlock<my_RWLock> u1(&mX1);
                    bcemt_WriteLockGuardUnlock<my_RWLock> u2(&mX2);
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
        // TESTING CLASS 'bcemt_WriteLockGuardTryLock'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_WriteLockGuardTryLock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested
        //   'bcemt_WriteLockGuardTryLock' objects using a common 'my_RWLock'
        //   object.  With each new object we verify that the lock function is
        //   called only if the constructor succeeds.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_WriteLockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied
        //   only if the constructor succeeded in acquiring the lock.  We then
        //   verify that 'release' makes no attempt to unlock the supplied
        //   object and that when the 'bcemt_WriteLockGuardTryLock' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bcemt_WriteLockGuardTryLock' can be
        //   created with a null lock, and that 'release' may be called on the
        //   guard.
        //
        // Testing:
        //   bcemt_WriteLockGuardTryLock();
        //   ~bcemt_WriteLockGuardTryLock();
        //   bcemt_WriteLockGuardTryLock::release();
        //   bcemt_WriteLockGuardTryLock::ptr();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "\tTesting: \n"
                 << "\t  bcemt_WriteLockGuardTryLock(); \n"
                 << "\t  ~bcemt_WriteLockGuardTryLock(); \n"
                 << "\t  bcemt_WriteLockGuardTryLock::release(); \n"
                 << "\t  bcemt_WriteLockGuardTryLock::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t trylock guard constructors & destructor" <<endl;
            {
                bcemt_WriteLockGuardTryLock<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
                ASSERT(0 == l1.ptr());
                {
                    bcemt_WriteLockGuardTryLock<my_RWLock> l2(&mX);
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                    {
                        bcemt_WriteLockGuardTryLock<my_RWLock> l3(&mX, 10);
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
                bcemt_WriteLockGuardTryLock<my_RWLock> l1(&mX, 2);

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
            bcemt_WriteLockGuardTryLock<my_RWLock> mX(0, 6);
        }
        {
            bcemt_WriteLockGuardTryLock<my_RWLock> mX(0, 6);
            mX.release();
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bcemt_WriteLockGuardUnlock'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_WriteLockGuardUnlock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested
        //   'bcemt_WriteLockGuardUnlock' objects using a common 'my_RWLock'
        //   object.  With each new object we verify that the unlock function
        //   is called.  As each object is destroyed, we verify that the lock
        //   function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_WriteLockGuardUnlock' and calling
        //   'release'.  We verify that the returned pointer matches the value
        //   we supplied.  We then verify that 'release' makes no attempt to
        //   unlock the supplied object and that when the
        //   'bcemt_WriteLockGuardUnlock' object is destroyed, it does not
        //   unlock the object.
        //
        //   Finally we test that a 'bcemt_WriteLockGuardUnlock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bcemt_WriteLockGuardUnlock();
        //   ~bcemt_WriteLockGuardUnlock();
        //   bcemt_WriteLockGuardUnlock::release();
        //   bcemt_WriteLockGuardUnlock::ptr();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "\tTesting: \n"
                 << "\t  bcemt_WriteLockGuardUnlock(); \n"
                 << "\t  ~bcemt_WriteLockGuardUnlock(); \n"
                 << "\t  bcemt_WriteLockGuardUnlock::release(); \n"
                 << "\t  bcemt_WriteLockGuardUnlock::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t unlock guard constructor & destructor" <<endl;
            {
                bcemt_WriteLockGuardUnlock<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                {
                    const int PRE_UNLOCKED = 0;
                    bcemt_WriteLockGuardUnlock<my_RWLock>
                                                         l2(&mX, PRE_UNLOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                    {
                        const int PRE_UNLOCKED = 1; mX.unlock();
                        bcemt_WriteLockGuardUnlock<my_RWLock> l3(&mX,
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
                bcemt_WriteLockGuardUnlock<my_RWLock> l1(&mX);
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
            bcemt_WriteLockGuardUnlock<my_RWLock> mX(0);
        }
        {
            bcemt_WriteLockGuardUnlock<my_RWLock> mX(0);
            mX.release();
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bcemt_WriteLockGuard'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_WriteLockGuard' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bcemt_WriteLockGuard'
        //   objects using a common 'my_RWLock' object.  With each new object
        //   we verify the the lock function was called.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_WriteLockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied.
        //   We then verify that 'release' makes no attempt to unlock the
        //   supplied object and that when the 'bcemt_WriteLockGuard' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bcemt_WriteLockGuard' can be created with
        //   a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bcemt_WriteLockGuard();
        //   ~bcemt_WriteLockGuard();
        //   bcemt_WriteLockGuard::release();
        //   bcemt_WriteLockGuard::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bcemt_WriteLockGuard(); \n"
                          << "\t  ~bcemt_WriteLockGuard(); \n"
                          << "\t  bcemt_WriteLockGuard::release(); \n"
                          << "\t  bcemt_WriteLockGuard::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t lock guard constructor & destructor" <<endl;
            {
                bcemt_WriteLockGuard<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bcemt_WriteLockGuard<my_RWLock> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                    const int PRE_LOCKED = 1; mX.lockWrite();
                        bcemt_WriteLockGuard<my_RWLock> l3(&mX, PRE_LOCKED);
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
                bcemt_WriteLockGuard<my_RWLock> l1(&mX);
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
            bcemt_WriteLockGuard<my_RWLock> mX(0);
        }
        {
            bcemt_WriteLockGuard<my_RWLock> mX(0);
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
        //   my_RWLock::lockWrite();
        //   my_RWLock::tryLockWrite();
        //   my_RWLock::unlock();
        //   my_RWLock::lockCount() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASSES TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "Testing my_RWLock." << endl;

        my_RWLock mX;    const my_RWLock &X = mX;
        ASSERT(0 == X.lockCount());

        mX.lockWrite();
        ASSERT(1 == X.lockCount());

        mX.unlock();
        ASSERT(0 == X.lockCount());

        mX.lockWrite();
        mX.lockWrite();
        ASSERT(2 == X.lockCount());
        mX.unlock();
        ASSERT(1 == X.lockCount());
        mX.lockWrite();
        mX.lockWrite();
        ASSERT(3 == X.lockCount());
        mX.unlock();
        mX.unlock();
        mX.unlock();
        ASSERT(0 == X.lockCount());

        mX.tryLockWrite();
        ASSERT(0 == X.lockCount());
        mX.tryLockWrite();
        ASSERT(1 == X.lockCount());
        mX.unlock();
        ASSERT(0 == X.lockCount());
        mX.tryLockWrite();
        mX.tryLockWrite();
        mX.tryLockWrite();
        mX.tryLockWrite();
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
