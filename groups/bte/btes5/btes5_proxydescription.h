// btes5_proxydescription.h                                           -*-C++-*-

#ifndef INCLUDED_BTES5_PROXYDECSRIPTION
#define INCLUDED_BTES5_PROXYDESCRIPTION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a description of a SOCKS5 proxy.
//
//@CLASSES:
//   bteso_ProxyDescription: SOCKS5 proxy address and optional credentials
//
//@SEE ALSO:
//  btes5_NetworkDescripttion
//
//@DESCRIPTION: This component provides a value-semantic attribute class,
// 'bteso_ProxyDecsription', describing a SOCKS5 proxy.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - - -
// Suppose that ...
//

#ifndef INCLUDED_USERCREDENTIALS
#include <btes5_credentials.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class btes5_ProxyDescription
                        // ============================
class btes5_ProxyDescription {
    // This value-semantic type describes a SOCKS5 proxy in terms of its
    // address and authentication credentials.

    // DATA
    bteso_Endpoint        d_address;     // TCP address of the proxy
    btes5_Credentials d_credentials; // SOCKS5 credentials

  public:
    // CREATORS
    explicit
    btes5_ProxyDescription(const bteso_Endpoint&         address,
                           bslma::Allocator             *allocator = 0);
        // Create a 'btes5_ProxyDescription' object with the specified
        // 'address' and unset credentials. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

    btes5_ProxyDescription(const bteso_Endpoint&         address,
                           const btes5_Credentials&  credentials,
                           bslma::Allocator             *allocator = 0);
        // Create a 'btes5_ProxyDescription' object with the specified
        // 'address' and 'credentials'. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

    btes5_ProxyDescription(const btes5_ProxyDescription&  original,
                          bslma::Allocator               *allocator = 0);
        // Create a 'btes5_ProxyDescription' object having the same value as
        // the specified 'original' object. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

    ~btes5_ProxyDescription();
        // Destroy this object.

    // MANIPULATORS
    // operator=(const btes5_ProxyDescription& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setAddress(const bteso_Endpoint& value);
        // Set the address of this object to the specified 'value'.

    void setCredentials(const btes5_Credentials& value);
        // Set the credentials of this object to the specified 'value'.

    // ACCESSORS
    const bteso_Endpoint& address() const;
        // Return a reference to a read-only address attribute of this object.

    const btes5_Credentials& credentials() const;
        // Return a reference to a read-only credentials attribute of this
        // object.

};

// FREE OPERATORS
bool operator==(const btes5_ProxyDescription& lhs,
                const btes5_ProxyDescription& rhs);
    // Return 'true' if and only if 'lhs.address() == rhs.address()' and
    // 'lhs.credentials() == rhs.credentials'.

bool operator!=(const btes5_ProxyDescription& lhs,
                const btes5_ProxyDescription& rhs);
    // Return 'true' if and only if either 'lhs.address() != rhs.address()' or
    // 'lhs.credentials() != rhs.credentials'.

bsl::ostream& operator<<(bsl::ostream&                 output,
                         const btes5_ProxyDescription& object);
    // Write the specified 'object' to the specified 'output' in human-readable
    // format, and return the reference to 'output'.

// TRAITS
namespace bslma {
    template<> struct UsesBslmaAllocator<btes5_ProxyDescription>
        : bsl::true_type {
    };
}

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
