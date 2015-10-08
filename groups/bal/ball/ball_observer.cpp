// ball_observer.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_observer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_observer_cpp,"$Id$ $CSID$")

#include <ball_recordattributes.h>              // for testing only
#include <ball_record.h>                        // for testing only
#include <ball_context.h>                       // for testing only
#include <ball_transmission.h>                  // for testing only
#include <ball_userfields.h>                    // for testing only

#include <bslmf_assert.h>
#include <bsls_assert.h>

namespace BloombergLP {

namespace ball {
                           // --------------
                           // class Observer
                           // --------------

// CREATORS
Observer::~Observer()
{
    BSLMF_ASSERT(sizeof(Observer) >= sizeof(int));

    // TBD: Remove this test once the observer changes in BDE 2.12 have
    // stabilized.

    *((unsigned int*)this) = 0xdeadbeef;
}

// MANIPULATORS
void Observer::publish(const Record& , const Context& )
{
    BSLS_ASSERT_OPT(false);  // Should not be called
}

void Observer::publish(const bsl::shared_ptr<const Record>& record,
                       const Context&                       context)
{
    publish(*record, context);
}

void Observer::releaseRecords()
{
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
