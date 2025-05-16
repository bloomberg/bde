// bslfmt_formattable.h                                               -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTABLE
#define INCLUDED_BSLFMT_FORMATTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a "trait" to check the presence of a `bsl::formatter`
//
//@CLASSES:
//  bsl::formattable<t_TYPE, t_CHAR>: `bsl::formatter` presence trait
//
//@MACROS:
//  BSLFMT_FORMATTABLE_DEFINED: `bsl::formatter` is provided
//
//@DESCRIPTION: This component conditionally provides `bsl::formatter`, a
// concept that determines if a type has an existing formatter for a given
// character type (of `char` or `wchar_t`).  The concept is defined only if the
// compiler and standard used has usable concept support.  The macro
// `BSLFMT_FORMATTABLE_DEFINED` may be used to determine if the concept is
// present or not.
//
// FOR BDE INTERNAL USE ONLY!  This concepts does not always exist, which is an
// unusual (but unavoidable) behavior for names provided by BDE style code
// therefore we strongly discourage direct use by users of BDE.  If you have a
// need to use this concept you are probably writing code that should really be
// part of `bslfmt`.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Verify the Presence of a bsl::formatter
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to ..
//
// First we ...
//
//```
//  TBD
//```

#include <bslscm_version.h>

#include <bslfmt_format.h>

#include <bsls_compilerfeatures.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS

#define BSLFMT_FORMATTABLE_DEFINED                                            1

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_FORMAT
// In C++23 we just use the `std`-defined concept directly
namespace bsl {
    using std::formattable;
}
#else

#include <concepts>
#include <type_traits>

#include <stddef.h>

namespace BloombergLP {
namespace bslfmt {

/// A dummy class that looks like an output iterator to use in the concept
/// implementation instead of the unknown (unspecified) output iterator that is
/// actually used by the standard implementation.  Note that we know the output
/// iterator our implementation uses but that is a component-private type, so
/// not really usable here either.
template <class t_CHAR>
class Formattable_DummyOutputIterator {
  public:
    using difference_type = ptrdiff_t;

    // MANIPULATORS
    Formattable_DummyOutputIterator& operator++();
    Formattable_DummyOutputIterator operator++(int);

    // ACCESSORS
    t_CHAR& operator*() const;
};

/// This concept, `bslfmt::Formattable_With`, is an implementation detail of
/// the `bsl::formattable` concept and it is not to be used directly.
///
/// This concept answers the question if the specified `t_TYPE` is formattable
/// with the given `t_CONTEXT`.  The `t_FORMATTER` type parameter is just a
/// "local variable" that retrieves the actual formatter type from the context,
/// it must not be specified when the concept is invoked.
template <class t_TYPE,
          class t_CONTEXT,
          class t_FORMATTER =
              typename t_CONTEXT::template
                  formatter_type<std::remove_const_t<t_TYPE>> >
concept Formattable_With =
  std::semiregular<t_FORMATTER> &&
  requires (t_FORMATTER& f, const t_FORMATTER& cf, t_TYPE&& t, t_CONTEXT fc,
            bsl::basic_format_parse_context<
                     typename t_CONTEXT::char_type
            > pc) {
    { f.parse(pc) } -> std::same_as<typename decltype(pc)::iterator>;
    { cf.format(t, fc) } -> std::same_as<typename t_CONTEXT::iterator>;
};

}  // close package namespace
}  // close enterprise namespace

/// The `bsl::formattable` concept answers the question if the specified
/// `t_TYPE` has a formatter specialization for the specified `t_CHAR`
/// character type available, in other words if `t_TYPE` can be used as a
/// `bsl::format` formatted argument with a format string of `t_CHAR`.
/// The `t_CHAR` character type must be one of `char` or `wchar_t`.
namespace bsl {
template <class t_TYPE, class t_CHAR>
concept formattable = BloombergLP::bslfmt::Formattable_With<
    std::remove_reference_t<t_TYPE>,
    bsl::basic_format_context<
        BloombergLP::bslfmt::Formattable_DummyOutputIterator<t_CHAR>,
        t_CHAR> >;
}  // close namespace bsl

#endif  // else of - BSLS_LIBRARYFEATURES_HAS_CPP23_FORMAT
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS

#endif  // INCLUDED_BSLFMT_FORMATTABLE

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
