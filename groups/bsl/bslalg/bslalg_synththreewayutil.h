// bslalg_synththreewayutil.h                                         -*-C++-*-
#ifndef INCLUDED_BSLALG_SYNTHTHREEWAYUTIL
#define INCLUDED_BSLALG_SYNTHTHREEWAYUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementation utilities for 'operator<=>'.
//
//@CLASSES:
//  bslalg::SynthThreeWayUtil: exposition-only utilities from <compare>
//
//@SEE_ALSO: bslstl_compare
//
//@DESCRIPTION: This component provides a namespace class,
// 'bslalg::SynthThreeWayUtil', that contains an implementation of the Standard
// exposition-only entities (see [expos.only.entity] in the C++20 Standard):
// 'synth-three-way' ('bslalg::SynthThreeWayUtil::compare') and
// 'synth-three-way-result' ('bslalg::SynthThreeWayUtil::Result').
//
// 'bslalg::SynthThreeWayUtil::compare' is a callable object with the following
// effective signature:
//..
//  template <class T1, class T2>
//  constexpr Result<T1, T2> compare(const T1& t1, const T2& t2);
//..
// It returns the result of 't1 <=> t2' expression if this expression is valid.
// Otherwise, if 't1 < t2' and 't2 < t1' expressions are well-formed and return
// boolean-testable results, 'operator<' is used to emulate 'operator<=>'.
// Otherwise this function is not defined and does not participate in overload
// resolution.
//
// As shown above, 'bslalg::SynthThreeWayUtil::Result<T1, T2>' is a type
// returned by 'bslalg::SynthThreeWayUtil::compare<T1, T2>'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing '<=>' For a Sequence Container
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use 'bslalg::SynthThreeWayUtil' to implement the
// three-way comparison operator ('<=>') for a list container (whose details
// have been elided):
//..
//  template <class T, class A>
//  bslalg::SynthThreeWayUtil::Result<T> operator<=>(const list<T,A>& lhs,
//                                                   const list<T,A>& rhs)
//  {
//      return bsl::lexicographical_compare_three_way(
//                                         lhs.begin(),
//                                         lhs.end(),
//                                         rhs.begin(),
//                                         rhs.end(),
//                                         bslalg::SynthThreeWayUtil::compare);
//  }
//..
// Now the '<', '>', '<=' and '>=' operators are automatically defined for the
// container:
//..
//  // Create some instance of the template
//  list<int> list1, list2;
//
//  // Empty lists are equal
//  assert(!(list1 <  list2));
//  assert(!(list1 >  list2));
//  assert(  list1 <= list2);
//  assert(  list1 >= list2);
//..

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON) &&            \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS)

#define BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE 1

#include <bslmf_booleantestable.h>

#include <concepts>
#include <utility>

namespace BloombergLP {
namespace bslalg {

                            // =======================
                            // class SynthThreeWayUtil
                            // =======================

struct SynthThreeWayUtil {
    // "Exposition-only" part of 'bslstl_compare.h'

    static constexpr
    struct {
        template <class t_TYPE1, class t_TYPE2>
        static constexpr bool isNoexcept(const t_TYPE1 *t1 = nullptr,
                                         const t_TYPE2 *t2 = nullptr)
        {
            if constexpr (std::three_way_comparable_with<t_TYPE1, t_TYPE2>)
                return noexcept(*t1 <=> *t2);
            else
                return noexcept(*t1 < *t2) && noexcept(*t2 < *t1);
        }

        template <class t_TYPE1, class t_TYPE2>
        [[nodiscard]]
        constexpr auto operator()(const t_TYPE1& t1, const t_TYPE2& t2) const
                                       noexcept(isNoexcept<t_TYPE1, t_TYPE2>())
        requires requires {
                { t1 < t2 } -> bslmf::BooleanTestable;
                { t2 < t1 } -> bslmf::BooleanTestable;
            }
        {
            if constexpr (std::three_way_comparable_with<t_TYPE1, t_TYPE2>)
                return t1 <=> t2;
            else
            {
                if (t1 < t2)
                    return std::weak_ordering::less;
                else if (t2 < t1)
                    return std::weak_ordering::greater;
                else
                    return std::weak_ordering::equivalent;
            }
        }
    } compare{};
        // Return the result of 't1 <=> t2' expression if this expression is
        // valid.  Otherwise, if 't1 < t2' and 't2 < t1' expressions are
        // well-formed and return boolean-testable results, use the '<'
        // operator to emulate '<=>'.  Otherwise this function is not defined
        // and does not participate in overload resolution.  Note that this is
        // an implementation of the exposition-only callable object
        // 'synth-three-way' defined by the ISO C++20 Standard
        // [expos.only.entity].

    template <class t_TYPE1, class t_TYPE2 = t_TYPE1>
    using Result = decltype(compare(std::declval<t_TYPE1&>(),
                                    std::declval<t_TYPE2&>()));
        // A type returned by 'SynthThreeWayUtil::compare<t_TYPE1, t_TYPE2>'.
        // Note that this is an implementation of the exposition-only type
        // alias 'synth-three-way-result' defined by the ISO C++20 Standard
        // [expos.only.entity].
};

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON  &&
        // BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS

#endif

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
