// baexml_datautil.cpp                  -*-C++-*-
#include <baexml_datautil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_datautil_cpp,"$Id$ $CSID$")

#include <baexml_decoder.h>
#include <baexml_schemaparser.h>

#include <bsl_cstring.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>

#ifdef TEST
#include <baexml_minireader.h>
#include <baexml_formatter.h>
#endif // TEST

namespace BloombergLP {

namespace {

// STATIC HELPER FUNCTIONS
int prepareAggregate(bcem_Aggregate                            *aggregate,
                     const char                                *elementName,
                     const bcema_SharedPtr<const bdem_Schema>&  schemaPtr,
                     bslma_Allocator                           *basicAllocator)
{
    const bdem_RecordDef *record = schemaPtr->lookupRecord(elementName);

    // For unnamed records, 'baexml_SchemaGenerator' synthesizes element
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

    bcema_SharedPtr<const bdem_RecordDef> recordDefPtr;
    recordDefPtr.loadAlias(schemaPtr, record);

    bcem_Aggregate tmp(recordDefPtr, basicAllocator);
    tmp.makeValue();

    *aggregate = tmp;
    return 0;
}

bcema_SharedPtr<const bdem_Schema> parseSchema(
                                            bsl::streambuf   *xsdSchema,
                                            baexml_Reader    *reader,
                                            baexml_ErrorInfo *errorInfo,
                                            bsl::ostream     *diagnosticStream,
                                            bslma_Allocator  *allocator)
{
    // Create a BDEM schema on the heap and keep it by shared pointer.

    bcema_SharedPtr<bdem_Schema> outSchemaPtr;
    outSchemaPtr.createInplace(allocator, allocator);

    // Parse XSD schema and load result into the BDEM schema.

    baexml_SchemaParser schemaParser(reader,
                                     errorInfo,
                                     diagnosticStream);
    // save xsdSchema stream position
    bsl::streambuf::pos_type pos = xsdSchema->pubseekoff(0,
                                                         bsl::ios_base::cur,
                                                         bsl::ios::in );

    bsl::string targetNamespace;

    int rc = schemaParser.parse(xsdSchema,
                                outSchemaPtr.ptr(),
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
            //                    << *(outSchemaPtr.ptr())
            //                    << bsl::endl;
        }

        outSchemaPtr.clear();
    }

    return outSchemaPtr;
}

}  // close unnamed namespace

                             // ----------------------
                             // struct baexml_DataUtil
                             // ----------------------

// CLASS METHODS
int baexml_DataUtil::createAggregateFromDocument(
                                     bcem_Aggregate          *result,
                                     bsl::streambuf          *xmlData,
                                     bsl::streambuf          *xsdSchema,
                                     baexml_ValidatingReader *reader,
                                     baexml_ErrorInfo        *errorInfo,
                                     bsl::ostream            *diagnosticStream,
                                     bslma_Allocator         *basicAllocator)
{
    baexml_DecoderOptions decoderOptions;
    return createAggregateFromDocument(result, xmlData, xsdSchema, reader,
                                       &decoderOptions, errorInfo,
                                       diagnosticStream, basicAllocator);
}

int baexml_DataUtil::createAggregateFromDocument(
                                     bcem_Aggregate          *result,
                                     bsl::streambuf          *xmlData,
                                     bsl::streambuf          *xsdSchema,
                                     baexml_ValidatingReader *reader,
                                     baexml_DecoderOptions   *decoderOptions,
                                     baexml_ErrorInfo        *errorInfo,
                                     bsl::ostream            *diagnosticStream,
                                     bslma_Allocator         *basicAllocator)
{
    bool oldFlag = reader->validationFlag();

    reader->enableValidation(false);
    bcema_SharedPtr<const bdem_Schema> outSchemaPtr =
                                                  parseSchema(xsdSchema,
                                                              reader,
                                                              errorInfo,
                                                              diagnosticStream,
                                                              basicAllocator);

    if (0 == outSchemaPtr.ptr()) {  // schema error
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

int baexml_DataUtil::createAggregateFromDocument(
                                            bcem_Aggregate   *result,
                                            bsl::streambuf   *xmlData,
                                            bsl::streambuf   *xsdSchema,
                                            baexml_Reader    *reader,
                                            baexml_ErrorInfo *errorInfo,
                                            bsl::ostream     *diagnosticStream,
                                            bslma_Allocator  *basicAllocator)
{
    baexml_DecoderOptions decoderOptions;
    return createAggregateFromDocument(result, xmlData, xsdSchema, reader,
                                       &decoderOptions, errorInfo,
                                       diagnosticStream, basicAllocator);
}

int baexml_DataUtil::createAggregateFromDocument(
                                       bcem_Aggregate        *result,
                                       bsl::streambuf        *xmlData,
                                       bsl::streambuf        *xsdSchema,
                                       baexml_Reader         *reader,
                                       baexml_DecoderOptions *decoderOptions,
                                       baexml_ErrorInfo      *errorInfo,
                                       bsl::ostream          *diagnosticStream,
                                       bslma_Allocator       *basicAllocator)
{
    bcema_SharedPtr<const bdem_Schema> outSchemaPtr =
                                                  parseSchema(xsdSchema,
                                                              reader,
                                                              errorInfo,
                                                              diagnosticStream,
                                                              basicAllocator);

    if (0 == outSchemaPtr.ptr()) {  // schema error
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

int baexml_DataUtil::createAggregateFromDocument(
                   bcem_Aggregate                            *result,
                   bsl::streambuf                            *xmlData,
                   const bcema_SharedPtr<const bdem_Schema>&  schema,
                   baexml_Reader                             *reader,
                   baexml_ErrorInfo                          *errorInfo,
                   bsl::ostream                              *diagnosticStream,
                   bslma_Allocator                           *basicAllocator)
{
    baexml_DecoderOptions decoderOptions;
    return createAggregateFromDocument(result, xmlData, schema, reader,
                                       &decoderOptions, errorInfo,
                                       diagnosticStream, basicAllocator);
}

int baexml_DataUtil::createAggregateFromDocument(
                   bcem_Aggregate                            *result,
                   bsl::streambuf                            *xmlData,
                   const bcema_SharedPtr<const bdem_Schema>&  schema,
                   baexml_Reader                             *reader,
                   baexml_DecoderOptions                     *decoderOptions,
                   baexml_ErrorInfo                          *errorInfo,
                   bsl::ostream                              *diagnosticStream,
                   bslma_Allocator                           *basicAllocator)
{

    baexml_Decoder decoder(decoderOptions, reader, errorInfo,
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
