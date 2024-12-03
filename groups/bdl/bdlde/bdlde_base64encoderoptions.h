// bdlde_base64encoderoptions.h                                       -*-C++-*-
#ifndef INCLUDED_BDLDE_BASE64ENCODEROPTIONS
#define INCLUDED_BDLDE_BASE64ENCODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_base64encoderoptions_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide a value-semantic attribute class for encoder options.
//
//@CLASSES:
//  bdlde::Base64EncoderOptions: options for encoder
//
//@SEE_ALSO: bdlde_base64decoderoptions, bdlde_base64encoder,
//           bdlde_base64decoder, bdlde_base64alphabet
//
//@DESCRIPTION: This component provides a value-semantic attribute class for
// specifying options for `bdlde::Base64Encoder`.
//
// This `class` supports default-generated copy construction and copy
// assignment, but the constructor is private.  To create an object one must
// call one of the class methods, which will return a newly-constructed object
// by value.  Specialized class methods are provided to create objects
// configured for the `mime`, `urlSafe`, and `standard` configurations.
//
// Other configurations may be obtained by specifying arguments to the `custom`
// class method, or by calling the setters after the object is created.
//
///Attributes
///----------
// ```
// Name           Type
// -------------  --------------------
// maxLineLength  int
// alphabet       Base64Alphabet::Enum
// isPaded        bool
// ```
// * `maxLineLength`: the maximum number of Base64 characters to output before
//    inserting a '\n'. If this value is 0, maxLineLength is ignored, and no
//    '\n' characters are output in the Base64 text.
//
// * `alphabet`: describes the set of available characters (or alphabet) that
//    may appear in the resulting encoded text.  Note that Base64 encoding
//    breaks binary data into 64 bit blocks, where the numeric values 0-61 are
//    encoded using [0-9a-zA-Z], the alphabet only impacts how the values 62
//    and 63 are represented:
//
//    - `e_BASIC` : defined in standard RFC 2045 section 6.8, and should be the
//      default choice, uses '+' and '/' for 62 and 63, respectively,
//
//    - `e_URL` : defined in RFC 4648 section 5, creates an encoded
//      representation suitable for use in a URL or filename. The alphabet
//      avoids characters like '/' that have a special meaning in the context
//      of a URL or filename. Uses '-' and '_' for 62 and 63, respectively.
//
// * `isPadded` : if true, indicates that output is padded with '=' characters
//   to a multiple of 4 characters.
//
///Pre-Defined Styles
///------------------
// The constructor is private; the client is expected to use the public class
// methods to create 'option's objects.  The 'custom' class method can create
// any arbitrary value, plus there are 3 pre-defined class methods.  The
// following table shows the hard-wired attribute values of these class methods
// -- if a value is in parentheses, it means the attribute is a parameter of
// the method and the value in parentheses is its default value.
//
// | class method | maxLineLength | alphabet | isPadded |
// | :----------- | :-----------: | :------: | :------: |
// | custom       |      any      |    any   |    any   |
// | mime         |      76       |  e_BASIC |   true   |
// | standard     | 0  [disabled] |  e_BASIC |  (true)  |
// | urlSafe      | 0  [disabled] |   e_URL  |  (false) |
//
// Note that in the above table, values in parentheses -- e.g. (true) indicate
// a default which may be overrideen by a user-supplied value.
//
///Standards References
///--------------------
// Below is some background on standards related to base64 encoding.
//
/// RFC-2045 - Mime Standard
///- - - - - - - - - - - - -
// The Base64 encoding originated as a way of encoding arbitrary binary data
// into the body of ASCII-only emails.  This was the MIME standard,
// https://datatracker.ietf.org/doc/html/rfc2045#section-6.8
//
// The `mime` factory function creates a Base64Encoder configured to the MIME
// specification.
//
/// RFC-4648 - The Base16|32|64 Encoding Standard
/// - - - - - - - - - - - - - - - - - - - - - - -
// Eventually, Base64 encoding was separately standardized in RFC-4648:
// https://datatracker.ietf.org/doc/html/rfc464
//
// This standard differs slightly from the original MIME Base64 encoding in
// that the encoding doesn't have line feeds, and the padding at the end of a
// line is optional.
//
// This standard provides a couple variants of the alphabet of characters that
// can be used to encode the data.
//
// **Standard Alphabet** - the standard alphabet is described in
// https://datatracker.ietf.org/doc/html/rfc4648#section-4.  A factory function
// `standard` is provided to create a `Base64Encoder` configured to use the
// standard alphabet.
//
// ** URL and Filename Safe Alphabet** - An alternative alphabet for encoding
// data to be used in file names or URLs in
// https://datatracker.ietf.org/doc/html/rfc4648#section-5.  A factory function
// `urlSafe` is provided to create a `Base64Encoder` configured to use the URL
// and filename safe alphabet.  This alphabet avoids characters, like '/', that
// have a special meaning the the context of a file name or URL.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we want a `Base64EncoderOptions` object configured for MIME
// encoding, meaning `maxLineLength == 76`, `alphabet == e_BASIC`, and
// `isPadded == true`.
//
// First, it turns out that those are the default values of the attributes, so
// all we have to do is default construct an object, and we're done.
// ```
// const bdlde::Base64EncoderOptions& mimeOptions =
//                                        bdlde::Base64EncoderOptions::mime();
// ```
// Then, we check the attributes:
// ```
// assert(mimeOptions.maxLineLength() == 76);
// assert(mimeOptions.alphabet()      == bdlde::Base64Alphabet::e_BASIC);
// assert(mimeOptions.isPadded()      == true);
// ```
// Now, we stream the object:
// ```
// mimeOptions.print(cout);
// ```
// Finally, we observe the output:
// ```
// [
//     maxLineLength = 76
//     alphabet = BASIC
//     isPadded = true
// ]
// ```
//
///Example 2:
/// - - - - -
// Suppose we want a `Base64EncoderOptions` object configured for translating
// URL's.  That would mean a `maxLineLength == 0`, `alphabet == e_URL`, and
// `isPadded == false`.
//
// First, the class method `urlSafe` returns an object configured exactly that
// way, so we simply call it:
// ```
// const bdlde::Base64EncoderOptions& urlOptions =
//                                     bdlde::Base64EncoderOptions::urlSafe();
// ```
// Then, we check the attributes:
// ```
// assert(urlOptions.maxLineLength() == 0);
// assert(urlOptions.alphabet()      == bdlde::Base64Alphabet::e_URL);
// assert(urlOptions.isPadded()      == false);
// ```
// Now, we stream the object:
// ```
// urlOptions.print(cout);
// ```
// Finally, we observe the output:
// ```
// [
//     maxLineLength = 0
//     alphabet = URL
//     isPadded = false
// ]
// ```
//
///Example 3:
/// - - - - -
// Suppose we want an options object configured for standard Base64:
//
// First, we can simply call the `standard` class method:
// ```
// const bdlde::Base64EncoderOptions& standardOptions =
//                                    bdlde::Base64EncoderOptions::standard();
// ```
// Then, we check the attributes:
// ```
// assert(standardOptions.maxLineLength() == 0);
// assert(standardOptions.alphabet()      == bdlde::Base64Alphabet::e_BASIC);
// assert(standardOptions.isPadded()      == true);
// ```
// Now, we stream the object:
// ```
// standardOptions.print(cout);
// ```
// Finally, we observe the output:
// ```
// [
//     maxLineLength = 0
//     alphabet = BASIC
//     isPadded = true
// ]
// ```
//
///Example 4:
/// - - - - -
// Suppose we want a really strangely configured options object with
// `maxLineLength == 200`, `alphabet == e_URL`, and padding.
//
// First, we can simply call the `custom` class method:
// ```
// const bdlde::Base64EncoderOptions& customOptions =
//           bdlde::Base64EncoderOptions::custom(200,
//                                               bdlde::Base64Alphabet::e_URL,
//                                               true);
// ```
// Then, we check the attributes:
// ```
// assert(customOptions.maxLineLength() == 200);
// assert(customOptions.alphabet()      == bdlde::Base64Alphabet::e_URL);
// assert(customOptions.isPadded()      == true);
// ```
// Now, we stream the object:
// ```
// cout << customOptions << endl;
// ```
// Finally, we observe the output:
// ```
// [ maxLineLength = 200 alphabet = URL isPadded = true ]
// ```

#include <bdlde_base64alphabet.h>

#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

                          // ==========================
                          // class Base64EncoderOptions
                          // ==========================

/// This `class` stores the configuration of a `Base64Encoder`.
class Base64EncoderOptions {

    // DATA
    int                  d_maxLineLength;    // the max line length of output
                                             // between CRLF's.  A value of
                                             // 0 means no CRLF's will be
                                             // added

    Base64Alphabet::Enum d_alphabet;         // the alphabet to be used --
                                             // basic or url

    bool                 d_isPadded;         // is the output to be padded with
                                             // '='s

  public:
    // PUBLIC TYPES
    enum { k_MIME_MAX_LINE_LENGTH = 76 };

  private:
    // PRIVATE CREATORS

    /// Create a `Base64EncoderOptions` object having the specified
    /// `maxLineLength, `alphabet', and `isPadded` attribute values.  The
    /// behavior is unless `0 <= maxLineLength` and `alphabet` is a defined
    /// value of `Base64Alphabet::Enum`.
    Base64EncoderOptions(int                  maxLineLength,
                         Base64Alphabet::Enum alphabet,
                         bool                 padded);

  public:
    // CLASS METHODS

    /// Return a `Base64EncoderOptions` object having the specified
    /// `maxLineLength, alphabet, and `isPadded' attribute values.  The
    /// behavior is unless `0 <= maxLineLength` and 'alphabet is a defined
    /// value of `Base64Alphabet::Enum`.
    static
    Base64EncoderOptions custom(int                  maxLineLength,
                                Base64Alphabet::Enum alphabet,
                                bool                 padded);

    /// Return a `Base64EncoderOptions` object having the attributes
    /// `maxLineLength == 76`, `alphabet == Base64Alphabet::e_BASIC`, and
    /// `isPadded == true`.  This conforms to RFC 2045.
    static
    Base64EncoderOptions mime();

    /// Return a `Base64EncoderOptions` object having the attributes
    /// `maxLineLength == 0`, `alphabet == Base64Alphabet::e_BASIC`, and
    /// `isPadded == false`.  If `padded` is not specified, it defaults to
    /// `true`.  This conforms to RFC 4648 section 4.
    static
    Base64EncoderOptions standard(bool padded = true);

    /// Return a `Base64EncoderOptions` object having the attributes
    /// `maxLineLength == 0`, `alphabet == Base64Alphabet::e_URL`, and
    /// the specified `padded`.  If `padded` is not specified, it defaults
    /// to `false`.  This conforms to RFC 4648 section 5.
    static
    Base64EncoderOptions urlSafe(bool padded = false);

    // CREATORS

    /// Default copy constructor.
    //! Base64EncoderOptions(const Base64EncoderOptions&) = default;

    /// Destroy this object.
    //! ~Base64EncoderOptions() = default;

    // MANIPULATORS

    /// Default operator=().
    //! Base64EncoderOptions& operator=(const Base64EncoderOptions&) = default;

    /// Set the `alphabet` attribute to the specified `value`.  The behavior
    /// is undefined unless `value` is either `e_BASIC` or `e_UTL`.
    void setAlphabet(Base64Alphabet::Enum value);

    /// Set the `isPadded` attribute to the specified `value`.
    void setIsPadded(bool value);

    /// Set the `maxLineLength` attribute to the specified `value`.  The
    /// behavior is undefined unless `0 <= value`.
    void setMaxLineLength(int value);

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Return the value of the `alphabet` attribute.
    Base64Alphabet::Enum alphabet() const;

    /// Return the value of the `isPadded` attribute.
    bool isPadded() const;

    /// Return the value of the `maxLineLength` attribute.
    int maxLineLength() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
bool operator==(const Base64EncoderOptions& lhs,
                const Base64EncoderOptions& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
bool operator!=(const Base64EncoderOptions& lhs,
                const Base64EncoderOptions& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const Base64EncoderOptions& rhs);

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

// PRIVATE CREATORS
inline
Base64EncoderOptions::Base64EncoderOptions(int                  maxLineLength,
                                           Base64Alphabet::Enum alphabet,
                                           bool                 padded)
: d_maxLineLength(maxLineLength)
, d_alphabet(alphabet)
, d_isPadded(padded)
{
    BSLS_ASSERT(0 <= maxLineLength);
    BSLS_ASSERT(Base64Alphabet::e_BASIC == alphabet ||
                                            Base64Alphabet::e_URL == alphabet);
}

// CLASS METHODS
inline
Base64EncoderOptions Base64EncoderOptions::custom(
                                            int                  maxLineLength,
                                            Base64Alphabet::Enum alphabet,
                                            bool                 padded)
{
    return Base64EncoderOptions(maxLineLength, alphabet, padded);
}

inline
Base64EncoderOptions Base64EncoderOptions::mime()
{
    return Base64EncoderOptions(k_MIME_MAX_LINE_LENGTH,
                                Base64Alphabet::e_BASIC,
                                true);
}

inline
Base64EncoderOptions Base64EncoderOptions::standard(bool padded)
{
    return Base64EncoderOptions(0, Base64Alphabet::e_BASIC, padded);
}

inline
Base64EncoderOptions Base64EncoderOptions::urlSafe(bool padded)
{
    return Base64EncoderOptions(0, Base64Alphabet::e_URL, padded);
}

// MANIPULATORS
inline
void Base64EncoderOptions::setAlphabet(Base64Alphabet::Enum value)
{
    BSLS_ASSERT(Base64Alphabet::e_BASIC == value ||
                                               Base64Alphabet::e_URL == value);

    d_alphabet = value;
}

inline
void Base64EncoderOptions::setIsPadded(bool value)
{
    d_isPadded = value;
}

inline
void Base64EncoderOptions::setMaxLineLength(int value)
{
    BSLS_ASSERT(0 <= value);

    d_maxLineLength = value;
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
