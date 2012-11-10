// objectpool.h                                                       -*-C++-*-
#ifndef INCLUDED_OBJECTPOOL
#define INCLUDED_OBJECTPOOL

//@PURPOSE: Provide a pool for efficient management of objects.
//
//@CLASSES:
//  pkg::ObjectPool: container for managed objects
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a generic pool of objects using the
// acquire-release idiom.  An object pool provides two main methods:
// 'getObject', which returns an object from the pool, and 'releaseObject',
// which returns an object to the pool for further reuse (thus avoiding the
// overhead of object construction and destruction).
//
///Object construction, destruction, and resetting
///- - - - - - - - - - - - - - - - - - - - - - - -
// This pool relies on the templated type used to construct an 'ObjectPool'
// provides a default constructor, destructor, and a 'reset' function that
// changes the state of an object to its default-constructed object.
//
///Usage
///-----
// This section illustrates intended use of this component.

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSL_LIST
#include <bsl_list.h>
#endif

namespace Enterprise {
namespace pkg {

using namespace BloombergLP;

                        // ================
                        // class ObjectPool
                        // ================

template <typename TYPE>
class ObjectPool {
    // This 'class' provides a pool of reusable objects of the parameterized
    // 'TYPE' and assumes that the parameterized 'TYPE' provides a default
    // constructor, a destructor, and a 'reset' method.

    // DATA
    bsl::list<TYPE *>  d_objects;      // list of managed objects
    bslma::Allocator  *d_allocator_p;  // memory allocator

  public:
    ObjectPool(bslma::Allocator *basicAllocator = 0);
        // Create an object pool that invokes the default constructor of the
        // the parameterized 'TYPE' to construct objects.  The optionally
        // specified 'basicAllocator' is used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~ObjectPool();
        // Destroy this object pool.  All objects created by this pool are
        // destroyed (even if some of them are still in use) and memory is
        // reclaimed.

    TYPE* getObject();
        // Return an address providing modifiable access to a
        // default-constructed object of the parameterized 'TYPE'.  If this
        // pool does not have any free objects then a default-constructed
        // object is allocated and returned.

    void releaseObject(TYPE *object);
        // Return the specified 'object' back to this object pool.  Invoke the
        // 'reset' method on 'object'.

    // The rest of the interface is elided for brevity.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------
                        // class ObjectPool
                        // ----------------

// CREATORS
template <typename TYPE>
inline
ObjectPool<TYPE>::ObjectPool(bslma::Allocator *basicAllocator)
: d_objects(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename TYPE>
inline
ObjectPool<TYPE>::~ObjectPool()
{
    const int size = d_objects.size();
    for (int i = 0; i < size; ++i) {
        d_allocator_p->deleteObject(d_objects[i]);
    }
}

// MANIPULATORS
template <typename TYPE>
TYPE *ObjectPool<TYPE>::getObject()
{
    if (d_objects.size()) {
        TYPE *object = d_objects.back();
        d_objects.pop_back();
        return object;                          // RETURN
    }

    TYPE *object;
    if (bslma::UsesBslmaAllocator<TYPE>::value) {
        object = new (*d_allocator_p) TYPE(d_allocator_p);
    }
    else {
        object = new (*d_allocator_p) TYPE();
    }
    return object;
}

template <typename TYPE>
inline
void ObjectPool<TYPE>::releaseObject(TYPE *object)
{
    d_objects.push_back(object);
}

}  // close package namespace
}  // close enterprise namespace

#endif
