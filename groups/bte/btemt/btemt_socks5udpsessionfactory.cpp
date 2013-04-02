// btemt_socks5udpsessionfactory.cpp               -*-C++-*-
#include <btemt_socks5udpsessionfactory.h>

#include <btemt_socks5negotiator.h>
#include <btemt_socks5udpchannel.h>
#include <btemt_udpasyncchannel.h>

#include <bteso_resolveutil.h>
#include <bteso_ipv4address.h>
#include <btemt_sessionpool.h>

#include <btemt_log.h>

#include <bcema_sharedptr.h>
#include <bdema_managedptr.h>
#include <bdeut_stringref.h>
#include <bslma_allocator.h>
#include <btemt_asyncchannel.h>
#include <bdef_bind.h>


namespace BloombergLP {
namespace btemt {

                        // -----------------------------
                        // class Socks5UdpSessionFactory
                        // -----------------------------

// PRIVATE MANIPULATORS
void Socks5UdpSessionFactory::negotiationCallback(
                           int                                    result,
                           Socks5Negotiator                      *negotiator,
                           btemt_AsyncChannel                    *channel,
                           const btemt_SessionFactory::Callback&  userCallback)
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5UdpSessionFactory");

    if (result == btemt::Socks5Negotiator::SUCCESS) {
        BTEMT_LOG_INFO << "Successful negotiation to: " << channel->peerAddress() << BTEMT_LOG_END;
        
        bteso_IPv4Address resolvedUdpAddress;
        InternetAddress udpAddress;
        if (d_ignoreUdpAssociateIp) {
            udpAddress = InternetAddress(negotiator->configuration().socksIpAddr(),
                                         negotiator->udpAssociateAddress().port());
        }
        else {
            if (negotiator->udpAssociateAddress().isIPv4()) {
                udpAddress = InternetAddress(negotiator->udpAssociateAddress().ipv4());
            }
            else {
                udpAddress = InternetAddress(negotiator->udpAssociateAddress().domainnameHost(),
                                             negotiator->udpAssociateAddress().port());
            }
        }
        
        int rv = Socks5NegotiatorUtil::resolveUdpAddress(&resolvedUdpAddress,
                                                         udpAddress);
        if (0 != rv) {
            BTEMT_LOG_ERROR << "Error resolving UDP target address, address=" << udpAddress
                           << ", rc=" << rv << BTEMT_LOG_END;
            channel->close();
            userCallback(-1, reinterpret_cast<btemt_Session*>(0));
        }

        bteso_IPv4Address bindAddr;
        bindAddr.setPortNumber(d_udpBindPort);

        bdema_ManagedPtr<btemt_AsyncChannel> udpAsyncChannel(
                    new (*d_allocator_p) UdpAsyncChannel(&d_eventManager,
                                                         bindAddr,
                                                         resolvedUdpAddress,
                                                         d_blobBufferFactory_p,
                                                         d_allocator_p),
                    d_allocator_p);

        btemt::InternetAddress destAddress(negotiator->configuration().destinationIpAddr(),
                                          negotiator->configuration().destinationPort());
        Socks5UdpChannel *sock5UdpChannel = 
                   new (*d_allocator_p) Socks5UdpChannel(udpAsyncChannel,
                                                         channel,
                                                         d_blobBufferFactory_p,
                                                         destAddress,
                                                         d_allocator_p);

        BTEMT_LOG_INFO << "Created UDP channel through socks, local bind port=" << d_udpBindPort
                      << ", UDP socks address=" << resolvedUdpAddress
                      << ", UDP destination address=" << destAddress
                      << BTEMT_LOG_END;

        d_sessionFactory_mp->allocate(sock5UdpChannel, userCallback);

    } else {
        channel->close();
            // indicate failure. TBD: check if more specific reason for error
            // is available.

        userCallback(-1, reinterpret_cast<btemt_Session*>(0));
    }

    d_allocator_p->deleteObject(negotiator);
}

// CREATORS
Socks5UdpSessionFactory::Socks5UdpSessionFactory(
                   bdema_ManagedPtr<btemt_SessionFactory>  sessionFactory,
                   const SocksConfiguration&               configuration,
                   Socks5CredentialsProvider              *credentialsProvider,
                   unsigned short                          udpBindPort,
                   bcema_BlobBufferFactory                *blobBufferFactory,
                   bslma::Allocator                        *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_eventManager(allocator)
, d_configuration(configuration)
, d_credentialsProvider_p(credentialsProvider)
, d_blobBufferFactory_p(blobBufferFactory)
, d_sessionFactory_mp(sessionFactory)
, d_udpBindPort(udpBindPort)
, d_ignoreUdpAssociateIp(false)
{
    BSLS_ASSERT(blobBufferFactory);
    d_eventManager.enable();
}

Socks5UdpSessionFactory::~Socks5UdpSessionFactory()
{
}

// MANIPULATORS
void Socks5UdpSessionFactory::ignoreUdpAssociateIp()
{
    d_ignoreUdpAssociateIp = true;
}

void Socks5UdpSessionFactory::allocate(
                               btemt_AsyncChannel                    *channel,
                               const btemt_SessionFactory::Callback&  callback)
{
    BTEMT_LOG_SET_CATEGORY("btemt.Socks5UdpSessionFactory");

    using namespace bdef_PlaceHolders;
    btemt::Socks5Negotiator *negotiator = 
        new (*d_allocator_p) btemt::Socks5Negotiator(
             channel,
             d_configuration,
             bdef_BindUtil::bind(&Socks5UdpSessionFactory::negotiationCallback,
                                 this, _1, _2, channel, callback),
             d_credentialsProvider_p,
             d_blobBufferFactory_p,
             Socks5Negotiator::UDP_ASSOCIATE_CONNECTION,
             d_allocator_p);
   negotiator->start(); 
}

void Socks5UdpSessionFactory::deallocate(btemt_Session *session)
{
    btemt_AsyncChannel *channel = session->channel();
    d_allocator_p->deleteObject(channel);
    d_sessionFactory_mp->deallocate(session);
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
