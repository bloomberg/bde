// bcec_objectpool.h                                                  -*-C++-*-
#ifndef INCLUDED_BCEC_OBJECTPOOL
#define INCLUDED_BCEC_OBJECTPOOL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe object pool.
//
//@CLASSES:
//          bcec_ObjectPool: thread-enabled container of managed objects
//  bcec_ObjectPoolFunctors: namespace for resetter and creator implementations
//
//@SEE_ALSO:
//
//@AUTHOR: Ujjwal Bhoota (ubhoota), David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides a generic thread-safe pool of
// objects using the acquire-release idiom.  An object pool provides two main
// methods: 'getObject', which returns an object from the pool, and
// 'releaseObject', which returns an object to the pool for further reuse
// (thus avoiding the overhead of object construction and destruction).  A
// major requirement of using the object pool is that any call to 'getObject'
// can be satisfied by any object in the pool.
//
///Object construction and destruction
///-----------------------------------
// The object pool owns the memory required to store the pooled objects,
// and manages the construction, resetting, and destruction of objects.
// The user may supply functors to create objects and to reset them to a
// valid state for their return to the pool.  Alternatively, this component
// supplies reasonable defaults.  Upon destruction, the object pool deallocates
// all memory associated with the objects in the pool.
//
// The object pool also implements the 'bcema_Factory' protocol for TYPE.
// Its 'createObject' and 'deleteObject' methods are provided *only* for this
// purpose and should not be invoked directly (they are just synonyms for
// 'getObject' and 'releaseObject', respectively).  The pool can thus be used
// anywhere a 'bcema_Factory' (or, therefore, a 'bcema_Deleter') is expected.
//
///Integrating with 'bdema_ManagedPtr' and 'bcema_SharedPtr'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bcec_ObjectPool' is designed to work with both managed and shared pointer
// types.  Because 'bcec_ObjectPool' provides a 'deleteObject' method, it can
// serve as a factory of both 'bdema_ManagedPtr' and 'bcema_SharedPtr' objects.
//
// For example, to create a managed pointer from an object pool of
// 'bsl::string' objects:
//..
//  bcec_ObjectPool<bsl::string> pool;
//  bdema_ManagedPtr<bsl::string> managedPtr(pool.getObject(), &pool);
//..
// To create a shared pointer (using the same object pool):
//..
//  bslma_Allocator *allocator = bslma_Default::allocator();
//  bcema_SharedPtr<bsl::string> sharedPtr(pool.getObject(), &pool, allocator);
//..
// Note that an allocator is a *required* argument to the 'bcema_SharedPtr'
// constructor used here, and the provided allocator is used to supply memory
// for the internal representation of the pointer, and not to allocate memory
// for the object itself.
//
///Creator and Resetter Template Contract
///--------------------------------------
// 'bcec_ObjectPool' is templated on two types 'CREATOR' and 'RESETTER' in
// addition to the underlying object 'TYPE'.  Objects of these types may be
// provided at construction.  The namespace 'bcec_ObjectPoolFunctors' provides
// several commonly used implementations.  The creator will be invoked as:
// 'void(*)(void*, bslma_Allocator*)'.  The resetter will be invoked as:
// 'void(*)(TYPE*);.  The creator functor is called to construct a new object
// of the parameterized 'TYPE' when the pool must be expanded (and thus it
// typically invokes placement 'new' and passes its allocator argument to
// the constructor of 'TYPE').  The resetter functor is called before each
// object is returned to the pool, and is required to put the object into a
// state such that it is ready to be reused.  The defaults for these types are
// as follows:
//..
//    CREATOR  = bcec_ObjectPoolFunctors::DefaultCreator
//    RESETTER = bcec_ObjectPoolFunctors::Nil
//..
// 'bcec_ObjectPoolFunctors::Nil' is a no-op; it is only suitable if the
// objects stored in the pool are *always* in a valid state to be reused.
// Otherwise another kind of 'RESETTER' should be provided.  In
// 'bcec_ObjectPoolFunctors', the classes 'Clear', 'RemoveAll', and 'Reset'
// are all acceptable types for 'RESETTER'.  Since these functor types are
// fully inlined, it is generally most efficient to define 'reset'
// (or 'clear' or 'removeAll') in the underlying 'TYPE' and allow the
// functor to call that method.  The 'CREATOR' functor defaults to an object
// that invokes the default constructor with placement new, passing the
// allocator argument if the type traits of the object indicate it uses an
// allocator (see 'bdealg_typetraits').  If a custom creator functor or a
// custom 'CREATOR' type is specified, it is the user's responsibility to
// ensure that it correctly passes its allocator argument to the constructor
// of 'TYPE' if 'TYPE' takes an allocator.
//
///Exception safety
///----------------
// There are two potential sources of exceptions in this component: memory
// allocation and object construction.  The object pool is exception-neutral
// with full guarantee of rollback for the following methods:  if an exception
// is thrown in 'getObject', 'reserveCapacity', or 'increaseCapacity', then the
// pool is in a valid unmodified state (i.e., identical to its state prior to
// the call to 'getObject').  No other method of 'bcec_ObjectPool' can throw.
//
///Pool replenishment policy
///-------------------------
// The 'growBy' parameter can be specified in the pool's constructor
// to instruct the pool how to increase its capacity each time the pool is
// depleted.  If 'growBy' is positive, the pool always replenishes itself with
// enough objects to satisfy at least 'growBy' object requests before
// the next replenishment.  If 'growBy' is negative, the pool will increase
// its capacity geometrically until it exceeds the internal maximum (which is
// implementation-defined), and after that it will be replenished with
// constant number of objects.  If 'growBy' is not specified, it defaults
// to -1 (i.e., geometric increase beginning at 1).
//
///Usage
///-----
// In this example, we simulate a database server accepting queries from
// clients and executing each query in a separate thread.  Client requests are
// simulated by function 'getClientQuery' which returns a query to be
// executed.  The class 'Query' encapsulates a database query and
// 'queryFactory' is an object of a query factory class 'QueryFactory'.
//..
//    enum {
//        CONNECTION_OPEN_TIME  = 100,    // (simulated) time to open
//                                        //  a connection (in microseconds)
//
//        CONNECTION_CLOSE_TIME = 8,     // (simulated) time to close
//                                       //  a connection (in microseconds)
//
//        QUERY_EXECUTION_TIME  = 4      // (simulated) time to execute
//                                       //  a query (in microseconds)
//    };
//
//    class my_DatabaseConnection
//        // This class simulates a database connection.
//    {
//      bslma_Allocator *d_allocator_p; // held
//      int              d_connectionId;
//      public:
//        BSLALG_DECLARE_NESTED_TRAITS(my_DatabaseConnection,
//                                     bslalg_TypeTraitUsesBslmaAllocator);
//
//        my_DatabaseConnection(int connectionId = 0,
//                              bslma_Allocator *basicAllocator = 0)
//          : d_allocator_p(bslma_Default::allocator(basicAllocator))
//          , d_connectionId(connectionId)
//        {
//            bcemt_ThreadUtil::microSleep(CONNECTION_OPEN_TIME);
//        }
//
//        ~my_DatabaseConnection()
//        {
//            bcemt_ThreadUtil::microSleep(CONNECTION_CLOSE_TIME);
//        }
//
//        void executeQuery(Query *query)
//        {
//            bcemt_ThreadUtil::microSleep(QUERY_EXECUTION_TIME);
//        }
//    };
//..
// The server runs several threads which, on each iteration, obtain a new
// client request from the query factory, and process it, until the desired
// total number of requests is achieved.
//..
//    void serverThread(bces_AtomicInt *queries, int max,
//                      void(*queryHandler)(Query*))
//    {
//        while (++(*queries) <= max) {
//            Query *query = queryFactory->createQuery();
//            queryHandler(query);
//        }
//    }
//..
// The main thread starts and joins these threads:
//..
//    enum {
//       NUM_THREADS = 8,
//       NUM_QUERIES = 10000
//    };
//
//    bces_AtomicInt numQueries = 0;
//    bcemt_ThreadGroup tg;
//
//    tg.addThreads(bdef_BindUtil::bind(&serverThread, &numQueries,
//                                      NUM_QUERIES, &queryHandler1),
//                  NUM_THREADS);
//    tg.joinAll();
//..
// We first give an implementation that does not uses the object
// pool.  Later we will give an implementation using an
// object pool to manage the database connections.  We also keep track of
// total response time for each case.
// When object pool is *not* used, each thread, in order to execute a
// query, creates a *new* database connection, calls its
// 'executeQuery' method to execute the query and finally closes the
// connection.
//..
//     void queryHandler1(Query *query)
//         // Handle the specified 'query' without using an objectpool.
//     {
//         bsls_PlatformUtil::Int64 t1 = bdes_TimeUtil::getTimer();
//         my_DatabaseConnection connection;
//         connection.executeQuery(query);
//         bsls_PlatformUtil::Int64 t2 = bdes_TimeUtil::getTimer();
//
//         totalResponseTime1 += t2 - t1;
//
//         queryFactory->destroyQuery(query);
//             // 'connection' is implicitly destroyed on function return.
//     }
//..
// In above strategy, clients always incur the delay associated with opening
// and closing a database connection.
// Now we show an implementation that will use object pool to *pool* the
// database connections.
//
///Object pool creation and functor argument
///- - - - - - - - - - - - - - - - - - - - -
// In order to create an object pool, we may specify, at construction
// time, a functor encapsulating object creation.  The pool invokes this
// this functor to create an object in a memory location supplied by the
// allocator specified at construction and owned by the pool.
// By default, the creator invokes the default constructor of the underlying
// type, passing the pool's allocator if the type uses the bslma_Allocator
// protocol to supply memory (as specified by the "Uses Bdema Allocator"
// trait, see bdealg_typetraits).  If this behavior is not sufficient, we
// can supply our own functor for type creation.
//
///Creating an object pool that constructs default objects
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// When the default constructor of our type is sufficient, whether or not
// that type uses bslma_Allocator, we can simply use the default behavior
// of 'bcec_ObjectPool':
//..
//    bcec_ObjectPool<my_DatabaseConnection> pool;
//..
///Creating an object pool that constructs non-default objects
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// In this example, if we decide that connection IDs must be supplied to
// objects allocated from the pool, we must define a function which invokes
// placement new appropriately.  When using a custom creator functor, it is
// the responsibility of client code to pass the pool's allocator (supplied as
// the second argument to the functor) to the new object if it uses
// bslma_Allocator.
//..
//    void createConnection(void *arena, bslma_Allocator *alloc, int id)
//    {
//       new (arena) my_DatabaseConnection(id, alloc);
//    }
//..
// then...
//..
//    int myId = 100;
//    bcec_ObjectPool<my_DatabaseConnection> pool(
//                             bdef_BindUtil::bind(&createConnection,
//                                                 bdef_PlaceHolders::_1,
//                                                 bdef_PlaceHolders::_2,
//                                                 myId));
//..
// Whichever creator we choose, the modified server looks like
//..
//    connectionPool = &pool;
//
//    for (int i = 0; i < NUM_QUERIES; ++i) {
//        my_Query *query = getClientQuery();
//        bcemt_ThreadUtil::create(&threads[i], queryHandler2, (void *)query);
//    }
//    for (int i = 0; i < NUM_QUERIES; ++i) {
//        bcemt_ThreadUtil::join(threads[i]);
//    }
//..
///Modified 'queryHandler'
///- - - - - - - - - - - -
// Now each thread, instead of creating a new connection, gets a
// connection from the object pool.  After using the connection,
// the client returns it back to the pool for further reuse.  The
// modified 'queryHandler' is following.
//..
//    bcec_ObjectPool<my_DatabaseConnection> *connectionPool;
//    void queryHandler2(Query *query)
//        // Process the specified 'query'.
//    {
//        bsls_PlatformUtil::Int64 t1 = bdes_TimeUtil::getTimer();
//        my_DatabaseConnection *connection = connectionPool->getObject();
//        connection->executeQuery(query);
//        bsls_PlatformUtil::Int64 t2 = bdes_TimeUtil::getTimer();
//
//        totalResponseTime2 += t2 - t1;
//
//        connectionPool->releaseObject(connection);
//        queryFactory->destroyQuery(query);
//    }
//..
// The total response time for each strategy is:
//..
// totalResponseTime1 = 199970775520
// totalResponseTime2 = 100354490480
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_FACTORY
#include <bcema_factory.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTIL
#include <bces_atomicutil.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDEMA_INFREQUENTDELETEBLOCKLIST
#include <bdema_infrequentdeleteblocklist.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTFROMTYPE
#include <bsls_alignmentfromtype.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

namespace BloombergLP {

                       // ==============================
                       // struct bcec_ObjectPoolFunctors
                       // ==============================

struct bcec_ObjectPoolFunctors {
    // This struct provides several functors that are suitable 'RESETTER'
    // parameter types for 'bcec_ObjectPool'.  It also provides a 'typedef'
    // that specifies the default 'CREATOR' parameter type for
    // 'bcec_ObjectPool'.

    // PUBLIC TYPES
    typedef bdef_Function<void(*)(void*, bslma_Allocator*)> DefaultCreator;
        // The default 'CREATOR' parameter type for the 'bcec_ObjectPool' class
        // template.

    template <class TYPE>
    class Nil {
        // This fully-inlined class, suitable as the 'RESETTER' parameter
        // type for 'bcec_ObjectPool', is a functor taking a pointer to the
        // parameterized 'TYPE' argument, and can be invoked as:
        // 'void(*)(TYPE*)'.  It does nothing.

      public:
        // Use compiler-generated constructors.

        void operator()(TYPE *object) const;
            // Inlined no-op function.
    };

    template <class TYPE>
    class Reset {
        // This fully-inlined class, suitable as the 'RESETTER' parameter type
        // for 'bcec_ObjectPool', is a functor taking a pointer to the
        // parameterized 'TYPE' argument, and can be invoked as:
        // 'void(*)(TYPE*)'.  It calls 'reset' upon the provided object.

      public:
        // Use compiler-generated constructors.

        void operator()(TYPE *object) const;
            // Inlined call to 'object->reset()'.
    };

    template <class TYPE>
    class Clear {
        // This fully-inlined class, suitable as the 'RESETTER' parameter type
        // for 'bcec_ObjectPool', is a functor taking a pointer to the
        // parameterized 'TYPE' argument, and can be invoked as:
        // 'void(*)(TYPE*)'.  It calls 'clear' upon the provided object.

      public:
        // Use compiler-generated constructors.

        void operator()(TYPE *object) const;
            // Inlined call to 'object->clear()'.
    };

    template <class TYPE>
    class RemoveAll {
        // This fully-inlined class, suitable as the 'RESETTER' parameter type
        // for 'bcec_ObjectPool', is a functor taking a pointer to the
        // parameterized 'TYPE' argument, and can be invoked as:
        // 'void(*)(TYPE*)'.  It calls 'removeAll' upon the provided object.

      public:
        // Use compiler-generated constructors.

        void operator()(TYPE *object) const;
            // Inlined call to 'object->removeAll()'.
    };

};

                   // ======================================
                   // class bcec_ObjectPool_CreatorConverter
                   // ======================================

template <class TYPE, class OTHERTYPE>
class bcec_ObjectPool_CreatorConverter {
    // The purpose of this private class is to avoid ambiguity between
    // different template instantiations of 'bdef_Function' accepted by the
    // constructors of 'bcec_ObjectPool'.  It should not be used directly.
    //
    // This version of the converter ignores the parameterized 'OTHERTYPE'.  It
    // requires a reference to an object of the parameterized 'TYPE' in its
    // constructor.

    // DATA
    const TYPE& d_creator;

  public:
    // CREATORS
    bcec_ObjectPool_CreatorConverter(const TYPE& creator);

    // ACCESSORS
    const TYPE& creator() const;
};

template <>
class bcec_ObjectPool_CreatorConverter<bcec_ObjectPoolFunctors::DefaultCreator,
                                       bdef_Function<void(*)(void*)> > {
    // The purpose of this private class is to avoid ambiguity between
    // different template instantiations of bdef_Function accepted by the
    // constructors of 'bcec_ObjectPool'.   It should not be used directly.
    //
    // This version of the converter is a full template specialization for the
    // case that the default creator type is used with a unary creator.  In
    // this case, 'creator' will return a binder (see 'bdef_bind') that adapts
    // the unary creator to a binary creator that discards the second
    // argument.  This usage is *DEPRECATED* and provided only for backward
    // compatibility.

    // DATA
    const bdef_Function<void(*)(void*)>& d_creator;

  public:
    // CREATORS
    bcec_ObjectPool_CreatorConverter(const bdef_Function<void(*)(void*)>&
                                                                     creator);

    // ACCESSORS
    bcec_ObjectPoolFunctors::DefaultCreator creator() const;
};

                     // ==================================
                     // class bcec_ObjectPool_CreatorProxy
                     // ==================================

template <class TYPE, class OTHERTYPE>
class bcec_ObjectPool_CreatorProxy {
    // This private class provides a default constructor which simply
    // invokes the default constructor of the parameterized 'TYPE';
    // the parameterized 'OTHERTYPE' is ignored.

    // DATA
    bsls_ObjectBuffer<TYPE> d_object;

    // NOT IMPLEMENTED
    bcec_ObjectPool_CreatorProxy& operator=(
                                 const bcec_ObjectPool_CreatorProxy&);
    bcec_ObjectPool_CreatorProxy(const bcec_ObjectPool_CreatorProxy&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcec_ObjectPool_CreatorProxy,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bcec_ObjectPool_CreatorProxy (bslma_Allocator *basicAllocator);
       // Create a new proxy and a new object of the parameterized 'TYPE'.
       // If 'TYPE' declares the "Uses Allocator" trait, 'basicAllocator' is
       // supplied to its default constructor; otherwise 'basicAllocator'
       // is ignored.

    bcec_ObjectPool_CreatorProxy(const TYPE&      other,
                                 bslma_Allocator *basicAllocator);
       // Create a new proxy and a new object constructed from the specified
       // 'other' object.  If 'TYPE' declares the "Uses Allocator" trait,
       // 'basicAllocator' is supplied to its copy constructor; otherwise
       // 'basicAllocator' is ignored.

    ~bcec_ObjectPool_CreatorProxy();
       // Destroy this proxy and the underlying object.

    // MANIPULATORS
    TYPE& object();
       // Return a reference to the modifiable object held by this proxy.
};

// SPECIALIZATIONS
template <class OTHERTYPE>
class bcec_ObjectPool_CreatorProxy<bcec_ObjectPoolFunctors::DefaultCreator,
                                   OTHERTYPE> {
    // This partial specialization of the 'bcec_ObjectPool_CreatorProxy' class
    // template provides a default constructor that creates a proxied
    // 'bdef_Function' object that invokes the default constructor of the
    // parameterized 'OTHERTYPE' with placement 'new'.

    // PRIVATE TYPES
    typedef bcec_ObjectPool_CreatorProxy<
                                       bcec_ObjectPoolFunctors::DefaultCreator,
                                       OTHERTYPE> MyType;

    // DATA
    bcec_ObjectPoolFunctors::DefaultCreator d_object;

    // NOT IMPLEMENTED
    bcec_ObjectPool_CreatorProxy(const bcec_ObjectPool_CreatorProxy&);
    bcec_ObjectPool_CreatorProxy& operator=(
                                 const bcec_ObjectPool_CreatorProxy&);

  private:
    // PRIVATE CLASS METHODS
    static void defaultConstruct(void *arena, bslma_Allocator *allocator);
      // Invoke 'bslalg_ScalarPrimitives::defaultConstruct(arena, allocator).
      // This method is necessary to select the correct overload for OTHERTYPE.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcec_ObjectPool_CreatorProxy,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bcec_ObjectPool_CreatorProxy(bslma_Allocator *basicAllocator);
       // Create a new proxy for a function object which invokes the default
       // constructor of OTHERTYPE.  Use 'basicAllocator' to supply memory.

    bcec_ObjectPool_CreatorProxy(
               const bcec_ObjectPoolFunctors::DefaultCreator&  rhs,
               bslma_Allocator                                *basicAllocator);
       // Create a proxy for a newly created function object constructed from
       // the specified 'rhs' creator.  Use a 'basicAllocator' to supply
       // memory.

    ~bcec_ObjectPool_CreatorProxy();
       // Destroy this proxy and the underlying object.

    // MANIPULATORS
    bcec_ObjectPoolFunctors::DefaultCreator& object();
       // Return a reference to the modifiable function object held by this
       // proxy.
};

                           // =====================
                           // class bcec_ObjectPool
                           // =====================

template <class TYPE,
          class CREATOR  = bcec_ObjectPoolFunctors::DefaultCreator,
          class RESETTER = bcec_ObjectPoolFunctors::Nil<TYPE>
         >
class bcec_ObjectPool : public bcema_Factory<TYPE> {
    // This class provides a thread-safe pool of reusable objects.  It also
    // implements the 'bcema_Factory' protocol: "creating" objects gets them
    // from the pool and "deleting" objects returns them to the pool.

    // PRIVATE TYPES
    typedef bcec_ObjectPool<TYPE, CREATOR, RESETTER> MyType;

    union ObjectNode {
        // This class stores a list pointer for linking the object nodes
        // together in the free objects list, in which case the reference count
        // is 0.  The list pointer is set to 0 when the reference count is not
        // 0, although that is not necessary.  A negative reference count
        // indicates a node which does not contain an initialized object (the
        // object was destroyed and the creator threw before node could be
        // released again).

        struct {
            ObjectNode           *d_next_p;
            bces_AtomicUtil::Int  d_refCount;
        } d_inUse;
        typename bsls_AlignmentFromType<TYPE>::Type d_dummy;
                                     // padding provider for proper alignment
                                     // of 'TYPE' objects
    };

    union BlockNode {
        // This class stores information about a block, which is organized as a
        // 'BlockNode' followed by 'd_numObjects' frames, each containing an
        // 'ObjectNode' followed by a 'TYPE', all of it suitably aligned.

        struct {
            BlockNode *d_next_p;
            int        d_numObjects; // number of objects in this block
        } d_inUse;
        typename bsls_AlignmentFromType<ObjectNode>::Type d_dummy;
                                     // padding provider for proper alignment
                                     // of 'ObjectNode'
    };

    class AutoCleanup {
        // This class, private to ObjectPool, implements a proctor for objects
        // created and stored into a temporary list of object nodes as in the
        // 'bcec_ObjectPool' type, used in the replenishing method called
        // from 'getObject', 'reserveCapacity' and 'increaseCapacity', to
        // ensure the exception-neutrality with full rollback guarantees of
        // the object pool.

      public:
        // TYPES
        typedef typename bcec_ObjectPool<TYPE, CREATOR, RESETTER>::ObjectNode
                                                                    ObjectNode;
        typedef typename bcec_ObjectPool<TYPE, CREATOR, RESETTER>::BlockNode
                                                                    BlockNode;

      private:
        // DATA
        BlockNode                       *d_block_p;      // held, not owned
        ObjectNode                      *d_head_p;       // held, not owned
        bdema_InfrequentDeleteBlockList *d_allocator_p;  // held, not owned
        int                              d_numNodes;

      public:
        // CREATORS
        AutoCleanup(BlockNode                       *block,
                    ObjectNode                      *head,
                    bdema_InfrequentDeleteBlockList *allocator,
                    int                              numNodes = 0);
            // Create a proctor for the list of the specified 'numNodes'
            // number of nodes with the specified 'head', using the
            // 'allocator' to deallocate the block starting at the specified
            // 'block' at destruction after all the objects in the list have
            // been destroyed, unless the 'release' method has been called.

        ~AutoCleanup();
            // Destroy this object, using the 'allocator' to deallocate
            // the block under management at destruction after all the objects
            // under management in the list have been destroyed, unless the
            // 'release' method has been called.

        // MANIPULATORS
        AutoCleanup& operator++();
            // Increment the number of nodes under management.  Nodes are added
            // sequentially in the list.

        void release();
            // Release the currently held list of nodes from management by this
            // proctor.
    };

    enum {
        // A block containing 'N' objects is organized with a single
        // 'BlockNode' followed by 'N' frames, each frame consisting of one
        // 'ObjectNode' and a 'TYPE', all suitably aligned.  The following
        // constants describe the size of a frame in terms of its multiple of
        // 'sizeof(ObjectNode)'.  We choose 'sizeof(ObjectNode)' as the basic
        // unit because it lets us do pointer arithmetic on 'ObjectNode *' more
        // easily.

        BCEC_ROUNDED_NUM_OBJECTS = (sizeof(TYPE) + sizeof(ObjectNode) - 1) /
                                                             sizeof(ObjectNode)
                                     // number of 'ObjectNode' needed to
                                     // contain an object of 'TYPE' (rounded
                                     // up to the next integer)

      , BCEC_NUM_OBJECTS_PER_FRAME = 1 + BCEC_ROUNDED_NUM_OBJECTS
                                     // number of 'ObjectNode' equivalent (in
                                     // size) to a frame (object node followed
                                     // by 'TYPE')

      , BCEC_MAX_NUM_OBJECTS_PER_FRAME = (INT_MAX / sizeof(ObjectNode) - 1) /
                                                     BCEC_NUM_OBJECTS_PER_FRAME
                                     // 'N' must be less than this
                                     // 'BCEC_MAX_NUM_OBJECTS_PER_FRAME' so
                                     // that the number of bytes in a block,
                                     // which is
                                     // '(1 + N * NUM_OBJECTS_PER_FRAME)' times
                                     // 'sizeof(ObjectNode)', does not overflow
    };

    enum {
        // Default configuration parameters.  Adjust these to tune up
        // performance of 'ObjectPool'.

        BCEC_GROW_FACTOR           =   2  // multiplicative factor to grow
                                          // capacity

      , BCEC_MAX_NUM_OBJECTS       = -32  // minimum 'd_numReplenishObjects'
                                          // value beyond which
                                          // 'd_numReplenishObjects' becomes
                                          // positive
    };

    // DATA
    bces_AtomicPointer<ObjectNode>
                           d_freeObjectsList;      // list of free objects

    bcec_ObjectPool_CreatorProxy<CREATOR, TYPE>
                           d_objectCreator;        // functor for object
                                                   // creation

    bslalg_ConstructorProxy<RESETTER>
                           d_objectResetter;       // functor to reset object

    int                    d_numReplenishObjects;  // pool growth behavior
                                                   // option (see above)

    bces_AtomicInt         d_numAvailableObjects;  // number of available
                                                   // objects

    bces_AtomicInt         d_numObjects;           // number of objects
                                                   // created by this pool

    BlockNode             *d_blockList;            // list of memory blocks

    bdema_InfrequentDeleteBlockList
                           d_blockAllocator;       // memory block supplier

    bslma_Allocator       *d_allocator_p;          // held, not owned

    bcemt_Mutex            d_mutex;                // pool replenishment
                                                   // serializer

    // NOT IMPLEMENTED
    bcec_ObjectPool(const MyType&, bslma_Allocator * = 0);
    bcec_ObjectPool& operator=(const MyType&);

    // FRIENDS
    friend class AutoCleanup;

  private:
    // PRIVATE MANIPULATORS
    void replenish();
        // Add additional objects to this pool based on the replenishment
        // policy specified by the 'growBy' argument at construction.

    void addObjects(int numObjects);
        // Create the specified 'numObjects' objects and attach them to
        // this object pool.

  public:
    // TYPES
    typedef RESETTER ResetterType;
    typedef CREATOR  CreatorType;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcec_ObjectPool,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bcec_ObjectPool(int              growBy = -1,
                    bslma_Allocator *basicAllocator = 0);
        // Create an object pool that invokes the default constructor of the
        // the parameterized 'TYPE' to construct objects.  When the pool is
        // depleted, it will increase its capacity according to the optionally
        // specified 'growBy' value.  If 'growBy' is positive, the pool
        // replenishes itself with at least 'growBy' new objects.  If 'growBy'
        // is negative, the amount of increase begins at '-growBy' and grows
        // geometrically up to an implementation-defined maximum.  When
        // objects are returned to the pool, the default value of RESETTER is
        // invoked with a pointer to the returned object to restore the
        // object to a reusable state.  The optionally specified
        // 'basicAllocator' is used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The
        // behavior is undefined unless '0 != growBy'.

    explicit
    bcec_ObjectPool(const CREATOR&   objectCreator,
                    int              growBy,
                    bslma_Allocator *basicAllocator = 0);
    explicit
    bcec_ObjectPool(const CREATOR&   objectCreator,
                    bslma_Allocator *basicAllocator = 0);
        // Create an object pool that uses the specified 'objectCreator'
        // (encapsulating the construction of objects) to create objects.
        // The client must ensure that 'objectCreator(buf, alloc)' creates an
        // object at memory location 'buf' using 'alloc' to supply memory.
        // When the pool is depleted, it will grow capacity according to the
        // optionally specified 'growBy' value.  If 'growBy' is positive, the
        // pool replenishes itself with at least 'growBy' new objects.  If
        // 'growBy' is negative, the amount of increase begins at '-growBy'
        // and grows geometrically up to an implementation-defined maximum.
        // When objects are returned to the pool, the default value of
        // RESETTER is invoked with a pointer to the returned object to
        // restore the object to a reusable state.  The optionally specified
        // 'basicAllocator' is used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The
        // behavior is undefined unless '0 != growBy'.

    bcec_ObjectPool(const CREATOR&   objectCreator,
                    const RESETTER&  objectResetter,
                    int              growBy = -1,
                    bslma_Allocator *basicAllocator = 0);
        // Create an object pool that uses the specified 'objectCreator'
        // (encapsulating the construction of objects) to create objects.
        // The client must ensure that 'objectCreator(buf, alloc)' creates an
        // object at memory location 'buf' using 'alloc' to supply memory.
        // When the pool is depleted, it will increase its capacity according
        // to the optionally specified 'growBy' value.  If 'growBy' is
        // positive, the pool replenishes itself with at least 'growBy' new
        // objects.  If 'growBy' is negative, the amount of increase begins
        // at '-growBy' and grows geometrically up to an
        // implementation-defined maximum.  When objects are returned to the
        // pool, the specified 'objectResetter' is invoked with a pointer to
        // the returned object to restore the object to a reusable state.  The
        // optionally specified 'basicAllocator' is used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless '0 != growBy'.

    template <class ANYPROTO>
    explicit
    bcec_ObjectPool(const bdef_Function<ANYPROTO>&   objectCreator,
                    int                              growBy,
                    bslma_Allocator                 *basicAllocator = 0);
    template <class ANYPROTO>
    explicit
    bcec_ObjectPool(const bdef_Function<ANYPROTO>&   objectCreator,
                    bslma_Allocator                 *basicAllocator = 0);
        // *DEPRECATED*  Use a creator of the parameterized 'CREATOR' type.

    virtual ~bcec_ObjectPool();
        // Destroy this object pool.  All objects created by this pool
        // are destroyed (even if some of them are still in use) and
        // memory is reclaimed.

    // MANIPULATORS
    TYPE *getObject();
        // Return an address of modifiable object from this object pool.  If
        // this pool is empty, it is replenished according to the strategy
        // specified at the pool construction (or an implementation-defined
        // strategy if none was provided).

    void increaseCapacity(int numObjects);
        // Create the specified 'numObjects' objects and add them to this
        // object pool.  The behavior is undefined unless 0 <= numObjects.

    void releaseObject(TYPE *object);
        // Return the specified 'object' back to this object pool.  Invoke
        // the RESETTER specified at construction, or the default RESETTER
        // if none was provided, before making the object available for reuse.
        // Note that if RESETTER is the default type
        // ('bcec_ObjectPoolFunctors::Nil'), then this method should be
        // invoked to return only *valid* objects because the pool uses
        // the released objects to satisfy further 'getObject' requests.
        // The behavior is undefined unless the 'object' was obtained from
        // this object pool's 'getObject' method.

    void reserveCapacity(int numObjects);
        // Create enough objects to satisfy requests for at least the
        // specified 'numObjects' objects before the next replenishment.
        // The behavior is undefined unless 0 <= numObjects.  Note that this
        // method is different from 'increaseCapacity' in that the number
        // of created objects may be less than 'numObjects'.

    // ACCESSORS
    int numAvailableObjects() const;
        // Return a *snapshot* of the number of objects available in this pool.

    int numObjects() const;
        // Return the (instantaneous) number of objects managed by
        // this pool.  This includes both the objects available in the pool
        // and the objects that were allocated from the pool and not yet
        // released.

    // 'bcema_Factory' INTERFACE
    virtual TYPE *createObject();
        // This concrete implementation of 'bcema_Factory::createObject'
        // invokes 'getObject'.  This should not be invoked directly.

    virtual void deleteObject(TYPE *object);
        // This concrete implementation of 'bcema_Factory::deleteObject'
        // invokes 'releaseObject' on the specified 'object', returning it
        // to this pool.  Note that this does *not* destroy the object and
        // should not be invoked directly.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                               // ---------------
                               // bcec_ObjectPool
                               // ---------------

// PRIVATE MANIPULATORS
template <class TYPE, class CREATOR, class RESETTER>
void bcec_ObjectPool<TYPE, CREATOR, RESETTER>::replenish()
{
    int numObjects = d_numReplenishObjects >= 0
                   ? d_numReplenishObjects
                   : -d_numReplenishObjects;
    addObjects(numObjects);

    // Grow pool capacity only if 'd_numReplenishObjects' is negative and
    // greater than 'BCEC_MAX_NUM_OBJECTS' (i.e., if the absolute value of
    // 'numObjects' is less than 'BCEC_MAX_NUM_OBJECTS').

    if (d_numReplenishObjects < 0) {
        if (d_numReplenishObjects > BCEC_MAX_NUM_OBJECTS) {
            d_numReplenishObjects *= BCEC_GROW_FACTOR;
        }
        else {
            d_numReplenishObjects = -d_numReplenishObjects;
        }
    }
}

template <class TYPE, class CREATOR, class RESETTER>
void bcec_ObjectPool<TYPE, CREATOR, RESETTER>::addObjects(int numObjects)
{
    // Allocate a single memory block to be used for creating block nodes,
    // object nodes, and objects.  Too large a value for 'numObjects' would
    // cause overflow in 'NUM_BYTES_PER_BLOCK' below.

    BSLS_ASSERT_SAFE(numObjects <= BCEC_MAX_NUM_OBJECTS_PER_FRAME);

    const int NUM_BYTES_PER_BLOCK = (int)(sizeof(BlockNode) +
                                          sizeof(ObjectNode) * numObjects *
                                                   BCEC_NUM_OBJECTS_PER_FRAME);

    BlockNode *start = (BlockNode *) d_blockAllocator.allocate(
                                                          NUM_BYTES_PER_BLOCK);

    // Create a block node

    start->d_inUse.d_next_p = d_blockList;
    start->d_inUse.d_numObjects = numObjects;

    // Create and link 'numObjects' objects

    ObjectNode *last = (ObjectNode *)(start + 1);
    AutoCleanup startGuard(start, last, &d_blockAllocator, 0);

    for (int i = 0; i < numObjects; ++i, ++startGuard) {
        last->d_inUse.d_next_p = last + BCEC_NUM_OBJECTS_PER_FRAME;
        bces_AtomicUtil::initInt(&last->d_inUse.d_refCount, 0);
        d_objectCreator.object()(last + 1, d_allocator_p);
        last += BCEC_NUM_OBJECTS_PER_FRAME;
    }
    last -= BCEC_NUM_OBJECTS_PER_FRAME;
    bces_AtomicUtil::initInt(&last->d_inUse.d_refCount, 0);

    // If all went well (no exceptions), attach it to 'd_blockList'

    startGuard.release();
    d_blockList = start;

    // Attach the created objects to 'd_freeObjectsList'

    ++start;
    ObjectNode *old;
    do {
        old = d_freeObjectsList;
        last->d_inUse.d_next_p = old;
    } while (old != d_freeObjectsList.testAndSwap(old, (ObjectNode *)start));

    d_numObjects.relaxedAdd(numObjects);
    d_numAvailableObjects.relaxedAdd(numObjects);
}

// CREATORS
template <class TYPE, class CREATOR, class RESETTER>
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::bcec_ObjectPool(
        int              growBy,
        bslma_Allocator *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(growBy)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::bcec_ObjectPool(
        const CREATOR&   objectCreator,
        int              growBy,
        bslma_Allocator *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(growBy)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
inline
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::bcec_ObjectPool(
        const CREATOR&   objectCreator,
        bslma_Allocator *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(-1)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
template <class ANYPROTO>
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::bcec_ObjectPool(
        const bdef_Function<ANYPROTO>& objectCreator,
        bslma_Allocator               *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(bcec_ObjectPool_CreatorConverter<CREATOR,
                                                   bdef_Function<ANYPROTO> >(
                                                     objectCreator).creator(),
                  basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(-1)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
template <class ANYPROTO>
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::bcec_ObjectPool(
        const bdef_Function<ANYPROTO>& objectCreator,
        int                            growBy,
        bslma_Allocator               *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(bcec_ObjectPool_CreatorConverter<CREATOR,
                                                   bdef_Function<ANYPROTO> >(
                                                     objectCreator).creator(),
                  basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(growBy)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::bcec_ObjectPool(
        const CREATOR&   objectCreator,
        const RESETTER&  objectResetter,
        int              growBy,
        bslma_Allocator *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(objectResetter, basicAllocator)
, d_numReplenishObjects(growBy)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::~bcec_ObjectPool()
{
    // Traverse the 'd_blockList', destroying all the objects associated
    // with each block, irrespective of whether their reference count is zero
    // or not.

    for (; d_blockList; d_blockList = d_blockList->d_inUse.d_next_p) {
        int numObjects = d_blockList->d_inUse.d_numObjects;
        ObjectNode *p = (ObjectNode *)(d_blockList + 1);
        for (; numObjects != 0; --numObjects) {
           ((TYPE *)(p + 1))->~TYPE();
            p += BCEC_NUM_OBJECTS_PER_FRAME;
      }
  }
}

// MANIPULATORS
template <class TYPE, class CREATOR, class RESETTER>
TYPE *bcec_ObjectPool<TYPE, CREATOR, RESETTER>::getObject()
{
    ObjectNode *p;
    do {
        p = d_freeObjectsList.relaxedLoad();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!p)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
            p = d_freeObjectsList;
            if (!p) {
                replenish();
                continue;
            }
        }
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                   2 != bces_AtomicUtil::addIntNv(&p->d_inUse.d_refCount,2))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            for (int i = 0; i < 3; ++i) {
                // To avoid unnecessary contention, assume that if we did
                // not get the first reference, then the other thread is
                // about to complete the pop.  Wait for a few cycles until
                // he does.  If he does not complete then go on and try to
                // acquire it ourselves.

                if (d_freeObjectsList != p) {
                    break;
                }
            }
        }

        // Force a dependent read of d_next_p to make sure that we're not
        // racing against a thread calling 'deallocate' for 'p' and that
        // checked the 'refCount' *before* we incremented it.  Either we can
        // observe the new free list value (== p) and because of the release
        // barrier, we can observe the new 'd_next_p' value (this relies on a
        // dependent load) or 'relaxedLoad' will the "old" (!= p) and the
        // condition will fail.
        // Note that 'h' is made volatile so that the compiler does not replace
        // the 'h->d_inUse' load with 'p->d_inUse' (and thus removing the data
        // dependency).
        // TBD to be completely thorough 'h->d_inUse.d_next_p' needs a load
        // dependent barrier (no-op on all current architectures though).

        const ObjectNode * volatile h = d_freeObjectsList.relaxedLoad();

        // Split the likely into 2 to workaround gcc 4.2 to gcc 4.4 bugs
        // documented in 'bsls_performancehint'.

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(h == p)
         && BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                  d_freeObjectsList.testAndSwap(p,h->d_inUse.d_next_p) == p)) {
            break;
        }

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        int refCount;
        for (;;) {
            refCount = bces_AtomicUtil::getInt(p->d_inUse.d_refCount);

            if (refCount & 1) {
                // The node is now free but not on the free list.
                // Try to take it.

                if (refCount == bces_AtomicUtil::testAndSwapInt(
                                                    &p->d_inUse.d_refCount,
                                                    refCount,
                                                    refCount^1)) {
                    // Taken!
                    p->d_inUse.d_next_p = 0;  // not strictly necessary
                    d_numAvailableObjects.relaxedAdd(-1);
                    return (TYPE*)(p + 1);

                }
            }
            else if (refCount == bces_AtomicUtil::testAndSwapInt(
                                                    &p->d_inUse.d_refCount,
                                                    refCount,
                                                    refCount - 2)) {
                break;
            }
        }
    } while(1);

    p->d_inUse.d_next_p = 0;  // not strictly necessary
    d_numAvailableObjects.relaxedAdd(-1);
    return (TYPE *)(p+1);
}

template <class TYPE, class CREATOR, class RESETTER>
void bcec_ObjectPool<TYPE, CREATOR, RESETTER>::increaseCapacity(int numObjects)
{
    if (numObjects > 0) {
       bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
       addObjects(numObjects);
    }
}

template <class TYPE, class CREATOR, class RESETTER>
void bcec_ObjectPool<TYPE, CREATOR, RESETTER>::releaseObject(TYPE *objPtr)
{
    ObjectNode *current = (ObjectNode *)(void *)objPtr - 1;
    d_objectResetter.object()(objPtr);

    int refCount = bces_AtomicUtil::getIntRelaxed(current->d_inUse.d_refCount);
    do {
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(2 == refCount)) {
            refCount =
                bces_AtomicUtil::testAndSwapInt(&current->d_inUse.d_refCount,
                                                2, 0);
            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(2 == refCount)) {
                break;
            }
        }

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        const int oldRefCount = refCount;
        refCount = bces_AtomicUtil::testAndSwapInt(
                                                &current->d_inUse.d_refCount,
                                                refCount,
                                                refCount - 1);
        if (oldRefCount == refCount) {
            // Someone else is still trying to pop this item.  Just let them
            // have it.

            d_numAvailableObjects.relaxedAdd(1);
            return;
        }

    } while(1);

    ObjectNode *head = d_freeObjectsList.relaxedLoad();
    for (;;) {
        current->d_inUse.d_next_p = head;
        ObjectNode * const oldHead = head;
        head = d_freeObjectsList.testAndSwap(head, current);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(oldHead == head)) {
            break;
        }
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    }
    d_numAvailableObjects.relaxedAdd(1);
}

template <class TYPE, class CREATOR, class RESETTER>
void bcec_ObjectPool<TYPE, CREATOR, RESETTER>::reserveCapacity(int numObjects)
{
   bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
   numObjects -= d_numObjects;
   if (numObjects > 0) {
      addObjects(numObjects);
   }
}

// ACCESSORS
template <class TYPE, class CREATOR, class RESETTER>
inline
int bcec_ObjectPool<TYPE, CREATOR, RESETTER>::numAvailableObjects() const
{
    return  d_numAvailableObjects;
}

template <class TYPE, class CREATOR, class RESETTER>
inline
int bcec_ObjectPool<TYPE, CREATOR, RESETTER>::numObjects() const
{
    return d_numObjects;
}

template <class TYPE, class CREATOR, class RESETTER>
inline
TYPE *bcec_ObjectPool<TYPE, CREATOR, RESETTER>::createObject()
{
    return getObject();
}

template <class TYPE, class CREATOR, class RESETTER>
inline
void bcec_ObjectPool<TYPE, CREATOR, RESETTER>::deleteObject(TYPE *object)
{
    releaseObject(object);
}

                      // --------------------------------
                      // bcec_ObjectPool_CreatorConverter
                      // --------------------------------

// CREATORS
template <class TYPE, class OTHERTYPE>
inline
bcec_ObjectPool_CreatorConverter<TYPE, OTHERTYPE>::
              bcec_ObjectPool_CreatorConverter(const TYPE& creator)
: d_creator(creator)
{
}

template <class TYPE, class OTHERTYPE>
inline
const TYPE& bcec_ObjectPool_CreatorConverter<TYPE, OTHERTYPE>::creator() const
{
    return d_creator;
}

inline
bcec_ObjectPool_CreatorConverter<bcec_ObjectPoolFunctors::DefaultCreator,
                                 bdef_Function<void(*)(void*)> >::
                            bcec_ObjectPool_CreatorConverter(
                                 const bdef_Function<void(*)(void*)> & creator)
: d_creator(creator)
{
}

                        // ----------------------------
                        // bcec_ObjectPool_CreatorProxy
                        // ----------------------------

// CLASS METHODS
template <class OTHERTYPE>
inline
void bcec_ObjectPool_CreatorProxy<bcec_ObjectPoolFunctors::DefaultCreator,
                                  OTHERTYPE>::defaultConstruct(
                                                  void            *arena,
                                                  bslma_Allocator *allocator)
{
    bslalg_ScalarPrimitives::defaultConstruct((OTHERTYPE*)arena, allocator);
}

// CREATORS
template <class TYPE, class OTHERTYPE>
inline
bcec_ObjectPool_CreatorProxy<TYPE, OTHERTYPE>::
            bcec_ObjectPool_CreatorProxy(bslma_Allocator *basicAllocator)
{
    bslalg_ScalarPrimitives::defaultConstruct(&d_object.object(),
                                              basicAllocator);
}

template <class TYPE, class OTHERTYPE>
inline
bcec_ObjectPool_CreatorProxy<TYPE, OTHERTYPE>::
            bcec_ObjectPool_CreatorProxy(const TYPE& rhs,
                                               bslma_Allocator *basicAllocator)
{
    bslalg_ScalarPrimitives::copyConstruct(&d_object.object(), rhs,
                                           basicAllocator);
}

template <class TYPE, class OTHERTYPE>
inline
bcec_ObjectPool_CreatorProxy<TYPE, OTHERTYPE>::
                                         ~bcec_ObjectPool_CreatorProxy()
{
    bslalg_ScalarDestructionPrimitives::destroy(&d_object.object());
}

template <class OTHERTYPE>
inline
bcec_ObjectPool_CreatorProxy<bcec_ObjectPoolFunctors::DefaultCreator,
                             OTHERTYPE>::bcec_ObjectPool_CreatorProxy(
                                               bslma_Allocator *basicAllocator)
: d_object(&MyType::defaultConstruct, basicAllocator)
{
}

template <class OTHERTYPE>
inline
bcec_ObjectPool_CreatorProxy<bcec_ObjectPoolFunctors::DefaultCreator,
                             OTHERTYPE>::bcec_ObjectPool_CreatorProxy(
                           const bcec_ObjectPoolFunctors::DefaultCreator& rhs,
                           bslma_Allocator *basicAllocator)
: d_object(rhs, basicAllocator)
{
}

template <class OTHERTYPE>
inline
bcec_ObjectPool_CreatorProxy<bcec_ObjectPoolFunctors::DefaultCreator,
                             OTHERTYPE>::~bcec_ObjectPool_CreatorProxy()
{
}

// MANIPULATORS
template <class TYPE, class OTHERTYPE>
inline
TYPE& bcec_ObjectPool_CreatorProxy<TYPE, OTHERTYPE>::object()
{
    return d_object.object();
}

template <class OTHERTYPE>
inline
bcec_ObjectPoolFunctors::DefaultCreator&
bcec_ObjectPool_CreatorProxy<bcec_ObjectPoolFunctors::DefaultCreator,
                             OTHERTYPE>::object()
{
    return d_object;
}

                       // ------------------------------
                       // bcec_ObjectPoolFunctors::Reset
                       // ------------------------------

// ACCESSORS
template <class TYPE>
inline
void bcec_ObjectPoolFunctors::Reset<TYPE>::operator()(TYPE *object) const
{
   object->reset();
}

                        // ----------------------------
                        // bcec_ObjectPoolFunctors::Nil
                        // ----------------------------

// ACCESSORS
template <class TYPE>
inline
void bcec_ObjectPoolFunctors::Nil<TYPE>::operator()(TYPE *) const
{
}

                       // ------------------------------
                       // bcec_ObjectPoolFunctors::Clear
                       // ------------------------------

// ACCESSORS
template <class TYPE>
inline
void bcec_ObjectPoolFunctors::Clear<TYPE>::operator()(TYPE *object) const
{
   object->clear();
}

                     // ----------------------------------
                     // bcec_ObjectPoolFunctors::RemoveAll
                     // ----------------------------------

// ACCESSORS
template <class TYPE>
inline
void bcec_ObjectPoolFunctors::RemoveAll<TYPE>::operator()(TYPE *object) const
{
   object->removeAll();
}

                        // ---------------------------
                        // bcec_ObjectPool_AutoCleanup
                        // ---------------------------

// CREATORS
template <class TYPE, class CREATOR, class RESETTER>
inline
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup::AutoCleanup(
                                    BlockNode                       *block,
                                    ObjectNode                      *head,
                                    bdema_InfrequentDeleteBlockList *allocator,
                                    int                              numNodes)
: d_block_p(block)
, d_head_p(head)
, d_allocator_p(allocator)
, d_numNodes(numNodes)
{
}

template <class TYPE, class CREATOR, class RESETTER>
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup::~AutoCleanup()
{
    enum {
        BCEC_NUM_OBJECTS_PER_FRAME =
           bcec_ObjectPool<TYPE, CREATOR, RESETTER>::BCEC_NUM_OBJECTS_PER_FRAME
    };
    if (d_head_p) {
        for (++d_head_p; d_numNodes > 0; --d_numNodes) {
            ((TYPE *)d_head_p)->~TYPE();
            d_head_p += BCEC_NUM_OBJECTS_PER_FRAME;
        }
        d_allocator_p->deallocate(d_block_p);
    }
}

// MANIPULATORS
template <class TYPE, class CREATOR, class RESETTER>
inline
typename bcec_ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup&
bcec_ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup::operator++()
{
    ++d_numNodes;
    return *this;
}

template <class TYPE, class CREATOR, class RESETTER>
inline
void bcec_ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup::release()
{
    d_block_p = 0;
    d_head_p = 0;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
