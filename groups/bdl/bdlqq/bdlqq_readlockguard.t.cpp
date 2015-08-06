// bdlqq_readlockguard.t.cpp                                          -*-C++-*-
#include <bdlqq_readlockguard.h>

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
// This program tests the functionality of the 'bdlqq::ReadLockGuard' class.
// It verifies that the class properly locks the synchronization object for
// read at construction time, and that it properly unlocks the object at
// destruction time.  A helper class, 'my_RWLock', is created to facilitate the
// test process.  'my_RWLock' implements the required lock and unlock
// interfaces and provides a means to determine when the functions are called.
//
//-----------------------------------------------------------------------------
// bdlqq::ReadLockGuard
// ============================================================================
// [2] bdlqq::ReadLockGuard();
// [2] ~bdlqq::ReadLockGuard();
// [2] bdlqq::ReadLockGuard::release();
// [2] bdlqq::ReadLockGuard::ptr();
//
// bdlqq::ReadLockGuardUnlock
// ============================================================================
// [3] bdlqq::ReadLockGuardUnlock();
// [3] ~bdlqq::ReadLockGuardUnlock();
// [3] bdlqq::ReadLockGuardUnlock::release();
// [3] bdlqq::ReadLockGuardUnlock::ptr();
//
// bdlqq::ReadLockGuardTryLock
// ============================================================================
// [4] bdlqq::ReadLockGuardTryLock();
// [4] ~bdlqq::ReadLockGuardTryLock();
// [4] bdlqq::ReadLockGuardTryLock::release();
// [4] bdlqq::ReadLockGuardTryLock::ptr();
//-----------------------------------------------------------------------------
// [1] Ensure helper class 'my_RWLock' works as expected
// [5] INTERACTION BET. 'bdlqq::ReadLockGuard' AND 'bdlqq::ReadLockGuardUnlock'
// [6] DEPRECATED 'bdlqq::LockReadGuard'
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

int someCondition = 0;
int someUpgradeCondition = 0;
int someOtherCondition = 0;

struct my_RWLock {
    // This class provides a simulated mutual exclusion mechanism which
    // conforms to the interface required by the 'bdlqq::ReadLockGuard' class.
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

// USAGE EXAMPLE

static void errorProneFunc(const my_Object *obj, my_RWLock *rwlock)
{
    rwlock->lockRead();
    if (someCondition) {
        obj->someMethod();
        rwlock->unlock();
        return;                                                       // RETURN
    } else if (someOtherCondition) {
        obj->someOtherMethod();
        return;                             // MISTAKE! forgot to unlock rwlock
                                                                      // RETURN
    }
    obj->defaultMethod();
    rwlock->unlock();
    return;
}

static void safeFunc(const my_Object *obj, my_RWLock *rwlock)
{
    bdlqq::ReadLockGuard<my_RWLock> guard(rwlock);
    if (someCondition) {
        obj->someMethod();
        return;                                                       // RETURN
    } else if (someOtherCondition) {
        obj->someOtherMethod();
        return;                         // OK, rwlock is automatically unlocked
                                                                      // RETURN
    }
    obj->defaultMethod();
    return;
}

static int safeButNonBlockingFunc(const my_Object *obj, my_RWLock *rwlock)
    // Perform task and return positive value if locking succeeds.
    // Return 0 if locking fails.
{
    const int RETRIES = 1; // use higher values for higher success rate
    bdlqq::ReadLockGuardTryLock<my_RWLock> guard(rwlock, RETRIES);
    if (guard.ptr()) { // rwlock is locked
        if (someCondition) {
            obj->someMethod();
            return 2;                                                 // RETURN
        } else if (someOtherCondition) {
            obj->someOtherMethod();
            return 3;                                                 // RETURN
        }
        obj->defaultMethod();
        return 1;                                                     // RETURN
    }
    return 0;
}

static void safeUpgradeFunc(my_Object *obj, my_RWLock *rwlock)
{
    const my_Object *constObj = obj;
    bdlqq::ReadLockGuard<my_RWLock> guard(rwlock);
    if (someUpgradeCondition) {
        obj->someUpgradeMethod();
        return;                                                       // RETURN
    } else if (someOtherCondition) {
        constObj->someOtherMethod();
        return;                         // OK, rwlock is automatically unlocked
                                                                      // RETURN
    }
    constObj->defaultMethod();
    return;
}

static void safeAtomicUpdateFunc(my_Object *obj, my_RWLock *rwlock)
{
    const my_Object *constObj = obj;
    rwlock->lockReadReserveWrite();
    const int PRELOCKED = 1;
    bdlqq::ReadLockGuard<my_RWLock> guard(rwlock, PRELOCKED);
    if (someUpgradeCondition) {
        rwlock->upgradeToWriteLock();
        obj->someUpgradeMethod();
        return;                                                       // RETURN
    } else if (someOtherCondition) {
        constObj->someOtherMethod();
        return;                                                       // RETURN
    }
    constObj->defaultMethod();
    return;
}

void f(my_RWLock *rwlock)
{
    bdlqq::ReadLockGuard<my_RWLock> guard(rwlock);

    // critical section here

    {
        bdlqq::ReadLockGuardUnlock<my_RWLock> guard(rwlock);

        // rwlock is unlocked here

    } // rwlock is locked again here

    // critical section here

} // rwlock is unlocked here

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
                safeUpgradeFunc(&obj, &rwlock);
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
        //   Backwards compatibility: ensure that the 'bdlqq::LockReadGuard'
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
                bdlqq::LockReadGuard<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bdlqq::LockReadGuard<my_RWLock> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                        const int PRE_LOCKED = 1; mX.lockRead();
                        bdlqq::LockReadGuard<my_RWLock> l3(&mX, PRE_LOCKED);
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
                bdlqq::LockReadGuard<my_RWLock> l1(&mX);
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
        // INTERACTION BETWEEN 'bdlqq::ReadLockGuard' AND
        // 'bdlqq::ReadLockGuardUnlock'
        //
        // Concern:
        //   That 'bdlqq::ReadLockGuard' and 'bdlqq::ReadLockGuardUnlock'
        //   interact together as expected.  That two different lock guards on
        //   two different 'my_RWLock' objects do not interfere with each
        //   other.
        //
        // Plan:
        //   We verify that using two independent 'my_RWLock' objects with two
        //   distinct 'bdlqq::ReadLockGuard' *and* 'bdlqq::ReadLockGuardUnlock'
        //   objects in the same scope have no effect on each other.
        //
        // Testing:
        //   Interaction between lock and unlock guards, as well as
        //   between two lock guards on two different 'my_RWLock' objects.
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\tTesting interaction between "
                    "'bdlqq::ReadLockGuard' and bdlqq::ReadLockGuardUnlock'"
                 << endl
                 << "=============================="
                    "========================================"
                 << endl;

        {
            my_RWLock mX1;    const my_RWLock &X1 = mX1;
            my_RWLock mX2;    const my_RWLock &X2 = mX2;

            mX1.lockRead();
            {
                bdlqq::ReadLockGuard<my_RWLock> l1(&mX1);
                bdlqq::ReadLockGuard<my_RWLock> l2(&mX2);
                ASSERT(2 == X1.lockCount());
                ASSERT(1 == X2.lockCount());

                {
                    bdlqq::ReadLockGuardUnlock<my_RWLock> u1(&mX1);
                    bdlqq::ReadLockGuardUnlock<my_RWLock> u2(&mX2);
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
        // TESTING CLASS 'bdlqq::ReadLockGuardTryLock'
        //
        // Concern:
        //   That the basic functionality of the 'bdlqq::ReadLockGuardTryLock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested
        //   'bdlqq::ReadLockGuardTryLock' objects using a common 'my_RWLock'
        //   object.  With each new object we verify that the lock function is
        //   called only if the constructor succeeds.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bdlqq::ReadLockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied
        //   only if the constructor succeeded in acquiring the lock.  We then
        //   verify that 'release' makes no attempt to unlock the supplied
        //   object and that when the 'bdlqq::ReadLockGuardTryLock' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bdlqq::ReadLockGuardTryLock' can be
        //   created with a null lock, and that 'release' may be called on the
        //   guard.
        //
        // Testing:
        //   bdlqq::ReadLockGuardTryLock();
        //   ~bdlqq::ReadLockGuardTryLock();
        //   bdlqq::ReadLockGuardTryLock::release();
        //   bdlqq::ReadLockGuardTryLock::ptr();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "\tTesting: \n"
                 << "\t  bdlqq::ReadLockGuardTryLock(); \n"
                 << "\t  ~bdlqq::ReadLockGuardTryLock(); \n"
                 << "\t  bdlqq::ReadLockGuardTryLock::release(); \n"
                 << "\t  bdlqq::ReadLockGuardTryLock::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t trylock guard constructors & destructor" <<endl;
            {
                bdlqq::ReadLockGuardTryLock<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
                ASSERT(0 == l1.ptr());
                {
                    bdlqq::ReadLockGuardTryLock<my_RWLock> l2(&mX);
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                    {
                        bdlqq::ReadLockGuardTryLock<my_RWLock> l3(&mX, 10);
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
                bdlqq::ReadLockGuardTryLock<my_RWLock> l1(&mX, 2);

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
            bdlqq::ReadLockGuardTryLock<my_RWLock> mX(0, 6);
        }
        {
            bdlqq::ReadLockGuardTryLock<my_RWLock> mX(0, 6);
            mX.release();
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bdlqq::ReadLockGuardUnlock'
        //
        // Concern:
        //   That the basic functionality of the 'bdlqq::ReadLockGuardUnlock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested
        //   'bdlqq::ReadLockGuardUnlock' objects using a common 'my_RWLock'
        //   object.  With each new object we verify that the unlock function
        //   is called.  As each object is destroyed, we verify that the lock
        //   function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bdlqq::ReadLockGuardUnlock' and calling
        //   'release'.  We verify that the returned pointer matches the value
        //   we supplied.  We then verify that 'release' makes no attempt to
        //   unlock the supplied object and that when the
        //   'bdlqq::ReadLockGuardUnlock' object is destroyed, it does not
        //   unlock the object.
        //
        //   Finally we test that a 'bdlqq::ReadLockGuardUnlock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bdlqq::ReadLockGuardUnlock();
        //   ~bdlqq::ReadLockGuardUnlock();
        //   bdlqq::ReadLockGuardUnlock::release();
        //   bdlqq::ReadLockGuardUnlock::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bdlqq::ReadLockGuardUnlock(); \n"
                          << "\t  ~bdlqq::ReadLockGuardUnlock(); \n"
                          << "\t  bdlqq::ReadLockGuardUnlock::release(); \n"
                          << "\t  bdlqq::ReadLockGuardUnlock::ptr(); \n"
                          << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t unlock guard constructor & destructor" <<endl;
            {
                bdlqq::ReadLockGuardUnlock<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                {
                    const int PRE_UNLOCKED = 0;
                    bdlqq::ReadLockGuardUnlock<my_RWLock> l2(
                                                            &mX, PRE_UNLOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                    {
                        const int PRE_UNLOCKED = 1; mX.unlock();
                        bdlqq::ReadLockGuardUnlock<my_RWLock> l3(&mX,
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
                bdlqq::ReadLockGuardUnlock<my_RWLock> l1(&mX);
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
            bdlqq::ReadLockGuardUnlock<my_RWLock> mX(0);
        }
        {
            bdlqq::ReadLockGuardUnlock<my_RWLock> mX(0);
            mX.release();
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bdlqq::ReadLockGuard'
        //
        // Concern:
        //   That the basic functionality of the 'bdlqq::ReadLockGuard' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bdlqq::ReadLockGuard'
        //   objects using a common 'my_RWLock' object.  With each new object
        //   we verify the the lock function was called.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bdlqq::ReadLockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied.
        //   We then verify that 'release' makes no attempt to unlock the
        //   supplied object and that when the 'bdlqq::ReadLockGuard' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bdlqq::ReadLockGuard' can be created with
        //   a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bdlqq::ReadLockGuard();
        //   ~bdlqq::ReadLockGuard();
        //   bdlqq::ReadLockGuard::release();
        //   bdlqq::ReadLockGuard::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bdlqq::ReadLockGuard(); \n"
                          << "\t  ~bdlqq::ReadLockGuard(); \n"
                          << "\t  bdlqq::ReadLockGuard::release(); \n"
                          << "\t  bdlqq::ReadLockGuard::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t lock guard constructor & destructor" <<endl;
            {
                bdlqq::ReadLockGuard<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bdlqq::ReadLockGuard<my_RWLock> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                    const int PRE_LOCKED = 1; mX.lockRead();
                        bdlqq::ReadLockGuard<my_RWLock> l3(&mX, PRE_LOCKED);
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
                bdlqq::ReadLockGuard<my_RWLock> l1(&mX);
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
            bdlqq::ReadLockGuard<my_RWLock> mX(0);
        }
        {
            bdlqq::ReadLockGuard<my_RWLock> mX(0);
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
