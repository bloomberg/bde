// bdlf_visitutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLF_VISITUTIL
#define INCLUDED_BDLF_VISITUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for visiting a variant with an overload set.
//
//@CLASSES:
//  bdlf::VisitUtil: namespace for visiting a variant with several callables.
//
//@SEE_ALSO: bdlf_overloaded, bslstl_variant
//
//@DESCRIPTION: This component provides a `struct`, `bdlf::VisitUtil`, that
// serves as a namespace for the overloaded utility function `visitWith`,
// which visits a variant object with an overload set formed from a sequence
// of callable objects.  The call:
// ```
// bdlf::VisitUtil::visitWith(variant, f1, f2, f3);
// ```
// is equivalent to:
// ```
// bsl::visit(bdlf::Overloaded{f1, f2, f3}, variant);
// ```
// except that, if `variant` is a `std::variant` or is of a type derived from a
// `std::variant`, `std::visit` is used in place of `bsl::visit`.  Classes
// derived from `bsl::variant` and from `std::variant` are supported
// regardless of whether the standard library implements P2162.
//
// The return type of the call above is deduced, so, as with `bsl::visit`, the
// callables must yield the same type and value category for each
// alternative.  If the return type is supplied as an explicit template
// argument, the callables may yield different types, each of which must be
// implicitly convertible to the specified type.  The call:
// ```
// bdlf::VisitUtil::visitWith<R>(variant, f1, f2, f3);
// ```
// is equivalent to:
// ```
// bsl::visit<R>(bdlf::Overloaded{f1, f2, f3}, variant);
// ```
// except that, if `variant` is a `std::variant` or is of a type derived from a
// `std::variant`, the visitation is performed as if by `std::visit<R>`, even
// if the standard library does not provide that overload.  The explicit
// template argument always specifies the return type, even if it is the same
// as the type of `variant`.
//
// Note that, unlike the variadic form of `std::visit`, `visitWith` accepts a
// single variant, and the variant is the *first* argument.  All subsequent
// arguments are callables that together form the overload set.
//
// This component requires C++17.  Support for `std::variant` is available
// only if the standard library provides `<variant>`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// First, create a `bsl::variant` object that can contain several different
// types:
// ```
// bsl::variant<unsigned, double, bsl::string> value;
// ```
// Then, set the value of the variant, and call `visitWith`, passing the
// variant followed by a lambda for each alternative.  Check the return value
// to see that the right lambda was called:
// ```
// value = 2U;
// int result = bdlf::VisitUtil::visitWith(
//                                       value,
//                                       [](unsigned)           { return 1; },
//                                       [](double)             { return 2; },
//                                       [](const bsl::string&) { return 3; });
// assert(1 == result);
// ```
// The callables do not have to cover each alternative individually; a
// generic lambda can act as a fallback for the alternatives that are not
// handled explicitly:
// ```
// value = bsl::string("2.0");
// result = bdlf::VisitUtil::visitWith(value,
//                                     [](double)      { return 2; },
//                                     [](const auto&) { return 0; });
// assert(0 == result);
// ```
// Finally, the callables can modify the active alternative when the variant
// is passed as a modifiable lvalue:
// ```
// value = 2.0;
// bdlf::VisitUtil::visitWith(value,
//                            [](double& d) { d *= 2; },
//                            [](auto&)     {});
// assert(4.0 == bsl::get<double>(value));
// ```
//
///Example 2: Callables Returning Different Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to obtain a numeric size of the value held by a variant that
// can contain either an `int` or a `bsl::string`: the value itself for an
// `int`, and the length for a `bsl::string`.  First, create the variant:
// ```
// bsl::variant<int, bsl::string> value(5);
// ```
// The lambda handling `int` returns `int`, whereas the lambda handling
// `bsl::string` returns `bsl::size_t`.  Since these types differ, calling
// `visitWith` without a template argument would fail to compile.  Instead,
// specify the return type explicitly; the result of the selected lambda is
// implicitly converted to it:
// ```
// auto intSize    = [](int i)                { return i; };
// auto stringSize = [](const bsl::string& s) { return s.length(); };
//
// bsl::size_t size = bdlf::VisitUtil::visitWith<bsl::size_t>(value,
//                                                            intSize,
//                                                            stringSize);
// assert(5 == size);
// ```
// Finally, change the active alternative and visit the variant again:
// ```
// value = bsl::string("abc");
// size  = bdlf::VisitUtil::visitWith<bsl::size_t>(value,
//                                                 intSize,
//                                                 stringSize);
// assert(3 == size);
// ```

#include <bdlscm_version.h>

#include <bdlf_overloaded.h>

#include <bslmf_util.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#include <bsl_type_traits.h>
#include <bsl_variant.h>

#include <utility>   // std::forward, std::move

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <variant>
#endif

namespace BloombergLP {
namespace bdlf {

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

                              // ================
                              // struct VisitUtil
                              // ================

/// This `struct` provides a namespace for utility functions that visit a
/// variant object with an overload set formed from a sequence of callables.
struct VisitUtil {

  private:
    // PRIVATE CLASS METHODS

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    /// Declared but not defined; the return type is a reference to the
    /// `std::variant` base of the argument with the cv-qualification and value
    /// category of `t_VARIANT`, or `void` if there is no such base.
    template <class t_VARIANT, class... t_ALTS>
    static decltype(bslmf::Util::forward_like<t_VARIANT>(
                                     std::declval<std::variant<t_ALTS...>&>()))
    stdBase(const std::variant<t_ALTS...>&);
    template <class t_VARIANT>
    static void stdBase(...);
#endif

    /// Invoke the specified `visitor` with the currently active alternative
    /// of the specified `variant`, and return the result.  If `variant` is a
    /// `std::variant` or is of a type derived from a `std::variant`, the
    /// visitation is performed by `std::visit` on the `std::variant` base
    /// with the cv-qualification and value category of `variant` preserved;
    /// otherwise it is performed by `bsl::visit`.
    template <class t_VISITOR, class t_VARIANT>
    static decltype(auto) visitImp(t_VISITOR&& visitor, t_VARIANT&& variant);

  public:
    // CLASS METHODS

    /// Invoke the overload set `bdlf::Overloaded{callable, callables...}`
    /// formed from the specified `callable` and `callables` with the currently
    /// active alternative of the specified `variant`, and return the result.
    /// If `variant` is a `std::variant` or is of a type derived from a
    /// `std::variant`, the visitation is performed by `std::visit` on the
    /// `std::variant` base with the cv-qualification and value category of
    /// `variant` preserved; otherwise it is performed by `bsl::visit`.  The
    /// program is ill-formed if the type of `variant` has more than one
    /// `std::variant` base, or if its `std::variant` base is not accessible.
    /// Throw `bad_variant_access` if `variant` is valueless by exception.  At
    /// least one callable must be supplied.  Note that the order of arguments
    /// is the reverse of that of `bsl::visit`, and that only one variant is
    /// supported.
    template <class t_CALLABLE, class t_VARIANT, class... t_CALLABLES>
    static decltype(auto) visitWith(t_VARIANT&&      variant,
                                    t_CALLABLE&&     callable,
                                    t_CALLABLES&&... callables);

    /// Invoke the overload set `bdlf::Overloaded{callable, callables...}`
    /// formed from the specified `callable` and `callables` with the currently
    /// active alternative of the specified `variant`, and return the result
    /// implicitly converted to the (template parameter) `t_RET`.  If `t_RET`
    /// is `void`, the result is discarded.  If `variant` is a `std::variant`
    /// or is of a type derived from a `std::variant`, the visitation is
    /// performed by `std::visit` on the `std::variant` base with the
    /// cv-qualification and value category of `variant` preserved; otherwise
    /// it is performed by `bsl::visit`.  The program is ill-formed if the type
    /// of `variant` has more than one `std::variant` base, or if its
    /// `std::variant` base is not accessible.  Throw `bad_variant_access` if
    /// `variant` is valueless by exception.  At least one callable must be
    /// supplied.  Note that, unlike the overload that deduces its return type,
    /// this overload does not require the callables to yield the same type for
    /// each alternative, but only requires each such type to be implicitly
    /// convertible to `t_RET`.
    template <class t_RET,
              class t_VARIANT,
              class t_CALLABLE,
              class... t_CALLABLES>
    static t_RET visitWith(t_VARIANT&&      variant,
                           t_CALLABLE&&     callable,
                           t_CALLABLES&&... callables);
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // ----------------
                              // struct VisitUtil
                              // ----------------

// PRIVATE CLASS METHODS
template <class t_VISITOR, class t_VARIANT>
inline
decltype(auto) VisitUtil::visitImp(t_VISITOR&& visitor, t_VARIANT&& variant)
{
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    typedef decltype(stdBase<t_VARIANT>(variant)) StdBaseRef;

    if constexpr (!bsl::is_void<StdBaseRef>::value) {
        return std::visit(std::forward<t_VISITOR>(visitor),
                          static_cast<StdBaseRef>(variant));
    }
    else
#endif
    {
        return bsl::visit(std::forward<t_VISITOR>(visitor),
                          std::forward<t_VARIANT>(variant));
    }
}

// CLASS METHODS
template <class t_CALLABLE, class t_VARIANT, class... t_CALLABLES>
inline
decltype(auto) VisitUtil::visitWith(t_VARIANT&&      variant,
                                    t_CALLABLE&&     callable,
                                    t_CALLABLES&&... callables)
{
    // Implementation note: `t_CALLABLE` precedes `t_VARIANT` so that an
    // explicit template argument, as in `visitWith<R>(variant, callables...)`,
    // binds to the type of a callable (as with the visitor of `std::visit`),
    // making this overload non-viable instead of ambiguous with the overload
    // returning `R`.

    return visitImp(Overloaded{std::forward<t_CALLABLE>(callable),
                               std::forward<t_CALLABLES>(callables)...},
                    std::forward<t_VARIANT>(variant));
}

template <class t_RET,
          class t_VARIANT,
          class t_CALLABLE,
          class... t_CALLABLES>
inline
t_RET VisitUtil::visitWith(t_VARIANT&&      variant,
                           t_CALLABLE&&     callable,
                           t_CALLABLES&&... callables)
{
    Overloaded visitor{std::forward<t_CALLABLE>(callable),
                       std::forward<t_CALLABLES>(callables)...};

    return visitImp(
        [&visitor](auto&& alternative) -> t_RET {
            typedef decltype(alternative) Alternative;

            if constexpr (bsl::is_void<t_RET>::value) {
                std::move(visitor)(std::forward<Alternative>(alternative));
            }
            else {
                return std::move(visitor)(
                                      std::forward<Alternative>(alternative));
            }
        },
        std::forward<t_VARIANT>(variant));
}

#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_CTAD)

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
