// baem_publicationscheduler.cpp      -*-C++-*-
#include <baem_publicationscheduler.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_publicationscheduler_cpp,"$Id$ $CSID$")

#include <baem_metricsmanager.h>

#include <bcep_timereventscheduler.h>
#include <bcemt_thread.h>
#include <bdef_bind.h>
#include <bdetu_systemtime.h>

#include <bsl_set.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace {

inline
bdet_TimeInterval INVALID_INTERVAL()
    // Return the invalid scheduling interval value.  Note that this function
    // is provided to avoid creating a statically initialized constant.
{
    return bdet_TimeInterval(0, 0);
}

struct ScheduleElementSort {
   // A functor to sort schedule elements alphabetically by category name.

   typedef bsl::pair<const baem_Category *,
                     bdet_TimeInterval> ScheduleElement;

   bool operator()(const ScheduleElement& lhs, const ScheduleElement& rhs)
   {
       return bsl::strcmp(lhs.first->name(), rhs.first->name()) < 0;
   }
};

struct CategorySort {
    // A functor to sort categories alphabetically.

    bool operator()(const baem_Category *lhs, const baem_Category *rhs)
    {
            return bsl::strcmp(lhs->name(), rhs->name()) < 0;
    }
};

bsl::ostream& printCategorySet(
                             bsl::ostream&                          stream,
                             const bsl::set<const baem_Category *>& categories)
    // Print, to the specified 'stream' the specified 'categories' in
    // alphabetic order.
{
    bsl::vector<const baem_Category *> categoryList;
    categoryList.reserve(categories.size());

    bsl::set<const baem_Category *>::const_iterator it = categories.begin();
    for (; it != categories.end(); ++it) {
        categoryList.push_back(*it);
    }

    bsl::sort(categoryList.begin(), categoryList.end(), CategorySort());
    bsl::vector<const baem_Category *>::const_iterator clIt =
                                                         categoryList.begin();
    for (; clIt != categoryList.end(); ++clIt) {
        stream << " " << (*clIt)->name();
    }
    stream << " ";
    return stream;
}

} // close unnamed namespace

               // ==========================================
               // struct baem_PublicationScheduler_ClockData
               // ==========================================

class baem_PublicationScheduler_ClockData {
    // The 'baem_PublicationScheduler_ClockData' class implements an
    // unconstrained pure-attribute class containing the data associated
    // with a scheduled publication interval.  Each "clock" created by the
    // publication scheduler with the 'bcep_TimerEventScheduler' is associated
    // with a 'ClockData' object describing the categories to publish when the
    // clock event occurs.  The 'handle' property holds a handle to the
    // 'bcep_TimerEventScheduler' "clock" that this 'ClockData' contains data
    // for.  The 'categories' property holds the set of categories scheduled
    // at the clock's publication interval.  The 'defaultClock' property
    // indicates whether the publication interval associated with this
    // 'ClockData' is the default publication period.  Finally, if
    // 'defaultClock' is 'true', then the 'nonDefaultCategories' property
    // holds the set of categories that are *not* published as part of the
    // default publication; otherwise (if 'defaultSchedule' is 'false') the
    // meaning of 'nonDefaultCategories' is undefined.  Note that a shared
    // pointer to a 'ClockData' object is bound (by
    // 'baem_PublicationScheduler'), with the
    // 'baem_PublicationScheduler::publish' method, into the 'bdef_Function'
    // object supplied to 'bcep_TimerEventScheduler::startClock'.

    // DATA
    bcemt_Mutex                d_mutex;       // synchronize access to data

    bcep_TimerEventScheduler::Handle
                               d_handle;      // handle to the associated clock

    bsl::set<const baem_Category *>
                               d_categories;  // set of categories published

    bool                       d_default;     // whether this is the default
                                              // scheduled publication

    bsl::set<const baem_Category *>
                               d_nonDefaultCategories;
                                              // if 'd_default' is 'true', the
                                              // set of categories that should
                                              // not be published as part of
                                              // the default publication

    // NOT IMPLEMENTED
    baem_PublicationScheduler_ClockData(
                                  const baem_PublicationScheduler_ClockData& );
    baem_PublicationScheduler_ClockData& operator=(
                                  const baem_PublicationScheduler_ClockData& );

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_PublicationScheduler_ClockData,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit baem_PublicationScheduler_ClockData(
                                             bslma_Allocator *basicAllocator);
        // Create a 'ClockData' object.  By default 'handle()' is invalid,
        // 'categories()' and 'nonDefaultCategories()' are empty, and
        // 'defaultClock()' is 'false'.

    ~baem_PublicationScheduler_ClockData();
        // Destroy this 'ClockData' object.

    // MANIPULATORS
    bcemt_Mutex *mutex();
        // Return the address of the modifiable 'bcemt_Mutex' used to
        // synchronize access to the properties of this object.  Note that
        // access to 'handle()' does not need to be synchronized because
        // 'handle()' is only modified or accessed by the
        // 'baem_PublicationScheduler' object's manipulators (which are
        // themselves synchronized by a mutex).

    bcep_TimerEventScheduler::Handle& handle();
        // Return a reference to the modifiable
        // 'bcep_TimerEventScheduler::Handle' object used to access the
        // 'bcep_TimerEventScheduler' "clock" this 'ClockData' is associated
        // with.

    bsl::set<const baem_Category *>& categories();
        // Return a reference to the modifiable set of categories scheduled at
        // the clock's publication interval.

    bool& defaultClock();
        // Return a reference to the boolean indicating whether the clock's
        // publication interval is the default publication interval.

    bsl::set<const baem_Category *>& nonDefaultCategories();
        // Return a reference to the modifiable set of categories that, if
        // 'defaultClock()' is 'true', should *not* be published as part of the
        // default publication.  If 'defaultClock()' is false, the meaning of
        // this property is undefined and the value must be an empty set.
};

               // ------------------------------------------
               // struct baem_PublicationScheduler_ClockData
               // ------------------------------------------

// CREATORS
baem_PublicationScheduler_ClockData::baem_PublicationScheduler_ClockData(
                                              bslma_Allocator *basicAllocator)
: d_handle(bcep_TimerEventScheduler::BCEP_INVALID_HANDLE)
, d_categories(basicAllocator)
, d_default(false)
, d_nonDefaultCategories(basicAllocator)
{
}

baem_PublicationScheduler_ClockData::~baem_PublicationScheduler_ClockData()
{
}

// MANIPULATORS
inline
bcemt_Mutex *baem_PublicationScheduler_ClockData::mutex()
{
    return &d_mutex;
}

inline
bcep_TimerEventScheduler::Handle& baem_PublicationScheduler_ClockData::handle()
{
    return d_handle;
}

inline
bsl::set<const baem_Category *>&
baem_PublicationScheduler_ClockData::categories()
{
    return d_categories;
}

inline
bool& baem_PublicationScheduler_ClockData::defaultClock()
{
    return d_default;
}

inline
bsl::set<const baem_Category *>&
baem_PublicationScheduler_ClockData::nonDefaultCategories()
{
    return d_nonDefaultCategories;
}

               // ========================================
               // struct baem_PublicationScheduler_Proctor
               // ========================================

class baem_PublicationScheduler_Proctor {
   // This class implements a proctor that, unless 'release()' is called,
   // sets the 'baem_PublicationScheduler' object supplied at construction
   // to its default state.  On construction a proctor object is provided the
   // address of a 'baem_PublicationScheduler' object, on destruction, if
   // 'release()' has not been called, the proctor will clear all of the
   // scheduler's internal state and cancel any managed clocks with the
   // underlying 'bcep_TimerEventScheduler'.   If 'release()' is called on a
   // proctor object, then the proctor object's destructor will have no
   // effect.  Note that the 'baem_PublicationScheduler_Proctor' class is a
   // friend of 'baem_PublicationScheduler' and has access to a scheduler's
   // private data members.

   // DATA
   baem_PublicationScheduler *d_scheduler_p;  // managed scheduler (held, not
                                              // owned)

   // NOT IMPLEMENTED
   baem_PublicationScheduler_Proctor(const baem_PublicationScheduler_Proctor&);
   baem_PublicationScheduler_Proctor& operator=(
                                     const baem_PublicationScheduler_Proctor&);

  public:

   // CREATORS
   baem_PublicationScheduler_Proctor(baem_PublicationScheduler *scheduler);
        // Create a proctor object that, unless 'release()' is called, will,
        // on destruction, set the specified 'scheduler' to its default state
        // and cancel any timer events managed by 'scheduler' with the
        // underlying 'bcep_TimerEventScheduler' object.

   ~baem_PublicationScheduler_Proctor();
        // Unless 'release()' has been called, clear all the internal state
        // variables of 'baem_PublicationScheduler' object supplied at
        // construction and cancel any managed clocks with the underlying
        // 'bcep_TimerEventScheduler'.

   // MANIPULATORS
   void release();
        // Release from management the 'baem_PublicationScheduler' object
        // supplied at construction.  After invoking this method, this
        // object's destructor will have no effect.
};

               // ----------------------------------------
               // struct baem_PublicationScheduler_Proctor
               // ----------------------------------------

// CREATORS
inline
baem_PublicationScheduler_Proctor::baem_PublicationScheduler_Proctor(
                                          baem_PublicationScheduler *scheduler)
: d_scheduler_p(scheduler)
{
}

baem_PublicationScheduler_Proctor::~baem_PublicationScheduler_Proctor()
{
    if (d_scheduler_p) {
        baem_PublicationScheduler::Clocks::iterator it =
                                              d_scheduler_p->d_clocks.begin();
        for (; it != d_scheduler_p->d_clocks.end(); ++it) {
            // Note that a clock may not have been scheduled (and therefore
            // have an invalid handle) if an exception occurred between the
            // 'ClockData' objects creation (and insertion into 'd_clocks')
            // and the call to 'bcep_TimerEventScheduler::startClock'.
            if (bcep_TimerEventScheduler::BCEP_INVALID_HANDLE !=
                it->second->handle()) {
                d_scheduler_p->d_scheduler_p->cancelClock(
                                                  it->second->handle(), true);
            }
        }
        d_scheduler_p->d_defaultInterval = INVALID_INTERVAL();
        d_scheduler_p->d_clocks.clear();
        d_scheduler_p->d_categories.clear();
    }
}

// MANIPULATORS
inline
void baem_PublicationScheduler_Proctor::release()
{
    d_scheduler_p = 0;
}

                   // -------------------------------
                   // class baem_PublicationScheduler
                   // -------------------------------

// PRIVATE MANIPULATORS
void baem_PublicationScheduler::publish(bcema_SharedPtr<ClockData> clockData)
{
    // This method publishes, to the contained 'baem_MetricsManager' object,
    // the categories associated with the specified 'clockData'.  If
    // 'clockData->defaultClock()' is 'true', this operation will publish all
    // metric categories, excluding 'clockData-nonDefaultCategories()',
    // using the 'd_metricsManager' object's 'publishAll' operation; otherwise
    // (if 'clockData->defaultClock()' is 'false') this operation will publish
    // 'clockData->categories()'.

    bcemt_LockGuard<bcemt_Mutex> guard(clockData->mutex());
    if (clockData->defaultClock()) {
        d_manager_p->publishAll(clockData->nonDefaultCategories());
    }
    else if (!clockData->categories().empty()) {
        d_manager_p->publish(clockData->categories());
    }
}

void baem_PublicationScheduler::cancelCategory(Categories::iterator categoryIt)
{
    // This method erases 'categoryIt' from the map of categories
    // 'd_categories' and then removes the category from the associated
    // 'baem_PublicationScheduler_ClockData' object held in 'd_clocks'.

    BSLS_ASSERT(categoryIt != d_categories.end());
    Clocks::iterator clockIt = d_clocks.find(categoryIt->second);
    BSLS_ASSERT(clockIt != d_clocks.end());

    const baem_Category *category = categoryIt->first;
    d_categories.erase(categoryIt);
    bcema_SharedPtr<ClockData> clock = clockIt->second;
    {
        bcemt_LockGuard<bcemt_Mutex> guard(clock->mutex());
        BSLS_ASSERT(clock->categories().end() !=
                       clock->categories().find(category));
        clock->categories().erase(category);
    }

    if (!clock->defaultClock()) {
        // If the 'ClockData' object, 'clock', associated with the canceled
        // category is not the default schedule's clock and it is now empty,
        // cancel the clock with the underlying 'bcep_TimerEventScheduler'.
        if (clock->categories().empty()) {
            d_scheduler_p->cancelClock(clock->handle());
            d_clocks.erase(clockIt);
        }

        // Additionally, if 'clock' was not the default schedule's clock, and
        // there is a default schedule, remove the canceled category from the
        // default schedule clock's list of non-default categories (indicating
        // the category should now be published as part of the default
        // publication).
        if (d_defaultInterval != INVALID_INTERVAL()) {
            Clocks::iterator dfltIt = d_clocks.find(d_defaultInterval);
            BSLS_ASSERT(d_clocks.end() != dfltIt);

            bcema_SharedPtr<ClockData>& defaultClock = dfltIt->second;
            bcemt_LockGuard<bcemt_Mutex> guard(defaultClock->mutex());
            defaultClock->nonDefaultCategories().erase(category);
        }
    }
}

int baem_PublicationScheduler::cancelDefaultSchedule()
{
    // Set the default interval data member 'd_defaultInterval' to the invalid
    // interval value, and update the associated
    // 'baem_PublicationScheduler_ClockData' object to reflect that change.

    if (d_defaultInterval == INVALID_INTERVAL()) {
        return -1;
    }

    bdet_TimeInterval interval = d_defaultInterval;
    d_defaultInterval = INVALID_INTERVAL();

    Clocks::iterator clockIt = d_clocks.find(interval);
    BSLS_ASSERT(clockIt != d_clocks.end());
    bcema_SharedPtr<ClockData> clock = clockIt->second;

    // If the default intervals 'ClockData' has no associated categories,
    // cancel the clock with the underlying 'bcep_TimerEventScheduler',
    // otherwise (if there are categories scheduled at the same interval) mark
    // the 'ClockData' as not being the default schedule's clock.
    if (clock->categories().empty()) {
        BSLS_ASSERT(clock->handle() !=
                       bcep_TimerEventScheduler::BCEP_INVALID_HANDLE);
        d_scheduler_p->cancelClock(clock->handle());
        d_clocks.erase(clockIt);
    }
    else {
        bcemt_LockGuard<bcemt_Mutex> guard(clock->mutex());
        clock->defaultClock() = false;
        clock->nonDefaultCategories().clear();
    }
    return 0;
}

// CREATORS
baem_PublicationScheduler::baem_PublicationScheduler(
                                    baem_MetricsManager      *metricsManager,
                                    bcep_TimerEventScheduler *eventScheduler,
                                    bslma_Allocator          *basicAllocator)
: d_scheduler_p(eventScheduler)
, d_manager_p(metricsManager)
, d_categories(basicAllocator)
, d_clocks(basicAllocator)
, d_defaultInterval(INVALID_INTERVAL())
, d_mutex()
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

baem_PublicationScheduler::~baem_PublicationScheduler()
{
    cancelAll();
}

// MANIPULATORS
void baem_PublicationScheduler::scheduleCategory(
                                           const baem_Category      *category,
                                           const bdet_TimeInterval&  interval)
{
    // This method locks the data members of this object; cancels the existing
    // schedule for the specified 'category' (if it has been previously
    // scheduled); then updates 'd_clocks' and 'd_categories' to reflect
    // the indicated schedule, creating and scheduling a new
    // 'baem_PublicationScheduler_ClockData' object if one does not exist for
    // 'interval'.

    BSLS_ASSERT(bdet_TimeInterval(0, 0) < interval);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

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
    baem_PublicationScheduler_Proctor proctor(this);

    d_categories.insert(bsl::make_pair(category, interval));
    Clocks::iterator clockIt = d_clocks.find(interval);
    bcema_SharedPtr<ClockData> clock;

    // Create a new 'ClockData' object if one does not exist for the
    // 'interval', otherwise update the existing 'clock'.
    if (clockIt == d_clocks.end()) {
        clock.createInplace(d_allocator_p, d_allocator_p);
        clock->categories().insert(category);
        d_clocks.insert(bsl::make_pair(interval, clock)).second;
        bcemt_LockGuard<bcemt_Mutex> guard(clock->mutex());
        clock->handle() = d_scheduler_p->startClock(
                     interval,
                     bdef_BindUtil::bindA(d_allocator_p,
                                          &baem_PublicationScheduler::publish,
                                          this,
                                          clock));
    }
    else {
        clock = clockIt->second;
        bcemt_LockGuard<bcemt_Mutex> guard(clock->mutex());
        clock->categories().insert(category);
    }

    // If there is a default schedule and it's not 'interval', then add
    // 'category' to the set of non-default categories maintained by
    // the default schedule's clock (indicating that 'category' should no
    // longer be published as part of the default publication).
    if (!clock->defaultClock() && d_defaultInterval != INVALID_INTERVAL()) {
        Clocks::iterator dfltIt = d_clocks.find(d_defaultInterval);
        BSLS_ASSERT(d_clocks.end() != dfltIt);

        bcema_SharedPtr<ClockData>& defaultClock = dfltIt->second;
        bcemt_LockGuard<bcemt_Mutex> guard(defaultClock->mutex());
        defaultClock->nonDefaultCategories().insert(category);
    }

    proctor.release();
}

void baem_PublicationScheduler::setDefaultSchedule(
                                             const bdet_TimeInterval& interval)
{
    // This method locks the data members of this object; cancels the existing
    // default schedule (if it has been previously set); then updates
    // 'd_clocks' and to reflect the indicated schedule, creating and
    // scheduling a new 'baem_PublicationScheduler_ClockData' object if one
    // does not exist for 'interval'.

    BSLS_ASSERT(bdet_TimeInterval(0, 0) < interval);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    // If 'interval' equals the existing default schedule interval, return
    // immediately.
    if (interval == d_defaultInterval) {
        return;
    }

    cancelDefaultSchedule();
    d_defaultInterval = interval;

    // Use 'proctor' to ensure that 'this' object is returned to a valid
    // (default) state if an exception occurs performing this operation.
    baem_PublicationScheduler_Proctor proctor(this);

    // Create a new 'ClockData' object if one does not exist for the
    // 'interval', otherwise update the existing 'clock'.
    bcema_SharedPtr<ClockData> clock;
    Clocks::iterator clockIt = d_clocks.find(interval);
    if (clockIt == d_clocks.end()) {
        clock.createInplace(d_allocator_p, d_allocator_p);
        d_clocks.insert(bsl::make_pair(interval, clock));
    }
    else {
        clock = clockIt->second;
    }

    bcemt_LockGuard<bcemt_Mutex> clockGuard(clock->mutex());
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
    // with the underlying 'bcep_TimerEventScheduler' object.
    if (clock->handle() == bcep_TimerEventScheduler::BCEP_INVALID_HANDLE) {
        clock->handle() = d_scheduler_p->startClock(
            interval,
            bdef_BindUtil::bindA(d_allocator_p,
                                 &baem_PublicationScheduler::publish,
                                 this,
                                 clock));
    }

    proctor.release();
}

int baem_PublicationScheduler::cancelCategorySchedule(
                                                 const baem_Category *category)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    Categories::iterator cIt = d_categories.find(category);
    if (cIt == d_categories.end()) {
        // This category has no specific schedule.
        return -1;                                                // RETURN
    }
    cancelCategory(cIt);

    return 0;
}

void baem_PublicationScheduler::cancelAll()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    Clocks::iterator it = d_clocks.begin();
    for (; it != d_clocks.end(); ++it) {
        d_scheduler_p->cancelClock(it->second->handle(), true);
    }
    d_defaultInterval = INVALID_INTERVAL();
    d_clocks.clear();
    d_categories.clear();
}

int baem_PublicationScheduler::clearDefaultSchedule()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return cancelDefaultSchedule();
}

// ACCESSORS
bool
baem_PublicationScheduler::findCategorySchedule(
                                         bdet_TimeInterval   *result,
                                         const baem_Category *category) const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    Categories::const_iterator catIt = d_categories.find(category);
    if (catIt == d_categories.end()) {
        return false;
    }
    *result = catIt->second;
    return true;
}

bool
baem_PublicationScheduler::getDefaultSchedule(bdet_TimeInterval *result) const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (d_defaultInterval == INVALID_INTERVAL()) {
        return false;
    }
    *result = d_defaultInterval;
    return true;
}

int baem_PublicationScheduler::getCategorySchedule(
                      bsl::vector<bsl::pair<const baem_Category *,
                                           bdet_TimeInterval> > *result) const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    result->clear();
    result->reserve(d_categories.size());
    Categories::const_iterator catIt = d_categories.begin();
    for (; catIt != d_categories.end(); ++catIt) {
        result->push_back(bsl::make_pair(catIt->first, catIt->second));
    }
    return d_categories.size();
}

bsl::ostream&
baem_PublicationScheduler::print(bsl::ostream&   stream,
                                 int             level,
                                 int             spacesPerLevel) const
{
    // We must sort the various sets of categories alphabetically to
    // ensure that the resulting formatted text is consistent
    // (for testing).

    typedef bsl::pair<const baem_Category *,bdet_TimeInterval> ScheduleElement;
    typedef bsl::vector<ScheduleElement>                       Schedule;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    Schedule schedule;
    Categories::const_iterator catIt = d_categories.begin();
    for (; catIt != d_categories.end(); ++catIt) {
        schedule.push_back(bsl::make_pair(catIt->first, catIt->second));
    }
    bsl::sort(schedule.begin(), schedule.end(), ScheduleElementSort());

    char SEP = (level <= 0) ? ' ' : '\n';
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << "[" << SEP;
    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "default interval: ";
    if (d_defaultInterval != INVALID_INTERVAL()) {
        stream << d_defaultInterval << SEP;
    }
    else {
        stream << "none" << SEP;
    }

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "scheduled categories: [ " << SEP;
    Schedule::const_iterator sIt = schedule.begin();
    for (; sIt != schedule.end(); ++sIt) {
        bdeu_Print::indent(stream, level + 2, spacesPerLevel);
        stream << "[ " << sIt->first->name() << " -> "
               << sIt->second << " ]" << SEP;
    }
    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << SEP;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "clock information: [ " << SEP;
    Clocks::const_iterator clockIt = d_clocks.begin();
    for (; clockIt != d_clocks.end(); ++clockIt) {
        bdeu_Print::indent(stream, level + 2, spacesPerLevel);
        stream << "[" << SEP;
        const bdet_TimeInterval&          interval   = clockIt->first;
        const bcema_SharedPtr<ClockData>& clock      = clockIt->second;
        bdeu_Print::indent(stream, level + 3, spacesPerLevel);
        stream << "interval: " << interval
               << "   default: " << clock->defaultClock()
               << "   handle: " << clock->handle() << SEP;

        bsl::set<const baem_Category *>::const_iterator ctIt =
                                                  clock->categories().begin();

        bdeu_Print::indent(stream, level + 3, spacesPerLevel);
        stream << "categories: [";
        printCategorySet(stream, clock->categories());
        stream << "] " << SEP;

        bdeu_Print::indent(stream, level + 3, spacesPerLevel);
        stream << "excluded categories: [";
        printCategorySet(stream, clock->nonDefaultCategories());
        stream << "] " << SEP;

        bdeu_Print::indent(stream, level + 2, spacesPerLevel);
        stream << "]" << SEP;
    }
    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << SEP;
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << SEP;

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
