// bteso_endpoint.h                                                   -*-C++-*-
#ifndef INCLUDED_BTESO_ENDPOINT
#define INCLUDED_BTESO_ENDPOINT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a hostname-based address of a TCP or UDP endpoint.
//
//@CLASSES:
//   bteso::Endpoint: TCP or UDP address as a hostname and port
//
//@SEE ALSO: bteso_IPv4Address
//
//@DESCRIPTION: This component provides a value-semantic attribute class,
//  'bteso_Endpoint', that represents a TCP or UDP endpoint as a hostname and
//  port combination.  The address represented by this class is unresolved,
//  and can be used for delayed resolution.  For instance, SOCKS5 protocol
//  supports passing the hostname from the client to the proxy host which may
//  have better access for resolving a DNS name to IP address.
//
// [!WARNING!] This component should only be used by API at this time.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Set Hostname and Port
/// - - - - - - - - - - - - - - - -
// Let us encode a TCP address as a hostname and port.
//
// First, we declare an empty (unset) 'bteso_Endpoint':
//..
//  bteso_Endpoint address;
//..
// Now, we set hostname and port:
//..
//  address.set("www.bloomberg.com", 80);
//..
// Finally, we have an object that describes the HTTP server at Bloomberg LP:
//..
//  assert(address.hostname() == "www.bloomberg.com");
//  assert(address.port() == 80);
//..

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {
                        // ====================
                        // class bteso_Endpoint
                        // ====================

class bteso_Endpoint {
    // This value-semantic class characterizes a TCP, UDP, SCTP, etc. address
    // as a host and port.  The host may be a domain name (requiring name
    // resolution), or a dotted-decimal address.  Objects can either have the
    // default value, or syntactically valid hostname and port.  The default
    // state is signified by the port attribute being 0 and the hostname being
    // an empty string; either one can be used as an indicator of the default
    // value.  Otherwise !both! hostname and port contain syntactically valid
    // values, as defined by 'isValid'.

    // DATA
    bsl::string d_hostname; // remote host name
    int         d_port;     // IP port

  public:
    // CLASS METHODS
    static bool isValid(const bslstl::StringRef& hostname, int port);
        // Return 'true' if '!hostname.size() && !port' (the default value) or
        // '1 <= hostname.size() && hostname.size <= 255' and
        // '1 <= port && port <= 65535', and 'false' otherwise.

    // CREATORS
    explicit bteso_Endpoint(bslma::Allocator *allocator = 0);
        // Construct a 'bteso_Endpoint' object. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

    bteso_Endpoint(const bteso_Endpoint&  original,
                   bslma::Allocator      *allocator = 0);
        // Create a 'bteso_Endpoint' object having the same value as the
        // specified 'original' object. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator.

    bteso_Endpoint(const bslstl::StringRef& hostname,
                   int                      port,
                   bslma::Allocator        *allocator = 0);
        // Create a bteso_Endpoint object from the specified 'hostname' and
        // 'port'. If the optionally specified 'allocator' is not 0 use it
        // to supply memory, otherwise use the default allocator. The behavior
        // is undefined unless 'hostname' is between 1 and 255 characters in
        // length (per RFC 1035) and '0 <= port && port <= 65525'.

    ~bteso_Endpoint();
        // Destroy this object.

    // MANIPULATORS
    //! bteso_Endpoint& operator=(const bteso_Endpoint& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void set(const bslstl::StringRef& hostname, int port);
        // Set the attributes of this object to the specified 'hostname' and
        // 'port'.  The behavior is undefined unless 'isValid(hostname, port)'
        // would return 'true'.

    int setIfValid(const bslstl::StringRef& hostname, int port);
        // Set the attributes of this object to the specified 'hostname' and
        // 'port' if 'isValid(hostname, port)' would return 'true' and return
        // 0.  Otherwise leave the value of this object unchanged and return a
        // non-zero value.

    void reset();
        // Reset this object to the default value.

    // ACCESSORS
    const bsl::string& hostname() const;
        // Return a reference providing non-modifiable access to the hostname
        // attribute of this object.

    int port() const;
        // Return the port number attribute of this object.

};

// FREE OPERATORS
bool operator==(const bteso_Endpoint& lhs, const bteso_Endpoint& rhs);
    // Return 'true' if the specified 'lhs' has the same hostname and port as
    // the specified 'rhs', and 'false' otherwise.

bool operator!=(const bteso_Endpoint& lhs, const bteso_Endpoint& rhs);
    // Return 'true' if specified 'lhs' has a different hostname or different
    // port from the specified 'rhs'.

bsl::ostream& operator<<(bsl::ostream&         output,
                         const bteso_Endpoint& object);
    // Write the specified 'object' to the specified 'output' in human-readable
    // format, and return the reference to 'output'.

// TRAITS
namespace bslma {
    template<> struct UsesBslmaAllocator<bteso_Endpoint> : bsl::true_type {
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
