// btes5_credentialsprovider.h                                        -*-C++-*-

#ifndef INCLUDED_BTES5_CREDENTIALSPROVIDER
#define INCLUDED_BTES5_CREDENTIALSPROVIDER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for providing SOCKS5 credentials during
// negotiation.
//
//@CLASSES:
//   bteso_CredentialsProvider
//
//@SEE ALSO:
//
//@DESCRIPTION: This component provides an abstract protocol for acquiring
// credentials to authneticate a SOCKS5 connection.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Acquire Username and Passowrd
/// - - - - - - - - - - - - - - - - - - - -
// During the process of SOCKS5 negotiation, a proxy host may require the
// username and password to authenticate the SOCKS5 client.  In this example we
// acquire the credentials from secure storage and pass them to the client code
// through a client-supplied callback.
//
// First, we define a class that implements the 'btes5_CredentialsProvider'
// protocol.
//..
//  class MyCredentialsProvider : public btes5_CredentialsProvider {
//    public:
//      // Construction and destruction elided.
//
//  virtual void acquireCredentials(const bteso_Endpoint& proxy,
//                                  Callback              callback);
//      // Acquire credentials and invoke the specified 'callback' with
//      // username and password to authenticate the SOCKS5 client with the
//      // specified 'proxy'.
//
//  virtual void cancelAcquiringCredentials();
//      // Cancel acquiring credentials.
//  };
//..
// Then, we define the 'acquireCredentials' method.  Since here we look up the
// credentials in secure storage which is a non-blocking operation, we can
// invoke the callback directly.
//..
//  void MyCredentialsProvider::acquireCredentials(
//      const bteso_Endpoint& proxy,
//      Callback              callback)
//  {
//      bsl::string username("Defaultuser");
//      bsl::string password("Defaultpassword");
//      // Look up the credentials for the specified 'proxy' ...
//      callback(0, username, password);
//  }
//..
// Now, we define the callback function which would make use of the acquired
// credentials by sending them to the SOCKS5 server.
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
    // Abstract protocol for SOCKS 5 username/password provider.  A concrete
    // implementation of this protocol must be specified to acquire credentials
    // at negotiation time.  The credentials are acquired when the
    // 'acquireCredentials' method is called.  When they become available
    // (possibly by prompting the user or looking up in secure storage), the
    // callback passed to 'acquireCredentials' will be invoked.  Note that the
    // callback may be invoked from a different thread, and may be invoked
    // before 'acquireCredentials' returns.
    //
    // Multiple credential acquisitions may be performed during a
    // 'btesCredentialsProvider' object's lifetime.  An acquisition is
    // outstanding from the time 'acquireCredentials' is called until the
    // specified callback is invoked.  An object supports only one outstanding
    // acquisition at a time.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void(*)(int                      status,
                                  const bslstl::StringRef& username,
                                  const bslstl::StringRef& password)>
        Callback;
        // A callback of this type is invoked to provide credentials for a
        // SOCKS5 authentication. If 'status != 0' no username or password is
        // available.

  public:
    // CREATORS
    btes5_CredentialsProvider();
        // Create a 'btes5_CredentialsProvider' object.

    virtual ~btes5_CredentialsProvider();
        // Destroy this object.

    // MANIPULATORS
    virtual void acquireCredentials(const bteso_Endpoint& proxy,
                                    Callback              callback) = 0;
        // Acquire credentials and invoke the specified 'callback' with
        // username and password to authenticate the SOCKS5 client with the
        // specified 'proxy'.

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
