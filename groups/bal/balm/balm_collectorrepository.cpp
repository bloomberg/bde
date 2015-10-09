// balm_collectorrepository.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_collectorrepository.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_collectorrepository_cpp,"$Id$ $CSID$")

#include <balm_metricid.h>

#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>   // for 'bsl::min' and 'bsl::max'
#include <bsl_ostream.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_utility.h>

#include <bsl_cstddef.h>           // for 'bsl::size_t'

namespace BloombergLP {

namespace {

inline
void combine(balm::MetricRecord *record, const balm::MetricRecord& value)
{
    record->metricId() = value.metricId();
    record->count()   += value.count();
    record->total()   += value.total();
    record->min()      = bsl::min(record->min(), value.min());
    record->max()      = bsl::max(record->max(), value.max());
}

}  // close unnamed namespace

namespace balm {

                    // ====================================
                    // class CollectorRepository_Collectors
                    // ====================================

template <class COLLECTOR>
class CollectorRepository_Collectors {
    // This implementation class provides a container mechanism for managing a
    // set of objects of templatized type 'COLLECTOR' that are all associated
    // with a single metric.  The behavior is undefined unless the templatized
    // type 'COLLECTOR' is either 'Collector' or
    // 'IntegerCollector'.  A 'CollectorRepository_Collectors'
    // object is supplied a 'MetricId' at construction, and provides a
    // default 'COLLECTOR' as well as a set of additional 'COLLECTOR' objects
    // for the identified metric.  Additional 'COLLECTOR' objects (beyond the
    // default) can be added using the 'addCollector' method.  A
    // 'collectAndReset' method is provided to obtain the aggregate value of
    // all the owned collectors and reset those collectors to their default
    // state.

    // PRIVATE TYPES
    typedef bsl::shared_ptr<COLLECTOR>               Collector;
        // A 'Collector' object is a shared pointer to an object of the
        // templatized type 'COLLECTOR'.

    typedef bsl::set<Collector>                      CollectorSet;
        // A 'CollectorSet' is a set of shared pointers to objects of the
        // templatized type 'COLLECTOR'.

    // DATA
    COLLECTOR         d_defaultCollector;  // default collector
    CollectorSet      d_addedCollectors;   // added collectors
    bslma::Allocator *d_allocator_p;       // allocator (held, not owned)

    // NOT IMPLEMENTED
    CollectorRepository_Collectors(const CollectorRepository_Collectors& );
    CollectorRepository_Collectors& operator=(
                                       const CollectorRepository_Collectors& );

  public:
    // PUBLIC TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CollectorRepository_Collectors,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    CollectorRepository_Collectors(const MetricId&   metricId,
                                   bslma::Allocator *basicAllocator = 0);
        // Create a 'CollectorRepository_Collectors' object to hold
        // objects of the templatized type 'COLLECTOR' for the specified
        // 'metricId'.   Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless the
        // templatized type 'COLLECTOR' is either 'Collector' or
        // 'IntegerCollector', and 'metricId.isValid()' is 'true'.

    ~CollectorRepository_Collectors();
        // Destroy this object.

    // MANIPULATORS
    COLLECTOR *defaultCollector();
        // Return a pointer to the default collector.  Note that this method
        // will return a valid address.

    bsl::shared_ptr<COLLECTOR> addCollector();
        // Add a new collector to the set of additional collectors and return
        // a shared pointer to the newly-added collector.  Note that this
        // method will return a valid pointer.

    int removeCollector(COLLECTOR *collector);
        // Remove the specified 'collector' from this container.  Return 0 on
        // success or a non-zero value if 'collector' was not returned from a
        // call to 'addCollector' on this object, or has previously been
        // removed.

    void collectAndReset(MetricRecord *record);
        // Load into the specified 'record' the aggregate value of all the
        // records collected by the collectors owned by this object; then
        // reset those collectors to their default values.  Note that all
        // collectors within this object record values for the same metric id,
        // so they can be aggregated into a single record.

    void collect(MetricRecord *record);
        // Load into the specified 'record' the aggregate value of all the
        // records collected by the collectors owned by this object.  Note
        // that all collectors within this object record values for the same
        // metric id, so they can be aggregated into a single record.  Also
        // note that because this operation does not reset the collectors,
        // subsequent 'collect' invocations will effectively re-collect the
        // current values.

    // ACCESSORS
    int getAddedCollectors(
                   bsl::vector<bsl::shared_ptr<COLLECTOR> > *collectors) const;
        // Append to the specified 'collectors' all the collectors that have
        // been added to this object (via the 'addCollector' method)  and not
        // subsequently removed.  Return the number of collectors that were
        // found.

    const MetricId& metricId() const;
        // Return a reference to the non-modifiable 'MetricId' object
        // identifying the metric for which the collectors in this container
        // are collecting values.
};

                // --------------------------------------------
                // class CollectorRepository_CollectorContainer
                // --------------------------------------------

// CREATORS
template <class COLLECTOR>
CollectorRepository_Collectors<COLLECTOR>::
      CollectorRepository_Collectors(const MetricId&   metricId,
                                     bslma::Allocator *basicAllocator)
: d_defaultCollector(metricId)
, d_addedCollectors(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class COLLECTOR>
inline
CollectorRepository_Collectors<COLLECTOR>::
~CollectorRepository_Collectors()
{
}

// MANIPULATORS
template <class COLLECTOR>
inline
COLLECTOR *
CollectorRepository_Collectors<COLLECTOR>::defaultCollector()
{
    return &d_defaultCollector;
}

template <class COLLECTOR>
bsl::shared_ptr<COLLECTOR>
CollectorRepository_Collectors<COLLECTOR>::addCollector()
{
    Collector collectorPtr(
                new (*d_allocator_p) COLLECTOR(d_defaultCollector.metricId()),
                d_allocator_p);
    d_addedCollectors.insert(collectorPtr);
    return collectorPtr;
}

template <class COLLECTOR>
int CollectorRepository_Collectors<COLLECTOR>::removeCollector(
                                                          COLLECTOR *collector)
{
    Collector collectorPtr(collector, bslstl::SharedPtrNilDeleter(), 0);
    bsl::size_t rc = d_addedCollectors.erase(collectorPtr);
    return rc > 0 ? 0 : -1;
}

template <class COLLECTOR>
void
CollectorRepository_Collectors<COLLECTOR>::collectAndReset(
                                                          MetricRecord *record)
{
    d_defaultCollector.loadAndReset(record);
    typename CollectorSet::iterator it = d_addedCollectors.begin();
    for (; it != d_addedCollectors.end(); ++it) {
        MetricRecord tempRecord;
        (*it)->loadAndReset(&tempRecord);
        combine(record, tempRecord);
    }
}

template <class COLLECTOR>
void
CollectorRepository_Collectors<COLLECTOR>::collect(MetricRecord *record)
{
    d_defaultCollector.load(record);
    typename CollectorSet::iterator it = d_addedCollectors.begin();
    for (; it != d_addedCollectors.end(); ++it) {
        MetricRecord tempRecord;
        (*it)->load(&tempRecord);
        combine(record, tempRecord);
    }
}

// ACCESSORS
template <class COLLECTOR>
int
CollectorRepository_Collectors<COLLECTOR>::getAddedCollectors(
                    bsl::vector<bsl::shared_ptr<COLLECTOR> > *collectors) const
{
    collectors->reserve(collectors->size() + d_addedCollectors.size());
    typename CollectorSet::const_iterator it = d_addedCollectors.begin();
    for (; it != d_addedCollectors.end(); ++it) {
        collectors->push_back(*it);
    }
    return d_addedCollectors.size();
}

template <class COLLECTOR>
inline
const MetricId&
CollectorRepository_Collectors<COLLECTOR>::metricId() const
{
    return d_defaultCollector.metricId();
}

                 // ==========================================
                 // class CollectorRepository_MetricCollectors
                 // ==========================================

class CollectorRepository_MetricCollectors {
    // This implementation class provides a container mechanism for managing
    // the 'Collector' and 'IntegerCollector' objects associated with a single
    // metric.  The 'collector' and 'intCollector' methods are provided to
    // access the individual containers for 'Collector' objects and
    // 'IntegerCollector' objects, respectively.   The 'collectAndReset' method
    // obtains the aggregate value of all the owned collectors and integer
    // collectors, and then resets those collectors and integer collectors to
    // their default state.

    // PRIVATE TYPES
    typedef CollectorRepository_Collectors<Collector>
                                                        Collectors;
    typedef CollectorRepository_Collectors<IntegerCollector>
                                                        IntCollectors;

    // DATA
    Collectors    d_collectors;     // collector objects
    IntCollectors d_intCollectors;  // integer collector objects

    // NOT IMPLEMENTED
    CollectorRepository_MetricCollectors(
                           const  CollectorRepository_MetricCollectors& );
    CollectorRepository_MetricCollectors& operator=(
                           const  CollectorRepository_MetricCollectors& );
  public:
    // PUBLIC TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CollectorRepository_MetricCollectors,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    CollectorRepository_MetricCollectors(const MetricId&   id,
                                         bslma::Allocator *basicAllocator = 0);
        // Create a 'CollectorRepository_MetricCollectors' object to hold
        // collector and integer collector objects for the specified
        // 'metricId'.   Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 'metricId.isValid()' is 'true'.

    ~CollectorRepository_MetricCollectors();
        // Destroy this object.

    // MANIPULATORS
    CollectorRepository_Collectors<Collector>& collectors();
        // Return a reference to the modifiable container of 'Collector'
        // objects.

    CollectorRepository_Collectors<IntegerCollector>& intCollectors();
        // Return a reference to the modifiable container of
        // 'IntegerCollector' objects.

    void collectAndReset(MetricRecord *record);
        // Load into the specified 'record' the aggregate value of all the
        // records collected by the collectors owned by this object; then
        // reset those collectors to their default values.  Note that all
        // collectors within this object record values for the same metric id,
        // so they can be aggregated into a single record.

    void collect(MetricRecord *record);
        // Load into the specified 'record' the aggregate value of all the
        // records collected by the collectors owned by this object.  Note
        // that all collectors within this object record values for the same
        // metric id, so they can be aggregated into a single record.  Also
        // note that because this operation does not reset the collectors,
        // subsequent 'collect' invocations will effectively re-collect the
        // current values.

    // ACCESSORS
    const CollectorRepository_Collectors<Collector>& collectors() const;
        // Return a reference to the non-modifiable container of 'Collector'
        // objects.

    const CollectorRepository_Collectors<IntegerCollector>&
                                                         intCollectors() const;
        // Return a reference to the non-modifiable container of
        // 'IntegerCollector' objects.

    const MetricId& metricId() const;
        // Return a reference to the non-modifiable 'MetricId' object
        // identifying the metric for which the collectors in this container
        // are collecting values.
};

                 // ------------------------------------------
                 // class CollectorRepository_MetricCollectors
                 // ------------------------------------------

// CREATORS
inline
CollectorRepository_MetricCollectors::
CollectorRepository_MetricCollectors(const MetricId&   id,
                                     bslma::Allocator *basicAllocator)
: d_collectors(id, basicAllocator)
, d_intCollectors(id, basicAllocator)
{
}

inline
CollectorRepository_MetricCollectors::
~CollectorRepository_MetricCollectors()
{
}

// MANIPULATORS
inline
CollectorRepository_Collectors<Collector>&
CollectorRepository_MetricCollectors::collectors()
{
    return d_collectors;
}

inline
CollectorRepository_Collectors<IntegerCollector>&
CollectorRepository_MetricCollectors::intCollectors()
{
    return d_intCollectors;
}

void CollectorRepository_MetricCollectors::collectAndReset(
                                                          MetricRecord *record)
{
    d_collectors.collectAndReset(record);
    MetricRecord tempRecord;
    d_intCollectors.collectAndReset(&tempRecord);
    combine(record, tempRecord);
}

void CollectorRepository_MetricCollectors::collect(MetricRecord *record)
{
    d_collectors.collect(record);
    MetricRecord tempRecord;
    d_intCollectors.collect(&tempRecord);
    combine(record, tempRecord);
}

// ACCESSORS
inline
const CollectorRepository_Collectors<Collector>&
CollectorRepository_MetricCollectors::collectors() const
{
    return d_collectors;
}

inline
const CollectorRepository_Collectors<IntegerCollector>&
CollectorRepository_MetricCollectors::intCollectors() const
{
    return d_intCollectors;
}

inline
const MetricId&
CollectorRepository_MetricCollectors::metricId() const
{
    return d_collectors.metricId();
}

                         // -------------------------
                         // class CollectorRepository
                         // -------------------------

// PRIVATE MANIPULATORS
CollectorRepository::MetricCollectors&
CollectorRepository::getMetricCollectors(const MetricId& metricId)
{
    Collectors::iterator cIt = d_collectors.find(metricId);
    if (cIt == d_collectors.end()) {
        // This metric is not in the map; create a new 'MetricCollectors' for
        // the 'metric' and add it to both 'd_collectors' and 'd_categories'.

        BSLS_ASSERT(metricId.isValid());
        const Category *category = metricId.category();

        MetricCollectorsSPtr collectorsPtr(
               new (*d_allocator_p) MetricCollectors(metricId, d_allocator_p),
               d_allocator_p);

        // To make this method exception safe: Reserve memory for inserting
        // 'collectorsPtr' into 'd_categories' before inserting it into
        // 'd_collectors'.  This avoids inconsistent data structures if the
        // second insertion (would have) caused an allocation exception.
        bsl::vector<MetricCollectors *>& colCategory = d_categories[category];
        colCategory.reserve(colCategory.size() + 1);

        cIt = d_collectors.insert(
                                bsl::make_pair(metricId, collectorsPtr)).first;
        colCategory.push_back(collectorsPtr.get());
    }
    return *cIt->second.get();
}

// MANIPULATORS
void CollectorRepository::collectAndReset(bsl::vector<MetricRecord> *records,
                                          const Category            *category)
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);

    CategorizedCollectors::iterator catIt = d_categories.find(category);
    if (catIt != d_categories.end()) {

        bsl::vector<MetricCollectors *>& metricCollectors = catIt->second;
        records->reserve(records->size() + metricCollectors.size());
        bsl::vector<MetricCollectors *>::iterator metricIt =
                                                     metricCollectors.begin();

        // Each 'MetricCollectors' object (in the 'd_categories' map) contains
        // the collectors for a single metric.
        for (; metricIt != metricCollectors.end(); ++metricIt) {
            MetricRecord record;
            (*metricIt)->collectAndReset(&record);
            records->push_back(record);
        }
    }
}

void CollectorRepository::collect(bsl::vector<MetricRecord> *records,
                                  const Category            *category)
{
    // PRIVATE TYPES
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);

    CategorizedCollectors::iterator catIt = d_categories.find(category);
    if (catIt != d_categories.end()) {

        bsl::vector<MetricCollectors *>& metricCollectors = catIt->second;
        records->reserve(records->size() + metricCollectors.size());
        bsl::vector<MetricCollectors *>::iterator metricIt =
                                                     metricCollectors.begin();

        // Each 'MetricCollectors' object (in the 'd_categories' map) contains
        // the collectors for a single metric.
        for (; metricIt != metricCollectors.end(); ++metricIt) {
            MetricRecord record;
            (*metricIt)->collect(&record);
            records->push_back(record);
        }
    }
}

Collector *CollectorRepository::getDefaultCollector(const MetricId& metricId)
{
    // First, obtain a read-lock, and test if the 'MetricCollectors' object
    // for 'metricId' already exists.
    {
        bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
        Collectors::iterator it = d_collectors.find(metricId);
        if (it != d_collectors.end()) {
            return it->second->collectors().defaultCollector();       // RETURN
        }
    }

    // Use 'getMetricCollectors' to create the metrics collectors object (if
    // one has not been created since the read-lock was released).
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
    return getMetricCollectors(metricId).collectors().defaultCollector();
}

IntegerCollector *CollectorRepository::getDefaultIntegerCollector(
                                                      const MetricId& metricId)
{
    // First, obtain a read-lock, and test if the 'MetricCollectors' object
    // for 'metricId' already exists.
    {
        bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
        Collectors::iterator it = d_collectors.find(metricId);
        if (it != d_collectors.end()) {
            return it->second->intCollectors().defaultCollector();    // RETURN
        }
    }

    // Use 'getMetricCollectors' to create the metrics collectors object (if
    // one has not been created since the read-lock was released).
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
    return getMetricCollectors(metricId).intCollectors().defaultCollector();
}

bsl::shared_ptr<Collector> CollectorRepository::addCollector(
                                                      const MetricId& metricId)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
    return getMetricCollectors(metricId).collectors().addCollector();
}

bsl::shared_ptr<IntegerCollector>
CollectorRepository::addIntegerCollector(const MetricId& metricId)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
    return getMetricCollectors(metricId).intCollectors().addCollector();
}

int CollectorRepository::getAddedCollectors(
               bsl::vector<bsl::shared_ptr<Collector> >         *collectors,
               bsl::vector<bsl::shared_ptr<IntegerCollector> >  *intCollectors,
               const MetricId&                                   metricId)
{
    int numFound = 0;
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
    Collectors::iterator cIt = d_collectors.find(metricId);
    if (cIt != d_collectors.end()) {
        numFound += cIt->second->collectors().getAddedCollectors(collectors);
        numFound += cIt->second->intCollectors().getAddedCollectors(
                                                                intCollectors);
    }
    return numFound;
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
