// btls5_proxydescription.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS5_PROXYDESCRIPTION
#define INCLUDED_BTLS5_PROXYDESCRIPTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for characterizing a SOCKS5 proxy.
//
//@CLASSES:
//  btls5::ProxyDescription: SOCKS5 proxy address and credentials
//
//@SEE_ALSO: btls5_networkdescription
//
//@DESCRIPTION: This component provides an unconstrained attribute class,
// 'btls5::ProxyDescription', describing a SOCKS5 proxy in terms of its address
// and, optionally, credentials used to authenticate with this proxy.
//
///Attributes
///----------
//..
//  Name        Type               Default                Simple Constraints
//  ----------- -----------        -------                ------------------
//  address     btlso::Endpoint    default-constructed    none
//  credentials btls5::Credentials default-constructed    none
//..
//: o 'address': address of the proxy host
//: o 'credentials': default (no credentials supplied) or username and password
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Describe a Proxy with Credentials
/// - - - - - - - - - - - - - - - - - - - - - -
// We would like to describe a proxy host that requires a username and password
// for authentication.
//
// First, we define a description with just the address (hostname and port):
//..
//  btls5::ProxyDescription proxy(btlso::Endpoint("proxy.corp.com", 1080));
//  assert(proxy.address().hostname() == "proxy.corp.com");
//  assert(proxy.address().port()     == 1080);
//..
// Now, we associate a set of credentials to authenticate connections with this
// proxy:
//..
//  proxy.setCredentials(btls5::Credentials("joe.user", "password123"));
//  assert(proxy.credentials().username() == "joe.user");
//  assert(proxy.credentials().password() == "password123");
//..
// Finally, we have a fully defined proxy description.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS5_CREDENTIALS
#include <btls5_credentials.h>
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

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace btls5 {

                           // ======================
                           // class ProxyDescription
                           // ======================

class ProxyDescription {
    // This unconstrained (value-semantic) type describes a SOCKS5 proxy in
    // terms of its address and optional authentication credentials.
    //
    // This class is *exception-neutral* and 'const' thread-safe.

    // DATA
    btlso::Endpoint d_address;      // TCP address of the proxy
    Credentials     d_credentials;  // SOCKS5 credentials

  public:
    // CREATORS
    explicit
    ProxyDescription(bslma::Allocator *basicAllocator = 0);
        // Create a 'ProxyDescription' object having default values for
        // 'address' and default credentials.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit
    ProxyDescription(const btlso::Endpoint&  address,
                     bslma::Allocator       *basicAllocator = 0);
        // Create a 'ProxyDescription' object having the specified 'address'
        // and default credentials.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ProxyDescription(const btlso::Endpoint&  address,
                     const Credentials&      credentials,
                     bslma::Allocator       *basicAllocator = 0);
        // Create a 'ProxyDescription' object having the specified 'address'
        // and 'credentials'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ProxyDescription(const ProxyDescription&  original,
                     bslma::Allocator        *basicAllocator = 0);
        // Create a 'ProxyDescription' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    //! ~ProxyDescription() = default;
        // Destroy this object.

    // MANIPULATORS
    //! ProxyDescription& operator=(const ProxyDescription& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setAddress(const btlso::Endpoint& value);
        // Set the 'address' attribute of this object to the specified 'value'.

    void setCredentials(const Credentials& value);
        // Set the 'credentials' attribute of this object to the specified
        // 'value'.

                                  // Aspects

    void swap(ProxyDescription& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const btlso::Endpoint& address() const;
        // Return a reference providing non-modifiable access to the 'address'
        // attribute of this object.

    const Credentials& credentials() const;
        // Return a reference providing non-modifiable access to the
        // 'credentials' attribute of this object.

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
bool operator==(const ProxyDescription& lhs, const ProxyDescription& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ProxyDescription' objects have the
    // same value if all of the corresponding values of their 'address' and
    // 'credentials' attributes are the same.

bool operator!=(const ProxyDescription& lhs, const ProxyDescription& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'ProxyDescription' objects do
    // not have the same value if any of the corresponding values of their
    // 'address' or 'credentials' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&           stream,
                         const ProxyDescription& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.

// FREE FUNCTIONS
void swap(btls5::ProxyDescription& a, btls5::ProxyDescription& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // ----------------------
                           // class ProxyDescription
                           // ----------------------

// CREATORS
inline
ProxyDescription::ProxyDescription(bslma::Allocator *basicAllocator)
: d_address(basicAllocator)
, d_credentials(basicAllocator)
{
}

inline
ProxyDescription::ProxyDescription(const btlso::Endpoint&  address,
                                   bslma::Allocator       *basicAllocator)
: d_address(address, basicAllocator)
, d_credentials(basicAllocator)
{
}

inline
ProxyDescription::ProxyDescription(const btlso::Endpoint&  address,
                                   const Credentials&      credentials,
                                   bslma::Allocator       *basicAllocator)
: d_address(address, basicAllocator)
, d_credentials(credentials, basicAllocator)
{
}

inline
ProxyDescription::ProxyDescription(const ProxyDescription&  original,
                                   bslma::Allocator        *basicAllocator)
: d_address(original.d_address, basicAllocator)
, d_credentials(original.d_credentials, basicAllocator)
{
}

// MANIPULATORS
inline
void ProxyDescription::setAddress(const btlso::Endpoint& value)
{
    d_address = value;
}

inline
void ProxyDescription::setCredentials(const Credentials& value)
{
    d_credentials = value;
}

                                  // Aspects

inline
void ProxyDescription::swap(ProxyDescription& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_address,     &other.d_address);
    bslalg::SwapUtil::swap(&d_credentials, &other.d_credentials);
}

// ACCESSORS
inline
const btlso::Endpoint& ProxyDescription::address() const
{
    return d_address;
}

inline
const Credentials& ProxyDescription::credentials() const
{
    return d_credentials;
}

                                  // Aspects

inline
bslma::Allocator *ProxyDescription::allocator() const
{
    return d_address.allocator();
}

}  // close package namespace

// FREE OPERATORS
inline
bool btls5::operator==(const ProxyDescription& lhs,
                       const ProxyDescription& rhs)
{
    return lhs.address()     == rhs.address()
        && lhs.credentials() == rhs.credentials();
}

inline
bool btls5::operator!=(const ProxyDescription& lhs,
                       const ProxyDescription& rhs)
{
    return lhs.address()     != rhs.address()
        || lhs.credentials() != rhs.credentials();
}

// FREE FUNCTIONS
inline
void btls5::swap(btls5::ProxyDescription& a, btls5::ProxyDescription& b)
{
    a.swap(b);
}

// TRAITS
namespace bslma {

template<>
struct UsesBslmaAllocator<btls5::ProxyDescription> : bsl::true_type {
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
