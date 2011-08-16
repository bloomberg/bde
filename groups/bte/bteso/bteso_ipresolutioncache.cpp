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
           bteso_IpResolutionCache_Entry::DataPtr         *result,
           const char                                     *hostname,
           int                                            *errorCode,
           const bdet_Datetime&                            currentTime,
           bteso_IpResolutionCache::ResolveByNameCallback  resolverCallback,
           bslma_Allocator                                *basicAllocator)
    // Load, into the specified 'result', a shared pointer to a newly created
    // 'bteso_IpResolutionCache_Data' object (using the specified
    // 'basicAllocator' to supply memory) containing the IPv4 addresses of the
    // host having the specified 'hostname' (retrieved using the specified
    // 'resolverCallback') and having the specified 'currentTime' as the load
    // time, and load into the specified 'errorCode', the error code of
    // 'resolverCallback' if it fails.  The behavior is undefined if
    // 'resolverCallback' is empty.
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(resolverCallback);
    BSLS_ASSERT(basicAllocator);

    bsl::vector<bteso_IPv4Address> hostAddresses;

    int rc = resolverCallback(&hostAddresses, hostname, INT_MAX, errorCode);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    result->createInplace(basicAllocator,
                          hostAddresses,
                          currentTime,
                          basicAllocator);
    return 0;
}

                        // ==================================
                        // class bteso_IpResolutionCache_Data
                        // ==================================

class bteso_IpResolutionCache_Data {
    // This class provides storage for a set of IP addresses and a
    // 'bdet_Datetime' to indicate the time these addresses were populated.

    // DATA
    bsl::vector<bteso_IPv4Address> d_addresses;     // set of IP addresses

    bdet_Datetime                  d_creationTime;  // time at which this
                                                    // object is created
  private:
    // NOT IMPLEMENTED
    bteso_IpResolutionCache_Data(const bteso_IpResolutionCache_Data&);
    bteso_IpResolutionCache_Data& operator=(
                                          const bteso_IpResolutionCache_Data&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_IpResolutionCache,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATOR
    bteso_IpResolutionCache_Data(
                    const bsl::vector<bteso_IPv4Address>&  ipAddresses,
                    const bdet_Datetime&                   creationTime,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create an object storing the specified 'ipAddresses', and having the
        // specified 'creationTime'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // ACCESSORS
    const bsl::vector<bteso_IPv4Address>& addresses() const;
        // Return a reference providing non-modifiable access to the IP
        // addresses stored in this object.

    const bdet_Datetime& creationTime() const;
        // Return a reference providing non-modifiable access to the time this
        // object was created.
};

                        // ----------------------------------
                        // class bteso_IpResolutionCache_Data
                        // ----------------------------------

// CREATORS
bteso_IpResolutionCache_Data::bteso_IpResolutionCache_Data(
                     const bsl::vector<bteso_IPv4Address>&  addresses,
                     const bdet_Datetime&                   creationTime,
                     bslma_Allocator                       *basicAllocator)
: d_addresses(addresses, basicAllocator)
, d_creationTime(creationTime)
{
}

// ACCESSORS
const bsl::vector<bteso_IPv4Address>& bteso_IpResolutionCache_Data::addresses()
                                                                          const
{
    return d_addresses;
}

const bdet_Datetime& bteso_IpResolutionCache_Data::creationTime() const
{
    return d_creationTime;
}

                        // ------------------------------
                        // class bteso_IpResolutionCache
                        // ------------------------------

// CREATORS
bteso_IpResolutionCache::bteso_IpResolutionCache(
                                               bslma_Allocator *basicAllocator)
: d_cache(basicAllocator)
, d_timeToLive(0, 1)
, d_rwLock()
, d_resolverCallback(bteso_ResolveUtil::defaultResolveByNameCallback())
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bteso_IpResolutionCache::bteso_IpResolutionCache(
                                       ResolveByNameCallback  resolverCallback,
                                       bslma_Allocator       *basicAllocator)
: d_cache(basicAllocator)
, d_timeToLive(0, 1)
, d_rwLock()
, d_resolverCallback(resolverCallback)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(resolverCallback);
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

    const bdet_Datetime now = bdetu_SystemTime::nowAsDatetimeGMT();

    {
        bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);

        AddressMap::iterator it = d_cache.find(hostname);
        if (d_cache.end() == it) {
            // The IP addresses of 'hostname' has never been cached.

            bcemt_ReadLockGuardUnlock<bcemt_RWMutex>
                                                    readUnlockGuard(&d_rwLock);

            // Acquire write lock to create entry for the map.

            bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);

            entry = &d_cache[hostname];

            // Read lock is re-acquired on the destruction of 'readLockGuard'.
        }
        else {
            entry = &it->second;
        }

        dataPtr = entry->data();
        if (dataPtr.ptr()) {
            // Check if the data is expired and, if it is, try to acquire the
            // 'updatingLock' of 'entry' to indicate this thread is refreshing
            // the data.

            if (0 == d_timeToLive.totalSeconds()
             || now < dataPtr->creationTime() + d_timeToLive
             || 0 != entry->updatingLock().tryLock()) {
                // Data is not expired or another thread is already refreshing
                // the data.  Return existing data.

                *result = dataPtr;
                return 0;                                             // RETURN
            }
        }
        else {
            // Data has never been loaded.

            {
                bcemt_ReadLockGuardUnlock<bcemt_RWMutex>
                                                    readUnlockGuard(&d_rwLock);

                // Lock the entry's 'updatingLock' to indicate the data is
                // being acquired.

                entry->updatingLock().lock();
            }

            dataPtr = entry->data();
            if (0 != dataPtr.ptr()) {
                // Another thread updated the entry during acquisition of the
                // lock.

                entry->updatingLock().unlock();

                *result = dataPtr;
                return 0;                                             // RETURN
            }
        }
    }

    // Either the data does not exist or has expired, and we have already
    // acquired the 'updatingLock' for the entry (indicating this thread
    // should update the entry).

    bcemt_LockGuard<bcemt_Mutex> updatingLockGuard(&entry->updatingLock(),
                                                   true);
    dataPtr = entry->data();

    int rc = createCacheData(&dataPtr,
                             hostname,
                             errorCode,
                             now,
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
    BSLS_ASSERT(1 <= maxNumAddresses);

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

void bteso_IpResolutionCache::removeAll()
{
    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);

    for (AddressMap::iterator it = d_cache.begin();
                              it != d_cache.end();
                              ++it) {
        it->second.reset();
    }
}

// ACCESSORS
int bteso_IpResolutionCache::lookupAddressRaw(
                         bsl::vector<bteso_IPv4Address> *result,
                         const char                     *hostname,
                         int                             maxNumAddresses) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(1 <= maxNumAddresses);

    enum {
        // Return values

        SUCCESS = 0,
        FAILURE = -1
    };

    bteso_IpResolutionCache_Entry::DataPtr dataPtr;

    {
        bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
        AddressMap::const_iterator it = d_cache.find(hostname);
        if (d_cache.end() == it) {
            return FAILURE;                                           // RETURN
        }
        dataPtr = it->second.data();
    }

    if (0 == dataPtr.ptr()) {
        return FAILURE;                                               // RETURN
    }

    int size = bsl::min(maxNumAddresses, (int)dataPtr->addresses().size());
    result->resize(size);
    bsl::copy(dataPtr->addresses().begin(),
              dataPtr->addresses().begin() + size,
              result->begin());
    return SUCCESS;
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
