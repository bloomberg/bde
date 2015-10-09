// btls5_networkdescription.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS5_NETWORKDESCRIPTION
#define INCLUDED_BTLS5_NETWORKDESCRIPTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a description of a network of SOCKS5 proxies.
//
//@CLASSES:
//  btls5::NetworkDescription: description of a network of SOCKS5 proxies
//
//@SEE_ALSO: btls5_proxydescription, btls5_networkconnector,
//           btls5_networkdescriptionutil
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'btls5::NetworkDescription', describing a network of SOCKS5 proxies
// necessary to reach one or more destination hosts.  An object of this type is
// used with 'btls5::NetworkConnector'; see the 'btls5_networkconnector'
// component for a more complete usage examples.
//
// Each proxy is described by its address (hostname and port) and credentials
// (username and password) for authentication (see RFC 1929).  The credentials
// can be empty (equal to the default 'btls5::Credentials' value) indicating no
// predefined credentials are available.  Proxies are grouped into levels, with
// proxies in level 0 reachable directly, and proxies in level 'n' reachable
// from those in level 'n - 1'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Describe a Two-level Proxy Network
///- - - - - - - - - - - - - - - - - - - - - - -
// Let's define a network of proxies necessary to reach the Internet from a
// corporate Intranet.  The Internet can be reached through 2 levels: a
// corporate proxy, which then has a connection to a regional proxy, which
// finally has direct access to the Internet.  Each proxy level has two proxies
// for redundancy.
//
// First, we declare an empty network:
//..
//  btls5::NetworkDescription network;
//  assert(0 == network.numLevels());
//..
// Then, we add the addresses of corporate proxies as level 0 (directly
// reachable from our host):
//..
//  int proxyNumber;  // ordinal number of proxy within a level
//
//  btlso::Endpoint corp1("proxy1.corp.com", 1081);
//  btlso::Endpoint corp2("proxy2.corp.com", 1082);
//  proxyNumber = network.addProxy(0, corp1);
//  assert(0 == proxyNumber);
//
//  proxyNumber = network.addProxy(0, corp2);
//  assert(1 == proxyNumber);
//  assert(1 == network.numLevels());
//..
// Now, we add the regional proxies that we can connect to only through the
// corporate proxies.  There are two regional proxies, for redundancy.
//..
//  btlso::Endpoint region1("proxy1.region.com", 1091);
//  btlso::Endpoint region2("proxy2.region.com", 1092);
//  proxyNumber = network.addProxy(1, region1);
//  assert(0 == proxyNumber);
//
//  proxyNumber = network.addProxy(1, region2);
//  assert(1 == proxyNumber);
//  assert(2 == network.numLevels());
//..
// Finally, we have a fully defined network that we can use for connection
// using 'btls5::NetworkDescription'.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS5_PROXYDESCRIPTION
#include <btls5_proxydescription.h>
#endif

#ifndef INCLUDED_BTLSO_ENDPOINT
#include <btlso_endpoint.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace btls5 {

class Credentials;

                          // ========================
                          // class NetworkDescription
                          // ========================

class NetworkDescription {
    // This value-semantic class allows the definition of multiple levels of
    // SOCKS5 proxies necessary to reach a destination host.

    // PRIVATE TYPES
    typedef bsl::vector<ProxyDescription> ProxyLevel;
        // Ordered list of SOCKS5 proxy servers at one level.

  public:
    // TYPES
    typedef bsl::vector<ProxyDescription>::const_iterator ProxyIterator;
        // Alias for the type of iterator providing read-only access to
        // descriptions in a level of SOCKS5 proxies.

  private:
    // DATA
    bsl::vector<ProxyLevel> d_proxies;  // all proxies, arranged by level

    // FRIENDS
    friend bool operator==(const NetworkDescription&,
                           const NetworkDescription&);
    friend bool operator!=(const NetworkDescription&,
                           const NetworkDescription&);

  public:
    // CREATORS
    explicit
    NetworkDescription(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'NetworkDescription' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    NetworkDescription(const NetworkDescription&  original,
                       bslma::Allocator          *basicAllocator = 0);
        // Create a 'NetworkDescription' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    //! ~NetworkDescription() = default;
        // Destroy this object.

    // MANIPULATORS
    NetworkDescription& operator=(const NetworkDescription& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    bsl::size_t addProxy(bsl::size_t level, const btlso::Endpoint& address);
        // Add a proxy host with the specified 'address' and empty credentials
        // to the specified 'level' in this 'NetworkDescription' object, and
        // return its ordinal number in the 'level'.  Note that if
        // 'numLevels() <= level', this function will create empty intermediate
        // proxy levels.

    bsl::size_t addProxy(bsl::size_t            level,
                         const btlso::Endpoint& address,
                         const Credentials&     credentials);
        // Add a proxy host with the specified 'address' and 'credentials' to
        // the specified 'level' in this 'NetworkDescription' object, and
        // return its ordinal number in the 'level'.  Note that if
        // 'numLevels() <= level', this function will create empty intermediate
        // proxy levels.

    void setCredentials(bsl::size_t        level,
                        bsl::size_t        order,
                        const Credentials& credentials);
        // Set credentials for the proxy identified by the specified 'level'
        // and 'order' in this 'NetworkDescription' object to the specified
        // 'credentials'.  The behavior is undefined unless 'level' and 'order'
        // identify a proxy previously added with 'addProxy'.

                                  // Aspects

    void swap(NetworkDescription& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    bsl::size_t numLevels() const;
        // Return the number of levels in this 'NetworkDescription' object.

    bsl::size_t numProxies(bsl::size_t level) const;
        // Return the number of proxies in the specified 'level' in this
        // 'NetworkDescription' object.  The behavior is undefined unless
        // 'level < numLevels()'.

    ProxyIterator beginLevel(bsl::size_t level) const;
        // Return the beginning iterator providing read-only access to proxies
        // in the specified 'level' in this 'NetworkDescription' object.  The
        // behavior is undefined unless 'level < numLevels()'.

    ProxyIterator endLevel(bsl::size_t level) const;
        // Return the ending iterator providing read-only access to proxies in
        // the specified 'level' in this 'NetworkDescription' object.  The
        // behavior is undefined unless 'level < numLevels()'.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const NetworkDescription& lhs, const NetworkDescription& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NetworkDescription' objects have the
    // same value if 'lhs.numLevels() == rhs.numLevels()' and every level
    // contains the same proxies in the same order in 'lhs' and 'rhs'.

bool operator!=(const NetworkDescription& lhs, const NetworkDescription& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'NetworkDescription' objects do
    // not have the same value if 'lhs.numLevels() != rhs.numLevels()' or at
    // least one level in 'lhs' has a different ordered list of proxies than
    // the corresponding level in 'rhs'.

bsl::ostream& operator<<(bsl::ostream&             output,
                         const NetworkDescription& object);
    // Write the specified 'object' to the specified 'output' in human-readable
    // format, and return a reference to 'output'.

// FREE FUNCTIONS
void swap(btls5::NetworkDescription& a, btls5::NetworkDescription& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class NetworkDescription
                          // ------------------------

// CREATORS
inline
NetworkDescription::NetworkDescription(bslma::Allocator *basicAllocator)
: d_proxies(basicAllocator)
{
}

inline
NetworkDescription::NetworkDescription(
                                     const NetworkDescription&  original,
                                     bslma::Allocator          *basicAllocator)
: d_proxies(original.d_proxies, basicAllocator)
{
}

// MANIPULATORS
inline
NetworkDescription& NetworkDescription::operator=(
                                                 const NetworkDescription& rhs)
{
    d_proxies = rhs.d_proxies;
    return *this;
}

inline
bsl::size_t NetworkDescription::addProxy(bsl::size_t            level,
                                         const btlso::Endpoint& address)
{
    return addProxy(level, address, Credentials());
}

                                  // Aspects

inline
void NetworkDescription::swap(NetworkDescription& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_proxies, &other.d_proxies);
}

// ACCESSORS
inline
bsl::size_t NetworkDescription::numLevels() const
{
    return d_proxies.size();
}

inline
bsl::size_t NetworkDescription::numProxies(bsl::size_t level) const
{
    BSLS_ASSERT_SAFE(level < d_proxies.size());

    return d_proxies[level].size();
}

inline
NetworkDescription::ProxyIterator NetworkDescription::beginLevel(
                                                       bsl::size_t level) const
{
    BSLS_ASSERT_SAFE(level < d_proxies.size());

    return d_proxies[level].begin();
}

inline
NetworkDescription::ProxyIterator NetworkDescription::endLevel(
                                                       bsl::size_t level) const
{
    BSLS_ASSERT_SAFE(level < d_proxies.size());

    return d_proxies[level].end();
}

                                  // Aspects

inline
bslma::Allocator *NetworkDescription::allocator() const
{
    return d_proxies.get_allocator().mechanism();
}

}  // close package namespace

// FREE OPERATORS
inline
bool btls5::operator==(const NetworkDescription& lhs,
                       const NetworkDescription& rhs)
{
    return lhs.d_proxies == rhs.d_proxies;
}

inline
bool btls5::operator!=(const NetworkDescription& lhs,
                       const NetworkDescription& rhs)
{
    return lhs.d_proxies != rhs.d_proxies;
}

inline
void btls5::swap(btls5::NetworkDescription& a, btls5::NetworkDescription& b)
{
    a.swap(b);
}

// TRAITS
namespace bslma {

template<>
struct UsesBslmaAllocator<btls5::NetworkDescription> : bsl::true_type {
};

}  // close namespace bslma

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
