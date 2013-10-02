// btes5_credentialsprovider.h                                        -*-C++-*-
#ifndef INCLUDED_BTES5_CREDENTIALSPROVIDER
#define INCLUDED_BTES5_CREDENTIALSPROVIDER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for providing SOCKS5 credentials.
//
//@CLASSES:
//  bteso_CredentialsProvider: SOCKS5 credentials provider protocol
//
//@SEE ALSO: btes5_networkconnector, btes5_negotiator
//
//@DESCRIPTION: This component provides a protocol,
// 'btes5_CredentialsProvider', for asynchronously providing credentials to
// authenticate a SOCKS5 connection.
//
///Thread Safety
///-------------
// The class 'btes5_CredentialsProvider' is fully thread-safe.
//
// The 'btes5_CredentialsProvider' protocol permits thread-aware
// implementations which execute response callback functors in different
// threads; callbacks provided by client code must not depend on being executed
// in any particular thread.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Acquire Username and Password
/// - - - - - - - - - - - - - - - - - - - -
// During the process of SOCKS5 negotiation, a proxy host may require the
// username and password to authenticate the SOCKS5 client.  In this example we
// use hard-coded credentials and pass them to the client code through a
// client-supplied callback.
//
// First, we define a class that implements the 'btes5_CredentialsProvider'
// protocol.
//..
//  class MyCredentialsProvider : public btes5_CredentialsProvider {
//    public:
//      // Construction and destruction elided.
//
//      virtual void acquireCredentials(const bteso_Endpoint&       proxy,
//                                      SuppliedCredentialsCallback callback);
//          // Acquire credentials and invoke the specified 'callback' with
//          // username and password to authenticate the SOCKS5 client with the
//          // specified 'proxy'.
//
//      virtual void cancelAcquiringCredentials();
//          // Cancel acquiring credentials.
//  };
//..
// Then, we define the 'acquireCredentials' method, which simply returns
// hard-coded user credentials; note that in a typical application credentials
// may be obtained from secure storage or by prompting the user:
//..
//  void MyCredentialsProvider::acquireCredentials(
//      const bteso_Endpoint&       proxy,
//      SuppliedCredentialsCallback callback)
//  {
//      bsl::string username("Defaultuser");
//      bsl::string password("Defaultpassword");
//      // Look up the credentials for the specified 'proxy' ...
//      callback(0, username, password);
//  }
//..
// Now, we define the callback function which would make use of the acquired
// credentials by sending them to the SOCKS5 server.  Note that typically this
// callback is *not* defined by clients of the 'btes5' package (e.g.,
// 'btes5_negotiator' defines such a callback which is passed to
// credential-provider implementations during the negotiation process).
//..
//  void credentialsCb(int                      status,
//                     const bslstl::StringRef& username,
//                     const bslstl::StringRef& password)
//  {
//      if (0 == status) {
//          assert("Defaultuser" == username);
//          assert("Defaultpassword" == password);
//          // send username and password to the SOCKS5 host ...
//      } else {
//          // credentials not available, report error and stop negotiation ...
//      }
//  }
//..
// Finally, we define the function which will be able to authenticate the
// connection using credentials supplied by a 'MyCredentialsProvider'.
//..
//  void socks5Authenticate()
//  {
//      MyCredentialsProvider provider;
//      bteso_Endpoint proxy("proxy1.corp.com", 1080);
//      provider.acquireCredentials(proxy, &credentialsCb);
//      // next stage of negotiation will be done by 'credentialsCb'
//  }
//..

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
#endif

namespace BloombergLP {

                        // ===============================
                        // class btes5_CredentialsProvider
                        // ===============================

class btes5_CredentialsProvider {
    // This class provides a protocol (a pure abstract interface) for providing
    // credentials for SOCKS5 authentication.  A concrete implementation of
    // this protocol must be defined to acquire credentials at negotiation
    // time.
    //
    // The method 'acquireCredentials' will be invoked to acquire credentials
    // for a SOCKS5 proxy authentication.  The results of credentials
    // acquisition are communicated asynchronously by invoking the
    // client-supplied callback.
    //
    // Multiple credential acquisitions may be performed during a
    // 'btes5_CredentialsProvider' object's lifetime.  An acquisition is
    // outstanding from the time 'acquireCredentials' is called until the
    // specified callback is invoked.  A 'btes5_CredentialsProvider' object
    // supports only one outstanding acquisition at a time.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void(*)(int                      status,
                                  const bslstl::StringRef& username,
                                  const bslstl::StringRef& password)>
        SuppliedCredentialsCallback;
        // Alias for a functor that is called to report the credentials that
        // have been acquired for a SOCKS5 authentication. An implementation of
        // 'acquireCredentials' accepts a 'SuppliedCredentialsCallback' as an
        // argument, and invokes the callback after the credentials have been
        // acquired. If credentials were acquired successfully, the supplied
        // 'status' should be 0, and 'username' and 'password' indicate the
        // username and password for the user-credentials; otherwise (if
        // credentials were not successfully acquired) the supplied 'status'
        // should be non-zero.

  public:
    // CREATORS
    virtual ~btes5_CredentialsProvider();
        // Destroy this object.

    // MANIPULATORS
    virtual void acquireCredentials(const bteso_Endpoint&       proxy,
                                    SuppliedCredentialsCallback callback) = 0;
        // Start acquiring credentials for authenticating with the specified
        // 'proxy' and arrange to invoke the specified 'callback' with the
        // results of the acquisition, including 'username' and 'password' if
        // credentials were acquired successfully.  Note this call should not
        // block; 'callback' may be invoked from a different thread and may be
        // invoked before 'acquireCredentials' returns.

    virtual void cancelAcquiringCredentials() = 0;
        // Cancel acquiring credentials.
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
