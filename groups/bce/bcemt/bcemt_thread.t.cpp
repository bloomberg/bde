// bcemt_thread.t.cpp              -*-C++-*-
#include <bcemt_thread.h>

#include <bces_platform.h>
#include <bcemt_lockguard.h>

#include <bdef_bind.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsl_deque.h>
#include <bsl_iostream.h>

#include <cstdlib>  // atoi()
#include <cstring>  // strcmp()

using namespace BloombergLP;
using namespace std;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// All of the classes defined in this component are simple wrappers around the
// pthreads interfaces.  We will perform a minimal test to ensure that our
// methods are hooked up correctly.
//
//-----------------------------------------------------------------------------
// bcemt_Condition
//
// [ 4] int timedWait(bcemt_Mutex*, const bdet_TimeInterval&);
//
//-----------------------------------------------------------------------------
// Usage Examples
// --------------
// [ 9] Windows specific example
// [ 5] Basic thread utilities example
// [ 6] Small stack example
// [ 7] Producer-consumer example
// [ 8] Thread-safe my_Account example
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";   \
                aSsErT(1, #X, __LINE__); } }
#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J <<         \
                        "\t" << #K << ": " << K << "\n";                      \
                aSsErT(1, #X, __LINE__); } }
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

// ----------------------------------------------------------------------------
// Case 1. Basic thread utilities example

extern "C" void *myThreadFunction(void *)
    // Print to standard output "Another second has passed"
    // every second for five seconds.
{
    for(int i = 0; i < 5; ++i) {
        bcemt_ThreadUtil::microSleep(1000);  // 1 msec used for efficiency
        if (verbose) bsl::cout << "Another millisecond has passed\n";
    }
    return 0;
}

// ----------------------------------------------------------------------------
// Case 2. Small stack example

extern "C" void *mySmallStackThreadFunction(void *threadArg)
    // Initialize a small object on the stack and do some work.
{
    char *initValue = (char *)threadArg;
    char Small[8];
    memset(&Small[0], *initValue, 8);
    // do some work ...
    return 0;
}

void createSmallStackSizeThread()
    // Create a detached thread with the small stack size
    // and perform some work
{
    enum { STACK_SIZE = 16384 };
    bcemt_Attribute attributes;
    attributes.setDetachedState(bcemt_Attribute::BCEMT_CREATE_DETACHED);
    attributes.setStackSize(STACK_SIZE);

    char initValue = 1;
    bcemt_ThreadUtil::Handle handle;
    bcemt_ThreadUtil::create(&handle,
                             attributes,
                             mySmallStackThreadFunction,
                             &initValue);
}

// ----------------------------------------------------------------------------
// Case 3. Producer-consumer example

struct my_WorkItem {
    int d_item;  // represents work to perform
};

struct my_WorkQueue {
    bsl::deque<my_WorkItem> d_queue;// queue of work requests
    bcemt_Mutex             d_mx;   // protects the shared queue
    bcemt_Condition         d_cv;   // signals the existence of new work
};

extern "C" void *producer(void *arg)
    // Receive commands from an external source, place them on
    // the shared 'my_WorkQueue' (passed as the specified void
    // pointer 'arg'), and return 'arg'.
{
    my_WorkQueue *queue = (my_WorkQueue *)arg;

    // For simplicity, the external source is modeled by a
    // for loop that generates 50 random numbers.

    for (int i = 0; i < 50; ++i) {
        my_WorkItem request;
        request.d_item = rand();
        queue->d_mx.lock();
        queue->d_queue.push_back(request);
        queue->d_mx.unlock();
        queue->d_cv.signal();
    }
    return queue;
}

extern "C" void *consumer(void *arg)
    // Consume the commands from the shared 'my_WorkQueue' (passed as
    // the specified void pointer 'arg') and return 0.  Note that this
    // function will continue processing commands until there are no
    // commands for at least one tenth of a second.
{
    my_WorkQueue *queue = (my_WorkQueue *)arg;
    int finished = 0;
    while (!finished) {
        // Set the timeout to be one second from now.

        bdet_TimeInterval timeout = bdetu_SystemTime::now();
        timeout.addMilliseconds(100);

        // Wait for work requests to be added to the queue.

        queue->d_mx.lock();
        while (0 == queue->d_queue.size()) {
            int status = queue->d_cv.timedWait(&queue->d_mx, timeout);
            if (0 != status) {
                break;
            }
        }

        if (0 != queue->d_queue.size()) {
            // The condition variable was either signaled or timed out
            // and there are work requests in the queue.

            my_WorkItem item = queue->d_queue.front();
            queue->d_queue.pop_front();
            queue->d_mx.unlock();

            // Process the work requests.
            // ...
        }
        else {
            // The wait timed out and the queue was empty.  Unlock the
            // mutex and return.

            queue->d_mx.unlock();
            finished = 1;
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// Case 4. Thread-safe my_Account example

namespace {  //unnamed namespace for private classes

                        // =============
                        // class my_Account
                        // =============

class my_Account {

    // DATA
    double d_money;   // amount of money in the account

  public:
    // CREATORS
    my_Account();
        // Create an account with zero balance.

    my_Account(const my_Account& original);
        // Create an account having the balance of the specified
        // 'other' account.

    ~my_Account();
        // Destroy this account.

    // MANIPULATORS
    my_Account& operator=(const my_Account& rhs);
        // Assign to this account the balance of the specified 'rhs' account.

    void deposit(double amount);
        // Deposit the specified 'amount' of money into this account.

    void withdraw(double amount);
        // Withdraw the specified 'amount' of money from this account.

    // ACCESSORS
    double balance() const;
        // Return the amount of money in this account that is available
        // for withdrawal.
};

// CREATORS
my_Account::my_Account()
: d_money(0)
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

                        // ==========================
                        // class my_SafeAccountHandle
                        // ==========================

class my_SafeAccountHandle {
    // This class provides a thread-safe handle to an account (held, not
    // owned) passed at construction.

    // DATA
    my_Account          *d_account_p;  // held, not owned
    mutable bcemt_Mutex  d_lock;

  private:
    // NOT IMPLEMENTED
    my_SafeAccountHandle(const my_SafeAccountHandle&);
    my_SafeAccountHandle& operator=(const my_SafeAccountHandle&);

  public:
    // CREATORS
    explicit
    my_SafeAccountHandle(my_Account *account);
        // Create a thread-safe handle to the specified 'account'.

    ~my_SafeAccountHandle();
        // Destroy this handle.  Note that the held account
        // is unaffected by this operation.

    // MANIPULATORS
    void deposit(double amount);
        // Atomically deposit the specified 'amount' of money into the
        // account held by this handle.  Note that this operation is
        // thread-safe, no 'lock' is needed.

    void lock();
        // Provide exclusive access to the underlying account held by
        // this object.

    void unlock();
        // Release exclusivity of the access to the underlying
        // account held by this object.

    void withdraw(double amount);
        // Atomically withdraw the specified 'amount' of money from the
        // account held by this handle.  Note that this operation is
        // thread-safe, no 'lock' is needed.

    // ACCESSORS
    my_Account *account() const;
        // Return the address of the account held by this handle.

    double balance() const;
        // Atomically return the amount of money that is available for
        // withdrawal from the account held by this handle.
};

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
    d_lock.lock(); // consider using 'bcemt_AutoLock'
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
    d_lock.lock(); // consider using 'bcemt_AutoLock'
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
    d_lock.lock();
    double res = d_account_p->balance();
    d_lock.unlock();
    return res;
}

                        // ===================
                        // class ThreadChecker
                        // ===================

class ThreadChecker {
    bcemt_Mutex d_mutex;
    int         d_count;

  public:
    // CREATORS
    ThreadChecker() : d_count(0) {}

    // MANIPULATORS
    void eval()
    {
        d_mutex.lock();
        ++d_count;
        d_mutex.unlock();
    }

    bdef_Function<void(*)()> getFunctor()
    {
        return bdef_BindUtil::bind(&ThreadChecker::eval, this);
    }

    // ACCESSORS
    int count() const
    {
       return d_count;
    }
};

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING CONDITION::TIMEDWAIT()
        //
        // Concerns:
        //   Similar to the 'microSleep' test case 10, test how accurate
        //   Condition::timedWait() is.
        //
        // Plan:
        //   Several times, timedWait for 0.3 seconds, then check how much
        //   time has passed and verify that it is within acceptable
        //   boundaries.
        // --------------------------------------------------------------------

        const bdet_TimeInterval SLEEP_SECONDS(0.3);
        const double OVERSHOOT_MIN = -1e-5;
#if defined(BSLS_PLATFORM__OS_SOLARIS)
        const double OVERSHOOT_MAX = 0.10;
#else
        const double OVERSHOOT_MAX = 0.05;
#endif

        bcemt_Condition condition;
        bcemt_Mutex     mutex;
        bcemt_LockGuard<bcemt_Mutex> guard(&mutex);

        for (int i = 0; i < 8; ++i) {
            const bdet_TimeInterval start = bdetu_SystemTime::now();
            const bdet_TimeInterval timeout = start + SLEEP_SECONDS;
            const bdet_TimeInterval minTimeout = timeout + OVERSHOOT_MIN;

            int sts;
            bdet_TimeInterval finish;

            int j;
            for (j = 0; j < 4; ++j) {
                sts = condition.timedWait(&mutex, timeout);
                finish = bdetu_SystemTime::now();
                if (finish > minTimeout) {
                    break;
                }
            }
            ASSERT(j < 4);
            LOOP_ASSERT(sts, -1 == sts);

            double overshoot =
                       (finish - start - SLEEP_SECONDS).totalSecondsAsDouble();

            if (veryVerbose) P(overshoot);

            LOOP2_ASSERT(overshoot, j, overshoot >= OVERSHOOT_MIN);
            LOOP3_ASSERT(overshoot, j, OVERSHOOT_MAX,
                                                    overshoot < OVERSHOOT_MAX);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING MICROSLEEP
        //
        // Concerns:
        //   That 'microSleep' always sleeps at least the given amount of
        //   time, and doesn't sleep an unreasonable amount of extra time.
        //
        // Plan:
        //   Several times, sleep for 0.3 seconds, then check how much time
        //   has passed and verify that it is within acceptable boundaries.
        // --------------------------------------------------------------------

        enum { SLEEP_MICROSECONDS = 300 * 1000 };
        const double SLEEP_SECONDS = SLEEP_MICROSECONDS * 1e-6;
        const double OVERSHOOT_MIN = -1e-5;
#if defined(BSLS_PLATFORM__OS_SOLARIS) || defined(BSLS_PLATFORM__OS_LINUX)
            const double TOLERANCE = 2;   // microSleep is obscenely
                                          // imprecise on Sun and to a
                                          // lesser extent Linux
#else
            const double TOLERANCE = 0.05;
#endif

        for (int i = 0; i < 8; ++i) {
            double start   = bdetu_SystemTime::now().totalSecondsAsDouble();
            bcemt_ThreadUtil::microSleep(SLEEP_MICROSECONDS);
            double elapsed = bdetu_SystemTime::now().totalSecondsAsDouble() -
                                                                         start;

            double overshoot = elapsed - SLEEP_SECONDS;

            if (veryVerbose) P(overshoot);

            LOOP_ASSERT( overshoot, overshoot >= OVERSHOOT_MIN);
            LOOP2_ASSERT(overshoot, TOLERANCE, overshoot < TOLERANCE);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // Windows specific example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nWindows specific example" << endl;

#ifdef BDE_WIN_THREADS
        my_Object *myojbect = new my_Object;
        AfxBeginThread(MyThreadProc, myobject);
#endif

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // Thread-safe my_Account example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nThread-safe my_Account example" << endl;

        my_Account account;
        account.deposit(100.50);
        double  paycheck = 50.25;
        my_SafeAccountHandle handle(&account);

                                   ASSERT(100.50 == handle.balance());
        handle.deposit(paycheck);  ASSERT(150.75 == handle.balance());

        double check[5] = {25.0, 100.0, 99.95, 75.0, 50.0};

        handle.lock();
        double originalBalance = handle.account()->balance();
        for (int i = 0; i < 5; ++i) {
            handle.account()->deposit(check[i]);
        }
        ASSERT(originalBalance + 349.95 == handle.account()->balance());
        handle.unlock();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Producer-consumer example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nProducer-consumer example" << endl;

        my_WorkQueue queue;

        bcemt_ThreadUtil::Handle mainHandle;
        mainHandle = bcemt_ThreadUtil::self();

        bcemt_Attribute attributes;
        attributes.setDetachedState(bcemt_Attribute::BCEMT_CREATE_JOINABLE);

        bcemt_ThreadUtil::Handle producerHandle;
        int status = bcemt_ThreadUtil::create(&producerHandle,
                                              attributes,
                                              &producer,
                                              &queue);
        ASSERT(0 == status);
        if (0 != status) {
            P(status);
            P(errno);
        }
        bcemt_ThreadUtil::Handle consumerHandle;
        status = bcemt_ThreadUtil::create(&consumerHandle,
                                          attributes,
                                          &consumer,
                                          &queue);
        ASSERT(0 == status);

        ASSERT(0== bcemt_ThreadUtil::isEqual(consumerHandle,producerHandle));
        ASSERT(0== bcemt_ThreadUtil::isEqual(mainHandle, producerHandle));
        ASSERT(0== bcemt_ThreadUtil::isEqual(mainHandle, consumerHandle));

        status = bcemt_ThreadUtil::join(consumerHandle);
        ASSERT(0 == status);
        void *retArg;
        status = bcemt_ThreadUtil::join(producerHandle, &retArg);
        ASSERT(0 == status);
        ASSERT((my_WorkQueue*)retArg == &queue);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // Small stack example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSmall stack example" << endl;

        createSmallStackSizeThread();

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // Basic thread utilities example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBasic thread utilities example" << endl;

        bcemt_Attribute attributes;
        bcemt_ThreadUtil::Handle handle;
        bcemt_ThreadUtil::create(&handle, attributes, myThreadFunction, 0);
        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::join(handle);
        if (verbose) bsl::cout << "A five second interval has elapsed\n";

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'timedWait'
        //
        // Concerns:
        //   That timedWait, before timing out, waits at least the allotted
        //   time.
        //
        // Plan:
        //   Do a timedWait for a substantial time on a condition that never
        //   becomes true, and verify that the time has elapsed afterward.
        //
        // Testing:
        //   int timedWait(bcemt_Mutex*, const bdet_TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TimedWait() timeout test\n";
        }

        bcemt_Condition cond;
        bcemt_Mutex mutex;
        bcemt_LockGuard<bcemt_Mutex> guard(&mutex);

        bdet_TimeInterval startT = bdetu_SystemTime::now();
        double start = startT.totalSecondsAsDouble();
        bdet_TimeInterval endT  = startT + bdet_TimeInterval(0.1);
        bdet_TimeInterval endT2 = startT + bdet_TimeInterval(0.11);
        int i;
        int sts;
        for (i = 1; 10 >= i; ++i) {
            sts = cond.timedWait(&mutex, endT);
            if (-1 == sts) {
                break;
            }
            if (bdetu_SystemTime::now() > endT2) {
                ASSERT(0 && "should have timed out by now");
                break;
            }
        }
        ASSERT(0 != sts);
        double finish = bdetu_SystemTime::now().totalSecondsAsDouble() - start;
        double end = endT.totalSecondsAsDouble() - start;
        LOOP2_ASSERT(finish, end, finish >= end);
        if (verbose) {
            cout << "Iterations: " << i << endl;
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'isEqual'
        //
        // Concerns: A valid thread handle is equal to itself.  Two valid
        // thread handles are equal.  An invalid thread handle is unequal to
        // any other thread handle, including itself.
        //
        // --------------------------------------------------------------------

        bcemt_Attribute attributes;
        bcemt_ThreadUtil::Handle validH1, validH2;
        bcemt_ThreadUtil::create(&validH1, attributes, myThreadFunction, 0);
        bcemt_ThreadUtil::create(&validH2, attributes, myThreadFunction, 0);
        bcemt_ThreadUtil::Handle validH1copy = validH1;

        ASSERT(1 == bcemt_ThreadUtil::isEqual(validH1, validH1));
        ASSERT(1 == bcemt_ThreadUtil::isEqual(validH1, validH1copy));
        ASSERT(0 == bcemt_ThreadUtil::isEqual(validH1, validH2));
        ASSERT(0 == bcemt_ThreadUtil::isEqual(
                                             bcemt_ThreadUtil::invalidHandle(),
                                             validH2));
        ASSERT(0 == bcemt_ThreadUtil::isEqual(
                                             bcemt_ThreadUtil::invalidHandle(),
                                             validH1copy));
        ASSERT(0 == bcemt_ThreadUtil::isEqual(validH1,
                                           bcemt_ThreadUtil::invalidHandle()));
        ASSERT(1 == bcemt_ThreadUtil::isEqual(
                                           bcemt_ThreadUtil::invalidHandle(),
                                           bcemt_ThreadUtil::invalidHandle()));

        bcemt_ThreadUtil::join(validH1);
        bcemt_ThreadUtil::join(validH2);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONCERN 'bcemt_Attribute' ENUM VALUES
        //   Concern: the 'enum' values for 'DetachedState' and
        //   'SchedulingPolicy' are converted into the corresponding
        //   enumerations values of 'bcemt_Attribute::Imp' without check.
        //
        // Plan: Simply check that the values agree.
        //
        // Testing:
        //    bcemt_Attribute::BCEMT_CREATE_JOINABLE
        //    bcemt_Attribute::BCEMT_CREATE_DETACHED
        //    bcemt_Attribute::BCEMT_SCHED_OTHER
        //    bcemt_Attribute::BCEMT_SCHED_FIFO
        //    bcemt_Attribute::BCEMT_SCHED_RR
        // --------------------------------------------------------------------

#if 0 // bcemt_ThreadAttributesImpl has been phased out
        typedef bcemt_ThreadAttributesImpl<bces_Platform::ThreadPolicy> Imp;

        ASSERT(bcemt_Attribute::BCEMT_CREATE_JOINABLE ==
                                                   Imp::BCEMT_CREATE_JOINABLE);
        ASSERT(bcemt_Attribute::BCEMT_CREATE_DETACHED ==
                                                   Imp::BCEMT_CREATE_DETACHED);

        ASSERT(bcemt_Attribute::BCEMT_SCHED_OTHER == Imp::BCEMT_SCHED_OTHER);
        ASSERT(bcemt_Attribute::BCEMT_SCHED_FIFO  == Imp::BCEMT_SCHED_FIFO);
        ASSERT(bcemt_Attribute::BCEMT_SCHED_RR    == Imp::BCEMT_SCHED_RR);
#endif

        if (verbose) {
            P((int)bcemt_Attribute::BCEMT_CREATE_JOINABLE);
            P((int)bcemt_Attribute::BCEMT_CREATE_DETACHED);
            P((int)bcemt_Attribute::BCEMT_SCHED_OTHER);
            P((int)bcemt_Attribute::BCEMT_SCHED_FIFO);
            P((int)bcemt_Attribute::BCEMT_SCHED_RR);

#if 0
            P((int)Imp::BCEMT_CREATE_JOINABLE);
            P((int)Imp::BCEMT_CREATE_DETACHED);
            P((int)Imp::BCEMT_SCHED_OTHER);
            P((int)Imp::BCEMT_SCHED_FIFO);
            P((int)Imp::BCEMT_SCHED_RR);
#endif
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Invokable functor test
        // --------------------------------------------------------------------
       if (verbose) {
          cout << "\nInvokable functor test" << endl;
       }

       enum { THREAD_COUNT = 10 }; // Actually twice this many

       bcemt_Attribute detached;
       detached.setDetachedState(bcemt_Attribute::BCEMT_CREATE_DETACHED);

       ThreadChecker joinableChecker;
       ThreadChecker detachedChecker;
       bcemt_ThreadUtil::Handle handles[THREAD_COUNT], dummy;
       for (int i = 0; i < THREAD_COUNT; ++i) {
          ASSERT(0 == bcemt_ThreadUtil::create(&handles[i],
                                               joinableChecker.getFunctor()));
          ASSERT(0 == bcemt_ThreadUtil::create(&dummy,
                                               detached,
                                               detachedChecker.getFunctor()));
       }

       // Join the joinable threads
       for (int i = 0; i < THREAD_COUNT; ++i) {
          ASSERT(0 == bcemt_ThreadUtil::join(handles[i]));
       }

       int iterations = 100;
       while ((THREAD_COUNT != joinableChecker.count() ||
               THREAD_COUNT != detachedChecker.count()) &&
              0 < --iterations)
       {
           bcemt_ThreadUtil::microSleep(100 * 1000);  // 100 msec
           bcemt_ThreadUtil::yield();
       }

       ASSERT(THREAD_COUNT == joinableChecker.count());
       ASSERT(THREAD_COUNT == detachedChecker.count());
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
