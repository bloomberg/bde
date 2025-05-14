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
//  BSLFMT_FORMATTABLE_DEFINED: `bsl::formatter` exists
//
//@DESCRIPTION: This component conditionally provides ...
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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
#define BSLFMT_FORMATTABLE_NAMESPACE_ std
#else   // of ifdef sBSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
#define BSLFMT_FORMATTABLE_NAMESPACE_ bsl
#endif  // else of - BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

namespace BloombergLP {
namespace bslfmt {

template <class t_CHAR>
class Formattable_OutputIterator {
  public:
    using difference_type = ptrdiff_t;

    // MANIPULATORS
    Formattable_OutputIterator& operator++();
    Formattable_OutputIterator operator++(int);

    // ACCESSORS
    t_CHAR& operator*() const;
};


template <class t_TYPE,
          class t_CONTEXT,
          class t_FORMATTER =
              typename t_CONTEXT::template
                  formatter_type<std::remove_const_t<t_TYPE>> >
concept Formattable_With =
  std::semiregular<t_FORMATTER> &&
  requires (t_FORMATTER& f, const t_FORMATTER& cf, t_TYPE&& t, t_CONTEXT fc,
            BSLFMT_FORMATTABLE_NAMESPACE_::basic_format_parse_context<
                     typename t_CONTEXT::char_type
            > pc) {
    { f.parse(pc) } -> std::same_as<typename decltype(pc)::iterator>;
    { cf.format(t, fc) } -> std::same_as<typename t_CONTEXT::iterator>;
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
template <class t_TYPE, class t_CHAR = char>
concept formattable = BloombergLP::bslfmt::Formattable_With<
    std::remove_reference_t<t_TYPE>,
    BSLFMT_FORMATTABLE_NAMESPACE_::basic_format_context<
        //BloombergLP::bslfmt::Format_ContextOutputIteratorRef<t_CHAR>,
        BloombergLP::bslfmt::Formattable_OutputIterator<t_CHAR>,
        t_CHAR> >;
}  // close namespace bsl

#undef BSLFMT_FORMATTABLE_NAMESPACE_

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
