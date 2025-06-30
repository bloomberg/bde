// bslfmt_enablestreamedformatter.h                                   -*-C++-*-

#ifndef INCLUDED_BSLFMT_ENABLESTREAMEDFORMATTER
#define INCLUDED_BSLFMT_ENABLESTREAMEDFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to enable stream based formatting of a type.
//
//@CLASSES:
//  bslfmt::EnableStreamedFormatter: trait to enable streamed `bsl::formatter`
//
//@DESCRIPTION: This component provides a trait, `EnableStreamedFormatter`,
// that a user can associate with a type, so that type will automatically use
// the streaming operator (`operator<<`) when the type is formatted with
// `bsl::format`.  The `EnableStreamedFormatter` trait is typically associated
// with a type using `BSLMF_NESTED_TRAIT_DECLARATION` (see
// `bslmf_detectnestedtrait`), and indicates that `bslfmt::StreamedFormatter`
// (`bslfmt_streamedformatter`) should be used to format values of the type.
//
// Be aware that enabling this trait for a type may preclude implementing a
// more type specific formatter in the future.  Users of the type may come to
// rely on the "string-like" format specification, or the output format, in
// ways that prevent the type owner for implementing a specific formatter for
// the type in the future.
//
// For more information read
// [the package documentation](bslfmt#Streaming-based Formatting).
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Enabling Formatting of a Streamable Type
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we own a simple type that supports `ostream` insert `operator<<` and
// want to quickly add `bsl::format`ing capability to it, based on streaming.
//
// First, we introduce a type with a streaming operator but without a formatter
// that enables `bsl::format` use:
//```
//  class NonFormattableType {};
//
//  std::ostream& operator<<(std::ostream& os, const NonFormattableType&)
//  {
//      return os << "The printout";
//  }
//
//  // The following would not compile:
//  //
//  // const NonFormattableType noFormatObj;
//  // bsl::string s = bsl::format("{}", noFormatObj);
//```
// Then, we enable formatting using the trait (notice the type name changed):
//```
//  class NowFormattableType {
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(NowFormattableType,
//                                     bslfmt::EnableStreamedFormatter);
//  };
//
//  std::ostream& operator<<(std::ostream& os, const NowFormattableType&)
//  {
//      return os << "The printout";
//  }
//```
// Next, we create an instance of this type and use `bsl::format` to format it:
//```
//  const NowFormattableType obj;
//  bsl::string s = bsl::format("{}", obj);
//```
// Finally, we verify the output is correct:
//```
//  assert(s == "The printout");
//```

#include <bslscm_version.h>

#include <bslfmt_format.h>
#include <bslfmt_streamedformatter.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_enableif.h>

#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslfmt {

                       // ==============================
                       // struct EnableStreamedFormatter
                       // ==============================

/// A nested-trait-declaration compatible boolean trait indicating whether a
/// type that is passed to `bsl::format` will be formatted using its `ostream`
/// insertion operator, `operator<<`.
template <class t_TYPE>
struct EnableStreamedFormatter
: bslmf::DetectNestedTrait<t_TYPE, EnableStreamedFormatter>::type {
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

/// Standard (`bsl`) formatter for types that have their
/// `bslfmt::EnableStreamedFormatter` set (to `true`).  Delegates to
/// `bslfmt::Streamed_Formatter`.
template <class t_STREAMABLE>
struct formatter<
    t_STREAMABLE,
    typename bsl::enable_if<
        BloombergLP::bslfmt::EnableStreamedFormatter<t_STREAMABLE>::value,
        char>::type> : BloombergLP::bslfmt::StreamedFormatter<t_STREAMABLE> {
};

}  // close namespace bsl

#endif  // INCLUDED_BSLFMT_ENABLESTREAMEDFORMATTER

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
