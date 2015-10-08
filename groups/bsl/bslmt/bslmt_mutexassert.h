// bslmt_mutexassert.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_MUTEXASSERT
#define INCLUDED_BSLMT_MUTEXASSERT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an assert macro for verifying that a mutex is locked.
//
//@CLASSES:
//
//@MACROS:
//  BSLMT_MUTEXASSERT_IS_LOCKED: verify a mutex is locked in non-opt modes
//  BSLMT_MUTEXASSERT_IS_LOCKED_SAFE: verify a mutex is locked in safe mode
//  BSLMT_MUTEXASSERT_IS_LOCKED_OPT: verify a mutex is locked in all modes
//
//@SEE_ALSO: bslmt_mutex
//
//@DESCRIPTION: This component provides macros for asserting that a mutex is
// locked.  It does not distinguish between locks held by the current thread or
// other threads.  If the macro is active in the current build mode, when the
// macro is called, if the supplied mutex is unlocked, the assert handler
// installed for 'BSLS_ASSERT' will be called.  The assert handler installed by
// default will report an error and abort the task.
//
// The three macros defined by the component are analogous to the macros
// defined by 'BSLS_ASSERT':
//
//: o 'BSLMT_MUTEXASSERT_IS_LOCKED': active when 'BSLS_ASSERT' is active
//:
//: o 'BSLMT_MUTEXASSERT_IS_LOCKED_SAFE': active when 'BSLS_ASSERT_SAFE' is
//:   active
//:
//: o 'BSLMT_MUTEXASSERT_IS_LOCKED_OPT': active when 'BSLS_ASSERT_OPT' is
//:   active
//
// In build modes where any one of these macros is not active, calling it will
// have no effect.
//
// If any of these asserts are in effect and fail (because the mutex in
// question was unlocked), the behavior parallels the behavior of the assertion
// macros defined in 'bsls_assert.h' -- 'bsls::Assert::invokeHandler' is
// called, with a source code expression, the name of the source file, and the
// line number in the source file where the macro was called.  If the default
// handler is installed, this will result in an error message and an abort.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Checking Consistency Within a Private Method
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes multithreaded code is written such that the author of a function
// requires that a caller has already acquired a mutex.  The
// 'BSLMT_MUTEXASSERT_IS_LOCKED*' family of assertions allows the programmers
// to verify, using defensive programming techniques, that the mutex in
// question is indeed locked.
//
// Suppose we have a fully thread-safe queue that contains 'int' values, and is
// guarded by an internal mutex.  We can use 'BSLMT_MUTEXASSERT_IS_LOCKED_SAFE'
// to ensure (in appropriate build modes) that proper internal locking of the
// mutex is taking place.
//
// First, we define the container:
//..
//  class MyThreadSafeQueue {
//      // This 'class' provides a fully *thread-safe* unidirectional queue of
//      // 'int' values.  See {'bsls_glossary'|Fully Thread-Safe}.  All public
//      // manipulators operate as single, atomic actions.
//
//      // DATA
//      bsl::deque<int>      d_deque;    // underlying non-*thread-safe*
//                                       // standard container
//
//      mutable bslmt::Mutex  d_mutex;    // mutex to provide thread safety
//
//      // PRIVATE MANIPULATOR
//      int popImp(int *result);
//          // Assign the value at the front of the queue to the specified
//          // '*result', and remove the value at the front of the queue;
//          // return 0 if the queue was not initially empty, and a non-zero
//          // value (with no effect) otherwise.  The behavior is undefined
//          // unless 'd_mutex' is locked.
//
//    public:
//      // ...
//
//      // MANIPULATORS
//      int pop(int *result);
//          // Assign the value at the front of the queue to the specified
//          // '*result', and remove the value at the front of the queue;
//          // return 0 if the queue was not initially empty, and a non-zero
//          // value (with no effect) otherwise.
//
//      void popAll(bsl::vector<int> *result);
//          // Assign the values of all the elements from this queue, in order,
//          // to the specified '*result', and remove them from this queue.
//          // Any previous contents of '*result' are discarded.  Note that, as
//          // with the other public manipulators, this entire operation occurs
//          // as a single, atomic action.
//
//      void push(int value);
//          // ...
//
//      template <class INPUT_ITER>
//      void pushRange(const INPUT_ITER& first, const INPUT_ITER& last);
//          // ...
//  };
//..
// Notice that our public manipulators have two forms: push/pop a single
// element, and push/pop a collection of elements.  Popping even a single
// element is non-trivial, so we factor this operation into a non-*thread-safe*
// private manipulator that performs the pop, and is used in both public 'pop'
// methods.  This private manipulator requires that the mutex be locked, but
// cannot lock the mutex itself, since the correctness of 'popAll' demands that
// all of the pops be collectively performed using a single mutex lock/unlock.
//
// Then, we define the private manipulator:
//..
//  // PRIVATE MANIPULATOR
//  int MyThreadSafeQueue::popImp(int *result)
//  {
//      BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&d_mutex);
//
//      if (d_deque.empty()) {
//          return -1;                                                // RETURN
//      }
//      else {
//          *result = d_deque.front();
//          d_deque.pop_front();
//          return 0;                                                 // RETURN
//      }
//  }
//..
// Notice that, on the very first line, the private manipulator verifies, as a
// precondition check, that the mutex has been acquired, using one of the
// 'BSLMT_MUTEXASSERT_IS_LOCKED*' macros.  We use the '...IS_LOCKED_SAFE...'
// version of the macro so that the check, which on some platforms is as
// expensive as locking the mutex, is performed in only the safe build mode.
//
// Next, we define the public manipulators; each of which must acquire a lock
// on the mutex (note that there is a bug in 'popAll'):
//..
//  // MANIPULATORS
//  int MyThreadSafeQueue::pop(int *result)
//  {
//      BSLS_ASSERT(result);
//
//      d_mutex.lock();
//      int rc = popImp(result);
//      d_mutex.unlock();
//      return rc;
//  }
//
//  void MyThreadSafeQueue::popAll(bsl::vector<int> *result)
//  {
//      BSLS_ASSERT(result);
//
//      const int size = static_cast<int>(d_deque.size());
//      result->resize(size);
//      int *begin = result->begin();
//      for (int index = 0; index < size; ++index) {
//          int rc = popImp(&begin[index]);
//          BSLS_ASSERT(0 == rc);
//      }
//  }
//
//  void MyThreadSafeQueue::push(int value)
//  {
//      d_mutex.lock();
//      d_deque.push_back(value);
//      d_mutex.unlock();
//  }
//
//  template <class INPUT_ITER>
//  void MyThreadSafeQueue::pushRange(const INPUT_ITER& first,
//                                    const INPUT_ITER& last)
//  {
//      d_mutex.lock();
//      d_deque.insert(d_deque.begin(), first, last);
//      d_mutex.unlock();
//  }
//..
// Notice that, in 'popAll', we forgot to lock/unlock the mutex!
//
// Then, in our function 'example2Function', we make use of our class to create
// and exercise a 'MyThreadSafeQueue' object:
//..
//  void testThreadSafeQueue(bsl::ostream& stream)
//  {
//      MyThreadSafeQueue queue;
//..
// Next, we populate the queue using 'pushRange':
//..
//      const int rawData[] = { 17, 3, 21, -19, 4, 87, 29, 3, 101, 31, 36 };
//      enum { k_RAW_DATA_LENGTH = sizeof rawData / sizeof *rawData };
//
//      queue.pushRange(rawData + 0, rawData + k_RAW_DATA_LENGTH);
//..
// Then, we pop a few items off the front of the queue and verify their values:
//..
//      int value = -1;
//
//      assert(0 == queue.pop(&value));    assert(17 == value);
//      assert(0 == queue.pop(&value));    assert( 3 == value);
//      assert(0 == queue.pop(&value));    assert(21 == value);
//..
// Next, we attempt to empty the queue with 'popAll', which, if built in safe
// mode, would fail because it neglects to lock the mutex:
//..
//      bsl::vector<int> v;
//      queue.popAll(&v);
//
//      stream << "Remaining raw numbers: ";
//      for (bsl::size_t ti = 0; ti < v.size(); ++ti) {
//          stream << (ti ? ", " : "") << v[ti];
//      }
//      stream << bsl::endl;
//  }
//..
// Then, we build in non-safe mode and run:
//..
//  Remaining raw numbers: -19, 4, 87, 29, 3, 101, 31, 36
//..
// Notice that, since the test case is being run in a single thread and our
// check is disabled, the bug where the mutex was not acquired does not
// manifest itself in a visible error, and we observe the seemingly correct
// output.
//
// Now, we build in safe mode (which enables our check), run the program (which
// calls 'example2Function'), and observe that, when we call 'popAll', the
// 'BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&d_mutex)' macro issues an error message
// and aborts:
//..
//  Assertion failed: BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(&d_mutex), file /bb/big
//  storn/dev_framework/bchapman/git/bde-core/groups/bce/bslmt/unix-Linux-x86_6
//  4-2.6.18-gcc-4.6.1/bslmt_mutexassertislocked.t.cpp, line 137
//  Aborted (core dumped)
//..
// Finally, note that the message printed above and the subsequent aborting of
// the program were the result of a call to 'bsls::Assert::invokeHandler',
// which in this case was configured (by default) to call
// 'bsls::Assert::failAbort'.  Other handlers may be installed that produce
// different results, but in all cases should prevent the program from
// proceeding normally.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLMT_MUTEXASSERT_IS_LOCKED(mutex_p) do {                         \
        bslmt::MutexAssert_Imp::assertIsLockedImpl(                           \
                           (mutex_p),                                         \
                           "BSLMT_MUTEXASSERT_IS_LOCKED(" #mutex_p ")",       \
                           __FILE__,                                          \
                           __LINE__); } while (false)
#else
    #define BSLMT_MUTEXASSERT_IS_LOCKED(mutex_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(mutex_p) do {                    \
        bslmt::MutexAssert_Imp::assertIsLockedImpl(                           \
                           (mutex_p),                                         \
                           "BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(" #mutex_p ")",  \
                           __FILE__,                                          \
                           __LINE__); } while (false)
#else
    #define BSLMT_MUTEXASSERT_IS_LOCKED_SAFE(mutex_p) ((void) 0)
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLMT_MUTEXASSERT_IS_LOCKED_OPT(mutex_p) do {                     \
        bslmt::MutexAssert_Imp::assertIsLockedImpl(                           \
                           (mutex_p),                                         \
                           "BSLMT_MUTEXASSERT_IS_LOCKED_OPT(" #mutex_p ")",   \
                           __FILE__,                                          \
                           __LINE__); } while (false)
#else
    #define BSLMT_MUTEXASSERT_IS_LOCKED_OPT(mutex_p) ((void) 0)
#endif

namespace BloombergLP {
namespace bslmt {

class Mutex;

                          // =====================
                          // class MutexAssert_Imp
                          // =====================

struct MutexAssert_Imp {
    // This 'struct' provides a (component private) namespace for
    // implementation functions of the assert macros defined in this component.
    // This class should *not* be used directly in client code.

    // CLASS METHODS
    static void assertIsLockedImpl(Mutex      *mutex,
                                   const char *text,
                                   const char *file,
                                   int         line);
        // If the specified 'mutex' is not locked, call
        // 'bsls::Assert::invokeHandler' with the specified 'text', 'file', and
        // 'line', where 'text' is text describing the assertion being
        // performed, 'file' is the name of the source file that called the
        // macro, and 'line' is the line number in the file where the macro was
        // called.  This function is intended to implement
        // 'BSLMT_MUTEXASSERT_IS_LOCKED', 'BSLMT_MUTEXASSERT_IS_LOCKED_SAFE',
        // and 'BSLMT_MUTEXASSERT_IS_LOCKED_OPT' and should not otherwise be
        // called directly.
};
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
