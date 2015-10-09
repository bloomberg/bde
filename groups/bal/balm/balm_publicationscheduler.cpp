// balm_publicationscheduler.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_publicationscheduler.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_publicationscheduler_cpp,"$Id$ $CSID$")

#include <balm_metricsmanager.h>

#include <bslmt_mutex.h>
#include <bslmt_lockguard.h>
#include <bdlmt_timereventscheduler.h>
#include <bdlf_bind.h>
#include <bdlt_currenttime.h>

#include <bsls_assert.h>

#include <bslma_default.h>
#include <bdlb_print.h>

#include <bsl_ostream.h>
#include <bsl_set.h>
#include <bsl_vector.h>
#include <bsl_cstring.h>
#include <bsl_algorithm.h>

namespace BloombergLP {

namespace balm {

namespace {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

inline
bsls::TimeInterval makeInvalidInterval()
    // Return the invalid scheduling interval value.  Note that this function
    // is provided to avoid creating a statically initialized constant.
{
    return bsls::TimeInterval(0, 0);
}

struct ScheduleElementSort {
   // A functor to sort schedule elements alphabetically by category name.

   typedef bsl::pair<const balm::Category *,
                     bsls::TimeInterval> ScheduleElement;

   bool operator()(const ScheduleElement& lhs, const ScheduleElement& rhs)
   {
       return bsl::strcmp(lhs.first->name(), rhs.first->name()) < 0;
   }
};

struct CategorySort {
    // A functor to sort categories alphabetically.

    bool operator()(const balm::Category *lhs, const balm::Category *rhs)
    {
            return bsl::strcmp(lhs->name(), rhs->name()) < 0;
    }
};

bsl::ostream& printCategorySet(
                            bsl::ostream&                           stream,
                            const bsl::set<const balm::Category *>& categories)
    // Print, to the specified 'stream' the specified 'categories' in
    // alphabetic order.
{
    bsl::vector<const balm::Category *> categoryList;
    categoryList.reserve(categories.size());

    bsl::set<const balm::Category *>::const_iterator it = categories.begin();
    for (; it != categories.end(); ++it) {
        categoryList.push_back(*it);
    }

    bsl::sort(categoryList.begin(), categoryList.end(), CategorySort());
    bsl::vector<const balm::Category *>::const_iterator clIt =
                                                          categoryList.begin();
    for (; clIt != categoryList.end(); ++clIt) {
        stream << " " << (*clIt)->name();
    }
    stream << " ";
    return stream;
}

}  // close unnamed namespace

                   // =====================================
                   // struct PublicationScheduler_ClockData
                   // =====================================

class PublicationScheduler_ClockData {
    // The 'PublicationScheduler_ClockData' class implements an unconstrained
    // pure-attribute class containing the data associated with a scheduled
    // publication interval.  Each "clock" created by the publication scheduler
    // with the 'bdlmt::TimerEventScheduler' is associated with a 'ClockData'
    // object describing the categories to publish when the clock event occurs.
    //  The 'handle' property holds a handle to the
    // 'bdlmt::TimerEventScheduler' "clock" that this 'ClockData' contains data
    // for.  The 'categories' property holds the set of categories scheduled at
    // the clock's publication interval.  The 'defaultClock' property
    // indicates whether the publication interval associated with this
    // 'ClockData' is the default publication period.  Finally, if
    // 'defaultClock' is 'true', then the 'nonDefaultCategories' property holds
    // the set of categories that are *not* published as part of the default
    // publication; otherwise (if 'defaultSchedule' is 'false') the meaning of
    // 'nonDefaultCategories' is undefined.  Note that a shared pointer to a
    // 'ClockData' object is bound (by 'PublicationScheduler'), with the
    // 'PublicationScheduler::publish' method, into the 'bsl::function' object
    // supplied to 'bdlmt::TimerEventScheduler::startClock'.

    // DATA
    bslmt::Mutex                d_mutex;       // synchronize access to data

    bdlmt::TimerEventScheduler::Handle
                               d_handle;      // handle to the associated clock

    bsl::set<const Category *>
                               d_categories;  // set of categories published

    bool                       d_default;     // whether this is the default
                                              // scheduled publication

    bsl::set<const Category *>
                               d_nonDefaultCategories;
                                              // if 'd_default' is 'true', the
                                              // set of categories that should
                                              // not be published as part of
                                              // the default publication

    // NOT IMPLEMENTED
    PublicationScheduler_ClockData(const PublicationScheduler_ClockData &);
    PublicationScheduler_ClockData& operator=(
                                        const PublicationScheduler_ClockData&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(PublicationScheduler_ClockData,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit PublicationScheduler_ClockData(
                                             bslma::Allocator *basicAllocator);
        // Create a 'ClockData' object.  By default 'handle()' is invalid,
        // 'categories()' and 'nonDefaultCategories()' are empty, and
        // 'defaultClock()' is 'false'.

    ~PublicationScheduler_ClockData();
        // Destroy this 'ClockData' object.

    // MANIPULATORS
    bslmt::Mutex *mutex();
        // Return the address of the modifiable 'bslmt::Mutex' used to
        // synchronize access to the properties of this object.  Note that
        // access to 'handle()' does not need to be synchronized because
        // 'handle()' is only modified or accessed by the
        // 'PublicationScheduler' object's manipulators (which are themselves
        // synchronized by a mutex).

    bdlmt::TimerEventScheduler::Handle& handle();
        // Return a reference to the modifiable
        // 'bdlmt::TimerEventScheduler::Handle' object used to access the
        // 'bdlmt::TimerEventScheduler' "clock" this 'ClockData' is associated
        // with.

    bsl::set<const Category *>& categories();
        // Return a reference to the modifiable set of categories scheduled at
        // the clock's publication interval.

    bool& defaultClock();
        // Return a reference to the boolean indicating whether the clock's
        // publication interval is the default publication interval.

    bsl::set<const Category *>& nonDefaultCategories();
        // Return a reference to the modifiable set of categories that, if
        // 'defaultClock()' is 'true', should *not* be published as part of the
        // default publication.  If 'defaultClock()' is false, the meaning of
        // this property is undefined and the value must be an empty set.
};

                   // -------------------------------------
                   // struct PublicationScheduler_ClockData
                   // -------------------------------------

// CREATORS
PublicationScheduler_ClockData::PublicationScheduler_ClockData(
                                              bslma::Allocator *basicAllocator)
: d_handle(bdlmt::TimerEventScheduler::e_INVALID_HANDLE)
, d_categories(basicAllocator)
, d_default(false)
, d_nonDefaultCategories(basicAllocator)
{
}

PublicationScheduler_ClockData::~PublicationScheduler_ClockData()
{
}

// MANIPULATORS
inline
bslmt::Mutex *PublicationScheduler_ClockData::mutex()
{
    return &d_mutex;
}

inline
bdlmt::TimerEventScheduler::Handle& PublicationScheduler_ClockData::handle()
{
    return d_handle;
}

inline
bsl::set<const Category *>&
PublicationScheduler_ClockData::categories()
{
    return d_categories;
}

inline
bool& PublicationScheduler_ClockData::defaultClock()
{
    return d_default;
}

inline
bsl::set<const Category *>&
PublicationScheduler_ClockData::nonDefaultCategories()
{
    return d_nonDefaultCategories;
}

                    // ===================================
                    // struct PublicationScheduler_Proctor
                    // ===================================

class PublicationScheduler_Proctor {
   // This class implements a proctor that, unless 'release()' is called, sets
   // the 'PublicationScheduler' object supplied at construction to its default
   // state.  On construction a proctor object is provided the address of a
   // 'PublicationScheduler' object, on destruction, if 'release()' has not
   // been called, the proctor will clear all of the scheduler's internal
   // state and cancel any managed clocks with the underlying
   // 'bdlmt::TimerEventScheduler'.   If 'release()' is called on a proctor
   // object, then the proctor object's destructor will have no effect.  Note
   // that the 'PublicationScheduler_Proctor' class is a friend of
   // 'PublicationScheduler' and has access to a scheduler's private data
   // members.

   // DATA
   PublicationScheduler *d_scheduler_p;  // managed scheduler (held, not
                                              // owned)

   // NOT IMPLEMENTED
   PublicationScheduler_Proctor(const PublicationScheduler_Proctor&);
   PublicationScheduler_Proctor& operator=(
                                     const PublicationScheduler_Proctor&);

  public:

   // CREATORS
   PublicationScheduler_Proctor(PublicationScheduler *scheduler);
       // Create a proctor object that, unless 'release()' is called, will, on
       // destruction, set the specified 'scheduler' to its default state and
       // cancel any timer events managed by 'scheduler' with the underlying
       // 'bdlmt::TimerEventScheduler' object.

   ~PublicationScheduler_Proctor();
       // Unless 'release()' has been called, clear all the internal state
       // variables of 'PublicationScheduler' object supplied at construction
       // and cancel any managed clocks with the underlying
       // 'bdlmt::TimerEventScheduler'.

   // MANIPULATORS
   void release();
       // Release from management the 'PublicationScheduler' object supplied
       // at construction.  After invoking this method, this object's
       // destructor will have no effect.
};

                    // -----------------------------------
                    // struct PublicationScheduler_Proctor
                    // -----------------------------------

// CREATORS
inline
PublicationScheduler_Proctor::PublicationScheduler_Proctor(
                                          PublicationScheduler *scheduler)
: d_scheduler_p(scheduler)
{
}

PublicationScheduler_Proctor::~PublicationScheduler_Proctor()
{
    if (d_scheduler_p) {
        PublicationScheduler::Clocks::iterator it =
                                              d_scheduler_p->d_clocks.begin();
        for (; it != d_scheduler_p->d_clocks.end(); ++it) {
            // Note that a clock may not have been scheduled (and therefore
            // have an invalid handle) if an exception occurred between the
            // 'ClockData' objects creation (and insertion into 'd_clocks')
            // and the call to 'bdlmt::TimerEventScheduler::startClock'.
            if (bdlmt::TimerEventScheduler::e_INVALID_HANDLE !=
                it->second->handle()) {
                d_scheduler_p->d_scheduler_p->cancelClock(
                                                  it->second->handle(), true);
            }
        }
        d_scheduler_p->d_defaultInterval = makeInvalidInterval();
        d_scheduler_p->d_clocks.clear();
        d_scheduler_p->d_categories.clear();
    }
}

// MANIPULATORS
inline
void PublicationScheduler_Proctor::release()
{
    d_scheduler_p = 0;
}

                         // --------------------------
                         // class PublicationScheduler
                         // --------------------------

// PRIVATE MANIPULATORS
void PublicationScheduler::publish(bsl::shared_ptr<ClockData> clockData)
{
    // This method publishes, to the contained 'MetricsManager' object, the
    // categories associated with the specified 'clockData'.  If
    // 'clockData->defaultClock()' is 'true', this operation will publish all
    // metric categories, excluding 'clockData-nonDefaultCategories()', using
    // the 'd_metricsManager' object's 'publishAll' operation; otherwise (if
    // 'clockData->defaultClock()' is 'false') this operation will publish
    // 'clockData->categories()'.

    bslmt::LockGuard<bslmt::Mutex> guard(clockData->mutex());
    if (clockData->defaultClock()) {
        d_manager_p->publishAll(clockData->nonDefaultCategories());
    }
    else if (!clockData->categories().empty()) {
        d_manager_p->publish(clockData->categories());
    }
}

void PublicationScheduler::cancelCategory(
                                         Categories::iterator categoryIterator)
{
    // This method erases 'categoryIt' from the map of categories
    // 'd_categories' and then removes the category from the associated
    // 'PublicationScheduler_ClockData' object held in 'd_clocks'.

    BSLS_ASSERT(categoryIterator != d_categories.end());
    Clocks::iterator clockIterator = d_clocks.find(categoryIterator->second);
    BSLS_ASSERT(clockIterator != d_clocks.end());

    const Category *category = categoryIterator->first;
    d_categories.erase(categoryIterator);
    bsl::shared_ptr<ClockData> clock = clockIterator->second;
    {
        bslmt::LockGuard<bslmt::Mutex> guard(clock->mutex());
        BSLS_ASSERT(clock->categories().end() !=
                       clock->categories().find(category));
        clock->categories().erase(category);
    }

    if (!clock->defaultClock()) {
        // If the 'ClockData' object, 'clock', associated with the canceled
        // category is not the default schedule's clock and it is now empty,
        // cancel the clock with the underlying 'bdlmt::TimerEventScheduler'.
        if (clock->categories().empty()) {
            d_scheduler_p->cancelClock(clock->handle());
            d_clocks.erase(clockIterator);
        }

        // Additionally, if 'clock' was not the default schedule's clock, and
        // there is a default schedule, remove the canceled category from the
        // default schedule clock's list of non-default categories (indicating
        // the category should now be published as part of the default
        // publication).
        if (d_defaultInterval != makeInvalidInterval()) {
            Clocks::iterator dfltIt = d_clocks.find(d_defaultInterval);
            BSLS_ASSERT(d_clocks.end() != dfltIt);

            bsl::shared_ptr<ClockData>& defaultClock = dfltIt->second;
            bslmt::LockGuard<bslmt::Mutex> guard(defaultClock->mutex());
            defaultClock->nonDefaultCategories().erase(category);
        }
    }
}

int PublicationScheduler::cancelDefaultSchedule()
{
    // Set the default interval data member 'd_defaultInterval' to the invalid
    // interval value, and update the associated
    // 'PublicationScheduler_ClockData' object to reflect that change.

    if (d_defaultInterval == makeInvalidInterval()) {
        return -1;                                                    // RETURN
    }

    bsls::TimeInterval interval = d_defaultInterval;
    d_defaultInterval = makeInvalidInterval();

    Clocks::iterator clockIt = d_clocks.find(interval);
    BSLS_ASSERT(clockIt != d_clocks.end());
    bsl::shared_ptr<ClockData> clock = clockIt->second;

    // If the default intervals 'ClockData' has no associated categories,
    // cancel the clock with the underlying 'bdlmt::TimerEventScheduler',
    // otherwise (if there are categories scheduled at the same interval) mark
    // the 'ClockData' as not being the default schedule's clock.
    if (clock->categories().empty()) {
        BSLS_ASSERT(clock->handle() !=
                       bdlmt::TimerEventScheduler::e_INVALID_HANDLE);
        d_scheduler_p->cancelClock(clock->handle());
        d_clocks.erase(clockIt);
    }
    else {
        bslmt::LockGuard<bslmt::Mutex> guard(clock->mutex());
        clock->defaultClock() = false;
        clock->nonDefaultCategories().clear();
    }
    return 0;
}

// CREATORS
PublicationScheduler::PublicationScheduler(
                                    MetricsManager             *metricsManager,
                                    bdlmt::TimerEventScheduler *eventScheduler,
                                    bslma::Allocator           *basicAllocator)
: d_scheduler_p(eventScheduler)
, d_manager_p(metricsManager)
, d_categories(basicAllocator)
, d_clocks(basicAllocator)
, d_defaultInterval(makeInvalidInterval())
, d_mutex()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

PublicationScheduler::~PublicationScheduler()
{
    cancelAll();
}

// MANIPULATORS
void PublicationScheduler::scheduleCategory(
                                           const Category            *category,
                                           const bsls::TimeInterval&  interval)
{
    // This method locks the data members of this object; cancels the existing
    // schedule for the specified 'category' (if it has been previously
    // scheduled); then updates 'd_clocks' and 'd_categories' to reflect the
    // indicated schedule, creating and scheduling a new
    // 'PublicationScheduler_ClockData' object if one does not exist for
    // 'interval'.

    BSLS_ASSERT(bsls::TimeInterval(0, 0) < interval);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    Categories::iterator catIt = d_categories.find(category);
    if (catIt != d_categories.end()) {
        // If the 'category' is being rescheduled with the same frequency as
        // it's currently scheduled, return immediately; otherwise, cancel the
        // existing schedule.
        if (catIt->second == interval) {
            return;                                                   // RETURN
        }
        cancelCategory(catIt);
    }

    // Use 'proctor' to ensure that 'this' object is returned to a valid
    // (default) state if an exception occurs performing this operation.
    PublicationScheduler_Proctor proctor(this);

    d_categories.insert(bsl::make_pair(category, interval));
    Clocks::iterator clockIt = d_clocks.find(interval);
    bsl::shared_ptr<ClockData> clock;

    // Create a new 'ClockData' object if one does not exist for the
    // 'interval', otherwise update the existing 'clock'.
    if (clockIt == d_clocks.end()) {
        clock.createInplace(d_allocator_p, d_allocator_p);
        clock->categories().insert(category);
        d_clocks.insert(bsl::make_pair(interval, clock));
        bslmt::LockGuard<bslmt::Mutex> guard(clock->mutex());
        clock->handle() = d_scheduler_p->startClock(
                          interval,
                          bdlf::BindUtil::bind(&PublicationScheduler::publish,
                                               this,
                                               clock));
    }
    else {
        clock = clockIt->second;
        bslmt::LockGuard<bslmt::Mutex> guard(clock->mutex());
        clock->categories().insert(category);
    }

    // If there is a default schedule and it's not 'interval', then add
    // 'category' to the set of non-default categories maintained by the
    // default schedule's clock (indicating that 'category' should no longer be
    // published as part of the default publication).
    if (!clock->defaultClock() && d_defaultInterval != makeInvalidInterval()) {
        Clocks::iterator dfltIt = d_clocks.find(d_defaultInterval);
        BSLS_ASSERT(d_clocks.end() != dfltIt);

        bsl::shared_ptr<ClockData>& defaultClock = dfltIt->second;
        bslmt::LockGuard<bslmt::Mutex> guard(defaultClock->mutex());
        defaultClock->nonDefaultCategories().insert(category);
    }

    proctor.release();
}

void PublicationScheduler::setDefaultSchedule(
                                            const bsls::TimeInterval& interval)
{
    // This method locks the data members of this object; cancels the existing
    // default schedule (if it has been previously set); then updates
    // 'd_clocks' and to reflect the indicated schedule, creating and
    // scheduling a new 'PublicationScheduler_ClockData' object if one does not
    // exist for 'interval'.

    BSLS_ASSERT(bsls::TimeInterval(0, 0) < interval);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    // If 'interval' equals the existing default schedule interval, return
    // immediately.
    if (interval == d_defaultInterval) {
        return;                                                       // RETURN
    }

    cancelDefaultSchedule();
    d_defaultInterval = interval;

    // Use 'proctor' to ensure that 'this' object is returned to a valid
    // (default) state if an exception occurs performing this operation.
    PublicationScheduler_Proctor proctor(this);

    // Create a new 'ClockData' object if one does not exist for the
    // 'interval', otherwise update the existing 'clock'.
    bsl::shared_ptr<ClockData> clock;
    Clocks::iterator clockIt = d_clocks.find(interval);
    if (clockIt == d_clocks.end()) {
        clock.createInplace(d_allocator_p, d_allocator_p);
        d_clocks.insert(bsl::make_pair(interval, clock));
    }
    else {
        clock = clockIt->second;
    }

    bslmt::LockGuard<bslmt::Mutex> clockGuard(clock->mutex());
    clock->defaultClock() = true;

    // Iterate over the map of scheduled categories 'd_categories', adding any
    // category *not* scheduled at the (new) default schedule interval to the
    // set of non default categories (indicating that those categories should
    // not be published as part of the default publication).
    Categories::iterator cIt = d_categories.begin();
    for (;cIt != d_categories.end(); ++cIt) {
        if (cIt->second != interval) {
            clock->nonDefaultCategories().insert(cIt->first);
        }
    }

    // If this is a new 'ClockData' (i.e., its handle is invalid), schedule it
    // with the underlying 'bdlmt::TimerEventScheduler' object.
    if (clock->handle() == bdlmt::TimerEventScheduler::e_INVALID_HANDLE) {
        clock->handle() = d_scheduler_p->startClock(
            interval,
            bdlf::BindUtil::bind(&PublicationScheduler::publish,
                                 this,
                                 clock));
    }

    proctor.release();
}

int PublicationScheduler::cancelCategorySchedule(const Category *category)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    Categories::iterator cIt = d_categories.find(category);
    if (cIt == d_categories.end()) {
        // This category has no specific schedule.
        return -1;                                                    // RETURN
    }
    cancelCategory(cIt);

    return 0;
}

void PublicationScheduler::cancelAll()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    Clocks::iterator it = d_clocks.begin();
    for (; it != d_clocks.end(); ++it) {
        d_scheduler_p->cancelClock(it->second->handle(), true);
    }
    d_defaultInterval = makeInvalidInterval();
    d_clocks.clear();
    d_categories.clear();
}

int PublicationScheduler::clearDefaultSchedule()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return cancelDefaultSchedule();
}

// ACCESSORS
bool
PublicationScheduler::findCategorySchedule(bsls::TimeInterval *result,
                                           const Category     *category) const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    Categories::const_iterator catIt = d_categories.find(category);
    if (catIt == d_categories.end()) {
        return false;                                                 // RETURN
    }
    *result = catIt->second;
    return true;
}

bool
PublicationScheduler::getDefaultSchedule(bsls::TimeInterval *result) const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    if (d_defaultInterval == makeInvalidInterval()) {
        return false;                                                 // RETURN
    }
    *result = d_defaultInterval;
    return true;
}

int PublicationScheduler::getCategorySchedule(
                     bsl::vector<bsl::pair<const Category *,
                                           bsls::TimeInterval> > *result) const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    result->clear();
    result->reserve(d_categories.size());
    Categories::const_iterator catIt = d_categories.begin();
    for (; catIt != d_categories.end(); ++catIt) {
        result->push_back(bsl::make_pair(catIt->first, catIt->second));
    }
    return d_categories.size();
}

bsl::ostream&
PublicationScheduler::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
    // We must sort the various sets of categories alphabetically to ensure
    // that the resulting formatted text is consistent (for testing).

    typedef bsl::pair<const Category *,bsls::TimeInterval> ScheduleElement;
    typedef bsl::vector<ScheduleElement>                       Schedule;

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    Schedule schedule;
    Categories::const_iterator catIt = d_categories.begin();
    for (; catIt != d_categories.end(); ++catIt) {
        schedule.push_back(bsl::make_pair(catIt->first, catIt->second));
    }
    bsl::sort(schedule.begin(), schedule.end(), ScheduleElementSort());

    char SEP = (level <= 0) ? ' ' : '\n';
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << "[" << SEP;
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "default interval: ";
    if (d_defaultInterval != makeInvalidInterval()) {
        stream << d_defaultInterval << SEP;
    }
    else {
        stream << "none" << SEP;
    }

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "scheduled categories: [ " << SEP;
    Schedule::const_iterator sIt = schedule.begin();
    for (; sIt != schedule.end(); ++sIt) {
        bdlb::Print::indent(stream, level + 2, spacesPerLevel);
        stream << "[ " << sIt->first->name() << " -> "
               << sIt->second << " ]" << SEP;
    }
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << SEP;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "clock information: [ " << SEP;
    Clocks::const_iterator clockIt = d_clocks.begin();
    for (; clockIt != d_clocks.end(); ++clockIt) {
        bdlb::Print::indent(stream, level + 2, spacesPerLevel);
        stream << "[" << SEP;
        const bsls::TimeInterval&          interval   = clockIt->first;
        const bsl::shared_ptr<ClockData>& clock      = clockIt->second;
        bdlb::Print::indent(stream, level + 3, spacesPerLevel);
        stream << "interval: " << interval
               << "   default: " << clock->defaultClock()
               << "   handle: " << clock->handle() << SEP;

        bsl::set<const Category *>::const_iterator ctIt =
                                                  clock->categories().begin();

        bdlb::Print::indent(stream, level + 3, spacesPerLevel);
        stream << "categories: [";
        printCategorySet(stream, clock->categories());
        stream << "] " << SEP;

        bdlb::Print::indent(stream, level + 3, spacesPerLevel);
        stream << "excluded categories: [";
        printCategorySet(stream, clock->nonDefaultCategories());
        stream << "] " << SEP;

        bdlb::Print::indent(stream, level + 2, spacesPerLevel);
        stream << "]" << SEP;
    }
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << SEP;
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << SEP;

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

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
