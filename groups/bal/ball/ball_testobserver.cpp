// ball_testobserver.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_testobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_testobserver_cpp,"$Id$ $CSID$")

#include <ball_transmission.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC DATA MEMBER INITIALIZATION
bsls::AtomicOperations::AtomicTypes::Int ball::TestObserver::s_count = {0};

namespace ball {
// CREATORS
TestObserver::~TestObserver()
{
}

// MANIPULATORS
void TestObserver::publish(const Record&  record,
                           const Context& context)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_record  = record;
    d_context = context;
    ++d_numRecords;

    if (d_verboseFlag) {
        d_stream << "Test Observer ID " << d_count
                 << " publishing record number " << d_numRecords << '\n'
                 << "Context: cause = " << context.transmissionCause() << '\n'
                 << "         count = " << context.recordIndex() + 1
                 << " of an expected "  << context.sequenceLength()
                 << " total records.\n" << bsl::flush;
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
