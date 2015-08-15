// balm_metricrecord.cpp                                              -*-C++-*-
#include <balm_metricrecord.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricrecord_cpp,"$Id$ $CSID$")

#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                          // -----------------------
                          // class balm::MetricRecord
                          // -----------------------

// PUBLIC CONSTANTS
const double balm::MetricRecord::k_DEFAULT_MIN =
                                       bsl::numeric_limits<double>::infinity();
const double balm::MetricRecord::k_DEFAULT_MAX =
                                      -bsl::numeric_limits<double>::infinity();
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
const double balm::MetricRecord::DEFAULT_MIN =
                                       bsl::numeric_limits<double>::infinity();
const double balm::MetricRecord::DEFAULT_MAX =
                                      -bsl::numeric_limits<double>::infinity();
#endif

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
