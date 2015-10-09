// btls5_testserver.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS5_TESTSERVER
#define INCLUDED_BTLS5_TESTSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test SOCKS5 proxy server.
//
//@CLASSES:
//  btls5::TestServer: a SOCKS5 proxy server
//  btls5::TestServerArgs: arguments to control a 'btls5::TestServer'
//
//@SEE_ALSO: btls5_negotiator, btls5_networkconnector
//
//@DESCRIPTION: This component implements a simple SOCKS5 server suitable for
// testing SOCKS5 clients.  Constructing a 'btls5::TestServer' creates a SOCKS5
// server operating in a different thread.  The behavior of the server is
// controlled by a 'btls5::TestServerArgs' passed during construction; the
// server can validate the protocol messages it receives, and either work
// normally (as a proxy) or simulate several failure conditions.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Connect Without Authentication
///- - - - - - - - - - - - - - - - - - - - -
// We would like to connect to a SOCKS5 proxy which requires no authentication.
// In typical applications, we would then use this connection to negotiate a
// connection to the destination server; in this example we only show the
// construction of the server and the initial connection.
//
// First, we construct a 'btls5::TestServer' object, which will create a
// listening thread on the local server, and load its address into 'proxy'.
// The port will be assigned by the operating system.
//..
//  btlso::Endpoint proxy;
//  btls5::TestServer Server(&proxy);
//  btlso::IPv4Address proxyAddress("127.0.0.1", proxy.port());
//..
// Then, we can connect to the test server:
//..
//  btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//  btlso::StreamSocket<btlso::IPv4Address> *socket = factory.allocate();
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
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS5_CREDENTIALS
#include <btls5_credentials.h>
#endif

#ifndef INCLUDED_BTLSO_ENDPOINT
#include <btlso_endpoint.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BDLB_BIGENDIAN
#include <bdlb_bigendian.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace btlmt {

class SessionPool;
class Session;

}

namespace btls5 {

                           // =====================
                           // struct TestServerArgs
                           // =====================

struct TestServerArgs {
    // This struct is used to control behavior of 'TestServer' objects.

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

        e_NONE,               // no logging
        e_ERROR,              // error condition
        e_DEBUG,              // debugging information
        e_TRACE               // trace: most verbose output
    };

    // DATA
    Mode                  d_mode;

    int                   d_reply;                // SOCKS5 reply field

    bsls::TimeInterval    d_delay;                // if set, wait this much
                                                  // before every response

    btlso::Endpoint       d_destination;          // override the connection
                                                  // address if set
    // logging settings

    bsl::string           d_label;                // use this label for
                                                  // diagnostic output

    Severity              d_verbosity;            // minimum severity for
                                                  // logging

    bsl::ostream         *d_logStream_p;          // stream to log to

    // The following values, if set (not 0) are used to validate request fields

    bdlb::BigEndianInt32  d_expectedIp;

    bdlb::BigEndianInt16  d_expectedPort;

    btlso::Endpoint       d_expectedDestination;

    Credentials           d_expectedCredentials;  // if set, prompt and test

    // CREATORS
    explicit TestServerArgs(bslma::Allocator *basicAllocator = 0);
        // Create a 'TestServerArgs' object initialized as follows:
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

                              // ================
                              // class TestServer
                              // ================

class TestServer {
    // This class implements a test server that support a subset of the SOCKS5
    // protocol.

  public:
    // TYPES
    class SessionFactory;  // manages test server sessions

  private:
    // DATA
    TestServerArgs                   d_args;           // proxy configuration
    bsl::shared_ptr<SessionFactory>  d_sessionFactory;
    bslma::Allocator                *d_allocator_p;    // not owned

  private:
    // NOT IMPLEMENTED
    TestServer(const TestServer&);              // = delete
    TestServer& operator=(const TestServer&);   // = delete

  public:
    // CREATORS
    explicit TestServer(btlso::Endpoint      *proxy,
                        bslma::Allocator     *basicAllocator = 0);
    explicit TestServer(btlso::Endpoint      *proxy,
                        const TestServerArgs *args,
                        bslma::Allocator     *basicAllocator = 0);
        // Create a 'TestServer' object.  If the specified 'proxy' is not 0,
        // load the address of the listening socket into 'proxy'.  The server
        // will run as a thread on 'localhost' with a system-assigned port.
        // Optionally specify 'args' to control the SOCKS5 server behavior,
        // otherwise use a default-constructed 'testServerArgs' value to
        // control behavior.  Optionally specify an 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless the
        // lifetime of 'basicAllocator' extends past the end of the server
        // thread, which may exist longer than the lifetime of this object.

    //! ~TestServer() = default;
        // Destroy this object.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
