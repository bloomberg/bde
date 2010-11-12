// bcecs_failover.h                                                   -*-C++-*-
#ifndef INCLUDED_BCECS_FAILOVER
#define INCLUDED_BCECS_FAILOVER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a generic failover and basic load balancing facility.
//
//@CLASSES:
//   bcecs_ResourcePool: protocol class for resource pools
//   bcecs_FailoverResource: container for pooled resource
//   bcecs_FailoverSet: failover set
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@SEE_ALSO:
//   bcecs_roundrobin
//
//@DESCRIPTION: This component provides a complete failover and basic
// (round-robin) load balancing facility.  'bcecs_FailoverSet' owns different
// pools implementing the 'bcecs_ResourcePool' protocols and allows a caller
// to retrieve resources as a 'bcecs_FailoverResource'.
//
///Failover set
///-------------
// 'bcecs_FailoverSet' is a templated set where different pools can be
// registered.  The role of the set is to provide users with resources
// pooled by the registered pools.  The set does not own any resources but
// owns the registered pools.
// Resource lookup in a set uses the priority parameter specified at pool
// registration using 'registerPool'.  The set starts with priority 0 and
// try to locate a valid pool using 'isValid' registered at that priority.
// If it can't, it will proceed with further priorities.  Otherwise the
// set will try to request an object from one of the valid pools registered
// at that priority.  If all these pools are not able to provide any, the
// lookup will stop.
//
///'bcecs_FailoverResource'
///------------------------
// Resources are provided to users by set as 'bcecs_FailoverResource'
// instances.  Users then can access directly the pool object by calling
// 'resource'.  Resources are owned by the pools registered in the
// 'bcecs_FailoverSet'.  Users should be aware that caching the return
// value of 'resource' beyond the lifetime of the 'bcecs_FailoverResource'
// instance will result in undefined behavior.  Once the last copy of a
// 'bcecs_FailoverResource' instance is destroyed (or released using
// 'release'), the pooled resource will be returned to be the pool.  If a
// provided resource is not usable, the user is responsible to call to
// 'markAsInvalid' so appropriate action can be taken.
//
///The 'bcecs_ResourcePool' protocol
///---------------------------------
// 'bcecs_ResourcePool' defines a protocol that pools must implement to be
// registered with 'bcecs_FailoverSet'.  They are mainly responsible for
// two tasks: pool invalidation/re-validation handling and set notification.
//
///Pool invalidation
///-----------------
// If a user detect that a resource obtained through a set is invalid, it is
// its responsibility to mark the resource as invalid.  This information will
// be passed to the pool so appropriate (and implementation defined) action
// can be taken.  At that point, it is expected that the pool will invalidate
// itself which means that he will not provide any resources until it becomes
// valid again.
// Pools can invalidate themselves at any moment if they detect they cannot
// provide proper objects.  Reaching an artificial maximum of resources is not
// considered a good reason for self-invalidation since it may force the set to
// look into failover priorities instead of blocking.
// Pools are also responsible for their revalidation and can re-validate
// themselves at any time.  Therefore a call to 'invalidate' may not leave the
// pool invalid for a long time if the pool considers it is appropriate to
// re-validate itself right away.
// Pools have a lot of latitude in the handling of invalidation/re-validation
// so they can implement the most appropriate strategy for the resources
// they pool while still being completely integrated with 'bcecs_FailoverSet'.
//
///Resource availability notification
///----------------------------------
// Resource availability notification from a 'bcecs_ResourcePool' to the set
// where it is registered is handled by the callback installed by the set
// by calling 'setResourceReadyCb'.  The pool is responsible for calling this
// callback when resources becomes available again after a period where they
// were not.  For example, if a pool is getting revalidated and is ready to
// provide new resources, it should call the installed callback.  If the pool
// supports the notion of a maximum of provided resources, the callback should
// be called when if new resources are available.  An example of such
// implementation follows.
// It should be noted that a spurious call to the callback is supported but
// not calling the callback when a pool is ready to provide resources again
// is a major flaw in a 'bcecs_ResourcePool' implementation.
//
///Thread-safety
///-------------
// All implementations of the 'bcecs_ResourcePool' protocol must be
// thread-safe.  'bcecs_FailoverSet' is also thread-safe.  Note that for
// efficiency purposes,  it is encouraged to provide a lockless 'isValid' in
// 'bcecs_ResourcePool' implementations.
//
///Usage
///-----
//..
//  ConnectionPool *connectionPool1; // pools 'Connection' objects
//  ConnectionPool *connectionPool2;
//
//  (...) // allocate the pools, and set them up
//
//  bcecs_FailoverSet<Connection> set(&timer);
//  set.registerPool(
//          bdema_ManagedPtr<bcecs_ResourcePool<Connection> >(connectionPool1),
//          bdet_TimeInterval(5 * 60), // retry after 5 mins
//          0); // main pool
//
//  set.registerPool(
//          bdema_ManagedPtr<bcecs_ResourcePool<Connection> >(connectionPool2),
//          bdet_TimeInterval(5 * 60), // retry after 5 mins
//          1); // Failover pool
//
//  (...)
//
//  {
//      // We need a connection
//      bcecs_FailoverResource<Connection> connectionResource;
//      set.getResource(&connectionResource);
//
//      // use resource
//
//      if (connectionResource.resource()->connect() == 0) {
//          // exchange traffic
//              (...)
//      }
//      else {
//          // connection is unusable, the pool where it came from will
//          // be marked invalid and will be tentatively revalidated
//          // at the end of the timeout specified by the call
//          // to 'registerPool'.
//          connectionResource.markAsInvalid();
//          connectionResource.release(); // Release resource now.
//      }
//
//      // At that point, the resource if it was valid is returned to its pool.
//  }
//..
// The following class is a compliant implementation a the 'bcecs_ResourcePool'
// protocol supporting a maximum number of allocated resources.  It is mostly
// useful for test purposes.  Resources ('int's in this case) are not pooled
// but allocated for each resource request.  Returned resources are checked to
// verify they were actually allocated from this pool.  The pool is invalidated
// each time a resource it marked as invalid and is revalidated only manually.
//..
// class TestResourcePool : public bcecs_ResourcePool<int> {
//     // Test implementation of the 'bcecs_ResourcePool' protocol.
//
//     bool                      d_valid;              // valid state
//     int                       d_allocated;          // number of allocated
//                                                     // resources
//     int                       d_maxResources;       // max number of
//                                                     // resources that can be
//                                                     // allocated (0 means no
//                                                     // max)
//     bsl::vector<int*>         d_allocatedPointers;  // list of allocated
//                                                     // pointers
//     bdef_Function<void (*)()> d_resCb;              // resource ready cb
//     mutable bcemt_Mutex       d_lock;               // lock
//     bslma_Allocator          *d_allocator_p;        // (held)
//
//   private:
//     // not implemented
//     TestResourcePool(TestResourcePool&);
//     TestResourcePool& operator=(const TestResourcePool&);
//
//     // PRIVATE MANIPULATORS
//     static void reInsertInt(int *ret, TestResourcePool* pool);
//         // Return the specified 'ret' allocated by the specified 'pool'.
//
//   public:
//     // CREATORS
//     explicit TestResourcePool(bslma_Allocator *basicAllocator = 0);
//         // Create a new 'TestResourcePool'.
//
//     virtual ~TestResourcePool();
//         // Destroy this object.
//
//     // MANIPULATORS
//     virtual int tryGetObject(bdema_ManagedPtr<int> *ptr, Handle *);
//         // Transfer into the specified 'ptr' a resource from this pool.
//         // Return 0 on success or if 'ptr' is null but this pool contains a
//         // resource that could have been transferred, and a non-zero value
//         // otherwise.  Note that the resource is removed from this pool on
//         // successful transfer.
//
//     virtual void invalidate(Handle);
//         // Set the pool as invalid.  This pool will not deliver any more
//         // resources until it is manually revalidated.
//
//     void validate();
//         // Revalidate pool.
//
//     void setResourceReadyCb(const bdef_Function<void (*)()>& cb);
//         // Set the callback that will be called when resources will be
//         // available again to the specified 'cb'.
//
//     void setMaxResources(int max = 0);
//         // Set the maximum number of resource that can be allocated at the
//         // same time from this pool to the specified 'max' if max is
//         // non-zero positive value, and set this maximum to the infinity
//         // otherwise.
//
//     // ACCESSORS
//     virtual bool isValid() const;
//         // Return 'true' if this pool is valid and 'false' otherwise.
//
//     int numAllocated() const;
//         // Return the number of allocated resources from this pool.
//
//     int maxResources() const;
//         // Return the maximum number of resources that can be allocated from
//         // this pool at the same time if any, and 0 otherwise.
// };
//
// // PRIVATE MANIPULATORS
// void TestResourcePool::reInsertInt(int *ret, TestResourcePool *pool)
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&pool->d_lock);
//     bsl::vector<int*>::iterator it = bsl::lower_bound(
//                                           pool->d_allocatedPointers.begin(),
//                                           pool->d_allocatedPointers.end(),
//                                           ret);
//     ASSERT(pool->d_allocatedPointers.end() != it);
//     ASSERT(*it == ret);
//     pool->d_allocatedPointers.erase(it);
//     pool->d_allocator_p->deleteObjectRaw(ret);
//
//     if (pool->d_resCb && pool->d_valid && pool->d_maxResources != 0 &&
//         pool->d_allocated == pool->d_maxResources) {
//         pool->d_resCb();
//     }
//     --pool->d_allocated;
// }
//
// // CREATORS
// TestResourcePool::TestResourcePool(bslma_Allocator* basicAllocator)
// : d_valid(true)
// , d_allocated(0)
// , d_maxResources(0)
// , d_allocator_p(bslma_Default::allocator(basicAllocator))
// {
// }
//
// TestResourcePool::~TestResourcePool()
// {
//     assert(0 == d_allocated);
//     assert(d_allocatedPointers.empty());
// }
//
// // MANIPULATORS
// int TestResourcePool::tryGetObject(bdema_ManagedPtr<int> *ptr, Handle *)
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
//     if (d_valid && (d_maxResources == 0 || d_allocated < d_maxResources)) {
//         if (ptr) {
//             int *res = new (*d_allocator_p) int;
//             bsl::vector<int*>::iterator it = bsl::lower_bound(
//                                                 d_allocatedPointers.begin(),
//                                                 d_allocatedPointers.end(),
//                                                 res);
//             if (d_allocatedPointers.end() == it) {
//                 d_allocatedPointers.push_back(res);
//             }
//             else {
//                 d_allocatedPointers.insert(it, res);
//             }
//             ++d_allocated;
//             bdema_ManagedPtr<int> tmp(res, this,
//                                       &TestResourcePool::reInsertInt);
//             *ptr = tmp;
//         }
//         return 0;
//     }
//     return 1;
// }
//
// void TestResourcePool::invalidate(Handle)
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
//     d_valid = false;
// }
//
// void TestResourcePool::setResourceReadyCb(
//                                          const bdef_Function<void(*)()>& cb)
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
//     d_resCb = cb;
// }
//
// void TestResourcePool::setMaxResources(int max)
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
//     if (max < 0) {
//         max = 0;
//     }
//     if (d_resCb &&
//         d_allocated >= d_maxResources && (max == 0 || max > d_allocated)) {
//         d_resCb();
//     }
//     d_maxResources = max;
// }
//
// void TestResourcePool::validate()
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
//     if (d_resCb && !d_valid &&
//         (d_maxResources == 0 || d_allocated < d_maxResources)) {
//         d_resCb();
//     }
//     d_valid = true;
// }
//
// // ACCESSORS
// bool TestResourcePool::isValid() const
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
//     return d_valid;
// }
//
// int TestResourcePool::numAllocated() const
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
//     return d_allocated;
// }
//
// int TestResourcePool::maxResources() const
// {
//     bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
//     return d_maxResources;
// }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BCECS_ROUNDROBIN
#include <bcecs_roundrobin.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_TIMEDSEMAPHORE
#include <bcemt_timedsemaphore.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDEF_MEMFN
#include <bdef_memfn.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_RAWDELETERPROCTOR
#include <bslma_rawdeleterproctor.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                          // ========================
                          // class bcecs_ResourcePool
                          // ========================

template <typename TYPE>
class bcecs_ResourcePool {
    // Provide a templatized protocol (or pure interface) for thread-safe
    // resource pools.

  public:
    // TYPES
    typedef int Handle;

    // CREATORS
    virtual ~bcecs_ResourcePool();
        // Destroy this object.

    // MANIPULATORS
    virtual int tryGetObject(bdema_ManagedPtr<TYPE> *ptr,
                             Handle                 *handle = 0) = 0;
        // Transfer into the specified 'ptr' a resource from this pool.  Return
        // 0 on success or if 'ptr' is null but this pool contains a resource
        // that could have been transferred, and a non-zero value otherwise.
        // On success and if the specified 'handle' is not 0, optionally load a
        // pool-defined handle into 'handle' identifying the resource loaded in
        // 'ptr'.  Note that the resource is removed from this pool on
        // successful transfer.

    virtual void invalidate(Handle handle) = 0;
        // Report that a resource allocated from this pool and identified by
        // the pool as the specified 'handle' was not unsable and set this pool
        // as invalid.  The pool will not return any resources until it becomes
        // valid again.

    virtual void setResourceReadyCb(const bdef_Function<void (*)()>& cb) = 0;
        // Set the callback function that this pool will call when resources
        // becomes available after any interruption (e.g invalidation, maximum
        // number of resources reached etc.).  Note that this method is called
        // by 'bcecs_FailoverSet' during pool's registration.

    // ACCESSORS
    virtual bool isValid() const = 0;
        // Return 'true' if this pool is valid, and 'false' otherwise.
};

                        // ============================
                        // class bcecs_FailoverResource
                        // ============================

template <typename TYPE>
class bcecs_FailoverResource;

// FREE OPERATORS
template <typename TYPE>
bool operator==(const bcecs_FailoverResource<TYPE>& lhs,
                const bcecs_FailoverResource<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' entries have the same
    // value, and 'false' otherwise.  Two 'bcecs_FailoverResource' objects have
    // the same value if they contain the same resource.

template <typename TYPE>
bool operator!=(const bcecs_FailoverResource<TYPE>& lhs,
                const bcecs_FailoverResource<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' entries does not have the
    // same value, and 'false' otherwise.  Two 'bcecs_FailoverResource' objects
    // have the same value if they contain the same resource.

template <typename TYPE>
class bcecs_FailoverSet;

template <typename TYPE>
class bcecs_FailoverResource {
    // Provide a shell for resource obtained through 'bcecs_FailoverSet'.  More
    // generally, this class supports a complete set of *in-core* *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'bsl::ostream' printing.  (A precise operational
    // definition of when two instances have the same value can be found in the
    // description of 'operator==' for the class.)  This class is *exception*
    // *neutral* with no guarantee of rollback: if an exception is thrown
    // during the invocation of a method on a pre-existing instance, the object
    // is left in a valid state, but its value is undefined.  In no event is
    // memory leaked.  Finally, *aliasing* (e.g., using all or part of an
    // object as both source and destination) is supported in all cases.

    bcema_SharedPtr<TYPE>                      d_ptr;          // resource
    typename bcecs_ResourcePool<TYPE>::Handle  d_handle;       // resource
                                                               // handle
    bcecs_ResourcePool<TYPE>                  *d_pool_p;       // originating
                                                               // pool
                                                               // (held)
    bool                                       d_valid;        // valid flag
    int                                        d_poolPriority; // priority of
                                                               // this
                                                               // resource's
                                                               // pool

  private:
    // FRIENDS
    friend class bcecs_FailoverSet<TYPE>;
    friend bool operator==<>(const bcecs_FailoverResource<TYPE>& lhs,
                             const bcecs_FailoverResource<TYPE>& rhs);
    friend bool operator!=<>(const bcecs_FailoverResource<TYPE>& lhs,
                             const bcecs_FailoverResource<TYPE>& rhs);

    // PRIVATE CREATORS
    bcecs_FailoverResource(
                         bdema_ManagedPtr<TYPE>&           ptr,
                typename bcecs_ResourcePool<TYPE>::Handle  handle,
                         bcecs_ResourcePool<TYPE>         *pool,
                         int                               poolPriority,
                         bslma_Allocator                  *basicAllocator = 0);
        // Create a 'bcecs_FailoverResource' object containing the specified
        // 'ptr' obtained from the specified 'pool' registered in the set at
        // the specified 'poolPriority'.  Optionally specify
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // PRIVATE ACCESSORS
    bcecs_ResourcePool<TYPE> *pool() const;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcecs_FailoverResource,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bcecs_FailoverResource();
        // Create a 'bcecs_FailoverResource' object having the default value.

    bcecs_FailoverResource(
                       const bcecs_FailoverResource<TYPE>& original,
                       bslma_Allocator                    *basicAllocator = 0);
        // Create a Resource having the same value as the specified 'original'
        // object.

    ~bcecs_FailoverResource();
        // Destroy this object.  Note that if this object is still valid, the
        // resource contained in this object will be returned to its pool.

    // MANIPULATORS
    bcecs_FailoverResource& operator=(const bcecs_FailoverResource<TYPE>& rhs);
        // Assign to this resource the same value as the specified 'rhs'
        // object.

    TYPE *resource();
        // Return a pointer to the resource contained by this object.  Note
        // that the lifetime of the object referenced by the returned pointer
        // is not guaranteed beyond the lifetime of this object and that the
        // returned pointer might be 0.

    void markAsInvalid();
        // Mark the resource contained by this object as invalid.  The behavior
        // is undefined unless this object was previously valid.  Note that
        // this method does not affect the resource in this object.

    void release();
        // Return the resource contained in this object to its pool and set
        // this object as invalid.

    // ACCESSORS
    bool isValid() const;
        // Return 'true' if the resource contained by this object is valid.

    int poolPriority() const;
        // Return the priority of the pool from which this resource has been
        // obtained if this object holds a resource, otherwise return -1.
};

                           // =======================
                           // class bcecs_FailoverSet
                           // =======================

template <typename TYPE>
class bcecs_FailoverSet {
    // This class implements a thread safe templatized container which
    // furnishes objects from different pools.  Each resource obtained from
    // this object is eventually returned to it.  Each pool is associated to
    // an user-supplied priority which is used to determine which pool will
    // be used to obtain resources.  Resources are always retrieved from the
    // valid pools with the lower priority.  A round robin order is applied
    // between valid pools of the same priority.  This container is *exception*
    // *neutral*: if an exception is thrown during the invocation of a method
    // on a pre-existing instance, the value of the container is left in a
    // undefined but consistent state.  In no event is memory leaked.

    // PRIVATE TYPES
    typedef bcecs_ResourcePool<TYPE>     PoolType;

    bsl::map<int, bcecs_RoundRobin<PoolType *> *> d_poolIndex; // Pools index
    bsl::vector<bdema_ManagedPtrDeleter> d_poolDeleters; // Pools mem deleters
    bces_AtomicInt                       d_waiters;      // number of waiters
                                                         // on the semaphore
    bcemt_TimedSemaphore                 d_sem;          // synchronization sem
    bcemt_Mutex                          d_registerLock; // lock for
                                                         // 'registerPool'
    bslma_Allocator                     *d_allocator_p;  // allocator (held)

  private:
    // not implemented
    bcecs_FailoverSet(const bcecs_FailoverSet&);
    bcecs_FailoverSet& operator=(const bcecs_FailoverSet&);

    // PRIVATE MANIPULATORS
    void resourceReadyCb();
        // Posts the semaphore if there are any waiters.

    int tryGetResourceImpl(bcecs_FailoverResource<TYPE> *resource);
        // Load into the specified 'resource' a pooled resource and return 0 if
        // such resource is available.  Otherwise, return a non-zero value.

  public:
    BSLALG_DECLARE_NESTED_TRAITS(bcecs_FailoverSet,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit bcecs_FailoverSet(bslma_Allocator *basicAllocator = 0);
        // Create a new 'bcecs_FailoverSet'.  Optionally specified
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bcecs_FailoverSet();
        // Destroy this object.

    // MANIPULATORS
    void getResource(bcecs_FailoverResource<TYPE> *resource);
        // Suspend the execution of the current thread until one resource is
        // available then load into the specified 'resource' a pooled resource.
        // Note that the lifetime of the resource contained in the returned
        // object is not guaranteed beyond the lifetime of this set and that
        // upon destruction of all copies of this instance, the resource will
        // be returned to a pool.  The behavior is undefined unless all pools
        // are registered before a caller attempts to get resources.

    int getResourceTimed(bcecs_FailoverResource<TYPE> *resource,
                         const bdet_TimeInterval&      timeout);
        // Suspend the execution of the current thread until one resource is
        // available or until the specified timeout (expressed as the absolute
        // time from 00:00::00 UTC, January 1, 1970), and return 0 and load
        // into the specified 'resource' a pooled resource upon success and a
        // non-zero value if an error or a timeout occurred.  Note that the
        // lifetime of the resource contained in the returned object is not
        // guaranteed beyond the lifetime of this set and that upon destruction
        // of all copies of this instance, the resource will be returned to a
        // pool.  The behavior is undefined unless all pools are registered
        // before a caller attempts to get resources.

    int tryGetResource(bcecs_FailoverResource<TYPE> *resource);
        // Load into the specified 'resource' a pooled resource and return 0 if
        // such resource is available.  Otherwise, return a non-zero value.
        // Note that the lifetime of the resource contained in the returned
        // object is not guaranteed beyond the lifetime of this set and that
        // upon destruction of all copies of this instance, the resource will
        // be returned to a pool.  The behavior is undefined unless all pools
        // are registered before a caller attempts to get resources.

    void registerPool(bdema_ManagedPtr<bcecs_ResourcePool<TYPE> >& pool,
                      int                                          priority);
        // Register the specified 'pool' and use the specified 'priority' to
        // classify the specified 'pool'.  The valid pools with the lowest
        // priority are always used to return resources.  The behavior is
        // undefined unless all pools are registered before a caller attempts
        // to get resources and 'priority >= 0'
};

// ============================================================================
//                    TEMPLATIZED FUNCTION DEFINITIONS
// ============================================================================

                          // ========================
                          // class bcecs_ResourcePool
                          // ========================

template <typename TYPE>
bcecs_ResourcePool<TYPE>::~bcecs_ResourcePool()
{
}

                        // ============================
                        // class bcecs_FailoverResource
                        // ============================

// PRIVATE CREATORS
template <typename TYPE>
bcecs_FailoverResource<TYPE>::bcecs_FailoverResource(
                               bdema_ManagedPtr<TYPE>&          ptr,
                      typename bcecs_ResourcePool<TYPE>::Handle handle,
                               bcecs_ResourcePool<TYPE>        *pool,
                               int                              poolPriority,
                               bslma_Allocator                 *basicAllocator)
: d_ptr(ptr, basicAllocator)
, d_handle(handle)
, d_pool_p(pool)
, d_valid(true)
, d_poolPriority(poolPriority)
{
}

// PRIVATE ACCESSORS
template <typename TYPE>
bcecs_ResourcePool<TYPE> *bcecs_FailoverResource<TYPE>::pool() const
{
    return d_pool_p;
}

// CREATORS
template <typename TYPE>
bcecs_FailoverResource<TYPE>::bcecs_FailoverResource()
: d_ptr()
, d_pool_p(0)
, d_valid(false)
, d_poolPriority(-1)
{
}

template <typename TYPE>
bcecs_FailoverResource<TYPE>::bcecs_FailoverResource(
                                  const bcecs_FailoverResource&  original,
                                  bslma_Allocator               *)
: d_ptr(original.d_ptr)
, d_handle(original.d_handle)
, d_pool_p(original.d_pool_p)
, d_valid(original.d_valid)
, d_poolPriority(original.d_poolPriority)
{
}

template <typename TYPE>
bcecs_FailoverResource<TYPE>::~bcecs_FailoverResource()
{
}

// MANIPULATORS
template <typename TYPE>
bcecs_FailoverResource<TYPE>&
bcecs_FailoverResource<TYPE>::operator=(const bcecs_FailoverResource& rhs)
{
    d_ptr          = rhs.d_ptr;
    d_handle       = rhs.d_handle;
    d_pool_p       = rhs.d_pool_p;
    d_valid        = rhs.d_valid;
    d_poolPriority = rhs.d_poolPriority;
    return *this;
}

template <typename TYPE>
TYPE *bcecs_FailoverResource<TYPE>::resource()
{
    return d_ptr.ptr();
}

template <typename TYPE>
void bcecs_FailoverResource<TYPE>::release()
{
    d_ptr.clear();
    d_pool_p = 0;
    d_valid = false;
    d_poolPriority = -1;
}

template <typename TYPE>
void bcecs_FailoverResource<TYPE>::markAsInvalid()
{
    BSLS_ASSERT_SAFE(0 != d_pool_p);
    d_pool_p->invalidate(d_handle);
    d_valid = false;
}

// ACCESSORS
template <typename TYPE>
bool bcecs_FailoverResource<TYPE>::isValid() const
{
    return d_valid && d_ptr;
}

template <typename TYPE>
int bcecs_FailoverResource<TYPE>::poolPriority() const
{
    return d_poolPriority;
}

// FREE OPERATORS
template <typename TYPE>
bool operator==(const bcecs_FailoverResource<TYPE>& lhs,
                const bcecs_FailoverResource<TYPE>& rhs)
{
    const bool isEqual = (lhs.d_ptr == rhs.d_ptr &&
                          lhs.d_handle == rhs.d_handle);
    BSLS_ASSERT_SAFE(!isEqual || lhs.d_pool_p == rhs.d_pool_p);
    BSLS_ASSERT_SAFE(!isEqual || lhs.d_poolPriority == rhs.d_poolPriority);
    return isEqual;
}

template <typename TYPE>
bool operator!=(const bcecs_FailoverResource<TYPE>& lhs,
                const bcecs_FailoverResource<TYPE>& rhs)
{
    return !(lhs == rhs);
}

                           // =======================
                           // class bcecs_FailoverSet
                           // =======================

// PRIVATE MANIPULATORS
template <typename TYPE>
void bcecs_FailoverSet<TYPE>::resourceReadyCb()
{
    if (d_waiters) {
        d_sem.post();
    }
}

template <typename TYPE>
int bcecs_FailoverSet<TYPE>::tryGetResourceImpl(
                                        bcecs_FailoverResource<TYPE> *resource)
{
    bdema_ManagedPtr<TYPE> mptr;
    typename bcecs_ResourcePool<TYPE>::Handle handle;

    for (typename bsl::map<int, bcecs_RoundRobin<PoolType *> *>::iterator
            it = d_poolIndex.begin();
            it != d_poolIndex.end(); ++it) {
        bcecs_RoundRobin<PoolType *> *rr = it->second;

        bool foundValid = false;
        for (int j = 0; j < rr->numObjects(); ++j) {
            PoolType *pool;
#if defined(BDE_BUILD_TARGET_SAFE)
            const int ret =
#endif
                            rr->next(&pool);
            BSLS_ASSERT_SAFE(1 == ret && pool);

            if (pool->isValid()) {
                if (0 == pool->tryGetObject(&mptr, &handle)) {
                    *resource = bcecs_FailoverResource<TYPE>(mptr, handle,
                                                             pool,
                                                             it->first,
                                                             d_allocator_p);
                    return 0;
                }

                // We support the fact that when trying to get new resources,
                // the pool might realize it cannot provide working resources
                // and then invalidates itself.

                if (pool->isValid()) {
                    foundValid = true;
                }
            }
        }
        if (foundValid) {
            // At least one valid pool was found at this priority but was
            // unable to provide any resources.  The policy is to stop here
            // even if pools of other priorities are valid.

            break;
        }
    }
    return 1;
}

// CREATORS
template <typename TYPE>
bcecs_FailoverSet<TYPE>::bcecs_FailoverSet(bslma_Allocator *basicAllocator)
: d_poolIndex(bsl::less<int>(), basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <typename TYPE>
bcecs_FailoverSet<TYPE>::~bcecs_FailoverSet()
{
    // Delete the resource pools.

    for (bsl::vector<bdema_ManagedPtrDeleter>::iterator it =
                                                        d_poolDeleters.begin();
         it != d_poolDeleters.end(); ++it) {
        it->deleteManagedObject();
    }

    // Delete the bcecs_RoundRobin objects.

    for (typename bsl::map<int, bcecs_RoundRobin<PoolType *> *>::iterator
            it = d_poolIndex.begin();
            it != d_poolIndex.end(); ++it) {
        d_allocator_p->deleteObjectRaw(it->second);
    }
}

// MANIPULATORS
template <typename TYPE>
int bcecs_FailoverSet<TYPE>::tryGetResource(
                                        bcecs_FailoverResource<TYPE> *resource)
{
    if (0 == tryGetResourceImpl(resource)) {
        if (d_waiters > 0 && 0 == resource->pool()->tryGetObject(0, 0)) {
            // Post if the pool that we obtained the resource from has more
            // resources available.

            d_sem.post();
        }
        return 0;
    }
    return 1;
}

template <typename TYPE>
void bcecs_FailoverSet<TYPE>::getResource(
                                        bcecs_FailoverResource<TYPE> *resource)
{
    if (0 != tryGetResourceImpl(resource)) {
        ++d_waiters;
        while (0 != tryGetResourceImpl(resource)) {
            d_sem.wait();
        }
        --d_waiters;
    }

    if (d_waiters > 0 && 0 == resource->pool()->tryGetObject(0, 0)) {
        // Post if the pool that we obtained the resource from has more
        // resources available.

        d_sem.post();
    }
}

template <typename TYPE>
int bcecs_FailoverSet<TYPE>::getResourceTimed(
                                        bcecs_FailoverResource<TYPE> *resource,
                                        const bdet_TimeInterval&      timeout)
{
    if (0 != tryGetResourceImpl(resource)) {
        ++d_waiters;
        while (0 != tryGetResourceImpl(resource)) {
            if (0 != d_sem.timedWait(timeout)) {
                --d_waiters;
                return 1;
            }
        }
        --d_waiters;
    }

    if (d_waiters > 0 && 0 == resource->pool()->tryGetObject(0, 0)) {
        // Post if the pool that we obtained the resource from has more
        // resources available.

        d_sem.post();
    }
    return 0;
}

template <typename TYPE>
void bcecs_FailoverSet<TYPE>::registerPool(
                         bdema_ManagedPtr<bcecs_ResourcePool<TYPE> >& pool,
                         int                                          priority)
{
    BSLS_ASSERT_SAFE(pool);
    BSLS_ASSERT_SAFE(priority >= 0);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_registerLock);

    // Reserve first for easy exception neutrality.

    d_poolDeleters.reserve(d_poolDeleters.size() + 1);
    bsl::pair<PoolType *,bdema_ManagedPtrDeleter> ptrInfo = pool.release();
    d_poolDeleters.push_back(ptrInfo.second);

    typename bsl::map<int, bcecs_RoundRobin<PoolType *> *>::iterator
        it = d_poolIndex.find(priority);
    if (d_poolIndex.end() == it) {
        bcecs_RoundRobin<PoolType *> *rr = new (*d_allocator_p)
            bcecs_RoundRobin<PoolType *>(d_allocator_p);
        bslma_RawDeleterProctor<bcecs_RoundRobin<PoolType *>, bslma_Allocator>
            deleterProctor(rr, d_allocator_p);
        d_poolIndex[priority] = rr;
        deleterProctor.release();
    }

    // Add to the 'bcecs_RoundRobin' object.

    d_poolIndex[priority]->add(ptrInfo.first);

    // Register our callback with the pool.

    ptrInfo.first->setResourceReadyCb(
       bdef_MemFnUtil::memFn(&bcecs_FailoverSet<TYPE>::resourceReadyCb, this));
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
