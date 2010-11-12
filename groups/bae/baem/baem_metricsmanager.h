// baem_metricsmanager.h              -*-C++-*-
#ifndef INCLUDED_BAEM_METRICSMANAGER
#define INCLUDED_BAEM_METRICSMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a manager for recording and publishing metric data.
//
//@CLASSES:
//  baem_MetricsManager: manager for recording and publishing metric data
//
//@SEE_ALSO: baem_publisher, baem_collectorrepository, baem_metricregistry,
//           baem_metric, baem_defaultmetricsmanager, baem_publicationscheduler
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a metrics manager class for managing
// the recording and publishing of metric data.  The metrics manager retrieves
// 'baem_MetricRecords' from both the collector repository it owns as well as
// any 'RecordsCollectionCallbacks' registered with it.  The metrics manager
// also provides methods to register 'baem_Publisher' objects.  The 'publish'
// method collects metrics for a category (or set of categories) and then
// sends the collected metrics to the publishers associated with that
// category (or set of categories).
//
// Note that a metric in this context is an event associated with a measured
// value.  This component does *not* define what constitutes an event or what
// the associated value represents.  A collected metric contains the count of
// event occurrences along with the total, minimum, and maximum aggregates of
// the measured values.
//
///Thread Safety
///-------------
// 'baem_MetricsManager' is fully *thread-safe*, meaning that all non-creator
// operations on a given instance can be safely invoked simultaneously from
// multiple threads.
//
///Usage
///-----
// The following examples demonstrate how to configure, collect, and publish
// metrics.
//
///Example 1 - Initialize a 'baem_MetricsManager'
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and configure a
// 'baem_MetricsManager' that we will use to record and publish metric values.
// We first create a 'baem_MetricsManager' object and a 'SimpleStreamPublisher'
// object.  Note that 'SimpleStreamPublisher' is an example implementation of
// the 'baem_Publisher' protocol defined in the 'baem_publisher' component.
// In practice, clients typically use a standard publisher class (e.g.,
// 'baem_StreamPublisher').
//..
//  int main(int argc, char *argv[]) {
//  {
//
//      // ...
//
//      bslma_Allocator    *allocator = bslma_Default::allocator(0);
//      baem_MetricsManager manager(allocator);
//
//      bcema_SharedPtr<baem_Publisher> publisher(
//                          new (*allocator) SimpleStreamPublisher(bsl::cout),
//                          allocator);
//      manager.addGeneralPublisher(publisher);
//
//      // ...
//
//..
///Example 2 - Recording Metric Values with 'baem_Collector'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This second example demonstrates using 'baem_Collector' objects (obtained
// from a metrics manager's collector repository) to collect metrics related to
// a hypothetical 'EventHandler' class.  On construction, the event handler
// obtains references to 'baem_Collector' objects from the metrics
// manager's collector repository.  On each handled event, the 'EventHandler',
// updates its collectors with the appropriate metric values.
//
// Note that the 'baem_metric' component provides both classes and macros to
// reduce the code required for collecting metric values.
//..
//  class EventHandler {
//      // Provide an example event-handling mechanism that records metrics
//      // for (1) the size of the processed event messages and (2) the number
//      // of failures, using 'baem_Collector' objects provided by a
//      // 'baem_MetricsManager'.
//
//      // DATA
//      baem_Collector *d_eventMessageSizes_p;  // collect the message sizes
//
//      baem_Collector *d_eventFailures_p;      // collect the number of
//                                              // failures
//
//  // ...
//
//    public:
//      // CREATORS
//..
// We obtain the addresses of the respective 'baem_Collector' objects that we
// will use to collect metrics values from the metrics managers' collector
// repository.  Note that, in practice, clients can use the
// 'baem_DefaultMetricManager' (see 'baem_defaultmetricsmanager' and
// 'baem_metric') rather than explicitly pass the address of a metrics manager.
//..
//      EventHandler(baem_MetricsManager *manager)
//      : d_eventMessageSizes_p(
//             manager->collectorRepository().getDefaultCollector(
//                                              "MyCategory", "messageSizes"))
//      , d_eventFailures_p(
//             manager->collectorRepository().getDefaultCollector(
//                                              "MyCategory", "eventFailures"))
//      {}
//
//      // MANIPULATORS
//..
// Then, when processing an "event", we update the 'baem_Collector' objects
// with the appropriate metric values for the event.
//..
//      int handleEvent(int eventId, const bsl::string& eventMessage)
//          // Process the event described by the specified 'eventId' and
//          // 'eventMessage' .  Return 0 on success, and a non-zero value
//          // if there was an error handling the event.
//      {
//         int returnCode = 0;
//         d_eventMessageSizes_p->update(eventMessage.size());
//
//  // ...    (Process the event)
//
//         if (0 != returnCode) {
//             d_eventFailures_p->update(1);
//         }
//         return returnCode;
//      }
//
//  // ...
//
//  };
//..
///Example 3 - Recording Metric Values with a Callback
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// The metrics manager provides a facility to register a callback to report
// metric values.  A callback should be used if clients want to customize how
// a metric, or group of metrics, are recorded.  In the following example, the
// 'EventHandlerWithCallback' class maintains a metric for the average number
// of events per second that it reports through a
// 'baem_MetricsManager::RecordsCollectionCallback'.
//..
//  // eventhandlerwithcallback.h
//
//  class EventHandlerWithCallback {
//      // Provide an example event handling mechanism that records a
//      // metric for the number of events per second and reports that metric
//      // using a 'baem_MetricsManager::RecordsCollectionCallback'.
//
//      // PRIVATE DATA
//      bces_AtomicInt       d_numEvents;         // number of events
//
//      bdet_TimeInterval    d_periodStart;       // start of the current
//                                                // period
//
//      baem_MetricId        d_eventsPerSecId;    // identifies the events-
//                                                // per-second metric
//      baem_MetricsManager::CallbackHandle
//                           d_callbackHandle;    // identifies the callback
//
//      baem_MetricsManager *d_metricsManager_p;  // metrics manager (held,
//                                                // but not owned)
//   // ...
//
//      // PRIVATE MANIPULATORS
//      void collectMetricsCb(bsl::vector<baem_MetricRecord> *records,
//                            bool                            resetFlag);
//          // Append to the specified 'records' the aggregated values of the
//          // metrics recorded by this event handler and, if 'resetFlag' is
//          // 'true', reset those metric values to their default state.  Note
//          // that this method is intended to be used as a callback, and is
//          // consistent with the
//          // 'baem_MetricsManager::RecordsCollectionCallback' function
//          // prototype.
//
//    public:
//      // CREATORS
//      EventHandlerWithCallback(baem_MetricsManager *manager,
//                               bslma_Allocator     *basicAllocator = 0);
//          // Initialize this object to use the specified 'manager' to record
//          // and publish metrics.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~EventHandlerWithCallback();
//          // Destroy this event handler.
//
//      // MANIPULATORS
//      int handleEvent(int eventId, const bsl::string& eventMessage);
//          // Process the event described by the specified 'eventId' and
//          // 'eventMessage'.  Return 0 on success, and a non-zero value if
//          // there was an error processing the event.
//
//  // ...
//
//  };
//..
// In the implementation of 'EventHandlerWithCallback' below, we ensure that
// the callback is registered on construction and removed before the object is
// destroyed.
//..
//  // eventhandlerwithcallback.cpp
//
//  namespace {
//
//  const char *METRIC_CATEGORY = "MyCategory";
//
//  }
//
//..
// The callback creates metric records and populates them with data collected
// by the event handler.
//..
//  // PRIVATE MANIPULATORS
//  void EventHandlerWithCallback::collectMetricsCb(
//                                  bsl::vector<baem_MetricRecord> *records,
//                                  bool                            resetFlag)
//  {
//      int numEvents = resetFlag ? d_numEvents.swap(0) : (int)d_numEvents;
//      bdet_TimeInterval now         = bdetu_SystemTime::now();
//      bdet_TimeInterval elapsedTime = now - d_periodStart;
//      d_periodStart = now;
//
//      baem_MetricRecord record(d_eventsPerSecId);
//      record.count() = 1;
//      record.total() = numEvents / elapsedTime.totalSecondsAsDouble();
//
//      records->push_back(record);
//  }
//
//..
// In the constructor, we initialize a metric id from the specified 'manager'
// object's metric registry.  We will also register the collection callback
// ('collectMetricsCb') with the supplied 'manager'.
//..
//  // CREATORS
//  EventHandlerWithCallback::EventHandlerWithCallback(
//                                         baem_MetricsManager *manager,
//                                         bslma_Allocator     *basicAllocator)
//  : d_numEvents(0)
//  , d_periodStart(bdetu_SystemTime::now())
//  , d_eventsPerSecId()
//  , d_callbackHandle(baem_MetricsManager::BAEM_INVALID_HANDLE)
//  , d_metricsManager_p(manager)
//  {
//      d_eventsPerSecId = d_metricsManager_p->metricRegistry().getId(
//                                        METRIC_CATEGORY, "eventsPerSecond");
//..
// We now register the callback function 'collectMetricsCb' with the metrics
// manager.  We use 'bdef_BindUtil' to bind the member function to a
// 'bdef_Function' matching the
// 'baem_MetricsManager::RecordsCollectionCallback' function prototype.  The
// private data member 'd_callbackHandle' is used to store the
// 'baem_MetricsManager::CallbackHandle' returned for the registered callback;
// we will use this handle later to remove the callback from the metrics
// manager.
//..
//      d_callbackHandle =
//         d_metricsManager_p->registerCollectionCallback(
//            METRIC_CATEGORY,
//            bdef_BindUtil::bindA(basicAllocator,
//                                 &EventHandlerWithCallback::collectMetricsCb,
//                                 this,
//                                 bdef_PlaceHolders::_1,
//                                 bdef_PlaceHolders::_2));
//  }
//
//..
// In the destructor we use the 'baem_MetricsManager::CallbackHandle', stored
// in 'd_callbackHandle', to remove the callback from the metrics manager.
// This prevents the metrics manager from invoking the callback method on an
// object that has already been destroyed.
//..
//  EventHandlerWithCallback::~EventHandlerWithCallback()
//  {
//      int rc =
//             d_metricsManager_p->removeCollectionCallback(d_callbackHandle);
//      assert(0 == rc);
//  }
//
//  // MANIPULATORS
//  int EventHandlerWithCallback::handleEvent(int                eventId,
//                                            const bsl::string& eventMessage)
//  {
//..
// We increment the atomic integer 'd_numEvents' to keep track of the number
// events handled by the 'handleEvent' method.  If collecting a metric is
// expensive (e.g., metrics requiring a system call to collect), clients should
// test whether the metric is enabled before performing the collection
// operation.
//..
//      // We don't test 'd_eventsPerSecId.category()->enabled())' before
//      // incrementing 'd_numEvents' because, in this instance, it will not
//      // improve performance.
//      ++d_numEvents;
//
//  // ...    (Process the event)
//
//      return 0;
//   }
//
//  // ...
//..
///Example 4 - Publishing a Metric
///- - - - - - - - - - - - - - - -
// The metrics manager provides a 'publish' operation to publish metrics for a
// category or set of categories.  In this example we will use the metrics
// manager's 'publishAll' operation to publish all the metrics managed by the
// metrics manager.  We will record metrics for "MyCategory" using instances of
// the 'EventHandler' and 'EventHandlerWithCallback' classes (defined above).
// This example assumes that an instance, 'manager', of the
// 'baem_MetricsManager' class has been initialized as in example 1.  Note
// that, in practice the publish operation is normally tied to a scheduling
// mechanism (e.g., see 'baem_publicationscheduler').
//..
//  EventHandler             eventHandler(&manager);
//  EventHandlerWithCallback eventHandlerWithCallback(&manager);
//
//  eventHandler.handleEvent(0, "A 28 character event message");
//  eventHandlerWithCallback.handleEvent(1, "A different event message");
//  manager.publishAll();
//
//  eventHandler.handleEvent(0, "A 28 character event message");
//  eventHandler.handleEvent(0, "A 28 character event message");
//  eventHandlerWithCallback.handleEvent(1, "A different event message");
//  manager.publishAll();
//..
// Executing the example should result in two samples being published to
// standard output.  Each sample should contain 3 metrics belonging to the
// metric category "MyCategory".  The metric "eventsPerSecond" is collected by
// the 'EventHandlerWithCallback', while "messageSizes", and "eventFailures"
// (both collected by 'EventHandler').
//..
// 09FEB2009_18:52:51.093+0000 3 Records
//         Elapsed Time: 0.001358s
//         MyCategory.eventsPerSecond [count = 1, total = 2267.57, ... ]
//         MyCategory.messageSizes [count = 1, total = 28, min = 28, max = 28]
//         MyCategory.eventFailures [count = 0, total = 0, ... ]
// 09FEB2009_18:52:51.096+0000 3 Records
//         Elapsed Time: 0.002217s
//         MyCategory.eventsPerSecond [count = 1, total = 453.721, ... ]
//         MyCategory.messageSizes [count = 2, total = 56, min = 28, max = 28]
//         MyCategory.eventFailures [count = 0, total = 0, ... ]
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_COLLECTORREPOSITORY
#include <baem_collectorrepository.h>
#endif

#ifndef INCLUDED_BAEM_METRICREGISTRY
#include <baem_metricregistry.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_HASH_MAP
#include <bsl_hash_map.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_SET
#include <bsl_set.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

class baem_Category;
class baem_Publisher;
class baem_MetricRecord;
class baem_MetricSample;

class baem_MetricsManager_PublisherRegistry;  // defined in implementation
class baem_MetricsManager_CallbackRegistry;   // defined in implementation
struct baem_MetricsManager_PublicationHelper;  // defined in implementation

                   // =========================
                   // class baem_MetricsManager
                   // =========================

class baem_MetricsManager {
    // This class implements a manager for the recording and publishing of
    // metrics.  Metrics managed by a 'baem_MetricsManager' are grouped into
    // categories identified by a string.  The metrics manager allows clients
    // to register a 'baem_Publisher' object using the 'addGeneralPublisher'
    // and 'addSpecificPublisher' operations.  Metrics can be recorded in one
    // of two ways: Clients can (1) implement their own metric collection
    // facilities and register a callback using this metric manager's
    // 'registerMetricsCallback' method; or (2) use the 'baem_Collector'
    // objects available from the 'baem_CollectorRepository' owned by this
    // metrics manager.

  public:
    // TYPES
    typedef bdef_Function<void (*)(bsl::vector<baem_MetricRecord> *, bool)>
                                                     RecordsCollectionCallback;
        // 'RecordsCollectionCallback' is an alias for a callback function
        // that appends to the supplied 'baem_MetricRecord' vector the values
        // of the collected metrics, and, if the provided 'bool' is 'true',
        // resets those metrics to their default values.  Clients can register
        // callbacks matching this prototype with the metrics manager.  Here
        // is an example prototype matching this callback:
        //..
        //  void collectionCb(bsl::vector<baem_MetricRecord> *records,
        //                    bool                            resetFlag);
        //..

    typedef int CallbackHandle;
        // Identifies a registered 'RecordsCollectionCallback'.

  private:
    // PRIVATE TYPES
    typedef bsl::map<const baem_Category *, bdet_TimeInterval> LastResetTimes;
        // A mapping from a category to the most recent reset
        // (represented as the interval since the epoch) of that category.
        // This is used to compute the time interval over which a metric was
        // collected.

    // DATA
    baem_MetricRegistry       d_metricRegistry;  // registry of metrics

    baem_CollectorRepository  d_collectors;      // repository of metric
                                                 // collectors

    bdema_ManagedPtr<baem_MetricsManager_CallbackRegistry>
                              d_callbacks;       // registry of callbacks
                                                 // (owned)

    bdema_ManagedPtr<baem_MetricsManager_PublisherRegistry>
                              d_publishers;      // registry of publishers
                                                 // (owned)

    bdet_TimeInterval         d_creationTime;    // time this object was
                                                 // created

    LastResetTimes            d_prevResetTimes;  // time of a category's
                                                 // previous reset

    bcemt_Mutex               d_publishLock;     // lock for 'publish',
                                                 // acquired before 'd_rwLock'

    mutable bcemt_RWMutex     d_rwLock;          // lock for the data maps

    bslma_Allocator          *d_allocator_p;     // allocator (held not owned)

    // FRIENDS
    friend struct baem_MetricsManager_PublicationHelper;

    // NOT IMPLEMENTED
    baem_MetricsManager(const baem_MetricsManager& );
    baem_MetricsManager& operator=(const baem_MetricsManager& );

  public:
    // CONSTANTS
    enum {
        BAEM_INVALID_HANDLE = -1   // an invalid callback handle
    };

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_MetricsManager,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_MetricsManager(bslma_Allocator *basicAllocator = 0);
        // Create a 'baem_MetricsManager'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~baem_MetricsManager();
        // Destroy this 'baem_MetricsManager'.

    // MANIPULATORS
    CallbackHandle registerCollectionCallback(
                               const char                       *categoryName,
                               const RecordsCollectionCallback&  callback);
        // Register the specified 'callback' to collect records for the
        // category having the specified 'categoryName', and return an opaque
        // integer handle that can be used later to remove the 'callback'.
        // The supplied 'callback' will be called to collect metrics from the
        // identified category each time the category is published or
        // collected, even if the category is disabled.  If the 'publish' or
        // 'collectSample' method is called on the identified category, and the
        // category is disabled, the 'callback' will be invoked so clients may
        // update any internal state, but the collected metrics will be
        // ignored.  Clients that wish to avoid overhead associated with
        // collecting metrics for a disabled category must test (within their
        // code) whether the category is disabled.  The behavior is undefined
        // if the 'callback' returns metrics belonging to metric categories
        // other than the one identified by 'categoryName', or if
        // 'categoryName' is not null-terminated.  Note that the same
        // 'callback' can be registered for a category multiple times, and
        // that registering the same 'callback' multiple times will result in
        // an equal number of invocations of the 'callback' when publishing
        // the category.

    CallbackHandle registerCollectionCallback(
                                   const baem_Category              *category,
                                   const RecordsCollectionCallback&  callback);
        // Register the specified 'callback' to collect records for the
        // specified 'category', and return an opaque integer handle that can
        // be used later to remove the 'callback'.  The supplied 'callback'
        // will be called to collect metrics from 'category' each time
        // 'category' is published, even if 'category' is disabled
        // (i.e., 'category->enabled()' is 'false').  If the 'publish' method
        // is called on 'category' and 'category' is disabled, the 'callback'
        // will be invoked so clients may update any internal state, but the
        // collected metrics will be ignored.  Clients that wish to avoid
        // overhead associated with collecting metrics for a disabled category
        // must test (within their code) whether the category is disabled.  The
        // behavior is undefined if the 'callback' returns metrics belonging to
        // metric categories other than 'category'.  Note that the same
        // 'callback' can be registered for a 'category' multiple times, and
        // that registering the same 'callback' multiple times will result in
        // an equal number of invocations of the 'callback' when publishing the
        // 'category'.

    int removeCollectionCallback(CallbackHandle handle);
        // Remove the callback associated with the specified 'handle'.  Return
        // 0 on success, or a non-zero value if 'handle' cannot be found.

    int addGeneralPublisher(const bcema_SharedPtr<baem_Publisher>& publisher);
        // Add the specified 'publisher' to the set of publishers that will be
        // used to propagate records for *every* category published by this
        // metrics manager.  Return 0 on success, and a non-zero value with no
        // effect if 'publisher' has already been registered.  After this
        // method returns, *any* category published by this metrics manager
        // will be published to 'publisher'.  Note that this method will have
        // no effect and return a non-zero value if 'publisher' was previously
        // registered for one or more specific categories (using the
        // alternative 'addSpecificPublisher' method).

    int addSpecificPublisher(
                         const char                             *categoryName,
                         const bcema_SharedPtr<baem_Publisher>&  publisher);
        // Add the specified 'publisher' to the set of publishers that will be
        // used to publish metrics for the category identified by the
        // specified 'categoryName'.  Return 0 on success, and a non-zero
        // value with no effect if 'publisher' has already been registered to
        // publish the identified category.  The behavior is undefined unless
        // 'categoryName' is null-terminated.  Note that this method will fail
        // and return a non-zero value if 'publisher' has previously been
        // registered to publish *every* category (using the alternative
        // 'addGeneralPublisher' method).

    int addSpecificPublisher(
                           const baem_Category                    *category,
                           const bcema_SharedPtr<baem_Publisher>&  publisher);
        // Add the specified 'publisher' to the set of publishers that will be
        // used to publish metrics for the specified 'category'.  Return 0 on
        // success, and a non-zero value with no effect if 'publisher' has
        // already been registered to publish 'category'.  The behavior is
        // undefined unless 'category' is a valid address returned by
        // the 'metricRegistry' method.  Note that this method will fail and
        // return a non-zero value if 'publisher' has previously been
        // registered to publish *every* category (using the alternative
        // 'addGeneralPublisher' method).

    int removePublisher(const baem_Publisher *publisher);
    int removePublisher(const bcema_SharedPtr<baem_Publisher>& publisher);
        // Stop publishing metrics to the specified 'publisher' (for each
        // category), and remove it from this metrics manager.  Return 0 on
        // success, and a non-zero value if 'publisher' cannot be found.

    baem_CollectorRepository& collectorRepository();
        // Return a reference to the modifiable repository of collectors
        // managed by this object.

    baem_MetricRegistry& metricRegistry();
        // Return a reference to the modifiable registry of metrics managed by
        // this object.

    void collectSample(baem_MetricSample              *sample,
                       bsl::vector<baem_MetricRecord> *records,
                       bool                            resetFlag = false);
    void collectSample(baem_MetricSample              *sample,
                       bsl::vector<baem_MetricRecord> *records,
                       const baem_Category    * const  categories[],
                       int                             numCategories,
                       bool                            resetFlag = false);

        // Load into the specified 'sample' a metric sample collected from the
        // indicated categories, and append to 'records' those collected
        // records which are referred to by 'sample'.  Optionally specify a
        // sequence of 'categories' of length 'numCategories'.  If a sequence
        // of categories is not supplied, a sample is collected from all
        // registered categories.  Optionally specify a 'resetFlag' that
        // determines if the collected metrics are reset as part of this
        // operation.  This operation will collect aggregated metric values
        // for each *enabled* category in the indicated categories from
        // registered callbacks as well as from its own
        // 'baem_CollectorRepository', and then append those values to
        // 'records' and update 'sample' with the addresses of those collected
        // 'records'.  If 'resetFlag' is 'true', the metrics being collected
        // are reset to their default state.  This operation also populates the
        // 'sample' with the time interval over which the sampled metrics were
        // collected.  This interval is computed as the elapsed time since the
        // last time the metrics were reset (either through a call to
        // the 'publish' or 'collectSample' methods).  If 'category' has not
        // previously been reset then this interval is taken to be the elapsed
        // time since the creation of this metrics manager.  The behavior is
        // undefined unless '0 <= numCategories', 'categories' refers to a
        // contiguous sequence of (at least) 'numCategories', and each category
        // in 'categories' appears only once.  Note that 'sample' is loaded
        // with the *addresses* of the metric records appended to 'records',
        // and modifying 'records' after this call returns may invalidate
        // 'sample'.

    void publish(const baem_Category *category, bool resetFlag = true);
        // Publish metrics associated with the specified 'category' if
        // 'category' is enabled; otherwise (if 'category' is not enabled)
        // this method has no effect.  Optionally specify a 'resetFlag' that
        // determines if the collected metrics are reset as part of this
        // operation.  If 'category' is enabled (i.e., 'category->isEnabled()'
        // is 'true'),  this operation will collect aggregated metric values
        // for 'category' from any registered callbacks as well as from its
        // own 'baem_CollectorRepository', and then publish those records
        // using any publishers associated with 'category'.  If 'resetFlag' is
        // 'true', the metrics being collected are reset to their default
        // state.  The metrics manager provides publishers the time interval
        // over which the published metrics were collected.  This
        // interval is computed as the elapsed time since the last time the
        // 'category' was reset (either through a call to the 'publish' or
        // 'collectSample' methods).  If 'category' has not previously been
        // reset then this interval is taken to be the elapsed time since the
        // creation of this metrics manager.  Note that the alternative
        // 'publish' methods that publish multiple categories in a single
        // invocation are more efficient than publishing a sequence of
        // categories individually.

    void publish(const baem_Category * const categories[],
                 int                         numCategories,
                 bool                        resetFlag = true);
        // Publish metrics belonging to the specified sequence of (unique)
        // 'categories', of length 'numCategories'.  Optionally specify a
        // 'resetFlag' that determines if the collected metrics are reset as
        // part of this operation.  This operation will collect aggregated
        // metric values for each *enabled* category in 'categories' from
        // registered callbacks as well as from its own
        // 'baem_CollectorRepository', and then publish those records using
        // any publishers associated with the category.  Any individual
        // category in 'categories' that is not enabled is ignored.  If
        // 'resetFlag' is 'true', the metrics being collected are reset to
        // their default state.  The metrics manager provides publishers the
        // time interval over which the published metrics were collected.
        // This interval is computed as the elapsed time since the last time
        // the 'category' was reset (either through a call to the 'publish' or
        // 'collectSample' methods).  If a category has not previously
        // been reset then this interval is taken to be the elapsed time since
        // the creation of this metrics manager.  The behavior is undefined
        // unless '0 <= numCategories', 'categories' refers to a contiguous
        // sequence of (at least) 'numCategories', and each category in
        // 'categories' appears only once.

    void publish(const bsl::set<const baem_Category *>& categories,
                 bool                                   resetFlag = true);
        // Publish metrics belonging to the specified 'categories'.
        // Optionally specify a 'resetFlag' that determines if the metrics
        // are reset as part of this operation.  This operation will collect
        // aggregated metric values for each *enabled* category in
        // 'categories' from registered callbacks as well as from its own
        // 'baem_CollectorRepository', and then publish those records using
        // any publishers associated with the category.   Any category in
        // 'categories' that is not enabled is ignored.  If 'resetFlag' is
        // 'true', the metrics being collected are reset to their default
        // state.  The metrics manager provides publishers the time interval
        // over which the published metrics were collected.  This interval is
        // computed as the elapsed time since the last time the 'category' was
        // reset (either through a call to the 'publish' or 'collectSample'
        // methods).  If a category has not previously been reset then this
        // interval is taken to be the elapsed time since the creation of this
        // metrics manager.

    void publishAll(bool resetFlag = true);
        // Publish metrics for every category registered with the contained
        // 'baem_MetricsRegistry' object.  Optionally specify a 'resetFlag'
        // that determines if the metrics are reset as part of this
        // operation.  This operation will collect aggregated metric values
        // for each enabled category in its 'metricRegistry()' from registered
        // callbacks as well as from its own 'baem_CollectorRepository', and
        // then publish those records using any publishers associated with the
        // category.  Any category that is not enabled is ignored.  If
        // 'resetFlag' is 'true', the metrics being collected are reset to
        // their default state.  The metrics manager provides publishers the
        // time interval over which a published category of metrics were
        // collected.  This interval is computed as the elapsed time since the
        // last time the category was reset (either through a call to the
        // 'publish' or 'collectSample' methods).  If a category has not
        // previously been reset then this interval is taken to be the elapsed
        // time since the creation of this metrics manager.

    void publishAll(const bsl::set<const baem_Category *>& excludedCategories,
                    bool                                   resetFlag = true);
        // Publish metrics for every category registered with the contained
        // 'baem_MetricsRegistry' object, except for the specified
        // 'excludedCategories'.  Optionally specify a 'resetFlag' that
        // determines if the metrics are reset as part of this operation.
        // This operation will collect aggregated metric values for each
        // *enabled* category in its 'metricRegistry()' (that is not in
        // 'excludedCategories') from registered callbacks as well as from its
        // own 'baem_CollectorRepository', and then publish those records
        // using any publishers associated with the category.  Any category
        // that is not enabled is ignored.  If 'resetFlag' is 'true', the
        // metrics being collected are reset to their default state.  The
        // metrics manager provides publishers the time interval over which a
        // published category of metrics were collected.  This interval is
        // computed as the elapsed time since the last time the category was
        // reset (either through a call to the 'publish' or 'collectSample'
        // methods).  If a category has not previously been reset then this
        // interval is taken to be the elapsed time since the creation of this
        // metrics manager.

    void setCategoryEnabled(const char *category,
                            bool        isEnabled = true);
        // Set whether the specified 'category' is enabled to the specified
        // 'isEnabled' value.  If 'category' has not been registered, register
        // it with the 'metricRegistry()'.  If a category is disabled it will
        // not be published (see the 'publish' methods), and higher level
        // components may not record values for metrics belonging to the
        // category (for an example, see the 'baem_metric' documentation).
        // Note that this operation is *not* atomic, and other threads may
        // simultaneously access the current enabled value for 'category' while
        // this operation is performed.  Also note that this operation has
        // *linear* performance with respect to the number of registered
        // category holders for 'category' (see 'baem_metricregistry' and
        // 'baem_category' for information on category holders).

    void setCategoryEnabled(const baem_Category *category,
                            bool                 value = true);
        // Set whether the specified 'category' is enabled to the specified
        // 'value'.  If a category is disabled it will not be
        // published (see the 'publish' methods), and higher level components
        // may not record values for metrics belonging to the category (for an
        // example, see the 'baem_metric' documentation).  The behavior is
        // undefined unless 'category' is a valid address of a category
        // previously returned by the metric registry owned by this
        // 'baem_MetricManager' object (i.e., 'metricRegistry()').  Note that
        // this operation is thread-safe, but *not* atomic: Other threads may
        // simultaneously access the current enabled value for 'category' while
        // this operation is performed.  Also note that this operation has
        // *linear* performance with respect to the number of registered
        // category holders for 'category' (see 'baem_metricregistry' and
        // 'baem_category' for information on category holders).

     void setAllCategoriesEnabled(bool value);
        // Set whether each category currently registered with
        // 'metricRegistry()' is enabled to the specified 'value', and ensure
        // that categories registered after this call are initialized as
        // either enabled or disabled, accordingly.  If a category is disabled
        // it will not be published (see the 'publish' methods), and higher
        // level components may not record values for metrics belonging to the
        // category (for an example, see the 'baem_metric' documentation).
        // This operation is logically equivalent to iterating over the list of
        // categories currently registered with 'metricRegistry()' and calling
        // 'setCategoryEnabled' on each category individually, and also setting
        // a default 'isEnabled()' value (for newly-created categories).
        // Hence, subsequent calls to 'setCategoryEnabled' will override this
        // value for a particular category.  Note that this operation is
        // thread-safe, but *not* atomic: Other threads may simultaneously
        // access the current enabled status for a category while this
        // operation completes.  Also note that this operation has *linear*
        // runtime performance with respect to the total number of category
        // holders registered with 'metricRegistry()' (see
        // 'baem_metricregistry' and 'baem_category' for information on
        // category holders).

    // ACCESSORS
    int findGeneralPublishers(bsl::vector<baem_Publisher *> *publishers) const;
        // Append to the specified 'publishers' the addresses of publishers
        // registered to publish metrics for every category.  Return the number
        // of publishers found.  Note that this method will not find publishers
        // associated with individual categories (i.e., category specific
        // publishers).

    int findSpecificPublishers(
                            bsl::vector<baem_Publisher *> *publishers,
                            const char                    *categoryName) const;
        // Append to the specified 'publishers' the addresses of any
        // publishers associated with the (particular) category identified by
        // the specified 'categoryName'.  Return the number of publishers
        // found for the 'category'.  The behavior is undefined unless
        // 'category' is null-terminated.  Note that this method will not find
        // publishers registered for every category (i.e., general publishers)

    int findSpecificPublishers(bsl::vector<baem_Publisher *> *publishers,
                               const baem_Category           *category) const;
        // Append to the specified 'publishers' the addresses of any
        // publishers associated with the (particular) specified 'category'.
        // Return the number of publishers found for the 'category'.  The
        // behavior is undefined unless 'category' is a valid address supplied
        // by 'metricRegistry()'.  Note that this method will not find
        // publishers registered for every category (i.e., general publishers).

    const baem_CollectorRepository& collectorRepository() const;
        // Return a reference to the non-modifiable repository of collectors
        // managed by this object.

    const baem_MetricRegistry& metricRegistry() const;
        // Return a reference to the non-modifiable registry of metrics managed
        // by this object.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // -------------------------
                   // class baem_MetricsManager
                   // -------------------------

// MANIPULATORS
inline
baem_MetricsManager::CallbackHandle
baem_MetricsManager::registerCollectionCallback(
                               const char                       *categoryName,
                               const RecordsCollectionCallback&  callback)
{
    return registerCollectionCallback(
                                d_metricRegistry.getCategory(categoryName),
                                callback);
}

inline
int baem_MetricsManager::addSpecificPublisher(
                          const char                             *categoryName,
                          const bcema_SharedPtr<baem_Publisher>&  publisher)
{
    return addSpecificPublisher(d_metricRegistry.getCategory(categoryName),
                                publisher);
}

inline
void baem_MetricsManager::setCategoryEnabled(const baem_Category *category,
                                             bool                 value)
{
    d_metricRegistry.setCategoryEnabled(category, value);
}

inline
void baem_MetricsManager::setCategoryEnabled(const char  *categoryName,
                                             bool         value)
{
    d_metricRegistry.setCategoryEnabled(
                                  d_metricRegistry.getCategory(categoryName),
                                  value);
}

inline
void baem_MetricsManager::setAllCategoriesEnabled(bool value)
{
    d_metricRegistry.setAllCategoriesEnabled(value);
}

inline
baem_CollectorRepository& baem_MetricsManager::collectorRepository()
{
    return d_collectors;
}

inline
baem_MetricRegistry& baem_MetricsManager::metricRegistry()
{
    return d_metricRegistry;
}

// ACCESSORS

inline
int baem_MetricsManager::findSpecificPublishers(
                               bsl::vector<baem_Publisher *> *publishers,
                               const char                    *category) const
{
    const baem_Category *categoryPtr = d_metricRegistry.findCategory(category);
    return categoryPtr ? findSpecificPublishers(publishers, categoryPtr) : 0;
}

inline
const baem_CollectorRepository&
baem_MetricsManager::collectorRepository() const
{
    return d_collectors;
}

inline
const baem_MetricRegistry& baem_MetricsManager::metricRegistry() const
{
    return d_metricRegistry;
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
