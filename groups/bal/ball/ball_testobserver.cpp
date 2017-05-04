// ball_testobserver.cpp                                              -*-C++-*-
#include <ball_testobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_testobserver_cpp,"$Id$ $CSID$")

#include <ball_recordattributes.h>     // for testing only
#include <ball_transmission.h>
#include <ball_userfields.h>           // for testing only

#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

                        // ------------------
                        // class TestObserver
                        // ------------------

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Int TestObserver::s_count = {0};

// CREATORS
TestObserver::~TestObserver()
{
}

// MANIPULATORS
void TestObserver::publish(const Record& record, const Context& context)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_record  = record;
    d_context = context;
    ++d_numRecords;

    if (d_verboseFlag) {
        *d_stream_p << "Test Observer ID "   << d_id
                    << " publishing record number "
                                             << d_numRecords
                    << "\nContext: cause = " << context.transmissionCause()
                    << "\n         count = " << context.recordIndex() + 1
                    << " of an expected "    << context.sequenceLength()
                    << " total records.\n"   << bsl::flush;
    }
}

void TestObserver::publish(const bsl::shared_ptr<const Record>& record,
                           const Context&                       context)
{
    BSLS_ASSERT(record);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_record  = *record;
    d_context =  context;
    ++d_numRecords;

    if (d_verboseFlag) {
        *d_stream_p << "Test Observer ID "   << d_id
                    << " publishing record number "
                                             << d_numRecords
                    << "\nContext: cause = " << context.transmissionCause()
                    << "\n         count = " << context.recordIndex() + 1
                    << " of an expected "    << context.sequenceLength()
                    << " total records.\n"   << bsl::flush;
    }
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
