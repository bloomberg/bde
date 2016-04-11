// bdlt_datetimeimputil.cpp                                           -*-C++-*-
#include <bdlt_datetimeimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_datetimeimputil_cpp,"$Id$ $CSID$")

#include <bdlt_timeunitratio.h>

#include <bslmf_assert.h>

namespace BloombergLP {
namespace bdlt {

                          // ----------------------
                          // struct DatetimeImpUtil
                          // ----------------------

// CLASS VALUES
const bsls::Types::Uint64 DatetimeImpUtil::k_0001_01_01_VALUE =
                                          1ULL << 46 | 1ULL << 42 | 1ULL << 37;

const bsls::Types::Uint64 DatetimeImpUtil::k_1970_01_01_VALUE =
                                       1970ULL << 46 | 1ULL << 42 | 1ULL << 37;

const bsls::Types::Uint64 DatetimeImpUtil::k_MAX_VALUE =
                       9999ULL << 46 | 12ULL << 42 | 31ULL << 37 | 24ULL << 32;

BSLMF_ASSERT(  DatetimeImpUtil::k_0001_01_01_VALUE
             < DatetimeImpUtil::k_1970_01_01_VALUE);
BSLMF_ASSERT(  DatetimeImpUtil::k_1970_01_01_VALUE
             < DatetimeImpUtil::k_MAX_VALUE);

// CLASS METHODS
const Datetime *DatetimeImpUtil::epoch_0001_01_01()
{
    return reinterpret_cast<const bdlt::Datetime *>(&k_0001_01_01_VALUE);
}

const Datetime *DatetimeImpUtil::epoch_1970_01_01()
{
    return reinterpret_cast<const bdlt::Datetime *>(&k_1970_01_01_VALUE);
}

const Datetime *DatetimeImpUtil::epoch_max()
{
    return reinterpret_cast<const bdlt::Datetime *>(&k_MAX_VALUE);
}

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
