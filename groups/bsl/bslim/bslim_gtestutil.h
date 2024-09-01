// bslim_gtestutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLIM_GTESTUTIL
#define INCLUDED_BSLIM_GTESTUTIL

#include <bsls_ident.h>

//@PURPOSE: Provide facilities for debugging BDE with gtest.
//
//@DESCRIPTION: The `bslim_gtestutil` component provides utitlities to
// facilitate testing with Google Test (GTest).
//
///Usage
///-----
// Suppose we have a string `str` that we want to output:
// ```
// bsl::string str =
//                "No matter where you go, There you are! -- Buckaroo Banzai";
// ```
// Call `PrintTo`, passing the string and a pointer to a `bsl::ostream`:
// ```
// PrintTo(str, &cout);
// cout << endl;
// ```
// Which results in the string being streamed to standard output, surrounded by
// double quotes:
// ```
// "No matter where you go, There you are! -- Buckaroo Banzai"
// ```

#include <bslscm_version.h>

#include <bsl_optional.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace testing {
namespace internal {

                      // ===================================
                      // bslim_Gtestutil_TestingStreamHolder
                      // ===================================

/// This `class` serves as a type in the `testing` namespace to be passed to
/// an unqualified call to `PrintTo`.  By supplying
/// `bslim_Gtestutil_TestingStreamHolder(&stream)`, which is implicitly
/// convertible to `bsl::ostream *`, we supply an argument in the `testing`
/// namespace in the `UniversalPrint` call within the
/// `bsl::PrintTo(const optional<TYPE>&, ...)` call below, which will affect
/// ADL to draw in `UniversalPrint` declarations from the `testing`
/// namespace into consideration.  For detailed discussion, see
/// `IMPLEMENTATION NOTE` in the implementation file.
class bslim_Gtestutil_TestingStreamHolder {

    // DATA
    bsl::ostream *d_stream_p;

  public:
    // CREATORS

    /// Create an object bound to the specified `stream`.
    explicit
    bslim_Gtestutil_TestingStreamHolder(bsl::ostream *stream);

    // bslim_Gtestutil_TestingStreamHolder(
    //                   const bslim_Gtestutil_TestingStreamHolder&) = default;

    // MANIPULATORS
    // bslim_Gtestutil_TestingStreamHolder& operator=(
    //                   const bslim_Gtestutil_TestingStreamHolder&) = default;

    // ACCESSORS

    /// Implicitly return a pointer to the stream this object is bound to.
    operator bsl::ostream *() const;
};

}  // close namespace internal
}  // close namespace testing

namespace bsl {

// FREE FUNCTIONS

/// Write the specified `value` to the specified `*stream`, surrounded by
/// double quotes.
void PrintTo(const string& value, ostream *stream);

/// Write the specified `value` to the specified `*stream`, surrounded by
/// double quotes, writing non-printable characters with '\x...' escapes.
void PrintTo(const wstring& value, ostream *stream);

/// Write the specified `value` to the specified `*stream`, surrounded by
/// double quotes.
void PrintTo(const BloombergLP::bslstl::StringRef& value, ostream *stream);

/// Write the specified `value` to the specified `*stream`, surrounded by
/// double quotes.
template <class TYPE>
void PrintTo(const optional<TYPE>& value, ostream *stream);

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

}  // close namespace bsl

namespace testing {
namespace internal {

                      // -----------------------------------
                      // bslim_Gtestutil_TestingStreamHolder
                      // -----------------------------------

// CREATOR
inline
bslim_Gtestutil_TestingStreamHolder::bslim_Gtestutil_TestingStreamHolder(
                                                          bsl::ostream *stream)
: d_stream_p(stream)
{}

// ACCESSORS
inline
bslim_Gtestutil_TestingStreamHolder::operator bsl::ostream *() const
{
    return d_stream_p;
}

}  // close namespace internal
}  // close namespace testing

// FREE FUNCTIONS
template <class TYPE>
inline
void bsl::PrintTo(const bsl::optional<TYPE>& value, bsl::ostream *stream)
{
    *stream << '(';
    if (!value.has_value()) {
        *stream << "nullopt";
    }
    else {
        // 'UniversalPrint' does not need to be forward declared above,
        // provided that it is eventually declared before this template
        // function is called.

        UniversalPrint(*value, testing::internal::
                                  bslim_Gtestutil_TestingStreamHolder(stream));
    }
    *stream << ')';
}

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
