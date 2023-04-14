// balm_publicationscheduler.h                                        -*-C++-*-
#ifndef INCLUDED_BALM_PUBLICATIONSCHEDULER
#define INCLUDED_BALM_PUBLICATIONSCHEDULER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a scheduler for publishing metrics.
//
//@CLASSES:
// balm::PublicationScheduler: a scheduler for publishing metrics
//
//@SEE_ALSO: balm_metricsmanager
//
//@DESCRIPTION: This component defines a class, 'balm::PublicationScheduler',
// that provides a scheduling mechanism for the publication of metrics. At
// construction, a 'balm::PublicationScheduler' is provided the addresses of a
// 'balm::MetricsManager' and a 'bdlmt::TimerEventScheduler'.  The publication
// scheduler provides a 'scheduleCategory' method that schedules an individual
// metric category to be published repeatedly at a given interval, and a
// 'setDefaultSchedule' method that schedules the publication of any category
// not given an individual schedule.  The 'balm::PublicationScheduler' creates
// timer events using the 'bdlmt::TimerEventScheduler'.  At the end of a
// scheduled time interval, the publication scheduler invokes the metrics
// manager's 'publish' operation with the set of categories to publish.  Note
// that the publication scheduler will combine categories that occur at the
// same frequency into a single invocation of the metrics manager's 'publish'
// operation.  The publication scheduler also provides a method to cancel the
// publication of a particular category, or of all categories.
//
///Alternative Systems for Telemetry
///---------------------------------
// Bloomberg software may alternatively use the GUTS telemetry API, which is
// integrated into Bloomberg infrastructure.
//
///Thread Safety
///-------------
// 'balm::PublicationScheduler' is fully *thread-safe*, meaning that all
// non-creator operations on a given instance can be safely invoked
// simultaneously from multiple threads.
//
///Usage
///-----
// The following example demonstrates how to use 'balm::PublicationScheduler'.
// Before instantiating the publication scheduler, we create a
// 'bdlmt::TimerEventScheduler' as well as a 'balm::MetricsManager'.  We obtain
// collectors for three different metric categories, "A", "B", and "C", that
// we will use to generate metric values for publication.
//..
//  bslma::Allocator         *allocator = bslma::Default::allocator(0);
//  bdlmt::TimerEventScheduler  timer(allocator);
//  balm::MetricsManager       manager(allocator);
//
//  balm::Collector *A = manager.collectorRepository().getDefaultCollector(
//                                                                   "A", "a");
//  balm::Collector *B = manager.collectorRepository().getDefaultCollector(
//                                                                   "B", "b");
//  balm::Collector *C = manager.collectorRepository().getDefaultCollector(
//                                                                   "C", "c");
//..
// We now create an instance of 'SimpleStreamPublisher', which implements the
// 'balm::Publisher' protocol.  Note that 'SimpleStreamPublisher' is an
// example implementation of the 'balm::Publisher' protocol defined in the
// 'balm_publisher' component.  In practice, clients typically use a standard
// publisher class (e.g., 'balm::StreamPublisher').
//..
//      bsl::shared_ptr<balm::Publisher> publisher(
//                          new (*allocator) SimpleStreamPublisher(bsl::cout),
//                          allocator);
//..
// We now register the 'publisher' we have created with the metrics 'manager'
// to publish our categories.  Then, we 'start' the timer-event scheduler we
// will supply to the 'balm::PublicationScheduler'.
//..
//  manager.addGeneralPublisher(publisher);
//  timer.start();
//..
// Now we construct a 'balm::PublicationScheduler' and pass it the respective
// addresses of both the metrics manager and the timer-event scheduler.  We
// schedule the publication of category "A" and "B" every .05 seconds, then we
// set the scheduled default publication to every .10 seconds.  Note that those
// time intervals were chosen to ensure fast and consistent output for this
// example.  In normal usage the interval between publications should be large
// enough to ensure that metric publication does not negatively affect the
// performance of the application (a 30 second interval is typical).
//..
//  balm::PublicationScheduler scheduler(&manager, &timer, allocator);
//  scheduler.scheduleCategory("A", bsls::TimeInterval(.05));
//  scheduler.scheduleCategory("B", bsls::TimeInterval(.05));
//  scheduler.setDefaultSchedule(bsls::TimeInterval(.10));
//..
// We can use the accessor operations to verify the schedule that we have
// specified.
//..
//  bsls::TimeInterval intervalA, intervalB, intervalC, defaultInterval;
//  assert( scheduler.findCategorySchedule(&intervalA, "A"));
//  assert( scheduler.findCategorySchedule(&intervalB, "B"));
//  assert(!scheduler.findCategorySchedule(&intervalC, "C"));
//  assert( scheduler.getDefaultSchedule(&defaultInterval));
//
//  assert(bsls::TimeInterval(.05) == intervalA);
//  assert(bsls::TimeInterval(.05) == intervalB);
//  assert(bsls::TimeInterval(.10) == defaultInterval);
//..
// Finally we add a couple of metrics and wait just over .1 seconds.
//..
//  A->update(1.0);
//  B->update(2.0);
//  C->update(3.0);
//  bslmt::ThreadUtil::sleep(bsls::TimeInterval(.11));
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
// 'balm::MetricsManager::publish' is invoked.

#include <balscm_version.h>

#include <balm_metricsmanager.h>

#include <bslmt_mutex.h>

#include <bdlmt_timereventscheduler.h>

#include <bsls_timeinterval.h>

#include <bslma_allocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_iosfwd.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_set.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>  // 'std::pmr::polymorphic_allocator'
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

#include <vector>                   // 'std::vector', 'std::pmr::vector'

namespace BloombergLP {


namespace balm {

class Category;

class PublicationScheduler_ClockData;  // defined in implementation
class PublicationScheduler_Proctor;    // defined in implementation

                         // ==========================
                         // class PublicationScheduler
                         // ==========================

class PublicationScheduler {
    // This class defines a mechanism for scheduling the periodic publication
    // of metrics.  Each publication scheduler object is supplied the address
    // of a 'MetricsManager' and a 'bdlmt::TimerEventScheduler' at
    // construction.  The metrics manager is used to publish metrics, while
    // the timer-event scheduler provides the underlying scheduling mechanism.
    // Metrics are scheduled for publication using the 'scheduleCategory' and
    // 'setDefaultSchedule' methods.  The 'scheduleCategory' method schedules
    // an individual category to be publisher periodically at the provided
    // interval, whereas 'setDefaultSchedule' schedules the periodic
    // publication of any category not given an individual schedule.  The
    // publication scheduler will create a recurring timer for each unique
    // time interval supplied, and will group together categories that share a
    // common time interval into a single call to 'MetricsManager::publish'.
    // Note that it is left unspecified whether publication events that occur
    // on a common multiple of *different* intervals will be grouped into a
    // single invocation of 'MetricsManager::publish'.

    // PRIVATE TYPES
    typedef PublicationScheduler_ClockData                  ClockData;
        // A private implementation type holding the data for a scheduled
        // publication frequency (e.g., the set of categories published at that
        // frequency).  Each "clock" created in the underlying
        // 'bdlmt::TimerEventScheduler' is associated with a 'ClockData'
        // object.

    typedef bsl::map<const Category *, bsls::TimeInterval>   Categories;
        // A map from a category to the publication interval for that
        // category.

    typedef bsl::map<bsls::TimeInterval,
                     bsl::shared_ptr<ClockData> >                Clocks;
        // A map from a time interval (i.e., publication period) to the clock
        // information for that time interval.

    // DATA
    bdlmt::TimerEventScheduler *d_scheduler_p;  // event scheduler (held)

    MetricsManager             *d_manager_p;    // metrics manager (held)

    Categories                  d_categories;   // map of category => schedule

    Clocks                      d_clocks;       // map of interval => clock
                                                // info

    bsls::TimeInterval          d_defaultInterval;
                                                // default publication interval

    mutable bslmt::Mutex        d_mutex;        // synchronize access to data
                                                // ('d_categories', 'd_clocks',
                                                // and 'd_defaultInterval')

    bslma::Allocator           *d_allocator_p;  // allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    PublicationScheduler(const PublicationScheduler& );
    PublicationScheduler& operator=(const PublicationScheduler& );

    // FRIENDS
    friend class PublicationScheduler_Proctor;

    // PRIVATE MANIPULATORS
    void publish(bsl::shared_ptr<ClockData> clockData);
        // Publish, to the held 'MetricsManager' object, the categories
        // indicated by the specified 'clockData'.  Note that this operation
        // serves as the event callback provided to the underlying
        // 'bdlmt::TimerEventScheduler': this method is bound with a
        // 'ClockData' object in the 'bsl::function' objects provided to
        // 'd_scheduler_p'.

    void cancelCategory(Categories::iterator categoryIterator);
        // Cancel the periodic publication of the category indicated by the
        // specified 'categoryIterator'.  Any scheduled publication of the
        // indicated category is either canceled or completed before this
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
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(PublicationScheduler,
                                                    bslma::UsesBslmaAllocator);

    // CREATORS
    PublicationScheduler(MetricsManager             *metricsManager,
                         bdlmt::TimerEventScheduler *eventScheduler,
                         bslma::Allocator           *basicAllocator = 0);
        // Create a publication scheduler that will use the specified
        // 'metricsManager' to publish metrics, and the specified
        // 'eventScheduler' to supply timer events.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~PublicationScheduler();
        // Destroy this publication scheduler and cancel any pending
        // publications.  Note that, if any metrics are currently being
        // published, this operation will block until all of their
        // publications have completed.

    // MANIPULATORS
    void scheduleCategory(const char                *category,
                          const bsls::TimeInterval&  interval);
        // Schedule the specified null-terminated string 'category' to be
        // published periodically at the specified 'interval' using the
        // 'MetricManager' supplied at construction.  If 'category' has
        // *already* been scheduled, change the scheduled period to 'interval';
        // any previously scheduled publication of 'category' is either
        // canceled or completed (atomically) prior to rescheduling.  If a
        // category is rescheduled with the same 'interval' as it is currently
        // scheduled, this operation has no effect.  The behavior is undefined
        // unless 'bsls::TimeInterval(0, 0) < interval'.

    void scheduleCategory(const Category            *category,
                          const bsls::TimeInterval&  interval);
        // Schedule the specified 'category' to be published periodically at
        // the specified 'interval' using the 'MetricManager' supplied at
        // construction.  If 'category' has *already* been scheduled, change
        // the scheduled period to 'interval'; any previously scheduled
        // publication of 'category' is either canceled or completed
        // (atomically) prior to rescheduling.  If a category is rescheduled
        // with the same 'interval' as it is currently scheduled, this
        // operation has no effect.  The behavior is undefined unless
        // 'bsls::TimeInterval(0, 0) < interval' and 'category' is a valid
        // address supplied by the 'balm::MetricRegistry' owned by the
        // 'MetricsManager' object supplied at construction.

    void setDefaultSchedule(const bsls::TimeInterval& interval);
        // Set, to the specified 'interval', the default interval for metrics
        // to be periodically published using the 'MetricsManager' supplied at
        // construction.  This method schedules every metric category not given
        // a individual schedule (using 'scheduleCategory'), to be published
        // periodically until that category is either given an individual
        // schedule, or the default schedule is canceled (using either
        // 'clearDefaultSchedule' or 'cancelAllPublications').  If a default
        // publication has *already* been scheduled, change its schedule to
        // 'interval'; any previously scheduled publication is either canceled
        // or completed (atomically) before rescheduling.  If the default
        // publication is rescheduled with the same 'interval' as it is
        // currently scheduled, this operation has no effect.  The behavior is
        // undefined unless 'bsls::TimeInterval(0, 0) < interval'.  Note that,
        // to exclude a category from any publication, clients can disable the
        // category using the 'MetricsManager' object supplied at construction.

    int cancelCategorySchedule(const char *category);
        // Cancel the periodic publication of the specified null-terminated
        // string 'category'.  Return 0 on success, and a non-zero value if the
        // 'category' is not scheduled for publication.  Any scheduled
        // publication of 'category' is either canceled or completed before
        // this method returns.  Note that if a default publication schedule
        // has been set (using 'setDefaultSchedule'), then 'category' will
        // continue to be published as part of that scheduled default
        // publication; to exclude a category from any publication, clients
        // can disable the category using the 'MetricsManager' object supplied
        // at construction.

    int cancelCategorySchedule(const Category *category);
        // Cancel the periodic publication of the specified 'category'.  Return
        // 0 on success, and a non-zero value if the 'category' is not
        // scheduled for publication.  Any scheduled publication of 'category'
        // is either canceled or completed before this method returns.  The
        // behavior is undefined unless 'category' is a valid address supplied
        // by the 'balm::MetricRegistry' owned by 'metricsManager'.  Note that
        // if a default publication schedule has been set (using
        // 'setDefaultSchedule'), then 'category' will continue to be published
        // as part of that scheduled default publication; to exclude a category
        // from any publication, clients can disable the category using the
        // 'MetricsManager' object supplied at construction.

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
        // the 'scheduleCategory' method.  Any scheduled publication is either
        // canceled or completed before this method returns.

    MetricsManager *manager();
        // Return the address of the modifiable metrics manager for which this
        // publication scheduler publishes metrics.

    // ACCESSORS
    bool findCategorySchedule(bsls::TimeInterval  *result,
                              const char          *category) const;
        // Load into the specified 'result' the individual schedule interval
        // (set using the 'scheduleCategory' method) that corresponds to the
        // specified null-terminated string 'category', if found, and return
        // 'true', or (if not found) return 'false' with no effect.  This
        // method will return 'false' and will not modify 'result' if
        // 'category' is published as part of the default scheduled
        // publication.

    bool findCategorySchedule(bsls::TimeInterval  *result,
                              const Category      *category) const;
        // Load into the specified 'result' the individual schedule interval
        // (set using the 'scheduleCategory' method) that corresponds to the
        // specified 'category', if found, and return 'true', or (if not
        // found) return 'false' with no effect.  This method will return
        // 'false' and will not modify 'result' if 'category' is published as
        // part of the default scheduled publication.  The behavior is
        // undefined unless 'category' is a valid address supplied by the
        // 'balm::MetricRegistry' owned by the 'MetricsManager' object
        // supplied at construction.

    bool getDefaultSchedule(bsls::TimeInterval *result) const;
        // Load into the specified 'result' the default scheduled interval,
        // (set using the 'setDefaultSchedule' method), for periodically
        // publishing metrics, if found, and return 'true', or (if not found)
        // return 'false' with no effect.

    int getCategorySchedule(
                    bsl::vector<bsl::pair<const Category *,
                                          bsls::TimeInterval> > *result) const;
    int getCategorySchedule(
                    std::vector<std::pair<const Category *,
                                          bsls::TimeInterval> > *result) const;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    int getCategorySchedule(
                std::pmr::vector<std::pair<const Category *,
                                           bsls::TimeInterval>> *result) const;
#endif
        // Load into the specified 'result' a representation of the current
        // schedule for publishing categories being followed by this scheduler
        // and return the number of scheduled categories.  The schedule is
        // represented using a series of (category address, time interval)
        // pairs; each pair in the series indicates the periodic time interval
        // that the associated category will be published.  Note that the
        // 'result' of this operation contains only those categories scheduled
        // using the 'scheduleCategory' operation, and does *not* include
        // categories published as part of the default publication.

    const MetricsManager *manager() const;
        // Return the address of the non-modifiable metrics manager for which
        // this publication scheduler will publish metrics.

    bsl::ostream& print(bsl::ostream&   stream,
                        int             level = 0,
                        int             spacesPerLevel = 4) const;
        // Print a formatted string describing the current state of this
        // 'PublicationScheduler' object to the specified 'stream' at the
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

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class PublicationScheduler
                         // --------------------------

// MANIPULATORS
inline
MetricsManager *PublicationScheduler::manager()
{
    return d_manager_p;
}

inline
void PublicationScheduler::scheduleCategory(
                                           const char                *category,
                                           const bsls::TimeInterval&  interval)
{
    scheduleCategory(d_manager_p->metricRegistry().getCategory(category),
                     interval);
}

inline
int PublicationScheduler::cancelCategorySchedule(const char *category)
{
    return cancelCategorySchedule(
                          d_manager_p->metricRegistry().getCategory(category));
}

// ACCESSORS
inline
const MetricsManager *PublicationScheduler::manager() const
{
    return d_manager_p;
}

inline
bool PublicationScheduler::findCategorySchedule(
                                            bsls::TimeInterval *result,
                                            const char         *category) const
{
    return findCategorySchedule(
                          result,
                          d_manager_p->metricRegistry().getCategory(category));
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
