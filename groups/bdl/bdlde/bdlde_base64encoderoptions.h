// bdlde_base64encoderoptions.h                                       -*-C++-*-
#ifndef INCLUDED_BDLDE_BASE64ENCODEROPTIONS
#define INCLUDED_BDLDE_BASE64ENCODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_base64encoderoptions_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes for encoder options.
//
//@CLASSES:
//  bdlde::Base64EncoderOptions: options for encoder
//
//@SEE_ALSO: bdlde_base64encorderoptions, bdlde_base64decorderoptions,
//           bdlde_base64encoder,         bdlde_base64decoder
//           bdlde_base64alphabet
//
//@DESCRIPTION: This component provides a value-semantic attribute class for
// specifying options for 'bdlde::Basee64Encoder'.
//
// This 'class' has only a default constructor, no value constructors.  We make
// up for this by having all the settors return a reference to the object, so a
// temporary can be default constructed and the settors called directly on that
// temporary and the result either bound to a reference or passed directly to
// the 'Base64Encoder' being constructed.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1:
/// - - - - -
// The following snippets of code provide a simple illustration of
// 'bdlde::Base64EncoderOptions' usage.
//
// This 'class' does not have a value constructor, only a default constructor.
// The settors all return a reference to the object, so in order
// to create an object with attributes other than the default ones, call
// the default constructor and the settors in a single statement, and bind the
// result to a reference (or pass the result directly to the 'Base64Encoder'
// being constructed).
//
// First, we declare some typedefs for brevity:
//..
//  typedef bdlde::Base64Alphabet       Alphabet;
//  typedef bdlde::Base64EncoderOptions EncoderOptions;
//..
// Then, we default construct and object, immediately call the desired settors
// on the temporary, and bind the result to a reference:
//..
//  EncoderOptions& options = EncoderOptions().setAlphabet(Alphabet::e_URL).
//                                                          setIsPadded(false);
//..
// Next, we observe that the properies are as expected:
//..
//  assert(options.maxLineLength() == 76);
//  assert(options.alphabet()      == Alphabet::e_URL);
//  assert(options.isPadded()      == false);
//..
// Now, we stream the object:
//..
//  options.print(cout);
//..
// Finally, we observe the output:
//..
//  [
//      maxLineLength = 76
//      alphabet = URL
//      isPadded = false
//  ]
//..

#include <bdlde_base64alphabet.h>

#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

                          // ==========================
                          // class Base64EncoderOptions
                          // ==========================

class Base64EncoderOptions {
    // DATA
    int                  d_maxLineLength;
    Base64Alphabet::Enum d_alphabet;
    bool                 d_isPadded;

  public:
    // PUBLIC TYPES
    enum { k_DEFAULT_MAX_LINE_LENGTH = 76 };

    // CREATORS
    Base64EncoderOptions();
        // Create an object of type 'EncoderOptions' having the default value.
        // The value of 'maxLineLength' will be 76, the value of 'alphabet'
        // will be 'Base64Alphabet::e_BASIC', and the value of 'isPadded' will
        // be 'true'.

    // Base64EncoderOptions(const Base64EncoderOptions&) = default;

    // ~Base64EncoderOptions() = default;

    // MANIPULATORS
    // Base64EncoderOptions& operator=(const Base64EncoderOptions&) = default;

    Base64EncoderOptions& setAlphabet(Base64Alphabet::Enum value);
        // Set the 'alphabet' attribute to the specified 'value'.  The behavior
        // is undefined unless 'value' is either 'e_BASIC' or 'e_UTL'.

    Base64EncoderOptions& setIsPadded(bool value);
        // Set the 'isPadded' attribute to the specified 'value'.

    Base64EncoderOptions& setMaxLineLength(int value);
        // Set the 'maxLineLength' attribute to the specified 'value'.  The
        // behavior is undefined unless '0 <= value'.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    Base64Alphabet::Enum alphabet() const;
        // Return the value of the 'alphabet' attribute.

    bool isPadded() const;
        // Return the value of the 'isPadded' attribute.

    int maxLineLength() const;
        // Return the value of the 'maxLineLength' attribute.
};

// FREE OPERATORS
bool operator==(const Base64EncoderOptions& lhs,
                const Base64EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

bool operator!=(const Base64EncoderOptions& lhs,
                const Base64EncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const Base64EncoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace
}  // close enterprise namespace

// TRAITS
namespace bsl {

template <>
struct is_trivially_copyable<BloombergLP::bdlde::Base64EncoderOptions> :
                                                             bsl::true_type {};


}  // close namespace bsl

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bdlde {

                        // --------------------------
                        // class Base64EncoderOptions
                        // --------------------------

// CREATORS
inline
Base64EncoderOptions::Base64EncoderOptions()
: d_maxLineLength(k_DEFAULT_MAX_LINE_LENGTH)
, d_alphabet(Base64Alphabet::e_BASIC)
, d_isPadded(true)
{}

// MANIPULATORS
inline
Base64EncoderOptions& Base64EncoderOptions::setAlphabet(
                                                    Base64Alphabet::Enum value)
{
    BSLS_ASSERT(Base64Alphabet::e_BASIC == value ||
                                               Base64Alphabet::e_URL == value);

    d_alphabet = value;

    return *this;
}

inline
Base64EncoderOptions& Base64EncoderOptions::setIsPadded(bool value)
{
    d_isPadded = value;

    return *this;
}

inline
Base64EncoderOptions& Base64EncoderOptions::setMaxLineLength(int value)
{
    BSLS_ASSERT(0 <= value);

    d_maxLineLength = value;

    return *this;
}

// ACCESSORS
inline
Base64Alphabet::Enum Base64EncoderOptions::alphabet() const
{
    return d_alphabet;
}

inline
bool Base64EncoderOptions::isPadded() const
{
    return d_isPadded;
}

inline
int Base64EncoderOptions::maxLineLength() const
{
    return d_maxLineLength;
}

}  // close package namespace

}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
