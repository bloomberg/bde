#if 0
// balxml_xxxdatautil.cpp                  -*-C++-*-
#include <balxml_xxxdatautil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_xxxdatautil_cpp,"$Id$ $CSID$")

#include <balxml_decoder.h>
#include <balxml_xxxschemaparser.h>

#include <bsl_cstring.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>

#ifdef TEST
#include <balxml_minireader.h>
#include <balxml_formatter.h>
#endif // TEST

namespace BloombergLP {

namespace {

// STATIC HELPER FUNCTIONS
int prepareAggregate(bdlaggxxx::Aggregate                            *aggregate,
                     const char                                *elementName,
                     const bsl::shared_ptr<const bdlmxxx::Schema>&  schemaPtr,
                     bslma::Allocator                          *basicAllocator)
{
    const bdlmxxx::RecordDef *record = schemaPtr->lookupRecord(elementName);

    // For unnamed records, 'baexml::SchemaGenerator' synthesizes element
    // names of the form "RECORD_d+", where 'd+' indicates the index of the
    // record definition in its containing schema.

    if (! record) {

        if (bsl::strlen(elementName) <= 7
         || bsl::memcmp(elementName, "RECORD_", 7) != 0) {
            return -1;                                                // RETURN
        }

        char *endPos;
        int index = (int)bsl::strtol(elementName + 7, &endPos, 10);

        if ('\0' != *endPos || index < 0 || index >= schemaPtr->numRecords()) {
            return -1;                                                // RETURN
        }
        record = &schemaPtr->record(index);
    }

    bsl::shared_ptr<const bdlmxxx::RecordDef> recordDefPtr;
    recordDefPtr.loadAlias(schemaPtr, record);

    bdlaggxxx::Aggregate tmp(recordDefPtr, basicAllocator);
    tmp.makeValue();

    *aggregate = tmp;
    return 0;
}

bsl::shared_ptr<const bdlmxxx::Schema> parseSchema(
                                            bsl::streambuf   *xsdSchema,
                                            balxml::Reader    *reader,
                                            balxml::ErrorInfo *errorInfo,
                                            bsl::ostream     *diagnosticStream,
                                            bslma::Allocator *allocator)
{
    // Create a BDEM schema on the heap and keep it by shared pointer.

    bsl::shared_ptr<bdlmxxx::Schema> outSchemaPtr;
    outSchemaPtr.createInplace(allocator, allocator);

    // Parse XSD schema and load result into the BDEM schema.

    balxml::SchemaParser schemaParser(reader,
                                     errorInfo,
                                     diagnosticStream);
    // save xsdSchema stream position
    bsl::streambuf::pos_type pos = xsdSchema->pubseekoff(0,
                                                         bsl::ios_base::cur,
                                                         bsl::ios::in );

    bsl::string targetNamespace;

    int rc = schemaParser.parse(xsdSchema,
                                outSchemaPtr.get(),
                                &targetNamespace,
                                "Schema");

    xsdSchema->pubseekpos(pos, bsl::ios::in);  // restore stream position

    if (rc != 0) {

        if (diagnosticStream && errorInfo) {

            *diagnosticStream << *errorInfo << bsl::endl;

            //(*diagnosticStream) << "targetNamespace="
            //                    << targetNamespace
            //                    << "    =====BDE_Schema===="
            //                    << bsl::endl
            //                    << *(outSchemaPtr.get())
            //                    << bsl::endl;
        }

        outSchemaPtr.reset();
    }

    return outSchemaPtr;
}

}  // close unnamed namespace

namespace balxml {
                             // ----------------------
                             // struct DataUtil
                             // ----------------------

// CLASS METHODS
int DataUtil::createAggregateFromDocument(
                                     bdlaggxxx::Aggregate          *result,
                                     bsl::streambuf          *xmlData,
                                     bsl::streambuf          *xsdSchema,
                                     ValidatingReader *reader,
                                     ErrorInfo        *errorInfo,
                                     bsl::ostream            *diagnosticStream,
                                     bslma::Allocator        *basicAllocator)
{
    DecoderOptions decoderOptions;
    return createAggregateFromDocument(result, xmlData, xsdSchema, reader,
                                       &decoderOptions, errorInfo,
                                       diagnosticStream, basicAllocator);
}

int DataUtil::createAggregateFromDocument(
                                     bdlaggxxx::Aggregate          *result,
                                     bsl::streambuf          *xmlData,
                                     bsl::streambuf          *xsdSchema,
                                     ValidatingReader *reader,
                                     DecoderOptions   *decoderOptions,
                                     ErrorInfo        *errorInfo,
                                     bsl::ostream            *diagnosticStream,
                                     bslma::Allocator        *basicAllocator)
{
    bool oldFlag = reader->validationFlag();

    reader->enableValidation(false);
    bsl::shared_ptr<const bdlmxxx::Schema> outSchemaPtr =
                                                  parseSchema(xsdSchema,
                                                              reader,
                                                              errorInfo,
                                                              diagnosticStream,
                                                              basicAllocator);

    if (0 == outSchemaPtr.get()) {  // schema error
        reader->enableValidation(oldFlag);
        return -1;                                                    // RETURN
    }

    reader->enableValidation(true);
    reader->removeSchemas();
    reader->addSchema("INCORE_SCHEMA", xsdSchema);

    int rc = createAggregateFromDocument(result,
                                         xmlData,
                                         outSchemaPtr,
                                         reader,
                                         decoderOptions,
                                         errorInfo,
                                         diagnosticStream,
                                         basicAllocator);

   reader->enableValidation(oldFlag);
   reader->removeSchemas();

   return rc;
}

int DataUtil::createAggregateFromDocument(
                                            bdlaggxxx::Aggregate   *result,
                                            bsl::streambuf   *xmlData,
                                            bsl::streambuf   *xsdSchema,
                                            Reader    *reader,
                                            ErrorInfo *errorInfo,
                                            bsl::ostream     *diagnosticStream,
                                            bslma::Allocator *basicAllocator)
{
    DecoderOptions decoderOptions;
    return createAggregateFromDocument(result, xmlData, xsdSchema, reader,
                                       &decoderOptions, errorInfo,
                                       diagnosticStream, basicAllocator);
}

int DataUtil::createAggregateFromDocument(
                                       bdlaggxxx::Aggregate        *result,
                                       bsl::streambuf        *xmlData,
                                       bsl::streambuf        *xsdSchema,
                                       Reader         *reader,
                                       DecoderOptions *decoderOptions,
                                       ErrorInfo      *errorInfo,
                                       bsl::ostream          *diagnosticStream,
                                       bslma::Allocator      *basicAllocator)
{
    bsl::shared_ptr<const bdlmxxx::Schema> outSchemaPtr =
                                                  parseSchema(xsdSchema,
                                                              reader,
                                                              errorInfo,
                                                              diagnosticStream,
                                                              basicAllocator);

    if (0 == outSchemaPtr.get()) {  // schema error
        return -1;                                                    // RETURN
    }

    return createAggregateFromDocument(result,
                                       xmlData,
                                       outSchemaPtr,
                                       reader,
                                       decoderOptions,
                                       errorInfo,
                                       diagnosticStream,
                                       basicAllocator);
}

int DataUtil::createAggregateFromDocument(
                   bdlaggxxx::Aggregate                            *result,
                   bsl::streambuf                            *xmlData,
                   const bsl::shared_ptr<const bdlmxxx::Schema>&  schema,
                   Reader                             *reader,
                   ErrorInfo                          *errorInfo,
                   bsl::ostream                              *diagnosticStream,
                   bslma::Allocator                          *basicAllocator)
{
    DecoderOptions decoderOptions;
    return createAggregateFromDocument(result, xmlData, schema, reader,
                                       &decoderOptions, errorInfo,
                                       diagnosticStream, basicAllocator);
}

int DataUtil::createAggregateFromDocument(
                   bdlaggxxx::Aggregate                            *result,
                   bsl::streambuf                            *xmlData,
                   const bsl::shared_ptr<const bdlmxxx::Schema>&  schema,
                   Reader                             *reader,
                   DecoderOptions                     *decoderOptions,
                   ErrorInfo                          *errorInfo,
                   bsl::ostream                              *diagnosticStream,
                   bslma::Allocator                          *basicAllocator)
{

    Decoder decoder(decoderOptions, reader, errorInfo,
                           diagnosticStream, diagnosticStream);

    int rc = decoder.open(xmlData);

    if (0 == rc) {

        rc = prepareAggregate(result,
                              reader->nodeLocalName(),
                              schema,
                              basicAllocator);
    }

    // Parse data and load result into bdem_List.

    if (0 == rc) {
        rc = decoder.decode(result);
    }

    if (0 == rc) {
        return 0;                                                     // RETURN
    }

    if (diagnosticStream && errorInfo) {
        *diagnosticStream << *errorInfo << bsl::endl;
    }

    return 1;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
#endif
