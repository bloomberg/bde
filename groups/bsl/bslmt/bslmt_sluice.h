// bslmt_sluice.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMT_SLUICE
#define INCLUDED_BSLMT_SLUICE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a "sluice" class.
//
//@CLASSES:
//  bslmt::Sluice: thread-aware sluice class
//
//@SEE_ALSO: bslmt_conditionimpl_win32
//
//@DESCRIPTION: This component provides a "sluice" class, `bslmt::Sluice`.  A
// sluice is useful for controlling the release of threads from a common
// synchronization point.  One or more threads may "enter" a `bslmt::Sluice`
// object (via the `enter` method), and then wait to be released (via either
// the `wait` or `timedWait` method).  Either one waiting thread (via the
// `signalOne` method), or all waiting threads (via the `signalAll` method),
// may be signaled for release.  In either case, `bslmt::Sluice` provides a
// guarantee against starvation; newly-entering threads will not indefinitely
// prevent threads that previously entered from being signaled.
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
///Example 1: Basic Usage
/// - - - - - - - - - - -
// `bslmt::Sluice` is intended to be used to implement other synchronization
// mechanisms.  In particular, the functionality provided by `bslmt::Sluice` is
// useful for implementing a condition variable:
// ```
// class MyCondition {
//     // This class implements a condition variable based on 'bslmt::Sluice'.
//
//     // DATA
//     bslmt::Sluice d_waitSluice;  // sluice object
//
//   public:
//     // MANIPULATORS
//     void wait(bslmt::Mutex *mutex)
//     {
//         const void *token = d_waitSluice.enter();
//         mutex->unlock();
//         d_waitSluice.wait(token);
//         mutex->lock();
//     }
//
//     void signal()
//     {
//         d_waitSluice.signalOne();
//     }
//
//     void broadcast()
//     {
//         d_waitSluice.signalAll();
//     }
// };
// ```

#include <bslscm_version.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_timedsemaphore.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_systemclocktype.h>

#include <bslma_allocator.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <bslmt_chronoutil.h>

#include <bsl_chrono.h>
#endif

namespace BloombergLP {
namespace bslmt {

                               // ============
                               // class Sluice
                               // ============

/// This class controls the release of threads from a common synchronization
/// point.  One or more threads may "enter" a `Sluice` object, and then wait
/// to be released.  Either one waiting thread (via the `signalOne` method),
/// or all waiting threads (via the `signalAll` method), may be signaled for
/// release.  In any case, `Sluice` provides a guarantee against starvation.
class Sluice {

  private:
    // PRIVATE TYPES

    /// This object represents one "generation" in a sluice.  A generation
    /// begins when a thread enters the sluice, and ends (ceases accepting
    /// new entering threads) when `signalOne` or `signalAll` is invoked.
    /// The last thread in the generation to invoke `wait` is responsible
    /// for returning the descriptor to the pool.
    struct GenerationDescriptor {

        // DATA
        int                   d_numThreads;   // number of threads entered, but
                                              // not yet finished waiting

        int                   d_numSignaled;  // number of threads signaled,
                                              // but not yet finished waiting

        TimedSemaphore        d_sema;         // semaphore on which to wait

        GenerationDescriptor *d_next;         // pointer to the next free
                                              // descriptor in the pool

        // CREATORS

        /// Create a generation descriptor object with the specified
        /// `clockType`.
        explicit GenerationDescriptor(bsls::SystemClockType::Enum clockType);
    };

    // DATA
    Mutex                 d_mutex;               // for synchronizing access to
                                                 // data members

    GenerationDescriptor *d_signaledGeneration;  // generation in which at
                                                 // least one, but not all,
                                                 // threads have been signaled

    GenerationDescriptor *d_pendingGeneration;   // generation in which no
                                                 // threads have been signaled
                                                 // yet

    GenerationDescriptor *d_descriptorPool;      // pool of available
                                                 // generation descriptors

    bsls::SystemClockType::Enum
                          d_clockType;           // the type of clock used for
                                                 // 'absTime' in 'timedWait'

    bslma::Allocator     *d_allocator_p;         // memory allocator (held, not
                                                 // owned)

  private:
    // NOT IMPLEMENTED
    Sluice(const Sluice&);
    Sluice& operator=(const Sluice&);

  public:
    // TYPES

    /// The value `timedWait` returns when a timeout occurs.
    enum { e_TIMED_OUT = TimedSemaphore::e_TIMED_OUT };

    // CREATORS

    /// Create a sluice.  Optionally specify a `clockType` indicating the
    /// type of the system clock against which the `bsls::TimeInterval`
    /// `absTime` timeouts passed to the `timedWait` method are to be
    /// interpreted (see {Supported Clock-Types} in the component
    /// documentation).  If `clockType` is not specified then the realtime
    /// system clock is used.  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit
    Sluice(bslma::Allocator *basicAllocator = 0);
    explicit
    Sluice(bsls::SystemClockType::Enum  clockType,
           bslma::Allocator            *basicAllocator = 0);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// Create a sluice.  Use the realtime system clock as the clock against
    /// which the `absTime` timeouts passed to the `timedWait` methods are
    /// interpreted (see {Supported Clock-Types} in the component-level
    /// documentation).  Optionally specify a `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit
    Sluice(const bsl::chrono::system_clock&,
           bslma::Allocator                 *basicAllocator = 0);

    /// Create a sluice.  Use the monotonic system clock as the clock
    /// against which the `absTime` timeouts passed to the `timedWait`
    /// methods are interpreted (see {Supported Clock-Types} in the
    /// component-level documentation).  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    explicit
    Sluice(const bsl::chrono::steady_clock&,
           bslma::Allocator                 *basicAllocator = 0);
#endif

    /// Destroy this sluice.
    ~Sluice();

    // MANIPULATORS

    /// Enter this sluice, and return the token on which the calling thread
    /// must subsequently wait.  The behavior is undefined unless `wait` or
    /// `timedWait` is invoked with the token before this sluice is
    /// destroyed.
    const void *enter();

    /// Signal all threads that have entered this sluice and have not yet
    /// been released.
    void signalAll();

    /// Signal one thread that has entered this sluice and has not yet been
    /// released.
    void signalOne();

    /// Wait for the specified `token` to be signaled, or until the
    /// specified `absTime` timeout expires.  `absTime` is an *absolute*
    /// time represented as an interval from some epoch, which is determined
    /// by the clock indicated at construction (see {Supported Clock-Types}
    /// in the component-level documentation).  Return 0 on success, and
    /// `e_TIMED_OUT` on timeout.  Any other value indicates that an error
    /// has occurred.  Errors are unrecoverable.  After an error, the sluice
    /// may be destroyed, but any other use has undefined behavior.  The
    /// `token` is released whether or not a timeout occurred.  The behavior
    /// is undefined unless `token` was obtained from a call to `enter` by
    /// this thread, and was not subsequently released (via a call to
    /// `timedWait` or `wait`).
    int timedWait(const void *token, const bsls::TimeInterval& absTime);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// Wait for the specified `token` to be signaled, or until the
    /// specified `absTime` timeout expires.  `absTime` is an *absolute*
    /// time represented as an interval from some epoch, which is determined
    /// by the clock associated with the time point.  Return 0 on success,
    /// and `e_TIMED_OUT` on timeout.  Any other value indicates that an
    /// error has occurred.  Errors are unrecoverable.  After an error, the
    /// sluice may be destroyed, but any other use has undefined behavior.
    /// The `token` is released whether or not a timeout occurred.  The
    /// behavior is undefined unless `token` was obtained from a call to
    /// `enter` by this thread, and was not subsequently released (via a
    /// call to `timedWait` or `wait`).
    template <class CLOCK, class DURATION>
    int timedWait(const void                                      *token,
                  const bsl::chrono::time_point<CLOCK, DURATION>&  absTime);
#endif

    /// Wait for the specified `token` to be signaled, and release the
    /// `token`.  The behavior is undefined unless `token` was obtained from
    /// a call to `enter` by this thread, and was not subsequently released
    /// (via a call to `timedWait` or `wait`).
    void wait(const void *token);

    // ACCESSORS

    /// Return the clock type used for timeouts.
    bsls::SystemClockType::Enum clockType() const;
};

}  // close package namespace

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                             // ------------
                             // class Sluice
                             // ------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// MANIPULATORS
template <class CLOCK, class DURATION>
int bslmt::Sluice::timedWait(
                      const void                                      *token,
                      const bsl::chrono::time_point<CLOCK, DURATION>&  absTime)
{

    GenerationDescriptor *g =
                static_cast<GenerationDescriptor *>(const_cast<void *>(token));

    for (;;) {
        int rc = g->d_sema.timedWait(absTime);

        LockGuard<Mutex> lock(&d_mutex);

        if (g->d_numSignaled) {
            BSLS_ASSERT(d_pendingGeneration != g);

            --g->d_numSignaled;

            rc = 0;
        }
        else if (0 == rc) {
            continue;
        }

        const int numThreads = --g->d_numThreads;

        if (0 == numThreads) {
            // The last thread is responsible for cleanup.

            if (d_signaledGeneration == g) {
                BSLS_ASSERT(0 != rc);
                d_signaledGeneration = 0;
            }

            if (d_pendingGeneration == g) {
                BSLS_ASSERT(0 != rc);
                d_pendingGeneration = 0;
            }

            g->d_next = d_descriptorPool;
            d_descriptorPool = g;
        }
        return rc;                                                    // RETURN
    }
}
#endif

// ACCESSORS
inline
bsls::SystemClockType::Enum bslmt::Sluice::clockType() const
{
    return d_clockType;
}

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
