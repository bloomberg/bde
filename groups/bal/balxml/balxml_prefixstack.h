// balxml_prefixstack.h                  -*-C++-*-
#ifndef INCLUDED_BALXML_PREFIXSTACK
#define INCLUDED_BALXML_PREFIXSTACK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a unique integer ID for each XML namespace.
//
//@CLASSES:
//  balxml::PrefixStack: stack of (namespace prefix, unique integer ID) pairs
//
//@SEE_ALSO: balxml_namespaceregistry
//
//@AUTHOR: Pablo Halpern (phalpern), Alexander Libman(alibman1)
//
//@DESCRIPTION: 'balxml::PrefixStack' keeps a collection of pairs - the prefix
// string and the integer associated with each namespace uri.  Registration of
// prefix with namespace works similar to "pushing in stack", i.e.,
// it hides the previous prefix<->namespaces association.  Deregistration
// of prefix removes the current association and opens the previous
// association of given prefix.
//
// It is safe to read or modify multiple instances of
// 'balxml::PrefixStack' simultaneously, each from a separate thread.
// It is safe to read a single instance of 'balxml::PrefixStack' from
// multiple threads, provided no thread is modifying it at the same time.
// It is not safe to read or modify an instance of
// 'balxml::PrefixStack' from one thread while any other thread is
// modifying the same instance.
// Modifying a 'balxml::PrefixStack' objects may modify the referenced
// 'balxml::NamespaceRegistry' object.  It is not safe to read or modify an
// instance of 'balxml::PrefixStack' from one thread while any other
// thread is (directly or indirectly) modifying the referenced
// 'balxml::NamespaceRegistry'.
//
///Usage
///-----
// In this example we demonstrate registering several prefixes with
// different namespaces and printing them along with their ID.
//..
//    balxml::NamespaceRegistry namespaces;
//    balxml::PrefixStack    prefixes(namespaces, allocator);
//
//    bsl::string uri1 = "http://www.google.com";
//    bsl::string uri2 = "http://www.yahoo.com";
//    bsl::string uri3 = "http://www.hotmail.com";
//    bsl::string uri4 = "http://www.msn.com";
//
//    bsl::string prefix1 = "a";
//    bsl::string prefix2 = "b";
//    bsl::string prefix3 = "c";
//
//    int namespaceId1 = prefixes.pushPrefix(prefix1, uri1);
//    int namespaceId2 = prefixes.pushPrefix(prefix2, uri2);
//    int namespaceId3 = prefixes.pushPrefix(prefix3, uri3);
//
//    bsl::cout << prefix1 << ":" << namespaceId1 << bsl::endl;
//    bsl::cout << prefix2 << ":" << namespaceId2 << bsl::endl;
//    bsl::cout << prefix3 << ":" << namespaceId3 << bsl::endl;
//
//    int namespaceId4 = prefixes.pushPrefix(prefix1, uri4);
//
//    bsl::cout << prefix1 << ":" << namespaceId1 << bsl::endl;
//
//    prefixes.popPrefix(prefix1);
//
//    bsl::cout << prefix1 << ":" << namespaceId1 << bsl::endl;
//
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace balxml {class NamespaceRegistry;

                        // ========================
                        // class PrefixStack
                        // ========================

class PrefixStack {
    // 'PrefixStack' allows associating a unique integer (namespace ID)
    // with prefix.

    // PRIVATE TYPES
    typedef bsl::vector< bsl::pair<bsl::string, int> > PrefixVector;

    // DATA
    NamespaceRegistry *d_namespaceRegistry;

    PrefixVector              d_prefixes;     // vector of pairs of namespace
                                              // prefix and integer id of the
                                              // namespace

    int                       d_numPrefixes;  // number of prefixes

  public:
    // CREATORS
    PrefixStack(NamespaceRegistry *namespaceRegistry,
                       bslma::Allocator         *basicAllocator = 0);
        // Construct an empty registry.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~PrefixStack();
        // Destroy this object.

    // TBD We are quite satisfied with compiler-generated copy constructor,
    // assignment and comparison operators

    // MANIPULATORS
    int pushPrefix(const bslstl::StringRef& prefix,
                   const bslstl::StringRef& namespaceUri);
        // Add prefix to namespace Id mapping and return namespace Id.
        // New mapping eclipses previous mapping.

    int popPrefixes(int count);
        // Remove last 'count' number prefixes.  Return the number of
        // actually removed prefixes.

    void reset();
        // Removes all prefixes from the internal collection.

    void restoreToSize(int size);
        // Restore stack to the specified 'size'.  The behavior is undefined
        // if PrefixStack contains fewer prefixes than requested size.

    // ACCESSORS
    int numPrefixes() const;
        // Return the current number of prefixes in the stack.

    NamespaceRegistry *namespaceRegistry() const;
        // Return the pointer of 'NamespaceRegistry' associated with
        // this PrefixStack.

    const char *lookupNamespacePrefix(const bslstl::StringRef& prefix) const;
        // Return a copy of the specified 'prefix' if 'prefix' is registered or
        // an empty string if 'prefix' is not registered.

    int lookupNamespaceId(const bslstl::StringRef& prefix) const;
        // Return ID of namespace or -1 if not registered.

    const char *lookupNamespaceUri(const bslstl::StringRef& prefix) const;
        // Return the URI of namespace or empty string if not
        // registered.

    const char *lookupNamespaceUri(int nsId) const;
        // Return the URI of namespace or empty string if not
        // registered.

    const char *namespacePrefixByIndex(int index) const;
        // Return the namespace prefix at the specified 'index' in the prefix
        // stack, where an 'index' of 0 is the oldest prefix on the stack.  If
        // 'index' is negative, return the prefix at position 'numPrefixes() -
        // index'.  Thus, an 'index' of -1 will return the most recent prefix
        // on the stack (i.e., the top of the stack).  The behavior is
        // undefined if 'index > numPrefixes()' or 'index < -numPrefixes()'.

    int namespaceIdByIndex(int index) const;
        // Return the namespace ID at the specified 'index' in the prefix
        // stack, where an 'index' of 0 is the oldest ID on the stack.  If
        // 'index' is negative, return the ID at position 'numPrefixes() -
        // index'.  Thus, an 'index' of -1 will return the most recent ID on
        // the stack (i.e., the top of the stack).  The behavior is undefined
        // if 'index > numPrefixes()' or 'index < -numPrefixes()'.

    const char *namespaceUriByIndex(int index) const;
        // Return the namespace URI at the specified 'index' in the prefix
        // stack, where an 'index' of 0 is the oldest URI on the stack.  If
        // 'index' is negative, return the URI at position 'numPrefixes() -
        // index'.  Thus, an 'index' of -1 will return the most recent URI on
        // the stack (i.e., the top of the stack).  The behavior is undefined
        // if 'index > numPrefixes()' or 'index < -numPrefixes()'.

    void print(bsl::ostream& stream, bool fullNames = false) const;
        // Print the content of this object to the specified 'stream'.
        // The optional parameter 'fullNames' specifies how should be printed
        // the namespaces: 'true' for the full names and 'false' only for IDs.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS
inline
PrefixStack::~PrefixStack()
{
}

// MANIPULATORS
inline
void PrefixStack::reset()
{
    d_numPrefixes = 0;
    d_prefixes.clear();
}

inline
void PrefixStack::restoreToSize(int size)
{
    BSLS_ASSERT_SAFE(size <= d_numPrefixes);
    d_numPrefixes = size;
}

// ACCESSORS
inline
NamespaceRegistry *PrefixStack::namespaceRegistry() const
{
    return d_namespaceRegistry;
}

inline
int PrefixStack::numPrefixes() const
{
    return d_numPrefixes;
}

inline
const char *PrefixStack::namespacePrefixByIndex(int index) const
{
    index = (index < 0 ? d_numPrefixes + index : index);
    BSLS_ASSERT_SAFE(0 <= index && index < d_numPrefixes);
    return d_prefixes[index].first.c_str();
}

inline
int PrefixStack::namespaceIdByIndex(int index) const
{
    index = (index < 0 ? d_numPrefixes + index : index);
    BSLS_ASSERT_SAFE(0 <= index && index < d_numPrefixes);
    return d_prefixes[index].second;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
