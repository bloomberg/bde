// bteso_ipresolutioncache.h                                          -*-C++-*-
#ifndef INCLUDED_BTESO_IPRESOLUTIONCACHE
#define INCLUDED_BTESO_IPRESOLUTIONCACHE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif

BDES_IDENT("$Id: $")

//@PURPOSE: Provide a cache for IP addresses resolved from given hostnames.
//
//@CLASSES:
//  bteso_IpResolutionCache: cache for IP addresses resolved from hostnames.
//
//@AUTHOR: Raymond Chiu (schiu49)
//
//@SEE_ALSO:
//  bteso_resolveutil, bteso_ipv4address
//
//@DESCRIPTION: This component defines a mechanism, 'bteso_IpResolutionCache',
// that serves as a cache of 'bteso_IPv4Address' objects that are associated
// with a hostname.  A 'bteso_IpResolutionCache' object is supplied the address
// of a 'bteso_ResolveUtil::ResolveByNameCallback' function at construction,
// which it subsequently uses to obtain the set of IP addresses for a given
// hostname.  The 'resolveAddress' method returns a set of IP addresses for a
// supplied hostname.  'resolveAddress' either returns values already residing
// in the cache (if they haven't expired), or invokes the supplied
// 'ResolveByNameCallback' to obtain the set of IP addresses, which are then
// cached for subsequent use.  IP addresses stored in the cache are considered
// valid for a user-defined time interval, set by the 'setTimeToLive' method.
// Stored IP addresses older than the configured interval are considered stale,
// and a subsequent request for the associated hostname will refresh that set
// of IP addresses by again invoking the 'ResolveByNameCallback' object
// supplied at construction.
//
///Thread Safety
///-------------
// 'bteso_IpResolutionCache' is fully *thread-safe*, meaning that all
// non-creator operations on an object can be safely invoked simultaneously
// from multiple threads.
//
///Usage
///-----
// In this section, we show intended usage of this component.
//
///Example 1: Retrieving the IPv4 Addresses of a Given Host
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a 'bteso_IpResolutionCache', and use it to
// retrieve the IPv4 addresses of several host machines.
//
// First, we create a 'bteso_IpResolutionCache' object, which by default, will
// use 'bteso_ResolveUtil::defaultResolveByNameCallback' to retrieve addresses
// that are not in the cache:
//..
//  bteso_IpResolutionCache cache;
//..
// Then, we verify the newly constructed cache does not contain the addresses
// of either "www.bloomberg.com" and "www.businessweek.com":
//..
//  bsl::vector<bteso_IPv4Address> ipAddresses;
//  assert(0 != cache.lookupAddressRaw(&ipAddresses, "www.bloomberg.com", 1));
//  assert(0 != cache.lookupAddressRaw(&ipAddresses,
//                                     "www.businessweek.com",
//                                     1));
//..
// Next, we call the 'resolveAddress' method to retrieve one of the IP address
// for "www.bloomberg.com", and then print out the returned address.  Note that
// since this is the first call to 'resolveAddress' for "www.bloomberg.com",
// 'resolverCallback' will be invoked to retrieve the addresses:
//..
//  int rc = cache.resolveAddress(&ipAddresses, "www.bloomberg.com", 1);
//  assert(0 == rc);
//  assert(1 == ipAddresses.size());
//..
// Now, we write the address to stdout:
//..
//  bsl::cout << "IP Address: " << ipAddresses[0] << std::endl;
//..
// The output of the preceding operation will look like:
//..
//  IP Address: 63.85.36.34:0
//..
// Note that the IP address may differ depending on the configurations of the
// machine on which the code is executed.
//
// Finally, we verify that a subsequent call to 'lookupAddressRaw' returns 0
// for "www.bloomberg.com" (indicating that its addresses are stored in the
// cache), but returns non-zero for "www.businessweek.com" (indicating its
// addresses are not):
//..
//  assert(0 == cache.lookupAddressRaw(&ipAddresses, "www.bloomberg.com", 1));
//  assert(0 != cache.lookupAddressRaw(&ipAddresses,
//                                     "www.businessweek.com",
//                                     1));
//..
//
///Example 2: Using Address Cache with 'bteso_ResolveUtil'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to configure the 'bteso_ResolverUtil'
// component to use a 'bteso_IpResolutionCache' object for resolving IP
// addresses.
//
// In order to use a 'bteso_IpResolutionCache' as the resolution callback in
// 'bteso_ResolveUtil', we must wrap the call to
// 'bteso_IpResolutionCache::resolveAddress' in a free function.
//
// When configuring 'bteso_ResolveUtil', a singleton cache should be created to
// ensure the cache exist for all calls to 'bteso_ResolveUtil::getAddress'.
// First, we create a function that initializes the singleton cache on the
// first execution and returns the address of the cache:
//..
//  static
//  bteso_IpResolutionCache *ipCacheInstance()
//  {
//      static bteso_IpResolutionCache *singletonCachePtr = 0;
//      BCEMT_ONCE_DO {
//          if (0 == singletonCachePtr) {
//              bslma_Allocator *allocator = bslma_Default::globalAllocator();
//              static bteso_IpResolutionCache cache(allocator);
//              singletonCachePtr = &cache;
//          }
//      }
//      return singletonCachePtr;
//  }
//..
// Then, we create a free function to wrap the
// 'bteso_IpResolutionCache::resolveAddress' method:
//..
//  static
//  int resolverCallback(bsl::vector<bteso_IPv4Address> *hostAddresses,
//                       const char                     *hostName,
//                       int                             numAddresses,
//                       int                            *errorCode)
//  {
//      return ipCacheInstance()->resolveAddress(hostAddresses,
//                                               hostName,
//                                               numAddresses,
//                                               errorCode);
//  }
//..
// Now, we set the callback for 'bteso_ResolveUtil' to the free function we
// just created:
//..
//  bteso_ResolveUtil::setResolveByNameCallback(&resolverCallback);
//..
// Finally, we call the 'bteso_ResolveUtil::getAddress' method to retrieve the
// IPv4 address of 'www.bloomberg.com':
//..
//  bteso_IPv4Address ipAddress;
//  bteso_ResolveUtil::getAddress(&ipAddress, "www.bloomberg.com");
//..
// Now, we write the address to stdout:
//..
//  bsl::cout << "IP Address: " << ipAddress << std::endl;
//..
// Finally, we observe the output to be in the form:
//..
//  IP Address: 63.85.36.34:0
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_RESOLVEUTIL
#include <bteso_resolveutil.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BCEMT_WRITELOCKGUARD
#include <bcemt_writelockguard.h>
#endif

#ifndef INCLUDED_BCEMT_READLOCKGUARD
#include <bcemt_readlockguard.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {

class bslma_Allocator;

class bteso_IpResolutionCache;
class bteso_IpResolutionCache_Data;  // defined in .cpp

                        // ===================================
                        // class bteso_IpResolutionCache_Entry
                        // ===================================

class bteso_IpResolutionCache_Entry {
    // This class implements an entry type that is used in the 'bsl::map'
    // contained in a 'bteso_IpResolutionCache' object.  This is an
    // implementation centric class, with each object of this
    // class containing a shared pointer to a 'bteso_IpResolutionCache_Data'
    // object (which stores a set of IP addresses and its creation time) and a
    // mutex variable to indicate whether there is a thread updating the data.
    // This class allows a thread to continue accessing existing data while
    // another thread lock the mutex variable and update the data.  Note
    // that synchronization of the data of an entry is managed by the cache
    // containing the entry.

  public:
    typedef bcema_SharedPtr<const bteso_IpResolutionCache_Data> DataPtr;
        // 'DataPtr' is an alias for a shared pointer to a
        // 'bteso_IpResolutionCache_Data' object (defined in the '.cpp' file)
        // that is used to contain the set of IP addresses for a hostname

  private:
    // DATA
    DataPtr     d_data;          // pointer to a 'bteso_IpResolutionCache_Data'
                                 // object

    bcemt_Mutex d_updatingLock;  // mutex used to signal that a thread is
                                 // retrieving new data (but does *not*
                                 // synchronize access to 'd_data')

  private:
    // NOT IMPLEMENTED
    bteso_IpResolutionCache_Entry& operator=(
                                         const bteso_IpResolutionCache_Entry&);

  public:
    // CREATORS
    bteso_IpResolutionCache_Entry();
        // Create a 'bteso_IpResolutionCache_Entry' object.  By default, this
        // object holds a null reference to a 'bteso_IpResolutionCache_Data'
        // object.

    bteso_IpResolutionCache_Entry(
                                const bteso_IpResolutionCache_Entry& original);
        // Create a 'bteso_IpResolutionCache_Entry' object that refers to the
        // same 'bteso_IpResolutionCache_Data' object as the specified
        // 'original'.  The newly created entry does not share an
        // 'updatingLock' with 'original'.  Note that this copy constructor is
        // provided to allow an entry to be stored in a 'bsl::map'.

    // MANIPULATORS
    void setData(DataPtr value);
        // Make this object refer to the same 'bteso_IpResolutionCache_Data'
        // object as the specified 'value'.  The behavior is undefined unless
        // the calling thread has a write lock on the cache containing this
        // entry.

    void reset();
        // Reset this object to hold a null reference to a
        // 'bteso_IpResolutionCache_Data' object.  The behavior is undefined
        // unless the calling thread has a write lock on the cache containing
        // this entry.

    bcemt_Mutex& updatingLock();
        // Return a reference providing modifiable access to a mutex used to
        // signal a thread is retrieving new 'data'.  Note that 'updatingLock'
        // does *not* synchronize access to 'data'; access to 'data' is
        // synchronized by a read-write mutex in the cache containing this
        // entry.

    // ACCESSORS
    DataPtr data() const;
        // Return a shared pointer to the non-modifiable
        // 'bteso_IpResolutionCache_Data' object referred to by this object.
        // The behavior is undefined unless the calling thread has a read lock
        // on the cache containing this entry.
};

                        // =============================
                        // class bteso_IpResolutionCache
                        // =============================

class bteso_IpResolutionCache {
    // This class provides an efficient mechanism for retrieving the IPv4
    // addresses of a host machine given its hostname.  The first time a client
    // requests the addresses for a hostname, those addresses are retrieved
    // (using the 'resolverCallback' supplied at construction), and cached for
    // future use.  Subsequent requests for the same hostname return the cached
    // information (unless the information is stale).  The cached addresses
    // remains in the cache for a user-defined time, which defaults to one
    // hour.  Stored IP addresses older than the configured interval are
    // considered stale, and a subsequent request for the associated hostname
    // will refresh that set of IP addresses by calling the 'resolverCallback',
    // supplied at construction.
    //
    // This class:
    //: o is *exception-neutral*
    //: o is *fully* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    typedef bteso_ResolveUtil::ResolveByNameCallback ResolveByNameCallback;
        // Alias to the function type of the resolver of 'bteso_ResolveUtil'.

  private:
    // DATA
    typedef bsl::map<bsl::string, bteso_IpResolutionCache_Entry> AddressMap;

    AddressMap             d_cache;             // map to store the data

    bdet_DatetimeInterval  d_timeToLive;        // configured interval for old
                                                // to become stale

    mutable bcemt_RWMutex  d_rwLock;            // access synchronization for
                                                // reading/writing to 'd_cache'
                                                // *and* the shared 'data' in
                                                // the entries of 'd_cache'

    ResolveByNameCallback  d_resolverCallback;  // callback to get data

    bslma_Allocator       *d_allocator_p;       // allocator (held, not owned)

  private:
    int getCacheData(bteso_IpResolutionCache_Entry::DataPtr *result,
                     const char                             *hostname,
                     int                                    *errorCode);
        // Load, into the specified 'result', a 'bcema_SharedPtr' referring to
        // a vector of addresses for the specified 'hostname', and, if an error
        // occurs, load into the specified 'errorCode', the error code returned
        // by the callback function.  If the cache already contains an entry
        // for 'hostname' younger than the configured time-to-live, that entry
        // is loaded into 'result', otherwise the callback supplied at
        // construction is invoked to populate a new entry in the cache, and
        // that entry is then loaded into 'result'.  Return 0 on success, and a
        // non-zero value otherwise.

  private:
    // NOT IMPLEMENTED
    bteso_IpResolutionCache(const bteso_IpResolutionCache&);
    bteso_IpResolutionCache& operator=(const bteso_IpResolutionCache&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_IpResolutionCache,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit bteso_IpResolutionCache(
                                    bslma_Allocator       *basicAllocator = 0);
    explicit bteso_IpResolutionCache(
                                    ResolveByNameCallback  resolverCallback,
                                    bslma_Allocator       *basicAllocator = 0);
        // Create a 'bteso_IpResolutionCache' object.  Optionally specify
        // 'resolverCallback' used to resolve the IP addresses from a hostname.
        // If 'resolverCallback' is not specified,
        // 'bteso_AddressUtil::getAddressDefault' will be used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    // MANIPULATORS
    void removeAll();
        // Remove all cached data.

    int resolveAddress(bsl::vector<bteso_IPv4Address> *result,
                       const char                     *hostname,
                       int                             maxNumAddresses,
                       int                            *errorCode = 0);
        // Load, into the specified 'result', the resolved IPv4 addresses of
        // the host with the specified 'hostname', up to the specified
        // 'maxNumAddresses' on success.  Optionally specify 'errorCode' to
        // store the error code returned by the resolver callback if the data
        // is not cached.  If 'errorCode' is 0, the error code of the callback
        // is ignored.  If the cache already contains an entry for 'hostname'
        // younger than the configured time-to-live, that entry is loaded into
        // 'result', otherwise, the callback supplied at construction is
        // invoked to populate a new entry in the cache, and that entry is then
        // loaded into 'result'.  Return 0 on success with no effect on the
        // 'errorCode', and a non-zero value with no effect on 'result'
        // otherwise.  The behavior is undefined unless '1 <= maxNumAddresses'.

    void setTimeToLive(const bdet_DatetimeInterval& value);
        // Set the time the cached IP addresses for a particular hostname may
        // exist before they are considered stale.  A 'value' of 0 seconds
        // indicates the addresses will never expire.  The behavior is
        // undefined unless '0 <= value.seconds()'.  Note that this function
        // will affect data that is already cached.

    // ACCESSORS
    bslma_Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    int lookupAddressRaw(
                        bsl::vector<bteso_IPv4Address> *result,
                        const char                     *hostname,
                        int                             maxNumAddresses) const;
        // Load, into the specified 'result', the IPv4 addresses of the host
        // with the specified 'hostname' up to the specified 'maxNumAddresses'
        // if the data already exist in the cache (regardless of whether the
        // addresses are stale).  Return 0 if the data has been previously
        // cached (by a call to 'resolveAddress'), and a non-zero value with no
        // effect on 'result' otherwise.  The behavior is undefined unless '1
        // <= maxNumAddresses'.  Note that this method will not refresh the
        // addresses even if they become stale.

    ResolveByNameCallback resolverCallback() const;
        // Return the address of the callback function that is used for
        // resolving the IP addresses from a hostname when the hostname is not
        // already in the cache or the IP addresses are stale.

    const bdet_DatetimeInterval& timeToLive() const;
        // Return a reference providing non-modifiable access to the time a set
        // of IP addresses for a particular hostname may exist in the cache
        // before they become stale.  Note that '0 <= timeToLive().seconds()'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------------------------
                        // class bteso_IpResolutionCache_Entry
                        // -----------------------------------

// CREATORS
inline
bteso_IpResolutionCache_Entry::bteso_IpResolutionCache_Entry()
: d_data()
, d_updatingLock()
{
}

inline
bteso_IpResolutionCache_Entry::bteso_IpResolutionCache_Entry(
                                    const bteso_IpResolutionCache_Entry& other)
: d_data(other.d_data)
, d_updatingLock()
{
}

// MANIPULATORS
inline
void bteso_IpResolutionCache_Entry::setData(DataPtr value)
{
    d_data = value;
}

inline
void bteso_IpResolutionCache_Entry::reset()
{
    d_data.reset();
}

inline
bcemt_Mutex& bteso_IpResolutionCache_Entry::updatingLock()
{
    return d_updatingLock;
}

// ACCESSORS
inline
bteso_IpResolutionCache_Entry::DataPtr bteso_IpResolutionCache_Entry::data()
                                                                          const
{
    return d_data;
}

                        // -----------------------------
                        // class bteso_IpResolutionCache
                        // -----------------------------

// MANIPULATORS

inline
void bteso_IpResolutionCache::setTimeToLive(const bdet_DatetimeInterval& value)
{
    BSLS_ASSERT_SAFE(0 <= value.totalSeconds());

    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);
    d_timeToLive = value;
}

// ACCESSORS
inline
bslma_Allocator *bteso_IpResolutionCache::allocator() const
{
    return d_allocator_p;
}

inline
bteso_IpResolutionCache::ResolveByNameCallback
                              bteso_IpResolutionCache::resolverCallback() const
{
    return d_resolverCallback;
}

inline
const bdet_DatetimeInterval& bteso_IpResolutionCache::timeToLive() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
    return d_timeToLive;
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
