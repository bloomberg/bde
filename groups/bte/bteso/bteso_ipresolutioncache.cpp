// bteso_ipresolutioncache.cpp                                        -*-C++-*-
#include <bteso_ipresolutioncache.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID("bteso_ipresolutioncache.cpp","$Id$ $CSID$")

#include <bcemt_lockguard.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>

#include <bslma_default.h>

#include <bdetu_systemtime.h>

namespace BloombergLP {

static
int createCacheData(
           bteso_IpResolutionCache_Entry::DataPtr          *result,
           const char                                      *hostname,
           int                                             *errorCode,
           int                                              timeToLive,
           bteso_IpResolutionCache::ResolveByNameCallback  resolverCallback,
           bslma_Allocator                                 *basicAllocator = 0)
    // Load, into the specified 'result', a shared pointer to a newly created
    // 'bteso_IpResolutionCache_Data' object containing the IPv4 addresses of
    // the host with the specified 'hostname' (retrieved using the specified
    // 'resolverCallback'), which will expire after the number of seconds
    // indicated by the specified 'timeToLive', and load, into the specified
    // 'errorCode', the error code of 'resolverCallback' if it fails.
    // Optionally specify a 'basicAllocator' used to supply memory.  If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used.
{
    bsl::vector<bteso_IPv4Address> hostAddresses;
    int rc = resolverCallback(&hostAddresses, hostname, INT_MAX, errorCode);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bdet_TimeInterval expirationTime = bdetu_SystemTime::now();
    expirationTime.addSeconds(timeToLive);

    result->createInplace(basicAllocator,
                          hostAddresses,
                          expirationTime,
                          basicAllocator);
    return 0;
}

                        // ==================================
                        // class bteso_IpResolutionCache_Data
                        // ==================================

class bteso_IpResolutionCache_Data {
    // This class provides storage for the a set of IP addresses and a
    // 'bdet_TimeInterval' to indicate the time these addresses expires.

    // DATA
    bsl::vector<bteso_IPv4Address> d_addresses;       // set of IP addresses

    bdet_TimeInterval              d_expirationTime;  // time from epoch until
                                                      // this data expires
  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_IpResolutionCache,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATOR
    bteso_IpResolutionCache_Data(
                    const bsl::vector<bteso_IPv4Address>&  ipAddresses,
                    const bdet_TimeInterval&               expirationTime,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create an object storing the specified 'ipAddresses', which expires
        // at the specified 'expirationTime' (expressed as the !ABSOLUTE! time
        // from 00:00:00 UTC, January 1, 1970).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // ACCESSORS
    const bsl::vector<bteso_IPv4Address>& addresses() const;
        // Return a reference providing non-modifiable access to the IP
        // addresses stored in this object.

    const bdet_TimeInterval& expirationTime() const;
        // Return a reference providing non-modifiable access to the time
        // (expressed as the !ABSOLUTE! time from 00:00:00 UTC, January 1,
        // 1970) addresses in the object expires.
};

                        // ----------------------------------
                        // class bteso_IpResolutionCache_Data
                        // ----------------------------------

// CREATORS
bteso_IpResolutionCache_Data::bteso_IpResolutionCache_Data(
                     const bsl::vector<bteso_IPv4Address>&  addresses,
                     const bdet_TimeInterval&               expirationTime,
                     bslma_Allocator                       *basicAllocator)
: d_addresses(addresses, basicAllocator)
, d_expirationTime(expirationTime)
{
}
                        // -----------------------------------
                        // class bteso_IpResolutionCache_Entry
                        // -----------------------------------

// CREATORS
bteso_IpResolutionCache_Entry::bteso_IpResolutionCache_Entry()
: d_data()
, d_updatingLock()
{
}

bteso_IpResolutionCache_Entry::bteso_IpResolutionCache_Entry(
                                   const bteso_IpResolutionCache_Entry&  other)
: d_data(other.d_data)
, d_updatingLock()
{
}

// ACCESSORS
const bsl::vector<bteso_IPv4Address>& bteso_IpResolutionCache_Data::addresses()
                                                                          const
{
    return d_addresses;
}

const bdet_TimeInterval& bteso_IpResolutionCache_Data::expirationTime() const
{
    return d_expirationTime;
}

                        // ------------------------------
                        // class bteso_IpResolutionCache
                        // ------------------------------

// CREATORS
bteso_IpResolutionCache::bteso_IpResolutionCache(
                                               bslma_Allocator *basicAllocator)
: d_cache(basicAllocator)
, d_timeToLiveInSeconds(3600)
, d_rwLock()
, d_resolverCallback(bteso_ResolveUtil::defaultResolveByNameCallback())
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bteso_IpResolutionCache::bteso_IpResolutionCache(
                                       ResolveByNameCallback  resolverCallback,
                                       bslma_Allocator       *basicAllocator)
: d_cache(basicAllocator)
, d_timeToLiveInSeconds(3600)
, d_rwLock()
, d_resolverCallback(resolverCallback)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

// MANIPULATORS
int bteso_IpResolutionCache::getCacheData(
                             bteso_IpResolutionCache_Entry::DataPtr *result,
                             const char                             *hostname,
                             int                                    *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(errorCode);

    bteso_IpResolutionCache_Entry::DataPtr dataPtr;
    bteso_IpResolutionCache_Entry *entry = 0;

    {
        bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);

        AddressMap::iterator it = d_cache.find(hostname);
        if (d_cache.end() == it) {
            // The IP addresses of 'hostname' has never been cached.

            bcemt_ReadLockGuardUnlock<bcemt_RWMutex> readLockGuard(&d_rwLock);

            // Acquire write lock to create entry for the map.

            bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);

            // Try to find 'hostname' again in case another thread added the
            // entry while trying to acquire the write lock.

            it = d_cache.lower_bound(hostname);
            if (d_cache.end() == it
             || d_cache.key_comp()(hostname, it->first)) {
                it = d_cache.insert(
                        it,
                        bsl::pair<bsl::string, bteso_IpResolutionCache_Entry> (
                                             hostname,
                                             bteso_IpResolutionCache_Entry()));
            }

            // Read lock is re-acquired on the destruction of 'readLockGuard'.
        }

        entry = &it->second;

        dataPtr = entry->data();
        if (dataPtr.ptr()) {
            // Check if the data is expired and, if it is, try to acquire the
            // lock to indicate this thread is refreshing the data.

            const bdet_TimeInterval now = bdetu_SystemTime::now();
            if (now <= dataPtr->expirationTime()
             || 0 != entry->updatingLock().tryLock()) {
                // Data is not expired or another thread is already refreshing
                // the data.  Return existing data.

                *result = dataPtr;
                return 0;                                             // RETURN
            }
        }
        else {
            // Data does not exist.

            readLockGuard.release()->unlock();

            // Lock the entry to indicate the data is being acquired.

            entry->updatingLock().lock();

            dataPtr = entry->data();
            if (0 != dataPtr.ptr()) {
                // Another thread updated the entry during acquisition of the
                // lock.

                entry->updatingLock().unlock();

                bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
                *result = entry->data();
                return 0;                                             // RETURN
            }
        }
    }

    // Create a lock guard for the lock.  The lock should have already been
    // acquired.

    bcemt_LockGuard<bcemt_Mutex> updatingLockGuard(&entry->updatingLock(),
                                                   true);
    dataPtr = entry->data();

    int rc = createCacheData(&dataPtr,
                             hostname,
                             errorCode,
                             d_timeToLiveInSeconds,
                             d_resolverCallback,
                             d_allocator_p);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);

    entry->setData(dataPtr);
    *result = dataPtr;
    return 0;
}

int bteso_IpResolutionCache::resolveAddress(
                               bsl::vector<bteso_IPv4Address> *result,
                               const char                     *hostname,
                               int                             maxNumAddresses,
                               int                            *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(0 < maxNumAddresses);

    bteso_IpResolutionCache_Entry::DataPtr dataPtr;

    int localErrorCode;
    if (0 == errorCode) {
        errorCode = &localErrorCode;
    }

    int rc = getCacheData(&dataPtr, hostname, errorCode);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    int size = bsl::min(maxNumAddresses, (int)dataPtr->addresses().size());
    result->resize(size);

    bsl::copy(dataPtr->addresses().begin(),
              dataPtr->addresses().begin() + size,
              result->begin());
    return 0;
}

void bteso_IpResolutionCache::clear()
{
    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);

    for (AddressMap::iterator it = d_cache.begin();
         it != d_cache.end();
         ++it) {
        it->second.reset();
    }
}

// ACCESSORS
int bteso_IpResolutionCache::lookupAddress(
                         bsl::vector<bteso_IPv4Address> *result,
                         const char                     *hostname,
                         int                             maxNumAddresses) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(0 < maxNumAddresses);

    bteso_IpResolutionCache_Entry::DataPtr dataPtr;

    {
        bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
        AddressMap::const_iterator it = d_cache.find(hostname);
        if (d_cache.end() == it) {
            return -1;                                                // RETURN
        }
        dataPtr = it->second.data();
    }

    if (0 == dataPtr.ptr()) {
        return -1;                                                    // RETURN
    }

    int size = bsl::min(maxNumAddresses, (int)dataPtr->addresses().size());
    result->resize(size);
    bsl::copy(dataPtr->addresses().begin(),
              dataPtr->addresses().begin() + size,
              result->begin());
    return 0;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
