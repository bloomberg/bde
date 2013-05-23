// btemt_socks5connector.cpp                                          -*-C++-*-
#include <btemt_socks5connector.h>

#include <btemt_sessionpool.h> // for testing only

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_socks5connector_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace btemt {

                        // ---------------------
                        // class Socks5Connector
                        // ---------------------
// CREATORS
Socks5Connector::Socks5Connector(
                  const ProxyGroup&                             socks5Servers,
                  bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                  int                                           minSourcePort,
                  int                                           maxSourcePort,
                  bslma::Allocator                             *basicAllocator)
: d_minSourcePort(minSourcePort)
, d_maxSourcePort(maxSourcePort)
, d_socks5Servers(socks5Servers)
, d_socketFactory_p(socketFactory)
, d_eventManager(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_eventManager.enable();
}

Socks5Connector::~Socks5Connector()
{
}

// MANIPULATORS
void Socks5Connector::connect(const ConnectCallback&      callback,
                              int                         timeoutSeconds,
                              const HostPort&             server)
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
    callback(Socks5Negotiator::BTEMT_INVALID_DESTINATION,
             0,
             0,
             &HostPort("local", 9999));
}

}  // close package namespace
}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
