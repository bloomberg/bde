// bcec_sharedobjectpool.h   -*-C++-*-
#ifndef INCLUDED_BCEC_SHAREDOBJECTPOOL
#define INCLUDED_BCEC_SHAREDOBJECTPOOL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE:  Provide a thread-safe pool of shared objects
//
//@CLASSES:
//           bcec_SharedObjectPool: thread-enabled container of shared objects
//
//@SEE_ALSO:  bcema_SharedPtr
//
//@AUTHOR: Vlad Kliatchko (vkliatch), David Schumann (dschumann1)
//
//@DESCRIPTION:  This component provides a generic thread-safe pool of shared
// objects using the acquire-release idiom.  The functionality provided is
// identical to 'bcec_ObjectPool', except that 'getObject' returns
// efficiently-constructed 'bcema_SharedPtr' objects instead of raw pointers.
// For client code that needs to provide shared access to objects in the pool,
// this functionality saves an additional allocation for the shared pointer
// itself.  Since the shared pointer and the object are contiguous in memory,
// this component also tends to improve performance by reducing "cache
// misses."
//
///Object construction and destruction
///-----------------------------------
// The object pool owns the memory required to store the pooled objects and
// the shared-pointer representations, and manages the construction,
// resetting, and destruction of objects.  The user may supply functors to
// create objects and to reset them to a valid state for their return to the
// pool; alternatively, this component supplies reasonable defaults.  Upon
// destruction the object pool deallocates all memory associated with the
// objects in the pool.  The behavior is undefined if there are any
// outstanding shared pointer references to the objects in the pool when it
// is destroyed.
//
///Creator and Resetter Template Contract
///--------------------------------------
// 'bcec_SharedObjectPool' is templated on two types 'CREATOR' and 'RESETTER'
// in addition to the underlying object 'TYPE'.  Objects of these types may be
// provided at construction (or defaults may be used).  The creator will be
// invoked as: 'void(*)(void*, bslma_Allocator*)'.  The resetter will be
// invoked as: 'void(*)(TYPE*);.  The creator functor will be called to
// construct a new object of the parameterized 'TYPE' when the pool must be
// expanded (and thus it will typically invoke placement new and pass its
// allocator argument to the constructor of 'TYPE').  The resetter functor
// will be called before each object is returned to the pool, and is required
// to put the object into a state such that it is ready to be reused.  The
// defaults for these types are as follows:
//..
//    CREATOR = bcec_ObjectPoolFunctors::DefaultCreator
//    RESETTER = bcec_ObjectPoolFunctors::Nil<TYPE>
//..
// 'bcec_ObjectPoolFunctors::Nil' is a no-op; it is only suitable if the
// objects stored in the pool are *always* in a valid state to be reused.
// Otherwise - that is, if anything must be done to render the objects ready
// for reuse - another kind of 'RESETTER' should be provided (so long as
// that type supplies 'void(*)(TYPE*)').  In 'bcec_ObjectPoolFunctors', the
// classes 'Clear', 'RemoveAll', and 'Reset' are all acceptable types for
// 'RESETTER'.  Since these "functor" types are fully inlined, it is
// generally most efficient to define 'reset()' (or 'clear()' or
// 'removeAll()') in the underlying 'TYPE' and allow the functor to call that
// method.  The 'CREATOR' functor defaults to an object that invokes the
// default constructor with placement new, passing the allocator argument if
// the type traits of the object indicate it uses an allocator (see
// 'bdealg_typetraits').  If a custom creator functor or a custom 'CREATOR'
// type is specified, it is the user's responsibility to ensure that it
// correctly passes its allocator argument through to the constructor of
// 'TYPE' if 'TYPE' uses allocator.
//
///Exception safety
///----------------
// There are two potential sources of exceptions in this component: memory
// allocation and object construction.  The object pool is exception-neutral
// with full guarantee of rollback for the following methods:  if an exception
// is thrown in 'getObject', 'reserveCapacity', or 'increaseCapacity', then
// the pool is in a valid unmodified state (i.e., identical to prior the call
// to 'getObject').  No other method of 'bcec_SharedObjectPool' can throw.
//
///Pool replenishment policy
///-------------------------
// The 'growBy' parameter can be specified in the pool's constructor
// to instruct the pool how it is to increase its capacity each time
// the pool is depleted.  If 'growBy' is positive, the pool always
// replenishes itself with enough objects so that it can satisfy at
// least 'growBy' object requests before the next replenishment.
// If 'growBy' is negative, the pool will increase its capacity
// geometrically until it exceeds the internal maximum (which itself is
// implementation-defined), and after that it will be replenished with
// constant number of objects.  If 'growBy' is not specified, an
// implementation-defined default will be chosen.  The behavior is undefined
// if growBy is 0.
//
///Usage Example
///-------------
// This component is intended to improve the efficiency of code which
// provides shared pointers to pooled objects.  As an example, consider
// a class which maintains a pool of 'bcema_Blob' objects and provides shared
// pointers to them.  Using 'bcec_ObjectPool', the class might be implemented
// like this:
//..
//  class SlowBlobPool {
//     bcec_ObjectPool<bcema_Blob>   d_blobPool;     // supply blobs
//     bcema_PoolAllocator           d_spAllocator;  // allocate shared pointer
//     bcema_PooledBlobBufferFactory d_blobFactory;  // supply blob buffers
//
//     enum {BUFFER_SIZE=65536};
//
//     static void createBlob(void* address, bcema_BlobBufferFactory *factory,
//                     bslma_Allocator *allocator) {
//         new (address) bcema_Blob(factory, allocator);
//     }
//
//     static void resetAndReturnBlob(bcema_Blob *blob,
//                                    bcec_ObjectPool<bcema_Blob> *pool) {
//         blob->removeAll();
//         pool->releaseObject(blob);
//     }
//
//   public:
//
//     SlowBlobPool(bslma_Allocator *basicAllocator = 0)
//       : d_spAllocator(basicAllocator)
//       , d_blobFactory(BUFFER_SIZE, basicAllocator)
//       , d_blobPool(bdef_BindUtil::bind(
//                                  &SlowBlobPool::createBlob,
//                                  bdef_PlaceHolders::_1,
//                                  &d_blobFactory,
//                                  basicAllocator), -1,
//                    basicAllocator)
//    {}
//
//    void getBlob(bcema_SharedPtr<bcema_Blob> *blob_sp) {
//        blob_sp->load(d_blobPool.getObject(),
//                      bdef_BindUtil::bind(
//                               &SlowBlobPool::resetAndReturnBlob,
//                               bdef_PlaceHolders::_1,
//                               &d_blobPool),
//                      &d_spAllocator);
//    }
// };
//..
// Note that 'SlowBlobPool' must allocate the shared pointer itself from its
// 'd_spAllocator' in addition to allocating the blob from its pool.  Moreover,
// note that since the same function will handle resetting the object and
// returning it to the pool, we must define a special function for that
// purpose and bind its arguments.
//
// We can solve both of these issues by using 'bcec_SharedObjectPool' instead:
//..
//  class FastBlobPool {
//     typedef bcec_SharedObjectPool<
//                 bcema_Blob,
//                 bcec_ObjectPoolFunctors::DefaultCreator,
//                 bcec_ObjectPoolFunctors::RemoveAll<bcema_Blob> > BlobPool;
//
//     BlobPool                      d_blobPool;     // supply blobs
//     bcema_PooledBlobBufferFactory d_blobFactory;  // supply blob buffers
//
//     enum {BUFFER_SIZE=65536};
//
//     static void createBlob(void* address, bcema_BlobBufferFactory *factory,
//                     bslma_Allocator *allocator) {
//         new (address) bcema_Blob(factory, allocator);
//     }
//
//   public:
//
//     FastBlobPool(bslma_Allocator *basicAllocator = 0)
//       : d_blobFactory(BUFFER_SIZE, basicAllocator)
//       , d_blobPool(bdef_BindUtil::bind(
//                                  &FastBlobPool::createBlob,
//                                  bdef_PlaceHolders::_1,
//                                  &d_blobFactory,
//                                  bdef_PlaceHolders::_2),
//                    -1, basicAllocator)
//    {}
//
//    void getBlob(bcema_SharedPtr<bcema_Blob> *blob_sp) {
//        *blob_sp = d_blobPool.getObject();
//    }
// };
//..
// Now the shared pointer and the object are allocated as one unit from the
// same allocator.  In addition, the resetter method is a fully-inlined class
// that is only responsible for resetting the object, improving efficiency
// and simplifying the design.  We can verify that use of
// 'bcec_SharedObjectPool' reduces the number of allocation requests:
//..
//
// bcema_TestAllocator slowAllocator, fastAllocator;
// {
//   SlowBlobPool slowPool(&slowAllocator);
//   FastBlobPool fastPool(&fastAllocator);
//
//   bcema_SharedPtr<bcema_Blob> blob_sp;
//
//   fastPool.getBlob(&blob_sp);
//   slowPool.getBlob(&blob_sp); // throw away the first blob
// }
//
// ASSERT(2 == slowAllocator.numAllocation());
// ASSERT(1 == fastAllocator.numAllocation());
// ASSERT(0 == slowAllocator.numBytesInUse());
// ASSERT(0 == fastAllocator.numBytesInUse());
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTPOOL
#include <bcec_objectpool.h>
#endif

#ifndef INCLUDED_BDEF_BIND
#include <bdef_bind.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

                      // ===============================
                      // class bcec_SharedObjectPool_Rep
                      // ===============================

template <class TYPE, class RESETTER>
class bcec_SharedObjectPool_Rep: public bcema_SharedPtrRep {

    typedef bcec_SharedObjectPool_Rep<TYPE, RESETTER> MyType;
    typedef bcec_ObjectPool<MyType,
                            bcec_ObjectPoolFunctors::DefaultCreator,
                            bcec_ObjectPoolFunctors::Reset<MyType> >
                                                      PoolType;

    // DATA
    bslalg_ConstructorProxy<RESETTER> d_objectResetter;

    PoolType                         *d_pool_p; // object pool (held)
    bsls_ObjectBuffer<TYPE>           d_instance; // area for embedded instance

    // NOT IMPLEMENTED
    bcec_SharedObjectPool_Rep(const bcec_SharedObjectPool_Rep& original);
    bcec_SharedObjectPool_Rep& operator=(const bcec_SharedObjectPool_Rep& rhs);

  public:
    // CREATORS
    template <class CREATOR>
    bcec_SharedObjectPool_Rep(CREATOR*                         objectCreator,
                              const bslalg_ConstructorProxy<RESETTER>&
                                                               objectResetter,
                              PoolType                        *pool,
                              bslma_Allocator                 *basicAllocator);
       // Construct a new rep object that, upon release, will invoke
       // the specified 'objectResetter' and return itself to the specified
       // 'pool'; then invoke 'objectCreator' to construct an object of
       // 'TYPE' embedded within the new rep object.  Use the specified
       // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
       // the currently installed default allocator is used.

    ~bcec_SharedObjectPool_Rep();
        // Destroy this representation object and the embedded instance of
        // 'TYPE'.

    // MANIPULATORS
    virtual void release();
        // Release this representation object.  This method is invoked when the
        // number of references reaches zero (i.e., when a call to
        // 'decrementRefs' returns zero) to dispose of this representation
        // object.  This virtual override will return the object, and
        // this representation, to the associated pool.

    void reset();
        // Invoke the object resetter specified at construction on the
        // associated object.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return (untyped) address of the object managed by this
        // representation.  This virtual override effectively returns
        // "(void*)ptr()".

    TYPE *ptr();
        // Return a pointer to the in-place object.
};
                           // ===========================
                           // class bcec_SharedObjectPool
                           // ===========================

template <class TYPE,
          class CREATOR=bcec_ObjectPoolFunctors::DefaultCreator,
          class RESETTER=bcec_ObjectPoolFunctors::Nil<TYPE> >
class bcec_SharedObjectPool {

    typedef bcec_SharedObjectPool<TYPE, CREATOR, RESETTER> MyType;
    typedef bcec_SharedObjectPool_Rep<TYPE, RESETTER>      RepType;
    typedef bcec_ObjectPool<RepType,
                            bcec_ObjectPoolFunctors::DefaultCreator,
                            bcec_ObjectPoolFunctors::Reset<RepType> >
                                                           PoolType;

    bcec_ObjectPool_CreatorProxy<CREATOR, TYPE>
                                d_objectCreator; // functor for object creation

    bslalg_ConstructorProxy<RESETTER>
                                d_objectResetter;  // functor to reset object

    PoolType                    d_pool;           // object pool (owned)

  private:
    // NOT IMPLEMENTED
    bcec_SharedObjectPool(const bcec_SharedObjectPool& original);
    bcec_SharedObjectPool& operator=(const bcec_SharedObjectPool& rhs);

    void constructRepObject(void *mem, bslma_Allocator* alloc);
        // Initializes a newly constructed bcec_SharedObjectPool_Rep object

  public:
    // TYPES
    typedef CREATOR    CreatorType;
    typedef RESETTER   ResetterType;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcec_SharedObjectPool,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bcec_SharedObjectPool(
            int                                    growBy = -1,
            bslma_Allocator                       *basicAllocator = 0);

    explicit
    bcec_SharedObjectPool(
            const CREATOR&                         objectCreator,
            int                                    growBy = -1,
            bslma_Allocator                       *basicAllocator = 0);

    explicit
    bcec_SharedObjectPool(
            const CREATOR&                         objectCreator,
            bslma_Allocator                       *basicAllocator = 0);

    bcec_SharedObjectPool(
            const CREATOR&                         objectCreator,
            const RESETTER&                        objectResetter,
            int                                    growBy = -1,
            bslma_Allocator                       *basicAllocator = 0);

        // Create an object pool that dispenses shared pointers to TYPE.
        // When the pool is depleted, it increases its capacity according to
        // the optionally specified 'growBy' value.  If 'growBy' is
        // positive, the pool always increases by at least 'growBy'.  If
        // 'growBy' is negative, the amount of increase begins at
        // '-growBy' and then grows geometrically up to an
        // implementation-defined maximum.  The optionally specified
        // 'objectCreator' is called whenever objects must be constructed.
        // If 'objectCreator' is not specified and the parameterized 'CREATOR'
        // is the default type (that is,
        // 'bcec_ObjectPoolFunctors::DefaultCreator'), a function
        // that calls the default constructor of 'TYPE' with placement new,
        // passing this pool's allocator if TYPE uses allocator, is used.  If
        // the parameterized 'CREATOR' is some other type, and 'objectCreator'
        // is not specified, the default value of the 'CREATOR' type is used.
        // The optionally specified 'objectResetter' is invoked with a pointer
        // to an object of 'TYPE' when the object is returned to the pool.  It
        // must reset the object into a valid state for reuse.  If
        // 'objectResetter' is not specified, a default RESETTER object is
        // used.  Optionally specify a basic allocator to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // is used.  The behavior is undefined if 'growBy' is 0.

    ~bcec_SharedObjectPool();
        // Destroy this object pool.  All objects created by this pool
        // are destroyed (even if some of them are still in use) and
        // memory is reclaimed.

    // MANIPULATORS
    bcema_SharedPtr<TYPE> getObject();
        // Return a pointer to an object from this object pool.  When the
        // last shared pointer to the object is destroyed, the object
        // will be reset as specified at construction and then returned to the
       //  pool.  If this pool is empty, it is replenished according to the
       //  strategy specified at construction.

    void increaseCapacity(int growBy);
        // Create the specified 'growBy' objects and add them to this
        // object pool.  The behavior is undefined unless 0 <= growBy.

    void reserveCapacity(int growBy);
        // Create enough objects to satisfy requests for at least the
        // specified 'growBy' objects before the next replenishment.
        // The behavior is undefined unless 0 <= growBy.  Note that this
        // method is different from 'increaseCapacity' in that the number
        // of created objects may be less than 'growBy'.

    // ACCESSORS
    int numAvailableObjects() const;
        // Return a *snapshot* of the number of objects available in this pool.

    int numObjects() const;
        // Return the (instantaneous) number of objects managed by
        // this pool.  This includes both the objects available in the pool
        // and the objects that were allocated from the pool and not yet
        // released.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------------
                        // class bcec_SharedObjectPool_Rep
                        // --------------------------------

// CREATORS
template <class TYPE, class RESETTER>
template <class CREATOR>
inline
bcec_SharedObjectPool_Rep<TYPE, RESETTER>::bcec_SharedObjectPool_Rep(
    CREATOR*                                 objectCreator,
    const bslalg_ConstructorProxy<RESETTER>& objectResetter,
    PoolType                                *pool,
    bslma_Allocator                         *basicAllocator)
: d_objectResetter(objectResetter,basicAllocator)
, d_pool_p(pool)
{
    (*objectCreator)(d_instance.buffer(), basicAllocator);
}

template <class TYPE, class RESETTER>
inline
bcec_SharedObjectPool_Rep<TYPE, RESETTER>::~bcec_SharedObjectPool_Rep()
{
    d_instance.object().~TYPE();
}

// MANIPULATORS
template <class TYPE, class RESETTER>
inline
void bcec_SharedObjectPool_Rep<TYPE, RESETTER>::reset()
{
    d_objectResetter.object()(&d_instance.object());
}

template <class TYPE, class RESETTER>
inline
void bcec_SharedObjectPool_Rep<TYPE, RESETTER>::release()
{
    d_pool_p->releaseObject(this);
}

// ACCESSORS
template <class TYPE, class RESETTER>
inline
void *bcec_SharedObjectPool_Rep<TYPE, RESETTER>::originalPtr() const
{
    return const_cast<void*>(static_cast<const void*>(d_instance.buffer()));
}

template <class TYPE, class RESETTER>
inline
TYPE *bcec_SharedObjectPool_Rep<TYPE, RESETTER>::ptr()
{
    return &d_instance.object();
}

                               // ---------------------
                               // bcec_SharedObjectPool
                               // ---------------------
// PRIVATE
template <class TYPE, class CREATOR, class RESETTER>
inline
void bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::constructRepObject(
    void *mem, bslma_Allocator* alloc)
{
    RepType *r = new (mem) RepType(&d_objectCreator.object(),
                                   d_objectResetter,
                                   &d_pool,
                                   alloc);
    r->decrementRefs();
}

// CREATORS
#if defined(BSLS_PLATFORM__CMP_MSVC)
// Visual C++ complains about any use of the 'this' pointer in a member
// initializer of a constructor.  The use cases below seem perfectly safe and
// correct, but there is no way to eliminate this warning from a prominent
// header file other than disabling it via a pragma.
#pragma warning(push)
#pragma warning(disable : 4355) // used 'this' in member initializer
#endif

template <class TYPE, class CREATOR, class RESETTER>
inline
bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::bcec_SharedObjectPool(
                                       const CREATOR&          objectCreator,
                                       const RESETTER&         objectResetter,
                                       int                     growBy,
                                       bslma_Allocator        *basicAllocator)
: d_objectCreator(objectCreator,basicAllocator)
, d_objectResetter(objectResetter,basicAllocator)
, d_pool(bdef_BindUtil::bind(&MyType::constructRepObject, this,
                             bdef_PlaceHolders::_1,
                             bdef_PlaceHolders::_2),
         growBy, basicAllocator)
{
}

template <class TYPE, class CREATOR, class RESETTER>
inline
bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::bcec_SharedObjectPool(
                                       int                     growBy,
                                       bslma_Allocator        *basicAllocator)
: d_objectCreator(basicAllocator)
, d_objectResetter(basicAllocator)
, d_pool(bdef_BindUtil::bind(&MyType::constructRepObject, this,
                             bdef_PlaceHolders::_1,
                             bdef_PlaceHolders::_2),
         growBy, basicAllocator)
{
}

template <class TYPE, class CREATOR, class RESETTER>
inline
bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::bcec_SharedObjectPool(
                                       const CREATOR&          objectCreator,
                                       int                     growBy,
                                       bslma_Allocator        *basicAllocator)
: d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(basicAllocator)
, d_pool(bdef_BindUtil::bind(&MyType::constructRepObject, this,
                             bdef_PlaceHolders::_1,
                             bdef_PlaceHolders::_2),
         growBy, basicAllocator)
{
}

template <class TYPE, class CREATOR, class RESETTER>
inline
bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::bcec_SharedObjectPool(
                                       const CREATOR&          objectCreator,
                                       bslma_Allocator        *basicAllocator)
: d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(basicAllocator)
, d_pool(bdef_BindUtil::bind(&MyType::constructRepObject, this,
                             bdef_PlaceHolders::_1,
                             bdef_PlaceHolders::_2),
         -1, basicAllocator)
{
}

#if defined(BSLS_PLATFORM__CMP_MSVC)
// Restore warnings so as not to affect their state in other files.
#pragma warning(pop)
#endif

template <class TYPE, class CREATOR, class RESETTER>
inline
bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::~bcec_SharedObjectPool()
{
}

// MANIPULATORS
template <class TYPE, class CREATOR, class RESETTER>
inline
bcema_SharedPtr<TYPE>
bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::getObject()
{
    RepType *rep = d_pool.getObject();
    bcema_SharedPtrRep *genericRep = rep;
    genericRep->incrementRefs();

    return bcema_SharedPtr<TYPE>(rep->ptr(), genericRep);
}

template <class TYPE, class CREATOR, class RESETTER>
inline
void
bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::increaseCapacity(int growBy)
{
    d_pool.increaseCapacity(growBy);
}

template <class TYPE, class CREATOR, class RESETTER>
inline
void
bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::reserveCapacity(int growBy)
{
    d_pool.reserveCapacity(growBy);
}

// ACCESSORS
template <class TYPE, class CREATOR, class RESETTER>
inline
int bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::numAvailableObjects() const
{
    return d_pool.numAvailableObjects();
}

template <class TYPE, class CREATOR, class RESETTER>
inline
int bcec_SharedObjectPool<TYPE, CREATOR, RESETTER>::numObjects() const
{
    return d_pool.numObjects();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
