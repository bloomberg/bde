// balm_collectorrepository.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_COLLECTORREPOSITORY
#define INCLUDED_BALM_COLLECTORREPOSITORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a repository for collectors.
//
//@CLASSES:
//   balm::CollectorRepository: a repository for collectors
//
//@SEE_ALSO: balm_collector, balm_integercollector, balm_metricsmanager
//
//@DESCRIPTION: This component defines a class, 'balm::CollectorRepository',
// that serves as a repository for 'balm::Collector' and
// 'balm::IntegerCollector' objects.  The collector repository supports
// operations to create and lookup collectors, as well as an operation to
// collect metric records from the collectors in the repository.  Collectors
// are identified by a metric id, which uniquely identifies the metric for
// which they collect values.  The 'getDefaultCollector' (and
// 'getDefaultIntegerCollector') operations return the default collector (or
// integer collector) for the supplied metric.  The 'addCollector' (and
// 'addIntegerCollector') operations create and return a new collector (or
// integer collector) for the specified metric.  Each collector instance can
// can safely collect values from multiple threads, however, the collector does
// use a mutex: Applications anticipating high contention for that lock can use
// 'addCollector' (and 'addIntegerCollector') to obtain multiple collectors and
// thereby reduce contention.  Finally, the 'collectAndReset' operation
// collects and returns metric records from each of the collectors in the
// repository.
//
///Thread Safety
///-------------
// 'balm::CollectorRepository' is fully *thread-safe*, meaning that all
// non-creator operations on a given instance can be safely invoked
// simultaneously from multiple threads.
//
///Usage
///-----
// The following example illustrates creating a 'balm::CollectorRepository',
// then looking up collectors in that repository, and finally collecting values
// from the repository.  We start by creating a repository and looking up 2
// collectors and 2 integer collectors:
//..
//  bslma::Allocator *allocator = bslma::Default::allocator(0);
//  balm::MetricRegistry  metricRegistry(allocator);
//  balm::CollectorRepository repository(&metricRegistry, allocator);
//
//  balm::Collector *collector1 = repository.getDefaultCollector("Test", "C1");
//  balm::Collector *collector2 = repository.getDefaultCollector("Test", "C2");
//  balm::IntegerCollector *intCollector1 =
//                         repository.getDefaultIntegerCollector("Test", "C3");
//  balm::IntegerCollector *intCollector2 =
//                         repository.getDefaultIntegerCollector("Test", "C4");
//
//      assert(collector1    != collector2);
//      assert(collector1    == repository.getDefaultCollector("Test", "C1"));
//      assert(intCollector1 != intCollector2);
//      assert(intCollector1 ==
//             repository.getDefaultIntegerCollector("Test", "C3"));
//..
// We now update the values in those collectors:
//..
//  collector1->update(1.0);
//  collector1->update(2.0);
//  collector2->update(4.0);
//
//  intCollector1->update(5);
//  intCollector2->update(6);
//..
// We can use the repository to collect recorded values from the collectors it
// manages.  Since there are collectors for four metrics, there should be four
// recorded values.  Note the order in which the records are returned is
// undefined.
//..
//  bsl::vector<balm::MetricRecord> records(allocator);
//  repository.collectAndReset(&records, metricRegistry.getCategory("Test"));
//      assert(4 == records.size());
//..
// Finally we write the recorded values to the console:
//..
//  bsl::vector<balm::MetricRecord>::const_iterator it;
//  for (it = records.begin(); it != records.end(); ++it) {
//       bsl::cout << *it << bsl::endl;
//  }
//..
// The output of the for-loop should be:
//..
//  [ Test.C1: 2 3 1 2 ]
//  [ Test.C2: 1 4 4 4 ]
//  [ Test.C3: 1 5 5 5 ]
//  [ Test.C4: 1 6 6 6 ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_COLLECTOR
#include <balm_collector.h>
#endif

#ifndef INCLUDED_BALM_INTEGERCOLLECTOR
#include <balm_integercollector.h>
#endif

#ifndef INCLUDED_BALM_METRICID
#include <balm_metricid.h>
#endif

#ifndef INCLUDED_BALM_METRICRECORD
#include <balm_metricrecord.h>
#endif

#ifndef INCLUDED_BALM_METRICREGISTRY
#include <balm_metricregistry.h>
#endif

#ifndef INCLUDED_BSLMT_RWMUTEX
#include <bslmt_rwmutex.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {


namespace balm {class Category;

class CollectorRepository_MetricCollectors;  // defined in implementation

                         // =========================
                         // class CollectorRepository
                         // =========================

class CollectorRepository {
    // This class defines a fully thread-safe repository mechanism for
    // 'Collector' and 'IntegerCollector' objects.  Collectors are identified
    // in the repository by a 'MetricId' object and also grouped together
    // according to the category of the metric.  This repository supports
    // operations to create, find, and collect metric records from the
    // collectors in the repository.

    // PRIVATE TYPES
    typedef CollectorRepository_MetricCollectors     MetricCollectors;
        // 'MetricCollectors' is an alias for the (private) implementation type
        // that contains the collectors and integer collectors for a single
        // metric id.

    typedef bsl::shared_ptr<MetricCollectors>             MetricCollectorsSPtr;
        // 'MetricCollectorsPtr' is an alias for a shared pointer to a
        // 'MetricRepository_MetricCollectors' object.

    typedef bsl::map<MetricId, MetricCollectorsSPtr> Collectors;
        // 'Collectors' is an alias for a map from a 'MetricId' object to the
        // collectors and integer collectors for that metric.

    typedef bsl::map<const Category *,
                     bsl::vector<MetricCollectors *> >  CategorizedCollectors;
        // 'CategorizedCollectors' is an alias for a map from a category to
        // the list of metric collectors belonging to that category.  Note
        // that each 'MetricCollectors' instance contains all the collectors
        // for a single metric.

    // DATA
    MetricRegistry         *d_registry_p;  // registry of ids (held, not owned)
    Collectors              d_collectors;  // collectors (owned)
    CategorizedCollectors   d_categories;  // map of category => collectors
    mutable bslmt::RWMutex  d_rwMutex;     // data lock
    bslma::Allocator       *d_allocator_p; // allocator (held, not owned)

    // NOT IMPLEMENTED
    CollectorRepository(const CollectorRepository& );
    CollectorRepository& operator=(const CollectorRepository& );

  private:
    // PRIVATE MANIPULATORS
    MetricCollectors& getMetricCollectors(const MetricId& metricId);
        // Return a reference to the modifiable collectors associated with the
        // specified 'metricId'.  If a collection of collectors for the
        // 'metricId' does not already exist, create one and add it to the map
        // of 'Collectors' ('d_collectors') and also the map of
        // 'CategorizedCollectors' ('d_categories').  The behavior is undefined
        // unless the calling thread has a *write* *lock* to 'd_rwMutex' and
        // 'metricId' is valid.

  public:
    // PUBLIC TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CollectorRepository,
                                                    bslma::UsesBslmaAllocator);

    // CREATORS
    CollectorRepository(MetricRegistry        *registry,
                        bslma::Allocator      *basicAllocator = 0);
        // Create an empty collector repository that will use the specified
        // 'registry' to identify the metrics for which it manages collectors.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if 'registry' is 0.

    ~CollectorRepository();
        // Free all the collectors in this repository and destroy this object.

    // MANIPULATORS
    void collectAndReset(bsl::vector<MetricRecord> *records,
                         const Category            *category);
        // Append to the specified 'records' the collected metric record
        // values from the collectors in this repository belonging to the
        // specified 'category'; then reset those collectors to their default
        // values.

    void collect(bsl::vector<MetricRecord> *records,
                 const Category            *category);
        // Append to the specified 'records' the collected metric record
        // values from the collectors in this repository belonging to the
        // specified 'category'.  Note that this operation does not reset the
        // managed collectors, so subsequent collection operations will
        // effectively re-collect the current values.

    Collector *getDefaultCollector(const char *category,
                                   const char *metricName);
        // Return the address of the modifiable default collector identified by
        // the specified null-terminated strings 'category' and 'metricName'.
        // If a collector for the identified metric does not already exist in
        // the repository, create one, add it to the repository, and return its
        // address.  In addition, if the identified metric has not already been
        // registered, add the identified metric to the 'metricRegistry'
        // supplied at construction.  Note that this operation is logically
        // equivalent to:
        //..
        //  getDefaultCollector(registry().getId(category, metricName))
        //..

    Collector *getDefaultCollector(const MetricId& metricId);
        // Return the address of the modifiable default collector identified by
        // the specified 'metricId'.  If a default collector for the identified
        // metric does not already exist in the repository, create one, add it
        // to the repository, and return its address.

    IntegerCollector *getDefaultIntegerCollector(const char *category,
                                                 const char *metricName);
        // Return the address of the modifiable default integer collector
        // identified by the specified 'category' and 'metricName'.  If a
        // default integer collector for the identified metric does not
        // already exist in the repository, create one, add it to the
        // repository, and return its address.  In addition, if the identified
        // metric has not already been registered, add the identified metric
        // to the 'metricRegistry' supplied at construction.  The behavior is
        // undefined unless 'category' and 'metricName' are null-terminated.
        // Note that this operation is logically equivalent to:
        //..
        //  getDefaultIntegerCollector(registry().getId(category, metricName))
        //..

    IntegerCollector *getDefaultIntegerCollector(const MetricId& metricId);
        // Return the address of the modifiable default integer collector
        // identified by the specified 'metricId'.  If a default integer
        // collector for the identified metric does not already exist in the
        // repository, create one, add it to the repository, and return its
        // address.

    bsl::shared_ptr<Collector> addCollector(const char *category,
                                            const char *metricName);
        // Return a shared pointer to a newly-created modifiable collector
        // identified by the specified null-terminated strings 'category' and
        // 'metricName', and add that collector to the repository.  If is not
        // already registered, also add the identified metric to the
        // 'metricRegistry' supplied at construction.  Note that this operation
        // is logically equivalent to:
        //..
        //  addCollector(registry().getId(category, metricName))
        //..

    bsl::shared_ptr<Collector> addCollector(const MetricId& metricId);
        // Return a shared pointer to a newly-created modifiable collector
        // identified by the specified 'metricId' and add that collector to the
        // repository.  The behavior is undefined unless 'metricId' is a valid
        // id returned by the 'MetricRepository' supplied at construction.

    bsl::shared_ptr<IntegerCollector> addIntegerCollector(
                                                       const char *category,
                                                       const char *metricName);
        // Return a shared pointer to a newly created modifiable integer
        // collector identified by the specified 'category' and 'metricName'
        // and add that collector to the repository.  If is not already
        // registered, also add the identified metric to the 'metricRegistry'
        // supplied at construction.  The behavior is undefined unless
        // 'category' and 'metricName' are null-terminated.  Note that this
        // operation is logically equivalent to:
        //..
        //  addIntegerCollector(registry().getId(category, metricName))
        //..

    bsl::shared_ptr<IntegerCollector> addIntegerCollector(
                                                     const MetricId& metricId);
        // Return a shared pointer to a newly-created modifiable collector
        // identified by the specified 'metricId' and add that collector to the
        // repository.  The behavior is undefined unless 'metricId' is a valid
        // id returned by the 'MetricRepository' supplied at construction.

    int getAddedCollectors(
               bsl::vector<bsl::shared_ptr<Collector> >         *collectors,
               bsl::vector<bsl::shared_ptr<IntegerCollector> >  *intCollectors,
               const MetricId&                                   metricId);
        // Append to the specified 'collectors' and 'intCollectors' shared
        // pointers to any collectors, and integer collectors, collecting
        // values for the metrics identified by the specified 'metricId' that
        // were added using the 'addCollector' or 'addIntegerCollector'
        // methods, and return the combined total number of collectors and
        // integer collectors that were found.  This method does *not* count
        // or return the default collectors for 'metricId'.  The behavior is
        // undefined unless 'metricId' is a valid id returned by the
        // 'MetricRepository' supplied at construction.

    MetricRegistry& registry();
        // Return a reference to the modifiable registry of metrics used by
        // this collector repository.

    // ACCESSORS
    const MetricRegistry& registry() const;
        // Return a reference to the non-modifiable registry of metrics used by
        // this collector repository.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // -------------------------
                         // class CollectorRepository
                         // -------------------------

// CREATORS
inline
CollectorRepository::CollectorRepository(MetricRegistry   *registry,
                                         bslma::Allocator *basicAllocator)
: d_registry_p(registry)
, d_collectors(basicAllocator)
, d_categories(basicAllocator)
, d_rwMutex()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
CollectorRepository::~CollectorRepository()
{
}

// MANIPULATORS
inline
Collector *CollectorRepository::getDefaultCollector(const char *category,
                                                    const char *metricName)
{
    return getDefaultCollector(d_registry_p->getId(category, metricName));
}

inline
IntegerCollector *CollectorRepository::getDefaultIntegerCollector(
                                                        const char *category,
                                                        const char *metricName)
{
    return getDefaultIntegerCollector(d_registry_p->getId(category,
                                                          metricName));
}

inline
bsl::shared_ptr<Collector> CollectorRepository::addCollector(
                                                        const char *category,
                                                        const char *metricName)
{
    return addCollector(d_registry_p->getId(category, metricName));
}

inline
bsl::shared_ptr<IntegerCollector>
CollectorRepository::addIntegerCollector(const char *category,
                                         const char *metricName)
{
    return addIntegerCollector(d_registry_p->getId(category, metricName));
}

inline
MetricRegistry& CollectorRepository::registry()
{
    return *d_registry_p;
}

// ACCESSORS
inline
const MetricRegistry& CollectorRepository::registry() const
{
    return *d_registry_p;
}
}  // close package namespace

}  // close enterprise namespace

#endif

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
