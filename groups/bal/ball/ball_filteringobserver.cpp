// ball_filteringobserver.cpp                                         -*-C++-*-
#include <ball_filteringobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_filteringobserver_cpp,"$Id$ $CSID$")

#include <ball_context.h>
#include <ball_patternutil.h>            // for testing only
#include <ball_record.h>
#include <ball_recordattributes.h>       // for testing only
#include <ball_severity.h>               // for testing only
#include <ball_testobserver.h>           // for testing only
#include <ball_userfields.h>             // for testing only

namespace BloombergLP {
namespace ball {

                           // -----------------------
                           // class FilteringObserver
                           // -----------------------

// CREATORS
FilteringObserver::~FilteringObserver()
{
}

// MANIPULATORS
void FilteringObserver::publish(const bsl::shared_ptr<const Record>& record,
                                const Context&                       context)
{
    BSLS_ASSERT(record);

    if (d_recordFilter && d_recordFilter(*record, context)) {
        // If the filter is installed and returns true, pass the log record to
        // the inner observer.
        d_innerObserver->publish(record, context);
    }

    // And drop otherwise.
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
