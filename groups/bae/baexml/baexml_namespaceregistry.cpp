// baexml_namespaceregistry.cpp                  -*-C++-*-
#include <baexml_namespaceregistry.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_namespaceregistry_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

#define ARRAY_LEN(X) int(sizeof(X)/sizeof(X[0]))

namespace BloombergLP {

namespace {

// The order of the elements in the following table
// must correspond to appropriate ID numbers, defined in
// header file.  Therefore ID can be used as index.

const char *const predefinedNamespaces[] =
{
    "http://www.w3.org/XML/1998/namespace",       // BAEXML_XML
    "http://www.w3.org/2000/xmlns/",              // BAEXML_XMLNS
    "http://www.w3.org/2001/XMLSchema",           // BAEXML_XMLSCHEMA
    "http://www.w3.org/2001/XMLSchema-instance",  // BAEXML_XMLSCHEMA_INSTANCE
    "http://schemas.xmlsoap.org/wsdl/",           // BAEXML_WSDL
    "http://schemas.xmlsoap.org/wsdl/soap/",      // BAEXML_WSDL_SOAP
    "http://bloomberg.com/schemas/bdem"           // BAEXML_BDEM
};

int lookupPredefinedId(const bdeut_StringRef& namespaceUri)
    // Private function.  Look up the specified 'namespaceUri' in the list of
    // preregistered namespaces and return the namespace ID or -1 if not
    // found.
{
    for (int i = 0; i < ARRAY_LEN(predefinedNamespaces); ++i) {
        if (namespaceUri == predefinedNamespaces[i]) {
            return i + baexml_NamespaceRegistry::BAEXML_PREDEF_MIN;
        }
    }

    return -1;
}

} // close unnamed namespace

int
baexml_NamespaceRegistry::lookupOrRegister(const bdeut_StringRef& namespaceUri)
{
    if (namespaceUri.length() == 0) {
        return -1;
    }

    int id = lookup(namespaceUri);
    if (id >= 0) {
        return id;
    }

    d_namespaces.push_back(namespaceUri);
    return d_namespaces.size() - 1;
}

int
baexml_NamespaceRegistry::lookup(const bdeut_StringRef& namespaceUri) const
{
    // Registry is typically small, so linear search is appropriate.
    // Since numeric ID is equal to index of string within the vector, it
    // would not be correct to sort or otherwise re-order the vector.

    if (namespaceUri.length() == 0) {
        return -1;
    }

    bsl::vector<bsl::string>::const_iterator iter;
    for (iter = d_namespaces.begin(); iter != d_namespaces.end(); ++iter) {
        if (namespaceUri == *iter) {
            // ID of namespace is equal to its index
            // within the 'd_namespaces' vector.
            return (iter - d_namespaces.begin());
        }
    }

    return lookupPredefinedId(namespaceUri);
}

const char *
baexml_NamespaceRegistry::lookup(int id) const
{
    // If id is non-negative, return registered namespace.
    if (0 <= id && size_t(id) < d_namespaces.size()) {
        return  d_namespaces[id].c_str();
    }
    else if (BAEXML_PREDEF_MIN <= id && id < BAEXML_PREDEF_MAX) {
        return predefinedNamespaces[id - BAEXML_PREDEF_MIN];
    }

    return "";
}

void
baexml_NamespaceRegistry::print(bsl::ostream& stream) const
{
    bsl::vector<bsl::string>::const_iterator it1(d_namespaces.begin());
    bsl::vector<bsl::string>::const_iterator it2(d_namespaces.end());

    for(int i=0; it1 != it2; ++it1, ++i) {
        stream << i << " => " << (*it1) << bsl::endl;
    }
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
