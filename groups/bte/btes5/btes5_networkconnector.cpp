// btes5_networkconnector.cpp                                         -*-C++-*-
#include <btes5_networkconnector.h>

#include <btes5_testserver.h> // for testing only
#include <btemt_sessionpool.h> // for testing only

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_socks5connector_cpp, "$Id$ $CSID$")

namespace BloombergLP {
                        // ----------------------------
                        // class btes5_NetworkConnector
                        // ----------------------------
// CREATORS
btes5_NetworkConnector::btes5_NetworkConnector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   int                                           minSourcePort,
                   int                                           maxSourcePort,
                   btes5_CredentialsProvider                    *provider,
                   bslma::Allocator                             *allocator)
: d_minSourcePort(minSourcePort)
, d_maxSourcePort(maxSourcePort)
, d_socks5Servers(socks5Servers)
, d_socketFactory_p(socketFactory)
, d_eventManager_p(eventManager)
, d_provider_p(provider)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    BSLS_ASSERT(eventManager);
    d_eventManager_p->enable();
}

btes5_NetworkConnector::btes5_NetworkConnector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   btes5_CredentialsProvider                    *provider,
                   bslma::Allocator                             *allocator)
: d_minSourcePort(0)
, d_maxSourcePort(0)
, d_socks5Servers(socks5Servers)
, d_socketFactory_p(socketFactory)
, d_eventManager_p(eventManager)
, d_provider_p(provider)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    BSLS_ASSERT(eventManager);
    d_eventManager_p->enable();
}

btes5_NetworkConnector::btes5_NetworkConnector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   int                                           minSourcePort,
                   int                                           maxSourcePort,
                   bslma::Allocator                             *allocator)
: d_minSourcePort(minSourcePort)
, d_maxSourcePort(maxSourcePort)
, d_socks5Servers(socks5Servers)
, d_socketFactory_p(socketFactory)
, d_eventManager_p(eventManager)
, d_provider_p(0)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    BSLS_ASSERT(eventManager);
    d_eventManager_p->enable();
}

btes5_NetworkConnector::btes5_NetworkConnector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   bslma::Allocator                             *allocator)
: d_minSourcePort(0)
, d_maxSourcePort(0)
, d_socks5Servers(socks5Servers)
, d_socketFactory_p(socketFactory)
, d_eventManager_p(eventManager)
, d_provider_p(0)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    BSLS_ASSERT(eventManager);
    d_eventManager_p->enable();
}

btes5_NetworkConnector::~btes5_NetworkConnector()
{
}

// MANIPULATORS
void btes5_NetworkConnector::connect(const ConnectionStateCallback& callback,
                                     const bdet_TimeInterval&       timeout,
                                     const bteso_Endpoint&          server)
{
/*** TODO: put real logic here
check at least one level, and each proxy level has at least one proxy
set timeout
create a vector of indices [levelCount], starting with 0.
connect(level=0, vector):

Connect:
 
call real-connector
 if index < level0.size
    connect(level0[index], connect0-callback)
connect0-callback(status)
 if fail index++ and call real-connector
 if succeed call negotiator(socket, level, order, negotiatorCb)

connect(level, indices)
 // theoretically, we have at least one more proxy to try
 if (0 == level) tcp-connect(d_proxies[0].indices[0], negotiatorCb)
 else negotiate

negotiatorCb(socket, level, order
 if success
  if last level SUCCEED
  else
   set indices[level+1..levelCount] = 0
   call negotiator(socket, level+1, 0, negotiatorCb)
 else
  if failure is bad password FAIL
  indices[level]++
  set indices[level+1..levelCount] = 0
  while (indices[level] == d_proxies[level].size()) {
   if (level == 0) FAIL
   set indices[level..levelCount] = 0
   indices[--level]++
  }
  connect(level=0, indices)

***/
    btes5_DetailedError error("not implemented yet");
    callback(BTES5_ERROR,
             0,
             d_socketFactory_p,
             error);
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
