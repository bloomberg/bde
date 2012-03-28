// bcec_objectcatalog.h                                               -*-C++-*-
#ifndef INCLUDED_BCEC_OBJECTCATALOG
#define INCLUDED_BCEC_OBJECTCATALOG

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an efficient indexed, thread-safe object container.
//
//@CLASSES:
//     bcec_ObjectCatalog: templatized, thread-safe, indexed object container
// bcec_ObjectCatalogIter: thread-safe iterator for 'bcec_ObjectCatalog'
//
//@SEE_ALSO:
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a thread-safe and efficient templatized
// catalog of objects.  A 'bcec_ObjectCatalog' supports efficient insertion of
// objects through the 'add' method, which returns a handle that can be used
// for further reference to the newly added element.  An element can be
// accessed by providing its handle to the 'find' function.  Thread-safe design
// implies that the element is returned by value into an object buffer rather
// than by reference (see this package documentation for a discussion of
// thread-safe container design).  Likewise, an element can be modified by
// providing its handle and a new value to the 'replace' method.  Finally, an
// element can be removed by passing its handle to the 'remove' method; the
// handle is then no longer valid and subsequent calls to 'find' or 'remove'
// with this handle will return 0.
//
// 'bcec_ObjectCatalogIter' provides thread safe iteration through all the
// objects of an object catalog of parameterized 'TYPE'.  The order of the
// iteration is implementation defined.  Thread safe iteration is provided
// by (read)locking the object catalog during the iterator's construction
// and unlocking it at the iterator's destruction.  This guarantees that
// during the life time of an iterator, the object catalog can't be
// modified (however multiple threads can still concurrently read the object
// catalog).
//
///Usage
///-----
// Consider a client sending queries to a server asynchronously.  When the
// response to a query arrives, the client needs to invoke the callback
// associated with that query.  For good performance, the callback should
// be invoked as quickly as possible.  One way to achieve this is as follows.
// The client creates a catalog for the functors associated with queries.
// It sends to the server the handle (obtained by passing the callback
// functor associated with the query to the 'add' method of catalog), along
// with the query.  The server does not interpret this handle in any way
// and sends it back to the client along with the computed query result.
// The client, upon receiving the response, gets the functor (associated
// with the query) back by passing the handle (contained in the response
// message) to the 'find' method of catalog.
//
// Assume the following declarations (we leave the implementations as
// undefined, as the definitions are largely irrelevant to this example):
//..
//   struct Query;
//       // Class simulating the query.
//
//   class QueryResult;
//       // Class simulating the result of a query.
//
//   class RequestMsg
//       // Class encapsulating the request message.  It encapsulates the
//       // actual query and the handle associated with the callback for
//       // the query.
//   {
//     public:
//       RequestMsg(Query query, int handle);
//           // Create a request message with the specified 'query' and
//           // 'handle'.
//   };
//
//   class ResponseMsg
//       // Class encapsulating the response message.  It encapsulates the
//       // query result and the handle associated with the callback for
//       // the query.
//   {
//     public:
//       QueryResult queryResult() const;
//           // Return the query result contained in this response message.
//       int handle() const;
//           // Return the handle contained in this response message.
//   };
//
//   void sendMessage(RequestMsg msg, RemoteAddress peer);
//       // Send the specified 'msg' to the specified 'peer'.
//
//   void recvMessage(ResponseMsg *msg, RemoteAddress peer);
//       // Get the response from the specified 'peer' into '*msg'.
//
//   void getQueryAndCallback(Query                                *query,
//                            bdef_Function<void (*)(QueryResult)> *callBack);
//       // Set the '*query' and '*callBack' to the next query and its
//       // associated callback (the functor to be called when the response
//       // to this query comes in).
//..
// Furthermore, let also the following variables be declared:
//..
//   RemoteAddress serverAddress;
//       // Address of remote server.
//
//   bcec_ObjectCatalog<bdef_Function<void (*)(QueryResult)> > catalog;
//       // Catalog of query callbacks, used by the client internally to
//       // keep track of callback functions across multiple queries.  The
//       // invariant is that each element corresponds to a pending query
//       // (i.e., the callback function has not yet been or is in the
//       // process of being invoked).
//
//..
// In some thread, the client executes the following code.
//..
//   while (1)
//   {
//       Query query;
//       bdef_Function<void (*)(QueryResult)> callBack;
//
//       // The following call blocks until a query becomes available.
//       getQueryAndCallback(&query, &callBack);
//
//       // Register 'callBack' in the object catalog.
//       int handle = catalog.add(callBack);
//       ASSERT(handle);
//
//       // Send query to server in the form of a 'RequestMsg'.
//       RequestMsg msg(query, handle);
//       sendMessage(msg, serverAddress);
//   }
//..
// In some other thread, the client executes the following code.
//..
//   while (1)
//   {
//       // The following call blocks until some response is available
//       // in the form of a 'ResponseMsg'.
//       ResponseMsg msg;
//       recvMessage(&msg, serverAddress);
//       int handle = msg.handle();
//       QueryResult result = msg.queryResult();
//
//       // Process query 'result' by applying registered 'callBack'
//       // to it.  The 'callBack' function is retrieved from the
//       // 'catalog' using the given 'handle'.
//       bdef_Function<void (*)(QueryResult)> callBack;
//       ASSERT(0 == catalog.find(handle, &callBack));
//       callBack(result);
//
//       // Finally, remove the no-longer-needed 'callBack' from the
//       // 'catalog'.  Assert so that 'catalog' may not grow unbounded
//       // if remove fails.
//       ASSERT(0 == catalog.remove(handle));
//   }
//..
//
///Iterator Usage
///- - - - - - -
// The following code fragment shows how to use bcec_ObjectCatalogIter to
// iterate through all the objects of 'catalog' (a catalog of objects of
// type 'MyType').
//..
//      for (bcec_ObjectCatalogIter<MyType> it(catalog); it; ++it) {
//          bsl::pair<int, MyType> p = it(); // p.first contains the handle and
//                                           // p.second contains the object
//          use(p.second);                   // the function 'use' uses the
//                                           // object in some way
//      }
//      // 'it' is now destroyed out of the scope, releasing the lock.
//..
// Note that the associated catalog is (read)locked when the iterator is
// constructed and is unlocked only when the iterator is destroyed.  This
// means that until the iterator is destroyed, all the threads trying to
// modify the catalog will remain blocked (even though multiple threads can
// concurrently read the object catalog).  So clients must make sure to
// destroy their iterators after they are done using them.  One easy way is
// to use the 'for (bcec_ObjectCatalogIter<MyType> it(catalog); ...' as above.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BCEMT_READLOCKGUARD
#include <bcemt_readlockguard.h>
#endif

#ifndef INCLUDED_BCEMT_WRITELOCKGUARD
#include <bcemt_writelockguard.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

template <class TYPE> class bcec_ObjectCatalog_AutoCleanup;
template <class TYPE> class bcec_ObjectCatalogIter;
template <class TYPE> class bcec_ObjectCatalog;

                   // ==========================================
                   // local class bcec_ObjectCatalog_AutoCleanup
                   // ==========================================

template <class TYPE>
class bcec_ObjectCatalog_AutoCleanup {
    // This class provides a specialized proctor object that, upon destruction
    // and unless the 'release' method is called (1) removes a managed
    // node from the 'bcec_ObjectCatalog', and (2) deallocates all
    // associated memory as necessary.

    bcec_ObjectCatalog<TYPE> *d_catalog_p;    // temporarily managed catalog
    typename bcec_ObjectCatalog<TYPE>::Node
                           *d_node_p;         // temporarily managed node
    bool                    d_deallocateFlag; // how to return the managed node

    // NOT IMPLEMENTED
    bcec_ObjectCatalog_AutoCleanup(const bcec_ObjectCatalog_AutoCleanup&);
    bcec_ObjectCatalog_AutoCleanup& operator=(const
                                              bcec_ObjectCatalog_AutoCleanup&);

    public:
    // CREATORS
    explicit bcec_ObjectCatalog_AutoCleanup(bcec_ObjectCatalog<TYPE> *catalog);
        // Create a proctor to manage the specified 'catalog'.

    ~bcec_ObjectCatalog_AutoCleanup();
        // Remove a managed node from the 'bcec_ObjectCatalog' (by returning it
        // to the catalog's free list or node pool, as specified in
        // 'manageNode'), deallocate all associated memory, and destroy this
        // object.

    // MANIPULATORS
    void manageNode(typename bcec_ObjectCatalog<TYPE>::Node *node,
                    bool                                     deallocateFlag);
        // Release from management the catalog node, if any, currently
        // managed by this object and begin managing the specified catalog
        // 'node'.  The specified 'deallocateFlag' tells the destructor
        // how to dispose of 'node' if 'node' is managed during the destruction
        // of this object.

    void releaseNode();
        // Release from management the catalog node, if any, currently
        // managed by this object, if any.

    void release();
        // Release from management all resources currently managed by this
        // object, if any.
};

                          // ========================
                          // class bcec_ObjectCatalog
                          // ========================

template <class TYPE>
class bcec_ObjectCatalog {
    // This class defines an efficient indexed object catalog of 'TYPE'
    // objects.  This container is *exception* *neutral* with no guarantee of
    // rollback: if an exception is thrown during the invocation of a method on
    // a pre-existing instance, the object is left in a valid but undefined
    // state.  In no event is memory leaked or a mutex left in a locked state.

    // PRIVATE TYPES
    enum {
        // Masks used for breaking up a handle.  Note: a handle (of type int)
        // is always 4 bytes, even on 64 bit modes.

        BCEC_INDEX_MASK      = 0x007fffff
      , BCEC_BUSY_INDICATOR  = 0x00800000
      , BCEC_GENERATION_INC  = 0x01000000
      , BCEC_GENERATION_MASK = 0xff000000
    };

    struct Node {
        union {
            char                               d_value[sizeof(TYPE)];

            Node                              *d_next_p; // when free, pointer
                                                         // to next free node

            bsls_AlignmentUtil::MaxAlignedType d_filler;
        };
        int  d_handle;
    };

    // DATA
    bsl::vector<Node*>     d_nodes;
    bdema_Pool             d_nodePool;
    Node                  *d_nextFreeNode_p;
    volatile int           d_length;
    mutable bcemt_RWMutex  d_lock;

    // FRIENDS
    friend class bcec_ObjectCatalog_AutoCleanup<TYPE>;
    friend class bcec_ObjectCatalogIter<TYPE>;

  private:
    // PRIVATE MANIPULATORS
    void freeNode(Node *node);
        // Add the specified 'node' to the free node list.  Destruction of the
        // object held in the node must be handled by the 'remove' function
        // directly.  (This is because 'freeNode' is also used in the
        // 'bcec_ObjectCatalog_AutoCleanup' guard, but there it should not
        // invoke the object's destructor.)

    // PRIVATE ACCESSORS
    Node *findNode(int handle) const;
        // Return a pointer to the node with the specified 'handle', or 0 if
        // not found.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcec_ObjectCatalog,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bcec_ObjectCatalog(bslma_Allocator *allocator = 0);
        // Create an empty object catalog, using the optionally specified
        // 'allocator' to supply any memory.

    ~bcec_ObjectCatalog();
        // Destroy this object catalog.

    // MANIPULATORS
    int add(TYPE const& object);
        // Add the value of the specified  'object' to this catalog and return
        // a non-zero integer handle that may be used to refer to the object in
        // future calls to this catalog.

    int remove(int handle, TYPE *valueBuffer = 0);
        // Optionally load into the specified 'valueBuffer' the value of the
        // object having the specified 'handle' and remove it from this
        // catalog.  Return zero on success, and a non-zero value if the
        // specified 'handle' is not contained in this catalog.  Note that
        // 'valueBuffer' is assigned into, and thus must point to a valid
        // 'TYPE' instance.

    void removeAll(bsl::vector<TYPE> *buffer = 0);
        // Remove all objects that are currently held in this catalog and
        // optionally load into the specified 'buffer' the removed objects.

    int replace(int handle, const TYPE& newObject);
        // Replace the object having the specified 'handle' with the specified
        // 'newObject'.  Return 0 on success, and a non-zero value if the
        // specified handle is not contained in this catalog.

    // ACCESSORS
    int find(int handle, TYPE *valueBuffer = 0) const;
        // Locate the object having the specified 'handle' and optionally load
        // its value into the specified 'valueBuffer'.  Return zero on success,
        // and a non-zero value if the specified 'handle' is not contained in
        // this catalog.  Note that 'valueBuffer' is assigned into, and thus
        // must point to a valid 'TYPE' instance.

    int length() const;
        // Return a "snapshot" of the number of items currently contained in
        // this catalog.

    // FOR TESTING PURPOSES ONLY
    void verifyState() const;
        // Verify that this catalog is in a consistent state.  This function
        // is introduced for testing purposes only.
};

                         // ============================
                         // class bcec_ObjectCatalogIter
                         // ============================

template <class TYPE>
class bcec_ObjectCatalogIter {
    // Provide thread safe iteration through all the objects of an object
    // catalog of parameterized 'TYPE'.  The order of the iteration is
    // implementation defined.  An iterator is *valid* if it is associated
    // with an object in the catalog, otherwise it is *invalid*.  Thread-safe
    // iteration is provided by (read)locking the object catalog during the
    // iterator's construction and unlocking it at the iterator's destruction.
    // This guarantees that during the life time of an iterator, the object
    // catalog can't be modified (nevertheless, multiple threads can
    // concurrently read the object catalog).

    const bcec_ObjectCatalog<TYPE> *d_catalog_p;
    int                             d_index;

    // NOT IMPLEMENTED
    bcec_ObjectCatalogIter(const bcec_ObjectCatalogIter& original);
    bcec_ObjectCatalogIter& operator=(const bcec_ObjectCatalogIter& rhs);

    bool operator==(const bcec_ObjectCatalogIter&) const;
    bool operator!=(const bcec_ObjectCatalogIter&) const;

    template<typename OTHER>
    bool operator==(const bcec_ObjectCatalogIter<OTHER>&) const;
    template<typename OTHER>
    bool operator!=(const bcec_ObjectCatalogIter<OTHER>&) const;

  public:
    // CREATORS
    explicit bcec_ObjectCatalogIter(const bcec_ObjectCatalog<TYPE>& catalog);
        // Create an iterator for the specified 'catalog' and associate it
        // with the first member of the 'catalog'.  If the 'catalog' is
        // empty then the iterator is initialized to be invalid.  The
        // 'catalog' is locked for read for the duration of iterator's life.

    ~bcec_ObjectCatalogIter();
        // Destroy this iterator and unlock the catalog associated with it.

    // MANIPULATORS
    void operator++();
        // Advance this iterator to refer to the next object of the associated
        // catalog; if there is no next object in the associated catalog, then
        // this iterator becomes *invalid*.  The behavior is undefined unless
        // this iterator is valid.  Note that the order of the iteration is
        // not specified.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the iterator is *valid*, and 0 otherwise.

    bsl::pair<int, TYPE> operator()() const;
        // Return a pair containing the handle (as the first element of the
        // pair) and the object (as the second element of the pair)
        // associated with this iterator.  The behavior is undefined unless
        // the iterator is *valid*.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

                          // ------------------------------------------
                          // local class bcec_ObjectCatalog_AutoCleanup
                          // ------------------------------------------

// CREATORS
template <class TYPE>
bcec_ObjectCatalog_AutoCleanup<TYPE>::bcec_ObjectCatalog_AutoCleanup(
                                             bcec_ObjectCatalog<TYPE> *catalog)
: d_catalog_p(catalog)
, d_node_p(0)
, d_deallocateFlag(false)
{
}

template <class TYPE>
bcec_ObjectCatalog_AutoCleanup<TYPE>::~bcec_ObjectCatalog_AutoCleanup()
{
    if (d_catalog_p && d_node_p) {
        if (d_deallocateFlag) {
            // Return node to the pool.

            d_catalog_p->d_nodePool.deallocate(d_node_p);
        } else {
            // Return node to the catalog's free list.

            d_catalog_p->freeNode(d_node_p);
        }
    }
}

// MANIPULATORS
template <class TYPE>
void bcec_ObjectCatalog_AutoCleanup<TYPE>::manageNode(
        typename bcec_ObjectCatalog<TYPE>::Node *node,
        bool                                     deallocateFlag)
{
    d_node_p = node;
    d_deallocateFlag = deallocateFlag;
}

template <class TYPE>
void bcec_ObjectCatalog_AutoCleanup<TYPE>::releaseNode()
{
    d_node_p = 0;
}

template <class TYPE>
void bcec_ObjectCatalog_AutoCleanup<TYPE>::release()
{
    d_catalog_p = 0;
    d_node_p = 0;
}

                          // ------------------------
                          // class bcec_ObjectCatalog
                          // ------------------------

// PRIVATE MANIPULATORS
template <class TYPE>
inline
void bcec_ObjectCatalog<TYPE>::freeNode(
                                 typename bcec_ObjectCatalog<TYPE>::Node *node)
{
    node->d_handle += BCEC_GENERATION_INC;
    node->d_handle &= ~BCEC_BUSY_INDICATOR;

    node->d_next_p   = d_nextFreeNode_p;
    d_nextFreeNode_p = node;
}

// PRIVATE ACCESSORS
template <class TYPE>
inline
typename bcec_ObjectCatalog<TYPE>::Node *
bcec_ObjectCatalog<TYPE>::findNode(int handle) const
{
    int index = handle & BCEC_INDEX_MASK;
    // if (d_nodes.size() < index || !(handle & BCEC_BUSY_INDICATOR)) return 0;

    if (0 > index                    ||
        index >= (int)d_nodes.size() ||
        !(handle & BCEC_BUSY_INDICATOR)) {
        return 0;
    }

    Node *node = d_nodes[index];

    return (node->d_handle == handle) ? node : 0;
}

// CREATORS
template <class TYPE>
inline
bcec_ObjectCatalog<TYPE>::bcec_ObjectCatalog(bslma_Allocator *allocator)
: d_nodes(allocator)
, d_nodePool(sizeof(Node), allocator)
, d_nextFreeNode_p(0)
, d_length(0)
{
}

template <class TYPE>
inline
bcec_ObjectCatalog<TYPE>::~bcec_ObjectCatalog()
{
    removeAll();
}

// MANIPULATORS
template <class TYPE>
int bcec_ObjectCatalog<TYPE>::add(const TYPE& object)
{
    int handle;
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);
    bcec_ObjectCatalog_AutoCleanup<TYPE> proctor(this);
    Node *node;

    if (d_nextFreeNode_p) {
        node = d_nextFreeNode_p;
        d_nextFreeNode_p = node->d_next_p;

        proctor.manageNode(node, false);
        // Destruction of this proctor will put node back onto the free list.
    } else {
        // If 'd_nodes' grows as big as the flags used to indicate BUSY and
        // generations, then the handle will be all mixed up!

        BSLS_ASSERT_SAFE(d_nodes.size() < BCEC_BUSY_INDICATOR);

        node = (Node *)d_nodePool.allocate();
        proctor.manageNode(node, true);
        // Destruction of this proctor will deallocate node.

        d_nodes.push_back(node);
        node->d_handle = static_cast<int>(d_nodes.size()) - 1;
        proctor.manageNode(node, false);
        // Destruction of this proctor will put node back onto the free list,
        // which is now OK since the 'push_back' succeeded without throwing.
    }

    node->d_handle |= BCEC_BUSY_INDICATOR;
    handle = node->d_handle;

    // We need to use the copyConstruct logic to pass the allocator through.
    bslalg_ScalarPrimitives::copyConstruct(
            (TYPE *)(void *)&node->d_value, object,
            d_nodes.get_allocator().mechanism());

    // If the copy constructor throws, the proctor will properly put the node
    // back onto the free list.  Otherwise, the proctor should do nothing.
    proctor.release();

    ++d_length;
    return handle;
}

template <class TYPE>
inline
int bcec_ObjectCatalog<TYPE>::remove(int handle, TYPE *valueBuffer)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    Node *node = findNode(handle);

    if (!node) {
        return -1;
    }

    if (valueBuffer) {
        *valueBuffer = *((TYPE *)(void *)&node->d_value);
    }

    ((TYPE *)(void *)&node->d_value)->~TYPE();
    freeNode(node);

    --d_length;
    return 0;
}

template <class TYPE>
void bcec_ObjectCatalog<TYPE>::removeAll(bsl::vector<TYPE> *buffer)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    for (typename bsl::vector<Node*>::iterator it = d_nodes.begin();
         it != d_nodes.end();++it) {
        if ((*it)->d_handle & BCEC_BUSY_INDICATOR) {
            if (buffer) {
                buffer->push_back(*((TYPE *)(void *)&(*it)->d_value));
            }
            ((TYPE *)(void *)(*it)->d_value)->~TYPE();
        }
    }
    // Even though we get rid of the container of 'Node*' without returning the
    // nodes to the pool prior, the release of the pool immediately after will
    // properly (and efficiently) dispose of those nodes without leaking
    // memory.
    d_nodes.clear();
    d_nodePool.release();
    d_nextFreeNode_p = 0;
    d_length = 0;
}

template <class TYPE>
int bcec_ObjectCatalog<TYPE>::replace(int handle, const TYPE& newObject)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    Node *node = findNode(handle);

    if (!node) {
        return -1;
    }

    ((TYPE *)(void *)&node->d_value)->~TYPE();
    // We need to use the copyConstruct logic to pass the allocator through.
    bslalg_ScalarPrimitives::copyConstruct(
            (TYPE *)(void *)&node->d_value, newObject,
            d_nodes.get_allocator().mechanism());

    return 0;
}

// ACCESSORS
template <class TYPE>
inline
int bcec_ObjectCatalog<TYPE>::find(int handle, TYPE *valueBuffer) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);

    Node *node = findNode(handle);

    if (!node) {
        return -1;
    }

    if (valueBuffer) {
        *valueBuffer = *((TYPE *)(void *)&node->d_value);
    }
    return 0;
}

template <class TYPE>
inline
int bcec_ObjectCatalog<TYPE>::length() const
{
    return d_length;
}

template <class TYPE>
void bcec_ObjectCatalog<TYPE>::verifyState() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);

    BSLS_ASSERT_SAFE((int)d_nodes.size() >= d_length);
    BSLS_ASSERT_SAFE(d_length >= 0);

    int nBusy = 0;
    for (int i = 0; i < (int)d_nodes.size(); i++) {
        BSLS_ASSERT_SAFE((d_nodes[i]->d_handle & BCEC_INDEX_MASK) ==
                                                                  (unsigned)i);
        if (d_nodes[i]->d_handle & BCEC_BUSY_INDICATOR) {
            nBusy++;
        }
    }
    BSLS_ASSERT_SAFE(d_length == nBusy);

    int nFree = 0;
    for (Node *p = d_nextFreeNode_p; p; p = p->d_next_p) {
        nFree++;
    }

    BSLS_ASSERT_SAFE(nFree+nBusy == (int)d_nodes.size());
}

                             // ----------------------
                             // bcec_ObjectCatalogIter
                             // ----------------------

// CREATORS
template <class TYPE>
inline
bcec_ObjectCatalogIter<TYPE>::bcec_ObjectCatalogIter(
                                       const bcec_ObjectCatalog<TYPE>& catalog)
: d_catalog_p(&catalog)
, d_index(-1)
{
    d_catalog_p->d_lock.lockRead();
    operator++();
}

template <class TYPE>
inline
bcec_ObjectCatalogIter<TYPE>::~bcec_ObjectCatalogIter()
{
    d_catalog_p->d_lock.unlock();
}

// MANIPULATORS
template <class TYPE>
void bcec_ObjectCatalogIter<TYPE>::operator++()
{
    ++d_index;
    while ((unsigned)d_index < d_catalog_p->d_nodes.size() &&
          !(d_catalog_p->d_nodes[d_index]->d_handle &
              bcec_ObjectCatalog<TYPE>::BCEC_BUSY_INDICATOR)) {
        ++d_index;
    }
}

// ACCESSORS
template <class TYPE>
inline
bcec_ObjectCatalogIter<TYPE>::operator const void *() const
{
    return (void *)(((unsigned)d_index < d_catalog_p->d_nodes.size()) ? this
                                                                      : 0);
}

template <class TYPE>
inline
bsl::pair<int, TYPE> bcec_ObjectCatalogIter<TYPE>::operator()() const
{
    return bsl::pair<int, TYPE>(
                    d_catalog_p->d_nodes[d_index]->d_handle,
                    *(TYPE *)(void *)(d_catalog_p->d_nodes[d_index]->d_value));
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
