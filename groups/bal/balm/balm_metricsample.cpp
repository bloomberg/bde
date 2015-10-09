// balm_metricsample.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricsample.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricsample_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace balm {
                          // -----------------------
                          // class MetricSampleGroup
                          // -----------------------

// ACCESSORS
bsl::ostream&
MetricSampleGroup::print(bsl::ostream& stream,
                         int           level,
                         int           spacesPerLevel) const
{
    const char *NL = (spacesPerLevel > 0) ? "\n" : " ";

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_elapsedTime << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "[" << NL;

    const_iterator it = begin();
    for (; it != end(); ++it) {
        bdlb::Print::indent(stream, level + 2, spacesPerLevel);
        stream << *it << NL;
    }

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << NL;
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << NL;
    return stream;
}

                             // ------------------
                             // class MetricSample
                             // ------------------

// CREATORS
MetricSample::MetricSample(const MetricSample&  original,
                           bslma::Allocator    *basicAllocator)
: d_timeStamp(original.d_timeStamp)
, d_records(original.d_records, basicAllocator)
, d_numRecords(original.d_numRecords)
{
}

// MANIPULATORS
MetricSample& MetricSample::operator=(const MetricSample& rhs)
{
    if (this != &rhs) {
        d_records     = rhs.d_records;
        d_timeStamp   = rhs.d_timeStamp;
        d_numRecords  = rhs.d_numRecords;
    }
    return *this;
}

// ACCESSORS
bsl::ostream& MetricSample::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    const char *NL = (spacesPerLevel > 0) ? "\n" : " ";

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_timeStamp << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "[" << NL;

    const_iterator it = begin();
    for (; it != end(); ++it) {
        it->print(stream, level + 2, spacesPerLevel);
    }

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << NL;
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << NL;
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
