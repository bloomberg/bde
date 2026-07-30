// baljsn_convertfromjsonoptions.h                                    -*-C++-*-
#ifndef INCLUDED_BALJSN_CONVERTFROMJSONOPTIONS
#define INCLUDED_BALJSN_CONVERTFROMJSONOPTIONS

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_convertfromjsonoptions_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide an attribute class for `JsonConverter` "from" options.
//
//@CLASSES:
//  baljsn::ConvertFromJsonOptions: options for `bdljsn::Json` to `bdlat`
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, `baljsn::ConvertFromJsonOptions`, that is
// used to specify options for converting `bdljsn::Json` objects to
// `bdlat`-compliant objects.
//
///Attributes
///----------
// ```
// Name                   Type    Default    Simple Constraints
// ---------------------- ------- ---------- ----------------------
// maxDepth               int     512        >= 0
// skipUnknownElements    bool    true       none
// ```
// * `maxDepth`: maximum allowed depth of the `Json` source object.  If
//    exceeded, the conversion fails.
//
// * `skipUnknownElements`: flag specifying if elements of the `Json` source
//   object that are unknown to the `bdlat` object should be quietly skipped.
//   If `false` any unknown element fails the conversion.
//
// The default values of `maxDepth` and `skipUnknownElements` are the *same* as
// the default values of the identically named `baljsn::DecoderOptions`
// options.
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
// First, we default-construct a `baljsn::ConvertFromJsonOptions` object:
// ```
// baljsn::ConvertFromJsonOptions options;
// assert(512  == options.maxDepth());
// assert(true == options.skipUnknownElements());
// ```
// Next, we populate that object with custom values:
// ```
// options.setMaxDepth(10);
// assert(10 == options.maxDepth());
//
// options.setSkipUnknownElements(false);
// assert(false == options.skipUnknownElements());
// ```

#include <bslh_hash.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace baljsn {

                        // ============================
                        // class ConvertFromJsonOptions
                        // ============================

class ConvertFromJsonOptions {
    // This simply constrained (value-semantic) attribute class specifies
    // options for converting `bdljsn::Json` objects to `bdlat`-compliant
    // objects.  See the [](#Attributes) section for description of the
    // options.

    // CLASS DATA
    static const int DEFAULT_INITIALIZER_MAX_DEPTH;

    // DATA
    int   d_maxDepth;            // maximum recursion depth

    bool  d_skipUnknownElements; // option to skip unknown elements

  public:
    // CREATORS

    /// Create an object of type `ConvertFromJsonOptions` having the default
    /// value.
    ConvertFromJsonOptions();

    /// Create a `ConvertFromJsonOptions` object having the value of the
    /// specified `original` object.
    //! ConvertFromJsonOptions(const ConvertFromJsonOptions& original)
    ///                                                             = default;

    /// Destroy this object.
    //! ~ConvertFromJsonOptions() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs`.
    //! ConvertFromJsonOptions& operator=(const ConvertFromJsonOptions& rhs)
    ///                                                              = default;

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Set the `maxDepth` attribute of this object to the specified `value`.
    /// The behavior is undefined unless `0 <= value`.
    void setMaxDepth(int value);

    /// Set the `skipUnknownElements` attribute of this object to the specified
    /// `value`.
    void setSkipUnknownElements(bool value);

    // ACCESSORS

    /// Format this object to the specified output `stream` at the optionally
    /// specified indentation `level` and return a reference to the modifiable
    /// `stream`.  If `level` is specified, optionally specify
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

    /// Return the value of the `maxDepth` attribute of this object.
    int maxDepth() const;

    /// Return the value of the `skipUnknownElements` attribute of this object.
    bool skipUnknownElements() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects have
    /// the same value, and `false` otherwise.  Two attribute objects have the
    /// same value if each respective attribute has the same value.
    friend bool operator==(const ConvertFromJsonOptions& lhs,
                           const ConvertFromJsonOptions& rhs)
    {
        return lhs.maxDepth()            == rhs.maxDepth()
            && lhs.skipUnknownElements() == rhs.skipUnknownElements();
    }

    /// Return `true` if the specified `lhs` and `rhs` attribute objects do
    /// not have the same value, and `false` otherwise.  Two attribute objects
    /// do not have the same value if any respective attribute do not have the
    /// same value.
    friend bool operator!=(const ConvertFromJsonOptions& lhs,
                           const ConvertFromJsonOptions& rhs)
    {
        return lhs.maxDepth()            != rhs.maxDepth()
            || lhs.skipUnknownElements() != rhs.skipUnknownElements();
    }

    /// Format the specified `rhs` to the specified output `stream` and return
    /// a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&                stream,
                                    const ConvertFromJsonOptions& rhs)
    {
        return rhs.print(stream, 0, -1);
    }

    /// Pass the specified `object` to the specified `hashAlg`.  This function
    /// integrates with the `bslh` modular hashing system and effectively
    /// provides a `bsl::hash` specialization for `ConvertFromJsonOptions`.
    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&             hashAlg,
                           const ConvertFromJsonOptions& object)
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.maxDepth());
        hashAppend(hashAlg, object.skipUnknownElements());
    }
};

}  // close package namespace

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace baljsn {

                        // ----------------------------
                        // class ConvertFromJsonOptions
                        // ----------------------------
// CREATORS

inline
ConvertFromJsonOptions::ConvertFromJsonOptions()
: d_maxDepth(DEFAULT_INITIALIZER_MAX_DEPTH)
, d_skipUnknownElements(true)
{
}

// MANIPULATORS

inline
void ConvertFromJsonOptions::reset()
{
    d_maxDepth            = DEFAULT_INITIALIZER_MAX_DEPTH;
    d_skipUnknownElements = true;
}

inline
void ConvertFromJsonOptions::setMaxDepth(int value)
{
    BSLS_ASSERT(0 <= value);

    d_maxDepth = value;
}

inline
void ConvertFromJsonOptions::setSkipUnknownElements(bool value)
{
    d_skipUnknownElements = value;
}

// ACCESSORS
inline
int ConvertFromJsonOptions::maxDepth() const
{
    return d_maxDepth;
}

inline
bool ConvertFromJsonOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
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
