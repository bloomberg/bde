// bslfmt_formattercharutil.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERCHARUTIL
#define INCLUDED_BSLFMT_FORMATTERCHARUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Character conversion utilities for `bsl::format`.
//
//@CLASSES:
//  bslfmt::FormatterCharUtil: utilities to perform character conversions
//
//@DESCRIPTION: This component provide the `FormatterCharUtil` `struct`
// template that is a namespace to utility functions that convert characters
// (e.g. from `char` to `wchar_t` or lowercase characters to uppercase).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Outputting a hexadecimal in upper case
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to output a hexadecimal number to some object (e.g. some
// character buffer) represented by the output iterator.  Additionally, we are
// required to have the number displayed in uppercase.
// ```
//  char         number[]     = "0x12cd";
//  const size_t sourceLength = sizeof(number) - 1;
// ```
// First, we convert the number to uppercase in place and verify the result:
// ```
//  bslfmt::FormatterCharUtil<char>::toUpper(number, number + sourceLength);
//  const char *expectedUppercaseNumber = "0X12CD";
//  assert(0 == std::strcmp(number, expectedUppercaseNumber));
// ```
// Next, we output this uppercase number to the destination, using
// `outputFromChar` function.  `OutputIterator` in this example is just a
// primitive class that minimally satisfies the requirements of the output
// iterator.
// ```
//  char destination[8];
//  std::memset(destination, 0, sizeof(destination));
//  OutputIterator<char> charIt(destination);
//
//  charIt = bslfmt::FormatterCharUtil<char>::outputFromChar(
//                                                       number,
//                                                       number + sourceLength,
//                                                       charIt);
//
//  assert(destination + sourceLength == charIt.ptr());
//  assert(0 == std::strcmp(number, destination));
// ```
// Finally, we demonstrate the main purpose of these functions - to unify the
// output of values to character strings and wide character strings.  All we
// need to do is just change the template parameter:
// ```
//  wchar_t wDestination[8];
//  std::memset(wDestination, 0, sizeof(wchar_t) * 8);
//
//  wchar_t wcharExpected[] = L"0X12CD";
//
//  OutputIterator<wchar_t> wcharIt(wDestination);
//  wcharIt = bslfmt::FormatterCharUtil<wchar_t>::outputFromChar(
//                                                       number,
//                                                       number + sourceLength,
//                                                       wcharIt);
//  assert(wDestination + sourceLength == wcharIt.ptr());
//  assert(0 == std::wcscmp(wcharExpected, wDestination));
// ```

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>

#include <locale>     // for 'std::ctype', 'locale'

namespace BloombergLP {
namespace bslfmt {

                         // ========================
                         // struct FormatterCharUtil
                         // ========================

/// This struct provides a namespace for a utility functions that convert
/// characters (e.g. `char` to `wchar_t` or lowercase characters to uppercase).
template <class t_CHAR>
struct FormatterCharUtil {
};

/// This is a specialization of `FormatterCharUtil` template providing utility
/// functions for outputting data to the objects specialized by `char` type and
/// referred by the corresponding output iterator.
template <>
struct FormatterCharUtil<char> {
    // CLASS METHODS

    /// Output to the specified output iterator `out` the character sequence
    /// starting at the specified `begin` address and ending immediately before
    /// the specified `end` address.  Return `out` incremented by the number of
    /// characters written.  The behaviour is undefined unless `begin <= end`.
    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char *begin,
                                     const char *end,
                                     t_ITERATOR  out);

    /// Output to the specified output iterator `out` the specified `value`.
    /// Return incremented `out`.
    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char value, t_ITERATOR out);

    /// Convert all characters in the sequence starting at the specified
    /// `begin` address and ending immediately before the specified `end`
    /// address to uppercase.  Note that conversion happens in-place.  The
    /// behaviour is undefined unless `begin <= end`.
    static void toUpper(char *begin, const char *end);
};

/// This is a specialization of `FormatterCharUtil` template providing utility
/// functions for outputting data to the objects specialized by `wchar_t` type
/// and referred by the corresponding output iterator.
template <>
struct FormatterCharUtil<wchar_t> {
    // CLASS METHODS

    /// Output to the specified output iterator `out` the character sequence
    /// starting at the specified `begin` address and ending immediately before
    /// the specified `end` address.  Return `out` incremented by the number of
    /// characters written.  The behaviour is undefined unless `begin <= end`.
    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char *begin,
                                     const char *end,
                                     t_ITERATOR  out);

    /// Output to the specified output iterator `out` the specified `value`.
    /// Return incremented `out`.
    template <class t_ITERATOR>
    static t_ITERATOR outputFromChar(const char value, t_ITERATOR out);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // ------------------------
                         // struct FormatterCharUtil
                         // ------------------------
// CLASS METHODS
template <class t_ITERATOR>
t_ITERATOR FormatterCharUtil<char>::outputFromChar(const char *begin,
                                                   const char *end,
                                                   t_ITERATOR  out)
{
    BSLS_ASSERT(begin <= end);

    typedef typename bsl::iterator_traits<t_ITERATOR>::value_type ValueType;
    BSLMF_ASSERT((bsl::is_same<ValueType, char>::value) ||
                 (bsl::is_same<ValueType, void>::value));

    return bsl::copy(begin, end, out);
}

template <class t_ITERATOR>
t_ITERATOR FormatterCharUtil<char>::outputFromChar(const char value,
                                                   t_ITERATOR out)
{
    typedef typename bsl::iterator_traits<t_ITERATOR>::value_type ValueType;
    BSLMF_ASSERT((bsl::is_same<ValueType, char>::value) ||
                 (bsl::is_same<ValueType, void>::value));

    *out++ = value;

    return out;
}

inline
void FormatterCharUtil<char>::toUpper(char *begin, const char *end)
{
    BSLS_ASSERT(begin <= end);

    for (; begin != end; (void)++begin) {
        if (*begin >= 'a' && *begin <= 'z') {
            *begin = static_cast<char>(*begin + 'A' - 'a');
        }
    }
}

template <class t_ITERATOR>
t_ITERATOR FormatterCharUtil<wchar_t>::outputFromChar(const char *begin,
                                                      const char *end,
                                                      t_ITERATOR  out)
{
    BSLS_ASSERT(begin <= end);

    typedef typename bsl::iterator_traits<t_ITERATOR>::value_type ValueType;
    BSLMF_ASSERT((bsl::is_same<ValueType, wchar_t>::value) ||
                 (bsl::is_same<ValueType, void>::value));

    static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());

    for (; begin != end; (void)++begin, (void)++out) {
        *out = ct.widen(*begin);
    }

    return out;
}

template <class t_ITERATOR>
t_ITERATOR FormatterCharUtil<wchar_t>::outputFromChar(const char value,
                                                      t_ITERATOR out)
{
    typedef typename bsl::iterator_traits<t_ITERATOR>::value_type ValueType;
    BSLMF_ASSERT((bsl::is_same<ValueType, wchar_t>::value) ||
                 (bsl::is_same<ValueType, void>::value));

    static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());

    *out++ = ct.widen(value);

    return out;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERCHARUTIL

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
