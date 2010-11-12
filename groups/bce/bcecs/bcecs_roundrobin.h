// bcecs_roundrobin.h                                                 -*-C++-*-
#ifndef INCLUDED_BCECS_ROUNDROBIN
#define INCLUDED_BCECS_ROUNDROBIN

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe, templatized, round robin container.
//
//@CLASSES:
//     bcecs_RoundRobin: Thread-safe, templatized, round robin container.
//
//@SEE_ALSO:
//
//@AUTHOR: Ujjwal Bhoota (ubhoota)
//
//@DESCRIPTION:
// This component provides 'bcecs_RoundRobin', a templatized container,
// which furnishes the contained objects in a round robin fashion.  Objects
// are added and removed *by value*.  The 'next' method returns the objects
// in a round robin order and this order relative to the order to
// insertion is undefined.  This component can be used in situations where
// things (for example requests to a collection of servers, in order to
// provide load balancing (see the usage example)) need to be scheduled in
// a round robin manner.
//
///Usage
///-----
// Following example demonstrates a client accepting requests from the
// user and displaying the response after getting it from one among five
// servers.  In order to provide the load balancing, the client forward the
// requests to the servers in a round robin fashion.
//..
//    typedef int ServerID;    // dummy server type
//    typedef int Query;       // dummy query type
//    typedef int QueryResult; // dummy query result type
//
//    Query getClientRequest()
//        // Return a query requested from the user.  Return 0 when no more
//        // queries are left.
//    {
//    ...
//    }
//
//    QueryResult executeQuery(ServerID server, Query query)
//        // Send the specified 'query' to the specified 'server' and
//        // return the query result obtained from the server.
//    {
//    ...
//    }
//
//
//    void display(QueryResult queryResult)
//        // Display the specified 'queryResult'.
//    {
//    ...
//    }
//
//    int main()
//    {
//        bcecs_RoundRobin<ServerID> roundRobin;
//
//        ServerID server1(1);
//        ServerID server2(2);
//        ServerID server3(3);
//        ServerID server4(4);
//        ServerID server5(5);
//
//        roundRobin.add(server1);
//        roundRobin.add(server2);
//        roundRobin.add(server3);
//        roundRobin.add(server4);
//        roundRobin.add(server5);
//
//        Query query;
//        while((query=getClientRequest()) != 0) {
//            ServerID server;
//            roundRobin.next(&server);
//            QueryResult result = executeQuery(server, query);
//            display(result);
//        }
//    }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

namespace BloombergLP {

                          // ======================
                          // class bcecs_RoundRobin
                          // ======================
template <class TYPE>
class bcecs_RoundRobin {
    // This class implements a thread safe templatized container which
    // furnishes the contained objects in a round robin order.  Objects are
    // added and removed *by value*.  The 'add' method enforces the
    // uniqness.  The 'next' method returns the objects in a round robin
    // order and this order relative to the order to insertion is undefined.

    struct Node {
        TYPE d_object;
        Node *d_next_p;

        Node(const TYPE &obj, Node *next): d_object(obj), d_next_p(next)
        {
        }
    };

    mutable bcemt_Mutex d_lock;
    bdema_Pool          d_pool;
    Node               *d_currentPosition_p;
    volatile int        d_numObjects;

  private:
    // NOT IMPLEMENTED
    bcecs_RoundRobin(const bcecs_RoundRobin<TYPE>& original);
    bcecs_RoundRobin& operator=(const bcecs_RoundRobin<TYPE>& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcecs_RoundRobin,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bcecs_RoundRobin(bslma_Allocator *allocator=0);
        // Create a round robin.  Use the optionally specified
        // 'allocator' to supply memory.  If 'allocator' is 0, the
        // currently installed default allocator is used.

    ~bcecs_RoundRobin();
        // Destroy this round robin.  Destroy all the objects contained and
        // release all associated memory.

    // MANIPULATORS
    int add(const TYPE& object);
        // Add the specified 'object' to this round robin and return 1 on
        // successful addition.  If an object with the same value is
        // already present, nothing is added (i.e., uniqness is enforced)
        // and 0 is returned.

    int next(TYPE *result);
        // Load into the specified 'result' the value of the next object in
        // the round robin order.  Return 1 on success.  If no object is
        // currently contained in this round robin, '*valueBuffer' is not
        // modified and 0 is returned (to indicate failure).  Note that the
        // objects are returned in the round robin order and this order
        // relative to the order of insertion is undefined.

    int remove(const TYPE& object);
        // Remove the specified 'object'.  Return 1 on success, and 0 if no
        // such object is present in the round robin.

    void removeAll();
        // Remove all objects that are currently contained in this round robin.

    // ACCESSORS
    int numObjects() const;
        // Return a "snapshot" of the number of objects currently contained in
        // this round robin.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

                          // ----------------------
                          // class bcecs_RoundRobin
                          // ----------------------

// PRIVATE

// CREATORS
template <class TYPE>
inline bcecs_RoundRobin<TYPE>::bcecs_RoundRobin(bslma_Allocator *allocator)
: d_pool(sizeof(Node), allocator)
, d_currentPosition_p(0)
, d_numObjects(0)
{
}

template <class TYPE>
inline bcecs_RoundRobin<TYPE>::~bcecs_RoundRobin()
{
    removeAll();
}

// MANIPULATORS
template <class TYPE>
int bcecs_RoundRobin<TYPE>::add(const TYPE& object)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    if (d_currentPosition_p == NULL) {
        void *addr = d_pool.allocate();
        d_currentPosition_p = new (addr) Node(object, (Node*) addr);
        ++d_numObjects;
        return 1;
    }
    Node *p = d_currentPosition_p;
    while (p->d_next_p->d_object != object) {
        p = p->d_next_p;
        if (p == d_currentPosition_p) break;
    }
    if (p->d_next_p->d_object == object) {
        return 0;
    }

    d_currentPosition_p->d_next_p =
        new (d_pool.allocate()) Node(object, d_currentPosition_p->d_next_p);
    ++d_numObjects;
    return 1;
}

template <class TYPE>
inline int bcecs_RoundRobin<TYPE>::next(TYPE *result)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    if(d_currentPosition_p == NULL) {
        return 0;
    }

    *result = d_currentPosition_p->d_object;
    d_currentPosition_p = d_currentPosition_p->d_next_p;
    return 1;
}

template <class TYPE>
int bcecs_RoundRobin<TYPE>::remove(const TYPE& object)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    if (d_currentPosition_p == NULL) {
        return 0;
    }

    Node *p = d_currentPosition_p;
    while(p->d_next_p->d_object != object) {
        p = p->d_next_p;
        if (p == d_currentPosition_p) break;
    }
    if (p->d_next_p->d_object != object) {
        return 0;
    }

    Node *del = p->d_next_p;          // 'del' is the node to be removed
    p->d_next_p = del->d_next_p;

    if (del == d_currentPosition_p) { // 'd_currentPosition_p' is the node
                                      // to be removed
        d_currentPosition_p = d_currentPosition_p->d_next_p;
    }

    if (del == p)   d_currentPosition_p = NULL;    // only 1 node

    del->~Node();
    d_pool.deallocate(del);
    --d_numObjects;
    return 1;
}

template <class TYPE>
void bcecs_RoundRobin<TYPE>::removeAll()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);

    if (d_currentPosition_p == NULL) {
        return;
    }
    // first flatten the list
    Node *p = d_currentPosition_p->d_next_p;
    d_currentPosition_p->d_next_p = NULL;

    // now traverse
    while(p != NULL) {
        Node *next = p->d_next_p;
        p->~Node();
        d_pool.deallocate(p);
        p = next;
    }

    d_currentPosition_p = NULL;
    d_numObjects = 0;
}

// ACCESSORS
template <class TYPE>
inline int bcecs_RoundRobin<TYPE>::numObjects() const
{
    return d_numObjects;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
