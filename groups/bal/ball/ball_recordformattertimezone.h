// ball_recordformattertimezone.h                                     -*-C++-*-
#ifndef INCLUDED_BALL_RECORDFORMATTERTIMEZONE
#define INCLUDED_BALL_RECORDFORMATTERTIMEZONE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate a set of timezone defaults for log timestamps.
//
//@CLASSES:
//  ball::RecordFormatterTimezone: namespace for enumerating timezones
//
//@DESCRIPTION: This component provides a namespace,
// `ball::RecordFormatterTimezone`, for the `enum` type
// `ball::RecordFormatterTimezone::Enum`.  `Enum` enumerates a
// list of time stamp timezone values that log record formatters may apply.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Selecting Timestamp Timezone for Logging
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a logging system and need to allow users to
// configure whether timestamps should be rendered in UTC or local time.  We
// can use `ball::RecordFormatterTimezone::Enum` to represent this choice.
//
// First, we define a variable to hold the configured timezone setting,
// defaulting to UTC:
// ```
//  ball::RecordFormatterTimezone::Enum timezone =
//                                        ball::RecordFormatterTimezone::e_UTC;
// ```
// Then, based on user configuration, we might change this to local time:
// ```
//  bool useLocalTime = true;  // from user configuration
//
//  if (useLocalTime) {
//      timezone = ball::RecordFormatterTimezone::e_LOCAL;
//  }
// ```
// Finally, we can use this value to control timestamp formatting:
// ```
//  if (ball::RecordFormatterTimezone::e_LOCAL == timezone) {
//      // Format timestamp in local time
//  }
//  else {
//      // Format timestamp in UTC
//  }
// ```
// This allows timestamps in log records to be rendered in either UTC (useful
// for distributed systems and log aggregation) or local time (useful for
// local debugging and human readability).
// ```
// assert(ball::RecordFormatterTimezone::e_LOCAL == timezone);
// ```

#include <balscm_version.h>

namespace BloombergLP {
namespace ball {

                       // ==============================
                       // struct RecordFormatterTimezone
                       // ==============================

/// This struct provides a namespace for enumerating timezone values for
/// timestamps in record formatters.
struct RecordFormatterTimezone {

    /// Timezone setting for timestamps in log record formatters.
    enum Enum {
        e_UTC,         // Use UTC timezone for timestamp
        e_LOCAL        // Use local timezone for timestamp
    };
};

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BALL_RECORDFORMATTERTIMEZONE

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
