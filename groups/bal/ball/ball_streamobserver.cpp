// ball_streamobserver.cpp                                            -*-C++-*-
#include <ball_streamobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_streamobserver_cpp,"$Id$ $CSID$")

#include <ball_context.h>                // for testing only
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_userfields.h>

#include <bdlt_datetime.h>

#include <bslmt_lockguard.h>

#include <bslstl_stringref.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

                           // --------------------
                           // class StreamObserver
                           // --------------------

// CREATORS
StreamObserver::~StreamObserver()
{
}

// MANIPULATORS
void StreamObserver::publish(const bsl::shared_ptr<const Record>& record,
                             const Context&)
{
    BSLS_ASSERT(record);

    const RecordAttributes& fixedFields = record->fixedFields();

    Severity::Level severityLevel = (Severity::Level)fixedFields.severity();

    const int bufferSize = 64;
    char      buffer[bufferSize];
    const int fractionalSecondPrecision = 3;

    const int numBytesWritten = fixedFields.timestamp().printToBuffer(
                                                    buffer,
                                                    bufferSize,
                                                    fractionalSecondPrecision);

    bslstl::StringRef message = fixedFields.messageRef();

    const ball::UserFields& customFields = record->customFields();
    const int               numCustomFields = customFields.length();

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    *d_stream_p << '\n';

    d_stream_p->write(buffer, numBytesWritten);

    *d_stream_p << ' '
                << fixedFields.processID()          << ' '
                << fixedFields.threadID()           << ' '
                << Severity::toAscii(severityLevel) << ' '
                << fixedFields.fileName()           << ' '
                << fixedFields.lineNumber()         << ' '
                << fixedFields.category()           << ' ';

    d_stream_p->write(message.data(), message.length());
    *d_stream_p << ' ';

    for (int i = 0; i < numCustomFields; ++i) {
        *d_stream_p << customFields[i] << ' ';
    }

    *d_stream_p << '\n' << bsl::flush;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
