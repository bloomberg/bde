// balxml_elementattribute.cpp                  -*-C++-*-
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
    const char               *qualifiedName,
    const char               *value,
    const char               *prefix,
    const char               *localName,
    int                       namespaceId,
    const char               *namespaceUri,
    unsigned                  flags)
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
                                    const char               *qualifiedName,
                                    const char               *value,
                                    const char               *prefix,
                                    const char               *localName,
                                    int                       namespaceId,
                                    const char               *namespaceUri,
                                    unsigned                  flags)
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
        return d_prefix;
    }

    if (! d_prefixStack) {
        return d_prefix = "";
    }

    const char* colon = bsl::strchr(d_qualifiedName, ':');
    if (! colon) {
        return d_prefix = "";
    }

    bslstl::StringRef prefix(d_qualifiedName, colon);
    d_prefix = d_prefixStack->lookupNamespacePrefix(prefix);

    return d_prefix;
}

const char *ElementAttribute::localName() const
{
    if (d_localName || ! d_qualifiedName) {
        // Local name is set or there is no qualified name.
        return d_localName;
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
        return d_namespaceId;
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
        return d_namespaceUri;
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

bsl::ostream&
ElementAttribute::print(bsl::ostream& os,
                               int           ,
                               int           ) const
{
    return os;
}
}  // close package namespace

} // Close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
