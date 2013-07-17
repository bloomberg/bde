// bdet_calendarcache.cpp                                             -*-C++-*-
#include <bdet_calendarcache.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_calendarcache_cpp,"$Id$ $CSID$")

#include <bdet_calendarloader.h>
#include <bdet_datetime.h>
#include <bdet_packedcalendar.h>
#include <bdet_systemtimeutil.h>

#include <bslma_default.h>
#include <bslma_deleterhelper.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_types.h>

#include <bsl_functional.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
bool hasExpired(const bdet_TimeInterval& interval,
                const bdet_Datetime&     loadTime)
    // Return 'true' if at least the specified time 'interval' has elapsed
    // since the specified 'loadTime, and 'false' otherwise.
{
    const bsls::Types::Int64 elapsedTime =
      (bdet_SystemTimeUtil::nowAsDatetimeUtc() - loadTime).totalMilliseconds();

    return elapsedTime >= interval.totalMilliseconds() ? true : false;
}

                     // ====================================
                     // class bdet_CalendarCache_EntryPtrRep
                     // ====================================

class bdet_CalendarCache_EntryPtrRep {
    // This class provides the representation object for a shared calendar
    // cache entry.  A reference to a shared calendar, via the representation
    // object that manages it. is acquired via 'acquireRef' and released via
    // 'releaseRef'.  The resources of a representation object, and of the
    // shared calendar to which it refers, are automatically recovered (via
    // 'dispose') when the number of outstanding references goes to 0.  Note
    // that an explicit allocator is *required* to create a representation
    // object.

    // DATA
    bsls::AtomicInt   d_sharedCount;  // number of references to this rep

    bdet_Datetime     d_loadTime;     // time when calendar was loaded

    bdet_Calendar    *d_ptr_p;        // pointer to out-of-place instance
                                      // (held, not owned)

    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    bdet_CalendarCache_EntryPtrRep(const bdet_CalendarCache_EntryPtrRep&);
    bdet_CalendarCache_EntryPtrRep& operator=(
                                   const bdet_CalendarCache_EntryPtrRep&);

  public:
    // CREATORS
    bdet_CalendarCache_EntryPtrRep(bdet_Calendar        *calendar,
                                   const bdet_Datetime&  loadTime,
                                   bslma::Allocator     *allocator);
        // Create a representation object for managing the specified shared
        // 'calendar' that was loaded at the specified 'loadTime' using the
        // specified 'allocator'.  The behavior is undefined unless the memory
        // for 'calendar' and the footprint of this object were both obtained
        // from 'allocator'.

    ~bdet_CalendarCache_EntryPtrRep();
        // Destroy this representation object.

    // MANIPULATORS
    void acquireRef();
        // Atomically acquire a reference to the shared calendar referred to by
        // this representation object.

    void dispose();
        // Destroy the shared calendar referred to by this representation
        // object, destroy this representation object, and deallocate any
        // associated memory.  This method is automatically invoked by
        // 'releaseRef' when the number of references reaches 0 and should not
        // be explicitly invoked otherwise.

    void releaseRef();
        // Atomically release a reference to the shared calendar referred to by
        // this representation object, disposing of the shared object and this
        // representation object if the number of references reaches 0.

    // ACCESSORS
    int numReferences() const;
        // Return the number of references to the shared calendar referred to
        // by this representation object.

    bdet_Datetime loadTime() const;
        // Return the time at which the shared calendar referred to by this
        // representation object was loaded.

    const bdet_Calendar *ptr() const;
        // Return an address providing non-modifiable access to the shared
        // calendar referred to by this representation object.
};

                     // ------------------------------------
                     // class bdet_CalendarCache_EntryPtrRep
                     // ------------------------------------

// CREATORS
bdet_CalendarCache_EntryPtrRep::bdet_CalendarCache_EntryPtrRep(
                                               bdet_Calendar        *calendar,
                                               const bdet_Datetime&  loadTime,
                                               bslma::Allocator     *allocator)
: d_sharedCount(0)                              // minimum consistency: relaxed
, d_loadTime(loadTime)
, d_ptr_p(calendar)
, d_allocator_p(allocator)
{
}

bdet_CalendarCache_EntryPtrRep::~bdet_CalendarCache_EntryPtrRep()
{
}

// MANIPULATORS
void bdet_CalendarCache_EntryPtrRep::acquireRef()
{
    BSLS_ASSERT(0 <= numReferences());

    d_sharedCount.addRelaxed(1);                // minimum consistency: relaxed
}

void bdet_CalendarCache_EntryPtrRep::dispose()
{
    BSLS_ASSERT(0 == numReferences());

    bslma::DeleterHelper::deleteObjectRaw(d_ptr_p, d_allocator_p);

    this->bdet_CalendarCache_EntryPtrRep::~bdet_CalendarCache_EntryPtrRep();
    d_allocator_p->deallocate(this);
}

void bdet_CalendarCache_EntryPtrRep::releaseRef()
{
    BSLS_ASSERT(0 < numReferences());

    if (0 == d_sharedCount.add(-1)) {   // release consistency: acquire/release
        dispose();
    }
}

// ACCESSORS
bdet_Datetime bdet_CalendarCache_EntryPtrRep::loadTime() const
{
    return d_loadTime;
}

int bdet_CalendarCache_EntryPtrRep::numReferences() const
{
    return d_sharedCount.loadRelaxed();        // minimum consistency: relaxed;
}

const bdet_Calendar *bdet_CalendarCache_EntryPtrRep::ptr() const
{
    return d_ptr_p;
}

                      // --------------------------------
                      // class bdet_CalendarCacheEntryPtr
                      // --------------------------------

// PRIVATE CREATORS
bdet_CalendarCacheEntryPtr::bdet_CalendarCacheEntryPtr(
                                           bdet_CalendarCache_EntryPtrRep *rep)
: d_rep_p(rep)
{
    BSLS_ASSERT(d_rep_p);

    d_rep_p->acquireRef();
}

// PRIVATE ACCESSORS
const bdet_CalendarCache_EntryPtrRep *bdet_CalendarCacheEntryPtr::rep() const
{
    return d_rep_p;
}

// CREATORS
bdet_CalendarCacheEntryPtr::bdet_CalendarCacheEntryPtr()
: d_rep_p(0)
{
}

bdet_CalendarCacheEntryPtr::bdet_CalendarCacheEntryPtr(
                                    const bdet_CalendarCacheEntryPtr& original)
: d_rep_p(original.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireRef();
    }
}

bdet_CalendarCacheEntryPtr::~bdet_CalendarCacheEntryPtr()
{
    if (d_rep_p) {
        d_rep_p->releaseRef();
    }
}

// MANIPULATORS
bdet_CalendarCacheEntryPtr& bdet_CalendarCacheEntryPtr::operator=(
                                         const bdet_CalendarCacheEntryPtr& rhs)
{
    if (this != &rhs) {
        if (d_rep_p) {
            d_rep_p->releaseRef();
        }

        d_rep_p = rhs.d_rep_p;

        if (d_rep_p) {
            d_rep_p->acquireRef();
        }
    }

    return *this;
}

// ACCESSORS
const bdet_Calendar *bdet_CalendarCacheEntryPtr::operator->() const
{
    return d_rep_p ? d_rep_p->ptr() : 0;
}

const bdet_Calendar *bdet_CalendarCacheEntryPtr::ptr() const
{
    return d_rep_p ? d_rep_p->ptr() : 0;
}

                        // ------------------------
                        // class bdet_CalendarCache
                        // ------------------------

// CREATORS
bdet_CalendarCache::bdet_CalendarCache(bdet_CalendarLoader *loader,
                                       bslma::Allocator    *basicAllocator)

// We have to supply 'bsl::less<key>()' because 'bsl::map' does not have
// a constructor that takes only an allocator.

: d_cache(bsl::less<bsl::string>(), basicAllocator)
, d_loader_p(loader)
, d_hasTimeOutFlag(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(loader);
}

bdet_CalendarCache::bdet_CalendarCache(
                                      bdet_CalendarLoader      *loader,
                                      const bdet_TimeInterval&  timeout,
                                      bslma::Allocator         *basicAllocator)

// We have to supply 'bsl::less<key>()' because 'bsl::map' does not have
// a constructor that takes only an allocator.

: d_cache(bsl::less<bsl::string>(), basicAllocator)
, d_loader_p(loader)
, d_timeOut(timeout)
, d_hasTimeOutFlag(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(loader);
}

bdet_CalendarCache::~bdet_CalendarCache()
{
}

// MANIPULATORS
bdet_CalendarCacheEntryPtr
bdet_CalendarCache::calendar(const char *calendarName)
{
    BSLS_ASSERT(calendarName);

    bsls::MutexImpProctor lockProctor(&d_lock);

    CacheIterator iter = d_cache.find(calendarName);

    if (iter != d_cache.end()) {
       if (!d_hasTimeOutFlag || !hasExpired(d_timeOut,
                                            iter->second.rep()->loadTime())) {
           return iter->second;
       }
       else {
           d_cache.erase(iter);  // calendar has expired; remove it from cache
       }
    }

    // Load calendar identified by 'calendarName'.

    bdet_PackedCalendar packedCalendar;  // temporary, so use default allocator

    if (d_loader_p->load(&packedCalendar, calendarName)) {
        return bdet_CalendarCacheEntryPtr();
    }

    // Create shared calendar object and proctor it.

    bdet_Calendar *calendarPtr =
                             new (*d_allocator_p) bdet_Calendar(packedCalendar,
                                                                d_allocator_p);

    bslma::RawDeleterProctor<bdet_Calendar, bslma::Allocator>
                                                calendarProctor(calendarPtr,
                                                                d_allocator_p);

    // Create rep object.

    bdet_CalendarCache_EntryPtrRep *repPtr =
                  new (*d_allocator_p) bdet_CalendarCache_EntryPtrRep(
                                       calendarPtr,
                                       bdet_SystemTimeUtil::nowAsDatetimeUtc(),
                                       d_allocator_p);

    // The management of 'repPtr' and (indirectly) 'calendarPtr' are adopted by
    // the 'bdet_CalendarCacheEntryPtr' that is inserted into the cache, so no
    // proctors are necessary from this point.

    calendarProctor.release();

    // Insert newly-loaded calendar into cache.

    d_cache[calendarName] = bdet_CalendarCacheEntryPtr(repPtr);

    return bdet_CalendarCacheEntryPtr(repPtr);
}

void bdet_CalendarCache::invalidate(const char *calendarName)
{
    BSLS_ASSERT(calendarName);

    bsls::MutexImpProctor lockProctor(&d_lock);

    CacheIterator iter = d_cache.find(calendarName);

    if (iter != d_cache.end()) {
        d_cache.erase(iter);
    }
}

void bdet_CalendarCache::invalidateAll()
{
    bsls::MutexImpProctor lockProctor(&d_lock);

    d_cache.clear();
}

// ACCESSORS
bdet_CalendarCacheEntryPtr
bdet_CalendarCache::calendar(const char *calendarName) const
{
    BSLS_ASSERT(calendarName);

    bsls::MutexImpProctor lockProctor(&d_lock);

    ConstCacheIterator iter = d_cache.find(calendarName);

    if (iter != d_cache.end()) {
       if (!d_hasTimeOutFlag || !hasExpired(d_timeOut,
                                            iter->second.rep()->loadTime())) {
           return iter->second;
       }
       else {
           d_cache.erase(iter);  // calendar has expired; remove it from cache
       }
    }

    return bdet_CalendarCacheEntryPtr();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
