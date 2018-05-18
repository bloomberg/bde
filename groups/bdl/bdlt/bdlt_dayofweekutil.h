// bdlt_dayofweekutil.h                                               -*-C++-*-
#ifndef INCLUDED_BDLT_DAYOFWEEKUTIL
#define INCLUDED_BDLT_DAYOFWEEKUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on 'bdlt::DayOfWeek::Enum'.
//
//@CLASSES:
//  bdlt::DayOfWeekUtil: functions operating on day-of-week enumeration
//
//@SEE_ALSO: bdlt_dayofweek
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlt::DayOfWeekUtil', that defines functions to determine the day of the
// week shifted from a given date by a certain number of days.
//
///Usage
///------
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we have some event occuring every ten days. Today is the day of the
// performance, Friday, and we want to know when the previous one took place
// and when the next one will be.
// First, we create a 'bdlt::DayOfWeek' variable for the current day:
//..
//  bdlt::DayOfWeek::Enum current = bdlt::DayOfWeek::e_FRI;
//..
// Next, we calculate previous and following event days using
// 'bdlt::DayOfWeekUtil::wrap' function:
//..
//  bdlt::DayOfWeek::Enum previous  = bdlt::DayOfWeekUtil::wrap(current, -10);
//  bdlt::DayOfWeek::Enum following = bdlt::DayOfWeekUtil::wrap(current,  10);
//..
// Finally, we want to verify, that days of week were calculated correctly:
//..
//  assert(bdlt::DayOfWeek::e_TUE == previous );
//  assert(bdlt::DayOfWeek::e_MON == following);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DAYOFWEEK
#include <bdlt_dayofweek.h>
#endif

namespace BloombergLP {
namespace bdlt {

                            // ====================
                            // struct DayOfWeekUtil
                            // ====================

struct DayOfWeekUtil {
    // This 'struct' provides a namespace for common non-primitive procedures
    // that operate on 'bdlt::DayOfWeek::Enum' objects.

    // CLASS METHODS
    static BSLS_CPP11_CONSTEXPR DayOfWeek::Enum wrap(DayOfWeek::Enum dayOfWeek,
                                                     int             numDays);
        // Return the 'DayOfWeek::Enum' value that is the specified (signed)
        // 'numDays' from the specified 'dayOfWeek'.  Note that this method
        // "wraps" 'dayOfWeek' to a later or earlier day of the week according
        // to whether 'numDays' is positive or negative.  For example, the
        // following both hold 'true':
        //..
        //  DayOfWeek::e_TUE == DayOfWeekUtil::wrap(DayOfWeek::e_WED,  6);
        //  DayOfWeek::e_THU == DayOfWeekUtil::wrap(DayOfWeek::e_WED, -6);
        //..
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // --------------------
                            // struct DayOfWeekUtil
                            // --------------------

// CLASS METHODS
inline
BSLS_CPP11_CONSTEXPR DayOfWeek::Enum DayOfWeekUtil::wrap(
                                                     DayOfWeek::Enum dayOfWeek,
                                                     int             numDays)
{
    // Adding an additional 7 days to the initial value to avoid negative
    // result of substraction.

    return ((static_cast<int>(dayOfWeek) + 7 + (numDays % 7)) % 7)
           ? static_cast<DayOfWeek::Enum>(
                       ((static_cast<int>(dayOfWeek) + 7 + (numDays % 7)) % 7))
           : DayOfWeek::e_SAT;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
