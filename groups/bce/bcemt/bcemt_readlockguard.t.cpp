// bcemt_readlockguard.t.cpp                                          -*-C++-*-
#include <bcemt_readlockguard.h>

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
// This program tests the functionality of the 'bcemt_ReadLockGuard' class.  It
// verifies that the class properly locks the synchronization object for read
// at construction time, and that it properly unlocks the object at destruction
// time.  A helper class, 'my_RWLock', is created to facilitate the test
// process.  'my_RWLock' implements the required lock and unlock interfaces and
// provides a means to determine when the functions are called.
//
//-----------------------------------------------------------------------------
// bcemt_ReadLockGuard
// ===================
// [2] bcemt_ReadLockGuard();
// [2] ~bcemt_ReadLockGuard();
// [2] bcemt_ReadLockGuard::release();
// [2] bcemt_ReadLockGuard::ptr();
//
// bcemt_ReadLockGuardUnlock
// =====================
// [3] bcemt_ReadLockGuardUnlock();
// [3] ~bcemt_ReadLockGuardUnlock();
// [3] bcemt_ReadLockGuardUnlock::release();
// [3] bcemt_ReadLockGuardUnlock::ptr();
//
// bcemt_ReadLockGuardTryLock
// ======================
// [4] bcemt_ReadLockGuardTryLock();
// [4] ~bcemt_ReadLockGuardTryLock();
// [4] bcemt_ReadLockGuardTryLock::release();
// [4] bcemt_ReadLockGuardTryLock::ptr();
//-----------------------------------------------------------------------------
// [1] Ensure helper class 'my_RWLock' works as expected
// [5] INTERACTION BETW. 'bcemt_ReadLockGuard' AND 'bcemt_ReadLockGuardUnlock'
// [6] DEPRECATED 'bcemt_LockReadGuard'
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
    // conforms to the interface required by the 'bcemt_ReadLockGuard' class.
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
            return 1;
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
        return;
    } else if (someOtherCondition) {
        obj->someOtherMethod();
        return;                             // MISTAKE! forgot to unlock rwlock
    }
    obj->defaultMethod();
    rwlock->unlock();
    return;
}

static void safeFunc(const my_Object *obj, my_RWLock *rwlock)
{
    bcemt_ReadLockGuard<my_RWLock> guard(rwlock);
    if (someCondition) {
        obj->someMethod();
        return;
    } else if (someOtherCondition) {
        obj->someOtherMethod();
        return;                         // OK, rwlock is automatically unlocked
    }
    obj->defaultMethod();
    return;
}

static int safeButNonBlockingFunc(const my_Object *obj, my_RWLock *rwlock)
    // Perform task and return positive value if locking succeeds.
    // Return 0 if locking fails.
{
    const int RETRIES = 1; // use higher values for higher success rate
    bcemt_ReadLockGuardTryLock<my_RWLock> guard(rwlock, RETRIES);
    if (guard.ptr()) { // rwlock is locked
        if (someCondition) {
            obj->someMethod();
            return 2;
        } else if (someOtherCondition) {
            obj->someOtherMethod();
            return 3;
        }
        obj->defaultMethod();
        return 1;
    }
    return 0;
}

static void safeUpgradeFunc(my_Object *obj, my_RWLock *rwlock)
{
    const my_Object *constObj = obj;
    bcemt_ReadLockGuard<my_RWLock> guard(rwlock);
    if (someUpgradeCondition) {
        obj->someUpgradeMethod();
        return;
    } else if (someOtherCondition) {
        constObj->someOtherMethod();
        return;                         // OK, rwlock is automatically unlocked
    }
    constObj->defaultMethod();
    return;
}

static void safeAtomicUpdateFunc(my_Object *obj, my_RWLock *rwlock)
{
    const my_Object *constObj = obj;
    rwlock->lockReadReserveWrite();
    const int PRELOCKED = 1;
    bcemt_ReadLockGuard<my_RWLock> guard(rwlock, PRELOCKED);
    if (someUpgradeCondition) {
        rwlock->upgradeToWriteLock();
        obj->someUpgradeMethod();
        return;
    } else if (someOtherCondition) {
        constObj->someOtherMethod();
        return;
    }
    constObj->defaultMethod();
    return;
}

void f(my_RWLock *rwlock)
{
    bcemt_ReadLockGuard<my_RWLock> guard(rwlock);

    // critical section here

    {
        bcemt_ReadLockGuardUnlock<my_RWLock> guard(rwlock);

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
        //   Backwards compatibility: ensure that the 'bcemt_LockReadGuard'
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
                bcemt_LockReadGuard<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bcemt_LockReadGuard<my_RWLock> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                        const int PRE_LOCKED = 1; mX.lockRead();
                        bcemt_LockReadGuard<my_RWLock> l3(&mX, PRE_LOCKED);
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
                bcemt_LockReadGuard<my_RWLock> l1(&mX);
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
        // INTERACTION BETWEEN 'bcemt_ReadLockGuard' AND
        // 'bcemt_ReadLockGuardUnlock'
        //
        // Concern:
        //   That 'bcemt_ReadLockGuard' and 'bcemt_ReadLockGuardUnlock'
        //   interact together as expected.  That two different lock guards on
        //   two different 'my_RWLock' objects do not interfere with each
        //   other.
        //
        // Plan:
        //   We verify that using two independent 'my_RWLock' objects with two
        //   distinct 'bcemt_ReadLockGuard' *and* 'bcemt_ReadLockGuardUnlock'
        //   objects in the same scope have no effect on each other.
        //
        // Testing:
        //   Interaction between lock and unlock guards, as well as
        //   between two lock guards on two different 'my_RWLock' objects.
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\tTesting interaction between "
                    "'bcemt_ReadLockGuard' and bcemt_ReadLockGuardUnlock'"
                 << endl
                 << "=============================="
                    "========================================"
                 << endl;

        {
            my_RWLock mX1;    const my_RWLock &X1 = mX1;
            my_RWLock mX2;    const my_RWLock &X2 = mX2;

            mX1.lockRead();
            {
                bcemt_ReadLockGuard<my_RWLock> l1(&mX1);
                bcemt_ReadLockGuard<my_RWLock> l2(&mX2);
                ASSERT(2 == X1.lockCount());
                ASSERT(1 == X2.lockCount());

                {
                    bcemt_ReadLockGuardUnlock<my_RWLock> u1(&mX1);
                    bcemt_ReadLockGuardUnlock<my_RWLock> u2(&mX2);
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
        // TESTING CLASS 'bcemt_ReadLockGuardTryLock'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_ReadLockGuardTryLock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested
        //   'bcemt_ReadLockGuardTryLock' objects using a common 'my_RWLock'
        //   object.  With each new object we verify that the lock function is
        //   called only if the constructor succeeds.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_ReadLockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied
        //   only if the constructor succeeded in acquiring the lock.  We then
        //   verify that 'release' makes no attempt to unlock the supplied
        //   object and that when the 'bcemt_ReadLockGuardTryLock' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bcemt_ReadLockGuardTryLock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bcemt_ReadLockGuardTryLock();
        //   ~bcemt_ReadLockGuardTryLock();
        //   bcemt_ReadLockGuardTryLock::release();
        //   bcemt_ReadLockGuardTryLock::ptr();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "\tTesting: \n"
                 << "\t  bcemt_ReadLockGuardTryLock(); \n"
                 << "\t  ~bcemt_ReadLockGuardTryLock(); \n"
                 << "\t  bcemt_ReadLockGuardTryLock::release(); \n"
                 << "\t  bcemt_ReadLockGuardTryLock::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t trylock guard constructors & destructor" <<endl;
            {
                bcemt_ReadLockGuardTryLock<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
                ASSERT(0 == l1.ptr());
                {
                    bcemt_ReadLockGuardTryLock<my_RWLock> l2(&mX);
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                    {
                        bcemt_ReadLockGuardTryLock<my_RWLock> l3(&mX, 10);
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
                bcemt_ReadLockGuardTryLock<my_RWLock> l1(&mX, 2);

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
            bcemt_ReadLockGuardTryLock<my_RWLock> mX(0, 6);
        }
        {
            bcemt_ReadLockGuardTryLock<my_RWLock> mX(0, 6);
            mX.release();
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bcemt_ReadLockGuardUnlock'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_ReadLockGuardUnlock'
        //   class template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested
        //   'bcemt_ReadLockGuardUnlock' objects using a common 'my_RWLock'
        //   object.  With each new object we verify that the unlock function
        //   is called.  As each object is destroyed, we verify that the lock
        //   function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_ReadLockGuardUnlock' and calling
        //   'release'.  We verify that the returned pointer matches the value
        //   we supplied.  We then verify that 'release' makes no attempt to
        //   unlock the supplied object and that when the
        //   'bcemt_ReadLockGuardUnlock' object is destroyed, it does not
        //   unlock the object.
        //
        //   Finally we test that a 'bcemt_ReadLockGuardUnlock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bcemt_ReadLockGuardUnlock();
        //   ~bcemt_ReadLockGuardUnlock();
        //   bcemt_ReadLockGuardUnlock::release();
        //   bcemt_ReadLockGuardUnlock::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bcemt_ReadLockGuardUnlock(); \n"
                          << "\t  ~bcemt_ReadLockGuardUnlock(); \n"
                          << "\t  bcemt_ReadLockGuardUnlock::release(); \n"
                          << "\t  bcemt_ReadLockGuardUnlock::ptr(); \n"
                          << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t unlock guard constructor & destructor" <<endl;
            {
                bcemt_ReadLockGuardUnlock<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                {
                    const int PRE_UNLOCKED = 0;
                    bcemt_ReadLockGuardUnlock<my_RWLock> l2(&mX, PRE_UNLOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                    {
                        const int PRE_UNLOCKED = 1; mX.unlock();
                        bcemt_ReadLockGuardUnlock<my_RWLock> l3(&mX,
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
                bcemt_ReadLockGuardUnlock<my_RWLock> l1(&mX);
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
            bcemt_ReadLockGuardUnlock<my_RWLock> mX(0);
        }
        {
            bcemt_ReadLockGuardUnlock<my_RWLock> mX(0);
            mX.release();
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bcemt_ReadLockGuard'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_ReadLockGuard' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bcemt_ReadLockGuard'
        //   objects using a common 'my_RWLock' object.  With each new object
        //   we verify the the lock function was called.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_ReadLockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied.
        //   We then verify that 'release' makes no attempt to unlock the
        //   supplied object and that when the 'bcemt_ReadLockGuard' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bcemt_ReadLockGuard' can be created with a
        //   null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bcemt_ReadLockGuard();
        //   ~bcemt_ReadLockGuard();
        //   bcemt_ReadLockGuard::release();
        //   bcemt_ReadLockGuard::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bcemt_ReadLockGuard(); \n"
                          << "\t  ~bcemt_ReadLockGuard(); \n"
                          << "\t  bcemt_ReadLockGuard::release(); \n"
                          << "\t  bcemt_ReadLockGuard::ptr(); \n" << endl;

        {
            my_RWLock mX;    const my_RWLock &X = mX;

            if (verbose)
                cout << "\t\t lock guard constructor & destructor" <<endl;
            {
                bcemt_ReadLockGuard<my_RWLock> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bcemt_ReadLockGuard<my_RWLock> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                    const int PRE_LOCKED = 1; mX.lockRead();
                        bcemt_ReadLockGuard<my_RWLock> l3(&mX, PRE_LOCKED);
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
                bcemt_ReadLockGuard<my_RWLock> l1(&mX);
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
            bcemt_ReadLockGuard<my_RWLock> mX(0);
        }
        {
            bcemt_ReadLockGuard<my_RWLock> mX(0);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
