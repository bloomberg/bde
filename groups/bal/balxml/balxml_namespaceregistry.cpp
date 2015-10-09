// balxml_namespaceregistry.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_namespaceregistry.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_namespaceregistry_cpp,"$Id$ $CSID$")

#include <bsl_cstddef.h>
#include <bsl_ostream.h>

#define ARRAY_LEN(X) int(sizeof(X)/sizeof(X[0]))

namespace BloombergLP {

namespace {

// The order of the elements in the following table must correspond to
// appropriate ID numbers, defined in header file.  Therefore ID can be used as
// index.

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

int lookupPredefinedId(const bslstl::StringRef& namespaceUri)
    // Private function.  Look up the specified 'namespaceUri' in the list of
    // preregistered namespaces and return the namespace ID or -1 if not found.
{
    for (int i = 0; i < ARRAY_LEN(predefinedNamespaces); ++i) {
        if (namespaceUri == predefinedNamespaces[i]) {
            return i + balxml::NamespaceRegistry::e_PREDEF_MIN;       // RETURN
        }
    }

    return -1;
}

}  // close unnamed namespace

namespace balxml {
int
NamespaceRegistry::lookupOrRegister(const bslstl::StringRef& namespaceUri)
{
    if (namespaceUri.length() == 0) {
        return -1;                                                    // RETURN
    }

    int id = lookup(namespaceUri);
    if (id >= 0) {
        return id;                                                    // RETURN
    }

    d_namespaces.push_back(namespaceUri);
    return d_namespaces.size() - 1;
}

int
NamespaceRegistry::lookup(const bslstl::StringRef& namespaceUri) const
{
    // Registry is typically small, so linear search is appropriate.  Since
    // numeric ID is equal to index of string within the vector, it would not
    // be correct to sort or otherwise re-order the vector.

    if (namespaceUri.length() == 0) {
        return -1;                                                    // RETURN
    }

    bsl::vector<bsl::string>::const_iterator iter;
    for (iter = d_namespaces.begin(); iter != d_namespaces.end(); ++iter) {
        if (namespaceUri == *iter) {
            // ID of namespace is equal to its index within the 'd_namespaces'
            // vector.
            return (iter - d_namespaces.begin());                     // RETURN
        }
    }

    return lookupPredefinedId(namespaceUri);
}

const char *
NamespaceRegistry::lookup(int id) const
{
    // If id is non-negative, return registered namespace.
    if (0 <= id && size_t(id) < d_namespaces.size()) {
        return  d_namespaces[id].c_str();                             // RETURN
    }
    else if (e_PREDEF_MIN <= id && id < BAEXML_PREDEF_MAX) {
        return predefinedNamespaces[id - e_PREDEF_MIN];               // RETURN
    }

    return "";
}

void
NamespaceRegistry::print(bsl::ostream& stream) const
{
    bsl::vector<bsl::string>::const_iterator it1(d_namespaces.begin());
    bsl::vector<bsl::string>::const_iterator it2(d_namespaces.end());

    for(int i=0; it1 != it2; ++it1, ++i) {
        stream << i << " => " << (*it1) << bsl::endl;
    }
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
