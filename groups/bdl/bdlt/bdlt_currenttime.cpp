// bdlt_currenttime.cpp                                               -*-C++-*-
#include <bdlt_currenttime.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_currenttime_cpp,"$Id$ $CSID$")

#include <bdlt_datetimeinterval.h>  // for testing only
#include <bdlt_timeunitratio.h>     // for testing only

#include <bsls_systemtime.h>

namespace BloombergLP {
namespace bdlt {

                            // -----------------
                            // class CurrentTime
                            // -----------------

// DATA
bsls::AtomicOperations::AtomicTypes::Pointer
CurrentTime::s_currenttimeCallback_p =
                 { reinterpret_cast<void *>(CurrentTime::currentTimeDefault) };

// CLASS METHODS
DatetimeTz CurrentTime::asDatetimeTz()
{
    Datetime now = utc();
    bsls::TimeInterval offset = LocalTimeOffset::localTimeOffset(now);
    now.addSeconds(offset.totalSeconds());

    BSLS_ASSERT((-1440                 < offset.totalMinutes()) &&
                (offset.totalMinutes() < 1440));
    return DatetimeTz(now, static_cast<int>(offset.totalMinutes()));
}

                       // ** default callbacks **

bsls::TimeInterval CurrentTime::currentTimeDefault()
{
    return bsls::SystemTime::nowRealtimeClock();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
