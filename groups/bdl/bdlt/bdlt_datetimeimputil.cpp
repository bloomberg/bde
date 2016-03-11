// bdlt_datetimeimputil.cpp                                           -*-C++-*-
#include <bdlt_datetimeimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_datetimeimputil_cpp,"$Id$ $CSID$")

#include <bdlt_timeunitratio.h>

namespace BloombergLP {
namespace bdlt {

                           // ----------------------
                           // struct DatetimeImpUtil
                           // ----------------------
// CLASS VALUES

#ifdef BDE_USE_PROLEPTIC_DATES

const bsls::Types::Uint64 DatetimeImpUtil::k_1970_01_01_TOTAL_SECONDS =
                                          719162ULL * TimeUnitRatio::k_S_PER_D;
                    // 719162 is 1970/01/01 - 0001/01/01 in Proleptic Gregorian

const bsls::Types::Uint64 DatetimeImpUtil::k_2470_01_01_TOTAL_SECONDS =
                                          901784ULL * TimeUnitRatio::k_S_PER_D;
                    // 901784 is 2470/01/01 - 0001/01/01 in Proleptic Gregorian

const bsls::Types::Uint64 DatetimeImpUtil::k_0001_01_01_VALUE = 3652058ULL + 1;
                   // 3652058 is 9999/12/31 - 0001/01/01 in Proleptic Gregorian

const bsls::Types::Uint64 DatetimeImpUtil::k_1970_01_01_VALUE =
   719162ULL * TimeUnitRatio::k_US_PER_D + DatetimeImpUtil::k_0001_01_01_VALUE;
                    // 719162 is 1970/01/01 - 0001/01/01 in Proleptic Gregorian

#else

const bsls::Types::Uint64 DatetimeImpUtil::k_1970_01_01_TOTAL_SECONDS =
                                          719164ULL * TimeUnitRatio::k_S_PER_D;
                                  // 719164 is 1970/01/01 - 0001/01/01 in POSIX

const bsls::Types::Uint64 DatetimeImpUtil::k_2470_01_01_TOTAL_SECONDS =
                                          901786ULL * TimeUnitRatio::k_S_PER_D;
                                  // 901786 is 2470/01/01 - 0001/01/01 in POSIX

const bsls::Types::Uint64 DatetimeImpUtil::k_0001_01_01_VALUE = 3652060ULL + 1;
                                 // 3652060 is 9999/12/31 - 0001/01/01 in POSIX

const bsls::Types::Uint64 DatetimeImpUtil::k_1970_01_01_VALUE =
   719164ULL * TimeUnitRatio::k_US_PER_D + DatetimeImpUtil::k_0001_01_01_VALUE;
                                  // 719164 is 1970/01/01 - 0001/01/01 in POSIX

#endif

const bsls::Types::Uint64 DatetimeImpUtil::k_2470_01_01_VALUE =
   182622ULL * TimeUnitRatio::k_NS_PER_D + DatetimeImpUtil::k_1970_01_01_VALUE;
                      // 182622 is 2470/01/01 - 1970/01/01

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
