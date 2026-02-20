// ball_recordformatteroptions.h                                      -*-C++-*-
#ifndef INCLUDED_BALL_RECORDFORMATTEROPTIONS
#define INCLUDED_BALL_RECORDFORMATTEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provides log record formatter option values.
//
//@CLASSES:
//  ball::RecordFormatterOptions: record formatter option values
//
//@SEE_ALSO: ball_recordformatterregistryutil
//
//@DESCRIPTION: This component provides a value-semantic attribute class,
// `RecordFormatterOptions`, that represents the options that are in addition
// to the format string for configuring a log record formatter "plugin".
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Configuring Formatter Options
///- - - - - - - - - - - - - - - - - - - - -
// Suppose we are configuring a log record formatter and want to specify
// options for how timestamps should be rendered.  We can use
// `ball::RecordFormatterOptions` to specify these configuration values.
//
// First, we create a `RecordFormatterOptions` object with default settings,
// which uses UTC timezone:
// ```
//  ball::RecordFormatterOptions options;
//  assert(ball::RecordFormatterTimezone::e_UTC == options.timezoneDefault());
// ```
// Then, if we want to configure the formatter to use local time instead, we
// can either create the options object with the desired timezone:
// ```
//  ball::RecordFormatterOptions localOptions(
//                                     ball::RecordFormatterTimezone::e_LOCAL);
//  assert(ball::RecordFormatterTimezone::e_LOCAL ==
//                                             localOptions.timezoneDefault());
// ```
// Or we can modify an existing options object:
// ```
//  options.setTimezoneDefault(ball::RecordFormatterTimezone::e_LOCAL);
//  assert(
//        ball::RecordFormatterTimezone::e_LOCAL == options.timezoneDefault());
// ```
// Finally, we can pass these options to a formatter configuration function
// (such as `ball::RecordJsonFormatter::loadJsonSchemeFormatter`) to configure
// how timestamps are rendered in the log output.

#include <balscm_version.h>

#include <ball_recordformattertimezone.h>

namespace BloombergLP {

namespace ball {

                        // ============================
                        // class RecordFormatterOptions
                        // ============================

/// This class provides a value-semantic type for representing options used to
/// configure log record formatters.  These options are supplemental to the
/// format specification string and affect how certain fields in log records
/// are rendered.  Currently, the only supported option is `timezoneDefault`,
/// which controls whether timestamps are displayed in UTC or local time.
class RecordFormatterOptions {
  private:
    // DATA
    RecordFormatterTimezone::Enum d_timezoneDefault;

  public:
    // CREATORS

    /// Create a default `RecordFormatterOptions` object with `e_UTC` as
    /// its `timezoneDefault` setting.
    RecordFormatterOptions();

    /// Create a `RecordFormatterOptions` object with the specified
    /// `timezoneDefault` setting.
    explicit RecordFormatterOptions(
                                RecordFormatterTimezone::Enum timezoneDefault);

    // MANIPULATORS

    /// Set the timezone default value attribute of this object to the
    /// specified `timezoneDefault`.
    void setTimezoneDefault(RecordFormatterTimezone::Enum timezoneDefault);

    // ACCESSORS

    /// Return a non-modifiable reference to the timezone default attribute of
    /// this object.
    RecordFormatterTimezone::Enum timezoneDefault() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `RecordFormatterOptions` objects have
/// the same value if their `timezoneDefault` attributes have the same value.
bool operator==(const RecordFormatterOptions& lhs,
                const RecordFormatterOptions& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `RecordFormatterOptions` objects
/// do not have the same value if their `timezoneDefault` attributes do not
/// have the same value.
bool operator!=(const RecordFormatterOptions& lhs,
                const RecordFormatterOptions& rhs);

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class RecordFormatterOptions
                        // ----------------------------

// CREATORS
inline
RecordFormatterOptions::RecordFormatterOptions()
: d_timezoneDefault(RecordFormatterTimezone::e_UTC)
{
}

inline
RecordFormatterOptions::RecordFormatterOptions(
                                 RecordFormatterTimezone::Enum timezoneDefault)
: d_timezoneDefault(timezoneDefault)
{
}

inline
void RecordFormatterOptions::setTimezoneDefault(
                                 RecordFormatterTimezone::Enum timezoneDefault)
{
    d_timezoneDefault = timezoneDefault;
}

// ACCESSORS
inline
RecordFormatterTimezone::Enum
RecordFormatterOptions::timezoneDefault() const
{
    return d_timezoneDefault;
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const ball::RecordFormatterOptions& lhs,
                      const ball::RecordFormatterOptions& rhs)
{
    return lhs.timezoneDefault() == rhs.timezoneDefault();
}

inline
bool ball::operator!=(const ball::RecordFormatterOptions& lhs,
                      const ball::RecordFormatterOptions& rhs)
{
    return !(lhs == rhs);
}

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
