// btemt_socks5sessionfactory.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_SOCKS5SESSIONFACTORY
#define INCLUDED_BTEMT_SOCKS5SESSIONFACTORY

//@PURPOSE: Provide a factory for negotiation SOCKS5 connections.
//
//@CLASSES:
//   Socks5SessionFactory: Mechanism for negotiation SOCKS5 connections.
//
//@AUTHOR: Eric Vander Weele (evander)
//
//@SEE ALSO:
//
//@DESCRIPTION: This component defines...
//
///Usage Example
///-------------
// Text
//..
//  Example code here
//..

#ifndef INCLUDED_BTEMT_SOCKSCONFIGURATION
#include <btemt_socksconfiguration.h>
#endif

#ifndef INCLUDED_BTEMT_SESSION
#include <btemt_session.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

#ifndef INCLUDED_BTEMT_SOCKS5NEGOTIATOR
#include <btemt_socks5negotiator.h>
#endif

#ifndef INCLUDED_BTEMT_SOCKS5CREDENTIALSPROVIDER
#include <btemt_socks5credentialsprovider.h>
#endif

namespace BloombergLP {

class bcema_BlobBufferFactory;
namespace bslma {class Allocator;}
class btemt_AsyncChannel;

namespace btemt {

                        // ==========================
                        // class Socks5SessionFactory
                        // ==========================

class Socks5SessionFactory : public btemt_SessionFactory {
    // Class documentation goes here

  public:
    typedef bdef_Function<void (*)(int            status,
                                   btemt_Session *session)>
                                                     SessionStatusCallback;

  private:
    // DATA
    bslma::Allocator                        *d_allocator_p;
    SocksConfiguration                      d_configuration;
    bdema_ManagedPtr<btemt_SessionFactory>  d_sessionFactory_mp;
    bcema_BlobBufferFactory                *d_blobBufferFactory_p;
    SessionStatusCallback                   d_sessionStatusCallback;
    Socks5CredentialsProvider              *d_credentialsProvider_p;

    // PRIVATE MANIPULATORS
    void negotiationCallback(
                          int                                    result,
                          btemt::Socks5Negotiator                *negotiator,
                          btemt_AsyncChannel                    *channel,
                          const btemt_SessionFactory::Callback&  userCallback);

  private:
    // NOT IMPLEMENTED
    Socks5SessionFactory();
    Socks5SessionFactory(const Socks5SessionFactory&);
    Socks5SessionFactory& operator=(const Socks5SessionFactory&);

  public:
    // CREATORS
    Socks5SessionFactory(
        const SocksConfiguration&                configuration,
        const SessionStatusCallback&             statusCallback,
        Socks5CredentialsProvider       *credentialsProvider,
        bdema_ManagedPtr<btemt_SessionFactory>&  sessionFactory,
        bcema_BlobBufferFactory                 *blobBufferFactory,
        bslma::Allocator                         *allocator = 0);
        // Create a 'Socks5SessionFactory' that can allocate a session object
        // from a 'btemt_AsyncChannel'.  This factory will try to negotiate a
        // SOCKS5 connection to the address in the specified 'configuration'
        // before successfully returning a session.  The specified
        // 'sessionFactory' will be used to provide the actual session.  Use
        // the specified 'blobBufferFactory' for blob creation in this factory
        // and in all created sessions.  'blobBufferFactory' *must* outlive
        // this factory and all allocated sessions from this factory.  Use the
        // optionally specified 'allocator' to supply other memory.

    virtual ~Socks5SessionFactory();
        // Deallocate this session factory.

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                    *channel,
                          const btemt_SessionFactory::Callback&  callback);
        // Allocate and establish a SOCKS5 connection to the specified
        // 'channel'.  The specified 'callback' will be invoked asynchronously
        // to report failure or success.  The allocated session will be of the
        // type of the specified 'sessionFactory' passed in upon construction.
        // If 'callback' is called with success status, you must call
        // 'deallocate()' on this factory with the created session to properly
        // destroy it.

    virtual void deallocate(btemt_Session *session);
        // Deallocate the specified 'session' that was created from a previous
        // call to 'allocate()'.  The behavior is undefined unless 'session'
        // was created from this factory and hasn't been previously
        // deallocated.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
