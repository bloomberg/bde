// balxml_elementattribute.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_elementattribute.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_elementattribute_cpp,"$Id$ $CSID$")

#include <balxml_prefixstack.h>
#include <balxml_namespaceregistry.h>

#include <bsl_string.h>
#include <bsl_cstring.h>

namespace BloombergLP {

namespace balxml {
ElementAttribute::ElementAttribute()
: d_prefixStack(0)
, d_qualifiedName(0)
, d_value(0)
, d_prefix(0)
, d_localName(0)
, d_namespaceId(INT_MIN)
, d_namespaceUri(0)
, d_flags(0)
{
}

ElementAttribute::ElementAttribute(
    const PrefixStack *prefixStack,
    const char        *qualifiedName,
    const char        *value,
    const char        *prefix,
    const char        *localName,
    int                namespaceId,
    const char        *namespaceUri,
    unsigned           flags)
: d_prefixStack(prefixStack)
, d_qualifiedName(qualifiedName)
, d_value(value)
, d_prefix(prefix)
, d_localName(localName)
, d_namespaceId(namespaceId)
, d_namespaceUri(namespaceUri)
, d_flags(flags)
{
}

void ElementAttribute::reset()
{
    d_prefixStack   = 0;
    d_qualifiedName = 0;
    d_value         = 0;
    d_prefix        = 0;
    d_localName     = 0;
    d_namespaceId   = INT_MIN;
    d_namespaceUri  = 0;
    d_flags         = 0;
}

void ElementAttribute::reset(const PrefixStack *prefixStack,
                             const char        *qualifiedName,
                             const char        *value,
                             const char        *prefix,
                             const char        *localName,
                             int                namespaceId,
                             const char        *namespaceUri,
                             unsigned           flags)
{
    d_prefixStack   = prefixStack;
    d_qualifiedName = qualifiedName;
    d_value         = value;
    d_prefix        = prefix;
    d_localName     = localName;
    d_namespaceId   = namespaceId;
    d_namespaceUri  = namespaceUri;
    d_flags         = flags;
}

// ACCESSORS
const char *ElementAttribute::prefix() const
{
    if (d_prefix || ! d_qualifiedName) {
        // Prefix is set or there is no qualified name.
        return d_prefix;                                              // RETURN
    }

    if (! d_prefixStack) {
        return d_prefix = "";                                         // RETURN
    }

    const char* colon = bsl::strchr(d_qualifiedName, ':');
    if (! colon) {
        return d_prefix = "";                                         // RETURN
    }

    bslstl::StringRef prefix(d_qualifiedName, colon);
    d_prefix = d_prefixStack->lookupNamespacePrefix(prefix);

    return d_prefix;
}

const char *ElementAttribute::localName() const
{
    if (d_localName || ! d_qualifiedName) {
        // Local name is set or there is no qualified name.
        return d_localName;                                           // RETURN
    }

    const char* colon = bsl::strchr(d_qualifiedName, ':');
    if (! colon) {
        // No colon.  Return entire qualified name.
        d_localName = d_qualifiedName;
    }
    else {
        // Return portion of name after colon.
        d_localName = colon + 1;
    }

    return d_localName;
}

int ElementAttribute::namespaceId() const
{
    if (INT_MIN != d_namespaceId || ! d_qualifiedName) {
        // namespace id is already set or there is no qualified name
        return d_namespaceId;                                         // RETURN
    }

    const char* nsPrefix = prefix();
    if (d_prefixStack && nsPrefix[0]) {
        // There's a prefix stack and a non-empty prefix.
        d_namespaceId = d_prefixStack->lookupNamespaceId(nsPrefix);
    }
    else {
        // There's no prefix stack or prefix is empty.
        d_namespaceId = -1;
    }

    return d_namespaceId;
}

const char *ElementAttribute::namespaceUri() const
{
    if (d_namespaceUri || ! d_qualifiedName) {
        // Namespace URI is already set or qualified name is null.
        return d_namespaceUri;                                        // RETURN
    }

    if (d_prefixStack) {
        int nsId = namespaceId();
        d_namespaceUri =
            d_prefixStack->namespaceRegistry()->lookup(nsId);
    }
    else {
        d_namespaceUri = "";
    }

    return d_namespaceUri;
}

bsl::ostream &ElementAttribute::print(bsl::ostream &stream, int, int) const
{
    return stream;
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
