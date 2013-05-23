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
///Example 1:
///- - - - - -
// Suppose that ...
//

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
    // Abstract protocol for SOCKS 5 username/password provider. A concrete
    // implementation of this protocol must be specified to acquire credentials
    // at negotiation time.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void(*)(int                      status,
                                  const bslstl::StringRef& username, 
                                  const bslstl::StringRef& password)>
        Callback;
        // A callback of this type is invoked to provide credentials for a
        // SOCKS5 authentication. If 'status != 0' no username or password is
        // available.

  private:
    // NOT IMPLEMENTED
    // btes5_CredentialsProvider(const btes5_CredentialsProvider&);
    // btes5_CredentialsProvider& operator=(const btes5_CredentialsProvider&);

  public:
    // CREATORS    
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
