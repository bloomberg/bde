// bslmt_timedcompletionguard.h                                       -*-C++-*-
#ifndef INCLUDED_BSLMT_TIMEDCOMPLETIONGUARD
#define INCLUDED_BSLMT_TIMEDCOMPLETIONGUARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide guard to verify work completion within a set duration.
//
//@CLASSES:
//  bslmt::TimedCompletionGuard: guard to verify completion within a duration
//
//@DESCRIPTION: This component provides a thread-enabled guard,
// `bslmt::TimedCompletionGuard`, which invokes a handler displaying provided
// diagnostic text if the guard is not destroyed or released within a specified
// duration.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Guarding Work on a Thread
///- - - - - - - - - - - - - - - - - -
// In the following example a `bslmt::TimedCompletionGuard` is used as a
// testing aid verify work done on a thread completes in a specified amount of
// time.  The work is done in two parts, but the actual tasks are not relevant
// to the example and is elided.
//
// First, define the two thread functions containing the work to be done:
// ```
//  /// Do some work based upon the specified `arg`.
//  void *myWorkPart1(void *arg)
//  {
//  //  // do some stuff...
//      (void)arg;
//      return 0;
//  }
//
//  /// Do some other work based upon the specified `arg`.
//  void *myWorkPart2(void *arg)
//  {
//      // do some other stuff...
//      (void)arg;
//      return 0;
//  }
// ```
// Next, we create and configure `tcg` to allow both parts of the work one
// second to complete and verify the guard was started successfully:
// ```
//  bslmt::TimedCompletionGuard tcg;
//
//  assert(0 == tcg.guard(bsls::TimeInterval(1, 0), "first part"));
// ```
// Then, we create a thread to execute the first part of the work, and join the
// created thread when the work completes:
// ```
//  {
//      bslmt::ThreadUtil::Handle workerHandle;
//
//      assert(0 == bslmt::ThreadUtil::create(&workerHandle, myWorkPart1, 0));
//
//      bslmt::ThreadUtil::join(workerHandle);
//  }
// ```
// Next, we update the text displayed if the duration should expire to reflect
// the second portion of the work:
// ```
//  tcg.updateText("second part");
// ```
// Now, we use a thread to execute the second part of the work:
// ```
//  {
//      bslmt::ThreadUtil::Handle workerHandle;
//
//      assert(0 == bslmt::ThreadUtil::create(&workerHandle, myWorkPart2, 0));
//
//      bslmt::ThreadUtil::join(workerHandle);
//  }
// ```
// Finally, we release the guard:
// ```
//  tcg.release();
// ```

#include <bslscm_version.h>

#include <bslma_allocator.h>

#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bsls_annotation.h>
#include <bsls_timeinterval.h>
#include <bsls_systemclocktype.h>

#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bslmt {

                       // ==========================
                       // class TimedCompletionGuard
                       // ==========================

/// This class implements a guard for work completion within a specified
/// duration.
class TimedCompletionGuard {
  public:
    // TYPES

    /// `Handler` is an alias for a pointer to a function returning `void`,
    /// and taking, as a parameter, a null-terminated string, which is the
    /// structure of the handler function supported by this class -- e.g.,
    /// ```
    /// void myHandler(const char *text);
    /// ```
    typedef void (*Handler)(const char *);

  private:

    // CLASS DATA
    static const bsls::TimeInterval k_DISABLED;  // value used in
                                                 // `d_expiration` to indicate
                                                 // thread should return from
                                                 // `guardThreadFunction`

    static const bsls::TimeInterval k_WAITING;   // value used in
                                                 // `d_expiration` to indicate
                                                 // thread is waiting for an
                                                 // expiration value

    // PRIVATE CLASS METHODS

    /// Thread function that implements the guard functionality on the
    /// specified `arg`, which is a pointer to a `ThreadCompletionGuard`
    /// object.
    static void *guardThreadFunction(void *arg);

    // DATA
    mutable bslmt::Mutex      d_handleMutex;   // mutex protecting
                                               // `d_threadHandle`,
                                               // synchronizes all public
                                               // methods

    bslmt::Mutex              d_dataMutex;     // mutex protecting all other
                                               // data, and used with
                                               // `d_condition`

    bslmt::Condition          d_condition;     // condition variable used to
                                               // signal when the work is
                                               // complete

    bslmt::ThreadUtil::Handle d_threadHandle;  // thread handle

    bsls::TimeInterval        d_expiration;    // expiration time

    bsl::string               d_text;          // text to display at expiration

    Handler                   d_handler;       // handler function for
                                               // expiration

    // NOT IMPLEMENTED
    TimedCompletionGuard(const TimedCompletionGuard&);
    TimedCompletionGuard& operator=(const TimedCompletionGuard&);

  public:
    // TYPES
    typedef bsl::allocator<> allocator_type;  // allocator-aware trait

    // CLASS METHODS

    /// (Default Handler) Emulate the invocation of the standard `assert`
    /// macro with a `false` argument, using the expression `text` to
    /// generate a helpful output message and then, after logging,
    /// unconditionally aborting.  Note that this handler function is the
    /// default installed assertion handler.
    BSLS_ANNOTATION_NORETURN
    static void failByAbort(const char *text);

    // CREATORS

    /// Create the guard.  Optionally specify a `handler` function that is
    /// invoked when a `set` duration expires.  If `handler` is not specified,
    /// `failByAbort` is used.  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is not specified, the currently
    /// installed default allocator is used.
    TimedCompletionGuard();
    explicit TimedCompletionGuard(const allocator_type& basicAllocator);
    TimedCompletionGuard(
                      Handler               function,
                      const allocator_type& basicAllocator = allocator_type());

    /// Invoke `release` and destroy this guard.
    ~TimedCompletionGuard();

    // MANIPULATORS

    /// Configure a thread to, at the expiration of the specified `duration`,
    /// invoke the handler assigned at object creation with the specified
    /// `text`, unless the `release` method is invoked or this guard is
    /// rescheduled with a subsequent call to `guard`.  Return 0 on success,
    /// and a non-zero value if a thread is not available and could not be
    /// created.  The behavior is undefined unless `duration` is a positive
    /// value.
    int guard(const bsls::TimeInterval& duration,
              const bsl::string_view&   text);

    /// If a thread is waiting to alert at expiration of the `set` duration,
    /// join the thread, thus preventing any alerting.
    void release();

    /// Use the specified `text` for alerting when the current duration
    /// expires.  Return 0 on success, and a non-zero value if
    /// `false == isGuarding()`.
    int updateText(const bsl::string_view& text);

    // ACCESSORS

    /// Return the allocator used to supply memory.
    allocator_type get_allocator() const;


    /// Return `true` if this guard is currently configured with a duration to
    /// guard, and `false` otherwise.
    bool isGuarding() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                       // --------------------------
                       // class TimedCompletionGuard
                       // --------------------------

// CREATORS
inline
TimedCompletionGuard::TimedCompletionGuard()
: d_handleMutex()
, d_dataMutex()
, d_condition(bsls::SystemClockType::e_MONOTONIC)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_expiration(k_DISABLED)
, d_text()
, d_handler(failByAbort)
{
}

inline
TimedCompletionGuard::TimedCompletionGuard(
                                          const allocator_type& basicAllocator)
: d_handleMutex()
, d_dataMutex()
, d_condition(bsls::SystemClockType::e_MONOTONIC)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_expiration(k_DISABLED)
, d_text(basicAllocator)
, d_handler(failByAbort)
{
}

inline
TimedCompletionGuard::TimedCompletionGuard(
                                          Handler               function,
                                          const allocator_type& basicAllocator)
: d_handleMutex()
, d_dataMutex()
, d_condition(bsls::SystemClockType::e_MONOTONIC)
, d_threadHandle(bslmt::ThreadUtil::invalidHandle())
, d_expiration(k_DISABLED)
, d_text(basicAllocator)
, d_handler(function)
{
}

inline
TimedCompletionGuard::~TimedCompletionGuard()
{
    release();
}

// ACCESSORS

inline
TimedCompletionGuard::allocator_type TimedCompletionGuard::get_allocator()
                                                                          const
{
    return d_text.get_allocator();
}

inline
bool TimedCompletionGuard::isGuarding() const
{
    bslmt::LockGuard<bslmt::Mutex> guardHandle(&d_handleMutex);

    return k_DISABLED != d_expiration && k_WAITING != d_expiration;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
