// bslfmt_bslformattable.h                                            -*-C++-*-

#ifndef INCLUDED_BSLFMT_BSLFORMATTABLE
#define INCLUDED_BSLFMT_BSLFORMATTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concept checking if a type's `bsl::formatter` is enabled.
//
//@CLASSES:
//  bslfmt::BslFormattable: concept checking if `bsl::formatter` is enabled
//
//@SEE_ALSO: bslfmt_formattable, bslfmt_formatterbase
//
//@DESCRIPTION: This component conditionally provides `bslfmt::BslFormattable`,
// a concept that determines if `bsl::formatter` is enabled for a type and a
// given character type (i.e. `char` or `wchar_t`).  The concept is defined
// only if concepts are available on the current platform
// (`BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS` is defined).
//
// A type `T` satisfies `bslfmt::BslFormattable<T, C>` if
// `bsl::formatter<U, C>`, where `U` is `T` with any reference and top-level
// `const` removed, is semiregular and its `parse` and `format` members can be
// called with the parse and format contexts of this package.
//
// This concept is intended for constraining a formatter that delegates to the
// `bsl::formatter` of another type (for example, a container formatter
// delegating to the formatter of its element type).  It assumes the delegated
// formatter accepts any format context.
//
///Relation to `bsl::formattable`
///------------------------------
// `bsl::formattable` (see `bslfmt_formattable`) determines if a type can be
// used as a formatted argument of `bsl::format`.  When `bsl::format` is
// implemented by `std::format`, the two concepts differ for:
//
// * types having only a `std::formatter` (such as the `std::chrono` types),
//   which satisfy only `bsl::formattable`, and
// * types whose `bsl::formatter` members `parse` and `format` are not
//   templates on the context type, so they accept only the standard (or only
//   this package's) contexts.
//
// Otherwise the two concepts are equivalent.
//
// Note that the result of either concept depends on which formatter
// specializations are visible at the point at which the concept is checked.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Constraining a Delegating Formatter
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a class template, `Box`, that holds a single value, and we
// want to format a `Box` by formatting its value.
//
// First, we define `Box`:
// ```
//  template <class t_TYPE>
//  struct Box {
//      t_TYPE d_value;
//  };
// ```
// Then, we define a `bsl::formatter` for `Box` that delegates to the
// `bsl::formatter` of the value type.  We constrain the specialization on
// `bslfmt::BslFormattable`, so that it is enabled only if that delegation is
// possible.  Since concepts may not be available (older compilers/standards)
// we protect the constraint with the preprocessor:
// ```
//  namespace bsl {
//
//  template <class t_TYPE, class t_CHAR>
//  #ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
//      requires BloombergLP::bslfmt::BslFormattable<t_TYPE, t_CHAR>
//  #endif
//  struct formatter<Box<t_TYPE>, t_CHAR> {
//      // DATA
//      formatter<t_TYPE, t_CHAR> d_formatter;
//
//      // MANIPULATORS
//      template <class t_PARSE_CONTEXT>
//      BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
//                                                    t_PARSE_CONTEXT& context)
//      {
//          return d_formatter.parse(context);
//      }
//
//      // ACCESSORS
//      template <class t_FORMAT_CONTEXT>
//      typename t_FORMAT_CONTEXT::iterator format(
//                                       const Box<t_TYPE>& box,
//                                       t_FORMAT_CONTEXT&  context) const
//      {
//          return d_formatter.format(box.d_value, context);
//      }
//  };
//
//  }  // close namespace bsl
// ```
// Now, we define a type that has no formatter:
// ```
//  struct NoFormatter {};
// ```
// Finally, we observe that a `Box` of that type, and a `Box` of such a `Box`,
// are not formattable: the constrained specialization is not used for them,
// rather than failing to compile when instantiated:
// ```
//  #ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
//  assert(!(bslfmt::BslFormattable<Box<NoFormatter>,      char>));
//  assert(!(bslfmt::BslFormattable<Box<Box<NoFormatter> >, char>));
//  #endif
// ```

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS

#include <bslfmt_format_context.h>
#include <bslfmt_formatparsecontext.h>

#include <concepts>
#include <type_traits>

#include <stddef.h>

namespace BloombergLP {
namespace bslfmt {

                  // ========================================
                  // class BslFormattable_DummyOutputIterator
                  // ========================================

/// A dummy iterator class that provides the signatures of an output iterator,
/// but whose implementation does nothing.  This type serves as a stand in for
/// the unknown output iterator when defining the `BslFormattable` concept.
template <class t_CHAR>
class BslFormattable_DummyOutputIterator {
  public:
    using difference_type = ptrdiff_t;

    // MANIPULATORS
    BslFormattable_DummyOutputIterator& operator++();
    BslFormattable_DummyOutputIterator operator++(int);

    // ACCESSORS
    t_CHAR& operator*() const;
};

// The following `static_assert`s verify the dummy output iterator conforms to
// some of the requirements of an output iterator.
static_assert(
         std::output_iterator<BslFormattable_DummyOutputIterator<char>, char>);
static_assert(
   std::output_iterator<BslFormattable_DummyOutputIterator<wchar_t>, wchar_t>);

                        // ===========================
                        // concept BslFormattable_With
                        // ===========================

/// This concept, `bslfmt::BslFormattable_With`, is an implementation detail
/// of the `bslfmt::BslFormattable` concept and it is not to be used directly.
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
concept BslFormattable_With =
  std::semiregular<t_FORMATTER> &&
  requires (t_FORMATTER&       f,
            const t_FORMATTER& cf,
            t_TYPE&&           t,
            t_CONTEXT          fc,
            basic_format_parse_context<typename t_CONTEXT::char_type> pc) {
    { f.parse(pc) } -> std::same_as<typename decltype(pc)::iterator>;
    { cf.format(t, fc) } -> std::same_as<typename t_CONTEXT::iterator>;
};

                           // ======================
                           // concept BslFormattable
                           // ======================

/// This concept is satisfied if `bsl::formatter` is enabled for the specified
/// `t_TYPE` and the specified `t_CHAR` character type, and its `parse` and
/// `format` members accept the contexts of this package.  `t_CHAR` must be
/// `char` or `wchar_t`.
template <class t_TYPE, class t_CHAR>
concept BslFormattable = BslFormattable_With<
    std::remove_reference_t<t_TYPE>,
    basic_format_context<BslFormattable_DummyOutputIterator<t_CHAR>, t_CHAR> >;

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS

#endif  // INCLUDED_BSLFMT_BSLFORMATTABLE

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
