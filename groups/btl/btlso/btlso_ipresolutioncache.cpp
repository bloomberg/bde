// btlso_ipresolutioncache.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_ipresolutioncache.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID("btlso_ipresolutioncache.cpp","$Id$ $CSID$")

#include <bslmt_lockguard.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>

#include <bslma_default.h>

#include <bdlt_currenttime.h>

namespace BloombergLP {

static
int createCacheData(
           btlso::IpResolutionCache_Entry::DataPtr         *result,
           const char                                      *hostname,
           int                                             *errorCode,
           const bdlt::Datetime&                            currentTime,
           btlso::IpResolutionCache::ResolveByNameCallback  resolverCallback,
           bslma::Allocator                                *basicAllocator)
    // Load, into the specified 'result', a shared pointer to a newly created
    // 'bteso::IpResolutionCache_Data' object (using the specified
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

    bsl::vector<btlso::IPv4Address> hostAddresses;

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

namespace btlso {

                        // ----------------------------
                        // class IpResolutionCache_Data
                        // ----------------------------

class IpResolutionCache_Data {
    // This class provides storage for a set of IP addresses and a
    // 'bdlt::Datetime' to indicate the time these addresses were populated.

    // DATA
    bsl::vector<IPv4Address> d_addresses;     // set of IP addresses

    bdlt::Datetime           d_creationTime;  // time at which this
                                              // object is created
  private:
    // NOT IMPLEMENTED
    IpResolutionCache_Data(const IpResolutionCache_Data&);
    IpResolutionCache_Data& operator=(const IpResolutionCache_Data&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(IpResolutionCache,
                                   bslma::UsesBslmaAllocator);

    // CREATOR
    IpResolutionCache_Data(
                          const bsl::vector<IPv4Address>&  ipAddresses,
                          const bdlt::Datetime&            creationTime,
                          bslma::Allocator                *basicAllocator = 0);
        // Create an object storing the specified 'ipAddresses', and having the
        // specified 'creationTime'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // ACCESSORS
    const bsl::vector<IPv4Address>& addresses() const;
        // Return a reference providing non-modifiable access to the IP
        // addresses stored in this object.

    const bdlt::Datetime& creationTime() const;
        // Return a reference providing non-modifiable access to the time this
        // object was created.
};

// CREATORS
IpResolutionCache_Data::IpResolutionCache_Data(
                               const bsl::vector<IPv4Address>&  addresses,
                               const bdlt::Datetime&            creationTime,
                               bslma::Allocator                *basicAllocator)
: d_addresses(addresses, basicAllocator)
, d_creationTime(creationTime)
{
}

// ACCESSORS
const bsl::vector<IPv4Address>& IpResolutionCache_Data::addresses() const
{
    return d_addresses;
}

const bdlt::Datetime& IpResolutionCache_Data::creationTime() const
{
    return d_creationTime;
}

                        // -----------------------
                        // class IpResolutionCache
                        // -----------------------

// CREATORS
IpResolutionCache::IpResolutionCache(bslma::Allocator *basicAllocator)
: d_cache(basicAllocator)
, d_timeToLive(0, 1)
, d_rwLock()
, d_resolverCallback(ResolveUtil::defaultResolveByNameCallback())
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

IpResolutionCache::IpResolutionCache(ResolveByNameCallback  resolverCallback,
                                     bslma::Allocator      *basicAllocator)
: d_cache(basicAllocator)
, d_timeToLive(0, 1)
, d_rwLock()
, d_resolverCallback(resolverCallback)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(resolverCallback);
}

// MANIPULATORS
int IpResolutionCache::getCacheData(
                                   IpResolutionCache_Entry::DataPtr *result,
                                   const char                       *hostname,
                                   int                              *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(errorCode);

    IpResolutionCache_Entry::DataPtr dataPtr;
    IpResolutionCache_Entry *entry = 0;

    const bdlt::Datetime now = bdlt::CurrentTime::utc();

    {
        bslmt::ReadLockGuard<bslmt::RWMutex> readLockGuard(&d_rwLock);

        AddressMap::iterator it = d_cache.find(hostname);
        if (d_cache.end() == it) {
            // The IP addresses of 'hostname' has never been cached.

            bslmt::ReadLockGuardUnlock<bslmt::RWMutex>
                                                    readUnlockGuard(&d_rwLock);

            // Acquire write lock to create entry for the map.

            bslmt::WriteLockGuard<bslmt::RWMutex> writeLockGuard(&d_rwLock);

            entry = &d_cache[hostname];

            // Read lock is re-acquired on the destruction of 'readLockGuard'.
        }
        else {
            entry = &it->second;
        }

        dataPtr = entry->data();
        if (dataPtr.get()) {
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
                bslmt::ReadLockGuardUnlock<bslmt::RWMutex>
                                                    readUnlockGuard(&d_rwLock);

                // Lock the entry's 'updatingLock' to indicate the data is
                // being acquired.

                entry->updatingLock().lock();
            }

            dataPtr = entry->data();
            if (0 != dataPtr.get()) {
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

    bslmt::LockGuard<bslmt::Mutex> updatingLockGuard(&entry->updatingLock(),
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

    bslmt::WriteLockGuard<bslmt::RWMutex> writeLockGuard(&d_rwLock);

    entry->setData(dataPtr);
    *result = dataPtr;
    return 0;
}

int IpResolutionCache::resolveAddress(
                                     bsl::vector<IPv4Address> *result,
                                     const char               *hostname,
                                     int                       maxNumAddresses,
                                     int                      *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(1 <= maxNumAddresses);

    IpResolutionCache_Entry::DataPtr dataPtr;

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

void IpResolutionCache::removeAll()
{
    bslmt::WriteLockGuard<bslmt::RWMutex> writeLockGuard(&d_rwLock);

    for (AddressMap::iterator it = d_cache.begin();
                              it != d_cache.end();
                              ++it) {
        it->second.reset();
    }
}

// ACCESSORS
int IpResolutionCache::lookupAddressRaw(
                               bsl::vector<IPv4Address> *result,
                               const char               *hostname,
                               int                       maxNumAddresses) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(1 <= maxNumAddresses);

    enum {
        // Return values

        SUCCESS = 0,
        FAILURE = -1
    };

    IpResolutionCache_Entry::DataPtr dataPtr;

    {
        bslmt::ReadLockGuard<bslmt::RWMutex> readLockGuard(&d_rwLock);
        AddressMap::const_iterator           it = d_cache.find(hostname);
        if (d_cache.end() == it) {
            return FAILURE;                                           // RETURN
        }
        dataPtr = it->second.data();
    }

    if (0 == dataPtr.get()) {
        return FAILURE;                                               // RETURN
    }

    int size = bsl::min(maxNumAddresses,
                        static_cast<int>(dataPtr->addresses().size()));

    result->resize(size);
    bsl::copy(dataPtr->addresses().begin(),
              dataPtr->addresses().begin() + size,
              result->begin());
    return SUCCESS;
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
