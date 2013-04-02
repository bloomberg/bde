// btemt_socks5sessionfactory.cpp               -*-C++-*-
#include <btemt_socks5sessionfactory.h>
#include <bslma_allocator.h>
#include <btemt_asyncchannel.h>
#include <bdef_bind.h>

namespace BloombergLP {
namespace btemt {

                        // --------------------------
                        // class Socks5SessionFactory
                        // --------------------------

// PRIVATE MANIPULATORS
void Socks5SessionFactory::negotiationCallback(
                           int                                    result,
                           btemt::Socks5Negotiator                *negotiator,
                           btemt_AsyncChannel                    *channel,
                           const btemt_SessionFactory::Callback&  userCallback)
{
    if (result == btemt::Socks5Negotiator::SUCCESS) {
        d_sessionStatusCallback(0, NULL);
        d_sessionFactory_mp->allocate(channel, userCallback);
    } else {
        channel->close();
        d_sessionStatusCallback(-1, NULL);  
            // indicate failure. TBD: check if more specific reason for error
            // is available.

        userCallback(-1, reinterpret_cast<btemt_Session*>(0));
    }

    d_allocator_p->deleteObject(negotiator);
}

// CREATORS
Socks5SessionFactory::Socks5SessionFactory(
    const SocksConfiguration&                configuration,
    const SessionStatusCallback&             statusCallback,
    Socks5CredentialsProvider               *credentialsProvider,
    bdema_ManagedPtr<btemt_SessionFactory>&  sessionFactory,
    bcema_BlobBufferFactory                 *blobBufferFactory,
    bslma::Allocator                         *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_configuration(configuration)
, d_sessionStatusCallback(statusCallback)
, d_credentialsProvider_p(credentialsProvider)
, d_sessionFactory_mp(sessionFactory)
, d_blobBufferFactory_p(blobBufferFactory)
{
    BSLS_ASSERT(d_sessionFactory_mp);
    BSLS_ASSERT(blobBufferFactory);
}

Socks5SessionFactory::~Socks5SessionFactory()
{
}

// MANIPULATORS
void Socks5SessionFactory::allocate(
                                btemt_AsyncChannel                    *channel,
                                const btemt_SessionFactory::Callback& callback)
{
    using namespace bdef_PlaceHolders;
    btemt::Socks5Negotiator *negotiator = 
        new (*d_allocator_p) btemt::Socks5Negotiator(
            channel,
            d_configuration,
            bdef_BindUtil::bind(&Socks5SessionFactory::negotiationCallback,
                                this, _1, _2, channel, callback),
            d_credentialsProvider_p,
            d_blobBufferFactory_p,
            Socks5Negotiator::TCP_CONNECTION,
            d_allocator_p);
   negotiator->start(); 
}

void Socks5SessionFactory::deallocate(btemt_Session *session)
{
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