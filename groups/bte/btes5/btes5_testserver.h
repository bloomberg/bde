// btes5_testserver.h                                                 -*-C++-*-
#ifndef INCLUDED_BTES5_TESTSERVER
#define INCLUDED_BTES5_TESTSERVER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a test SOCKS5 proxy server.
//
//@CLASSES:
//  btes5_TestServer: a SOCKS5 proxy server
//  btes5_TestServerArgs: arguments to control a 'btes5_TestServer'
//
//@SEE_ALSO: btes5_negotiator, btes5_networkconnector
//
//@DESCRIPTION: This component implements a simple SOCKS5 server suitable for
// testing SOCKS5 clients.  Constructing a 'btes5_TestServer' creates a SOCKS5
// server operating in a different thread.  The behavior of the server is
// controlled by a 'btes5_TestServerArgs' passed during construction; the
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
///- - - - - - - - - - - - - - - - - - - - -
// We would like to connect to a SOCKS5 proxy which requires no
// authentication.  In typical applications, we would then use this connection
// to negotiate a connection to the destination server; in this example we only
// show the construction of the server and the initial connection.
//
// First, we construct a 'btes5_TestServer' object, which will create a
// listening thread on the local server, and load its address into 'proxy'.
// The port will be assigned by the operating system.
//..
//  bteso_Endpoint proxy;
//  btes5_TestServer Server(&proxy);
//  bteso_IPv4Address proxyAddress("127.0.0.1", proxy.port());
//..
// Then, we can connect to the test server:
//..
//  bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
//  bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();
//  assert(socket);
//  int rc = socket->connect(proxyAddress);
//..
// Now, the socket is connected to the test server.  A typical application
// would negotiate, using the SOCKS5 protocol, to connect to the destination
// server.
//..
//  assert(!rc);
//..
// Finally, we can free the resources associated with this connection:
//..
//  factory.deallocate(socket);

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

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

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
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

    // DATA
    Mode              d_mode;
    int               d_reply;  // SOCKS5 reply field
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

    // CREATORS
    explicit btes5_TestServerArgs(bslma::Allocator *basicAllocator = 0);
        // Create a 'btes5_TestServerArgs' object initialized as follows:
        //: o 'd_mode               = e_SUCCEED_AND_CLOSE'
        //: o 'd_reply              = 0'
        //: o 'd_label'               ""
        //: o 'd_verbosity          = e_DEBUG'
        //: o 'd_logStream_p        = &bsl::cout'
        //: o 'd_expectedIp'        = 0'
        //: o 'd_expectedPort       = 0'
        //: o 'd_expectedDestination' default
        //: o 'd_expectedCredentials' default
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
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

  private:
    // NOT IMPLEMENTED
    btes5_TestServer(const btes5_TestServer&);              // = delete
    btes5_TestServer& operator=(const btes5_TestServer&);   // = delete

  public:
    // CREATORS
    explicit btes5_TestServer(bteso_Endpoint             *proxy,
                              bslma::Allocator           *basicAllocator = 0);
    explicit btes5_TestServer(bteso_Endpoint             *proxy,
                              const btes5_TestServerArgs *args,
                              bslma::Allocator           *basicAllocator = 0);
        // Create a 'btes5_TestServer' object, loading its address into the
        // specified 'proxy'.  The server will run as a thread on 'localhost'
        // with a system-assigned port.  Optionally specify 'args' to control
        // the SOCKS5 server behavior, otherwise use a default-constructed
        // 'btes5_testServerArgs' value to control behavior.  Optionally
        // specify an 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless the lifetime of
        // 'basicAllocator' extends past the end of the server thread, which
        // may exist longer than the lifetime of this object.

    //! ~btes5_TestServer() = default;
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
