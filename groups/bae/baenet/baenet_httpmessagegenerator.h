// baenet_httpmessagegenerator.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPMESSAGEGENERATOR
#define INCLUDED_BAENET_HTTPMESSAGEGENERATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide a mechanism for generating HTTP messages.
//
//@CLASSES:
//  baenet_HttpMessageGenerator: generator for HTTP messages
//
//@SEE_ALSO: RFC 2616
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:  This component provides a 'bcema_Blob'-based HTTP message
// generator.  Given an HTTP start line, an HTTP header, and a series of entity
// data blobs, the 'baenet_HttpMessageGenerator' class will invoke a
// user-supplied callback with the HTTP message.  Note that the callback may be
// invoked multiple times as data is fed into the generator.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we are implementing a simple HTTP server that serves files to
// clients.  The following 'handleRequest' function accepts the name of the
// requested file and also a pointer to a streambuf to send the response to:
//..
//  void handleRequest(const char     *requestedFileName,
//                     bsl::streambuf *connectionToClient);
//..
// We also need a 'handleError' function that we will define later:
//..
//  void handleError(bsl::streambuf               *connectionToClient,
//                   baenet_HttpStatusCode::Value  code,
//                   const char                   *reason,
//                   bcema_BlobBufferFactory      *blobBufferFactory);
//..
// We need a callback function that we can use to send the response to the
// client:
//..
//  void sendResponseToClient(bsl::streambuf    *connectionToClient,
//                            const bcema_Blob&  data)
//  {
//      bdex_ByteOutStreamFormatter bosf(connectionToClient);
//      bcema_BlobUtil::write(bosf, data);
//  }
//..
// The implementation of the 'handleRequest' function is shown below:
//..
//  void handleRequest(const char              *requestedFileName,
//                     bsl::streambuf          *connectionToClient,
//                     bcema_BlobBufferFactory *blobBufferFactory)
//  {
//      bsl::fstream file(requestedFileName);
//
//      if (!file.is_open()) {
//          handleError(connectionToClient,
//                      baenet_HttpStatusCode::NOT_FOUND,
//                      "File not found",
//                      blobBufferFactory);
//          return;
//      }
//
//      baenet_HttpMessageGenerator generator(blobBufferFactory);
//      baenet_HttpResponseHeader   header;
//      baenet_HttpStatusLine       statusLine;
//
//      statusLine.statusCode()   = baenet_HttpStatusCode::OK;
//      statusLine.reasonPhrase() = "Here is the file";
//
//      header.generalFields().transferEncoding()
//                                = baenet_HttpTransferEncoding::BAENET_CHUNKED;
//
//      int retCode = generator.startEntity(statusLine,
//                                          header,
//                                          bdef_BindUtil::bind(
//                                                       &sendResponseToClient,
//                                                       connectionToClient,
//                                                       _1));
//      assert(0 == retCode);
//
//      while (0 <= file.rdbuf()->sgetc()) {
//          bdex_ByteInStreamFormatter bisf(file.rdbuf());
//          bcema_Blob                 data;
//
//          bcema_BlobUtil::read(bisf, &data, 1024);
//
//          retCode = generator.addEntityData(data);
//          assert(0 == retCode);
//      }
//
//      retCode = generator.endEntity();
//      assert(0 == retCode);
//  }
//..
// The 'handleRequest' function above demonstrates the usage of this component
// with entity data.  The following 'handleError' function demonstrates the
// usage of this component without entity data:
//..
//  void handleError(bsl::streambuf               *connectionToClient,
//                   baenet_HttpStatusCode::Value  code,
//                   const char                   *reason,
//                   bcema_BlobBufferFactory      *blobBufferFactory)
//  {
//      baenet_HttpMessageGenerator generator(blobBufferFactory);
//      baenet_HttpResponseHeader   header;
//      baenet_HttpStatusLine       statusLine;
//
//      statusLine.statusCode()   = code;
//      statusLine.reasonPhrase() = reason;
//
//      header.entityFields().contentLength() = 0;
//
//      int retCode = generator.startEntity(statusLine,
//                                          header,
//                                          bdef_BindUtil::bind(
//                                                       &sendResponseToClient,
//                                                       connectionToClient,
//                                                       _1));
//      assert(0 == retCode);
//
//      retCode = generator.endEntity();
//      assert(0 == retCode);
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAENET_HTTPTRANSFERENCODING
#include <baenet_httptransferencoding.h>
#endif

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCESB_BLOBSTREAMBUF
#include <bcesb_blobstreambuf.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEF_BIND
#include <bdef_bind.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDEF_MEMFN
#include <bdef_memfn.h>
#endif

#ifndef INCLUDED_BDEF_PLACEHOLDER
#include <bdef_placeholder.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif


namespace BloombergLP {

class bslma_Allocator;
class bdet_Datetime;
class bdet_DatetimeTz;

class baenet_HttpContentType;
class baenet_HttpHost;
class baenet_HttpRequestLine;
class baenet_HttpStatusLine;
class baenet_HttpViaRecord;

                     // =================================
                     // class baenet_HttpMessageGenerator
                     // =================================

class baenet_HttpMessageGenerator {
    // This class is used to generate an HTTP message.  Currently only identity
    // encoding is supported.  In the future, chunked encoding will be
    // supported.

  public:
    // TYPES
    typedef bdef_Function<void(*)(const bcema_Blob& data)> MessageDataCallback;

  private:
    // PRIVATE DATA MEMBERS
    bcema_BlobBufferFactory *d_blobBufferFactory_p;
    MessageDataCallback      d_messageDataCallback;
    bool                     d_useChunkEncoding;

    // PRIVATE CLASS FUNCTIONS
    static void generateStartLine(bsl::ostream&                 stream,
                                  const baenet_HttpRequestLine& startLine);
    static void generateStartLine(bsl::ostream&                stream,
                                  const baenet_HttpStatusLine& startLine);

  private:
    // NOT IMPLEMENTED
    baenet_HttpMessageGenerator(const baenet_HttpMessageGenerator&);
    baenet_HttpMessageGenerator& operator=(const baenet_HttpMessageGenerator&);

  public:
    // CREATORS
    explicit baenet_HttpMessageGenerator(
                                  bcema_BlobBufferFactory *blobBufferFactory,
                                  bslma_Allocator         *basicAllocator = 0);
        // Create an HTTP message generator and use the specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator will be used.

    ~baenet_HttpMessageGenerator();
        // Destroy this object.

    // MANIPULATORS
    template <typename STARTLINE_TYPE, typename HEADER_TYPE>
    int startEntity(const STARTLINE_TYPE&      startLine,
                    const HEADER_TYPE&         header,
                    const MessageDataCallback& messageDataCallback);
        // Start a new entity using the specified 'startLine' and the specified
        // 'header'.  Use the specified 'messageDataCallback' to send back the
        // HTTP message.  If 'header.basicFields().transferEncoding()'
        // evaluates to 'HttpTransferEncoding::BAENET_CHUNKED' then data must
        // be called back in chunks with the appropriate hex value prepended
        // (as defined in RFC2616).  Return 0 on success, and a non-zero value
        // otherwise.

    int addEntityData(const bcema_Blob& data);
        // Add the specified entity 'data' to the current message.  Return 0 on
        // success, and a non-zero value otherwise.

    int endEntity();
        // End the current entity.  Return 0 on success, and a non-zero value
        // otherwise.
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

              // ===============================================
              // class baenet_HttpMessageGenerator_GenerateField
              // ===============================================

class baenet_HttpMessageGenerator_GenerateField {
    // This helper class is used to generate a field as a string.

    // PRIVATE DATA MEMBERS
    bsl::ostream *d_stream_p;

    // PRIVATE MANIPULATORS
    template <typename TYPE>
    int executeImp(const TYPE&            object,
                   const bdeut_StringRef& fieldName,
                   bdeat_TypeCategory::Array);

    template <typename TYPE>
    int executeImp(const TYPE&            object,
                   const bdeut_StringRef& fieldName,
                   bdeat_TypeCategory::NullableValue);

    template <typename TYPE>
    int executeImp(const TYPE&            object,
                   const bdeut_StringRef& fieldName,
                   bdeat_TypeCategory::Enumeration);

  private:
    // NOT IMPLEMENTED
    baenet_HttpMessageGenerator_GenerateField(
                             const baenet_HttpMessageGenerator_GenerateField&);
    baenet_HttpMessageGenerator_GenerateField& operator=(
                             const baenet_HttpMessageGenerator_GenerateField&);

  public:
    // CREATORS
    explicit baenet_HttpMessageGenerator_GenerateField(bsl::ostream *stream);

    ~baenet_HttpMessageGenerator_GenerateField();

    // MANIPULATORS
    template <typename TYPE>
    int operator()(const TYPE&  object,
                   const char  *fieldName,
                   int          fieldNameLength);

    template <typename TYPE, typename INFO_TYPE>
    int operator()(const TYPE&      object,
                   const INFO_TYPE& info);

    template <typename TYPE>
    int execute(const TYPE&            object,
                const bdeut_StringRef& fieldName);

    int execute(const int&             object,
                const bdeut_StringRef& fieldName);

    int execute(const bsl::string&     object,
                const bdeut_StringRef& fieldName);

    int execute(const bdet_Datetime&   object,
                const bdeut_StringRef& fieldName);

    int execute(const bdet_DatetimeTz& object,
                const bdeut_StringRef& fieldName);

    int execute(const baenet_HttpHost& object,
                const bdeut_StringRef& fieldName);

    int execute(const baenet_HttpContentType& object,
                const bdeut_StringRef&        fieldName);

    int execute(const baenet_HttpViaRecord& object,
                const bdeut_StringRef&      fieldName);
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // class baenet_HttpMessageGenerator
                     // ---------------------------------

// CREATORS

inline
baenet_HttpMessageGenerator::baenet_HttpMessageGenerator(
                                    bcema_BlobBufferFactory *blobBufferFactory,
                                    bslma_Allocator         *basicAllocator)
: d_blobBufferFactory_p(blobBufferFactory)
, d_messageDataCallback(basicAllocator)
, d_useChunkEncoding(false)
{
    BSLS_ASSERT_SAFE(d_blobBufferFactory_p);
}

inline
baenet_HttpMessageGenerator::~baenet_HttpMessageGenerator()
{
}

// MANIPULATORS

template <typename STARTLINE_TYPE, typename HEADER_TYPE>
int baenet_HttpMessageGenerator::startEntity(
                                const STARTLINE_TYPE&      startLine,
                                const HEADER_TYPE&         header,
                                const MessageDataCallback& messageDataCallback)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    d_messageDataCallback = messageDataCallback;
    BSLS_ASSERT_SAFE(d_messageDataCallback);

    int numTransferEncodings = header.basicFields().transferEncoding().size();

    if (2 <= numTransferEncodings) {
        return BAENET_FAILURE;
    }

    d_useChunkEncoding = (0 == numTransferEncodings)
                         ? false
                         : (baenet_HttpTransferEncoding::BAENET_CHUNKED
                                == header.basicFields().transferEncoding()[0]);

    if (d_useChunkEncoding) {
        return BAENET_FAILURE;  //  chunked encoding not yet supported
    }

    bcema_Blob data(d_blobBufferFactory_p);

    {
        bcesb_OutBlobStreamBuf osb(&data);
        bsl::ostream           os(&osb);

        baenet_HttpMessageGenerator::generateStartLine(os, startLine);

        baenet_HttpMessageGenerator_GenerateField generateField(&os);

        if (0 != header.accessFields(generateField)) {
            return BAENET_FAILURE;
        }

        os << "\r\n";  // terminate header
    }

    d_messageDataCallback(data);

    return BAENET_SUCCESS;
}

              // -----------------------------------------------
              // class baenet_HttpMessageGenerator_GenerateField
              // -----------------------------------------------

// PRIVATE MANIPULATORS

template <typename TYPE>
int baenet_HttpMessageGenerator_GenerateField::executeImp(
                                              const TYPE&            object,
                                              const bdeut_StringRef& fieldName,
                                              bdeat_TypeCategory::Array)
{
    enum { BAENET_SUCCESS = 0, BAENET_FAILURE = -1 };

    int size = bdeat_ArrayFunctions::size(object);

    if (0 == size) {
        return BAENET_SUCCESS;  // common case
    }

    typedef typename
    bdeat_ArrayFunctions::ElementType<TYPE>::Type      ElementType;
    typedef bdef_Function<int (*)(const ElementType&)> Functor;

    typedef baenet_HttpMessageGenerator_GenerateField TargetClass;
    typedef bdef_MemFnInstance<int (TargetClass::*)(const ElementType&,
                                                    const bdeut_StringRef&),
                               TargetClass*>          MemFn;

    MemFn memFn(&TargetClass::execute, this);

    using bdef_PlaceHolders::_1;

    Functor generateFieldFunctor = bdef_BindUtil::bind(memFn,
                                                       _1,
                                                       fieldName);

    for (int i = 0; i < size; ++i) {
        if (0 != bdeat_ArrayFunctions::accessElement(object,
                                                     generateFieldFunctor,
                                                     i)) {
            return BAENET_FAILURE;
        }
    }

    return BAENET_SUCCESS;
}

template <typename TYPE>
int baenet_HttpMessageGenerator_GenerateField::executeImp(
                                             const TYPE&            object,
                                             const bdeut_StringRef& fieldName,
                                             bdeat_TypeCategory::NullableValue)
{
    enum { BAENET_SUCCESS = 0 };

    if (bdeat_NullableValueFunctions::isNull(object)) {
        return BAENET_SUCCESS;
    }

    typedef typename
    bdeat_NullableValueFunctions::ValueType<TYPE>::Type ValueType;
    typedef bdef_Function<int (*)(const ValueType&)>    Functor;

    typedef baenet_HttpMessageGenerator_GenerateField TargetClass;
    typedef bdef_MemFnInstance<int (TargetClass::*)(const ValueType&,
                                                    const bdeut_StringRef&),
                               TargetClass*>          MemFn;

    MemFn memFn(&TargetClass::execute, this);

    using bdef_PlaceHolders::_1;

    Functor generateFieldFunctor = bdef_BindUtil::bind(memFn,
                                                       _1,
                                                       fieldName);

    return bdeat_NullableValueFunctions::accessValue(object,
                                                     generateFieldFunctor);
}

template <typename TYPE>
int baenet_HttpMessageGenerator_GenerateField::executeImp(
                                              const TYPE&            object,
                                              const bdeut_StringRef& fieldName,
                                              bdeat_TypeCategory::Enumeration)
{
    enum { BAENET_SUCCESS = 0 };

    bsl::string value;

    bdeat_EnumFunctions::toString(&value, object);

    (*d_stream_p) << fieldName << ": " << value << "\r\n";

    return BAENET_SUCCESS;
}

// CREATORS

inline
baenet_HttpMessageGenerator_GenerateField::
                baenet_HttpMessageGenerator_GenerateField(bsl::ostream *stream)
: d_stream_p(stream)
{
}

inline
baenet_HttpMessageGenerator_GenerateField::
                                   ~baenet_HttpMessageGenerator_GenerateField()
{
}

// MANIPULATORS

template <typename TYPE>
inline
int baenet_HttpMessageGenerator_GenerateField::operator()(
                                                  const TYPE&  object,
                                                  const char  *fieldName,
                                                  int          fieldNameLength)
{
    return execute(object, bdeut_StringRef(fieldName, fieldNameLength));
}

template <typename TYPE, typename INFO_TYPE>
inline
int baenet_HttpMessageGenerator_GenerateField::operator()(
                                                       const TYPE&      object,
                                                       const INFO_TYPE& info)
{
    bdeut_StringRef fieldName(info.name(), info.nameLength());
    return execute(object, fieldName);
}

template <typename TYPE>
inline
int baenet_HttpMessageGenerator_GenerateField::execute(
                                              const TYPE&            object,
                                              const bdeut_StringRef& fieldName)
{
    typedef typename
    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    return executeImp(object, fieldName, TypeCategory());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
