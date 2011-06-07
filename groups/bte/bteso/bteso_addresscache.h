// bteso_addresscache.h                                               -*-C++-*-
#ifndef INCLUDED_BTESO_ADDRESSCACHE
#define INCLUDED_BTESO_ADDRESSCACHE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif

BDES_IDENT("$Id: $")

//@PURPOSE: Provide a cache for IP addresses.
//
//@CLASSES:
//  bteso_AddressCache: a cache for IP addresses.
//
//@AUTHOR: Raymond Chiu (schiu49)
//
//@SEE_ALSO:
//  bteso_resolveutil, bteso_ipv4address
//
//@DESCRIPTION: This component defines a class, 'bteso_AddressCache', that
// serves as a cache of 'bsl::vector<bteso_IPv4Address>' objects.  An address
// cache is supplied, on construction, with a
// 'bteso_ResolveUtil::ResolveByNameCallback' function to obtain IP addresses
// given the hostname.  Invocation of the 'resolveAddress' method for a
// specified hostname returns the addresses of either previously
// cached data, or if that data expired or is not a cache-resident, a newly
// loaded set of IP addresses, which is cached for use in subsequent calls to
// 'resolveAddress'.  Addresses returned are valid for a user-defined time
// period, set by the 'setTimeToLiveInSeconds' method.  After which, the
// addresses will expire.
//
///Thread Safety
///-------------
// 'bteso_AddressCache' is fully *thread-safe*, meaning that all non-creator
// operations on an object can be safely invoked simultaneously from multiple
// threads.
//
///Usage
///-----
// In this section, we show the intended usage of this component.
//
///Example 1: Retrieving the IPv4 addresses of a given host
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a 'bteso_AddressCache', and use it to retrieve
// the IPv4 addresses of several host machines.
//
// First, we create a 'bteso_AddressCache' object.  By default, it will use
// 'bteso_ResolveUtil::defaultResolveByNameCallback' to retrieve addresses that
// are not in the cache:
//..
//  bteso_AddressCache cache;
//..
// Then, we verify the newly constructed cache does not contain the addresses
// of either 'sundev1' and 'ibm1':
//..
//  bsl::vector<bteso_IPv4Address> hostAddresses;
//  assert(0 != cache.lookupAddress(&hostAddresses, "sundev1", 1));
//  assert(0 != cache.lookupAddress(&hostAddresses, "ibm1", 1));
//..
// Next, we call 'resolveAddress' method to retrieve a list of IPv4 address for
// the 'sundev1' machine.  Since this is the first call to the 'resolveAddress'
// method, the default callback function will be invoked to retrieve the
// addresses:
//..
//  int rc = cache.resolveAddress(&hostAddresses, "sundev1", 1);
//  assert(                                  0 == rc);
//  assert(                                  1 == hostAddresses.size());
//  assert(bteso_IPv4Address("172.17.1.20", 0) == hostAddresses[0]);
//..
//  Finally, we verify that subsequent call to 'lookupAddress' returns the
//  cached value for 'sundev1', but not 'ibm1':
//..
//  assert(0 == cache.lookupAddress(&hostAddresses, "sundev1", 1));
//  assert(0 != cache.lookupAddress(&hostAddresses, "ibm1", 1));
//..
//
///Example 2: Using this cache with 'bteso_resolveutil'
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// We can use the cache with the 'bteso_resolveutil' component by setting the
// resolver callback.  When using 'bteso_ResolveUtil::setResolverCallback'
// method, we need a free function instead of a member function of a class.
//
// First, we declared a static variable that will point to a cache:
//..
// static bteso_AddressCache* s_addressCachePtr;
//..
// Then, we create a function using the pointer that was just declared:
//..
// static
// int resolverCallback(bsl::vector<bteso_IPv4Address> *hostAddresses,
//                   const char                     *hostName,
//                   int                             numAddresses,
//                   int                            *errorCode)
// {
//     return s_addressCachePtr->resolveAddress(hostAddresses,
//                                              hostName,
//                                              numAddresses,
//                                              errorCode);
// }
//..
// Next, we create a cache that will be used by the free function, and assign
// the static pointer to refer to this cache.
//..
//  bteso_AddressCache addressCache;
//  s_addressCachePtr = &addressCache;
//..
// Then, we set the callback for 'bteso_resolveutil':
//..
//  bteso_ResolveUtil::setResolveByNameCallback(resolverCallback);
//..
// Finally, we call the 'bteso_ResolveUtil::getAddress' method to retrieve the
// IPv4 address of 'sundev1':
//..
//  bteso_IPv4Address ipv4;
//  bteso_ResolveUtil::getAddress(&ipv4, "sundev1");
//  assert(bteso_IPv4Address("172.17.1.20", 0) == ipv4);
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

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {

class bslma_Allocator;

class bteso_AddressCache;

                        // =============================
                        // class bteso_AddressCacheData
                        // =============================

class bteso_AddressCacheData {
    // This class provides storage for the cached data and a
    // 'bdet_TimeInterval' to indicate the time this data expires.

    bsl::vector<bteso_IPv4Address> d_addresses;       // cached data

    bdet_TimeInterval              d_expirationTime;  // time from epoch until
                                                      // data expires
  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_AddressCache,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATOR
    bteso_AddressCacheData(
                    const bsl::vector<bteso_IPv4Address>&  data,
                    const bdet_TimeInterval&               expirationTime,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create an object storing the specified 'data', which expires at the
        // specified 'expirationTime'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // ACCESSORS
    const bsl::vector<bteso_IPv4Address>& addresses() const;
        // Returns a reference to the non-modifiable 'addresses'.

    const bdet_TimeInterval& expirationTime() const;
        // Returns a reference to the non-modifiable 'expirationTime'.
};

                        // =============================
                        // class bteso_AddressCacheEntry
                        // =============================

class bteso_AddressCacheEntry {
    // This class provides an entry type for the cache.  It contains a shared
    // pointer to the data and the mutex for access synchronization.

  public:
    typedef bcema_SharedPtr<const bteso_AddressCacheData> DataPtr;

  private:
    DataPtr             d_data;  // pointer to cached data
    mutable bcemt_Mutex d_lock;  // access synchronization

  public:
    // CREATORS
    bteso_AddressCacheEntry();
        // Create a 'bteso_AddressCacheEntry' object to be inserted into the
        // cached.  This object contains a shared pointer to refer to a
        // non-modifiable 'bteso_AddressCacheData' object, which stores the
        // actual data.

    bteso_AddressCacheEntry(const bteso_AddressCacheEntry& original);
        // Create a 'bteso_AddressCacheEntry' object that points to the same
        // data as the specified 'original'.

    // ACCESSORS
    DataPtr data() const;
        // Returns a 'bcema_SharedPtr' pointing to a non-modifiable
        // 'bteso_AddressCacheData' that contains the cached data.

    // MANIPULATORS
    void setData(DataPtr value);
        // Set 'd_data' to refer to the same data as 'value'.

    void lock() const;
        // Acquire a lock on this object.  If this object is currently locked
        // by a different thread, then suspend execution of the current thread
        // until a lock can be acquired.  The behavior is undefined if the
        // calling thread already owns the lock on this object, and may result
        // in deadlock.

    void unlock() const;
        // Release a lock on this object that was previously acquired through a
        // call to 'lock', or a successful call to 'tryLock', enabling another
        // thread to acquire a lock on this object.  The behavior is undefined
        // unless the calling thread currently owns the lock on this object.

    int tryLock() const;
        // Attempt to acquire a lock on this object.  Return 0 on success, and
        // a non-zero value if this object is already locked by a different
        // thread.  The behavior is undefined if the calling thread already
        // owns the lock on this object, and may result in deadlock.
};

                        // ========================
                        // class bteso_AddressCache
                        // ========================

class bteso_AddressCache {
    // This class provides an efficient mechanism for retrieving the IPv4
    // address of the host machine given the hostname.  The first time a client
    // requests addresses for some hostnames, that addresses is loaded (using
    // the 'resolverCallback' supplied at construction), returned to the
    // client, and cached for future use.  Subsequent requests for the same
    // hostname return the cached information.  The returned values are valid
    // for a user define time, which is defaulted to one hour.  After which,
    // the addresses will expire, and will have to be loaded again.
    //
    // This class:
    //: o is *exception-neutral*
    //: o is *fully* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    typedef bteso_ResolveUtil::ResolveByNameCallback ResolveByNameCallback;

  private:
    typedef bsl::map<bsl::string, bteso_AddressCacheEntry> AddressMap;

    AddressMap             d_cache;             // map to store the data
    int                    d_timeToLiveInSeconds;
                                                // time before the data expires
    mutable bcemt_RWMutex  d_rwLock;            // access synchronization
    ResolveByNameCallback  d_resolverCallback;  // callback to get data
    bslma_Allocator       *d_allocator_p;       // allocator (held, not owned)

  private:
    int getCacheData(bteso_AddressCacheEntry::DataPtr *result,
                     const char                       *hostname,
                     int                              *errorCode);
        // Load, into the specified 'result', a 'bcema_SharedPtr' pointing to a
        // vector of addresses for the specified 'hostname', and load, into the
        // specified 'errorCode', the error code of the callback if the data is
        // not cached and an error occurred.  It returns 0 with no effect on
        // 'errorCode' upon success, and otherwise returns a negative value.
        // Upon failure and if 'errorCode' is not 0, it also loads a native
        // error code into 'errorCode'.  The behavior is undefined unless
        // '0 < numAddresses'.  Note that, in any case, any 'hostAddresses'
        // entry present upon return must contain a valid IPv4Address
        // corresponding to the 'hostName', and 'hostAddresses' must be resized
        // accordingly.



  private:
    // NOT IMPLEMENTED
    bteso_AddressCache(const bteso_AddressCache&);
    bteso_AddressCache& operator=(const bteso_AddressCache&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_AddressCache,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bteso_AddressCache(bslma_Allocator       *basicAllocator = 0);
    bteso_AddressCache(ResolveByNameCallback  resolverCallback,
                       bslma_Allocator       *basicAllocator = 0);
        // Create a 'bteso_AddressCache' object.  Optionally, specify
        // 'resolverCallback' to be used to resolve the address if hostname is
        // not already in the cache.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  If 'resolverCallback' is not
        // specified, 'bteso_AddressUtil::getAddressDefault' will be used.

    // MANIPULATORS
    void clear();
        // Remove all cached data.

    int resolveAddress(bsl::vector<bteso_IPv4Address> *result,
                       const char                     *hostname,
                       int                             numAddresses,
                       int                            *errorCode = 0);
        // Load, into the specified 'result', the resolved IPv4 address(es) of
        // the specified 'hostname' up to the specified 'numAddresses' on
        // success.  Optionally, specify 'errorCode' to store the error code of
        // the callback if the data is not cached and an error occurred.  If
        // 'errorCode' is 0, the error code of the callback is ignored.
        // Returns '0' on success with no effect on the 'errorCode', and a
        // non-zero value with no effect on 'result' otherwise.  The behavior
        // is undefined unless '0 < numAddresses'.

    ResolveByNameCallback setResolverCallback(ResolveByNameCallback value);
        // Set the function to be used for resolving the address from hostname
        // when the hostname is not already in the cache.

    void setTimeToLiveInSeconds(int value);
        // Set the time a cached data may exist before it expires.  Note that
        // this function does *not* affect data that is already cached.  The
        // behavior is undefined unless 'value < 0'.

    // ACCESSOR
    bslma_Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    int lookupAddress(bsl::vector<bteso_IPv4Address> *result,
                      const char                     *hostname,
                      int                             numAddresses) const;
        // Load, into the specified 'result', the IPv4 addresses of the host
        // with the specified 'hostname' up to the specified 'numAddresses' if
        // the data is already exist in the cache.  Return 0 if the data has
        // been previously cached (by a call to 'resolveAddress'), and a
        // non-zero value with no effect on 'result' otherwise.  The behavior
        // is undefined unless '0 < numAddresses'.

    ResolveByNameCallback resolverCallback() const;
        // Return the callback function to used for resolving the address from
        // hostname when the hostname is not already in the cache.

    int timeToLiveInSeconds() const;
        // Return the value of 'timeToLiveInSeconds'.
};

                        // -----------------------------
                        // class bteso_AddressCacheData
                        // -----------------------------

inline
const bsl::vector<bteso_IPv4Address>& bteso_AddressCacheData::addresses() const
{
    return d_addresses;
}

inline
const bdet_TimeInterval& bteso_AddressCacheData::expirationTime() const
{
    return d_expirationTime;
}

                        // -----------------------------
                        // class bteso_AddressCacheEntry
                        // -----------------------------

// MANIPULATORS
inline
void bteso_AddressCacheEntry::setData(DataPtr value)
{
    d_data = value;
}

inline
void bteso_AddressCacheEntry::lock() const
{
    d_lock.lock();
}

inline
void bteso_AddressCacheEntry::unlock() const
{
    d_lock.unlock();
}

inline
int bteso_AddressCacheEntry::tryLock() const
{
    return d_lock.tryLock();
}

// ACCESSORS
inline
bteso_AddressCacheEntry::DataPtr bteso_AddressCacheEntry::data() const
{
    return d_data;
}

                        // ------------------------
                        // class bteso_AddressCache
                        // ------------------------

// MANIPULATORS
inline
bteso_AddressCache::ResolveByNameCallback
           bteso_AddressCache::setResolverCallback(ResolveByNameCallback value)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);
    d_resolverCallback = value;
    ResolveByNameCallback temp = d_resolverCallback;
    return temp;
}

inline
void bteso_AddressCache::setTimeToLiveInSeconds(int value)
{
    BSLS_ASSERT_SAFE(0 < value);

    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);
    d_timeToLiveInSeconds = value;
}

// ACCESSORS
inline
bslma_Allocator *bteso_AddressCache::allocator() const
{
    return d_allocator_p;
}

inline
bteso_AddressCache::ResolveByNameCallback
                                   bteso_AddressCache::resolverCallback() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
    return d_resolverCallback;
}

inline
int bteso_AddressCache::timeToLiveInSeconds() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
    return d_timeToLiveInSeconds;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
