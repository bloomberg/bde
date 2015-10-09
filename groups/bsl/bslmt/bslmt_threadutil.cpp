// bslmt_threadutil.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_threadutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_threadutil_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>

#include <bsls_platform.h>

#include <bsl_cmath.h>

#include <bsl_c_limits.h>

namespace BloombergLP {

                            // -----------------
                            // struct ThreadUtil
                            // -----------------

// CLASS METHODS
int bslmt::ThreadUtil::convertToSchedulingPriority(
               ThreadAttributes::SchedulingPolicy policy,
               double                             normalizedSchedulingPriority)
{
    BSLS_ASSERT_OPT((int) policy >= ThreadAttributes::e_SCHED_MIN);
    BSLS_ASSERT_OPT((int) policy <= ThreadAttributes::e_SCHED_MAX);

    BSLS_ASSERT_OPT(normalizedSchedulingPriority >= 0.0);
    BSLS_ASSERT_OPT(normalizedSchedulingPriority <= 1.0);

    const int minPri = getMinSchedulingPriority(policy);
    const int maxPri = getMaxSchedulingPriority(policy);

    if (minPri == ThreadAttributes::e_UNSET_PRIORITY ||
        maxPri == ThreadAttributes::e_UNSET_PRIORITY) {
        return ThreadAttributes::e_UNSET_PRIORITY;                    // RETURN
    }

#if !defined(BSLS_PLATFORM_OS_CYGWIN)
    double ret = (maxPri - minPri) * normalizedSchedulingPriority +
                                                                  minPri + 0.5;
#else
    // On Cygwin, a lower numerical value implies a higher thread priority:
    //   minSchedPriority = 15, maxSchedPriority = -14

    double ret = - ((minPri - maxPri) * normalizedSchedulingPriority - minPri)
                                                                         + 0.5;
#endif

    return static_cast<int>(bsl::floor(ret));
}

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
