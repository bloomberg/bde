// bslmt_latch.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLMT_LATCH
#define INCLUDED_BSLMT_LATCH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a single-use mechanism for synchronizing on an event count.
//
//@CLASSES:
//  bslmt::Latch: single-use synchronization mechanism on a count of events
//
//@SEE_ALSO: bslmt_barrier
//
//@DESCRIPTION: This component defines a mechanism, `bslmt::Latch`, that allows
// one or more threads to wait until a certain number of operations have been
// performed by other threads.
//
// A latch maintains a `currentCount` of operations that must be performed
// before threads waiting on the latch are released.  The initial operation
// count for the latch is supplied at construction, and is decremented by calls
// to either `arrive`, `arriveAndWait`, or `countDown`.  Threads may wait on a
// latch to be released by calling either `wait` or `arriveAndWait`, and
// threads calling those methods will block until the `currentCount` is 0.
//
// *WARNING*: `bslmt::Latch` is *not* a reusable synchronization mechanism.
// The behavior is undefined when calling `countDown`, `arrive`, and
// `arriveAndWait` after the `currentCount` becomes 0 (the latch has been
// released).
//
///Comparison to `bslmt::Barrier`
///------------------------------
// A latch provides a basic synchronization tool, similar to a barrier.  A
// latch is distinct from a barrier in that:
//
// * A latch is single-use only, whereas a barrier can be used multiple times.
// * Threads waiting on a barrier are blocked, whereas threads that `arrive`
//   at a latch are not blocked (only waiting threads are blocked).
// * `wait` on a barrier always decrements the count of waiting threads by 1,
//   whereas `countDown` on a latch can indicate multiple events.
//
// An example use of a barrier is to coordinate a set of threads working in
// lock-step to complete a multi-step operation.  An example use of a latch is
// for one thread to coordinate the completion of a multi-step operation being
// performed by a set of "child" threads.
//
///Comparison to `bslmt::Semaphore`
///--------------------------------
// A latch is conceptually similar to a semaphore with a negative count.
// However, typically semaphore implementations (including `bslmt::Semaphore`
// and POSIX) do not allow for negative counts.  Waiting on a latch configured
// for `N` events is cleaner than one thread calling `wait` on a semaphore `N`
// times in a loop.  Furthermore, if the use case involves multiple threads
// waiting on a set of events, using a latch is cleaner than each thread
// waiting on a semaphore and then immediately calling `post` (to release the
// next waiting thread).
//
///Undefined Behavior When Decrementing the Event Count
///----------------------------------------------------
// The methods `arrive`, `arriveAndWait`, and `countDown` all document that it
// is undefined behavior to decrement the event count below 0.  Note that it
// isn't possible to use a latch's visible state to determine whether it is
// safe (i.e., not undefined behavior) to call `arrive`, `arriveAndWait`, or
// `countDown`.  A limit on the number of times the event count is decremented
// must be imposed by the logic of the program.  For example, in the usage
// example below a latch is created with an event count that matches the number
// of threads that will call `arrive` on that latch.
//
///Supported Clock-Types
///---------------------
// `bsls::SystemClockType` supplies the enumeration indicating the system clock
// on which timeouts supplied to other methods should be based.  If the clock
// type indicated at construction is `bsls::SystemClockType::e_REALTIME`, the
// `absTime` argument passed to the `timedWait` method should be expressed as
// an *absolute* offset since 00:00:00 UTC, January 1, 1970 (which matches the
// epoch used in `bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)`.
// If the clock type indicated at construction is
// `bsls::SystemClockType::e_MONOTONIC`, the `absTime` argument passed to the
// `timedWait` method should be expressed as an *absolute* offset since the
// epoch of this clock (which matches the epoch used in
// `bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Parallelizable Algorithm
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a `bslmt::Latch` object to help implement an
// operation that can be parallelized across a series of sub-tasks (or "jobs").
// The "parent" operation enqueues the jobs and blocks on a thread pool, and
// uses the latch as a signaling mechanism to indicate when all of the jobs
// have been completed and return to the caller.
//
// The use of a `bslmt::Latch`, rather than a `bslmt::Barrier`, is important to
// ensure that jobs in the thread pool do not block until the entire task is
// completed (preventing the thread pool from processing additional work).
//
// Suppose, for example, we want to provide a C++ type for computing a vector
// sum (vector in the mathematical sense).  That is, for two input vectors, `A`
// and `B`, each of length `N`, the result is a vector, `R`, of length `N`,
// where each element at index `i` has the value:
// ```
// R[i] = A[i] + B[i];
// ```
// This function can easily be computed in parallel because the value for each
// result index only depends on the input vectors.
//
// First, assume we have a class, `FixedThreadPool`, providing the following
// public interface (for brevity, the details have been elided; see
// `bdlmt_fixedthreadpool` or `bdlmt_threadpool` for examples of thread pools):
// ```
// class FixedThreadPool {
//
//   public:
//     // ...
//
//     // Enqueue the specified `job` to be executed by the next available
//     // thread.
//     void enqueueJob(const bsl::function<void()>& job);
// };
// ```
// Next, we declare the signature for our vector sum function,
// `parallelVectorSum`:
// ```
// /// Load the specified `result` array with the vector sum of the
// /// specified `inputA` and `inputB`, each having at least the specified
// /// `numElements`, using the specified `threadPool` to perform the
// /// operation in parallel using the specified `numJobs` parallel jobs.
// /// The behavior is undefined unless `numElements > 0`, `numJobs > 0`,
// /// and `result`, `inputA`, and `inputB` each contain at least
// /// `numElements`.
// void parallelVectorSum(double          *result,
//                        const double    *inputA,
//                        const double    *inputB,
//                        int              numElements,
//                        FixedThreadPool *threadPool,
//                        int              numJobs);
// ```
// Now, we declare a helper function, `vectorSumJob`, that will be used as a
// sub-task by `parallelVectorSum`.  `vectorSumJob` computes a single-threaded
// vector sum and uses a `bslmt::Latch` object, `completionSignal`, to indicate
// to the parent task that the computation has been completed:
// ```
// /// Load the specified `result` array with the vector sum of the
// /// specified `inputA` and `inputB`, each having at least the specified
// /// `numElements`, and when the operation is complete signal the
// /// specified `completionSignal`.  The behavior is undefined unless
// /// `numElements > 0` and `result`, `inputA`, and `inputB` each contain
// /// at least `numElements`.
// void vectorSumJob(double       *result,
//                   bslmt::Latch *completionSignal,
//                   const double *inputA,
//                   const double *inputB,
//                   int           numElements)
// {
//     for (int i = 0; i < numElements; ++i) {
//         result[i] = inputA[i] + inputB[i];
//     }
//
//     completionSignal->arrive();
// }
// ```
// Note that `bslmt::Latch::arrive` does not block the current thread (unlike
// `bslmt::Barrier::wait`), and within the context of a thread pool, this job
// will complete and the thread will be returned to the pool to accept more
// work.
//
// Next, we provide a rudimentary function argument binder (specific to this
// usage example) in view of the fact that such a facility is not available at
// this level in the BDE hierarchy:
// ```
// // This class provides an invokable that is tailored to bind the
// // `vectorSumJob` (defined above) to its requisite five arguments.
// class UsageBinder {
//
//   public:
//     // TYPES
//     typedef void FREE_FUNCTION(double       *,
//                                bslmt::Latch *,
//                                const double *,
//                                const double *,
//                                int           );
//
//   private:
//     // DATA
//     FREE_FUNCTION *d_func_p;
//     double        *d_arg1_p;
//     bslmt::Latch  *d_arg2_p;
//     const double  *d_arg3_p;
//     const double  *d_arg4_p;
//     int            d_arg5;
//
//   public:
//     // CREATORS
//
//     /// Create a `UsageBinder` object that binds the specified
//     /// `functionPtr` to the specified `arg1Ptr`, `arg2Ptr`, `arg3Ptr`,
//     /// `arg4Ptr`, and `arg5` arguments.
//     UsageBinder(FREE_FUNCTION *functionPtr,
//                 double        *arg1Ptr,
//                 bslmt::Latch  *arg2Ptr,
//                 const double  *arg3Ptr,
//                 const double  *arg4Ptr,
//                 int            arg5)
//     : d_func_p(functionPtr)
//     , d_arg1_p(arg1Ptr)
//     , d_arg2_p(arg2Ptr)
//     , d_arg3_p(arg3Ptr)
//     , d_arg4_p(arg4Ptr)
//     , d_arg5(arg5)
//     {
//     }
//
//     // MANIPULATORS
//
//     /// Invoke the function that was supplied at construction on the
//     /// arguments that were supplied at construction.
//     void operator()()
//     {
//         (*d_func_p)(d_arg1_p, d_arg2_p, d_arg3_p, d_arg4_p, d_arg5);
//     }
// };
// ```
// Then, we define `parallelVectorSum`:
// ```
// void parallelVectorSum(double          *result,
//                        const double    *inputA,
//                        const double    *inputB,
//                        int              numElements,
//                        FixedThreadPool *threadPool,
//                        int              numJobs)
// {
//     // Ensure that there is at least 1 element per job.
//
//     if (numElements < numJobs) {
//         numJobs = numElements;
//     }
//
//     const int jobSize = numElements / numJobs;
// ```
// Now, we define a `bslmt::Latch` object, `completionSignal`, that we will
// use to track the completion of this work:
// ```
//     bslmt::Latch completionSignal(numJobs);
//
//     for (int i = 0; i < numJobs; ++i) {
//         // If 'numJobs' doesn't evenly divide 'numElements', the last job
//         // will process the remaining elements.  For simplicity, we have
//         // chosen not distribute the elements between jobs as evenly as is
//         // possible.
//
//         int offset = i * jobSize;
//         int size   = (i == numJobs - 1) ? jobSize + numElements % numJobs
//                                         : jobSize;
//         assert(0 != size);
//
//         threadPool->enqueueJob(UsageBinder(vectorSumJob,
//                                            result + offset,
//                                            &completionSignal,
//                                            inputA + offset,
//                                            inputB + offset,
//                                            size));
//     }
// ```
// Finally, calling `wait` on the latch will block this function from returning
// until all the queued jobs computing the vector sum have been completed:
// ```
//     completionSignal.wait();
// }
// ```

#include <bslscm_version.h>

#include <bslmt_condition.h>
#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_libraryfeatures.h>
#include <bsls_systemclocktype.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <bslmt_chronoutil.h>

#include <bsl_chrono.h>
#endif

namespace BloombergLP {
namespace bslmt {

                             // ===========
                             // class Latch
                             // ===========

/// This class defines a thread synchronization mechanism that allows one or
/// more threads to wait until a certain number of operations have been
/// performed by other threads.
class Latch {

    // DATA
    Mutex           d_mutex;     // mutex used to synchronize threads waiting
                                 // for the latch to release

    Condition       d_cond;      // condition variable used for signaling
                                 // waiting threads

    bsls::AtomicInt d_sigCount;  // count of number of times this latch has
                                 // been "signaled"

  private:
    // NOT IMPLEMENTED
    Latch(const Latch&);
    Latch& operator=(const Latch&);

  public:
    // TYPES

    /// The value `timedWait` returns when a timeout occurs.
    enum { e_TIMED_OUT = Condition::e_TIMED_OUT };

    // CREATORS

    /// Create a latch that will synchronize on the specified `count`
    /// number of events, and when `count` events have been recorded will
    /// release any waiting threads.  Optionally specify a `clockType`
    /// indicating the type of the system clock against which the
    /// `bsls::TimeInterval` `absTime` timeouts passed to the `timedWait`
    /// method are to be interpreted.  If `clockType` is not specified then
    /// the realtime system clock is used.  The behavior is undefined unless
    /// `0 <= count`.
    explicit Latch(int                         count,
                   bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// Create a latch that will synchronize on the specified `count` number
    /// of events, and when `count` events have been recorded will release
    /// any waiting threads.  Use the realtime system clock as the clock
    /// against which the `absTime` timeouts passed to the `timedWait`
    /// methods are interpreted (see {Supported Clock-Types} in the
    /// component-level documentation).  The behavior is undefined unless
    /// `0 <= count`.
    Latch(int count, const bsl::chrono::system_clock&);

    /// Create a latch that will synchronize on the specified `count` number
    /// of events, and when `count` events have been recorded will release
    /// any waiting threads.  Use the monotonic system clock as the clock
    /// against which the `absTime` timeouts passed to the `timedWait`
    /// methods are interpreted (see {Supported Clock-Types} in the
    /// component-level documentation).  The behavior is undefined unless
    /// `0 <= count`.
    Latch(int count, const bsl::chrono::steady_clock&);
#endif

    /// Destroy this latch.  The behavior is undefined if any threads are
    /// waiting on this latch.
    ~Latch();

    // MANIPULATORS

    /// Decrement the number of events that this latch is waiting for by 1,
    /// and if the resulting number of events is 0 release any waiting
    /// threads.  The behavior is undefined unless the sum of all events
    /// that have arrived at this latch does not exceed the count with which
    /// it was initialized.  Note that the initial count of events is
    /// supplied at construction.
    void arrive();

    /// Decrement the number of events that this latch is waiting for by 1,
    /// and if the resulting number of events is 0 release any waiting
    /// threads; otherwise, block until the required number of events has
    /// been reached.  The behavior is undefined unless the sum of all
    /// events that have arrived at this latch does not exceed the count
    /// with which it was initialized.  Note that the initial count of
    /// events is supplied at construction.  Also note that this method is
    /// equivalent to the following sequence:
    /// ```
    /// arrive();
    /// wait();
    /// ```
    void arriveAndWait();

    /// Decrement the number of events that this latch is waiting for by the
    /// specified `numEvents`, and if the resulting number of events is 0
    /// release any waiting threads.  The behavior is undefined unless
    /// `numEvents > 0` and the sum of all events that have arrived at this
    /// latch does not exceed the count with which it was initialized.  Note
    /// that the initial count of events is supplied at construction.
    void countDown(int numEvents);

    /// Block until the number of events that this latch is waiting for
    /// reaches 0, or until the specified `absTime` timeout expires.  Return
    /// 0 on success, `e_TIMED_OUT` on timeout, and a non-zero value
    /// different from `e_TIMED_OUT` if an error occurs.  Errors are
    /// unrecoverable.  After an error, the latch may be destroyed, but any
    /// other use has undefined behavior.  `absTime` is an *absolute* time
    /// represented as an interval from some epoch as determined by the
    /// clock specified at construction (see {Supported Clock-Types} in the
    /// component-level documentation).  Note that a return value of 0 does not
    /// guarantee this object is safe to destroy.
    int timedWait(const bsls::TimeInterval& absTime);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// Block until the number of events that this latch is waiting for
    /// reaches 0, or until the specified `absTime` timeout expires.  Return
    /// 0 on success, `e_TIMED_OUT` on timeout, and a non-zero value
    /// different from `e_TIMED_OUT` if an error occurs.  Errors are
    /// unrecoverable.  After an error, the latch may be destroyed, but any
    /// other use has undefined behavior.  `absTime` is an *absolute* time
    /// represented as an interval from some epoch, which is determined by
    /// the clock associated with the time point.  Note that a return value of
    /// 0 does not guarantee this object is safe to destroy.
    template <class CLOCK, class DURATION>
    int timedWait(const bsl::chrono::time_point<CLOCK, DURATION>& absTime);
#endif

    /// Block until the number of events that this latch is waiting for
    /// reaches 0.  Note that completion of this method does not guarantee this
    /// object is safe to destroy.
    void wait();

    // ACCESSORS

    /// Return the clock type used for timeouts.
    bsls::SystemClockType::Enum clockType() const;

    /// Return the current number of events for which this latch is waiting.
    /// Note that this method is provided primarily for debugging purposes
    /// (i.e., its intended use is not as a synchronization mechanism), and
    /// can be used only as an upper bound for the current count without
    /// other external state information.
    int currentCount() const;

    /// Return `true` if this latch has already been released (i.e., the
    /// number of events the latch is waiting on is 0), and `false`
    /// otherwise.  This method does not block.  Note that a return value
    /// of `true` indicates a permanent state change (the latch has released
    /// and will never be un-released), but a return value of `false` is
    /// ephemeral and cannot typically be acted upon without additional
    /// external state information.  Also note that a return value of `true`
    /// does not guarantee this object is safe to destroy.
    bool tryWait() const;
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                             // -----------
                             // class Latch
                             // -----------

// CREATORS
inline
Latch::Latch(int count, bsls::SystemClockType::Enum clockType)
: d_mutex()
, d_cond(clockType)
, d_sigCount(count)
{
    BSLS_ASSERT_SAFE(0 <= count);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
Latch::Latch(int count, const bsl::chrono::system_clock&)
: d_mutex()
, d_cond(bsls::SystemClockType::e_REALTIME)
, d_sigCount(count)
{
    BSLS_ASSERT_SAFE(0 <= count);
}

inline
Latch::Latch(int count, const bsl::chrono::steady_clock&)
: d_mutex()
, d_cond(bsls::SystemClockType::e_MONOTONIC)
, d_sigCount(count)
{
    BSLS_ASSERT_SAFE(0 <= count);
}

// MANIPULATORS
template <class CLOCK, class DURATION>
inline
int Latch::timedWait(const bsl::chrono::time_point<CLOCK, DURATION>& absTime)
{
    return bslmt::ChronoUtil::timedWait(this, absTime);
}
#endif

// ACCESSORS
inline
bsls::SystemClockType::Enum Latch::clockType() const
{
    return d_cond.clockType();
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
