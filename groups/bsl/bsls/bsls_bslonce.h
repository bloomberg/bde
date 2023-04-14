// bsls_bslonce.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLS_BSLONCE
#define INCLUDED_BSLS_BSLONCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide BSL a thread-safe way to execute code once per process.
//
//@CLASSES:
//   bsls::BslOnce: statically initializable gate-keeper for a once-block
//   bsls::BslOnceGuard: guard for safely using 'bsls::BslOnce'
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a pair of classes, 'bsls::BslOnce'
// and 'bsls::BslOnceGuard', which give the caller a way to run a block of
// code exactly once within the current process, particularly in the presence
// of multiple threads.  The typical purpose of this one-time execution is the
// initialization of a singleton on first use.
//
// [!WARNING!] Clients outside of 'bsl' should *not* use this component.
// Because of its location in the hierarchy, this component guards critical
// sections using a spin-lock.  Equivalent components that are more robust and
// efficient will be provided at a higher level (see 'bslmt_once').
//
// A 'bsls::BslOnce' object can be statically initialized using the
// 'BSLS_BSLONCE_INITIALIZER' macro.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bsls::BslOnce' to Perform a Singleton Initialization
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates using 'bsls::BslOnce' to initialize a
// singleton object.
//
// First we declare a 'struct', 'MySingleton', whose definition is elided:
//..
//  struct MySingleton {
//
//    // PUBLIC DATA
//    int d_exampleData;
//
//    // ...
//  };
//..
// Notice that the data members are public because we want to avoid dynamic
// runtime initialize (i.e., initialization at run-time before the start of
// 'main') when an object of this type is declared in a static context.
//
// Now we implement a function 'getSingleton' that returns a singleton object.
// 'getSingleton' uses 'BslOnce' to ensure the singleton is initialized only
// once, and that the singleton is initialized before the function returns:
//..
//  MySingleton *getSingleton()
//      // Return a reference to a modifiable singleton object.
//  {
//     static MySingleton singleton = { 0 };
//     static BslOnce     once      = BSLS_BSLONCE_INITIALIZER;
//
//     BslOnceGuard onceGuard;
//     if (onceGuard.enter(&once)) {
//       // Initialize 'singleton'.  Note that this code is executed exactly
//       // once.
//
//     }
//     return &singleton;
//  }
//..
// Notice that 'BslOnce' must be initialized to 'BSLS_BSLONCE_INITIALIZER', and
// that 'singleton' is a function scoped static variable to avoid allocating
// it on the 'heap' (which might be reported as leaked memory).

#include <bsls_atomicoperations.h>

#ifdef BDE_BUILD_TARGET_SAFE
// This component needs to be below bsls_assert in the physical hierarchy, so
// 'BSLS_ASSERT' macros can't be used here.  To workaround this issue, we use
// the C 'assert' instead.

#include <assert.h>
#define BSLS_BSLONCE_ASSERT_SAFE(x) assert((x))

#else

#define BSLS_BSLONCE_ASSERT_SAFE(x)

#endif

namespace BloombergLP {
namespace bsls {

                        // =============
                        // class BslOnce
                        // =============

#define BSLS_BSLONCE_INITIALIZER { { 0xdead } }
    // Use this macro to initialize an object of type 'bsls::Once'.  E.g.:
    //..
    //  bsls::Once once = BSLS_BSLONCE_INITIALIZER;
    //..
    // Note that we use an unlikely arbitrary value to permit effectively
    // asserting a 'BslOnce' for correct initialization.


struct BslOnce {
     // This 'struct' provides a simple data type for ensuring a block of code
     // is executed (only) once.  Note that this is defined as a 'struct' to
     // allow constant initialization in a global or static context using
     // 'BSLS_BSLONCE_INITIALIZER'.

  public:
     // PUBLIC DATA
     bsls::AtomicOperations::AtomicTypes::Int d_onceState;
                           // The state of the one-time block of code managed
                           // by this object (must be one of the 'State'
                           // values).  This value is public to allow static
                           // initialization (with 'BSLS_BSLONCE_INITIALIZER'),
                           // but should never be directly accessed or
                           // modified.


  private:
     // PRIVATE TYPES
     enum State {
         // Note that we select unusual integer values in order to more
         // effectively test (in appropriate build modes) that 'd_onceState'
         // was correctly initialized.

         e_NOT_ENTERED = 0xdead,
         e_IN_PROGRESS,
         e_DONE
     };

    // PRIVATE MANIPULATORS
    bool doEnter();
        // Enter the one-time block of code.  Return 'true' if the one-time
        // block of code has been entered, and 'false' if the one-time block of
        // code has already been executed.  If this function returns 'false'
        // then the thread of execution in which 'enter' returned 'true' has
        // already called 'leave' -- i.e., the one-time block of code is
        // guaranteed to have *completed* execution.  The behavior is undefined
        // unless this object was originally initialized to
        // 'BSLS_BSLONCE_INITIALIZER'.  Note that this private variant of
        // 'enter' does not perform a test before attempting to acquire the
        // spin-lock, and is meant to be implemented out of line (so that the
        // expected path of 'enter' may be more easily inlined).

  public:
    // MANIPULATORS
    bool enter();
        // Enter the one-time block of code.  Return 'true' if the one-time
        // block of code has been entered, and 'false' if the one-time block of
        // code has already been executed.  If this function returns 'false'
        // then the thread of execution in which 'enter' returned 'true' has
        // already called 'leave' -- i.e., the one-time block of code is
        // guaranteed to have *completed* execution.  The behavior is undefined
        // unless this object was originally initialized to
        // 'BSLS_BSLONCE_INITIALIZER'.  Note that a successful 'enter' locks a
        // spin-lock; it is imperative that 'leave' be called quickly.

    void leave();
        // Exit the one-time block of code.  The behavior is undefined unless
        // the caller had previously called 'enter', and 'enter' had returned
        // 'true'.
};


                        // ==================
                        // class BslOnceGuard
                        // ==================

class BslOnceGuard {
    // This class provides a guard for managing a 'BslOnce' for the purpose of
    // executing a block of code (only) once.

  private:

     // DATA
    BslOnce *d_once;  // once gate-keeper

    // NOT IMPLEMENTED
    BslOnceGuard(const BslOnceGuard&);
    BslOnceGuard& operator=(const BslOnceGuard&);


  public:
    // CREATORS
    BslOnceGuard();
        // Create a guard to manage a block of code that is executed once.

    ~BslOnceGuard();
        // Destroy this guard, and if 'enter' had been called on this object
        // without a subsequent call to 'leave', then call 'leave' to signal
        // the completion of the one-time block of code.

    // MANIPULATORS
    bool enter(BslOnce *once);
        // Enter the one-time block of code that is managed by the specified
        // 'once'.  Return 'true' if the one-time block of code has been
        // entered, and 'false' if the one-time block of code has already been
        // executed.  If this function returns 'false' then the thread of
        // execution in which 'enter' returned 'true' has already called
        // 'leave' -- i.e., the one-time block of code is guaranteed to have
        // *completed* execution.  The behavior is undefined unless 'once' was
        // originally initialized to 'BSLS_BSLONCE_INITIALIZER'.  Note that a
        // successful 'enter' locks a spin-lock; it is imperative that 'leave'
        // be called quickly.

    void leave();
        // Exit the one-time block of code.  The behavior is undefined unless
        // the caller had previously called 'enter', and 'enter' had returned
        // 'true'.
};


// ============================================================================
//                         INLINE DEFINITIONS
// ============================================================================

                        // -------------
                        // class BslOnce
                        // -------------


// MANIPULATORS
inline
bool BslOnce::enter()
{
    if (e_DONE == bsls::AtomicOperations::getIntAcquire(&d_onceState)) {
        return false;                                                 // RETURN
    }

    return doEnter();
};

inline
void BslOnce::leave()
{
    BSLS_BSLONCE_ASSERT_SAFE(e_IN_PROGRESS ==
                          bsls::AtomicOperations::getIntRelaxed(&d_onceState));
    bsls::AtomicOperations::setIntRelease(&d_onceState, e_DONE);
}


                        // ==================
                        // class BslOnceGuard
                        // ==================


// CREATORS
inline
BslOnceGuard::BslOnceGuard()
: d_once(0)
{
}

inline
BslOnceGuard::~BslOnceGuard()
{
    if (d_once) {
        d_once->leave();
    }
}

// MANIPULATORS
inline
bool BslOnceGuard::enter(BslOnce *once)
{
    BSLS_BSLONCE_ASSERT_SAFE(once);
    BSLS_BSLONCE_ASSERT_SAFE(!d_once);

    bool success = once->enter();

    // If the block guarded by 'once' has successfully been entered, set
    // 'd_once' so that 'leave' will be called when this guard is destroyed.

    if (success) {
        d_once = once;
    }
    return success;
}

inline
void BslOnceGuard::leave()
{
    BSLS_BSLONCE_ASSERT_SAFE(d_once);

    d_once->leave();
    d_once = 0;

}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
