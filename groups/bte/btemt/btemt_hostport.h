// btemt_hostport.h                                                   -*-C++-*-
#ifndef INCLUDED_BTEMT_HOSTPORT
#define INCLUDED_BTEMT_HOSTPORT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an  ...
//
//@CLASSES:
//   btemt::HostPort: IP endpoint as hostname and port.
//
//@SEE ALSO: bteso_IPv4Address
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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef BSLMA_DEFAULT
#include <bslma_default.h>
#endif

namespace BloombergLP {
namespace btemt {

                        // ==============
                        // class HostPort
                        // ==============

class HostPort {
    // This value-semantic class characterizes an IP address as a host and
    // port. The host may be a domain name (requiring name resolution), or
    // decimal-dotted address.

  public:
    // DATA
    bsl::string d_hostname; // remote host name
    int         d_port;     // IP port


    // CREATORS
    HostPort(const char       *hostname,
             int               port,
             bslma::Allocator *basicAllocator = 0);
        // Construct a HostPort object from the specified 'hostname' and
        // 'port'. If the optionally specified ’basicAllocator’ is not 0 use it
        // to supply memory, otherwise use the default allocator.

    ~HostPort();
        // Destroy this object.
};

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================
                               // ---------------
                               // struct HostPort
                               // ---------------
// CREATORS
inline
HostPort::HostPort(const char       *hostname,
                   int               port,
                   bslma::Allocator *basicAllocator)
: d_hostname(hostname, bslma_Default::allocator(basicAllocator))
, d_port(port)
{
    BSLS_ASSERT(hostname);
    BSLS_ASSERT(0 <= port && port <= 65535);
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const HostPort address)
{
    return stream << address.d_hostname << ':' << address.d_port;
}

}  // close package namespace
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
