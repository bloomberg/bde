// bcemt_threadimpl_pthread.t.cpp            -*-C++-*-

#include <bcemt_threadimpl_pthread.h>

#include <bces_platform.h>
#include <bdetu_systemtime.h>
#include <iostream>
#include <cstring>  // strcmp()
#include <cstdlib>  // atoi()

#ifdef BCES_PLATFORM__POSIX_THREADS
#include <pthread.h>
#endif

using namespace BloombergLP;
using namespace std;


//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// All of the classes defined in this component are simple wrappers around the
// pthreads interfaces.  We will perform a minimal test to ensure that our
// methods are hooked up correctly.
//-----------------------------------------------------------------------------
//
// bcemt_AttributeImpl
// --------------------
// [ 2] bcemt_AttributeImpl();
// [ 3] bcemt_AttributeImpl(const bcemt_AttributeImpl& attribute);
// [ 2] ~bcemt_AttributeImpl();
// [ 3] bcemt_AttributeImpl& operator=(const bcemt_AttributeImpl& rhs);
// [ 2] void setDetachedState(DetachedState detachedState);
// [ 2] void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
// [ 2] void setSchedulingPriority(int schedulingPriority);
// [ 2] void setInheritSchedule(int inheritSchedule);
// [ 2] void setStackSize(int stackSize);
// [ 2] void setGuardSize(int guardSize);
// [ 2] DetachedState detachedState() const;
// [ 2] SchedulingPolicy schedulingPolicy() const;
// [ 2] int schedulingPriority() const;
// [ 2] int inheritSchedule() const;
// [ 2] int stackSize() const;
// [ 2] int guardSize() const;
// [ 3] int operator==(const bcemt_AttributeImpl<bces_Platform::Posix...
// [ 3] int operator!=(const bcemt_AttributeImpl<bces_Platform::Posix...
// [ 2] static int getMinSchedPriority(int policy = -1)
// [ 2] static int getMaxSchedPriority(int policy = -1)
//
// bcemt_ThreadUtilImpl
// ---------------------
// [ 6] static int create(Handle *thread, const bcemt_AttributeImpl ...
// [ 6] static int create(Handle *thread, bcemt_ThreadFunction function,..
// [ 6] static int join(Handle &thread, void **status = 0);
// [ 6] static void yield();
// [ 6] static void sleep(const bdet_TimeInterval &sleepTime);
// [ 6] static void microSleep(int microseconds, int seconds=0);
// [ 6] static void exit(void *status);
// [ 6] static Handle self();
// [ 6] static int detach(Handle &handle);
// [ 6] static NativeHandle nativeHandle(const Handle &thread);
// [ 6] static int isEqual(const Handle &lhs, const Handle &rhs);
// [ 6] static Id selfId();
// [ 6] static int selfIdAsInt();
// [ 6] static int isEqualId(const Id &lhs, const Id &rhs);
// [ 7] static int createKey(Key *key, Destructor destructor );
// [ 7] static int deleteKey(Key &key);
// [ 7] static void* getSpecific(const Key &key);
// [ 7] static int setSpecific(const Key &key, const void * value);
// [ 6] static int getConcurrency();
// [ 6] static void setConcurrency(int newLevel);
//
// bcemt_MutexImpl
// ---------------
// [ 4] bcemt_MutexImpl();
// [ 4] ~bcemt_MutexImpl();
// [ 4] void lock();
// [ 4] int tryLock();
// [ 4] void unlock();
//
// bcemt_RecursiveMutexImpl
// ------------------------
// [ 4] bcemt_RecursiveMutexImpl();
// [ 4] ~bcemt_RecursiveMutexImpl();
// [ 4] void lock();
// [ 4] int tryLock();
// [ 4] void unlock();
//
// bcemt_ConditionImpl
// -------------------
// [ 5] bcemt_ConditionImpl();
// [ 5] ~bcemt_ConditionImpl();
// [ 5] int wait(bcemt_MutexImpl<bces_Platform::PosixThreads> *mutex);
// [ 5] int timedWait(bcemt_MutexImpl<bces_Platform::PosixThreads> *mutex);
// [ 5] int signal();
// [ 5] int broadcast();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BCES_PLATFORM__POSIX_THREADS

typedef bcemt_AttributeImpl<bces_Platform::PosixThreads> TA;
typedef bcemt_ThreadUtilImpl<bces_Platform::PosixThreads> TU;
typedef bcemt_MutexImpl<bces_Platform::PosixThreads> MX;
typedef bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads> RMX;
typedef bcemt_ConditionImpl<bces_Platform::PosixThreads> CD;

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------


struct my_ThreadTestArg {
    // This structure defines a general set or arguments used to communicate
    // with test functions.
    CD           *d_startCond_p;
    CD           *d_stopCond_p;
    MX           *d_mutex_p;
    RMX          *d_rmutex_p;
    TU::Key      *d_key_p;
    volatile long d_startCount;
    volatile long d_stopCount;
    volatile long d_keyDestructCount;

    my_ThreadTestArg( CD *startCond=0, CD *stopCond=0,
                      MX *mutex=0, TU::Key *key=0, RMX *rmutex=0)
    : d_startCond_p(startCond)
    , d_stopCond_p(stopCond)
    , d_mutex_p(mutex)
    , d_rmutex_p(rmutex)
    , d_key_p(key)
    , d_startCount(0)
    , d_stopCount(0)
    , d_keyDestructCount(0)
    {
    }

    ~my_ThreadTestArg()
    {
    }
};

void myMilliSleep(int milliseconds)
{
    timespec naptime;
    naptime.tv_sec = milliseconds/1000;
    naptime.tv_nsec = (milliseconds%1000) * 1000000;
    nanosleep(&naptime, 0);
}

extern "C" {

void *testDetach(void *ptr)
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    arg->d_mutex_p->lock();
    ++arg->d_startCount;
    arg->d_mutex_p->unlock();

    while (2 != arg->d_startCount);

    arg->d_mutex_p->lock();
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();

    while (2 != arg->d_stopCount);

    return arg;
}


void *testSelfDetach(void *ptr)
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    TU::Handle self = TU::self();
    TU::detach(self);
    
    arg->d_mutex_p->lock();
    ++arg->d_startCount;
    arg->d_mutex_p->unlock();
    while (2 != arg->d_startCount);

    arg->d_mutex_p->lock();
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    while (2 != arg->d_stopCount);

    return arg;
}

void *testThreadFunc1(void *ptr)
    // This function is used to test certain thread related conditions.  It
    // begin by locking the provided mutex and increment the 'd_startCount'.
    // It will then signal 'd_startCond_p' if provided.  Next it will wait for
    // 'd_stopCond_p' to be signaled if provided.  Finally it will increment
    // the 'd_stopCount' and return the pointer to the specified argument.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_mutex_p);

    arg->d_mutex_p->lock();
    ++arg->d_startCount;
    if (arg->d_startCond_p) arg->d_startCond_p->signal();
    if (arg->d_stopCond_p) arg->d_stopCond_p->wait(arg->d_mutex_p);
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    return arg;
}

void *testThreadFunc2(void *ptr)
    // This function is used to test certain thread related conditions.  It
    // begin by locking the provided mutex and increment the 'd_startCount'.
    // It will then signal 'd_startCond_p' if provided.  Next it will wait for
    // 'd_stopCond_p' to be signaled if provided.  Finally it will increment
    // the 'd_stopCount' and call the 'exit' function to exit the thread with
    // the pointer to the specified argument and it's exit value.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_mutex_p);

    arg->d_mutex_p->lock();
    ++arg->d_startCount;
    if (arg->d_startCond_p) arg->d_startCond_p->signal();
    if (arg->d_stopCond_p) arg->d_stopCond_p->wait(arg->d_mutex_p);
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    TU::exit(arg);
    return 0;
}

void *testThreadFunc3(void *)
    // This function is used to test the self join functionality.  It simply
    // tries to join on it's self and asserts that the request fails.
{
    TU::Handle self = TU::self();
    ASSERT(0 != TU::join(self));
    return 0;
}

void *testThreadFunc4(void *ptr)
    // This function is the same as 'testThreadFunc2' with the exception that
    // it detaches its self.  It is used to the self detach and join.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_mutex_p);

    arg->d_mutex_p->lock();
    TU::Handle self = TU::self();
    TU::detach(self);
    ++arg->d_startCount;
    if (arg->d_startCond_p) arg->d_startCond_p->signal();
    if (arg->d_stopCond_p) arg->d_stopCond_p->wait(arg->d_mutex_p);
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    TU::exit(arg);
    return (void*)0;
}

void *testThreadSpecificFunc1(void *ptr)
    // This function is used to test thread specific storage.  It begins by
    // locking the provided mutex and incrementing the 'd_startCount'.  It will
    // then signal 'd_startCond_p' if provided.  Next it get the current value
    // associated with the provided thread specific key.  It asserts the the
    // initial value is zero.  Then it associates a value with the key and
    // again assert that the value is is correct. Next it will wait for
    // 'd_stopCond_p' to be signaled if provided.  Finally it will increment
    // the 'd_stopCount' and return the pointer to the specified argument.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_mutex_p);

    arg->d_mutex_p->lock();
    ++arg->d_startCount;
    if (arg->d_startCond_p) arg->d_startCond_p->signal();

    ASSERT(0 == TU::getSpecific(*arg->d_key_p));
    ASSERT(0 == TU::setSpecific(*arg->d_key_p, arg));
    ASSERT(arg == TU::getSpecific(*arg->d_key_p));

    if (arg->d_stopCond_p) arg->d_stopCond_p->wait(arg->d_mutex_p);
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    return arg;
}

void *testThreadSpecificFunc2(void *ptr)
    // This function is the same as 'testThreadSpecificFunc2' with the
    // exception that it exits using the explicit 'exit' call.  It it used to
    // verify that thread specific destructors behave as expected even when
    // a thread exits using the 'exit' call.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_mutex_p);

    arg->d_mutex_p->lock();
    ++arg->d_startCount;
    if (arg->d_startCond_p) arg->d_startCond_p->signal();

    ASSERT(0 == TU::getSpecific(*arg->d_key_p));
    ASSERT(0 == TU::setSpecific(*arg->d_key_p, arg));
    ASSERT(arg == TU::getSpecific(*arg->d_key_p));

    if (arg->d_stopCond_p) arg->d_stopCond_p->wait(arg->d_mutex_p);
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    TU::exit(arg);
    return 0;
}

void testThreadSpecicDestructor1(void *ptr)
    // This function implements a thread specific destructor function.  It is
    // used in test thread specific destructors.  It assumes that the key value
    // which it is associated with is a 'my_ThreadTestArg' structure.  When the
    // destructor is invoked, it locks the provided mutex, increments the
    // 'd_keyDestructCound' variable,  and unlocks the mutex.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_mutex_p);

    arg->d_mutex_p->lock();
    ++arg->d_keyDestructCount;
    arg->d_mutex_p->unlock();
}

void* testMutexFunc1(void *ptr)
    // This function is used to test mutex objects.  It assumes that the
    // provided argument is a pointer to a mutex object.  It simply locks the
    // mutex, then unlocks it and returns 0.
{

    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_mutex_p);
    ++arg->d_startCount;
    arg->d_mutex_p->lock();
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    return ptr;
}

void* testMutexFunc2(void *ptr)
    // This function is used to test mutex objects.  It assumes that the
    // provided argument is a pointer to a mutex object.  It loops the tryLock
    // function until it is able to aquire the lock, then unlocks it and
    // returns 0.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_mutex_p);

    ++arg->d_startCount;
    while (arg->d_mutex_p->tryLock());
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    return ptr;
}

void*  testRMXFunc1(void *ptr)
    // This function is used to test recursive mutex objects.  It assumes that
    // the provided argument is a pointer to a critical section object.  It
    // simply locks the mutex, then unlocks it and returns 0.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_rmutex_p);

    ++arg->d_startCount;
    arg->d_rmutex_p->lock();
    ++arg->d_stopCount;
    arg->d_rmutex_p->unlock();
    return ptr;
}

void* testRMXFunc2(void *ptr)
    // This function is used to test recursive mutex objects.  It assumes that
    // the provided argument is a pointer to a mutex object.  It loops the
    // tryLock function until it is able to aquire the lock, then unlocks it
    // and returns 0.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    ASSERT(0 != arg->d_rmutex_p);

    ++arg->d_startCount;
    while (arg->d_rmutex_p->tryLock());
    ++arg->d_stopCount;
    arg->d_rmutex_p->unlock();
    return ptr;
}

void* testConditionFunc1(void *ptr)
    // This function is used to test condition variable objects.  It assumes
    // that the provided argument is a pointer to a 'my_ThreadTestArg'
    // structure. It begins by locking the provided mutex and incrementing the
    // 'd_startCount'.  Then it waits on the 'd_startCond_p' condition variable
    // to be signaled, increments the 'd_stopCount' variable, unlocks the
    // mutex, and returns 0.
{
    my_ThreadTestArg *arg = (my_ThreadTestArg*)ptr;

    ASSERT(0 != arg);
    arg->d_mutex_p->lock();
    ++arg->d_startCount;
    arg->d_stopCond_p->wait(arg->d_mutex_p);
    ++arg->d_stopCount;
    arg->d_mutex_p->unlock();
    return ptr;
}


} // extern "C" {


#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

#ifdef BCES_PLATFORM__POSIX_THREADS
    TU::setConcurrency(25);
#endif

    switch (test) {
#ifdef BCES_PLATFORM__POSIX_THREADS
      case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // TESTING thread specific storage
        //   Verify that thread specific storage functions are hooked up
        //   properly to their respective pthread functions.
        //
        // Plan:
        //   Begin by bootstraping the functions to assert basic functionality.
        //   Then create a key with a destructor function.  Spawn a new thread
        //   that associates a value with the key and exits.  Assert tht the
        //   destructor function is called.  Next repeat the process using a
        //   thread the exits using the explicit 'exit' call and assert that
        //   the destructor is still called.  Finally repeat the process again,
        //   but this time delete the key before the thread exits and assert
        //   that the destructor is not called.
        //
        // Testing:
        //   int createKey(Key *key, Destructor destructor);
        //   int deleteKey(Key &key);
        //   void* getSpecific(const Key &key);
        //   int setSpecific(const Key &key, const void * value);
        //--------------------------------------------------------------------

        if (verbose) cout << "Testing: thread specific storage" << endl
                          << "================================" << endl;

        if (veryVerbose) cout <<
                           "\tTesting: basic thread specific storage"
                          << endl <<
                           "\t--------------------------------------" << endl;

        {
            const int NKEYS=5;

            TU::Key keys[NKEYS];
            for (int i=0; i < NKEYS; ++i) {
                TU::Key &key=keys[i];

                LOOP_ASSERT(i, 0 == TU::createKey(&key,0));
                LOOP_ASSERT(i, 0 == TU::getSpecific(key));

                LOOP_ASSERT(i, 0 == TU::setSpecific(key,&key));
                LOOP_ASSERT(i, &key == TU::getSpecific(key));
            }
            for (int i=0; i < NKEYS; ++i) {
                LOOP_ASSERT(i, &keys[i] == TU::getSpecific(keys[i]));
            }
            for (int i=0; i < NKEYS; ++i) {
                LOOP_ASSERT(i, 0 == TU::deleteKey(keys[i]));
            }
        }

        if (veryVerbose) cout << "\tTesting: key destructors" << endl
                              << "\t------------------------" << endl;

        {
            TU::Key key;
            ASSERT(0 == TU::createKey(&key,testThreadSpecicDestructor1));
#ifndef BDES_PLATFORM__OS_LINUX
            // Linuxthreads has a bug here.  NPTL works fine though.

            ASSERT(0 == TU::getSpecific(key));
#endif

            MX mutex;
            TA attr;
            TU::Handle h;

            my_ThreadTestArg arg(0,0,&mutex,&key);

            ASSERT(0 == TU::create(&h, attr, testThreadSpecificFunc1, &arg) );
            ASSERT(0 == TU::join(h));
            ASSERT(1 == arg.d_startCount);
            ASSERT(1 == arg.d_stopCount);
            ASSERT(1 == arg.d_keyDestructCount);
            ASSERT(0 == TU::deleteKey(key));
        }

        if (veryVerbose) cout << "\tTesting: key destructors(with exit)"
                              << endl
                              << "\t-----------------------------------"
                              << endl;
        {
            TU::Key key;
            ASSERT(0 == TU::createKey(&key,testThreadSpecicDestructor1));
            ASSERT(0 == TU::getSpecific(key));

            MX mutex;
            TA attr;
            TU::Handle h;

            my_ThreadTestArg arg(0,0,&mutex,&key);

            ASSERT(0 == TU::create(&h, attr, testThreadSpecificFunc2, &arg) );
            ASSERT(0 == TU::join(h));
            ASSERT(1 == arg.d_startCount);
            ASSERT(1 == arg.d_stopCount);
            ASSERT(1 == arg.d_keyDestructCount);
            ASSERT(0 == TU::deleteKey(key));
        }

        if (veryVerbose) cout << "\tTesting: key destructors(deletekey)"
                              << endl
                              << "\t-----------------------------------"
                              << endl;
        {
            TU::Key key;
            ASSERT(0 == TU::createKey(&key,testThreadSpecicDestructor1));
            ASSERT(0 == TU::getSpecific(key));

            MX mutex;
            TA attr;
            CD stopCond;
            CD startCond;
            TU::Handle h;

            my_ThreadTestArg arg(&startCond,&stopCond,&mutex,&key);

            mutex.lock();
            ASSERT(0 == TU::create(&h, attr, testThreadSpecificFunc1, &arg) );
            startCond.wait(&mutex);
            mutex.unlock();

            ASSERT(0 == TU::deleteKey(key));
            stopCond.signal();

            ASSERT(0 == TU::join(h));
            ASSERT(1 == arg.d_startCount);
            ASSERT(1 == arg.d_stopCount);
            ASSERT(0 == arg.d_keyDestructCount);
        }
      }break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING thread creation and management
        //   Test the thead creation and related functions.
        //
        // Plan:
        //   1. Test 'create' by creating a thread using the "detached"
        //   attribute.   Wait for the thread to start and signal the specified
        //   condition variable. Assert that the thread ran correctly and
        //   incremented the start count.  Then repeat the process calling
        //   'create' with no attribute argument.  Assert the the again the
        //   thread ran correctly
        //
        //   2. Test 'join' by creating a joinable thread and then waiting for
        //   the thread to start and signal the specified condition variable.
        //   Assert that the thread ran correctly and then 'join' with it.
        //   Assert that the 'join' succeeds and that the thread exit value
        //   is the pointer to the argument that was specified at thread
        //   creation. Next repeat the process using a non-joinable("detached")
        //   thread and verify that 'join' returns an error.  Finally verify
        //   'join' fails when the provided thread argument is the current
        //   thread.
        //
        //   3. To test 'detach', create a joinable thread, detach it and 
        //   assert that it is no longer joinalble.  To test "self detach",
        //   create a joinable thread, wait for it to detach itself, 
        //   and assert that it is no longer joinalble.  
        // 
        //   4. Test 'exit' by creating a joinable thread that will exit with
        //   an exit value of the its pointer argument.  Assert that join works
        //   correctly on the thead and that the thread exit value is the
        //   expected value.
        //
        //   5. Test 'yield' just by tring to call it 
        //
        //   6. Test 'sleep' by suspending the current thread for some period 
        //   of time.  Using the system time, determine the current time
        //   immediately before the call to 'sleep' and immediately after.
        //   Verify that the elapsed time is within the expected range.
        //
        //   7. Test 'nativeHandle' by creating a thread and determining the
        //   native handle using 'nativeHandle' and joining with the thread
        //   by directly calling 'pthread_join' using the native handle.
        //
        //   8. Test 'selfId' by directly calling 'pthread_equal' with the
        //   result of 'pthread_self' and the id returned by 'selfId'.  Verify
        //   that the two id values are equal.  Test 'selfIdAsInt' by comparing
        //   its return value with 'selfId' returned value cast as an 'int'.
        //
        //   9. For a sequence of independent test values, set the concurency
        //   level.  Assert that the result of 'getConcurrency' is equal to
        //   the set level, or -1 if the function is not supported on this
        //   platform.
        //
        // Testing:
        //   int create(Handle *thread, ...
        //   int join(Handle &thread, void **status = 0);
        //   void sleep(const bdet_TimeInterval &sleepTime);
        //   void microSleep(int microseconds, int seconds=0);
        //   void exit(void *status);
        //   Handle self();
        //   NativeHandle nativeHandle(const Handle &thread);
        //   Id selfId();
        //   int selfIdAsInt();
        //   int isEqualId(const Id &lhs, const Id &rhs);
        //--------------------------------------------------------------------

        if (verbose) cout << "Testing: thread creation and management" << endl
                          << "=======================================" << endl;

        if (veryVerbose) cout << "\tTesting: create" << endl
                              << "\t---------------" << endl;

        {
            MX mutex;
            CD startCond;
            CD stopCond;
            TA attr;
            TU::Handle h;
            int res;

            my_ThreadTestArg arg(&startCond,0,&mutex);
            attr.setDetachedState(TA::CREATE_DETACHED);

            ASSERT(0 == arg.d_startCount);
            ASSERT(0 == arg.d_stopCount);
            mutex.lock();

            res = TU::create(&h, attr, testThreadFunc1, &arg);
            ASSERT(0 == res);
            startCond.wait(&mutex);
            mutex.unlock();

            ASSERT(1 == arg.d_startCount);
            if (veryVerbose) {
                T_(); P(arg.d_startCount);
            }
        }

        if (veryVerbose) cout << "\tTesting: create(default attributes)"
                              << endl
                              << "\t-----------------------------------"
                              << endl;

        {
            MX mutex;
            CD startCond;
            CD stopCond;
            TU::Handle h;

            my_ThreadTestArg arg(&startCond,0,&mutex);

            ASSERT(0 == arg.d_startCount);
            ASSERT(0 == arg.d_stopCount);
            mutex.lock();

            ASSERT(0 == TU::create(&h, testThreadFunc1, &arg) );
            startCond.wait(&mutex);
            mutex.unlock();

            ASSERT(1 == arg.d_startCount);
            if (veryVerbose) {
                T_(); P(arg.d_startCount);
            }
            ASSERT(0 == TU::join(h));
        }

        if (verbose) cout << "\tTesting: join" << endl
                          << "\t-------------" << endl;
        {
            MX mutex;
            CD startCond;
            CD stopCond;
            TA attr;
            TU::Handle h;

            my_ThreadTestArg arg(&startCond,0,&mutex);

            ASSERT(0 == arg.d_startCount);
            ASSERT(0 == arg.d_stopCount);
            mutex.lock();

            ASSERT(0 == TU::create(&h, attr, testThreadFunc1, &arg));
            startCond.wait(&mutex);
            mutex.unlock();

            ASSERT(1 == arg.d_startCount);
            void *exitStatus;
            int res;
            res = TU::join(h,&exitStatus);
            ASSERT(0 == res);
            ASSERT(&arg == exitStatus);

            if ( veryVerbose ) {
                T_(); P_(arg.d_startCount); P_(res); P_(&arg); P(exitStatus);
            }

        }

        if (verbose) cout << "\tTesting: join detached" << endl
                          << "\t----------------------" << endl;
        {
            MX mutex;
            CD startCond;
            CD stopCond;
            TA attr;
            TU::Handle h;

            my_ThreadTestArg arg(&startCond,0,&mutex);
            attr.setDetachedState(TA::CREATE_DETACHED);

            ASSERT(0 == arg.d_startCount);
            ASSERT(0 == arg.d_stopCount);
            mutex.lock();

            ASSERT(0 == TU::create(&h, attr, testThreadFunc1, &arg));
            startCond.wait(&mutex);
            mutex.unlock();

            ASSERT(1 == arg.d_startCount);
            void *exitStatus;
            int res = TU::join(h,&exitStatus);
            ASSERT(0 != res);
            if (veryVerbose) {
                T_(); P(res);
            }
        }

        if (verbose) cout << "\tTesting: self join " << endl
                          << "\t-------------------" << endl;
        {
            TA attr;
            TU::Handle h;
            TU::Handle self = TU::self();
            ASSERT(0 != TU::join(self));
            ASSERT(0 == TU::create(&h, attr, testThreadFunc3, 0));
            ASSERT(0 == TU::join(h,0));
        }


        if (verbose) cout << "\tTesting: detach" << endl
                          << "\t---------------" << endl;
        {
            MX mutex;
            CD startCond;
            CD stopCond;
            TA attr;
            TU::Handle h;
            void *exitStatus;

            my_ThreadTestArg arg(&startCond,&stopCond,&mutex);
            ASSERT(0 == TU::create(&h, attr, testDetach, &arg));

            mutex.lock();
            ++arg.d_startCount;
            mutex.unlock();
            while (2 != arg.d_startCount);

            ASSERT(0 == TU::detach(h));
            ASSERT(0 != TU::join(h,&exitStatus));
            
            mutex.lock();
            ++arg.d_stopCount;
            mutex.unlock();
            while (2 != arg.d_stopCount);  
        }

        if (verbose) cout << "\tTesting: self detach" << endl
                          << "\t--------------------" << endl;
        {
            MX mutex;
            CD startCond;
            CD stopCond;
            TA attr;
            TU::Handle h;
            void *exitStatus;

            my_ThreadTestArg arg(&startCond,&stopCond,&mutex);
            ASSERT(0 == TU::create(&h, attr, testSelfDetach, &arg));

            mutex.lock();
            ++arg.d_startCount;
            mutex.unlock();
            while (2 != arg.d_startCount);

            ASSERT(0 != TU::join(h,&exitStatus));
            
            mutex.lock();
            ++arg.d_stopCount;
            mutex.unlock();
            while (2 != arg.d_stopCount);
        }

        if (verbose) cout << "\tTesting: exit" << endl
                          << "\t-------------" << endl;
        {
            MX mutex;
            CD startCond;
            TA attr;
            TU::Handle h;

            my_ThreadTestArg arg(&startCond,0,&mutex);

            ASSERT(0 == arg.d_startCount);
            ASSERT(0 == arg.d_stopCount);
            mutex.lock();

            ASSERT(0 == TU::create(&h, attr, testThreadFunc2, &arg) );
            startCond.wait(&mutex);
            mutex.unlock();
            void *exitStatus;
            ASSERT(0 == TU::join(h,&exitStatus));
            ASSERT(&arg == exitStatus);
        }
        
        if (verbose) cout << "\tTesting: yield" << endl
                          << "\t--------------" << endl;
        {
            TU::yield();
        }

        if (verbose) cout << "\tTesting: sleep" << endl
                          << "\t--------------" << endl;
        {
            bdet_TimeInterval start,duration;

            start = bdetu_SystemTime::now();
            TU::sleep(bdet_TimeInterval(0, 1000000));
            duration = bdetu_SystemTime::now()-start;

            // assume some error margin            
            ASSERT(bdet_TimeInterval(0,1000000 - 500000) <= duration);
            ASSERT(bdet_TimeInterval(1,1000000 + 500000) > duration);
            if (veryVerbose) {
              T_(); P_(start); P(duration);
            }
        }

        if (verbose) cout << "\tTesting: microSleep" << endl
                          << "\t-------------------" << endl;
        {
            bdet_TimeInterval start,duration;

            start = bdetu_SystemTime::now();
            TU::microSleep(100000);
            duration = bdetu_SystemTime::now()-start;

            // assume some error margin
            ASSERT(bdet_TimeInterval(0,(100000-50000)*1000) <= duration);
            ASSERT(bdet_TimeInterval(0,(100000+50000)*1000) > duration);
            if (veryVerbose) {
              T_(); P_(start); P(duration);
            }
        }

        if (verbose) cout << "\tTesting: nativeHandle" << endl
                          << "\t------------------------" << endl;
        {
            MX mutex;
            CD startCond;
            CD stopCond;
            TA attr;
            TU::Handle h;
            TU::NativeHandle nh;

            my_ThreadTestArg arg(&startCond,&stopCond,&mutex);

            ASSERT(0 == arg.d_startCount);
            ASSERT(0 == arg.d_stopCount);

            mutex.lock();
            ASSERT(0 == TU::create(&h, attr, testThreadFunc2, &arg)) ;
            nh = TU::nativeHandle(h);
            startCond.wait(&mutex);
            mutex.unlock();

            myMilliSleep(1); myMilliSleep(1);
            stopCond.signal();

            ASSERT(0 == pthread_join(nh,0));
        }

        if (verbose) cout << "\tTesting: selfId/selfIdAsInt" << endl
                          << "\t---------------------------" << endl;
        {
            TU::Id id = TU::selfId();
            ASSERT( 0 != TU::isEqualId(pthread_self(),id));
            ASSERT( 0 != pthread_equal(pthread_self(),id));
            ASSERT(TU::selfIdAsInt() == (int) id);
            if (veryVerbose) {
              T_(); P_(pthread_self()); P(id);
            }
        }

        if (veryVerbose) cout << "\tTesting: setConcurrency/getConcurrency"
                              << endl
                              << "\t---------------------------------------"
                              << endl;
        {
            static const struct {
                int                  d_lineNum;
                int                  d_level;
            } VALUES[] = {
                //line level
                //---- -----
                { L_ , 1    },
                { L_ , 5    },
                { L_ , 7    },
                { L_ , 9    },
                { L_ , 10   },
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            for (int i=0; i < NUM_VALUES; ++i) {
                const int LINE = VALUES[i].d_lineNum;
                const int LEVEL = VALUES[i].d_level;
                TU::setConcurrency(LEVEL);
                LOOP_ASSERT(LINE, LEVEL == TU::getConcurrency()
                            || -1 == TU::getConcurrency());
            }

        }

      }break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING bcemt_ConditionImpl
        //   Verify that condition variables are properly "hooked-up" to the
        //   the appropriate pthread functions.
        //
        // Plan:
        //   Begin by perfroming a basic test of methods(except wait since we
        //   would block).  Then create a condtion and two threads to wait on
        //   the condition.  Assert that both threads are blocked waiting for
        //   the condition.  Next signal the condiion and assert that one and
        //   only one thread was awakened.  Then signal the condtion again and
        //   assert that the last thread was wakenedd it.
        //
        //   Next verify that broadcast works correctly, by again creating two
        //   threads to wait on a condition.  Assert that both threads are
        //   blocked on waiting for the condition.   Finally broadcast the
        //   condition and assert that both threads are awakened.
        //
        // Testing:
        //   bcemt_ConditionImpl();
        //   ~bcemt_ConditionImpl();
        //   int wait(bcemt_MutexImpl<bces_Platform::PosixThreads> *mutex);
        //   int timedWait(bcemt_MutexImpl...
        //   int signal();
        //   int broadcast();
        //--------------------------------------------------------------------

        if (verbose) cout << endl
             << "Testing: bcemt_ConditionImpl" << endl
             << "============================" << endl;
        {
            CD cd;
            MX mx;

            mx.lock();
            ASSERT(0 != cd.timedWait(&mx,
                              bdetu_SystemTime::now().addMilliseconds(1)));
            cd.signal();
            ASSERT(0 != cd.timedWait(&mx,
                              bdetu_SystemTime::now().addMilliseconds(1)));
            cd.broadcast();
            ASSERT(0 != cd.timedWait(&mx,
                              bdetu_SystemTime::now().addMilliseconds(1)));
            mx.unlock();
        }

        if (veryVerbose) cout << "\tTesting: signal" << endl
                              << "\t---------------" << endl;
        {
            MX mutex;
            CD cond;
            pthread_t handles[2];
            my_ThreadTestArg arg(0,&cond,&mutex);

            ASSERT(0 == pthread_create(&handles[0], 0,
                                       testConditionFunc1, &arg));
            ASSERT(0 == pthread_create(&handles[1], 0,
                                       testConditionFunc1, &arg));

            while(2 != arg.d_startCount);

            enum { NUM_CHECK = 50 };
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            myMilliSleep(1); myMilliSleep(1);
            cond.signal();
            while(1 != arg.d_stopCount);
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(1 == arg.d_stopCount);

            myMilliSleep(1); myMilliSleep(1);
            cond.signal();
            while(2 != arg.d_stopCount);

            pthread_join(handles[0], 0);
            pthread_join(handles[1], 0);
        }

        if (veryVerbose) cout << "\tTesting: broadcast" << endl
                              << "\t------------------" << endl;
        {
            MX mutex;
            CD cond;
            pthread_t handles[2];
            my_ThreadTestArg arg(0,&cond,&mutex);

            ASSERT(0 == pthread_create(&handles[0],0,testConditionFunc1,&arg));
            ASSERT(0 == pthread_create(&handles[1],0,testConditionFunc1,&arg));

            while(2 != arg.d_startCount);

            enum { NUM_CHECK = 50 };
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            myMilliSleep(1); myMilliSleep(1);myMilliSleep(1); 
            cond.broadcast();
            while(2 != arg.d_stopCount);

            pthread_join(handles[0], 0);
            pthread_join(handles[1], 0);
        }

      }break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING bcemt_MutexImpl and bcemt_RecursiveMutex
        //   Test that the mutexes are properly "hooked-up" to the appropriate
        //   pthread functions.
        //
        // Plan:
        //   For each class, begin by perfroming a basic test of each method.
        //   Next, create a new object and lock it.  Spawn a thread and pass
        //   it the address of the object.  The thread tries to lock the type,
        //   unlock it, and exit.  Assert the thread is blocked attempting to
        //   lock the object.  Then release the obejct and assert that thread
        //   aquires the lock and exits.
        //
        // Testing:
        //   bcemt_MutexImpl
        //   ---------------
        //   bcemt_MutexImpl();
        //   ~bcemt_MutexImpl();
        //   void lock();
        //   int tryLock();
        //   void unlock();
        //
        //   bcemt_RecursiveMutexImpl
        //   ------------------------
        //   bcemt_RecursiveMutexImpl();
        //   ~bcemt_RecursiveMutex();
        //   void lock();
        //   int tryLock();
        //   void unlock();
        //--------------------------------------------------------------------

        if (verbose) cout << endl
             << "Testing: bcemt_MutexImpl" << endl
             << "========================" << endl;


        {
            MX mx;

            mx.lock();
            mx.unlock();

            ASSERT(0 == mx.tryLock());
            mx.unlock();
        }

        if (veryVerbose) cout << "\tTesting: mutex lock" << endl
                              << "\t-------------------" << endl;
        {
            MX mx;
            pthread_t h;
            my_ThreadTestArg arg(0,0,&mx);

            mx.lock();
            ASSERT(0 == pthread_create(&h,0,testMutexFunc1,&arg));
            while(1 != arg.d_startCount);
            
            enum { NUM_CHECK = 50 };
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);
            
            mx.unlock();
            while(1 != arg.d_stopCount);
            pthread_join(h, 0);
        }

        if (veryVerbose) cout << "\tTesting: mutex tryLock" << endl
                              << "\t----------------------" << endl;
        {
            MX mx;
            pthread_t h;
            my_ThreadTestArg arg(0,0,&mx);

            mx.lock();

            ASSERT(0 == pthread_create(&h,0,testMutexFunc2,&arg));

            while(1 != arg.d_startCount);

            enum { NUM_CHECK = 50 };
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            mx.unlock();
            while(1 != arg.d_stopCount);
            pthread_join(h, 0);
        }

        if (verbose) cout << endl
             << "Testing: bcemt_RecursiveMutex" << endl
             << "=============================" << endl;
        {
            RMX rmx;

            rmx.lock();
            ASSERT(0 == rmx.tryLock());
            rmx.unlock();
            rmx.unlock();
        }

        if (veryVerbose) cout << "\tTesting: recursive lock" << endl
                              << "\t-----------------------" << endl;
        {
            RMX rmx;
            my_ThreadTestArg arg(0,0,0,0,&rmx);
            pthread_t h;

            rmx.lock();
            rmx.lock();
            ASSERT(0 == rmx.tryLock());

            ASSERT(0 == pthread_create(&h,0,testRMXFunc1,&arg));

            while(1 != arg.d_startCount);

            enum { NUM_CHECK = 50 };
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            rmx.unlock();
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            rmx.unlock();
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            rmx.unlock();

            while(1 != arg.d_stopCount);
            pthread_join(h,0);
        }


        if (veryVerbose) cout << "\tTesting: recursive tryLock" << endl
                              << "\t--------------------------" << endl;
        {
            RMX rmx;
            my_ThreadTestArg arg(0,0,0,0,&rmx);
            pthread_t h;

            rmx.lock();
            rmx.lock();
            ASSERT(0 == rmx.tryLock());

            ASSERT(0 == pthread_create(&h,0,testRMXFunc2,&arg));

            while(1 != arg.d_startCount);

            enum { NUM_CHECK = 50 };
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            rmx.unlock();
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            rmx.unlock();
            for (int i=0; i<NUM_CHECK; ++i)
                ASSERT(0 == arg.d_stopCount);

            rmx.unlock();

            while(1 != arg.d_stopCount);
            pthread_join(h,0);


        }

      }break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING bcemt_AttributeImpl copy constructors/assignment operators
        //   Verify that bcemt_AttributeImpl copy constructors and assignment
        //   operators behave as expected.
        //
        // Plan:
        //   First create an instance of the 'bcemt_AttributeImpl' using
        //   the default constructor and assert the all non-platform
        //   specific defaults are correct.  Next, for a sequence of
        //   independent test values, set the respective attributes
        //   and assert that all attributes are set to the expected
        //   values.
        //
        // Testing:
        //   bcemt_AtributeImpl(const bcemt_AtributeImpl& attribute);
        //   bcemt_AtributeImpl& operator=(const bcemt_AtributeImpl& rhs);
        //   int operator==(const bcemt_AtributeImpl<...
        //   int operator!=(const bcemt_AtributeImpl<...
        //--------------------------------------------------------------------
        static const struct {
            int                  d_lineNum;
            TA::DetachedState    d_detachedState;
            TA::SchedulingPolicy d_schedPolicy;
            int                  d_schedPriority;
            int                  d_inheritSched;
            int                  d_stackSize;
            int                  d_guardSize;
        } VALUES[] = {
            //line det. state           policy
            //---- -------------------  -----------------------
            //    priority inherit stack      guard
            //    -------- ------- ---------- ------
            { L_ , TA::CREATE_JOINABLE, TA::BCEMT_SCHED_OTHER,
                  0      , 1     , 512*1024 , 1000 },
            { L_ , TA::CREATE_DETACHED, TA::BCEMT_SCHED_FIFO ,
                  10     , 0     , 256*1024 , 0 },
            { L_ , TA::CREATE_DETACHED, TA::BCEMT_SCHED_RR   ,
                  5      , 1     , 0        , 512 },
            { L_ , TA::CREATE_JOINABLE, TA::BCEMT_SCHED_FIFO ,
                  7      , 1     , 100*1024 , 0 },
            { L_ , TA::CREATE_JOINABLE, TA::BCEMT_SCHED_OTHER,
                  -5     , 0     , 1000*1024, 256 }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        if (verbose) cout << endl
             << "Testing: bcemt_AtributeImpl assignment operators" << endl
             << "=================================================" << endl;
        {
            TA x;
            const TA &X = x;
            ASSERT(TA::CREATE_JOINABLE == X.detachedState());
#ifndef BDES_PLATFORM__OS_CYGWIN
            ASSERT(TA::BCEMT_SCHED_OTHER == X.schedulingPolicy());
#else
            ASSERT(TA::BCEMT_SCHED_FIFO == X.schedulingPolicy());
#endif
            ASSERT(0 != X.inheritSchedule());
        }

        if (veryVerbose) cout << "\tTesting: copy constructor" << endl
                              << "\t-------------------------" << endl;
        for (int i = 0; i < NUM_VALUES; ++i) {
            const int LINE          = VALUES[i].d_lineNum;
            const TA::DetachedState   DETACHEDSTATE= VALUES[i].d_detachedState;
            const TA::SchedulingPolicy SCHEDPOLICY = VALUES[i].d_schedPolicy;
            const int SCHEDPRIORITY = VALUES[i].d_schedPriority;
            const int INHERITSCHED  = VALUES[i].d_inheritSched;
            const int STACKSIZE     = VALUES[i].d_stackSize;
            const int GUARDSIZE     = VALUES[i].d_guardSize;

            if (veryVerbose) {
                T_(); P_(i); P_(LINE); P(DETACHEDSTATE);
                T_(); P_(SCHEDPOLICY); P_(SCHEDPRIORITY); P(INHERITSCHED);
                T_(); P_(STACKSIZE); P(GUARDSIZE);
            }

            TA x;
            const TA &X = x;

            x.setDetachedState(DETACHEDSTATE);
            x.setSchedulingPolicy(SCHEDPOLICY);
            x.setSchedulingPriority(SCHEDPRIORITY);
            x.setInheritSchedule(INHERITSCHED);
            x.setStackSize(STACKSIZE);
            x.setGuardSize(GUARDSIZE);

            TA y1(X);
            const TA &Y1=y1;

            LOOP_ASSERT(LINE,X == Y1);
            LOOP_ASSERT(LINE,!(X != Y1));
        }

        if (veryVerbose) cout << "\tTesting: assignment operator" << endl
                              << "\t----------------------------" << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int LINE          = VALUES[i].d_lineNum;
            const TA::DetachedState   DETACHEDSTATE= VALUES[i].d_detachedState;
            const TA::SchedulingPolicy SCHEDPOLICY = VALUES[i].d_schedPolicy;
            const int SCHEDPRIORITY = VALUES[i].d_schedPriority;
            const int INHERITSCHED  = VALUES[i].d_inheritSched;
            const int STACKSIZE     = VALUES[i].d_stackSize;
            const int GUARDSIZE     = VALUES[i].d_guardSize;

            if (veryVerbose) {
                T_(); P_(i); P_(LINE); P(DETACHEDSTATE);
                T_(); P_(SCHEDPOLICY); P_(SCHEDPRIORITY); P(INHERITSCHED);
                T_(); P_(STACKSIZE); P(GUARDSIZE);
            }

            TA x;
            const TA &X = x;

            x.setDetachedState(DETACHEDSTATE);
            x.setSchedulingPolicy(SCHEDPOLICY);
            x.setSchedulingPriority(SCHEDPRIORITY);
            x.setInheritSchedule(INHERITSCHED);
            x.setStackSize(STACKSIZE);
            x.setGuardSize(GUARDSIZE);

            if (veryVerbose) {
                T_(); P_(i); P_(LINE); P(X.detachedState());
                T_(); P_(X.schedulingPolicy()); P_(X.schedulingPriority());
                T_(); P_(X.inheritSchedule()); P(X.stackSize());
                T_(); P(X.guardSize());
            }

            TA y;
            const TA &Y=y;

            y = X;

            if (veryVerbose) {
                T_(); P_(i); P_(LINE); P(Y.detachedState());
                T_(); P_(Y.schedulingPolicy()); P(Y.schedulingPriority());
                T_(); P_(Y.inheritSchedule()); P(Y.stackSize());
                T_(); P(Y.guardSize());
            }

            LOOP_ASSERT(LINE, X == Y);
            LOOP_ASSERT(LINE, !(X != Y));
        }

      }break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING bcemt_AttributeImpl basic manipulators
        //   Verify that all bcemt_AttributeImpl manipulators and basic
        //   accessors work as expected.
        //
        // Plan:
        //   First create an instance of the 'bcemt_AttributeImpl' using
        //   the default constructor and assert the all non-platform
        //   specific defaults are correct.  Next, for a sequence of
        //   independent test values, set the respective attributes
        //   and assert that all attributes are set to the expected
        //   values.
        // Testing:
        //   bcemt_AtributeImpl();
        //   ~bcemt_AtributeImpl();
        //   void setDetachedState(DetachedState detachedState);
        //   void setSchedulingPolicy(SchedulingPolicy schedulingPolicy);
        //   void setSchedulingPriority(int schedulingPriority);
        //   void setInheritSchedule(int inheritSchedul);
        //   void setStackSize(int stackSize);
        //   void setGuardSize(int guardSize);
        //   DetachedState detachedState() const;
        //   SchedulingPolicy schedulingPolicy() const;
        //   int stackSize() const;
        //   int schedulingPriority() const;
        //   int inheritSchedule() const;
        //   int guardSize() const;
        //   int operator==(const bcemt_AtributeImpl<...
        //   int operator!=(const bcemt_AtributeImpl<...
        //
        //--------------------------------------------------------------------
        static const struct {
            int                  d_lineNum;
            TA::DetachedState    d_detachedState;
            TA::SchedulingPolicy d_schedPolicy;
            int                  d_schedPriority;
            int                  d_inheritSched;
            int                  d_stackSize;
            int                  d_guardSize;
        } VALUES[] = {
            //line det. state           policy
            //---- -------------------  -----------------------
            //    priority inherit stack      guard
            //    -------- ------- ---------- ------
            { L_ , TA::CREATE_JOINABLE, TA::BCEMT_SCHED_OTHER,
                   TA::getMinSchedPriority(TA::BCEMT_SCHED_OTHER),
                   1     , 512*1024 , 1000 },
            { L_ , TA::CREATE_DETACHED, TA::BCEMT_SCHED_FIFO ,
                   TA::getMinSchedPriority(TA::BCEMT_SCHED_FIFO),
                   0     , 256*1024 , 0 },
            { L_ , TA::CREATE_DETACHED, TA::BCEMT_SCHED_RR   ,
                   TA::getMinSchedPriority(TA::BCEMT_SCHED_RR),
                   1     , 100*1024, 512 },
            { L_ , TA::CREATE_JOINABLE, TA::BCEMT_SCHED_OTHER ,
                   TA::getMaxSchedPriority(TA::BCEMT_SCHED_OTHER),
                   1     , 100*1024 , 0 },
            { L_ , TA::CREATE_JOINABLE, TA::BCEMT_SCHED_FIFO,
                   TA::getMaxSchedPriority(TA::BCEMT_SCHED_FIFO),
                   0     , 1000*1024, 256 },
            { L_ , TA::CREATE_JOINABLE, TA::BCEMT_SCHED_OTHER,
                   TA::getMaxSchedPriority(TA::BCEMT_SCHED_OTHER),
                   0     , 1000*1024, 256 },
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        if (verbose) cout << endl
                 << "Testing: bcemt_AtributeImpl basic manipulators" << endl
                 << "==============================================" << endl;
        {
            TA x;
            const TA &X = x;
            ASSERT(TA::CREATE_JOINABLE == X.detachedState());
#ifndef BDES_PLATFORM__OS_CYGWIN
            ASSERT(TA::BCEMT_SCHED_OTHER == X.schedulingPolicy());
#endif
            ASSERT(0 != X.inheritSchedule());
        }

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int LINE          = VALUES[i].d_lineNum;
            const TA::DetachedState   DETACHEDSTATE= VALUES[i].d_detachedState;
            const TA::SchedulingPolicy SCHEDPOLICY = VALUES[i].d_schedPolicy;
            const int SCHEDPRIORITY = VALUES[i].d_schedPriority;
            const int INHERITSCHED  = VALUES[i].d_inheritSched;
            const int STACKSIZE     = VALUES[i].d_stackSize;
            const int GUARDSIZE     = VALUES[i].d_guardSize;

#if !defined(BDES_PLATFORM__OS_CYGWIN)
            // Cygwin does not support the priority/policy attributes
            LOOP_ASSERT(LINE, SCHEDPRIORITY != -1);
#endif
            if (veryVerbose) {
                T_(); P_(i); P_(LINE); P(DETACHEDSTATE);
                T_(); P_(SCHEDPOLICY); P_(SCHEDPRIORITY); P(INHERITSCHED);
                T_(); P_(STACKSIZE); P(GUARDSIZE);
            }

            TA x;
            const TA &X = x;

            x.setDetachedState(DETACHEDSTATE);
            x.setSchedulingPolicy(SCHEDPOLICY);
            x.setSchedulingPriority(SCHEDPRIORITY);
            x.setInheritSchedule(INHERITSCHED);
            x.setStackSize(STACKSIZE);
            x.setGuardSize(GUARDSIZE);

            LOOP_ASSERT(LINE, DETACHEDSTATE == X.detachedState());

#ifndef BDES_PLATFORM__OS_CYGWIN
            LOOP_ASSERT(LINE, SCHEDPOLICY == X.schedulingPolicy());
#else
            // Cygwin does not support policy but will return a policy of 1
            // by default
            LOOP_ASSERT(LINE, 1 == X.schedulingPolicy());
#endif

            LOOP_ASSERT(LINE, SCHEDPRIORITY == X.schedulingPriority());
            LOOP_ASSERT(LINE, INHERITSCHED == X.inheritSchedule());
            LOOP_ASSERT(LINE, STACKSIZE == X.stackSize());

#if !defined(BDES_PLATFORM__OS_DGUX) && !defined(BDES_PLATFORM__OS_CYGWIN)
            LOOP_ASSERT(LINE, GUARDSIZE == X.guardSize());
#endif
        }

        ASSERT(TA::getMinSchedPriority() != -1);
        ASSERT(TA::getMaxSchedPriority() != -1);
        ASSERT(TA::getMinSchedPriority(-2) == -1);
        ASSERT(TA::getMaxSchedPriority(-2) == -1);
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // Breathing Test:
        //    Excersise a broad cross-section of the functionality before
        //    begining the test in ernest.
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;

        if (veryVerbose) cout << endl
                          << "\ttesting mutex" << endl
                          << "\t-------------" << endl;

        {
            MX mx;
            mx.lock();
            mx.unlock();
            ASSERT(0 == mx.tryLock());
            mx.unlock();
        }


        if (veryVerbose) cout << endl
                          << "\ttesting recursive mutex" << endl
                          << "\t-----------------------" << endl;

        {
            RMX rmx;
            rmx.lock();
            rmx.lock();
            ASSERT(0 == rmx.tryLock());
            rmx.unlock();
            rmx.unlock();
            rmx.unlock();
        }

        if (veryVerbose) cout << endl
                          << "\ttesting conditon variable" << endl
                          << "\t-------------------------" << endl;

        {
            MX mx;
            CD cd;

            cd.signal();;
            cd.broadcast();
            mx.lock();
            ASSERT(0 != cd.timedWait(&mx,
                              bdetu_SystemTime::now().addMilliseconds(100)));

            mx.unlock();
        }

        if (veryVerbose) cout << endl
                          << "\ttesting create thread" << endl
                          << "\t---------------------" << endl;

        {
            MX mutex;
            TA attr;
            TU::Handle h;

            my_ThreadTestArg arg(0,0,&mutex);

            ASSERT(0 == TU::create(&h, attr, testThreadFunc1, &arg) );
            ASSERT(0 == TU::join(h));
        }

      } break;
#endif
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
