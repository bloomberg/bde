// bdlat_fuzzutiloptions.h                                            -*-C++-*-
#ifndef INCLUDED_BDLAT_FUZZUTILOPTIONS_H
#define INCLUDED_BDLAT_FUZZUTILOPTIONS_H

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide options for `bdlat::FuzzUtil`.
//
//@CLASSES:
// bdlat::FuzzUtilOptions: options for `bdlat::FuzzUtil`
//
//@SEE_ALSO: bdlat_fuzzutil
//
//@DESCRIPTION: This component provides a set of customization options for
// `bdlat::FuzzUtil`, e.g. limits on the lengths of generated strings and
// arrays.

#include <bslscm_version.h>

namespace BloombergLP {
namespace bdlat {

                            // =====================
                            // class FuzzUtilOptions
                            // =====================

/// Options for `bdlat::FuzzUtil`.
class FuzzUtilOptions {
    // DATA
    unsigned d_maxArrayLength;
    unsigned d_maxStringLength;

  public:
    // TYPES

    /// Default values
    static const unsigned k_MAX_ARRAY_LENGTH_DEFAULT  = 8;
    static const unsigned k_MAX_STRING_LENGTH_DEFAULT = 32;

    // CREATORS

    /// Set all the attributes to to the corresponding default values.
    FuzzUtilOptions();

    // MANIPULATORS

    /// Set the maximum array length to the specified `value`.  Return a
    /// reference to `*this`.
    FuzzUtilOptions& setMaxArrayLength(unsigned value);

    /// Set the maximum string length to the specified `value`.  Return a
    /// reference to `*this`.
    FuzzUtilOptions& setMaxStringLength(unsigned value);

    // ACCESSORS

    /// Return the maximum array length.
    unsigned maxArrayLength() const;

    /// Return the maximum string length.
    unsigned maxStringLength() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same value,
/// and `false` otherwise.  Two `FuzzUtilOptions` objects have the same value
/// if all of the corresponding values of their `maxArrayLength`, and
/// `maxStringLength` attributes are the same.
bool operator==(const FuzzUtilOptions& lhs, const FuzzUtilOptions& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the same
/// value, and `false` otherwise.  Two `FuzzUtilOptions` objects do not have
/// the same value if any of the corresponding values of their
/// `maxArrayLength`, and `maxStringLength` attributes are not the same.
bool operator!=(const FuzzUtilOptions& lhs, const FuzzUtilOptions& rhs);

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // ---------------------
                            // class FuzzUtilOptions
                            // ---------------------

// MANIPULATORS
inline
FuzzUtilOptions& FuzzUtilOptions::setMaxArrayLength(unsigned value)
{
    d_maxArrayLength = value;
    return *this;
}

inline
FuzzUtilOptions& FuzzUtilOptions::setMaxStringLength(unsigned value)
{
    d_maxStringLength = value;
    return *this;
}

// ACCESSORS
inline
unsigned FuzzUtilOptions::maxArrayLength() const
{
    return d_maxArrayLength;
}

inline
unsigned FuzzUtilOptions::maxStringLength() const
{
    return d_maxStringLength;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlat::operator==(const FuzzUtilOptions& lhs, const FuzzUtilOptions& rhs)
{
    return lhs.maxArrayLength() == rhs.maxArrayLength()
        && lhs.maxStringLength() == rhs.maxStringLength();
}

inline
bool bdlat::operator!=(const FuzzUtilOptions& lhs, const FuzzUtilOptions& rhs)
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
