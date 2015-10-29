// bdlt_epochutil.cpp                                                 -*-C++-*-
#include <bdlt_epochutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_epochutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdlt {

                            // CONSTANTS

// The following 'epochData' object represents the footprint of a
// 'bdlt::Datetime' object as a pair of 32-bit integers, load-time initialized
// to the value representing the epoch time (1970/1/1_00:00:00.000).  The test
// driver must verify this representation.
//
// Note that if a change is made to the underlying data format of a
// 'bdlt::Datetime' object, the 'epochData' object must be changed to match.
//
// This code runs afoul of strict aliasing restrictions, but it is difficult to
// devise a legal formulation to get a load-time-initialized 'Datetime' object
// that adheres to those restrictions without C++11 'constexpr' constructors.
// (In C, a union can be used to type pun, but not in C++.)

#ifdef BDE_USE_PROLEPTIC_DATES
static const int epochData[2] = { 719163, 0 };
                                 // 719163 is 1970/01/01 in Proleptic Gregorian
#else 
static const int epochData[2] = { 719165, 0 };
                                 // 719165 is 1970/01/01 in POSIX
#endif

                            // ----------------
                            // struct EpochUtil
                            // ----------------

// CLASS DATA

const bdlt::Datetime *EpochUtil::s_epoch_p =
                           reinterpret_cast<const bdlt::Datetime *>(epochData);

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
