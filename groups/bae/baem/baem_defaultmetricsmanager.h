// baem_defaultmetricsmanager.h    -*-C++-*-
#ifndef INCLUDED_BAEM_DEFAULTMETRICSMANAGER
#define INCLUDED_BAEM_DEFAULTMETRICSMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide for a default instance of the metrics manager.
//
//@CLASSES:
//            baem_DefaultMetricsManager: namespace for the default instance
// baem_DefaultMetricsManagerScopedGuard: guard for the default instance
//
//@SEE_ALSO: baem_metricsmanager, baem_metric
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a namespace for a default instance
// of the 'baem_MetricsManager'.  This 'baem_DefaultMetricsManager'
// provides static operations to create, access, and destroy the default
// instance of the 'baem_MetricsManager'.  The
// 'baem_DefaultMetricsManagedScopedGuard' provides a proctor that creates a
// default metrics manager on construction and destroys it on destruction.
//
///Thread Safety
///-------------
// The default 'baem_MetricsManager' instance, once initialized, can be safely
// accessed from multiple threads.  However, the 'create' and 'destroy'
// operations supplied by the 'baem_DefaultMetricsManager' are *not*
// *thread-safe*.  Care must be taken, particularly when releasing the
// instance.  The expected usage is that the instance will be created
// during the initialization of an application (while the task has a
// single thread) and that it will be destroyed just prior to termination
// (when there is similarly just a single thread).
//
///Usage
///-----
// The following examples demonstrate how to create, configure, and destroy
// the default 'baem_MetricsManager' instance.
//
///Example 1 - Create and Access the Default 'baem_MetricsManager' Instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'baem_MetricManager'
// instance and publish a single metric to the console.  See the
// documentation of 'baem_metric' and 'baem_metricsmanager' for information on
// how to record metrics.
//
// First we create a 'baem_DefaultMetricsManagerScopedGuard', which manages the
// lifetime of the default metrics manager instance.  At construction, we
// provide the 'baem_DefaultMetricsManagerScopedGuard' an output stream
// ('stdout') to which it will publish metrics.  Note that the default
// metrics manager is intended to be created and destroyed by the *owner* of
// 'main'.  The instance should be created during the initialization of an
// application (while the task has a single thread) and destroyed just prior to
// termination (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//
//      // ...
//
//      baem_DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// static 'instance' method.
//..
//      baem_MetricsManager *manager  = baem_DefaultMetricsManager::instance();
//                    assert(0 != manager);
//..
// We use the default 'manager' instance to update the collector for a single
// metric, and then publish all the collected metrics.
//..
//      baem_Collector *myMetric =
//            manager->collectorRepository().getDefaultCollector("MyCategory",
//                                                               "MyMetric");
//      myMetric->update(10);
//      manager->publishAll();
//..
// The output of this example would look similar to:
//..
// 05FEB2009_19:20:12.697+0000 1 Records
//    Elapsed Time: 0.009311s
//            MyCategory.MyMetric [ count = 1, total = 10, min = 10, max = 10 ]
//..
// Note that the default metrics manager will be destroyed when 'managerGuard'
// exits this scope and is destroyed.  Clients that choose to explicitly call
// the 'baem_DefaultMetricsManager::create' method must also explicitly call
// 'baem_DefaultMetricsManager::destroy()'.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class baem_MetricsManager;
class bslma_Allocator;

                   // =================================
                   // struct baem_DefaultMetricsManager
                   // =================================

struct baem_DefaultMetricsManager {
    // This struct provides a namespace for static functions that create,
    // access, and destroy the default instance of the 'baem_MetricsManager'.
    // The expected usage is that the default instance will be created
    // during the initialization of an application (while the task has a
    // single thread) and that it will be destroyed just prior to termination
    // (when there is similarly a single thread).

  private:
    // CLASS DATA
    static baem_MetricsManager *s_singleton_p; // metrics manager default
                                               // instance

    static bslma_Allocator     *s_allocator_p; // allocator used to initialize
                                               // the singleton
  public:
    // CLASS METHODS
    static baem_MetricsManager *manager(baem_MetricsManager *manager = 0);
        // If the specified 'manager' is not 0, return 'manager'; otherwise
        // return the address of the default metrics manager instance, or 0 if
        // the default metrics manager instance has not yet been created or
        // has already been destroyed.  Note that this operation is logically
        // equivalent to 'manager ? manager : instance()'.

    static baem_MetricsManager *create(bslma_Allocator *basicAllocator = 0);
        // Create the default 'baem_MetricsManager' instance and return the
        // address of the modifiable created instance.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed global allocator is used.  The
        // behavior is undefined unless '0 == baem_MetricsManager::instance()'
        // prior to calling this method, or if this method is called from one
        // thread while another thread is attempting to access the default
        // metrics manager instance (i.e., this method is *not* thread-safe).
        // Note that the returned default metrics manager instance is not
        // configured with a publisher; clients must create a 'baem_Publisher'
        // and add it to the default metrics manager in order to publish
        // metrics.

    static baem_MetricsManager *create(bsl::ostream&    stream,
                                       bslma_Allocator *basicAllocator = 0);
        // Create the default 'baem_MetricsManager' instance and configure it
        // with a 'baem_StreamPublisher' that will publish recorded metrics to
        // the specified 'stream', then return the address of the modifiable
        // created metrics manager instance.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed global allocator is used.  The
        // behavior is undefined unless '0 == baem_MetricsManager::instance()'
        // prior to calling this method, or if this method is called from one
        // thread while another thread is attempting to access the default
        // metrics manager instance (i.e., this method is *not* thread-safe).

    static baem_MetricsManager *instance();
        // Return the default instance of the 'baem_MetricsManager' or
        //  0 if the default instance has not yet been created or has
        //  already been destroyed.

    static void destroy();
        // Destroy the default instance of 'baem_MetricsManager'.  After this
        // method returns, 'instance()' will return 0.  The behavior is
        // undefined if 'instance()' is 0 or if this method is called from one
        // thread while another thread is accessing the default metrics
        // manager instance (i.e., this method is *not* thread-safe).
};

               // ===========================================
               // class baem_DefaultMetricsManagerScopedGuard
               // ===========================================

class baem_DefaultMetricsManagerScopedGuard {
    // This class implements a scoped guard that, on construction, creates the
    // default instance of the metrics manager, and, on destruction, destroys
    // that instance.  Note that the behavior is undefined if the default
    // instance of the metrics manager is created before creating this guard,
    // or if the default instance is externally destroyed before destroying
    // this guard.

    // NOT IMPLEMENTED
    baem_DefaultMetricsManagerScopedGuard(
                                const baem_DefaultMetricsManagerScopedGuard& );
    baem_DefaultMetricsManagerScopedGuard& operator=(
                                const baem_DefaultMetricsManagerScopedGuard& );

  public:
    // CREATORS
    baem_DefaultMetricsManagerScopedGuard(bsl::ostream&    stream,
                                          bslma_Allocator *basicAllocator = 0);
        // Create a scoped guard which invokes
        // 'baem_DefaultMetricsManager::create()' to create a default metrics
        // manager instance that is configured with a stream publisher that
        // will publish collected metrics to the specified 'stream'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed global allocator
        // is used.  The behavior is undefined unless
        // 'baem_DefaultMetricsManager::instance()' is 0 prior to creating
        // the guard.

    baem_DefaultMetricsManagerScopedGuard(bslma_Allocator *basicAllocator = 0);
        // Create a scoped guard which invokes the
        // 'baem_DefaultMetricsManager::create' method.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator'
        // is 0, the currently installed global allocator is used.
        // The behavior is undefined unless
        // '0 == baem_DefaultMetricsManager::instance()' prior to creating
        // the guard.  Note that the default metrics manager instance is not
        // configured with a publisher; clients must create a 'baem_Publisher'
        // object and add it to the default metrics manager in order to publish
        // metrics.

    ~baem_DefaultMetricsManagerScopedGuard();
        // Destroy this scoped guard which invokes
        // 'baem_DefaultMetricsManager::destroy()'.  The behavior is undefined
        // if the default instance of the metrics manager is externally
        // destroyed prior to this destructor being invoked.

    // ACCESSORS
    baem_MetricsManager *instance() const;
        // Return the address of the 'baem_MetricsManager' object managed by
        // this scoped guard.  The behavior is undefined if the default
        // instance of the metrics manager is externally destroyed, or if the
        // returned address is retained after this scoped guard is destroyed.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // --------------------------------
                   // class baem_DefaultMetricsManager
                   // --------------------------------

// CLASS METHODS
inline
baem_MetricsManager *baem_DefaultMetricsManager::instance()
{
    return s_singleton_p;
}

inline
baem_MetricsManager *baem_DefaultMetricsManager::manager(
                                                  baem_MetricsManager *manager)
{
    return manager ? manager : s_singleton_p;
}

               // -------------------------------------------
               // class baem_DefaultMetricsManagerScopedGuard
               // -------------------------------------------

// CREATORS
inline
baem_DefaultMetricsManagerScopedGuard::baem_DefaultMetricsManagerScopedGuard(
                                               bsl::ostream&    stream,
                                               bslma_Allocator *basicAllocator)
{
    baem_DefaultMetricsManager::create(stream, basicAllocator);
}

inline
baem_DefaultMetricsManagerScopedGuard::baem_DefaultMetricsManagerScopedGuard(
                                               bslma_Allocator *basicAllocator)
{
    baem_DefaultMetricsManager::create(basicAllocator);
}

inline
baem_DefaultMetricsManagerScopedGuard::~baem_DefaultMetricsManagerScopedGuard()
{
    baem_DefaultMetricsManager::destroy();
}

// ACCESSORS
inline
baem_MetricsManager *baem_DefaultMetricsManagerScopedGuard::instance() const
{
    return baem_DefaultMetricsManager::instance();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
