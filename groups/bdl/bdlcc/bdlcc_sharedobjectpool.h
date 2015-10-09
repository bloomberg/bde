// bdlcc_sharedobjectpool.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_SHAREDOBJECTPOOL
#define INCLUDED_BDLCC_SHAREDOBJECTPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe pool of shared objects.
//
//@CLASSES:
//  bdlcc::SharedObjectPool: thread-enabled container of shared objects
//
//@SEE_ALSO: bcema_sharedptr
//
//@DESCRIPTION: This component provides a generic thread-safe pool of shared
// objects, 'bdlcc::SharedObjectPool', using the acquire-release idiom.  The
// functionality provided is identical to 'bdlcc::ObjectPool', except that
// 'getObject' returns efficiently-constructed 'bsl::shared_ptr' objects
// instead of raw pointers.  For client code that needs to provide shared
// access to objects in the pool, this functionality saves an additional
// allocation for the shared pointer itself.  Since the shared pointer and the
// object are contiguous in memory, this component also tends to improve
// performance by reducing "cache misses."
//
///Object Construction and Destruction
///-----------------------------------
// The object pool owns the memory required to store the pooled objects and the
// shared-pointer representations, and manages the construction, resetting, and
// destruction of objects.  The user may supply functors to create objects and
// to reset them to a valid state for their return to the pool; alternatively,
// this component supplies reasonable defaults.  Upon destruction the object
// pool deallocates all memory associated with the objects in the pool.  The
// behavior is undefined if there are any outstanding shared pointer references
// to the objects in the pool when it is destroyed.
//
///Creator and Resetter Template Contract
///--------------------------------------
// 'bdlcc::SharedObjectPool' is templated on two types 'CREATOR' and 'RESETTER'
// in addition to the underlying object 'TYPE'.  Objects of these types may be
// provided at construction (or defaults may be used).  The creator will be
// invoked as: 'void(*)(void*, bslma::Allocator*)'.  The resetter will be
// invoked as: 'void(*)(TYPE*)'.  The creator functor will be called to
// construct a new object of the parameterized 'TYPE' when the pool must be
// expanded (and thus it will typically invoke placement new and pass its
// allocator argument to the constructor of 'TYPE').  The resetter functor will
// be called before each object is returned to the pool, and is required to put
// the object into a state such that it is ready to be reused.  The defaults
// for these types are as follows:
//..
//    CREATOR = bdlcc::ObjectPoolFunctors::DefaultCreator
//    RESETTER = bdlcc::ObjectPoolFunctors::Nil<TYPE>
//..
// 'bdlcc::ObjectPoolFunctors::Nil' is a no-op; it is only suitable if the
// objects stored in the pool are *always* in a valid state to be reused.
// Otherwise - that is, if anything must be done to render the objects ready
// for reuse - another kind of 'RESETTER' should be provided (so long as that
// type supplies 'void(*)(TYPE*)').  In 'bdlcc::ObjectPoolFunctors', the
// classes 'Clear', 'RemoveAll', and 'Reset' are all acceptable types for
// 'RESETTER'.  Since these "functor" types are fully inline, it is generally
// most efficient to define 'reset()' (or 'clear()' or 'removeAll()') in the
// underlying 'TYPE' and allow the functor to call that method.  The 'CREATOR'
// functor defaults to an object that invokes the default constructor with
// placement new, passing the allocator argument if the type traits of the
// object indicate it uses an allocator (see 'bslma_usesbslmaallocator').  If a
// custom creator functor or a custom 'CREATOR' type is specified, it is the
// user's responsibility to ensure that it correctly passes its allocator
// argument through to the constructor of 'TYPE' if 'TYPE' uses allocator.
//
///Exception Safety
///----------------
// There are two potential sources of exceptions in this component: memory
// allocation and object construction.  The object pool is exception-neutral
// with full guarantee of rollback for the following methods: if an exception
// is thrown in 'getObject', 'reserveCapacity', or 'increaseCapacity', then the
// pool is in a valid unmodified state (i.e., identical to prior the call to
// 'getObject').  No other method of 'bdlcc::SharedObjectPool' can throw.
//
///Pool Replenishment Policy
///-------------------------
// The 'growBy' parameter can be specified in the pool's constructor to
// instruct the pool how it is to increase its capacity each time the pool is
// depleted.  If 'growBy' is positive, the pool always replenishes itself with
// enough objects so that it can satisfy at least 'growBy' object requests
// before the next replenishment.  If 'growBy' is negative, the pool will
// increase its capacity geometrically until it exceeds the internal maximum
// (which itself is implementation-defined), and after that it will be
// replenished with constant number of objects.  If 'growBy' is not specified,
// an implementation-defined default will be chosen.  The behavior is undefined
// if growBy is 0.
//
///Usage
///-----
// This component is intended to improve the efficiency of code which provides
// shared pointers to pooled objects.  As an example, consider a class which
// maintains a pool of 'vector<char>' objects and provides shared pointers to
// them.  Using 'bdlcc::ObjectPool', the class might be implemented like this:
//..
//  typedef vector<char> CharArray;
//
//  class SlowCharArrayPool {
//      bdlma::ConcurrentPoolAllocator d_spAllocator;  // alloc. shared pointer
//      bdlcc::ObjectPool<CharArray>   d_charArrayPool;  // supply charArrays
//
//      static void createCharArray(void *address, bslma::Allocator *allocator)
//      {
//          new (address) CharArray(allocator);
//      }
//
//      static void resetAndReturnCharArray(
//                                     CharArray                    *charArray,
//                                     bdlcc::ObjectPool<CharArray> *pool)
//      {
//          charArray->clear();
//          pool->releaseObject(charArray);
//      }
//
//    private:
//      // Not implemented:
//      SlowCharArrayPool(const SlowCharArrayPool&);
//
//    public:
//      SlowCharArrayPool(bslma::Allocator *basicAllocator = 0)
//      : d_spAllocator(basicAllocator)
//      , d_charArrayPool(bdlf::BindUtil::bind(
//                                         &SlowCharArrayPool::createCharArray,
//                                         bdlf::PlaceHolders::_1,
//                                         basicAllocator),
//                        -1,
//                        basicAllocator)
//      {
//      }
//
//      void getCharArray(bsl::shared_ptr<CharArray> *charArray_sp)
//      {
//          charArray_sp->reset(d_charArrayPool.getObject(),
//                              bdlf::BindUtil::bind(
//                                 &SlowCharArrayPool::resetAndReturnCharArray,
//                                 bdlf::PlaceHolders::_1,
//                                 &d_charArrayPool),
//                              &d_spAllocator);
//      }
//  };
//..
// Note that 'SlowCharArrayPool' must allocate the shared pointer itself from
// its 'd_spAllocator' in addition to allocating the charArray from its pool.
// Moreover, note that since the same function will handle resetting the object
// and returning it to the pool, we must define a special function for that
// purpose and bind its arguments.
//
// We can solve both of these issues by using 'bdlcc::SharedObjectPool'
// instead:
//..
//  class FastCharArrayPool {
//      typedef bdlcc::SharedObjectPool<
//              CharArray,
//              bdlcc::ObjectPoolFunctors::DefaultCreator,
//              bdlcc::ObjectPoolFunctors::Clear<CharArray> > CharArrayPool;
//
//      CharArrayPool d_charArrayPool;     // supply charArrays
//
//      static void createCharArray(void *address, bslma::Allocator *allocator)
//      {
//          new (address) CharArray(allocator);
//      }
//
//    private:
//      // Not implemented:
//      FastCharArrayPool(const FastCharArrayPool&);
//
//    public:
//      FastCharArrayPool(bslma::Allocator *basicAllocator = 0)
//      : d_charArrayPool(bdlf::BindUtil::bind(
//                                         &FastCharArrayPool::createCharArray,
//                                         bdlf::PlaceHolders::_1,
//                                         bdlf::PlaceHolders::_2),
//                        -1,
//                        basicAllocator)
//      {
//      }
//
//      void getCharArray(bsl::shared_ptr<CharArray> *charArray_sp)
//      {
//          *charArray_sp = d_charArrayPool.getObject();
//      }
//  };
//..
// Now the shared pointer and the object are allocated as one unit from the
// same allocator.  In addition, the resetter method is a fully-inlined class
// that is only responsible for resetting the object, improving efficiency and
// simplifying the design.  We can verify that use of 'bdlcc::SharedObjectPool'
// reduces the number of allocation requests:
//..
//  bslma::TestAllocator slowAllocator, fastAllocator;
//  {
//      SlowCharArrayPool slowPool(&slowAllocator);
//      FastCharArrayPool fastPool(&fastAllocator);
//
//      bsl::shared_ptr<CharArray> charArray_sp;
//
//      fastPool.getCharArray(&charArray_sp);
//      slowPool.getCharArray(&charArray_sp);  // throw away the first array
//  }
//
//  assert(2 == slowAllocator.numAllocations());
//  assert(1 == fastAllocator.numAllocations());
//  assert(0 == slowAllocator.numBytesInUse());
//  assert(0 == fastAllocator.numBytesInUse());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLCC_OBJECTPOOL
#include <bdlcc_objectpool.h>
#endif

#ifndef INCLUDED_BDLF_BIND
#include <bdlf_bind.h>
#endif

#ifndef INCLUDED_BDLF_PLACEHOLDER
#include <bdlf_placeholder.h>
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

#ifndef INCLUDED_BSLMA_SHAREDPTRREP
#include <bslma_sharedptrrep.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {
namespace bdlcc {

                         // ==========================
                         // class SharedObjectPool_Rep
                         // ==========================

template <class TYPE, class RESETTER>
class SharedObjectPool_Rep: public bslma::SharedPtrRep {

    typedef SharedObjectPool_Rep<TYPE, RESETTER> MyType;
    typedef ObjectPool<MyType,
                            ObjectPoolFunctors::DefaultCreator,
                            ObjectPoolFunctors::Reset<MyType> >
                                                      PoolType;

    // DATA
    bslalg::ConstructorProxy<RESETTER> d_objectResetter;

    PoolType                          *d_pool_p;   // object pool (held)
    bsls::ObjectBuffer<TYPE>           d_instance; // area for embedded
                                                   // instance

    // NOT IMPLEMENTED
    SharedObjectPool_Rep(const SharedObjectPool_Rep&);
    SharedObjectPool_Rep& operator=(const SharedObjectPool_Rep&);

  public:
    // CREATORS
    template <class CREATOR>
    SharedObjectPool_Rep(
                    CREATOR*                                   objectCreator,
                    const bslalg::ConstructorProxy<RESETTER>&  objectResetter,
                    PoolType                                  *pool,
                    bslma::Allocator                          *basicAllocator);
        // Construct a new rep object that, upon release, will invoke the
        // specified 'objectResetter' and return itself to the specified
        // 'pool'; then invoke 'objectCreator' to construct an object of 'TYPE'
        // embedded within the new rep object.  Use the specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~SharedObjectPool_Rep();
        // Destroy this representation object and the embedded instance of
        // 'TYPE'.

    // MANIPULATORS
    virtual void disposeRep();
        // Release this representation object.  This method is invoked when the
        // number of weak references and the number of strong references reach
        // zero.  This virtual override will return the object, and this
        // representation, to the associated pool.

    virtual void disposeObject();
        // Release the object being managed by this representation.  This
        // method is invoked when the number of strong references reaches zero.
        // Note that if there are any weak references to the shared object then
        // this function does nothing, including not destroying the object or
        // returning it to the pool.

    void reset();
        // Invoke the object resetter specified at construction on the
        // associated object.

    virtual void *getDeleter(const std::type_info& type);
        // Return NULL.  Shared object pools strictly control the delete policy
        // for their objects, and do not expose it to end users.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return (untyped) address of the object managed by this
        // representation.  This virtual override effectively returns
        // "(void*)ptr()".

    TYPE *ptr();
        // Return a pointer to the in-place object.
};

                           // ======================
                           // class SharedObjectPool
                           // ======================

template <class TYPE,
          class CREATOR=ObjectPoolFunctors::DefaultCreator,
          class RESETTER=ObjectPoolFunctors::Nil<TYPE> >
class SharedObjectPool {

    typedef SharedObjectPool<TYPE, CREATOR, RESETTER> MyType;
    typedef SharedObjectPool_Rep<TYPE, RESETTER>      RepType;
    typedef ObjectPool<RepType,
                            ObjectPoolFunctors::DefaultCreator,
                            ObjectPoolFunctors::Reset<RepType> >
                                                           PoolType;

    typename ObjectPool_ProxyPicker<CREATOR>::template Selector<TYPE>::Proxy
                                d_objectCreator; // functor for object creation

    bslalg::ConstructorProxy<RESETTER>
                                d_objectResetter;  // functor to reset object

    PoolType                    d_pool;           // object pool (owned)

  private:
    // NOT IMPLEMENTED
    SharedObjectPool(const SharedObjectPool&);
    SharedObjectPool& operator=(const SharedObjectPool&);

    void constructRepObject(void *memory, bslma::Allocator *alloc);
        // Initializes a newly constructed SharedObjectPool_Rep object

  public:
    // TYPES
    typedef CREATOR    CreatorType;
    typedef RESETTER   ResetterType;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SharedObjectPool,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    SharedObjectPool(int growBy = -1, bslma::Allocator *basicAllocator = 0);

    explicit
    SharedObjectPool(const CREATOR&    objectCreator,
                     int               growBy = -1,
                     bslma::Allocator *basicAllocator = 0);

    explicit
    SharedObjectPool(const CREATOR&    objectCreator,
                     bslma::Allocator *basicAllocator = 0);

    SharedObjectPool(const CREATOR&    objectCreator,
                     const RESETTER&   objectResetter,
                     int               growBy = -1,
                     bslma::Allocator *basicAllocator = 0);

        // Create an object pool that dispenses shared pointers to TYPE.  When
        // the pool is depleted, it increases its capacity according to the
        // optionally specified 'growBy' value.  If 'growBy' is positive, the
        // pool always increases by at least 'growBy'.  If 'growBy' is
        // negative, the amount of increase begins at '-growBy' and then grows
        // geometrically up to an implementation-defined maximum.  The
        // optionally specified 'objectCreator' is called whenever objects must
        // be constructed.  If 'objectCreator' is not specified and the
        // parameterized 'CREATOR' is the default type (that is,
        // 'ObjectPoolFunctors::DefaultCreator'), a function that calls the
        // default constructor of 'TYPE' with placement new, passing this
        // pool's allocator if TYPE uses allocator, is used.  If the
        // parameterized 'CREATOR' is some other type, and 'objectCreator' is
        // not specified, the default value of the 'CREATOR' type is used.  The
        // optionally specified 'objectResetter' is invoked with a pointer to
        // an object of 'TYPE' when the object is returned to the pool.  It
        // must reset the object into a valid state for reuse.  If
        // 'objectResetter' is not specified, a default RESETTER object is
        // used.  Optionally specify a basic allocator to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if 'growBy' is 0.

    ~SharedObjectPool();
        // Destroy this object pool.  All objects created by this pool are
        // destroyed (even if some of them are still in use) and memory is
        // reclaimed.

    // MANIPULATORS
    bsl::shared_ptr<TYPE> getObject();
        // Return a pointer to an object from this object pool.  When the last
        // shared pointer to the object is destroyed, the object will be reset
        // as specified at construction and then returned to the pool.  If this
        // pool is empty, it is replenished according to the strategy specified
        // at construction.

    void increaseCapacity(int growBy);
        // Create the specified 'growBy' objects and add them to this object
        // pool.  The behavior is undefined unless '0 <= growBy'.

    void reserveCapacity(int growBy);
        // Create enough objects to satisfy requests for at least the specified
        // 'growBy' objects before the next replenishment.  The behavior is
        // undefined unless '0 <= growBy'.  Note that this method is different
        // from 'increaseCapacity' in that the number of created objects may be
        // less than 'growBy'.

    // ACCESSORS
    int numAvailableObjects() const;
        // Return a *snapshot* of the number of objects available in this pool.

    int numObjects() const;
        // Return the (instantaneous) number of objects managed by this pool.
        // This includes both the objects available in the pool and the objects
        // that were allocated from the pool and not yet released.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class SharedObjectPool_Rep
                         // --------------------------

// CREATORS
template <class TYPE, class RESETTER>
template <class CREATOR>
inline
SharedObjectPool_Rep<TYPE, RESETTER>::SharedObjectPool_Rep(
                     CREATOR*                                   objectCreator,
                     const bslalg::ConstructorProxy<RESETTER>&  objectResetter,
                     PoolType                                  *pool,
                     bslma::Allocator                          *basicAllocator)
: d_objectResetter(objectResetter,basicAllocator)
, d_pool_p(pool)
{
    (*objectCreator)(d_instance.buffer(), basicAllocator);
}

template <class TYPE, class RESETTER>
inline
SharedObjectPool_Rep<TYPE, RESETTER>::~SharedObjectPool_Rep()
{
    d_instance.object().~TYPE();
}

// MANIPULATORS
template <class TYPE, class RESETTER>
inline
void SharedObjectPool_Rep<TYPE, RESETTER>::reset()
{
    d_objectResetter.object()(&d_instance.object());
}

template <class TYPE, class RESETTER>
inline
void SharedObjectPool_Rep<TYPE, RESETTER>::disposeRep()
{
    d_pool_p->releaseObject(this);
}

template <class TYPE, class RESETTER>
inline
void *SharedObjectPool_Rep<TYPE, RESETTER>::getDeleter(
                                                const std::type_info& /*type*/)
{
    return 0;
}

template <class TYPE, class RESETTER>
inline
void SharedObjectPool_Rep<TYPE, RESETTER>::disposeObject()
{
    // No-op
}

// ACCESSORS
template <class TYPE, class RESETTER>
inline
void *SharedObjectPool_Rep<TYPE, RESETTER>::originalPtr() const
{
    return const_cast<void*>(static_cast<const void*>(d_instance.buffer()));
}

template <class TYPE, class RESETTER>
inline
TYPE *SharedObjectPool_Rep<TYPE, RESETTER>::ptr()
{
    return &d_instance.object();
}

                             // ----------------
                             // SharedObjectPool
                             // ----------------

// PRIVATE
template <class TYPE, class CREATOR, class RESETTER>
inline
void SharedObjectPool<TYPE, CREATOR, RESETTER>::constructRepObject(
                                                      void             *memory,
                                                      bslma::Allocator *alloc)
{
    RepType *r = new (memory) RepType(&d_objectCreator.object(),
                                      d_objectResetter,
                                      &d_pool,
                                      alloc);
    r->resetCountsRaw(0, 0);
}

}  // close package namespace

// CREATORS
#if defined(BSLS_PLATFORM_CMP_MSVC)
// Visual C++ complains about any use of the 'this' pointer in a member
// initializer of a constructor.  The use cases below seem perfectly safe and
// correct, but there is no way to eliminate this warning from a prominent
// header file other than disabling it via a pragma.
#pragma warning(push)
#pragma warning(disable : 4355) // used 'this' in member initializer
#endif

namespace bdlcc {

template <class TYPE, class CREATOR, class RESETTER>
inline
SharedObjectPool<TYPE, CREATOR, RESETTER>::SharedObjectPool(
                                              const CREATOR&    objectCreator,
                                              const RESETTER&   objectResetter,
                                              int               growBy,
                                              bslma::Allocator *basicAllocator)
: d_objectCreator(objectCreator,basicAllocator)
, d_objectResetter(objectResetter,basicAllocator)
, d_pool(bdlf::BindUtil::bind(&MyType::constructRepObject, this,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2),
         growBy, basicAllocator)
{
}

template <class TYPE, class CREATOR, class RESETTER>
inline
SharedObjectPool<TYPE, CREATOR, RESETTER>::SharedObjectPool(
                                              int               growBy,
                                              bslma::Allocator *basicAllocator)
: d_objectCreator(basicAllocator)
, d_objectResetter(basicAllocator)
, d_pool(bdlf::BindUtil::bind(&MyType::constructRepObject, this,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2),
         growBy, basicAllocator)
{
}

template <class TYPE, class CREATOR, class RESETTER>
inline
SharedObjectPool<TYPE, CREATOR, RESETTER>::SharedObjectPool(
                                              const CREATOR&    objectCreator,
                                              int               growBy,
                                              bslma::Allocator *basicAllocator)
: d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(basicAllocator)
, d_pool(bdlf::BindUtil::bind(&MyType::constructRepObject, this,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2),
         growBy, basicAllocator)
{
}

template <class TYPE, class CREATOR, class RESETTER>
inline
SharedObjectPool<TYPE, CREATOR, RESETTER>::SharedObjectPool(
                                              const CREATOR&    objectCreator,
                                              bslma::Allocator *basicAllocator)
: d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(basicAllocator)
, d_pool(bdlf::BindUtil::bind(&MyType::constructRepObject, this,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2),
         -1, basicAllocator)
{
}
}  // close package namespace

#if defined(BSLS_PLATFORM_CMP_MSVC)
// Restore warnings so as not to affect their state in other files.
#pragma warning(pop)
#endif

namespace bdlcc {

template <class TYPE, class CREATOR, class RESETTER>
inline
SharedObjectPool<TYPE, CREATOR, RESETTER>::~SharedObjectPool()
{
}

// MANIPULATORS
template <class TYPE, class CREATOR, class RESETTER>
inline
bsl::shared_ptr<TYPE>
SharedObjectPool<TYPE, CREATOR, RESETTER>::getObject()
{
    RepType *rep = d_pool.getObject();
    bslma::SharedPtrRep *genericRep = rep;
    genericRep->resetCountsRaw(1, 0);

    return bsl::shared_ptr<TYPE>(rep->ptr(), genericRep);
}

template <class TYPE, class CREATOR, class RESETTER>
inline
void
SharedObjectPool<TYPE, CREATOR, RESETTER>::increaseCapacity(int growBy)
{
    d_pool.increaseCapacity(growBy);
}

template <class TYPE, class CREATOR, class RESETTER>
inline
void
SharedObjectPool<TYPE, CREATOR, RESETTER>::reserveCapacity(int growBy)
{
    d_pool.reserveCapacity(growBy);
}

// ACCESSORS
template <class TYPE, class CREATOR, class RESETTER>
inline
int SharedObjectPool<TYPE, CREATOR, RESETTER>::numAvailableObjects() const
{
    return d_pool.numAvailableObjects();
}

template <class TYPE, class CREATOR, class RESETTER>
inline
int SharedObjectPool<TYPE, CREATOR, RESETTER>::numObjects() const
{
    return d_pool.numObjects();
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
