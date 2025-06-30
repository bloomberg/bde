// bslfmt_streamedformatter.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_STREAMEDFORMATTER
#define INCLUDED_BSLFMT_STREAMEDFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter that uses the `ostream` insert `operator<<`.
//
//@CLASSES:
//  bslfmt::StreamedFormatter: formatter that uses `ostream` `operator<<`
//
//@SEE ALSO: bslfmt_streamed, bslfmt_enablestreamedformatter
//
//@DESCRIPTION: This component provides a base formatter template that may be
// used to enable `bsl::format`ing of values that have an `ostream` insert
// `operator<<` implemented.  The formatting template is also compatible with
// `std::format` on platforms where it is provided.  Note that this component
// is not recommended to be used directly but mainly via the `bslfmt_streamed`
// component that provides a formattable-wrapper, or by defining the
// `bslfmt_enablestreamedformatter` nested trait in the class or template that
// should have a permanent streamed formatter.
//
// The use case for this component is *very* narrow, so please read
// [the package documentation](bslfmt#Streaming-based Formatting) before using
// it and possibly locking yourself into forever supporting an inadequate
// format string specification.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Enable Formatting of a Streamable Object
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.
//
// First, we define the type with a streaming operator, but without a formatter
// specialization:
//```
//  class StreamableType {};
//
//  std::ostream& operator<<(std::ostream& os, const StreamableType&)
//  {
//      return os << "The printout";
//  }
//```
// Then, we enable formatting of this type using `bslfmt::StreamedFormatter` as
// its implementation:
//```
// namespace bsl {
// template <>
// struct formatter<StreamableType, char> :
// BloombergLP::bslfmt::StreamedFormatter<StreamableType> {
// };
// }  // close namespace bsl
//```
// Next, we create an instance of this type and use `bsl::format` to format it:
//```
// const StreamableType obj;
// bsl::string s = bsl::format("{}", obj);
//```
// Finally, we verify the output is correct:
//```
// assert(s == "The printout");
//```

#include <bslscm_version.h>

#include <bslfmt_formatterstring.h>

#include <bslstl_ostringstream.h>
#include <bslstl_string.h>

#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslfmt {

                          // ========================
                          // struct StreamedFormatter
                          // ========================

/// This class provides the implementations of `parse` and `format` for
/// formatters that format a type using its streaming operator.
template <class t_STREAMABLE>
struct StreamedFormatter {
  private:
    // DATA
    bsl::formatter<bsl::string_view, char> d_stringFormatter;

  public:
    // MANIPULATORS

    /// Parse and validate the specification string stored in the iterator
    /// accessed via the `begin()` method of the parseContext passed via the
    /// specified `parseContext` parameter.  Where nested parameters are
    /// encountered in the specification string then the `next_arg_id` and
    /// `check_arg_id` are called on `fc` as specified in the C++ Standard.
    /// Return an end iterator of the parsed range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                t_PARSE_CONTEXT& parseContext);

    // ACCESSORS

    /// Format the specified `value` according to the specification stored as a
    /// result of a previous call to the `parse` method, and write the result
    /// to the iterator accessed by calling the `out()` method on the specified
    /// `formatContext` parameter.  Return an end iterator of the output range.
    /// Throw an exception of type `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                      const t_STREAMABLE& value,
                                      t_FORMAT_CONTEXT&   formatContext) const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // struct StreamedFormatter
                          // ------------------------

// MANIPULATORS
template <class t_STREAMABLE>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
StreamedFormatter<t_STREAMABLE>::parse(t_PARSE_CONTEXT& parseContext)
{
    return d_stringFormatter.parse(parseContext);
}

// ACCESSORS
template <class t_STREAMABLE>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator StreamedFormatter<t_STREAMABLE>::format(
                                       const t_STREAMABLE& value,
                                       t_FORMAT_CONTEXT&   formatContext) const
{
    bsl::ostringstream os;
    os << value;

    return d_stringFormatter.format(os.view(), formatContext);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_STREAMEDFORMATTER

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
