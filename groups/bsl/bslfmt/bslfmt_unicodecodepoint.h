// bslfmt_unicodecodepoint.h                                          -*-C++-*-

#ifndef INCLUDED_BSLFMT_UNICODECODEPOINT
#define INCLUDED_BSLFMT_UNICODECODEPOINT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a unicode code point representation
//
//@CLASSES:
//  bslfmt::UnicodeCodePoint: unicode code point representation
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, `bslfmt::UnicodeCodePoint`, that is used
// to encapsulate a Unicode code point attributes.
//
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

#include <bsla_unreachable.h>

#include <bsls_assert.h>

#include <string.h>  // `std::size_t`

namespace BloombergLP {
namespace bslfmt {

                        // ======================
                        // class UnicodeCodePoint
                        // ======================

/// This simply constrained (value-semantic) attribute class characterizes a
/// Unicode code point represented by a set of integral values.  This class is
/// private to this package and should not be used by clients.
class UnicodeCodePoint {
  public:
    // TYPES
    enum UtfEncoding { e_UTF8, e_UTF16, e_UTF32 };

  private:
    // DATA
    bool              d_isValid;
    int               d_numSourceBytes;
    unsigned long int d_codePointValue;
    int               d_codePointWidth;

    /// Extract a UTF-8 code point from no more than `maxBytes` of the byte
    /// stream at the specified `bytes` location.  Return a
    /// `CodePointExtractionResult` providing a decode status and, if the
    /// decode is valid, a count of the source bytes used and the decoded
    /// Unicode code point value.  Byte Order Markers are not supported.
    void extractUtf8(const void *bytes, size_t maxBytes);

    /// Extract a UTF-16 code point from no more than the specified `maxBytes`
    /// of the byte stream at the specified `bytes` location.  Return a
    /// `CodePointExtractionResult` providing a decode status and, if the
    /// decode is valid, a count of the source bytes used and the decoded
    /// Unicode code point value.  Behavior is undefined if `bytes` is not a
    /// valid pointer to an array of `numBytes/2` `wchar_t` types in contiguous
    /// memory.  Behaviour is undefined if `16 != sizeof(wchar_t)`.  Endianness
    /// is assumed to be the same as for the `wchar_t` type and Byte Order
    /// Markers are not supported.
    void extractUtf16(const void *bytes, size_t maxBytes);

    /// Extract a UTF-32 code point from no more than the specified `maxBytes`
    /// of the byte stream at the specified `bytes` location.  Return a
    /// `CodePointExtractionResult` providing a decode status and, if the
    /// decode is valid, a count of the source bytes used and the decoded
    /// Unicode code point value.  Behavior is undefined if `bytes` is not a
    /// valid pointer to an array of `numBytes/2` `wchar_t` types in contiguous
    /// memory.  Behaviour is undefined if `32 != sizeof(wchar_t)`.  Endianness
    /// is assumed to be the same as for the `wchar_t` type and Byte Order
    /// Markers are not supported.
    void extractUtf32(const void *bytes, size_t maxBytes);

  public:
    // CREATORS

    /// Create an uninitialized Unicode code point object
    UnicodeCodePoint();

    // MANIPULATORS

    /// Extract a code point from no more than the specified `maxBytes` of the
    /// byte stream at the specified `bytes` location in the specified
    /// `encoding`.  Behavior is undefined if the input bytes are not in the
    /// specified encoding.  Unicode Byte Order Markers are not supported and
    /// behavior is undefined if the input data contains an embedded BOM.
    /// Endianness is assumed to be that of the type pointed to by `bytes`.
    ///
    /// For UTF-8, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes` `unsigned char` types in contiguous memory.
    ///
    /// For UTF-16, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes/2` `wchar_t` types in contiguous memory.
    /// Behaviour is undefined if `2 != sizeof(wchar_t)`.  Endianness is
    /// assumed to be the same as for the `wchar_t` type and Byte Order Markers
    /// are not supported.
    ///
    /// For UTF-32, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes/4` `wchar_t` types in contiguous memory.
    /// Behaviour is undefined if `4 != sizeof(wchar_t)`.  Endianness is
    /// assumed to be the same as for the `wchar_t` type and Byte Order Markers
    /// are not supported.
    void extract(UtfEncoding encoding, const void *bytes, size_t maxBytes);

    /// Reset this object to the default state.
    void reset();

    // ACCESSORS

    /// Return `true` if this object was successfully extracted and contains
    /// valid code point data, and `false` otherwise.
    bool isValid() const;

    /// Return the number of bytes occupied by this code point.
    int numSourceBytes() const;

    /// Return the value of this code point.
    unsigned long int codePointValue() const;

    /// Return the width of this code point.
    int codePointWidth() const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // class UnicodeCodePoint
                          // ----------------------

// CREATORS
inline
UnicodeCodePoint::UnicodeCodePoint()
{
    reset();
}

// MANIPULATORS
inline
void UnicodeCodePoint::extract(UtfEncoding  encoding,
                               const void  *bytes,
                               size_t       maxBytes)
{
    switch (encoding) {
      case e_UTF8: {
        extractUtf8(bytes, maxBytes);
      } break;
      case e_UTF16: {
        extractUtf16(bytes, maxBytes);
      } break;
      case e_UTF32: {
        extractUtf32(bytes, maxBytes);
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("unreachable");
        BSLA_UNREACHABLE;
      }
    }
}

inline
void UnicodeCodePoint::reset()
{
    d_isValid        = false;
    d_numSourceBytes = 0;
    d_codePointValue = 0;
    d_codePointWidth = 0;
}

// ACCESSSORS
inline
bool UnicodeCodePoint::isValid() const
{
    return d_isValid;
}

inline
int UnicodeCodePoint::numSourceBytes() const
{
    return d_numSourceBytes;
}

inline
unsigned long int UnicodeCodePoint::codePointValue() const
{
    return d_codePointValue;
}

inline
int UnicodeCodePoint::codePointWidth() const
{
    return d_codePointWidth;
}

}  // close package namespace
}  // close enterprise namespace


#endif  // INCLUDED_BSLFMT_UNICODECODEPOINT

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
