// balxml_prefixstack.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_prefixstack.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_prefixstack_cpp,"$Id$ $CSID$")

#include <balxml_namespaceregistry.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>

#define ARRAY_LEN(X) int(sizeof(X)/sizeof(X[0]))

namespace BloombergLP {

namespace {

struct PredefinedPrefix {
    const char * d_prefix;
    int          d_nsid;
};

const PredefinedPrefix predefinedPrefixes[] =
{
    { "xml",   balxml::NamespaceRegistry::e_XML },
    { "xmlns", balxml::NamespaceRegistry::e_XMLNS },
    { "xsi",   balxml::NamespaceRegistry::e_XMLSCHEMA_INSTANCE }
};

const PredefinedPrefix nullPrefix = { "", -1 };

const PredefinedPrefix& lookupPredefinedPrefix(const bslstl::StringRef& prefix)
    // Return the namespace ID for the specified predefined 'prefix' or -1 if
    // 'prefix' is not predefined.  The behavior is undefined unless
    // 'prefix.data()' is non-null.
{
    for (int i = 0; i < ARRAY_LEN(predefinedPrefixes); ++i) {
        if (prefix == predefinedPrefixes[i].d_prefix) {
            return predefinedPrefixes[i];                             // RETURN
        }
    }

    return nullPrefix;
}

}  // close unnamed namespace

namespace balxml {
PrefixStack::PrefixStack(NamespaceRegistry *namespaceRegistry,
                         bslma::Allocator  *basicAllocator)
: d_namespaceRegistry(namespaceRegistry)
, d_prefixes(basicAllocator)
, d_numPrefixes(0)
{
}

int PrefixStack::pushPrefix(const bslstl::StringRef& prefix,
                            const bslstl::StringRef& namespaceUri)
{
    int nsId = d_namespaceRegistry->lookupOrRegister(namespaceUri);

    // if nsId is -1, this means namespaceUri is empty we can interpret this as
    // "undeclared namespace" example: xmlns:myprefix=""

    if ((unsigned) d_numPrefixes >= d_prefixes.size()) {
        // It is more efficient to push an empty element onto the vector then
        // fill in the values than it is to construct a pair<string,int> on the
        // stack and then copy it into the vector.
        d_prefixes.push_back(PrefixVector::value_type());
        BSLS_ASSERT((unsigned) d_numPrefixes < d_prefixes.size());
    }

    d_prefixes[d_numPrefixes].first = prefix;
    d_prefixes[d_numPrefixes].second = nsId;

    ++d_numPrefixes;

    return nsId;
}

int PrefixStack::popPrefixes(int count)
{
    if (count > d_numPrefixes) {
        count = d_numPrefixes;
    }

    d_numPrefixes -= count;
    return count;
}

int
PrefixStack::lookupNamespaceId(const bslstl::StringRef& prefix) const
{
    // Prefix registry is typically small, so linear search is appropriate.
    PrefixVector::const_iterator i = d_prefixes.begin() + d_numPrefixes;
    while (i != d_prefixes.begin()) {
        --i;
        if (i->first == prefix) {
            return i->second;                                         // RETURN
        }
    }

    return lookupPredefinedPrefix(prefix).d_nsid;
}

const char *
PrefixStack::lookupNamespacePrefix(const bslstl::StringRef& prefix) const
{
    PrefixVector::const_iterator i = d_prefixes.begin() + d_numPrefixes;
    while (i != d_prefixes.begin()) {
        --i;
        if (i->first == prefix) {
            return i->first.c_str();                                  // RETURN
        }
    }

    return lookupPredefinedPrefix(prefix).d_prefix;
}

const char *
PrefixStack::lookupNamespaceUri(const bslstl::StringRef& prefix) const
{
    int id = lookupNamespaceId(prefix);
    return  d_namespaceRegistry->lookup(id);
}

const char *
PrefixStack::lookupNamespaceUri(int nsId) const
{
    return d_namespaceRegistry->lookup(nsId);
}

const char *PrefixStack::namespaceUriByIndex(int index) const
{
    index = (index < 0 ? d_numPrefixes + index : index);
    BSLS_ASSERT(0 <= index && index < d_numPrefixes);
    return d_namespaceRegistry->lookup(d_prefixes[index].second);
}

void
PrefixStack::print(bsl::ostream& stream, bool fullNames) const
{
    PrefixVector::const_iterator it1(d_prefixes.begin());
    PrefixVector::const_iterator it2(it1 + d_numPrefixes);

    for(int i=0; it1 != it2; ++it1, ++i)
    {
        stream << it1->first << " => " << it1->second;
        if (fullNames) {
            stream << " (" << lookupNamespaceUri(it1->second) << ')';
        }
        stream << '\n';
    }

    stream << bsl::flush;
}

}  // close package namespace
}  // close enterprise namespace

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
