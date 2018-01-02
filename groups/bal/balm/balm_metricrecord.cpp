// balm_metricrecord.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricrecord.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricrecord_cpp,"$Id$ $CSID$")

#include <bsl_cfloat.h>  // DBL_MAX
#include <bsl_ostream.h>

namespace BloombergLP {

                          // ------------------------
                          // class balm::MetricRecord
                          // ------------------------

// PUBLIC CONSTANTS

// During static initialization, we cannot rely on calling
// 'bsl::numeric_limits<double>::infinity()'.  One common idiom for obtaining a
// compile-time "infinity" value, '1.0/.0', generates an error on MSVC.  So, we
// fall back on 'DBL_MAX * 2'.
//
// Note that no solution to this problem is explicitly allowed by the standard,
// so we are necessarily relying on observing that this strategy happens to
// work on all of our supported platforms.

const double balm::MetricRecord::k_DEFAULT_MIN = DBL_MAX * 2;
const double balm::MetricRecord::k_DEFAULT_MAX = DBL_MAX * -2;


namespace balm {
// ACCESSORS
bsl::ostream& MetricRecord::print(bsl::ostream& stream) const
{
    stream << "[ " << d_metricId << ": " << d_count
           << " " << d_total
           << " " << d_min
           << " " << d_max
           << " ]";
    return stream;
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
