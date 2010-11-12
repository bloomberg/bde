// baexml_errorinfo.cpp              -*-C++-*-
#include <baexml_errorinfo.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_errorinfo_cpp,"$Id$ $CSID$")



namespace BloombergLP  {

baexml_ErrorInfo::baexml_ErrorInfo(bslma_Allocator *basicAllocator)
: d_severity    (BAEXML_NO_ERROR)
, d_lineNumber  (0)
, d_columnNumber(0)
, d_source      (basicAllocator)
, d_message     (basicAllocator)
{
}

baexml_ErrorInfo::baexml_ErrorInfo(const baexml_ErrorInfo& other,
                                   bslma_Allocator        *basicAllocator)
: d_severity    (other.d_severity)
, d_lineNumber  (other.d_lineNumber)
, d_columnNumber(other.d_columnNumber)
, d_source      (other.d_source, basicAllocator)
, d_message     (other.d_message, basicAllocator)
{
}

baexml_ErrorInfo::~baexml_ErrorInfo()
{
}

baexml_ErrorInfo& baexml_ErrorInfo::operator=(const baexml_ErrorInfo& rhs)
{
    if (this != &rhs) {
        d_severity = rhs.d_severity;
        d_lineNumber = rhs.d_lineNumber;
        d_columnNumber = rhs.d_columnNumber;
        d_source = rhs.d_source;
        d_message = rhs.d_message;
    }

    return *this;
}

void
baexml_ErrorInfo::reset()
{
    d_severity = BAEXML_NO_ERROR;
    d_lineNumber = 0;
    d_columnNumber = 0;
    d_source.clear();
    d_message.clear();
}

void
baexml_ErrorInfo::setError(Severity               severity,
                           int                    lineNumber,
                           int                    columnNumber,
                           const bdeut_StringRef& source,
                           const bdeut_StringRef& errorMsg)
{
    if (severity > d_severity) {
        d_severity = severity;
        d_lineNumber = lineNumber;
        d_columnNumber = columnNumber;
        d_source = source;
        d_message = errorMsg;
    }
}

bool operator==(const baexml_ErrorInfo& lhs, const baexml_ErrorInfo& rhs)
{
    return (lhs.severity() == rhs.severity() &&
            lhs.lineNumber() == rhs.lineNumber() &&
            lhs.columnNumber() == rhs.columnNumber() &&
            lhs.source() == rhs.source() &&
            lhs.message() == rhs.message());
}

bsl::ostream& operator<<(bsl::ostream&           stream,
                         const baexml_ErrorInfo& errInfo)
{
    const char *severityStr;

    switch (errInfo.severity()) {
      case baexml_ErrorInfo::BAEXML_NO_ERROR:
        return stream;
      case baexml_ErrorInfo::BAEXML_WARNING:
        severityStr = "Warning";
        break;
      case baexml_ErrorInfo::BAEXML_ERROR:
        severityStr = "Error";
        break;
      case baexml_ErrorInfo::BAEXML_FATAL_ERROR:
        severityStr = "Fatal Error";
        break;
      default:
        severityStr = "Unknown Error";
        break;
    }

    stream << errInfo.source()       << ':'
           << errInfo.lineNumber()   << '.'
           << errInfo.columnNumber() << ": "
           << severityStr            << ": "
           << errInfo.message()
           << bsl::flush;

    return stream;
}

} // namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
