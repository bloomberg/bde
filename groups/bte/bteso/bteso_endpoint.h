// bteso_endpoint.h                                                   -*-C++-*-
#ifndef INCLUDED_BTESO_ENDPOINT
#define INCLUDED_BTESO_ENDPOINT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an  ...
//
//@CLASSES:
//   bteso::Endpoint: <<description>>
//
//@SEE ALSO:
//
//@DESCRIPTION: This component provides a value-semantic attribute class, <#=
// "'"+CLASS_NAME+"'" #>, that is ...
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - - -
// Suppose that ...
//

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
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
    // as a host and port. The host may be a domain name (requiring name
    // resolution), or a dotted-decimal address.

    // DATA
    bsl::string d_hostname; // remote host name
    int         d_port;     // IP port

  public:
    // CREATORS
    explicit bteso_Endpoint(bslma::Allocator *allocator = 0);
        // Construct a 'bteso_Endpoint' object. If the optionally specified
        // 'allocator' is not 0 use it to supply memory, otherwise use the
        // default allocator. Note that 'isSet()' will return 'false' for this
        // object until 'operator=()' or 'set()' is executed.

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
        // 'port'. The behavior is undefined unless
        // '1 <= hostname.size() && hostname.size <= 255' and
        // '0 < port && port <= 65535'.

    void reset();
        // Reset this object to an empty state.

    // ACCESSORS
    bool isSet() const;
        // Return 'true' if this object is set, and 'false' otherwise.

    const bsl::string& hostname() const;
        // Return a reference providing non-modifiable access to the hostname
        // attribute of this object. The behavior is undefined unless
        // 'true == isSet()'.

    int port() const;
        // Return the port number attribute of this object. The behavior is
        // undefined unless 'true == isSet()'.

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
