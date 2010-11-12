// baexml_elementattribute.cpp                  -*-C++-*-
#include <baexml_elementattribute.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_elementattribute_cpp,"$Id$ $CSID$")

#include <baexml_prefixstack.h>
#include <baexml_namespaceregistry.h>

#include <bdeut_stringref.h>

#include <bsl_string.h>
#include <bsl_cstring.h>

namespace BloombergLP {

baexml_ElementAttribute::baexml_ElementAttribute()
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

baexml_ElementAttribute::baexml_ElementAttribute(
    const baexml_PrefixStack *prefixStack,
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

void baexml_ElementAttribute::reset()
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

void baexml_ElementAttribute::reset(const baexml_PrefixStack *prefixStack,
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
const char *baexml_ElementAttribute::prefix() const
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

    bdeut_StringRef prefix(d_qualifiedName, colon);
    d_prefix = d_prefixStack->lookupNamespacePrefix(prefix);

    return d_prefix;
}

const char *baexml_ElementAttribute::localName() const
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

int baexml_ElementAttribute::namespaceId() const
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

const char *baexml_ElementAttribute::namespaceUri() const
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
baexml_ElementAttribute::print(bsl::ostream& os,
                               int           ,
                               int           ) const
{
    return os;
}

} // Close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
