// bdlcc_objectpool.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_OBJECTPOOL
#define INCLUDED_BDLCC_OBJECTPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe object pool.
//
//@CLASSES:
//  bdlcc::ObjectPool: thread-enabled container of managed objects
//  bdlcc::ObjectPoolFunctors: namespace for resetter/creator implementations
//
//@SEE_ALSO: bdlcc_sharedobjectpool
//
//@DESCRIPTION: This component provides a generic thread-safe pool of objects,
// 'bdlcc::ObjectPool', using the acquire-release idiom and a 'struct' with
// useful functors for a pool of objects, 'bdlcc::ObjectPoolFunctors'.  An
// object pool provides two main methods: 'getObject', which returns an object
// from the pool, and 'releaseObject', which returns an object to the pool for
// further reuse (thus avoiding the overhead of object construction and
// destruction).  A major requirement of using the object pool is that any call
// to 'getObject' can be satisfied by any object in the pool.
//
///Object Construction and Destruction
///-----------------------------------
// The object pool owns the memory required to store the pooled objects, and
// manages the construction, resetting, and destruction of objects.  The user
// may supply functors to create objects and to reset them to a valid state for
// their return to the pool.  Alternatively, this component supplies reasonable
// defaults.  Upon destruction, the object pool deallocates all memory
// associated with the objects in the pool.
//
// The object pool also implements the 'bdlma::Factory' protocol for TYPE.  Its
// 'createObject' and 'deleteObject' methods are provided *only* for this
// purpose and should not be invoked directly (they are just synonyms for
// 'getObject' and 'releaseObject', respectively).  The pool can thus be used
// anywhere a 'bdlma::Factory' (or, therefore, a 'bdlma::Deleter') is expected.
//
///Integrating with 'bslma::ManagedPtr' and 'bsl::shared_ptr'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlcc::ObjectPool' is designed to work with both managed and shared
// pointer types.  Note however, that 'bdlcc_sharedobjectpool' is an
// object-pool specifically designed for use with shared pointers.
//
// Because 'bdlcc::ObjectPool' provides a 'deleteObject' method, it can serve
// as a factory of both 'bslma::ManagedPtr' and 'bsl::shared_ptr' objects.  For
// example, to create a managed pointer from an object pool of 'bsl::string'
// objects:
//..
//  bdlcc::ObjectPool<bsl::string> pool;
//  bslma::ManagedPtr<bsl::string> managedPtr(pool.getObject(), &pool);
//..
// To create a shared pointer (using the same object pool):
//..
//  bslma::Allocator *allocator = bslma::Default::allocator();
//  bsl::shared_ptr<bsl::string> sharedPtr(pool.getObject(), &pool, allocator);
//..
// Note that an allocator is a *required* argument to the 'bsl::shared_ptr'
// constructor used here, and the provided allocator is used to supply memory
// for the internal representation of the pointer, and not to allocate memory
// for the object itself.
//
///Creator and Resetter Template Contract
///--------------------------------------
// 'bdlcc::ObjectPool' is templated on two types 'CREATOR' and 'RESETTER' in
// addition to the underlying object 'TYPE'.  Objects of these types may be
// provided at construction.  The namespace 'bdlcc::ObjectPoolFunctors'
// provides several commonly used implementations.  The creator will be invoked
// as: 'void(*)(void*, bslma::Allocator*)'.  The resetter will be invoked as:
// 'void(*)(TYPE*)'.  The creator functor is called to construct a new object
// of the parameterized 'TYPE' when the pool must be expanded (and thus it
// typically invokes placement 'new' and passes its allocator argument to the
// constructor of 'TYPE').  The resetter functor is called before each object
// is returned to the pool, and is required to put the object into a state such
// that it is ready to be reused.  The defaults for these types are as follows:
//..
//    CREATOR  = bdlcc::ObjectPoolFunctors::DefaultCreator
//    RESETTER = bdlcc::ObjectPoolFunctors::Nil
//..
// 'bdlcc::ObjectPoolFunctors::Nil' is a no-op; it is only suitable if the
// objects stored in the pool are *always* in a valid state to be reused.
// Otherwise another kind of 'RESETTER' should be provided.  In
// 'bdlcc::ObjectPoolFunctors', the classes 'Clear', 'RemoveAll', and 'Reset'
// are all acceptable types for 'RESETTER'.  Since these functor types are
// fully inlined, it is generally most efficient to define 'reset' (or 'clear'
// or 'removeAll') in the underlying 'TYPE' and allow the functor to call that
// method.  The 'CREATOR' functor defaults to an object that invokes the
// default constructor with placement new, passing the allocator argument if
// the type traits of the object indicate it uses an allocator (see
// 'bslalg_typetraits').  If a custom creator functor or a custom 'CREATOR'
// type is specified, it is the user's responsibility to ensure that it
// correctly passes its allocator argument to the constructor of 'TYPE' if
// 'TYPE' takes an allocator.
//
///Exception safety
///----------------
// There are two potential sources of exceptions in this component: memory
// allocation and object construction.  The object pool is exception-neutral
// with full guarantee of rollback for the following methods: if an exception
// is thrown in 'getObject', 'reserveCapacity', or 'increaseCapacity', then the
// pool is in a valid unmodified state (i.e., identical to its state prior to
// the call to 'getObject').  No other method of 'bdlcc::ObjectPool' can throw.
//
///Pool replenishment policy
///-------------------------
// The 'growBy' parameter can be specified in the pool's constructor to
// instruct the pool how to increase its capacity each time the pool is
// depleted.  If 'growBy' is positive, the pool always replenishes itself with
// enough objects to satisfy at least 'growBy' object requests before the next
// replenishment.  If 'growBy' is negative, the pool will increase its capacity
// geometrically until it exceeds the internal maximum (which is
// implementation-defined), and after that it will be replenished with constant
// number of objects.  If 'growBy' is not specified, it defaults to -1 (i.e.,
// geometric increase beginning at 1).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Handling Database Queries
/// - - - - - - - - - - - - - - - - - -
// In this example, we simulate a database server accepting queries from
// clients and executing each query in a separate thread.  Client requests are
// simulated by function 'getClientQuery' which returns a query to be executed.
// The class 'Query' encapsulates a database query and 'queryFactory' is an
// object of a query factory class 'QueryFactory'.
//..
//  enum {
//      k_CONNECTION_OPEN_TIME  = 100,  // (simulated) time to open a
//                                      // connection (in microseconds)
//
//      k_CONNECTION_CLOSE_TIME = 8,    // (simulated) time to close a
//                                      // connection (in microseconds)
//
//      k_QUERY_EXECUTION_TIME  = 4     // (simulated) time to execute a query
//                                      // (in microseconds)
//  };
//
//  class my_DatabaseConnection
//      // This class simulates a database connection.
//  {
//    public:
//      my_DatabaseConnection()
//      {
//          bslmt::ThreadUtil::microSleep(k_CONNECTION_OPEN_TIME);
//      }
//
//      ~my_DatabaseConnection()
//      {
//          bslmt::ThreadUtil::microSleep(k_CONNECTION_CLOSE_TIME);
//      }
//
//      void executeQuery(Query *query)
//      {
//          bslmt::ThreadUtil::microSleep(k_QUERY_EXECUTION_TIME);
//          (void)query;
//      }
//  };
//..
// The server runs several threads which, on each iteration, obtain a new
// client request from the query factory, and process it, until the desired
// total number of requests is achieved.
//..
//  extern "C" void serverThread(bsls::AtomicInt *queries,
//                               int              max,
//                               void             (*queryHandler)(Query*))
//  {
//      while (++(*queries) <= max) {
//          Query *query = queryFactory->createQuery();
//          queryHandler(query);
//      }
//  }
//..
// We first give an implementation that does not uses the object pool.  Later
// we will give an implementation using an object pool to manage the database
// connections.  We also keep track of total response time for each case.  When
// object pool is *not* used, each thread, in order to execute a query, creates
// a *new* database connection, calls its 'executeQuery' method to execute the
// query and finally closes the connection.
//..
//  void queryHandler1(Query *query)
//      // Handle the specified 'query' without using an objectpool.
//  {
//      bsls::Types::Int64 t1 = bsls::TimeUtil::getTimer();
//      my_DatabaseConnection connection;
//      connection.executeQuery(query);
//      bsls::Types::Int64 t2 = bsls::TimeUtil::getTimer();
//
//      totalResponseTime1 += t2 - t1;
//
//      queryFactory->destroyQuery(query);
//
//      // 'connection' is implicitly destroyed on function return.
//  }
//..
// The main thread starts and joins these threads:
//..
//  enum {
//      k_NUM_THREADS = 8,
//      k_NUM_QUERIES = 10000
//  };
//
//  bsls::AtomicInt numQueries(0);
//  bslmt::ThreadGroup tg;
//
//  tg.addThreads(bdlf::BindUtil::bind(&serverThread,
//                                     &numQueries,
//                                     static_cast<int>(k_NUM_QUERIES),
//                                     &queryHandler1),
//                k_NUM_THREADS);
//  tg.joinAll();
//..
// In above strategy, clients always incur the delay associated with opening
// and closing a database connection.  Now we show an implementation that will
// use object pool to *pool* the database connections.
//
///Object Pool Creation and Functor Argument
///- - - - - - - - - - - - - - - - - - - - -
// In order to create an object pool, we may specify, at construction time, a
// functor encapsulating object creation.  The pool invokes this functor to
// create an object in a memory location supplied by the allocator specified at
// construction and owned by the pool.  By default, the creator invokes the
// default constructor of the underlying type, passing the pool's allocator if
// the type uses the bslma::Allocator protocol to supply memory (as specified
// by the "Uses Bslma Allocator" trait, see 'bslalg_typetraits').  If this
// behavior is not sufficient, we can supply our own functor for type creation.
//
///Creating an Object Pool that Constructs Default Objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When the default constructor of our type is sufficient, whether or not that
// type uses 'bslma::Allocator', we can simply use the default behavior of
// 'bdlcc::ObjectPool':
//..
//  bdlcc::ObjectPool<my_DatabaseConnection> pool(-1);
//..
//
///Creating an Object Pool that Constructs Non-Default Objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, if we decide that connection IDs must be supplied to
// objects allocated from the pool, we must define a function which invokes
// placement new appropriately.  When using a custom creator functor, it is the
// responsibility of client code to pass the pool's allocator (supplied as the
// second argument to the functor) to the new object if it uses
// bslma::Allocator.
//..
//    void createConnection(void *arena, bslma::Allocator *alloc, int id)
//    {
//       new (arena) my_DatabaseConnection(id, alloc);
//    }
//..
// then...
//..
//    int myId = 100;
//    bdlcc::ObjectPool<my_DatabaseConnection> pool(
//                             bdlf::BindUtil::bind(&createConnection,
//                                                 bdlf::PlaceHolders::_1,
//                                                 bdlf::PlaceHolders::_2,
//                                                 myId));
//..
// Whichever creator we choose, the modified server looks like
//..
//  connectionPool = &pool;
//
//  for (int i = 0; i < k_NUM_QUERIES; ++i) {
//      my_Query *query = getClientQuery();
//      bslmt::ThreadUtil::create(&threads[i], queryHandler2, (void *)query);
//  }
//  for (int i = 0; i < k_NUM_QUERIES; ++i) {
//      bslmt::ThreadUtil::join(threads[i]);
//  }
//..
//
///Modified 'queryHandler'
///- - - - - - - - - - - -
// Now each thread, instead of creating a new connection, gets a connection
// from the object pool.  After using the connection, the client returns it
// back to the pool for further reuse.  The modified 'queryHandler' is
// following.
//..
//    bdlcc::ObjectPool<my_DatabaseConnection> *connectionPool;
//
//    void queryHandler2(Query *query)
//        // Process the specified 'query'.
//    {
//        bsls::Types::Int64 t1 = bsls::TimeUtil::getTimer();
//        my_DatabaseConnection *connection = connectionPool->getObject();
//        connection->executeQuery(query);
//        bsls::Types::Int64 t2 = bsls::TimeUtil::getTimer();
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_FACTORY
#include <bdlma_factory.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BDLMA_INFREQUENTDELETEBLOCKLIST
#include <bdlma_infrequentdeleteblocklist.h>
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

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {
namespace bdlcc {

                         // =========================
                         // struct ObjectPoolFunctors
                         // =========================

struct ObjectPoolFunctors {
    // This struct provides several functors that are suitable 'RESETTER'
    // parameter types for 'ObjectPool'.  It also provides a 'typedef' that
    // specifies the default 'CREATOR' parameter type for 'ObjectPool'.

    // PUBLIC TYPES
    typedef bsl::function<void(void *, bslma::Allocator *)> DefaultCreator;
        // The default 'CREATOR' parameter type for the 'ObjectPool' class
        // template.

    template <class TYPE>
    class Nil {
        // This fully-inlined class, suitable as the 'RESETTER' parameter type
        // for 'ObjectPool', is a functor taking a pointer to the parameterized
        // 'TYPE' argument, and can be invoked as: 'void(*)(TYPE*)'.  It does
        // nothing.

      public:
        // Use compiler-generated constructors.

        void operator()(TYPE *object) const;
            // Inlined no-op function.
    };

    template <class TYPE>
    class Reset {
        // This fully-inlined class, suitable as the 'RESETTER' parameter type
        // for 'ObjectPool', is a functor taking a pointer to the parameterized
        // 'TYPE' argument, and can be invoked as: 'void(*)(TYPE*)'.  It calls
        // 'reset' upon the provided object.

      public:
        // Use compiler-generated constructors.

        void operator()(TYPE *object) const;
            // Inlined call to 'object->reset()'.
    };

    template <class TYPE>
    class Clear {
        // This fully-inlined class, suitable as the 'RESETTER' parameter type
        // for 'ObjectPool', is a functor taking a pointer to the parameterized
        // 'TYPE' argument, and can be invoked as: 'void(*)(TYPE*)'.  It calls
        // 'clear' upon the provided object.

      public:
        // Use compiler-generated constructors.

        void operator()(TYPE *object) const;
            // Inlined call to 'object->clear()'.
    };

    template <class TYPE>
    class RemoveAll {
        // This fully-inlined class, suitable as the 'RESETTER' parameter type
        // for 'ObjectPool', is a functor taking a pointer to the parameterized
        // 'TYPE' argument, and can be invoked as: 'void(*)(TYPE*)'.  It calls
        // 'removeAll' upon the provided object.

      public:
        // Use compiler-generated constructors.

        void operator()(TYPE *object) const;
            // Inlined call to 'object->removeAll()'.
    };

};

                     // =================================
                     // class ObjectPool_CreatorConverter
                     // =================================

template <class TYPE, class OTHERTYPE>
class ObjectPool_CreatorConverter {
    // The purpose of this private class is to avoid ambiguity between
    // different template instantiations of 'bsl::function' accepted by the
    // constructors of 'ObjectPool'.  It should not be used directly.
    //
    // This version of the converter ignores the parameterized 'OTHERTYPE'.  It
    // requires a reference to an object of the parameterized 'TYPE' in its
    // constructor.

    // DATA
    const TYPE& d_creator;

  public:
    // CREATORS
    ObjectPool_CreatorConverter(const TYPE& creator);

    // ACCESSORS
    const TYPE& creator() const;
};

template <>
class ObjectPool_CreatorConverter<ObjectPoolFunctors::DefaultCreator,
                                  bsl::function<void(void *)> > {
    // The purpose of this private class is to avoid ambiguity between
    // different template instantiations of bsl::function accepted by the
    // constructors of 'ObjectPool'.  It should not be used directly.
    //
    // This version of the converter is a full template specialization for the
    // case that the default creator type is used with a unary creator.  In
    // this case, 'creator' will return a binder (see 'bdlf_bind') that adapts
    // the unary creator to a binary creator that discards the second argument.
    // This usage is *DEPRECATED* and provided only for backward compatibility.

    // DATA
    const bsl::function<void(void *)>& d_creator;

  public:
    // CREATORS
    ObjectPool_CreatorConverter(const bsl::function<void(void *)>& creator);

    // ACCESSORS
    ObjectPoolFunctors::DefaultCreator creator() const;
};

                       // =============================
                       // class ObjectPool_GeneralProxy
                       // =============================

template <class TYPE>
class ObjectPool_GeneralProxy {
    // This private class template provides a default constructor which simply
    // invokes the default constructor of the parameterized 'TYPE'.

    // DATA
    bsls::ObjectBuffer<TYPE> d_object;

    // NOT IMPLEMENTED
    ObjectPool_GeneralProxy& operator=(const ObjectPool_GeneralProxy&);
    ObjectPool_GeneralProxy(const ObjectPool_GeneralProxy&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ObjectPool_GeneralProxy,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    ObjectPool_GeneralProxy(bslma::Allocator *basicAllocator);
        // Create a new proxy and a new object of the parameterized 'TYPE'.  If
        // 'TYPE' declares the "Uses Allocator" trait, the specified
        // 'basicAllocator' is supplied to its default constructor; otherwise
        // 'basicAllocator' is ignored.

    ObjectPool_GeneralProxy(const TYPE&       other,
                            bslma::Allocator *basicAllocator);
        // Create a new proxy and a new object constructed from the specified
        // 'other' object.  If 'TYPE' declares the "Uses Allocator" trait, the
        // specified 'basicAllocator' is supplied to its copy constructor;
        // otherwise 'basicAllocator' is ignored.

    ~ObjectPool_GeneralProxy();
        // Destroy this proxy and the underlying object.

    // MANIPULATORS
    TYPE& object();
        // Return a reference to the modifiable object held by this proxy.
};

                       // =============================
                       // class ObjectPool_DefaultProxy
                       // =============================

// SPECIALIZATIONS
template <class TYPE>
class ObjectPool_DefaultProxy {
    // This private class template provides a default constructor that creates
    // a proxied 'bsl::function' object that invokes the default constructor of
    // the parameterized 'TYPE' with placement 'new'.

    // PRIVATE TYTPES
    typedef ObjectPoolFunctors::DefaultCreator Creator;

    // DATA
    Creator d_object;

    // NOT IMPLEMENTED
    ObjectPool_DefaultProxy(const ObjectPool_DefaultProxy&);
    ObjectPool_DefaultProxy& operator=(const ObjectPool_DefaultProxy&);

  private:
    // PRIVATE CLASS METHODS
    static void defaultConstruct(void *arena, bslma::Allocator *allocator);
        // Invoke, with the specified 'arena' and 'allocator',
        // 'bslalg::ScalarPrimitives::defaultConstruct(arena, allocator)'.
        // This method is necessary to select the correct overload for TYPE.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ObjectPool_DefaultProxy,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    ObjectPool_DefaultProxy(bslma::Allocator *basicAllocator);
        // Create a new proxy for a function object which invokes the default
        // constructor of TYPE.  Use the specified 'basicAllocator' to supply
        // memory.

    ObjectPool_DefaultProxy(const Creator&    rhs,
                            bslma::Allocator *basicAllocator);
        // Create a proxy for a newly created function object constructed from
        // the specified 'rhs' creator.  Use a 'basicAllocator' to supply
        // memory.

    ~ObjectPool_DefaultProxy();
        // Destroy this proxy and the underlying object.

    // MANIPULATORS
    Creator& object();
        // Return a reference to the modifiable function object held by this
        // proxy.
};

                        // ============================
                        // class ObjectPool_ProxyPicker
                        // ============================
template <class CREATOR>
struct ObjectPool_ProxyPicker {
    // For a 'CREATOR' type other than the specialization below, provide a
    // metafunction that returns 'ObjectPool_GeneralProxy<CREATOR>' as the
    // creator proxy for all types.
    template <class TYPE>
    struct Selector {
        typedef ObjectPool_GeneralProxy<CREATOR> Proxy;
    };
};

template <>
struct ObjectPool_ProxyPicker<ObjectPoolFunctors::DefaultCreator>
{
    // For the ObjectPoolFunctors::DefaultCreator, provide a metafunction that
    // returns 'ObjectPool_DefaultProxy<TYPE>' as the creator proxy for type
    // 'TYPE'.
    template <class TYPE>
    struct Selector
    {
        typedef ObjectPool_DefaultProxy<TYPE> Proxy;
    };
};
                              // ================
                              // class ObjectPool
                              // ================

template <class TYPE,
          class CREATOR  = ObjectPoolFunctors::DefaultCreator,
          class RESETTER = ObjectPoolFunctors::Nil<TYPE> >
class ObjectPool : public bdlma::Factory<TYPE> {
    // This class provides a thread-safe pool of reusable objects.  It also
    // implements the 'bdlma::Factory' protocol: "creating" objects gets them
    // from the pool and "deleting" objects returns them to the pool.

    // PRIVATE TYPES
    typedef ObjectPool<TYPE, CREATOR, RESETTER> MyType;

    union ObjectNode {
        // This class stores a list pointer for linking the object nodes
        // together in the free objects list, in which case the reference count
        // is 0.  The list pointer is set to 0 when the reference count is not
        // 0, although that is not necessary.  A negative reference count
        // indicates a node which does not contain an initialized object (the
        // object was destroyed and the creator threw before node could be
        // released again).

        struct {
            ObjectNode                               *d_next_p;
            bsls::AtomicOperations::AtomicTypes::Int  d_refCount;
        } d_inUse;
        typename bsls::AlignmentFromType<TYPE>::Type d_dummy;
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
        typename bsls::AlignmentFromType<ObjectNode>::Type d_dummy;
                                     // padding provider for proper alignment
                                     // of 'ObjectNode'
    };

    class AutoCleanup {
        // This class, private to ObjectPool, implements a proctor for objects
        // created and stored into a temporary list of object nodes as in the
        // 'ObjectPool' type, used in the replenishing method called from
        // 'getObject', 'reserveCapacity' and 'increaseCapacity', to ensure the
        // exception-neutrality with full rollback guarantees of the object
        // pool.

      public:
        // TYPES
        typedef typename ObjectPool<TYPE, CREATOR, RESETTER>::ObjectNode
                                                                    ObjectNode;
        typedef typename ObjectPool<TYPE, CREATOR, RESETTER>::BlockNode
                                                                    BlockNode;

      private:
        // DATA
        BlockNode                        *d_block_p;      // held, not owned
        ObjectNode                       *d_head_p;       // held, not owned
        bdlma::InfrequentDeleteBlockList *d_allocator_p;  // held, not owned
        int                               d_numNodes;

      public:
        // CREATORS
        AutoCleanup(BlockNode                        *block,
                    ObjectNode                       *head,
                    bdlma::InfrequentDeleteBlockList *allocator,
                    int                               numNodes = 0);
            // Create a proctor for the list of the optionally specified
            // 'numNodes' number of nodes with the specified 'head', using the
            // 'allocator' to deallocate the block starting at the specified
            // 'block' at destruction after all the objects in the list have
            // been destroyed, unless the 'release' method has been called.

        ~AutoCleanup();
            // Destroy this object, using the 'allocator' to deallocate the
            // block under management at destruction after all the objects
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

        k_ROUNDED_NUM_OBJECTS = (sizeof(TYPE) + sizeof(ObjectNode) - 1) /
                                                            sizeof(ObjectNode),
                                     // number of 'ObjectNode' needed to
                                     // contain an object of 'TYPE' (rounded up
                                     // to the next integer)

        k_NUM_OBJECTS_PER_FRAME = 1 + k_ROUNDED_NUM_OBJECTS,
                                     // number of 'ObjectNode' equivalent (in
                                     // size) to a frame (object node followed
                                     // by 'TYPE')

        k_MAX_NUM_OBJECTS_PER_FRAME = (INT_MAX / sizeof(ObjectNode) - 1) /
                                                        k_NUM_OBJECTS_PER_FRAME
                                     // 'N' must be less than this
                                     // 'k_MAX_NUM_OBJECTS_PER_FRAME' so that
                                     // the number of bytes in a block, which
                                     // is '(1 + N * NUM_OBJECTS_PER_FRAME)'
                                     // times 'sizeof(ObjectNode)', does not
                                     // overflow
    };

    enum {
        // Default configuration parameters.  Adjust these to tune up
        // performance of 'ObjectPool'.

        k_GROW_FACTOR           =   2,  // multiplicative factor to grow
                                        // capacity

        k_MAX_NUM_OBJECTS       = -32   // minimum 'd_numReplenishObjects'
                                        // value beyond which
                                        // 'd_numReplenishObjects' becomes
                                        // positive
    };

    // DATA
    bsls::AtomicPointer<ObjectNode>
                           d_freeObjectsList;      // list of free objects

    typename ObjectPool_ProxyPicker<CREATOR>::template Selector<TYPE>::Proxy
                           d_objectCreator;        // functor for object
                                                   // creation

    bslalg::ConstructorProxy<RESETTER>
                           d_objectResetter;       // functor to reset object

    int                    d_numReplenishObjects;  // pool growth behavior
                                                   // option (see above)

    bsls::AtomicInt        d_numAvailableObjects;  // number of available
                                                   // objects

    bsls::AtomicInt        d_numObjects;           // number of objects created
                                                   // by this pool

    BlockNode             *d_blockList;            // list of memory blocks

    bdlma::InfrequentDeleteBlockList
                           d_blockAllocator;       // memory block supplier

    bslma::Allocator      *d_allocator_p;          // held, not owned

    bslmt::Mutex           d_mutex;                // pool replenishment
                                                   // serializer

    // NOT IMPLEMENTED
    ObjectPool(const MyType&, bslma::Allocator * = 0);
    ObjectPool& operator=(const MyType&);

    // FRIENDS
    friend class AutoCleanup;

  private:
    // PRIVATE MANIPULATORS
    void replenish();
        // Add additional objects to this pool based on the replenishment
        // policy specified by the 'growBy' argument at construction.

    void addObjects(int numObjects);
        // Create the specified 'numObjects' objects and attach them to this
        // object pool.

  public:
    // TYPES
    typedef RESETTER ResetterType;
    typedef CREATOR  CreatorType;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ObjectPool,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    ObjectPool(int               growBy = -1,
               bslma::Allocator *basicAllocator = 0);
        // Create an object pool that invokes the default constructor of the
        // the parameterized 'TYPE' to construct objects.  When the pool is
        // depleted, it will increase its capacity according to the optionally
        // specified 'growBy' value.  If 'growBy' is positive, the pool
        // replenishes itself with at least 'growBy' new objects.  If 'growBy'
        // is negative, the amount of increase begins at '-growBy' and grows
        // geometrically up to an implementation-defined maximum.  When objects
        // are returned to the pool, the default value of RESETTER is invoked
        // with a pointer to the returned object to restore the object to a
        // reusable state.  The optionally specified 'basicAllocator' is used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // '0 != growBy'.

    explicit
    ObjectPool(const CREATOR&    objectCreator,
               int               growBy,
               bslma::Allocator *basicAllocator = 0);
    explicit
    ObjectPool(const CREATOR&    objectCreator,
               bslma::Allocator *basicAllocator = 0);
        // Create an object pool that uses the specified 'objectCreator'
        // (encapsulating the construction of objects) to create objects.  The
        // client must ensure that 'objectCreator(buf, alloc)' creates an
        // object at memory location 'buf' using 'alloc' to supply memory.
        // When the pool is depleted, it will grow capacity according to the
        // optionally specified 'growBy' value.  If 'growBy' is positive, the
        // pool replenishes itself with at least 'growBy' new objects.  If
        // 'growBy' is negative, the amount of increase begins at '-growBy' and
        // grows geometrically up to an implementation-defined maximum.  When
        // objects are returned to the pool, the default value of RESETTER is
        // invoked with a pointer to the returned object to restore the object
        // to a reusable state.  The optionally specified 'basicAllocator' is
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless '0 != growBy'.

    ObjectPool(const CREATOR&    objectCreator,
               const RESETTER&   objectResetter,
               int               growBy = -1,
               bslma::Allocator *basicAllocator = 0);
        // Create an object pool that uses the specified 'objectCreator'
        // (encapsulating the construction of objects) to create objects.  The
        // client must ensure that 'objectCreator(buf, alloc)' creates an
        // object at memory location 'buf' using 'alloc' to supply memory.
        // When the pool is depleted, it will increase its capacity according
        // to the optionally specified 'growBy' value.  If 'growBy' is
        // positive, the pool replenishes itself with at least 'growBy' new
        // objects.  If 'growBy' is negative, the amount of increase begins at
        // '-growBy' and grows geometrically up to an implementation-defined
        // maximum.  When objects are returned to the pool, the specified
        // 'objectResetter' is invoked with a pointer to the returned object to
        // restore the object to a reusable state.  The optionally specified
        // 'basicAllocator' is used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The behavior
        // is undefined unless '0 != growBy'.

    template <class ANYPROTO>
    explicit
    ObjectPool(const bsl::function<ANYPROTO>&  objectCreator,
               int                             growBy,
               bslma::Allocator               *basicAllocator = 0);
    template <class ANYPROTO>
    explicit
    ObjectPool(const bsl::function<ANYPROTO>&  objectCreator,
               bslma::Allocator               *basicAllocator = 0);
        // *DEPRECATED* Use a creator of the parameterized 'CREATOR' type.

    virtual ~ObjectPool();
        // Destroy this object pool.  All objects created by this pool are
        // destroyed (even if some of them are still in use) and memory is
        // reclaimed.

    // MANIPULATORS
    TYPE *getObject();
        // Return an address of modifiable object from this object pool.  If
        // this pool is empty, it is replenished according to the strategy
        // specified at the pool construction (or an implementation-defined
        // strategy if none was provided).

    void increaseCapacity(int numObjects);
        // Create the specified 'numObjects' objects and add them to this
        // object pool.  The behavior is undefined unless '0 <= numObjects'.

    void releaseObject(TYPE *object);
        // Return the specified 'object' back to this object pool.  Invoke the
        // RESETTER specified at construction, or the default RESETTER if none
        // was provided, before making the object available for reuse.  Note
        // that if RESETTER is the default type ('ObjectPoolFunctors::Nil'),
        // then this method should be invoked to return only *valid* objects
        // because the pool uses the released objects to satisfy further
        // 'getObject' requests.  The behavior is undefined unless the 'object'
        // was obtained from this object pool's 'getObject' method.

    void reserveCapacity(int numObjects);
        // Create enough objects to satisfy requests for at least the specified
        // 'numObjects' objects before the next replenishment.  The behavior is
        // undefined unless '0 <= numObjects'.  Note that this method is
        // different from 'increaseCapacity' in that the number of created
        // objects may be less than 'numObjects'.

    // ACCESSORS
    int numAvailableObjects() const;
        // Return a *snapshot* of the number of objects available in this pool.

    int numObjects() const;
        // Return the (instantaneous) number of objects managed by this pool.
        // This includes both the objects available in the pool and the objects
        // that were allocated from the pool and not yet released.

    // 'bdlma::Factory' INTERFACE
    virtual TYPE *createObject();
        // This concrete implementation of 'bdlma::Factory::createObject'
        // invokes 'getObject'.  This should not be invoked directly.

    virtual void deleteObject(TYPE *object);
        // This concrete implementation of 'bdlma::Factory::deleteObject'
        // invokes 'releaseObject' on the specified 'object', returning it to
        // this pool.  Note that this does *not* destroy the object and should
        // not be invoked directly.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                                // ----------
                                // ObjectPool
                                // ----------

// PRIVATE MANIPULATORS
template <class TYPE, class CREATOR, class RESETTER>
void ObjectPool<TYPE, CREATOR, RESETTER>::replenish()
{
    int numObjects = d_numReplenishObjects >= 0
                   ? d_numReplenishObjects
                   : -d_numReplenishObjects;
    addObjects(numObjects);

    // Grow pool capacity only if 'd_numReplenishObjects' is negative and
    // greater than 'k_MAX_NUM_OBJECTS' (i.e., if the absolute value of
    // 'numObjects' is less than 'k_MAX_NUM_OBJECTS').

    if (d_numReplenishObjects < 0) {
        if (d_numReplenishObjects > k_MAX_NUM_OBJECTS) {
            d_numReplenishObjects *= k_GROW_FACTOR;
        }
        else {
            d_numReplenishObjects = -d_numReplenishObjects;
        }
    }
}

template <class TYPE, class CREATOR, class RESETTER>
void ObjectPool<TYPE, CREATOR, RESETTER>::addObjects(int numObjects)
{
    // Allocate a single memory block to be used for creating block nodes,
    // object nodes, and objects.  Too large a value for 'numObjects' would
    // cause overflow in 'NUM_BYTES_PER_BLOCK' below.

    BSLS_ASSERT_SAFE(numObjects <= k_MAX_NUM_OBJECTS_PER_FRAME);

    const int NUM_BYTES_PER_BLOCK = (int)(sizeof(BlockNode) +
                                          sizeof(ObjectNode) * numObjects *
                                                   k_NUM_OBJECTS_PER_FRAME);

    BlockNode *start = (BlockNode *) d_blockAllocator.allocate(
                                                          NUM_BYTES_PER_BLOCK);

    // Create a block node

    start->d_inUse.d_next_p = d_blockList;
    start->d_inUse.d_numObjects = numObjects;

    // Create and link 'numObjects' objects

    ObjectNode *last = (ObjectNode *)(start + 1);
    AutoCleanup startGuard(start, last, &d_blockAllocator, 0);

    for (int i = 0; i < numObjects; ++i, ++startGuard) {
        last->d_inUse.d_next_p = last + k_NUM_OBJECTS_PER_FRAME;
        bsls::AtomicOperations::initInt(&last->d_inUse.d_refCount, 0);
        d_objectCreator.object()(last + 1, d_allocator_p);
        last += k_NUM_OBJECTS_PER_FRAME;
    }
    last -= k_NUM_OBJECTS_PER_FRAME;
    bsls::AtomicOperations::initInt(&last->d_inUse.d_refCount, 0);

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

    d_numObjects.addRelaxed(numObjects);
    d_numAvailableObjects.addRelaxed(numObjects);
}

// CREATORS
template <class TYPE, class CREATOR, class RESETTER>
ObjectPool<TYPE, CREATOR, RESETTER>::ObjectPool(
                                              int               growBy,
                                              bslma::Allocator *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(growBy)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
ObjectPool<TYPE, CREATOR, RESETTER>::ObjectPool(
                                              const CREATOR&    objectCreator,
                                              int               growBy,
                                              bslma::Allocator *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(growBy)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
inline
ObjectPool<TYPE, CREATOR, RESETTER>::ObjectPool(
                                              const CREATOR&    objectCreator,
                                              bslma::Allocator *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(-1)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
template <class ANYPROTO>
ObjectPool<TYPE, CREATOR, RESETTER>::ObjectPool(
                                const bsl::function<ANYPROTO>&  objectCreator,
                                bslma::Allocator               *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(
      ObjectPool_CreatorConverter<CREATOR, bsl::function<ANYPROTO> >(
                                                      objectCreator).creator(),
      basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(-1)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
template <class ANYPROTO>
ObjectPool<TYPE, CREATOR, RESETTER>::ObjectPool(
                                const bsl::function<ANYPROTO>&  objectCreator,
                                int                             growBy,
                                bslma::Allocator               *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(
      ObjectPool_CreatorConverter<CREATOR, bsl::function<ANYPROTO> >(
                                                      objectCreator).creator(),
      basicAllocator)
, d_objectResetter(basicAllocator)
, d_numReplenishObjects(growBy)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
ObjectPool<TYPE, CREATOR, RESETTER>::ObjectPool(
                                              const CREATOR&    objectCreator,
                                              const RESETTER&   objectResetter,
                                              int               growBy,
                                              bslma::Allocator *basicAllocator)
: d_freeObjectsList(0)
, d_objectCreator(objectCreator, basicAllocator)
, d_objectResetter(objectResetter, basicAllocator)
, d_numReplenishObjects(growBy)
, d_blockList(0)
, d_blockAllocator(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != d_numReplenishObjects);
}

template <class TYPE, class CREATOR, class RESETTER>
ObjectPool<TYPE, CREATOR, RESETTER>::~ObjectPool()
{
    // Traverse the 'd_blockList', destroying all the objects associated with
    // each block, irrespective of whether their reference count is zero or
    // not.

    for (; d_blockList; d_blockList = d_blockList->d_inUse.d_next_p) {
        int numObjects = d_blockList->d_inUse.d_numObjects;
        ObjectNode *p = (ObjectNode *)(d_blockList + 1);
        for (; numObjects != 0; --numObjects) {
           ((TYPE *)(p + 1))->~TYPE();
            p += k_NUM_OBJECTS_PER_FRAME;
      }
  }
}

// MANIPULATORS
template <class TYPE, class CREATOR, class RESETTER>
TYPE *ObjectPool<TYPE, CREATOR, RESETTER>::getObject()
{
    ObjectNode *p;
    do {
        p = d_freeObjectsList.loadRelaxed();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!p)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
            p = d_freeObjectsList;
            if (!p) {
                replenish();
                continue;
            }
        }
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
            2 != bsls::AtomicOperations::addIntNv(&p->d_inUse.d_refCount,2))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            for (int i = 0; i < 3; ++i) {
                // To avoid unnecessary contention, assume that if we did not
                // get the first reference, then the other thread is about to
                // complete the pop.  Wait for a few cycles until he does.  If
                // he does not complete then go on and try to acquire it
                // ourselves.

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
        // dependent load) or 'loadRelaxed' will the "old" (!= p) and the
        // condition will fail.  Note that 'h' is made volatile so that the
        // compiler does not replace the 'h->d_inUse' load with 'p->d_inUse'
        // (and thus removing the data dependency).  TBD to be completely
        // thorough 'h->d_inUse.d_next_p' needs a load dependent barrier (no-op
        // on all current architectures though).

        const ObjectNode * volatile h = d_freeObjectsList.loadRelaxed();

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
            refCount = bsls::AtomicOperations::getInt(&p->d_inUse.d_refCount);

            if (refCount & 1) {
                // The node is now free but not on the free list.  Try to take
                // it.

                if (refCount == bsls::AtomicOperations::testAndSwapInt(
                                                    &p->d_inUse.d_refCount,
                                                    refCount,
                                                    refCount^1)) {
                    // Taken!
                    p->d_inUse.d_next_p = 0;  // not strictly necessary
                    d_numAvailableObjects.addRelaxed(-1);
                    return (TYPE*)(p + 1);                            // RETURN

                }
            }
            else if (refCount == bsls::AtomicOperations::testAndSwapInt(
                                                    &p->d_inUse.d_refCount,
                                                    refCount,
                                                    refCount - 2)) {
                break;
            }
        }
    } while (1);

    p->d_inUse.d_next_p = 0;  // not strictly necessary
    d_numAvailableObjects.addRelaxed(-1);
    return (TYPE *)(p+1);
}

template <class TYPE, class CREATOR, class RESETTER>
void ObjectPool<TYPE, CREATOR, RESETTER>::increaseCapacity(int numObjects)
{
    if (numObjects > 0) {
       bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
       addObjects(numObjects);
    }
}

template <class TYPE, class CREATOR, class RESETTER>
void ObjectPool<TYPE, CREATOR, RESETTER>::releaseObject(TYPE *object)
{
    ObjectNode *current = (ObjectNode *)(void *)object - 1;
    d_objectResetter.object()(object);

    int refCount = bsls::AtomicOperations::getIntRelaxed(
                                                 &current->d_inUse.d_refCount);
    do {
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(2 == refCount)) {
            refCount = bsls::AtomicOperations::testAndSwapInt(
                                                  &current->d_inUse.d_refCount,
                                                  2,
                                                  0);
            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(2 == refCount)) {
                break;
            }
        }

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        const int oldRefCount = refCount;
        refCount = bsls::AtomicOperations::testAndSwapInt(
                                                &current->d_inUse.d_refCount,
                                                refCount,
                                                refCount - 1);
        if (oldRefCount == refCount) {
            // Someone else is still trying to pop this item.  Just let them
            // have it.

            d_numAvailableObjects.addRelaxed(1);
            return;                                                   // RETURN
        }

    } while (1);

    ObjectNode *head = d_freeObjectsList.loadRelaxed();
    for (;;) {
        current->d_inUse.d_next_p = head;
        ObjectNode * const oldHead = head;
        head = d_freeObjectsList.testAndSwap(head, current);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(oldHead == head)) {
            break;
        }
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    }
    d_numAvailableObjects.addRelaxed(1);
}

template <class TYPE, class CREATOR, class RESETTER>
void ObjectPool<TYPE, CREATOR, RESETTER>::reserveCapacity(int numObjects)
{
   bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
   numObjects -= d_numObjects;
   if (numObjects > 0) {
      addObjects(numObjects);
   }
}

// ACCESSORS
template <class TYPE, class CREATOR, class RESETTER>
inline
int ObjectPool<TYPE, CREATOR, RESETTER>::numAvailableObjects() const
{
    return  d_numAvailableObjects;
}

template <class TYPE, class CREATOR, class RESETTER>
inline
int ObjectPool<TYPE, CREATOR, RESETTER>::numObjects() const
{
    return d_numObjects;
}

template <class TYPE, class CREATOR, class RESETTER>
inline
TYPE *ObjectPool<TYPE, CREATOR, RESETTER>::createObject()
{
    return getObject();
}

template <class TYPE, class CREATOR, class RESETTER>
inline
void ObjectPool<TYPE, CREATOR, RESETTER>::deleteObject(TYPE *object)
{
    releaseObject(object);
}

                       // ---------------------------
                       // ObjectPool_CreatorConverter
                       // ---------------------------

// CREATORS
template <class TYPE, class OTHERTYPE>
inline
ObjectPool_CreatorConverter<TYPE, OTHERTYPE>::
ObjectPool_CreatorConverter(const TYPE& creator)
: d_creator(creator)
{
}

template <class TYPE, class OTHERTYPE>
inline
const TYPE& ObjectPool_CreatorConverter<TYPE, OTHERTYPE>
::creator() const
{
    return d_creator;
}

inline
ObjectPool_CreatorConverter<ObjectPoolFunctors::DefaultCreator,
                            bsl::function<void(void *)> >::
ObjectPool_CreatorConverter(const bsl::function<void(void *)>& creator)
: d_creator(creator)
{
}

                         // -----------------------
                         // ObjectPool_DefaultProxy
                         // -----------------------

// CLASS METHODS
template <class TYPE>
inline
void ObjectPool_DefaultProxy<TYPE>
::defaultConstruct(void *arena, bslma::Allocator *allocator)
{
    bslalg::ScalarPrimitives::defaultConstruct((TYPE *)arena, allocator);
}

// CREATORS
template <class TYPE>
inline
ObjectPool_GeneralProxy<TYPE>
::ObjectPool_GeneralProxy(bslma::Allocator *basicAllocator)
{
    bslalg::ScalarPrimitives::defaultConstruct(&d_object.object(),
                                               basicAllocator);
}

template <class TYPE>
inline
ObjectPool_GeneralProxy<TYPE>
::ObjectPool_GeneralProxy(const TYPE& other, bslma::Allocator *basicAllocator)
{
    bslalg::ScalarPrimitives::copyConstruct(&d_object.object(),
                                            other,
                                            basicAllocator);
}

template <class TYPE>
inline
ObjectPool_GeneralProxy<TYPE>
::~ObjectPool_GeneralProxy()
{
    bslalg::ScalarDestructionPrimitives::destroy(&d_object.object());
}

template <class TYPE>
inline
ObjectPool_DefaultProxy<TYPE>
::ObjectPool_DefaultProxy(bslma::Allocator *basicAllocator)
: d_object(bsl::allocator_arg_t(),
           bsl::allocator<Creator>(basicAllocator),
           &ObjectPool_DefaultProxy::defaultConstruct)
{
}

template <class TYPE>
inline
ObjectPool_DefaultProxy<TYPE>
::ObjectPool_DefaultProxy(const Creator&    rhs,
                          bslma::Allocator *basicAllocator)
: d_object(bsl::allocator_arg_t(),
           bsl::allocator<Creator>(basicAllocator),
           rhs)
{
}

template <class TYPE>
inline
ObjectPool_DefaultProxy<TYPE>
::~ObjectPool_DefaultProxy()
{
}

// MANIPULATORS
template <class TYPE>
inline
TYPE& ObjectPool_GeneralProxy<TYPE>::object()
{
    return d_object.object();
}

template <class TYPE>
inline
typename ObjectPool_DefaultProxy<TYPE>::Creator&
ObjectPool_DefaultProxy<TYPE>
::object()
{
    return d_object;
}
}  // close package namespace

                     // --------------------------------
                     // bdlcc::ObjectPoolFunctors::Reset
                     // --------------------------------

// ACCESSORS
template <class TYPE>
inline
void bdlcc::ObjectPoolFunctors::Reset<TYPE>::operator()(TYPE *object) const
{
   object->reset();
}

                      // ------------------------------
                      // bdlcc::ObjectPoolFunctors::Nil
                      // ------------------------------

// ACCESSORS
template <class TYPE>
inline
void bdlcc::ObjectPoolFunctors::Nil<TYPE>::operator()(TYPE *) const
{
}

                     // --------------------------------
                     // bdlcc::ObjectPoolFunctors::Clear
                     // --------------------------------

// ACCESSORS
template <class TYPE>
inline
void bdlcc::ObjectPoolFunctors::Clear<TYPE>::operator()(TYPE *object) const
{
   object->clear();
}

                   // ------------------------------------
                   // bdlcc::ObjectPoolFunctors::RemoveAll
                   // ------------------------------------

// ACCESSORS
template <class TYPE>
inline
void bdlcc::ObjectPoolFunctors::RemoveAll<TYPE>::operator()(TYPE *object) const
{
   object->removeAll();
}

                      // -----------------------------
                      // bdlcc::ObjectPool_AutoCleanup
                      // -----------------------------

// CREATORS
template <class TYPE, class CREATOR, class RESETTER>
inline
bdlcc::ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup::AutoCleanup(
                                   BlockNode                        *block,
                                   ObjectNode                       *head,
                                   bdlma::InfrequentDeleteBlockList *allocator,
                                   int                               numNodes)
: d_block_p(block)
, d_head_p(head)
, d_allocator_p(allocator)
, d_numNodes(numNodes)
{
}

template <class TYPE, class CREATOR, class RESETTER>
bdlcc::ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup::~AutoCleanup()
{
    enum {
        k_NUM_OBJECTS_PER_FRAME =
           bdlcc::ObjectPool<TYPE, CREATOR, RESETTER>::k_NUM_OBJECTS_PER_FRAME
    };
    if (d_head_p) {
        for (++d_head_p; d_numNodes > 0; --d_numNodes) {
            ((TYPE *)d_head_p)->~TYPE();
            d_head_p += k_NUM_OBJECTS_PER_FRAME;
        }
        d_allocator_p->deallocate(d_block_p);
    }
}

// MANIPULATORS
template <class TYPE, class CREATOR, class RESETTER>
inline
typename bdlcc::ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup&
bdlcc::ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup::operator++()
{
    ++d_numNodes;
    return *this;
}

template <class TYPE, class CREATOR, class RESETTER>
inline
void bdlcc::ObjectPool<TYPE, CREATOR, RESETTER>::AutoCleanup::release()
{
    d_block_p = 0;
    d_head_p = 0;
}

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
