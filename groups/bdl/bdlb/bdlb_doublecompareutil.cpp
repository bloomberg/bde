// bdlb_doublecompareutil.cpp                                         -*-C++-*-
#include <bdlb_doublecompareutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_doublecompare_cpp,"$Id$ $CSID$")

#include <bdlb_float.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlb {

const double DoubleCompareUtil::k_DEFAULT_RELATIVE_TOLERANCE = 1e-12;
const double DoubleCompareUtil::k_DEFAULT_ABSOLUTE_TOLERANCE = 1e-24;

/// Return the absolute value of the specified `input`.
static inline
double fabsval(double input)
{
    return input >= 0.0 ? input : -input;
}

                     // ------------------------------
                     // struct bldb::DoubleCompareUtil
                     // ------------------------------

// CLASS METHODS
DoubleCompareUtil::CompareResult
DoubleCompareUtil::fuzzyCompare(double a,
                                double b,
                                double relTol,
                                double absTol)
{
    BSLS_ASSERT(relTol == relTol);
    BSLS_ASSERT(bdlb::Float::isFinite(relTol));
    BSLS_ASSERT(bdlb::Float::signBit(relTol) == false);

    BSLS_ASSERT(absTol == absTol);
    BSLS_ASSERT(bdlb::Float::isFinite(absTol));
    BSLS_ASSERT(bdlb::Float::signBit(absTol) == false);

    // First check for the *very* special case of NaNs.
    if (a != a || b != b) {
        return e_NON_COMPARABLE;                                      // RETURN
    }

    // Next check for the *very* special case of true equality.
    if (a == b) {
        return e_EQUAL;                                               // RETURN
    }

    // Next check for special case where the relative difference is "infinite".
    if (a == -b) {
        if (fabsval(a - b) <= absTol) {
            return e_EQUAL;                                           // RETURN
        }
        else if (a < b) {
            return e_LESS_THAN;                                       // RETURN
        }
        else {
            return e_GREATER_THAN;                                    // RETURN
        }
    }

    // Otherwise process the normal case involving both absolute and relative
    // tolerances.

    const double difference = fabsval(a - b);
    const double average    = fabsval((a + b) / 2.0);

    if (difference <= absTol || difference / average <= relTol) {
        return e_EQUAL;                                               // RETURN
    }
    else if (a < b) {
        return e_LESS_THAN;                                           // RETURN
    }
    else {
        return e_GREATER_THAN;                                        // RETURN
    }
}

}  // close package namespace
}  // close corporate namespace

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
