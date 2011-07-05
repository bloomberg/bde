// baenet_httpgeneratorutil.cpp  -*-C++-*-
#include <baenet_httpgeneratorutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpgeneratorutil_cpp,"$Id$ $CSID$")

#include <baenet_httpcontenttype.h>
#include <baenet_httphost.h>
#include <baenet_httpmessageparser.h> // for testing only
#include <baenet_httprequestline.h>
#include <baenet_httpstatusline.h>
#include <baenet_httpviarecord.h>

#include <baenet_httprequestheader.h>
#include <baenet_httpresponseheader.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcesb_blobstreambuf.h>

#include <bdeat_arrayfunctions.h>
#include <bdeat_enumfunctions.h>
#include <bdeat_nullablevaluefunctions.h>
#include <bdeat_typecategory.h>

#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdeut_stringref.h>
#include <bdex_byteoutstreamformatter.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_cstdio.h>

namespace BloombergLP {

namespace {

enum { INT32_HEX_MAX_LENGTH = 13 };

// HELPER FUNCTIONS

bsl::ostream& printDatetimeWithTimezone(bsl::ostream&        stream,
                                        const bdet_Datetime& datetime,
                                        int                  minuteOffset = 0)
{
    char oldFill = stream.fill('0');

    using bsl::setw;

    switch (datetime.dayOfWeek()) {
      case bdet_DayOfWeek::BDET_MON: stream << "Mon, ";  break;
      case bdet_DayOfWeek::BDET_TUE: stream << "Tue, ";  break;
      case bdet_DayOfWeek::BDET_WED: stream << "Wed, ";  break;
      case bdet_DayOfWeek::BDET_THU: stream << "Thu, ";  break;
      case bdet_DayOfWeek::BDET_FRI: stream << "Fri, ";  break;
      case bdet_DayOfWeek::BDET_SAT: stream << "Sat, ";  break;
      case bdet_DayOfWeek::BDET_SUN: stream << "Sun, ";  break;

      default: stream << "Err, "; break;
    }

    stream << setw(2) << datetime.day() << " ";

    switch (datetime.month()) {
      case  1: stream << "Jan "; break;
      case  2: stream << "Feb "; break;
      case  3: stream << "Mar "; break;
      case  4: stream << "Apr "; break;
      case  5: stream << "May "; break;
      case  6: stream << "Jun "; break;
      case  7: stream << "Jul "; break;
      case  8: stream << "Aug "; break;
      case  9: stream << "Sep "; break;
      case 10: stream << "Oct "; break;
      case 11: stream << "Nov "; break;
      case 12: stream << "Dec "; break;

      default: stream << "Err "; break;
    }

    stream << setw(4) << datetime.year()   << " "
           << setw(2) << datetime.hour()   << ":"
           << setw(2) << datetime.minute() << ":"
           << setw(2) << datetime.second();

    if (minuteOffset < 0) {
        stream << "-";
        minuteOffset = -minuteOffset;
    }
    else {
        stream << "+";
    }

    int hourOffset = minuteOffset / 60;
    minuteOffset   = minuteOffset % 60;

    stream << setw(2) << hourOffset
           << setw(2) << minuteOffset;

    stream.fill(oldFill);

    return stream;
}

                         // ==========================
                         // class HeaderFieldGenerator
                         // ==========================

class HeaderFieldGenerator {
    // This helper class is used to generate an HTTP header field as a string.

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
    HeaderFieldGenerator(
                             const HeaderFieldGenerator&);
    HeaderFieldGenerator& operator=(
                             const HeaderFieldGenerator&);

  public:
    // CREATORS
    explicit HeaderFieldGenerator(bsl::ostream *stream);

    ~HeaderFieldGenerator();

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

                     // --------------------------
                     // class HeaderFieldGenerator
                     // --------------------------

// PRIVATE MANIPULATORS

template <typename TYPE>
int HeaderFieldGenerator::executeImp(const TYPE&            object,
                                     const bdeut_StringRef& fieldName,
                                     bdeat_TypeCategory::Array)
{
    const int size = static_cast<int>(bdeat_ArrayFunctions::size(object));

    if (0 == size) {
        return 0;  // common case
    }

    typedef typename
    bdeat_ArrayFunctions::ElementType<TYPE>::Type      ElementType;
    typedef bdef_Function<int (*)(const ElementType&)> Functor;

    typedef HeaderFieldGenerator TargetClass;
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
            return -1;
        }
    }

    return 0;
}

template <typename TYPE>
int HeaderFieldGenerator::executeImp(const TYPE&            object,
                                     const bdeut_StringRef& fieldName,
                                     bdeat_TypeCategory::NullableValue)
{
    if (bdeat_NullableValueFunctions::isNull(object)) {
        return 0;
    }

    typedef typename
    bdeat_NullableValueFunctions::ValueType<TYPE>::Type ValueType;
    typedef bdef_Function<int (*)(const ValueType&)>    Functor;

    typedef HeaderFieldGenerator TargetClass;
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
int HeaderFieldGenerator::executeImp(const TYPE&            object,
                                     const bdeut_StringRef& fieldName,
                                     bdeat_TypeCategory::Enumeration)
{
    bsl::string value;
    bdeat_EnumFunctions::toString(&value, object);

    (*d_stream_p) << fieldName << ": " << value << "\r\n";

    return 0;
}

// CREATORS

inline
HeaderFieldGenerator::HeaderFieldGenerator(bsl::ostream *stream)
: d_stream_p(stream)
{
}

inline
HeaderFieldGenerator::~HeaderFieldGenerator()
{
}

// MANIPULATORS

template <typename TYPE>
inline
int HeaderFieldGenerator::operator()(const TYPE&  object,
                                     const char  *fieldName,
                                     int          fieldNameLength)
{
    return execute(object, bdeut_StringRef(fieldName, fieldNameLength));
}

template <typename TYPE, typename INFO_TYPE>
inline
int HeaderFieldGenerator::operator()(const TYPE&      object,
                                     const INFO_TYPE& info)
{
    bdeut_StringRef fieldName(info.name(), info.nameLength());
    return execute(object, fieldName);
}

template <typename TYPE>
inline
int HeaderFieldGenerator::execute(const TYPE&            object,
                                  const bdeut_StringRef& fieldName)
{
    typedef typename
    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    return executeImp(object, fieldName, TypeCategory());
}

int HeaderFieldGenerator::execute(const int&             object,
                                  const bdeut_StringRef& fieldName)
{
    (*d_stream_p) << fieldName << ": " << object << "\r\n";

    return 0;
}

int HeaderFieldGenerator::execute(const bsl::string&     object,
                                  const bdeut_StringRef& fieldName)
{
    (*d_stream_p) << fieldName << ": " << object << "\r\n";

    return 0;
}

int HeaderFieldGenerator::execute(const bdet_Datetime&   object,
                                  const bdeut_StringRef& fieldName)
{
    (*d_stream_p) << fieldName << ": ";

    printDatetimeWithTimezone(*d_stream_p, object);

    (*d_stream_p) << "\r\n";

    return 0;
}

int HeaderFieldGenerator::execute(const bdet_DatetimeTz& object,
                                  const bdeut_StringRef& fieldName)
{
    (*d_stream_p) << fieldName << ": ";

    printDatetimeWithTimezone(*d_stream_p,
                              object.localDatetime(),
                              object.offset());

    (*d_stream_p) << "\r\n";

    return 0;
}

int HeaderFieldGenerator::execute(const baenet_HttpHost& object,
                                  const bdeut_StringRef& fieldName)
{
    (*d_stream_p) << fieldName << ": " << object.name();

    if (!object.port().isNull()) {
        (*d_stream_p) << ":" << object.port();
    }

    (*d_stream_p) << "\r\n";

    return 0;
}

int HeaderFieldGenerator::execute(const baenet_HttpContentType& object,
                                  const bdeut_StringRef&        fieldName)
{
    (*d_stream_p) << fieldName << ": "
                  << object.type() << "/" << object.subType();

    if (!object.charset().isNull()) {
        (*d_stream_p) << "; charset=" << object.charset();
    }

    if (!object.boundary().isNull()) {
        (*d_stream_p) << "; boundary=" << object.boundary();
    }

    if (!object.id().isNull()) {
        (*d_stream_p) << "; id=" << object.id();
    }

    if (!object.name().isNull()) {
        (*d_stream_p) << "; name=" << object.name();
    }

    (*d_stream_p) << "\r\n";

    return 0;
}

int HeaderFieldGenerator::execute(const baenet_HttpViaRecord& object,
                                  const bdeut_StringRef&      fieldName)
{
    (*d_stream_p) << fieldName << ": ";

    if (!object.protocolName().isNull()) {
        (*d_stream_p) << object.protocolName() << '/';
    }

    (*d_stream_p) << object.protocolVersion()
                  << ' '
                  << object.viaHost().name();

    if (!object.viaHost().port().isNull()) {
        (*d_stream_p) << ':' << object.viaHost().port().value();
    }

    if (!object.comment().isNull()) {
        (*d_stream_p) << " (" << object.comment() << ")";
    }

    (*d_stream_p) << "\r\n";

    return 0;
}

}  // close unnamed namespace

                     // ------------------------------
                     // class baenet_HttpGeneratorUtil
                     // ------------------------------

int baenet_HttpGeneratorUtil::generateHeader(
        bcema_Blob                      *result,
        const baenet_HttpRequestLine&    requestLine,
        const baenet_HttpRequestHeader&  header)
{
    bcesb_OutBlobStreamBuf osb(result);
    return generateHeader(&osb, requestLine, header);
}

int baenet_HttpGeneratorUtil::generateHeader(
        bsl::streambuf                  *destination,
        const baenet_HttpRequestLine&    requestLine,
        const baenet_HttpRequestHeader&  header)
{
    bsl::ostream os(destination);

    os << baenet_HttpRequestMethod::toString(requestLine.method())
       << ' '
       << requestLine.requestUri()
       << " HTTP/"
       << requestLine.majorVersion()
       << '.'
       << requestLine.minorVersion()
       << "\r\n";

    if (!os) {
        return -1;
    }

    HeaderFieldGenerator generateField(&os);

    if (0 != header.accessFields(generateField)) {
        return -2;
    }

    os << "\r\n";

    if (!os) {
        return -3;
    }

    return 0;
}

int baenet_HttpGeneratorUtil::generateHeader(
        bcema_Blob                       *result,
        const baenet_HttpStatusLine&      statusLine,
        const baenet_HttpResponseHeader&  header)
{
    bcesb_OutBlobStreamBuf osb(result);
    return generateHeader(&osb, statusLine, header);
}

int baenet_HttpGeneratorUtil::generateHeader(
        bsl::streambuf                   *destination,
        const baenet_HttpStatusLine&      statusLine,
        const baenet_HttpResponseHeader&  header)
{
    bsl::ostream os(destination);

    os << "HTTP/"
       << statusLine.majorVersion()
       << '.'
       << statusLine.minorVersion()
       << ' '
       << statusLine.statusCode()
       << ' '
       << statusLine.reasonPhrase()
       << "\r\n";

    if (!os) {
        return -1;
    }

    HeaderFieldGenerator generateField(&os);

    if (0 != header.accessFields(generateField)) {
        return -2;
    }

    os << "\r\n";

    if (!os) {
        return -3;
    }

    return 0;
}

int baenet_HttpGeneratorUtil::generateBody(bcema_Blob        *result,
                                           const bcema_Blob&  data)
{
    bcema_BlobUtil::append(result, data);
    return 0;
}

int baenet_HttpGeneratorUtil::generateBody(bcema_Blob *result,
                                           const void *data,
                                           int         numBytes)
{
    bcema_BlobUtil::append(result,
                           static_cast<const char*>(data),
                           0,
                           numBytes);
    return 0;
}

int baenet_HttpGeneratorUtil::generateBody(bsl::streambuf    *destination,
                                           const bcema_Blob&  data)
{
    bdex_ByteOutStreamFormatter bosf(destination);
    bcema_BlobUtil::write(bosf, data);

    if (!bosf) {
        return -1;
    }

    return 0;
}

int baenet_HttpGeneratorUtil::generateBody(bsl::streambuf *destination,
                                           const void     *data,
                                           int             numBytes)
{
    bsl::streamsize numBytesWritten =
                     destination->sputn(static_cast<const char*>(data),
                                        numBytes);

    if (numBytesWritten != numBytes) {
        return -1;
    }

    return 0;
}

int baenet_HttpGeneratorUtil::generateBody(
        bcema_Blob                         *result,
        const bcema_Blob&                   data,
        baenet_HttpTransferEncoding::Value  encoding,
        bool                                isFinal)
{
    if (encoding == baenet_HttpTransferEncoding::BAENET_IDENTITY) {
        return generateBody(result, data);
    }
    else if (encoding == baenet_HttpTransferEncoding::BAENET_CHUNKED) {
        if (data.length() > 0) {
            char header[INT32_HEX_MAX_LENGTH];
            int  headerLength = bsl::sprintf(header, "%x\r\n", data.length());

            bcema_BlobUtil::append(result, header, 0, headerLength);
            bcema_BlobUtil::append(result, data);
            bcema_BlobUtil::append(result, "\r\n", 0, 2);
        }

        if (isFinal) {
            const char TRAILER[] = "0\r\n\r\n";
            bcema_BlobUtil::append(result, TRAILER, sizeof TRAILER - 1);
        }
    }
    else {
        BSLS_ASSERT(0);
    }

    return 0;
}

int baenet_HttpGeneratorUtil::generateBody(
        bcema_Blob                         *result,
        const void                         *data,
        int                                 length,
        baenet_HttpTransferEncoding::Value  encoding,
        bool                                isFinal)
{
    if (encoding == baenet_HttpTransferEncoding::BAENET_IDENTITY) {
        return generateBody(result, data, length);
    }
    else if (encoding == baenet_HttpTransferEncoding::BAENET_CHUNKED) {
        if (length > 0) {
            char header[INT32_HEX_MAX_LENGTH];
            int  headerLength = bsl::sprintf(header, "%x\r\n", length);

            bcema_BlobUtil::append(result, header, 0, headerLength);

            bcema_BlobUtil::append(result,
                                   static_cast<const char*>(data),
                                   0,
                                   length);

            bcema_BlobUtil::append(result, "\r\n", 0, 2);
        }

        if (isFinal) {
            const char TRAILER[] = "0\r\n\r\n";
            bcema_BlobUtil::append(result, TRAILER, sizeof TRAILER - 1);
        }
    }
    else {
        BSLS_ASSERT(0);
    }

    return 0;
}

int baenet_HttpGeneratorUtil::generateBody(
        bsl::streambuf                     *destination,
        const bcema_Blob&                   data,
        baenet_HttpTransferEncoding::Value  encoding,
        bool                                isFinal)
{
    if (encoding == baenet_HttpTransferEncoding::BAENET_IDENTITY) {
        return generateBody(destination, data);
    }
    else if (encoding == baenet_HttpTransferEncoding::BAENET_CHUNKED) {
        if (data.length() > 0) {
            char header[INT32_HEX_MAX_LENGTH];
            int  headerLength = bsl::sprintf(header, "%x\r\n", data.length());

            bsl::streamsize numBytesWritten;

            numBytesWritten = destination->sputn(header, headerLength);
            if (numBytesWritten != headerLength) {
                return -1;
            }

            {
                bdex_ByteOutStreamFormatter bosf(destination);
                bcema_BlobUtil::write(bosf, data);

                if (!bosf) {
                    return -2;
                }
            }

            numBytesWritten = destination->sputn("\r\n", 2);
            if (numBytesWritten != 2) {
                return -3;
            }
        }

        if (isFinal) {
            const char TRAILER[] = "0\r\n\r\n";
            bsl::streamsize numBytesWritten;
            numBytesWritten = destination->sputn(TRAILER, sizeof TRAILER - 1);
            if (numBytesWritten != sizeof TRAILER - 1) {
                return -4;
            }
        }
    }
    else {
        BSLS_ASSERT(0);
    }

    return 0;
}

int baenet_HttpGeneratorUtil::generateBody(
        bsl::streambuf                     *destination,
        const void                         *data,
        int                                 length,
        baenet_HttpTransferEncoding::Value  encoding,
        bool                                isFinal)
{
    if (encoding == baenet_HttpTransferEncoding::BAENET_IDENTITY) {
        return generateBody(destination, data, length);
    }
    else if (encoding == baenet_HttpTransferEncoding::BAENET_CHUNKED) {
        if (length > 0) {
            char header[INT32_HEX_MAX_LENGTH];
            int  headerLength = bsl::sprintf(header, "%x\r\n", length);

            bsl::streamsize numBytesWritten;

            numBytesWritten = destination->sputn(header, headerLength);
            if (numBytesWritten != headerLength) {
                return -1;
            }

            numBytesWritten = destination->sputn(
                                           static_cast<const char*>(data),
                                           length);
            if (numBytesWritten != length) {
                return -2;
            }

            numBytesWritten = destination->sputn("\r\n", 2);
            if (numBytesWritten != 2) {
                return -3;
            }
        }

        if (isFinal) {
            const char TRAILER[] = "0\r\n\r\n";
            bsl::streamsize numBytesWritten;
            numBytesWritten = destination->sputn(TRAILER, sizeof TRAILER - 1);
            if (numBytesWritten != sizeof TRAILER - 1) {
                return -4;
            }
        }
    }
    else {
        BSLS_ASSERT(0);
    }

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

