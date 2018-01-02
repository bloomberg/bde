// balxml_namespaceregistry.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_NAMESPACEREGISTRY
#define INCLUDED_BALXML_NAMESPACEREGISTRY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a unique integer ID for each XML namespace.
//
//@CLASSES:
//   balxml::NamespaceRegistry: namespace-to-id mapping registry
//
//@SEE_ALSO: baexml::PrefixTable
//
//@DESCRIPTION: This component provides an in-core value-semantic type,
// 'balxml::NamespaceRegistry', that associates an integer ID with each
// registered namespace URI.  In typical usage, client code would call the
// 'lookupOrRegister' method each time it encounters a namespace URI.  The
// 'lookupOrRegister' method will return the ID corresponding to the URI,
// assigning a new ID if none already exists.  The client can also retrieve
// the ID an already-registered namespace by providing the URI to the 'lookup'
// method and can retrieve the URI of an already-registered namespace by
// providing the ID to the 'lookup' method.
//
// Note that namespace IDs may be negative.  Client code should not assume an
// incremental assignment of IDs starting at zero.  (See Preregistered
// Namespaces), below.
//
///Preregistered Namespaces
///------------------------
// Even before any namespaces have been registered, a
// 'balxml::NamespaceRegistry' can be used to lookup several preregistered
// namespaces.  The IDs for these preregistered namespaces are declared as
// constants within the 'balxml::NamespaceRegistry' class.  These constants and
// their associated URI's are as follows:
//..
//  Namespace ID               URI String
//  ============               ==========
//  BAEXML_XML                   http://www.w3.org/XML/1998/namespace
//  BAEXML_XMLNS                 http://www.w3.org/2000/xmlns/
//  BAEXML_XMLSCHEMA             http://www.w3.org/2001/XMLSchema
//  BAEXML_XMLSCHEMA_INSTANCE    http://www.w3.org/2001/XMLSchema-instance
//  BAEXML_WSDL                  http://schemas.xmlsoap.org/wsdl/
//  BAEXML_WSDL_SOAP             http://schemas.xmlsoap.org/wsdl/soap/
//  BAEXML_BDEM                  http://bloomberg.com/schemas/bdem
//..
// Note that the above constants are negative numbers.  In addition, the
// value, -1, is permanently assigned to the empty string.  The use of
// predefined namespace IDs allows client code avoid lookups of the above,
// well-known URIs.
//
///Thread Safety
///-------------
// It is safe to read or modify multiple instances of
// 'balxml::NamespaceRegistry' simultaneously, each from a separate thread.  It
// is safe to read a single instance of 'balxml::NamespaceRegistry' from
// multiple threads, provided no thread is modifying it at the same time.  It
// is not safe to read or modify an instance of 'balxml::NamespaceRegistry'
// from one thread while any other thread is modifying the same instance.
//
///Usage
///-----
// Typically, a program will register namespaces as it encounters them in an
// XML document.  Alternatively, namespaces that are important to the program
// are registered in advance, as in the following code:
//..
//  const char googleUri[] = "http://www.google.com/schemas/results.xsd";
//  const char yahooUri[]  = "http://www.yahoo.com/xsd/searchResults.xsd";
//
//  balxml::NamespaceRegistry namespaceRegistry;
//  int googleId = namespaceRegistry.lookupOrRegister(googleUri);
//  assert(googleId >= 0);
//  int yahooId = namespaceRegistry.lookupOrRegister(yahooUri);
//  assert(yahooId >= 0);
//  assert(yahooId != googleId);
//..
// Later, IDs can be looked up without concern for whether they have already
// been registered.  Any new namespaces are simply given a new ID:
//..
//  char input[100];
//
//  // First input is a new namespace URI.
//  bsl::strcpy(input, "http://www.bloomberg.com/schemas/example.xsd");
//  int id1 = namespaceRegistry.lookupOrRegister(input);
//  assert(id1 >= 0);
//  assert(id1 != googleId);
//  assert(id1 != yahooId);
//
//  // Next input happens to be the same as yahoo.
//  bsl::strcpy(input, "http://www.yahoo.com/xsd/searchResults.xsd");
//  int id2 = namespaceRegistry.lookupOrRegister(input);
//  assert(id2 == yahooId);
//..
// If one of the preregistered namespaces is presented, it's predefined ID is
// returned, even though it was never explicitly registered:
//..
//  bsl::strcpy(input, "http://www.w3.org/2001/XMLSchema");
//  int id3 = namespaceRegistry.lookupOrRegister(input);
//  assert(id3 == balxml::NamespaceRegistry::BAEXML_XMLSCHEMA);
//..
// Using the 'lookup' method, a namespace ID can be looked up without
// registering it.  In this case, an unregistered namespace will result in an
// ID of -1:
//..
//  assert(googleId  == namespaceRegistry.lookup(googleUri));
//  assert(balxml::NamespaceRegistry::BAEXML_BDEM ==
//         namespaceRegistry.lookup("http://bloomberg.com/schemas/bdem"));
//  assert(-1 == namespaceRegistry.lookup("urn:1234"));
//..
// There is also a 'lookup' method for performing the reverse mapping -- from
// ID to URI:
//..
//  const char *uri = namespaceRegistry.lookup(googleId);
//  assert(0 == bsl::strcmp(uri, googleUri));
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace balxml {
                          // =======================
                          // class NamespaceRegistry
                          // =======================

class NamespaceRegistry {
    // Mapping that associates a unique integer with each registered namespace
    // URI.

  private:
    // PRIVATE MEMBER VARIABLES
    bsl::vector<bsl::string> d_namespaces; // vector of namespaces, indexed by
                                           // the namespace ID.
    friend inline
    bool operator==(const NamespaceRegistry& lhs,
                    const NamespaceRegistry& rhs);
        // Must be a friend for engineering reasons.  Unlike most
        // value-semantic types, there is no efficient way to read the entire
        // value of a namespace registry object.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(NamespaceRegistry,
                                                    bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    enum {
        // Preregistered namespace IDs.
        e_NO_NAMESPACE = -1,    // (empty URI string)

        e_PREDEF_MIN = 0x40000000,

        e_XML = e_PREDEF_MIN, // http://www.w3.org/XML/1998/namespace
        e_XMLNS,              // http://www.w3.org/2000/xmlns/
        e_XMLSCHEMA,          // http://www.w3.org/2001/XMLSchema
        e_XMLSCHEMA_INSTANCE, // http://www.w3.org/2001/XMLSchema-instance
        e_WSDL,               // http://schemas.xmlsoap.org/wsdl/
        e_WSDL_SOAP,          // http://schemas.xmlsoap.org/wsdl/soap/
        e_BDEM,               // http://bloomberg.com/schemas/bdem

        BAEXML_PREDEF_MAX
    };

    // CREATORS
    NamespaceRegistry(bslma::Allocator *basicAllocator = 0);
        // Construct an empty registry.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the current
        // default allocator is used.

    NamespaceRegistry(const NamespaceRegistry&  other,
                      bslma::Allocator         *basicAllocator=0);
        // Construct a copy of the specified 'other' namespace registry using
        // the (optionally) specified 'basicAllocator'.  For a given URI, the
        // results of calling 'lookup' by URI will produce equal results for
        // this object and for 'other'.  For a given integer ID, the result of
        // calling 'lookup' by ID will produce different pointers that compare
        // equal using 'strcmp'.

    ~NamespaceRegistry();
        // Destroy this object.  Release all memory to the allocator used at
        // construction.

    // MANIPULATORS
    NamespaceRegistry& operator=(const NamespaceRegistry& rhs);
        // Discard the contents of this registry and assign it the contents of
        // the specified 'rhs' registry.  For a given URI, the results of
        // calling 'lookup' by URI will produce equal results for this object
        // and for 'rhs'.  For a given integer ID, the result of calling
        // 'lookup' by ID will produce different pointers that compare equal
        // using 'strcmp'.

    int lookupOrRegister(const bslstl::StringRef& namespaceUri);
        // Return the integer ID for the specified 'namespaceUri', assigning a
        // new ID if the 'namespaceUri' has not been registered before.  Note
        // that the IDs for pre-registered namespaces (including the empty
        // URI) are less than zero.  (See "Preregistered Namespaces" in the
        // 'balxml_namespaceregistry component-level documentation.)

    void reset();
        // Removes all registered namespaces.  Preregistered namespaces are
        // not removed.

    // ACCESSORS
    int lookup(const bslstl::StringRef& namespaceUri) const;
        // Return the integer ID for the specified 'namespaceUri' or -1 if the
        // namespace has not been registered.  Note that not all negative
        // return values correspond to unregistered namespaces.  Preregistered
        // namespaces always have negative IDs.  (See "Preregistered
        // Namespaces" in the 'balxml_namespaceregistry' component-level
        // documentation.)  Note that a return value of -1 can mean either an
        // unregistered namespace or an empty URI string.  This dual-use of -1
        // is deliberate and simplifies error handling in most clients.

    const char *lookup(int id) const;
        // Return the null-terminated string containing the URI of the
        // namespace registered with the specified 'id' or an empty (not null)
        // string if 'id' does not correspond to a preregistered namespace or
        // a namespace that was previously registered with this object.

    void print(bsl::ostream& stream) const;
        // Print the contents of this object to the specified 'stream' in
        // human-readable form.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS
inline
bool operator==(const NamespaceRegistry& lhs, const NamespaceRegistry& rhs);
    // Return true if the specified 'lhs' registry has the same value as the
    // specified 'rhs' registry and false otherwise.  The two registries have
    // the same value if, for any possible URI string, 'u',
    // 'lhs.lookup(u) == rhs.lookup(u)'.

inline
bool operator!=(const NamespaceRegistry& lhs, const NamespaceRegistry& rhs);
    // Return true if the specified 'lhs' registry does not have the same
    // value as the specified 'rhs' registry and false otherwise.  The two
    // registries do not have the same value if there exists a URI string,
    // 'u', such that 'lhs.lookup(u)' != 'rhs.lookup(u)'.

inline
bsl::ostream& operator<<(bsl::ostream& os, const NamespaceRegistry& r);
    // Print the contents of the specified 'r' registry to the specified 'os'
    // stream in human-readable form and return a modifiable reference to
    // 'os'.

// CREATORS
inline
NamespaceRegistry::NamespaceRegistry(bslma::Allocator *basicAllocator)
: d_namespaces(basicAllocator)
{
}

inline
NamespaceRegistry::NamespaceRegistry(const NamespaceRegistry&  other,
                                     bslma::Allocator         *basicAllocator)
: d_namespaces(other.d_namespaces, basicAllocator)
{
}

inline
NamespaceRegistry::~NamespaceRegistry()
{
}

// MANIPULATORS
inline
NamespaceRegistry&
NamespaceRegistry::operator=(const NamespaceRegistry& rhs)
{
    d_namespaces = rhs.d_namespaces;
    return *this;
}

inline
void NamespaceRegistry::reset()
{
    d_namespaces.clear();
}
}  // close package namespace

// FREE OPERATORS
inline
bool balxml::operator==(const NamespaceRegistry& lhs,
                        const NamespaceRegistry& rhs)
{
    return lhs.d_namespaces == rhs.d_namespaces;
}

inline
bool balxml::operator!=(const NamespaceRegistry& lhs,
                        const NamespaceRegistry& rhs)
{
    return ! (lhs == rhs);
}

inline
bsl::ostream& balxml::operator<<(bsl::ostream& os, const NamespaceRegistry& r)
{
    r.print(os);
    return os;
}

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BAEXML_NAMESPACEREGISTRY)

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
