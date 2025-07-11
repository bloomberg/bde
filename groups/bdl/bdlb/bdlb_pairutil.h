// bdlb_pairutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLB_PAIRUTIL
#define INCLUDED_BDLB_PAIRUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide support functions for `bsl::pair`.
//
//@CLASSES:
//  bdlb::PairUtil: namespace for functions working with `bsl::pair`
//
//@DESCRIPTION: This component provides the class `bdlb::PairUtil`, which has
// the following tools for working with `bsl::pair`:
// * `tie`:               function that is intended to be used in place of
//                        `bsl::tie` when the right-hand side of the
//                        assignment is a `bsl::pair`
// * `adaptForRanges`:    function that adapt `bsl` containers whose value type
//                        is a `bsl::pair` type to work with range adaptors
// * `stdPairRefAdaptor`: function object that takes a `bsl::pair` by reference
//                        and returns a `std::pair` where each element is a
//                        reference to the corresponding member of the
//                        `bsl::pair`
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage of `tie` Function
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we need to implement a function that takes a `bsl::map` and stores
// into out-parameters the key and value corresponding to the first entry in
// the map.  Using `bsl::map`s container interface, we can obtain a reference
// to a `bsl::pair` of the key and value.  We can then use
// `bdlb::PairUtil::tie` to assign from both the key and value in a single
// expression:
// ```
// /// Load into the specified `key` and the specified `value` the key and
// /// value for the first entry in the specified `map` and return `true`,
// /// or else fail by storing 0 and an empty string and return `false`
// /// when `map` is empty.
// bool getFirst(int                              *key,
//               bsl::string                      *value,
//               const bsl::map<int, bsl::string>& map)
// {
//     if (map.empty()) {
//         *key = 0;
//         value->clear();
//         return false;                                              // RETURN
//     }
//     bdlb::PairUtil::tie(*key, *value) = *map.begin();
//     return true;
// }
//
// void usageExample()
// {
//     bsl::map<int, bsl::string> map;
//     map[30782530] = "bbi10";
//
//     int         uuid;
//     bsl::string username;
//
//     bool result = getFirst(&uuid, &username, map);
//     assert(result);
//     assert(30782530 == uuid);
//     assert("bbi10"  == username);
// }
// ```
//
///Example 2: Adapting `bsl` Container For Ranges
/// - - - - - - - - - - - - - - - - - - - - - - -
// Let's assume that we have a `bsl::map` storing employee indexes and their
// names, and we want to get a list of employee names:
// ```
//  bsl::map<int, bsl::string_view> employees{{1, "John Dow"},
//                                            {2, "Jane Dow"},
//                                            {3, "James Dow"}};
// ```
// However, if we were to try and access the names using `bsl::views::values`
// we would see a compilation error:
//
// auto names = employees | bsl::views::values;  // does not compile
// auto namesIt = names.begin();
// assert("John Dow" == *namesIt);
//
// This fails to because `bsl::pair`, unlike the `std::pair`, does not model
// the `tuple-like` concept, which is a requirement of the
// `bsl::views::values`.  This problem can be resolved using the
// `bdlb::PairUtil::adaptForRanges` function on the container:
// ```
//  auto names = bdlb::PairUtil::adaptForRanges(employees)
//                                                        | bsl::views::values;
//  auto namesIt = names.begin();
//  assert("John Dow" == *namesIt);
// ```
// And of course this function allows you to create chains of adaptors using a
// pipeline operator:
// ```
//  const auto startsWithJa = [](bsl::string_view name) -> bool
//  {
//      return name.starts_with("Ja");
//  };
//
//  auto jaNames = bdlb::PairUtil::adaptForRanges(employees)
//                                          | bsl::views::values
//                                          | bsl::views::filter(startsWithJa);
//  assert(bsl::ranges::equal(jaNames,
//                            bsl::vector<bsl::string_view>{"Jane Dow",
//                                                          "James Dow"}));
// ```

#include <bdlscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_removereference.h>

#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
#include <bsl_ranges.h>
#endif

#include <bsl_utility.h>


namespace BloombergLP {
namespace bdlb {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

                       // =================================
                       // struct PairUtil_StdPairRefAdaptor
                       // =================================

/// This component-private struct is a function object converting `bsl::pair`
/// object into its standard counterpart storing references to the attributes
/// of the original object.
struct PairUtil_StdPairRefAdaptor {
  public:
    // ACCESSORS

    /// Return `std::pair` containing references to the fields of the specified
    /// `pair`.
    template <class t_FIRST, class t_SECOND>
    constexpr std::pair<t_FIRST&, t_SECOND&> operator()(
                                     bsl::pair<t_FIRST, t_SECOND>& pair) const;
    template <class t_FIRST, class t_SECOND>
    constexpr std::pair<const t_FIRST&, const t_SECOND&>
    operator()(const bsl::pair<t_FIRST, t_SECOND>& pair) const;
};
#endif

                               // ==============
                               // class PairUtil
                               // ==============

/// This `struct` provides a namespace for the functions that make working with
/// `bsl::pair`-like containers more convenient or adapting such containers to
/// work with standard library utilities.
struct PairUtil {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    // CLASS DATA

    /// A function object that takes a `bsl::pair` by reference and returns a
    // `std::pair` where each element is a reference to the corresponding
    // member of the `bsl::pair`.
    static constexpr PairUtil_StdPairRefAdaptor stdPairRefAdaptor{};
#endif

    // CLASS METHODS

    /// Return a `bsl::pair` object holding a reference to the specified
    /// `first` and a reference to the specified `second`, respectively.
    template <class t_FIRST, class t_SECOND>
    static bsl::pair<t_FIRST&, t_SECOND&> tie(t_FIRST&  first,
                                              t_SECOND& second);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    /// Return a view containing all elements of the specified `container`,
    /// converted to `std::pair` by calling `stdPairRefAdaptor`.  This function
    /// participates in overload resolution only if container is a range whose
    /// value type is a `bsl::pair` type.
    template <class t_CONTAINER>
    static auto adaptForRanges(t_CONTAINER&& container)
        -> decltype(bsl::views::transform(bsl::forward<t_CONTAINER>(container),
                                          stdPairRefAdaptor));
#endif
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // ---------------------------------
                       // struct PairUtil_StdPairRefAdaptor
                       // ---------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
// ACCESSORS
template <class t_FIRST, class t_SECOND>
inline
constexpr std::pair<t_FIRST&, t_SECOND&>
PairUtil_StdPairRefAdaptor::operator()(
                                      bsl::pair<t_FIRST, t_SECOND>& pair) const
{
    BSLMF_ASSERT(!bsl::is_rvalue_reference_v<t_FIRST>);
    BSLMF_ASSERT(!bsl::is_rvalue_reference_v<t_SECOND>);
    return {pair.first, pair.second};
}

template <class t_FIRST, class t_SECOND>
inline
constexpr std::pair<const t_FIRST&, const t_SECOND&>
PairUtil_StdPairRefAdaptor::operator()(
                                const bsl::pair<t_FIRST, t_SECOND>& pair) const
{
    BSLMF_ASSERT(!bsl::is_rvalue_reference_v<t_FIRST>);
    BSLMF_ASSERT(!bsl::is_rvalue_reference_v<t_SECOND>);

    return {pair.first, pair.second};
}
#endif

                              // ---------------
                              // struct PairUtil
                              // ---------------

// CLASS METHODS
template <class t_FIRST, class t_SECOND>
inline
bsl::pair<t_FIRST&, t_SECOND&> PairUtil::tie(t_FIRST& first, t_SECOND& second)
{
    return bsl::pair<t_FIRST&, t_SECOND&>(first, second);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
template <class t_CONTAINER>
inline auto
PairUtil::adaptForRanges(t_CONTAINER&& container)
-> decltype(bsl::views::transform(bsl::forward<t_CONTAINER>(container),
                                  stdPairRefAdaptor))
{
    return std::forward<t_CONTAINER>(container)
                                    | bsl::views::transform(stdPairRefAdaptor);
}
#endif

}  // close package namespace
}  // close enterprise namespace

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
