// btes5_networkdescription.h                                         -*-C++-*-
#ifndef INCLUDED_BTES5_NETWORKDESCRIPTION
#define INCLUDED_BTES5_NETWORKDESCRIPTION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a description of a network of SOCKS5 proxies.
//
//@CLASSES:
//  btes5_NetworkDescription - description of a network of SOCKS5 proxies
//  btes5_NetworkDescriptionUtil - functions to manipulate network descriptions
//
//@SEE ALSO:
//  btes5_ProxyDescription, btes5_NetworkConnector
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'btes5_NetworkDescription', describing a network of SOCKS5 proxies necessary
// to reach one or more destimation hosts. An object of this type is used with
// btes5_NetworkConnector, see the btes5_networkconnector component for more
// complete usage examples.
//
// [!WARNING!] This component should only be used by API at this time.
//
///Usage
///-----
///Example 1: Describe a Two-level Proxy Network
///- - - - - - - - - - - - - - - - - - - - - - -
// Let's define a network of proxies necessary to reach the Internet from a
// corporate Intranet.  The Internet can be reached through 2 levels: a
// corporate proxy, which then has a connection to a regional proxy, which
// finally has direct access to the Internet addresses of interest.  Each proxy
// level has two proxies for redundancy.
//
// First, we declare an empty network:
//..
//  btes5_NetworkDescription network;
//  assert(0 == network.levelCount());
//..
// Then, we add the addresses of corporate proxies as level 0 (directly
// reachable from our host):
//..
//  bteso_Endpoint corp1("proxy1.corp.com", 1081);
//  bteso_Endpoint corp2("proxy2.corp.com", 1082);
//  network.addProxy(0, corp1);
//  network.addProxy(0, corp2);
//  assert(1 == network.levelCount());
//..
// Now, we add the regional proxies, we can only connect to through the
// corporate proxies.  There are two regional proxies, for redundancy.
//..
//  bteso_Endpoint region1("proxy2.region.com", 1091);
//  bteso_Endpoint region2("proxy2.region.com", 1092);
//  network.addProxy(1, region1);
//  network.addProxy(1, region2);
//  assert(2 == network.levelCount());
//..
// Finally, we have a fully defined network which we can use for connection
// using 'btes_networkDescriptor'.
//..

#ifndef INCLUDED_BTES5_PROXYDESCRIPTION
#include <btes5_proxydescription.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BTES5_PROXYDESCRIPTION
#include <btes5_proxydescription.h>
#endif
#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {
                     // ===================================
                     // class btes5_NetworkDescription
                     // ===================================
class btes5_NetworkDescription {
    // This value-semantic class allows definition of multiple levels of SOCKS5
    // proxies necessary to reach a destination host. It's used by class
    // 'Socks5Connector'. Each proxy is described by its adddress (hostname and
    // port) and credentials (username and password) for authentication (see
    // RFC 1929). The credentials can be unset, indicating no predefined
    // credentials are available. Proxies are grouped into levels, with proxies
    // in level 0 reachable directly, and proxies in level 'n' reachable from
    // those in level 'n - 1'. Proxies in each level will be tried in the order
    // they were added using 'addProxy()'.

    // PRIVATE TYPES
    typedef bsl::vector<btes5_ProxyDescription> ProxyLevel;
        // Group of SOCKS5 proxy servers at one level. Proxy connections will
        // be attempted in the order specified in the vector.

  public:
    // TYPES
    typedef bsl::vector<btes5_ProxyDescription>::const_iterator ProxyIterator;
        // Alias for the type of iterator providing read-only access to
        // descriptions in a level of SOCKS5 proxies.

  private:
    // DATA
    bsl::vector<ProxyLevel>  d_proxies;     // all proxies, arranged by level
    bslma::Allocator        *d_allocator_p; // held, not owned

  public:
    // CREATORS
    explicit
    btes5_NetworkDescription(bslma::Allocator *allocator = 0);
        // Create an empty 'btes5_NetworkDescription' object. If the
        // optionally specified 'allocator' is not 0, use it to allocate
        // memory, otherwise use the default allocator.

    btes5_NetworkDescription(const btes5_NetworkDescription&  original,
                             bslma::Allocator                *allocator = 0);
        // Create a 'btes5_NetworkDescription' object having the same
        // value as the specified 'original' object. If the optionally
        // specified 'allocator' is not 0 use it to supply memory,
        // otherwise use the default allocator.

    ~btes5_NetworkDescription(); // Destroy this object.

    // MANIPULATORS
    btes5_NetworkDescription& operator=(const btes5_NetworkDescription& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    bsl::size_t addProxy(bsl::size_t                  level,
                         const bteso_Endpoint&        address,
                         const btes5_Credentials&     credentials);
        // Add a proxy host with the specified 'address' and 'credentials' to
        // the specified 'level', and return its ordinal number in the 'level'.
        // The behavior is undefined unless 'level >= 0'.

    bsl::size_t addProxy(bsl::size_t level, const bteso_Endpoint& address);
        // Add a proxy host with the specified 'address' to the specified
        // 'level' and unset credentials, and return its ordinal number in the
        // 'level'. The behavior is undefined unless 'level >= 0'.

    void setCredentials(bsl::size_t                  level,
                        bsl::size_t                  order,
                        const btes5_Credentials&     credentials);
        // Set credentials for the proxy identified by the specified 'level'
        // and 'order' to the specified 'credentials'. The behavior is
        // undefined unless 'level' and 'order' identify a proxy previously
        // added with 'addProxy'.

    // ACCESSORS
    bsl::size_t levelCount() const;
        // Return the number of levels in this 'btes5_NetworkDescription'
        // object.

    bsl::size_t numProxies(bsl::size_t level) const;
        // Return the number of proxies in the specified 'level'. The behavior
        // is undefined unless '0 <= level && level < levelCount()'.

    ProxyIterator beginLevel(bsl::size_t level) const;
        // Return the beginning iterator providing read-only access to proxies
        // in the specified 'level'. The behavior is undefined unless
        // '0 <= level && level < levelCount()'.

    ProxyIterator endLevel(bsl::size_t level) const;
        // Return the ending iterator providing read-only access to proxies
        // in the specified 'level'. The behavior is undefined unless
        // '0 <= level && level < levelCount()'.

};
                     // ===================================
                     // struct btes5_NetworkDescriptionUtil
                     // ===================================
struct btes5_NetworkDescriptionUtil {
    // Namespace for utility functions that operate over
    // 'btes5_NetworkDescription' objects.

    static void setLevelCredentials(btes5_NetworkDescription     *proxyNetwork,
                                    bsl::size_t                   level,
                                    const btes5_Credentials&  credentials);
        // Assign the specified 'credentials' for authenticating with every
        // proxy in the specified 'level' of the specified 'proxyNetwork'. The
        // behavior is undefined unless '0 <= level && level < levelCount()'.

    static void setAllCredentials(btes5_NetworkDescription      *proxyNetwork,
                                  const btes5_Credentials&       credentials);
        // Assign the specified 'credentials' for authenticating with every
        // proxy in the specified 'proxyNetwork'. The behavior is undefined
        // unless '0 <= level && level < levelCount()'.

};

// FREE OPERATORS
bool operator==(const btes5_NetworkDescription& lhs,
                const btes5_NetworkDescription& rhs);
    // Return 'true' if 'lhs.levelCount() == rhs.levelCount()' and every level
    // contains the same proxies in the same order in 'lhs' and 'rhs', and
    // return 'false' otherwise.

bool operator!=(const btes5_NetworkDescription& lhs,
                const btes5_NetworkDescription& rhs);
    // Return 'true' if '!(lhs == rhs)', and 'false' otherwise.

bsl::ostream& operator<<(bsl::ostream&                   output,
                         const btes5_NetworkDescription& object);
    // Write the specified 'error' to the specified 'output' in human-readable
    // format, and return the reference to 'output'.

// TRAITS
namespace bslma {
    template<> struct UsesBslmaAllocator<btes5_NetworkDescription>
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
