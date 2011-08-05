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
//@DESCRIPTION: This component defines a class, 'bteso_IpResolutionCache', that
// serves as a cache of 'bsl::vector<bteso_IPv4Address>' objects.  A
// 'bteso_IpResolutionCache' object is supplied a
// 'bteso_ResolveUtil::ResolveByNameCallback' function on construction that it
// uses to obtain the set of IP addresses for a given hostname.  The provided
// 'resolveAddress' method returns a set of IP addresses for the supplied
// hostname, either returning values already residing in the cache, or, if the
// values have expired or are not already present in the cache, invokes the
// supplied 'ResolveByNameCallback' is to obtain a set of IP addresses to
// return, which is also cached for subsequent re-use.  IP addresses for a
// hostname that are stored in the cache are considered valid for a
// user-defined time interval, set by the 'setTimeToLiveInSeconds' method.
// Stored IP addresses older than the configured interval are considered stale,
// and a subsequent request for the associated hostname will refresh that set
// of IP addresses by calling the supplied 'ResolveByNameCallback' again.
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
// First, we create a 'bteso_IpResolutionCache' object.  By default, it will
// use 'bteso_ResolveUtil::defaultResolveByNameCallback' to retrieve addresses
// that are not in the cache:
//..
//  bteso_IpResolutionCache cache;
//..
// Then, we verify the newly constructed cache does not contain the addresses
// of either "www.bloomberg.com" and "www.businessweek.com":
//..
//  bsl::vector<bteso_IPv4Address> ipAddresses;
//  assert(0 != cache.lookupAddress(&ipAddresses, "www.bloomberg.com", 1));
//  assert(0 != cache.lookupAddress(&ipAddresses, "www.businessweek.com", 1));
//..
// Next, we call the 'resolveAddress' method to retrieve one of the IP address
// for "www.bloomberg.com", and then print out the returned address.  Note that
// since this is the first call to 'resolveAddress' for "www.bloomberg.com",
// 'resolverCallback' will be invoked to retrieve the addresses:
//..
//  int rc = cache.resolveAddress(&ipAddresses, "www.bloomberg.com", 1);
//  assert(0 == rc);
//  assert(1 == ipAddresses.size());
//  bsl::cout << "IP Address: " << ipAddresses[0] << std::endl;
//..
//  Finally, we verify that a subsequent call to 'lookupAddress' returns 0 for
//  "www.bloomberg.com" indicating its addresses are stored in the cache, but
//  returns 1 for "www.businessweek.com", indicating its addresses are not
//  stored in the cache:
//..
//  assert(0 == cache.lookupAddress(&ipAddresses, "www.bloomberg.com", 1));
//  assert(0 != cache.lookupAddress(&ipAddresses, "www.businessweek.com", 1));
//..
//
///Example 2: Using Address Cache with 'bteso_ResolveUtil'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to configure the 'bteso_ResolverUtil'
// component to use the cache for resolving IP addresses.
//
// First, we create a cache for the IP addresses:
//..
//  bteso_IpResolutionCache cache;
//..
// Now, we set the callback for 'bteso_ResolveUtil' by using 'bdef_BindUtil':
//..
//  using namespace bdef_PlaceHolders;
//  bteso_ResolveUtil::setResolveByNameCallback(
//                bdef_BindUtil::bind(&bteso_IpResolutionCache::resolveAddress,
//                                    &cache,
//                                    _1,
//                                    _2,
//                                    _3,
//                                    _4));
//..
// Finally, we call the 'bteso_ResolveUtil::getAddress' method to retrieve the
// IPv4 address of 'www.bloomberg.com':
//..
//  bteso_IPv4Address ipAddress;
//  bteso_ResolveUtil::getAddress(&ipAddress, "www.bloomberg.com");
//  bsl::cout << "IP Address: " << ipAddress << std::endl;
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

class bteso_IpResolutionCache;
class bteso_IpResolutionCache_Data;  // defined in .cpp

                        // ===================================
                        // class bteso_IpResolutionCache_Entry
                        // ===================================

class bteso_IpResolutionCache_Entry {
    // This class provides an entry type that can be used in a map.  It
    // contains a shared pointer to a 'bteso_IpResolutionCache_Data' object,
    // which stores a set of IP addresses and their expiration time, and a
    // mutex variable to indicate whether there is a thread updating the data.

  public:
    typedef bcema_SharedPtr<const bteso_IpResolutionCache_Data> DataPtr;
        // Alias for shared pointer to a 'bteso_IpResolutionCache_Data' object.

  private:
    // DATA
    DataPtr             d_data;          // pointer to a
                                         // 'bteso_IpResolutionCache_Data'
                                         // object

    mutable bcemt_Mutex d_updatingLock;  // this mutex is used to signal that a
                                         // thread is retrieving new data, but
                                         // does *not* synchronize access to
                                         // 'd_data'

  private:
    // NOT IMPLEMENTED
    bteso_IpResolutionCache_Entry& operator=(
                                         const bteso_IpResolutionCache_Entry&);

  public:
    // CREATORS
    bteso_IpResolutionCache_Entry();
        // Create a 'bteso_IpResolutionCache_Entry' object.  By default, 'data'
        // is an empty shared pointer.

    bteso_IpResolutionCache_Entry(
                                const bteso_IpResolutionCache_Entry& original);
        // Create a 'bteso_IpResolutionCache_Entry' object that points to the
        // same data as the specified 'original'.  Note that this copy
        // constructor is needed for its use in a 'bsl::map'.

    // MANIPULATORS
    void setData(DataPtr value);
        // Set the shared pointer, 'data', to refer to the same
        // 'bteso_IpResolutionCache_Data' object as the specified 'value'.

    bcemt_Mutex& updatingLock() const;
        // Return a reference providing modifiable access to the mutex variable
        // of this object.  Locking the mutex variable signals this thread is
        // retrieving new data, but does *not* synchronize access to 'd_data'.

    void reset();
        // Reset the shared pointer, 'data', to the empty state.

    // ACCESSORS
    DataPtr data() const;
        // Return a shared pointer to the non-modifiable
        // 'bteso_IpResolutionCache_Data' referred to by this object.
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
    // information.  The cached addresses remains in the cache for a
    // user-defined time, which defaults to one hour.  Stored IP addresses
    // older than the configured interval are considered stale, and a
    // subsequent request for the associated hostname will refresh that set of
    // IP addresses by calling the supplied 'resolverCallback' again.
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

    int                    d_timeToLiveInSeconds;
                                                // time before the data expires

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
        // Load, into the specified 'result', a 'bcema_SharedPtr' pointing to a
        // vector of addresses for the specified 'hostname', and load, into the
        // specified 'errorCode', the error code returned by the callback
        // function (if the data is not cached).  Return 0 on success, and a
        // negative value otherwise.

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
    void clear();
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
        // is ignored.  Return '0' on success with no effect on the
        // 'errorCode', and a non-zero value with no effect on 'result'
        // otherwise.  The behavior is undefined unless '0 < maxNumAddresses'.

    void setTimeToLiveInSeconds(int value);
        // Set the time the cached IP addresses for a particular hostname may
        // exist before they are considered stale.  The behavior is undefined
        // unless '0 < value'.  Note that this function does *not* affect data
        // that is already cached.

    // ACCESSORS
    bslma_Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    int lookupAddress(bsl::vector<bteso_IPv4Address> *result,
                      const char                     *hostname,
                      int                             maxNumAddresses) const;
        // Load, into the specified 'result', the IPv4 addresses of the host
        // with the specified 'hostname' up to the specified 'maxNumAddresses'
        // if the data already exist in the cache.  Return 0 if the data has
        // been previously cached (by a call to 'resolveAddress'), and a
        // non-zero value with no effect on 'result' otherwise.  The behavior
        // is undefined unless '0 < maxNumAddresses'.  Note that this method
        // will not refresh the addresses even if they become stale.

    ResolveByNameCallback resolverCallback() const;
        // Return the callback function that will be used for resolving the IP
        // addresses from a hostname when the hostname is not already in the
        // cache.

    int timeToLiveInSeconds() const;
        // Return the time the set of IP addresses for a particular hostname
        // may exist in the cache before they become stale.  Note that
        // '0 < timeToLiveInSeconds()'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------------------------
                        // class bteso_IpResolutionCache_Entry
                        // -----------------------------------

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
bcemt_Mutex& bteso_IpResolutionCache_Entry::updatingLock() const
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
void bteso_IpResolutionCache::setTimeToLiveInSeconds(int value)
{
    BSLS_ASSERT_SAFE(0 < value);

    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);
    d_timeToLiveInSeconds = value;
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
    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
    return d_resolverCallback;
}

inline
int bteso_IpResolutionCache::timeToLiveInSeconds() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
    return d_timeToLiveInSeconds;
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
