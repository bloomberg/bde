// bdlt_localtimeoffset.cpp                                           -*-C++-*-
#include <bdlt_localtimeoffset.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_localtimeoffset_cpp,"$Id$ $CSID$")

#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_epochutil.h>

#include <bsls_platform.h>

#include <bsl_c_time.h>

namespace BloombergLP {
namespace bdlt {

                            // ---------------------
                            // class LocalTimeOffset
                            // ---------------------

// DATA
bsls::AtomicOperations::AtomicTypes::Pointer
LocalTimeOffset::s_localTimeOffsetCallback_p =
         { reinterpret_cast<void *>(
                 reinterpret_cast<bsls::Types::IntPtr>(
                     LocalTimeOffset::localTimeOffsetDefault)) };

// CLASS METHODS

                       // ** default callback **

bsls::TimeInterval LocalTimeOffset::
                            localTimeOffsetDefault(const Datetime& utcDatetime)
{
    bsl::time_t currentTime;
    int         status = EpochUtil::convertToTimeT(&currentTime, utcDatetime);
    
    (void)status;
    BSLS_ASSERT(0 == status);

    struct tm localTm;
    struct tm   gmtTm;
#if defined(BSLS_PLATFORM_OS_WINDOWS) || ! defined(BDE_BUILD_TARGET_MT)

    // Note that the Windows implementation of localtime and gmttime, when
    // using multithreaded libraries, are thread-safe.

    localTm = *localtime(&currentTime);
      gmtTm =    *gmtime(&currentTime);
#else
    localtime_r(&currentTime, &localTm);
       gmtime_r(&currentTime,   &gmtTm);
#endif

    Datetime localDatetime;
    Datetime   gmtDatetime;
    DatetimeUtil::convertFromTm(&localDatetime, localTm);
    DatetimeUtil::convertFromTm(  &gmtDatetime,   gmtTm);

    return bsls::TimeInterval((localDatetime - gmtDatetime).totalSeconds(), 0);
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
