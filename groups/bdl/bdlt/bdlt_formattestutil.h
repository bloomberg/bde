// bdlt_formattestutil.h                                              -*-C++-*-
#ifndef INCLUDED_BDLT_FORMATTESTUTIL
#define INCLUDED_BDLT_FORMATTESTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros and utilities for testing of `format` in bdlt.
//
//@CLASSES:
//  bdlt::FormatTestUtil: namespace class for static functions
//
//@MACROS:
//  BDLT_FORMATTESTUTIL_TEST_FORMAT
//  BDLT_FORMATTESTUTIL_TEST_FORMAT_ARG
//
//@SEE_ALSO: bslfmt::FormatterTestUtil
//
//@DESCRIPTION: This component provides a set of macros and functions that
// support BDE-style testing of the formatting functionality for `bdlt` value
// types.  It provides two macros that facilitate easy invocation of the
// functions defined in the `bslfmt::FormatterTestUtil` utility `struct` for
// both both `char` and `wchar_t`-based strings.  In addition, this component
// provides functions facilitating conversions between `bsl::string` and
// `bsl::wstring`.

#include <bdlscm_version.h>

#include <bslfmt_formattertestutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

namespace BloombergLP {

#define BDLT_FORMATTESTUTIL_TEST_FORMAT(exp, format, value)                   \
    if (!bdlt::FormatTestUtil::testFormat(                                    \
                              __FILE__, __LINE__, exp, format, value)) {      \
        testStatus += testStatus < 100;                                       \
    }

#define BDLT_FORMATTESTUTIL_TEST_FORMAT_ARG(exp, format, value, arg)          \
    if (!bdlt::FormatTestUtil::testFormatArg(                                 \
                              __FILE__, __LINE__, exp, format, value, arg)) { \
        testStatus += testStatus < 100;                                       \
    }

namespace bdlt {

                            // ====================
                            // class FormatTestUtil
                            // ====================

/// This `struct` serves as a namespace for static functions to facilitate
/// testing of formatting of `bdlt` value types -- used in the implementation
/// of the above macros, and for conversion between `bsl::string` and
/// `bsl::wstring`.
struct FormatTestUtil {
    // CLASS METHODS

    /// Convert the string referred to by the specified `viewIn` from `wchar_t`
    /// to `char` and return the resulting `bsl::string` by value.  The
    /// behavior is undefined unless all of the `wchar_t` values in `viewIn`
    /// are ascii.
    static bsl::string narrow(const bsl::wstring_view& viewIn);

    /// Using the specified `format` and `value`, compare the string result of
    /// calling `bsl::format(format, value)` to the specified `exp`.  If they
    /// don't match, write an error message including the specified `fileName`,
    /// `line`, the result of the call, and the specified `exp` to `cout` and
    /// return `false`, otherwise convert the specified `format` and `exp` to
    /// wide strings, and repeat the process with wide strings.  If both tests
    /// passed, return `true`.
    template <class t_VALUE_TYPE>
    static bool testFormat(const char              *fileName,
                           int                      line,
                           const bsl::string_view&  exp,
                           const bsl::string_view&  format,
                           const t_VALUE_TYPE&      value);

    /// Using the specified `format`, `value`, and `arg`, compare the string
    /// result of calling `bsl::format(format, value)` to the specified `exp`.
    /// If they don't match, write an error message including `fileName`,
    /// `line`, the result of the call, `exp`, and `arg` to `cout` and return
    /// `false`, otherwise convert the specified `format` and `exp` to wide
    /// strings, and repeat the process with wide strings.  If both tests
    /// passed, return `true`.
    template <class t_VALUE_TYPE, class t_ARG_TYPE>
    static bool testFormatArg(const char              *fileName,
                              int                      line,
                              const bsl::string_view&  exp,
                              const bsl::string_view&  format,
                              const t_VALUE_TYPE&      value,
                              const t_ARG_TYPE&        arg);

    /// Load the `wstring` version of the specified `char`-based string view
    /// `viewIn` into the specified `result`.  The behavior is undefined unless
    /// all of the `char` values in `viewIn` are ascii.
    static void widen(bsl::wstring *result, const bsl::string_view& viewIn);
};

                            // --------------------
                            // class FormatTestUtil
                            // --------------------

template <class t_VALUE_TYPE>
bool FormatTestUtil::testFormat(const char              *fileName,
                                int                      line,
                                const bsl::string_view&  exp,
                                const bsl::string_view&  format,
                                const t_VALUE_TYPE&      value)
{
    bsl::string message;
    bool rc = bslfmt::FormatterTestUtil<char>::testEvaluateVFormat(
                                           &message, exp, true, format, value);
    if (!rc) {
        bsl::cout << "Error " << fileName << "(" << line <<
                                    "): <char>testEvaluateVFormat(\"" <<
                 format << "\") failed, msg: \"" << message << "\"\n";
        return false;                                                 // RETURN
    }

    bsl::wstring wFormat, expw;
    widen(&wFormat, format);
    widen(&expw,    exp);
    rc = bslfmt::FormatterTestUtil<wchar_t>::testEvaluateVFormat(
                                         &message, expw, true, wFormat, value);
    if (!rc) {
        bsl::cout << "Error " << fileName << "(" << line <<
                                    "): <wchar_t>testEvaluateVFormat(\"" <<
                 format << "\") failed, msg: \"" << message << "\"\n";
        return false;                                                 // RETURN
    }

    return true;
}

template <class t_VALUE_TYPE, class t_ARG_TYPE>
bool FormatTestUtil::testFormatArg(const char              *fileName,
                                   int                      line,
                                   const bsl::string_view&  exp,
                                   const bsl::string_view&  format,
                                   const t_VALUE_TYPE&      value,
                                   const t_ARG_TYPE&        arg)
{
    bsl::string message;
    bool rc = bslfmt::FormatterTestUtil<char>::testEvaluateVFormat(
                                      &message, exp, true, format, value, arg);
    if (!rc) {
        bsl::cout << "Error " << fileName << "(" << line <<
                                    "): <char>testEvaluateVFormat(\"" <<
                 format << "\") failed, msg: \"" << message << "\"\n";
        return false;                                                 // RETURN
    }

    bsl::wstring wFormat, expw;
    widen(&wFormat, format);
    widen(&expw,    exp);
    rc = bslfmt::FormatterTestUtil<wchar_t>::testEvaluateVFormat(
                                    &message, expw, true, wFormat, value, arg);
    if (!rc) {
        bsl::cout << "Error " << fileName << "(" << line <<
                                    "): <wchar_t>testEvaluateVFormat(\"" <<
                 format << "\") failed, msg: \"" << message << "\"\n";
        return false;                                                 // RETURN
    }

    return true;
}

inline
void FormatTestUtil::widen(bsl::wstring            *result,
                           const bsl::string_view&  viewIn)
{
    result->clear();
    result->reserve(viewIn.length());
    typedef bsl::string_view::iterator It;
    for (It it = viewIn.begin(), end = viewIn.end(); it < end; ++it) {
        const char c = *it;
        BSLS_ASSERT_SAFE(!(0x80 & c));

        *result += wchar_t(c);
    }
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
