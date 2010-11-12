// baexml_util.h                  -*-C++-*-
#ifndef INCLUDED_BAEXML_UTIL
#define INCLUDED_BAEXML_UTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a suite of common XML utilities.
//
//@CLASSES:
//  baexml_Util: namespace for common XML utilities
//
//@SEE_ALSO:
//
//@AUTHOR: Alexander Libman (alibman1), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a namespace for various XML utilities.
// Included is a method for extracting the 'targetNamespace' from an XSD
// schema.  Any top-level XSD schema must have a 'targetNamespace' attribute
// in the '<schema>' element that identifies the namespace that the XSD schema
// defines.
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
//  rc = baexml_Util::extractNamespaceFromXsd(strGood, &resultNs);
//  assert(rc);
//  assert(resultNs == targetNs);
//
//  resultNs.clear();
//  rc = baexml_Util::extractNamespaceFromXsd(strBad1, &resultNs);
//  assert(!rc);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP  {

                        // ==================
                        // struct baexml_Util
                        // ==================

struct baexml_Util {

    // CLASS METHODS
    static bool extractNamespaceFromXsd(const bsl::string&  xsdSource,
                                        bsl::string        *targetNamespace);

    static bool extractNamespaceFromXsd(bsl::streambuf *xsdSource,
                                        bsl::string    *targetNamespace);
        // Extract the target namespace from the XSD schema in the specified
        // 'xsdSource' and load it into the specified 'targetNamespace'.
        // Return 'true' on success, and 'false' otherwise.  Note that a valid
        // XSD schema must have a "targetNamespace" attribute in the '<schema>'
        // element that identifies the namespace that the XSD schema defines.
        // If no such attribute is found, or the 'xsdSource' is ill-formed,
        // this method returns 'false'.  Otherwise, it returns 'true' and
        // populates the output parameter 'targetNamespace' with the value in
        // the first 'attribute="value"' pair found for the target namespace.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
