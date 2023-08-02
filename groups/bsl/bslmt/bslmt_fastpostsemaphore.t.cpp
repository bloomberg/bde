// bslmt_fastpostsemaphore.t.cpp                                      -*-C++-*-

#include <bslmt_fastpostsemaphore.h>

#include <bslim_testutil.h>

#include <bsls_atomic.h>
#include <bsls_systemtime.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
// A 'bslmt::FastPostSemaphore' uses an implementation class and hence testing
// the forwarding to the implementation is all that is required.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] FastPostSemaphore(clockType = e_REALTIME);
// [ 2] FastPostSemaphore(int count, clockType = e_REALTIME);
//
// MANIPULATORS
// [ 4] void enable();
// [ 4] void disable();
// [ 5] void post();
// [ 5] void post(int value);
// [ 5] void postWithRedundantSignal(value, available, blocked);
// [ 7] int take(int maximumToTake);
// [ 7] int takeAll();
// [ 5] int timedWait(const bsls::TimeInterval& absTime);
// [ 3] int tryWait();
// [ 5] int wait();
//
// ACCESSORS
// [ 8] bsls::SystemClockType::Enum clockType() const;
// [ 4] int getDisabledState() const;
// [ 6] int getValue() const;
// [ 4] bool isDisabled() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE

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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -MN03

                            // ==================
                            // class AnotherClock
                            // ==================

class AnotherClock {
    // 'AnotherClock' is a C++11-compatible clock that is very similar to
    // 'bsl::chrono::steady_clock'.  The only difference is that it uses a
    // different epoch; it begins 10000 "ticks" after the beginning of
    // 'steady_clock's epoch.

  private:
    typedef bsl::chrono::steady_clock base_clock;

  public:
    typedef base_clock::duration                  duration;
    typedef base_clock::rep                       rep;
    typedef base_clock::period                    period;
    typedef bsl::chrono::time_point<AnotherClock> time_point;

    static const bool is_steady = base_clock::is_steady;

    // CLASS METHODS
    static time_point now();
        // Return a time point representing the time since the beginning of the
        // epoch.
};

// CLASS METHODS
AnotherClock::time_point AnotherClock::now()
{
    base_clock::duration ret = base_clock::now().time_since_epoch();
    return AnotherClock::time_point(ret - duration(10000));
}

                             // ===============
                             // class HalfClock
                             // ===============

class HalfClock {
    // 'HalfClock' is a C++11-compatible clock that is very similar to
    // 'bsl::chrono::steady_clock'.  The difference is that it runs "half as
    // fast" as 'steady_clock'.

  private:
    typedef bsl::chrono::steady_clock base_clock;

  public:
    typedef base_clock::duration               duration;
    typedef base_clock::rep                    rep;
    typedef base_clock::period                 period;
    typedef bsl::chrono::time_point<HalfClock> time_point;

    static const bool is_steady = base_clock::is_steady;

    // CLASS METHODS
    static time_point now();
        // Return a time point representing the time since the beginning of the
        // epoch.
};

// CLASS METHODS
HalfClock::time_point HalfClock::now()
{
    base_clock::duration ret = base_clock::now().time_since_epoch();
    return HalfClock::time_point(ret/2);
}

// BDE_VERIFY pragma: pop

template <class CLOCK>
int WaitForTimeout(bslmt::FastPostSemaphore& mX, int secondsToWait)
    // Wait on the specified 'FastPostSemaphore' 'mX' for the specified
    // 'secondsToWait' seconds based on the specified 'CLOCK'.  If the call to
    // 'timedWait' returns 'e_TIMED_OUT', indicating that a timeout has
    // occurred, verify that at least that much time has elapsed (measured by
    // the clock).
{
    typename CLOCK::time_point tp = CLOCK::now() +
                                           bsl::chrono::seconds(secondsToWait);
    int                        ret = mX.timedWait(tp);
    if (bslmt::FastPostSemaphore::e_TIMED_OUT == ret) {
        ASSERT(CLOCK::now() >= tp);
    }
    return ret;
}
#endif

typedef bslmt::FastPostSemaphore Obj;

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Queue
///- - - - - - - - - - - - -
// This example illustrates a very simple fixed-size queue where potential
// clients can push integers to a queue, and later retrieve the integer values
// from the queue in FIFO order.  Also, 'waitUntilEmpty' is implemented to
// depict the common usage of 'getDisabledState'.
//
// First, we define the 'IntQueue' class:
//..
    class IntQueue {
        // FIFO queue of integer values.

        // DATA
        bsl::vector<int>         d_data;            // queue values

        bslmt::FastPostSemaphore d_pushSem;         // resource availability
                                                    // for push

        bslmt::FastPostSemaphore d_popSem;          // resource availability
                                                    // for pop

        bsls::AtomicUint         d_pushIdx;         // index to push to

        bsls::AtomicUint         d_popIdx;          // index to pop from

        mutable bslmt::Mutex     d_emptyMutex;      // blocking point for
                                                    // 'waitUntilEmpty'

        mutable bslmt::Condition d_emptyCondition;  // condition variable for
                                                    // 'waitUntilEmpty'

        // NOT IMPLEMENTED
        IntQueue(const IntQueue&);
        IntQueue& operator=(const IntQueue&);

      public:
        // PUBLIC CONSTANTS
        enum ReturnValue {
            e_SUCCESS  = bslmt::FastPostSemaphore::e_SUCCESS,   // indicates
                                                                // success

            e_DISABLED = bslmt::FastPostSemaphore::e_DISABLED,  // indicates
                                                                // queue is
                                                                // disabled

            e_FAILED = bslmt::FastPostSemaphore::e_FAILED       // indicates
                                                                // failure
        };

        // CREATORS
        explicit
        IntQueue(bsl::size_t capacity, bslma::Allocator *basicAllocator = 0);
            // Create an 'IntQueue' object with the specified 'capacity'.
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        //! ~IntQueue() = default;
            // Destroy this object.

        // MANIPULATORS
        void disablePopFront();
            // Disable dequeueing from this queue.  All subsequent invocations
            // of 'popFront' and 'waitUntilEmpty' will fail immediately.  All
            // blocked invocations of 'popFront' and 'waitUntilEmpty' will fail
            // immediately.  If the queue is already dequeue disabled, this
            // method has no effect.

        void enablePopFront();
            // Enable dequeuing.  If the queue is not dequeue disabled, this
            // method has no effect.

        int popFront(int *value);
            // Remove the element from the front of this queue and load that
            // element into the specified 'value'.  If the queue is empty,
            // block until it is not empty.  Return 0 on success, and a nonzero
            // value if the queue is disabled.

        int pushBack(int value);
            // Append the specified 'value' to the back of this queue, blocking
            // until either space is available - if necessary - or the queue is
            // disabled.  Return 0 on success, and a nonzero value if the queue
            // is disabled.

        // ACCESSORS
        int waitUntilEmpty() const;
            // Block until all the elements in this queue are removed.  Return
            // 0 on success, and a non-zero value if the queue is not empty and
            // 'isPopFrontDisabled()'.  A blocked thread waiting for the queue
            // to empty will return a non-zero value if 'disablePopFront' is
            // invoked.
    };
//..
// Next, implement the queue:
//..
    // CREATORS
    IntQueue::IntQueue(bsl::size_t capacity, bslma::Allocator *basicAllocator)
    : d_data(capacity, basicAllocator)
    , d_pushSem(static_cast<int>(capacity))
    , d_popSem(0)
    , d_pushIdx(0)
    , d_popIdx(0)
    , d_emptyMutex()
    , d_emptyCondition()
    {
    }

    // MANIPULATORS
    void IntQueue::disablePopFront()
    {
        d_popSem.disable();
    }

    void IntQueue::enablePopFront()
    {
        d_popSem.enable();
    }

    int IntQueue::popFront(int *value)
    {
        // wait for available element

        int rv = d_popSem.wait();
        if (0 != rv) {
            return rv;                                                // RETURN
        }

        *value = d_data[d_popIdx++ % d_data.size()];

        d_pushSem.post();  // signal additional empty element

        if (0 == d_popSem.getValue()) {
            {
                bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
            }
            d_emptyCondition.broadcast();
        }

        return 0;
    }

    int IntQueue::pushBack(int value)
    {
        // wait for an empty element

        int rv = d_pushSem.wait();
        if (0 != rv) {
            return rv;                                                // RETURN
        }

        d_data[d_pushIdx++ % d_data.size()] = value;

        d_popSem.post();  // signal additional available element

        return 0;
    }

    // ACCESSORS
    int IntQueue::waitUntilEmpty() const
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);

        int state = d_popSem.getDisabledState();

        while (d_popSem.getValue()) {
            if (state != d_popSem.getDisabledState()) {
                return e_DISABLED;                                    // RETURN
            }
            d_emptyCondition.wait(&d_emptyMutex);
        }

        return e_SUCCESS;
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Then, declare an instance of 'IntQueue':
//..
    IntQueue queue(10);
//..
// Next, populate some values:
//..
    ASSERT(0 == queue.pushBack(5));
    ASSERT(0 == queue.pushBack(7));
    ASSERT(0 == queue.pushBack(3));
//..
// Now, pop and verify the values:
//..
    int value;

    ASSERT(0 == queue.popFront(&value));
    ASSERT(5 == value);

    ASSERT(0 == queue.popFront(&value));
    ASSERT(7 == value);

    ASSERT(0 == queue.popFront(&value));
    ASSERT(3 == value);
//..
// Finally, use 'waitUntilEmpty' to verify the queue is empty:
//..
//  ASSERT(IntQueue::e_SUCCESS == queue.waitUntilEmpty());
//..
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'clockType'
        //
        // Concerns:
        //: 1 'clockType' returns the clock type passed to the constructor.
        //:
        //: 2 'clockType' is declared 'const'.
        //
        // Plan:
        //: 1 Create a 'const' object, and then query it to make sure that the
        //:   correct clock type is returned.
        //
        // Testing:
        //   bsls::SystemClockType::Enum clockType() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'clockType'" << endl
                          << "===================" << endl;

        const Obj def;
        ASSERT(bsls::SystemClockType::e_REALTIME == def.clockType());

        const Obj rt(bsls::SystemClockType::e_REALTIME);
        ASSERT(bsls::SystemClockType::e_REALTIME == rt.clockType());

        const Obj mt(bsls::SystemClockType::e_MONOTONIC);
        ASSERT(bsls::SystemClockType::e_MONOTONIC == mt.clockType());

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        const Obj rt0((bsl::chrono::system_clock()));
        ASSERT(bsls::SystemClockType::e_REALTIME == rt0.clockType());

        const Obj mt0((bsl::chrono::steady_clock()));
        ASSERT(bsls::SystemClockType::e_MONOTONIC == mt0.clockType());

        const Obj rt1(1, bsl::chrono::system_clock());
        ASSERT(bsls::SystemClockType::e_REALTIME == rt1.clockType());

        const Obj mt1(1, bsl::chrono::steady_clock());
        ASSERT(bsls::SystemClockType::e_MONOTONIC == mt1.clockType());
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'take' AND 'takeAll'
        //   Ensure the manipulators forward as expected.
        //
        // Concerns:
        //: 1 The manipulators forward to the underlying implementation
        //:   correctly.
        //
        // Plan:
        //: 1 Directly verify the result of 'take' and 'takeAll' during a
        //:   sequence of operations on the semaphore.  (C-1)
        //
        // Testing:
        //   int take(int maximumToTake);
        //   int takeAll();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'take' AND 'takeAll'" << endl
                 << "============================" << endl;
        }

        {
            Obj mX(3);

            ASSERT(1 == mX.take(1));
        }
        {
            Obj mX(3);

            ASSERT(3 == mX.take(5));
        }
        {
            Obj mX(3);

            ASSERT(3 == mX.takeAll());
        }
        {
            Obj mX(1);

            ASSERT(1 == mX.takeAll());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'getValue'
        //   Ensure the accessor forwards as expected.
        //
        // Concerns:
        //: 1 The accessor forwards to the underlying implementation correctly.
        //
        // Plan:
        //: 1 Directly verify the result of 'getValue' throughout a sequence of
        //:   operations on the semaphore.  (C-1)
        //
        // Testing:
        //   int getValue() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'getValue'" << endl
                 << "==================" << endl;
        }
        {
            Obj mX(3);  const Obj& X = mX;

            ASSERT(3 == X.getValue());

            ASSERT(Obj::e_SUCCESS == mX.tryWait());

            ASSERT(2 == X.getValue());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'post', 'timedWait', AND 'wait'
        //   Ensure the manipulators forward as expected.
        //
        // Concerns:
        //: 1 The manipulators 'post', 'timedWait, and 'wait' forward to the
        //:   underlying implementation correctly.
        //
        // Plan:
        //: 1 Create semaphores with varying initial count, invoke a
        //:   manipulator, and directly verify the count using 'tryWait'.
        //:
        //: 2 Directly verify the timeout functionality of 'timedWait', the
        //:   return value of the method, and use 'tryWait' to verify the
        //:   semaphore count.
        //:
        //: 3 Create and disable semaphores and verify the return value of
        //:   'wait' and 'timedWait'.  (C-1)
        //
        // Testing:
        //   void post();
        //   void post(int value);
        //   void postWithRedundantSignal(value, available, blocked);
        //   int timedWait(const bsls::TimeInterval& absTime);
        //   int wait();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'post', 'timedWait', AND 'wait'" << endl
                 << "=======================================" << endl;
        }

        if (verbose) cout << "\nDirect test of count adjustments." << endl;
        {
            for (int initialCount = -5; initialCount < 10; ++initialCount) {
                {
                    Obj mX(initialCount);

                    int count = initialCount;
                    while (count < 5) {
                        mX.post();
                        ++count;
                    }

                    while (count) {
                        ASSERT(Obj::e_SUCCESS == mX.wait());
                        --count;
                    }
                    ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
                }
                {
                    Obj mX(initialCount);

                    int count = initialCount;
                    while (count < 5) {
                        mX.post(1);
                        ++count;
                    }

                    while (count) {
                        ASSERT(Obj::e_SUCCESS ==
                                        mX.timedWait(bsls::TimeInterval(1.0)));
                        --count;
                    }
                    ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
                }
                {
                    Obj mX(initialCount);

                    int count = initialCount;
                    while (count < 5) {
                        mX.post(3);
                        count += 3;
                    }

                    while (count) {
                        ASSERT(Obj::e_SUCCESS == mX.wait());
                        --count;
                    }
                    ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
                }
                {
                    Obj mX(initialCount);

                    int count = initialCount;
                    while (count < 5) {
                        mX.postWithRedundantSignal(1, 5, 5);
                        ++count;
                    }

                    while (count) {
                        ASSERT(Obj::e_SUCCESS ==
                                        mX.timedWait(bsls::TimeInterval(1.0)));
                        --count;
                    }
                    ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
                }
                {
                    Obj mX(initialCount);

                    int count = initialCount;
                    while (count < 5) {
                        mX.postWithRedundantSignal(3, 5, 5);
                        count += 3;
                    }

                    while (count) {
                        ASSERT(Obj::e_SUCCESS == mX.wait());
                        --count;
                    }
                    ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
                }
            }
        }

        if (verbose) cout << "\nDirect test of 'timedWait' concerns." << endl;
        {
            Obj mX;

            bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
            bsls::TimeInterval until = now + bsls::TimeInterval(0.2);

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(until));

            ASSERT((bsls::SystemTime::nowRealtimeClock() - now)
                                                   >= bsls::TimeInterval(0.1));

            mX.post();

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        {
            Obj mX;
            int rc;

            rc = WaitForTimeout<bsl::chrono::system_clock>(mX, 1);
            ASSERT(Obj::e_TIMED_OUT == rc);

            rc = WaitForTimeout<bsl::chrono::steady_clock>(mX, 1);
            ASSERT(Obj::e_TIMED_OUT == rc);

            rc = WaitForTimeout<HalfClock>(mX, 1);
            ASSERT(Obj::e_TIMED_OUT == rc);

            rc = WaitForTimeout<AnotherClock>(mX, 1);
            ASSERT(Obj::e_TIMED_OUT == rc);

            mX.post();

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
        }
#endif

        if (verbose) {
            cout << "\nDirect test of return value when disabled." << endl;
        }
        {
            Obj mX;

            bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
            bsls::TimeInterval until = now + bsls::TimeInterval(0.2);

            mX.disable();

            ASSERT(Obj::e_DISABLED == mX.wait());
            ASSERT(Obj::e_DISABLED == mX.timedWait(until));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'enable', 'disable', 'getDisabledState', AND 'isDisabled'
        //   Ensure the methods forward as expected.
        //
        // Concerns:
        //: 1 The methods 'enable', 'disable', 'getDisabledState', and
        //:   'isDisabled' forward to the underlying implementation correctly.
        //
        // Plan:
        //: 1 Directly verify the effects of 'enable' and 'disable' using
        //:   'tryWait'.  Using the known state of the semaphore, verify
        //:   'getDisabledState' and 'isDisabled'.  (C-1)
        //
        // Testing:
        //   void enable();
        //   void disable();
        //   int getDisabledState() const;
        //   bool isDisabled() const;
        // --------------------------------------------------------------------

        if (verbose) {
//----------^
cout << endl
     << "TESTING 'enable', 'disable', 'getDisabledState', AND 'isDisabled'"
     << endl
     << "================================================================="
     << endl;
//----------v
        }

        {
            Obj mX;  const Obj& X = mX;

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
            ASSERT(!X.isDisabled());

            const int state1 = X.getDisabledState();
            ASSERT(0 == (state1 & 1));

            mX.disable();

            const int state2 = X.getDisabledState();
            ASSERT(1 == (state2 & 1));
            ASSERT(state2 != state1);

            ASSERT(Obj::e_DISABLED == mX.tryWait());
            ASSERT( X.isDisabled());

            mX.enable();

            const int state3 = X.getDisabledState();
            ASSERT(0 == (state3 & 1));
            ASSERT(state3 != state1);
            ASSERT(state3 != state2);

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
            ASSERT(!X.isDisabled());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'tryWait'
        //   Ensure the manipulator forwards as expected.
        //
        // Concerns:
        //: 1 The method 'tryWait' forwards to the underlying implementation
        //:   correctly.
        //
        // Plan:
        //: 1 Create semaphores with varying initial count, invoke 'tryWait'
        //:   and directly verify the results.
        //:
        //: 2 Create a semaphore with varying initial count, use the untested
        //:   'disable' method to disable this semaphore, and verify the result
        //:   of 'tryWait'.  (C-1)
        //
        // Testing:
        //   int tryWait();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'tryWait'" << endl
                          << "=================" << endl;

        for (int initialCount = 0; initialCount < 10; ++initialCount) {
            Obj mX(initialCount);

            for (int i = 0; i < initialCount; ++i) {
                ASSERT(Obj::e_SUCCESS == mX.tryWait());
            }
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());
        }

        for (int initialCount = 0; initialCount < 10; ++initialCount) {
            Obj mX(initialCount);

            mX.disable();

            ASSERT(Obj::e_DISABLED == mX.tryWait());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS
        //   The basic concern is that the constructors operate as expected.
        //
        // Concerns:
        //: 1 The semaphore count is correctly initialized.
        //:
        //: 2 The clock is correctly initialized.
        //
        // Plan:
        //: 1 Use the untested 'tryWait' and 'post' to verify the count.  (C-1)
        //:
        //: 2 Use the untested 'timedWait' to verify the clock.  (C-2)
        //
        // Testing:
        //   FastPostSemaphore(clockType = e_REALTIME);
        //   FastPostSemaphore(int count, clockType = e_REALTIME);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CREATORS" << endl
                          << "========" << endl;

        {
            // verify default count is 0 and default clock is realtime

            Obj mX;

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            mX.post();

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(
                                           bsls::SystemTime::nowRealtimeClock()
                                                   + bsls::TimeInterval(0.1)));

            bsls::TimeInterval duration = bsls::SystemTime::nowMonotonicClock()
                                        - start;

            ASSERT(bsls::TimeInterval(0.05) <= duration);
            ASSERT(bsls::TimeInterval(0.15) >= duration);
        }
        {
            // verify initial count can be set

            Obj mX(-1);

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            mX.post();

            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            mX.post();

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(
                                           bsls::SystemTime::nowRealtimeClock()
                                                   + bsls::TimeInterval(0.1)));

            bsls::TimeInterval duration = bsls::SystemTime::nowMonotonicClock()
                                        - start;

            ASSERT(bsls::TimeInterval(0.05) <= duration);
            ASSERT(bsls::TimeInterval(0.15) >= duration);
        }
        {
            // verify clock can be set

            Obj mX(bsls::SystemClockType::e_MONOTONIC);

            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(
                                          bsls::SystemTime::nowMonotonicClock()
                                                   + bsls::TimeInterval(0.1)));

            bsls::TimeInterval duration = bsls::SystemTime::nowMonotonicClock()
                                        - start;

            ASSERT(bsls::TimeInterval(0.05) <= duration);
            ASSERT(bsls::TimeInterval(0.15) >= duration);
        }
        {
            // verify initial count and clock can be set

            Obj mX(2, bsls::SystemClockType::e_MONOTONIC);

            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_SUCCESS     == mX.tryWait());
            ASSERT(Obj::e_WOULD_BLOCK == mX.tryWait());

            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            ASSERT(Obj::e_TIMED_OUT == mX.timedWait(
                                          bsls::SystemTime::nowMonotonicClock()
                                                   + bsls::TimeInterval(0.1)));

            bsls::TimeInterval duration = bsls::SystemTime::nowMonotonicClock()
                                        - start;

            ASSERT(bsls::TimeInterval(0.05) <= duration);
            ASSERT(bsls::TimeInterval(0.15) >= duration);
        }
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
        //: 1 Instantiate an object and verify basic functionality.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            Obj X;
            X.post();
            X.post(2);
            X.wait();
            ASSERT(0 == X.timedWait(bsls::SystemTime::nowRealtimeClock() +
                                    bsls::TimeInterval(60)));
            ASSERT(0 == X.tryWait());
            ASSERT(0 != X.tryWait());
            ASSERT(0 != X.timedWait(bsls::SystemTime::nowRealtimeClock() +
                                    bsls::TimeInterval(0.1)));
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
