// bbldc_basicactual36525.cpp                                         -*-C++-*-
#include <bbldc_basicactual36525.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_basicactual36525_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>

namespace BloombergLP {
namespace bbldc {

                         // -----------------------
                         // struct BasicActual36525
                         // -----------------------

// CLASS METHODS
double BasicActual36525::yearsDiff(const bdlt::Date& beginDate,
                                   const bdlt::Date& endDate)
{
#if defined(BSLS_PLATFORM_CMP_GNU) && (BSLS_PLATFORM_CMP_VERSION >= 50301)
    // Storing the result value in a 'volatile double' removes extra-precision
    // available in floating-point registers.

    const volatile double rv =
#else
    const double rv =
#endif
                      (endDate - beginDate) / 365.25;

    return rv;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
