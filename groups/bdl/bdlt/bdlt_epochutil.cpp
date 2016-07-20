// bdlt_epochutil.cpp                                                 -*-C++-*-
#include <bdlt_epochutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_epochutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdlt {

                             // ----------------
                             // struct EpochUtil
                             // ----------------

// CLASS DATA


#ifdef BDE_USE_PROLEPTIC_DATES
const EpochUtil::TimeT64 EpochUtil::s_earliestAsTimeT64 = -62135596800LL;
                                                 // January    1, 0001 00:00:00
#else
const EpochUtil::TimeT64 EpochUtil::s_earliestAsTimeT64 = -62135769600LL;
                                                 // January    1, 0001 00:00:00
#endif

const EpochUtil::TimeT64 EpochUtil::s_latestAsTimeT64   = 253402300799LL;
                                                 // December  31, 9999 23:59:59

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
