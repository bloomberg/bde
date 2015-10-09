// ball_defaultobserver.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_defaultobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_defaultobserver_cpp,"$Id$ $CSID$")

#include <ball_context.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_userfields.h>
#include <ball_userfieldvalue.h>
#include <bdlt_datetime.h>

#include <bslmt_lockguard.h>



#include <bsl_ostream.h>

#include <bslstl_stringref.h>

namespace BloombergLP {

namespace ball {
                           // ---------------------
                           // class DefaultObserver
                           // ---------------------

// CREATORS
DefaultObserver::~DefaultObserver()
{
}

// MANIPULATORS
void DefaultObserver::publish(const Record&  record, const Context&)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    const RecordAttributes& fixedFields = record.fixedFields();

    // convert the severity level to ASCII value for publishing
    Severity::Level severityLevel =
                                  (Severity::Level)fixedFields.severity();

    *d_stream << '\n';

    *d_stream << fixedFields.timestamp()               << ' '
              << fixedFields.processID()               << ' '
              << fixedFields.threadID()                << ' '
              << Severity::toAscii(severityLevel) << ' '
              << fixedFields.fileName()                << ' '
              << fixedFields.lineNumber()              << ' '
              << fixedFields.category()                << ' ';

    bslstl::StringRef message = fixedFields.messageRef();
    d_stream->write(message.data(), message.length());

    *d_stream << ' ';

    const ball::UserFields& userFields = record.userFields();
    const int numUserFields = userFields.length();
    for (int i = 0; i < numUserFields; ++i) {
        *d_stream << userFields[i] << ' ';
    }

    *d_stream << '\n' << bsl::flush;
}

}  // close package namespace

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
