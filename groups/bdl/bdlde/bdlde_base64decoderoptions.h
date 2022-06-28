// bdlde_base64decoderoptions.h                                       -*-C++-*-
#ifndef INCLUDED_BDLDE_BASE64DECODEROPTIONS
#define INCLUDED_BDLDE_BASE64DECODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_base64decoderoptions_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute class for decoder options.
//
//@CLASSES:
//  bdlde::Base64DecoderOptions: options for decoder
//
//@SEE_ALSO: bdlde_base64encorderoptions, bdlde_base64decorderoptions,
//           bdlde_base64encoder,         bdlde_base64decoder
//           bdlde_base64alphabet
//
//@DESCRIPTION: This component provides a value-semantic attribute class for
// specifying options for 'bdlde::Base64Decoder'.
//
// This 'class' supports default-generated copy construction and copy
// assignment, but the constructor is private.  To create an object one must
// call one of the class methods, which will return a newly-constructed object
// by value.  Specialized class methods are provided to create objects
// configured for the 'mime', 'urlSafe', and 'standard' configurations.
//
// Other configurations may be obtained by specifying arguments to the 'custom'
// class method, or by calling the setters after the object is created.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1:
/// - - - - -
// Suppose we want a 'Base64DecoderOptions' object configured for MIME
// encoding, meaning 'alphabet == e_BASIC', 'isPadded == true', and
// 'ignoreMode = e_IGNORE_WHITESPACE'.
//
// First, we call the 'mime' class method, and we're done.
//..
//  const bdlde::Base64DecoderOptions& mimeOptions =
//                                         bdlde::Base64DecoderOptions::mime();
//..
// Then, we check the attributes:
//..
//  assert(mimeOptions.ignoreMode() ==
//                               bdlde::Base64IgnoreMode::e_IGNORE_WHITESPACE);
//  assert(mimeOptions.alphabet()   == bdlde::Base64Alphabet::e_BASIC);
//  assert(mimeOptions.isPadded()   == true);
//..
// Now, we stream the object:
//..
//  mimeOptions.print(cout);
//..
// Finally, we observe the output:
//..
//  [
//      ignoreMode = IGNORE_WHITESPACE
//      alphabet = BASIC
//      isPadded = true
//  ]
//..
//
///Example 2:
/// - - - - -
// Suppose we want a 'Base64DecoderOptions' object configured for translating
// URL's.  That would mean 'alphabet == e_URL', 'isPadded == false', and
// ignoring neither unrecognized characters nor whitespace.
//
// First, the class method 'urlSafe' returns an object configured exactly that
// way, so we simply call it:
//..
//  const bdlde::Base64DecoderOptions& urlOptions =
//                                      bdlde::Base64DecoderOptions::urlSafe();
//..
// Then, we check the attributes:
//..
//  assert(urlOptions.ignoreMode() == bdlde::Base64IgnoreMode::e_IGNORE_NONE);
//  assert(urlOptions.alphabet()   == bdlde::Base64Alphabet::e_URL);
//  assert(urlOptions.isPadded()   == false);
//..
// Now, we stream the object:
//..
//  urlOptions.print(cout);
//..
// Finally, we observe the output:
//..
//  [
//      ignoreMode = IGNORE_NONE
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
//  assert(standardOptions.ignoreMode() ==
//                                     bdlde::Base64IgnoreMode::e_IGNORE_NONE);
//  assert(standardOptions.alphabet()            ==
//                                             bdlde::Base64Alphabet::e_BASIC);
//  assert(standardOptions.isPadded()            == true);
//..
// Now, we stream the object:
//..
//  standardOptions.print(cout);
//..
// Finally, we observe the output:
//..
//  [
//      ignoreMode = IGNORE_NONE
//      alphabet = BASIC
//      isPadded = true
//  ]
//..
//
///Example 4:
/// - - - - -
// Suppose we want a really strangely configured options object with
// 'alphabet == e_URL', and padding, and ignoreing neither unrecognized
// characters nor whitespace.
//
// First, we can simply call the 'custom' class method.  The 'padded' and
// 'unrecognizedIsError == true' arguments are last, and they default to
// 'true', so we don't have to pass that.
//..
//  const bdlde::Base64DecoderOptions& customOptions =
//                         bdlde::Base64DecoderOptions::custom(
//                                      bdlde::Base64IgnoreMode::e_IGNORE_NONE,
//                                      bdlde::Base64Alphabet::e_URL,
//                                      true);
//..
// Then, we check the attributes:
//..
//  assert(customOptions.ignoreMode() ==
//                                     bdlde::Base64IgnoreMode::e_IGNORE_NONE);
//  assert(customOptions.alphabet()   == bdlde::Base64Alphabet::e_URL);
//  assert(customOptions.isPadded()   == true);
//..
// Now, we stream the object:
//..
//  cout << customOptions << endl;
//..
// Finally, we observe the output:
//..
//  [ ignoreMode = IGNORE_NONE alphabet = URL isPadded = true ]
//..

#include <bdlde_base64alphabet.h>
#include <bdlde_base64ignoremode.h>

#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

                          // ==========================
                          // class Base64DecoderOptions
                          // ==========================

class Base64DecoderOptions {
    // This 'class' stores the configuration of a 'Base64Decoder'.

    typedef Base64IgnoreMode    IgnoreMode;

    // DATA
    IgnoreMode::Enum     d_ignoreMode;    // what types of chars, if any, are
                                          // ignored
    Base64Alphabet::Enum d_alphabet;      // alphabet -- basic or url
    bool                 d_isPadded;      // is input padded with '='?

  private:
    // PRIVATE CREATORS
    Base64DecoderOptions(IgnoreMode::Enum     ignoreMode,
                         Base64Alphabet::Enum alphabet,
                         bool                 padded);
        // Create a 'Base64DecoderOptions' object having the specified
        // 'alphabet', 'padded', and 'ignoreMode' attribute values.  The
        // behavior is undefined unless 'alphabet' is a defined value of
        // 'Base64Alphabet::Enum' and 'ignoreMode' is a defined value of
        // 'Base64IgnoreMode::Enum'.

  public:
    // CLASS METHODS
    static
    Base64DecoderOptions custom(
                       IgnoreMode::Enum     ignoreMode,
                       Base64Alphabet::Enum alphabet,
                       bool                 padded);
        // Return a 'Base64DecoderOptions' object having the specified
        // 'alphabet', 'padded', and 'ignoreMode' attribute values.  The
        // behavior is unless 'ignoreMode' is a defined value of 'IgnoreMode',
        // and 'alphabet' is a defined value of 'Base64Alphabet::Enum'.

    static
    Base64DecoderOptions mime(IgnoreMode::Enum ignoreMode =
                                              IgnoreMode::e_IGNORE_WHITESPACE);
        // Return a 'Base64DecoderOptions' object having the attributes
        // 'alphabet == Base64Alphabet::e_BASIC', 'isPadded == true', and the
        // specified 'ignoreMode'.  If 'ignoreMode' is not specified, it
        // defaults to 'e_IGNORE_WHITESPACE'.  This conforms to RFC 2045.

    static
    Base64DecoderOptions standard(IgnoreMode::Enum ignoreMode =
                                                     IgnoreMode::e_IGNORE_NONE,
                                  bool padded                 = true);
        // Return a 'Base64DecoderOptions' object having the specified
        // 'ignoreMode' and 'padded', and the attribute
        // 'alphabet == Base64Alphabet::e_BASIC'.  If 'padded' is not
        // specified, it defaults to 'true'.  If 'ignoreMode' is not specified,
        // it defaults to 'e_IGNORE_NOTHING'.  This conforms to RFC 4648
        // section 4.

    static
    Base64DecoderOptions urlSafe(IgnoreMode::Enum ignoreMode =
                                                    IgnoreMode::e_IGNORE_NONE,
                                 bool             padded = false);
        // Return a 'Base64DecoderOptions' object having the attributes
        // 'alphabet == Base64Alphabet::e_URL', 'isPadded == false' and the
        // specified 'ignoreMode'.  If 'ignoreMode' is not specified, it
        // defaults to 'e_IGNORE_NOTHING'.  This conforms to RFC 4648 section
        // 5.

    // CREATORS
    // Base64DecoderOptions(const Base64DecoderOptions&) = default;

    // ~Base64DecoderOptions() = default;

    // MANIPULATORS
    // Base64DecoderOptions& operator=(const Base64DecoderOptions&) = default;

    void setAlphabet(Base64Alphabet::Enum value);
        // Set the 'alphabet' attribute to the specified 'value'.  The behavior
        // is undefined unless 'value' is either 'e_BASIC' or 'e_UTL'.

    void setIgnoreMode(IgnoreMode::Enum value);
        // Set the 'ignoreMode' attribute to the specified 'value'.  The
        // behavior is undefined unless 'value' is valid value of the
        // 'IgnoreMode' enum.

    void setIsPadded(bool value);
        // Set the 'isPadded' attribute to the specified 'value'.

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

    IgnoreMode::Enum ignoreMode() const;
        // Return the value of the 'ignoreMode' attribute.

    bool isPadded() const;
        // Return the value of the 'isPadded' attribute.
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
Base64DecoderOptions::Base64DecoderOptions(IgnoreMode::Enum     ignoreMode,
                                           Base64Alphabet::Enum alphabet,
                                           bool                 padded)
: d_ignoreMode(ignoreMode)
, d_alphabet(alphabet)
, d_isPadded(padded)
{
    BSLS_ASSERT(Base64IgnoreMode::e_IGNORE_NONE         == ignoreMode ||
                Base64IgnoreMode::e_IGNORE_WHITESPACE   == ignoreMode ||
                Base64IgnoreMode::e_IGNORE_UNRECOGNIZED == ignoreMode);
    BSLS_ASSERT(Base64Alphabet::e_BASIC == alphabet ||
                                            Base64Alphabet::e_URL == alphabet);
}

// CLASS METHODS
inline
Base64DecoderOptions Base64DecoderOptions::custom(
                                               IgnoreMode::Enum     ignoreMode,
                                               Base64Alphabet::Enum alphabet,
                                               bool                 padded)
{
    return Base64DecoderOptions(ignoreMode,
                                alphabet,
                                padded);
}

inline
Base64DecoderOptions Base64DecoderOptions::mime(IgnoreMode::Enum ignoreMode)
{
    return Base64DecoderOptions(ignoreMode,
                                Base64Alphabet::e_BASIC,
                                true);
}

inline
Base64DecoderOptions Base64DecoderOptions::standard(
                                                   IgnoreMode::Enum ignoreMode,
                                                   bool             padded)
{
    return Base64DecoderOptions(ignoreMode,
                                Base64Alphabet::e_BASIC,
                                padded);
}

inline
Base64DecoderOptions Base64DecoderOptions::urlSafe(IgnoreMode::Enum ignoreMode,
                                                   bool             padded)
{
    return Base64DecoderOptions(ignoreMode,
                                Base64Alphabet::e_URL,
                                padded);
}

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
void Base64DecoderOptions::setIgnoreMode(IgnoreMode::Enum value)
{
    BSLS_ASSERT(static_cast<unsigned>(value) <= 2);

    d_ignoreMode = value;
}

// ACCESSORS
inline
Base64Alphabet::Enum Base64DecoderOptions::alphabet() const
{
    return d_alphabet;
}

inline
Base64IgnoreMode::Enum Base64DecoderOptions::ignoreMode() const
{
    return d_ignoreMode;
}

inline
bool Base64DecoderOptions::isPadded() const
{
    return d_isPadded;
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
