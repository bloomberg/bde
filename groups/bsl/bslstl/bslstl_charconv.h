// bslstl_charconv.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_CHARCONV
#define INCLUDED_BSLSTL_CHARCONV

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations for functions not in the system library.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_charconv.h
//
//@SEE_ALSO: bsl+bslhdrs
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_charconv.h>' instead.  This component provides implementations for
// standard algorithms that are not provided by the underlying standard library
// implementation.  For example, 'to_chars' is a C++17 algorithm, and it is
// provided here for code using C++03 - C++14 or for compilers that do not
// provide '<charconv>' in C++17.
//
// 'to_chars' is locale-independent, non-allocating, and non-throwing, and
// provides a safe and more performant alternative to 'snprintf' in contexts
// where complex formatting options or locale are not important.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Demonstrating Writing a number to a 'streambuf'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to write a function that writes an 'int' to a 'streambuf'.
// We can use 'bsl::to_chars' to write the 'int' to a buffer, then write the
// buffer to the 'streambuf'.
//
// First, we declare our function:
//..
//  void writeJsonScalar(std::streambuf *result, int value)
//      // Write the specified 'value', in decimal, to the specified 'result'.
//  {
//..
// Then, we declare a buffer long enough to store any 'int' value in decimal.
//..
//      char buffer[11];        // size large enough to write 'INT_MIN', the
//                              // worst-case value, in decimal.
//..
// Next, we declare a variable to store the return value:
//..
//      bsl::to_chars_result sts;
//..
// Then, we call the function:
//..
//      sts = bsl::to_chars(buffer, buffer + sizeof(buffer), value);
//..
// Next, we check that the buffer was long enough, which should always be the
// case:
//..
//      assert(bsl::ErrcEnum() == sts.ec);
//..
// Now, we check that 'sts.ptr' is in the range
// '[ buffer + 1, buffer + sizeof(buffer) ]', which will always be the case
// whether 'to_chars' succeeded or failed.
//..
//      assert(buffer  <  sts.ptr);
//      assert(sts.ptr <= buffer + sizeof(buffer));
//..
// Finally, we write our buffer to the 'streambuf':
//..
//      result->sputn(buffer, sts.ptr - buffer);
//  }
//..

#include <bslscm_version.h>

#include <bslstl_errc.h>

#include <bslalg_numericformatterutil.h>
#include <bslmf_assert.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_removecv.h>
#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)

#include <charconv>

#endif

namespace BloombergLP {
namespace bslstl {

                            // ========================
                            // struct 'to_chars_result'
                            // ========================

struct to_chars_result {
    // This 'struct' represents the result of the 'to_chars' function.  On a
    // successful call to 'to_chars_result', 'ptr' is the one past the end
    // pointer of the sequence of characters written, and 'ec' is a default
    // constructed ErrcEnum.  On failure, 'ptr' is set to the end of the buffer
    // supplied to 'to_chars' and 'ec' is set to 'errc::value_to_large'.

    // PUBLIC DATA
    char          *ptr;
    bsl::ErrcEnum  ec;
};

// FREE OPERATORS
template <class INTEGRAL_TYPE>
to_chars_result
to_chars(char *first, char *last, INTEGRAL_TYPE value, int base = 10);
    // Write the specified 'value' into the character buffer starting a the
    // specified 'first' and ending at the specified 'last'.  Optionally
    // specify 'base', the base in which the number is to be written.  If
    // 'base' is not specified, decimal is used.  Return a 'to_chars_result'
    // 'struct' indicating success or failure, and the end of the written
    // result.  On success, the output string is to begin at 'first', the 'ptr'
    // field in the return value is to point at the end of the representation,
    // and the 'ec' field will be 0.  If the buffer specified by
    // '[ first .. last )' is not large enough for the result, return a
    // 'struct' with 'ptr' set to 'last' and 'ec' set to
    // 'errc::value_too_large'.  Insufficient room in the output buffer is the
    // only failure mode.  The behavior is undefined unless 'first < last' and
    // 'base' is in the range '[ 2 .. 36 ]'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

// FREE OPERATORS
template <class INTEGRAL_TYPE>
inline
to_chars_result
to_chars(char *first, char *last, INTEGRAL_TYPE value, int base)
{
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);
    BSLS_ASSERT(first < last);

    typedef bslalg::NumericFormatterUtil Util;
    typedef bsls::Types::Uint64          Uint64;

    BSLMF_ASSERT(bsl::is_integral<INTEGRAL_TYPE>::value);
    BSLMF_ASSERT(!(bsl::is_same<typename bsl::remove_cv<INTEGRAL_TYPE>::type,
                                bool>::value));
    BSLMF_ASSERT(sizeof(INTEGRAL_TYPE) <= sizeof(Uint64));

    char *end = Util::toChars(first, last, value, base);
    if (!end) {
        const to_chars_result ret = { last, bsl::errc::value_too_large };
        return ret;                                                   // RETURN
    }

    const to_chars_result ret = { end , bsl::ErrcEnum() };
    return ret;
}

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)

using std::to_chars_result;
using std::to_chars;
using std::from_chars;
using std::from_chars_result;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV)
using std::chars_format;
#endif

#else

using BloombergLP::bslstl::to_chars_result;
using BloombergLP::bslstl::to_chars;

#endif

}  // close namespace bsl

#endif  // INCLUDED_BSLSTL_CHARCONV

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
