// baem_collectorrepository.h              -*-C++-*-
#ifndef INCLUDED_BAEM_COLLECTORREPOSITORY
#define INCLUDED_BAEM_COLLECTORREPOSITORY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a repository for collectors.
//
//@CLASSES:
//   baem_CollectorRepository: a repository for collectors
//
//@SEE_ALSO: baem_collector, baem_integercollector, baem_metricsmanager
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component defines a class, 'baem_CollectorRepository',
// that serves as a repository for 'baem_Collector' and 'baem_IntegerCollector'
// objects.  The collector repository supports operations to create and lookup
// collectors, as well as an operation to collect metric records from the
// collectors in the repository.  Collectors are identified by a metric
// id, which uniquely identifies the metric for which they collect values.  The
// 'getDefaultCollector' (and 'getDefaultIntegerCollector') operations return
// the default collector (or integer collector) for the supplied metric.  The
// 'addCollector' (and 'addIntegerCollector') operations create and return a
// new collector (or integer collector) for the specified metric.  Each
// collector instance can safely collect values from multiple threads, however,
// the collector does use a mutex: Applications anticipating high contention
// for that lock can use 'addCollector' (and 'addIntegerCollector') to obtain
// multiple collectors and thereby reduce contention.  Finally, the
// 'collectAndReset' operation collects and returns metric records from each
// of the collectors in the repository.
//
///Thread Safety
///-------------
// 'baem_CollectorRepository' is fully *thread-safe*, meaning that all
// non-creator operations on a given instance can be safely invoked
// simultaneously from multiple threads.
//
///Usage
///-----
// The following example illustrates creating a 'baem_CollectorRepository',
// then looking up collectors in that repository, and finally collecting values
// from the repository.  We start by creating a repository and looking up 2
// collectors and 2 integer collectors:
//..
//  bslma_Allocator *allocator = bslma_Default::allocator(0);
//  baem_MetricRegistry  metricRegistry(allocator);
//  baem_CollectorRepository repository(&metricRegistry, allocator);
//
//  baem_Collector *collector1 = repository.getDefaultCollector("Test", "C1");
//  baem_Collector *collector2 = repository.getDefaultCollector("Test", "C2");
//  baem_IntegerCollector *intCollector1 =
//                         repository.getDefaultIntegerCollector("Test", "C3");
//  baem_IntegerCollector *intCollector2 =
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
//  bsl::vector<baem_MetricRecord> records(allocator);
//  repository.collectAndReset(&records, metricRegistry.getCategory("Test"));
//      assert(4 == records.size());
//..
// Finally we write the recorded values to the console:
//..
//  bsl::vector<baem_MetricRecord>::const_iterator it;
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_COLLECTOR
#include <baem_collector.h>
#endif

#ifndef INCLUDED_BAEM_INTEGERCOLLECTOR
#include <baem_integercollector.h>
#endif

#ifndef INCLUDED_BAEM_METRICID
#include <baem_metricid.h>
#endif

#ifndef INCLUDED_BAEM_METRICRECORD
#include <baem_metricrecord.h>
#endif

#ifndef INCLUDED_BAEM_METRICREGISTRY
#include <baem_metricregistry.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class baem_Category;

class baem_CollectorRepository_MetricCollectors;  // defined in implementation

                    // ==============================
                    // class baem_CollectorRepository
                    // ==============================

class baem_CollectorRepository {
    // This class defines a fully thread-safe repository mechanism for
    // 'baem_Collector' and 'baem_IntegerCollector' objects.  Collectors are
    // identified in the repository by a 'baem_MetricId' object and also
    // grouped together according to the category of the metric.  This
    // repository supports operations to create, find, and collect metric
    // records from the collectors in the repository.

    // PRIVATE TYPES
    typedef baem_CollectorRepository_MetricCollectors     MetricCollectors;
        // 'MetricCollectors' is an alias for the (private) implementation type
        // that contains the collectors and integer collectors for a single
        // metric id.

    typedef bcema_SharedPtr<MetricCollectors>             MetricCollectorsSPtr;
        // 'MetricCollectorsPtr' is an alias for a shared pointer to a
        // 'baem_MetricRepository_MetricCollectors' object.

    typedef bsl::map<baem_MetricId, MetricCollectorsSPtr> Collectors;
        // 'Collectors' is an alias for a map from a 'baem_MetricId' object
        // to the collectors and integer collectors for that metric.

    typedef bsl::map<const baem_Category *,
                     bsl::vector<MetricCollectors *> >  CategorizedCollectors;
        // 'CategorizedCollectors' is an alias for a map from a category to
        // the list of metric collectors belonging to that category.  Note
        // that each 'MetricCollectors' instance contains all the collectors
        // for a single metric.

    // DATA
    baem_MetricRegistry   *d_registry_p;   // registry of ids (held, not owned)
    Collectors             d_collectors;   // collectors (owned)
    CategorizedCollectors  d_categories;   // map of category => collectors
    mutable bcemt_RWMutex  d_rwMutex;      // data lock
    bslma_Allocator       *d_allocator_p;  // allocator (held, not owned)

    // NOT IMPLEMENTED
    baem_CollectorRepository(const baem_CollectorRepository& );
    baem_CollectorRepository& operator=(const baem_CollectorRepository& );

  private:
    // PRIVATE MANIPULATORS
    MetricCollectors& getMetricCollectors(const baem_MetricId& metricId);
        // Return a reference to the modifiable collectors associated with the
        // specified 'metricId'.  If a collection of collectors for the
        // 'metricId' does not already exist, create one and add it to the map
        // of 'Collectors' ('d_collectors') and also the map of
        // 'CategorizedCollectors' ('d_categories').  The behavior is undefined
        // unless the calling thread has a *write* *lock* to 'd_rwMutex' and
        // 'metricId' is valid.

  public:
    // PUBLIC TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_CollectorRepository,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_CollectorRepository(baem_MetricRegistry *registry,
                             bslma_Allocator     *basicAllocator = 0);
        // Create an empty collector repository that will use the specified
        // 'registry' to identify the metrics for which it manages collectors.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if 'registry' is 0.

    ~baem_CollectorRepository();
        // Free all the collectors in this repository and destroy this object.

    // MANIPULATORS
    void collectAndReset(bsl::vector<baem_MetricRecord> *records,
                         const baem_Category            *category);
        // Append to the specified 'records' the collected metric record
        // values from the collectors in this repository belonging to the
        // specified 'category'; then reset those collectors to their
        // default values.

    void collect(bsl::vector<baem_MetricRecord> *records,
                 const baem_Category            *category);
        // Append to the specified 'records' the collected metric record
        // values from the collectors in this repository belonging to the
        // specified 'category'.  Note that this operation does not reset the
        // managed collectors, so subsequent collection operations will
        // effectively re-collect the current values.

    baem_Collector *getDefaultCollector(const char *category,
                                        const char *metricName);
        // Return the address of the modifiable default collector
        // identified by the specified 'category' and 'metricName'.  If a
        // default collector for the identified metric does not already exist
        // in the repository, create one, add it to the repository, and return
        // its address.  In addition, if the identified metric has not already
        // been registered, add the identified metric to the 'metricRegistry'
        // supplied at construction.  The behavior is undefines unless
        // 'category' and 'metricName' are null-terminated.  Note that this
        // operation is logically equivalent to:
        //..
        //  getDefaultCollector(registry().getId(category, metricName))
        //..

    baem_Collector *getDefaultCollector(const baem_MetricId& metricId);
        // Return the address of the modifiable default collector
        // identified by the specified 'metricId'.  If a default collector for
        // the identified metric does not already exist in the repository,
        // create one, add it to the repository, and return its address.

    baem_IntegerCollector *getDefaultIntegerCollector(const char *category,
                                                      const char *metricName);
        // Return the address of the modifiable default integer collector
        // identified by the specified 'category' and 'metricName'.  If a
        // default integer collector for the identified metric does not
        // already exist in the repository, create one, add it to the
        // repository, and return its address.  In addition, if the identified
        // metric has not already been registered, add the identified metric
        // to the 'metricRegistry' supplied at construction.  The behavior is
        // undefined unless 'category' and 'metricName' are
        // null-terminated.  Note that this operation is logically equivalent
        // to:
        //..
        //  getDefaultIntegerCollector(registry().getId(category, metricName))
        //..

    baem_IntegerCollector *getDefaultIntegerCollector(
                                                const baem_MetricId& metricId);
        // Return the address of the modifiable default integer collector
        // identified by the specified 'metricId'.  If a default integer
        // collector for the identified metric does not already exist in the
        // repository, create one, add it to the repository, and return its
        // address.

    bcema_SharedPtr<baem_Collector> addCollector(const char *category,
                                                 const char *metricName);
        // Return a shared pointer to a newly-created modifiable collector
        // identified by the specified 'category' and 'metricName' and add
        // that collector to the repository.  If is not already registered,
        // also add the identified metric to the 'metricRegistry' supplied at
        // construction.   The behavior is undefined unless 'category' and
        // 'metricName' are null-terminated.  Note that this operation is
        // logically equivalent to:
        //..
        //  addCollector(registry().getId(category, metricName))
        //..

    bcema_SharedPtr<baem_Collector> addCollector(
                                               const baem_MetricId& metricId);
        // Return a shared pointer to a newly-created modifiable collector
        // identified by the specified 'metricId' and add that collector to the
        // repository.  The behavior is undefined unless 'metricId' is a valid
        // id returned by the 'baem_MetricRepository' supplied at construction.

    bcema_SharedPtr<baem_IntegerCollector> addIntegerCollector(
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

    bcema_SharedPtr<baem_IntegerCollector> addIntegerCollector(
                                               const baem_MetricId& metricId);
        // Return a shared pointer to a newly-created modifiable collector
        // identified by the specified 'metricId' and add that collector to the
        // repository.  The behavior is undefined unless 'metricId' is a valid
        // id returned by the 'baem_MetricRepository' supplied at construction.

    int getAddedCollectors(
         bsl::vector<bcema_SharedPtr<baem_Collector> >         *collectors,
         bsl::vector<bcema_SharedPtr<baem_IntegerCollector> >  *intCollectors,
         const baem_MetricId&                                   metricId);
        // Append to the specified 'collectors' and 'intCollectors' shared
        // pointers to any collectors, and integer collectors, collecting
        // values for the metrics identified by the specified 'metricId' that
        // were added using the 'addCollector' or 'addIntegerCollector'
        // methods, and return the combined total number of collectors and
        // integer collectors that were found.  This method does *not* count
        // or return the default collectors for 'metricId'.  The behavior is
        // undefined unless 'metricId' is a valid id returned by the
        // 'baem_MetricRepository' supplied at construction.

    baem_MetricRegistry& registry();
        // Return a reference to the modifiable registry of metrics used by
        // this collector repository.

    // ACCESSORS
    const baem_MetricRegistry& registry() const;
        // Return a reference to the non-modifiable registry of metrics used by
        // this collector repository.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ------------------------------
                       // class baem_CollectorRepository
                       // ------------------------------

// CREATORS
inline
baem_CollectorRepository::baem_CollectorRepository(
                                          baem_MetricRegistry *registry,
                                          bslma_Allocator     *basicAllocator)

: d_registry_p(registry)
, d_collectors(basicAllocator)
, d_categories(basicAllocator)
, d_rwMutex()
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
baem_CollectorRepository::~baem_CollectorRepository()
{
}

// MANIPULATORS
inline
baem_Collector *baem_CollectorRepository::getDefaultCollector(
                                                      const char *category,
                                                      const char *metricName)
{
    return getDefaultCollector(d_registry_p->getId(category, metricName));
}

inline
baem_IntegerCollector *baem_CollectorRepository::getDefaultIntegerCollector(
                                                        const char *category,
                                                        const char *metricName)
{
    return getDefaultIntegerCollector(d_registry_p->getId(category,
                                                          metricName));
}

inline
bcema_SharedPtr<baem_Collector> baem_CollectorRepository::addCollector(
                                                       const char *category,
                                                       const char *metricName)
{
    return addCollector(d_registry_p->getId(category, metricName));
}

inline
bcema_SharedPtr<baem_IntegerCollector>
baem_CollectorRepository::addIntegerCollector(const char *category,
                                              const char *metricName)
{
    return addIntegerCollector(d_registry_p->getId(category, metricName));
}

inline
baem_MetricRegistry& baem_CollectorRepository::registry()
{
    return *d_registry_p;
}

// ACCESSORS
inline
const baem_MetricRegistry& baem_CollectorRepository::registry() const
{
    return *d_registry_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
