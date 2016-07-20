// bdlt_iso8601utilconfiguration.h                                    -*-C++-*-
#ifndef INCLUDED_BDLT_ISO8601UTILCONFIGURATION
#define INCLUDED_BDLT_ISO8601UTILCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class to configure ISO 8601 string generation.
//
//@CLASSES:
//  bdlt::Iso8601UtilConfiguration: configuration for ISO 8601 strings
//
//@SEE_ALSO: bdlt_iso8601util
//
//@DESCRIPTION: This component provides an unconstrained (value-semantic)
// attribute class, 'bdlt::Iso8601UtilConfiguration', that may be used to
// configure various aspects of generated ISO 8601 strings.
//
///Attributes
///----------
//..
//             Name             Type   Default
//  -------------------------   ----   -------
//  fractionalSecondPrecision   int     3
//  omitColonInZoneDesignator   bool    false
//  useCommaForDecimalSign      bool    false
//  useZAbbreviationForUtc      bool    false
//..
//: o 'fractionalSecondPrecision': number of digits used to represent
//:   fractional seconds; must be in the range '0 .. 6'.
//:
//: o 'omitColonInZoneDesignator': 'true' if ':' should be omitted from zone
//:   designators.
//:
//: o 'useCommaForDecimalSign': 'true' if ',' should be used as the decimal
//:   sign in fractional seconds; otherwise, '.' should be used.
//:
//: o 'useZAbbreviationForUtc': 'true' if 'Z' should be used for the zone
//:   designator instead of '+00:00' (specific to UTC).
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
///Example 1: Configuring ISO 8601 String Generation
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creation of a 'bdlt::Iso8601UtilConfiguration'
// object that may be used to influence the format of the output produced by a
// hypothetical utility, 'my::Iso8601Util', that generates and parses ISO 8601
// strings for 'bdlt' vocabulary types (see 'bdlt_iso8601util', which provides
// just such functionality).  In particular, suppose that given a sample
// 'bdlt::TimeTz' object:
//..
//  const bdlt::TimeTz timeTz(bdlt::Time(8, 59, 59, 123), 240);
//..
// 'my::Iso8601Util' produces, by default, the following (valid) ISO 8601
// string:
//..
//  08:59:59.123+04:00
//..
// However, we would like to produce the following (also valid) ISO 8601 string
// instead:
//..
//  08:59:59,123+0400
//..
// 'bdlt::Iso8601UtilConfiguration' can be used to obtain the desired result
// assuming that 'my::Iso8601Util' uses 'bdlt::Iso8601UtilConfiguration' to
// affect the format of generated strings in this fashion (e.g., again see
// 'bdlt_iso8601util').
//
// First, we construct a 'bdlt::Iso8601UtilConfiguration' object that has the
// default value:
//..
//  bdlt::Iso8601UtilConfiguration configuration;
//  assert( configuration.fractionalSecondPrecision() == 3);
//  assert(!configuration.omitColonInZoneDesignator());
//  assert(!configuration.useCommaForDecimalSign());
//  assert(!configuration.useZAbbreviationForUtc());
//..
// Then, we modify 'configuration' to indicate that we want to use ',' as the
// decimal sign (in fractional seconds):
//..
//  configuration.setUseCommaForDecimalSign(true);
//  assert( configuration.fractionalSecondPrecision() == 3);
//  assert(!configuration.omitColonInZoneDesignator());
//  assert( configuration.useCommaForDecimalSign());
//  assert(!configuration.useZAbbreviationForUtc());
//..
// Finally, we modify 'configuration' to indicate that we want to omit the ':'
// in zone designators:
//..
//  configuration.setOmitColonInZoneDesignator(true);
//  assert( configuration.fractionalSecondPrecision() == 3);
//  assert( configuration.omitColonInZoneDesignator());
//  assert( configuration.useCommaForDecimalSign());
//  assert(!configuration.useZAbbreviationForUtc());
//..
// Our 'configuration' object can now be supplied to 'my::Iso8601Util' to
// produce the desired result.
//
///Example 2: Setting the Process-Wide Default Configuration
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to establish the process-wide default
// configuration.
//
// First, we retrieve the default configuration in effect at process start-up
// and note that it has the default-constructed value:
//..
//  bdlt::Iso8601UtilConfiguration configuration =
//                      bdlt::Iso8601UtilConfiguration::defaultConfiguration();
//  assert(bdlt::Iso8601UtilConfiguration() == configuration);
//  assert( configuration.fractionalSecondPrecision() == 3);
//  assert(!configuration.omitColonInZoneDesignator());
//  assert(!configuration.useCommaForDecimalSign());
//  assert(!configuration.useZAbbreviationForUtc());
//..
// Next, we modify 'configuration' to indicate that we want to output 'Z' when
// the zone designator is UTC (i.e., instead of '+00:00'):
//..
//  configuration.setUseZAbbreviationForUtc(true);
//  assert( configuration.fractionalSecondPrecision() == 3);
//  assert(!configuration.omitColonInZoneDesignator());
//  assert(!configuration.useCommaForDecimalSign());
//  assert( configuration.useZAbbreviationForUtc());
//..
// Then, we modify 'configuration' to display milliseconds:
//..
//  configuration.setFractionalSecondPrecision(6);
//  assert( configuration.fractionalSecondPrecision() == 6);
//  assert(!configuration.omitColonInZoneDesignator());
//  assert(!configuration.useCommaForDecimalSign());
//  assert( configuration.useZAbbreviationForUtc());
//..
// Now, we set the default configuration to the value of our 'configuration'
// object:
//..
//  bdlt::Iso8601UtilConfiguration::setDefaultConfiguration(configuration);
//..
// Finally, we verify that the default configuration was updated as expected:
//..
//  const bdlt::Iso8601UtilConfiguration newConfiguration =
//                      bdlt::Iso8601UtilConfiguration::defaultConfiguration();
//  assert( newConfiguration.fractionalSecondPrecision() == 6);
//  assert(!newConfiguration.omitColonInZoneDesignator());
//  assert(!newConfiguration.useCommaForDecimalSign());
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

                      // ==============================
                      // class Iso8601UtilConfiguration
                      // ==============================

class Iso8601UtilConfiguration {
    // This unconstrained (value-semantic) attribute class characterizes how to
    // configure certain behavior in 'Iso8601Util' functions.  Currently, only
    // the 'generate' and 'generateRaw' methods of that utility are affected by
    // 'Iso8601UtilConfiguration' settings.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.

  private:
    // PRIVATE TYPES
    enum {
        // This enumeration denotes the distinct bits that define the values of
        // each of the four configuration attributes.

        k_FRACTIONAL_SECOND_PRECISION_MASK  = 0x07,
        k_OMIT_COLON_IN_ZONE_DESIGNATOR_BIT = 0x08,
        k_USE_COMMA_FOR_DECIMAL_SIGN_BIT    = 0x10,
        k_USE_Z_ABBREVIATION_FOR_UTC_BIT    = 0x20
    };

    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Int
                    s_defaultConfiguration;  // process-wide configuration

    // DATA
    int             d_configurationMask;     // bitmask defining configuration

    // FRIENDS
    friend bool operator==(const Iso8601UtilConfiguration&,
                           const Iso8601UtilConfiguration&);
    friend bool operator!=(const Iso8601UtilConfiguration&,
                           const Iso8601UtilConfiguration&);

  private:
    // PRIVATE CREATORS
    explicit Iso8601UtilConfiguration(int configurationMask);
        // Create an 'Iso8601UtilConfiguration' object having the value
        // indicated by the specified 'configurationMask'.  The behavior is
        // undefined unless 'configurationMask' represents a valid
        // 'Iso8601UtilConfiguration' value.

  public:
    // CLASS METHODS
    static Iso8601UtilConfiguration defaultConfiguration();
        // Return the value of the process-wide 'Iso8601UtilConfiguration' that
        // is currently in effect.

    static void setDefaultConfiguration(
                                const Iso8601UtilConfiguration& configuration);
        // Set the value of the process-wide 'Iso8601UtilConfiguration' to the
        // specified 'configuration'.  Note that the expected usage is that the
        // process-wide configuration will be established *once*, early in
        // 'main', and not changed throughout the lifetime of a process.

    // CREATORS
    Iso8601UtilConfiguration();
        // Create an 'Iso8601UtilConfiguration' object having the (default)
        // attribute values:
        //..
        //  fractionalSecondPrecision() == 3
        //  omitColonInZoneDesignator() == false
        //  useCommaForDecimalSign()    == false
        //  useZAbbreviationForUtc()    == false
        //..

    Iso8601UtilConfiguration(const Iso8601UtilConfiguration& original);
        // Create an 'Iso8601UtilConfiguration' object having the value of the
        // specified 'original' configuration.

    ~Iso8601UtilConfiguration();
        // Destroy this object.

    // MANIPULATORS
    Iso8601UtilConfiguration& operator=(const Iso8601UtilConfiguration& rhs);
        // Assign to this object the value of the specified 'rhs'
        // configuration, and return a reference providing modifiable access to
        // this object.

    void setFractionalSecondPrecision(int value);
        // Set the 'fractionalSecondPrecision' attribute of this object to the
        // specified 'value'.  The behavior is undefined unless '0 <= value'
        // and '6 >= value'.

    void setOmitColonInZoneDesignator(bool value);
        // Set the 'omitColonInZoneDesignator' attribute of this object to the
        // specified 'value'.

    void setUseCommaForDecimalSign(bool value);
        // Set the 'useCommaForDecimalSign' attribute of this object to the
        // specified 'value'.

    void setUseZAbbreviationForUtc(bool value);
        // Set the 'useZAbbreviationForUtc' attribute of this object to the
        // specified 'value'.

    // ACCESSORS
    int fractionalSecondPrecision() const;
        // Return the value of the 'fractionalSecondPrecision' attribute of
        // this object.

    bool omitColonInZoneDesignator() const;
        // Return the value of the 'omitColonInZoneDesignator' attribute of
        // this object.

    bool useCommaForDecimalSign() const;
        // Return the value of the 'useCommaForDecimalSign' attribute of this
        // object.

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
bool operator==(const Iso8601UtilConfiguration& lhs,
                const Iso8601UtilConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Iso8601UtilConfiguration' objects
    // have the same value if each of their 'fractionalSecondPrecision',
    // 'omitColonInZoneDesignator', 'useCommaForDecimalSign', and
    // 'useZAbbreviationForUtc' attributes (respectively) have the same value.

bool operator!=(const Iso8601UtilConfiguration& lhs,
                const Iso8601UtilConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Iso8601UtilConfiguration'
    // objects do not have the same value if any of their
    // 'fractionalSecondPrecision', 'omitColonInZoneDesignator',
    // 'useCommaForDecimalSign', or 'useZAbbreviationForUtc' attributes
    // (respectively) do not have the same value.

bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const Iso8601UtilConfiguration& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)', but with the attribute names elided.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                      // ------------------------------
                      // class Iso8601UtilConfiguration
                      // ------------------------------

// PRIVATE CREATORS
inline
Iso8601UtilConfiguration::Iso8601UtilConfiguration(int configurationMask)
: d_configurationMask(configurationMask)
{
    BSLS_ASSERT_SAFE(0 == (configurationMask
                           & ~(k_FRACTIONAL_SECOND_PRECISION_MASK
                             | k_OMIT_COLON_IN_ZONE_DESIGNATOR_BIT
                             | k_USE_COMMA_FOR_DECIMAL_SIGN_BIT
                             | k_USE_Z_ABBREVIATION_FOR_UTC_BIT)));
}

// CLASS METHODS
inline
Iso8601UtilConfiguration Iso8601UtilConfiguration::defaultConfiguration()
{
    return Iso8601UtilConfiguration(
               bsls::AtomicOperations::getIntRelaxed(&s_defaultConfiguration));
}

inline
void Iso8601UtilConfiguration::setDefaultConfiguration(
                                 const Iso8601UtilConfiguration& configuration)
{
    bsls::AtomicOperations::setIntRelaxed(&s_defaultConfiguration,
                                          configuration.d_configurationMask);
}

// CREATORS
inline
Iso8601UtilConfiguration::Iso8601UtilConfiguration()
: d_configurationMask(3)
{
}

inline
Iso8601UtilConfiguration::Iso8601UtilConfiguration(
                                      const Iso8601UtilConfiguration& original)
: d_configurationMask(original.d_configurationMask)
{
}

inline
Iso8601UtilConfiguration::~Iso8601UtilConfiguration()
{
    BSLS_ASSERT_SAFE(0 == (d_configurationMask
                           & ~(k_FRACTIONAL_SECOND_PRECISION_MASK
                             | k_OMIT_COLON_IN_ZONE_DESIGNATOR_BIT
                             | k_USE_COMMA_FOR_DECIMAL_SIGN_BIT
                             | k_USE_Z_ABBREVIATION_FOR_UTC_BIT)));
}

// MANIPULATORS
inline
Iso8601UtilConfiguration& Iso8601UtilConfiguration::operator=(
                                           const Iso8601UtilConfiguration& rhs)
{
    d_configurationMask = rhs.d_configurationMask;

    return *this;
}

// ACCESSORS
inline
int Iso8601UtilConfiguration::fractionalSecondPrecision() const
{
    return d_configurationMask & k_FRACTIONAL_SECOND_PRECISION_MASK;
}

inline
bool Iso8601UtilConfiguration::omitColonInZoneDesignator() const
{
    return d_configurationMask & k_OMIT_COLON_IN_ZONE_DESIGNATOR_BIT;
}

inline
bool Iso8601UtilConfiguration::useCommaForDecimalSign() const
{
    return d_configurationMask & k_USE_COMMA_FOR_DECIMAL_SIGN_BIT;
}

inline
bool Iso8601UtilConfiguration::useZAbbreviationForUtc() const
{
    return d_configurationMask & k_USE_Z_ABBREVIATION_FOR_UTC_BIT;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const Iso8601UtilConfiguration& lhs,
                      const Iso8601UtilConfiguration& rhs)
{
    return lhs.d_configurationMask == rhs.d_configurationMask;
}

inline
bool bdlt::operator!=(const Iso8601UtilConfiguration& lhs,
                      const Iso8601UtilConfiguration& rhs)
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
