// balm_integercollector.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_integercollector.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_integercollector_cpp,"$Id$ $CSID$")

#include <bsls_types.h>

#include <bsl_climits.h>

namespace BloombergLP {

                        // ----------------------------
                        // class balm::IntegerCollector
                        // ----------------------------

// PUBLIC CONSTANTS
const int balm::IntegerCollector::k_DEFAULT_MIN = INT_MAX;
const int balm::IntegerCollector::k_DEFAULT_MAX = INT_MIN;

namespace balm {
// MANIPULATORS
void IntegerCollector::loadAndReset(MetricRecord *records)
{
    int                count;
    bsls::Types::Int64 total;
    int                min;
    int                max;
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        count = d_count;
        total = d_total;
        min   = d_min;
        max   = d_max;

        d_count = 0;
        d_total = 0;
        d_min   = k_DEFAULT_MIN;
        d_max   = k_DEFAULT_MAX;
    }
    // Perform the conversion to double values outside of the lock.
    records->metricId() = d_metricId;
    records->count()    = count;
    records->total()    = static_cast<double>(total);
    records->min()      = (k_DEFAULT_MIN == min)
                        ? MetricRecord::k_DEFAULT_MIN
                        : min;
    records->max()      = (k_DEFAULT_MAX == max)
                        ? MetricRecord::k_DEFAULT_MAX
                        : max;
}

// ACCESSORS
void IntegerCollector::load(MetricRecord *record) const
{
    int                count;
    bsls::Types::Int64 total;
    int                min;
    int                max;

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        count = d_count;
        total = d_total;
        min   = d_min;
        max   = d_max;
    }

    // Perform the conversion to double values outside of the lock.
    record->metricId() = d_metricId;
    record->count()    = count;
    record->total()    = static_cast<double>(total);
    record->min()      = (k_DEFAULT_MIN == min)
                       ? MetricRecord::k_DEFAULT_MIN
                       : min;
    record->max()      = (k_DEFAULT_MAX == max)
                       ? MetricRecord::k_DEFAULT_MAX
                       : max;
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
