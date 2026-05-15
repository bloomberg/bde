// bslmf_containercompatiblerange.h                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_CONTAINERCOMPATIBLERANGE
#define INCLUDED_BSLMF_CONTAINERCOMPATIBLERANGE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concept for the Standard `container-compatible-range`.
//
//@CLASSES:
//  bslmf::ContainerCompatibleRange: the `container-compatible-range` concept
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a concept,
// `bslmf::ContainerCompatibleRange`, that specifies the requirements for
// range-types used to create/set containers.  This is a concrete concept
// corresponding the Standard, exposition-only, concept
// `container-compatible-range`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Declaring a Range Compatible Interface
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a container class that we want to be able to accept content
// from a range.  We can use `bslmf::ContainerCompatibleRange` concept to
// constrain the relevant templates, removing them from the overload set unless
// the (template argument) `t_RANGE` meets the required constraints.
// ```
//                      // =================
//                      // class MyContainer
//                      // =================
//
//  template <class t_TYPE>
//  class MyContainer {
//
//    public:
//      // CREATORS
//
//      // ...
//
//      template <bslmf::ContainerCompatibleRange<t_TYPE> t_RANGE>
//      MyContainer(bsl::from_range_t , t_RANGE&& range);
//
//      // ...
//
//      // MANIPULATORS
//
//      // ...
//      template <bslmf::ContainerCompatibleRange<t_TYPE> t_RANGE>
//      MyContainer& append_range(t_RANGE&& range);
//
//      // ...
//
//  };
// ```
// Notice that the (singular) tag value `bsl::from_range` is used to clearly
// direct overload resolution to the range constructor when that is the intent.
// For other methods, we have the option to incorporate "range" into the method
// name to avoid ambiguous overloads -- i.e., `append_range` is clearly
// different from assorted overloads of `append` (not shown).
//
// Finally, notice that the constraint on `t_RANGE` and `t_TYPE` must be
// repeated in the method definitions:
// ```
//                      // -----------------
//                      // class MyContainer
//                      // -----------------
//
//  // CREATORS
//
//  // ...
//
//  template <class t_TYPE>
//  template <bslmf::ContainerCompatibleRange<t_TYPE> t_RANGE>
//  MyContainer<t_TYPE>::MyContainer(bsl::from_range_t , t_RANGE&& range)
//  {
//      // ...
//  }
//
//  // MANIPULATORS
//
//  // ...
//
//  template <class t_TYPE>
//  template <bslmf::ContainerCompatibleRange<t_TYPE> t_RANGE>
//  MyContainer<t_TYPE>& MyContainer<t_TYPE>::append_range(t_RANGE&& range)
//  {
//      // ...
//      return *this;
//  }
//
//  int main()
//  {
//      int              intArray[] = {  7,  13 };
//      MyContainer<int> mc(bsl::from_range, intArray);
//                                    // OK, `intArray` is a range.
//
//      std::vector<int> iVector    = { 42, 666 };
//      mc.append_range(iVector);     // OK, `iVector` is also a range.
//
//      std::vector<double> dVector = { 1.0, 32.2, 211.95 };
//      mc.append_range(dVector);     // OK, `dVector` is also a range *and*
//                                    // `double` is convertible to `int`.
//
//      std::vector<std::string> sVector = { "Tom", "Dick", "Harry" };
//      mc.append_range(sVector);     // Error, `sVector` is a range but
//                                    // `bsl::string` is *not* implicitly
//                                    // convertible to `int`.
//
//      int intValue = -1;
//      mc.append_range(intValue);    // Error, `intValue` has expected type
//                                    // *but* is not a range.
//
//      return 0;
//  }
// ```

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS) \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)

#include <concepts>
#include <ranges>

namespace BloombergLP {
namespace bslmf {

/// Require that (template parameter) `t_RANGE` meets the requirements of
/// input range and that the values referenced by the iterators of that
/// range type have the same type or are convertible to the (template
/// parameter) `t_TYPE`.
template <typename t_RANGE, typename t_TYPE>
concept ContainerCompatibleRange =
    std::ranges::input_range<t_RANGE>
 && std::convertible_to<std::ranges::range_reference_t<t_RANGE>, t_TYPE>;

}  // close package namespace
}  // close enterprise namespace
#endif
#endif

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
