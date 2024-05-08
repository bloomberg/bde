// bdlm_metricsregistry.cpp                                           -*-C++-*-

#include <bdlm_metricsregistry.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlm_metricsregistry_cpp,"$Id$ $CSID$")

#include <bslma_default.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_once.h>

#include <bsls_assert.h>
#include <bsls_log.h>

#include <bsl_map.h>

namespace BloombergLP {
namespace bdlm {

                       // ==========================
                       // class MetricsRegistry_Data
                       // ==========================

class MetricsRegistry_Data {
    // This is a *component* *private* type used to describe the data
    // associated with a metrics registration.  DO NOT USE.

  public:
    // PUBLIC DATA
    MetricDescriptor               d_descriptor;
    MetricsAdapter::Callback       d_callback;
    MetricsAdapter::CallbackHandle d_adaptedHandle;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MetricsRegistry_Data,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    MetricsRegistry_Data();
        // Create a 'MetricsRegistry_Data' object that uses the default
        // allocator to supply memory.

    explicit MetricsRegistry_Data(bslma::Allocator *basicAllocator);
        // Create a 'MetricsRegistry_Data' object that uses the specified
        // 'basicAllocator' to supply memory.

    MetricsRegistry_Data(const MetricsRegistry_Data& original,
                         bslma::Allocator           *basicAllocator = 0);
        // Create a 'MetricsRegistry_Data' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // to supply memory; otherwise, the default allocator is used.

    MetricsRegistry_Data(bslmf::MovableRef<MetricsRegistry_Data> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'MetricsRegistry_Data' object having the same value and the
        // same allocator as the specified 'original' object.  The value of
        // 'original' becomes unspecified but valid, and its allocator remains
        // unchanged.

    MetricsRegistry_Data(
                      bslmf::MovableRef<MetricsRegistry_Data>  original,
                      bslma::Allocator                        *basicAllocator);
        // Create a 'MetricsRegistry_Data' object having the same value as the
        // specified 'original' object, using the specified 'basicAllocator' to
        // supply memory.  The allocator of 'original' remains unchanged.  If
        // 'original' and the newly created object have the same allocator,
        // then the value of 'original' becomes unspecified but valid, and no
        // exceptions will be thrown; otherwise 'original' is unchanged and an
        // exception may be thrown.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

                          // ==========================
                          // class MetricsRegistry_Impl
                          // ==========================

class MetricsRegistry_Impl {
    // This class is a mechanism that implements the 'MetricsRegistry' class
    // (which uses a "pimpl" design idiom).

    // PRIVATE TYPES
    typedef MetricsAdapter::Callback       Callback;
    typedef MetricsAdapter::CallbackHandle CallbackHandle;

  public:
    // TYPES
    typedef bsl::map<CallbackHandle, MetricsRegistry_Data> MetricDataMap;

  private:
    // DATA
    MetricsAdapter       *d_metricsAdapter_p;  // held, but not owned, metrics
                                               // adapter

    MetricDataMap         d_metricData;        // registration data

    CallbackHandle        d_nextKey;           // next key for inserting into
                                               // 'd_metricData'

    mutable bslmt::Mutex  d_mutex;             // mutex to protect the metrics
                                               // registry and 'd_metricData'

    // FRIENDS
    friend class MetricsRegistryRegistrationHandle;

    // NOT IMPLEMENTED
    MetricsRegistry_Impl(const MetricsRegistry_Impl&) BSLS_KEYWORD_DELETED;
    MetricsRegistry_Impl& operator=(
                             const MetricsRegistry_Impl&) BSLS_KEYWORD_DELETED;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MetricsRegistry_Impl,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit MetricsRegistry_Impl(bslma::Allocator *basicAllocator = 0);
        // Create a 'MetricsRegistry' object that stores the information
        // necessary to forward the register and unregister of metrics to a
        // metrics registry supplied with 'setMetricsRegistry'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~MetricsRegistry_Impl();
        // Unregister all registered metrics and destroy this 'MetricsRegistry'
        // object.

    // MANIPULATORS
    CallbackHandle registerCollectionCallback(
                                      const bdlm::MetricDescriptor& descriptor,
                                      const Callback&               callback);
        // Register the metric described by the specified 'descriptor' and
        // associate it with the specified 'callback' to collect data from the
        // metric.  Return a handle for the registered callback.  The metric
        // and associated callback remain registered with this registry until
        // either the handle is unregistered or destroyed.  When a
        // 'MetricsAdapter' is associated with this registry using
        // 'setMetricsAdapter', this objects registers all the registered
        // metrics and callbacks with that adapter, and similarly unregisters
        // them if the 'MetricAdapter' is later disassociated with this
        // registry (either on this objects destruction, or due to a call to
        // 'removeMetricsAdapter' or 'setMetricsAdapter').  In this way, a
        // 'MetricsRegistry' serves as an intermediary between users of 'bdlm'
        // that register metrics and the subsystem for collecting and
        // publishing metrics being adapted by a concrete instance of
        // 'bdlm::MetricAdapter'.

    int removeCollectionCallback(const CallbackHandle& handle);
        // Remove the callback associated with the specified 'handle' from the
        // associated adapter, if provided, and from this registry.  Return 0
        // on success, or a non-zero value if 'handle' cannot be found.

    void removeMetricsAdapter(MetricsAdapter *adapter);
        // If the specified 'adapter' is the currently associated registrar,
        // remove all registered metrics from it and disassociate the registry.
        // Note that this operation takes an 'adapter' to disambiguate
        // multiple, potentially concurrent, calls to this method and
        // 'setMetricsAdapter'.

    void setMetricsAdapter(MetricsAdapter *adapter);
        // Configure this metrics registry to register all metrics collection
        // callbacks with the specified 'adapter'.  This operation first, if
        // there is already an associated metrics adapter, unregisters all the
        // collection callbacks from that adapter, then registers the
        // collection callbacks with the new 'adapter'.

    // ACCESSORS

    int numRegisteredCollectionCallbacks() const;
        // Return the number of registered metrics collection callbacks.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

                          // ====================================
                          // class MetricsRegistry_ElementProctor
                          // ====================================

class MetricsRegistry_ElementProctor {
    // 'MetricDataMap' element proctor.

    // PRIVATE TYPES
    typedef MetricsRegistry_Impl::MetricDataMap MetricDataMap;

    // DATA
    MetricDataMap&          d_map;
    MetricDataMap::iterator d_it;

    // NOT IMPLEMENTED
    MetricsRegistry_ElementProctor(
                   const MetricsRegistry_ElementProctor&) BSLS_KEYWORD_DELETED;
    MetricsRegistry_ElementProctor &operator=(
                   const MetricsRegistry_ElementProctor&) BSLS_KEYWORD_DELETED;
  public:
    // CREATORS
    MetricsRegistry_ElementProctor(MetricDataMap&          map,
                                   MetricDataMap::iterator iter);
        // Create a proctor that conditionally manages an element in the
        // specified 'map' pointed by the specified 'iter' by erasing it (if
        // not released -- see 'release') upon destruction.

    ~MetricsRegistry_ElementProctor();
        // Destroy this proctor, and erase the element it manages if the
        // 'release' member function wasn't called.

    // MANIPULATORS
    void release();
        // Release from management the element currently managed by this
        // proctor.
};
                       // --------------------------
                       // class MetricsRegistry_Data
                       // --------------------------

// CREATORS
inline
MetricsRegistry_Data::MetricsRegistry_Data()
: d_descriptor()
, d_callback()
, d_adaptedHandle()
{
}

inline
MetricsRegistry_Data::MetricsRegistry_Data(bslma::Allocator *basicAllocator)
: d_descriptor(basicAllocator)
, d_callback(bsl::allocator_arg, basicAllocator)
, d_adaptedHandle()
{
}

inline
MetricsRegistry_Data::MetricsRegistry_Data(
                                    const MetricsRegistry_Data& original,
                                    bslma::Allocator           *basicAllocator)
: d_descriptor(original.d_descriptor, basicAllocator)
, d_callback(bsl::allocator_arg, basicAllocator, original.d_callback)
, d_adaptedHandle(original.d_adaptedHandle)
{
}

inline
MetricsRegistry_Data::MetricsRegistry_Data(
        bslmf::MovableRef<MetricsRegistry_Data> original) BSLS_KEYWORD_NOEXCEPT
: d_descriptor(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_descriptor))
, d_callback(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_callback))
, d_adaptedHandle(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_adaptedHandle))
{
}

inline
MetricsRegistry_Data::MetricsRegistry_Data(
                        bslmf::MovableRef<MetricsRegistry_Data> original,
                        bslma::Allocator                       *basicAllocator)
: d_descriptor(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_descriptor), basicAllocator)
, d_callback(bsl::allocator_arg, basicAllocator, bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_callback))
, d_adaptedHandle(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_adaptedHandle))
{
}

// ACCESSORS
                                  // Aspects
inline
bslma::Allocator *MetricsRegistry_Data::allocator() const
{
    return d_descriptor.allocator();
}

                          // --------------------------
                          // class MetricsRegistry_Impl
                          // --------------------------

// CREATORS
inline
MetricsRegistry_Impl::MetricsRegistry_Impl(bslma::Allocator *basicAllocator)
: d_metricsAdapter_p(0)
, d_metricData(basicAllocator)
, d_nextKey(0)
{
}

MetricsRegistry_Impl::~MetricsRegistry_Impl()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (d_metricsAdapter_p) {
        for (MetricDataMap::iterator iter = d_metricData.begin();
             iter != d_metricData.end();
             ++iter) {
            d_metricsAdapter_p->removeCollectionCallback(
                                                 iter->second.d_adaptedHandle);
        }
        d_metricsAdapter_p = 0;
    }
}

// MANIPULATORS
MetricsRegistry_Impl::CallbackHandle
MetricsRegistry_Impl::registerCollectionCallback(
                                      const bdlm::MetricDescriptor& descriptor,
                                      const Callback&               callback)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    bsl::pair<MetricDataMap::iterator, bool> res =
                                           d_metricData.try_emplace(d_nextKey);
    BSLS_ASSERT(res.second);
    MetricsRegistry_ElementProctor proctor(d_metricData, res.first);

    MetricsRegistry_Data& data = res.first->second;
    data.d_descriptor = descriptor;
    data.d_callback   = callback;
    if (d_metricsAdapter_p) {
        data.d_adaptedHandle =
                     d_metricsAdapter_p->registerCollectionCallback(descriptor,
                                                                    callback);
    }

    proctor.release();
    return d_nextKey++;
}

int MetricsRegistry_Impl::removeCollectionCallback(
                                                  const CallbackHandle& handle)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    MetricDataMap::iterator iter = d_metricData.find(handle);
    if (iter != d_metricData.end()) {
        if (d_metricsAdapter_p) {
            d_metricsAdapter_p->removeCollectionCallback(
                                                 iter->second.d_adaptedHandle);
        }
        d_metricData.erase(iter);
        return 0;                                                     // RETURN
    }
    return 1;
}

void MetricsRegistry_Impl::removeMetricsAdapter(MetricsAdapter *adapter)
{
    BSLS_ASSERT(adapter);
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (d_metricsAdapter_p == adapter) {
        for (MetricDataMap::iterator iter = d_metricData.begin();
             iter != d_metricData.end();
             ++iter) {
            d_metricsAdapter_p->removeCollectionCallback(
                                                 iter->second.d_adaptedHandle);
        }
        d_metricsAdapter_p = 0;
    }
    else {
        BSLS_LOG_ERROR("Attempt to remove unknown 'MetricsAdapter'");
    }
}

void MetricsRegistry_Impl::setMetricsAdapter(MetricsAdapter *adapter)
{
    BSLS_ASSERT(adapter);
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    for (MetricDataMap::iterator iter = d_metricData.begin();
         iter != d_metricData.end();
         ++iter) {
        if (d_metricsAdapter_p) {
            d_metricsAdapter_p->removeCollectionCallback(
                                                 iter->second.d_adaptedHandle);
        }
        iter->second.d_adaptedHandle = adapter->registerCollectionCallback(
                                                     iter->second.d_descriptor,
                                                     iter->second.d_callback);
    }

    d_metricsAdapter_p = adapter;
}

// ACCESSORS

int MetricsRegistry_Impl::numRegisteredCollectionCallbacks() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return static_cast<int>(d_metricData.size());
}

                                  // Aspects

inline
bslma::Allocator *MetricsRegistry_Impl::allocator() const
{
    return d_metricData.get_allocator().mechanism();
}

                          // ------------------------------------
                          // class MetricsRegistry_ElementProctor
                          // ------------------------------------

// CREATORS
inline
MetricsRegistry_ElementProctor::MetricsRegistry_ElementProctor(
                                                  MetricDataMap&          map,
                                                  MetricDataMap::iterator iter)
: d_map(map)
, d_it(iter)
{
}

inline
MetricsRegistry_ElementProctor::~MetricsRegistry_ElementProctor()
{
    if (d_it != d_map.end()) {
        d_map.erase(d_it);
    }
}

// MANIPULATORS
inline
void MetricsRegistry_ElementProctor::release()
{
    d_it = d_map.end();
}

                          // ---------------------
                          // class MetricsRegistry
                          // ---------------------

// CREATORS
MetricsRegistry::MetricsRegistry(bslma::Allocator *basicAllocator)
: d_impl(bsl::allocate_shared<MetricsRegistry_Impl>(basicAllocator))
{
}

MetricsRegistry::~MetricsRegistry()
{
}

// CLASS METHODS
MetricsRegistry& MetricsRegistry::defaultInstance()
{
    static MetricsRegistry *s_metricsRegistry_p;

    BSLMT_ONCE_DO {
        // Note that the 'MetricsRegistryImp' held by this 'MetricsRegistry'
        // should be released when 's_metricsRegistry' is destroyed, but the
        // control block for the shared pointer will not be released until all
        // 'MetricsRegistryRegistrationHandle' objects that referred to the
        // 'MetricsRegistry' have been destroyed.  I.e., this may appear to
        // leak the allocation of the control block if there are
        // 'MetricsRegistryRegistrationHandle' objects that are not destroyed.
        static MetricsRegistry s_metricsRegistry(
                                            bslma::Default::globalAllocator());

        s_metricsRegistry_p = &s_metricsRegistry;
    }

    return *s_metricsRegistry_p;
}

// MANIPULATORS
void MetricsRegistry::registerCollectionCallback(
                                 MetricsRegistryRegistrationHandle *result,
                                 const bdlm::MetricDescriptor&      descriptor,
                                 const Callback&                    callback)
{
    BSLS_ASSERT(result);
    CallbackHandle key = d_impl->registerCollectionCallback(descriptor,
                                                            callback);
    MetricsRegistryRegistrationHandle(d_impl, key).swap(*result);
}

void MetricsRegistry::removeMetricsAdapter(MetricsAdapter *adapter)
{
    d_impl->removeMetricsAdapter(adapter);
}

void MetricsRegistry::setMetricsAdapter(MetricsAdapter *adapter)
{
    d_impl->setMetricsAdapter(adapter);
}

// ACCESSORS

int MetricsRegistry::numRegisteredCollectionCallbacks() const
{
    return d_impl->numRegisteredCollectionCallbacks();
}

                                  // Aspects

bslma::Allocator *MetricsRegistry::allocator() const
{
    return d_impl->allocator();
}

                 // ---------------------------------------
                 // class MetricsRegistryRegistrationHandle
                 // ---------------------------------------

// CREATORS
MetricsRegistryRegistrationHandle::MetricsRegistryRegistrationHandle(
                           const bsl::weak_ptr<MetricsRegistry_Impl>& registry,
                           MetricsRegistry::CallbackHandle            handle)
: d_registry(registry)
, d_handle(handle)
{
}

MetricsRegistryRegistrationHandle::MetricsRegistryRegistrationHandle(
                 bslmf::MovableRef<MetricsRegistryRegistrationHandle> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_registry(MoveUtil::move(MoveUtil::access(original).d_registry))
, d_handle(MoveUtil::move(MoveUtil::access(original).d_handle))
{
}

MetricsRegistryRegistrationHandle::~MetricsRegistryRegistrationHandle()
{
    unregister();
}

// MANIPULATORS
MetricsRegistryRegistrationHandle&
MetricsRegistryRegistrationHandle::operator=(
                 bslmf::MovableRef<MetricsRegistryRegistrationHandle> original)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    MetricsRegistryRegistrationHandle& ref = original;
    if (&ref != this) {
        unregister();
        d_registry = MoveUtil::move(ref.d_registry);
        d_handle   = MoveUtil::move(ref.d_handle);
    }
    return *this;
}

int MetricsRegistryRegistrationHandle::unregister()
{
    int rv = 0;

    bsl::shared_ptr<MetricsRegistry_Impl> registry = d_registry.lock();
    if (registry) {
        rv = registry->removeCollectionCallback(d_handle);
    }
    d_registry.reset();

    return rv;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
