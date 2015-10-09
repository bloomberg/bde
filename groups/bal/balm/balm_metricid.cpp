// balm_metricid.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricid.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricid_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

namespace balm {
                               // --------------
                               // class MetricId
                               // --------------

// ACCESSORS
bsl::ostream& MetricId::print(bsl::ostream& stream) const
{
    if (0 == d_description_p) {
        stream << "INVALID_ID";
    }
    else {
        stream << *d_description_p;
    }
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
