// bslmt_once.h                                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_ONCE
#define INCLUDED_BSLMT_ONCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe way to execute code once per process.
//
//@CLASSES:
//        bslmt::Once: Gate-keeper for code executed only once per process
//   bslmt::OnceGuard: Guard class for safely using 'bslmt::Once'
//
//@SEE_ALSO: bslmt_qlock
//
//@DESCRIPTION: This component provides a pair of classes, 'bslmt::Once' and
// 'bslmt::OnceGuard', which give the caller a way to run a body of code
// exactly once within the current process, particularly in the presence of
// multiple threads.  This component also defines the macro 'BSLMT_ONCE_DO',
// which provides syntactic sugar to make one-time execution nearly fool-proof.
// A common use of one-time execution is the initialization of singletons on
// first use.
//
// The 'bslmt::Once' class is designed to be statically allocated and
// initialized using the 'BSLMT_ONCE_INITIALIZER' macro.  Client code may use
// the 'bslmt::Once' object in one of two ways: 1) it may use the 'callOnce'
// method to call a function or functor or 2) it may call the 'enter' and
// 'leave' methods just before and after the code that is intended to be
// executed only once.  That code must be executed conditionally on 'enter'
// returning 'true', indicating that the caller is the first thread to pass
// through this region of code.  The 'leave' method must be executed at the end
// of the code region, indicating that the one-time execution has completed and
// unblocking any threads waiting on 'enter'.
//
// A safer way to use the 'enter' and 'leave' methods of 'bslmt::Once' is to
// manage the 'bslmt::Once' object using a 'bslmt::OnceGuard' object
// constructed from the 'bslmt::Once' object.  Calling 'enter' on the
// 'bslmt::OnceGuard' object will call 'enter' on its associated 'bslmt::Once'
// object.  If the call to 'enter' returns 'true', then the destructor for the
// guard will automatically call 'leave' on its associated 'bslmt::Once'
// object.  The 'bslmt::OnceGuard' class is intended to be allocated on the
// stack (i.e., as a local variable) so that it is automatically destroyed at
// the end of its enclosing block.  Thus, the to call 'leave' of the
// 'bslmt::Once' object is enforced by the compiler.
//
// An even easier way to use the facilities of this component is to use the
// 'BSLMT_ONCE_DO' macro.  This macro behaves like an 'if' statement --
// executing the following [compound] statement the first time the control
// passes through it in the course of a program's execution, and blocking other
// calling threads until the [compound] statement is executed the first time.
// Thus, bracketing arbitrary code in a 'BSLMT_ONCE_DO' construct is the
// easiest way to ensure that code will be executed only once for a program.
// The 'BSLMT_ONCE_DO' behaves correctly even if there are 'return' statements
// within the one-time code block.
//
// The implementation of this component uses appropriate memory barriers so
// that changes made in the one-time execution code are immediately visible to
// all threads at the end of the one-time code block.
//
///Warning
///-------
// The 'BSLMT_ONCE_DO' macro consists of a declaration and a 'for' loop.
// Consequently, the following is syntactically incorrect:
//..
//  if (xyz) BSLMT_ONCE_DO { stuff() }
//..
// Also, a 'break' or 'continue' statement within a 'BSLMT_ONCE_DO' construct
// terminates the 'BSLMT_ONCE_DO', not a surrounding loop or 'switch'
// statement.  For example:
//..
//  switch (xyz) {
//    case 0: BSLMT_ONCE_DO { stuff(); break; /* does not break case */ }
//    case 1: // Oops! case 0 falls through to here.
//  }
//..
//
///Thread Safety
///-------------
// Objects of the 'bslmt::Once' class are intended to be shared among threads
// and may be accessed and modified simultaneously in multiple threads by using
// the methods provided.  To allow static initialization, 'bslmt::Once' is a
// POD type with public member variables.  It is not safe to directly access or
// manipulate its member variables (including object initialization)
// simultaneously from multiple threads.  (Note that static initialization
// takes place before multiple threading begins, and is thus safe.)
//
// The 'bslmt::OnceGuard' objects are designed to be used only by their creator
// threads and are typically created on the stack.  It is not safe to use a
// 'bslmt::OnceGuard' by a thread other than its creator.
//
///Usage
///-----
// Typically, the facilities in this component are used to implement a
// thread-safe singleton.  Below, we implement the a singleton four ways,
// illustrating the two ways to directly use 'bslmt::Once', the use of
// 'bslmt::OnceGuard', and the use of 'BSLMT_ONCE_DO'.  In each example, the
// singleton functions take a C-string ('const char*') argument and return a
// reference to a 'bsl::string' object constructed from the input string.  Only
// the first call to each singleton function affect the contents of the
// singleton string.  (The argument is ignored on subsequent calls.)
//
///First Implementation
/// - - - - - - - - - -
// Our first implementation uses the 'BSLMT_ONCE_DO' construct, the
// recommended way to use this component.  The function is a variation of the
// singleton pattern described by Scott Meyers, except that the 'BSLMT_ONCE_DO'
// macro is used to handle multiple entries to the function in a thread-safe
// manner:
//..
//  const bsl::string& singleton0(const char *s)
//  {
//      static bsl::string *theSingletonPtr = 0;
//      BSLMT_ONCE_DO {
//          static bsl::string theSingleton(s,
//                                          bslma::Default::globalAllocator());
//          theSingletonPtr = &theSingleton;
//      }
//      return *theSingletonPtr;
//  }
//..
// The 'BSLMT_ONCE_DO' mechanism suffices for most situations; however, if more
// flexibility is required, review the remaining examples in this series for
// more design choices.  The next example will use the lowest-level facilities
// of 'bslmt::Once'.  The two following examples use progressively higher-level
// facilities to produce simpler singleton implementations (though none as
// simple as the 'BSLMT_ONCE_DO' example above).
//
///Second Implementation
///- - - - - - - - - - -
// The next singleton function implementation directly uses the 'doOnce' method
// of 'bslmt::Once'.  We begin by declaring a simple function that does most of
// the work of the singleton, i.e., constructing the string and setting a
// (static) pointer to the string:
//..
//  static bsl::string *theSingletonPtr = 0;
//
//  void singletonImp(const char *s)
//  {
//      static bsl::string theSingleton(s);
//      theSingletonPtr = &theSingleton;
//  }
//..
// The above function is *not* thread-safe.  Firstly, many threads might
// attempt to simultaneously construct the 'theSingleton' object.  Secondly,
// once 'theSingletonPtr' is set by one thread, other threads still might not
// see the change (and try to initialize the singleton again).
//
// The 'singleton1' function, below, calls 'singletonImp' via the 'callOnce'
// method of 'bslmt::Once' to ensure that 'singletonImp' is called by only one
// thread and that the result is visible to all threads.  We start by creating
// and initializing a static object of type 'bslmt::Once':
//..
//  #include <bdlf_bind.h>
//
//  const bsl::string& singleton1(const char *s)
//  {
//      static bslmt::Once once = BSLMT_ONCE_INITIALIZER;
//..
// Since the 'callOnce' method takes only a no-argument functor (or function),
// to call 'callOnce', we must bind our argument 's' to our function,
// 'singletonImp' using a binder method and then pass that functor to
// 'callOnce'.  The first thread (and only the first thread) entering this
// section of code we will set 'theSingleton'.
//..
//      once.callOnce(bdlf::BindUtil::bind(singletonImp, s));
//      return *theSingletonPtr;
//  }
//..
// Once we return from 'callOnce', the appropriate memory barrier has been
// executed so that the change to 'theSingletonPtr' is visible to all threads.
// A thread calling 'callOnce' after the initialization has completed would
// immediately return from the call.  A thread calling 'callOnce' while
// initialization is still in progress would block until initialization
// completes and then return.
//
// *Implementation* *Note*: As an optimization, developers sometimes pre-check
// the value to be set, 'theSingletonPtr' in this case, to avoid (heavy) memory
// barrier operations; however, that practice is not recommended here.  First,
// the value of the string may be cached by a different CPU, even though the
// pointer has already been updated on the common memory bus.  Second, The
// implementation of the 'callOnce' method is fast enough that a pre-check
// would not provide any performance benefit.
//
// The one advantage of this implementation over the previous one is that an
// exception thrown from within 'singletonImp' will cause the 'bslmt::Once'
// object to be restored to its original state, so that the next entry into the
// singleton will retry the operation.
//
///Third Implementation
/// - - - - - - - - - -
// Our next implementation, 'singleton2', eliminates the need for the
// 'singletonImp' function and thereby does away with the use of the
// 'bdlf::BindUtil' method; however, it does require use of
// 'bslmt::Once::OnceLock', created on each thread's stack and passed to the
// methods of 'bslmt::Once'.  First, we declare a static 'bslmt::Once' object
// as before, and also declare a static pointer to 'bsl::string':
//..
//  const bsl::string& singleton2(const char *s)
//  {
//      static bslmt::Once   once            = BSLMT_ONCE_INITIALIZER;
//      static bsl::string *theSingletonPtr = 0;
//..
// Next, we define a local 'bslmt::Once::OnceLock' object and pass it to the
// 'enter' method:
//..
//      bslmt::Once::OnceLock onceLock;
//      if (once.enter(&onceLock)) {
//..
// If the 'enter' method returns 'true', we proceed with the initialization of
// the singleton, as before.
//..
//          static bsl::string theSingleton(s);
//          theSingletonPtr = &theSingleton;
//..
// When initialization is complete, the 'leave' method is called for the same
// context cookie previously used in the call to 'enter':
//..
//          once.leave(&onceLock);
//      }
//..
// When any thread reaches this point, initialization has been complete and
// initialized string is returned:
//..
//      return *theSingletonPtr;
//  }
//..
//
///Fourth Implementation
///- - - - - - - - - - -
// Our final implementation, 'singleton3', uses 'bslmt::OnceGuard' to simplify
// the previous implementation by using 'bslmt::OnceGuard' to hide (automate)
// the use of 'bslmt::Once::OnceLock'.  We begin as before, defining a static
// 'bslmt::Once' object and a static 'bsl::string' pointer:
//..
//  const bsl::string& singleton3(const char *s)
//  {
//      static bslmt::Once  once            = BSLMT_ONCE_INITIALIZER;
//      static bsl::string *theSingletonPtr = 0;
//..
// We then declare a local 'bslmt::OnceGuard' object and associate it with the
// 'bslmt::Once' object before entering the one-time initialization region:
//..
//      bslmt::OnceGuard onceGuard(&once);
//      if (onceGuard.enter()) {
//          static bsl::string theSingleton(s);
//          theSingletonPtr = &theSingleton;
//      }
//      return *theSingletonPtr;
//  }
//..
// Note that it is unnecessary to call 'onceGuard.leave()' because that is
// called automatically before the function returns.  This machinery makes the
// code more robust in the presence of, e.g., return statements in the
// initialization code.
//
// If there is significant code after the end of the one-time initialization,
// the guard and the initialization code should be enclosed in an extra block
// so that the guard is destroyed as soon as validly possible and allow other
// threads waiting on the initialization to continue.  Alternatively, one can
// call 'onceGuard.leave()' explicitly at the end of the initialization.
//
///Using the Semaphore Implementations
///- - - - - - - - - - - - - - - - - -
// The following pair of functions, 'thread1func' and 'thread2func' which will
// be run by different threads:
//..
//  void *thread1func(void *)
//  {
//      const bsl::string& s0 = singleton0("0 hello");
//      const bsl::string& s1 = singleton1("1 hello");
//      const bsl::string& s2 = singleton2("2 hello");
//      const bsl::string& s3 = singleton3("3 hello");
//
//      assert('0' == s0[0]);
//      assert('1' == s1[0]);
//      assert('2' == s2[0]);
//      assert('3' == s3[0]);
//
//      // ... lots more code goes here
//      return 0;
//  }
//
//  void *thread2func(void *)
//  {
//      const bsl::string& s0 = singleton0("0 goodbye");
//      const bsl::string& s1 = singleton1("1 goodbye");
//      const bsl::string& s2 = singleton2("2 goodbye");
//      const bsl::string& s3 = singleton3("3 goodbye");
//
//      assert('0' == s0[0]);
//      assert('1' == s1[0]);
//      assert('2' == s2[0]);
//      assert('3' == s3[0]);
//
//      // ... lots more code goes here
//      return 0;
//  }
//..
// Both threads attempt to initialize the four singletons.  In our example,
// each thread passes a distinct argument to the singleton, allowing us to
// identify the thread that initializes the singleton.  (In practice, the
// arguments passed to a specific singleton are almost always fixed and most
// singletons don't take arguments at all.)
//
// Assuming that the first thread function wins all of the races to initialize
// the singletons, the first singleton is set to "0 hello", the second
// singleton to "1 hello", etc.
//..
//  int usageExample1()
//  {
//      void startThread1();
//      void startThread2();
//
//      startThread1();
//      startThread2();
//
//      assert(singleton0("0") == "0 hello");
//      assert(singleton1("1") == "1 hello");
//      assert(singleton2("2") == "2 hello");
//      assert(singleton3("3") == "3 hello");
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_QLOCK
#include <bslmt_qlock.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

#if defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLMT_ONCE_UNIQNUM __COUNTER__
        // MSVC: The '__LINE__' macro breaks when '/ZI' is used (see Q199057 or
        // KB199057).  Fortunately the '__COUNTER__' extension provided by MSVC
        // is even better.
#else
#   define BSLMT_ONCE_UNIQNUM __LINE__
#endif

#define BSLMT_ONCE_DO \
    BSLMT_ONCE_DO_IMP(BSLMT_ONCE_CAT(bslmt_doOnceObj, BSLMT_ONCE_UNIQNUM))
    // This macro provides a simple control construct to bracket a piece of
    // code that should only be executed once during the course of a
    // multithreaded program.  Usage:
    //..
    //  BSLMT_ONCE_DO { /* one-time code goes here */ }
    //..
    // Leaving a 'BSLMT_ONCE_DO' construct via 'break', 'continue', or 'return'
    // will put the construct in a "done" state (unless 'BSLMT_ONCE_CANCEL' has
    // been called) and will unblock all threads waiting to enter the one-time
    // region.  Note that a 'break' or 'continue' within the one-time code will
    // terminate only the 'BSLMT_ONCE_DO' construct, not any surrounding loop
    // or switch statement.  Due to a bug in the Microsoft Visual C++ 2003
    // compiler, the behavior is undefined if an exception is thrown from
    // within this construct and is not caught within the same construct.  Only
    // one call to 'BSLMT_ONCE_DO' may appear on a single source-code line in
    // any code block.

#define BSLMT_ONCE_CANCEL() bslmt_doOnceGuard.cancel()
    // This macro provides a way to cancel once processing within a
    // 'BSLMT_ONCE_DO' construct.  It will not compile outside of a
    // 'BSLMT_ONCE_DO' construct.  Executing this function-like macro will set
    // the state of the 'BSLMT_ONCE_DO' construct to "not entered", possibly
    // unblocking a thread waiting to enter the one-time code region.  Note
    // that this macro does not exit the 'BSLMT_ONCE_DO' construct (i.e., it
    // does not have 'break' or 'return' semantics).

#define BSLMT_ONCE_INITIALIZER { BSLMT_QLOCK_INITIALIZER, { 0 } }

namespace bslmt {

    // Use this macro to initialize an object of type 'Once'.  E.g.:
    //..
    //  Once once = BSLMT_ONCE_INITIALIZER;
    //..

                                // ==========
                                // class Once
                                // ==========

class Once {
    // Gate-keeper class for code that should only execute once per process.
    // This class is a POD-type and can be statically initialized to the value
    // of the 'BSLMT_ONCE_INITIALIZE' macro.  For this reason, it does not have
    // any explicitly-declared constructors or destructor.

    // PRIVATE TYPES
    enum { e_NOT_ENTERED, e_IN_PROGRESS, e_DONE };

  private:
    // NOT IMPLEMENTED
    Once& operator=(const Once&);
        // Copy-assignment is not allowed.  We cannot declare a private copy
        // constructor because that would make this class a non-POD.

  public:
    // These variables are public so that this (POD) type can be statically
    // initialized.  Do not access these variables directly.

    // DATA
    QLock                                    d_mutex;
                                        // public, but do *not* access directly
    bsls::AtomicOperations::AtomicTypes::Int d_state;
                                        // public, but do *not* access directly

  public:
    // PUBLIC TYPES
    typedef QLockGuard OnceLock;
        // Special token created by a single thread to pass to the 'enter',
        // 'leave', and 'cancel' methods.

    // MANIPULATORS
    bool enter(OnceLock *onceLock);
        // Lock the internal mutex using the specified 'onceLock' (possibly
        // blocking if another thread has already locked the mutex).  If no
        // other thread has yet called 'enter' or 'callOnce' on this object,
        // return 'true'.  Otherwise, unlock the mutex and return 'false'.  The
        // mutex lock may be skipped if it can be determined that it will not
        // be needed.  The behavior is undefined if 'onceLock' is already in a
        // locked state on entry to this method.  Note that if 'enter' returns
        // 'true', the caller *must* eventually call 'leave', or else other
        // threads may block indefinitely.

    void leave(OnceLock *onceLock);
        // Set this object into a state such that pending and future calls to
        // 'enter' or 'callOnce' will return 'false' or do nothing,
        // respectively, then unlock the internal mutex using the specified
        // 'onceLock' (possibly unblocking pending calls to 'enter' or
        // 'callOnce').  The behavior is undefined unless 'onceLock' was locked
        // by a matching call to 'enter' on this object and has not been
        // tampered-with since.

    void cancel(OnceLock *onceLock);
        // Revert this object to the state it was in before 'enter' or
        // 'callOnce' was called, then unlock the internal mutex using the
        // specified 'onceLock' (possibly unblocking pending calls to 'enter'
        // or 'callOnce').  This method may only be used to cancel execution of
        // one-time code that has not yet completed.  The behavior is undefined
        // unless 'onceLock' was locked by a matching call to 'enter' on this
        // object and has not been tampered-with since (especially by calling
        // 'leave').

    template <class FUNC>
    void callOnce(FUNC& function);
    template <class FUNC>
    void callOnce(const FUNC& function);
        // If no other thread has yet called 'enter' or 'callOnce', then call
        // the specified 'function' and set this object to the state where
        // pending and future calls to 'enter' or 'callOnce' will return
        // 'false' or do nothing, respectively.  Otherwise, wait for the
        // one-time code to complete and return without calling 'function'
        // where 'function' is a function or functor that can be called with no
        // arguments.  Note that one-time code is considered not to have run if
        // 'function' terminates with an exception.
};

                             // ===============
                             // class OnceGuard
                             // ===============

class OnceGuard {
    // Guard class for using 'Once' safely.  Construct an object of this class
    // before conditionally entering one-time processing code.  Destroy the
    // object when the one-time code is complete.  When used this way, this
    // object will be in an "in-progress" state during the time that the
    // one-time code is being executed.

    // PRIVATE TYPES
    enum State { e_NOT_ENTERED, e_IN_PROGRESS, e_DONE };

    // DATA
    Once::OnceLock  d_onceLock;
    Once           *d_once;
    State           d_state;

    // NOT IMPLEMENTED
    OnceGuard(const OnceGuard&);
    OnceGuard& operator=(const OnceGuard&);

  public:
    // CREATORS
    explicit OnceGuard(Once *once = 0);
        // Initialize this object to guard the (optionally) specified 'once'
        // object.  If 'once' is not specified, then it must be set later using
        // the 'setOnce' method before other methods may be called.

    ~OnceGuard();
        // Destroy this object.  If this object is not in an "in-progress"
        // state, do nothing.  If this object is in an "in-progress" state and
        // is being destroyed in the course of normal processing, then call
        // 'leave' on the associated 'Once' object.  Due to a bug in the MS
        // VC++ 2003 compiler, the behavior is undefined if this destructor is
        // called in the course of stack-unwinding during exception processing
        // (i.e., if an exception escapes from the one-time code region.
        // [Eventually, we hope to call 'cancel' if this destructor is called
        // during exception-processing.]

    // MANIPULATORS
    void setOnce(Once *once);
        // Set this object to guard the specified 'once' object.  The behavior
        // is undefined if this object is currently in the "in-progress" state.

    bool enter();
        // Call 'enter' on the associated 'Once' object and return the result.
        // If 'Once::enter' returns 'true', set this object into the
        // "in-progress" state.  The behavior is undefined unless this object
        // has been associated with a 'Once' object, either in the constructor
        // or using 'setOnce', or if this object is already in the
        // "in-progress" state.

    void leave();
        // If this object is in the "in-progress" state, call 'leave' on the
        // associated 'Once' object and exit the "in-progress" state.
        // Otherwise, do nothing.

    void cancel();
        // If this object is in the "in-progress" state, call 'cancel' on the
        // associated 'Once' object and exit the "in-progress" state.
        // Otherwise, do nothing.

    // ACCESSORS
    bool isInProgress() const;
        // Return 'true' if this object is in the "in-progress" state.  The
        // object is in-progress if 'enter' has been called and returned 'true'
        // and neither 'leave' nor 'cancel' have been called.  The one-time
        // code controlled by this object should only be executing if this
        // object is in the "in-progress" state.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                    // ----------------------------------
                    // Token concatenation support macros
                    // ----------------------------------

// Second layer needed to ensure that arguments are expanded before
// concatenation.
#define BSLMT_ONCE_CAT(X, Y) BSLMT_ONCE_CAT_IMP(X, Y)
#define BSLMT_ONCE_CAT_IMP(X, Y) X##Y

                  // -------------------------------------
                  // Implementation of BSLMT_ONCE_DO Macro
                  // -------------------------------------

// Use a for-loop to initialize the guard, test if we can enter the
// once-region, then leave the once-region at the end.  Each invocation of this
// macro within a source file supplies a different 'doOnceObj' name.
#define BSLMT_ONCE_DO_IMP(doOnceObj)                                          \
    static BloombergLP::bslmt::Once doOnceObj = BSLMT_ONCE_INITIALIZER;       \
    for (BloombergLP::bslmt::OnceGuard bslmt_doOnceGuard(&doOnceObj);         \
         bslmt_doOnceGuard.enter(); bslmt_doOnceGuard.leave())

                             // ---------------
                             // class OnceGuard
                             // ---------------

// CREATORS
inline
bslmt::OnceGuard::OnceGuard(Once *once)
: d_once(once)
, d_state(e_NOT_ENTERED)
{
}

// MANIPULATORS
inline
void bslmt::OnceGuard::setOnce(Once *once)
{
    BSLS_ASSERT_SAFE(e_IN_PROGRESS != d_state);

    d_once = once;
    d_state = e_NOT_ENTERED;
}

// ACCESSORS
inline
bool bslmt::OnceGuard::isInProgress() const
{
    return e_IN_PROGRESS == d_state;
}

                                // ----------
                                // class Once
                                // ----------

template <class FUNC>
inline
void bslmt::Once::callOnce(FUNC& function)
{
    OnceGuard guard(this);
    if (guard.enter()) {
#ifdef BDE_BUILD_TARGET_EXC
        try {
            function();
        }
        catch (...) {
            guard.cancel();
            throw;
        }
#else
        function();
#endif
    }
}

template <class FUNC>
inline
void bslmt::Once::callOnce(const FUNC& function)
{
    OnceGuard guard(this);
    if (guard.enter()) {
#ifdef BDE_BUILD_TARGET_EXC
        try {
            function();
        }
        catch (...) {
            guard.cancel();
            throw;
        }
#else
        function();
#endif
    }
}

}  // close enterprise namespace

#if !defined(BSL_DOUBLE_UNDERSCORE_XLAT) || 1 == BSL_DOUBLE_UNDERSCORE_XLAT
#define BSLMT_ONCE__CAT(X, Y) BSLMT_ONCE_CAT(X, Y)
#endif

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
