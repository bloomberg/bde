// btemt_socks5credentialsprovider.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_SOCKS5CREDENTIALSPROVIDER
#define INCLUDED_BTEMT_SOCKS5CREDENTIALSPROVIDER

//@PURPOSE:
//
//@CLASSES:
//   Socks5CredentialsProvider: <<description>>
//
//@AUTHOR: <<Pooh Bear>> (<<UNIX name>>)
//
//@SEE ALSO:
//
//@DESCRIPTION: This component defines ...
//
///Usage Example
///-------------
// Text
//..
//  Example code here
//..

#include <bdeut_stringref.h>
#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

namespace BloombergLP {

namespace bslma {class Allocator;}

namespace btemt {

                        // ===============================
                        // class Socks5CredentialsProvider
                        // ===============================

class Socks5CredentialsProvider {
    // Abstract protocol for SOCKS 5 username/password provider

  public:
    // PUBLIC TYPES
      typedef bdef_Function<
           void(*)(const bdeut_StringRef& username, 
                   const bdeut_StringRef& password)> Socks5CredentialsCallback;

  private:
    // NOT IMPLEMENTED
    // Socks5CredentialsProvider(const Socks5CredentialsProvider&);
    // Socks5CredentialsProvider& operator=(const Socks5CredentialsProvider&);

  public:
    // CREATORS    
    virtual ~Socks5CredentialsProvider();
        // Destroy this object.

    // MANIPULATORS
    virtual void acquireSocks5Credentials(
                                       Socks5CredentialsCallback callback) = 0;

    virtual void cancelAcquiringCredentials() = 0;

    // ACCESSORS
};

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // -------------------------------
                        // class Socks5CredentialsProvider
                        // -------------------------------

// CREATORS

// MANIPULATORS

// ACCESSORS

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
