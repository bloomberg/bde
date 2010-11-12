// bdecs_calendarcache.cpp            -*-C++-*-
#include <bdecs_calendarcache.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdecs_calendarcache_cpp,"$Id$ $CSID$")

#include <bdecs_calendarloader.h>
#include <bdecs_packedcalendar.h>
#include <bdet_datetime.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_rawdeleterproctor.h>

#include <bsl_functional.h>
#include <bsl_string.h>

namespace BloombergLP {


                        // ==============================
                        // class bdecs_CalendarCacheEntry
                        // ==============================


class bdecs_CalendarCacheEntry {
    // This class implements an entry in the calendar cache.  It contains the
    // calendar object, the time when it is loaded into the cache, and a flag
    // which indicates a forced reload of the calendar object if set to true.

    bdecs_Calendar   d_calendar;         // the calendar object
    bdet_Datetime    d_loadTime;         // the time when this entry is loaded
    bool             d_forceReloadFlag;  // set it to 'true' to force a reload
    bool             d_firstLoadFlag;    // 'true' if 'd_calendar' has never
                                         // been loaded and 'false' otherwise
    bslma_Allocator *d_allocator_p;      // memory allocator(held, not owned)

  private:
    // NOT IMPLEMENTED
    bdecs_CalendarCacheEntry(const bdecs_CalendarCacheEntry&);
    bdecs_CalendarCacheEntry& operator=(const bdecs_CalendarCacheEntry&);

  public:
    // CREATORS
    explicit bdecs_CalendarCacheEntry(bslma_Allocator *basicAllocator = 0);
        // Create an empty calendar cache entry object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bdecs_CalendarCacheEntry();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Mark this entry expired.

    const bdecs_Calendar *calendar(bdecs_CalendarLoader     *loader,
                                   const char               *name,
                                   const bdet_TimeInterval&  timeout);
        // Return the calendar from the cache if the cache entry is valid, or
        // load it into the cache using the calendar loader otherwise.

    const bdecs_Calendar *calendar(bdecs_CalendarLoader *loader,
                                   const char           *name);
        // Reload the calendar from the calendar loader if 'd_forceReloadFlag'
        // is set; return the calendar from the cache otherwise.
};

// CREATORS
inline
bdecs_CalendarCacheEntry::bdecs_CalendarCacheEntry(
                                               bslma_Allocator *basicAllocator)
: d_calendar(basicAllocator)
, d_forceReloadFlag(true)
, d_firstLoadFlag(true)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bdecs_CalendarCacheEntry::~bdecs_CalendarCacheEntry()
{
}

// MANIPULATORS
const bdecs_Calendar *bdecs_CalendarCacheEntry::calendar(
                                                  bdecs_CalendarLoader *loader,
                                                  const char           *name)
{
    if (d_forceReloadFlag) {
        bdecs_PackedCalendar packedCalendar(d_allocator_p);
        int ret = loader->load(&packedCalendar, name);
        if (ret != 0) {
            if (d_firstLoadFlag) {
                return 0;
            }
            else {
                return &d_calendar;
            }
        }
        d_loadTime = bdetu_SystemTime::nowAsDatetimeGMT();
        bdecs_Calendar calendar(packedCalendar, d_allocator_p);
        d_calendar.swap(&calendar);
        d_forceReloadFlag = false;
        d_firstLoadFlag = false;
    }
    return &d_calendar;
}

const bdecs_Calendar *bdecs_CalendarCacheEntry::calendar(
                                             bdecs_CalendarLoader     *loader,
                                             const char               *name,
                                             const bdet_TimeInterval&  timeout)
{
    if ((bdetu_SystemTime::nowAsDatetimeGMT() - d_loadTime).seconds() >=
                                                           timeout.seconds()) {

        // This entry has expired.

        d_forceReloadFlag = true;
    }

    return calendar(loader, name);
}

inline void bdecs_CalendarCacheEntry::invalidate()
{
    d_forceReloadFlag = true;
}


                        // -------------------------
                        // class bdecs_CalendarCache
                        // -------------------------


// CREATORS
bdecs_CalendarCache::bdecs_CalendarCache(bdecs_CalendarLoader *loader,
                                         bslma_Allocator      *basicAllocator)

// We have to pass 'bsl::less<key>()' because 'bsl::map' does not have
// a constructor which takes only a allocator.

: d_cache(bsl::less<bsl::string>(), basicAllocator)
, d_loader_p(loader)
, d_useTimeOutFlag(false)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bdecs_CalendarCache::bdecs_CalendarCache(
                                      bdecs_CalendarLoader     *loader,
                                      const bdet_TimeInterval&  timeout,
                                      bslma_Allocator          *basicAllocator)

// We have to pass 'bsl::less<key>()' because 'bsl::map' does not have
// a constructor which takes only an allocator.

: d_cache(bsl::less<bsl::string>(), basicAllocator)
, d_loader_p(loader)
, d_timeOut(timeout)
, d_useTimeOutFlag(true)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bdecs_CalendarCache::~bdecs_CalendarCache()
{
    for (CacheIterator it = d_cache.begin(); it != d_cache.end(); ++it) {
        d_allocator_p->deleteObjectRaw(it->second);
    }
}

// MANIPULATORS
const bdecs_Calendar *bdecs_CalendarCache::calendar(const char *calendarName)
{
    if (!calendarName) {
        return 0;
    }
    CacheIterator it = d_cache.find(calendarName);
    if (it != d_cache.end()) {
        if (d_useTimeOutFlag) {
            return it->second->calendar(d_loader_p, calendarName, d_timeOut);
        }
        else {
            return it->second->calendar(d_loader_p, calendarName);
        }
    }

    // Insert a new entry.

    bdecs_CalendarCacheEntry *entry_p;
    entry_p = new(*d_allocator_p) bdecs_CalendarCacheEntry(d_allocator_p);
    if (0 == entry_p) {
        return 0;
    }

    // The 'bslma_RawDeleterProctor' object below is used to release the memory
    // pointed to by 'entry_p' in the case when an exception takes place.  It
    // will release the memory when it is destroyed.

    bslma_RawDeleterProctor<bdecs_CalendarCacheEntry, bslma_Allocator>
                                        deleterProctor(entry_p, d_allocator_p);

    const bdecs_Calendar *calendar_p = entry_p->calendar(d_loader_p,
                                                         calendarName);
    if (0 == calendar_p) {
        return 0;
    }

    d_cache[calendarName] = entry_p;

    // No exception is thrown.  Release the auto deleter object so it won't
    // free the memory pointer to by entry_p.

    deleterProctor.release();

    return calendar_p;
}

void bdecs_CalendarCache::invalidate(const char *calendarName)
{
    if (calendarName) {
        CacheIterator it = d_cache.find(calendarName);
        if (it != d_cache.end()) {
            it->second->invalidate();
        }
    }
}

void bdecs_CalendarCache::invalidateAll()
{
    for (CacheIterator it = d_cache.begin(); it != d_cache.end(); ++it) {
        it->second->invalidate();
    }
}


                        // -----------------------------
                        // class bdecs_CalendarCacheIter
                        // -----------------------------

// CREATORS
bdecs_CalendarCacheIter::bdecs_CalendarCacheIter(
                                     const CacheConstIterator&  iter,
                                     bdecs_CalendarLoader      *loader,
                                     bool                       useTimeOutFlag,
                                     const bdet_TimeInterval&   timeout)
: d_iterator(iter)
, d_loader_p(loader)
, d_useTimeOutFlag(useTimeOutFlag)
, d_timeOut_p(&timeout)
{
}

// MANIPULATORS
bdecs_CalendarCacheIter&
bdecs_CalendarCacheIter::operator=(const bdecs_CalendarCacheIter& original)
{
    d_iterator       = original.d_iterator;
    d_loader_p       = original.d_loader_p;
    d_useTimeOutFlag = original.d_useTimeOutFlag;
    d_timeOut_p      = original.d_timeOut_p;
    return *this;
}

bdecs_CalendarCacheIter operator++(bdecs_CalendarCacheIter& iter, int)
{
    bdecs_CalendarCacheIter imp(iter);
    ++iter;
    return imp;
}

bdecs_CalendarCacheIter operator--(bdecs_CalendarCacheIter& iter, int)
{
    bdecs_CalendarCacheIter imp(iter);
    --iter;
    return imp;
}

// ACCESSORS
bdecs_CalendarCache_PairRef bdecs_CalendarCacheIter::operator*() const
{
    return bdecs_CalendarCache_PairRef(bdecs_CalendarCache_Pair
                                   (d_iterator->first,
                                    d_useTimeOutFlag
                                    ? *d_iterator->second->calendar(
                                                  d_loader_p,
                                                  d_iterator->first.c_str(),
                                                 *d_timeOut_p)
                                    : *d_iterator->second->calendar(
                                                  d_loader_p,
                                                  d_iterator->first.c_str())));
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
