// baem_metricsmanager.cpp      -*-C++-*-
#include <baem_metricsmanager.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_metricsmanager_cpp,"$Id$ $CSID$")

#include <baem_metricsample.h>
#include <baem_publisher.h>

#include <bael_log.h>

#include <bcemt_readlockguard.h>
#include <bcemt_writelockguard.h>

#include <bdef_bind.h>
#include <bdef_placeholder.h>
#include <bdetu_systemtime.h>
#include <bdeu_print.h>

#include <bslma_default.h>

///Implementation Note
///-------------------
// The 'baem_MetricsManager' is implemented using 4 classes defined in this
// implementation file:
//
//                     PublicationHelper: namespace for 'publish' template
//                            MapProctor: a proctor for inserting into a map
// baem_MetricsManager_PublisherRegistry: registry of 'baem_Publisher' objects
//  baem_MetricsManager_CallbackRegistry: registry of callback functors
//
// The classes are defined here, in the implementation, to avoid exposing the
// implementation details of the metrics manager in the component header.

namespace BloombergLP {

namespace {

const char LOG_CATEGORY[] = "BAEM.METRICSMANAGER";

struct SampleDescription {
    // This type is used by 'collectSample' to indirectly refer to a series of
    // records in a vector (in case the vector is resized).

    // PUBLIC DATA
    int               d_beginIndex;
    int               d_size;
    bdet_TimeInterval d_elapsedTime;

    // CREATORS
    SampleDescription(int                      beginIndex,
                      int                      size,
                      const bdet_TimeInterval& elapsedTime)
    : d_beginIndex(beginIndex)
    , d_size(size)
    , d_elapsedTime(elapsedTime)
    {
    }
};

}  // close unnamed namespace

                // ============================================
                // struct baem_MetricsManager_PublicationHelper
                // ============================================

struct baem_MetricsManager_PublicationHelper {
    // This class provides a namespace for auxiliary template operations for
    // the 'baem_MetricsManager' class.  The operations provided serve as
    // private manipulators of a 'baem_MetricsManager' object but, they are
    // defined here, in the implementation, to avoid defining the (substantive)
    // private template operations in the header.  Note that this class is a
    // friend of 'baem_MetricsManager'.

    typedef bsl::map<baem_Publisher *, baem_MetricSample> SampleCache;
        // An alias for a mapping of 'baem_Publisher' to the
        // 'baem_MetricSample' for that publisher.

    // CLASS METHODS
    static void updateSampleCache(SampleCache                   *sampleCache,
                                  baem_Publisher                *publisher,
                                  const baem_MetricSampleGroup&  sampleGroup,
                                  const bdet_DatetimeTz&         timeStamp);
        // Update the specified 'sampleCache' entry for the specified
        // 'publisher' with the specified 'sampleGroup' collected at the
        // specified 'timeStamp'.  If a 'baem_MetricSample' does not already
        // exist for 'publisher' in the 'sampleCache', create one and add it
        // to the 'sampleCache'.

    static void collect(bsl::vector<baem_MetricRecord> *records,
                        bdet_TimeInterval              *elapsedTime,
                        baem_MetricsManager            *manager,
                        const baem_Category            *category,
                        const bdet_TimeInterval&        now,
                        bool                            resetFlag);
        // Append to the specified 'records' the metrics collected from the
        // specified 'manager' for the specified 'category', and load into
        // specified 'elapsedTime' the time interval from when they were last
        // reset to the specified 'currentTime'; if 'resetFlag' is 'true',
        // reset the metrics to their default state.  This operation will
        // collect aggregated metric record values from metric collection
        // callbacks in 'manager.d_callbackRegistry' as well as from
        // 'baem_Collector' objects owned by 'manager.d_collectors', and if
        // 'resetFlag' is 'true', reset those collectors and callbacks to
        // their default state.  If a 'category' has not been previously
        // reset, then the 'elapsedTime' is computed from the creation of
        // 'manager'.  Note that this operation does *not* test if 'category'
        // is enabled.

    template <typename ConstForwardCategoryIterator>
    static void publish(baem_MetricsManager                 *manager,
                        const ConstForwardCategoryIterator&  categoriesBegin,
                        const ConstForwardCategoryIterator&  categoriesEnd,
                        bool                                 resetFlag);
        // Publish, to publishers managed by the specified 'manager', metrics
        // records for the sequence of categories from the specified
        // 'categoriesBegin' to the specified 'categoriesEnd' collected from
        // metrics managed by 'manager', and if 'resetFlag' is 'true', reset
        // the metrics being collected to their default state.  The
        // specified template type 'ConstForwardCategoryIterator' must be a
        // forward iterator over a container of pointers to constant
        // 'baem_Category' objects, e.g.:
        //..
        //  bsl::set<const baem_Category *>::const_iterator
        //..
        // This operation will collect aggregated metric record values from
        // metric collection callbacks in 'manager.d_callbackRegistry' as well
        // as from 'baem_Collector' objects owned by 'manager.d_collectors',
        // for the categories being iterated over, and then publish those
        // records using any publishers in 'manager.d_publishers' registered
        // for those categories.  The map of previous publication times,
        // 'manager.d_prevResetTimes', is used to determine the time interval
        // that the collected metrics provide values for, and is updated as
        // part of this operation.  This operation obtains a *lock* on
        // 'manager.d_publishLock' and then a *read* *lock* on
        // 'manager.d_rwLock' (to access the properties of 'manager').  The
        // behavior is undefined unless each category appears only once between
        // 'categoriesBegin' and 'categoriesEnd'.  Note that this class is a
        // friend of 'baem_MetricsManager' and has access to the private data
        // members of 'manager'.
};

                          // ================
                          // class MapProctor
                          // ================

template <typename CONTAINER>
class MapProctor {
    // This class implements a proctor that, unless 'release' is called,
    // erases an element from a templatized container object.  The templatized
    // type 'CONTAINER' may be any type that supports an 'erase' operation
    // taking a 'CONTAINER::iterator' object, however it was explicitly
    // intended to support 'bsl::map' and 'bsl::hash_map' objects.  On
    // construction, a 'MapProctor' is provided the address of a 'CONTAINER'
    // object and an iterator (i.e., 'CONTAINER::iterator') into that object.
    // On destruction, if 'release()' has not been called, a 'MapProctor'
    // object will call 'erase' on the supplied 'CONTAINER', passing the
    // supplied 'CONTAINER::iterator'.  If 'release()' is called on a
    // 'MapProctor' object, then the 'MapProctor' object's destructor will
    // have no effect.

    // DATA
    CONTAINER                      *d_map;       // managed map

    bsl::pair<typename CONTAINER::iterator,
              bool>                 d_iterator;  // map element to remove

    // NOT IMPLEMENTED
    MapProctor(const MapProctor& );
    MapProctor& operator=(const MapProctor& );

  public:
    // CREATORS
    MapProctor(CONTAINER *map, const typename CONTAINER::iterator& iterator);
        // Create a proctor object that will manage the element in the
        // specified 'map' indicated by the specified 'iterator'.  Unless
        // 'release()' is called prior to this object's destruction, this
        // object's destructor will 'erase' the 'iterator' from the 'map'
        // (i.e., 'map->erase(iterator)').  The behavior is undefined unless
        // 'map' is a valid address of a 'CONTAINER' object, and 'iterator' is
        // a valid iterator into that 'map', and remains a valid iterator for
        // the lifetime of this object.

    MapProctor(CONTAINER                                      *map,
               bsl::pair<typename CONTAINER::iterator, bool>&  insertResult);
        // Create a proctor object that will manage the element in the
        // specified 'map' indicated by the specified 'insertResult'.   The
        // boolean value 'insertResult.second' indicates whether the proctor
        // is 'active' and should manage the element indicated by
        // 'insertResult.first'.  If 'insertResult.second' is 'true' and
        // 'release()' is not called prior to this object's destruction, this
        // object's destructor will 'erase' the 'insertResult' iterator from
        // the 'map' (i.e., 'map->erase(insertResult.first)').  If
        // 'insertResult.second' is 'false' or 'release()' is called prior to
        // this object's destruction, then this object's destructor will have
        // no effect.  The behavior is undefined unless 'map' is a valid
        // address of a 'CONTAINER' object, and either 'insertResult.second' is
        // 'false' or 'insertResult.first' is a valid iterator into 'map' and
        // remains valid for the lifetime of this object.  Note that the type
        // of 'insertResult' matches the return type of 'bsl::map::insert()'
        // and 'bsl::hash_map::insert()'.

    ~MapProctor();
        // If 'active()' is 'true', erase the element indicated by the
        // iterator supplied at construction from the map supplied at
        // construction, and destroy this proctor.

    // MANIPULATORS
    void release();
        // Set 'active()' to 'false'.  After invoking this method, destroying
        // this object will have no effect on the map supplied at construction.
};

               // ===========================================
               // class baem_MetricsManager_PublisherRegistry
               // ===========================================

class baem_MetricsManager_PublisherRegistry {
    // This class provides a mechanism for associating a 'baem_Publisher'
    // object with one or more categories, or associating a 'baem_Publisher'
    // with every category.  The 'baem_Metricsmanager_PublisherRegistry' is
    // responsible for managing the registration of 'baem_Publisher' objects
    // for a 'baem_MetricsManager' object.

    // PRIVATE TYPES
    template <typename T>
    struct SharedPtrLess {
        // A structure containing the less-than comparison operator for shared
        // pointers.  Used for creating stl maps and sets keyed on shared
        // pointers.

        bool operator()(const bcema_SharedPtr<T>& lhs,
                        const bcema_SharedPtr<T>& rhs) const
            // Return 'true' if 'lhs' is ordered before 'rhs', and return
            // 'false' otherwise.  'lhs' is ordered before 'rhs' if the
            // address pointed to by 'lhs' is less than the address pointed to
            // by 'rhs'.  Note that this does not compare the values of type T.
            {
                return lhs.ptr() < rhs.ptr();
            }
    };

    typedef bcema_SharedPtr<baem_Publisher>           PublisherPtr;
        // 'PublisherPtr' is an alias for a shared pointer to a
        // 'baem_Publisher' object.

    typedef bsl::multimap<const baem_Category *,
                          PublisherPtr>               SpecificPublishers;
        // 'SpecificPublishers' is an alias for a map from a category to the
        // set of publishers for that category.

    typedef bsl::set<PublisherPtr,
                     SharedPtrLess<baem_Publisher> >  PublisherSet;
        // 'PublisherSet' type is an alias for set of managed pointers to
        // publisher objects.

    typedef bsl::map<const baem_Category *,
                     SpecificPublishers::iterator>    PublisherRegistration;
        // 'PublisherRegistration' is an alias for a map, for a particular
        // publisher, from a category address to the iterator into a
        // 'SpecificPublishers' multimap where that publisher is registered.

    typedef bsl::map<bcema_SharedPtr<baem_Publisher>,
                     PublisherRegistration,
                     SharedPtrLess<baem_Publisher> >  RegistrationInfo;
        // 'RegistrationInfo' is an alias for a map from a publisher to the
        // iterators into a 'SpecificPublishers' multimap where that publisher
        // is registered.  It serves as a reverse map for a
        // 'SpecificPublishers'.

    // DATA
    SpecificPublishers  d_specificPublishers;  // map of category to set of
                                               // publishers for that category

    RegistrationInfo    d_registry;            // map of publisher to set of
                                               // categories for which it
                                               // publishes

    PublisherSet        d_generalPublishers;   // set of publishers publishing
                                               // for 'all' categories

    bslma_Allocator    *d_allocator_p;         // allocator (held, not owned)

    // NOT IMPLEMENTED
    baem_MetricsManager_PublisherRegistry(
                                 const baem_MetricsManager_PublisherRegistry&);
    baem_MetricsManager_PublisherRegistry& operator=(
                                 const baem_MetricsManager_PublisherRegistry&);

  public:
    // PUBLIC TYPES
    typedef PublisherSet::iterator general_iterator;
        // An alias for an iterator over the general publishers of this
        // registry.  This should only be used with a lock on the properties
        // of the 'baem_MetricsManager' (therefore, it is not exposed to
        // clients).

    typedef SpecificPublishers::iterator specific_iterator;
        // An alias for an iterator over the specific publishers of this
        // registry.  This should only be used with a lock on the properties
        // of the 'baem_MetricsManager' (therefore, it is not exposed to
        // clients).

    // PUBLIC TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_MetricsManager_PublisherRegistry,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_MetricsManager_PublisherRegistry(bslma_Allocator *basicAllocator);
        // Create an empty publisher registry.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~baem_MetricsManager_PublisherRegistry();
        // Destroy this publisher registry.

    // MANIPULATORS
    int addGeneralPublisher(const bcema_SharedPtr<baem_Publisher>& publisher);
        // Add the specified 'publisher' to the set of publishers that should
        // be used to propagate records for *every* category published.
        // Return 0 on success, and a non-zero value with no effect if
        // 'publisher' has already been registered.  Note that this method
        // will have no effect and return a non-zero value if 'publisher' was
        // previously registered for one or more specific categories (using
        // the alternative 'addSpecificPublisher' method).

    int addSpecificPublisher(
                          const baem_Category                    *category,
                          const bcema_SharedPtr<baem_Publisher>&  publisher);
        // Add the specified 'publisher' to the set of publishers that should
        // be used to publish metrics for the specified 'category'.  Return 0
        // on success, and a non-zero value with no effect if 'publisher'
        // has already been registered to publish 'category'.  Note that
        // this method will return a non-zero value if the publisher has
        // previously been registered to publish every category (using the
        // alternative 'addGeneralPublisher' method).

    int removePublisher(const baem_Publisher *publisher);
        // Remove the specified 'publisher' from this registry.  Return 0 on
        // success, or a non-zero value if the publisher' cannot be found.
        // Note that metrics should no longer be published using 'publisher'
        // for any category.

    general_iterator beginGeneral();
        // Return an iterator at the beginning of the sequence of general
        // iterators held by this registry.

    general_iterator endGeneral();
        // Return an iterator one past the end of the sequence of general
        // iterators held by this registry.

    specific_iterator beginSpecific();
        // Return an iterator at the beginning of the sequence of specific
        // iterators held by this registry.

    specific_iterator endSpecific();
        // Return an iterator one past the end of the sequence of specific
        // iterators held by this registry.

    specific_iterator lowerBound(const baem_Category *category);
        // Return an iterator at the beginning of the range of specific
        // publishers registered for the specified 'category'.

    specific_iterator upperBound(const baem_Category *category);
        // Return an iterator one past the range of specific publishers
        // registered for the specified 'category'.

    // ACCESSORS
    int findGeneralPublishers(bsl::vector<baem_Publisher *> *publishers) const;
        // Append to the specified 'publishers', the addresses of publishers
        // registered to publish metrics for every category.  Return the
        // number of publishers found.  Note that this method will not find
        // publishers associated with individual categories (i.e., category
        // specific publishers).

    int findSpecificPublishers(bsl::vector<baem_Publisher *> *publishers,
                               const baem_Category           *category) const;
        // Append to the specified 'publishers', the addresses of publishers
        // associated with the (particular) specified 'category'.  Return the
        // number of publishers found for 'category'.  Note that this method
        // will not find publishers registered for every category (i.e,
        // general publishers).
};

               // ==========================================
               // class baem_MetricsManager_CallbackRegistry
               // ==========================================

class baem_MetricsManager_CallbackRegistry {
    // This class provides a mechanism for associating a
    // 'MetricsCollectionCallback' functor with a category.  The
    // 'baem_Metricsmanager_CallbackRegistry' is responsible for managing the
    // registration of 'MetricsCollectionCallback' objects for a
    // 'baem_MetricsManager' object.

    // PRIVATE TYPES
    typedef baem_MetricsManager::CallbackHandle      CallbackHandle;
    typedef baem_MetricsManager::RecordsCollectionCallback
                                                     RecordsCollectionCallback;
    typedef bsl::multimap<const baem_Category *,
                          RecordsCollectionCallback> CallbackMap;
        // A 'CallbackMap' type is a map from a category to the sequence of
        // 'MetricsCollectionCallback' functions for that category.

    typedef bsl::map<CallbackHandle,
                     CallbackMap::iterator>          CallbackHandleMap;
        // A 'CallbackHandleMap' type is a map from a callback handle to the
        // iterator into a 'CallbackMap' object for that handle.

    // DATA
    int               d_nextHandle;  // next 'CallbackHandle' value
    CallbackMap       d_callbacks;   // map from a category to callbacks
    CallbackHandleMap d_handles;     // map from a callback handle to callback

    // NOT IMPLEMENTED
    baem_MetricsManager_CallbackRegistry(
                                  const baem_MetricsManager_CallbackRegistry&);
    baem_MetricsManager_CallbackRegistry& operator=(
                                  const baem_MetricsManager_CallbackRegistry&);
  public:
    // TYPES
    typedef CallbackMap::iterator iterator;
        // An alias for an iterator over the callbacks in this registry.
        // This should only be used with a lock on the properties of the
        // 'baem_MetricsManager' (therefore, it is not exposed to clients).

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_MetricsManager_CallbackRegistry,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_MetricsManager_CallbackRegistry(bslma_Allocator *basicAllocator = 0);
        // Create an empty callback registry.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~baem_MetricsManager_CallbackRegistry();
        // Destroy this callback registry.

    // MANIPULATORS
    CallbackHandle registerCollectionCallback(
                                  const baem_Category              *category,
                                  const RecordsCollectionCallback&  callback);
        // Register the specified 'callback' to record metrics for the
        // specified 'category', and return an opaque integer handle that can
        // be used later to remove the 'callback'.  The behavior is undefined
        // if 'callback' returns metrics belonging to metric categories
        // other than 'category'.  Note that the same 'callback' can be
        // registered for a 'category' multiple times, and that registering
        // the same 'callback' multiple times will result in an equal number of
        // invocations of the 'callback' when publishing the 'category'.

    int removeCollectionCallback(CallbackHandle handle);
        // Remove the callback associated with the specified 'handle'.  Return
        // 0 on success, or a non-zero value if 'handle' cannot be found.

    iterator begin();
        // Return an iterator at the beginning of the collection of
        // callbacks.

    iterator lowerBound(const baem_Category *category);
        // Return an iterator at the beginning of the range of callbacks
        // registered for the specified 'category'.

    iterator upperBound(const baem_Category *category);
        // Return an iterator one past the range of callbacks registered for
        // the specified 'category'.

    iterator end();
        // Return an iterator one past the end of the collection of callbacks.

    // ACCESSORS
    int findCallbacks(
               bsl::vector<const RecordsCollectionCallback *> *callbacks,
               const baem_Category                            *category) const;
        // Append to the specified 'callbacks' the addresses to any metric
        // collection callbacks registered for the specified 'category'.
        // Return the number of callbacks found for the 'category'.
};

// ===========================================================================
//                           FUNCTION DEFINITIONS
// ===========================================================================

                // --------------------------------------------
                // struct baem_MetricsManager_PublicationHelper
                // --------------------------------------------

void baem_MetricsManager_PublicationHelper::updateSampleCache(
                               SampleCache                   *sampleCache,
                               baem_Publisher                *publisher,
                               const baem_MetricSampleGroup&  sampleGroup,
                               const bdet_DatetimeTz&         timeStamp)
{
    SampleCache::iterator it = sampleCache->find(publisher);
    if (it == sampleCache->end()) {
        baem_MetricSample newSample;
        newSample.setTimeStamp(timeStamp);
        it = sampleCache->insert(bsl::make_pair(publisher, newSample)).first;
    }
    it->second.appendGroup(sampleGroup);
}

void baem_MetricsManager_PublicationHelper::collect(
                                   bsl::vector<baem_MetricRecord> *records,
                                   bdet_TimeInterval              *elapsedTime,
                                   baem_MetricsManager            *manager,
                                   const baem_Category            *category,
                                   const bdet_TimeInterval&        currentTime,
                                   bool                            resetFlag)
{
    typedef baem_MetricsManager::RecordsCollectionCallback Callback;
    typedef bsl::vector<const Callback *>                  CBVector;
    typedef baem_MetricsManager_CallbackRegistry::iterator CBRegistryIterator;
    CBVector callbacks;

    CBRegistryIterator cbkIt  = manager->d_callbacks->lowerBound(category);
    CBRegistryIterator cbkEnd = manager->d_callbacks->upperBound(category);

    // Invoke the metric collection callback functions.
    for (; cbkIt != cbkEnd; ++cbkIt) {
        (cbkIt->second)(records, resetFlag);
    }

    // Collect records from the collector repository.
    if (resetFlag) {
        manager->d_collectors.collectAndReset(records, category);
    } else {
        manager->d_collectors.collect(records, category);
    }

    // Compute the elapsed time since the previous reset, and if 'resetFlag'
    // is 'true', update the last reset time to 'currentTime'.
    baem_MetricsManager::LastResetTimes::iterator tmIt =
                                      manager->d_prevResetTimes.find(category);
    if (tmIt == manager->d_prevResetTimes.end()) {
        *elapsedTime = currentTime - manager->d_creationTime;
        if (resetFlag) {
            manager->d_prevResetTimes.insert(bsl::make_pair(category,
                                                            currentTime));
        }
    }
    else {
        *elapsedTime = currentTime - tmIt->second;
        if (resetFlag) {
            tmIt->second = currentTime;
        }
    }
}

template <typename ConstForwardCategoryIterator>
void baem_MetricsManager_PublicationHelper::publish(
                       baem_MetricsManager                 *manager,
                       const ConstForwardCategoryIterator&  categoriesBegin,
                       const ConstForwardCategoryIterator&  categoriesEnd,
                       bool                                 resetFlag)

{
    if (categoriesBegin == categoriesEnd) {
        return;                                                       // RETURN
    }

    typedef bsl::map<baem_Publisher *, baem_MetricSample>  SampleCache;
    typedef bsl::vector<bcema_SharedPtr<bsl::vector<baem_MetricRecord> > >
                                                           RecordBuffer;

    // Iterate over the categories, storing their records in a 'RecordBuffer'
    // and populating the samples in the 'SampleCache'.  The samples in the
    // sample cache refer to records in the record buffer.
    RecordBuffer recordBuffer;  // holds onto collected record vectors

    SampleCache  sampleCache;   // publisher -> sample (samples point to
                                // records in the 'recordBuffer')

    bdet_DatetimeTz   timeStamp(bdetu_SystemTime::nowAsDatetimeUtc(), 0);
    bdet_TimeInterval now = bdetu_SystemTime::now();

    // Lock the publication lock *then* lock the other object properties.
    bcemt_LockGuard<bcemt_Mutex> publishGuard(&manager->d_publishLock);
    bcemt_ReadLockGuard<bcemt_RWMutex> propertiesGuard(&manager->d_rwLock);

    // Build the 'sampleCache' by iterating over the categories and collecting
    // records for those categories.
    ConstForwardCategoryIterator catIt;
    for (catIt = categoriesBegin; catIt != categoriesEnd; ++catIt) {
        if (!(*catIt)->enabled()) {
            continue;
        }
        bcema_SharedPtr<bsl::vector<baem_MetricRecord> > records;
        records.createInplace();

        // Hold the elapsed time over which these metrics were collected.
        bdet_TimeInterval elapsedTime;

        // Collect the metrics.
        collect(records.ptr(), &elapsedTime, manager, *catIt, now, resetFlag);

        // If their are no collected records then this category can be ignored.
        if (records->empty()) {
            continue;
        }

        if (elapsedTime == bdet_TimeInterval()) {
            BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
            BAEL_LOG_ERROR << "Invalid elapsed time interval of 0 for "
                           << "published metrics." << BAEL_LOG_END;
            elapsedTime.addNanoseconds(1);
        }

        // Append the collected records to the buffer of records.
        recordBuffer.push_back(records);
        baem_MetricSampleGroup sampleGroup(records->data(),
                                           records->size(),
                                           elapsedTime);

        // Add 'sampleGroup' to all the general publishers and specific
        // publishers for 'category'.
        baem_MetricsManager_PublisherRegistry::general_iterator gIt =
                                         manager->d_publishers->beginGeneral();
        for (; gIt != manager->d_publishers->endGeneral(); ++gIt) {
            updateSampleCache(&sampleCache, gIt->ptr(),
                              sampleGroup, timeStamp);
        }

        baem_MetricsManager_PublisherRegistry::specific_iterator sIt =
                                     manager->d_publishers->lowerBound(*catIt);
        baem_MetricsManager_PublisherRegistry::specific_iterator sEnd =
                                     manager->d_publishers->upperBound(*catIt);
        for (; sIt != sEnd; ++sIt) {
            updateSampleCache(&sampleCache, sIt->second.ptr(),
                              sampleGroup, timeStamp);
        }
    }

    // We now have a 'sampleCache' containing a map from a publisher to the
    // sample to publish for that publisher.  Iterate over the 'sampleCache'
    // and publish those samples.
    SampleCache::iterator smplIt;
    for (smplIt = sampleCache.begin(); smplIt != sampleCache.end(); ++smplIt) {
        baem_Publisher     *publisher = smplIt->first;
        baem_MetricSample&  sample    = smplIt->second;

        publisher->publish(sample);
    }
}

                          // ----------------
                          // class MapProctor
                          // ----------------

// CREATORS
template <typename CONTAINER>
inline
MapProctor<CONTAINER>::MapProctor(
                                 CONTAINER                           *map,
                                 const typename CONTAINER::iterator&  iterator)
: d_map(map)
, d_iterator(bsl::make_pair(iterator, true))
{
}

template <typename CONTAINER>
inline
MapProctor<CONTAINER>::MapProctor(
                  CONTAINER                                      *map,
                  bsl::pair<typename CONTAINER::iterator, bool>&  insertResult)
: d_map(map)
, d_iterator(insertResult)
{
}

template <typename CONTAINER>
inline
MapProctor<CONTAINER>::~MapProctor()
{
    if (0 != d_map && d_iterator.second) {
        d_map->erase(d_iterator.first);
    }
}

// MANIPULATORS
template <typename CONTAINER>
inline
void MapProctor<CONTAINER>::release()
{
    d_map = 0;
}

               // -------------------------------------------
               // class baem_MetricsManager_PublisherRegistry
               // -------------------------------------------

baem_MetricsManager_PublisherRegistry::baem_MetricsManager_PublisherRegistry(
                                              bslma_Allocator *basicAllocator)
: d_specificPublishers(basicAllocator)
, d_registry(basicAllocator)
, d_generalPublishers(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

baem_MetricsManager_PublisherRegistry::~baem_MetricsManager_PublisherRegistry()
{
}

// MANIPULATORS
int baem_MetricsManager_PublisherRegistry::addGeneralPublisher(
                              const bcema_SharedPtr<baem_Publisher>& publisher)
{
    if (d_generalPublishers.end() !=
        d_generalPublishers.find(publisher)) {
        // This 'publisher' is already registered to publish every category.

        return -1;                                                    // RETURN
    }

    // Note that it is possible for an empty 'RegistrationInfo' to have been
    // created for 'publisher' if an exception (e.g., an allocation exception)
    // occurred during the 'addSpecificPublisher' method call.
    RegistrationInfo::const_iterator it = d_registry.find(publisher);
    if (it != d_registry.end() && !it->second.empty()) {
        // This 'publisher' is already registered to publish a 'category'.

        return -1;                                                    // RETURN
    }

    d_generalPublishers.insert(publisher);
    return 0;
}

int baem_MetricsManager_PublisherRegistry::addSpecificPublisher(
                            const baem_Category                    *category,
                            const bcema_SharedPtr<baem_Publisher>&  publisher)
{
    if (d_generalPublishers.end() !=
        d_generalPublishers.find(publisher)) {
        // This 'publisher' is already registered to publish every category.

        return -1;                                                    // RETURN
    }

    PublisherRegistration& registration = d_registry[publisher];
    if (registration.end() != registration.find(category)) {
        // This 'publisher' is already registered to publish this 'category'.
        return -1;                                                    // RETURN
    }

    // This 'publisher' has not been registered for this 'category'.  Add it to
    // 'd_specificPublishers' and the reverse map 'd_registry'.
    SpecificPublishers::iterator it =
            d_specificPublishers.insert(SpecificPublishers::value_type(
                                                      category, publisher));

    MapProctor<SpecificPublishers> mapProctor(&d_specificPublishers, it);

    registration.insert(PublisherRegistration::value_type(category, it));

    mapProctor.release();

    return 0;
}

int baem_MetricsManager_PublisherRegistry::removePublisher(
                                               const baem_Publisher *publisher)
{
    bcema_SharedPtr<baem_Publisher> publisherSPtr(
                              const_cast<baem_Publisher *>(publisher),
                              bcema_SharedPtrNilDeleter(),
                              0);

    PublisherSet::iterator allIt = d_generalPublishers.find(publisherSPtr);
    if (allIt != d_generalPublishers.end()) {
        // The 'publisher' is in the set of publishers publishing every
        // category.

        d_generalPublishers.erase(allIt);

        // Since this publisher was registered as a general publisher it
        // should not have been registered as a category specific publisher.
        BSLS_ASSERT(d_registry.end() == d_registry.find(publisherSPtr) ||
                    d_registry.find(publisherSPtr)->second.empty());

        return 0;                                                     // RETURN
    }

    RegistrationInfo::iterator regIt = d_registry.find(publisherSPtr);
    if (regIt != d_registry.end()) {
        PublisherRegistration& registration = regIt->second;
        PublisherRegistration::iterator prIt = registration.begin();
        for (; prIt != registration.end(); ++prIt) {
            d_specificPublishers.erase(prIt->second);
        }

        d_registry.erase(regIt);
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
baem_MetricsManager_PublisherRegistry::general_iterator
baem_MetricsManager_PublisherRegistry::beginGeneral()
{
    return d_generalPublishers.begin();
}

inline
baem_MetricsManager_PublisherRegistry::general_iterator
baem_MetricsManager_PublisherRegistry::endGeneral()
{
    return d_generalPublishers.end();
}

inline
baem_MetricsManager_PublisherRegistry::specific_iterator
baem_MetricsManager_PublisherRegistry::beginSpecific()
{
    return d_specificPublishers.begin();
}

inline
baem_MetricsManager_PublisherRegistry::specific_iterator
baem_MetricsManager_PublisherRegistry::endSpecific()
{
    return d_specificPublishers.end();
}

inline
baem_MetricsManager_PublisherRegistry::specific_iterator
baem_MetricsManager_PublisherRegistry::lowerBound(
                                                const baem_Category *category)
{
    return d_specificPublishers.lower_bound(category);
}

inline
baem_MetricsManager_PublisherRegistry::specific_iterator
baem_MetricsManager_PublisherRegistry::upperBound(
                                                const baem_Category *category)
{
    return d_specificPublishers.upper_bound(category);
}

// ACCESSORS
int baem_MetricsManager_PublisherRegistry::findGeneralPublishers(
                               bsl::vector<baem_Publisher *> *publishers) const
{
    int count = d_generalPublishers.size();
    if (0 == count) {
        return -0;                                                    // RETURN
    }
    publishers->reserve(publishers->size() + count);
    PublisherSet::iterator it = d_generalPublishers.begin();
    for (; it != d_generalPublishers.end(); ++it) {
        publishers->push_back(it->ptr());
    }
    return count;
}

int baem_MetricsManager_PublisherRegistry::findSpecificPublishers(
                               bsl::vector<baem_Publisher *> *publishers,
                               const baem_Category           *category) const
{
    int count = d_specificPublishers.count(category);
    if (0 == count) {
        return count;                                                 // RETURN
    }
    publishers->reserve(publishers->size() + count);
    SpecificPublishers::const_iterator pbIt =
                                    d_specificPublishers.lower_bound(category);
    SpecificPublishers::const_iterator pbEnd =
                                    d_specificPublishers.upper_bound(category);
    for (; pbIt != pbEnd; ++pbIt) {
        publishers->push_back(pbIt->second.ptr());
    }
    return count;
}

               // ------------------------------------------
               // class baem_MetricsManager_CallbackRegistry
               // ------------------------------------------

// CREATORS
baem_MetricsManager_CallbackRegistry::baem_MetricsManager_CallbackRegistry(
                                               bslma_Allocator *basicAllocator)
: d_nextHandle(0)
, d_callbacks(basicAllocator)
, d_handles(basicAllocator)
{
}

baem_MetricsManager_CallbackRegistry::~baem_MetricsManager_CallbackRegistry()
{
}

// MANIPULATORS
baem_MetricsManager_CallbackRegistry::CallbackHandle
baem_MetricsManager_CallbackRegistry::registerCollectionCallback(
                                    const baem_Category              *category,
                                    const RecordsCollectionCallback&  callback)
{
    int handle = d_nextHandle++;
    CallbackMap::iterator it =
               d_callbacks.insert(CallbackMap::value_type(category, callback));

    MapProctor<CallbackMap> proctor(&d_callbacks, it);

    d_handles.insert(CallbackHandleMap::value_type(handle, it));

    proctor.release();

    return handle;
}

int baem_MetricsManager_CallbackRegistry::removeCollectionCallback(
                                                         CallbackHandle handle)
{
    CallbackHandleMap::iterator it = d_handles.find(handle);
    if (it == d_handles.end()) {
        return -1;
    }

    d_callbacks.erase(it->second);
    d_handles.erase(it);
    return 0;
}

inline
baem_MetricsManager_CallbackRegistry::iterator
baem_MetricsManager_CallbackRegistry::begin()
{
    return d_callbacks.begin();
}

inline
baem_MetricsManager_CallbackRegistry::iterator
baem_MetricsManager_CallbackRegistry::lowerBound(const baem_Category *category)
{
    return d_callbacks.lower_bound(category);
}

inline
baem_MetricsManager_CallbackRegistry::iterator
baem_MetricsManager_CallbackRegistry::upperBound(const baem_Category *category)
{
    return d_callbacks.upper_bound(category);
}

inline
baem_MetricsManager_CallbackRegistry::iterator
baem_MetricsManager_CallbackRegistry::end()
{
    return d_callbacks.end();
}

// ACCESSORS
int baem_MetricsManager_CallbackRegistry::findCallbacks(
               bsl::vector<const RecordsCollectionCallback *> *callbacks,
               const baem_Category                            *category) const
{
    int count = d_callbacks.count(category);
    if (0 == count) {
        return 0;
    }
    callbacks->reserve(callbacks->size() + count);
    CallbackMap::const_iterator cbkIt  = d_callbacks.lower_bound(category);
    CallbackMap::const_iterator cbkEnd = d_callbacks.upper_bound(category);
    for (; cbkIt != cbkEnd; ++cbkIt) {
        callbacks->push_back(&cbkIt->second);
    }
    return count;
}

                         // -------------------------
                         // class baem_MetricsManager
                         // -------------------------

// CREATORS
baem_MetricsManager::baem_MetricsManager(bslma_Allocator *basicAllocator)
: d_metricRegistry(basicAllocator)
, d_collectors(&d_metricRegistry, basicAllocator)
, d_callbacks(0)
, d_publishers(0)
, d_creationTime(bdetu_SystemTime::now())
, d_prevResetTimes(basicAllocator)
, d_publishLock()
, d_rwLock()
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_callbacks.load(
             new (*d_allocator_p) baem_MetricsManager_CallbackRegistry(
                                                                d_allocator_p),
             d_allocator_p);

    d_publishers.load(
             new (*d_allocator_p) baem_MetricsManager_PublisherRegistry(
                                                               d_allocator_p),
             d_allocator_p);
}

baem_MetricsManager::~baem_MetricsManager()
{
}

// MANIPULATORS
void baem_MetricsManager::collectSample(
                                    baem_MetricSample              *sample,
                                    bsl::vector<baem_MetricRecord> *records,
                                    bool                            resetFlag)
{
    bsl::vector<const baem_Category *> allCategories;
    d_metricRegistry.getAllCategories(&allCategories);
    collectSample(sample,
                  records,
                  allCategories.data(),
                  allCategories.size(),
                  resetFlag);
}

void baem_MetricsManager::collectSample(
                               baem_MetricSample              *sample,
                               bsl::vector<baem_MetricRecord> *records,
                               const baem_Category     * const categories[],
                               int                             numCategories,
                               bool                            resetFlag)
{
    bdet_DatetimeTz   timeStamp(bdetu_SystemTime::nowAsDatetimeUtc(), 0);
    bdet_TimeInterval now = bdetu_SystemTime::now();

    sample->setTimeStamp(timeStamp);

    // We use an intermediate structure to hold indirect references into
    // 'records' in case 'records' must be resized.
    bsl::vector<SampleDescription> samples;
    samples.reserve(numCategories);

    // Lock the publication lock *then* lock the other object properties.
    bcemt_LockGuard<bcemt_Mutex> publishGuard(&d_publishLock);
    bcemt_ReadLockGuard<bcemt_RWMutex> propertiesGuard(&d_rwLock);

    const baem_Category * const *category = categories;
    for ( ; category != categories + numCategories; ++category) {
        if (!(*category)->enabled()) {
            continue;
        }

        // Hold the elapsed time over which these metrics were collected.
        bdet_TimeInterval elapsedTime;

        int beginIndex = records->size();

        // Collect the metrics.
        baem_MetricsManager_PublicationHelper::collect(
                     records, &elapsedTime, this, *category, now, resetFlag);

        int size = records->size() - beginIndex;

        // If their are no collected records then this category can be ignored.
        if (0 < size) {
            samples.push_back(
                SampleDescription(beginIndex, size, elapsedTime));
        }
    }

    // Now the 'records' vector is full, we can add addresses into it to
    // 'sample'.
    bsl::vector<SampleDescription>::const_iterator it = samples.begin();
    for (; it != samples.end(); ++it) {
        sample->appendGroup(&(*records)[it->d_beginIndex],
                            it->d_size,
                            it->d_elapsedTime);
    }
}

void baem_MetricsManager::publish(const baem_Category *category,
                                  bool                 resetFlag)
{
    publish(&category, 1, resetFlag);
}

void baem_MetricsManager::publish(const baem_Category * const categories[],
                                  int                         numCategories,
                                  bool                        resetFlag)
{
    baem_MetricsManager_PublicationHelper::publish(this,
                                                   categories,
                                                   categories + numCategories,
                                                   resetFlag);
}

void baem_MetricsManager::publish(
                            const bsl::set<const baem_Category *>& categories,
                            bool                                   resetFlag)
{
    baem_MetricsManager_PublicationHelper::publish(this,
                                                   categories.begin(),
                                                   categories.end(),
                                                   resetFlag);
}

void baem_MetricsManager::publishAll(bool resetFlag)
{
    bsl::vector<const baem_Category *> allCategories;
    d_metricRegistry.getAllCategories(&allCategories);
    publish(allCategories.data(), allCategories.size(), resetFlag);
}

void baem_MetricsManager::publishAll(
                     const bsl::set<const baem_Category *>& excludedCategories,
                     bool                                   resetFlag)
{
    if (excludedCategories.empty()) {
        publishAll(resetFlag);
        return;                                                       // RETURN
    }

    bsl::vector<const baem_Category *> allCategories;
    bsl::vector<const baem_Category *> includedCategories;

    d_metricRegistry.getAllCategories(&allCategories);
    includedCategories.reserve(
                          allCategories.size() - excludedCategories.size());
    bsl::vector<const baem_Category *>::const_iterator it =
                                                        allCategories.begin();
    for (; it != allCategories.end(); ++it) {
        if (excludedCategories.end() == excludedCategories.find(*it)) {
            includedCategories.push_back(*it);
        }
    }

    if (includedCategories.size() > 0) {
        publish(includedCategories.data(),
                includedCategories.size(),
                resetFlag);
    }
}

baem_MetricsManager::CallbackHandle
baem_MetricsManager::registerCollectionCallback(
                                   const baem_Category              *category,
                                   const RecordsCollectionCallback&  callback)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwLock);
    return d_callbacks->registerCollectionCallback(category, callback);
}

int baem_MetricsManager::removeCollectionCallback(CallbackHandle handle)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwLock);
    return d_callbacks->removeCollectionCallback(handle);
}

int baem_MetricsManager::addGeneralPublisher(
                             const bcema_SharedPtr<baem_Publisher>& publisher)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwLock);
    return d_publishers->addGeneralPublisher(publisher);
}
int baem_MetricsManager::addSpecificPublisher(
                           const baem_Category                    *category,
                           const bcema_SharedPtr<baem_Publisher>&  publisher)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwLock);
    return d_publishers->addSpecificPublisher(category, publisher);
}

int baem_MetricsManager::removePublisher(const baem_Publisher *publisher)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwLock);
    return d_publishers->removePublisher(publisher);
}

int baem_MetricsManager::removePublisher(
                             const bcema_SharedPtr<baem_Publisher>& publisher)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwLock);
    return d_publishers->removePublisher(publisher.ptr());
}

// ACCESSORS
int baem_MetricsManager::findGeneralPublishers(
                               bsl::vector<baem_Publisher *> *publishers) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwLock);
    return d_publishers->findGeneralPublishers(publishers);
}

int baem_MetricsManager::findSpecificPublishers(
                                 bsl::vector<baem_Publisher *> *publishers,
                                 const baem_Category           *category) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwLock);
    return d_publishers->findSpecificPublishers(publishers, category);
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
