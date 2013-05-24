// btes5_testserver.h                                           -*-C++-*-
#ifndef INCLUDED_BTEMT_TESTSOCKS5SERVER
#define INCLUDED_BTEMT_TESTSOCKS5SERVER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an test SOCSK5 proxy server.
//
//@CLASSES:
//   btes5::btes5_TestServer: <<description>>
//
//@SEE ALSO: btes5_negotiator, btes5_connector
//
//@DESCRIPTION: This component implements a simple SOCKS5 server suitable for
//  testing SOCKS5 clients.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Connect Without Authentication
///- - - - - -
// We would like to connect to a server nyplat1:8194 using a SOCKS5 server
// which requires no authentication. First, we construct a 'btes5_TestServer'
// object, which will create a listening thread on the local server, and load
// its adddress into 'proxy'. The port will be assigned by the operating
// system.
//..
//  bteso_Endpoint proxy;
//  btes5_TestServer Server(&proxy);
//..
//..
// Next we define a callback function to process the connection result.
// Normally, here we would start normal input/output to the destimation server
// (assuming the connection was successful).
//..
//  void connectCb(int status, bteso_StreamSocket<bteso_IPv4Address> *socket)
//  {
//      if (status) {
//          cout << "Can't connect through proxy, status=" << status << endl;
//      }
//      cout << "Connection established" << endl;
//  }
//..
// Now, we construct a 'Socks5Connector' specifying the proxy address.
//..
//  ProxyGroup Servers;
//  Servers.addProxy(proxy);
//..
// Finally, we can connect to our destination through the proxy.
//..
//  connect(connectCb, bteso_Endpoint("nyplat1", 8194);

#ifndef INCLUDED_BDEUT_BIGENDIAN
#include <bdeut_bigendian.h>
#endif

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
#endif

#ifndef INCLUDED_BTESO_INETSTREAMSOCKETFACTORY
#include <bteso_inetstreamsocketfactory.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

namespace BloombergLP {

                         // ===========================
                         // struct btes5_TestServerArgs
                         // ===========================
struct btes5_TestServerArgs {
    // This struct is used to control behavior of 'btes5_TestServer' objects.

    bteso_StreamSocket<bteso_IPv4Address>   *d_connection_p;
    int                                      d_reply;
    int                                      d_status;
    bool                                     d_ignoreConnectAttempts;
    bdeut_BigEndianInt32                     d_expectedIp;
    bdeut_BigEndianInt16                     d_expectedPort;

    // CONSTRUCTORS
    btes5_TestServerArgs(int numWaitThreads = 3);
};
                        // ======================
                        // class btes5_TestServer
                        // ======================

class btes5_TestServer {
    // This class implements a test server that support a subset of the SOCKS5
    // protocol.

    // DATA

    // NOT IMPLEMENTED
    btes5_TestServer(const btes5_TestServer&); // = delete
    void operator=(const btes5_TestServer&);   // = delete

  public:

    // CREATORS
    btes5_TestServer(bteso_Endpoint             *proxy,
                     const btes5_TestServerArgs *userArgs = 0);
        // Create a 'btes5_TestServer' object, loading its address into the
        // specified 'proxy'. The server will run as a thread on 'localhost'
        // with a system-assigned port. If the optionally specified 'userArgs'
        // is not 0 use it to control the SOCKS5 server behavior.

    // ~btes5_TestServer() = default;
        // Destroy this object.

};

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
