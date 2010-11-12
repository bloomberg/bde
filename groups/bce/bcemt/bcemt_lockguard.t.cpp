// bcemt_lockguard.t.cpp                                              -*-C++-*-
#include <bcemt_lockguard.h>

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
// This program tests the functionality of the 'bcemt_LockGuard' class.  It
// verifies that the class properly locks the synchronization object at
// construction time, and that it properly unlocks the object at destruction
// time.  A helper class, 'my_Mutex', is created to facilitate the test
// process.  'my_Mutex' implements the required lock and unlock interfaces
// and provides a means to determine when the functions are called.
//
//-----------------------------------------------------------------------------
// bcemt_LockGuard
// ===============
// [2] bcemt_LockGuard();
// [2] ~bcemt_LockGuard();
// [2] release();
//
// bcemt_LockGuardUnlock
// =================
// [3] bcemt_LockGuardUnlock();
// [3] ~bcemt_LockGuardUnlock();
// [3] release();

// bcemt_LockGuardTryLock
// =================
// [4] bcemt_LockGuardTryLock();
// [4] ~bcemt_LockGuardTryLock();
// [4] release();
//-----------------------------------------------------------------------------
// [1] Ensure helper class 'my_Mutex' works as expected
// [5] INTERACTION BETW. 'bcemt_LockGuard' AND 'bcemt_LockGuardUnlock'
// [6] DEPRECATED 'bcemt_TryLockGuard' and 'bcemt_UnLockGuard'
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
int someOtherCondition = 0;

struct my_Mutex {
    // This class provides a simulated mutual exclusion mechanism which
    // conforms to the interface required by the bcemt_LockGuard class.
    // It operates using a counter to track the symbolic "locked" state.
    // Each call to the lock and unlock functions increment or decrement
    // the lock count respectively.  The current state of the lock count
    // is accessible through the lockCount method.  The 'tryLock' is designed
    // to fail the first time, then succeed every other time.

    int d_count;
    int d_attempt;

    my_Mutex() : d_count(0), d_attempt(1) {}
    ~my_Mutex() {};
    int lockCount() const { return d_count; }
    int tryLock() { if ((++d_attempt)%2) lock(); else return 1; return 0; }
    void lock() { ++ d_count; }
    void unlock() { --d_count; }
};

struct my_Object {
    void someMethod() {};
    void someOtherMethod() {};
    void defaultMethod() {};
};

// USAGE EXAMPLE

static void errorProneFunc(my_Object *obj, my_Mutex *mutex)
{
    mutex->lock();
    if (someCondition) {
        obj->someMethod();
        mutex->unlock();
        return;
    } else if (someOtherCondition) {
        obj->someOtherMethod();
        return;                              // MISTAKE! forgot to unlock mutex
    }
    obj->defaultMethod();
    mutex->unlock();
    return;
}

static void safeFunc(my_Object *obj, my_Mutex *mutex)
{
    bcemt_LockGuard<my_Mutex> guard(mutex);
    if (someCondition) {
        obj->someMethod();
        return;
    } else if (someOtherCondition) {
        obj->someOtherMethod();
        return;                          // OK, mutex is automatically unlocked
    }
    obj->defaultMethod();
    return;
}

static int safeButNonBlockingFunc(my_Object *obj, my_Mutex *mutex)
    // Perform task and return positive value if locking succeeds.
    // Return 0 if locking fails.
{
    const int RETRIES = 1; // use higher values for higher success rate
    bcemt_LockGuardTryLock<my_Mutex> guard(mutex, RETRIES);
    if (guard.ptr()) { // mutex is locked
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

void f(my_Mutex *mutex)
{
    bcemt_LockGuard<my_Mutex> guard(mutex);

    // critical section here

    {
        bcemt_LockGuardUnlock<my_Mutex> guard(mutex);

        // mutex is unlocked here

    } // mutex is locked again here

    // critical section here

} // mutex is unlocked here

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
        //   That 'bcemt_TryLockGuard' and 'bcemt_UnLockGuard' are equivalent
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
                bcemt_TryLockGuard<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
                ASSERT(0 == l1.ptr());
                {   // next will succeed
                    bcemt_TryLockGuard<my_Mutex> l2(&mX);
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                    {   // next will succeed at second attempt
                        bcemt_TryLockGuard<my_Mutex> l3(&mX,10);
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
                bcemt_TryLockGuard<my_Mutex> l1(&mX, 2);

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
                bcemt_UnLockGuard<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                {
                    const int PRE_UNLOCKED = 0;
                    bcemt_UnLockGuard<my_Mutex> l2(&mX, PRE_UNLOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                    {
                        const int PRE_UNLOCKED = 1; mX.unlock();
                        bcemt_UnLockGuard<my_Mutex> l3(&mX, PRE_UNLOCKED);
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
                bcemt_UnLockGuard<my_Mutex> l1(&mX);
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
        // INTERACTION BETWEEN 'bcemt_LockGuard' AND 'bcemt_LockGuardUnlock'
        //
        // Concern:
        //   That bcemt_LockGuard and bcemt_LockGuardUnlock interact together
        //   as expected.  That two different lock guards on two different
        //   'my_Mutex' objects do not interfere with each other.
        //
        // Plan:
        //   We verify that using two independent 'my_Mutex' objects with two
        //   distinct 'bcemt_LockGuard' *and* 'bcemt_LockGuardUnlock' objects
        //   in the same scope have no effect on each other.
        //
        // Testing:
        //   Interaction between lock and unlock guards, as well as
        //   between two lock guards on two different 'my_Mutex' objects.
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\tTesting interaction between "
                    "'bcemt_LockGuard' and bcemt_LockGuardUnlock'" << endl
                 << "=============================="
                    "========================================" << endl;

        {
            my_Mutex mX1;    const my_Mutex &X1 = mX1;
            my_Mutex mX2;    const my_Mutex &X2 = mX2;

            mX1.lock();
            {
                bcemt_LockGuard<my_Mutex> l1(&mX1);
                bcemt_LockGuard<my_Mutex> l2(&mX2);
                ASSERT(2 == X1.lockCount());
                ASSERT(1 == X2.lockCount());

                {
                    bcemt_LockGuardUnlock<my_Mutex> u1(&mX1);
                    bcemt_LockGuardUnlock<my_Mutex> u2(&mX2);
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
        // TESTING CLASS 'bcemt_LockGuardTryLock'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_LockGuardTryLock' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bcemt_LockGuardTryLock'
        //   objects using a common 'my_Mutex' object.  With each new object we
        //   verify that the lock function is called only if the constructor
        //   succeeds.  As each object is destroyed, we verify that the unlock
        //   function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_LockGuard' and calling 'release'.  We
        //   verify that the returned pointer matches the value we supplied
        //   only if the constructor succeeded in acquiring the lock.  We then
        //   verify that 'release' makes no attempt to unlock the supplied
        //   object and that when the 'bcemt_LockGuardTryLock' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bcemt_LockGuardTryLock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bcemt_LockGuardTryLock();
        //   ~bcemt_LockGuardTryLock();
        //   bcemt_LockGuardTryLock::release();
        //   bcemt_LockGuardTryLock::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bcemt_LockGuardTryLock(); \n"
                          << "\t  ~bcemt_LockGuardTryLock(); \n"
                          << "\t  bcemt_LockGuardTryLock::release(); \n"
                          << endl;

        {
            my_Mutex mX;    const my_Mutex &X = mX;

            if (verbose)
                cout << "\t\t trylock guard constructors & destructor" <<endl;
            {   // next will fail
                bcemt_LockGuardTryLock<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(0 == X.lockCount());
                ASSERT(0 == l1.ptr());
                {   // next will succeed
                    bcemt_LockGuardTryLock<my_Mutex> l2(&mX);
                    if(verbose)  P(X.lockCount());
                    ASSERT(1 == X.lockCount());
                    {   // next will succeed at second attempt
                        bcemt_LockGuardTryLock<my_Mutex> l3(&mX,10);
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
                bcemt_LockGuardTryLock<my_Mutex> l1(&mX, 2);

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
            bcemt_LockGuardTryLock<my_Mutex> mX(0, 6);
        }
        {
            bcemt_LockGuardTryLock<my_Mutex> mX(0, 6);
            mX.release();
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bcemt_LockGuardUnlock'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_LockUnGuard' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bcemt_LockGuardUnlock'
        //   objects using a common 'my_Mutex' object.  With each new object
        //   we verify that the unlock function is called.  As each object is
        //   destroyed, we verify that the lock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_LockGuardUnlock' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied.
        //   We then verify that 'release' makes no attempt to unlock
        //   the supplied object and that when the 'bcemt_LockGuardUnlock'
        //   object is destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bcemt_LockGuardUnlock' can be created
        //   with a null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bcemt_LockGuardUnlock();
        //   ~bcemt_LockGuardUnlock();
        //   bcemt_LockGuardUnlock::release();
        //   bcemt_LockGuardUnlock::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bcemt_LockGuardUnlock(); \n"
                          << "\t  ~bcemt_LockGuardUnlock(); \n"
                          << "\t  bcemt_LockGuardUnlock::release(); \n"
                          << endl;

        {
            my_Mutex mX;    const my_Mutex &X = mX;

            if (verbose)
                cout << "\t\t unlock guard constructor & destructor" <<endl;
            {
                bcemt_LockGuardUnlock<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(-1 == X.lockCount());
                {
                    const int PRE_UNLOCKED = 0;
                    bcemt_LockGuardUnlock<my_Mutex> l2(&mX, PRE_UNLOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(-2 == X.lockCount());
                    {
                        const int PRE_UNLOCKED = 1; mX.unlock();
                        bcemt_LockGuardUnlock<my_Mutex> l3(&mX, PRE_UNLOCKED);
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
                bcemt_LockGuardUnlock<my_Mutex> l1(&mX);
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
            bcemt_LockGuardUnlock<my_Mutex> mX(0);
        }
        {
            bcemt_LockGuardUnlock<my_Mutex> mX(0);
            mX.release();
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'bcemt_LockGuard'
        //
        // Concern:
        //   That the basic functionality of the 'bcemt_LockGuard' class
        //   template is correct.
        //
        // Plan:
        //   We begin by creating a series of nested 'bcemt_LockGuard'
        //   objects using a common 'my_Mutex' object.  With each new object
        //   we verify the the lock function was called.  As each object is
        //   destroyed, we verify that the unlock function is called.
        //
        //   Next, we verify that the 'release' function works properly by
        //   constructing a new 'bcemt_LockGuard' and calling 'release'.
        //   We verify that the returned pointer matches the value we supplied.
        //   We then verify that 'release' makes no attempt to unlock
        //   the supplied object and that when the 'bcemt_LockGuard' object is
        //   destroyed, it does not unlock the object.
        //
        //   Finally we test that a 'bcemt_LockGuard' can be created with a
        //   null lock, and that 'release' may be called on the guard.
        //
        // Testing:
        //   bcemt_LockGuard();
        //   ~bcemt_LockGuard();
        //   bcemt_LockGuard::release();
        //   bcemt_LockGuard::ptr();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\tTesting: \n"
                          << "\t  bcemt_LockGuard(); \n"
                          << "\t  ~bcemt_LockGuard(); \n"
                          << "\t  bcemt_LockGuard::release(); \n" << endl;

        {
            my_Mutex mX;    const my_Mutex &X = mX;

            if (verbose)
                cout << "\t\t lock guard constructor & destructor" <<endl;
            {
                bcemt_LockGuard<my_Mutex> l1(&mX);
                if(verbose)  P(X.lockCount());
                ASSERT(1 == X.lockCount());
                {
                    const int PRE_LOCKED = 0;
                    bcemt_LockGuard<my_Mutex> l2(&mX, PRE_LOCKED);
                    if(verbose)  P(X.lockCount());
                    ASSERT(2 == X.lockCount());
                    {
                        const int PRE_LOCKED = 1; mX.lock();
                        bcemt_LockGuard<my_Mutex> l3(&mX, PRE_LOCKED);
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
                bcemt_LockGuard<my_Mutex> l1(&mX);
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
            bcemt_LockGuard<my_Mutex> mX(0);
        }
        {
            bcemt_LockGuard<my_Mutex> mX(0);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
