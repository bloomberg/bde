// balm_metricdescription.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricdescription.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricdescription_cpp,"$Id$ $CSID$")

#include <balm_category.h>
#include <balm_metricformat.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace balm {
                          // -----------------------
                          // class MetricDescription
                          // -----------------------

// ACCESSORS
bsl::ostream& MetricDescription::print(bsl::ostream& stream) const
{
    stream << d_category_p->name() << "." << d_name_p;
    return stream;
}

bsl::ostream& MetricDescription::printDescription(
                                                    bsl::ostream& stream) const
{
    stream << "[ " << d_category_p->name() << "." << d_name_p << " ";
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    stream << d_preferredPublicationType;
    if (d_format) {
        stream << " format: " << *d_format;
    }

    stream << " user data: (";
    bsl::vector<const void *>::const_iterator it = d_userData.begin();
    for ( ; it != d_userData.end(); ++it) {
        if (it != d_userData.begin()) {
            stream << ' ';
        }
        stream << *it;
    }
    stream << " ) ]";

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
