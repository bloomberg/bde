// balxml_util.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_UTIL
#define INCLUDED_BALXML_UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of common XML utilities.
//
//@CLASSES:
//  balxml::Util: namespace for common XML utilities
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a namespace, 'balxml::Util', for
// various XML utilities.  Included is a method for extracting the
// 'targetNamespace' from an XSD schema.  Any top-level XSD schema must have a
// 'targetNamespace' attribute in the '<schema>' element that identifies the
// namespace that the XSD schema defines.
//
// Note that a valid XSD file must have this attribute for the root element
// '<schema>' and the only place that the token "targetNamespace" can appear
// prior to its occurrence as the required attribute is in the comment
// string(s) before '<schema>'.  And any comments cannot be embedded in other
// comments or inside a tag.  The extraction algorithm relies on these XML
// facts to jump over any comments prior to the first occurrence of
// "targetNamespace" and extract the attribute value.  It leaves all other
// validation of 'xsdSource' to a parser.
//
///Usage
///-----
//..
//  // 'etalon' targetNamespace:
//  static const char *targetNs = "http://localhost:2000/calc.wsdl";
//
//  // well-formed and correct XSD:
//  static const char goodSchema[] =
//      "<?xml version='1.0' encoding='UTF-8'?>"
//      " <schema targetNamespace='http://localhost:2000/calc.wsdl'"
//      "        elementFormDefault='qualified'"
//      "        xmlns='http://www.w3.org/2001/XMLSchema'"
//      "        xmlns:xs='http://www.w3.org/2001/XMLSchema'"
//      "        xmlns:bdem='http://bloomberg.com/schemas/bdem'"
//      "        xmlns:m_bascalc='http://localhost:2000/calc.wsdl'"
//      "        xmlns:tns='http://localhost:2000/calc.wsdl'"
//      "   <xs:complexType name='Options'>"
//      "   <xs:sequence>"
//      "      <xs:element name='MaxDepth' type='xs:int'"
//      "                  minOccurs='0' maxOccurs='1'"
//      "                  default='32'"
//      "                  bdem:allowsDirectManipulation='0'>"
//      "      </xs:element>"
//      "    </xs:sequence>"
//      "  </xs:complexType>"
//      "  <complexType name='Configuration'>"
//      "    <sequence>"
//      "      <element name='Options' type='m_bascalc:Options'/>"
//      "    </sequence>"
//      "  </complexType>"
//      "  <element name='Configuration' type='m_bascalc:Options'/>"
//      "</schema>";
//
//  // well-formed XSD with no target namespace:
//  static const char badSchema1[] =
//      "<?xml version='1.0' encoding='UTF-8'?>"
//      "        elementFormDefault='qualified'"
//      "        xmlns='http://www.w3.org/2001/XMLSchema'"
//      "        xmlns:xs='http://www.w3.org/2001/XMLSchema'"
//      "        xmlns:bdem='http://bloomberg.com/schemas/bdem'"
//      "        xmlns:m_bascalc='http://localhost:2000/calc.wsdl'"
//      "        xmlns:tns='http://localhost:2000/calc.wsdl'"
//      "   <xs:complexType name='Options'>"
//      "   <xs:sequence>"
//      "      <xs:element name='MaxDepth' type='xs:int'"
//      "                  minOccurs='0' maxOccurs='1'"
//      "                  default='32'"
//      "                  bdem:allowsDirectManipulation='0'>"
//      "      </xs:element>"
//      "    </xs:sequence>"
//      "  </xs:complexType>"
//      "  <complexType name='Configuration'>"
//      "    <sequence>"
//      "      <element name='Options' type='m_bascalc:Options'/>"
//      "    </sequence>"
//      "  </complexType>"
//      "  <element name='Configuration' type='m_bascalc:Options'/>"
//      "</schema>";
//
//  bsl::string strGood(goodSchema);
//  bsl::string strBad1(badSchema1);
//
//  bsl::string resultNs;
//  bool rc;
//
//  rc = balxml::Util::extractNamespaceFromXsd(strGood, &resultNs);
//  assert(rc);
//  assert(resultNs == targetNs);
//
//  resultNs.clear();
//  rc = balxml::Util::extractNamespaceFromXsd(strBad1, &resultNs);
//  assert(!rc);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP  {

namespace balxml {
                                // ===========
                                // struct Util
                                // ===========

struct Util {

    // CLASS METHODS
    static bool extractNamespaceFromXsd(const bsl::string&  xsdSource,
                                        bsl::string        *targetNamespace);

    static bool extractNamespaceFromXsd(bsl::streambuf *xsdSource,
                                        bsl::string    *targetNamespace);
        // Extract the target namespace from the XSD schema in the specified
        // 'xsdSource' and load it into the specified 'targetNamespace'.
        // Return 'true' on success, and 'false' otherwise.  Note that a valid
        // XSD schema must have a 'targetNamespace' attribute in the '<schema>'
        // element that identifies the namespace that the XSD schema defines.
        // If no such attribute is found, or the 'xsdSource' is ill-formed,
        // this method returns 'false'.  Otherwise, it returns 'true' and
        // populates the output parameter 'targetNamespace' with the value in
        // the first 'attribute="value"' pair found for the target namespace.
};
}  // close package namespace

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
