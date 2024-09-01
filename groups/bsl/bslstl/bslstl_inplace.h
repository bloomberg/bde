// bslstl_inplace.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_INPLACE
#define INCLUDED_BSLSTL_INPLACE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard-compliant in place construction tag types.
//
//@CLASSES:
//  bsl::in_place_t: tag type for in-place construction
//  bsl::in_place_type_t: tag type for in-place construction of a given type
//  bsl::in_place_index_t: tag type for in-place construction at a given index
//
//@CANONICAL_HEADER: bsl_utility.h
//
//@DESCRIPTION: This component provides an implementation of standard
// compliant tag types for in-place construction: `bsl::in_place_t`,
// `bsl::in_place_type_t`, and `bsl::in_place_index_t`.  Tag type
// `bsl::in_place_t` is used in constructors of `bsl::optional` to indicate
// that the contained object should be constructed in-place.  Tag type
// `bsl::in_place_type_t<TYPE>` is used in constructors of `bsl::variant` to
// indicate that the object of type `TYPE` should be constructed in-place.  Tag
// type `bsl::in_place_index_t<INDEX>` is used in constructors of
// `bsl::variant` to indicate that the alternative with index `INDEX` should
// be constructed in-place.
//
//@SEE_ALSO: bslstl_optional, bslstl_variant

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <stddef.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <utility>  // for std::in_place_t
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

using std::in_place_t;
using std::in_place;

using std::in_place_type_t;
using std::in_place_type;

using std::in_place_index_t;
using std::in_place_index;
#else

                              // ================
                              // class in_place_t
                              // ================

/// This trivial tag type is passed to the constructors of types that
/// contain a single object to indicate that the contained object should be
/// constructed in-place.
struct in_place_t {

    // CREATORS

    /// Create an `in_place_t` value.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
    explicit BSLS_KEYWORD_CONSTEXPR in_place_t() = default;
#else
    explicit BSLS_KEYWORD_CONSTEXPR in_place_t() BSLS_KEYWORD_NOEXCEPT;
#endif
};

// CREATORS
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
inline
BSLS_KEYWORD_CONSTEXPR in_place_t::in_place_t() BSLS_KEYWORD_NOEXCEPT
{
    // This `constexpr` function has to be defined before initializing the
    // `constexpr` value, `in_place`, below.
}
#endif  // !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)

/// Value of type `in_place_t` used as an argument to functions that take an
/// `in_place_t` argument.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
inline constexpr in_place_t in_place = in_place_t();
#else
extern const in_place_t in_place;
#endif

                           // =====================
                           // class in_place_type_t
                           // =====================

/// This trivial tag type is passed to the constructors of types that can
/// contain objects of multiple types to indicate the type of contained
/// object to create.
template <class TYPE>
struct in_place_type_t {

    // CREATORS

    /// Create an `in_place_type_t` value.  On platforms that allow for
    /// defaulted special member functions, we opt for the compiler provided
    /// one too keep the type trivial.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
    explicit BSLS_KEYWORD_CONSTEXPR in_place_type_t() = default;
#else
    explicit BSLS_KEYWORD_CONSTEXPR in_place_type_t() BSLS_KEYWORD_NOEXCEPT;
#endif
};

// CREATORS
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
template <class TYPE>
inline
BSLS_KEYWORD_CONSTEXPR
in_place_type_t<TYPE>::in_place_type_t() BSLS_KEYWORD_NOEXCEPT
{
}
#endif  // !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
/// Value of type `in_place_type_t<TYPE>` used as an argument to functions
/// that take an `in_place_type_t` argument.
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr in_place_type_t<TYPE> in_place_type;
#endif

                           // ======================
                           // class in_place_index_t
                           // ======================

/// This trivial tag type is passed to the constructors of `bsl::variant` to
/// indicate the index of the alternative to create.
template <size_t INDEX>
struct in_place_index_t {

    // CREATORS

    /// Create an `in_place_index_t` object.  This constructor is trivial if
    /// the platform supports defaulted special member functions.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
    explicit BSLS_KEYWORD_CONSTEXPR in_place_index_t() = default;
#else
    explicit BSLS_KEYWORD_CONSTEXPR in_place_index_t() BSLS_KEYWORD_NOEXCEPT;
#endif
};

// CREATORS
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
template <size_t INDEX>
inline
BSLS_KEYWORD_CONSTEXPR
in_place_index_t<INDEX>::in_place_index_t() BSLS_KEYWORD_NOEXCEPT
{
}
#endif  //!defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
/// Value of type `in_place_index_t<INDEX>` used as an argument to functions
/// that take an `in_place_index_t` argument.
template <size_t INDEX>
BSLS_KEYWORD_INLINE_VARIABLE constexpr in_place_index_t<INDEX> in_place_index;
#endif

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

}  // close namespace bsl

#endif  // INCLUDED_BSLSTL_INPLACE

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
