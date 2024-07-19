// bslfmt_formatterutils.h                                            -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERUTILS
#define INCLUDED_BSLFMT_FORMATTERUTILS

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

struct Formatter_UnicodeUtils {
  public:
    // PUBLIC TYPES
    struct CodePointExtractionResult {
      public:
        // PUBLIC TYPES
        bool              isValid;
        int               numSourceBytes;
        int               sourceEncoding;
        unsigned long int codePointValue;
    };

    // CLASS METHODS

    /// Extract a UTF-8 code point from no more than the specified `maxBytes`
    /// of the byte stream at the specified `bytes` location. Return a `
    /// Extract a UTF-8 code point from no more than `maxBytes` of the byte
    /// stream at the specified `bytes` location. Return a
    /// `CodePointExtractionResult` providing a decode status and, if the
    /// decode is valid, a count of the source bytes used and the decoded
    /// Unicode code point value. Byte Order Markers are not supported.
    static CodePointExtractionResult extractUtf8(void *bytes, int maxBytes);

    /// Extract a UTF-16 code point from no more than the specified `maxBytes`
    /// of the byte stream at the specified `bytes` location. Return a
    /// `CodePointExtractionResult` providing a decode status and, if the
    /// decode is valid, a count of the source bytes used and the decoded
    /// Unicode code point value. Behavior is undefined if `bytes` is not a
    /// valid pointer to an array of `numBytes/2` `wchar_t` types in contiguous
    /// memory. Behaviour is undefined if `16 != sizeof(wchar_t)`. Endianness
    /// is assumed to be the same as for the `wchar_t` type and Byte Order
    /// Markers are not supported.
    static CodePointExtractionResult extractUtf16(void *bytes, int maxBytes);

    /// Extract a UTF-32 code point from no more than the specified `maxBytes`
    /// of the byte stream at the specified `bytes` location. Return a
    /// `CodePointExtractionResult` providing a decode status and, if the
    /// decode is valid, a count of the source bytes used and the decoded
    /// Unicode code point value. Behavior is undefined if `bytes` is not a
    /// valid pointer to an array of `numBytes/2` `wchar_t` types in contiguous
    /// memory. Behaviour is undefined if `32 != sizeof(wchar_t)`. Endianness
    /// is assumed to be the same as for the `wchar_t` type and Byte Order
    /// Markers are not supported.
    static CodePointExtractionResult extractUtf32(void *bytes, int maxBytes);
};

template <class t_CHAR>
struct Formatter_CharUtils {
  public:
    // CLASS METHODS
};

template <>
struct Formatter_CharUtils<char> {
  public:
    // CLASS METHODS
    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char *begin,
                                     const char *end,
                                     t_ITERATOR  out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, char>::value));

        return bsl::copy(begin, end, out);
    }

    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char v, t_ITERATOR out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, char>::value));

        *out++ = v;

        return out;
    }
};


template <>
struct Formatter_CharUtils<wchar_t> {
  public:
    // CLASS METHODS
    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char *begin,
                                     const char *end,
                                     t_ITERATOR  out)
    {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
            valuetype;
        BSLMF_ASSERT((bsl::is_same<valuetype, wchar_t>::value));

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
        BSLMF_ASSERT((bsl::is_same<valuetype, wchar_t>::value));

        static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());

        *out++ = ct.widen(v);

        return out;
    }
};

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
