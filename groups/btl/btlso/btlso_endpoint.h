// btlso_endpoint.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_ENDPOINT
#define INCLUDED_BTLSO_ENDPOINT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hostname-based address of a TCP or UDP endpoint.
//
//@CLASSES:
//  btlso::Endpoint: attributes characterizing a TCP or UDP address
//
//@SEE_ALSO: btlso_ipv4address
//
//@DESCRIPTION: This component provides a complex-constrained (value-semantic)
// attribute class, 'btlso::Endpoint', that represents a TCP or UDP endpoint as
// a hostname and port combination.  The address represented by this class is
// unresolved and unvalidated except for length, and can be used for delayed
// resolution.  For instance, the SOCKS5 protocol supports passing the hostname
// from the client to the proxy host, which may have better access for
// resolving a domain name to IP address.
//
///Attributes
///----------
//..
//  Name      Type         Default Constraints
//  --------- -----------  ------- -------------------------------------------
//  hostname  bsl::string  ""      empty or 1-255 characters in length
//  port      int          0       0 if 'hostname' is empty, else [1 .. 65535]
//..
//: o 'hostname': IP hostname
//: o 'port': TCP or UDP port
//
// A 'btlso::Endpoint' object can be in the default state, with 'port' equal to
// 0 and 'hostname' empty, or a valid hostname and port combination (as defined
// by 'isValid').  Typically such an object represents an address of a TCP or
// UDP endpoint, with 'hostname' being either a dotted-decimal address or a
// name suitable for resolution.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Set Hostname and Port
/// - - - - - - - - - - - - - - - -
// Let us encode a TCP address as a hostname and port.
//
// First, we declare an empty 'btlso::Endpoint':
//..
//  btlso::Endpoint address;
//..
// Now, we set the hostname and port of our endpoint:
//..
//  address.set("www.bloomberg.com", 80);
//..
// Finally, we have an object that describes the HTTP server at Bloomberg LP:
//..
//  assert(address.hostname() == "www.bloomberg.com");
//  assert(address.port()     == 80);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace btlso {

                        // ==============
                        // class Endpoint
                        // ==============

class Endpoint {
    // This value-semantic class characterizes a TCP, UDP, SCTP, etc. address
    // as a hostname and port.  The semantics of hostname are defined by
    // higher-level components; it may, for instance, contain a domain name
    // (requiring name resolution), or a dotted-decimal address.  Objects can
    // either have the default value, or syntactically valid hostname and port.
    // The default state is signified by the port attribute being 0 and the
    // hostname being an empty string; either one can be used as an indicator
    // of the default value.  Otherwise !both! hostname and port contain
    // syntactically valid values, as defined by 'isValid'.

    // DATA
    bsl::string d_hostname;  // hostname
    int         d_port;      // IP port

  public:
    // CLASS METHODS
    static bool isValid(const bslstl::StringRef& hostname, int port);
        // Return 'true' if the specified 'hostname' and 'port' represent a
        // valid value for a 'Endpoint' object, and 'false' otherwise.
        // 'hostname' and 'port' represent a valid 'Endpoint' value if
        // '!hostname.size() && !port' (the default value), or
        // '1 <= hostname.size()' and 'hostname.size <= 255' and '1 <= port'
        // and 'port <= 65535'.

    // CREATORS
    explicit Endpoint(bslma::Allocator *basicAllocator = 0);
        // Create a 'Endpoint' object having the (default) attribute values:
        //..
        //  hostname() == ""
        //  port()     == 0
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Endpoint(const bslstl::StringRef&  hostname,
             int                       port,
             bslma::Allocator         *basicAllocator = 0);
        // Create a Endpoint object from the specified 'hostname' and 'port'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'isValid(hostname, port)' is
        // 'true'.

    Endpoint(const Endpoint& original, bslma::Allocator *basicAllocator = 0);
        // Create a 'Endpoint' object having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~Endpoint() = default;
        // Destroy this object.

    // MANIPULATORS
    Endpoint& operator=(const Endpoint& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void set(const bslstl::StringRef& hostname, int port);
        // Set the attributes of this object to the specified 'hostname' and
        // 'port'.  The behavior is undefined unless 'isValid(hostname, port)'
        // is 'true'.

    int setIfValid(const bslstl::StringRef& hostname, int port);
        // Set the attributes of this object to the specified 'hostname' and
        // 'port' if 'isValid(hostname, port)' is 'true' and return 0.
        // Otherwise leave the value of this object unchanged and return a
        // non-zero value.

                                   // Aspects

    void swap(Endpoint& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const bsl::string& hostname() const;
        // Return a reference providing non-modifiable access to the hostname
        // attribute of this object.

    int port() const;
        // Return the port number attribute of this object.

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
bool operator==(const Endpoint& lhs, const Endpoint& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Endpoint' objects have the same
    // value if all of the corresponding values of their 'hostname' and 'port'
    // attributes are the same.

bool operator!=(const Endpoint& lhs, const Endpoint& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Endpoint' objects do not have
    // the same value if any of the corresponding values of their 'hostname'
    // and 'port' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream& stream, const Endpoint& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.

// FREE FUNCTIONS
void swap(btlso::Endpoint& a, btlso::Endpoint& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // --------------
                            // class Endpoint
                            // --------------

// CLASS METHODS
inline
bool Endpoint::isValid(const bslstl::StringRef& hostname, int port)
{
    return (1 <= hostname.length() && hostname.length() <= 255
                                                 && 1 <= port && port <= 65535)
        || (hostname.isEmpty() && 0 == port);
}

// CREATORS
inline
Endpoint::Endpoint(bslma::Allocator *basicAllocator)
: d_hostname(basicAllocator)
, d_port(0)
{
}

inline
Endpoint::Endpoint(const bslstl::StringRef&  hostname,
                   int                       port,
                   bslma::Allocator         *basicAllocator)
: d_hostname(basicAllocator)
, d_port(0)
{
    set(hostname, port);  // assert preconditions and set attributes
}

inline
Endpoint::Endpoint(const Endpoint& original, bslma::Allocator *basicAllocator)
: d_hostname(original.d_hostname, basicAllocator)
, d_port(original.d_port)
{
}

// MANIPULATORS
inline
Endpoint& Endpoint::operator=(const Endpoint& rhs)
{
    d_hostname = rhs.d_hostname;
    d_port     = rhs.d_port;
    return *this;
}

inline
void Endpoint::set(const bslstl::StringRef& hostname, int port)
{
    BSLS_ASSERT_SAFE(isValid(hostname, port));

    d_hostname.assign(hostname.begin(), hostname.end());
    d_port = port;
}

inline
int Endpoint::setIfValid(const bslstl::StringRef& hostname, int port)
{
    if (isValid(hostname, port)) {
        d_hostname.assign(hostname.begin(), hostname.end());
        d_port = port;
        return 0;                                                     // RETURN
    }
    else {
        return -1;                                                    // RETURN
    }
}

                                  // Aspects

inline
void Endpoint::swap(Endpoint& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_hostname, &other.d_hostname);
    bslalg::SwapUtil::swap(&d_port,     &other.d_port);
}

// ACCESSORS
inline
const bsl::string& Endpoint::hostname() const
{
    return d_hostname;
}

inline
int Endpoint::port() const
{
    return d_port;
}

                                  // Aspects

inline
bslma::Allocator *Endpoint::allocator() const
{
    return d_hostname.get_allocator().mechanism();
}

}  // close package namespace

// FREE OPERATORS
inline
bool btlso::operator==(const Endpoint& lhs, const Endpoint& rhs)
{
    return lhs.port() == rhs.port() && lhs.hostname() == rhs.hostname();
}

inline
bool btlso::operator!=(const Endpoint& lhs, const Endpoint& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
inline
void btlso::swap(btlso::Endpoint& a, btlso::Endpoint& b)
{
    a.swap(b);
}

// TRAITS
namespace bslma {

template<>
struct UsesBslmaAllocator<btlso::Endpoint> : bsl::true_type {
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
