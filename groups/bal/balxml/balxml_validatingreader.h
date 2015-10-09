// balxml_validatingreader.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_VALIDATINGREADER
#define INCLUDED_BALXML_VALIDATINGREADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a common reader protocol for parsing and validating XML.
//
//@CLASSES:
//  balxml::ValidatingReader: reader protocol for parsing and validating XML
//
//@SEE_ALSO: balxml_reader
//
//@DESCRIPTION: This component represents an abstract class
// 'balxml::ValidatingReader' - an XML reader that provides data validation
// against DTD or/and XML Schemas(XSD).  The 'balxml::ValidatingReader'
// inherits from the 'balxml::Reader' interface and therefore fully compliant
// with it.  In addition, 'balxml::ValidatingReader' provides additional
// methods to control the validation.  The 'enableValidation' method specifies
// what type of validation the reader should perform.  Setting 'validationFlag'
// to 'false' produces a non-validating reader.  Setting it to 'true' forces
// the reader perform the validation of input XML data against XSD schemas.
//
///Schema Location and obtaining Schemas
///-------------------------------------
// In validating mode the reader should be able obtain external XSD schemas.
// 'balxml::ValidatingReader' requires that all schema sources must be
// represented in the form of 'bsl::streambuf' objects.  According to W3C
// standard an information about external XSD schemas can be defined in three
// places:
//
//: o In an instance document, the attribute 'xsi:schemaLocation' provides
//:   hints from the author to a processor regarding the location of schema
//:   documents.  The 'schemaLocation' attribute value consists of one or more
//:   pairs of URI references, separated by white space.  The first member of
//:   each pair is a namespace name, and the second member of the pair is a
//:   hint describing where to find an appropriate schema document for that
//:   namespace.  The presence of these hints does not require the processor to
//:   obtain or use the cited schema documents, and the processor is free to
//:   use other schemas obtained by any suitable means.  For example, XercesC
//:   has a property XercesSchemaExternalSchemaLocation, that informs parser
//:   about available schemas exactly in the same format as the attribute
//:   'schemaLocation' in the document instance.
//
// Example:
//..
//      <purchaseReport
//          xmlns="http://www.example.com/Report"
//          xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
//          xsi:schemaLocation="http://www.example.com/Report
//                              http://www.example.com/Report.xsd"
//           period="P3M" periodEnding="1999-12-31">
//..
//
//: o In a schema, the 'include' element has a required 'schemaLocation'
//:   attribute, and it contains a URI reference which must identify a schema
//:   document.
//:
//: o Also in a schema, the import element has optional namespace and
//:   'schemaLocation' attributes.  If present, the 'schemaLocation' attribute
//:   is understood in a way which parallels the interpretation of
//:   'xsi:schemaLocation' in (1).  Specifically, it provides a hint from the
//:   author to a processor regarding the location of a schema document that
//:   the author warrants supplies the required components for the namespace
//:   identified by the namespace attribute.
//
// For all mentioned cases, having the URI reference which identifies a schema
// and an optional namespace, the processor(parser) should obtain
// 'bsl::streambuf' object for the schema.  For this purpose
// 'balxml::ValidatingReader' interface defines the two level schemas
// resolution process:
//
//: 1 The reader(parser) must lookup schema in internal cache.  If the schema
//:   is found, it must be used.
//:
//: 2 Otherwise reader must use the associated resolver to obtain schema (see
//:   'balxml::Reader::XmlResolverFunctor').
//
// Both the schema cache and resolver should be setup before the method 'open'
// is called.
//
///Schema Cache
///------------
// 'balxml::ValidatingReader' provides two abstract methods to maintain the
// schema cache:
//
//: o 'addSchema', add a schema to the cache
//: o 'removeSchemas', clear the cache and remove all schemas
//
///Thread Safety
///-------------
// This component does not provide any functions that present a thread safety
// issue, since the 'balxml::Reader' class is abstract and cannot be
// instantiated.  There is no guarantee that any specific derived class will
// provide a thread-safe implementation.
//
///Usage
///-----
// In this example, we will create a validating parser that parses and
// validates document again the schema.
//..
//  #include <a_xercesc_reader.h>
//
//  #include <balxml_validatingreader.h>
//  #include <balxml_errorinfo.h>
//  #include <iostream>
//  #include <strstream>
//..
// The following string describes an XSD schema for the documents we are going
// to parse:
//..
// const char TEST_XSD_STRING[] =
//    "<?xml version='1.0' encoding='UTF-8'?>"
//    "<xsd:schema xmlns:xsd='http://www.w3.org/2001/XMLSchema'"
//    "            xmlns='http://bloomberg.com/schemas/directory'"
//    "            targetNamespace='http://bloomberg.com/schemas/directory'"
//    "            elementFormDefault='qualified'"
//    "            attributeFormDefault='qualified' >"
//    " "
//    "<xsd:complexType name='entryType'>"
//    "    <xsd:sequence>"
//    "    <xsd:element name='name' type='xsd:string'/>"
//    "    <xsd:element name='phone'>"
//    "        <xsd:complexType>"
//    "        <xsd:simpleContent>"
//    "            <xsd:extension base='xsd:string'>"
//    "                <xsd:attribute name='phonetype' type='xsd:string'/>"
//    "            </xsd:extension>"
//    "        </xsd:simpleContent>"
//    "        </xsd:complexType>"
//    "    </xsd:element>"
//    "    <xsd:element name='address' type='xsd:string'/>"
//    "    </xsd:sequence>"
//    "</xsd:complexType>"
//    " "
//    "<xsd:element name='directory-entry' type='entryType'/>"
//    "</xsd:schema>";
//..
// The following string describes correct XML for a conforming schema.  The
// top-level element contains one XML namespace attribute, with one embedded
// entry describing a user:
//..
//  const char TEST_GOOD_XML_STRING[] =
//    "<?xml version='1.0' encoding='UTF-8'?>\n"
//    "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'\n"
//    "     xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'\n"
//    "     xsi:schemaLocation='http://bloomberg.com/schemas/directory  \n"
//    "                         aaa.xsd' >\n"
//    "    <name>John Smith</name>\n"
//    "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
//    "    <address/>\n"
//    "</directory-entry>\n";
//..
// The following string describes invalid XML.  More specifically, the XML
// document is well-formed, but does not conform to our schema:
//..
//  const char TEST_BAD_XML_STRING[] =
//    "<?xml version='1.0' encoding='UTF-8'?>\n"
//    "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'\n"
//    "     xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'\n"
//    "     xsi:schemaLocation='http://bloomberg.com/schemas/directory  \n"
//    "                         aaa.xsd' >\n"
//    "    <name>John Smith</name>\n"
//    "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
//    "</directory-entry>\n";
//..
// Now we define a 'parse' method for parsing an XML document and validating
// against an XSD schema:
//..
//int parse(balxml::ValidatingReader *reader,
//          const char              *xmlData,
//          const char              *xsdSchema)
//{
//..
// In order to read the XML, we first need to construct a
// 'balxml::NamespaceRegistry' object, a 'balxml::PrefixStack' object, and a
// 'TestReader' object, where 'TestReader' is a derived implementation of
// 'balxml_validatingreader'.
//..
//    balxml::NamespaceRegistry namespaces;
//    balxml::PrefixStack prefixStack(&namespaces);
//
//    ASSERT(!reader->isOpen());
//..
// The reader uses a 'balxml::PrefixStack' to manage namespace prefixes so we
// need to set it before we call open.
//..
//    reader->setPrefixStack(&prefixStack);
//    ASSERT(reader->prefixStack() == &prefixStack);
//..
// Setup validation
//..
//    reader->removeSchemas();
//
//    reader->enableValidation(true);
//    ASSERT(reader->validationFlag());
//
//    bsl::istrstream schemaStream(xsdSchema);
//    reader->addSchema("aaa.xsd", schemaStream.rdbuf());
//..
// Now we call the 'open' method to setup the reader for parsing using the data
// contained in the in the XML string.
//..
//    int rc = reader->open(xmlData, bsl::strlen(xmlData), 0, "UTF-8");
//    ASSERT(rc == 0);
//..
// Confirm that the 'bdem::Reader' has opened properly
//..
//    ASSERT(reader->isOpen());
//..
// Do actual document reading
//..
//    while(1) {
//        rc = reader->advanceToNextNode ();
//        if (rc != 0) {
//            break;
//       }
//..
//      process current node here
//..
//    }
//..
// Cleanup and close the reader.
//..
//    reader->close();
//    ASSERT(!reader->isOpen());
//
//    reader->setPrefixStack(0);
//    ASSERT(reader->prefixStack() == 0);
//
//    return rc;
//}
//..
// The main program parses an XML string using the TestReader
//..
//int usageExample()
//{
//    a_xercesc::Reader  reader;
//
//    int rc = parse(&reader, TEST_GOOD_XML_STRING, TEST_XSD_STRING);
//..
//  Normal end of data
//..
//    ASSERT(rc==1);
//
//    int rc = parse(&reader, TEST_BAD_XML_STRING, TEST_XSD_STRING);
//..
//  Parser error - document validation failed
//..
//    ASSERT(rc==-1);
//
//    return 0;
//}
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALXML_READER
#include <balxml_reader.h>
#endif

namespace BloombergLP {

namespace balxml {
                           // ======================
                           // class ValidatingReader
                           // ======================

class ValidatingReader : public Reader {
    // TBD Class description

  public:
    // CREATORS
    virtual ~ValidatingReader();
        // Destroy this object.

    // MANIPULATORS
    virtual void enableValidation(bool validationFlag) = 0;
        // Enable or disable XML validation, if the specified 'validationFlag'
        // is true and disable it otherwise.  A validating reader is often
        // faster if validation is disabled.  This operation does not take
        // effect until the next call to the 'open' method.

    virtual int addSchema(const char *location, bsl::streambuf *schema) = 0;
        // Associate the specified 'location' with the XSD document in the
        // specified 'schema' stream.  Return 0 on success and non-zero on
        // error.  This method may be called more than once to add multiple
        // location-schema associations to a pre-resolved schema cache.  During
        // XML validation, a reference to a schema with a specific location
        // will be looked up in the schema cache.  Only if this resolution
        // fails, is the schema resolver used to find the external schema.
        // (See 'setResolver' in the 'Reader' base class.)  The effects of
        // calling this method when the reader is already open is not specified
        // and a derived class implementation may treat it as an error.  The
        // behavior is undefined unless 'schema' is a repositionable stream
        // (i.e., it must be possible to read from the stream, then seek to the
        // beginning of the stream and read the same bytes again.)

    virtual void removeSchemas() = 0;
        // Remove all location-to-schema associations that were added using
        // 'addSchema'.  This method should be called each time after parsing
        // is finished and the reader would be re-used for parsing other
        // documents with different schemas.  Note that calling 'close' on the
        // reader *does* *not* remove the schemas.

    // ACCESSORS
    virtual bool validationFlag() const = 0;
        // Return true if the reader has validation turned on false otherwise.
};
}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

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
