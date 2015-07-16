#if 0
// balxml_xxxdatautil.h            -*-C++-*-
#ifndef INCLUDED_BALXML_XXXDATAUTIL
#define INCLUDED_BALXML_XXXDATAUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Utilities that produce bdlaggxxx::Aggregate from XML data and XML schema
//
//@AUTHOR: Alexander Libman (alibman1@bloomberg.net)
//
//@SEE_ALSO: balxml_decoder, balxml_xxxschemaparser, bdlaggxxx_aggregate
//
//@CLASSES:
//  balxml::DataUtil: namespace for functions that read XML into bdlaggxxx::Aggregate
//
//@DESCRIPTION: This component provides a utility class 'balxml::DataUtil',
// with static member functions for parsing a schema document and a
// conforming XML data document and creating a 'bdlaggxxx::Aggregate' in one step.
// The function, 'createAggregateFromDocument', is overloaded in a number of
// different ways, allowing many arguments to be defaulted.  The caller must
// supply an XML reader (derived from 'balxml::Reader'), which will be used to
// perform the XML parsing.
//
// The schema may be supplied either as XSD text, or as a managed pointer to a
// 'bdlmxxx::Schema' object.  If an XSD document is used, and if the XML reader is
// capable of performing validation, then the XML document is validated
// against the XSD schema and constraints are enforced.  (A validating reader
// may also be supplied with a 'bdlmxxx::Schema', but no validation is performed.)
//
///Usage
///-----
// We begin with a simple XSD schema document that describes a basic gift
// registry.  This XSD schema specifies constraints that are not currently
// supported by the 'bdem' messaging package (e.g., <maxLength>,
// <minExclusive> etc.).  Whether or not these constraints are enforced
// depends on the type of XML reader used, as described below.
//..
//  static const char xsdString[] =
//      "<?xml version='1.0' encoding='UTF-8' ?>\n"
//      "<schema xmlns='http://www.w3.org/2001/XMLSchema'\n"
//      "    targetNamespace='http://bloomberg.com/schemas/giftRegistry'\n"
//      "    xmlns:gr='http://bloomberg.com/schemas/giftRegistry'\n"
//      "    elementFormDefault='qualified'>\n"
//      "    <simpleType name='GiftDescription'>\n"
//      "        <restriction base='string'>\n"
//      "            <maxLength value='40'/>\n"
//      "        </restriction>\n"
//      "    </simpleType>\n"
//      "    <simpleType name='PurchaseStatus'>\n"
//      "        <restriction base='string'>\n"
//      "            <enumeration value='Received'/>\n"
//      "            <enumeration value='Ordered'/>\n"
//      "            <enumeration value='NoAction'/>\n"
//      "        </restriction>\n"
//      "    </simpleType>\n"
//      "    <simpleType name='Price'>\n"
//      "        <restriction base='float'>\n"
//      "            <minExclusive value='0.00'/>\n"
//      "            <maxInclusive value='10000.00'/>\n"
//      "        </restriction>\n"
//      "    </simpleType>\n"
//      "\n"
//      "    <complexType name='GiftItem'>\n"
//      "        <sequence>\n"
//      "            <element name='name' type='string'/>\n"
//      "            <element name='price' type='gr:Price'/>\n"
//      "            <element name='description' type='gr:GiftDescription'/>\n"
//      "            <element name='status' type='gr:PurchaseStatus'/>\n"
//      "            <element name='purchaser' type='string'/>\n"
//      "        </sequence>\n"
//      "    </complexType>\n"
//      "    <complexType name='Registry'>\n"
//      "        <sequence>\n"
//      "            <element name='createdBy' type='string'/>\n"
//      "            <element name='creationDate' type='date'/>\n"
//      "            <element name='giftItem' type='gr:GiftItem'\n"
//      "                minOccurs='0' maxOccurs='unbounded'/>\n"
//      "        </sequence>\n"
//      "    </complexType>\n"
//      "    <element name='registry' type='gr:Registry'/>\n"
//      "</schema>\n";
//..
// Now we create an XML data document that conforms to (and references) the
// above schema.  Although the document specifies a 'schemaLocation' of
// "giftRegistry.xml", the 'createAggregateFromDocument' function will ignore
// this hint and instead use the explicitly-supplied schema document.
//..
//  const char xmlString[] =
//      "<?xml version='1.0' encoding='UTF-8' ?>\n"
//      "<registry xmlns='http://bloomberg.com/schemas/giftRegistry'\n"
//      "     xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'\n"
//      "     xsi:schemaLocation='http://bloomberg.com/schemas/giftRegistry "
//      "giftRegistry.xsd'>\n"
//      "    <createdBy>Garfield The FatCat</createdBy>\n"
//      "    <creationDate>2004-11-24</creationDate>\n"
//      "    <giftItem>\n"
//      "        <name>turkey flavored donuts</name>\n"
//      "        <price>4.97</price>\n"
//      "        <description>Only available at Drunken Donuts</description>\n"
//      "        <status>Received</status>\n"
//      "        <purchaser>Odie</purchaser>\n"
//      "    </giftItem>\n"
//      "    <giftItem>\n"
//      "        <name>King size cat bed</name>\n"
//      "        <price>299.99</price>\n"
//      "        <description>Please go to Beds R Cats</description>\n"
//      "        <status>Ordered</status>\n"
//      "        <purchaser>Jon</purchaser>\n"
//      "    </giftItem>\n"
//      "</registry>\n";
//..
// In order to parse the schema and data documents, we must present them as
// streams.  The 'bdlsb::FixedMemInStreamBuf' class is well suited for
// presenting a fixed-length string as a stream:
//..
//  bdlsb::FixedMemInStreamBuf xmlDoc(xmlString, bsl::strlen(xmlString));
//  bdlsb::FixedMemInStreamBuf xsdDoc(xsdString, bsl::strlen(xsdString));
//..
// Now we must create a reader object, an aggregate object to receive the
// output, and an error object to capture error messages.  In this example we
// are using 'balxml::MiniReader', which is a fast, but non-validating reader.
// The XSD constraints such as <maxLength>, <minExclusive> are ignored by
// non-validating readers.  If we were to use a validating reader instead,
// then the XML data would be validated against the original schema, enforcing
// all of the constraints.
//..
//  balxml::MiniReader reader;
//  bdlaggxxx::Aggregate    aggregate;
//  balxml::ErrorInfo  errInfo;
//..
// Using the XSD schema and XML document as input, we use 'balxml::DataUtil' to
// populate the aggregate object:
//..
//  int status = balxml::DataUtil::createAggregateFromDocument(&aggregate,
//                                                            &xmlDoc,
//                                                            &xsdDoc,
//                                                            &reader,
//                                                            &errInfo);
//
//  assert(0 == status);
//  if (status) {
//      bsl::cerr << "Data error: " << errInfo;
//      return -2;
//  }
//..
// Finally, we verify that the aggregate contains the data from the XML
// document:
//..
//  assert(aggregate["createdBy"].asString() == "Garfield The FatCat");
//  assert(aggregate["creationDate"].asDateTz().localDate() ==
//         bdlt::Date(2004,11,24));
//  assert(aggregate["giftItem"].length() == 2);
//
//  bdlaggxxx::Aggregate item = aggregate["giftItem"][0];
//  assert(item["name"].asString() == "turkey flavored donuts");
//  assert(bsl::fabs(item["price"].asFloat() - 4.97) < 0.001);
//  assert(item["description"].asString()=="Only available at Drunken Donuts");
//  assert(item["status"].asInt() == 2); // Received
//  assert(item["purchaser"].asString() == "Odie");
//
//  item = aggregate["giftItem"][1];
//  assert(item["name"].asString() == "King size cat bed");
//  assert(bsl::fabs(item["price"].asFloat() - 299.99) < 0.001);
//  assert(item["description"].asString()=="Please go to Beds R Cats");
//  assert(item["status"].asInt() == 1); // Ordered
//  assert(item["purchaser"].asString() == "Jon");
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALXML_DECODEROPTIONS
#include <balxml_decoderoptions.h>
#endif

#ifndef INCLUDED_BALXML_READER
#include <balxml_reader.h>
#endif

#ifndef INCLUDED_BALXML_VALIDATINGREADER
#include <balxml_validatingreader.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_AGGREGATE
#include <bdlaggxxx_aggregate.h>
#endif

#ifndef INCLUDED_BDLMXXX_SCHEMA
#include <bdlmxxx_schema.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {

namespace balxml {
                             // ======================
                             // struct DataUtil
                             // ======================

struct DataUtil {
    // namespace for functions that read XML into bdlaggxxx::Aggregate

    // CLASS METHODS
    static int createAggregateFromDocument(
                bdlaggxxx::Aggregate          *result,
                bsl::streambuf          *xmlData,
                bsl::streambuf          *xsdSchema,
                ValidatingReader *reader,
                ErrorInfo        *errorInfo = 0,
                bsl::ostream            *diagnosticStream = 0,
                bslma::Allocator        *basicAllocator = 0);
    static int createAggregateFromDocument(
                bdlaggxxx::Aggregate          *result,
                bsl::streambuf          *xmlData,
                bsl::streambuf          *xsdSchema,
                ValidatingReader *reader,
                DecoderOptions   *decoderOptions,
                ErrorInfo        *errorInfo = 0,
                bsl::ostream            *diagnosticStream = 0,
                bslma::Allocator        *basicAllocator = 0);
    static int createAggregateFromDocument(
                bdlaggxxx::Aggregate          *result,
                bsl::streambuf          *xmlData,
                bsl::streambuf          *xsdSchema,
                Reader           *reader,
                ErrorInfo        *errorInfo = 0,
                bsl::ostream            *diagnosticStream = 0,
                bslma::Allocator        *basicAllocator = 0);
    static int createAggregateFromDocument(
                bdlaggxxx::Aggregate          *result,
                bsl::streambuf          *xmlData,
                bsl::streambuf          *xsdSchema,
                Reader           *reader,
                DecoderOptions   *decoderOptions,
                ErrorInfo        *errorInfo = 0,
                bsl::ostream            *diagnosticStream = 0,
                bslma::Allocator        *basicAllocator = 0);
        // Set the specified 'result' aggregate to the structure obtained by
        // parsing the specified 'xsdSchema' XSD document and the data
        // obtained by parsing the specified 'xmlData' XML document, using the
        // specified 'reader' to perform the XML and XSD parsing and return 0
        // on success and non-zero on error.  The optionally-specified
        // 'decoderOptions', if any, are used to control the way that the
        // 'xmlData' is parsed.  (See the 'balxml_decoder' and
        // 'balxml_decoderoptions' components.)  The most serious parsing
        // error are reported to the caller via the optionally-specified
        // 'errorInfo' object, if supplied.  Trace and error information is
        // written to the optionally-specified 'diagnosticStream', if
        // supplied.  The 'result' aggregate allocate memory from the
        // optionally-specified 'basicAllocator', if supplied, or from the
        // default allocator if 'basicAllocator' is zero.  If the reader is a
        // validating reader, then the data will be validated against the
        // schema.

    static int createAggregateFromDocument(
               bdlaggxxx::Aggregate                            *result,
               bsl::streambuf                            *xmlData,
               const bsl::shared_ptr<const bdlmxxx::Schema>&  schema,
               Reader                             *reader,
               ErrorInfo                          *errorInfo = 0,
               bsl::ostream                              *diagnosticStream = 0,
               bslma::Allocator                          *basicAllocator = 0);
    static int createAggregateFromDocument(
               bdlaggxxx::Aggregate                            *result,
               bsl::streambuf                            *xmlData,
               const bsl::shared_ptr<const bdlmxxx::Schema>&  schema,
               Reader                             *reader,
               DecoderOptions                     *decoderOptions,
               ErrorInfo                          *errorInfo = 0,
               bsl::ostream                              *diagnosticStream = 0,
               bslma::Allocator                          *basicAllocator = 0);
        // Set the specified 'result' aggregate to the structure described by
        // the specified 'schema' and the data
        // obtained by parsing the specified 'xmlData' XML document, using the
        // specified 'reader' to perform the XSD parsing and return 0
        // on success and non-zero on error.  The optionally-specified
        // 'decoderOptions', if any, are used to control the way that the
        // 'xmlData' is parsed.  (See the 'balxml_decoder' and
        // 'balxml_decoderoptions' components.)  The most serious parsing
        // error are reported to the caller via the optionally-specified
        // 'errorInfo' object, if supplied.  Trace and error information is
        // written to the optionally-specified 'diagnosticStream', if
        // supplied.  The 'result' aggregate allocate memory from the
        // optionally-specified 'basicAllocator', if supplied, or from the
        // default allocator if 'basicAllocator' is zero.
};
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
#endif
