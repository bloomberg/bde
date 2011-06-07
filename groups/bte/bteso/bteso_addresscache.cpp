// bteso_addresscache.cpp                                             -*-C++-*-
#include <bteso_addresscache.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID("bteso_addresscache.cpp","$Id$ $CSID$")

#include <bcemt_lockguard.h>

#include <bcemt_threadattributes.h>  // For testing only
#include <bcemt_threadutil.h>  // For testing only
#include <bces_atomictypes.h>  // For testing only
#include <bsls_byteorder.h>  // For testing only

#include <bdetu_systemtime.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bslma_default.h>

namespace BloombergLP {

static
int createCacheData(
                 bteso_AddressCacheEntry::DataPtr          *result,
                 const char                                *hostname,
                 int                                       *errorCode,
                 int                                        timeToLive,
                 bteso_AddressCache::ResolveByNameCallback  resolverCallback,
                 bslma_Allocator                           *basicAllocator = 0)
    // Load, into the specified 'result', a shared pointer to a newly created
    // 'bteso_AddressCacheData' object containing the IPv4 addresses of host
    // with the specified 'hostname' (retrieved using the specified
    // 'resolverCallback'), which will expire after the number of seconds
    // indicated by the specified 'timeToLive', and load, into the specified
    // 'errorCode', the error code of 'resolverCallback' if it fails.
    // Optionally specify a 'basicAllocator' used to supply memory.  If
    // 'basicAllocator' is 0, the currently installed default allocator is
    // used.
{
    bsl::vector<bteso_IPv4Address> hostAddresses;
    int rc = resolverCallback(&hostAddresses, hostname, 128, errorCode);
    if (rc) {
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

                        // ----------------------------
                        // class bteso_AddressCacheData
                        // ----------------------------

// CREATORS
bteso_AddressCacheData::bteso_AddressCacheData(
                     const bsl::vector<bteso_IPv4Address>&  addresses,
                     const bdet_TimeInterval&               expirationTime,
                     bslma_Allocator                       *basicAllocator)
: d_addresses(addresses, basicAllocator)
, d_expirationTime(expirationTime)
{
}
                        // -----------------------------
                        // class bteso_AddressCacheEntry
                        // -----------------------------

// CREATORS
bteso_AddressCacheEntry::bteso_AddressCacheEntry()
: d_data()
, d_lock()
{
}

bteso_AddressCacheEntry::bteso_AddressCacheEntry(
                                         const bteso_AddressCacheEntry&  other)
: d_data(other.d_data)
, d_lock()
{
}

                        // ------------------------
                        // class bteso_AddressCache
                        // ------------------------

// CREATORS
bteso_AddressCache::bteso_AddressCache(bslma_Allocator *basicAllocator)
: d_cache(basicAllocator)
, d_timeToLiveInSeconds(3600)
, d_rwLock()
, d_resolverCallback(bteso_ResolveUtil::defaultResolveByNameCallback())
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bteso_AddressCache::bteso_AddressCache(ResolveByNameCallback  resolverCallback,
                                       bslma_Allocator       *basicAllocator)
: d_cache(basicAllocator)
, d_timeToLiveInSeconds(3600)
, d_rwLock()
, d_resolverCallback(resolverCallback)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

// MANIPULATORS
int bteso_AddressCache::getCacheData(
                                   bteso_AddressCacheEntry::DataPtr *result,
                                   const char                       *hostname,
                                   int                              *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(errorCode);

    bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);

    AddressMap::iterator it = d_cache.find(hostname);
    if (d_cache.end() == it) {
        // Key has never been cached

        bcemt_ReadLockGuardUnlock<bcemt_RWMutex> readLockGuard(&d_rwLock);

        // Acquire write lock to create entry for the map.

        bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);

        it = d_cache.lower_bound(hostname);
        if (d_cache.end() == it || d_cache.key_comp()(hostname, it->first)) {
            // 'hostname' still have not been added to the map during the
            // acquisition of the write-lock on 'd_rwLock'.

            it = d_cache.insert(
                              it,
                              bsl::pair<bsl::string, bteso_AddressCacheEntry> (
                                                   hostname,
                                                   bteso_AddressCacheEntry()));
        }

        // Read lock is re-acquired on the destruction of 'readLockGuard'.
    }

    bteso_AddressCacheEntry *entry = &it->second;

    bteso_AddressCacheEntry::DataPtr dataPtr = entry->data();

    if (0 == dataPtr.ptr()) {
        // Release read lock in case the thread is blocked on trying to acquire
        // the lock for 'entry'.

        readLockGuard.release()->unlock();

        bcemt_LockGuard<bteso_AddressCacheEntry> entryLock(entry);

        dataPtr = entry->data();
        if (0 == dataPtr.ptr()) {
            int rc = createCacheData(&dataPtr,
                                     hostname,
                                     errorCode,
                                     d_timeToLiveInSeconds,
                                     d_resolverCallback,
                                     d_allocator_p);
            if (rc) {
                return rc;                                            // RETURN
            }

            bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);

            entry->setData(dataPtr);
            *result = dataPtr;
            return 0;                                                 // RETURN
        }
        else {
            // Another thread updated the entry during acquisition of
            // 'entryLock'.

            bcemt_ReadLockGuard<bcemt_RWMutex> readLockGuard(&d_rwLock);
            *result = entry->data();
            return 0;                                                 // RETURN
        }
    }

    const bdet_TimeInterval now = bdetu_SystemTime::now();

    if (dataPtr->expirationTime() <= now) {
        // Data has expired.  Try to lock entry for update.

        bcemt_LockGuardTryLock<bteso_AddressCacheEntry> entryLock(entry);
        if (entryLock.ptr()) {
            // Update data if 'tryLock' succeed.  If 'tryLock' fails, another
            // thread is already updating the entry.  Use the expired data
            // instead of waiting for the update.

            dataPtr = entry->data();
            if (dataPtr->expirationTime() <= now) {
                readLockGuard.release()->unlock();

                int rc = createCacheData(&dataPtr,
                                         hostname,
                                         errorCode,
                                         d_timeToLiveInSeconds,
                                         d_resolverCallback,
                                         d_allocator_p);
                if (rc) {
                    return rc;                                        // RETURN
                }

                bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);
                entry->setData(dataPtr);
            }
        }
    }

    *result = dataPtr;
    return 0;
}

int bteso_AddressCache::resolveAddress(
                                 bsl::vector<bteso_IPv4Address> *result,
                                 const char                     *hostname,
                                 int                             numAddresses,
                                 int                            *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(0 < numAddresses);

    bteso_AddressCacheEntry::DataPtr dataPtr;

    int err;
    if (0 == errorCode) {
        errorCode = &err;
    }

    int rc = getCacheData(&dataPtr, hostname, errorCode);
    if (rc) {
        return rc;                                                    // RETURN
    }

    int size = bsl::min(numAddresses, (int)dataPtr->addresses().size());
    result->resize(size);
    std::vector<bteso_IPv4Address>::const_iterator src =
                                                  dataPtr->addresses().begin();
    std::vector<bteso_IPv4Address>::iterator dst = result->begin();
    for(int i=0; i<size; ++i) {
        *dst++ = *src++;
    }

    return 0;
}

void bteso_AddressCache::clear()
{
    bcemt_WriteLockGuard<bcemt_RWMutex> writeLockGuard(&d_rwLock);
    d_cache.clear();
}

// ACCESSORS
int bteso_AddressCache::lookupAddress(
                            bsl::vector<bteso_IPv4Address> *result,
                            const char                     *hostname,
                            int                             numAddresses) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(0 < numAddresses);

    bteso_AddressCacheEntry::DataPtr dataPtr;

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

    int size = bsl::min(numAddresses, (int)dataPtr->addresses().size());
    result->resize(size);
    std::vector<bteso_IPv4Address>::const_iterator src =
                                                  dataPtr->addresses().begin();
    std::vector<bteso_IPv4Address>::iterator dst = result->begin();
    for(int i=0; i<size; ++i) {
        *dst++ = *src++;
    }
    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
