// baenet_httpmessagegenerator.cpp  -*-C++-*-
#include <baenet_httpmessagegenerator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpmessagegenerator_cpp,"$Id$ $CSID$")

#include <bcema_blob.h>

#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdeut_stringref.h>

#include <baenet_httpcontenttype.h>
#include <baenet_httphost.h>
#include <baenet_httprequestline.h>
#include <baenet_httpstatusline.h>
#include <baenet_httpviarecord.h>

#include <baenet_httprequestheader.h>   // for testing only
#include <baenet_httpresponseheader.h>  // for testing only

namespace BloombergLP {

namespace {

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

}  // close unnamed namespace

                     // ---------------------------------
                     // class baenet_HttpMessageGenerator
                     // ---------------------------------

// PRIVATE CLASS FUNCTIONS

void baenet_HttpMessageGenerator::generateStartLine(
                                       bsl::ostream&                 stream,
                                       const baenet_HttpRequestLine& startLine)
{
    stream << baenet_HttpRequestMethod::toString(startLine.method())
           << ' ' << startLine.requestUri()
           << " HTTP/" << startLine.majorVersion()
           << '.' << startLine.minorVersion() << "\r\n";
}

void baenet_HttpMessageGenerator::generateStartLine(
                                        bsl::ostream&                stream,
                                        const baenet_HttpStatusLine& startLine)
{
    stream << "HTTP/" << startLine.majorVersion()
           << '.' << startLine.minorVersion()
           << ' ' << startLine.statusCode()
           << ' ' << startLine.reasonPhrase() << "\r\n";
}

// MANIPULATORS

int baenet_HttpMessageGenerator::addEntityData(const bcema_Blob& data)
{
    enum { BAENET_SUCCESS = 0 };

    // TBD: assert if data length does not match Content-Length (if specified)

    d_messageDataCallback(data);

    return BAENET_SUCCESS;
}

int baenet_HttpMessageGenerator::endEntity()
{
    enum { BAENET_SUCCESS = 0 };

    return BAENET_SUCCESS;
}

              // -----------------------------------------------
              // class baenet_HttpMessageGenerator_GenerateField
              // -----------------------------------------------

// MANIPULATORS

int baenet_HttpMessageGenerator_GenerateField::execute(
                                              const int&             object,
                                              const bdeut_StringRef& fieldName)
{
    enum { BAENET_SUCCESS = 0 };

    (*d_stream_p) << fieldName << ": " << object << "\r\n";

    return BAENET_SUCCESS;
}

int baenet_HttpMessageGenerator_GenerateField::execute(
                                              const bsl::string&     object,
                                              const bdeut_StringRef& fieldName)
{
    enum { BAENET_SUCCESS = 0 };

    (*d_stream_p) << fieldName << ": " << object << "\r\n";

    return BAENET_SUCCESS;
}

int baenet_HttpMessageGenerator_GenerateField::execute(
                                              const bdet_Datetime&   object,
                                              const bdeut_StringRef& fieldName)
{
    enum { BAENET_SUCCESS = 0 };

    (*d_stream_p) << fieldName << ": ";

    printDatetimeWithTimezone(*d_stream_p, object);

    (*d_stream_p) << "\r\n";

    return BAENET_SUCCESS;
}

int baenet_HttpMessageGenerator_GenerateField::execute(
                                              const bdet_DatetimeTz& object,
                                              const bdeut_StringRef& fieldName)
{
    enum { BAENET_SUCCESS = 0 };

    (*d_stream_p) << fieldName << ": ";

    printDatetimeWithTimezone(*d_stream_p,
                              object.localDatetime(),
                              object.offset());

    (*d_stream_p) << "\r\n";

    return BAENET_SUCCESS;
}

int baenet_HttpMessageGenerator_GenerateField::execute(
                                              const baenet_HttpHost& object,
                                              const bdeut_StringRef& fieldName)
{
    enum { BAENET_SUCCESS = 0 };

    (*d_stream_p) << fieldName << ": " << object.name();

    if (!object.port().isNull()) {
        (*d_stream_p) << ":" << object.port();
    }

    (*d_stream_p) << "\r\n";

    return BAENET_SUCCESS;
}

int baenet_HttpMessageGenerator_GenerateField::execute(
                                       const baenet_HttpContentType& object,
                                       const bdeut_StringRef&        fieldName)
{
    enum { BAENET_SUCCESS = 0 };

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

    return BAENET_SUCCESS;
}

int baenet_HttpMessageGenerator_GenerateField::execute(
                                         const baenet_HttpViaRecord& object,
                                         const bdeut_StringRef&      fieldName)
{
    enum { BAENET_SUCCESS = 0 };

    (*d_stream_p) << fieldName << ": ";

    if (!object.protocolName().isNull()) {
        (*d_stream_p) << object.protocolName() << "/";
    }

    (*d_stream_p) << object.protocolVersion() << " " << object.viaHost();

    if (!object.comment().isNull()) {
        (*d_stream_p) << " (" << object.comment() << ")";
    }

    (*d_stream_p) << "\r\n";

    return BAENET_SUCCESS;
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
