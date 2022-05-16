// bdlde_base64decoderoptions.h                                       -*-C++-*-
#ifndef INCLUDED_BDLDE_BASE64DECODEROPTIONS
#define INCLUDED_BDLDE_BASE64DECODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_base64decoderoptions_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes for decoder options.
//
//@CLASSES:
//  bdlde::Base64DecoderOptions: options for decoder
//
//@SEE_ALSO: bdlde_base64encorderoptions, bdlde_base64decorderoptions,
//           bdlde_base64encoder,         bdlde_base64decoder
//           bdlde_base64alphabet
//
//@DESCRIPTION: This component provides a value-semantic attribute class for
// specifying options for 'bdlde::Basee64Decoder'.
//
// This 'class' has a value constructor, which, if called with no arguments
// specified, configures the created object with the configuration that most
// clients will want.  There is another frequently-desired configuration for
// translating URL's, and that configuration can be most easily obtained by
// calling the 'standardURL' class method, which returns an
// appropriately-configured options object.
//
// Other configurations may be obtained by specifying arguments to the value
// constructor, or by calling the settors after the object is created.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1:
/// - - - - -
// Suppose we want a 'Base64DecoderOptions' object configured for MIME
// encoding, meaning 'maxLineLength == 76', 'alphabet == e_BASIC', and
// 'isPadded == true'.
//
// First, it turns out that those are the default values of the attributes, so
// all we have to do is default construct an object, and we're done.
//..
//  const bdlde::Base64DecoderOptions& mimeOptions =
//                                         bdlde::Base64DecoderOptions::mime();
//..
// Then, we check the attributes:
//..
//  assert(mimeOptions.maxLineLength() == 76);
//  assert(mimeOptions.alphabet()      == bdlde::Base64Alphabet::e_BASIC);
//  assert(mimeOptions.isPadded()      == true);
//..
// Now, we stream the object:
//..
//  mimeOptions.print(cout);
//..
// Finally, we observe the output:
//..
//  [
//      maxLineLength = 76
//      alphabet = BASIC
//      isPadded = true
//  ]
//..
//
///Example 2:
/// - - - - -
// Suppose we want a 'Base64DecoderOptions' object configured for translating
// URL's.  That would mean a 'maxLineLength == 0', 'alphabet == e_URL', and
// 'isPadded == true'.
//
// First, the class method 'urlSafe' returns an object configured exactly that
// way, so we simply call it:
//..
//  const bdlde::Base64DecoderOptions& urlOptions =
//                                      bdlde::Base64DecoderOptions::urlSafe();
//..
// Then, we check the attributes:
//..
//  assert(urlOptions.maxLineLength() == 0);
//  assert(urlOptions.alphabet()      == bdlde::Base64Alphabet::e_URL);
//  assert(urlOptions.isPadded()      == true);
//..
// Now, we stream the object:
//..
//  urlOptions.print(cout);
//..
// Finally, we observe the output:
//..
//  [
//      maxLineLength = 0
//      alphabet = URL
//      isPadded = false
//  ]
//..
//
///Example 3:
/// - - - - -
// Suppose we want an options object configured for standard Base64:
//
// First, we can simply call the 'standard' class method:
//..
//  const bdlde::Base64DecoderOptions& standardOptions =
//                                     bdlde::Base64DecoderOptions::standard();
//..
// Then, we check the attributes:
//..
//  assert(standardOptions.maxLineLength() == 0);
//  assert(standardOptions.alphabet()      == bdlde::Base64Alphabet::e_BASIC);
//  assert(standardOptions.isPadded()      == true);
//..
// Now, we stream the object:
//..
//  standardOptions.print(cout);
//..
// Finally, we observe the output:
//..
//  [
//      maxLineLength = 0
//      alphabet = BASIC
//      isPadded = true
//  ]
//..
//
///Example 4:
/// - - - - -
// Suppose we want a really strangely configured options object with
// 'maxLineLength == 200', 'alphabet == e_URL', and padding.
//
// First, we can simply call the 'custom' class method.  The 'padded' argument
// is the last argument, and it defaults to 'true', so we don't have to pass
// that.
//..
//  const bdlde::Base64DecoderOptions& customOptions =
//      bdlde::Base64DecoderOptions::custom(200, bdlde::Base64Alphabet::e_URL);
//..
// Then, we check the attributes:
//..
//  assert(customOptions.maxLineLength() == 200);
//  assert(customOptions.alphabet()      == bdlde::Base64Alphabet::e_URL);
//  assert(customOptions.isPadded()      == true);
//..
// Now, we stream the object:
//..
//  cout << customOptions << endl;
//..
// Finally, we observe the output:
//..
//  [ maxLineLength = 200 alphabet = URL isPadded = true ]
//..

#include <bdlde_base64alphabet.h>

#include <bdlde_base64encoderoptions.h>

#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

                          // ==========================
                          // class Base64DecoderOptions
                          // ==========================

class Base64DecoderOptions {
    // DATA
    bool                 d_unrecognizedIsError;
    bool                 d_isPadded;
    Base64Alphabet::Enum d_alphabet;

    // PRIVATE CREATORS
    Base64DecoderOptions(bool                 unrecognizedIsError,
                         Base64Alphabet::Enum alphabet,
                         bool                 padded);
        // Create a 'Base64DecoderOptions' object having the specified
        // alphabet, 'padded', and 'unrecognizedIsError' attribute values.  The
        // behavior is unless 'alphabet is a defined value of
        // 'Base64Alphabet::Enum'.

  public:
    // CLASS METHODS
    static
    Base64DecoderOptions custom(
                       bool                 unrecognizedIsError = true,
                       Base64Alphabet::Enum alphabet = Base64Alphabet::e_BASIC,
                       bool                 padded   = true);
        // Return a 'Base64DecoderOptions' object having the specified
        // 'alphabet, 'padded', and 'unrecognizedIsError' attribute values.
        // The behavior is unless '0 <= maxLineLength' and 'alphabet is a
        // defined value of 'Base64Alphabet::Enum'.

    static
    Base64DecoderOptions custom(
                       const Base64EncoderOptions& encoderOptions,
                       bool                        unrecognizedIsError = true);
        // Return a 'Base64DecoderOptions' object having the specified
        // 'alphabet, 'padded', and 'unrecognizedIsError' attribute values.
        // The behavior is unless '0 <= maxLineLength' and 'alphabet is a
        // defined value of 'Base64Alphabet::Enum'.

    static
    Base64DecoderOptions mime(bool unrecognizedIsError = true);
        // Return a 'Base64DecoderOptions' object having the attributes
        // 'alphabet == Base64Alphabet::e_BASIC', 'isPadded == true', and the
        // specified 'unrecognizedIsError'.  If 'unrecognizedIsError' is not
        // specified, it defaults to 'true'.

    static
    Base64DecoderOptions standard(bool unrecognizedIsError = true,
                                  bool padded              = true);
        // Return a 'Base64DecoderOptions' object having the attributes
        // 'alphabet == Base64Alphabet::e_BASIC' and the specified 'isPadded'.
        // If 'padded' is not specified, it defaults to 'true'.  If
        // 'unrecognizedIsError' is not specified, it defaults to 'true'.

    static
    Base64DecoderOptions urlSafe(bool unrecognizedIsError = true,
                                 bool padded              = true);
        // Return a 'Base64DecoderOptions' object having the attributes
        // 'alphabet == Base64Alphabet::e_URL' and the specified 'padded' and
        // 'unrecognizedIsError'.  If 'padded' is not specified, it defaults to
        // 'true'.  If 'unrecognizedIsError' is not specified, it defaults to
        // 'true'.

    // CREATORS
    Base64DecoderOptions();
        // Create a 'Base64DecoderOptions' object having the 'mime' options:
        // 'alphabet == e_BASIC', 'isPadded == true', and
        // 'unrecognizedIsError = true' attribute values.

    // Base64DecoderOptions(const Base64DecoderOptions&) = default;

    // ~Base64DecoderOptions() = default;

    // MANIPULATORS
    // Base64DecoderOptions& operator=(const Base64DecoderOptions&) = default;

    void setAlphabet(Base64Alphabet::Enum value);
        // Set the 'alphabet' attribute to the specified 'value'.  The behavior
        // is undefined unless 'value' is either 'e_BASIC' or 'e_UTL'.

    void setIsPadded(bool value);
        // Set the 'isPadded' attribute to the specified 'value'.

    void setUnrecognizedIsError(int value);
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

    bool unrecognizedIsError() const;
        // Return the value of the 'unrecognizedIsError' attribute.
};

// FREE OPERATORS
bool operator==(const Base64DecoderOptions& lhs,
                const Base64DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

bool operator!=(const Base64DecoderOptions& lhs,
                const Base64DecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const Base64DecoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace
}  // close enterprise namespace

// TRAITS
namespace bsl {

template <>
struct is_trivially_copyable<BloombergLP::bdlde::Base64DecoderOptions> :
                                                             bsl::true_type {};


}  // close namespace bsl

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bdlde {

                        // --------------------------
                        // class Base64DecoderOptions
                        // --------------------------

// PRIVATE CREATORS
inline
Base64DecoderOptions::Base64DecoderOptions(
                                      bool                 unrecognizedIsError,
                                      Base64Alphabet::Enum alphabet,
                                      bool                 padded)
: d_unrecognizedIsError(unrecognizedIsError)
, d_isPadded(padded)
, d_alphabet(alphabet)
{
    BSLS_ASSERT(Base64Alphabet::e_BASIC == alphabet ||
                                            Base64Alphabet::e_URL == alphabet);
}

// CLASS METHODS
inline
Base64DecoderOptions Base64DecoderOptions::custom(
                                      bool                 unrecognizedIsError,
                                      Base64Alphabet::Enum alphabet,
                                      bool                 padded)
{
    return Base64DecoderOptions(unrecognizedIsError,
                                alphabet,
                                padded);
}

inline
Base64DecoderOptions Base64DecoderOptions::custom(
                               const Base64EncoderOptions& encoderOptions,
                               bool                        unrecognizedIsError)
{
    return Base64DecoderOptions(unrecognizedIsError,
                                encoderOptions.alphabet(),
                                encoderOptions.isPadded());
}

inline
Base64DecoderOptions Base64DecoderOptions::mime(bool unrecognizedIsError)
{
    return Base64DecoderOptions(unrecognizedIsError,
                                Base64Alphabet::e_BASIC,
                                true);
}

inline
Base64DecoderOptions Base64DecoderOptions::standard(bool unrecognizedIsError,
                                                    bool padded)
{
    return Base64DecoderOptions(unrecognizedIsError,
                                Base64Alphabet::e_BASIC,
                                padded);
}

inline
Base64DecoderOptions Base64DecoderOptions::urlSafe(bool unrecognizedIsError,
                                                   bool padded)
{
    return Base64DecoderOptions(unrecognizedIsError,
                                Base64Alphabet::e_URL,
                                padded);
}

// CREATORS
inline
Base64DecoderOptions::Base64DecoderOptions()
: d_unrecognizedIsError(true)
, d_isPadded(true)
, d_alphabet(Base64Alphabet::e_BASIC)
{}

// MANIPULATORS
inline
void Base64DecoderOptions::setAlphabet(Base64Alphabet::Enum value)
{
    BSLS_ASSERT(Base64Alphabet::e_BASIC == value ||
                                               Base64Alphabet::e_URL == value);

    d_alphabet = value;
}

inline
void Base64DecoderOptions::setIsPadded(bool value)
{
    d_isPadded = value;
}

inline
void Base64DecoderOptions::setUnrecognizedIsError(int value)
{
    BSLS_ASSERT(0 <= value);

    d_unrecognizedIsError = value;
}

// ACCESSORS
inline
Base64Alphabet::Enum Base64DecoderOptions::alphabet() const
{
    return d_alphabet;
}

inline
bool Base64DecoderOptions::isPadded() const
{
    return d_isPadded;
}

inline
bool Base64DecoderOptions::unrecognizedIsError() const
{
    return d_unrecognizedIsError;
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
