// bslfmt_formaterror.h                                               -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATERROR
#define INCLUDED_BSLFMT_FORMATERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception type for format library errors
//
//@CLASSES:
//  bsl::format_error: standard-compliant format library exception type
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides an implementation of the C++20 Standard
// Library's `format_error`, providing an excption type thrown in the event of
// an error in the formatting library.
//
// Where the standard library `<format>` header is available, this is an alias
// to the `std::format_error` type.
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use `bsl::format_error`.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Throw an exception
/// - - - - - - - - - - - - -
//
// Typically this would be thrown from attempts to use the `format` or
// `vformat` functions.  However, as this is at the very bottom of the
// dependency hierarchy the usage example cannot accurately reflect that case.
//
// ```
//   try
//   {
//     throw bsl::format_error("Error message");
//   }
//   catch (const bsl::format_error &exc)
//   {
//     assert(true); // Successful catch.
//   }
// ```
//

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_string.h>

#include <stdexcept>

namespace BloombergLP {
namespace bslfmt {

                             // ==================
                             // class format_error
                             // ==================

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
using std::format_error;
#else
/// This class represents exceptions thrown by the formatting library.
class format_error : public std::runtime_error {
  public:
    // CREATORS

    /// Create an object of this type holding the error message given by the
    /// specified `whatArg`
    BSLS_KEYWORD_EXPLICIT format_error(const std::string& whatArg);
    BSLS_KEYWORD_EXPLICIT format_error(const char *whatArg);

    /// Create an object of this type holding the error message given by the
    /// specified `whatArg` Note that if a `bsl::string` is passed to the
    /// `std::string` constructor, two copies occur (one to initialize
    /// `whatArg`, and one to initialize the internal reference-counted
    /// string).  This constructor ensures that only a single copy needs to be
    /// performed.
    BSLS_KEYWORD_EXPLICIT format_error(const bsl::string& whatArg);

    /// Create an object of this type which is a copy of the specified `other`.
    format_error(const format_error& other) BSLS_KEYWORD_NOEXCEPT;
};
#endif

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

// Unlike other format-related types and free functions, `format_error`
// specifically needs to be promoted into the `bsl` namespace here rather than
// in `bslfmt_format.h` so that `formatter` partial specializations can be
// written to reference `bsl::format_error` and thus remain compatible with
// both the BSL implementation and the standard library implementation.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    using std::format_error;
#else
    using BloombergLP::bslfmt::format_error;
#endif

}  // close namespace bsl


// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

namespace BloombergLP {
namespace bslfmt {

                             // ------------------
                             // class format_error
                             // ------------------

// CREATORS
inline
format_error::format_error(const std::string& whatArg)
: runtime_error(whatArg)
{
}

inline
format_error::format_error(const char *whatArg)
: runtime_error(whatArg)
{
}

// If a `bsl::string` is passed to the `std::string` constructor, two copies
// occur (one to initialize `whatArg`, and one to initialize the internal
// reference-counted string).  This constructor ensures that only a single copy
// needs to be performed.
inline
format_error::format_error(const bsl::string& whatArg)
: runtime_error(whatArg.c_str())
{
}

inline
format_error::format_error(const format_error& other) BSLS_KEYWORD_NOEXCEPT
: runtime_error(other)
{
}


}  // close package namespace
}  // close enterprise namespace

#endif  // !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

#endif  // INCLUDED_BSLFMT_FORMATERROR

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
