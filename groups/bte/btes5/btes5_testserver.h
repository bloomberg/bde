// btes5_testserver.h                                                 -*-C++-*-
#ifndef INCLUDED_BTEMT_TESTSOCKS5SERVER
#define INCLUDED_BTEMT_TESTSOCKS5SERVER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a test SOCSK5 proxy server.
//
//@CLASSES:
//  btes5::btes5_TestServer: a SOCKS5 proxy server
//  btes5_TestServerArgs: arguments to control a 'btes5_TestServer'
//
//@SEE ALSO: btes5_negotiator, btes5_connector
//
//@DESCRIPTION: This component implements a simple SOCKS5 server suitable for
// testing SOCKS5 clients.  Constructing a 'btes5_TestServer' creates a SOCKS5
// server operating in a different thread.  The behavior of the server is
// controlled by a 'btes5_testServerArgs' passed during construction; the
// server can validate the protocol messages it receives, and either work
// normally (as a proxy) or simulate several failure conditions.
//
// [!WARNING!] This component should only be used by API at this time.
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

#ifndef INCLUDED_BTES5_CREDENTIALS
#include <btes5_credentials.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDEUT_BIGENDIAN
#include <bdeut_bigendian.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
#endif

#ifndef INCLUDED_BTESO_INETSTREAMSOCKETFACTORY
#include <bteso_inetstreamsocketfactory.h>
#endif

namespace BloombergLP {

class btemt_SessionPool;
class btemt_Session;

                         // ===========================
                         // struct btes5_TestServerArgs
                         // ===========================

struct btes5_TestServerArgs {
    // This struct is used to control behavior of 'btes5_TestServer' objects.

    // TYPES
    enum Mode {
        // Modes of operation of the test SOCKS5 server.
        e_IGNORE,             // ignore any requests
        e_FAIL,               // send an error response
        e_SUCCEED_AND_CLOSE,  // send success and close the connection
        e_ECHO,               // echo back any received data
        e_CONNECT             // try to connect and proxy if requested
    };

    enum Severity {
        // Severity of log messages.
        e_NONE,   // no logging
        e_ERROR,  // error condition
        e_DEBUG,  // debugging information
        e_TRACE   // trace: most verbose output
    };

    Mode              d_mode;
    int               d_reply;  // SOCSK5 reply field
    bdet_TimeInterval d_delay;  // if set, wait this much before every response

    bteso_Endpoint d_destination; // override the connection address if set

    // logging settings

    bsl::string   d_label;        // use this label for diagnostic output
    Severity      d_verbosity;    // minimum severity for logging
    bsl::ostream *d_logStream_p;  // stream to log to

    // The following values, if set (not 0) are used to validate request fields

    bdeut_BigEndianInt32 d_expectedIp;
    bdeut_BigEndianInt16 d_expectedPort;
    bteso_Endpoint       d_expectedDestination;
    btes5_Credentials    d_expectedCredentials;  // if set, prompt and test

    // CONSTRUCTORS
    btes5_TestServerArgs(bslma::Allocator *allocator = 0);
        // Create a 'btes5_TestServerArgs' object initialized as follows:
        //: o 'd_mode = e_SUCCEED_AND_CLOSE'
        //: o 'd_reply = 0'
        //: o 'd_label' unset
        //: o 'd_verbosity = e_DEBUG'
        //: o 'd_logStream_p = &bsl::cout'
        //: o 'd_expectedIp = 0'
        //: o 'd_expectedPort = 0'
        //: o 'd_expectedDestination' unset
        //: o 'd_expectedCredentials' unset
        // Optionally specify an ’allocator’ used to supply memory. If
        // ’allocator’ is 0, the currently installed default allocator is used. 

};

                        // ======================
                        // class btes5_TestServer
                        // ======================
class btes5_TestServer {
    // This class implements a test server that support a subset of the SOCKS5
    // protocol.

  public:
    // TYPES
    class SessionFactory;  // manages test server sessions

  private:
    // DATA
    btes5_TestServerArgs               d_args;         // proxy configuration
    bcema_SharedPtr<SessionFactory>    d_sessionFactory;
    bslma::Allocator                  *d_allocator_p;  // not owned

    // NOT IMPLEMENTED
    btes5_TestServer(const btes5_TestServer&); // = delete
    void operator=(const btes5_TestServer&);   // = delete

  public:
    // CREATORS
    btes5_TestServer(bteso_Endpoint             *proxy,
                     const btes5_TestServerArgs *args = 0,
                     bslma::Allocator           *allocator = 0);
        // Create a 'btes5_TestServer' object, loading its address into the
        // specified 'proxy'. The server will run as a thread on 'localhost'
        // with a system-assigned port. If the optionally specified 'args'
        // is not 0 use it to control the SOCKS5 server behavior. Optionally
        // specify an ’allocator’ used to supply memory. If ’allocator’ is 0,
        // the currently installed default allocator is used. The behavior is
        // undefined unless the lifetime of 'allocator' extends past the end of
        // the server thread, which may exist longer than the lifetime of this
        // object.

    ~btes5_TestServer();
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
