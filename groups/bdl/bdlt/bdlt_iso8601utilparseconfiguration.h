// bdlt_iso8601utilparseconfiguration.h                               -*-C++-*-
#ifndef INCLUDED_BDLT_ISO8601UTILPARSECONFIGURATION
#define INCLUDED_BDLT_ISO8601UTILPARSECONFIGURATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class to configure ISO 8601 string parsing.
//
//@CLASSES:
//  bdlt::Iso8601UtilParseConfiguration: config for ISO 8601 string parsing
//
//@SEE_ALSO: bdlt_iso8601util, bdlt_iso8601utilconfiguration
//
//@DESCRIPTION: This component provides an unconstrained (value-semantic)
// attribute class, `bdlt::Iso8601UtilParseConfiguration`, that may be used to
// configure various aspects of generated ISO 8601 strings.
//
///Attributes
///----------
// ```
//   Name             Type   Default
// ----------------   ----   -------
// relaxed            bool    false
// basic              bool    false
// ```
// * `relaxed == false` means that date and time fields of `Datetime`s must
//   be separated by `t` or `T`.  `relaxed == true` means that they may be
//   separated by `t`, `T`, or ` `.
// * `basic == false` means that year, month and day fields within `Date`s
//   must be separated by `-` and hour, minute, and second fields within
//   `Time`s must be separated by `:`.  `basic == true` means that the fields
//   within `Date`s and `Time`s must not be separated at all.
//
// Note that in parsing, the `:` between the hour and minute fields of the time
// zone is always optional.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// Our type, `Iso8601UtilParseConfiguration`, has two boolean attributes,
// `basic` and `relaxed`.
// ```
//     typedef bdlt::Iso8601UtilParseConfiguration Config;
// ```
// A default configured object has both attributes being `false`:
// ```
//     Config c;
//     assert(!c.basic());
//     assert(!c.relaxed());
// ```
// The `setBasic` sets the `basic` attribute, leaves the `relaxed` attribute
// alone:
// ```
//     Config c2 = c.setBasic();
//     assert( c.basic());
//     assert(!c.relaxed());
// ```
// `setBasic` and `setRelaxed` take a boolean argument that defaults to `true`:
// ```
//     for (int ii = 0; ii < 16; ++ii) {
//         const bool basic    = ii & 1;
//         const bool relaxed  = ii & 2;
//         const bool basicB   = ii & 4;
//         const bool relaxedB = ii & 8;
//
//         // 'c' can have any valid state at this point.
//
//         const Config& c3 = c.setBasic(basic);
//         assert(&c3 != &c);                // copy, not reference, returned
//         const Config& c4 = c.setRelaxed(relaxed);
//         assert(&c4 != &c);                // copy, not reference, returned
//
//         assert(c.basic()   == basic);
//         assert(c.relaxed() == relaxed);
//
//         c.setRelaxed(relaxedB);
//         c.setBasic(basicB);
//
//         assert(c.relaxed() == relaxedB);
//         assert(c.basic()   == basicB);
//
//         Config d = c;    // copy 'c' to 'd'
//
//         assert(d.relaxed() == relaxedB);
//         assert(d.basic()   == basicB);
//
//         assert(d == c);
//         assert(&d != &c);        // 'd' is a copy, not a reference
//
//         d.setBasic();      // defaults to 'true'
//         d.setRelaxed();    // defaults to 'true'
//
//         assert(d.basic()   == true);
//         assert(d.relaxed() == true);
//     }
// ```

#include <bdlscm_version.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlt {

                      // ====================================
                      // class Iso8601UtilParseConfiguration
                      // ====================================

/// This unconstrained (value-semantic) attribute class characterizes how to
/// configure certain behavior in `Iso8601Util` functions.  See the
/// [](#Attributes) section for information on the class attributes.
class Iso8601UtilParseConfiguration {

  private:
    // DATA
    bool             d_basic;
    bool             d_relaxed;

    // FRIENDS
    friend bool operator==(Iso8601UtilParseConfiguration,
                           Iso8601UtilParseConfiguration);
    friend bool operator!=(Iso8601UtilParseConfiguration,
                           Iso8601UtilParseConfiguration);

  public:
    // CREATORS

    /// Create an `Iso8601UtilParseConfiguration` object having the
    /// (default) attribute values:
    /// ```
    /// relaxed() == false
    /// basic()   == false
    /// ```
    Iso8601UtilParseConfiguration();

    /// Create an `Iso8601UtilParseConfiguration` object having the value of
    /// the specified `original` configuration.
    //! Iso8601UtilParseConfiguration(
    //!                const Iso8601UtilParseConfiguration original) = default;

    /// Destroy this object.
    ~Iso8601UtilParseConfiguration();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs`
    /// configuration, and return a reference providing modifiable access to
    /// this object.
    // Iso8601UtilParseConfiguration& operator=(
    //                     const Iso8601UtilParseConfiguration& rhs) = default;

    /// Set the `basic` field of this object to the specified `value` and
    /// return a copy of this object.
    Iso8601UtilParseConfiguration setBasic(bool value = true);

    /// Set the `relaxed` field of this object to the specified `value` and
    /// return a copy of this object.
    Iso8601UtilParseConfiguration setRelaxed(bool value = true);

    // ACCESSORS

    /// Return `true` if the `basic` field of this object is set and `false`
    /// otherwise.
    bool basic() const;

    /// Return `true` if the `relaxed` field of this object is set and
    /// `false` otherwise.
    bool relaxed() const;

                                  // Aspects

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to `stream`.
    /// Optionally specify an initial indentation `level`, whose absolute
    /// value is incremented recursively for nested objects.  If `level` is
    /// specified, optionally specify `spacesPerLevel`, whose absolute value
    /// indicates the number of spaces per indentation level for this and
    /// all of its nested objects.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative,
    /// format the entire output on one line, suppressing all but the
    /// initial indentation (as governed by `level`).  If `stream` is not
    /// valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `Iso8601UtilParseConfiguration`
/// objects have the same value if each of their `basic` and `relaxed`
/// attributes (respectively) have the same value.
bool operator==(Iso8601UtilParseConfiguration lhs,
                Iso8601UtilParseConfiguration rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `Iso8601UtilParseConfiguration`
/// objects do not have the same value if either of their `basic` or
/// `relaxed` attributes (respectively) do not have the same value.
bool operator!=(Iso8601UtilParseConfiguration lhs,
                Iso8601UtilParseConfiguration rhs);

/// Write the value of the specified `object` to the specified output
/// `stream` in a single-line format, and return a reference to `stream`.
/// If `stream` is not valid on entry, this operation has no effect.  Note
/// that this human-readable format is not fully specified and can change
/// without notice.  Also note that this method has the same behavior as
/// `object.print(stream, 0, -1)`, but with the attribute names elided.
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const Iso8601UtilParseConfiguration& object);

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                      // -----------------------------------
                      // class Iso8601UtilParseConfiguration
                      // -----------------------------------

// CREATORS
inline
Iso8601UtilParseConfiguration::Iso8601UtilParseConfiguration()
: d_basic(false)
, d_relaxed(false)
{
}

inline
Iso8601UtilParseConfiguration::~Iso8601UtilParseConfiguration()
{}

// MANIPULATORS
inline
Iso8601UtilParseConfiguration Iso8601UtilParseConfiguration::setBasic(
                                                                    bool value)
{
    d_basic = value;

    return *this;
}

inline
Iso8601UtilParseConfiguration Iso8601UtilParseConfiguration::setRelaxed(
                                                                    bool value)
{
    d_relaxed = value;

    return *this;
}

// ACCESSORS
inline
bool Iso8601UtilParseConfiguration::basic() const
{
    return d_basic;
}

inline
bool Iso8601UtilParseConfiguration::relaxed() const
{
    return d_relaxed;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(Iso8601UtilParseConfiguration lhs,
                      Iso8601UtilParseConfiguration rhs)
{
    return lhs.d_basic == rhs.d_basic && lhs.d_relaxed == rhs.d_relaxed;
}

inline
bool bdlt::operator!=(Iso8601UtilParseConfiguration lhs,
                      Iso8601UtilParseConfiguration rhs)
{
    return lhs.d_basic != rhs.d_basic || lhs.d_relaxed != rhs.d_relaxed;
}

}  // close enterprise namespace

#endif

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
