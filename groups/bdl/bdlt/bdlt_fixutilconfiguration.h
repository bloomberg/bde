// bdlt_fixutilconfiguration.h                                        -*-C++-*-
#ifndef INCLUDED_BDLT_FIXUTILCONFIGURATION
#define INCLUDED_BDLT_FIXUTILCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class to configure FIX string generation.
//
//@CLASSES:
//  bdlt::FixUtilConfiguration: configuration for FIX strings
//
//@SEE_ALSO: bdlt_fixutil
//
//@DESCRIPTION: This component provides an unconstrained (value-semantic)
// attribute class, 'bdlt::FixUtilConfiguration', that may be used to configure
// various aspects of generated FIX strings.
//
///Attributes
///----------
//..
//             Name             Type   Default
//  -------------------------   ----   -------
//  fractionalSecondPrecision   int     3
//  useZAbbreviationForUtc      bool    false
//..
//: o 'fractionalSecondPrecision': number of digits used to represent
//:   fractional seconds; must be in the range '0 .. 6'.
//:
//: o 'useZAbbreviationForUtc': 'true' if 'Z' should be used for the timezone
//:   offset instead of '+00:00' (specific to UTC).
//
///Default Configuration
///---------------------
// This component also provides a (process-wide) default configuration that may
// be set and retrieved via the 'setDefaultConfiguration' and
// 'defaultConfiguration' class methods, respectively.  See Usage Example 2 for
// further details.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Configuring FIX String Generation
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creation of a 'bdlt::FixUtilConfiguration' object
// that may be used to influence the format of the output produced by a
// hypothetical utility, 'my::FixUtil', that generates and parses FIX strings
// for 'bdlt' vocabulary types (see 'bdlt_fixutil', which provides just such
// functionality).  In particular, suppose that given a sample
// 'bdlt::DatetimeTz' object:
//..
//  const bdlt::DatetimeTz datetimeTz(
//                             bdlt::Datetime(2005, 1, 31, 8, 59, 59, 123), 0);
//..
// 'my::FixUtil' produces, by default, the following string (which is a valid
// FIX string):
//..
//  20050131-08:59:59.123+00:00
//..
// However, we would like to produce the following (also valid FIX) string
// instead:
//..
//  20050131-08:59:59.123000Z
//..
// 'bdlt::FixUtilConfiguration' can be used to obtain the desired result
// assuming that 'my::FixUtil' uses 'bdlt::FixUtilConfiguration' to affect the
// format of generated strings in this fashion (e.g., again see
// 'bdlt_fixutil').
//
// First, we construct a 'bdlt::FixUtilConfiguration' object that has the
// default value:
//..
//  bdlt::FixUtilConfiguration configuration;
//  assert( configuration.fractionalSecondPrecision() == 3);
//  assert(!configuration.useZAbbreviationForUtc());
//..
// Then, we modify 'configuration' to indicate that we want to use 6 digits of
// precision in the fractional seconds:
//..
//  configuration.setFractionalSecondPrecision(6);
//  assert( configuration.fractionalSecondPrecision() == 6);
//  assert(!configuration.useZAbbreviationForUtc());
//..
// Finally, we modify 'configuration' to indicate that we want to use 'Z' as an
// abbreviation for UTC:
//..
//  configuration.setUseZAbbreviationForUtc(true);
//  assert( configuration.fractionalSecondPrecision() == 6);
//  assert( configuration.useZAbbreviationForUtc());
//..
// Our 'configuration' object can now be supplied to 'my::FixUtil' to produce
// the desired result.
//
///Example 2: Setting the Process-Wide Default Configuration
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to establish the process-wide default
// configuration.
//
// First, we retrieve the default configuration in effect at process start-up
// and note that it has the default-constructed value:
//..
//  bdlt::FixUtilConfiguration configuration =
//                          bdlt::FixUtilConfiguration::defaultConfiguration();
//  assert(bdlt::FixUtilConfiguration() == configuration);
//  assert( configuration.fractionalSecondPrecision() == 3);
//  assert(!configuration.useZAbbreviationForUtc());
//..
// Next, we modify 'configuration' to indicate that we want to output 'Z' when
// the timezone offset is UTC (i.e., instead of '+00:00'):
//..
//  configuration.setUseZAbbreviationForUtc(true);
//  assert( configuration.fractionalSecondPrecision() == 3);
//  assert( configuration.useZAbbreviationForUtc());
//..
// Then, we modify 'configuration' to display milliseconds:
//..
//  configuration.setFractionalSecondPrecision(6);
//  assert( configuration.fractionalSecondPrecision() == 6);
//  assert( configuration.useZAbbreviationForUtc());
//..
// Now, we set the default configuration to the value of our 'configuration'
// object:
//..
//  bdlt::FixUtilConfiguration::setDefaultConfiguration(configuration);
//..
// Finally, we verify that the default configuration was updated as expected:
//..
//  const bdlt::FixUtilConfiguration newConfiguration =
//                      bdlt::FixUtilConfiguration::defaultConfiguration();
//  assert( newConfiguration.fractionalSecondPrecision() == 6);
//  assert( newConfiguration.useZAbbreviationForUtc());
//..
// Note that the expected usage is that the process-wide configuration will be
// established *once*, early in 'main', and not changed throughout the lifetime
// of a process.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlt {

                        // ==========================
                        // class FixUtilConfiguration
                        // ==========================

class FixUtilConfiguration {
    // This unconstrained (value-semantic) attribute class characterizes how to
    // configure certain behavior in 'FixUtil' functions.  Currently, only the
    // 'generate' and 'generateRaw' methods of that utility are affected by
    // 'FixUtilConfiguration' settings.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.

  private:
    // PRIVATE TYPES
    enum {
        // This enumeration denotes the distinct bits that define the values of
        // each of the two configuration attributes.

        k_FRACTIONAL_SECOND_PRECISION_MASK = 0x07,
        k_USE_Z_ABBREVIATION_FOR_UTC_BIT   = 0x08
    };

    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Int
                     s_defaultConfiguration;  // process-wide configuration

    // DATA
    int              d_configurationMask;     // bitmask defining configuration

    // FRIENDS
    friend bool operator==(const FixUtilConfiguration&,
                           const FixUtilConfiguration&);
    friend bool operator!=(const FixUtilConfiguration&,
                           const FixUtilConfiguration&);

  private:
    // PRIVATE CREATORS
    explicit FixUtilConfiguration(int configurationMask);
        // Create a 'FixUtilConfiguration' object having the value indicated by
        // the specified 'configurationMask'.  The behavior is undefined unless
        // 'configurationMask' represents a valid 'FixUtilConfiguration' value.

  public:
    // CLASS METHODS
    static FixUtilConfiguration defaultConfiguration();
        // Return the value of the process-wide 'FixUtilConfiguration' that is
        // currently in effect.

    static void setDefaultConfiguration(
                                    const FixUtilConfiguration& configuration);
        // Set the value of the process-wide 'FixUtilConfiguration' to the
        // specified 'configuration'.  Note that the expected usage is that the
        // process-wide configuration will be established *once*, early in
        // 'main', and not changed throughout the lifetime of a process.

    // CREATORS
    FixUtilConfiguration();
        // Create a 'FixUtilConfiguration' object having the (default)
        // attribute values:
        //..
        //  fractionalSecondPrecision() == 3
        //  useZAbbreviationForUtc()    == false
        //..

    FixUtilConfiguration(const FixUtilConfiguration& original);
        // Create a 'FixUtilConfiguration' object having the value of the
        // specified 'original' configuration.

    ~FixUtilConfiguration();
        // Destroy this object.

    // MANIPULATORS
    FixUtilConfiguration& operator=(const FixUtilConfiguration& rhs);
        // Assign to this object the value of the specified 'rhs'
        // configuration, and return a reference providing modifiable access to
        // this object.

    void setFractionalSecondPrecision(int value);
        // Set the 'fractionalSecondPrecision' attribute of this object to the
        // specified 'value'.  The behavior is undefined unless '0 <= value'
        // and '6 >= value'.  Note that the FIX protocol allows for much higher
        // precision.

    void setUseZAbbreviationForUtc(bool value);
        // Set the 'useZAbbreviationForUtc' attribute of this object to the
        // specified 'value'.

    // ACCESSORS
    int fractionalSecondPrecision() const;
        // Return the value of the 'fractionalSecondPrecision' attribute of
        // this object.

    bool useZAbbreviationForUtc() const;
        // Return the value of the 'useZAbbreviationForUtc' attribute of this
        // object.

                                  // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.
};

// FREE OPERATORS
bool operator==(const FixUtilConfiguration& lhs,
                const FixUtilConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'FixUtilConfiguration' objects have
    // the same value if each of their 'fractionalSecondPrecision' and
    // 'useZAbbreviationForUtc' attributes (respectively) have the same value.

bool operator!=(const FixUtilConfiguration& lhs,
                const FixUtilConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'FixUtilConfiguration' objects
    // do not have the same value if any of their 'fractionalSecondPrecision'
    // or 'useZAbbreviationForUtc' attributes (respectively) do not have the
    // same value.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const FixUtilConfiguration& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)', but with the attribute names elided.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // class FixUtilConfiguration
                        // --------------------------

// PRIVATE CREATORS
inline
FixUtilConfiguration::FixUtilConfiguration(int configurationMask)
: d_configurationMask(configurationMask)
{
    BSLS_ASSERT_SAFE(0 == (configurationMask
                           & ~(k_FRACTIONAL_SECOND_PRECISION_MASK
                             | k_USE_Z_ABBREVIATION_FOR_UTC_BIT)));
}

// CLASS METHODS
inline
FixUtilConfiguration FixUtilConfiguration::defaultConfiguration()
{
    return FixUtilConfiguration(
               bsls::AtomicOperations::getIntRelaxed(&s_defaultConfiguration));
}

inline
void FixUtilConfiguration::setDefaultConfiguration(
                                     const FixUtilConfiguration& configuration)
{
    bsls::AtomicOperations::setIntRelaxed(&s_defaultConfiguration,
                                          configuration.d_configurationMask);
}

// CREATORS
inline
FixUtilConfiguration::FixUtilConfiguration()
: d_configurationMask(3)
{
}

inline
FixUtilConfiguration::FixUtilConfiguration(
                                          const FixUtilConfiguration& original)
: d_configurationMask(original.d_configurationMask)
{
}

inline
FixUtilConfiguration::~FixUtilConfiguration()
{
    BSLS_ASSERT_SAFE(0 == (d_configurationMask
                           & ~(k_FRACTIONAL_SECOND_PRECISION_MASK
                             | k_USE_Z_ABBREVIATION_FOR_UTC_BIT)));
}

// MANIPULATORS
inline
FixUtilConfiguration& FixUtilConfiguration::operator=(
                                               const FixUtilConfiguration& rhs)
{
    d_configurationMask = rhs.d_configurationMask;

    return *this;
}

// ACCESSORS
inline
int FixUtilConfiguration::fractionalSecondPrecision() const
{
    return d_configurationMask & k_FRACTIONAL_SECOND_PRECISION_MASK;
}

inline
bool FixUtilConfiguration::useZAbbreviationForUtc() const
{
    return d_configurationMask & k_USE_Z_ABBREVIATION_FOR_UTC_BIT;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const FixUtilConfiguration& lhs,
                      const FixUtilConfiguration& rhs)
{
    return lhs.d_configurationMask == rhs.d_configurationMask;
}

inline
bool bdlt::operator!=(const FixUtilConfiguration& lhs,
                      const FixUtilConfiguration& rhs)
{
    return lhs.d_configurationMask != rhs.d_configurationMask;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
