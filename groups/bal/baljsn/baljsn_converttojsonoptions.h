// baljsn_converttojsonoptions.h                                      -*-C++-*-
#ifndef INCLUDED_BALJSN_CONVERTTOJSONOPTIONS
#define INCLUDED_BALJSN_CONVERTTOJSONOPTIONS

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_converttojsonoptions_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide options for conversion of `bdlat` to `Json` objects.
//
//@CLASSES:
//  baljsn::ConvertToJsonOptions: options for `bdlat` to `bdljsn::Json`
//
//@DESCRIPTION: This component provides a single, (value-semantic) attribute
// class, `baljsn::ConvertToJsonOptions`, that is used to specify options for
// converting `bdlat`-compliant objects to `bdljsn::Json` objects.
//
///Attributes
///----------
// ```
// Name                   Type    Default    Simple Constraints
// ---------------------- ------- ---------- ----------------------
// convertEmptyArrays     bool    true       none
// convertNullElements    bool    true       none
// ```
// * `convertEmptyArrays`: whether empty arrays should be converted to
//   `bdljsn::JsonArray()` objects.  Note that empty arrays that occur as
//   selections of choices are always converted regardless of this option
//   setting.
//
// * `convertNullElements`: whether null elements in arrays should be
//   converted as `bdljsn::JsonNull()` objects.
//
// The default values of these options are the *opposite* of the default
// values of the similarly named `baljsn::EncoderOptions` options.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Populating an Options Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for JSON conversion of `bdlat` objects.  This example shows how to create
// and populate an options object.
//
// First, we default-construct a `baljsn::ConvertToJsonOptions` object:
// ```
// baljsn::ConvertToJsonOptions options;
// assert(true == options.convertEmptyArrays());
// assert(true == options.convertNullElements());
// ```
// Next, we populate that object with custom values:
// ```
// options.setConvertEmptyArrays(false);
// assert(false == options.convertEmptyArrays());
//
// options.setConvertNullElements(false);
// assert(false == options.convertNullElements());
// ```

#include <bslh_hash.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace baljsn {

                        // ==========================
                        // class ConvertToJsonOptions
                        // ==========================

class ConvertToJsonOptions {
    // This (value-semantic) attribute class specifies options for converting
    // `bdlat`-compliant objects to `bdljsn::Json` objects.  See the
    // [](#Attributes) section for description of the options.

    // DATA
    bool  d_convertEmptyArrays;   // option specifying if empty arrays should
                                  // be converted. Empty arrays occurring as
                                  // selections of choices are always
                                  // converted.

    bool  d_convertNullElements;  // option specifying if null elements should
                                  // be converted

  public:
    // CREATORS

    /// Create a `ConvertToJsonOptions` object having the default value.
    ConvertToJsonOptions();

    /// Create a `ConvertToJsonOptions` object having the value of the
    /// specified `original` object.
    //! ConvertToJsonOptions(const ConvertToJsonOptions& original)
    //                                                               = default;

    /// Destroy this object.
    //! ~ConvertToJsonOptions() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs`.
    //! ConvertToJsonOptions& operator=(const ConvertToJsonOptions& rhs)
    //                                                             = default;

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Set the `convertEmptyArrays` attribute of this object to the specified
    /// `value`.
    void setConvertEmptyArrays(bool value);

    /// Set the `convertNullElements` attribute of this object to the specified
    /// `value`.
    void setConvertNullElements(bool value);

    // ACCESSORS

    /// Format this object to the specified output `stream` at the optionally
    /// specified indentation `level` and return a reference to the
    /// modifiable `stream`.  If `level` is specified, optionally specify
    /// `spacesPerLevel`, the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of `level * spacesPerLevel`.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// `stream` is initially invalid, this operation has no effect.  Note
    /// that a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Return the value of the `convertEmptyArrays` attribute of this object.
    bool convertEmptyArrays() const;

    /// Return the value of the `convertNullElements` attribute of this object.
    bool convertNullElements() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects have
    /// the same value, and `false` otherwise.  Two attribute objects have the
    /// same value if each respective attribute has the same value.
    friend bool operator==(const ConvertToJsonOptions& lhs,
                           const ConvertToJsonOptions& rhs)
    {
        return lhs.convertEmptyArrays()  == rhs.convertEmptyArrays()
            && lhs.convertNullElements() == rhs.convertNullElements();
    }

    /// Return `true` if the specified `lhs` and `rhs` attribute objects do not
    /// have the same value, and `false` otherwise.  Two attribute objects do
    /// not have the same value if any respective attribute do not have the
    /// same value.
    friend bool operator!=(const ConvertToJsonOptions& lhs,
                           const ConvertToJsonOptions& rhs)
    {
        return lhs.convertEmptyArrays()  != rhs.convertEmptyArrays()
            || lhs.convertNullElements() != rhs.convertNullElements();
    }

    /// Format the specified `rhs` to the specified output `stream` and return
    /// a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&                stream,
                                    const ConvertToJsonOptions& rhs)
    {
        return rhs.print(stream, 0, -1);
    }

    /// Pass the specified `object` to the specified `hashAlg`.  This function
    /// integrates with the `bslh` modular hashing system and effectively
    /// provides a `bsl::hash` specialization for `ConvertToJsonOptions`.
    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&           hashAlg,
                           const ConvertToJsonOptions& object)
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.convertEmptyArrays());
        hashAppend(hashAlg, object.convertNullElements());
    }
};

}  // close package namespace

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace baljsn {

                        // --------------------------
                        // class ConvertToJsonOptions
                        // --------------------------

// CREATORS

inline
ConvertToJsonOptions::ConvertToJsonOptions()
: d_convertEmptyArrays(true)
, d_convertNullElements(true)
{
}

// MANIPULATORS

inline
void ConvertToJsonOptions::reset()
{
    d_convertEmptyArrays  = true;
    d_convertNullElements = true;
}

inline
void ConvertToJsonOptions::setConvertEmptyArrays(bool value)
{
    d_convertEmptyArrays = value;
}

inline
void ConvertToJsonOptions::setConvertNullElements(bool value)
{
    d_convertNullElements = value;
}

// ACCESSORS

inline
bool ConvertToJsonOptions::convertEmptyArrays() const
{
    return d_convertEmptyArrays;
}

inline
bool ConvertToJsonOptions::convertNullElements() const
{
    return d_convertNullElements;
}

}  // close package namespace
}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
