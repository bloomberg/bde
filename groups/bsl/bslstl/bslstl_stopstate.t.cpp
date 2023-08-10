// bslstl_stopstate.t.cpp                                             -*-C++-*-
#include <bslstl_stopstate.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] StopState::StopState();
//
// MANIPULATORS
// [ 2] bool StopState::enregister(StopStateCallbackNode*);
// [ 3] void StopState::deregister(StopStateCallbackNode*);
// [ 2] bool StopState::requestStop();
//
// ACCESSORS
// [ 2] bool StopState::stopRequested();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// [ *] CONCERN: In no case does memory come from the global allocator.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

bsls::AtomicInt testStatus(0);

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        int oldValue = testStatus.loadRelaxed();
        int currentValue;
        do {
            currentValue = oldValue;
            int newValue = currentValue;
            if (0 <= currentValue && currentValue <= 100) {
                ++newValue;
            } else {
                return;                                               // RETURN
            }
            oldValue = testStatus.testAndSwapAcqRel(currentValue, newValue);
        } while (oldValue != currentValue);
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
typedef bslstl::StopState             Obj;
typedef bslstl::StopStateCallbackNode Node;

bool verbose             = false;
bool veryVerbose         = false;
bool veryVeryVerbose     = false;
bool veryVeryVeryVerbose = false;

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
struct Latch {
    // Minimal latch for pre-C++20 build modes.
    explicit Latch(int count) : d_count(count) {}

    void arriveAndWait()
        // Atomically decrement 'd_count' and then wait for 'd_count' to become
        // zero.  When 'd_count' becomes zero, all waiting threads are
        // unblocked.
    {
        if (d_count.add(-1)) {
            do { } while (d_count.load());
        }
    }

    bsls::AtomicInt d_count;
};

// We're levelized below 'bslmt' and we can't use 'std::thread' in C++03, so we
// must implement a minimal abstraction over platform-specific threading
// utilities in order to test the behavior of 'StopState' when it is accessed
// concurrently from multiple threads.
#if defined(BSLS_PLATFORM_OS_WINDOWS)
class Thread {
    // Abstraction over a Windows thread.
  private:
    struct Thunk {
        void (*d_fn_p)(void*);
        void  *d_arg_p;
        static DWORD WINAPI threadProc(void *arg)
        {
            Thunk *thunk = static_cast<Thunk*>(arg);
            thunk->d_fn_p(thunk->d_arg_p);
            return 0;
        }
    };

    Thunk  d_thunk;
    DWORD  d_id;
    HANDLE d_handle;

  private:
    // NOT IMPLEMENTED
    Thread(const Thread&) BSLS_KEYWORD_DELETED;
    Thread& operator=(const Thread&) BSLS_KEYWORD_DELETED;

  public:
    typedef DWORD Id;

    Thread(void (*fn)(void*), void *arg)
    {
        d_thunk.d_fn_p = fn;
        d_thunk.d_arg_p = arg;
        d_handle =
                 CreateThread(NULL, 0, &Thunk::threadProc, &d_thunk, 0, &d_id);
        if (!d_handle) {
            std::cout << "insufficient resources to create thread"
                      << std::endl;
            abort();
        }
    }

    ~Thread()
    {
        if (d_handle) {
            std::cout << "thread was not joined prior to being destroyed"
                      << std::endl;
            abort();
        }
    }

    void join()
    {
        if (WaitForSingleObject(d_handle, INFINITE) != WAIT_OBJECT_0) {
            std::cout << "join failed: " << GetLastError() << std::endl;
            abort();
        }
        CloseHandle(d_handle);
        d_handle = 0;
    }

    Id id() const
    {
        return d_id;
    }

    static Id currentThreadId()
    {
        return GetCurrentThreadId();
    }
};
#else
class Thread {
    // Abstraction over a Unix thread.
  private:
    struct Thunk {
        void (*d_fn_p)(void*);
        void  *d_arg_p;

        static void* startRoutine(void *arg)
        {
            Thunk *thunk = static_cast<Thunk*>(arg);
            thunk->d_fn_p(thunk->d_arg_p);
            return NULL;
        }
    };

    Thunk     d_thunk;
    pthread_t d_thread;
    bool      d_joined;

  private:
    // NOT IMPLEMENTED
    Thread(const Thread&) BSLS_KEYWORD_DELETED;
    Thread& operator=(const Thread&) BSLS_KEYWORD_DELETED;

  public:
    typedef pthread_t Id;

    Thread(void (*fn)(void*), void *arg) : d_joined(false)
    {
        d_thunk.d_fn_p = fn;
        d_thunk.d_arg_p = arg;
        if (pthread_create(&d_thread, NULL, &Thunk::startRoutine, &d_thunk)) {
            std::cout << "insufficient resources to create thread"
                      << std::endl;
            abort();
        }
    }

    ~Thread()
    {
        if (!d_joined) {
            std::cout << "thread was not joined prior to being destroyed"
                      << std::endl;
            abort();
        }
    }

    void join()
    {
        if (pthread_join(d_thread, NULL)) {
            std::cout << "failed to join with thread" << std::endl;
            abort();
        }
        d_joined = true;
    }

    Id id() const
    {
        return d_thread;
    }

    static Id currentThreadId()
    {
        return pthread_self();
    }
};
#endif

// ============================================================================
//                               TEST CASE 2
// ----------------------------------------------------------------------------
namespace test_case_2 {
struct Callback0 : Node {
    // Basic callback that can be registered with a 'StopState'.  When invoked,
    // it sets a flag indicating that it has been invoked, and remembers the
    // thread ID of the thread that is invoking it.
    Callback0()
    : d_finished(false)
    {
    }

    void invoke() BSLS_NOTHROW_SPEC
    {
        d_invokingThread = Thread::currentThreadId();
        d_finished.store(true);
    }

    bool finished() const
    {
        return d_finished.load();
    }

    Thread::Id invokingThread() const
    {
        return d_invokingThread;
    }

    bsls::AtomicBool d_finished;
    Thread::Id       d_invokingThread;
};

struct GlobalState;

struct Callback1 : Callback0 {
    // Callback that can be registered with a 'StopState' and offers additional
    // functionality beyond that of 'Callback0'.
    explicit Callback1(GlobalState *state) : d_state_p(state) {}

    void invoke() BSLS_NOTHROW_SPEC;
        // Arrive and wait at 'd_state_p->d_invoke1LatchBegin'.  Then, create a
        // 'Callback0' object, register it with 'd_state_p->d_stopState' and
        // verify that 'enregister' returns false and that the 'Callback0' has
        // been invoked.  Finally, arrive and wait at
        // 'd_state_p->d_invoke1LatchEnd' and call the base class
        // implementation of 'invoke' to record completion.

    GlobalState *d_state_p;
};

struct GlobalState {
    Obj              d_stopState;
        // the stop state on which 'enregister' and 'requestStop' will be
        // called during this test case

    Latch            d_register01Latch;
        // latch at which the main thread and thread 1 will arrive and wait
        // before registering C0 and C1, respectively (P-2)

    Latch            d_stopLatch;
        // latch at which the main thread and thread 2 will arrive and wait
        // before both calling 'requestStop' (P-3)

    Latch            d_invoke1LatchBegin;
        // latch at which C1 will arrive and wait at the beginning of its
        // invocation, so thread 3 can attempt to register a callback while C1
        // is in progress (P-3)

    Latch            d_invoke1LatchEnd;
        // latch at which C1 will arrive and wait at the end of its invocation
        // (P-3)

    Latch            d_thread4Latch;
        // latch at which thread 4 and the main thread will arrive and wait
        // before their final calls to 'enregister' (P-5)

    bsls::AtomicBool d_stoppedByT2;
        // flag indicating whether thread 2 (as opposed to the main thread)
        // successfully requested a stop on 'd_stopState'

    Callback0        d_c0;
        // callback 'C0' (see test plan)

    Callback1        d_c1;
        // callback 'C1' (see test plan)

    GlobalState()
    : d_register01Latch(2)
    , d_stopLatch(2)
    , d_invoke1LatchBegin(2)
    , d_invoke1LatchEnd(2)
    , d_thread4Latch(2)
    , d_c1(this)
    {
    }
};

void Callback1::invoke() BSLS_NOTHROW_SPEC
{
    d_state_p->d_invoke1LatchBegin.arriveAndWait();

    Callback0 cb;
    // 'enregister' should return 'false' during a stop
    ASSERT(!d_state_p->d_stopState.enregister(&cb));
    // 'enregister' should invoke callback immediately during a stop
    ASSERT(cb.finished());

    d_state_p->d_invoke1LatchEnd.arriveAndWait();

    Callback0::invoke();
}

void thread1Func(void *arg)
    // Call 'enregister' and verify that registration succeeded and that the
    // callback has not yet been invoked (P-1).  (This thread will be joined
    // with prior to any call to 'requestStop'.)
{
    GlobalState *state = static_cast<GlobalState*>(arg);
    state->d_register01Latch.arriveAndWait();
    // 'enregister' should return 'true' prior to a stop
    ASSERT(state->d_stopState.enregister(&state->d_c1));
    // 'enregister' should not invoke callback immediately prior to a stop
    ASSERT(!state->d_c1.finished());
}

void thread2Func(void *arg)
    // Call 'requestStop'.  If this thread succeeded in requesting the stop,
    // verify that both callbacks that were registered prior to 'requestStop'
    // have completed, and store the value 'true' into 'd_stoppedByT2'.  (P-3)
{
    GlobalState *state = static_cast<GlobalState*>(arg);
    state->d_stopLatch.arriveAndWait();
    state->d_stoppedByT2.store(state->d_stopState.requestStop());
    if (state->d_stoppedByT2.load()) {
        // 'C0' and 'C1' should be invoked before 'requestStop' returns 'true'
        ASSERT(state->d_c0.finished());
        ASSERT(state->d_c1.finished());
    }
    // 'stopRequested' should return 'true' after any call to 'requestStop'
    ASSERT(state->d_stopState.stopRequested());
}

void thread3Func(void *arg)
    // While 'C1' is executing, call 'enregister' with another callback and
    // verify that it returns 'false' and invokes the latter callback
    // immediately since a stop is already in progress (P-3).
{
    GlobalState *state = static_cast<GlobalState*>(arg);
    state->d_invoke1LatchBegin.arriveAndWait();
    Callback0 cb;
    // 'enregister' should return 'false' during a stop
    ASSERT(!state->d_stopState.enregister(&cb));
    // 'enregister' should invoke callback immediately during a stop
    ASSERT(cb.finished());
    state->d_invoke1LatchEnd.arriveAndWait();
}

void thread4Func(void *arg)
    // At roughly the same time as the main thread, call 'enregister' and
    // verify that it returns 'false' and invokes the callback immediately
    // since a stop has already been completed (P-5).
{
    GlobalState *state = static_cast<GlobalState*>(arg);
    state->d_thread4Latch.arriveAndWait();
    Callback0 cb;
    // 'enregister' should return 'false' after a stop
    ASSERT(!state->d_stopState.enregister(&cb));
    // 'enregister' should invoke callback immediately after a stop
    ASSERT(cb.finished());
}
}  // close namespace test_case_2

// ============================================================================
//                               TEST CASE 3
// ----------------------------------------------------------------------------
namespace test_case_3 {
struct GenericCallback : Node {
    // Basic callback that can be registered with a 'StopState' and records the
    // number of times it has been invoked.
    GenericCallback()
    : d_count(0)
    {
    }

    void invoke() BSLS_NOTHROW_SPEC
    {
        d_count.add(1);
    }

    int count() const
    {
        return d_count.load();
    }

    bsls::AtomicInt d_count;
};

struct GlobalState;

struct Callback2To6 : GenericCallback {
    // The type of the callbacks 'C2', 'C3', 'C4', 'C5', and 'C6' described in
    // the test plan.
    GlobalState *d_globalState_p;

    void setGlobalState(GlobalState *statePtr)
    {
        d_globalState_p = statePtr;
    }

    void invoke() BSLS_NOTHROW_SPEC;
        // The behavior of each of the five callbacks of this type depends on
        // the order in which it was executed relative to the others.  See the
        // test plan for more details.
};

struct GlobalState {
    Obj              d_stopState;
        // the stop state on which 'enregister', 'deregister', and
        // 'requestStop' will be called during this test case

    GenericCallback  d_c1;
        // callback 'C1' described in (P-1)

    int              d_nextCallbackNum;
        // value of 'i' such that the next 'Callback2To6' that will be invoked
        // will be labelled 'Ci' (see the test plan for an explanation of the
        // behavior of each 'Ci')

    Callback2To6     d_callbackPool[5];      // unordered
        // the five objects that will be labelled 'C2' through 'C6' (depending
        // on the (unspecified) order in which they will be invoked)

    Callback2To6*    d_callbackSequence[7];
        // map from 'i' to a pointer to 'Ci', where 'i' is between 2 and 6,
        // inclusive; elements 0 and 1 are unused

    Latch            d_cancel1Latch;
        // latch used to ensure that 'C1' is deregistered before 'C2' through
        // 'C6' are registered

    Latch            d_cancel3LatchBegin;
        // latch at which thread 1 and the main thread (executing 'C2') arrive
        // and wait before thread 1 deregisters 'C3' and the main thread
        // deregisters 'C2' (P-3)

    Latch            d_cancel3LatchEnd;
        // latch at which thread 1 and main thread (executing 'C2') arrive and
        // wait when the steps in P-3 have been completed

    Latch            d_cancel4Latch;
        // latch at which the main thread (executing 'C4') and thread 1 arrive
        // and wait so that thread 1 can deregister 'C4' while it is already
        // running (P-4)

    GlobalState()
    : d_nextCallbackNum(2)
    , d_cancel1Latch(2)
    , d_cancel3LatchBegin(2)
    , d_cancel3LatchEnd(2)
    , d_cancel4Latch(2)
    {
        for (int i = 0; i < 5; i++) {
            d_callbackPool[i].setGlobalState(this);
        }
    }
};

void Callback2To6::invoke() BSLS_NOTHROW_SPEC
{
    d_globalState_p->d_callbackSequence[d_globalState_p->d_nextCallbackNum] =
                                                                          this;

    switch (d_globalState_p->d_nextCallbackNum) {
      case 2: {
        // P-3
        //
        // Find a callback that is not in the set.
        Callback2To6 *c3 = 0;
        for (int i = 0; i < 5; i++) {
            c3 = &d_globalState_p->d_callbackPool[i];
            if (c3 != this) break;
        }
        d_globalState_p->d_callbackSequence[3] = c3;
        d_globalState_p->d_cancel3LatchBegin.arriveAndWait();
        d_globalState_p->d_stopState.deregister(this);
        d_globalState_p->d_cancel3LatchEnd.arriveAndWait();
        d_globalState_p->d_nextCallbackNum = 4;
      } break;
      case 4: {
        // P-4
        //
        // Tell thread 1 to deregister us.
        d_globalState_p->d_cancel4Latch.arriveAndWait();
        d_globalState_p->d_nextCallbackNum++;
      } break;
      case 5: {
        // P-6
        //
        // Perform no action other than to update 'd_nextCallbackNum' and
        // invoke the base class implementation.
        d_globalState_p->d_nextCallbackNum++;
      } break;
      case 6: {
        // P-6
        //
        // Deregister 'C5' (which has already completed).
        d_globalState_p->d_stopState.deregister(
                                       d_globalState_p->d_callbackSequence[5]);
        d_globalState_p->d_nextCallbackNum++;
      } break;
      default: {
        const int nextCallbackNum = d_globalState_p->d_nextCallbackNum;
        ASSERTV(nextCallbackNum, false);
      }
    }
    GenericCallback::invoke();
}

void thread1Func(void *arg)
    // Execute portions of P-2..4 that need to run in parallel with other
    // operations as described in the test plan.
{
    GlobalState *state = static_cast<GlobalState*>(arg);
    // P-2
    state->d_stopState.deregister(&state->d_c1);
    state->d_cancel1Latch.arriveAndWait();
    // P-3
    state->d_cancel3LatchBegin.arriveAndWait();
    state->d_stopState.deregister(state->d_callbackSequence[3]);
    state->d_cancel3LatchEnd.arriveAndWait();
    // P-4
    state->d_cancel4Latch.arriveAndWait();
    state->d_stopState.deregister(state->d_callbackSequence[4]);
    // 'C4' should be complete before 'deregister' returns
    ASSERT(1 == state->d_callbackSequence[4]->count());
}

}  // close namespace test_case_3
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST %s CASE %d\n", __FILE__, test);

    // CONCERN: In no case does memory come from the global allocator.
    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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
        //  USAGE EXAMPLE
        // --------------------------------------------------------------------

        // There is no usage example because this is an internal component.
        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEREGISTERING CALLBACKS
        //
        // Concerns:
        //: 1 A callback can be deregistered before a stop has been requested.
        //:
        //: 2 A callback that was registered before a stop was requested, but
        //:   has not been invoked yet, can be deregistered while the stop is
        //:   in progress.
        //:
        //: 3 A callback that was registered before a stop was requested can
        //:   deregister itself.
        //:
        //: 4 A callback that was registered before a stop was requested and is
        //:   currently being invoked can be deregistered by another thread.
        //:   The invocation is completed before 'deregister' returns.
        //:
        //: 5 A callback that was registered before a stop was requested can be
        //:   deregistered after it was invoked but while the stop is still in
        //:   progress.
        //:
        //: 6 A callback that was registered before a stop was requested can be
        //:   deregistered after the stop completes.
        //:
        //: 7 A callback that is registered before a stop has been requested,
        //:   and is deregistered before it is invoked, is not invoked.
        //:
        //: 8 A callback that is registered before a stop has been requested,
        //:   and is deregistered during or after its invocation, is invoked
        //:   only once.
        //
        // Plan:
        //: 1 Create a 'StopState' object, a thread 'T', and a set of 11
        //:   callbacks, each of which has a counter that is incremented upon
        //:   each invocation.  Register one callback, 'C1', with the stop
        //:   state.
        //:
        //: 2 Register five additional callbacks with the stop state while, in
        //:   'T', deregistering 'C1'.  (C-1)
        //:
        //: 3 In the main thread, call 'requestStop'.  The remaining five
        //:   callbacks from P-1 share an additional counter that is used to
        //:   determine the order in which they are invoked.  Denote the first
        //:   one that is invoked by 'C2'.  'C2' signals 'T' to deregister
        //:   another callback; call it 'C3'.  Simultaneously, 'C2' deregisters
        //:   itself and waits for 'T' to return from 'deregister' before
        //:   itself returning.  (C-2..3)
        //:
        //: 4 Denote the next callback that is invoked by 'C4'.  'C4' signals
        //:   'T' to deregister 'C4' and verifies that 'C4' has completed
        //:   invocation before 'deregister' returns.  (C-4)
        //:
        //: 5 Denote the last two remaining callbacks from P-1 by 'C5' and
        //:   'C6', where 'C5' is executed first.  'C6' calls 'deregister' on
        //:   'C5'.  (C-5)
        //:
        //: 6 In the main thread, call 'deregister' on 'C6'.  Verify that 'C1'
        //:   and 'C3' were invoked zero times and that 'C2' and 'C4'
        //:   through 'C6' were each invoked only once. (C-6..8)
        //
        // Testing:
        //   void StopState::deregister(StopStateCallbackNode*);
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEREGISTERING CALLBACKS"
                            "\n=======================\n");

        using namespace test_case_3;

        // P-1
        GlobalState globalState;
        globalState.d_stopState.enregister(&globalState.d_c1);
        Thread t(thread1Func, &globalState);

        // P-2
        globalState.d_cancel1Latch.arriveAndWait();
        for (int i = 0; i < 5; i++) {
            if (veryVerbose) printf("Registering callback %d of 5\n", i);
            globalState.d_stopState.enregister(&globalState.d_callbackPool[i]);
        }

        // P-3..5
        globalState.d_stopState.requestStop();

        // P-6
        globalState.d_stopState.deregister(globalState.d_callbackSequence[6]);
        t.join();
        ASSERT(0 == globalState.d_c1.count());
        ASSERT(1 == globalState.d_callbackSequence[2]->count());
        ASSERT(0 == globalState.d_callbackSequence[3]->count());
        ASSERT(1 == globalState.d_callbackSequence[4]->count());
        ASSERT(1 == globalState.d_callbackSequence[5]->count());
        ASSERT(1 == globalState.d_callbackSequence[6]->count());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // REQUESTING STOPS AND REGISTERING CALLBACKS
        //   'enregister' cannot be validated separately from 'requestStop'
        //   because whether or not 'enregister' invokes the callback
        //   immediately depends on whether 'requestStop' has been called, and
        //   whether or not 'requestStop' calls any callbacks depends on
        //   whether 'enregister' has been called.
        //
        // Concerns:
        //: 1 On a newly constructed 'StopState', 'stopRequested' returns
        //:   'false'.
        //:
        //: 2 Callbacks can be registered on a 'StopState' that has not had a
        //:   stop requested.  In such cases, 'enregister' returns 'true' and
        //:   does not invoke the callback immediately.
        //:
        //: 3 'requestStop' can be called from multiple threads.  Exactly one
        //:   call will return 'true'.
        //:
        //: 4 After any call to 'requestStop', 'stopRequested' returns 'true'.
        //:
        //: 5 'enregister' can be called from any thread while a stop is in
        //:   progress.  The callback is immediately invoked and 'enregister'
        //:   returns 'false'.
        //:
        //: 6 The thread that successfully requests the stop invokes all
        //:   registered callbacks (in an unspecified order) before returning.
        //:
        //: 7 'enregister' can be called after 'requestStop' has returned
        //:   'true'.  The callback is immediately invoked and 'enregister'
        //:   returns 'false'.
        //
        // Plan:
        //: 1 Create a 'StopState' object and verify that 'stopRequested'
        //:   returns 'false'.  (C-1)
        //:
        //: 2 Create a thread, 'T1'.  Have both 'T1' and the main thread call
        //:   'enregister' at roughly the same time, where the callback
        //:   registered by the main thread is called 'C0', and the callback
        //:   registered by 'T1' is called 'C1'.  Each callback stores the
        //:   address of a separate flag, which will be set upon invocation.
        //:   Verify that 'enregister' returns 'true' in both cases and that
        //:   neither flag has been set.  (C-2)
        //:
        //: 3 Create two threads, 'T2' and 'T3'.  Have both 'T2' and the main
        //:   thread call 'requestStop' at roughly the same time and record
        //:   whether the call returned 'true'.  In the thread where the call
        //:   returned 'true', verify that both 'C0' and 'C1' have set the
        //:   flag indicating that their invocations are complete.  Have both
        //:   'T2' and the main thread call 'stopRequested' and verify that the
        //:   result was 'true' in both cases.  Upon invocation, callback 'C1'
        //:   calls 'enregister' for an additional callback and 'T3' also calls
        //:   'enregister' for an additional callback while 'C1' is still
        //:   executing.  Both callback 'C1' and thread 'T3' verify that
        //:   'enregister' returned 'false' and that the callback was invoked
        //:   immediately.  'C0' and 'C1' each record the invoking thread ID.
        //:   (C-4..6)
        //:
        //: 4 Verify that exactly one call to 'requestStop' returned 'true' and
        //:   that the thread that got a return value of 'true' is the one that
        //:   invoked both 'C0' and 'C1'.  (C-3)
        //:
        //: 5 Create a thread, 'T4'.  Have both 'T4' and the main thread call
        //:   'enregister' at roughly the same time and verify that the call
        //:   returned 'false' and that the callback has set a flag to indicate
        //:   completion by the time 'enregister' returned.  (C-7)
        //
        // Testing:
        //   StopState::StopState();
        //   bool StopState::enregister(StopStateCallbackNode*);
        //   bool StopState::requestStop();
        //   bool StopState::stopRequested();
        // --------------------------------------------------------------------

        if (verbose) printf("\nREQUESTING STOPS AND REGISTERING CALLBACKS"
                            "\n==========================================\n");

        using namespace test_case_2;

        // P-1
        GlobalState state;
        ASSERT(!state.d_stopState.stopRequested());

        // P-2
        Thread t1(thread1Func, &state);
        state.d_register01Latch.arriveAndWait();
        ASSERT(state.d_stopState.enregister(&state.d_c0));
        ASSERT(!state.d_c0.finished());
        t1.join();

        // P-3
        Thread           t3(thread3Func, &state);
        Thread           t2(thread2Func, &state);
        const Thread::Id t2Id = t2.id();

        state.d_stopLatch.arriveAndWait();
        const bool stoppedByMain = state.d_stopState.requestStop();
        if (stoppedByMain) {
            ASSERT(state.d_c0.finished());
            ASSERT(state.d_c1.finished());
        }
        ASSERT(state.d_stopState.stopRequested());
        t2.join();
        t3.join();

        // P-4
        ASSERT(1 == stoppedByMain + state.d_stoppedByT2.load());
        const Thread::Id expectedInvokingThread = stoppedByMain
                                                    ? Thread::currentThreadId()
                                                    : t2Id;
        ASSERT(expectedInvokingThread == state.d_c0.invokingThread());
        ASSERT(expectedInvokingThread == state.d_c1.invokingThread());

        // P-5
        Thread    t4(thread4Func, &state);
        Callback0 cb;

        state.d_thread4Latch.arriveAndWait();
        ASSERT(!state.d_stopState.enregister(&cb));
        ASSERT(cb.finished());
        t4.join();
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
        //: 1 Create a 'StopState' object, request a stop, and verify that the
        //:   stop was requested.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        Obj mX; const Obj& X = mX;

        ASSERT(mX.requestStop());
        ASSERT(X.stopRequested());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        std::cerr << "Error, non-zero test status = " << testStatus << "."
                  << std::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
