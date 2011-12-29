// bdecs_calendarcache.h            -*-C++-*-
#ifndef INCLUDED_BDECS_CALENDARCACHE
#define INCLUDED_BDECS_CALENDARCACHE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an efficient cache for 'bdecs_Calendar'.
//
//@CLASSES:
//               bdecs_CalendarCache: cache for loading calendars using their
//                                    names
//          bdecs_CalendarCache_Pair: struct for storing the (name, calendar)
//                                    pair returned by calendar cache's
//                                    iterators
// bdecs_CalendarCacheIter_PairProxy: proxy object for
//                                    'bdecs_CalendarCache_Pair'
//       bdecs_CalendarCache_PairRef: reference object for
//                                    'bdecs_CalendarCache_Pair'
//           bdecs_CalendarCacheIter: iterator for 'bdecs_CalendarCache'
//
//@AUTHOR: Guillaume Morin (gmorin1), Yu Zhao (yzhao3)
//
//@SEE_ALSO: bdecs_calendar
//
//@DESCRIPTION: This component provides the 'bdecs_CalendarCache' class, which
// is a cache for ('bdecs_Calendar') calendars.  'bdecs_CalendarCache' is able
// to load new calendars through a loader specified at construction.
//
// The calendar cache also supports the notion of a cached calendar becoming
// invalid.  In particular, the user may optionally specify at cache
// construction a valid lifetime for all calendars in a cache object.  That
// lifetime begins, for each calendar, when the calendar is loaded (via the
// 'calendar' method) for the first time.  Subsequent requests for that
// calendar within the valid lifetime will be fulfilled from the cache.
// Requests after calendar expiry will cause the calendar to be reloaded.  If
// this lifetime value is not specified during the construction of the calendar
// cache object, it is taken to be infinity.  Users can also force a specific
// calendar (all calendars) to expire by calling 'invalidate'
// ('invalidateAll').
//
// For an individual 'bdecs_CalendarCache' object, the pointers returned by
// multiple calls to the 'calendar' method for a specific calendar are always
// the same, even after that calendar is reload due to expiry or invalidation.
// This pointer will remain valid as long as this 'bdecs_CalendarCache' object
// is valid.  The calendar object it points to, however, may not be up-to-date
// if the associated cache entry has expired.  A reload of this entry by
// calling 'calendar' again is necessary to get the most up-to-date calendar
// object.
//
// 'bdecs_CalendarCache' provides a complete set of STL-like 'const'
// bidirectional iterators.  These iterators iterate over both valid and
// expired entries in the cache.  When a user tries to access an expired
// calendar cache entry via the iterator, the calendar will be reloaded to
// ensure updated calendar information is returned.
//
///Thread Safety
///-------------
// This class can be used in a multithreaded environment provided that at most
// one thread is using a specific 'bdecs_CalendarCache' object at any time.
// This class does not provide protection against multiple readers/writers from
// more than one thread.  The pointers returned by the 'calendar' method may
// not always point to a calendar object in a consistent state in this case.
// Therefore, in a multithreaded environment, the application must use a lock
// or equivalent to prevent two or more threads from accessing a calendar cache
// object at the same time.
//
///Usage
///-----
// The subsections below illustrate how to use 'bdecs_CalendarCache'.
//
///Using Calendar Cache with no timeout
///- - - - - - - - - - - - - - - - - -
// A calendar cache can be created with or without a timeout value.  The
// example below shows how to use a 'bdecs_CalendarCache' object with no
// timeout value specified.  It also demonstrates the usage of 'invalidate'
// and 'invalidateAll'.
//..
//      // This example shows how to use a calendar cache object with no
//      // timeout
//
//      bapcdr_HolidayLoader loader;
//      bdecs_CalendarCache cache(&loader);
//      const bdecs_Calendar *cal;
//
//      cal = cache.calendar("US");  // load "US" calendar from loader
//      assert(cal != 0);
//
//      cal = cache.calendar("FR");  // load "FR" calendar from loader
//      assert(cal != 0);
//
//      cal = cache.calendar("FR");  // "FR" calendar returned from cache
//      assert(cal != 0);
//
//      cache.invalidate("US");      // "US" calendar invalidated
//      cal = cache.calendar("US");  // reload "US" calendar from loader
//      assert(cal != 0);
//
//      cache.invalidateAll();       // both entries are invalidated
//      cal = cache.calendar("US");  // reload "US" calendar from loader
//      assert(cal != 0);
//      cal = cache.calendar("FR");  // reload "FR" calendar from loader
//      assert(cal != 0);
//..
///Using Calendar Cache with a timeout value
///- - - - - - - - - - - - - - - - - - - - -
// The example below shows how to use a 'bdecs_CalendarCache' object with a
// timeout value specified.  It also shows the behavior of the timeout
// mechanism.
//..
//      // This example shows how to use a calendar cache object with a timeout
//      // value specified
//
//      bapcdr_HolidayLoader loader;
//      bdecs_CalendarCache cache(&loader, bdet_TimeInterval(3));
//      const bdecs_Calendar *cal;
//
//      cal = cache.calendar("US");  // load "US" calendar from loader
//      assert(cal != 0);
//
//      sleep(2);                    // wait 2 seconds
//      cal = cache.calendar("FR");  // load "FR" calendar from loader
//      assert(cal != 0);
//
//      sleep(2);                    // wait 2 more seconds
//      cal = cache.calendar("US");  // "US" calendar has expired.  It
//                                   // will be retrieved from cache.
//      assert(cal != 0);
//
//      cal = cache.calendar("FR");  // "FR" still in cache
//      assert(cal != 0);
//
//      sleep(2);                    // wait 2 more seconds
//      cal = cache.calendar("FR");  // "FR" calendar has expired.  It
//                                   // will be retrieved from cache.
//      assert(cal != 0);
//
//      cache.invalidate("US");      // "US" calendar invalidated
//      cal = cache.calendar("US");  // "US" calendar reloaded
//      assert(cal != 0);
//..
///Using Iterators
///- - - - - - - -
// The iterators returned by 'begin', 'end', 'rbegin', and 'rend' behaves
// exactly like the STL iterators.  So applications can use them to iterate
// over all the entries (including both valid and expired entries) in the
// cache.  Note that calling an iterator's 'operator*' or 'operator->' method
// will cause calendar object to be reloaded if this entry has expired.  Below
// is an example of how to use 'ConstIterator'.
//..
//      // This is an example of how to use 'ConstIterator'
//      bapcdr_HolidayLoader loader;
//      bdecs_CalendarCache cache(&loader);
//      const bdecs_Calendar *cal;
//
//      cal = cache.calendar("US");
//      assert(cal != 0);
//      cal = cache.calendar("FR");
//      assert(cal != 0);
//      cal = cache.calendar("CH");
//      assert(cal != 0);
//      cal = cache.calendar("EN");
//      assert(cal != 0);
//      cache.invalidate("EN");
//
//      // Print all calendars names including the invalidated one
//      bdecs_CalendarCache::ConstIterator it;
//      for (it = cache.begin(); it != cache.end(); ++it) {
//          bsl::cout << it->first << bsl::endl;
//      }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDECS_CALENDAR
#include <bdecs_calendar.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif


namespace BloombergLP {

class bslma_Allocator;
class bdecs_CalendarLoader;
class bdecs_CalendarCacheEntry;
class bdecs_CalendarCacheIter;


                        // =========================
                        // class bdecs_CalendarCache
                        // =========================


class bdecs_CalendarCache {
    // This class implements a cache for loading and holding calendars of type
    // 'bdecs_Calendar'.  An optional timeout value for entries in the cache
    // governs each calendar's valid lifetimes.  A calendar is loaded via the
    // 'calendar' method.  This method initially loads the calendar using the
    // calendar loader, and caches calendars for faster access on subsequent
    // calls, until individual calendars expire in which case the expired
    // calendar is reloaded on next access.  The pointer returned by the
    // 'calendar' method points to the calendar object inside the cache.  Two
    // methods, 'invalidate' and 'invalidateAll', are provided for applications
    // to mark one entry or all entries in the cache for reload in the future.
    //
    // This class supports STL-like iterators and reverse iterators.  They are
    // accessed through the 'begin', 'end', 'rbegin', and 'rend' methods.
    // They can be used to iterate over all the entries in the cache using the
    // 'operator++' and 'operator--' methods.  The value pointed to by an
    // iterator is a STL-pair-like object which has the calendar name as its
    // 'first' and the calendar object as 'second'.  Note that both of these
    // fields are references to the objects inside the map.
    //
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing instance, the container is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.

    bsl::map<bsl::string, bdecs_CalendarCacheEntry *>  d_cache;
        // cache which stores (name, calendar entry) pairs

    bdecs_CalendarLoader                              *d_loader_p;
        // pointer to calendar loader

    bdet_TimeInterval                                  d_timeOut;
        // timeout value (meaningful only if d_useTimeOutFlag is 'true')

    bool                                               d_useTimeOutFlag;
        // 'true' if this calendar cache object has a timeout value and 'false'
        // otherwise

    bslma_Allocator                                   *d_allocator_p;
        // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    bdecs_CalendarCache(const bdecs_CalendarCache&);
    bdecs_CalendarCache& operator=(const bdecs_CalendarCache&);

    // PRIVATE TYPES
    typedef bsl::map<bsl::string, bdecs_CalendarCacheEntry *>::iterator
                                                                 CacheIterator;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdecs_CalendarCache,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // PUBLIC TYPES
    typedef bdecs_CalendarCacheIter                       ConstIterator;
    typedef ConstIterator                                 Iterator;
    typedef bsl::reverse_iterator<bdecs_CalendarCacheIter>
                                                          ConstReverseIterator;
    typedef ConstReverseIterator                          ReverseIterator;

    // CREATORS
    bdecs_CalendarCache(bdecs_CalendarLoader *loader,
                        bslma_Allocator      *basicAllocator = 0);
        // Create an empty cache that will use the specified 'loader' to obtain
        // named calendars.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless the
        // loader remains valid throughout the lifetime of this cache.  Note
        // that calendars in this cache will never expire due to a timeout.

    bdecs_CalendarCache(bdecs_CalendarLoader     *loader,
                        const bdet_TimeInterval&  timeout,
                        bslma_Allocator          *basicAllocator = 0);
        // Create an empty cache that will use the specified 'loader' to obtain
        // named calendars.  Each entry in this calendar cache will become
        // invalid after the specified timeout period has passed since that
        // calendar was loaded.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless the
        // loader remains valid throughout the lifetime of this cache.

    ~bdecs_CalendarCache();
        // Destroy this object.

    // MANIPULATORS
    const bdecs_Calendar *calendar(const char *calendarName);
        // Return the non-modifiable address of the calendar corresponding to
        // the specified 'calendarName', if 'calendarName' can be loaded
        // through the loader specified at construction, and 0 otherwise.  When
        // retrieving the calendar object, this method first tries to locate a
        // valid cache entry.  If no such entry is found, this method loads
        // 'calendarName' using the loader of this cache.  If the loader fails
        // and an invalid calendar is cached, that invalid calendar is
        // returned.

    void invalidate(const char *calendarName);
        // Mark the cache entry for the specified 'calendarName' as invalid if
        // 'calendarName' is present.  This entry, if present, will be reloaded
        // the next time it is accessed.  If the specified entry is not found,
        // this method has no effect.

    void invalidateAll();
        // Invalidate all entries in the cache.

    // ACCESSORS
    ConstIterator begin() const;
        // Return an iterator referencing the first 'bdecs_CalendarCache_Pair'
        // object in the cache.  The cache is alphabetically ordered on the
        // calendar names.  If the calendar cache is empty, the returned
        // iterator will be positioned at 'end()'.

    ConstIterator end() const;
        // Return an iterator indicating one position past the last
        // 'bdecs_CalendarCache_Pair' object in the cache.  The cache is
        // alphabetically ordered on the calendar names.  If the calendar cache
        // is empty, the returned iterator will be positioned at 'begin()'.

    ConstReverseIterator rbegin() const;
        // Return an iterator referencing the last 'bdecs_CalendarCache_Pair'
        // object in the cache.  The cache is alphabetically ordered on the
        // calendar names.  If the calendar cache is empty, the returned
        // iterator will be positioned at 'rend()'.

    ConstReverseIterator rend() const;
        // Return an iterator indicating one position past the first
        // 'bdecs_CalendarCache_Pair' object in the cache.  The cache is
        // alphabetically ordered on the calendar names.  If the calendar cache
        // is empty, the returned iterator will be positioned at 'rbegin()'.
};


                        // ==============================
                        // class bdecs_CalendarCache_Pair
                        // ==============================


struct bdecs_CalendarCache_Pair {
    // This class implements a simple (calendar name, calendar) pair object.
    // Both the string for calendar name and the calendar object are stored as
    // references.  This class is created for backward compatibility purpose.
    // It is used instead of the STL 'pair' object because the 'pair' object
    // does not allow its 'first' and 'second' to be references.

    const bsl::string&    first;   // name of the calendar
    const bdecs_Calendar& second;  // the calendar object

    // FRIENDS
    friend bool operator==(const bdecs_CalendarCache_Pair& lhs,
                           const bdecs_CalendarCache_Pair& rhs);
    friend bool operator!=(const bdecs_CalendarCache_Pair& lhs,
                           const bdecs_CalendarCache_Pair& rhs);
  private:
    // NOT IMPLEMENTED
    bdecs_CalendarCache_Pair& operator=(const bdecs_CalendarCache_Pair&);

  public:
    // CREATORS
    bdecs_CalendarCache_Pair(const bsl::string&    name,
                             const bdecs_Calendar& calendar);
        // Create a new 'bdecs_CalendarCache_Pair' object using 'name' as
        // 'first' and 'calendar' as 'second'.

    bdecs_CalendarCache_Pair(const bdecs_CalendarCache_Pair& original);
        // Create a new 'bdecs_CalendarCache_Pair' object that has the same
        // value as 'original'.

    ~bdecs_CalendarCache_Pair();
        // Destroy this object.
};

// FREE OPERATORS
inline
bool operator==(const bdecs_CalendarCache_Pair& lhs,
                const bdecs_CalendarCache_Pair& rhs);

inline
bool operator!=(const bdecs_CalendarCache_Pair& lhs,
                const bdecs_CalendarCache_Pair& rhs);

                     // =======================================
                     // class bdecs_CalendarCacheIter_PairProxy
                     // =======================================


class bdecs_CalendarCacheIter_PairProxy {
    // This object is a proxy class for the iterator value of
    // 'bdecs_CalendarCacheIter' for use by the arrow operator of
    // 'bdecs_CalendarCacheIter'.  An instance of this class behaves as
    // the 'bdecs_CalendarCache_Pair' object with which it was constructed.

     bdecs_CalendarCache_Pair d_pair; // proxied pair

  private:
    // NOT IMPLEMENTED
    bdecs_CalendarCacheIter_PairProxy&
                           operator=(const bdecs_CalendarCacheIter_PairProxy&);

  public:
    // CREATORS
    bdecs_CalendarCacheIter_PairProxy(const bdecs_CalendarCache_Pair& pair);
        // Create a proxy object for the specified 'pair'.

   ~bdecs_CalendarCacheIter_PairProxy();
        // Destroy this object.

    bdecs_CalendarCacheIter_PairProxy(
                            const bdecs_CalendarCacheIter_PairProxy& original);
        // Create a proxy object referencing the same value as the
        // specified 'original'.

    // ACCESSORS
    const bdecs_CalendarCache_Pair *operator->() const;
        // Return the address of the proxied object.
};


                        // =================================
                        // class bdecs_CalendarCache_PairRef
                        // =================================


struct bdecs_CalendarCache_PairRef : bdecs_CalendarCache_Pair {
    // This private class is used by the arrow operator of
    // 'bdecs_CalendarCacheIter'.  The objects instantiated from this class
    // serve as references to 'bdecs_CalendarCache_Pair' objects.

  private:
    // NOT IMPLEMENTED
    bdecs_CalendarCache_PairRef& operator=(const bdecs_CalendarCache_PairRef&);

  public:
    // CREATORS
    explicit bdecs_CalendarCache_PairRef(const bdecs_CalendarCache_Pair& pair);
        // Create a new object using the specified 'pair'.

    bdecs_CalendarCache_PairRef(const bdecs_CalendarCache_PairRef& original);
        // Create a new object having the value of the specified 'original'.

    ~bdecs_CalendarCache_PairRef();
        // Destroy this object.

    // ACCESSORS
    bdecs_CalendarCacheIter_PairProxy operator&() const;
};


                        // =============================
                        // class bdecs_CalendarCacheIter
                        // =============================


class bdecs_CalendarCacheIter {
    // Provide read-only, sequential access in increasing (alphabetical) order
    // to the entries in a 'bdecs_CalendarCache' object.

    bsl::map<bsl::string, bdecs_CalendarCacheEntry *>::const_iterator
                                                              d_iterator;
        // the iterator for the map which stores all the cache entries

    bdecs_CalendarLoader                                     *d_loader_p;
        // pointer to the calendar loader

    bool                                                      d_useTimeOutFlag;
        // 'true' if the associated calendar cache has a timeout value and
        // 'false' otherwise

    const bdet_TimeInterval                                  *d_timeOut_p;
        // pointer to the timeout value for the calendar cache (meaningful only
        // if 'd_useTimeOutFlag' is 'true')

    // FRIENDS
    friend class bdecs_CalendarCache;
    friend bool operator==(const bdecs_CalendarCacheIter& lhs,
                           const bdecs_CalendarCacheIter& rhs);
    friend bool operator!=(const bdecs_CalendarCacheIter& lhs,
                           const bdecs_CalendarCacheIter& rhs);

  private:
    // PRIVATE TYPES
    typedef bsl::map<bsl::string, bdecs_CalendarCacheEntry *>::const_iterator
                                                            CacheConstIterator;

    // PRIVATE CREATORS
    bdecs_CalendarCacheIter(const CacheConstIterator&  iter,
                            bdecs_CalendarLoader      *loader,
                            bool                       useTimeOutFlag,
                            const bdet_TimeInterval&   timeout);
        // Create a calendar cache iterator using the specified 'iter',
        // 'loader', 'userTimeOut', and 'timeout'.  'iter' is the iterator for
        // the map object in the calendar cache object.  'loader' is a pointer
        // to the calendar loader object.  'useTimeOutFlag' is 'true' if the
        // associated calendar object has a timeout value and 'false'
        // otherwise.  'timeout' is a reference to the timeout value in the
        // calendar cache object.  Its value is meaningful only if
        // 'useTimeOutFlag' is 'true'.

  public:
    // PUBLIC TYPES
    typedef bdecs_CalendarCache_Pair          value_type;
    typedef int                               difference_type;
    typedef bdecs_CalendarCacheIter_PairProxy pointer;
    typedef bdecs_CalendarCache_PairRef       reference;

    typedef bsl::bidirectional_iterator_tag iterator_category;

    // CREATORS
    bdecs_CalendarCacheIter();
        // Create an uninitialized object.

    bdecs_CalendarCacheIter(const bdecs_CalendarCacheIter& original);
        // Create an iterator using the specified 'original' iterator.

    ~bdecs_CalendarCacheIter();
        // Destroy this object.

    // MANIPULATORS
    bdecs_CalendarCacheIter& operator=(const bdecs_CalendarCacheIter& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator.

    bdecs_CalendarCacheIter& operator++();
        // Advance this iterator to refer to the next entry in the calendar
        // cache and return a reference to this modifiable object.  The
        // behavior is undefined unless this iterator references a valid entry.

    bdecs_CalendarCacheIter& operator--();
        // Regress this iterator to refer to the previous entry in the calendar
        // cache and return a reference to this modifiable object.  The
        // behavior is undefined if this iterator is positioned at the
        // beginning of the sequence.

    // ACCESSORS
    bdecs_CalendarCache_PairRef operator*() const;
        // Return the 'bdecs_CalendarCache_Pair' object referenced by this
        // iterator.  It will reload the calendar via the loader if this entry
        // has expired or has been marked for reload.

    bdecs_CalendarCacheIter_PairProxy operator->() const;
        // Return a proxy for the current calendar cache entry.
};

// FREE OPERATORS
inline
bool operator==(const bdecs_CalendarCacheIter& lhs,
                const bdecs_CalendarCacheIter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators point to the
    // same entry in the same calendar cache, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' both iterate over the same
    // calendar cache.

inline
bool operator!=(const bdecs_CalendarCacheIter& lhs,
                const bdecs_CalendarCacheIter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not point to
    // the same entry in the same calendar cache, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' both iterate over the same
    // calendar cache.

bdecs_CalendarCacheIter operator++(bdecs_CalendarCacheIter& iterator, int);
    // Advance the specified 'iterator' to refer to the next entry in the
    // calendar cache and return the previous value of 'iterator'.  The
    // behavior is undefined unless this iterator references a valid entry.

bdecs_CalendarCacheIter operator--(bdecs_CalendarCacheIter& iterator, int);
    // Regress the specified 'iterator' to refer to the previous entry in the
    // calendar cache and return the previous value of 'iterator'.  The
    // behavior is undefined if this iterator is positioned at the beginning of
    // the sequence.


// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================


                        // -------------------------
                        // class bdecs_CalendarCache
                        // -------------------------

// ACCESSORS
inline
bdecs_CalendarCache::ConstIterator bdecs_CalendarCache::begin() const
{
    return ConstIterator(d_cache.begin(),  d_loader_p,
                         d_useTimeOutFlag, d_timeOut);
}

inline
bdecs_CalendarCache::ConstIterator bdecs_CalendarCache::end() const
{
    return ConstIterator(d_cache.end(),    d_loader_p,
                         d_useTimeOutFlag, d_timeOut);
}

inline
bdecs_CalendarCache::ConstReverseIterator bdecs_CalendarCache::rbegin() const
{
    return ConstReverseIterator(end());
}

inline
bdecs_CalendarCache::ConstReverseIterator bdecs_CalendarCache::rend() const
{
    return ConstReverseIterator(begin());
}


                        // ------------------------------
                        // class bdecs_CalendarCache_Pair
                        // ------------------------------

// CREATORS
inline
bdecs_CalendarCache_Pair::bdecs_CalendarCache_Pair(
                                                const bsl::string&    name,
                                                const bdecs_Calendar& calendar)
: first(name)
, second(calendar)
{
}

inline
bdecs_CalendarCache_Pair::bdecs_CalendarCache_Pair(
                                      const bdecs_CalendarCache_Pair& original)
: first(original.first)
, second(original.second)
{
}

inline
bdecs_CalendarCache_Pair::~bdecs_CalendarCache_Pair()
{
}

// FREE OPERATORS
inline
bool operator==(const bdecs_CalendarCache_Pair& lhs,
                const bdecs_CalendarCache_Pair& rhs)
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

inline
bool operator!=(const bdecs_CalendarCache_Pair& lhs,
                const bdecs_CalendarCache_Pair& rhs)
{
    return lhs.first != rhs.first || lhs.second != rhs.second;
}


                       // ---------------------------------------
                       // class bdecs_CalendarCacheIter_PairProxy
                       // ---------------------------------------

// CREATORS
inline
bdecs_CalendarCacheIter_PairProxy::bdecs_CalendarCacheIter_PairProxy(
                                          const bdecs_CalendarCache_Pair& pair)
: d_pair(pair)
{
}

inline
bdecs_CalendarCacheIter_PairProxy::bdecs_CalendarCacheIter_PairProxy(
                             const bdecs_CalendarCacheIter_PairProxy& original)
: d_pair(original.d_pair)
{
}

inline
bdecs_CalendarCacheIter_PairProxy::
~bdecs_CalendarCacheIter_PairProxy()
{
}

// ACCESSORS
inline
const bdecs_CalendarCache_Pair*
bdecs_CalendarCacheIter_PairProxy::operator->() const
{
    return &d_pair;
}


                        // ---------------------------------
                        // class bdecs_CalendarCache_PairRef
                        // ---------------------------------

// CREATORS
inline
bdecs_CalendarCache_PairRef::
bdecs_CalendarCache_PairRef(const bdecs_CalendarCache_Pair& pair)
:bdecs_CalendarCache_Pair(pair)
{
}

inline
bdecs_CalendarCache_PairRef::
bdecs_CalendarCache_PairRef(const bdecs_CalendarCache_PairRef& original)
:bdecs_CalendarCache_Pair(original)
{
}

inline
bdecs_CalendarCache_PairRef::~bdecs_CalendarCache_PairRef()
{
}

// ACCESSORS
inline
bdecs_CalendarCacheIter_PairProxy
bdecs_CalendarCache_PairRef::operator&() const
{
    return *this;
}


                        // -----------------------------
                        // class bdecs_CalendarCacheIter
                        // -----------------------------

// CREATORS
inline
bdecs_CalendarCacheIter::bdecs_CalendarCacheIter()
{
}

inline
bdecs_CalendarCacheIter::bdecs_CalendarCacheIter(
                                       const bdecs_CalendarCacheIter& original)
: d_iterator(original.d_iterator)
, d_loader_p(original.d_loader_p)
, d_useTimeOutFlag(original.d_useTimeOutFlag)
, d_timeOut_p(original.d_timeOut_p)
{
}

inline
bdecs_CalendarCacheIter::~bdecs_CalendarCacheIter()
{
}

// MANIPULATORS
inline
bdecs_CalendarCacheIter& bdecs_CalendarCacheIter::operator++()
{
    ++d_iterator;
    return *this;
}

inline
bdecs_CalendarCacheIter& bdecs_CalendarCacheIter::operator--()
{
    --d_iterator;
    return *this;
}

// ACCESSORS
inline
bdecs_CalendarCacheIter_PairProxy
bdecs_CalendarCacheIter::operator->() const
{
    return bdecs_CalendarCacheIter_PairProxy(this->operator*());
}

// FREE OPERATORS
inline
bool operator==(const bdecs_CalendarCacheIter& lhs,
                const bdecs_CalendarCacheIter& rhs)
{
    return lhs.d_iterator       == rhs.d_iterator
        && lhs.d_loader_p       == rhs.d_loader_p
        && lhs.d_useTimeOutFlag == rhs.d_useTimeOutFlag
        && lhs.d_timeOut_p      == rhs.d_timeOut_p;
}

inline
bool operator!=(const bdecs_CalendarCacheIter& lhs,
                const bdecs_CalendarCacheIter& rhs)
{
    return lhs.d_iterator       != rhs.d_iterator
        || lhs.d_loader_p       != rhs.d_loader_p
        || lhs.d_useTimeOutFlag != rhs.d_useTimeOutFlag
        || lhs.d_timeOut_p      != rhs.d_timeOut_p;
}


}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
