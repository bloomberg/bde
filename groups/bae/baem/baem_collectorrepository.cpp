// baem_collectorrepository.cpp      -*-C++-*-
#include <baem_collectorrepository.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_collectorrepository_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bcemt_readlockguard.h>
#include <bcemt_writelockguard.h>

#include <bsl_algorithm.h>   // for 'bsl::min' and 'bsl::max'
#include <bsl_ostream.h>
#include <bsl_set.h>

#include <bsl_cstddef.h>           // for 'bsl::size_t'

namespace BloombergLP {

namespace {

inline
void combine(baem_MetricRecord *record, const baem_MetricRecord& value)
{
    record->metricId() = value.metricId();
    record->count()   += value.count();
    record->total()   += value.total();
    record->min()      = bsl::min(record->min(), value.min());
    record->max()      = bsl::max(record->max(), value.max());
}

}  // close unnamed namespace

              // =========================================
              // class baem_CollectorRepository_Collectors
              // =========================================

template <typename COLLECTOR>
class baem_CollectorRepository_Collectors {
    // This implementation class provides a container mechanism for managing a
    // set of objects of templatized type 'COLLECTOR' that are all associated
    // with a single metric.  The behavior is undefined unless the templatized
    // type 'COLLECTOR' is either 'baem_Collector' or 'baem_IntegerCollector'.
    // A 'baem_CollectorRepository_Collectors' object is supplied a
    // 'baem_MetricId' at construction, and provides a default 'COLLECTOR' as
    // well as a set of additional 'COLLECTOR' objects for the identified
    // metric.  Additional 'COLLECTOR' objects (beyond the default) can be
    // added using the 'addCollector' method.  A 'collectAndReset' method is
    // provided to obtain the aggregate value of all the owned collectors and
    // reset those collectors to their default state.

    // PRIVATE TYPES
    typedef bcema_SharedPtr<COLLECTOR>               Collector;
        // A 'Collector' object is a shared pointer to an object of the
        // templatized type 'COLLECTOR'.

    typedef bsl::set<Collector, bcema_SharedPtrLess> CollectorSet;
        // A 'CollectorSet' is a set of shared pointers to objects of the
        // templatized type 'COLLECTOR'.

    // DATA
    COLLECTOR        d_defaultCollector;  // default collector
    CollectorSet     d_addedCollectors;   // added collectors
    bslma_Allocator *d_allocator_p;       // allocator (held, not owned)

    // NOT IMPLEMENTED
    baem_CollectorRepository_Collectors(
                          const baem_CollectorRepository_Collectors& );
    baem_CollectorRepository_Collectors& operator=(
                          const baem_CollectorRepository_Collectors& );

  public:
    // PUBLIC TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_CollectorRepository_Collectors,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_CollectorRepository_Collectors(
                                     const baem_MetricId&  metricId,
                                     bslma_Allocator      *basicAllocator = 0);
        // Create a 'baem_CollectorRepository_Collectors' object to hold
        // objects of the templatized type 'COLLECTOR' for the specified
        // 'metricId'.   Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless the
        // templatized type 'COLLECTOR' is either 'baem_Collector' or
        // 'baem_IntegerCollector', and 'metricId.isValid()' is 'true'.

    ~baem_CollectorRepository_Collectors();
        // Destroy this object.

    // MANIPULATORS
    COLLECTOR *defaultCollector();
        // Return a pointer to the default collector.  Note that this method
        // will return a valid address.

    bcema_SharedPtr<COLLECTOR> addCollector();
        // Add a new collector to the set of additional collectors and return
        // a shared pointer to the newly-added collector.  Note that this
        // method will return a valid pointer.

    int removeCollector(COLLECTOR *collector);
        // Remove the specified 'collector' from this container.  Return 0 on
        // success or a non-zero value if 'collector' was not returned from a
        // call to 'addCollector' on this object, or has previously been
        // removed.

    void collectAndReset(baem_MetricRecord *record);
        // Load into the specified 'record' the aggregate value of all the
        // records collected by the collectors owned by this object; then
        // reset those collectors to their default values.  Note that all
        // collectors within this object record values for the same metric id,
        // so they can be aggregated into a single record.

    void collect(baem_MetricRecord *record);
        // Load into the specified 'record' the aggregate value of all the
        // records collected by the collectors owned by this object.  Note
        // that all collectors within this object record values for the same
        // metric id, so they can be aggregated into a single record.  Also
        // note that because this operation does not reset the collectors,
        // subsequent 'collect' invocations will effectively re-collect the
        // current values.

    // ACCESSORS
    int getAddedCollectors(
                   bsl::vector<bcema_SharedPtr<COLLECTOR> > *collectors) const;
        // Append to the specified 'collectors' all the collectors that have
        // been added to this object (via the 'addCollector' method)  and
        // not subsequently removed.  Return the number of collectors that
        // were found.

    const baem_MetricId& metricId() const;
        // Return a reference to the non-modifiable 'baem_MetricId' object
        // identifying the metric for which the collectors in this container
        // are collecting values.
};

              // -------------------------------------------------
              // class baem_CollectorRepository_CollectorContainer
              // -------------------------------------------------

// CREATORS
template <typename COLLECTOR>
baem_CollectorRepository_Collectors<COLLECTOR>::
baem_CollectorRepository_Collectors(const baem_MetricId&  metricId,
                                    bslma_Allocator      *basicAllocator)
: d_defaultCollector(metricId)
, d_addedCollectors(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <typename COLLECTOR>
inline
baem_CollectorRepository_Collectors<COLLECTOR>::
~baem_CollectorRepository_Collectors()
{
}

// MANIPULATORS
template <typename COLLECTOR>
inline
COLLECTOR *
baem_CollectorRepository_Collectors<COLLECTOR>::defaultCollector()
{
    return &d_defaultCollector;
}

template <typename COLLECTOR>
bcema_SharedPtr<COLLECTOR>
baem_CollectorRepository_Collectors<COLLECTOR>::addCollector()
{
    Collector collectorPtr(
                new (*d_allocator_p) COLLECTOR(d_defaultCollector.metricId()),
                d_allocator_p);
    d_addedCollectors.insert(collectorPtr);
    return collectorPtr;
}

template <typename COLLECTOR>
int baem_CollectorRepository_Collectors<COLLECTOR>::removeCollector(
                                                          COLLECTOR *collector)
{
    Collector collectorPtr(collector, bcema_SharedPtrNilDeleter(), 0);
    bsl::size_t rc = d_addedCollectors.erase(collectorPtr);
    return rc > 0 ? 0 : -1;
}

template <typename COLLECTOR>
void
baem_CollectorRepository_Collectors<COLLECTOR>::collectAndReset(
                                                     baem_MetricRecord *record)
{
    d_defaultCollector.loadAndReset(record);
    typename CollectorSet::iterator it = d_addedCollectors.begin();
    for (; it != d_addedCollectors.end(); ++it) {
        baem_MetricRecord tempRecord;
        (*it)->loadAndReset(&tempRecord);
        combine(record, tempRecord);
    }
}

template <typename COLLECTOR>
void
baem_CollectorRepository_Collectors<COLLECTOR>::collect(
                                                     baem_MetricRecord *record)
{
    d_defaultCollector.load(record);
    typename CollectorSet::iterator it = d_addedCollectors.begin();
    for (; it != d_addedCollectors.end(); ++it) {
        baem_MetricRecord tempRecord;
        (*it)->load(&tempRecord);
        combine(record, tempRecord);
    }
}

// ACCESSORS
template <typename COLLECTOR>
int
baem_CollectorRepository_Collectors<COLLECTOR>::getAddedCollectors(
                    bsl::vector<bcema_SharedPtr<COLLECTOR> > *collectors) const
{
    collectors->reserve(collectors->size() + d_addedCollectors.size());
    typename CollectorSet::const_iterator it = d_addedCollectors.begin();
    for (; it != d_addedCollectors.end(); ++it) {
        collectors->push_back(*it);
    }
    return d_addedCollectors.size();
}

template <typename COLLECTOR>
inline
const baem_MetricId&
baem_CollectorRepository_Collectors<COLLECTOR>::metricId() const
{
    return d_defaultCollector.metricId();
}

           // ===============================================
           // class baem_CollectorRepository_MetricCollectors
           // ===============================================

class baem_CollectorRepository_MetricCollectors {
    // This implementation class provides a container mechanism for
    // managing the 'baem_Collector' and 'baem_IntegerCollector' objects
    // associated with a single metric.  The 'collector' and
    // 'intCollector' methods are provided to access the individual
    // containers for 'baem_Collector' objects and 'baem_IntegerCollector'
    // objects, respectively.   The 'collectAndReset' method obtains the
    // aggregate value of all the owned collectors and integer collectors, and
    // then resets those collectors and integer collectors to their default
    // state.

    // PRIVATE TYPES
    typedef baem_CollectorRepository_Collectors<baem_Collector>
                                                        Collectors;
    typedef baem_CollectorRepository_Collectors<baem_IntegerCollector>
                                                        IntCollectors;

    // DATA
    Collectors    d_collectors;     // collector objects
    IntCollectors d_intCollectors;  // integer collector objects

    // NOT IMPLEMENTED
    baem_CollectorRepository_MetricCollectors(
                           const  baem_CollectorRepository_MetricCollectors& );
    baem_CollectorRepository_MetricCollectors& operator=(
                           const  baem_CollectorRepository_MetricCollectors& );
  public:
    // PUBLIC TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_CollectorRepository_MetricCollectors,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_CollectorRepository_MetricCollectors(
                                    const baem_MetricId&  id,
                                    bslma_Allocator      *basicAllocator = 0);
        // Create a 'baem_CollectorRepository_MetricCollectors' object to hold
        // collector and integer collector objects for the specified
        // 'metricId'.   Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 'metricId.isValid()' is 'true'.

    ~baem_CollectorRepository_MetricCollectors();
        // Destroy this object.

    // MANIPULATORS
    baem_CollectorRepository_Collectors<baem_Collector>& collectors();
        // Return a reference to the modifiable container of 'baem_Collector'
        // objects.

    baem_CollectorRepository_Collectors<baem_IntegerCollector>&
                                                               intCollectors();
        // Return a reference to the modifiable container of
        // 'baem_IntegerCollector' objects.

    void collectAndReset(baem_MetricRecord *record);
        // Load into the specified 'record' the aggregate value of all the
        // records collected by the collectors owned by this object; then
        // reset those collectors to their default values.  Note that all
        // collectors within this object record values for the same metric id,
        // so they can be aggregated into a single record.

    void collect(baem_MetricRecord *record);
        // Load into the specified 'record' the aggregate value of all the
        // records collected by the collectors owned by this object.  Note
        // that all collectors within this object record values for the same
        // metric id, so they can be aggregated into a single record.  Also
        // note that because this operation does not reset the collectors,
        // subsequent 'collect' invocations will effectively re-collect the
        // current values.

    // ACCESSORS
    const baem_CollectorRepository_Collectors<baem_Collector>&
                                                            collectors() const;
        // Return a reference to the non-modifiable container of
        // 'baem_Collector' objects.

    const baem_CollectorRepository_Collectors<baem_IntegerCollector>&
                                                         intCollectors() const;
        // Return a reference to the non-modifiable container of
        // 'baem_IntegerCollector' objects.

    const baem_MetricId& metricId() const;
        // Return a reference to the non-modifiable 'baem_MetricId' object
        // identifying the metric for which the collectors in this container
        // are collecting values.
};

              // -----------------------------------------------
              // class baem_CollectorRepository_MetricCollectors
              // -----------------------------------------------

// CREATORS
inline
baem_CollectorRepository_MetricCollectors::
baem_CollectorRepository_MetricCollectors(const baem_MetricId&  id,
                                          bslma_Allocator      *basicAllocator)
: d_collectors(id, basicAllocator)
, d_intCollectors(id, basicAllocator)
{
}

inline
baem_CollectorRepository_MetricCollectors::
~baem_CollectorRepository_MetricCollectors()
{
}

// MANIPULATORS
inline
baem_CollectorRepository_Collectors<baem_Collector>&
baem_CollectorRepository_MetricCollectors::collectors()
{
    return d_collectors;
}

inline
baem_CollectorRepository_Collectors<baem_IntegerCollector>&
baem_CollectorRepository_MetricCollectors::intCollectors()
{
    return d_intCollectors;
}

void baem_CollectorRepository_MetricCollectors::collectAndReset(
                                                    baem_MetricRecord *record)
{
    d_collectors.collectAndReset(record);
    baem_MetricRecord tempRecord;
    d_intCollectors.collectAndReset(&tempRecord);
    combine(record, tempRecord);
}

void baem_CollectorRepository_MetricCollectors::collect(
                                                    baem_MetricRecord *record)
{
    d_collectors.collect(record);
    baem_MetricRecord tempRecord;
    d_intCollectors.collect(&tempRecord);
    combine(record, tempRecord);
}

// ACCESSORS
inline
const baem_CollectorRepository_Collectors<baem_Collector>&
baem_CollectorRepository_MetricCollectors::collectors() const
{
    return d_collectors;
}

inline
const baem_CollectorRepository_Collectors<baem_IntegerCollector>&
baem_CollectorRepository_MetricCollectors::intCollectors() const
{
    return d_intCollectors;
}

inline
const baem_MetricId&
baem_CollectorRepository_MetricCollectors::metricId() const
{
    return d_collectors.metricId();
}

                          // ------------------------------
                          // class baem_CollectorRepository
                          // ------------------------------

// PRIVATE MANIPULATORS
baem_CollectorRepository::MetricCollectors&
baem_CollectorRepository::getMetricCollectors(const baem_MetricId& metricId)
{
    Collectors::iterator cIt = d_collectors.find(metricId);
    if (cIt == d_collectors.end()) {
        // This metric is not in the map; create a new 'MetricCollectors' for
        // the 'metric' and add it to both 'd_collectors' and 'd_categories'.

        BSLS_ASSERT(metricId.isValid());
        const baem_Category *category = metricId.category();

        MetricCollectorsSPtr collectorsPtr(
               new (*d_allocator_p) MetricCollectors(metricId, d_allocator_p),
               d_allocator_p);

        // To make this method exception safe: Reserve memory for inserting
        // 'collectorsPtr' into 'd_categories' before inserting it into
        // 'd_collectors'.  This avoids inconsistent data structures if
        // the second insertion (would have) caused an allocation exception.
        bsl::vector<MetricCollectors *>& colCategory = d_categories[category];
        colCategory.reserve(colCategory.size() + 1);

        cIt = d_collectors.insert(
                         bsl::make_pair(metricId, collectorsPtr)).first;
        colCategory.push_back(collectorsPtr.ptr());
    }
    return *cIt->second.ptr();
}

// MANIPULATORS
void baem_CollectorRepository::collectAndReset(
                                bsl::vector<baem_MetricRecord> *records,
                                const baem_Category            *category)
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);

    CategorizedCollectors::iterator catIt = d_categories.find(category);
    if (catIt != d_categories.end()) {

        bsl::vector<MetricCollectors *>& metricCollectors = catIt->second;
        records->reserve(records->size() + metricCollectors.size());
        bsl::vector<MetricCollectors *>::iterator metricIt =
                                                     metricCollectors.begin();

        // Each 'MetricCollectors' object (in the 'd_categories' map) contains
        // the collectors for a single metric.
        for (; metricIt != metricCollectors.end(); ++metricIt) {
            baem_MetricRecord record;
            (*metricIt)->collectAndReset(&record);
            records->push_back(record);
        }
    }
}

void baem_CollectorRepository::collect(
                                bsl::vector<baem_MetricRecord> *records,
                                const baem_Category            *category)
{
    // PRIVATE TYPES
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);

    CategorizedCollectors::iterator catIt = d_categories.find(category);
    if (catIt != d_categories.end()) {

        bsl::vector<MetricCollectors *>& metricCollectors = catIt->second;
        records->reserve(records->size() + metricCollectors.size());
        bsl::vector<MetricCollectors *>::iterator metricIt =
                                                     metricCollectors.begin();

        // Each 'MetricCollectors' object (in the 'd_categories' map) contains
        // the collectors for a single metric.
        for (; metricIt != metricCollectors.end(); ++metricIt) {
            baem_MetricRecord record;
            (*metricIt)->collect(&record);
            records->push_back(record);
        }
    }
}

baem_Collector *baem_CollectorRepository::getDefaultCollector(
                                                 const baem_MetricId& metricId)
{
    // First, obtain a read-lock, and test if the 'MetricCollectors' object
    // for 'metricId' already exists.
    {
        bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
        Collectors::iterator it = d_collectors.find(metricId);
        if (it != d_collectors.end()) {
            return it->second->collectors().defaultCollector();
        }
    }

    // Use 'getMetricCollectors' to create the metrics collectors object (if
    // one has not been created since the read-lock was released).
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    return getMetricCollectors(metricId).collectors().defaultCollector();
}

baem_IntegerCollector *baem_CollectorRepository::getDefaultIntegerCollector(
                                                 const baem_MetricId& metricId)
{
    // First, obtain a read-lock, and test if the 'MetricCollectors' object
    // for 'metricId' already exists.
    {
        bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
        Collectors::iterator it = d_collectors.find(metricId);
        if (it != d_collectors.end()) {
            return it->second->intCollectors().defaultCollector();
        }
    }

    // Use 'getMetricCollectors' to create the metrics collectors object (if
    // one has not been created since the read-lock was released).
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    return getMetricCollectors(metricId).intCollectors().defaultCollector();
}

bcema_SharedPtr<baem_Collector> baem_CollectorRepository::addCollector(
                                                 const baem_MetricId& metricId)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    return getMetricCollectors(metricId).collectors().addCollector();
}

bcema_SharedPtr<baem_IntegerCollector>
baem_CollectorRepository::addIntegerCollector(const baem_MetricId& metricId)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    return getMetricCollectors(metricId).intCollectors().addCollector();
}

int baem_CollectorRepository::getAddedCollectors(
         bsl::vector<bcema_SharedPtr<baem_Collector> >         *collectors,
         bsl::vector<bcema_SharedPtr<baem_IntegerCollector> >  *intCollectors,
         const baem_MetricId&                                   metricId)
{
    int numFound = 0;
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    Collectors::iterator cIt = d_collectors.find(metricId);
    if (cIt != d_collectors.end()) {
        numFound += cIt->second->collectors().getAddedCollectors(collectors);
        numFound += cIt->second->intCollectors().getAddedCollectors(
                                                                intCollectors);
    }
    return numFound;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
