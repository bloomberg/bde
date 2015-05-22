// bdlt_epochutil.cpp                                                 -*-C++-*-
#include <bdlt_epochutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_epochutil_cpp,"$Id$ $CSID$")

#ifndef BDE_OPENSOURCE_PUBLICATION
#include <bdlt_date.h>
#include <bdlb_bitutil.h>
#include <bsls_log.h>
#endif

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

static const int epochData[2] = { 719163, 0 };
                                 // 719163 is 1970/01/01 in Proleptic Gregorian
#ifndef BDE_OPENSOURCE_PUBLICATION
static const int posixEpochData[2]
                              = { 719165, 0 };
                                 // 719165 is 1970/01/01 in POSIX
#endif

                            // ----------------
                            // struct EpochUtil
                            // ----------------

// CLASS DATA

const bdlt::Datetime *EpochUtil::s_epoch_p =
                           reinterpret_cast<const bdlt::Datetime *>(epochData);
#ifndef BDE_OPENSOURCE_PUBLICATION
const bdlt::Datetime *EpochUtil::s_posixEpoch_p =
                      reinterpret_cast<const bdlt::Datetime *>(posixEpochData);

// In the POSIX calendar, the first day after 1752/09/02 is 1752/09/14.  With
// 639798 for the "magic" serial date value, '>' is the appropriate comparison
// operator to use in the 'logIfProblematicDateValue' function.

const int MAGIC_SERIAL = 639798;  // 1752/09/02 POSIX
                                  // 1752/09/15 proleptic Gregorian

// To limit spewing to 'stderr', log an occurrence of a problematic date value
// only if the associated logging context count is 1, 8, or 256.

const int LOG_THROTTLE_MASK = 1 | 8 | 256;

// PRIVATE CLASS METHODS
void EpochUtil::logIfProblematicDateValue(const char  *fileName,
                                          int          lineNumber,
                                          int          locationId,
                                          const Date&  date)
{
    if (!Date::isLoggingEnabled()
     || (date > *reinterpret_cast<const Date *>(&MAGIC_SERIAL))) {
        return;                                                       // RETURN
    }

    static bsls::AtomicOperations::AtomicTypes::Int counts[32] = { 0 };

    if (locationId < 0 || locationId > 31) {
        return;                                                       // RETURN
    }

    const int tmpCount
             = bsls::AtomicOperations::addIntNvRelaxed(&counts[locationId], 1);

    if ((LOG_THROTTLE_MASK & tmpCount)
     && 1 == bdlb::BitUtil::numBitsSet(
                             static_cast<bdlb::BitUtil::uint32_t>(tmpCount))) {

        bsls::Log::logFormattedMessage(fileName, lineNumber,
                                       "WARNING: bad 'Date' value: "
                                       "%d/%d/%d [%d] "
                                       "(see {TEAM 481627583<GO>})",
                                       date.year(), date.month(), date.day(),
                                       tmpCount);
    }
}

#endif

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
