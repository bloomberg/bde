// baem_publicationscheduler.h              -*-C++-*-
#ifndef INCLUDED_BAEM_PUBLICATIONSCHEDULER
#define INCLUDED_BAEM_PUBLICATIONSCHEDULER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a scheduler for publishing metrics.
//
//@CLASSES:
// baem_PublicationScheduler: a scheduler for publishing metrics
//
//@SEE_ALSO: baem_metricsmanager
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION:  This component defines a 'baem_PublicationScheduler' class
// that provides a scheduling mechanism for the publication of metrics.
// At construction, a 'baem_PublicationScheduler' is provided the addresses of
// a 'baem_MetricsManager' and a 'bcep_TimerEventScheduler'.  The publication
// scheduler provides a 'scheduleCategory' method that schedules an
// individual metric category to be published repeatedly at a given interval,
// and a 'setDefaultSchedule' method that schedules the publication of any
// category not given an individual schedule.  The 'baem_PublicationScheduler'
// creates timer events using the 'bcep_TimerEventScheduler'.  At the end of a
// scheduled time interval, the publication scheduler invokes the metrics
// manager's 'publish' operation with the set of categories to publish.  Note
// that the publication scheduler will combine categories that occur at the
// same frequency into a single invocation of the metrics manager's 'publish'
// operation.  The publication scheduler also provides a method to cancel the
// publication of a particular category, or of all categories.
//
///Thread Safety
///-------------
// 'baem_PublicationScheduler' is fully *thread-safe*, meaning that all
// non-creator operations on a given instance can be safely invoked
// simultaneously from multiple threads.
//
///Usage
///-----
// The following example demonstrates how to use 'baem_PublicationScheduler'.
// Before instantiating the publication scheduler, we create a
// 'bcep_TimerEventScheduler' as well as a 'baem_MetricsManager'.  We obtain
// collectors for three different metric categories, "A", "B", and "C", that
// we will use to generate metric values for publication.
//..
//  bslma_Allocator          *allocator = bslma_Default::allocator(0);
//  bcep_TimerEventScheduler  timer(allocator);
//  baem_MetricsManager       manager(allocator);
//
//  baem_Collector *A = manager.collectorRepository().getDefaultCollector(
//                                                                   "A", "a");
//  baem_Collector *B = manager.collectorRepository().getDefaultCollector(
//                                                                   "B", "b");
//  baem_Collector *C = manager.collectorRepository().getDefaultCollector(
//                                                                   "C", "c");
//..
// We now create an instance of 'SimpleStreamPublisher', which implements the
// 'baem_Publisher' protocol.  Note that 'SimpleStreamPublisher' is an
// example implementation of the 'baem_Publisher' protocol defined in the
// 'baem_publisher' component.  In practice, clients typically use a standard
// publisher class (e.g., 'baem_StreamPublisher').
//..
//      bcema_SharedPtr<baem_Publisher> publisher(
//                          new (*allocator) SimpleStreamPublisher(bsl::cout),
//                          allocator);
//..
// We now register the 'publisher' we have created with the metrics 'manager'
// to publish our categories and 'start' the timer-event scheduler.
//..
//  manager.addGeneralPublisher(publisher);
//  timer.start();
//..
// Now we construct a 'baem_PublicationScheduler' and pass it the respective
// addresses of both the metrics manager and the timer-event scheduler.  We
// schedule the publication of category "A" and "B" every .05 seconds, then we
// set the scheduled default publication to every .10 seconds.  Note that those
// time intervals were chosen to ensure fast and consistent output for this
// example.  In normal usage the interval between publications should be large
// enough to ensure that metric publication does not negatively affect the
// performance of the application (a 30 second interval is typical).
//..
//  baem_PublicationScheduler scheduler(&manager, &timer, allocator);
//  scheduler.scheduleCategory("A", bdet_TimeInterval(.05));
//  scheduler.scheduleCategory("B", bdet_TimeInterval(.05));
//  scheduler.setDefaultSchedule(bdet_TimeInterval(.10));
//..
// We can use the accessor operations to verify the schedule that we have
// specified.
//..
//  bdet_TimeInterval intervalA, intervalB, intervalC, defaultInterval;
//  assert( scheduler.findCategorySchedule(&intervalA, "A"));
//  assert( scheduler.findCategorySchedule(&intervalB, "B"));
//  assert(!scheduler.findCategorySchedule(&intervalC, "C"));
//  assert( scheduler.getDefaultSchedule(&defaultInterval));
//
//  assert(bdet_TimeInterval(.05) == intervalA);
//  assert(bdet_TimeInterval(.05) == intervalB);
//  assert(bdet_TimeInterval(.10) == defaultInterval);
//..
// Finally we add a couple of metrics and wait just over .1 seconds.
//..
//  A->update(1.0);
//  B->update(2.0);
//  C->update(3.0);
//  bcemt_ThreadUtil::sleep(bdet_TimeInterval(.11));
//..
// The output of the publication should look similar to:
//..
// 19NOV2008_18:34:26.766+0000    2 Records   0.0517s Elapsed Time
//         A.a [count = 1, total = 1, min = 1, max = 1]
//         B.b [count = 1, total = 2, min = 2, max = 2]
// 19NOV2008_18:34:26.816+0000    2 Records   0.050183s Elapsed Time
//         A.a [count = 0, total = 0, min = inf, max = -inf]
//         B.b [count = 0, total = 0, min = inf, max = -inf]
// 19NOV2008_18:34:26.817+0000    1 Records   0.102473s Elapsed Time
//         C.c [count = 1, total = 3, min = 3, max = 3]
//..
// Note that category 'C' is published as part of the scheduled default
// publication.  Also note that categories 'A' and 'B' are emitted as a single
// publication: the scheduler combines categories published at the same
// frequency into a single publication event to minimize the number of times
// 'baem_MetricsManager::publish' is invoked.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_METRICSMANAGER
#include <baem_metricsmanager.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEP_TIMEREVENTSCHEDULER
#include <bcep_timereventscheduler.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_SET
#include <bsl_set.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class baem_Category;

class baem_PublicationScheduler_ClockData;  // defined in implementation
class baem_PublicationScheduler_Proctor;    // defined in implementation

                   // ===============================
                   // class baem_PublicationScheduler
                   // ===============================

class baem_PublicationScheduler {
    // This class defines a mechanism for scheduling the periodic publication
    // of metrics.  Each publication scheduler object is supplied the address
    // of a 'baem_MetricsManager' and a 'bcep_TimerEventScheduler' at
    // construction.  The metrics manager is used to publish metrics, while
    // the timer-event scheduler provides the underlying scheduling mechanism.
    // Metrics are scheduled for publication using the 'scheduleCategory'
    // and 'setDefaultSchedule' methods.  The 'scheduleCategory' method
    // schedules an individual category to be publisher periodically at the
    // provided interval, whereas 'setDefaultSchedule' schedules the periodic
    // publication of any category not given an individual schedule.  The
    // publication scheduler will create a recurring timer for each unique
    // time interval supplied, and will group together categories that share a
    // common time interval into a single call to
    // 'baem_MetricsManager::publish'.  Note that it is left unspecified
    // whether publication events that occur on a common multiple of
    // *different* intervals will be grouped into a single invocation of
    // 'baem_MetricsManager::publish'.

    // PRIVATE TYPES
    typedef baem_PublicationScheduler_ClockData                  ClockData;
        // A private implementation type holding the data for a scheduled
        // publication frequency (e.g., the set of categories published at that
        // frequency).  Each "clock" created in the underlying
        // 'bcep_TimerEventScheduler' is associated with a 'ClockData' object.

    typedef bsl::map<const baem_Category *, bdet_TimeInterval>   Categories;
        // A map from a category to the publication interval for that
        // category.

    typedef bsl::map<bdet_TimeInterval,
                     bcema_SharedPtr<ClockData> >                Clocks;
        // A map from a time interval (i.e., publication period) to the clock
        // information for that time interval.

    // DATA
    bcep_TimerEventScheduler *d_scheduler_p;  // event scheduler (held)

    baem_MetricsManager      *d_manager_p;    // metrics manager (held)

    Categories                d_categories;   // map of category => schedule

    Clocks                    d_clocks;       // map of interval => clock info

    bdet_TimeInterval         d_defaultInterval;
                                              // default publication interval

    mutable bcemt_Mutex       d_mutex;        // synchronize access to data
                                              // ('d_categories', 'd_clocks',
                                              // and 'd_defaultInterval')

    bslma_Allocator          *d_allocator_p;  // allocator (held, not owned)


    // NOT IMPLEMENTED
    baem_PublicationScheduler(const baem_PublicationScheduler& );
    baem_PublicationScheduler& operator=(const baem_PublicationScheduler& );

    // FRIENDS
    friend class baem_PublicationScheduler_Proctor;

    // PRIVATE MANIPULATORS
    void publish(bcema_SharedPtr<ClockData> clockData);
        // Publish, to the held 'baem_MetricsManager' object, the categories
        // indicated by the specified 'clockData'.  Note that this operation
        // serves as the event callback provided to the underlying
        // 'bcep_TimerEventScheduler': this method is bound with a
        // 'ClockData' object in the 'bdef_Function' objects provided to
        // 'd_scheduler_p'.

    void cancelCategory(Categories::iterator categoryIterator);
        // Cancel the periodic publication of the category indicated by the
        // specified 'categoryIterator'.  Any scheduled publication of
        // the indicated category is either canceled or completed before this
        // method returns.  The behavior is undefined unless
        // 'categoryIterator'  is a valid iterator over 'd_categories' and
        // 'd_mutex' is *locked*.

    int cancelDefaultSchedule();
        // If the default publication schedule has been set (using
        // 'setDefaultSchedule'), cancel that periodic default publication,
        // and return 0.  This method has no effect and will return a non-zero
        // value if a default publication schedule has not been set.  Any
        // scheduled publication is either canceled or completed before this
        // method returns.  The behavior is undefined unless 'd_mutex' is
        // *locked*.

  public:
    // PUBLIC TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_PublicationScheduler,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_PublicationScheduler(baem_MetricsManager      *metricsManager,
                              bcep_TimerEventScheduler *eventScheduler,
                              bslma_Allocator          *basicAllocator = 0);
        // Create a publication scheduler that will use the specified
        // 'metricsManager' to publish metrics, and the specified
        // 'eventScheduler' to supply timer events.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~baem_PublicationScheduler();
        // Destroy this publication scheduler and cancel any pending
        // publications.  Note that, if any metrics are currently being
        // published, this operation will block until all of their
        // publications have completed.

    // MANIPULATORS
    void scheduleCategory(const char               *category,
                          const bdet_TimeInterval&  interval);
        // Schedule the specified 'category' to be published periodically at
        // the specified 'interval' using the 'baem_MetricManager' supplied at
        // construction.  If 'category' has *already* been scheduled, change
        // the scheduled period to 'interval'; any previously scheduled
        // publication of 'category' is either canceled or completed
        // (atomically) prior to rescheduling.  If a category is rescheduled
        // with the same 'interval' as it is currently scheduled, this
        // operation has no effect.  The behavior is undefined unless
        // 'bdet_TimeInterval(0, 0) < interval' and 'category' is
        // null-terminated.

    void scheduleCategory(const baem_Category      *category,
                          const bdet_TimeInterval&  interval);
        // Schedule the specified 'category' to be published periodically at
        // the specified 'interval' using the 'baem_MetricManager' supplied at
        // construction.  If 'category' has *already* been scheduled, change
        // the scheduled period to 'interval'; any previously scheduled
        // publication of 'category' is either canceled or completed
        // (atomically) prior to rescheduling.  If a category is rescheduled
        // with the same 'interval' as it is currently scheduled, this
        // operation has no effect.  The behavior is undefined unless
        // 'bdet_TimeInterval(0, 0) < interval' and 'category' is a valid
        // address supplied by the 'baem_MetricRegistry' owned by the
        // 'baem_MetricsManager' object supplied at construction.

    void setDefaultSchedule(const bdet_TimeInterval& interval);
        // Set, to the specified 'interval', the default interval for metrics
        // to be periodically published using the 'baem_MetricsManager'
        // supplied at construction.  This method schedules every metric
        // category not given a individual schedule (using 'scheduleCategory'),
        // to be published periodically until that category is either given an
        // individual schedule, or the default schedule is canceled (using
        // either 'clearDefaultSchedule' or 'cancelAllPublications').  If a
        // default publication has *already* been scheduled, change its
        // schedule to 'interval'; any previously scheduled publication is
        // either canceled or completed (atomically) before rescheduling.  If
        // the default publication is rescheduled with the same 'interval' as
        // it is currently scheduled, this operation has no effect.  The
        // behavior is undefined unless 'bdet_TimeInterval(0, 0) < interval'.
        // Note that, to exclude a category from any publication, clients can
        // disable the category using the 'baem_MetricsManager' object supplied
        // at construction.

    int cancelCategorySchedule(const char *category);
        // Cancel the periodic publication of the specified 'category'.  Return
        // 0 on success, and a non-zero value if the specified 'category' is
        // not scheduled for publication.  Any scheduled publication of
        // 'category' is either canceled or completed before this method
        // returns.  The behavior is undefined unless 'category' is
        // null-terminated.  Note that if a default publication schedule has
        // been set (using 'setDefaultSchedule'), then 'category' will continue
        // to be published as part of that scheduled default publication; to
        // exclude a category from any publication, clients can disable the
        // category using the 'baem_MetricsManager' object supplied at
        // construction.

    int cancelCategorySchedule(const baem_Category *category);
        // Cancel the periodic publication of the specified 'category'.  Return
        // 0 on success, and a non-zero value if the specified 'category' is
        // not scheduled for publication.  Any scheduled publication of
        // 'category' is either canceled or completed before this method
        // returns.  The behavior is undefined unless 'category' is a valid
        // address supplied by the 'baem_MetricRegistry' owned by
        // 'metricsManager'.  Note that if a default publication schedule has
        // been set (using 'setDefaultSchedule'), then 'category' will continue
        // to be published as part of that scheduled default publication; to
        // exclude a category from any publication, clients can disable the
        // category using the 'baem_MetricsManager' object supplied at
        // construction.

    int clearDefaultSchedule();
        // If the default publication schedule has been set (using
        // 'setDefaultSchedule'), cancel that periodic default publication, and
        // return 0.  This method has no effect and will return a non-zero
        // value if a default publication schedule has not been set.  Any
        // scheduled publication is either canceled or completed before this
        // method returns.

    void cancelAll();
        // Cancel all periodic publication of metrics.  This operation
        // (atomically) clears the default publication schedule and cancels the
        // publication schedule of any category individually scheduled using
        // the 'scheduleCategory' method.  Any scheduled publication is
        // either canceled or completed before this method returns.

    baem_MetricsManager *manager();
        // Return the address of the modifiable metrics manager for which this
        // publication scheduler publishes metrics.

    // ACCESSORS
    bool findCategorySchedule(bdet_TimeInterval  *result,
                              const char         *category) const;
         // Load into the specified 'result' the individual schedule interval
         // (set using the 'scheduleCategory' method) that corresponds to the
         // specified 'category', if found, and return 'true', or (if not
         // found) return 'false' with no effect.  This method will return
         // 'false' and will not modify 'result' if 'category' is published as
         // part of the default scheduled publication.  The behavior is
         // undefined unless 'category' is null-terminated.

    bool findCategorySchedule(bdet_TimeInterval   *result,
                              const baem_Category *category) const;
         // Load into the specified 'result' the individual schedule interval
         // (set using the 'scheduleCategory' method) that corresponds to the
         // specified 'category', if found, and return 'true', or (if not
         // found) return 'false' with no effect.  This method will return
         // 'false' and will not modify 'result' if 'category' is published as
         // part of the default scheduled publication.  The behavior is
         // undefined unless 'category' is a valid address supplied by the
         // 'baem_MetricRegistry' owned by the 'baem_MetricsManager' object
         // supplied at construction.

    bool getDefaultSchedule(bdet_TimeInterval *result) const;
        // Load into the specified 'result' the default scheduled interval,
        // (set using the 'setDefaultSchedule' method), for periodically
        // publishing metrics, if found, and return 'true', or (if not found)
        // return 'false' with no effect.

    int getCategorySchedule(
                   bsl::vector<bsl::pair<const baem_Category *,
                                         bdet_TimeInterval> > *result) const;
        // Load into the specified 'result' a representation of the current
        // schedule for publishing categories being followed by this
        // scheduler and return the number of scheduled categories.  The
        // schedule is represented using a series of (category address, time
        // interval) pairs; each pair in the series indicates the periodic time
        // interval that the associated category will be published.  Note that
        // the 'result' of this operation contains only those categories
        // scheduled using the 'scheduleCategory' operation, and does *not*
        // include categories published as part of the default publication.

    const baem_MetricsManager *manager() const;
        // Return the address of the non-modifiable metrics manager for
        // which this publication scheduler will publish metrics.

    bsl::ostream& print(bsl::ostream&   stream,
                        int             level = 0,
                        int             spacesPerLevel = 4) const;
        // Print a formatted string describing the current state of this
        // 'baem_PublicationScheduler' object to the specified 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress all indentation AND format
        // the entire output on one line.  If 'stream' is not valid on entry,
        // this operation has no effect.  Note that this is provided primarily
        // for debugging purposes.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // -------------------------------
                   // class baem_PublicationScheduler
                   // -------------------------------

// MANIPULATORS
inline
baem_MetricsManager *baem_PublicationScheduler::manager()
{
    return d_manager_p;
}

inline
void baem_PublicationScheduler::scheduleCategory(
                                            const char               *category,
                                            const bdet_TimeInterval&  interval)
{
    scheduleCategory(d_manager_p->metricRegistry().getCategory(category),
                     interval);
}

inline
int baem_PublicationScheduler::cancelCategorySchedule(const char *category)
{
    return cancelCategorySchedule(
                          d_manager_p->metricRegistry().getCategory(category));
}

// ACCESSORS
inline
const baem_MetricsManager *baem_PublicationScheduler::manager() const
{
    return d_manager_p;
}

inline
bool baem_PublicationScheduler::findCategorySchedule(
                                           bdet_TimeInterval *result,
                                           const char        *category) const
{
    return findCategorySchedule(
                          result,
                          d_manager_p->metricRegistry().getCategory(category));
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
