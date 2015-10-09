// balxml_errorinfo.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_errorinfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_errorinfo_cpp,"$Id$ $CSID$")



namespace BloombergLP  {

namespace balxml {
ErrorInfo::ErrorInfo(bslma::Allocator *basicAllocator)
: d_severity    (e_NO_ERROR)
, d_lineNumber  (0)
, d_columnNumber(0)
, d_source      (basicAllocator)
, d_message     (basicAllocator)
{
}

ErrorInfo::ErrorInfo(const ErrorInfo& other, bslma::Allocator *basicAllocator)
: d_severity(other.d_severity)
, d_lineNumber(other.d_lineNumber)
, d_columnNumber(other.d_columnNumber)
, d_source(other.d_source, basicAllocator)
, d_message(other.d_message, basicAllocator)
{
}

ErrorInfo::~ErrorInfo()
{
}

ErrorInfo& ErrorInfo::operator=(const ErrorInfo& rhs)
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
ErrorInfo::reset()
{
    d_severity = e_NO_ERROR;
    d_lineNumber = 0;
    d_columnNumber = 0;
    d_source.clear();
    d_message.clear();
}

void ErrorInfo::setError(Severity                 severity,
                         int                      lineNumber,
                         int                      columnNumber,
                         const bslstl::StringRef& source,
                         const bslstl::StringRef& errorMsg)
{
    if (severity > d_severity) {
        d_severity = severity;
        d_lineNumber = lineNumber;
        d_columnNumber = columnNumber;
        d_source = source;
        d_message = errorMsg;
    }
}
}  // close package namespace

bool balxml::operator==(const ErrorInfo& lhs, const ErrorInfo& rhs)
{
    return (lhs.severity() == rhs.severity() &&
            lhs.lineNumber() == rhs.lineNumber() &&
            lhs.columnNumber() == rhs.columnNumber() &&
            lhs.source() == rhs.source() &&
            lhs.message() == rhs.message());
}

bsl::ostream& balxml::operator<<(bsl::ostream&    stream,
                                 const ErrorInfo& errInfo)
{
    const char *severityStr;

    switch (errInfo.severity()) {
      case ErrorInfo::e_NO_ERROR:
        return stream;                                                // RETURN
      case ErrorInfo::e_WARNING:
        severityStr = "Warning";
        break;
      case ErrorInfo::e_ERROR:
        severityStr = "Error";
        break;
      case ErrorInfo::e_FATAL_ERROR:
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

}  // close enterprise namespace

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
