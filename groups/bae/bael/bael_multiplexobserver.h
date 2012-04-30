// bael_multiplexobserver.h                                           -*-C++-*-
#ifndef INCLUDED_BAEL_MULTIPLEXOBSERVER
#define INCLUDED_BAEL_MULTIPLEXOBSERVER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a multiplexing observer that forwards to other observers.
//
//@CLASSES:
//    bael_MultiplexObserver: multiplexing observer that forwards log records
//
//@SEE_ALSO: bael_record, bael_context, bael_defaultobserver,
//           bael_loggermanager
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'bael_Observer' protocol for receiving and processing log records:
//..
//               ( bael_MultiplexObserver )
//                            |             ctor
//                            |             registerObserver
//                            |             deregisterObserver
//                            |             numRegisteredObservers
//                            V
//                    ( bael_Observer )
//                                          dtor
//                                          publish
//                                          releaseRecords
//..
// 'bael_MultiplexObserver' is a concrete class derived from 'bael_Observer'
// that processes the log records it receives through its 'publish' method
// by forwarding them to other concrete observers.  'bael_MultiplexObserver'
// maintains a registry of observers to which it forwards log records.  Clients
// of 'bael_MultiplexObserver' register observers using the 'registerObserver'
// method and unregister observers with the 'deregisterObserver' method.  Once
// registered, an observer receives all log records that its associated
// multiplexing observer receives.
//
///THREAD SAFETY
///-------------
// 'bael_MultiplexObserver' is thread-safe and thread-enabled, meaning that
// multiple threads may share the same instance, or may have their own
// instances.
//
///Usage
///-----
// Multiplexing observers are used to interface a 'bael' logging system, which
// generates log records, with the multiplicity of observers that are to
// receive the generated records that are published.  Establishing this
// interface proceeds in three logical steps:
//..
//    (1) Create a distinguished 'bael_MultiplexObserver' that will be the
//        unique observer to receive log records directly from the logging
//        system.
//    (2) Create the other observers required by the application and register
//        each of these observers with some 'bael_MultiplexObserver'.  (Note
//        that a 'bael_MultiplexObserver' may be registered with another
//        'bael_MultiplexObserver'.)
//    (3) Install the distinguished multiplexor from step (1) within the
//        'bael' logging system.
//..
// This example demonstrates the use of a multiplexing observer to forward log
// records from a 'bael' logging system to three registered observers.
// Each of the three registered observers performs distinct actions upon
// receipt of log records:
//..
//    (1) 'defaultObserver', an instance of 'bael_DefaultObserver', formats
//        the records it receives and outputs them to 'stdout'.
//    (2) 'logfileObserver', an instance of 'my_LogfileObserver' (assumed to
//        be a concrete class derived from 'bael_Observer') writes selected
//        records to a log file.
//    (3) 'encryptingObserver', an instance of 'my_EncryptingObserver' (also
//        assumed to be a concrete class derived from 'bael_Observer') creates
//        a compact, encrypted representation of each record, suitable for
//        sending over an unsecure network.
//..
// First we create the three downstream observers that will be registered with
// multiplexor observer:
//..
//     bael_DefaultObserver   defaultObserver;
//     my_LogfileObserver     logfileObserver;
//     my_EncryptingObserver  encryptingObserver;
//..
// Next, we create an initially empty multiplexing observer 'multiplexor' and
// register the three downstream observers 'multiplexor':
//..
//     bael_MultiplexObserver multiplexor;
//     assert(0 == multiplexor.numRegisteredObservers());
//
//     multiplexor.registerObserver(&defaultObserver);
//     multiplexor.registerObserver(&logfileObserver);
//     multiplexor.registerObserver(&encryptingObserver);
//     assert(3 == multiplexor.numRegisteredObservers());
//..
// Then, 'multiplexor' is installed within a 'bael' logging system to be the
// direct recipient of published log records.  This registration is done by
// supplying 'multiplexor' to the 'bael_LoggerManager::initSingleton' method
// that is used to initialize the singleton logger manager:
//..
//     bael_LoggerManager::initSingleton(&multiplexor);
//..
// (Others variants of 'bael_LoggerManager::initSingleton' also take a
// 'bael_Observer*'.)  Henceforth, all log records that are published by the
// logging system will be transmitted to the 'publish' method of 'multiplexor'
// which, in turn, forwards them to 'defaultObserver', 'logfileObserver', and
// 'encryptingObserver' by calling their respective 'publish' methods.
//
// Finally, deregister the three observers when the logs have been all
// forwarded:
//..
//     multiplexor.deregisterObserver(&defaultObserver);
//     multiplexor.deregisterObserver(&logfileObserver);
//     multiplexor.deregisterObserver(&encryptingObserver);
//..
// Note that any observer must exist before registering with multiplexor.
// Any observer already registered must deregister before its destruction.
// Additional observers may be registered with 'multiplexor' at any time.
// Similarly, observers may be unregistered at any time.  This capability
// allows for extremely flexible observation scenarios.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_OBSERVER
#include <bael_observer.h>
#endif

#ifndef INCLUDED_BCEMT_READLOCKGUARD
#include <bcemt_readlockguard.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BCEMT_WRITELOCKGUARD
#include <bcemt_writelockguard.h>
#endif

#ifndef INCLUDED_BSL_SET
#include <bsl_set.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class bael_Record;
class bael_Context;

                         // ============================
                         // class bael_MultiplexObserver
                         // ============================

class bael_MultiplexObserver : public bael_Observer {
    // This class provides a multiplexing implementation of the 'bael_Observer'
    // protocol.  Other concrete observers may be registered with a
    // multiplexing observer ('registerObserver' method) and later unregistered
    // ('deregisterObserver' method).  The 'publish' method of this class
    // forwards the log records that it receives to the 'publish' method of
    // each registered observer.

    // DATA
    bsl::set<bael_Observer *> d_observerSet;  // observer registry

    mutable bcemt_RWMutex     d_rwMutex;      // protects concurrent access
                                              // to 'd_observerSet'

    // NOT IMPLEMENTED
    bael_MultiplexObserver(const bael_MultiplexObserver&);
    bael_MultiplexObserver& operator=(const bael_MultiplexObserver&);

  public:
    // CREATORS
    explicit bael_MultiplexObserver(bslma_Allocator *basicAllocator = 0);
        // Create a multiplexing observer having no registered observers.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    virtual ~bael_MultiplexObserver();
        // Destroy this multiplexing observer.  Note that this method has no
        // effect on the lifetime of observers registered with this observer,
        // if any.

    // MANIPULATORS
    virtual void publish(const bael_Record&  record,
                         const bael_Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context' by forwarding 'record' and 'context' to each of the
        // observers registered with this multiplexing observer.
        //
        // DEPRECATED: use the alternative 'publish' overload instead.

    virtual void publish(const bcema_SharedPtr<const bael_Record>&  record,
                         const bael_Context&                        context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.  This concrete publish implementations processes the
        // 'record' by forwarding 'record' and 'context' to each of the
        // observers registered with this multiplexing observer.

    virtual void releaseRecords();
        // Discard any shared reference to a 'bael_Record' object that was
        // supplied to the 'publish' method, and is held by this observer.
        // This implementation processes 'releaseRecords' by calling
        // 'releaseRecords' on each of the registered observers.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared-pointers must be released.

    int registerObserver(bael_Observer *observer);
        // Add the specified 'observer' to the registry of this multiplexing
        // observer.  Return 0 if 'observer' is non-null and was not already
        // registered with this multiplexing observer, and a non-zero value
        // (with no effect) otherwise.  Henceforth, this multiplexing observer
        // will forward each record it receives through its 'publish' method,
        // including the record's context, to the 'publish' method of
        // 'observer', until 'observer' is deregistered.  The behavior is
        // undefined unless 'observer' remains valid until it is deregistered
        // from this multiplexing observer or until this observer is destroyed.

    int deregisterObserver(bael_Observer *observer);
        // Remove the specified 'observer' from the registry of this
        // multiplexing observer.  Return 0 if 'observer' is non-null and was
        // registered with this multiplexing observer, and a non-zero value
        // (with no effect) otherwise.  Henceforth, 'observer' will no longer
        // receive log records from this multiplexing observer.

    // ACCESSORS
    int numRegisteredObservers() const;
        // Return the number of observers registered with this multiplexing
        // observer.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // ----------------------------
                         // class bael_MultiplexObserver
                         // ----------------------------

// CREATORS
inline
bael_MultiplexObserver::bael_MultiplexObserver(bslma_Allocator *basicAllocator)
: d_observerSet(basicAllocator)
{
}

// ACCESSORS
inline
int bael_MultiplexObserver::numRegisteredObservers() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    return static_cast<int>(d_observerSet.size());
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
