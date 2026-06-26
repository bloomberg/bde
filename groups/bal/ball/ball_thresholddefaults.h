// ball_thresholddefaults.h                                           -*-C++-*-
#ifndef INCLUDED_BALL_THRESHOLDDEFAULTS
#define INCLUDED_BALL_THRESHOLDDEFAULTS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide default threshold values.
//
//@CLASSES:
//  ball::ThresholdDefaults: threshold defaults values
//
//@SEE_ALSO: ball_loggermanagerdefaults, ball_categorymanager
//
//@DESCRIPTION: This component provides a namespace for the 4 default threshold
// level constants.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Accessing Default Threshold Values
///- - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to access the default threshold
// values provided by 'ball::ThresholdDefaults'.
//
// Suppose we are implementing a function that initializes threshold levels
// for a logging system.  We can use the constants provided by
// 'ball::ThresholdDefaults' to supply reasonable default values:
// ```
//  void initializeThresholds(int *recordLevel,
//                            int *passLevel,
//                            int *triggerLevel,
//                            int *triggerAllLevel)
//      // Initialize the specified threshold levels to their default values.
//  {
//      *recordLevel     = ball::ThresholdDefaults::k_RECORD_LEVEL;
//      *passLevel       = ball::ThresholdDefaults::k_PASS_LEVEL;
//      *triggerLevel    = ball::ThresholdDefaults::k_TRIGGER_LEVEL;
//      *triggerAllLevel = ball::ThresholdDefaults::k_TRIGGER_ALL_LEVEL;
//  }
// ```
// Then, we can call this function and verify it sets the values correctly:
// ```
//  int recordLevel, passLevel, triggerLevel, triggerAllLevel;
//
//  initializeThresholds(&recordLevel,
//                       &passLevel,
//                       &triggerLevel,
//                       &triggerAllLevel);
// ```
// Finally, the default values provide a reasonable starting configuration
// where 'k_RECORD_LEVEL' (`e_OFF`, 0) disables recording of messages to
// memory, 'k_PASS_LEVEL' (`e_ERROR`, 64) passes only ERROR level messages and
// above, 'k_TRIGGER_LEVEL' (`e_OFF`, 0) disables trigger of publication on
// single message, and 'k_TRIGGER_ALL_LEVEL' (`e_OFF`, 0) disables trigger of
// publication on all messages:
// ```
//  assert(ball::Severity::e_OFF   == recordLevel   );
//  assert(ball::Severity::e_ERROR == passLevel      );
//  assert(ball::Severity::e_OFF   == triggerLevel   );
//  assert(ball::Severity::e_OFF   == triggerAllLevel);
// ```

#include <balscm_version.h>

#include <ball_severity.h>

namespace BloombergLP {
namespace ball {

                          // =======================
                          // class ThresholdDefaults
                          // =======================

/// This struct provides a namespace for default threshold level related
/// constants.
struct ThresholdDefaults {

    // CONSTANTS
    static const int k_RECORD_LEVEL      = Severity::e_OFF;    // disabled
    static const int k_PASS_LEVEL        = Severity::e_ERROR;  // ERROR & above
    static const int k_TRIGGER_LEVEL     = Severity::e_OFF;    // disabled
    static const int k_TRIGGER_ALL_LEVEL = Severity::e_OFF;    // disabled
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
