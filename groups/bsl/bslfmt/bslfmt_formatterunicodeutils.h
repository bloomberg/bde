// bslfmt_formatterunicodeutils.h                                     -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERUNICODEUTILS
#define INCLUDED_BSLFMT_FORMATTERUNICODEUTILS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Private unicode utilities for use by `bsl::format`
//
//@CLASSES:
//  Formatter_UnicodeUtils: Namespace struct for generated unicode data tables.
//
//@DESCRIPTION: This component is a namespace struct to provide utility
// functions that extract unicode codepoints and associated data from byte
// arrays.
//
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_issame.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formaterror.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

namespace BloombergLP {
namespace bslfmt {

                        // ============================
                        // class Formatter_UnicodeUtils
                        // ============================

struct Formatter_UnicodeUtils {
  public:
    // PUBLIC TYPES

    enum UtfEncoding {
        e_UTF8,
        e_UTF16,
        e_UTF32
    };

    struct CodePointExtractionResult {
      public:
        // PUBLIC TYPES
        bool              isValid;
        int               numSourceBytes;
        UtfEncoding       sourceEncoding;
        unsigned long int codePointValue;
        int               codePointWidth;
    };

    struct GraphemeClusterExtractionResult {
      public:
        // PUBLIC TYPES
        bool              isValid;
        int               numSourceBytes;
        int               numCodePoints;
        UtfEncoding       sourceEncoding;
        unsigned long int firstCodePointValue;
        int               firstCodePointWidth;
    };

    // CLASS METHODS

    /// Extract a code point from no more than the specified `maxBytes` of the
    /// byte stream at the specified `bytes` location in the specified
    /// `encoding`. Return a `CodePointExtractionResult` providing a decode
    /// status and, if the decode is valid, a count of the source bytes used
    /// and the decoded Unicode code point value. Behavior is undefined if the
    /// input bytes are not in the specified encoding. Unicode Byte Order
    /// Markers are not supported and behavior is undefined if the input data
    /// contains an embedded BOM. Endianness is assumed to be that of the type
    /// pointed to by `bytes`.
    ///
    /// For UTF-8, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes` `unsigned char` types in contiguous memory.
    ///
    /// For UTF-16, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes/2` `wchar_t` types in contiguous memory.
    /// Behaviour is undefined if `2 != sizeof(wchar_t)`. Endianness is assumed
    /// to be the same as for the `wchar_t` type and Byte Order Markers are not
    /// supported.
    ///
    /// For UTF-32, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes/4` `wchar_t` types in contiguous memory.
    /// Behaviour is undefined if `4 != sizeof(wchar_t)`. Endianness is assumed
    /// to be the same as for the `wchar_t` type and Byte Order Markers are not
    /// supported.
    static CodePointExtractionResult extractCodePoint(UtfEncoding  encoding,
                                                      const void  *bytes,
                                                      size_t       maxBytes);

    /// Extract a grapheme cluster from no more than the specified `maxBytes`
    /// of the byte stream at the specified `bytes` location in the specified
    /// `encoding`. Return a `GraphemeClusterExtractionResult` providing a
    /// decode status and, if the decode is valid, a count of the source bytes
    /// used and the initial decoded Unicode code point value. Behavior is
    /// undefined if the input bytes are not in the specified encoding. Unicode
    /// Byte Order Markers are not supported and behavior is undefined if the
    /// input data contains an embedded BOM. Endianness is assumed to be that
    /// of the type pointed to by `bytes`.
    ///
    /// For UTF-8, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes` `unsigned char` types in contiguous memory.
    ///
    /// For UTF-16, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes/2` `wchar_t` types in contiguous memory.
    /// Behaviour is undefined if `2 != sizeof(wchar_t)`. Endianness is assumed
    /// to be the same as for the `wchar_t` type and Byte Order Markers are not
    /// supported.
    ///
    /// For UTF-32, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes/4` `wchar_t` types in contiguous memory.
    /// Behaviour is undefined if `4 != sizeof(wchar_t)`. Endianness is assumed
    /// to be the same as for the `wchar_t` type and Byte Order Markers are not
    /// supported.
    static GraphemeClusterExtractionResult extractGraphemeCluster(
                                                        UtfEncoding  encoding,
                                                        const void  *bytes,
                                                        size_t       maxBytes);

    /// Based on the value of the initial byte of a unicode code point in UTF-8
    /// representation, calculate and return the number of bytes used in that
    /// representation. The value of the first byte is specified by
    /// `firstChar`. If `firstChar` does not contain a valid value for the
    /// first byte of a UTF-8 codepoint representation return -1;
    static BSLS_KEYWORD_CONSTEXPR_CPP20 int codepointBytesIfValid(
                                                         const char firstChar);

    /// Based on the value of the initial byte of a unicode code point in
    /// UTF-16 (where `sizeof(wchar_t)==2`) or UTF-32 (where
    /// `sizeof(wchar_t)==4`) representation, calculate and return the number
    /// of bytes used in that representation. The value of the first byte is
    /// specified by `firstChar`. If `firstChar` does not contain a valid value
    /// for the first byte of a UTF-16 or UTF-32 (depending on
    /// `sizeof(wchar_t)`) codepoint representation return -1;
    static BSLS_KEYWORD_CONSTEXPR_CPP20 int codepointBytesIfValid(
                                                      const wchar_t firstChar);
};

template <class t_CHAR>
struct Formatter_CharUtils {
};

template <>
struct Formatter_CharUtils<char> {

    // CLASS DATA
    static const char *const s_toUpper_p;  //

  public:
    // CLASS METHODS
    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char *begin,
                                     const char *end,
                                     t_ITERATOR  out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, char>::value) ||
                     (bsl::is_same<valuetype, void>::value));

        return bsl::copy(begin, end, out);
    }

    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char v, t_ITERATOR out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, char>::value) ||
                     (bsl::is_same<valuetype, void>::value));

        *out++ = v;

        return out;
    }

    static void toUpper(char *begin, const char *end)
    {
        for (; begin != end; (void)++begin) {
            *begin = s_toUpper_p[static_cast<unsigned char>(*begin)];
        }
    }
};


template <>
struct Formatter_CharUtils<wchar_t> {

    // CLASS DATA
    static const char *const s_toUpper_p;  //

  public:
    // CLASS METHODS
    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char *begin,
                                     const char *end,
                                     t_ITERATOR  out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, wchar_t>::value) ||
                     (bsl::is_same<valuetype, void   >::value));

        static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());

        for (; begin != end; (void)++begin, (void)++out) {
            *out = ct.widen(*begin);
        }

        return out;
    }

    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char v, t_ITERATOR out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, wchar_t>::value) ||
                     (bsl::is_same<valuetype, void   >::value));

        static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());

        *out++ = ct.widen(v);

        return out;
    }

    static void toUpper(char *begin, const char *end)
    {
        for (; begin != end; (void)++begin) {
            *begin = s_toUpper_p[static_cast<unsigned char>(*begin)];
        }
    }
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class Formatter_UnicodeUtils
                        // ----------------------------

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 int Formatter_UnicodeUtils::codepointBytesIfValid(
                                                          const char firstChar)
{
    unsigned char c = static_cast<unsigned char>(firstChar);
    if ((c & 0x80) == 0x00)
        return 1;
    else if ((c & 0xe0) == 0xc0)
        return 2;
    else if ((c & 0xf0) == 0xe0)
        return 3;
    else if ((c & 0xf8) == 0xf0)
        return 4;
    else
        return -1;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 int Formatter_UnicodeUtils::codepointBytesIfValid(
                                                      const wchar_t firstChar)
{
    switch (sizeof(wchar_t)) {
      case 2: { // UTF-16
        if (static_cast<unsigned int>(firstChar) <
            static_cast<unsigned int>(0xd800))
            return 2;
        else if (static_cast<unsigned int>(firstChar) <
                 static_cast<unsigned int>(0xdc00))
            return 4;
        else
            return -1;
      } break;
      case 4: { // UTF-32
        if (static_cast<unsigned long>(firstChar) <
            static_cast<unsigned long>(0x80000000U))
            return 4;
        else
            return -1;
      } break;
      default: {
        return -1; // unsuported wchar_t size.
      } break;
    }
}

}  // close namespace bslfmt
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERBASE

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
