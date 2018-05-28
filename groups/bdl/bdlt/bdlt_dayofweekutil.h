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
//  bdlt::DayOfWeekUtil: functions operating on the day-of-week enumeration
//
//@SEE_ALSO: bdlt_dayofweek
//
//@DESCRIPTION: This utility component provides a 'struct',
// 'bdlt::DayOfWeekUtil', that serves as a namespace for functions operating on
// the day-of-week enumeration.
//
// This component provides the following (static) methods:
//..
//  'add'                      Determine the day of the week that results from
//                             shifting a given 'bdlt::DayOfWeek::Enum' value
//                             by a given (signed) number of days.
//..
//
///Usage
///------
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we have some event occurring every ten days.  Today is the day of
// the performance, assumed to be a Friday, and we want to know when the
// previous one took place and when the next one will be.
//
// First, we create a 'bdlt::DayOfWeek' variable for the current day:
//..
//  bdlt::DayOfWeek::Enum current = bdlt::DayOfWeek::e_FRI;
//..
// Next, we calculate previous and following event days using the
// 'bdlt::DayOfWeekUtil::add' function:
//..
//  bdlt::DayOfWeek::Enum previous  = bdlt::DayOfWeekUtil::add(current, -10);
//  bdlt::DayOfWeek::Enum following = bdlt::DayOfWeekUtil::add(current,  10);
//..
// Finally, we verify the resultant day-of-week values:
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
    // that operate on 'bdlt::DayOfWeek::Enum' values.

    // CLASS METHODS
    static BSLS_CPP11_CONSTEXPR DayOfWeek::Enum add(DayOfWeek::Enum dayOfWeek,
                                                    int             numDays);
        // Return the 'DayOfWeek::Enum' value that is the specified (signed)
        // 'numDays' from the specified 'dayOfWeek'.  Note that
        // 'DayOfWeek::Enum' is closed under this operation (in the expected
        // manner).  For example, the following hold 'true':
        //..
        //  DayOfWeek::e_MON == DayOfWeekUtil::add(DayOfWeek::e_SUN,  1);
        //  DayOfWeek::e_SAT == DayOfWeekUtil::add(DayOfWeek::e_SUN, -1);
        //  DayOfWeek::e_SAT == DayOfWeekUtil::add(DayOfWeek::e_SUN,  6);
        //  DayOfWeek::e_MON == DayOfWeekUtil::add(DayOfWeek::e_SUN, -6);
        //  DayOfWeek::e_SUN == DayOfWeekUtil::add(DayOfWeek::e_SUN,  7);
        //  DayOfWeek::e_SUN == DayOfWeekUtil::add(DayOfWeek::e_SUN, -7);
        //  DayOfWeek::e_MON == DayOfWeekUtil::add(DayOfWeek::e_SUN,  8);
        //  DayOfWeek::e_SAT == DayOfWeekUtil::add(DayOfWeek::e_SUN, -8);
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
BSLS_CPP11_CONSTEXPR DayOfWeek::Enum DayOfWeekUtil::add(
                                                     DayOfWeek::Enum dayOfWeek,
                                                     int             numDays)
{
    // The '+ 7' prevents a negative result.

    return static_cast<DayOfWeek::Enum>(
            ((static_cast<int>(dayOfWeek) - 1 + numDays % 7) % 7 + 7) % 7 + 1);
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
