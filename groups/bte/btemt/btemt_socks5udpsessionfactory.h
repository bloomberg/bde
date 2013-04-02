// btemt_socks5udpsessionfactory.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_SOCKS5UDPSESSIONFACTORY
#define INCLUDED_BTEMT_SOCKS5UDPSESSIONFACTORY

//@PURPOSE: Provide a factory for negotiation SOCKS5 connections.
//
//@CLASSES:
//   Socks5UdpSessionFactory: Mechanism for negotiation SOCKS5 connections.
//
//@AUTHOR: Kevin McMahon (kmcmahon)
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

#ifndef INCLUDED_BTEMT_TCPTIMEREVENTMANAGER
#include <btemt_tcptimereventmanager.h>
#endif

namespace BloombergLP {

class bcema_BlobBufferFactory;
namespace bslma {class Allocator;}
class btemt_AsyncChannel;

namespace btemt {

class Socks5Negotiator;

                        // ==========================
                        // class Socks5UdpSessionFactory
                        // ==========================

class Socks5UdpSessionFactory : public btemt_SessionFactory {
    // Class documentation goes here

  public:
    typedef bdef_Function<void (*)(int            status,
                                   btemt_Session *session)>
                                                         SessionStatusCallback;

  private:
    // DATA
    bslma::Allocator                        *d_allocator_p;
    btemt_TcpTimerEventManager              d_eventManager;
    SocksConfiguration                      d_configuration;
    bcema_BlobBufferFactory                *d_blobBufferFactory_p;
    bdema_ManagedPtr<btemt_SessionFactory>  d_sessionFactory_mp;
    Socks5CredentialsProvider              *d_credentialsProvider_p;
    unsigned short                          d_udpBindPort;
    bool                                    d_ignoreUdpAssociateIp;

    // PRIVATE MANIPULATORS
    void negotiationCallback(
                          int                                    result,
                          btemt::Socks5Negotiator                *negotiator,
                          btemt_AsyncChannel                    *channel,
                          const btemt_SessionFactory::Callback&  userCallback);

  private:
    // NOT IMPLEMENTED
    Socks5UdpSessionFactory(const Socks5UdpSessionFactory& rhs);
    Socks5UdpSessionFactory& operator=(const Socks5UdpSessionFactory&);

  public:
    // CREATORS
    Socks5UdpSessionFactory(
                   bdema_ManagedPtr<btemt_SessionFactory>  sessionFactory,
                   const SocksConfiguration&               configuration,
                   Socks5CredentialsProvider              *credentialsProvider,
                   unsigned short                          udpBindPort,
                   bcema_BlobBufferFactory                *blobBufferFactory,
                   bslma::Allocator                        *allocator = 0);
        // Create a 'Socks5UdpSessionFactory' that can allocate a session object
        // from a 'btemt_AsyncChannel'.  This factory will try to connect to a
        // SOCKS5 proxy, establish a UDP association port, and create a session
        // using a 'btemt_AsyncChannel' commnicating with that UDP port.
        // before successfully returning a session.  The specified
        // 'sessionFactory' will be used to provide the actual session.  Use
        // the specified 'blobBufferFactory' for blob creation in this factory
        // and in all created sessions.  'blobBufferFactory' *must* outlive
        // this factory and all allocated sessions from this factory.  Use the
        // optionally specified 'allocator' to supply other memory.
        // TODO: 'udpBindPort' should be folded into the 'SocksConfiguration'.

    virtual ~Socks5UdpSessionFactory();
        // Deallocate this session factory.

    // MANIPULATORS
    void ignoreUdpAssociateIp();
        // In a standard SOCKS5 UDP associate response the proxy designates
        // the IP address and port where subsequent UDP packets can be sent
        // and the proxy is allowed to give a different IP address than its
        // own, possibily another server.  However, in some cases the proxy
        // may  be behind a NAT and respond in the UDP associate with its
        // own untranslated IP.  Attempting to connect back to that IP will
        // fail since that address is only valid behind the NAT, which you
        // can't get behind.  Therefore, by calling this function this
        // factory will ignore the IP address sent back in the UDP associate
        // response and instead connect back to the same IP as the SOCKS5
        // proxy with the port listed in the associate response.  Note that
        // this isn't standard behavior for a SOCKS5 negotiation, but it
        // apparently happens frequently enough to warrent this option.

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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
