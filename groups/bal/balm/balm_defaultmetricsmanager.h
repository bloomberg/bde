// balm_defaultmetricsmanager.h                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_DEFAULTMETRICSMANAGER
#define INCLUDED_BALM_DEFAULTMETRICSMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide for a default instance of the metrics manager.
//
//@CLASSES:
//            balm::DefaultMetricsManager: namespace for the default instance
// balm::DefaultMetricsManagerScopedGuard: guard for the default instance
//
//@SEE_ALSO: balm_metricsmanager, balm_metric
//
//@DESCRIPTION: This component provides a namespace for a default instance of
// the 'balm::MetricsManager'.  This 'balm::DefaultMetricsManager' provides
// static operations to create, access, and destroy the default instance of the
// 'balm::MetricsManager'.  The 'balm::DefaultMetricsManagedScopedGuard'
// provides a proctor that creates a default metrics manager on construction
// and destroys it on destruction.
//
// 'balm::DefaultMetricsManagerScopedGuard' is also here.
//
///Thread Safety
///-------------
// The default 'balm::MetricsManager' instance, once initialized, can be safely
// accessed from multiple threads.  However, the 'create' and 'destroy'
// operations supplied by the 'balm::DefaultMetricsManager' are *not*
// *thread-safe*.  Care must be taken, particularly when releasing the
// instance.  The expected usage is that the instance will be created during
// the initialization of an application (while the task has a single thread)
// and that it will be destroyed just prior to termination (when there is
// similarly just a single thread).
//
///Usage
///-----
// The following examples demonstrate how to create, configure, and destroy
// the default 'balm::MetricsManager' instance.
//
///Example 1: Create and Access the Default 'balm::MetricsManager' Instance
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'balm::MetricManager'
// instance and publish a single metric to the console.  See the documentation
// of 'balm_metric' and 'balm_metricsmanager' for information on how to record
// metrics.
//
// First we create a 'balm::DefaultMetricsManagerScopedGuard', which manages
// the lifetime of the default metrics manager instance.  At construction, we
// provide the 'balm::DefaultMetricsManagerScopedGuard' an output stream
// ('stdout') to which it will publish metrics.  Note that the default metrics
// manager is intended to be created and destroyed by the *owner* of 'main'.
// The instance should be created during the initialization of an application
// (while the task has a single thread) and destroyed just prior to termination
// (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//
//      // ...
//
//      balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// static 'instance' method.
//..
//     balm::MetricsManager *manager = balm::DefaultMetricsManager::instance();
//     assert(0 != manager);
//..
// The default metrics manager, by default, is configured with a
// 'balm::StreamPublisher' object that will publish all recorded metrics to the
// consoled.  We use the default 'manager' instance to update the collector
// for a single metric, and then publish all metrics.
//..
//      balm::Collector *myMetric =
//                      manager->collectorRepository().getDefaultCollector(
//                                                   "MyCategory", "MyMetric");
//      myMetric->update(10);
//      manager->publishAll();
//
//      // ... rest of program elided ...
//  }
//..
// The output of this example would look similar to:
//..
// 05FEB2009_19:20:12.697+0000 1 Records
//    Elapsed Time: 0.009311s
//            MyCategory.MyMetric [ count = 1, total = 10, min = 10, max = 10 ]
//..
// Note that the default metrics manager will be destroyed when 'managerGuard'
// exits this scope and is destroyed.  Clients that choose to explicitly call
// 'balm::DefaultMetricsManager::create()' must also explicitly call
// 'balm::DefaultMetricsManager::destroy()'.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {


namespace balm {

class MetricsManager;

                        // ============================
                        // struct DefaultMetricsManager
                        // ============================

struct DefaultMetricsManager {
    // This struct provides a namespace for static functions that create,
    // access, and destroy the default instance of the 'MetricsManager'.  The
    // expected usage is that the default instance will be created during the
    // initialization of an application (while the task has a single thread)
    // and that it will be destroyed just prior to termination (when there is
    // similarly a single thread).

  private:
    // CLASS DATA
    static MetricsManager      *s_singleton_p; // metrics manager default
                                               // instance

    static bslma::Allocator    *s_allocator_p; // allocator used to initialize
                                               // the singleton
  public:
    // CLASS METHODS
    static MetricsManager *manager(MetricsManager *manager = 0);
        // If the optionally specified 'manager' is not 0, return 'manager';
        // otherwise return the address of the default metrics manager
        // instance, or 0 if the default metrics manager instance has not yet
        // been created or has already been destroyed.  Note that this
        // operation is logically equivalent to
        // 'manager ? manager : instance()'.

    static MetricsManager *create(bslma::Allocator *basicAllocator = 0);
        // Create the default 'MetricsManager' instance and return the address
        // of the modifiable created instance.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed global allocator is used.  The behavior is
        // undefined unless '0 == MetricsManager::instance()' prior to calling
        // this method, or if this method is called from one thread while
        // another thread is attempting to access the default metrics manager
        // instance (i.e., this method is *not* thread-safe).  Note that the
        // returned default metrics manager instance is not configured with a
        // publisher; clients must create a 'Publisher' and add it to the
        // default metrics manager in order to publish metrics.

    static MetricsManager *create(bsl::ostream&     stream,
                                  bslma::Allocator *basicAllocator = 0);
        // Create the default 'MetricsManager' instance and configure it with
        // a 'StreamPublisher' that will publish recorded metrics to the
        // specified 'stream', then return the address of the modifiable
        // created metrics manager instance.  Optionally specify
        // 'basicAllocator' to use to obtain memory.  If 'basicAllocator' is 0,
        // the currently installed global allocator is used.  The behavior is
        // undefined unless '0 == MetricsManager::instance()' prior to calling
        // this method, or if this method is called from one thread while
        // another thread is attempting to access the default metrics manager
        // instance (i.e., this method is *not* thread-safe).

    static MetricsManager *instance();
        // Return the default instance of the 'MetricsManager' or 0 if the
        // default instance has not yet been created or has already been
        // destroyed.

    static void destroy();
        // Destroy the default instance of 'MetricsManager'.  After this
        // method returns, 'instance()' will return 0.  The behavior is
        // undefined if 'instance()' is 0 or if this method is called from one
        // thread while another thread is accessing the default metrics
        // manager instance (i.e., this method is *not* thread-safe).
};

                   // ======================================
                   // class DefaultMetricsManagerScopedGuard
                   // ======================================

class DefaultMetricsManagerScopedGuard {
    // This class implements a scoped guard that, on construction, creates the
    // default instance of the metrics manager, and, on destruction, destroys
    // that instance.  Note that the behavior is undefined if the default
    // instance of the metrics manager is created before creating this guard,
    // or if the default instance is externally destroyed before destroying
    // this guard.

    // NOT IMPLEMENTED
    DefaultMetricsManagerScopedGuard(const DefaultMetricsManagerScopedGuard&);
    DefaultMetricsManagerScopedGuard& operator=(
                                      const DefaultMetricsManagerScopedGuard&);

  public:
    // CREATORS
    DefaultMetricsManagerScopedGuard(bsl::ostream&     stream,
                                     bslma::Allocator *basicAllocator = 0);
        // Create a scoped guard which invokes
        // 'DefaultMetricsManager::create()' to create a default metrics
        // manager instance that is configured with a stream publisher that
        // will publish collected metrics to the specified 'stream'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed global allocator is
        // used.  The behavior is undefined unless
        // 'DefaultMetricsManager::instance()' is 0 prior to creating the
        // guard.

    DefaultMetricsManagerScopedGuard(bslma::Allocator *basicAllocator = 0);
        // Create a scoped guard which invokes the
        // 'DefaultMetricsManager::create' method.  Optionally specify a
        // 'basicAllocator' used to obtain memory.  If 'basicAllocator' is 0,
        // the currently installed global allocator is used.  The behavior is
        // undefined unless '0 == DefaultMetricsManager::instance()' prior to
        // creating the guard.  Note that the default metrics manager instance
        // is not configured with a publisher; clients must create a
        // 'Publisher' object and add it to the default metrics manager in
        // order to publish metrics.

    ~DefaultMetricsManagerScopedGuard();
        // Destroy this scoped guard which invokes
        // 'DefaultMetricsManager::destroy()'.  The behavior is undefined if
        // the default instance of the metrics manager is externally destroyed
        // prior to this destructor being invoked.

    // ACCESSORS
    MetricsManager *instance() const;
        // Return the address of the 'MetricsManager' object managed by this
        // scoped guard.  The behavior is undefined if the default instance of
        // the metrics manager is externally destroyed, or if the returned
        // address is retained after this scoped guard is destroyed.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ---------------------------
                        // class DefaultMetricsManager
                        // ---------------------------

// CLASS METHODS
inline
MetricsManager *DefaultMetricsManager::instance()
{
    return s_singleton_p;
}

inline
MetricsManager *DefaultMetricsManager::manager(
                                                  MetricsManager *manager)
{
    return manager ? manager : s_singleton_p;
}

                   // --------------------------------------
                   // class DefaultMetricsManagerScopedGuard
                   // --------------------------------------

// CREATORS
inline
DefaultMetricsManagerScopedGuard::DefaultMetricsManagerScopedGuard(
                                              bsl::ostream&     stream,
                                              bslma::Allocator *basicAllocator)
{
    DefaultMetricsManager::create(stream, basicAllocator);
}

inline
DefaultMetricsManagerScopedGuard::DefaultMetricsManagerScopedGuard(
                                              bslma::Allocator *basicAllocator)
{
    DefaultMetricsManager::create(basicAllocator);
}

inline
DefaultMetricsManagerScopedGuard::~DefaultMetricsManagerScopedGuard()
{
    DefaultMetricsManager::destroy();
}

// ACCESSORS
inline
MetricsManager *DefaultMetricsManagerScopedGuard::instance() const
{
    return DefaultMetricsManager::instance();
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
