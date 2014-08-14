// pkg_objectpool.h                                                   -*-C++-*-
#ifndef INCLUDED_PKG_OBJECTPOOL
#define INCLUDED_PKG_OBJECTPOOL

//@PURPOSE: Provide a pool for efficient management of objects.
//
//@CLASSES:
//  pkg::ObjectPool: container for managed objects
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a mechanism, 'pkg::ObjectPool', for
// managing a generic pool of objects using the acquire-release idiom.  An
// object pool provides two main methods: 'getObject', which returns an object
// from the pool, and 'releaseObject', which returns an object to the pool for
// further reuse (thus avoiding the overhead of object construction and
// destruction).
//
///Object Construction, Destruction, and Resetting
///- - - - - - - - - - - - - - - - - - - - - - - -
// This pool requires the templated type used to construct an 'ObjectPool' to
// provide a default constructor, public destructor, and a 'reset' function
// that changes the state of an object to its default-constructed object.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using an Object Pool of Database Connections
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// An object pool can be used to store objects that are expensive to create and
// destroy.
//
// First, we define a 'class', 'DatabaseConnection', that represents a database
// connection:
//..
//  class Query;
//  class QueryResult;
//
//  class DatabaseConnection {
//      // This 'class' represents a database connection.
//
//      // DATA
//
//      // ..
//
//      bslma::Allocator d_allocator_p;   // memory allocator (held, not owned)
//
//    public:
//      // CREATORS
//      DatabaseConnection(bslma::Allocator basicAllocator = 0);
//          // Create a 'DatabaseConnection' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~DatabaseConnection();
//          // Destroy this 'DatabaseConnection' object.
//
//      // MANIPULATORS
//      void reset();
//          // Reset this object.
//
//      int executeQuery(QueryResult *queryResult, const Query& query);
//          // Execute the specified 'query' and load the result in the
//          // specified 'queryResult'.  Return 0 on success, and a non-zero
//          // value otherwise.
//  };
//
//  // TRAITS
//  namespace BloombergLP{
//  namespace bslma {

//  template <> struct UsesBslmaAllocator<DatabaseConnection>
//                                                         : bsl::true_type {};

//  }
//  }
//..
// Then, we will provide the function definitions for this 'class'.  To keep
// this example simple, the constructor, destructor, and 'reset' methods of
// this 'class' are no-ops and are therefore not shown below:
//..
//  int DatabaseConnection::executeQuery(QueryResult  *queryResult,
//                                       const Query&  query)
//  {
//      // Execute 'query' and load the result into 'queryResult'
//
//      return 0;
//  }
//..
// Next, we create an object pool representing a list of connections.  Since
// we do not supply an allocator, 'pool' will use the default allocator to
// supply memory:
//..
//  ObjectPool<DatabaseConnection> pool;
//..
// Now, the 'pool' object can be used to retrieve and return connections as
// required:
//..
//  DatabaseConnection *connection = pool.getObject();
//
//  // Use the 'connection'.
//
//  pool.releaseObject(connection);
//..

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

                        // ================
                        // class ObjectPool
                        // ================

template <typename TYPE>
class ObjectPool {
    // This 'class' provides a pool of reusable objects of template parameter
    // type 'TYPE' and assumes that 'TYPE' provides a default constructor, a
    // public destructor, and a 'reset' method.

    // DATA
    bsl::list<TYPE *>               d_objects;      // list of managed objects
    BloombergLP::bslma::Allocator  *d_allocator_p;  // memory allocator (held,
                                                    // not owned)

    // PRIVATE CLASS METHODS
    TYPE *createObject(bsl::false_type);
        // Construct an object of the specified 'TYPE' that *does not* use the
        // allocator passed to its constructor.

    TYPE *createObject(bsl::true_type);
        // Construct an object of the specified 'TYPE' that *does* use the
        // allocator passed to its constructor.

  public:
    // CREATORS
    explicit ObjectPool(BloombergLP::bslma::Allocator *basicAllocator = 0);
        // Create an object pool that invokes the default constructor of the
        // the parameterized 'TYPE' to construct objects.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~ObjectPool();
        // Destroy this object pool.  All objects created by this pool are
        // destroyed (even if some of them are still in use) and memory is
        // reclaimed.

    // MANIPULATORS
    TYPE *getObject();
        // Return an address providing modifiable access to a
        // default-constructed object of the parameterized 'TYPE'.  If this
        // pool does not have any free objects then a default-constructed
        // object is allocated and returned.

    void releaseObject(TYPE *object);
        // Return the specified 'object' back to this object pool for
        // subsequent reuse.  Invoke the 'reset' method on 'object'.  The
        // behavior is undefined unless 'object' was constructed by this pool.

    // The rest of the interface is elided for brevity.
};

}  // close package namespace
}  // close enterprise namespace

// TRAITS
namespace BloombergLP{
namespace bslma {

template <typename TYPE>
struct UsesBslmaAllocator<Enterprise::pkg::ObjectPool<TYPE> >
                                                           : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

namespace Enterprise {
namespace pkg {

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------
                        // class ObjectPool
                        // ----------------

// PRIVATE CLASS METHODS
template <typename TYPE>
inline
TYPE *ObjectPool<TYPE>::createObject(bsl::false_type)
{
    return new (*d_allocator_p) TYPE();
}

template <typename TYPE>
inline
TYPE *ObjectPool<TYPE>::createObject(bsl::true_type)
{
    return new (*d_allocator_p) TYPE(d_allocator_p);
}

// CREATORS
template <typename TYPE>
inline
ObjectPool<TYPE>::ObjectPool(BloombergLP::bslma::Allocator *basicAllocator)
: d_objects(basicAllocator)
, d_allocator_p(BloombergLP::bslma::Default::allocator(basicAllocator))
{
}

template <typename TYPE>
ObjectPool<TYPE>::~ObjectPool()
{
    for (typename bsl::list<TYPE *>::iterator iter = d_objects.begin();
         iter != d_objects.end();
         ++iter) {
        d_allocator_p->deleteObject(*iter);
    }
    d_objects.clear();
}

// MANIPULATORS
template <typename TYPE>
TYPE *ObjectPool<TYPE>::getObject()
{
    if (d_objects.size()) {
        TYPE *object = d_objects.back();
        d_objects.pop_back();
        return object;                                                // RETURN
    }

    return createObject(BloombergLP::bslma::UsesBslmaAllocator<TYPE>());
}

template <typename TYPE>
inline
void ObjectPool<TYPE>::releaseObject(TYPE *object)
{
    object->reset();
    d_objects.push_back(object);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
